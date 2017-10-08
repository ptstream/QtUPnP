#ifndef SERVICE_HPP
#define SERVICE_HPP

#include "statevariable.hpp"
#include "action.hpp"
#include <QSharedDataPointer>
#include <QMap>

START_DEFINE_UPNP_NAMESPACE

/*! The state variables map.
 * \param QString: The variable name.
 * \param CStateVariable: The state variable.
 */
typedef QMap<QString, CStateVariable> TMStateVariables;

/*! The actions map.
 * \param QString: The variable name.
 * \param CAction: The action
 */
typedef QMap<QString, CAction> TMActions;

/*! The forward declaration of CService data. */
class SServiceData;

/*! \brief The CService class holds information about a service being advertised or found by a control point.
 *
 * It maintains a list of actions and state variables.
 *
 * \remark Use implicit Sharing QT technology.
 */
class UPNP_API CService
{
public:
  /*! Default constructor. */
  CService ();

  /*! Copy constructor. */
  CService (CService const & other);

  /*! Equal operator. */
  CService& operator = (CService const & other);

  /*! Destructor. */
  ~CService ();

  /*! Sets the service type. */
  void setServiceType (QString const & type);

  /*! Sets the control url. */
  void setControlURL (QString const & );

  /*! Sets the event suburl. */
  void setEventSubURL (QString const & url);

  /*! Sets the SCP url. */
  void setScpdURL (QString const & url);

  /*! Sets the Subsscribe SID. */
  void setSubscribeSID (QString const & id);

  /*! Sets the state variables. */
  void setStateVariables (TMStateVariables const &stateVariables);

  /*! Sets the actions. */
  void setActions (TMActions const & actions);

  /*! Sets the instance identifiers. */
  void setInstanceIDs (QVector<unsigned> const & ids);

  /*! Sets the major version. */
  void setMajorVersion (unsigned version);

  /*! Sets the minor version. */
  void setMinorVersion (unsigned version);

  /*! Sets the evented. A service is evented if at least one state variable is evented. */
  void setEvented (bool evented);

  /*! Returns the service type. */
  QString const & serviceType () const;

  /*! Returns the control url. */
  QString const & controlURL () const;

  /*! Returns the event suburl. */
  QString const & eventSubURL () const;

  /*! Returns the SCP url. */
  QString const & scpdURL () const;

  /*! Returns the Subsscribe SID. */
  QString const & subscribeSID () const;

  /*! Returns the state variables. */
  TMStateVariables const & stateVariables () const;

  /*! Returns the state variables. */
  TMStateVariables& stateVariables ();

  /*! Returns the actions. */
  TMActions const & actions () const;

  /*! Sets the actions. */
  TMActions& actions ();

  /*! Returns the instance identifiers. */
  QVector<unsigned> const & instanceIDs () const;

  /*! Returns the major version. */
  unsigned majorVersion () const;

  /*! Returns the minor version. */
  unsigned minorVersion () const;

  /*! Returns rhe highest supported version of the device. */
  unsigned highestSupportedVersion () const;

  /*! Returns the evented. A service is evented if at least one state variable is evented. */
  bool isEvented () const;

  /*! Clear the SID. */
  void clearSID ();

  /*! Parse the xml data. */
  bool parseXml (QByteArray const & data);

  /*! Returns the state variable.
   * \param name: The variable name.
   * \return The variable.
   */
  CStateVariable stateVariable (QString const & name) const;

private:
  QSharedDataPointer<SServiceData> m_d; //!< Shared data pointer.
};

} // Namespace

#endif // SERVICE_HPP
