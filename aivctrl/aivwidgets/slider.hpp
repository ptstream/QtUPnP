#ifndef SLIDER_HPP
#define SLIDER_HPP

#include <QSlider>

/*! \brief This class changes the page up and page down QSlider mechanism.
 *
 * When page up or page down is clicked, the value goes to the position of the cursor. By default
 * the value is incremented or decremented by pageStep.
 */
class CSlider : public QSlider
{
public:
  /*! Default constructor. */
  CSlider (QWidget* parent = nullptr);

  /*! Sets the QSlider value at the cursor position. */
  void jumpToMousePosition (int action);

  /*! Returns the flag m_mousePressed. */
  bool mousePressed () const { return m_mousePressed; }

protected :
  /*! Sets the flag m_mousePressed at true. */
  virtual void mousePressEvent (QMouseEvent* event);

  /*! Sets the flag m_mousePressed at false. */
  virtual void mouseReleaseEvent (QMouseEvent* event);

protected :
  bool m_mousePressed = false; //!< Flag m_mousePressed.
};

#endif // SLIDER_HPP
