#include "actionParams.hpp"
#include "ui_actionParams.h"

CActionParams::CActionParams (QWidget *parent) : QDialog (parent), m_ui (new Ui::CActionParams)
{
  m_ui->setupUi (this);
}

CActionParams::~CActionParams ()
{
  delete m_ui;
}

void CActionParams::setHidden (QList<QWidget*> const & ws, bool hide)
{
  for (QWidget* w : ws)
  {
    w->setHidden (hide);
  }
}

void CActionParams::on_m_copy_clicked ()
{
  QString const & text = m_ui->m_current->text ();
  m_ui->m_new->setText (text);
}

void CActionParams::on_m_allowed_currentTextChanged (QString const & text)
{
  m_ui->m_new->setText (text);
}

bool CActionParams::sendMute (bool& current)
{
  CActionParams s;
  s.m_ui->m_name->setText ("SetMute");
  setHidden ( {s.m_ui->m_min, s.m_ui->m_max, s.m_ui->m_lmin, s.m_ui->m_lmax});
  s.m_ui->m_current->setText (current ? "true" : "false");
  s.m_ui->m_allowed->addItems ({QString("false"), QString ("true")});
  s.m_ui->m_allowed->setCurrentIndex (!current);
  int ok = s.exec () != 0;
  if (ok)
  {
    current = s.m_ui->m_new->text () == "true";
  }

  return ok;
}

bool CActionParams::sendVolume (int& current, int min, int max, int step)
{
  CActionParams s;
  s.m_ui->m_name->setText ("SetVolume");
  s.m_ui->m_min->setText (QString::number (min));
  s.m_ui->m_max->setText (QString::number (max));
  s.m_ui->m_current->setText (QString::number (current));
  if (min < max && step > 0)
  {
    for (int k = min; k <= max; k += step)
    {
      s.m_ui->m_allowed->addItem (QString::number (k));
    }
  }

  s.m_ui->m_allowed->setCurrentText (QString::number (current));

  int ok = s.exec () != 0;
  if (ok)
  {
    current = s.m_ui->m_new->text ().toInt ();
  }

  return ok;
}

bool CActionParams::sendVolumeDB (int& current, int min, int max)
{
  CActionParams s;
  s.m_ui->m_name->setText ("SetVolumeDB");
  s.m_ui->m_min->setText (QString::number (min));
  s.m_ui->m_max->setText (QString::number (max));
  s.m_ui->m_current->setText (QString::number (current));
  s.m_ui->m_allowed->setHidden (true);

  bool ok = s.exec () != 0;
  if (ok)
  {
    current = s.m_ui->m_new->text ().toInt ();
  }

  return ok;
}

bool CActionParams::sendAVTransportURI (QString& current)
{
  CActionParams s;
  s.m_ui->m_name->setText ("SetAVTransportURI");
  setHidden ({s.m_ui->m_min, s.m_ui->m_max, s.m_ui->m_lmin, s.m_ui->m_lmax,
              s.m_ui->m_lallowed, s.m_ui->m_allowed});
  s.m_ui->m_current->setText (current);
  int ok = s.exec () != 0;
  if (ok)
  {
    current = s.m_ui->m_new->text ();
  }

  return ok;
}

bool CActionParams::sendNextAVTransportURI (QString& current)
{
  CActionParams s;
  s.m_ui->m_name->setText ("SetNextAVTransportURI");
  s.m_ui->m_current->setText (current);
  setHidden ({s.m_ui->m_min, s.m_ui->m_max, s.m_ui->m_lmin, s.m_ui->m_lmax,
              s.m_ui->m_lallowed, s.m_ui->m_allowed});
  s.m_ui->m_current->setText (current);
  int ok = s.exec () != 0;
  if (ok)
  {
    current = s.m_ui->m_new->text ();
  }

  return ok;
}

bool CActionParams::sendPPS (QString const & transportState, QString const & action)
{
  CActionParams s;
  s.m_ui->m_name->setText (action);
  s.m_ui->m_current->setText (transportState);
  setHidden ({s.m_ui->m_min, s.m_ui->m_max, s.m_ui->m_lmin, s.m_ui->m_lmax,
              s.m_ui->m_lallowed, s.m_ui->m_allowed, s.m_ui->m_copy});

  QString state = "STOPPED";
  if (action == "Pause")
  {
    state = "PAUSE_PLAYBACK";
  }
  else if (action == "Play")
  {
    state = "PLAY";
  }

  s.m_ui->m_new->setText (state);
  s.m_ui->m_new->setEnabled (false);
  return s.exec () != 0;
}

bool CActionParams::sendPN (QString const & action)
{
  CActionParams s;
  s.m_ui->m_name->setText (action);
  setHidden ({s.m_ui->m_min, s.m_ui->m_max, s.m_ui->m_lmin, s.m_ui->m_lmax,
              s.m_ui->m_lallowed, s.m_ui->m_allowed, s.m_ui->m_copy,
              s.m_ui->m_current, s.m_ui->m_lcurrent, s.m_ui->m_new, s.m_ui->m_copy,
              s.m_ui->m_new, s.m_ui->m_lnew});

  return s.exec () != 0;
}

bool CActionParams::sendPlayMode (QString& current, QStringList const & allowed)
{
  CActionParams s;
  s.m_ui->m_name->setText ("SetPlayMode");
  setHidden ({s.m_ui->m_min, s.m_ui->m_max, s.m_ui->m_lmin, s.m_ui->m_lmax});
  s.m_ui->m_current->setText (current);
  s.m_ui->m_allowed->addItems (allowed);
  s.m_ui->m_allowed->setCurrentText (current);
  int ok = s.exec () != 0;
  if (ok)
  {
    current = s.m_ui->m_new->text ();
  }

  return ok;
}

bool CActionParams::sendSeek (QString& current, QString const & trackTime)
{
  CActionParams s;
  s.m_ui->m_name->setText ("Seek");
  s.m_ui->m_min->setText ("0:00:00");
  s.m_ui->m_max->setText (trackTime);
  s.m_ui->m_current->setText (current);
  int ok = s.exec () != 0;
  if (ok)
  {
    current = s.m_ui->m_new->text ();
  }

  return ok;
}

bool CActionParams::sendSearch (QString& criteria, QStringList const & allowed)
{
  CActionParams s;
  s.m_ui->m_name->setText ("Search");
  s.m_ui->m_allowed->addItems (allowed);
  s.m_ui->m_current->setText (criteria);
  s.m_ui->m_new->clear ();
  setHidden ({s.m_ui->m_min, s.m_ui->m_max, s.m_ui->m_lmin, s.m_ui->m_lmax});
  int ok = s.exec () != 0;
  if (ok)
  {
    criteria = s.m_ui->m_new->text ();
  }

  return ok;
}

bool CActionParams::selectPreset (QString& preset, QStringList const & allowed)
{
  CActionParams s;
  s.m_ui->m_name->setText ("SelectPreset");
  s.m_ui->m_allowed->addItems (allowed);
  s.m_ui->m_current->setText (preset);
  s.m_ui->m_new->clear ();
  setHidden ({s.m_ui->m_min, s.m_ui->m_max, s.m_ui->m_lmin, s.m_ui->m_lmax});
  int ok = s.exec () != 0;
  if (ok)
  {
    preset = s.m_ui->m_new->text ();
  }

  return ok;
}
