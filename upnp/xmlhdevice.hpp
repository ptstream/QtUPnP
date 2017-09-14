#ifndef XMLH_DEVICE_HPP
#define XMLH_DEVICE_HPP

#include "xmlh.hpp"

START_DEFINE_UPNP_NAMESPACE

class CDevice;

/*! \brief Provides the implementation of the device response parser. */
class CXmlHDevice : public CXmlH
{
public:
  enum ETempService { Id, Type, ScpdURL, ControlURL, EventSubURL, Last };
  typedef std::array<QString, Last> TTempService;

  /*! Default constructor. */
  CXmlHDevice (CDevice& device);

  /*! The reader calls this function when it has parsed a start element tag.
   * See  QXmlContentHandler documentation.
   */
  virtual bool startElement (QString const & namespaceURI, QString const & localName, QString const & qName, QXmlAttributes const & atts);

  /*! The parser calls this function when it has parsed a chunk of character data
   * See  QXmlContentHandler documentation.
   */
  virtual bool characters (QString const & name);

  /*! The reader calls this function when it has parsed an end element tag.
   * See  QXmlContentHandler documentation.
   */
  virtual bool endElement (QString const & namespaceURI, QString const & localName, QString const & qName);

private :
  CDevice& m_device;
  QStack<QVector<TTempService>> m_tempServices;
  QStack<CDevice*> m_subDevices;
  CDevice* m_current = nullptr; //!< Current parsed device.
  QString m_urlBase; //!< base url in case of embedded devices.
  bool m_deviceList = false;
};

} // Namespace

#endif // XMLH_DEVICE_HPP
