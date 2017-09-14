#include "browsereply.hpp"
#include "helper.hpp"
#include <QDebug>

START_DEFINE_UPNP_NAMESPACE

/*! \brief Internal strucure for CBrowse. */
struct SBrowseReplyData : public QSharedData
{
  SBrowseReplyData () {}
  SBrowseReplyData (SBrowseReplyData const & other);

  unsigned m_numberReturned = 0;
  unsigned m_totalMatches = 0;
  unsigned m_updateID = 0;
  QList<CDidlItem> m_items;
};

SBrowseReplyData::SBrowseReplyData (SBrowseReplyData const & other) : QSharedData (other),
                    m_numberReturned (other.m_numberReturned),
                    m_totalMatches (other.m_totalMatches),
                    m_updateID (other.m_updateID), m_items (other.m_items)
{
}

}//Namespace

USING_UPNP_NAMESPACE

CBrowseReply::CBrowseReply () : m_d (new SBrowseReplyData)
{
}

CBrowseReply::CBrowseReply (CBrowseReply const & other) : m_d (other.m_d)
{
}

CBrowseReply& CBrowseReply::operator = (CBrowseReply const & other)
{
  if (this != &other)
  {
    m_d.operator = (other.m_d);
  }

  return *this;
}

CBrowseReply::~CBrowseReply ()
{
}

void CBrowseReply::setNumberReturned (unsigned numberReturned)
{
  m_d->m_numberReturned = numberReturned;
}

void CBrowseReply::setTotalMatches (unsigned totalMatches)
{
  m_d->m_totalMatches = totalMatches;
}

void CBrowseReply::setUpdateID (unsigned updateID)
{
  m_d->m_updateID = updateID;
}

void CBrowseReply::setItems (QList<CDidlItem> const & items)
{
  m_d->m_items = items;
}

unsigned CBrowseReply::numberReturned () const
{
  return m_d->m_numberReturned;
}

unsigned CBrowseReply::totalMatches () const
{
  return m_d->m_totalMatches;
}

unsigned CBrowseReply::updateID () const
{
  return m_d->m_updateID;
}

QList<CDidlItem> const & CBrowseReply::items () const
{
  return m_d->m_items;
}

QList<CDidlItem>& CBrowseReply::items ()
{
  return m_d->m_items;
}

CBrowseReply& CBrowseReply::operator += (CBrowseReply const & other)
{
  m_d->m_numberReturned += other.m_d->m_numberReturned;
  m_d->m_totalMatches   += other.m_d->m_totalMatches;
  m_d->m_updateID        = other.m_d->m_updateID;
  m_d->m_items          += other.m_d->m_items;
  return *this;
}

QList<CDidlItem> CBrowseReply::search (QList<CDidlItem> const & items, QString text, int returned,
                                       int commonPrefixLength)
{
  typedef QPair<float, int> TDistance;

  QString simplifiedTitle;
  simplifiedTitle.reserve (100); // To minimize reallocation.
  QList<CDidlItem> results;
  if (!text.isEmpty ())
  {
    int   cItems      = items.size ();
    float distanceMax = 0.0f;
    for (int iItem = 0; iItem < cItems; ++iItem)
    {
      QString const & title = items[iItem].title ();
      float length          = static_cast<float>(title.length () + 1);
      distanceMax           = std::max (distanceMax, length);
    }

    if (returned <= 0 || returned > cItems)
    {
      returned = cItems;
    }

    text = concatenateWords (text);
    QVector<TDistance> distances (cItems);
    int                cMatches = 0;
    // Pass 1. Search exact matches.
    for (int iItem = 0; iItem < cItems; ++iItem)
    {
      float distance  = 0.0f;
      simplifiedTitle = ::concatenateWords (items[iItem].title ());
      int index       = simplifiedTitle.indexOf (text);
      if (index != -1)
      {
        distance += distanceMax - static_cast<float>(index);
        distances[iItem] = TDistance (distance, iItem);
        ++cMatches;
      }
    }

    // Pass 2. Compute the Jaro Winkler distance
    if (cMatches < returned)
    {
      for (int iItem = 0; iItem < cItems; ++iItem)
      {
        if (distances[iItem].first == 0.0f)
        {
          simplifiedTitle  = concatenateWords (items[iItem].title ());
          float distance   = jaroWinklerDistance (text, simplifiedTitle, commonPrefixLength);
          distances[iItem] = TDistance (distance, iItem);
        }
      }
    }

    // Sort the distances.
    std::sort (distances.begin (), distances.end (),
               [] (TDistance const & d1, TDistance const & d2) { return d1.first > d2.first; });

    results.reserve (returned);
    for (int iItem = 0; iItem < returned; ++iItem)
    {
      results.push_back (items[distances[iItem].second]);
    }
  }

  return results;
}

QList<CDidlItem> CBrowseReply::search (QString text, int returned, int commonPrefixLength) const
{
  return search (m_d->m_items, text, returned, commonPrefixLength);
}

QStringList CBrowseReply::sortCapabilities (bool sameContent) const
{
  return sortCapabilities (*this, sameContent);
}

QStringList CBrowseReply::sortCapabilities (CBrowseReply const & reply, bool sameContent)
{
  QStringList caps;
  for (CDidlItem const & item : reply.m_d->m_items)
  {
    for (QMultiMap<QString, CDidlElem>::const_iterator ite = item.elems ().cbegin (), ende = item.elems ().cend (); ite != ende; ++ite)
    {
      QString const name = ite.key ();
      if (!ite.value ().isEmpty ())
      {
        QStringList::const_iterator end = caps.cend ();
        if (std::find (caps.cbegin (), end, name) == end)
        {
          caps.push_back (name);
          TMProps const & props = (*ite).props ();
          for (TMProps::const_iterator itp = props.cbegin (), endp = props.cend (); itp != endp; ++itp)
          {
            if (!itp.value ().isEmpty ())
            {
              caps.push_back (name + '@' + itp.key ());
            }
          }
        }
      }
    }

    if (sameContent)
    {
      break;
    }
  }

  return caps;
}

void CBrowseReply::sort (CBrowseReply& reply, QString const & criteria, ESortDir dir)
{
  enum EComparType { String, Numerical, Time };

  QString elemName, propName;
  int     index = criteria.indexOf ('@');
  if (index != -1)
  {
    elemName = criteria.left (index);
    propName = criteria.right (criteria.length () - index - 1);
  }
  else
  {
    elemName = criteria;
  }

  EComparType       type = String;
  QString           value;
  QList<CDidlItem>& items = reply.m_d->m_items;
  for (int iItem = 0, end = items.size (); iItem < end; ++iItem)
  {
    CDidlElem const & elem = items[iItem].value (elemName);
    value                  = index != -1 ? elem.props ().value (propName) : elem.value ();
    if (!value.isEmpty ())
    {
      bool ok;
      value.toLongLong (&ok);
      type = !ok && ::isDuration (value) ? Time : Numerical;
      break;
    }
  }

  if (!value.isEmpty ())
  {
    int const kmax = 15;
    QString value;
    if (type != String)
    {
      value.resize (kmax);
    }

    typedef QPair<QString, CDidlItem> TSortItem;
    QVector<TSortItem> itemArray (items.size ());
    for (int iItem = 0, end = items.size (); iItem < end; ++iItem)
    {
      CDidlElem const & elem = items[iItem].value (elemName);
      value                  = index != -1 ? elem.props ().value (propName) : elem.value ();
      if (type != String)
      {
        int k = kmax - 1;
        for  (int i = value.length () - 1; i >= 0; --i, --k)
        {
          value[k] = value[i];
        }

        for (; k >= 0; --k)
        {
          value[k] = '0';
        }
      }

      itemArray[iItem] = TSortItem (value, items[iItem]);
    }

    auto lt = [] (TSortItem const & s1, TSortItem const & s2) -> bool { return s1.first < s2.first; };
    auto gt = [] (TSortItem const & s1, TSortItem const & s2) -> bool { return s1.first > s2.first; };
    std::sort (itemArray.begin (), itemArray.end (), dir ? gt : lt);

    for (int iItem = 0, end = items.size (); iItem < end; ++iItem)
    {
      items[iItem] = itemArray[iItem].second;
    }
  }
}

void CBrowseReply::sort (QString const & criteria, ESortDir dir)
{
  return sort (*this, criteria, dir);
}

CBrowseReply operator + (CBrowseReply const & r1, CBrowseReply const & r2)
{
  CBrowseReply sum = r1;
  sum             += r2;
  return sum;
}

QStringList CBrowseReply::dump ()
{
  QStringList texts;
  texts << QString ("numberReturned=%1,totalMatches=%2,updateID=%3").arg (m_d->m_numberReturned)
                                            .arg (m_d->m_totalMatches).arg (m_d->m_updateID);
  QList<CDidlItem> const & items = m_d->m_items;
  for (int iItem = 0; iItem < items.size (); ++iItem)
  {
    texts += QString ("%1-----------------------").arg (iItem);
    texts += items[iItem].dump ();
  }

  return texts;
}

void CBrowseReply::debug ()
{
  QStringList texts = dump ();
  for (QString const & text : texts)
  {
    qDebug () << text;
  }
}
