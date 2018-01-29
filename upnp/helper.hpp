#ifndef HELPER_UPNP_HPP
#define HELPER_UPNP_HPP 1

#include "upnp_global.hpp"
#include "using_upnp_namespace.hpp"
#include <QString>

START_DEFINE_UPNP_NAMESPACE

/*! Defines a set of functions not include in object. */
/*
  struct SIp4TableElem
  {
    enum EStatus { stUnknow, stRouter };
    enum EType { tyUnknow, tyWireless, tyEthernet };

    SIp4TableElem () {}
    SIp4TableElem (SIp4TableElem const & other) { *this = other; }
    SIp4TableElem (EType type, EStatus status, quint32 ip4Addr) : m_type (type), m_status (status), m_ip4Addr (ip4Addr) {}

    SIp4TableElem& operator = (SIp4TableElem const & other)
    {
      m_type    = other.m_type;
      m_status  = other.m_status;
      m_ip4Addr = other.m_ip4Addr;
      return *this;
    }

    EType   m_type    = tyUnknow;
    EStatus m_status  = stUnknow;
    quint32 m_ip4Addr = 0;
  };

QVector<SIp4TableElem> UPNP_API ip4NetTable ();
*/

/*! \brief Returns actual version of the upnp library. */
char const * UPNP_API libraryVersion ();

/*! Returns actual library name. */
char const * UPNP_API libraryName ();

/*! \brief Replaces 127.0.0.1 by the true IP address.
 *
 * This function is to fix a problem on WMP.
 * When the track is on the WMP server on the local machine the uri contains 127.0.0.1
 * in place of the local address on the network even if the device is at another address on the network.
 */
QString UPNP_API replace127_0_0_1 (QString const & uri);

/*! \brief Returns the application executable folder. */
QString UPNP_API applicationFolder ();

/*! \brief Returns the system for USER-AGENT header. */
QString UPNP_API buildSystemHeader ();

/*! \brief Convert a string time to milliseconds.
 *
 * The time must be in form: h:mn:s.ms. The minimum value must be a value for the seconds.
 */
long long UPNP_API timeToMS (QString const & time);

/*! \brief Remove diacritics.
 *
 * The code is from https://www.codeproject.com/tips/410074/removing-diacritics-from-strings.
 * \param s: The string to converted.
 * \return The string without diacritics.
 */
QString UPNP_API removeDiacritics (QString const & s);

/*! \brief Computes the Jarno Winkler distance between two strings.
 *
 * The code is derived from http://codes-sources.commentcamarche.net/source/49753-distance-de-jaro-winkler
 *
 * \param s1: The first string.
 * \param s2: The second string.
 * \param commonPrefixLength: The Winkler common prefix length. The value must be 0 <= cpl <= 4.
 * for all other values, commonPrefixLength = 4.
 */
float UPNP_API jaroWinklerDistance (const QString& s1, QString const & s2, int commonPrefixLength = 4);

/*! \brief Returns true if the argument representes an UPnP duration.
 *
 * The form must be "hh:mm:ss:xxx" or "h:mm:ss".
 * See http://upnp.org/specs/av/UPnP-av-ContentDirectory-Service.pdf.
 */
bool UPNP_API isDuration (QString const & s);

/*! Return the nearest int. */
inline int UPNP_API nearestInt (float val) { return static_cast<int>(val < 0 ? val - 0.5f : val + 0.5f); }

/*! Returns the http mime type from the UPnP protocol info. */
QString UPNP_API httpContentType (QString const & dlnaProtocolInfo);

/*! \brief  Write data in QtUPnP-dump.xml file.
 *
 * \param data: The data to write.*
 * \param folder: The file folder. QStandardPaths::TempLocation is used if folder is empty.
 */
void UPNP_API xmlDump (QByteArray const & data, QString folder = QString::null);

} // namespace

#endif //HELPER_HPP

