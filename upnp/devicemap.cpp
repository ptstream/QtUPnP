
#include "devicemap.hpp"
#include "datacaller.hpp"
#include "eventingmanager.hpp"
#include "httpserver.hpp"
#include <QNetworkAccessManager>

USING_UPNP_NAMESPACE

CDeviceMap::CDeviceMap ()
{
  QHostAddress address = CUpnpSocket::localHostAddress ();
  m_httpServer         = new CHTTPServer (address, 0, nullptr);
  m_naMgr              = new QNetworkAccessManager ();
  m_httpServer->setNetworkAccessManager (m_naMgr);
}

CDeviceMap::~CDeviceMap ()
{
  delete m_httpServer;
  delete m_naMgr;
}

bool CDeviceMap::subscribe (CDevice& device, int renewDelay, int requestTimeout)
{
  bool        success;
  int         cServices = 0, cEventings = 0;
  TMServices& services  = device.services ();
  for (TMServices::iterator it = services.begin (), end = services.end (); it != end; ++it)
  {
    CService& service = *it;
    if (service.isEvented ())
    {
      ++cEventings;
      CEventingManager em (m_naMgr);
      success = em.subscribe (device.url (), service.eventSubURL (),
                              m_httpServer->serverAddress (), m_httpServer->serverPort (),
                              renewDelay, requestTimeout);
      if (success)
      {
        service.setSubscribeSID (em.sid ());
        ++cServices;
      }
    }
  }

  success = cEventings == cServices;
  return success;
}

void CDeviceMap::renewSubscribe (CDevice& device, int requestTimeout)
{
  QUrl const & url      = device.url ();
  TMServices & services = device.services ();
  for (TMServices::iterator it = services.begin (), end = services.end (); it != end; ++it)
  {
    CService& service = *it;
    if (service.isEvented ())
    {
      QString const & sid = service.subscribeSID ();
      if (!sid.isEmpty ())
      {
        CEventingManager em (m_naMgr);
        if (!em.renewSubscribe (url, service.eventSubURL (), sid, requestTimeout))
        {
          service.clearSID ();
        }
      }
    }
  }
}

void CDeviceMap::unsubscribe (CDevice& device, int requestTimeout)
{
  QUrl const & url      = device.url ();
  TMServices & services = device.services ();
  for (TMServices::iterator it = services.begin (), end = services.end (); it != end; ++it)
  {
    CService&       service = *it;
    QString const & sid     = service.subscribeSID ();
    if (!sid.isEmpty ())
    {
      CEventingManager em (m_naMgr);
      em.unsubscribe (url, service.eventSubURL (), sid, requestTimeout);
      service.clearSID ();
    }
  }
}

CDeviceMap::iterator CDeviceMap::insertDevice (QString const & uuid)
{
  CDevice device;
  device.setUUID (uuid);
  return insert (uuid, device); // Insert in the map.
}

void CDeviceMap::insertDevice (CDevice& device)
{
  insert (device.uuid (), device); // Insert in the map.
  QList<CDevice>& subDevices = device.subDevices ();
  for (QList<CDevice>::iterator it = subDevices.begin (), end = subDevices.end (); it != end; ++it)
  {
    insertDevice (*it);
  }
}

void CDeviceMap::removeDevice (QString const & uuid)
{
  remove (uuid);
}

bool CDeviceMap::extractServiceComponents (CDevice& device, int timeout)
{
  bool success = device.extractServiceComponents (m_naMgr, timeout);
  if (success && m_expandEmbeddedDevices)
  {
    QString         parentUUID = device.uuid ();
    QList<CDevice>& subDevices = device.subDevices ();
    for (QList<CDevice>::iterator it = subDevices.begin (), begin = it, end = subDevices.end (); it != end; ++it)
    {
      CDevice& subDevice = *it;
      QString  uuid      = subDevice.uuid ();
      if (uuid == parentUUID)
      { // Case of subdevice where uuid is equal at parent uuid. Add the index of subdevice.
        // The form will be uuid&0, uuid&0&0... Other forms are not possible because at the same level
        // uuid must be different. The case append with InternetGatewayDevice device. Each device and subdevice
        // have only one subdevice.
        uuid = parentUUID + QString ("&%1").arg (it - begin);
      }

      subDevice.setUUID (uuid);
      success &= extractServiceComponents (subDevice, timeout);
      if (success)
      {
        device.setType ();
        insertDevice (subDevice); // Insert embended device in the map.
      }
    }
  }

  return success;
}

int CDeviceMap::extractDevicesFromNotify (QList<CUpnpSocket::SNDevice> const & nDevices, int timeout)
{
  // For each device in the temp buffer
  for (CUpnpSocket::SNDevice const & nDevice : nDevices)
  {
    if (nDevice.m_type != CUpnpSocket::SNDevice::Unknown)
    {
      if (nDevice.m_type == CUpnpSocket::SNDevice::Byebye)
      {
        if (contains (nDevice.m_uuid))
        {
          removeDevice (nDevice.m_uuid);
          QStringList::const_iterator end = m_lostDevices.cend ();
          if (std::find (m_lostDevices.cbegin (), end, nDevice.m_uuid) == end)
          {
            m_lostDevices.push_back (nDevice.m_uuid);
            m_newDevices.removeOne (nDevice.m_uuid);
          }
        }
      }
      else if (!contains (nDevice.m_uuid) && m_invalidDevices.value (nDevice.m_uuid) < m_deviceFails)
      {
        bool        success      = false;
        char const * failMessage = nullptr;
        CDevice&    device       = *insertDevice (nDevice.m_uuid); // Insert in the map.
        QByteArray  data         = CDataCaller (m_naMgr).callData (nDevice.m_url, timeout); // Get services, name, from device url.
        if (!data.isEmpty ())
        {
          QUrl url (nDevice.m_url.toString (QUrl::RemoveQuery));
          device.setURL (url); // Store url without query.
          success = device.parseXml (data); // Extract services
          if (success)
          {
            device.setType ();
            CDevice::EType type = device.type ();
            if (!m_avOnly || type == CDevice::MediaServer || type == CDevice::MediaRenderer)
            {
              success = extractServiceComponents (device, timeout); // Extract state variables and actions
              if (!success)
              {
                failMessage = "Bad service components:" ;
              }
              else
              {
                QStringList::const_iterator end = m_newDevices.cend ();
                if (std::find (m_newDevices.cbegin (), end, nDevice.m_uuid) == end)
                {
                  m_newDevices.push_back (nDevice.m_uuid);
                  m_lostDevices.removeOne (nDevice.m_uuid);
                }
              }
            }
            else
            {
              m_invalidDevices.insert (url.toString (), 1000);
            }
          }
          else
          {
            failMessage = "Invalid service:";
          }
        }
        else
        {
          ++m_invalidDevices[nDevice.m_uuid];
          failMessage = "Invalid device:";
        }

        if (!success)
        {
          qDebug () << failMessage << nDevice.m_uuid;
          removeDevice (nDevice.m_uuid);
        }
      }
    }
  }

  return size ();
}

QString CDeviceMap::uuid (QString const & host) const
{
  QString uuid;
  for (TMDevices::const_iterator it = cbegin (), end = cend (); it != end; ++it)
  {
    CDevice const & device = it.value ();
    QUrl const &    url    = device.url ();
    if (url.host () == host)
    {
      uuid = device.uuid ();
      break;
    }
  }

  return uuid;
}

QPair<QString, QString> CDeviceMap::eventSender (QString const & sid) const
{
  QPair<QString, QString> ds;
  for (TMDevices::const_iterator itd = cbegin (), end = cend (); itd != end && ds.second.isEmpty (); ++itd)
  {
    CDevice const &    device   = *itd;
    TMServices const & services = device.services ();
    for (TMServices::const_iterator its = services.cbegin (), end = services.cend (); its != end && ds.second.isEmpty (); ++its)
    {
      CService const & service = its.value ();
      if (service.subscribeSID () == sid)
      {
        ds.first  = device.uuid ();
        ds.second = its.key ();
      }
    }
  }

  return ds;
}
