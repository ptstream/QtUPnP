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

/*! \brief A partial HTTP server used by UPnP events and playlist manager.
 *
 * This class manages only HTTP header verb "NOTIFY" of UPnP events and "HEAD" and "GET" to send the plalists
 * content.
 */
class UPNP_API CHTTPServer : public QTcpServer
{
  Q_OBJECT

public :
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
  void sendHttpResponse (CHTTPParser const * httpParser, QTcpSocket* socket);

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

  void mediaRequest (QString request); // Version 1.1

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
   * \return True in case of success.
   */
  QByteArray headerResponse (qint64 contentLength, char const * contentType = nullptr) const;

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
  QNetworkAccessManager* m_naMgr = nullptr; //!< The streaming network access manager.
  QTcpSocket* m_streamingSocket = nullptr; // Version 1.1
  QNetworkReply* m_httpsReply = nullptr; // Version 1.1
  QNetworkRequest m_httpsRequest; // Version 1.1
  int m_httpsBufferSize = 2041; // Version 1.1
  bool m_startStreaming = true; // Version 1.1
  QByteArray m_chunkSize; // Version 1.1
};

} // End namespace

#endif
