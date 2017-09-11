#ifndef DATA_CALLER_HPP
#define DATA_CALLER_HPP 1

#include "using_upnp_namespace.hpp"
#include "upnp_global.hpp"
#include <QEventLoop>
#include <QNetworkReply>

class QNetworkAccessManager;
class QUrl;

START_DEFINE_UPNP_NAMESPACE

/*! \brief CDataCaller provides the mechanism to ask data from the device.
 *
 * The data are asked using a QNetworkManager.
 */
class UPNP_API CDataCaller : public QEventLoop
{
  Q_OBJECT

public :
  enum ETime { Timeout = 10000 }; //!< HTTP request timeout in ms (10s).

  /*! Default contructor. */
  CDataCaller (QObject* parent = nullptr);

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
};

} // End namespace

#endif
