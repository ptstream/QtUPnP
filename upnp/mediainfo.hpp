#ifndef MEDIA_INFO_HPP
#define MEDIA_INFO_HPP

#include "didlitem.hpp"
#include <QSharedDataPointer>

START_DEFINE_UPNP_NAMESPACE

struct SMediaInfoData;

/*! \brief The CMediaInfo class holds information about GetMediaInfo action.
 *
 * See http://upnp.org/specs/av/UPnP-av-AVTransport-Service.pdf.
 *
 * \remark Use implicit Sharing QT technology.
 */
class UPNP_API CMediaInfo
{
public:
  /*! Default constructor. */
  CMediaInfo ();

  /*! Copy constructor. */
  CMediaInfo (const CMediaInfo &);

  /*! Equal operator. */
  CMediaInfo& operator = (const CMediaInfo &);

  /*! Destructor. */
  ~CMediaInfo ();

  /*! Sets the track number.*/
  void setNrTracks (unsigned nrTracks);

  /*! Sets the media duration. */
  void setMediaDuration (QString const & duration);

  /*! Sets current uri. */
  void setCurrentURI (QString const & uri);

  /*! Sets current uri metadata. */
  void setCurrentURIMetaData (QString const & metadata);

  /*! Sets next uri. */
  void setNextURI (QString const & uri);

  /*! Sets next uri metadata. */
  void setNextURIMetaData (QString const & metadata);

  /*! Sets the play medium. */
  void setPlayMedium (QString const & medium);

  /*! Sets the record medium. */
  void setRecordMedium (QString const & medium);

  /*! Sets the write status. */
  void setWriteStatus (QString const & status);

  /*! Returns the track number.*/
  unsigned nrTracks () const;

  /*! Returns the media duration. */
  QString const & mediaDuration () const;

  /*! Returns current uri. */
  QString const & currentURI () const;

  /*! Returns current uri metadata. */
  QString const & currentURIMetaData () const;

  /*! Returns next uri. */
  QString const & nextURI () const;

  /*! Returns next uri metadata. */
  QString const & nextURIMetaData () const;

  /*! Returns the play medium. */
  QString const & playMedium () const;

  /*! Returns the record medium. */
  QString const & recordMedium () const;

  /*! Returns the write status. */
  QString const & writeStatus () const;

  /*! Converts the current uri metadata in a CDidlItem. */
  CDidlItem currentURIDidlItem () const;

  /*! Converts the next uri metadata in a CDidlItem. */
  CDidlItem nextURIDidlItem () const;

private :
  /*! Converts metadata in a CDidlItem. */
  static CDidlItem didlItem (QString const & metaData);

private:
  QSharedDataPointer<SMediaInfoData> m_d; //!< Shared data pointer.
};

} //Namespace

#endif // MEDIA_INFO_HPP
