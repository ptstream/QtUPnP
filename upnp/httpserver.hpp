#ifndef HTTPSERVER_HPP
#define HTTPSERVER_HPP  1

#include "httpparser.hpp"
#include "didlitem.hpp"
#include <QTcpServer>
#include <QNetworkReply>

class QNetworkAccessManager;

START_DEFINE_UPNP_NAMESPACE

/*! Defines the type for evented variable constraints.
 * \param QString: The constraint name.
 * \param QString: The constraint value.
 */
typedef QPair<QString, QString> TEventCst; // Constaint e.g. channel="Master" of <Volume Channel="Master" val="30"/>

/*! Defines the type for evented variable constraint list.
 * \param TEventCst: The constraints.
 */
typedef QList<TEventCst> TEventCsts; // Constaint list.

/*! Defines the type for evented variable (value and constraint list).
 * \param QString: The variable value.
 * \param TEventCsts: The list of constraints.
 */
typedef QPair<QString, TEventCsts> TEventValue; // Variable value & constraint list e.g. val="30" Channel="Master" of <Volume Channel="Master" val="30"/>

/*! Defines the type of evented variable (variable name and value).
 * \param QString: The variable name.
 * \param TEventValue: The value and the constraints.
 */
typedef QMap<QString, TEventValue> TMEventVars; // Variables and all values. e.g <Volume Channel="Master" val="30"/>

/*! Defines type for QNetworkReply::connect function. */
typedef void (QNetworkReply::*TFctNetworkReplyError) (QNetworkReply::NetworkError);

/*! Defines type for QTcpSocket::connect function. */
typedef void (QAbstractSocket::*TFctSocketError) (QAbstractSocket::SocketError);

/*! \brief A partial HTTP server used by UPnP events, playlist manager and streaming server.
 *
 * This class manages only HTTP header verb "NOTIFY" of UPnP events and "HEAD" and "GET".
 *
 * \li UPnP events.
 * For UPnP events, Notify verb is used. The event content describes the values of state variables.
 *
 * Playlists
 * For renderers using playlists, because setAVTransportURI use only a single url, the playlist
 * content is sent to the renderer using http server. The transport url is the playlist name.
 *
 * \li Streaming server
 * The streaming server replace the https server. Once the renderer asks a data (audio, video, image)
 * a https request is sent to the https server. The https reply is converted in a http reply and sent
 * to the renderer.
 */
class UPNP_API CHTTPServer : public QTcpServer
{
  Q_OBJECT

public :
//  enum EStreamingState { UnknowStreamingState, InitStreaming, StartStreaming, StreamingInProgress };

  /*! Defines type for QTcpSocket::byWritten function. */
  typedef QPair<qint64, qint64> TMWritingLength;

  /*! Constructs a server.
   * The server listens for incoming connections on address address and port port.
   */
  CHTTPServer (QHostAddress const & address, quint16 port, QObject* parent);

  /*! Destructor. */
  ~CHTTPServer ();

  /*! Returns true if the server was built correctly. */
  bool isDone () const { return m_done; }

  void setNetworkAccessManager (QNetworkAccessManager* naMgr) { m_naMgr = naMgr; }

  /*! Builds an uri from a playlist name.
   * \param name: The playlist name.
   * \return The uri.
   */
  QString playlistURI (QString const & name) const;

  /*! Returns the listen addresse of the server in form of ip:port (e.g. 192.168.1.25::6569). */
  QString serverListenAddress () const;

  /*! Sets the current playlist name.
   * \param name: The playlist name.
   */
  void setPlaylistName (QString const & name) { m_playlistName = name; }

  /*! Returns the current playlist name. */
  QString const & playlistName () const { return m_playlistName; }

  /*! Clears the current playlist of the http server. */
  void clearPlaylist ();

  /*! Sets the playlist content. */
  void setPlaylistContent (QByteArray const & content);

  /*! Returns the map of variable to be updated. */
  TMEventVars const & vars () const { return m_vars; }

  void setStreamingRequest (QNetworkRequest const & nreq) { m_httpsRequest = nreq; } // Version 1.1
  void abortStreaming (); // Version 1.1
  void clearStreaming (); // Version 1.1

  /*! Returns the an uuid in form uuid:xxxx by uuid_xxx. */
  static QString formatUUID (QString const & uuid);

  /*! Returns the an uuid in form uuid_xxxx by uuid:xxx.
   * It is the reverse function of formatUUID.
   */
  static QString unformatUUID (QString const & uuid);

protected slots :
  /*! The device has emitted an event. */
  void socketReadyRead ();

  /*! The device generates an error. */
  void socketError (QAbstractSocket::SocketError err);

  /*! The socket linking aivctrl and device is diconnected. */
  void socketDisconnected ();

  /*! Write data to the device. */
  void socketBytesWritten (qint64 bytes);

  /*! Sends the http response (UPnP eventing, playlist manager, streaming https).
   * This function decode the message and sends the appropriate response.
   * \param httpParser: The current http parser.
   * \param socket: The socket for the response.
   */
  void sendResponse (CHTTPParser const * httpParser, QTcpSocket* socket);
  void streamBlock ();

  void httpsError (QNetworkReply::NetworkError err); // Version 1.1
  void httpsFinished (); // Version 1.1
  void httpsReadyRead (); // Version 1.1

protected :
  /*! A new connection is available. */
  virtual void incomingConnection (qintptr socketDescriptor);

signals:
  /*! The signal is emmited when the server ends the decoding. */
  void eventReady (QString const & sid);

  /*! Emitted when http parser in socketReadyRead slot has detected a full message. */
  void httpValidReadMessage (CHTTPParser const *, QTcpSocket*);

  /*! Ready to stream a bock of data. */
  void streamingReady (); // Version 1.1

  /*! Emitted when the didlitem contains an url managed by a plugin. */
  void mediaRequest (QString request); // Version 1.1

  void serverComStarted ();
  void serverComEnded ();
  void rendererComStarted ();
  void rendererComEnded ();

private :
  /*! Sends the http response to the renderer.
   * \param socket: The socket to write the bytes.
   * \param bytes: Bytes to send.
   * \return True in case of success.
   */
  bool sendHttpResponse (QTcpSocket* socket, QByteArray const & bytes);

  /*! Sends the http response to the renderer.
   * \param socket: The socket to write the bytes.
   * \param bytes: Bytes to send.
   * \param cBytes: Number of bytes to send.
   * \return True in case of success.
   */
  bool sendHttpResponse (QTcpSocket* socket, char const * bytes, int cBytes);

  /*! Connect to device.
   * param socket: Socket to connect.
   * \return True in case of success.
   */
  bool connectToHost (QTcpSocket* socket);

  /*! Sends a simple HTTP response to the device.
   * \param socket: The socket waiting the response.
   * \param reject: True for reject response, false to accept reponse.
   * \return True in case of success.
   */
  bool httpSimpleResponse (QTcpSocket* socket, bool reject = false);

  /*! Sends an accept HTTP response to the device.
   * \param socket: The socket waiting the response.
   * \return True in case of success.
   */
  bool accept (QTcpSocket* socket);

  /*! Sends an reject HTTP response to the device.
   * \param socket: The socket waiting the response.
   * \return True in case of success.
   */
  bool reject (QTcpSocket* socket);

  /*! Returns the header from the content length and the content type.
   * \param contentLength: The length of the content.
   * \param contentType: The content type. It is th UPnP item format.
   * E.g. for an audio playlist the format is "audio/x-mpegurl".
   * \return The http header.
   */
  QByteArray headerResponse (qint64 contentLength, QByteArray const & contentType = QByteArray ()) const;

  /*! Returns the header from the https reply.
   * \param reply: The https server reply.
   * \param headers: Mandatory header.
   * \return The http header.
   */
  QByteArray streamingHeaderResponse (QNetworkReply const * reply, QList<QPair<QByteArray, QByteArray>> const & headers) const;

  /*! Starts the streaming. A head or get request is sent to the http server.
   * \param request: The request to send.
   * \param method: Head or get method.
   * \param socket: The incomming socket.
   * \return True in case of success.
   */
  bool startStreaming (QNetworkRequest const & request, QString const & method, QTcpSocket* socket); // Version 1.1

  /*! Formats the HTTP date and time for the header.
   * \param dt: The date and time to convert.
   * \returns The HTTP formated time.
   */
  static QByteArray time (QDateTime const & dt);

  /*! Formats the HTTP current date and time for the header.
   * \returns The HTTP formated time.
   */
  static QByteArray currentTime ();

private :
  typedef QPair<QByteArray, int> TMessageData;
  bool m_done = false; //!< The TCP server has well finished its initialization.
  QMap<QTcpSocket*, CHTTPParser> m_eventMessages;
  QMap<QTcpSocket*, qint64> m_writingSocketSizes;
  TMEventVars m_vars; //!< The map of variables modified by the UPnP event.
  mutable QByteArray m_playlistContent; //!< The formatted playlist.
  QString m_playlistName; //!< The current playlist name.
  int m_connectToHostTimeout = 2000; // 2s

  QNetworkAccessManager* m_naMgr = nullptr; //!< The https network access manager.
  QTcpSocket* m_streamingSocket = nullptr; //!< The socket open by the renderer.
  QNetworkReply* m_httpsReply = nullptr; //!< The reply of the https server.
  QNetworkRequest m_httpsRequest; //!< The request to send to the https server.
  int m_httpsBufferSize; //!< The https buffer size to limit the amount of data in memory.
  int m_httpsReadDataTimeout; //!< Wait to have https data to stream a new block. Very smal delay.
  int m_httpsReadDataWaitingRetry; //!< Number of waits. Total waiting time id approximativly m_httpsReadDataTimeout x m_httpsReadDataWaitingRetry.
  bool m_readyReadConnected = false; //!< In case of very small amount of data, the slot readyRead is not emitted just finished.
  QByteArray m_streamingResponseBuffer; //!< Intermediat buffer between https read and straming to renderer.
};

} // End namespace

#endif
