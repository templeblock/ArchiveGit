/*
// $Workfile: ITEMDATA.H $
// $Revision: 1 $
// $Date: 3/03/99 6:06p $
*/

/*
// Revision History:
//
// $Log: /PM8/App/ITEMDATA.H $
// 
// 1     3/03/99 6:06p Gbeddow
// 
//    Rev 1.0   14 Aug 1997 15:21:56   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:34:22   Fred
// Initial revision.
// 
//    Rev 1.1   03 Sep 1996 12:33:26   Jay
// Speed ups, etc.
// 
//    Rev 1.0   14 Mar 1996 13:38:08   Jay
// Initial revision.
// 
//    Rev 1.0   16 Feb 1996 12:34:30   FRED
// Initial revision.
// 
//    Rev 2.4   07 Jul 1995 16:13:20   JAY
// Include rearranging; document saving with album.
// 
//    Rev 2.3   21 Jun 1995 15:50:56   JAY
// New multi-manager code.
// 
//    Rev 2.2   19 Jun 1995 17:19:20   JAY
// Item extensions
// 
//    Rev 2.1   08 Feb 1995 13:39:42   JAY
// Reverted. New series.
// 
//    Rev 1.0   03 Nov 1994 12:56:36   FRED
//  
// 
//    Rev 1.8   01 Nov 1994 09:59:24   JAY
// New data manager routines
// 
//    Rev 1.7   19 Oct 1994 08:12:52   JAY
// Restructuring
// 
//    Rev 1.6   10 Oct 1994 17:43:34   JAY
//  
// 
//    Rev 1.5   06 Oct 1994 13:19:04   JAY
// CDataRecord is now a property object.
// 
//    Rev 1.4   22 Sep 1994 14:00:38   JAY
// Special override of Duplicate() to handle reference counts.
// 
//    Rev 1.3   22 Sep 1994 11:23:36   JAY
// Added Reset() method to CBookItemDataDescriptor.
// 
//    Rev 1.2   20 Sep 1994 14:11:14   JAY
// Implemented 'generated items' for new scheme.
// 
//    Rev 1.1   20 Sep 1994 11:59:50   JAY
// Restructuring to enable load and save to work.
// 
//    Rev 1.0   12 Sep 1994 10:46:34   JAY
// Initial revision.
*/

/*
// Name: CItemData
// Layer: Data Manager
// Function: Data for a book item (or items) which can exist in categories.
//  This data can exist in more than one category (that is, it can be shared),
//  so it maintains a reference count.
*/

class CItemData;
class CItemDataDescriptor;

#ifndef __ITEMDATA_H__
#define __ITEMDATA_H__

#include "cmanstd.h"

class CDrawContext;
class CItemProxy;

/*
// Some basic sorting keys.
*/

enum
{
	ITEM_SORT_None,				// No sorting.
	ITEM_SORT_Name					// Sorting by name.
};

/*
// A descriptor for a book's item data.
*/

class CItemDataDescriptor
{
public:
	CItemDataDescriptor();

	void Reset(void);

	void CreateItem(DATATYPE dwDataType, LPVOID m_pInitData = NULL);
	void ReferenceItem(DATAID dwDataId);
	void TemporaryReferenceItem(DATAID dwDataId, CItemProxy* pReferenceProxy = NULL);

	BOOL m_fNewItem;
	BOOL m_fTemporaryRef;
	BOOL m_fUnique;

	// If m_fNewItem == FALSE.
	DATAID m_dwDataId;
	CItemProxy* m_pReferenceProxy;

	// If m_fNewItem == TRUE.
	DATATYPE m_wDataType;
	LPVOID m_pInitData;
	DWORD m_dwItemFlags;
};

class CItemDataState : public CStandardDataRecordState
{
/*
// Construction/destruction.
*/

public:
	CItemDataState();
	virtual ~CItemDataState();

	// implementation

	virtual void Init(void);

	enum
	{
		PID_Last				= CStandardDataRecordState::PID_Last
	};

	DECLARE_OBJECT_PROPERTIES(CItemDataState)
//	DEFINE_OBJECT_PROPERTY_COPY()
public:
};

/*
// This is the base class for any item data class.
// An item data object holds data for an item object.
*/

class CItemData : public CStandardDataRecord
{
/*
// Construction/destruction.
*/

public:
	CItemData();
	virtual ~CItemData();

/*
// General utility.
*/

public:
	virtual void Draw(CDrawContext* pContext, const CRect& crContent, int nSubItem);

/*
// Properties.
*/
public:
	DEFINE_PROPERTY_STATE(CItemDataState)

/*
// Duplication portion.
*/

public:
	virtual CDataRecord* Duplicate(CDataManager* pDupManager = NULL);
	virtual void Assign(const CDataRecord& Record);

/*
// Book item data access.
*/

public:
	virtual int SubItems(void) const = 0;
	virtual CRect Bounds(int nSubItem) const = 0;
	virtual CSize Resolution(int nSubItem) const = 0;
/*
// Sorting support.
// General mechanism. Keys defined elsewhere.
*/
public:
	virtual BOOL SortKey(int nKey, CString& csKey);
};

#endif			// __ITEMDATA_H__
