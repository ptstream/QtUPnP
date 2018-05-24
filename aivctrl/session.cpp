#include "session.hpp"
#include "helper.hpp"
#include <QXmlStreamWriter>
#include <QDir>
#include <QApplication>
#include <QDesktopWidget>


CSession::CSession (QString const & renderer, int volume, int playMode,
                    bool remainingTime, QRect const & rect, Qt::WindowStates windowStates,
                    QString const & language, unsigned status) : CXmlH (), m_renderer (renderer),
                    m_volume (volume), m_playMode (playMode),
                    m_remainingTime (remainingTime), m_rect (rect),
                    m_windowStates (windowStates), m_language (language), m_status (status)
{
}

QString CSession::standardFilePath ()
{
  QString filePath;
  QString folder = appDataDirectory ();
  if (!folder.isEmpty ())
  {
    QDir dir (folder);
    filePath = dir.absoluteFilePath (m_fileName);
  }

  return filePath;
}

bool CSession::characters (QString const & name)
{
  QString const & tag = m_stack.top ();
  if (tag == "uuid")
  {
    m_renderer = name;
  }
  else if (tag == "volume")
  {
    m_volume = name.toInt ();
  }
  else if (tag == "playMode")
  {
    m_playMode = name.toInt ();
  }
  else if (tag == "remainingTime")
  {
    m_remainingTime = !name.isEmpty () && name[0] == '1';
  }
  else if (tag == "left")
  {
    m_rect.setLeft (name.toInt ());
  }
  else if (tag == "top")
  {
    m_rect.setTop (name.toInt ());
  }
  else if (tag == "width")
  {
    m_rect.setWidth (name.toInt ());
  }
  else if (tag == "height")
  {
    m_rect.setHeight (name.toInt ());
  }
  else if (tag == "states")
  {
    m_windowStates = static_cast<Qt::WindowStates>(name.toInt ());
  }
  else if (tag == "language")
  {
    m_language = name;
  }
  else if (tag == "status")
  {
    m_status = name.toUInt (nullptr, 16);
  }
  return true;
}

void CSession::save (QString fileName)
{
  if (!m_renderer.isEmpty ())
  {
    if (fileName.isEmpty ())
    {
      fileName = standardFilePath ();
    }

    if (!fileName.isEmpty ())
    {
      QFile file (fileName);
      if (file.open (QIODevice::WriteOnly | QIODevice::Text))
      {
        QXmlStreamWriter stream (&file);
        stream.setAutoFormatting (true);
        stream.writeStartDocument ();

        stream.writeStartElement ("session");
        stream.writeAttribute ("version", "1.0.0");

        stream.writeStartElement ("renderer");

        stream.writeStartElement ("volume");
        stream.writeCharacters (QString::number (m_volume));
        stream.writeEndElement ();

        stream.writeStartElement ("playMode");
        stream.writeCharacters (QString::number (m_playMode));
        stream.writeEndElement ();

        stream.writeStartElement ("remainingTime");
        stream.writeCharacters (QString::number (m_remainingTime));
        stream.writeEndElement ();

        stream.writeStartElement ("uuid");
        stream.writeCharacters (m_renderer);
        stream.writeEndElement ();

        stream.writeEndElement (); // Renderer

        stream.writeStartElement ("ui");

        stream.writeStartElement ("left");
        stream.writeCharacters (QString::number (m_rect.left ()));
        stream.writeEndElement ();

        stream.writeStartElement ("top");
        stream.writeCharacters (QString::number (m_rect.top ()));
        stream.writeEndElement ();

        stream.writeStartElement ("width");
        stream.writeCharacters (QString::number (m_rect.width ()));
        stream.writeEndElement ();

        stream.writeStartElement ("height");
        stream.writeCharacters (QString::number (m_rect.height ()));
        stream.writeEndElement ();

        stream.writeStartElement ("states");
        stream.writeCharacters (QString::number (m_windowStates, 16));
        stream.writeEndElement ();

        stream.writeEndElement (); // ui

        stream.writeStartElement ("language");
        stream.writeCharacters (m_language);
        stream.writeEndElement ();

        stream.writeStartElement ("status");
        stream.writeCharacters (QString::number (m_status, 16));
        stream.writeEndElement ();

        stream.writeEndElement (); // session
        stream.writeEndDocument ();
      }
    }
  }
}

bool CSession::restore (QString fileName)
{
  if (fileName.isEmpty ())
  {
    fileName = standardFilePath ();
  }

  bool success = false;
  if (!fileName.isEmpty ())
  {
    QFile file (fileName);
    if (file.open (QIODevice::ReadOnly | QIODevice::Text))
    {
      QByteArray data = file.readAll ();
      success = parse (data);
    }
  }

  return success;
}

void CSession::setGeometry (QWidget* widget) const
{
  if (m_rect.isValid ())
  {
    QRect wRect = m_rect;
    QRect sRect = QApplication::desktop ()->screenGeometry ();
    int   dx   = 0, dy = 0;
    if (wRect.right () > sRect.right ())
    {
      dx = sRect.right () - wRect.right ();
    }
    else if (wRect.left () < sRect.left ())
    {
      dx = wRect.left () - sRect.left ();
    }
    else if (wRect.bottom () > sRect.bottom ())
    {
      dy = sRect.bottom () - sRect.bottom ();
    }
    else if (wRect.top () < sRect.top ())
    {
      dy = wRect.top () - sRect.top ();
    }

    if (dx != 0 || dy != 0)
    {
      wRect.translate (dx, dy);
    }

    widget->setGeometry (wRect);
    widget->setWindowState (m_windowStates);
  }
}
