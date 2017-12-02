#include "mainwindow.hpp"
#include "ui_mainwindow.h"
#include "../upnp/renderingcontrol.hpp"
#include "../upnp/avtransport.hpp"

USING_UPNP_NAMESPACE

void CMainWindow::on_m_volume_valueChanged (int volume)
{
  if (!ui->m_volume->signalsBlocked ())
  {
    searchAction (false);
    CRenderingControl (m_cp).setVolume (m_renderer, volume);
  }
}

void CMainWindow::on_m_volume2_valueChanged (int volume)
{
  if (!ui->m_volume->signalsBlocked ())
  {
    searchAction (false);
    CRenderingControl (m_cp).setVolume (m_renderer, volume);
  }
}

void CMainWindow::on_m_position_valueChanged (int position)
{
  if (!ui->m_position->signalsBlocked ())
  {
    searchAction (false);
    QTime   time         = QTime (0, 0).addMSecs (position);
    QString timePosition = time.toString ("hh:mm:ss");
    CAVTransport (m_cp).seek (m_renderer, timePosition);
  }
}

void CMainWindow::on_m_volume_actionTriggered (int action)
{
  searchAction (false);
  static_cast<CSlider*>(sender ())->jumpToMousePosition (action);
}

void CMainWindow::on_m_volume2_actionTriggered (int action)
{
  searchAction (false);
  static_cast<CSlider*>(sender ())->jumpToMousePosition (action);
}

void CMainWindow::on_m_position_actionTriggered (int action)
{
  searchAction (false);
  static_cast<CSlider*>(sender ())->jumpToMousePosition (action);
}
