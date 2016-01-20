/*
// $Workfile: ITEMPROX.H $
// $Revision: 1 $
// $Date: 3/03/99 6:06p $
*/

/*
// Revision History:
//
// $Log: /PM8/App/ITEMPROX.H $
// 
// 1     3/03/99 6:06p Gbeddow
// 
//    Rev 1.0   14 Aug 1997 15:21:58   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:34:22   Fred
// Initial revision.
// 
//    Rev 1.4   10 Oct 1996 14:31:20   Jay
// More dynamic graphic/item updating.
// 
//    Rev 1.3   04 Oct 1996 10:44:24   Jay
// Improvements, etc.
// 
//    Rev 1.2   03 Sep 1996 12:34:26   Jay
// Speed ups, etc.
// 
//    Rev 1.1   15 Aug 1996 17:48:06   Jay
// FlushItemData
// 
//    Rev 1.0   14 Mar 1996 13:38:10   Jay
// Initial revision.
// 
//    Rev 1.0   16 Feb 1996 12:34:32   FRED
// Initial revision.
// 
//    Rev 2.6   15 Sep 1995 14:04:52   JAY
// More 32-bit changes.
// 
//    Rev 2.5   18 Jul 1995 08:37:10   JAY
// Added UnlockItemData
// 
//    Rev 2.4   21 Jun 1995 15:50:54   JAY
// New multi-manager code.
// 
//    Rev 2.3   20 Jun 1995 14:16:58   JAY
//  
// 
//    Rev 2.2   13 Jun 1995 08:25:30   JAY
// Shareable album
// 
//    Rev 2.1   08 Feb 1995 13:39:44   JAY
// Reverted. New series.
// 
//    Rev 1.3   11 Nov 1994 13:00:34   JAY
// Moved IO structs to header file from CPP file.
// 
//    Rev 1.2   09 Nov 1994 08:51:24   JAY
// Now caches the bounds and resolution if only a single item.
// 
//    Rev 1.1   07 Nov 1994 16:49:54   JAY
// Init() now returns a BOOL (success or not).
// 
//    Rev 1.0   03 Nov 1994 12:56:38   FRED
//  
// 
//    Rev 1.2   01 Nov 1994 09:59:24   JAY
// New data manager routines
// 
//    Rev 1.1   21 Oct 1994 09:13:06   JAY
// Duplicate, Assign.
// 
//    Rev 1.0   19 Oct 1994 08:13:54   JAY
// Initial revision.
*/ 

/*
// Name: CItemProxy
// Layer: Data
// Function: Represents a book item (or multiple items) within a category.
//  This prevents the need to keep the item data record continuously locked.
//  This piece is an instance of the item data and is particular to the
//  category that owns it (that is, it is not shared).
//
//  Note that all CItemProxy objects are the same. In the current
//  implementation, you should not derive from this class because there is
//  no way to specify the class type during serialization. All specialization
//  should be done at the CItemData level, where each object is a data manager
//  record and can be created via a registered create function.
*/

class CItemProxy;
class CItemProxyState;

#ifndef __ITEMPROX_H__
#define __ITEMPROX_H__

#include "cmanstd.h"

class CItemData;
class CItemDataDescriptor;
class CDrawContext;

struct CItemProxyRecord
{
	DWORD m_dwItemID;
	DWORD m_dwDataID;
	WORD  m_wItemCount;
	// Bounds for the 1 item case (assume we write it in all cases).
	RECTS m_crBounds;
	SIZES m_czResolution;
	WORD m_wDataType;
	DWORD m_dwItemFlags;
};

/*
// This is the higher-level component of a book's items.
*/

class CItemProxy : public CBaseObject
{
public:
	// management

	CItemProxy(CDataManager* pManager);
	virtual ~CItemProxy();

	// implementation

	virtual BOOL Init(const CItemDataDescriptor& Descriptor,
							ITEMID dwItemID = NULL_DATAID,
							BOOL fReset = TRUE);

	// Access optimization.

	virtual CItemData* LockItemData(BOOL fReadOnly = FALSE);
	void UnlockItemData(CItemData* pData, BOOL fReadOnly = FALSE)
		{ ASSERT(pData == m_pItemData); }
	void FlushItemData(void);
	void FlushThumbnail(void);

	// Item data functions

	virtual void Draw(CDrawContext* pContext, const CRect& crContent, int nSubItem);
	virtual int SubItems(void) const;
	virtual CRect Bounds(int nSubItem);
	virtual CSize Resolution(int nSubItem);

	void UpdateFromItem(CItemData* pItemData);

/*
// Notification functions.
*/

public:
	virtual void OnDelete(void);

/*
// Item data id management.
// This is the ID that hooks this item to the lower level item data component.
*/

public:
	DATAID DataID(void) const;
	ITEMID ItemID(void) const;
	DATATYPE DataType(void) const;
	DWORD ItemFlags(void) const;
	void ItemFlags(DWORD dwItemFlags);

/*
// Data manager access.
// A CItemProxy needs a data manager to access its CDataItem.
*/

public:
	CDataManager* Manager(void) const;

/*
// File management.
*/
public:
	virtual ERRORCODE ReadData(StorageDevice* pDevice);
	virtual ERRORCODE WriteData(StorageDevice* pDevice);
	virtual ST_MAN_SIZE SizeofData(StorageDevice* pDevice);

/*
// Duplication support.
*/
public:
	virtual CItemProxy* Duplicate(CDataManager* pDupManager = NULL);
	virtual void Assign(const CItemProxy& Item);

/*
// Miscellaneous.
*/
public:
	void NewItemID(void);
protected:
	CDataManager* m_pManager;
	CItemData* m_pItemData;			// Cached data.

	// ItemProxy data.
	// Some of this used to be in the state (which no longer exists).
	// The rest was separate variables. It has all been combined.

	CItemProxyRecord m_Record;
};

// Inlines

inline DATAID CItemProxy::DataID(void) const
	{ return m_Record.m_dwDataID; }
inline ITEMID CItemProxy::ItemID(void) const
	{ return m_Record.m_dwItemID; }
inline DATATYPE CItemProxy::DataType(void) const
	{ return m_Record.m_wDataType; }
inline DWORD CItemProxy::ItemFlags(void) const
	{ return m_Record.m_dwItemFlags; }
inline void CItemProxy::ItemFlags(DWORD dwItemFlags)
	{ m_Record.m_dwItemFlags = dwItemFlags; }
inline CDataManager* CItemProxy::Manager(void) const
	{ return m_pManager; }

#endif			// __ITEMPROX_H__
