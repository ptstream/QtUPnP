
#include "helper.hpp"
#include "upnpsocket.hpp"
#include <QStandardPaths>
#include <QDir>
#include <QCoreApplication>

//#ifdef Q_OS_WIN
//#include <winsock2.h>
//#include <ws2ipdef.h>
//#include <iphlpapi.h>
//#include <Ws2tcpip.h>
//#endif

char const * QtUPnP::libraryVersion ()
{
  return "1.1.4";
}

char const * QtUPnP::libraryName ()
{
  return "QtUPnP";
}

QString QtUPnP::replace127_0_0_1 (QString const & uri)
{
  QString correctedURI = uri;
  correctedURI.replace ("127.0.0.1", CUpnpSocket::localHostAddress ().toString ());
  return correctedURI;
}

/*
QVector<QtUPnP::SIp4TableElem> QtUPnP::ip4NetTable ()
{
  quint32 localAddr = CUpnpSocket::localHostAddress ().toIPv4Address ();
  quint8  localB1   = (localAddr & 0xff000000) >> 24;
  quint8  localB2   = (localAddr & 0x00ff0000) >> 16;

  QVector<QtUPnP::SIp4TableElem> ipAddrs;
#ifdef Q_OS_WIN
  PMIB_IPNET_TABLE2 ipTable = nullptr;
  unsigned          status  = ::GetIpNetTable2 (AF_INET, &ipTable);
  if (status == NO_ERROR)
  {
    for (unsigned i = 0; i < ipTable->NumEntries; ++i)
    {
      if (ipTable->Table[i].State == NlnsUnreachable)
      {
        continue;
      }

      SOCKADDR_IN ipv4 = ipTable->Table[i].Address.Ipv4;
      if (ipv4.sin_addr.S_un.S_un_b.s_b1 == localB1 &&
          ipv4.sin_addr.S_un.S_un_b.s_b2 == localB2 &&
          ipv4.sin_addr.S_un.S_un_b.s_b4 != 0xff)
      {
        QtUPnP::SIp4TableElem::EType type = QtUPnP::SIp4TableElem::tyUnknow;
        if (ipTable->Table[i].InterfaceLuid.Info.IfType == IF_TYPE_ETHERNET_CSMACD)
        {
          type = QtUPnP::SIp4TableElem::tyEthernet;
        }
        else if (ipTable->Table[i].InterfaceLuid.Info.IfType == IF_TYPE_IEEE80211)
        {
          type = QtUPnP::SIp4TableElem::tyWireless;
        }

        QtUPnP::SIp4TableElem::EStatus status = QtUPnP::SIp4TableElem::stUnknow;
        if (ipTable->Table[i].IsRouter != 0)
        {
          status = QtUPnP::SIp4TableElem::stRouter;
        }

        quint32 addr = ((ipv4.sin_addr.S_un.S_un_b.s_b1 << 24) & 0xff000000) |
                       ((ipv4.sin_addr.S_un.S_un_b.s_b2 << 16) & 0x00ff0000) |
                       ((ipv4.sin_addr.S_un.S_un_b.s_b3 <<  8) & 0x0000ff00) |
                       ipv4.sin_addr.S_un.S_un_b.s_b4;

        QVector<QtUPnP::SIp4TableElem>::iterator it;
        for (it = ipAddrs.begin (); it != ipAddrs.end (); ++it)
        {
          QtUPnP::SIp4TableElem& elem = *it;
          if (elem.m_ip4Addr == addr && elem.m_type == QtUPnP::SIp4TableElem::tyWireless && type == QtUPnP::SIp4TableElem::tyEthernet)
          {
            elem.m_type = type;
            break;
          }
        }

        if (it == ipAddrs.end ())
        {
          QtUPnP::SIp4TableElem elem (type, status, addr);
          ipAddrs.push_back (elem);
        }
      }
    }

    auto lessThan = [](QtUPnP::SIp4TableElem const & e1, QtUPnP::SIp4TableElem const & e2) -> int
    {
      if (e1.m_ip4Addr == e2.m_ip4Addr)
      {
        return 0;
      }

      return (e1.m_ip4Addr &0xff) > (e2.m_ip4Addr & 0xff) ? -1 : +1;
    };

    std::sort (ipAddrs.begin (), ipAddrs.end (), lessThan);
  }

  ::FreeMibTable (ipTable);
#endif
  return ipAddrs;
}
*/

QString QtUPnP::applicationFolder ()
{
  QFileInfo fileInfo (QCoreApplication::applicationFilePath ());
  return fileInfo.absolutePath ();
}

QString QtUPnP::buildSystemHeader ()
{
  QString name;
#ifdef Q_OS_WIN
  QSysInfo::WinVersion version = QSysInfo::WindowsVersion;
  switch (version)
  {
    case QSysInfo::WV_VISTA:
      name = "Microsoft-Windows/6.0";
      break;

    case QSysInfo::WV_WINDOWS7:
      name = "Microsoft-Windows/6.1";
      break;

    case QSysInfo::WV_WINDOWS8:
      name = "Microsoft-Windows/6.2";
      break;


    case QSysInfo::WV_WINDOWS8_1:
      name = "Microsoft-Windows/6.3";
      break;

    default :
      name = "Microsoft-Windows/10.0";
      break;
  }
#else
  name = QSysInfo::kernelType () + '/' + QSysInfo::kernelVersion ();
#endif

  return name;
}

long long QtUPnP::timeToMS (QString const & time)
{
  long long duration = 0;
  if (!time.isEmpty ())
  {
    QStringList hms      = time.split (':');
    int         size     = hms.size ();
    if (size > 0)
    {
      duration = static_cast<long long>(hms[size - 1].toDouble ()) * 1000;
      if (size >= 2)
      {
        unsigned m = hms[size - 2].toUInt ();
        duration  += m * 60 * 1000;
        if (size >= 3)
        {
          unsigned h = hms[size - 3].toUInt ();
          duration  += h * 3600 * 1000;
        }
      }
    }
  }

  return duration;
}

int QtUPnP::timeToS (QString const & time)
{
  long long ms  = timeToMS (time);
  auto      msd = static_cast<double>(ms);
  return static_cast<int>(qRound64 (msd / 1000.0));
}

/*! \brief Internal structure used by determineMatchesAndTransposes static function. */
struct SMatchResults
{
   int numberOfMatches = 0;
   int numberOfTransposes = 0;
};

static int matchingWindow (QString const & s1, QString const & s2)
{
  return (std::max (s1.length (), s2.length ()) / 2) - 1;
}

static SMatchResults determineMatchesAndTransposes (QString const & s1, QString const & s2)
{
   SMatchResults matchResults;
   int window = matchingWindow (s1, s2);
   QString shortest, longest;
   if(s1.length () <= s2.length ())
   {
     shortest = s1;
     longest = s2;
   }
   else
   {
     shortest = s2;
     longest = s1;
   }

   int shortestLen = shortest.length ();
   int longestLen  = longest.length ();

   int matchedOutOfPosition = 0;
   for (int i = 0; i < shortestLen; ++i)
   {
     if (shortest[i] == longest[i])
     {
       ++matchResults.numberOfMatches;
       continue;
     }

      int backwardBoundary = (i - window) < 0 ? 0 : i - window;
      int forwardBoundary  = (i + window) > (longestLen - 1) ? longestLen - 1 : i + window;
      for (int b = backwardBoundary; b <= forwardBoundary; ++b)
      {
        if (longest[b] == shortest[i])
        {
          ++matchResults.numberOfMatches;
          ++matchedOutOfPosition;
          break;
        }
     }
   }

   matchResults.numberOfTransposes = matchedOutOfPosition / 2;
   return matchResults;
 }

static float jaroDistance (float numMatches, float numTransposes, float s1Length, float s2Length)
{
  float dist = 0.0f;
  if (numMatches != 0)
  {
    // (|A| / m)
    float s1Norm = numMatches / s1Length;
    // (|B| / m)
    float s2Norm = numMatches / s2Length;
    // ((m - t) / m)
    float matchTransNorm = (numMatches - numTransposes) / numMatches;
    // 1/3 * ((|A| / m) + (|B| / m) + ((m - t) / m))
    dist = (s1Norm + s2Norm + matchTransNorm) / 3.0f;
  }

  return dist;
}

static int winklerCommonPrefix (QString s1, QString s2, int commonPrefixLength)
{
  if (commonPrefixLength < 0 || commonPrefixLength > 4)
  {
    commonPrefixLength = 4;
  }

  int boundary     = std::min (s1.length (), s2.length ());
  int commonPrefix = 0;
  int i            = 0;
  while (i < boundary && s1[i] == s2[i] && (commonPrefix <= commonPrefixLength || commonPrefixLength < 0))
  {
    ++commonPrefix;
    ++i;
  }

  return commonPrefix;
}

float QtUPnP::jaroWinklerDistance (QString const & s1, QString const & s2, int commonPrefixLength)
{
  SMatchResults matchResults = determineMatchesAndTransposes (s1, s2);
  float         jaroDistance = ::jaroDistance (matchResults.numberOfMatches,  matchResults.numberOfTransposes, s1.length (), s2.length ());
  //Find the Winkler common prefix length (4 characters max)
  int winklerCommonPrefix = ::winklerCommonPrefix (s1, s2, commonPrefixLength);
  //Find the Jaro-Winkler Distance = Jd + (l * p * ( 1 - Jd));
  float dist = jaroDistance + (winklerCommonPrefix * 0.1f) * (1.0f - jaroDistance);
  return dist;
}

QString QtUPnP::removeDiacritics (QString const & s)
{
  QString normalizedString = s.normalized (QString::NormalizationForm_D);
  QString result;
  for (QChar c : normalizedString)
  {
    if (c.category () != QChar::Mark_NonSpacing)
    {
      result.append (c);
    }
  }

  return result.normalized (QString::NormalizationForm_C);
}

bool QtUPnP::isDuration (QString const & s)
{
  QRegExp re ("((\\d+:)?\\d{1,2}:\\d{1,2}(\\.\\d+)?)"); // 13:12:42.126 - 13:12:42
  return re.exactMatch (s);
}

QString QtUPnP::httpContentType (QString const & dlnaProtocolInfo)
{
  QString contentType;
  QStringList elems = dlnaProtocolInfo.split (':');
  if (elems.size () > 2)
  {
    contentType = elems[2];
  }

  return contentType;
}

void QtUPnP::xmlDump (QByteArray const & data, QString folder)
{
  if (folder.isEmpty ())
  {
    QStringList folders = QStandardPaths::standardLocations (QStandardPaths::TempLocation);
    if (!folders.isEmpty ())
    {
      folder = folders.first ();
    }
  }

  QDir dir (folder);
  QFile file (dir.absoluteFilePath ("QtUPnP-dump.xml"));
  file.open (QIODevice::WriteOnly);
  file.write(data);
  file.close ();
}
