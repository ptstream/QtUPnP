#ifndef TRANSPORT_SETTINGS_HPP
#define TRANSPORT_SETTINGS_HPP

#include "using_upnp_namespace.hpp"
#include "upnp_global.hpp"
#include <QSharedDataPointer>

START_DEFINE_UPNP_NAMESPACE

struct STransportSettingsData;

/*! \brief The CTransportSettings class holds information about GetTransportSettings action.
 *
 * See http://upnp.org/specs/av/UPnP-av-AVTransport-Service.pdf.
 *
 * \remark Use implicit Sharing QT technology.
 */
class UPNP_API CTransportSettings
{
public:
  /*! Default constructor. */
  CTransportSettings ();

  /*! Copy constructor. */
  CTransportSettings (const CTransportSettings &);

  /*! Equal operator. */
  CTransportSettings& operator = (const CTransportSettings &);

  /*! Destructor. */
  ~CTransportSettings ();

  /*! Sets the play mode. */
  void setPlayMode (QString const & mode);

  /*! Sets the record quality. */
  void setRecQualityMode (QString const & mode);

  /*! Returns the play mode. */
  QString const & playMode () const;

  /*! Returns the record quality. */
  QString const & recQualityMode () const;


private:
  QSharedDataPointer<STransportSettingsData> m_d; //!< Shared data pointer.
};

} //Namespace

#endif // TRANSPORT_SETTINGS_HPP
