#ifndef TEXTMETADATA_HPP
#define TEXTMETADATA_HPP

#include <QTextEdit>

/*! \brief This class provides functionalities to show a paragraphe from a QStringList.
 *
 * One line for QStringList QString. A html text is sent at the QTextEdit.
 */
class CTextMetaData : public QTextEdit
{
  Q_OBJECT
public:
  /*! Default constructor. */
  CTextMetaData (QWidget* parent = nullptr);

  /*! Sets the texts. */
  void setText (QStringList const & texts);

  /*! Truncates texts.The texts are truncated at the widget size. */
  QString truncateText (QString const & text);

  /*! Clears all texts. */
  void clear ();

  /*! Truncates texts.The texts are truncated at the widget size. */
  static QString truncateText (QString const & text, QWidget* widget);

protected :
  /*! Retruncates texts at the new size */
  virtual void resizeEvent (QResizeEvent* event);

  /*! Updates the QTextEdit. */
  void updateText ();

protected :
  QStringList m_texts; //!< The texts. One for each line.
};

#endif // TEXTMETADATA_HPP
