#include "slider.hpp"
#include <QStyleOptionSlider>
#include <QMouseEvent>
#include <QProxyStyle>
#include <QDebug>

CSlider::CSlider (QWidget* parent) : QSlider (parent)
{
  setTracking (false);
}

void CSlider::mousePressEvent (QMouseEvent* event)
{
  QSlider::mousePressEvent (event);
  m_mousePressed = true;
  // Very important because the slider can enter in an infinit loop.
  setRepeatAction (QAbstractSlider::SliderNoAction);
}

void CSlider::mouseReleaseEvent (QMouseEvent* event)
{
  QSlider::mouseReleaseEvent (event);
  m_mousePressed = false;
  // Very important because the slider can enter in an infinit loop.
  setRepeatAction (QAbstractSlider::SliderNoAction);
}

void CSlider::jumpToMousePosition (int action)
{
  if (!signalsBlocked () && (action == QAbstractSlider::SliderPageStepAdd || action == QAbstractSlider::SliderPageStepSub))
  {
    QStyleOptionSlider opt;
    opt.initFrom (this);
    QRect  sr = style ()->subControlRect (QStyle::CC_Slider, &opt, QStyle::SC_SliderHandle, this);
    float  normalizedPosition;
    QPoint localPos = mapFromGlobal (QCursor::pos ());
    if (orientation () == Qt::Vertical)
    {
      float  halfHandleHeight = (0.5f * sr.height ()) + 0.5f;
      int height             = this->height ();
      int   adaptedPosY      = height - localPos.y ();
      if (adaptedPosY < halfHandleHeight)
      {
        adaptedPosY = halfHandleHeight;
      }

      if (adaptedPosY > height - halfHandleHeight )
      {
        adaptedPosY = height - halfHandleHeight;
      }

      float newHeight    = (height - halfHandleHeight) - halfHandleHeight;
      normalizedPosition = (adaptedPosY - halfHandleHeight)  / newHeight;
    }
    else
    {
      float halfHandleWidth = (0.5f * sr.width ()) + 0.5f;
      int   adaptedPosX     = localPos.x ();
      if (adaptedPosX < halfHandleWidth)
      {
        adaptedPosX = halfHandleWidth;
      }

      int width = this->width ();
      if (adaptedPosX > width - halfHandleWidth)
      {
        adaptedPosX = width - halfHandleWidth;
      }

      float newWidth     = (width - halfHandleWidth) - halfHandleWidth;
      normalizedPosition = (adaptedPosX - halfHandleWidth)  / newWidth;
    }

    int newVal = minimum () + ((maximum () - minimum ()) * normalizedPosition);
    if (invertedAppearance ())
    {
      newVal = maximum () - newVal;
    }

    setValue (newVal);
  }
}
