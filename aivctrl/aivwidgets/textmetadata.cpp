#include "textmetadata.hpp"

CTextMetaData::CTextMetaData (QWidget* parent) : QTextEdit (parent)
{
  QFontMetrics fontMetrics = this->fontMetrics ();
  int          spacing     = fontMetrics.lineSpacing ();
  setMaximumHeight (4 * spacing);
  setSizePolicy (QSizePolicy::Expanding, QSizePolicy::Fixed);
}

QString CTextMetaData::truncateText (QString const & text)
{
  return truncateText (text, this);
}

QString CTextMetaData::truncateText (QString const & text, QWidget* widget)
{
  char const * suspensionPoint = "...";

  QString truncatedText = text;
  if (!truncatedText.isEmpty () && (truncatedText.size () > 1 || truncatedText[0] != ' '))
  {
    QFontMetrics fontMetrics = widget->fontMetrics ();
    int          textWidth   = fontMetrics.width (truncatedText, -1);
    int          width       = 17 * widget->width () / 20; // 15%
    if (textWidth >= width)
    {
      int length     = truncatedText.length ();
      truncatedText += suspensionPoint;
      while (length > 0 && fontMetrics.width (truncatedText, -1) >= width)
      {
        truncatedText = truncatedText.left (--length) + suspensionPoint;
      }
    }
  }

  return truncatedText;
}

void CTextMetaData::resizeEvent (QResizeEvent* event)
{
  QTextEdit::resizeEvent (event);
  updateText ();
}

void CTextMetaData::updateText ()
{
  QString const lf   = "<br/>";
  QString       html = "<p style=\"text-align:center\">";
  if (m_texts.isEmpty ())
  {
    m_texts << " ";
  }
  else if (m_texts.first ().isEmpty ())
  {
    m_texts.first () = ' ';
  }

  for (QStringList::const_iterator begin = m_texts.cbegin (), it = begin, end = m_texts.cend (); it != end; ++it)
  {
    if (it != begin)
    {
      html += lf;
    }

    html += truncateText (*it);
  }

  html += "</p>";
  QTextEdit::setHtml (html);
}

void CTextMetaData::setText (QStringList const & texts)
{
  m_texts = texts;
  updateText ();
}

void CTextMetaData::clear ()
{
  m_texts.clear ();
  QTextEdit::clear ();
}
