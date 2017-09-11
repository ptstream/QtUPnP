#ifndef DEVICE_CAPS_HPP
#define DEVICE_CAPS_HPP

#include "using_upnp_namespace.hpp"
#include "upnp_global.hpp"
#include <QSharedDataPointer>

START_DEFINE_UPNP_NAMESPACE

struct SDeviceCapsData; //!< Data

/*! \brief The CDeviceCaps class holds information about GetDeviceCapabilities action.
 *
 * See http://upnp.org/specs/av/UPnP-av-AVTransport-Service.pdf.
 *
 * \remark Use implicit Sharing QT technology.
 */
class UPNP_API CDeviceCaps
{
public:
  /*! Default constructor. */
  CDeviceCaps ();

  /*! Copy constructor. */
  CDeviceCaps (const CDeviceCaps &);

  /*! Equal operator. */
  CDeviceCaps& operator = (const CDeviceCaps &);

  /*! Destructor. */
  ~CDeviceCaps ();

  /*! Sets the play medias. */
  void setPlayMedias (QStringList const & medias);

  /*! Sets the record media. */
  void setRecMedias (QStringList const & medias);

  /*! Sets the record quality modes. */
  void setRecQualityModes (QStringList const & modes);

  /*! Returns the play medias. */
  QStringList const & playMedias () const;

  /*! QStringList the record media. */
  QStringList const & recMedias () const;

  /*! QStringList the record quality modes. */
  QStringList const & recQualityModes () const;

private:
  QSharedDataPointer<SDeviceCapsData> m_d; //!< Shared data pointer.
};

} //Namespace

#endif // DEVICE_CAPS_HPP
