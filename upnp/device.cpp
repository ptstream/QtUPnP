#include "device.hpp"
#include "xmlhdevice.hpp"
#include "datacaller.hpp"

START_DEFINE_UPNP_NAMESPACE

/*! \brief Internal structure of CDevice. */
struct SDeviceData : public QSharedData
{
  SDeviceData () {}
  SDeviceData (SDeviceData const & other);

  mutable qint8 m_managePlaylists = -1; //!< The renderer can managed playlits.
  int m_type = 0; //!< The type of the device e.g. server.
  QUrl m_url; //!< The url.
  QString m_uuid; //! < The uuid.
  QString m_modelName; //!< The model name.
  QString m_modelNumber; //!< The model number.
  QString m_modelURL;
  QString m_modelDesc;
  QString m_serialNumber;
  QString m_upc;
  QString m_presentationURL;
  QString m_manufacturer; //!< Manufacturer.
  QString m_manufacturerURL;
  QString m_friendlyName; //!< The friendly name.
  QString m_deviceType; //!< Device type.
  QStringList m_dlnaDocs;
  QStringList m_dlnaCaps;
  QList<CDevicePixmap> m_pixmaps; //!< The list of pixmaps.
  unsigned short m_minorVersion = 0; //!< Minor version.
  unsigned short m_majorVersion = 1; //!< Major version.
  unsigned m_connectionID = 0;
  TMServices m_services; //!< The list of services.
  QList<CDevice> m_subDevices; //!< Sub device list.
  QString m_parentUUID; //!< Parent uuid in case of embedded device.
};

SDeviceData::SDeviceData (SDeviceData const & other) :  QSharedData (other),
      m_managePlaylists (other.m_managePlaylists),
      m_type (other.m_type),
      m_url (other.m_url), m_uuid (other.m_uuid), m_modelName (other.m_modelName),
      m_modelNumber (other.m_modelNumber), m_modelURL (other.m_modelURL), m_modelDesc (other.m_modelDesc),
      m_serialNumber (other.m_serialNumber), m_manufacturer (other.m_manufacturer),
      m_manufacturerURL (other.m_manufacturerURL),
      m_friendlyName (other.m_friendlyName),
      m_deviceType (other.m_deviceType), m_dlnaDocs (other.m_dlnaDocs), m_dlnaCaps (other.m_dlnaCaps),
      m_pixmaps (other.m_pixmaps),
      m_minorVersion (other.m_minorVersion), m_majorVersion (other.m_majorVersion),
      m_connectionID (other.m_connectionID), m_services (other.m_services),
      m_subDevices (other.m_subDevices), m_parentUUID (other.m_parentUUID)
{
}

}// Namespace

USING_UPNP_NAMESPACE

CDevice::CDevice () : m_d (new SDeviceData)
{
}

CDevice::CDevice (CDevice const & rhs) : m_d (rhs.m_d)
{
}

CDevice &CDevice::operator = (CDevice const & rhs)
{
  if (this != &rhs)
  {
    m_d.operator = (rhs.m_d);
  }

  return *this;
}

CDevice::~CDevice ()
{
}

void CDevice::setType (EType type)
{
  m_d->m_type = type;
}

void CDevice::setType ()
{
  EType           type       = Unknown;
  QString const & deviceType = this->deviceType ();
  if (deviceType.indexOf ("MediaServer") != -1)
  {
    type = MediaServer;
  }
  else if (deviceType.indexOf ("MediaRender") != -1)
  {
    type = MediaRenderer;
  }
  else if (deviceType.indexOf ("DimmableLight") != -1)
  {
    type = DimmableLight;
  }
  else if (deviceType.indexOf ("Printer") != -1)
  {
    type = Printer;
  }
  else if (deviceType.indexOf ("Basic") != -1)
  {
    type = Basic;
  }
  else if (deviceType.indexOf ("InternetGatewayDevice") != -1)
  {
    type = InternetGateway;
  }
  else if (deviceType.indexOf ("WANDevice") != -1)
  {
    type = WANDevice;
  }
  else if (deviceType.indexOf ("WANConnectionDevice") != -1)
  {
    type = WANConnectionDevice;
  }

  setType (type);
}

void CDevice::setURL (QUrl const & url)
{
  m_d->m_url = url;
}

void CDevice::setUUID (QString const & uuid)
{
  m_d->m_uuid = uuid;
}

void CDevice::setPlaylistStatus (EPlaylistStatus state)
{
  m_d->m_managePlaylists = state;
}

void CDevice::setModelName (QString const & name)
{
  m_d->m_modelName = name;
}

void CDevice::setModelNumber (QString const & number)
{
  m_d->m_modelNumber = number;
}

void CDevice::setModelURL (QString const & url)
{
  m_d->m_modelURL = url;
}

void CDevice::setModelDesc (QString const & desc)
{
  m_d->m_modelDesc = desc;
}

void CDevice::setSerialNumber (QString const & number)
{
  m_d->m_serialNumber = number;
}

void CDevice::setUpc (QString const & upc)
{
  m_d->m_upc = upc;
}

void CDevice::setPresentationURL (QString const & url)
{
  m_d->m_presentationURL = url;
}

void CDevice::setManufacturer (QString const & manufacturer)
{
  m_d->m_manufacturer = manufacturer;
}

void CDevice::setManufacturerURL (QString const & url)
{
  m_d->m_manufacturerURL = url;
}

void CDevice::setFriendlyName (QString const & name)
{
  m_d->m_friendlyName = name;
}

void CDevice::setDeviceType (QString const & type)
{
  m_d->m_deviceType = type;
}

void CDevice::setDlnaDocs (QStringList const & docs)
{
  m_d->m_dlnaDocs = docs;
}

void CDevice::addDlnaDoc (QString const & doc)
{
  m_d->m_dlnaDocs << doc;
}

void CDevice::setDlnaCaps (QStringList const & caps)
{
  m_d->m_dlnaCaps = caps;
}

void CDevice::addPixmap ()
{
  m_d->m_pixmaps.push_back (CDevicePixmap ());
}

void CDevice::setPixmaps (QList<CDevicePixmap> const & pixmaps)
{
  m_d->m_pixmaps = pixmaps;
}

void CDevice::setMajorVersion (unsigned version)
{
  m_d->m_majorVersion = static_cast<unsigned short>(version);
}

void CDevice::setMinorVersion (unsigned version)
{
  m_d->m_minorVersion = static_cast<unsigned short>(version);
}

void CDevice::setConnectionID (unsigned id)
{
  m_d->m_connectionID = id;
}

void CDevice::setServices (TMServices const & services)
{
  m_d->m_services = services;
}

void CDevice::setParentUUID (QString const & uuid)
{
  m_d->m_parentUUID = uuid;
}

CDevice::EType CDevice::type () const
{
  return static_cast<EType>(m_d->m_type);
}

QString const & CDevice::uuid () const
{
  return m_d->m_uuid;
}

QUrl const & CDevice::url () const
{
  return m_d->m_url;
}

CDevice::EPlaylistStatus CDevice::playlistStatus () const
{
  if (m_d->m_managePlaylists == UnknownHandler)
  {
    m_d->m_managePlaylists = hasProtocol ("audio/x-mpegurl") ? PlaylistHandler : NoPlaylistHandler;
  }

  return static_cast<EPlaylistStatus>(m_d->m_managePlaylists);
}

bool CDevice::isSubDevice () const
{
  return !m_d->m_parentUUID.isEmpty ();
}

QString const & CDevice::modelName () const
{
  return m_d->m_modelName;
}

QString const & CDevice::modelNumber () const
{
  return m_d->m_modelNumber;
}

QString const & CDevice::modelURL () const
{
  return m_d->m_modelURL;
}

QString const & CDevice::modelDesc () const
{
  return m_d->m_modelDesc;
}

QString const & CDevice::serialNumber () const
{
  return m_d->m_serialNumber;
}

QString const & CDevice::upc () const
{
  return m_d->m_upc;
}

QString const & CDevice::presentationURL () const
{
  return m_d->m_presentationURL;
}

QString const & CDevice::manufacturer () const
{
  return m_d->m_manufacturer;
}

QString const & CDevice::manufacturerURL () const
{
  return m_d->m_manufacturerURL;
}

QString const & CDevice::friendlyName () const
{
  return m_d->m_friendlyName;
}

QString const & CDevice::deviceType () const
{
  return m_d->m_deviceType;
}

QStringList const & CDevice::dlnaDocs () const
{
  return m_d->m_dlnaDocs;
}

QStringList const & CDevice::dlnaCaps () const
{
  return m_d->m_dlnaCaps;
}

QList<CDevicePixmap> const & CDevice::pixmaps () const
{
  return m_d->m_pixmaps;
}

QList<CDevicePixmap>& CDevice::pixmaps ()
{
  return m_d->m_pixmaps;
}

unsigned CDevice::minorVersion () const
{
  return m_d->m_minorVersion;
}

unsigned CDevice::majorVersion () const
{
  return m_d->m_majorVersion;
}

unsigned CDevice::highestSupportedVersion () const
{
  unsigned version = 0;
  int      index   = m_d->m_deviceType.lastIndexOf (':');
  if (index != -1)
  {
    QString s = m_d->m_deviceType.right (m_d->m_deviceType.length () - index - 1);
    version   = s.toUInt ();
  }

  return version;
}

unsigned CDevice::connectionID () const
{
  return m_d->m_connectionID;
}

TMServices const & CDevice::services () const
{
  return m_d->m_services;
}

TMServices& CDevice::services ()
{
  return m_d->m_services;
}

QString const & CDevice::parentUUID () const
{
  return m_d->m_parentUUID;
}

QString CDevice::name () const
{
  QString name = friendlyName ();
  if (name.isEmpty ())
  {
    name = modelName ();
    if (name.isEmpty ())
    {
      name = url ().toString ();
    }
  }

  return name;
}

QUrl CDevice::pixmap (EPreferedPixmapType type, char const * mimeType, int width, int height) const
{
  QUrl                         url;
  QString                      path;
  int                          preferedCriteria = width * height * 24;
  QList<CDevicePixmap> const & pixmaps          = this->pixmaps ();
  if (!pixmaps.isEmpty ())
  {
    switch (type)
    {
      case Nearest :
      {
        int dmin = std::numeric_limits<int>::max ();
        for (CDevicePixmap const & pixmap : pixmaps)
        {
          int dist = pixmap.preferedCriteria () - preferedCriteria;
          if (dist < dmin || (dist == dmin && pixmap.hasMimeType (mimeType)))
          {
            dmin = dist;
            path = pixmap.url ();
          }
        }

        break;
      }

      case SmResol :
      {
        int cmin = std::numeric_limits<int>::max ();
        for (CDevicePixmap const & pixmap : pixmaps)
        {
          int criteria = pixmap.preferedCriteria ();
          if ((criteria < cmin) || (criteria == cmin && pixmap.hasMimeType (mimeType)))
          {
            cmin = criteria;
            path = pixmap.url ();
          }
        }

        break;
      }

      case HiResol :
      {
        int cmax = -std::numeric_limits<int>::max ();
        for (CDevicePixmap const & pixmap : pixmaps)
        {
          int criteria = pixmap.preferedCriteria ();
          if ((criteria > cmax) || (criteria == cmax && pixmap.hasMimeType (mimeType)))
          {
            cmax = criteria;
            path = pixmap.url ();
          }
        }

        break;
      }
    }
  }

  if (!path.isEmpty ())
  {
    url = m_d->m_url;
    url.setPath (path);
  }

  return url;
}

QByteArray CDevice::pixmapBytes (QNetworkAccessManager* naMgr, EPreferedPixmapType type, char const * mimeType,
                                 int width, int height, int timeout) const
{
  QByteArray bytes;
  QUrl       url = pixmap (type, mimeType, width, height);
  if (!url.isEmpty ())
  {
    bytes = CDataCaller (naMgr).callData (url.toString (), timeout);
  }

  return bytes;
}

bool CDevice::parseXml (QByteArray const & data)
{
  CXmlHDevice h (*this);
  return h.parse (data) | CXmlH::tolerantMode ();
}

bool CDevice::extractServiceComponents (QNetworkAccessManager* naMgr, int timeout)
{
  bool success = false;
  for (TMServices::iterator its = m_d->m_services.begin (), end = m_d->m_services.end (); its != end; ++its)
  {
    CService& service = its.value ();
    QString   scpdURL = service.scpdURL ();
    success           = true;
    if (!scpdURL.isEmpty ())
    {
      QUrl url = m_d->m_url;
      url.setPath (scpdURL);
      CDataCaller dc (naMgr);
      QByteArray  data = dc.callData (url.toString (), timeout);
      if (!data.isEmpty ())
      {
        success = service.parseXml (data);
        if (success)
        {
          TMStateVariables const & variables = service.stateVariables ();
          for (TMStateVariables::const_iterator itv = variables.cbegin (), end = variables.cend (); itv != end; ++itv)
          {
            CStateVariable const & var = itv.value ();
            if (var.isEvented ())
            {
              service.setEvented (var.isEvented ());
              break;
            }
          }
        }
      }
    }
  }

  return success;
}

bool CDevice::hasSubscribed () const
{
  bool subscribed = false;
  for (TMServices::const_iterator it = m_d->m_services.begin (), end = m_d->m_services.end (); it != end; ++it)
  {
    CService const & service = it.value ();
    if (!service.subscribeSID ().isEmpty ())
    {
      subscribed = true;
    }
  }

  return subscribed;
}

QList<QString> CDevice::subscribedServices () const
{
  QStringList services;
  for (TMServices::const_iterator it = m_d->m_services.begin (), end = m_d->m_services.end (); it != end; ++it)
  {
    CService const & service = it.value ();
    if (!service.subscribeSID ().isEmpty ())
    {
      services << it.key ();
    }
  }

  return services;
}

CStateVariable CDevice::stateVariable (QString const & name, QString const & serviceID) const
{
  CStateVariable     variable;
  TMServices const & services = this->services ();
  if (serviceID.isEmpty ())
  {
    for (TMServices::const_iterator it = services.cbegin (), end = services.end (); it != end; ++it)
    {
      CService const &         service   = it.value ();
      TMStateVariables const & variables = service.stateVariables ();
      if (variables.contains (name))
      {
        variable = variables.value (name);
        break;
      }
    }
  }
  else
  {
    if (services.contains (serviceID))
    {
      CService const & service = services.value (serviceID);
      variable                 = service.stateVariable (name);
    }
  }

  return variable;
}

QList<CDevice> const & CDevice::subDevices () const
{
  return m_d->m_subDevices;
}

QList<CDevice>& CDevice::subDevices ()
{
  return m_d->m_subDevices;
}

QStringList CDevice::serviceIDs (QString const & actionName) const
{
  QStringList services;
  services.reserve (m_d->m_services.size ());
  for (TMServices::const_iterator its = m_d->m_services.cbegin (), end = m_d->m_services.cend (); its != end; ++its)
  {
    CService const &  service = its.value ();
    TMActions const & actions = service.actions ();
    if (actions.contains (actionName))
    {
      services << its.key ();
    }
  }

  return services;
}

CAction CDevice::action (QString const & serviceID, QString const & name) const
{
  CAction action;
  if (serviceID.isEmpty ())
  {
    for (TMServices::const_iterator its = m_d->m_services.cbegin (), end = m_d->m_services.cend (); its != end; ++its)
    {
      CService const &  service = its.value ();
      TMActions const & actions = service.actions ();
      if (actions.contains (name))
      {
        action = actions.value (name);
      }
    }
  }
  else
  {
    CService const & service = services ().value (serviceID);
    action                   = service.actions ().value (name);
  }

  return action;
}

bool CDevice::hasAction (QString const & serviceID, QString const & name) const
{
  bool success = false;
  if (serviceID.isEmpty ())
  {
    for (TMServices::const_iterator its = m_d->m_services.cbegin (), end = m_d->m_services.cend (); its != end && !success; ++its)
    {
      CService const &  service = its.value ();
      TMActions const & actions = service.actions ();
      success                   = actions.contains (name);
    }
  }
  else
  {
    CService const & service = services ().value (serviceID);
    success                  = service.actions ().contains (name);
  }

  return success;
}

bool CDevice::hasProtocol (QString const & protocol, bool exact) const
{
  bool             found     = false;
  CService const & service   = m_d->m_services.value ("urn:upnp-org:serviceId:ConnectionManager");
  CStateVariable   variable  = service.stateVariable ("SinkProtocolInfo");
  QString          protocols = variable.value ().toString ();
  if (protocols.isEmpty ())
  {
    variable  = service.stateVariable ("SourceProtocolInfo");
    protocols = variable.value ().toString ();
  }

  if (!exact)
  {
    found = protocols.indexOf (protocol) != -1;
  }
  else
  {
    QStringList protocolInfos = protocols.split (',');
    found = std::find (protocolInfos.begin (), protocolInfos.end (), protocol) != protocolInfos.end ();
  }

  return found;
}

CDevice CDevice::subDevice (QString const & uuid) const
{
  CDevice subDevice;
  for (CDevice const & device : m_d->m_subDevices)
  {
    if (device.uuid () == uuid)
    {
      subDevice = device;
      break;
    }
  }

  return subDevice;
}

bool CDevice::hasSubDevice (QString const & uuid) const
{
  bool success = false;
  for (CDevice const & device : m_d->m_subDevices)
  {
    if (device.uuid () == uuid)
    {
      success = true;
      break;
    }
  }

  return success;
}

void CDevice::setSubscribtionDisabled (QStringList const & ids, bool disable)
{
  bool enable = !disable;
  for (QString const & id : ids)
  {
    if (m_d->m_services.contains (id))
    {
      m_d->m_services[id].setEvented (enable);
    }
  }
}

QStringList CDevice::eventedServices () const
{
  QStringList ids;
  for (TMServices::const_iterator its = m_d->m_services.cbegin (), end = m_d->m_services.cend (); its != end; ++its)
  {
    CService const & service = its.value ();
    if (service.isEvented ())
    {
      ids << its.key ();
    }
  }

  return ids;
}
