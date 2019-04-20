#include "controlpoint.hpp"
#include "initialdiscovery.hpp"
#include "xmlhaction.hpp"
#include "actioninfo.hpp"
#include "multicastsocket.hpp"
#include "unicastsocket.hpp"
#include "plugin.hpp"
#include "dump.hpp"
#include <QDir>
#include <QLibrary>
#include <QCoreApplication>

USING_UPNP_NAMESPACE

/*! Use an empty list. */
QList<CControlPoint::TArgValue> CControlPoint::noArgs = QList<CControlPoint::TArgValue> ();

CControlPoint::CControlPoint (QObject* parent) : QObject (parent)
{
  m_done = initialize ();
  if (m_done)
  {
    m_newDevicesDetectedTimer.setSingleShot (true);
#ifdef Q_OS_LINUX
#define TIMEOUT 500
#else
#define TIMEOUT 100
#endif
    m_newDevicesDetectedTimer.setInterval (TIMEOUT);
    connect (&m_newDevicesDetectedTimer, SIGNAL(timeout()), this, SLOT(newDevicesDetected()));
    loadPlugins ();
  }
}

CControlPoint::~CControlPoint ()
{
  CUpnpSocket::clearSkippedAddresses ();
  CUpnpSocket::clearSkippedUUID ();
  if (!m_closing)
  {
    close ();
  }
}

CMulticastSocket* CControlPoint::initializeMulticast (QHostAddress const & host, QHostAddress const & group, char const * name)
{
  auto socket = new CMulticastSocket (this);
  if (socket->initialize (host, group))
  {
    socket->setName (name);
    connect (socket, SIGNAL(readyRead()), this, SLOT(readDatagrams()));
  }
  else
  {
    delete socket;
    socket = nullptr;
  }

  return socket;
}

CUnicastSocket* CControlPoint::initializeUnicast (QHostAddress const & host, char const * name)
{
  auto socket = new CUnicastSocket (this);
  if (socket->bind (host))
  {
    socket->setName (name);
    connect (socket, SIGNAL(readyRead()), this, SLOT(readDatagrams()));
  }
  else
  {
    delete socket;
    socket = nullptr;
  }

  return socket;
}

bool CControlPoint::initialize ()
{
  new CDump (this);
  bool done         = false;
  m_multicastSocket = initializeMulticast (QHostAddress::AnyIPv4, CMulticastSocket::upnpMulticastAddr, "MulticastSocket");
  if (m_multicastSocket != nullptr)
  {
    m_multicastSocket6 = initializeMulticast (QHostAddress::AnyIPv6, CMulticastSocket::upnpMulticastAddr6, "MulticastSocket6");
    m_unicastSocket    = initializeUnicast (CUpnpSocket::localHostAddress (), "UnicastSocket");
    if (m_unicastSocket != nullptr)
    {
      m_unicastSocketLocal = initializeUnicast (QHostAddress ("127.0.0.1"), "UnicastSocketLocal");
      CHTTPServer* server = m_devices.httpServer ();
      connect (server, &CHTTPServer::eventReady, this, &CControlPoint::updateEventVars);
      connect (server, &CHTTPServer::mediaRequest, this, &CControlPoint::mediaRequest);
      connect (server, &CHTTPServer::serverComStarted, this, &CControlPoint::serverComStarted);
      connect (server, &CHTTPServer::serverComEnded, this, &CControlPoint::serverComEnded);
      connect (server, &CHTTPServer::rendererComStarted, this, &CControlPoint::rendererComStarted);
      connect (server, &CHTTPServer::rendererComEnded, this, &CControlPoint::rendererComEnded);
      done = true;
    }
  }

  return done;
}

bool CControlPoint::avDiscover ()
{
  bool success = false;
  if (!m_closing)
  {
    char const * ignored[] { "InternetGatewayDevice",
                             "WANConnectionDevice",
                             "WANDevice",
                             "WFADevice",
                             "Printer",
                            };

    for (char const * device : ignored)
    {
      CUpnpSocket::addSkippedUUID (device);
    }

    m_devices.setAVOnly ();
    char const * urns[] = { "urn:schemas-upnp-org:device:MediaServer:1",
                            "urn:schemas-upnp-org:device:MediaRenderer:1",
                            "upnp:rootdevice",
                          };

    int               cDeviceTypes = sizeof (urns) / sizeof (char const *);
    int               cDiscoveries = cDeviceTypes * 4;
    int               iDiscovery   = 0;
    CInitialDiscovery initDiscovery (nullptr, CMulticastSocket::upnpMulticastAddr, CMulticastSocket::upnpMulticastPort);
    for (int iDeviceType = 0; iDeviceType < cDeviceTypes; ++iDeviceType)
    {
      int          index = iDeviceType % cDeviceTypes;
      char const * urn   = urns[index];

      initDiscovery.setSocket (m_unicastSocket);
      success |= initDiscovery.discover (false, urn);
      emit searched (urn, ++iDiscovery, cDiscoveries);
      success |= initDiscovery.discover (false, urn);
      emit searched (urn, ++iDiscovery, cDiscoveries);

      initDiscovery.setSocket (m_unicastSocketLocal);
      success |= initDiscovery.discover (false, urn);
      emit searched (urn, ++iDiscovery, cDiscoveries);
      success |= initDiscovery.discover (false, urn);
      emit searched (urn, ++iDiscovery, cDiscoveries);

      ++iDeviceType;
    }
  }

  return success;
}

bool CControlPoint::discover (char const * nt)
{
  bool success = false;
  if (!m_closing)
  {
    int               iDiscovery   = 0;
    CInitialDiscovery initDiscovery (m_unicastSocket, CMulticastSocket::upnpMulticastAddr, CMulticastSocket::upnpMulticastPort);

    success |= initDiscovery.discover (false, nt);
    emit searched (nt, ++iDiscovery, 4);

    success |= initDiscovery.discover (false, nt);
    emit searched (nt, ++iDiscovery, 4);

    initDiscovery.setSocket (m_unicastSocketLocal);
    success |= initDiscovery.discover (false, nt);
    emit searched (nt, ++iDiscovery, 4);

    success |= initDiscovery.discover (false, nt);
    emit searched (nt, ++iDiscovery, 4);
  }

  return success;
}

void CControlPoint::readDatagrams ()
{
  auto               socket    = static_cast<CUpnpSocket*>(sender ());
  QByteArray const & datagrams = socket->readDatagrams ();
  if (!datagrams.isEmpty () && datagrams.endsWith ("\r\n\r\n"))
  {
    socket->decodeDatagram ();
    m_newDevicesDetectedTimer.start ();
  }
}

void CControlPoint::newDevicesDetected ()
{
#ifdef Q_OS_LINUX
  CUpnpSocket* sockets[]           = { m_unicastSocket, m_unicastSocketLocal, m_multicastSocket, m_multicastSocket6 };
  int          cRemainingDatagrams = 0;
  for (CUpnpSocket* socket : sockets)
  {
    if (socket != nullptr)
    {
      cRemainingDatagrams += socket->datagram ().size ();
    }
  }


  if (cRemainingDatagrams != 0)
  { // Delayed devices creation.
    m_newDevicesDetectedTimer.start ();
    return;
  }
#endif

  ++m_level;
  if (extractDevicesFromNotify () != 0) // Extracts devices from UDP socket cache.
  {
    if (m_level == 1)
    {
      QStringList lostDevices = m_devices.lostDevices ();
      for (QString const & device : lostDevices)
      {
        emit lostDevice (device);
      }

      QStringList newDevices = m_devices.newDevices ();
      for (QString const & device : newDevices)
      {
        emit newDevice (device);
      }

      m_devices.lostDevices ().clear ();
      m_devices.newDevices ().clear ();
    }
  }

  --m_level;
}

void CControlPoint::renewalTimeout ()
{
  auto timer = static_cast<QTimer*>(sender ());
  for (QMap<QString, TSubscriptionTimer>::const_iterator it = m_subcriptionTimers.cbegin (), end = m_subcriptionTimers.cend (); it != end; ++it)
  {
    TSubscriptionTimer const & stimer = it.value ();
    if (timer == stimer.first)
    {
      renewSubscribe (it.key (), stimer.second);
      break;
    }
  }
}

void CControlPoint::updateEventVars (QString const & sid)
{
  if (!m_closing)
  {
    QStringList emitter;
    QPair<QString, QString> ds = m_devices.eventSender (sid);
    if (!ds.first.isEmpty () && this->contains (ds.first))
    {
      CDevice&          device         = m_devices[ds.first];
      CService&         service        = device.services ()[ds.second];
      TMStateVariables& stateVariables = service.stateVariables ();
      TMEventVars       eventVars      = m_devices.httpServer ()->vars ();
      emitter.reserve (eventVars.size () + 2);
      emitter << ds.first << ds.second;
      for (TMEventVars::const_iterator it = eventVars.cbegin (), end = eventVars.cend (); it != end; ++it)
      {
        QString const & name = it.key ();
        if (stateVariables.contains (name))
        {
          CStateVariable&     var   = stateVariables[name];
          TEventValue const & value = it.value ();
          var.setValue (value.first, value.second);
          emitter << name;
        }
      }
    }

    if (!emitter.isEmpty ())
    {
      emit eventReady (emitter);
    }
  }
}

void CControlPoint::networkAccessManager (QString const & deviceUUID, QNetworkReply::NetworkError errorCode, QString const & errorDesc)
{
  emit networkError (deviceUUID, errorCode, errorDesc);
}

QList<CUpnpSocket::SNDevice> CControlPoint::ndevices () const
{
  QList<CUpnpSocket::SNDevice> devices;
  int                          cDevices = 0;
  CUpnpSocket*                 sockets[] = { m_unicastSocket, m_unicastSocketLocal, m_multicastSocket, m_multicastSocket6 };
  for (CUpnpSocket* socket : sockets)
  {
    if (socket != nullptr)
    {
      cDevices += socket->devices ().size ();
    }
  }

  devices.reserve (cDevices);
  for (CUpnpSocket* socket : sockets)
  {
    if (socket != nullptr)
    {
      QList<CUpnpSocket::SNDevice> const & socketDevices = socket->devices ();
      for (CUpnpSocket::SNDevice const & device : socketDevices)
      {
        devices.push_back (device);
      }

      socket->resetDevices ();
    }
  }

  return devices;
}

int CControlPoint::extractDevicesFromNotify (int timeout)
{
  int cDevices = 0;
  if (!m_closing)
  {
    QList<CUpnpSocket::SNDevice> const & ndevs = ndevices ();
    cDevices = m_devices.extractDevicesFromNotify (ndevs, timeout);
  }

  return cDevices;
}

CActionInfo CControlPoint::invokeAction (CDevice& device, CService& service,
                                         QString const & actionName, QList<TArgValue>& args, int timeout)
{
  m_lastActionError.clear ();
  bool           success = false;
  CActionInfo    actionInfo;
  if (!m_closing)
  {
    QString const & uuid       = device.uuid (); // Save device uuid because it can be deleted during event loop.
    CDevice::EType  deviceType = device.type ();
    CActionManager  actionManager (m_devices.networkAccessManager ());
    connect (&actionManager, SIGNAL(networkError(QString const &, QNetworkReply::NetworkError, QString const & )),
             this, SLOT(networkAccessManager(QString const &, QNetworkReply::NetworkError, QString const &)));

    TMActions const & actions = service.actions (); // Action list of service
    if (actions.contains (actionName))
    { // The service has the action
      CAction const & action  = actions.value (actionName);
      QUrl            url     = device.url (); // Base url
      url.setPath (service.controlURL ()); // complete service url
      actionInfo.startMessage (device.uuid (), service.serviceType (), actionName); // Start the HTTP message with upnp format.

      success                       = true;
      TMArguments const & arguments = action.arguments (); // Argument list of action.
      for (TArgValue const & arg : args)
      {
        if (arguments.contains (arg.first))
        { // Known argument name.
          CArgument const & argument = arguments.value (arg.first); // Get the argument.
          if (argument.dir () == CArgument::In)
          { // In argument
            QString const &   relatedStateVariableName = argument.relatedStateVariable (); // Get related state variable name.
            TMStateVariables& stateVariables           = service.stateVariables (); // Get related state variable.
            if (stateVariables.contains (relatedStateVariableName))
            { // Known state variable.
              CStateVariable& stateVariable = stateVariables[relatedStateVariableName]; //Get the variable.
              stateVariable.setValue (arg.second); // Change the variable value.
              actionInfo.addArgument (arg.first, arg.second); // Add argument at the SOAP message
            }
            else
            {
              argStateVarRelationship (arg.first, relatedStateVariableName, actionName, device, service);
            }
          }
        }
        else
        {
          unknownArg (arg.first, actionName, device, service);
          success = false;
          break;
        }
      }

      if (success)
      {
        actionInfo.endMessage (); // End the HTTP message.
        startNetworkCom (deviceType);
        success = actionManager.post (uuid, url, actionInfo, timeout); // Invoke the action.
        endNetworkCom (deviceType);
        if (success && m_devices.contains (uuid))
        {
          actionInfo.setSucceeded (success);

          // Parse the action response.
          QMap<QString, QString> vars; // Response of the action.
          CXmlHAction            h (actionName, vars);
          h.parse (actionInfo.response ());
          int errorCode = h.errorCode ();
          if (errorCode != 0)
          {
            emit upnpError (errorCode, h.errorDesc ());
          }
          else
          {
            TMArguments const & arguments = action.arguments ();
            // Update the out arguments value.
            for (TArgValue& arg : args)
            {
              if (arguments.contains (arg.first))
              { // Known argument.
                CArgument const & argument                 = arguments.value (arg.first);
                QString const &   relatedStateVariableName = argument.relatedStateVariable ();
                TMStateVariables& stateVariables           = service.stateVariables ();
                if (stateVariables.contains (relatedStateVariableName))
                { // Known state variable.
                  CStateVariable& stateVariable = stateVariables[relatedStateVariableName];
                  if (argument.dir () == CArgument::Out)
                  { // Argument out direction.
                    QString value = vars.value (arg.first);
                    stateVariable.setValue (value); // Update the related state variable.
                    stateVariable.constraints ().clear (); // Constraints are used only from event response.
                    arg.second = value; // Update the argument value.
                  }
                }
                else
                {
                  argStateVarRelationship (arg.first, relatedStateVariableName, actionName, device, service);
                }
              }
              else
              {
                unknownArg (arg.first, actionName, device, service);
              }
            }
          }
        }
      }
    }
    else
    {
      unknownAction (actionName, device, service);
    }
  }

  return actionInfo;
}

CActionInfo CControlPoint::invokeAction (QString const & deviceUUID, QString const & serviceID,
                                         QString const & actionName, QList<TArgValue>& args, int timeout)
{
  CActionInfo actionInfo;
  if (!deviceUUID.isEmpty () && m_devices.contains (deviceUUID))
  {
    CDevice&    device   = m_devices[deviceUUID];
    TMServices& services = device.services ();
    if (services.contains (serviceID))
    {
      CService& service = services[serviceID];
      actionInfo        = invokeAction (device, service, actionName, args, timeout);
    }
    else
    {
      m_lastActionError.clear ();
      m_lastActionError.setString (ErrorDeviceUUID, deviceUUID);
      m_lastActionError.setString (ErrorServiceTypeOrID, serviceID);
    }
  }
  else
  {
    m_lastActionError.clear ();
    m_lastActionError.setString (ErrorDeviceUUID, deviceUUID);
  }

  return actionInfo;
}

CActionInfo CControlPoint::invokeAction (QString const & deviceUUID,
                QString const & actionName, QList<TArgValue>& args, int timeout)
{
  CActionInfo actionInfo;
  if (!deviceUUID.isEmpty () && m_devices.contains (deviceUUID))
  {
    CDevice&             device   = m_devices[deviceUUID];
    TMServices&          services = device.services ();
    TMServices::iterator its      = services.begin (), end = services.end ();
    while (its != end && !actionInfo.succeeded ())
    {
      CService& service = *its;
      actionInfo        = invokeAction (device, service, actionName, args, timeout);
      if (!m_devices.contains (deviceUUID))
      {
        break; // Must be deleted during event loop.
      }

      ++its;
    }
  }
  else
  {
    m_lastActionError.clear ();
    m_lastActionError.setString (ErrorDeviceUUID, deviceUUID);
  }

  return actionInfo;
}

CStateVariable CControlPoint::stateVariable (QString const & deviceUUID, QString const & serviceID, QString const & name) const
{
  CStateVariable var;
  if (!deviceUUID.isEmpty ())
  {
    CDevice const & device = this->device (deviceUUID);
    var                    = device.stateVariable (name, serviceID);
  }

  return var;
}

bool CControlPoint::subscribe (QString const & deviceUUID, int renewalDelay, int requestTimeout)
{
  bool success = false;
  if (!deviceUUID.isEmpty () && m_devices.contains (deviceUUID))
  {
    CDevice&       device = m_devices[deviceUUID];
    CDevice::EType type   = device.type ();
    networkComStarted (type);
    success = m_devices.subscribe (device, renewalDelay, requestTimeout);
    if (success)
    {
      auto timer = new QTimer;
      timer->setInterval ((renewalDelay - m_renewalGard) * 1000);
      timer->setSingleShot (true);
      timer->start ();
      connect (timer, SIGNAL(timeout()), this, SLOT(renewalTimeout()));
      m_subcriptionTimers.insert (deviceUUID, TSubscriptionTimer (timer, requestTimeout));
    }

    networkComEnded (type);
  }

  return success;
}

void CControlPoint::unsubscribe (QString const & deviceUUID, int requestTimeout)
{
  if (!deviceUUID.isEmpty () && m_devices.contains (deviceUUID))
  {
    CDevice&       device = m_devices[deviceUUID];
    CDevice::EType type   = device.type ();
    QTimer* timer = m_subcriptionTimers.value (deviceUUID).first;
    if (timer != nullptr)
    {
      delete timer;
      m_subcriptionTimers.remove (deviceUUID);
    }

    networkComStarted (type);
    m_devices.unsubscribe (device, requestTimeout);
    networkComEnded (type);
  }
}

void CControlPoint::renewSubscribe (QString const & deviceUUID, int requestTimeout)
{
  if (!deviceUUID.isEmpty () && m_devices.contains (deviceUUID))
  {
    CDevice&       device = m_devices[deviceUUID];
    CDevice::EType type   = device.type ();
    networkComStarted (type);
    m_devices.renewSubscribe (device, requestTimeout);
    networkComEnded (type);
    QTimer* timer = m_subcriptionTimers.value (deviceUUID).first;
    timer->start ();
  }
}

QStringList CControlPoint::devices (CDevice::EType type) const
{
  QStringList deviceUUIDs;
  for (CDevice const & device : m_devices)
  {
    if (device.type () == type)
    {
      deviceUUIDs << device.uuid ();
    }
  }

  return deviceUUIDs;
}

int CControlPoint::devicesCount (CDevice::EType type) const
{
  int count = 0;
  for (CDevice const & device : m_devices)
  {
    if (device.type () == type)
    {
      ++count;
    }
  }

  return count;
}

bool CControlPoint::hasDevice (CDevice::EType type) const
{
  bool success = false;
  for (CDevice const & device : m_devices)
  {
    if (device.type () == type)
    {
      success = true;
      break;
    }
  }

  return success;
}

QStringList CControlPoint::renderers () const
{
  return devices (CDevice::MediaRenderer);
}

QStringList CControlPoint::servers () const
{
  return devices (CDevice::MediaServer);
}

bool CControlPoint::hasRenderer () const
{
  return hasDevice (CDevice::MediaRenderer);
}

bool CControlPoint::hasServer () const
{
  return hasDevice (CDevice::MediaServer);
}

int CControlPoint::renderersCount () const
{
  return devicesCount (CDevice::MediaRenderer);
}

int CControlPoint::serversCount () const
{
  return devicesCount (CDevice::MediaServer);
}

QString CControlPoint::playlistURI (QString const & name) const
{
  QString             uri;
  CHTTPServer const * server = httpServer ();
  if (server != nullptr)
  {
    uri = server->playlistURI (name);
  }

  return uri;
}

QString CControlPoint::serverListenAddress () const
{
  QString             uri;
  CHTTPServer const * server = httpServer ();
  if (server != nullptr)
  {
    uri = server->serverListenAddress ();
  }

  return uri;
}

int CControlPoint::setPlaylistContent (QList<CDidlItem::TPlaylistElem> const & items, CDidlItem::EPlaylistFormat format)
{
  int           size   = 0;
  CHTTPServer * server = httpServer ();
  if (server != nullptr)
  {
    QByteArray content = CDidlItem::buildsPlaylist (items, format);
    server->setPlaylistContent (content);
    size = content.size ();
  }

  return size;
}

void CControlPoint::setPlaylistName (QString const & name)
{
  CHTTPServer* server = httpServer ();
  if (server != nullptr)
  {
    server->setPlaylistName (name);
  }
}

QString CControlPoint::playlistName () const
{
  QString             name;
  CHTTPServer const * server = httpServer ();
  if (server != nullptr)
  {
    name = server->playlistName ();
  }

  return name;
}

void CControlPoint::clearPlaylist ()
{
  CHTTPServer* server = httpServer ();
  if (server != nullptr)
  {
    server->clearPlaylist ();
  }
}

QList<QString> CControlPoint::subscribedDevices () const
{
  QStringList devices;
  for (TMDevices::const_iterator it = m_devices.begin (), end = m_devices.end (); it != end; ++it)
  {
    CDevice const & device = it.value ();
    if (device.hasSubscribed ())
    {
      devices << it.key ();
    }
  }

  return devices;
}

void CControlPoint::removeDevice (QString const & uuid)
{
  m_devices.removeDevice (uuid);
}

void CControlPoint::initActionError (QString const & action, CDevice const & device, CService const & service)
{
  m_lastActionError.setString (ErrorDeviceUUID, device.uuid ());
  m_lastActionError.setString (ErrorServiceTypeOrID, service.serviceType ());
  m_lastActionError.setString (ErrorActionName, action);
}

void CControlPoint::initArgError (QString const & arg, QString const & action, CDevice const & device, CService const & service)
{
  initActionError (action,  device, service);
  m_lastActionError.setString (ErrorArgName, arg);
}

void CControlPoint::unknownAction (QString const & action, CDevice const & device, CService const & service)
{
  initActionError (action,  device, service);
  m_lastActionError.setString (ErrorActionString,
                               QString ("Unknown action: %1 for device : %2 and service: %3")
                                        .arg (action, device.uuid (), service.serviceType ()));
}

void CControlPoint::unknownArg (QString const & action, QString const & arg, CDevice const & device, CService const & service)
{
  initArgError (action, arg, device, service);
  m_lastActionError.setString (ErrorActionString,
                               QString ("Unknown argument: %1 for action: %2 for device : %3 and service: %4")
                               .arg (arg, action, device.uuid (), service.serviceType ()));
}

void CControlPoint::argStateVarRelationship (QString const & arg, QString const & stateVar, QString const & action,
                    CDevice const & device, CService const & service)
{
  initArgError (action, arg, device, service);
  m_lastActionError.setString (ErrorRelatedStateVar, stateVar);
  m_lastActionError.setString (ErrorActionString,
                               QString ("Wrong relationship between argument and state variable: %1 for action: %2 for device : %3 and service: %4")
                                        .arg (arg, action, device.uuid (), service.serviceType ()));
}

void CControlPoint::unknownService (QString const & uuid, QString const & id)
{
  m_lastActionError.setString (ErrorDeviceUUID, uuid);
  m_lastActionError.setString (ErrorServiceTypeOrID, id);
  m_lastActionError.setString (ErrorActionString,
                               QString ("Wrong service identifier: %1 for device: %2").arg (uuid, id));

}

void CControlPoint::unknownDevice (QString const & uuid)
{
  m_lastActionError.setString (ErrorDeviceUUID, uuid);
  m_lastActionError.setString (ErrorActionString, QString ("Wrong device uuid: %1").arg (uuid));
}

QNetworkAccessManager* CControlPoint::networkAccessManager () const
{
  return m_devices.networkAccessManager ();
}

void CControlPoint::close ()
{
  m_closing         = true;
  QStringList uuids = subscribedDevices ();
  if (!uuids.isEmpty ())
  {
    for (QString const & uuid : uuids)
    {
      unsubscribe (uuid);
    }
  }
}

QByteArray CControlPoint::callData (QString const & uri, int timeout)
{
  QByteArray bytes = CDataCaller (m_devices.networkAccessManager ()).callData (uri, timeout);
  return bytes;
}

QString CControlPoint::deviceUUID (QString const & uri, CDevice::EType type) const
{
  QString     deviceUUID;
  QString     ip    = QUrl (uri).host ();
  QStringList uuids = devices (type);
  for (QString const & uuid : uuids)
  {
    CDevice const & device = this->device (uuid);
    QUrl const &    url    = device.url ();
    QString         host   = url.host ();
    if (host == ip)
    {
      deviceUUID = uuid;
      break;
    }
  }

  return deviceUUID;
}

void CControlPoint::loadPlugins ()
{
  QString appFolder = ::applicationFolder ();
  QDir    folder (appFolder);
  if (folder.exists () && folder.cd ("plugins"))
  {
    QStringList            names = folder.entryList (QDir::Files | QDir::NoDotAndDotDot);
    QMap<QString, QString> files;
    foreach (QString const & name, names)
    {
      QString   filePath = folder.absoluteFilePath (name);
      QFileInfo fi (filePath);
      if (!fi.isSymLink ())
      {
        QString suffix = fi.suffix ().toLower ();
        if (suffix == "so" || suffix.toLower () == "dll")
        { // It is .dll or .so files.
          QFunctionPointer fct = QLibrary::resolve (filePath, "pluginObject");
          if (fct != nullptr)
          {
            auto     pluginObject = reinterpret_cast<CPlugin::TFctPluginObject>(fct);
            CPlugin* plugin       = (*pluginObject) ();
            if (plugin != nullptr)
            {
              plugin->setName (fi.baseName ());
              QString const & uuid = plugin->uuid ();
              m_plugins.insert (uuid, plugin);
              files.insert (uuid, filePath);
              break;
            }
          }
        }
      }
    }

    // For each plugins get image and authentification files.
    for (QMap<QString, QString>::const_iterator it = files.cbegin (), end = files.cend (); it != end; ++it)
    {
      QString const & uuid   = it.key ();
      QString         file   = it.value ();
      CPlugin*        plugin = m_plugins.value (uuid);
      if (plugin != nullptr)
      {
        int index = file.lastIndexOf ('.');
        if (index != -1)
        {
          file.truncate (index);
          file += ".png";
          if (QFile::exists (file))
          {
            plugin->setPixmap (file);
          }
          else
          {
            file.truncate (index);
            file += ".jpg";
            if (QFile::exists (file))
            {
              plugin->setPixmap (file);
            }
          }

          file.truncate (index);
          file += ".ids";
          if (QFile::exists (file))
          {
            plugin->restoreAuth (file);
          }
        }
      }
    }
  }
}

QStringList CControlPoint::plugins () const
{
  QStringList uuids;
  uuids.reserve (20);
  for (QMap<QString, CPlugin*>::const_iterator it = m_plugins.cbegin (), end = m_plugins.end (); it != end; ++it)
  {
    uuids.append (it.key ());
  }

  return uuids;
}

CPlugin* CControlPoint::plugin (QString const & uuid)
{
  return m_plugins.value (uuid);
}

void CControlPoint::mediaRequest (QString request)
{
  bool success = false;
  request.remove ("/plugin/");
  int index = request.indexOf ('/');
  if (index != -1)
  {
    QString uuid = request.left (index);
    if (!uuid.isEmpty ())
    {
      request = request.mid (index + 1);
      if (!request.isEmpty ())
      {
        uuid            = CHTTPServer::unformatUUID (uuid);
        CPlugin* plugin = m_plugins.value (uuid);
        if (plugin != nullptr)
        {
          QNetworkRequest nreq = plugin->mediaRequest (request);
          httpServer ()->setStreamingRequest (nreq);
          success = nreq.url ().isValid ();
        }
      }
    }
  }

  if (!success)
  {
    request.clear ();
  }
}

void CControlPoint::abortStreaming ()
{
  CHTTPServer* server = httpServer ();
  if (server != nullptr)
  {
    server->abortStreaming ();
  }
}

void CControlPoint::rendererComStarted ()
{
  emit networkComStarted (CDevice::MediaRenderer);
}

void CControlPoint::rendererComEnded ()
{
  emit networkComEnded (CDevice::MediaRenderer);
}

void CControlPoint::serverComStarted ()
{
  emit networkComStarted (CDevice::MediaServer);
}

void CControlPoint::serverComEnded ()
{
  emit networkComEnded (CDevice::MediaServer);
}

QList<QPair<QString, QUrl>> CControlPoint::devicesFinding () const
{
  QList<QPair<QString, QUrl>> pairs;
  pairs.reserve (m_devices.size ());
  for (CDevice const & device : m_devices)
  {
    if (!device.isSubDevice ())
    { // Only root devices because subdevices will be detected from root devices.
      pairs.append (QPair<QString, QUrl> (device.uuid (), device.url ()));
    }
  }

  return pairs;
}

int CControlPoint::extractDevices (QList<QPair<QString, QUrl>> const & pairs, bool oneByOne, int timeout)
{
  QList<CUpnpSocket::SNDevice> ndevs;
  int                          cDevices = 0;
  if (oneByOne)
  {
    for (CUpnpSocket::SNDevice const & ndev : ndevs)
    {
      QList<CUpnpSocket::SNDevice> ndevTemp;
      ndevTemp.append (ndev);
      cDevices += m_devices.extractDevicesFromNotify (ndevTemp, timeout);
      QCoreApplication::sendPostedEvents ();
      QCoreApplication::processEvents ();
    }
  }
  else
  {
    ndevs.reserve (pairs.size ());
    for (QPair<QString, QUrl> const & pair : pairs)
    {
      CUpnpSocket::SNDevice ndev;
      ndev.m_type = CUpnpSocket::SNDevice::Response;
      ndev.m_url  = pair.second;
      ndev.m_uuid = pair.first;
      ndevs.append (ndev);
    }

    cDevices = m_devices.extractDevicesFromNotify (ndevs, timeout);
  }

  return cDevices;
}
