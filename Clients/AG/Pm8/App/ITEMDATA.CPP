/*
// $Workfile: ITEMDATA.CPP $
// $Revision: 1 $
// $Date: 3/03/99 6:06p $
*/

/*
// Revision History:
//
// $Log: /PM8/App/ITEMDATA.CPP $
// 
// 1     3/03/99 6:06p Gbeddow
// 
//    Rev 1.0   14 Aug 1997 15:21:56   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:38:56   Fred
// Initial revision.
// 
//    Rev 1.1   03 Sep 1996 12:33:26   Jay
// Speed ups, etc.
// 
//    Rev 1.0   14 Mar 1996 13:32:26   Jay
// Initial revision.
// 
//    Rev 1.0   16 Feb 1996 12:29:02   FRED
// Initial revision.
// 
//    Rev 2.4   07 Jul 1995 16:13:14   JAY
// Include rearranging; document saving with album.
// 
//    Rev 2.3   21 Jun 1995 15:50:56   JAY
// New multi-manager code.
// 
//    Rev 2.2   19 Jun 1995 17:19:18   JAY
// Item extensions
// 
//    Rev 2.1   08 Feb 1995 13:37:40   JAY
// Reverted. New series.
// 
//    Rev 1.0   03 Nov 1994 12:54:02   FRED
//  
// 
//    Rev 1.9   01 Nov 1994 10:00:20   JAY
// New data manager routines
// 
//    Rev 1.8   19 Oct 1994 16:24:06   JAY
// Doesn't do the Release() anymore in the Delete().
// 
//    Rev 1.7   19 Oct 1994 08:12:02   JAY
// Restructuring
// 
//    Rev 1.6   10 Oct 1994 17:43:02   JAY
//  
// 
//    Rev 1.5   06 Oct 1994 13:18:58   JAY
// CDataRecord is now a property object.
// 
//    Rev 1.4   22 Sep 1994 14:00:26   JAY
// Special override of Duplicate() to handle reference counts.
// 
//    Rev 1.3   22 Sep 1994 11:23:52   JAY
// Added Reset() method to CBookItemDataDescriptor.
// 
//    Rev 1.2   20 Sep 1994 14:11:14   JAY
// Implemented 'generated items' for new scheme.
// 
//    Rev 1.1   20 Sep 1994 11:59:48   JAY
// Restructuring to enable load and save to work.
// 
//    Rev 1.0   12 Sep 1994 10:46:36   JAY
// Initial revision.
*/

#include "stdafx.h"
#include "itemdata.h"
#include "cdrawcon.h"

//////////////////////////////////////////////////////////////////////////
// CItemData

CItemData::CItemData()
{
}

CItemData::~CItemData()
{
}

void CItemData::Draw(CDrawContext* pContext, const CRect& crContent, int nSubItem)
{
	TRACE("CItemData without Draw() method\n");
	CDC* pDC = pContext->GetDC();

	CBrush cbDummy(RGB(255,128,64));
	pDC->FillRect(&crContent, &cbDummy);

	pContext->ReleaseDC();
}

/*
// Duplicate a record.
// The caller must Release() the record if the dup is successful.
*/

CDataRecord* CItemData::Duplicate(CDataManager* pDupManager /*=NULL*/)
{
	if (pDupManager == NULL)
	{
	/* Keep it within the family. */
		pDupManager = m_pManager;
	}

	if (pDupManager == m_pManager)
	{
	/* Same data manager as us. This is just another reference. */
		RecordAddRef();
		AddRef();				// Dual pointers will get two Release's.
		return this;
	}

/* Do the copying duplicate to the other data manager. */

	return CDataRecord::Duplicate(pDupManager);
}

/*
// Assignment for a CItemData object.
*/

void CItemData::Assign(const CDataRecord& Record)
{
	CDataRecord::Assign(Record);
}

BOOL CItemData::SortKey(int nKey, CString& csKey)
{
	return FALSE;
}

//////////////////////////////////////////////////////////////////////
// CItemDataDescriptor

CItemDataDescriptor::CItemDataDescriptor()
{
	Reset();
}

void CItemDataDescriptor::Reset(void)
{
	m_fNewItem = TRUE;
	m_fUnique = FALSE;
	m_fTemporaryRef = FALSE;
	m_dwDataId = NULL_DATAID;
	m_wDataType = NULL_DATATYPE;
	m_dwItemFlags = 0;
	m_pInitData = NULL;
	m_pReferenceProxy = NULL;
}

void CItemDataDescriptor::CreateItem(DATATYPE wDataType,
												 LPVOID pInitData /*=NULL*/)
{
	m_fNewItem = TRUE;
	m_wDataType = wDataType;
	m_pInitData = pInitData;
}

void CItemDataDescriptor::ReferenceItem(DATAID dwDataId)
{
	m_fNewItem = FALSE;
	m_fTemporaryRef = FALSE;
	m_dwDataId = dwDataId;
}

void CItemDataDescriptor::TemporaryReferenceItem(DATAID dwDataId, CItemProxy* pReferenceProxy /*=NULL*/)
{
	m_fNewItem = FALSE;
	m_fTemporaryRef = TRUE;
	m_dwDataId = dwDataId;
	m_pReferenceProxy = pReferenceProxy;
}

//////////////////////////////////////////////////////////////////////
// CItemDataState

// Properties

BEGIN_OBJECT_PROPERTIES(CItemDataState)
	OBJECT_PROPERTY_INHERIT(CDataRecordState)
END_OBJECT_PROPERTIES()

CItemDataState::CItemDataState()
{
}

CItemDataState::~CItemDataState()
{
}

void CItemDataState::Init(void)
{
	CDataRecordState::Init();
}

