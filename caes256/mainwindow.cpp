#include "mainwindow.hpp"
#include "ui_mainwindow.h"
#include "../upnp/aesencryption.h"
#include <QCryptographicHash>

CMainWindow::CMainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::CMainWindow)
{
  ui->setupUi(this);
}

CMainWindow::~CMainWindow()
{
  delete ui;
}

void CMainWindow::on_m_crypt_clicked ()
{
  QByteArray crypted, uncrypted;
  QLineEdit* les[] = { ui->m_u1, ui->m_u2 };
  QLabel*    las[] = { ui->m_c1, ui->m_c2 };
  for (unsigned k = 0; k < sizeof (les) / sizeof (void*); ++k)
  {
    QString text = les[k]->text ();
    if (!text.isEmpty ())
    {
      uncrypted = text.toLatin1 ();
#if HIDDENFILES==1
      // The AIVCtrl AES256 key and iv
#include "../../Hidden-Files/aes256.h"
#else
      // The example AES256 key and iv to build from GitHub.
#include "../upnp/aes256.h"
#endif
      QByteArray hashKey = QCryptographicHash::hash (key, QCryptographicHash::Sha256);
      QByteArray hashIV  = QCryptographicHash::hash (iv, QCryptographicHash::Sha256);
      crypted            = CAESEncryption::Crypt (CAESEncryption::AES_256, CAESEncryption::CBC, uncrypted, hashKey, hashIV);
      crypted            = crypted.toBase64 ();
    }
    else
    {
      crypted = "Nothing to crypt.";
    }

    las[k]->setText (crypted);
  }
}

