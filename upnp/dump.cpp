#include "dump.hpp"

USING_UPNP_NAMESPACE

CDump* CDump::m_dump = nullptr;

CDump::CDump (QObject* parent) : QObject (parent)
{
  m_dump = this;
}

void CDump::dump (QString const & text)
{
  emit m_dump->dumpReady (text);
}

void CDump::dump (QByteArray const & bytes)
{
  QString text (bytes);
  emit m_dump->dumpReady (text);
}
