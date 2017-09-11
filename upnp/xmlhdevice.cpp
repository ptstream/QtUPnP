#include "xmlhdevice.hpp"
#include "device.hpp"

USING_UPNP_NAMESPACE

CXmlHDevice::CXmlHDevice (CDevice& device) : m_device (device), m_current (&m_device)
{
}

bool CXmlHDevice::startElement (QString const & namespaceURI, QString const & localName, QString const & qName, QXmlAttributes const & atts)
{
  CXmlH::startElement (namespaceURI, localName, qName, atts);
  if (qName == "icon")
  {
    m_current->addPixmap ();
  }
  else if (qName == "service")
  {
    m_tempServices.top ().push_back (TTempService ());
  }
  else if (qName == "device")
  {
    if (m_deviceList)
    {
      m_subDevices.push (m_current);
      CDevice device;
      device.setParentUUID (m_current->uuid ());
      m_current->subDevices ().push_back (device);
      m_current = &m_current->subDevices ().last ();
    }

    m_tempServices.push (QVector<TTempService> ());
    m_tempServices.top ().reserve (10);
  }
  else if (qName == "deviceList")
  {
    m_deviceList = true;
  }

  return true;
}

bool CXmlHDevice::characters (QString const & name)
{
  QString const & tag = m_stack.top ();
  if (tag == "major")
  {
    m_current->setMajorVersion (name.toUInt ());
  }
  else if (tag == "minor")
  {
    m_current->setMinorVersion (name.toUInt ());
  }
  else if (tag == "deviceType")
  {
    m_current->setDeviceType (name);
  }
  else if (tag == "dlna:X_DLNADOC")
  {
    m_current->addDlnaDoc (name);
  }
  else if (tag == "dlna:X_CAP")
  {
    QStringList caps = name.split (',');
    m_current->setDlnaCaps (caps);
  }
  else if (tag == "UDN")
  {
    m_current->setUUID (name);
  }
  else if (tag == "friendlyName")
  {
    m_current->setFriendlyName (name);
  }
  else if (tag == "manufacturer")
  {
    m_current->setManufacturer (name);
  }
  else if (tag == "manufacturerURL")
  {
    m_current->setManufacturerURL (name);
  }
  else if (tag == "modelName")
  {
    m_current->setModelName (name);
  }
  else if (tag == "modelNumber")
  {
    m_current->setModelNumber (name);
  }
  else if (tag == "modelURL")
  {
    m_current->setModelURL (name);
  }
  else if (tag == "modelDescription")
  {
    m_current->setModelDesc (name);
  }
  else if (tag == "serialNumber")
  {
    m_current->setSerialNumber (name);
  }
  else if (tag == "upc")
  {
    m_current->setUpc (name);
  }
  else if (tag == "presentationURL")
  {
    m_current->setPresentationURL (name);
  }
  else if (tag == "mimetype")
  {
    QList<CDevicePixmap>& pixmaps = m_current->pixmaps ();
    if (!pixmaps.isEmpty ())
    {
      pixmaps.last ().setMimeType (name);
    }
  }
  else if (tag == "width")
  {
    QList<CDevicePixmap>& pixmaps = m_current->pixmaps ();
    if (!pixmaps.isEmpty ())
    {
      pixmaps.last ().setWidth (name.toInt ());
    }
  }
  else if (tag == "height")
  {
    QList<CDevicePixmap>& pixmaps = m_current->pixmaps ();
    if (!pixmaps.isEmpty ())
    {
      pixmaps.last ().setHeight (name.toInt ());
    }
  }
  else if (tag == "depth")
  {
    QList<CDevicePixmap>& pixmaps = m_current->pixmaps ();
    if (!pixmaps.isEmpty ())
    {
      pixmaps.last ().setDepth (name.toInt ());
    }
  }
  else if (tag == "url")
  {
    QList<CDevicePixmap>& pixmaps = m_current->pixmaps ();
    if (!pixmaps.isEmpty ())
    {
      pixmaps.last ().setUrl (name);
    }
  }
  else if (tag == "URLBase")
  {
    m_urlBase = name;
  }
  else if (tag == "serviceType")
  {
    if (!m_tempServices.top ().isEmpty ())
    {
      m_tempServices.top ().last ()[Type] = name;
    }
  }
  else if (tag == "serviceId")
  {
    if (!m_tempServices.top ().isEmpty ())
    {
      m_tempServices.top ().last ()[Id] = name;
    }
  }
  else if (tag == "SCPDURL")
  {
    if (!m_tempServices.top ().isEmpty ())
    {
      m_tempServices.top ().last ()[ScpdURL] = prependSlash (name);
    }
  }
  else if (tag == "controlURL")
  {
    if (!m_tempServices.top ().isEmpty ())
    {
      m_tempServices.top ().last ()[ControlURL] = prependSlash (name);
    }
  }
  else if (tag == "eventSubURL")
  {
    if (!m_tempServices.top ().isEmpty ())
    {
      m_tempServices.top ().last ()[EventSubURL] = prependSlash (name);
    }
  }

  return true;
}

bool CXmlHDevice::endElement (QString const & namespaceURI, QString const & localName, QString const & qName)
{
  CXmlH::endElement (namespaceURI, localName, qName);
  if (qName == "device")
  {
    TMServices services;
    for (TTempService const & tempService : m_tempServices.pop ())
    {
      CService service;
      service.setServiceType (tempService[Type]);
      service.setControlURL (tempService[ControlURL]);
      service.setScpdURL (tempService[ScpdURL]);
      service.setEventSubURL (tempService[EventSubURL]);
      services.insert (tempService[Id], service);
    }

    m_current->setServices (services);
    if (m_current->url ().isEmpty () && !m_urlBase.isEmpty ())
    {
      m_current->setURL (m_urlBase);
    }

    m_current = !m_subDevices.empty () ? m_subDevices.pop () : &m_device;
  }

  return true;
}
