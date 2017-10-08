#ifndef DATA_CALLER_HPP
#define DATA_CALLER_HPP 1

#include "using_upnp_namespace.hpp"
#include "upnp_global.hpp"
#include <QEventLoop>
#include <QNetworkReply>

class QNetworkAccessManager;

START_DEFINE_UPNP_NAMESPACE

/*! \brief Provides the mechanism to ask data from the device.
 *
 * The data are asked using a QNetworkManager.
 */
class UPNP_API CDataCaller : public QEventLoop
{
  Q_OBJECT

public :
  enum ETime { Timeout = 10000 }; //!< HTTP request timeout in ms (10s).

  /*! Default contructor.
   *
   * The QNetworkAccessManager is created internaly.
   */
  CDataCaller (QObject* parent = nullptr);

  /*! Constructor with network access manager.
   *
   * The internal QNetworkAccessManager is not created. The object use naMgr.
   * It is sometimes useful because create and destroy the network access manager
   * consumes time. Use this contructor when you want call callData function many times.
   */
  CDataCaller (QNetworkAccessManager* naMgr, QObject* parent = nullptr);

  /*! Destructor. */
  ~CDataCaller ();

  /*! Sends a request and return the responds.
   * \param url: The request.
   * \param timeout: The time out to wait responds in ms.
   * \return The array of bytes of the responds.
   */
  QByteArray callData (QUrl const & url, int timeout = Timeout);

  /*! Read the request.
   * \remark Normally you do not have to call this function.
   * \return The array of bytes of the responds.
   */
  QByteArray readAll ();

protected:
  /*! This function is called in case of timeout. */
  virtual void timerEvent (QTimerEvent*);

private slots:
  /*! Slot call when the responds if ok. */
  void finished ();

  /*! Slot for network error. */
  void error (QNetworkReply::NetworkError err);

private :
  QString m_request; //!< The request.
  QNetworkAccessManager* m_naMgr = nullptr; //!< The current netword access manager. see CDataCaller (QNetworkAccessManager* naMgr, QObject* parent).
};

} // End namespace

#endif
