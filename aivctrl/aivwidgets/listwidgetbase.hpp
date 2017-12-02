#ifndef LISTWIDGETBASE_HPP
#define LISTWIDGETBASE_HPP

#include <QListWidget>

/*! \brief CListWidgetBase is the base class of CContentDirectoryBrowser. */
class CListWidgetBase : public QListWidget
{
  Q_OBJECT

public:
  /* Default constructor. */
  CListWidgetBase (QWidget* parent = nullptr);

  /*! Sets the default icons. */
  void setDefaultIcons (QStringList const & files) { m_defaultIcons = files; }

  /*! Returns the default icons. */
  QStringList const & defaultIcons () const { return m_defaultIcons; }

  /*! Set the last selected item row. */
  void setSelectedRow (int row) { m_selectedRow = row; }

  /*! Returns the last selected item row. */
  int selectedRow () const { return m_selectedRow; }

  /*! Returns the last bolded item. */
  int boldIndex () const;

protected slots :
  /*! Saves the row of the selected item. */
  void saveSelected (QListWidgetItem* item);

protected :
  QStringList m_defaultIcons; //!< Default icons.
  int m_selectedRow = -1; //!< The last selected row.
};

#endif // LISTWIDGETBASE_HPP
