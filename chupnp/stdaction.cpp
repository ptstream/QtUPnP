#include "stdaction.hpp"
#include "ui_stdaction.h"
#include "../upnp/actioninfo.hpp"
#include <QTextEdit>
#include <QLabel>

CStdAction::CStdAction (CControlPoint* cp, QString const & deviceUUID, QString const & action, QWidget* parent) :
            QDialog (parent), ui (new Ui::CStdAction), m_cp (cp), m_deviceUUID (deviceUUID), m_actionName (action)
{
  ui->setupUi (this);
  m_outLayout = new QGridLayout (ui->m_out);
  m_outLayout->setSizeConstraint (QLayout::SetMaximumSize);
  m_inLayout  = new QGridLayout (ui->m_in);
  m_inLayout->setSizeConstraint (QLayout::SetMinimumSize);
  createWidgets (action);
}

CStdAction::~CStdAction ()
{
  delete ui;
}

void CStdAction::createWidgets (QString const & actionName)
{
  CDevice const & device = m_cp->device (m_deviceUUID);
  QStringList serviceIDs = device.serviceIDs (actionName);
  if (!serviceIDs.isEmpty ())
  {
    m_serviceID = serviceIDs.first ();
    if (serviceIDs.size () > 1)
    {
      // To do. Actually assume the action name is always different accross all services.
    }

    CService const &    service = device.services ().value (m_serviceID);
    CAction             action = service.actions ().value (actionName);
    TMArguments const & args   = action.arguments ();
    int                 row    = 0;
    for (TMArguments::const_iterator it = args.cbegin (), end = args.cend (); it != end; ++it)
    {
      CArgument const &      arg               = it.value ();
      QString const &        stateVariableName = arg.relatedStateVariable ();
      CStateVariable const & var               = service.stateVariables ().value (stateVariableName);
      bool                   stringType        = var.type () == CStateVariable::String;

      QString const     name  = it.key ();
      CArgument::EDir   dir   = arg.dir ();
      QWidget*          parent;
      QGridLayout*      layout;
      bool              readOnly;
      if (dir == CArgument::In)
      {
        parent   = ui->m_in;
        layout   = m_inLayout;
        readOnly = false;
      }
      else
      {
        parent   = ui->m_out;
        layout   = m_outLayout;
        readOnly = true;
      }

      QLabel* lName = new QLabel (parent);
      lName->setObjectName ("name:" + name);
      lName->setText (name);
      layout->addWidget (lName, row, 0, Qt::AlignRight);

      QTextEdit* tValue = new QTextEdit (parent);
      tValue->setObjectName (name);
      tValue->setReadOnly (readOnly);
      if (!stringType)
      {
        tValue->setVerticalScrollBarPolicy (Qt::ScrollBarAlwaysOff);
        tValue->setMaximumHeight (22);
      }

      layout->addWidget (tValue, row, 1);
      ++row;
    }
  }
}

void CStdAction::on_m_invoke_clicked ()
{
  m_args.clear ();
  QList<QTextEdit*> argWidgets;
  argWidgets << ui->m_in->findChildren<QTextEdit*>();
  int cInArgs = argWidgets.size ();
  argWidgets << ui->m_out->findChildren<QTextEdit*>();
  for (QTextEdit* argWidget : argWidgets)
  {
    CControlPoint::TArgValue av (argWidget->objectName (), argWidget->toPlainText ());
    m_args << av;
  }

  CActionInfo actionInfo = m_cp->invokeAction (m_deviceUUID, m_serviceID, m_actionName, m_args);
  ui->m_response->setText (actionInfo.response ());
  for (int iOutArg = cInArgs, cArgs = argWidgets.size (); iOutArg < cArgs; ++iOutArg)
  {
    QString const & text = m_args[iOutArg].second;
    argWidgets[iOutArg]->setText (text);
  }
}

void CStdAction::on_m_close_clicked ()
{
  accept ();
}
