
/*
// $Workfile: CNOTIFY.CPP $
// $Revision: 1 $
// $Date: 3/03/99 6:04p $
*/

/*
// Revision History:
//
// $Log: /PM8/App/CNOTIFY.CPP $
// 
// 1     3/03/99 6:04p Gbeddow
// 
//    Rev 1.0   14 Aug 1997 15:19:12   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:37:38   Fred
// Initial revision.
// 
//    Rev 1.0   14 Mar 1996 13:42:38   Jay
// Initial revision.
// 
//    Rev 1.0   16 Feb 1996 12:06:04   FRED
// Initial revision.
// 
//    Rev 2.0   07 Feb 1995 15:47:36   JAY
// Initial revision.
// 
//    Rev 1.0   30 Nov 1994 16:40:06   JAY
// Initial revision.
// 
//    Rev 1.0   03 Nov 1994 12:53:26   FRED
//  
// 
//    Rev 1.3   26 Oct 1994 08:06:58   JAY
// Removed CNotifyTarget since we now have CBaseObject to bind to at notify time.
// 
//    Rev 1.2   25 Oct 1994 16:22:36   JAY
// Now based on CBase object.
// 
//    Rev 1.1   19 Oct 1994 08:11:40   JAY
// Restructuring
// 
//    Rev 1.0   14 Oct 1994 09:31:58   JAY
// Initial revision.
*/ 

#include "stdafx.h"
#include "cnotify.h"

/////////////////////////////////////////////////////////////////////////////
// CNotifyItem

CNotifyItem::CNotifyItem(CBaseObject* pObject)
{
	ASSERT(pObject != NULL);
	m_pObject = pObject;
}

CNotifyItem::~CNotifyItem()
{
}

/////////////////////////////////////////////////////////////////////////////
// CNotify

CNotify::CNotify()
{
	m_nNotifyDepth = 0;
}

CNotify::~CNotify()
{
	int nSize = m_cpaItems.GetSize();

	for (int i = 0; i < nSize; i++)
	{
		CNotifyItem* pItem = (CNotifyItem*)m_cpaItems.GetAt(i);
		if (pItem != NULL)
		{
			m_cpaItems.SetAt(i, NULL);
			delete pItem;
		}
	}
}

void CNotify::RegisterNotify(CBaseObject* pObject, int nNotify, NOTIFY_FUNCTION pFunction)
{
/* Make sure we're clean. */

	UnregisterNotify(pObject, nNotify);

	CNotifyItem* pItem = new CNotifyItem(pObject);
	ASSERT(pItem != NULL);

	if (pItem != NULL)
	{
		pItem->m_nNotify = nNotify;
		pItem->m_pFunction = pFunction;

		m_cpaItems.Add(pItem);

		ASSERT(m_cpaItems.GetSize() < 30);		// Check runaway case.
	}
}

void CNotify::UnregisterNotify(CBaseObject* pObject, int nNotify)
{
	int nSize = m_cpaItems.GetSize();

	for (int i = 0; i < nSize; i++)
	{
		CNotifyItem* pItem = (CNotifyItem*)m_cpaItems.GetAt(i);

		if (pItem != NULL && pItem->m_pObject == pObject && pItem->m_nNotify == nNotify)
		{
		/* Just NULL us out. */

			m_cpaItems.SetAt(i, NULL);
			delete pItem;

		/* Assume there is only one. */
			break;
		}
	}
}

void CNotify::DoNotify(int nNotify, LPVOID pData)
{
/* Nest into the notify. */

	m_nNotifyDepth++;

/* Do the notify. */

	for (int i = 0; i < m_cpaItems.GetSize(); i++)
	{
		CNotifyItem* pItem = (CNotifyItem*)m_cpaItems.GetAt(i);

		if (pItem == NULL)
		{
		/* This guy must have unregistered at some point. */
			if (m_nNotifyDepth == 1)
			{
			/* We are free to remove him now. */
				m_cpaItems.RemoveAt(i--);				/* Garbage collection! */
			}
		}
		else
		{
		/* Do the notify. */
			if (pItem->m_nNotify == nNotify)
			{
				((pItem->m_pObject)->*(pItem->m_pFunction))(nNotify, pData);
			}
		}
	}

/* Done with the notify. */

	m_nNotifyDepth--;
}
