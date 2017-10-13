#ifndef HTTPSERVER_HPP
#define HTTPSERVER_HPP  1

#include "httpparser.hpp"
#include "didlitem.hpp"
#include <QTcpServer>

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

/*! \brief A partial HTTP server used by UPnP events and playlist manager.
 *
 * This class manages only HTTP header verb "NOTIFY" of UPnP events and "HEAD" and "GET" to send the plalists
 * content.
 */
class UPNP_API CHTTPServer : public QTcpServer
{
  Q_OBJECT

public :
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

  /*! Sets the current playlist name.
   * \param name: The playlist name.
   */
  void setPlaylistName (QString const & name) { m_playlistName = name; }

  /*! Returns the current playlist name. */
  QString const & playlistName () const { return m_playlistName; }

  /*! Clears the current playlist of the http server. */
  void clearPlaylist () { m_playlistName.clear (); }

  /*! Builds an audio file uri from a name.
   * Not use actually.
   */
  QString audioFileURI (QString const & name) const;

  /*! Sets the playlist content. */
  void setPlaylistContent (QByteArray const & content);

  /*! Returns the map of variable to be updated. */
  TMEventVars const & vars () const { return m_vars; }

 /*! Returns the playlist base name.
  *
  * The playlist base name is the name where ':' is replaced by '_'.
  *
  * A good way is the name the playlist with the renderer uuid. Normally the uuid
  * start bt "uuid:". The a file name this function simply replace ':' by '_'.
  * \param name: The playlist name.
  * \return The base name.
  */
  static QString playlistBaseName (QString const & name);

protected slots :
  /*! The device has emited an event. */
  void readyRead ();

  /*! This slot is called every time a payload of data has been written to the device.
   * The bytes argument is set to the number of bytes that were written in this payload.
   */
  void bytesWritten (qint64 bytes);

protected :
  /*! A new connection is available. */
  virtual void incomingConnection (qintptr socketDescriptor);

signals:
  /*! The signal is emmited when the server ends the decoding. */
  void eventReady (QString const & sid);

private :

  void parseMessage (CHTTPParser const & httpParser, int headerLength, int contentLength,
                     QByteArray const & eventMessage, QTcpSocket* socket);

  /*! Connect to device.
   * param socket: Socket to connect.
   * \param host: IP address of the device.
   * \param port: Connection port.
   * \return True in case of success.
   */
  bool connectToHost (QTcpSocket* socket, QHostAddress host, quint16 port);

  /*! Sends a simple HTTP response to the device.
   * \param host: IP address of the device.
   * \param port: Connection port.
   * \param reject: True for reject response, false to accept reponse.
   * \return True in case of success.
   */
  bool httpSimpleResponse (QTcpSocket* socket, QHostAddress address, quint16 port, bool reject = false);

  /*! Sends an accept HTTP response to the device.
   * \param host: IP address of the device.
   * \param port: Connection port.
   * \return True in case of success.
   */
  bool accept (QTcpSocket* socket, QHostAddress address, quint16 port);

  /*! Sends an reject HTTP response to the device.
   * \param host: IP address of the device.
   * \param port: Connection port.
   * \return True in case of success.
   */
  bool reject (QTcpSocket* socket, QHostAddress address, quint16 port);

  /*! Sends the playlist content to the renderer.
   * \param host: IP address of the device.
   * \param port: Connection port.
   * \param response: Response to send.
   * \return True in case of success.
   */
  bool httpPlaylistResponse (QTcpSocket* socket, QHostAddress address, quint16 port, QByteArray const & response);

  /*! Returns the header from the content length and the content type.
   * \param contentLength: The length of the content.
   * \param contentType: The content type. It is th UPnP audio item format.
   * E.g. for an audio playlist the formatr is "audio/x-mpegurl".
   * \return True in case of success.
   */
  QByteArray headerResponse (int contentLength, char const * contentType = nullptr) const;

  /*! Returns the response for the HTTP "HEAD" request.
   * \param internalPlaylist: Must be true.
   * \return The response to send.
   */
  QByteArray headMethodResponse (bool internalPlaylist = true) const;

  /*! Returns the response for the HTTP "GET" request.
   * \param internalPlaylist: Must be true.
   * \return The response to send.
   */
  QByteArray getMethodResponse (bool internalPlaylist = true) const;

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
  bool m_done = false; //<! The TCP server has well finished its initialization.
  QMap<QTcpSocket*, TMessageData> m_eventMessage;
  TMEventVars m_vars; //!< The map of variables modified by the UPnP event.
  mutable QByteArray m_playlistContent; //!< The formatted playlist.
  QString m_playlistName; //!< The current playlist name.
  mutable int m_playlistIndex = 1; //!< Playlist index incremented by one at each use of setPlaylistName.
};

} // End namespace

#endif
