/*
// $Workfile: ITEMPROX.CPP $
// $Revision: 1 $
// $Date: 3/03/99 6:06p $
*/

/*
// Revision History:
//
// $Log: /PM8/App/ITEMPROX.CPP $
// 
// 1     3/03/99 6:06p Gbeddow
// 
//    Rev 1.0   14 Aug 1997 15:21:56   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:38:56   Fred
// Initial revision.
// 
//    Rev 1.6   10 Oct 1996 14:31:20   Jay
// More dynamic graphic/item updating.
// 
//    Rev 1.5   04 Oct 1996 10:44:22   Jay
// Improvements, etc.
// 
//    Rev 1.4   02 Oct 1996 08:25:50   Jay
// Got rid of a memset().
// 
//    Rev 1.3   03 Sep 1996 12:34:26   Jay
// Speed ups, etc.
// 
//    Rev 1.2   30 Aug 1996 10:45:24   Jay
// Keyword/item stuff
// 
//    Rev 1.1   15 Aug 1996 17:48:06   Jay
// FlushItemData
// 
//    Rev 1.0   14 Mar 1996 13:32:28   Jay
// Initial revision.
// 
//    Rev 1.0   16 Feb 1996 12:29:04   FRED
// Initial revision.
// 
//    Rev 2.9   15 Sep 1995 14:05:02   JAY
// More 32-bit changes.
// 
//    Rev 2.8   05 Sep 1995 10:17:28   JAY
// 'od' if item count is ever not 1 (signaling corruption).
// 
//    Rev 2.7   18 Jul 1995 08:36:44   JAY
// Added UnlockItemData
// 
//    Rev 2.6   21 Jun 1995 15:50:56   JAY
// New multi-manager code.
// 
//    Rev 2.5   20 Jun 1995 14:16:58   JAY
//  
// 
//    Rev 2.4   19 Jun 1995 17:19:14   JAY
// Item extensions
// 
//    Rev 2.3   13 Jun 1995 08:25:44   JAY
// Shareable album
// 
//    Rev 2.2   07 Jun 1995 15:38:30   JAY
// Brought over new versions from CATALOG.
// 
//    Rev 1.4   23 Dec 1994 10:25:26   JAY
// New CDrawException breathing code.
// 
//    Rev 1.3   11 Nov 1994 13:00:46   JAY
// Moved IO structs to header file from CPP file.
// 
//    Rev 1.2   09 Nov 1994 08:51:10   JAY
// Now caches the bounds and resolution if only a single item.
// 
//    Rev 1.1   07 Nov 1994 16:50:06   JAY
// Init() now returns a BOOL (success or not).
// 
//    Rev 1.0   03 Nov 1994 12:54:06   FRED
//  
// 
//    Rev 1.2   01 Nov 1994 09:58:36   JAY
// New data manager routines
// 
//    Rev 1.1   21 Oct 1994 09:13:12   JAY
// Duplicate, Assign.
// 
//    Rev 1.0   19 Oct 1994 08:13:52   JAY
// Initial revision.
*/ 

#include "stdafx.h"
#include "itemdata.h"
#include "itemprox.h"
#include "cdrawcon.h"

/////////////////////////////////////////////////////////////////////////////
// CItemProxy

CItemProxy::CItemProxy(CDataManager* pManager)
{
	m_pManager = pManager;
	m_pItemData = NULL;

	m_Record.m_dwItemID = NULL_ITEMID;
	m_Record.m_dwDataID = NULL_DATAID;
	m_Record.m_wItemCount = 0;
	m_Record.m_wDataType = NULL_DATATYPE;
	m_Record.m_dwItemFlags = 0;
}

CItemProxy::~CItemProxy()
{
	m_pItemData->SafeRelease();
}

BOOL CItemProxy::Init(const CItemDataDescriptor& Descriptor,
							ITEMID dwItemID /*=NULL_ITEMID*/,
							BOOL fReset /*=TRUE*/)
{
/*
// Handle the data item.
// This will create a new item if desired or merely reference an existing one.
*/

	ASSERT(m_pManager != NULL);
	CItemData* pItemData = NULL;

	DATAID DataID;

	if (Descriptor.m_fNewItem)
	{
		ASSERT(Descriptor.m_wDataType != NULL_DATATYPE);
		ASSERT(!Descriptor.m_fTemporaryRef);
//		Descriptor.m_fTemporaryRef = FALSE;		// Make it so, Number One...

		pItemData = (CItemData*)m_pManager->NewRecord(Descriptor.m_wDataType, Descriptor.m_pInitData);
		if (pItemData == NULL)
		{
			// The init failed. Mark ourselves as not initialized.
			m_Record.m_dwDataID = NULL_DATAID;
			return FALSE;
		}

		DataID = pItemData->Id();
	}
	else
	{
	/* Reference an existing record. */
		ASSERT(Descriptor.m_dwDataId != NULL_DATAID);

		if (Descriptor.m_pReferenceProxy == NULL || !Descriptor.m_fTemporaryRef)
		{
			// We need the item.
			pItemData = (CItemData*)m_pManager->GetRecord(Descriptor.m_dwDataId);
			if (pItemData == NULL)
			{
				// The init failed. Mark ourselves as not initialized.
				m_Record.m_dwDataID = NULL_DATAID;
				return FALSE;
			}
			// In the multi-manager case, the id that the record has may not be
			// the same as the one passed in. We MUST use the one passed in to
			// maintain our ability to access it in the future.
		}
		DataID = Descriptor.m_dwDataId;
	}

	// Remember the id for this data.
	m_Record.m_dwDataID = DataID;

	if (pItemData == NULL)
	{
		CItemProxy* pRefProxy = Descriptor.m_pReferenceProxy;
		ASSERT(pRefProxy != NULL);

		m_Record.m_wDataType = pRefProxy->DataType();
		m_Record.m_dwItemFlags = pRefProxy->ItemFlags();

		// Set the sub-item information.
		m_Record.m_wItemCount = pRefProxy->SubItems();

		if (m_Record.m_wItemCount == 1)
		{
		/* Cache the solitary bounds and resolution information. */
			m_Record.m_crBounds = pRefProxy->m_Record.m_crBounds;
			m_Record.m_czResolution = pRefProxy->m_Record.m_czResolution;
		}
	}
	else
	{
		m_Record.m_wDataType = pItemData->Type();
		m_Record.m_dwItemFlags = Descriptor.m_dwItemFlags;

	/* We are another reference for this guy (unless temporary). */

		if (!Descriptor.m_fTemporaryRef)
		{
			pItemData->RecordAddRef();
		}

	/* Set the sub-item information. */

		UpdateFromItem(pItemData);

	/* Bye-bye record! */

		pItemData->Release();
	}

	// Make sure we have a valid item id.

	if (dwItemID == NULL_DATAID)
	{
		NewItemID();
	}
	else
	{
		m_Record.m_dwItemID = dwItemID;
	}
	return TRUE;
}

void CItemProxy::UpdateFromItem(CItemData* pItemData)
{
	m_Record.m_wItemCount = pItemData->SubItems();

	if (m_Record.m_wItemCount == 1)
	{
	/* Cache the solitary bounds and resolution information. */
		CRect crBounds = pItemData->Bounds(0);
		m_Record.m_crBounds.left = (SHORT)crBounds.left;
		m_Record.m_crBounds.top = (SHORT)crBounds.top;
		m_Record.m_crBounds.right = (SHORT)crBounds.right;
		m_Record.m_crBounds.bottom = (SHORT)crBounds.bottom;

		CSize czResolution = pItemData->Resolution(0);
		m_Record.m_czResolution.cx = (SHORT)czResolution.cx;
		m_Record.m_czResolution.cy = (SHORT)czResolution.cy;
	}
}

void CItemProxy::NewItemID(void)
{
	ASSERT(m_pManager != NULL);
	m_Record.m_dwItemID = m_pManager->AllocateItemID(SubItems());
}

/*
// Lock the item for this proxy.
*/

CItemData* CItemProxy::LockItemData(BOOL fReadOnly /*=FALSE*/)
{
	if (m_pItemData == NULL)
	{
//		m_pItemData = (CItemData*)m_pManager->GetRecord(m_Record.m_dwDataID, DB_ANY_TYPE, fReadOnly);
		// We always release it not read-only. So don't get it that way.
		m_pItemData = (CItemData*)m_pManager->GetRecord(m_Record.m_dwDataID, DB_ANY_TYPE);
	}
	return m_pItemData;
}

void CItemProxy::FlushItemData(void)
{
	// Release the data.
	m_pItemData->SafeRelease();
	m_pItemData = NULL;

	// Flush the data.
	DATAID idData = m_Record.m_dwDataID;
	if (idData != NULL_DATAID)
	{
		m_pManager->FlushRecord(idData);
	}
}

void CItemProxy::FlushThumbnail(void)
{
	if (m_pItemData != NULL)
	{
		m_pItemData->OnFlush();
	}
}

#if 0
void CItemProxy::UnlockItemData(CItemData* pItemData)
{
	ASSERT(pItemData == m_pItemData);
/*
// And nothing else to do since we always hang on to it.
// If we were in a sharing environment, we would need to unlock it at this
// point.
*/
}
#endif

/*
// Draw this item.
*/

void CItemProxy::Draw(CDrawContext* pContext, const CRect& crContent, int nSubItem)
{
	ASSERT(m_Record.m_dwDataID != NULL_DATAID);

	if (m_Record.m_dwDataID != NULL_DATAID)
	{
		ASSERT(m_pManager != NULL);

		CItemData* pItemData = LockItemData(TRUE);
		if (pItemData != NULL)
		{
			TRY
			{
				pItemData->Draw(pContext, crContent, nSubItem);
			}
			CATCH(CDrawException, e)
			{
				UnlockItemData(pItemData, TRUE);
				THROW_LAST();
			}
			AND_CATCH_ALL(e)
			{
			}
			END_CATCH_ALL
			UnlockItemData(pItemData, TRUE);
		}
	}
}

int CItemProxy::SubItems(void) const
{
	return m_Record.m_wItemCount;
}

CRect CItemProxy::Bounds(int nSubItem)
{
	CRect crBounds;

	if (m_Record.m_wItemCount == 1)
	{
		crBounds.left = m_Record.m_crBounds.left;
		crBounds.top = m_Record.m_crBounds.top;
		crBounds.right = m_Record.m_crBounds.right;
		crBounds.bottom = m_Record.m_crBounds.bottom;
	}
	else
	{
		CItemData* pItemData = LockItemData(TRUE);
		if (pItemData != NULL)
		{
			crBounds = pItemData->Bounds(nSubItem);
			pItemData->ReleaseReadOnly();
		}
		else
		{
			crBounds.SetRectEmpty();
		}
	}
	return crBounds;
}

CSize CItemProxy::Resolution(int nSubItem)
{
	CSize czSize;

	if (m_Record.m_wItemCount == 1)
	{
		czSize.cx = m_Record.m_czResolution.cx;
		czSize.cy = m_Record.m_czResolution.cy;
	}
	else
	{
		CItemData* pItemData = LockItemData(TRUE);
		if (pItemData != NULL)
		{
			czSize = pItemData->Resolution(nSubItem);
			pItemData->ReleaseReadOnly();
		}
		else
		{
			czSize = CSize(1, 1);
		}
	}
	return czSize;
}

/*
// Delete notification.
*/

void CItemProxy::OnDelete(void)
{
	ASSERT(m_pManager != NULL);
	/*
	// If this item has data, inform the data manager that we no longer need it.
	*/
	if (m_pManager != NULL && m_Record.m_dwDataID != NULL_DATAID)
	{
		m_pManager->DeleteRecord(m_Record.m_dwDataID);
		m_Record.m_dwDataID = NULL_DATAID;
	}
}

ERRORCODE CItemProxy::ReadData(StorageDevice* pDevice)
{
	ERRORCODE error;

	if ((error = pDevice->read(&m_Record, sizeof(m_Record))) == ERRORCODE_None)
	{
	}
	return error;
}

ERRORCODE CItemProxy::WriteData(StorageDevice* pDevice)
{
	ERRORCODE error;

	if ((error = pDevice->write(&m_Record, sizeof(m_Record))) == ERRORCODE_None)
	{
	}
	return error;
}

ST_MAN_SIZE CItemProxy::SizeofData(StorageDevice* pDevice)
{
	ST_MAN_SIZE size = sizeof(m_Record);
	return size;
}

CItemProxy* CItemProxy::Duplicate(CDataManager* pDupManager /*=NULL*/)
{
/* Make sure we have a valid data manager. */

	if (pDupManager == NULL)
	{
		pDupManager = m_pManager;
	}
	ASSERT(pDupManager != NULL);

/* Create the new book item. */

	CItemProxy* pNewProxy;

	if ((pNewProxy = new CItemProxy(pDupManager)) != NULL)
	{
		pNewProxy->Assign(*this);
		pNewProxy->NewItemID();
	}
	return pNewProxy;
}

void CItemProxy::Assign(const CItemProxy& Item)
{
	CDataManager* pSourceManager = Item.Manager();

	m_Record.m_crBounds = Item.m_Record.m_crBounds;
	m_Record.m_czResolution = Item.m_Record.m_czResolution;

	m_Record.m_dwItemID = Item.ItemID();
	m_Record.m_dwDataID = pSourceManager->DuplicateRecord(Item.DataID(), m_pManager);
	m_Record.m_wItemCount = Item.SubItems();
	m_Record.m_wDataType = Item.DataType();
	m_Record.m_dwItemFlags = Item.ItemFlags();
}
