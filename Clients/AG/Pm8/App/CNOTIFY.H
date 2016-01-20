
/*
// $Workfile: CNOTIFY.H $
// $Revision: 1 $
// $Date: 3/03/99 6:04p $
*/

/*
// Revision History:
//
// $Log: /PM8/App/CNOTIFY.H $
// 
// 1     3/03/99 6:04p Gbeddow
// 
//    Rev 1.0   14 Aug 1997 15:19:14   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:33:08   Fred
// Initial revision.
// 
//    Rev 1.0   14 Mar 1996 13:45:28   Jay
// Initial revision.
// 
//    Rev 1.0   16 Feb 1996 12:07:48   FRED
// Initial revision.
// 
//    Rev 2.0   07 Feb 1995 15:48:24   JAY
// Initial revision.
// 
//    Rev 1.0   30 Nov 1994 16:40:06   JAY
// Initial revision.
// 
//    Rev 1.0   03 Nov 1994 12:56:06   FRED
//  
// 
//    Rev 1.2   26 Oct 1994 08:07:00   JAY
// Removed CNotifyTarget since we now have CBaseObject to bind to at notify time.
// 
//    Rev 1.1   25 Oct 1994 16:22:30   JAY
// Now based on CBase object.
// 
//    Rev 1.0   14 Oct 1994 09:31:56   JAY
// Initial revision.
*/ 

class CNotifyItem;
class CNotify;

#ifndef __CNOTIFY_H__
#define __CNOTIFY_H__

/*
// An object capable of receiving a notification.
// This is just a CBaseObject. We don't need a separate CNotifyTarget anymore.
*/

typedef void (CBaseObject::*NOTIFY_FUNCTION)(int nNotify, LPVOID pData);

/*
// A little structure to remember a notify target.
*/

class CNotifyItem
{
public:
	CNotifyItem(CBaseObject* pObject);
	~CNotifyItem();

	CBaseObject*		m_pObject;
	int					m_nNotify;
	NOTIFY_FUNCTION	m_pFunction;
};

/*
// A notification manager.
*/

class CNotify
{
public:
	CNotify();
	~CNotify();

	void RegisterNotify(CBaseObject* pObject, int nNotify, NOTIFY_FUNCTION pFunction);
	void UnregisterNotify(CBaseObject* pObject, int nNotify);
	void DoNotify(int nNotify, LPVOID pData);
protected:
	int m_nNotifyDepth;
	CPtrArray m_cpaItems;
};

#endif			// __CNOTIFY_H__
