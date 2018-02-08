#ifndef DUMP_HPP
#define DUMP_HPP

#include "using_upnp_namespace.hpp"
#include "upnp_global.hpp"
#include <QObject>

START_DEFINE_UPNP_NAMESPACE

/*! \brief The CDump class provides just a signal with text data.
 * This class can be used to dump text information to a text widget (e.g. QTextEdit).
 * For this, just connect dumpReady signal to the user defined slot.
 * \code
 * QPlainTextEdit* debugInfo = new QPlainTextEdit (this);
 * connect (CDump::dumpObject (), SIGNAL(dumpReady(QString const &)), debugInfo, SLOT(insertPlainText(QString const &)));
 * \endcode
 */
class UPNP_API CDump : public QObject
{
  Q_OBJECT
public:
  /*! Contructor. */
  explicit CDump (QObject* parent = 0);

  /*! Returns the unique dump object. */
  static CDump* dumpObject () { return m_dump; }

  /*! Emits the signal from the unique object.
   * \param text: Data to emit.
   */
  static void dump (QString const & text);

  /*! Emits the signal from the unique object.
   * \param bytes: Data to emit.
   */
  static void dump (QByteArray const & bytes);

signals:
  /*! The signal. */
  void dumpReady (QString const & text);

private :
  static CDump* m_dump; //!< The unique object.
};

} // Namespace

#endif // DUMP_HPP
