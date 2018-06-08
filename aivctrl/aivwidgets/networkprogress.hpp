#ifndef NETWORKPROGRESS_HPP
#define NETWORKPROGRESS_HPP

#include <QWidget>
#include <QTimer>

/*! \brief This widget draw a pattern to simulate thes network communications.
 *
 * The patterns are:
 * \li Two simple segments.
 * \li A plusation.
 * \li A binary value.
 * When start function is call, the pattern is shown until stop calling or a delay of one second
 * if stop commes to fast.
 */
class CNetworkProgress : public QWidget
{
public:
  /*! The pattern type. */
  enum EType { Default, //!< 2 segments.
               Wave, //!< A plusation.
               Binary //!< A binary value between 0 and 32.
             };

  /*! Default constructor. */
  explicit CNetworkProgress (char const * name, QWidget* parent = nullptr);

  /*! Inverts the sens. Left to right for false, right to left for true. */
  void setInverted (bool invert) { m_inverted = invert; }

  /*! Start the pulsations. */
  void start ();

  /*! Stops the pulsations. */
  void stop ();

  /*! Sets the type of pattern. */
  void setType (EType type) { m_type = type; }

  /*! Returns the pattern type. */
  EType type () const { return m_type; }

  /*! Sets the maximum binary value. */
  void setBinaryValue (int value) { m_binaryValue = value; }

protected slots :
  /*! Ready to draw a new plusation. */
  void timeout ();

  /*! Timeout of threshold timer. */
  void fresholdTimerTimeout ();

protected :
  /*! Draw the widget content. */
  virtual void paintEvent (QPaintEvent*);

private :
  QTimer m_timer; //!< Pulsation timer.
  QTimer m_fresholdTimer; //!< The minimun delay to clear the pattern after stop.
  int m_position = 0; //!< Pulsation position.
  int m_lastPosition = 0; //!< Last pulsation position.
  int m_timerInteval = 150; //!< Interval betwwen two pulsation. */
  int m_waveWidth = 12; //!< Pulsation width en pixels.
  int m_inc = 100000; //!< Increment the pulsation position. See the constant m_invalidInc. (m_inc=m_invalidInc generates a warning with clank).
  bool m_inverted = false; //!< Forward or reverse flag.
  EType m_type = Default; //!< The pattern type.
  int m_binaryValue = 32; //*! The max binray value. */

  int const m_invalidInc = 100000; //!< Just for initialisation.
};

#endif // NETWORKPROGRESS_HPP
