#include "mainwindow.hpp"
#include <QApplication>

/*! This simple program is used to encrypt client_id and client_secret
 * to use with the QtUPnP plugins. They are the OAuth2 tokens.
 * The two encrypted values are stored in .ids file.
 *
 * All secret values are AES256 crypted with key and iv defined in ../upnp/aes256.h.
 * The crypt an decrypt use cryptographic hashes (sha256) of key and iv before use.
 * Lastly rhese two values are converted in base 64.
 */
int main(int argc, char *argv[])
{
  QApplication a(argc, argv);
  CMainWindow w;
  w.show();

  return a.exec();
}
