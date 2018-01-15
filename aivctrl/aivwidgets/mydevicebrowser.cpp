#include "mydevicebrowser.hpp"
#include "widgethelper.hpp"
#include <QShortcut>
#include <QMessageBox>

CMyDeviceBrowserItem::CMyDeviceBrowserItem (QIcon const & icon, QString const & name, EType type) :
                             QListWidgetItem (icon, name), m_type (type)
{
}

bool CMyDeviceBrowserItem::isContainer () const
{
  return m_type == AudioContainer || m_type == ImageContainer || m_type == VideoContainer;
}

bool CMyDeviceBrowserItem::isContainer (EType type)
{
  return type == AudioContainer || type == ImageContainer || type == VideoContainer;
}

bool CMyDeviceBrowserItem::isUserPlaylist () const
{
  return m_type == CMyDeviceBrowserItem::AudioPlaylist ||
         m_type == CMyDeviceBrowserItem::ImagePlaylist ||
         m_type == CMyDeviceBrowserItem::VideoPlaylist;
}

bool CMyDeviceBrowserItem::isFavorite () const
{
  return m_type == CMyDeviceBrowserItem::AudioFavorite ||
         m_type == CMyDeviceBrowserItem::ImageFavorite ||
         m_type == CMyDeviceBrowserItem::VideoFavorite;
}

bool CMyDeviceBrowserItem::operator < (QListWidgetItem const & other) const
{
  bool ret = false;
  if (m_type < static_cast<CMyDeviceBrowserItem const &>(other).m_type)
  {
    ret = true;
  }
  else if (m_type == static_cast<CMyDeviceBrowserItem const &>(other).m_type)
  {
    ret = text () < other.text ();
  }

  return ret;
}


QString CMyDeviceBrowserItem::favoritesPlaylistName (int type)
{
  QString name;
  switch (type)
  {
    case CMyDeviceBrowserItem::AudioFavorite :
      name = "Audio favorites";
      break;

    case CMyDeviceBrowserItem::ImageFavorite :
      name = "Image favorites";
      break;

    case CMyDeviceBrowserItem::VideoFavorite :
      name = "Video favorites";
      break;

    default :
      break;
  }

  return name;
}

QString CMyDeviceBrowserItem::defaultNewPlaylistName (int type)
{
  QString name;
  switch (type)
  {
    case CMyDeviceBrowserItem::AudioContainer:
    case CMyDeviceBrowserItem::AudioFavorite :
    case CMyDeviceBrowserItem::AudioPlaylist :
      name = QObject::tr ("New audio playlist");
      break;

    case CMyDeviceBrowserItem::ImageContainer:
    case CMyDeviceBrowserItem::ImageFavorite :
    case CMyDeviceBrowserItem::ImagePlaylist :
      name = QObject::tr ("New image playlist");
      break;

    default :
      name = QObject::tr ("New video playlist");
      break;
  }

  return name;
}

QString CMyDeviceBrowserItem::playlistName () const
{
  QString name = favoritesPlaylistName ();
  if (name.isEmpty ())
  {
    name = this->text ();
    removeCount (name);
  }

  return name;
}

int CMyDeviceBrowserItem::removeCount (QString& text)
{
  int           count = -1;
  QRegExp const rx (" \\(([0-9]+)\\)");
  int           index = text.lastIndexOf (rx);
  if (index != -1)
  {
    int coef = 1;
    count    = 0;
    for (int k = text.length () - 2; text[k].isNumber (); --k)
    {
      count += (text[k].toLatin1 () - '0') * coef;
      coef  *= 10;
    }

    text.truncate (index);
  }

  return count;
}

void CMyDeviceBrowserItem::setCount (int count)
{
  QString text   = this->text ();
  int     cItems = removeCount (text);
  if (count != cItems)
  {
    text += QString (" (%1)").arg (count);
    setText (text);
  }
}

CMyDeviceBrowser::CMyDeviceBrowser (QWidget* parent) : QListWidget (parent)
{
  setTransparentBackGround (this);
#ifdef Q_OS_LINUX
  setItemMouseOverColor(this);
#endif
  connect (new QShortcut (QKeySequence (Qt::Key_Delete), this), &QShortcut::activated, this, &CMyDeviceBrowser::delKey);
  connect (new QShortcut (QKeySequence (Qt::Key_Escape), this), &QShortcut::activated, this, &CMyDeviceBrowser::clearSelection);
  connect (new QShortcut (QKeySequence (Qt::Key_Return), this), &QShortcut::activated, this, &CMyDeviceBrowser::keyReturn);
}

void CMyDeviceBrowser::createDefaultPlaylists ()
{
  setSortingEnabled (false);
  QString title ("Audio playlists");
  CMyDeviceBrowserItem* item = new CMyDeviceBrowserItem (::resIcon ("audio"), title, CMyDeviceBrowserItem::AudioContainer);
  item->setTextAlignment (Qt::AlignHCenter | Qt::AlignVCenter);
  item->setBackground (m_rootColor);
  QListWidget::addItem (item);

  title = "Image playlists";
  item  = new CMyDeviceBrowserItem (::resIcon ("image"), title, CMyDeviceBrowserItem::ImageContainer);
  item->setTextAlignment (Qt::AlignHCenter | Qt::AlignVCenter);
  item->setBackground (m_rootColor);
  QListWidget::addItem (item);

  title = "Video playlists";
  item  = new CMyDeviceBrowserItem (::resIcon ("video"), title, CMyDeviceBrowserItem::VideoContainer);
  item->setTextAlignment (Qt::AlignHCenter | Qt::AlignVCenter);
  item->setBackground (m_rootColor);
  QListWidget::addItem (item);

  QIcon icon = ::resIcon ("favorites");
  title = CMyDeviceBrowserItem::favoritesPlaylistName (CMyDeviceBrowserItem::AudioFavorite);
  item  = new CMyDeviceBrowserItem (icon, title, CMyDeviceBrowserItem::AudioFavorite);
  QListWidget::addItem (item);
  emit newPlaylist (title, CMyDeviceBrowserItem::AudioFavorite);

  title = CMyDeviceBrowserItem::favoritesPlaylistName (CMyDeviceBrowserItem::ImageFavorite);
  item  = new CMyDeviceBrowserItem (icon, title, CMyDeviceBrowserItem::ImageFavorite);
  QListWidget::addItem (item);
  emit newPlaylist (title, CMyDeviceBrowserItem::ImageFavorite);

  title = CMyDeviceBrowserItem::favoritesPlaylistName (CMyDeviceBrowserItem::VideoFavorite);
  item  = new CMyDeviceBrowserItem (icon, title, CMyDeviceBrowserItem::VideoFavorite);
  QListWidget::addItem (item);
  emit newPlaylist (title, CMyDeviceBrowserItem::VideoFavorite);
  setSortingEnabled (true);
  sortItems ();
}

void CMyDeviceBrowser::retranslateSpecialItems ()
{
  for (int iItem = 0, cItems = count (); iItem < cItems; ++iItem)
  {
    QString               name;
    CMyDeviceBrowserItem* item = static_cast<CMyDeviceBrowserItem*>(this->item (iItem));
    switch (item->type ())
    {
      case CMyDeviceBrowserItem::AudioContainer :
        name = QObject::tr ("Audio playlists");
        break;

      case CMyDeviceBrowserItem::AudioFavorite :
        name = QObject::tr ("Audio favorites");
        break;

      case CMyDeviceBrowserItem::ImageContainer :
        name = QObject::tr ("Image playlists");
        break;

      case CMyDeviceBrowserItem::ImageFavorite :
        name = QObject::tr ("Image favorites");
        break;

      case CMyDeviceBrowserItem::VideoContainer :
        name = QObject::tr ("Video playlists");
        break;

      case CMyDeviceBrowserItem::VideoFavorite :
        name = QObject::tr ("Video favorites");
        break;

      default :
        break;
    }

    if (!name.isEmpty ())
    {
      item->setText (name);
    }
  }
}

bool CMyDeviceBrowser::isCollapsed (CMyDeviceBrowserItem* item)
{
  bool collapse = false;
  if (item->isContainer ())
  {
    int row = this->row (item) + 1;
    item    = this->item (row);
    if (item != nullptr)
    {
      collapse = item->isHidden ();
    }
  }

  return collapse;
}

void CMyDeviceBrowser::setCollapsed (CMyDeviceBrowserItem::EType type, bool collapse)
{
  if (type == CMyDeviceBrowserItem::AudioContainer || type == CMyDeviceBrowserItem::ImageContainer || type == CMyDeviceBrowserItem::VideoContainer)
  {
    CMyDeviceBrowserItem* item = this->item (type);
    setCollapsed (item, collapse);
  }
}

void CMyDeviceBrowser::setCollapsed (CMyDeviceBrowserItem* item, bool collapse)
{
  if (item->isContainer ())
  {
    int row = this->row (item) + 1;
    while ((item = this->item (row)) != nullptr && !item->isContainer ())
    {
      item->setHidden (collapse);
      ++row;
    }
  }
}

bool CMyDeviceBrowser::collapseExpand (CMyDeviceBrowserItem* item)
{
  bool collapse = false;
  if (item->isContainer ())
  {
    QPoint      pos   = viewport ()->mapFromGlobal (QCursor::pos ());
    QModelIndex index = indexAt (pos);
    QRect       rect  = visualRect (index);
    QSize       size  = iconSize ();
    rect.setSize (size);
    if (rect.contains (pos))
    {
      int                   row      = this->row (item) + 1;
      CMyDeviceBrowserItem* nextItem = this->item (row);
      if (nextItem != nullptr)
      {
        collapse = !nextItem->isHidden ();
        setCollapsed (item, collapse);
      }
    }
  }

  return collapse;
}

void CMyDeviceBrowser::containerDoubleClicked (CMyDeviceBrowserItem* item)
{
  if (item->isContainer ())
  {
    if (isCollapsed (item))
    {
      setCollapsed (item, false);
    }

    CMyDeviceBrowserItem::EType playlistType = static_cast<CMyDeviceBrowserItem::EType>(item->type ());
    switch (playlistType)
    {
      case CMyDeviceBrowserItem::AudioContainer :
        playlistType = CMyDeviceBrowserItem::AudioPlaylist;
        break;

      case CMyDeviceBrowserItem::ImageContainer :
        playlistType = CMyDeviceBrowserItem::ImagePlaylist;
        break;

      default :
        playlistType = CMyDeviceBrowserItem::VideoPlaylist;
        break;
    }

    // Find the last item for the category.
    QString name = CMyDeviceBrowserItem::defaultNewPlaylistName (playlistType);
    item         = new CMyDeviceBrowserItem (::resIcon ("playlist"), name, playlistType);
    name         = updateName (item);
    item->setText (name);
    item->setFlags (item->flags () | Qt::ItemIsEditable | Qt::ItemIsSelectable | Qt::ItemIsEnabled);
    QListWidget::addItem (item);
    setCurrentItem (item);
    editItem (item);
    emit newPlaylist (name, playlistType);
    updateContainerItemCount ();
  }
}

void CMyDeviceBrowser::commitData (QWidget *editor)
{
  CMyDeviceBrowserItem* item = static_cast<CMyDeviceBrowserItem*>(currentItem ());
  QString               name = item->text ();
  QListWidget::commitData (editor);
  if (item->text ().isEmpty ())
  {
    name =  CMyDeviceBrowserItem::defaultNewPlaylistName (item->type ());
    item->setText (name);
  }

  QString newName = updateName (item);
  item->setText (newName);
  emit renamePlaylist (name, newName);
}

void CMyDeviceBrowser::delKey ()
{
  QList<QListWidgetItem*> items = selectedItems ();
  for (QListWidgetItem* item : items)
  {
    CMyDeviceBrowserItem* mdbItem = static_cast<CMyDeviceBrowserItem*>(item);
    if (mdbItem->isUserPlaylist ())
    {
      QString const               name  = mdbItem->text ();
      QString                     title = QObject::tr ("Confirm playlist removing");
      QMessageBox::StandardButton btn   = QMessageBox::question (this, title, name);
      if (btn == QMessageBox::Yes)
      {
        emit removePlaylist (mdbItem->playlistName ());
        delete item;
        updateContainerItemCount ();
      }
    }
  }
}

void CMyDeviceBrowser::rename ()
{
  QList<QListWidgetItem*> items = selectedItems ();
  if (!items.isEmpty ())
  {
    QListWidgetItem* item = items.first ();
    editItem (item);
  }
}


QString CMyDeviceBrowser::updateName (QListWidgetItem* item)
{
  int         cItems = count ();
  QStringList existingNames;
  existingNames.reserve (cItems);
  for (int iItem = 0; iItem < cItems; ++iItem)
  {
    CMyDeviceBrowserItem* itemPlaylist = this->item (iItem);
    if (item != itemPlaylist && itemPlaylist->type () >= CMyDeviceBrowserItem::AudioFavorite)
    {
      existingNames << itemPlaylist->text ();
    }
  }

  return ::updateName (item->text (), existingNames);
}

CMyDeviceBrowserItem* CMyDeviceBrowser::addItem (int type, QString const & name)
{
  CMyDeviceBrowserItem* item = new CMyDeviceBrowserItem (::resIcon ("playlist"), name, static_cast<CMyDeviceBrowserItem::EType>(type));
  item->setFlags (item->flags () | Qt::ItemIsEditable | Qt::ItemIsSelectable | Qt::ItemIsEnabled);
  QListWidget::addItem (item);
  return item;
}

CMyDeviceBrowserItem* CMyDeviceBrowser::containerItem (CMyDeviceBrowserItem::EType type)
{
  if (!CMyDeviceBrowserItem::isContainer (type))
  {
    switch (type)
    {
      case CMyDeviceBrowserItem::AudioFavorite :
      case CMyDeviceBrowserItem::AudioPlaylist :
        type = CMyDeviceBrowserItem::AudioContainer;
        break;

      case CMyDeviceBrowserItem::ImageFavorite :
      case CMyDeviceBrowserItem::ImagePlaylist :
        type = CMyDeviceBrowserItem::ImageContainer;
        break;

      default :
        type = CMyDeviceBrowserItem::VideoContainer;
        break;
    }
  }

  return this->item (type);
}

CMyDeviceBrowserItem* CMyDeviceBrowser::containerItem (CMyDeviceBrowserItem* item)
{
  CMyDeviceBrowserItem* container = nullptr;
  if (item->isContainer ())
  {
    container = item;
  }
  else
  {
    for (int iItem = row (item); iItem >= 0; --iItem)
    {
      CMyDeviceBrowserItem* mdbItem = static_cast<CMyDeviceBrowserItem*>(this->item (iItem));
      if (mdbItem->isContainer ())
      {
        container = mdbItem;
        break;
      }
    }
  }

  return container;
}

CMyDeviceBrowserItem* CMyDeviceBrowser::item (CMyDeviceBrowserItem::EType type)
{
  CMyDeviceBrowserItem* itemFromType = nullptr;
  for (int iItem = 0, cItems = count (); iItem < cItems; ++iItem)
  {
    CMyDeviceBrowserItem* item = static_cast<CMyDeviceBrowserItem*>(this->item (iItem));
    if (item != nullptr && item->type () == type)
    {
      itemFromType = item;
      break;
    }
  }

  return itemFromType;
}

void CMyDeviceBrowser::keyReturn ()
{
  if (hasFocus())
  {
    QList<QListWidgetItem*> items = selectedItems ();
    if (!items.isEmpty ())
    {
      emit itemDoubleClicked (items.first ());
    }
  }
}

void CMyDeviceBrowser::setCount (CMyDeviceBrowserItem::EType type, int count)
{
  CMyDeviceBrowserItem* item = this->item (type);
  if (item != nullptr)
  {
    item->setCount (count);
  }
}

void CMyDeviceBrowser::setCount (QString const & name, int count)
{
  for (int iItem = 0, cItems = this->count (); iItem < cItems; ++iItem)
  {
    CMyDeviceBrowserItem* item      = static_cast<CMyDeviceBrowserItem*>(this->item (iItem));
    QString               text      = item->text ();
    int                   itemCount = CMyDeviceBrowserItem::removeCount (text);
    if (name == text && count != itemCount)
    {
      item->setCount (count);
      break;
    }
  }
}

void CMyDeviceBrowser::updateContainerItemCount ()
{
  int cAudios = 0, cImages = 0, cVideos = 0;
  for (int iItem = 0, cItems = this->count (); iItem < cItems; ++iItem)
  {
    CMyDeviceBrowserItem* item = static_cast<CMyDeviceBrowserItem*>(this->item (iItem));
    switch (item->type ())
    {
      case CMyDeviceBrowserItem::AudioPlaylist :
      case CMyDeviceBrowserItem::AudioFavorite :
        ++cAudios;
        break;

      case CMyDeviceBrowserItem::ImagePlaylist :
      case CMyDeviceBrowserItem::ImageFavorite  :
        ++cImages;
        break;

      case CMyDeviceBrowserItem::VideoPlaylist :
      case CMyDeviceBrowserItem::VideoFavorite :
        ++cVideos;
        break;

      default :
        break;
    }
  }

  CMyDeviceBrowserItem* item = this->item (CMyDeviceBrowserItem::AudioContainer);
  item->setCount (cAudios);

  item = this->item (CMyDeviceBrowserItem::ImageContainer);
  item->setCount (cImages);

  item = this->item (CMyDeviceBrowserItem::VideoContainer);
  item->setCount (cVideos);
}




