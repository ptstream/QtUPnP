#ifndef POSITION_HPP
#define POSITION_HPP

#include "../upnp/positioninfo.hpp"
#include "slider.hpp"

namespace QtUPnP
{
  class CControlPoint;
}

/*! This class manages a CSlider derived of QSlider with the content of QtUPnP::CPositionInfo.
 * The QtUPnP::CPositionInfo::relTime () and QtUPnP::CPositionInfo::absTime () function are
 * used to set the value and the maximum of the slider.
 */

class CPosition : public CSlider
{
  Q_OBJECT
public:
  /*! Default construvtor. */
  CPosition (QWidget* parent = nullptr);

  /*! Updates the position for a renderer. This function invokes GetPosition action. */
  void updatePosition (QtUPnP::CControlPoint* cp, QString const & renderer);

  /*! Returns the current QtUPnP::CPositionInfo. */
  QtUPnP::CPositionInfo const & positionInfo () const { return m_positionInfo; }

  /*! Returns the maximum possible absTime. */
  int maxAbsTime () const { return m_maxAbsTime; }

private :
  QtUPnP::CPositionInfo m_positionInfo; //!< The current QtUPnP::CPositionInfo.
  int const m_maxAbsTime = 356400000; //!< The maximum posible absTime (99h in ms). Used to detect error.
};

#endif // POSITION_HPP
