#include "mainwindow.hpp"
#include "ui_mainwindow.h"

QStringList CMainWindow::serverIcons ()
{
  return { QString::null,
           "server",
         };
}

QStringList CMainWindow::contentDirectoryIcons ()
{
  return { QString::null, //!< unknown
           "image",       //!< object.item.imageItem
           "image",       //!< object.item.imageItem.photo
           "audio",       //!< bject.item.audioItem
           "audio",       //!< object.item.audioItem.musicTrack
           "audio",       //!< object.item.audioItem.audioBroadcast
           "audio",       //!< object.item.audioItem.audioBook
           "video",       //!< object.item.videoItem
           "video",       //!< object.item.videoItem.movie
           "video",       //!< object.item.videoItem.videoBroadcast
           "video",       //!< object.item.videoItem.musicVideoClip
           "text",        //!< object.item.textItem
           "bookmark",    //!< object.item.bookmarkItem
           QString::null, //!< object.item.epgItem
           "audio",       //!< object.item.epgItem.audioProgram
           "video",       //!< object.item.epgItem.videoProgram
           "playlist",    //!< object.item.playlistItem
           "folder",      //!< object.container
           "folder",      //!< object.container.playlistContainer
           "music_album", //!< object.container.album.musicAlbum
           "photo_album", //!< object.container.album.photoAlbum
           "folder",      //!< object.container.epgContainer
           "folder",      //!< object.container.storageSystem
           "folder",      //!< object.container.storageVolume
           "folder",      //!< object.container.storageFolder
           "folder",      //!< object.container.bookmarkFolder
           "artist",      //!< object.container.person
           "artist",      //!< object.container.person.musicArtist
           "genre",       //!< object.container.genre
           "genre",       //!< object.container.genre.musicGenre
           "genre",       //!< object.container.genre.movieGenre
         };
}
