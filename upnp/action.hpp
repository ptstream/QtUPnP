#ifndef ACTION_HPP
#define ACTION_HPP

#include "argument.hpp"
#include <QSharedDataPointer>
#include <QMap>

START_DEFINE_UPNP_NAMESPACE

/*! The arguments map.
 * \param QString: Argument name.
 * \param CArgument: The argument.
 */
typedef QMap<QString, CArgument> TMArguments;

/*! Forward CAction data. */
struct SActionData;

/*! \brief The CAction class holds information about an UPnP action.
 *
 * It contains the map of arguments.
 *
 * \remark Use implicit Sharing QT technology.
 */
class UPNP_API CAction
{
public:
  /*! Default constructor. */
  CAction ();

  /*! Copy constructor. */
  CAction (CAction const & rhs);

  /*! Equal operator. */
  CAction & operator = (CAction const & other);

  /*! Destructor. */
  ~CAction ();

  /*! Sets the argument map. */
  void setArguments (TMArguments const & args);

  /*! Return the arguments map as a constant reference. */
  TMArguments const & arguments () const;

  /*! Return the arguments map as a constant reference. */
  TMArguments& arguments ();

private:
  QSharedDataPointer<SActionData> m_d; //!< Shared data pointer.
};

} // Namespace

#endif // ACTION_HPP
