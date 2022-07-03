#include "mainwindow.hpp"
#include "helper.hpp"
#include "item.hpp"
#include "ui_mainwindow.h"
#include "../upnp/controlpoint.hpp"

#include <QElapsedTimer>

void CMainWindow::loadServices (QString const & uuid)
{
  QElapsedTimer ti;
  ti.start ();

  m_deviceUUID = uuid;
  ui->m_services->clear ();
  CDevice const & device      = m_cp->device (uuid);
  QString         text        = device.name () + " [" + device.uuid () + ']';
  QTreeWidgetItem* itemDevice = new QTreeWidgetItem (ui->m_services, QStringList (text));
  itemDevice->setExpanded (true);

  QMap<QString, CService> const &  services = device.services ();
  for (TMServices::const_iterator it = services.begin (); it != services.end (); ++it)
  {
    CService const & service = *it;
    QString const &  name    = service.serviceType ();

    QTreeWidgetItem*         itemService   = new QTreeWidgetItem (itemDevice, QStringList (name));
    QTreeWidgetItem*         itemVariables = new QTreeWidgetItem (itemService, QStringList ("Variables"));
    TMStateVariables const & variables     = service.stateVariables (); // Get the service variables.
    for (TMStateVariables::const_iterator it = variables.cbegin (), end = variables.cend (); it != end; ++it)
    {
      QString const &        name       = it.key ();
      QTreeWidgetItem*       itemVarDef = new QTreeWidgetItem (itemVariables, QStringList (name));
      CStateVariable const & var        = it.value ();

      QString text ("Evented: ");
      text += var.isEvented () ? "yes" : "no";
      new QTreeWidgetItem (itemVarDef, QStringList (text));

      CStateVariable::EType type = var.type (); // Variable type.
      text                       = "Type: ";
      switch (type)
      {
        case CStateVariable::I4 :
          text += "i4";
          break;

        case CStateVariable::Ui4 :
          text += "ui4";
          break;

        case CStateVariable::Real :
          text += "real";
          break;

        case CStateVariable::String :
          text += "string";
          break;

        case CStateVariable::Boolean :
          text += "boolean";
          break;

        default :
          text += "unknown";
      }

      new QTreeWidgetItem (itemVarDef, QStringList (text));
      QStringList allowed = var.allowedValues (); // Allowed value.
      if (!allowed.isEmpty ())
      {
        QTreeWidgetItem* allowedItem = new QTreeWidgetItem (itemVarDef, QStringList ("Allowed"));
        for (QString const & mode : allowed)
        {
          new QTreeWidgetItem (allowedItem, QStringList (mode));
        }

        if (name == "CurrentPlayMode")
        {
          m_current.m_playModes = allowed;
        }
      }

      // Range and step.
      double min  = var.minimum ();
      double max  = var.maximum ();
      double step = var.step ();
      if (min < max && step > 0)
      {
        if (name == "Volume")
        {
          m_current.m_volMin  = min;
          m_current.m_volMax  = max;
          m_current.m_volStep = step;
        }

        QTreeWidgetItem* rangeItem = new QTreeWidgetItem (itemVarDef, QStringList ("Range:"));
        text                       = "Minimum: " + QString::number (min);
        new QTreeWidgetItem (rangeItem, QStringList (text));
        text = "Maximum: " + QString::number (max);
        new QTreeWidgetItem (rangeItem, QStringList (text));
        text = "Step: " + QString::number (step);
        new QTreeWidgetItem (rangeItem, QStringList (text));
      }

      text = var.value ().toString ();
      if (!text.isEmpty ())
      {
        text.prepend ("Value:");
        QTreeWidgetItem* itemValue = new QTreeWidgetItem (itemVarDef, QStringList (text));
        QList<TConstraint> const & csts = var.constraints ();
        if (!csts.isEmpty ())
        {
          for (TConstraint const & cst : csts)
          {
            text = cst.first + ':' + cst.second;
            new QTreeWidgetItem (itemValue, QStringList (text));
          }
        }
      }
    }

    itemVariables->sortChildren (0, Qt::AscendingOrder);

    QTreeWidgetItem*  itemActions = new QTreeWidgetItem (itemService, QStringList ("Actions"));
    TMActions const & actions     = service.actions (); // Get the actions.
    for (TMActions::const_iterator it = actions.cbegin (), end = actions.cend (); it != end; ++it)
    {
      QString const &  name  = it.key (); // Action name.
      QTreeWidgetItem* itemAction;
      if (name == "Browse")
      {
        itemAction = new CItem (itemActions, CDidlItem (), BrowseType);
        itemAction->setText (0, name);
      }
       else if (name == "Search")
      {
        itemAction = new CItem (itemActions, CDidlItem (), SearchType);
        itemAction->setText (0, name);
      }
      else
      {
         itemAction = new QTreeWidgetItem (itemActions, QStringList (name), ActionType);
      }

      sendGetAction (itemAction);
      if (!isHandled (name))
      {
        setItemColor (itemAction, QColor (128, 128, 128));
        itemAction->setFlags (Qt::NoItemFlags);
      }
    }

    itemActions->sortChildren (0, Qt::AscendingOrder);
  }

  itemDevice->sortChildren (0, Qt::AscendingOrder);
  ui->m_services->resizeColumnToContents (0);
  addTotalTime (itemDevice, ti.elapsed ());
}

bool CMainWindow::subscription (QString const & uuid)
{
  bool success = false;
  if (!m_deviceUUID.isEmpty ())
  {
    CDevice const & currentDevice = m_cp->device (m_deviceUUID);
    if (currentDevice.hasSubscribed ())
    {
      m_cp->unsubscribe (m_deviceUUID);
      QTreeWidgetItem* item  = ui->m_devices->topLevelItem (0);
      QBrush           brush = item->foreground (0);
      QList<QTreeWidgetItem*> items = ui->m_services->findItems (uuid, Qt::MatchContains | Qt::MatchRecursive);
      if (!items.isEmpty ())
      {
        items.first ()->setForeground (0, brush);
      }
    }
  }

  if (m_cp->subscribe (uuid))
  {
    success                     = true;
    CDevice const &    device   = m_cp->device (uuid);
    TMServices const & services = device.services ();
    for (TMServices::const_iterator it = services.begin (); it != services.end (); ++it)
    {
      CService const & service = it.value ();
      QString const &  id      = service.subscribeSID ();
      if (!id.isEmpty ())
      { // Subscription is OK
        QList<QTreeWidgetItem*> items = ui->m_services->findItems (service.serviceType (), Qt::MatchContains | Qt::MatchRecursive);
        if (!items.isEmpty ())
        {
          QString text ("Subscribed [");
          text                              += id + "] (";
          TMStateVariables const & variables = service.stateVariables ();
          for (TMStateVariables::const_iterator begin = variables.cbegin (), end = variables.cend (), it = begin; it != end; ++it)
          {
            CStateVariable const & var = it.value ();
            if (var.isEvented ())
            {
              if (*(text.cend () - 1) != '(')
              {
                text += ", ";
              }

              QString const & name = it.key ();
              text                += name;
            }
          }

          text += ')';
          QTreeWidgetItem* item = new QTreeWidgetItem (QStringList (text));
          items.first ()->insertChild (0, item);
        }
      }
    }
  }

  return success;
}
