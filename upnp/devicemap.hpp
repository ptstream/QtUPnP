#ifndef DEVICE_MAP_HPP
#define DEVICE_MAP_HPP 1

#include "device.hpp"
#include "eventingmanager.hpp"
#include "upnpsocket.hpp"
#include <QTimer>

class QNetworkAccessManager;

START_DEFINE_UPNP_NAMESPACE

/*! Defines the map of devices. */
typedef QMap<QString, CDevice> TMDevices;

class CHTTPServer;

/*! \brief It is the container of devices. */
class UPNP_API CDeviceMap : public TMDevices
{
public :
  /*! Default constructor. */
  CDeviceMap ();

  /*! Destructor. */
  ~CDeviceMap ();

  /*! Extracts devices from CUpnpSocket responds and add it in the map.
   * \param nDevices: upnp socket responds.
   * \param timeout: Timeout for image request.
   * \return The number of devices in the map.
   */
  int extractDevicesFromNotify (QList<CUpnpSocket::SNDevice> const & nDevices,
                                int timeout = CDataCaller::Timeout);

  /*! Subscribes eventing services. */
  bool subscribe (CDevice& device, int renewalDelay = CEventingManager::RenewalDelay,
                  int requestTimeout = CEventingManager::RequestTimeout);

  /*! Unsubscribes eventing services. */
  void unsubscribe (CDevice& device, int requestTimeout = CEventingManager::RequestTimeout);

  /*! Renews subscribe eventing services. */
  void renewSubscribe (CDevice& device, int requestTimeout = CEventingManager::RequestTimeout);

  /*! Returns the uuid from id address.
   * \param host: The ip address.
   * \return The uuid.
   */
  QString uuid (QString const & host) const;

  /*! Returns the device uuid and the service from event sid.
   * \param sid: The event sid.
   * \return first=device uuid, second=service id.
   */
  QPair<QString, QString> eventSender (QString const & sid) const;

  /*! Returns the tcp server for eventing. */
  CHTTPServer* httpServer () { return m_httpServer; }

  /*! Returns the tcp server for eventing. */
  CHTTPServer const * httpServer () const { return m_httpServer; }

  /*! Removes a device of the map.
   *\param uuid: The device uuid.
   */
  void removeDevice (QString const & uuid);

  /*! Returns the new device list as a reference. */
  QStringList& newDevices () { return m_newDevices; }

  /*! Returns the lost device list as a reference. */
  QStringList& lostDevices () { return m_lostDevices; }

  /*! Returns the new device list as a const reference. */
  QStringList const & newDevices () const { return m_newDevices; }

  /*! Returns the lost device list as a const reference. */
  QStringList const & lostDevices () const { return m_lostDevices; }

  /*! Returns the netwok access manager created to speed up retreive data. */
  QNetworkAccessManager* networkAccessManager () const { return m_naMgr; }

  /*! Set UPnP/AV only. Just AV servers and renderers are handle */
  void setAVOnly () { m_avOnly = true; }

protected :
  /*! Inserts a new device. */
  void insertDevice (CDevice& device);

  /*! Inserts a new device.
   * \param uuid: The uuid of the device.
   * \return An iterator of the new device.
   */
  iterator insertDevice (QString const & uuid);

 /*! Extracts the services components. */
  bool extractServiceComponents (CDevice& device, int timeout);

private :
  CHTTPServer* m_httpServer = nullptr; //!< The http server for eventing.
  QNetworkAccessManager* m_naMgr = nullptr;  //!< The network access manager for dataCaller.
  QStringList m_newDevices; //!< List of new devices.
  QStringList m_lostDevices; //!< List of lost devices.
  QMap<QString, int> m_invalidDevices; //!< Invalid devices. The device is invalid when get services fails twice.
  int m_deviceFails = 2; //!< Max number of fails to consider the device invalid.
  bool m_avOnly = false; //!< The discovery is launched from avDiscovery.
};

} // End namespace

#endif
