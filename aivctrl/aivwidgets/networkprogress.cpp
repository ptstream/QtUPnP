#include "networkprogress.hpp"
#include <QTimerEvent>
#include <QResizeEvent>
#include <QPainter>
#include <QDebug>

CNetworkProgress::CNetworkProgress (char const * name, QWidget* parent) : QWidget (parent)
{
  setObjectName (name);
  m_timer.setInterval (m_timerInteval);
  connect (&m_timer, &QTimer::timeout, this, &CNetworkProgress::timeout);
  m_y2           = m_waveWidth / 2;
  m_y4           = m_waveWidth / 4;
  m_lastPosition = width () / 2;
}

void CNetworkProgress::CNetworkProgress::start ()
{
  if (!m_timer.isActive ())
  {
    m_position = m_lastPosition;
    m_timer.start ();
    update ();
  }
}

void CNetworkProgress::CNetworkProgress::stop ()
{
  if (!m_skipStop)
  {
    m_lastPosition = m_position;
    m_timer.stop ();
    m_position = 0;
    update ();
  }
}

void CNetworkProgress::paintEvent (QPaintEvent*)
{
  QPainter painter (this);

  painter.setRenderHint (QPainter::Antialiasing);

  QPen pen = painter.pen ();
  pen.setWidth (2);
  pen.setColor (QColor (60, 60, 255));
  painter.setPen (pen);

  QPainterPath path;

  int y = height () / 2;
  path.moveTo (0, y);
  if (m_position != 0)
  {
    int position = m_inverted ? width () - m_position : m_position;
    path.lineTo (position - m_y2, y);
    path.lineTo (position - m_y4, y - m_waveWidth);
    path.lineTo (position + m_y4, y + m_waveWidth);
    path.lineTo (position + m_y2, y);
  }

  path.lineTo (width (), y);
  painter.drawPath (path);
}


void CNetworkProgress::timeout ()
{
  int width = this->width ();
  int inc   = m_waveWidth;
  if (m_inc == m_invalidInc)
  {
    m_inc = inc;
  }

  if (m_position > width - inc)
  {
    m_position = width;
    m_inc      = -inc;
  }
  else if (m_position < inc)
  {
    m_position = 0;
    m_inc      = inc;
  }

  m_position += m_inc;
  update ();
}


