#ifndef NETWORKPROGRESS_HPP
#define NETWORKPROGRESS_HPP

#include <QWidget>
#include <QTimer>

/*! \brief This widget draw a pulsation when it is activate.
 * When start function is call, a pulsation is add until stop calling.
 * To show grouping pulsations, it is possible to ignore stops.
 */
class CNetworkProgress : public QWidget
{
public:
  /*! Default constructor. */
  explicit CNetworkProgress (char const * name, QWidget* parent = nullptr);

  /*! Inverts the sens. Left to right for false, right to left for true. */
  void setInverted (bool invert) { m_inverted = invert; }

  /*! Ignores stop. */
  void setSkipStop (bool skip) { m_skipStop = skip; }

  /*! Returns true if stops are igniored. */
  bool hasSkipStop () const { return m_skipStop; }

  /*! Start the pulsations. */
  void start ();

  /*! Stops the pulsations. */
  void stop ();

protected slots :
  /*! Ready to draw a new plusation. */
  void timeout ();

protected :
  /*! Draw the widget content. */
  virtual void paintEvent (QPaintEvent*);

private :
  QTimer m_timer; //!< Pulsation timer.
  int m_position = 0; //!< Pulsation position.
  int m_lastPosition = 0; //!< Last pulsation position.
  int m_timerInteval = 150; //!< Interval betwwen two pulsation. */
  int m_waveWidth = 12; //!< Pulsation width en pixels.
  int m_y2; //!< m_waveWidth / 2.
  int m_y4; //!< m_waveWidth / 4.
  int m_inc = m_invalidInc; //!< Increment the pusation position.
  bool m_inverted = false; //!< Forward or reverse flag.
  bool m_skipStop = false; //!< Ignore stop fmag.

  int const m_invalidInc = 100000; //!< Just for initialisation.
};

#endif // NETWORKPROGRESS_HPP
