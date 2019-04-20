#ifndef SESSION_HPP
#define SESSION_HPP

#include "../upnp/xmlh.hpp"
#include <QRect>
#include <QString>

class CSession  : public QtUPnP::CXmlH
{
public :
  CSession () {}
  CSession (QString const & renderer, int volume, int playMode,
            bool remainingTime, QRect const & rect, Qt::WindowStates windowStates,
            int iconSize, QString const & language, unsigned status);

  virtual bool characters (QString const & name);

  void save (QString fileName =  QString ());
  bool restore (QString fileName =  QString ());

  QString standardFilePath ();

  QString const & renderer () const { return m_renderer; }
  int volume () const { return m_volume; }
  int playMode () const { return m_playMode; }
  bool remainingTime () const { return m_remainingTime; }
  QRect const & geometry () const { return m_rect; }
  Qt::WindowStates windowStates () const { return m_windowStates; }
  int iconSize () const { return m_iconSize; }
  QString const & language () const { return m_language; }
  unsigned status () const { return m_status; }

  void setGeometry (QWidget* widget) const;

protected :
  QString m_renderer;
  int m_volume = 0;
  int m_playMode = 0;
  bool m_remainingTime = false;
  QRect m_rect;
  Qt::WindowStates m_windowStates = Qt::WindowNoState;
  int m_iconSize = 32;
  QString m_language;
  unsigned m_status = 0;
  QString const m_fileName = "session.xml";
};

#endif // SESSION_HPP
