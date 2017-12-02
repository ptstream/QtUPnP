#ifndef XMLHPLAYLIST_HPP
#define XMLHPLAYLIST_HPP

#include "../upnp/xmlh.hpp"
#include "../upnp/didlitem.hpp"

class CPlaylist;

/*! Xml handler to read a playlist at format xspf.
 * This handler creates from standard tag of the xspf file the CDidlItems.
 * If QtUPnP extensions tags exists, these are mixed with the previous.
 */
class CXmlHPlaylist : public QtUPnP::CXmlH
{
public:
  CXmlHPlaylist (CPlaylist& playlist);

  virtual bool startElement (QString const &namespaceURI, QString const & localName, QString const &, QXmlAttributes const &);
  virtual bool characters (QString const & name);

private :
  CPlaylist& m_playlist;
  QString m_name;
};

#endif // XMLHPLAYLIST_HPP
