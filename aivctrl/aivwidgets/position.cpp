#include "position.hpp"
#include "../upnp/avtransport.hpp"
#include "../upnp/helper.hpp"

USING_UPNP_NAMESPACE

CPosition::CPosition (QWidget* parent) : CSlider (parent)
{
}

void CPosition::updatePosition (CControlPoint* cp, QString const & renderer)
{
  CAVTransport avt (cp);
  m_positionInfo = avt.getPositionInfo (renderer);
  if (!m_positionInfo.trackURI ().isEmpty ())
  {
    QString const & absTime   = m_positionInfo.trackDuration (); // Try track duration member
    int             absTimeMS = static_cast<int>(::timeToS (absTime)); // Convert to milliseconds.
    if (absTimeMS <= 0 || absTimeMS > m_maxAbsTime)
    { // Bad abs time. Some renderer defines absTime to string and return 2147483647. Try duration from res elems.
      CDidlItem       didlItem = m_positionInfo.didlItem ();
      QString const & duration = didlItem.duration (-1); // Get first elem with property duration.
      absTimeMS                = static_cast<int>(::timeToS (duration));
      if (absTimeMS <= 0 || absTimeMS > m_maxAbsTime)
      {
        setEnabled (false);
        return;
      }
    }

    setEnabled (true);
    blockSignals (true);
    setMaximum (absTimeMS);
#ifdef Q_OS_MACOS
    setTickInterval (absTimeMS / 10);
#endif
    QString const & relTime   = m_positionInfo.relTime ();
    int             relTimeMS = static_cast<int>(::timeToS (relTime));
    if (relTimeMS < 0)
    { // Bad relTime.
      relTimeMS = 0;
    }

    if (relTimeMS > absTimeMS)
    { // Bad real time.
      relTimeMS = absTimeMS;
    }

    setValue (relTimeMS);
    blockSignals (false);
  }
}
