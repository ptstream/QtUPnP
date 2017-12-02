#include "cover.hpp"
#include "widgethelper.hpp"
#include "../upnp/datacaller.hpp"
#include <QPainter>

USING_UPNP_NAMESPACE

CCover::CCover (QWidget* parent) : QLabel (parent)
{
}

void CCover::setImage (QString const & uri)
{
  bool defName = true;
  if (!uri.isEmpty ())
  {
    QByteArray pxmBytes = CDataCaller ().callData (uri, m_imageTimeout);
    if (!pxmBytes.isEmpty ())
    {
      m_pixmap.loadFromData (pxmBytes);
      if (!m_pixmap.isNull ())
      {
        defName = false;
      }
    }
  }

  if (defName)
  {
    m_pixmap = QPixmap (::resIconFullPath (m_defaultPixmap));
  }

  update ();
}

void CCover::paintEvent (QPaintEvent* event)
{
  if (!m_pixmap.isNull ())
  {
    float wl = width (),          hl = height ();
    float wp = m_pixmap.width (), hp = m_pixmap.height ();
    float ws = wl / wp;
    float hs = hl / hp;
    QPixmap pxm;
    if (ws < hs)
    {
      pxm = m_pixmap.scaledToWidth (wl, Qt::SmoothTransformation);
    }
    else
    {
      pxm = m_pixmap.scaledToHeight (hl, Qt::SmoothTransformation);
    }

    QPainter paint (this);
    int      x = (wl - pxm.width ())  / 2;
    int      y = (hl - pxm.height ()) / 2;
    if (!isEnabled ())
    {
      paint.setOpacity (0.2);
    }

    paint.drawPixmap (x, y, pxm);
  }
  else
  {
    QLabel::paintEvent (event);
  }
}
