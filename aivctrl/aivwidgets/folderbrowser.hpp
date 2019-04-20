#ifndef FOLDERBROWSER_HPP
#define FOLDERBROWSER_HPP

#include <QLineEdit>

#include <QStack>
#include <QSharedDataPointer>

class CListWidgetBase;
struct SFolderItemData;

/*! \brief This class is the items of CFolderBrowser. */
class CFolderItem
{
public:
  /*! Default constructor. */
  CFolderItem ();

  /*! Constructor. */
  CFolderItem (int folderIndex, QString const & name,
               QString const & parentID =  QString (), QString const & id =  QString (),
               CListWidgetBase* listWidget = nullptr);
  /*! Copy constructor. */
  CFolderItem (CFolderItem const & rhs);

  /*! Equal operator. */
  CFolderItem& operator = (CFolderItem const & rhs);

  /*! Destructor. */
  ~CFolderItem ();

  /*! Sets the stacked widget index of the current folder. */
  void setStackedWidgetIndex (int index);

  /*! Sets the upnp identifier of the current folder. */
  void setID (QString const & id);

  /*! Sets the upnp parent identifier of the current folder. */
  void setParentID (QString const & id);

  /*! Set selected row of the current folder. */
  void setRow (int row);

  /*! Sets the name of the current folder. */
  void setName (QString const & name);

  /*! Sets the list widget item of the current folder. */
  void setListWidget (CListWidgetBase* listWidget);

  /*! Returns the stacked widget index of the current folder. */
  int stackedWidgetIndex () const;

  /*! Returns the upnp identifier of the current folder. */
  QString const & id () const;

  /*! Returns the upnp parent identifier of the current folder. */
  QString const & parentID () const;

  /*! Returns selected row of the current folder. */
  int row () const;

  /*! Returns the name of the current folder. */
  QString const & name () const;

  /*! Returns the list widget item of the current folder. */
  CListWidgetBase* listWidget () const;

  /*! Select and scroll to list widget item of the current folder. */
  void setListWidgetCurrentItem () const;

private:
  QSharedDataPointer<SFolderItemData> m_d; // Data.
};

/*! \brief The class CFolderItem provides functionalities to show folder titles in a single line.
 *
 * This widget is similar at access bar of the Windows explorer. It is used to show the list of folders of a
 * server during the browse action invokation.
 */
class CFolderBrowser : public QLineEdit
{
  Q_OBJECT
public:
  /*! Default constructor. */
  CFolderBrowser (QWidget* parent = nullptr);

  /*! Push a CFolderItem on the stack.
   * \param index: Stacked widget page index.
   * \param name: Name to show.
   * \param parentID: UPnP parent identifier.
   * \param id: UPnP identifier.
   * \param listWidget: The current QListWidget.
   */
  void push (int index, QString name =  QString (), QString const & parentID = nullptr,
             QString const & id =  QString (), CListWidgetBase* listWidget = nullptr);

  /*! Pop a CFolderItem on the stack. */
  CFolderItem pop ();

  /*! Returns the CFolderItem at the top of the stack. */
  CFolderItem const & top () const { return m_items.top (); }

  /*! Returns the stack size. */
  int stackSize () const  { return m_items.size (); }

  /*! Removes all elements of the stack. */
  void clearStack ();

  /*! Removes all the items from begin up to (but not including) end.
   * Returns true in case of success.
   */
  bool erase (int start, int end = -1);

  /*! Returns true in case of empty stack. */
  bool isEmpty () const  { return m_items.isEmpty (); }

  /*! Updates the text of the QLineEdit. */
  void updateText ();

  /*! Returns the separator of titles. */
  QString const & separator () const { return m_separator; }

protected :
  /*! Finds of title from a position (generally the cursor position).
   * \param start: The first character of the folder.
   * \param start: The last character of the folder.
   * \param position: The x coordinate.
   * \return True if the title folder isfound.
   */
  bool findFolder (int& start, int& end, int pos);

protected :
  /*! To select the folder under the cursor. */
  virtual void enterEvent (QEvent *);

  /*! To unselect the folder under the cursor. */
  virtual void leaveEvent (QEvent*);

  /*! To select or unselect the folder under the cursor. */
  virtual void mouseMoveEvent (QMouseEvent* event);

  /*! To emit indexSelected at the cursor position. */
  virtual void mouseReleaseEvent (QMouseEvent* event);

signals :
  /*! Emitted when clicked. */
  void indexSelected (int index);

protected :
  QStack<CFolderItem> m_items; //!< Stack of folder titles.
  QString const m_separator = " > "; //!< Default separator.
  QChar const m_slash = QLatin1Char ('/'); //!< Slash before first folder.
  bool m_selectedFolder = false; //!< A folder is selected.
};


#endif // FOLDERBROWSER_HPP
