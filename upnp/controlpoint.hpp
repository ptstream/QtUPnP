#ifndef CONTROL_POINT_HPP
#define CONTROL_POINT_HPP 1

#include "devicemap.hpp"
#include "actionmanager.hpp"
#include "httpserver.hpp"
#include "helper.hpp"

START_DEFINE_UPNP_NAMESPACE

class CUnicastSocket;
class CMulticastSocket;

/*! \brief The CControlPoint class implements the base functionalities of an UPnP ControlPoint.
 *
 * It can search and inspect devices, invoke actions of services and subscribes to events.
 * Probably, You will only need one control point for the application.
 * All root devices are searched.
 * The control insists particularly on UNpN/AV servers and renderers.
 * Because the discovery device and the UPnP events are asynchronous, it is not recommended
 * to use certain class directly for a long time.
 * In fact, a reference on a class (e.g. CDevice, CService, CAction, CArgument...) may be valid
 * only during the same event of the main message loop.
 *
 * A good way is to create the control point in the constructor of the QMainWindow.
 * We launch the discovery in the slot timer because discovery can generate freezes in case of
 * slow or busy network. When the timer signal is emitted, all widgets are polished.
 * It is not mandatory.
 *
 * It is not recommanded to delete the control point before calling CControlPoint::close function because
 * Qt creates a lot of threads to manage network communications and QtUPnP creates many event loops.
 * The better way is to call CControlPoint::close in MainWindow::closeEvent. In this case, the main event loop
 * dispatch correctly the differents system events.
 * For more security, if multiple CControlPoint are used, after call CControlPoint::close function, add
 * \code
 *   QCoreApplication::sendPostedEvents ();
 *   QCoreApplication::processEvents ();
 * \endcode
 */
class UPNP_API CControlPoint : public QObject
{
  Q_OBJECT

public :
  /*! Typedef of state value.
   * \param first: The variable name.
   * \param second: The variable value.
   */
  typedef QPair<QString, QString> TArgValue;

  /*! Typedef of subsription timer.
   * \param first: The timer pointer.
   * \param in: The associated timeout in ms.
   */
  typedef QPair<QTimer*, int> TSubscriptionTimer;

  /*! Multicast IP protocols. */
  enum EMulticastIPProtocol { McpIPV4, //!< IPV4 protocol
                              McpIPV6, //!< IPV6 protocol
                              McpIPLast, //!< IP last protocol.
                            };

  /*! Index of action error string. */
  enum EActionErrorType { ErrorDeviceUUID, //!< Device uuid.
                          ErrorServiceTypeOrID, //!< Service type or service id.
                          ErrorActionName, //!< Action name.
                          ErrorArgName, //!< Argument name.
                          ErrorRelatedStateVar, //!< Related state variable name.
                          ErrorActionString, //!< Human-readable description of the action error.
                          ErrorLastType, //!< Use just for set size.
                        };

  /*! Constructor.
   * Allocates and initializes all internal datas. The discovery is not launched.
   */
  CControlPoint (QObject* parent);

  /*! Destructor.
   * Free all internal data and unsubscribe subscripted services.
   * If you pass a non null parent at the constructor, it is not necessary to call the destructor.
   */
  virtual ~CControlPoint ();

  /*! The state of the control point creation.
   * \return True the control point is well constructed. You should not use this address
   * if the control point is not well construct.
   */
  bool isDone () const { return m_done; }

  /*! Checks the existence of UPnP/AV renderers.
   * \return True if control point contains at least one renderer.
   */
  bool hasRenderer () const;

  /*! Checks the existence of UPnP/AV servers.
   * \return True if control point contains at least one server.
   */
  bool hasServer () const;

  /*! Returns the list of UPnP devices uuids of a particular type.
   * \param type: The type of devices.
   * \return The list of device uuids.
   */
  QStringList devices (CDevice::EType type) const;

  /*! Checks the existence of UPnP/AV devices from a particular type.
   * \param type: The type of devices.
   * \return True if control point contains at least one device of determined type.
   */
  bool hasDevice (CDevice::EType type) const;

  /*! Checks the existence of UPnP/AV devices from a particular uuid.
   * \param uuid: Device uuid.
   * \return True if the device exists.
   */
  bool contains (QString const & uuid) const { return m_devices.contains (uuid); }

  /*! Retuns the list of renderers.
   * \return The list of renderers.
   */
  QStringList renderers () const;

  /*! Retuns the list of servers.
   * \return The list of servers.
   */
  QStringList servers () const;

  /*! Launch the discovery for all root devices with special case for UPnP/AV devices.
   * \return true if success, false for error.
   */
  bool avDiscover ();

  /*! Launch the discovery.
   * \param nt: Type of searching. See http://upnp.org/specs/arch/UPnP-arch-DeviceArchitecture-v1.1/
   * \return true if success, false for error.
   */
  bool discover (char const * nt = "upnp:rootdevice");

  /*! Extracts devices from notify message after discovery.
   * \return The number of devices.
   */
  int extractDevicesFromNotify (int timeout = CDataCaller::Timeout);

  /*! Returns a const reference of the device map. */
  CDeviceMap const & devices () { return m_devices; }

  /*! Returns a device class reference.
   * \param uuid: Device uuid.
   * \return The device class reference.
   */
  CDevice& device (QString const & uuid) { return m_devices[uuid]; }

  /*! Returns a device class const reference.
   * \param uuid: Device uuid.
   * \return The device class const reference.
   */
  CDevice device (QString const & uuid) const { return this->m_devices.value (uuid); }

  /*! Invokes an action.
   * \param deviceUUID: The device uuid.
   * \param serviceID: The service identifier.
   * \param actionName: The name of the action.
   * \param args: The action parameters.
   * \param timeout: The time out to wait responds in ms.
   * \return The action information.
   */
  CActionInfo invokeAction (QString const & deviceUUID, QString const & serviceID, QString const & actionName,
                            QList<TArgValue>& args = noArgs,
                            int timeout = CActionManager::Timeout);

  /*! Invokes an action.
   * \param deviceUUID: The device uuid.
   * \param actionName; The name of the action.
   * \param args: The action parameters.
   * \param timeout: The time out to wait responds in ms.
   * \return The action information.
   * \remark This function assumes that all device actions have a different name.
   */
  CActionInfo invokeAction (QString const & deviceUUID, QString const & actionName,
                            QList<TArgValue>& args = noArgs,
                            int timeout = CActionManager::Timeout);

  /*! Returns the http server for UPnP events.
   * \return The server. It is a not fully implemented HTTP server.
   */
  CHTTPServer* httpServer () { return m_devices.httpServer (); }

  /*! Returns the http server to manage events. */
  CHTTPServer const * httpServer () const { return m_devices.httpServer (); }

  /*! Subscribe at the device.
   * \param deviceUUID: The device uuid.
   * \param renewalDelay: The renewal delay.
   * \param requestTimeout: The time out to wait responds in ms.
   */
  bool subscribe (QString const & deviceUUID, int renewalDelay = CEventingManager::RenewalDelay,
                  int requestTimeout = CEventingManager::RequestTimeout);

  /*! Unubscribe at the device.
   * \param deviceUUID: The device uuid.
   * \param requestTimeout: The time out to wait responds in ms.*
   */
  void unsubscribe (QString const & deviceUUID, int requestTimeout = CEventingManager::RequestTimeout);

  /*! Renew subscribe at the device.
   * \param deviceUUID: The device uuid.
   * \param requestTimeout: The time out to wait responds in ms.* *
   */
  void renewSubscribe (QString const & deviceUUID, int requestTimeout = CEventingManager::RequestTimeout);

  /*! Sets the discovery retry number.
   *
   * Discovery used UDP sockets. UDP is an unreliable connectionless protocol. UDP datagrams
   * can be lost. It is necessary to send datagrams more than once.
   * \param count: The number of retry discovery. The default is 5.
   */
  void setDiscoveryRetryCount (int count)  { m_discoveryRetryCount = count; }

  /*! Returns the state variable.
   * \param deviceUUID: The device uuid.
   * \param serviceID: The service identifier.
   * \param name: The variable name.
   * \return The variable.
   */
  CStateVariable stateVariable (QString const & deviceUUID, QString const & serviceID, QString const & name) const;

  /*! Returns the playlist URI.
   * \param name: The name of the playlist.
   * \return The playlist uri.
   */
  QString playlistURI (QString const & name) const;

  /*! Create the playlist content using MU3 format.
   * \param items: The list of DIDL-Lite items.
   * \param format: The format of the playlist.
   * \return The track number of the playlist.
   */
  int setPlaylistContent (QList<CDidlItem::TPlaylistElem> const & items, CDidlItem::EPlaylistFormat format);

  /*! Sets the current playlist name of the http server.
   * \return the name of the playlist.
  */
  void setPlaylistName (QString const & name);

  /*! Returns the current playlist name of the http server. */
  QString playlistName() const;

  /*! Clears the current playlist of the http server. */
  void clearPlaylist ();

  /*! Sets renew subscription gard.
   * \param s: Gard in seconds. Default 2mn.
   */
  void setRenewSubscriptionGard (int s) { m_renewalGard = s; }

  /*! Returns renew subscription gard. Default 2mn */
  int renewSubscriptionGard () const { return m_renewalGard; }

  /*! Returns the list of device uuids that have subscribed. */
  QList<QString> subscribedDevices () const;

  /*! Removes a device.
   * \param uuid: The device uuid
   */
  void removeDevice (QString const & uuid);

  /*! Returns the last action error. */
  QVector<QString> const & lastActionError ();

  /*! Returns the last action error.
   * \return A QString vector with ErrorLastType size.
   * Each index is defined by EActionErrorType enumeration.
   */
  QVector<QString> const & lastActionError () const { return m_lastActionError.m_strings; }

  /*! Returns true if an action error occured. */
  bool errorOccured () const { return m_lastActionError.m_strings[ErrorActionName].isEmpty (); }

  /*! Returns the last action error as a human-readable description. */
  QString const & lastActionErrorString () const { return m_lastActionError.m_strings[ErrorActionString]; }

  /*! Returns the netwok access manager created to speed up retreive data. */
  QNetworkAccessManager* networkAccessManager () const;

  /*! Close the control point. This function must be call before the destructor. */
  void close ();

  /*! Returns true if the control point is closing. */
  bool isClosing () const { return m_closing; }

  /*! Constant to define a empty list of argument. */
  static QList<CControlPoint::TArgValue> noArgs;

protected slots :
  /*! Receipts unicast & multicast datagrams. */
  void readDatagrams ();

  /*! Renewal subscribe timeout is emitted. */
  void renewalTimeout ();

  /*! An UPnP event was emitted.
   * \param sid: The subscripted sid.
   */
  void updateEventVars (QString const & sid);

  /*! A network error was emitted.
   * \param deviceUUID: The device uuid.
   * \param errorCode: The QNetworkReply::NetworkError generated by the QNetworkAccessManager.
   * \param errorDesc: The error description.
   */
  void networkAccessManager (QString const & deviceUUID, QNetworkReply::NetworkError errorCode, QString const & errorDesc);

signals :
  /*! Emitted when the urn type dicovery is launched. */
  void searched (char const *, int, int);

  /*! Emitted when a new device is detected. */
  void newDevice (QString const & uuid);

  /*! Emitted when a device is lost. */
  void lostDevice (QString const & uuid);

  /*! An UPnP event was emitted.
   * \param data: Index 0 the device uuid, index = 1 service id, others variable names affected by the event.
   */
  void eventReady (QStringList const & data);

  /*! An UPnP error was generated.
   * \param errorCode: UPnP error code.
   * \param errorString: UPnP error description.
   * \remark May be the device has generated a network error before.
   */
  void upnpError (int errorCode, QString const & errorString);

  /*! A network error was generated.
   * \param deviceUUID: The uuid of the device.
   * \param errorCode: QNetworkReply::networkError code.
   * \param errorDesc: The error description.
   * \remark May be the device has generated a network error before.
   */
  void networkError (QString const & deviceUUID, QNetworkReply::NetworkError errorCode, QString const & errorDesc);

private :
  /*! Initialize the control point. This function is called by the constructor.
   * \return True in case of well initialization.
   */
  bool initialize ();

  /*! Returns the list of device recently discovered. */
  QList<CUpnpSocket::SNDevice> ndevices () const;

  /*! New devices are detected. */
  void newDevicesDetected();

  /*! Invoke an action of a device and a service.
   *
   * Before send the action, the state variables of the services are updated by the "in" arguments.
   * When the device accepts the action, the state variables of the services are updated by the "out" arguments.
   * When the device doesn't accept the action, the signal upnpError is . emitted.
   * In some cases, with the signal upnpError, the signal networkError is also . emitted.
   * For some devices, the in arguments are mandatory and must be have the exact order.
   * For out arguments, they are not all mandatory but in this case, state variables will not be updated.
   *
   * Errors generated:
   * \li An action error is generated when the actionName is unknown by the service of the device.
   * The invocation stops in this case.
   * \li An argument error is generated when the argument name is unknown by the action.
   * The invocation stops in this case.
   * \li An argument error is generated when the related state variable linked at the argument name is unknown by the service.
   * The invocation continues but the state variable will be not updated.
   *
   * \param device: The device reference.
   * \param service: The service reference.
   * \param actioName: The name of the action.
   * \param args: The arguments of the action.
   * \param timeout: The desired timeout for this action.
   * \return The CActionInfo that contains information about the invocation.
   */
  CActionInfo invokeAction (CDevice& device, CService& service,
                            QString const & actionName, QList<TArgValue>& args,
                            int timeout);
private :
  struct SLastActionError
  {
    SLastActionError () { m_strings.resize (ErrorLastType); }
    void clear () { for (QString& string : m_strings) { string.clear (); } }
    void setString (EActionErrorType type, QString const & string) { m_strings[type] = string; }

    QVector<QString> m_strings;
  };

  /*! Initializes action error. */
  void initActionError (QString const & action, CDevice const & device, CService const & service);

  /*! Initializes argument error. */
  void initArgError (QString const & arg, QString const & action, CDevice const & device, CService const & service);

  /*! The action is unknown. */
  void unknownAction(QString const & action, CDevice const & device, CService const & service);

  /*! The argument name is unknown. */
  void unknownArg (QString const & arg, QString const & action, CDevice const & device, CService const & service);

  /*! The argument related state variable is unknown. */
  void argStateVarRelationship (QString const & arg, QString const & stateVar, QString const & action, CDevice const & device,
                                CService const & service);

  /*! The service is unknown. */
  void unknownService (QString const & uuid, QString const & id);

  /*! The device is unknown. */
  void unknownDevice (QString const & uuid);

private :
  bool m_done = false; //!< The status of the creation.
  bool m_closing = false; //!< The control point  is being closed.
  QHostAddress m_upnpMulticastAddr = QHostAddress ("239.255.255.250"); //!< Standard multicast IPV4 address.
  QHostAddress m_upnpMulticastAddr6 = QHostAddress ("FF02::C"); //!< Standard multicast IPV6 address.
  quint16 m_upnpMulticastPort = 1900; //!< Standard multicats  port.
  CUnicastSocket* m_unicastSockets[6]; //!< Unicast sockets.
  CMulticastSocket* m_multicastSockets[McpIPLast];  //!< Multicast sockets 0 ipv4, 1 ipv6.
  CDeviceMap m_devices; //!< Map of discovered devices.
  QMap<QString, TSubscriptionTimer> m_subcriptionTimers; //!< Map of subscription timers.
  int m_discoveryRetryCount = 3; //!< Number of discovery messages sent.
  int m_discoveryPause = 500; //!< Delay between two discoveries message in ms.
  int m_renewalGard = 120; //!< Gard for renewing in seconds (2 mn).
  SLastActionError m_lastActionError; //!< Last error generate by the last action.
  int m_level = 0; //!< To emit signal only once.

}; // CControlPoint

} // Namespace

#endif //CONTROL_POINT_HPP
