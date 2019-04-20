#include "pluginobject.hpp"
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QFileInfo>

USING_UPNP_NAMESPACE

//fields=appProperties,capabilities,contentHints,createdTime,description,explicitlyTrashed,fileExtension,folderColorRgb,fullFileExtension,headRevisionId,iconLink,id,imageMediaMetadata,isAppAuthorized,kind,lastModifyingUser,md5Checksum,mimeType,modifiedByMeTime,modifiedTime,name,originalFilename,ownedByMe,owners,parents,permissions,properties,quotaBytesUsed,shared,sharedWithMeTime,sharingUser,size,spaces,starred,thumbnailLink,trashed,version,videoMediaMetadata,viewedByMe,viewedByMeTime,viewersCanCopyContent,webContentLink,webViewLink,writersCanShare

/*! Can be use if the renderer handle correctly https protocol. */
//#define HTTPSHANDLED

/*! Returns the suffix of json name field. */
static QString suffix (QString const & name)
{
  QFileInfo fi (name);
  return fi.suffix ().toLower ();
}

/*! Converts UPnP sort criteria in Google sort criteria.
 * Just +dc:title and -dc:title is handled.
 */
static QString orderBy (QString const & sortCriteria)
{
  QString pluginCriteria;
  QStringList keys = sortCriteria.split (',');
  for (QString& key : keys)
  {
    key = key.trimmed ();
    if (key.endsWith ("dc:title"))
    {
      pluginCriteria += "folder,name";
      if (key[0] == '-')
      {
        pluginCriteria += " desc";
      }
    }
  }

  return pluginCriteria;
}

CBrowseReply CPluginObject::browse (QString const & objectID, CContentDirectory::EBrowseType /*type*/,
                                    QString const & /*filter*/, int /*startingIndex*/, int /*requestedCount*/,
                                    QString const & sortCriteria)
{
  CBrowseReply reply;
  if (isValidOAuth2Autorisation ())
  {
    QString googleSortCriteria = orderBy (sortCriteria.isEmpty () ? "dc:title" : sortCriteria);
    QString query              = m_apiHost + "files";
    QString parentID           = objectID == "0" ? "root" : objectID;
    query                     += QString ("?q='%1'%20in%20parents%20and%20trashed=false").arg (parentID);
    query                     += "&fields=files(id,mimeType,name,iconLink,imageMediaMetadata,size,thumbnailLink,videoMediaMetadata)";
    query                     += "&orderBy=" + googleSortCriteria;

    QList<CPlugin::TVarVal> headers;
    headers.append (accessTokenHeader ());
    QByteArray data = callData (CPlugin::Get, query, headers);
    if (!data.isEmpty ())
    {
      QJsonParseError     error;
      QJsonDocument       doc (QJsonDocument::fromJson (data, &error));
      QJsonObject const & json    = doc.object ();
      bool                success = error.error == QJsonParseError::NoError;
      if (success)
      { // Parse the return.
        char const *      protocolInfoFormat = "http-get:*:%1:DLNA.ORG_OP=01;DLNA.ORG_FLAGS=01700000000000000000000000000000";
        QJsonArray        files              = json["files"].toArray ();
        QList<CDidlItem>& items              = reply.items ();
        for (QJsonArray::const_iterator it = files.constBegin (), end = files.constEnd (); it != end; ++it)
        {
          QJsonObject file = (*it).toObject ();
          QString name     = file["name"].toString ();
          QString mimeType = file["mimeType"].toString ();
          QString tag      = "item";
          QString className;
          QString protocolInfo;
          if (mimeType.contains ("audio"))
          {
            className = "object.item.audioItem.musicTrack";
            if (mimeType.endsWith ("aiff") || mimeType.endsWith ("aif"))
            {
              mimeType = "audio/x-aiff";
            }
            else
            {
              QString suffix = ::suffix (name);
              if (suffix == "mp4" || suffix == "m4a")
              {
                mimeType = "audio/mp4";
              }
            }

            protocolInfo = QString (protocolInfoFormat).arg (mimeType);
          }
          else if (mimeType.contains ("image"))
          {
            className    = "object.item.imageItem.photo";
            protocolInfo = QString (protocolInfoFormat).arg (mimeType);
          }
          else if (mimeType.contains ("video"))
          { // Google return video/ogg. convert it to audio/ogg.
            QString suffix = ::suffix (name);
            if (suffix == "ogg")
            {
              className    = "object.item.audioItem.musicTrack";
              protocolInfo = QString (protocolInfoFormat).arg ("audio/ogg");
            }
            else
            {
              className = "object.item.videoItem.movie";
              protocolInfo = QString (protocolInfoFormat).arg (mimeType);
            }
          }
          else if (mimeType == "application/vnd.google-apps.folder")
          {
            className = "object.container.storageFolder";
            tag       = "container";
          }
          else if (mimeType.startsWith ("application/"))
          { // Handle unknown or incorrect mime type.
            QString suffix = ::suffix (name);
            if (suffix == "dsf" || suffix == "dff" || suffix == "dts" ||
                suffix == "ape" || suffix == "oga" || suffix == "flac")
            {
              className    = "object.item.audioItem.musicTrack";
              protocolInfo = QString (protocolInfoFormat).arg ("audio/x-" + suffix);
            }
            if (suffix == "wv")
            {
              className    = "object.item.audioItem.musicTrack";
              protocolInfo = QString (protocolInfoFormat).arg ("audio/x-wavpack");
            }
            else if (suffix == "ogv")
            {
              className    = "object.item.videoItem.movie";
              protocolInfo = QString (protocolInfoFormat).arg ("video/ogv");
            }
          }
          else
          {
            // to do
          }

          if (!className.isEmpty ())
          {
            CDidlItem didlItem;
            didlItem.insert ("aivctrl:plugin", CDidlElem (m_uuid));
            didlItem.insert ("upnp:class", CDidlElem (className));
            didlItem.insert ("dc:title", CDidlElem (name));

            QString albumArtURI = file["thumbnailLink"].toString ();
            if (albumArtURI.isEmpty ())
            {
              albumArtURI = file["iconLink"].toString ();
            }

            if (!albumArtURI.isEmpty ())
            {
              didlItem.insert ("dc:albumArtURI", CDidlElem (albumArtURI));
            }

            QString id = file["id"].toString ();
            if (!id.isEmpty ())
            {
              CDidlElem item;
              item.addProp ("id", id);
              item.addProp ("parentID", parentID);
              didlItem.insert (tag, item);
              if (tag == "item")
              {
                CDidlElem res;
                QString   size = file["size"].toString ();
                if (!size.isEmpty ())
                {
                  res.addProp ("size", size);
                }

                if (className.endsWith ("musicTrack") || className.endsWith ("movie"))
                {
                  QString ms = file["durationMillis"].toString ();
                  if (!ms.isEmpty ())
                  {
                    QString  duration = QTime (0, 0).addMSecs (ms.toInt ()).toString ("hh:mm:ss.zzz");
                    res.addProp ("duration", duration);
                  }
                }

                if (!className.endsWith ("musicTrack"))
                {
                  QString w = file["width"].toString ();
                  QString h = file["height"].toString ();
                  if (!w.isEmpty () && !h.isEmpty ())
                  {
                    res.addProp ("resolution", w + 'x' + h);
                  }
                }

#ifdef HTTPSHANDLED
                // The renderer must handle redirections.
                QString query (m_apiHost + "files/" + id + "?alt=media");
                addAccessToken (query);
                res.setValue (query);
#else
                QString name = file["name"].toString ();
                id          += "?Content-Type=" + httpContentType (protocolInfo);
                id           = QUrl::toPercentEncoding (id);
                res.setValue (id);
                res.addProp ("protocolInfo", protocolInfo);
#endif
                didlItem.insert ("res", res);
              }

              items.append (didlItem);
            }
          }
        }

        unsigned numberReturned = static_cast<unsigned>(items.size ());
        reply.setNumberReturned (numberReturned);
        reply.setTotalMatches (numberReturned);
      }
    }
  }

  return reply;
}
