#include "dump.hpp"
#include <QTime>

USING_UPNP_NAMESPACE

CDump* CDump::m_dump = nullptr;

CDump::CDump (QObject* parent) : QObject (parent)
{
  m_dump = this;
}

void CDump::dump (QString const & text)
{
  QTime   time = QTime::currentTime ();
  QString t    = time.toString () + ':' + text + '\n';
  emit m_dump->dumpReady (t);
}

void CDump::dump (QByteArray const & bytes)
{
  dump (QString::fromUtf8 (bytes));
}
