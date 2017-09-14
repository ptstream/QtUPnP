#ifndef WAITING_LOOP_HPP
#define WAITING_LOOP_HPP

#include "using_upnp_namespace.hpp"
#include "upnp_global.hpp"
#include <QEventLoop>

START_DEFINE_UPNP_NAMESPACE

/*! \brief Defines a waiting loop to stop the current thread without
 * freeze the UI during certain operations.
 */
class UPNP_API CWaitingLoop : public QEventLoop
{
  Q_OBJECT

public:
  /*! Contructs CWaitingLoop with a delay of timeout. */
  CWaitingLoop (int timeout, ProcessEventsFlags flags, QObject* parent = nullptr);

  /*! Starts the loop with a delay of timeout. */
  void start (int timeout) { m_idTimer = QEventLoop::startTimer (timeout); }

  /*! Creates and starts a waiting loop. */
  static void wait (int timeout, ProcessEventsFlags flags);

protected:
  /*! Event generate by the timer. */
  virtual void timerEvent (QTimerEvent*);

private :
  ProcessEventsFlags m_flags = QEventLoop::AllEvents;
  int m_idTimer = 0; //!< Qt timer identifier. */
};

} // End namespace

#endif // WAITING_LOOP_HPP
