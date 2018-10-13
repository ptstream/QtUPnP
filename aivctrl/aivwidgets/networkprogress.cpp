#include "networkprogress.hpp"
#include <QTimerEvent>
#include <QResizeEvent>
#include <QPainter>
#include <QDateTime>
#include <QDebug>

CNetworkProgress::CNetworkProgress (char const * name, QWidget* parent) : QWidget (parent)
{
  setObjectName (name);
  m_timer.setInterval (m_timerInteval);
  m_fresholdTimer.setInterval (250);
  m_fresholdTimer.setSingleShot (true);
  connect (&m_timer, &QTimer::timeout, this, &CNetworkProgress::timeout);
  m_lastPosition = width () / 2;
  connect (&m_fresholdTimer, &QTimer::timeout, this, &CNetworkProgress::fresholdTimerTimeout);
}

void CNetworkProgress::CNetworkProgress::start ()
{
  if (!static_cast<QWidget*>(parent ())->isHidden () && !m_fresholdTimer.isActive ())
  {
    m_position = m_lastPosition;
    m_timer.start ();
    update ();
    m_fresholdTimer.start ();
  }
}

void CNetworkProgress::CNetworkProgress::stop ()
{
  if (!static_cast<QWidget*>(parent ())->isHidden () && !m_fresholdTimer.isActive ())
  {
    m_lastPosition = m_position;
    m_position     = 0;
    m_timer.stop ();
    update ();
  }
}

void CNetworkProgress::fresholdTimerTimeout ()
{
  if (!static_cast<QWidget*>(parent ())->isHidden () && m_timer.isActive ())
  {
    stop ();
  }
}

void CNetworkProgress::paintEvent (QPaintEvent*)
{
  QPainter painter (this);

  painter.setRenderHint (QPainter::Antialiasing);

  QPen pen = painter.pen ();
  pen.setColor (QColor (60, 60, 255));
  painter.setPen (pen);

  QPainterPath path;

  int y = height () / 2;
  path.moveTo (0, y);
  if (m_position != 0)
  {
    int position = m_inverted ? width () - m_position : m_position;
     switch (m_type)
    {
      case Wave :
      {
        int w2 = m_waveWidth / 2;
        int w4 = m_waveWidth / 4;
        path.lineTo (position - w2, y);
        path.lineTo (position - w4, y - m_waveWidth);
        path.lineTo (position + w4, y + m_waveWidth);
        path.lineTo (position + w2, y);
        break;
      }

      case Binary :
      {
        QString value = QString::number (qrand () % m_binaryValue, 2);
        QFont   font  = this->font ();
        QFontMetrics fm (font);
        int          xOffset = fm.width (value) / 2;
        int          yOffset = fm.height () / 4;
        path.lineTo (position - xOffset, y);
        path.addText (position - xOffset, y + yOffset, font, value);
        path.moveTo (position + xOffset, y);
        break;
      }

      default :
      {
        int w2 = m_waveWidth / 2;
        path.lineTo (position - w2, y);
        path.moveTo (position + w2, y);
        break;
      }
    }
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


