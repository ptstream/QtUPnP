#ifndef POSITION_INFO_HPP
#define POSITION_INFO_HPP

#include "didlitem.hpp"
#include <QSharedDataPointer>

START_DEFINE_UPNP_NAMESPACE

struct SPositionInfoData;

/*! \brief The CPositionInfo class holds information about GetPositionInfo action.
 *
 * See http://upnp.org/specs/av/UPnP-av-AVTransport-Service.pdf.
 *
 * \remark Use implicit Sharing QT technology.
 */
class UPNP_API CPositionInfo
{
public:
  /*! Default constructor. */
  CPositionInfo ();

  /*! Copy constructor. */
  CPositionInfo (const CPositionInfo &);

  /*! Equal operator. */
  CPositionInfo& operator = (const CPositionInfo &);

  /*! Destructor. */
  ~CPositionInfo ();

  /*! Sets the track number. */
  void setTrack (unsigned track);

  /*! Sets the track duration. */
  void setTrackDuration (QString const & trackDuration);

  /*! Sets the track metadata. */
  void setTrackMetaData (QString const & trackMetaData);

  /*! Sets the track uri. */
  void setTrackURI (QString const & trackURI);

  /*! Sets the track real time. */
  void setRelTime (QString const & relTime);

  /*! Sets the track absolute time. */
  void setAbsTime (QString const & absTime);

  /*! Sets the track real count. */
  void setRelCount (int relCount);

  /*! Sets the track absolute count. */
  void setAbsCount (int absCount);

  /*! Returns the track number. */
  unsigned track () const;

  /*! Returns the track duration. */
  QString const & trackDuration () const;

  /*! Returns the track metadata. */
  QString const & trackMetaData () const;

  /*! Returns the track uri. */
  QString const & trackURI () const;

  /*! Returns the track real time. */
  QString const & relTime () const;

  /*! Returns the track metadata. */
  QString const & absTime () const;

  /*! Returns the track real count. */
  int relCount () const;

  /*! Returns the track absolute count. */
  int absCount () const;

  /*! Converts the track metadata in a CDidlItem. */
  CDidlItem didlItem () const;

private:
  QSharedDataPointer<SPositionInfoData> m_d; //!< Shared data pointer.
};

} //Namespace

#endif // POSITION_INFO_HPP
