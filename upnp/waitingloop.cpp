#include "using_upnp_namespace.hpp"
#include "waitingloop.hpp"
#include <QTimerEvent>

USING_UPNP_NAMESPACE

CWaitingLoop::CWaitingLoop (int timeout, ProcessEventsFlags flags, QObject* parent) : QEventLoop (parent), m_flags (flags)
{
  if (timeout > 0)
  {
    m_idTimer = QEventLoop::startTimer (timeout);
  }
}

void CWaitingLoop::timerEvent (QTimerEvent*)
{
  QEventLoop::killTimer (m_idTimer);
  QEventLoop::quit ();
}

void CWaitingLoop::wait (int timeout, ProcessEventsFlags flags)
{
  CWaitingLoop loop (timeout, flags);
  loop.exec (flags);
}
