#ifndef PLAYLISTBROWSER_HPP
#define PLAYLISTBROWSER_HPP

#include "contentdirectorybrowser.hpp"

namespace QtUPnP
{
  class CControlPoint;
}

/*! \brief Playlist content browser. */
class CPlaylistBrowser : public CContentDirectoryBrowser
{
  Q_OBJECT

public:
  /*! Default constructor. */
  CPlaylistBrowser (QWidget* parent);

  /*! Invokes SetAVTransportURI action. */
  void setAVTransportURI (QtUPnP::CControlPoint* cp, QString const & renderer, int itemRow);

  /*! Invokes SetNextAVTransportURI action. */
  void setNextAVTransportURI (QtUPnP::CControlPoint* cp, QString const & renderer);

  /*! Returns the playlist name. */
  QString const & name () const { return m_name; }

  /*! Sets the playlist name. */
  void setName (QString const & name) { m_name = name; }

public slots :
  /*! Removes an item. */
  void delKey ();

signals :
  /*! An item has been removed. */
  void removeIDs (QString const & name, QStringList const & ids);

  /*! Items has been moved (drag and drop). */
  void rowsMoved (QModelIndex const & parent, int start, int end, QModelIndex const & destination, int row);

  /*! Items are about to moved. */
  void rowsAboutToBeMoved (QModelIndex const & parent, int start, int end, QModelIndex const & destination, int row);

protected :
  QString m_name; //!< Playlist name.
};

#endif // PLAYLISTBROWSER_HPP
