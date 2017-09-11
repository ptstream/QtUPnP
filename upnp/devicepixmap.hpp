#ifndef DEVICE_PIXMAP_HPP
#define DEVICE_PIXMAP_HPP

#include "using_upnp_namespace.hpp"
#include "upnp_global.hpp"
#include <QSharedDataPointer>

START_DEFINE_UPNP_NAMESPACE

class SDevicePixmapData;

/*! \brief The CDevicePixmap class holds information about a device image.
 *
 * \remark Use implicit Sharing QT technology.
 */
class UPNP_API CDevicePixmap
{
public:
  /*! Default constructor. */
  CDevicePixmap ();

  /*! Copy constructor. */
  CDevicePixmap (const CDevicePixmap & other);

  /*! Equal operator. */
  CDevicePixmap& operator = (CDevicePixmap const & other);

  /*! Destructor. */
  ~CDevicePixmap ();

  /*! Sets the image url. */
  void setUrl (QString const & url);

  /*! Sets the mime type. */
  void setMimeType (QString mimeType);

  /*! Sets the width. */
  void setWidth (int w);

  /*! Sets the height. */
  void setHeight ( int h);

  /*! Sets the depth. */
  void setDepth (int d);

  /*! Returns the image url. */
  QString const & url () const;

  /*! Returns the mime type. */
  QString const & mimeType () const;

  /*! Returns the width. */
  int width () const;

  /*! Returns the height. */
  int height () const;

  /*! Returns the depth. */
  int depth () const;

  /*! Return the prefered criteria. It is width * height * depth. */
  int preferedCriteria () const;

  /*! Returns true if the image has the mimeTpe. */
  bool hasMimeType (char const * mimeType) const;

   /*! Return the prefered criteria. It is width * height * depth. */
  static int preferedCriteria (int w, int h, int d) { return w * h * d; }

private:
  QSharedDataPointer<SDevicePixmapData> m_d; //!< Shared data pointer.
};

} // Namespace

#endif // DEVICE_PIXMAP_HPP
