/*
// $Workfile: CDATAMAN.CPP $
// $Revision: 1 $
// $Date: 3/03/99 6:03p $
*/

/*
// Revision History:
//
// $Log: /PM8/App/CDATAMAN.CPP $
// 
// 1     3/03/99 6:03p Gbeddow
// 
//    Rev 1.0   14 Aug 1997 15:18:50   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:37:28   Fred
// Initial revision.
// 
//    Rev 1.2   04 Oct 1996 10:43:56   Jay
// Improvements, etc.
// 
//    Rev 1.1   01 Sep 1996 12:41:36   Jay
// More configurable albums
// 
//    Rev 1.0   14 Mar 1996 13:25:48   Jay
// Initial revision.
// 
//    Rev 1.0   16 Feb 1996 12:27:08   FRED
// Initial revision.
// 
//    Rev 2.5   15 Sep 1995 14:04:58   JAY
// More 32-bit changes.
// 
//    Rev 2.4   16 Aug 1995 08:14:10   JAY
// Fixed remove catagory.
// 
//    Rev 2.3   19 Jun 1995 17:19:14   JAY
// Item extensions
// 
//    Rev 2.2   13 Jun 1995 08:23:04   JAY
// Shareable album
// 
//    Rev 2.1   08 Feb 1995 13:34:26   JAY
// Reverted. New series.
// 
//    Rev 1.0   03 Nov 1994 12:52:52   FRED
//  
// 
//    Rev 1.13   01 Nov 1994 09:59:22   JAY
// New data manager routines
// 
//    Rev 1.12   26 Oct 1994 12:01:16   JAY
// Removing 'WithChildren' stuff.
// 
//    Rev 1.11   26 Oct 1994 10:23:46   JAY
// Removed ULONG. CBaseObjects class.
// 
//    Rev 1.10   25 Oct 1994 16:21:14   JAY
// Release() now returns a ULONG.
// 
//    Rev 1.9   21 Oct 1994 09:14:02   JAY
// DuplicateRecord now Release's the source record which it had gotten.
// 
//    Rev 1.8   19 Oct 1994 16:26:14   JAY
// Does Release() after Delete() in DeleteRecord().
// 
//    Rev 1.7   19 Oct 1994 08:40:22   JAY
// No change.
// 
//    Rev 1.6   19 Oct 1994 08:12:00   JAY
// Restructuring
// 
//    Rev 1.5   14 Oct 1994 09:33:08   JAY
// Notify. Consistent use of AddRef() and Release()
// 
//    Rev 1.4   12 Oct 1994 13:00:52   JAY
// Integration of Commit and Update at CPropertyObject level
// 
//    Rev 1.3   06 Oct 1994 13:18:58   JAY
// CDataRecord is now a property object.
// 
//    Rev 1.2   22 Sep 1994 11:22:24   JAY
// Added Duplicate and Assign methods to CDataRecord.
// Added DuplicateRecord to CDataManager.
// Added AllocateItemID to CDataManager.
// 
// 
//    Rev 1.1   15 Sep 1994 14:47:00   JAY
// Added modified logic.
// Changed CreateRecord() to NewRecord().
// 
//    Rev 1.0   12 Sep 1994 10:47:32   JAY
// Initial revision.
*/

#include "stdafx.h"
#include "cdataman.h"

// Properties

BEGIN_OBJECT_PROPERTIES(CDataRecordState)
END_OBJECT_PROPERTIES()

//////////////////////////////////////////////////////////////////////////
// CDataManager

/*
// The init routine for a generic data manager.
*/

void CDataManager::Init(void)
{
// Default implementation does nothing.
}

/*
// Delete a record given its id.
*/

void CDataManager::DeleteRecord(DATAID id)
{
	if (id != NULL_DATAID)
	{
		CDataRecord* pRecord;

		if ((pRecord = GetRecord(id)) != NULL)
		{
			pRecord->RecordRelease();
			pRecord->Release();
		}
	}
}

CDataRecord* CDataManager::DuplicateRecord(CDataRecord* pRecord, CDataManager* pDupManager /*=NULL*/)
{
	ASSERT(pRecord != NULL);
	return pRecord->Duplicate(pDupManager);
}

DATAID CDataManager::DuplicateRecord(DATAID dwID, CDataManager* pDupManager /*=NULL*/)
{
	CDataRecord* pRecord;
	DATAID dwNewID = NULL_DATAID;

	if (dwID != NULL_DATAID && (pRecord = GetRecord(dwID)) != NULL)
	{
	/* Duplicate the record. */

		CDataRecord* pNewRecord = DuplicateRecord(pRecord, pDupManager);
		if (pNewRecord != NULL)
		{
		/* Grab the id for the new record before releasing. */
			dwNewID = pNewRecord->Id();
			pNewRecord->Release();
		}
		pRecord->Release();
	}
	return dwNewID;
}

//////////////////////////////////////////////////////////////////////////
// CDataRecord

CDataRecord::CDataRecord(void)
{
}


#if 0
BOOL CDataRecord::Init(DATAID id, DATATYPE type, CDataManager* pManager, LPVOID, BOOL fReset /*=TRUE*/)
{
	CPropertyObject::Init(fReset);

	PSet(PID_Id, id);
	PSet(PID_Type, type);

	m_pManager = pManager;
	m_fModified = FALSE;

	return TRUE;
}
#endif

/*
// Delete notification.
// This is called when the data manager is about to delete us.
*/

void CDataRecord::OnDelete(void)
{
/* Default calls OnDelete() for all children. */

	int nChildren = Children();

	for (int i = 0; i < nChildren; i++)
	{
		CDataRecord* pChild = (CDataRecord*)Child(i);
		ASSERT(pChild != NULL);
		if (pChild != NULL)
		{
			pChild->OnDelete();
		}
	}
}

/*
// Default action for deleting a data record.
*/

DWORD CDataRecord::RecordRelease(void)
{
	SelfDestruct();
	return 0;
}

/*
// Default action for releasing a data record.
*/

DWORD CDataRecord::Release(void)
{
	return CPropertyObject::Release();
}

/*
// Default action for releasing a data record (read-only).
*/

DWORD CDataRecord::ReleaseReadOnly(void)
{
	// Ignore read-only-ness.
	return CPropertyObject::Release();
}

/*
// Duplicate a record.
// The caller must Release() the record if the dup is successful.
*/

CDataRecord* CDataRecord::Duplicate(CDataManager* pDupManager /*=NULL*/)
{
	if (pDupManager == NULL)
	{
	/* Keep it within the family. */
		pDupManager = Manager();
	}

	CDataRecord* pNewRecord;

	if ((pNewRecord = pDupManager->NewRecord(Type())) != NULL)
	{
		pNewRecord->Assign(*this);
		pNewRecord->RecordAddRef();
	}

	return pNewRecord;
}

/*
// Base level for CDataRecord assignment.
*/

void CDataRecord::Assign(const CDataRecord& Record)
{
// Base level does nothing.
}

CDataRecord* CDataRecord::ChildRecord(int nIndex, BOOL fReadOnly /*=FALSE*/)
{
// Take a pointer and convert it to a data id.
	CDataRecordStub* pStub = (CDataRecordStub*)Child(nIndex);
	DATAID DataID = pStub->Id();

	CDataManager* pManager = Manager();
	ASSERT(pManager != NULL);

	return pManager->GetRecord(DataID, DB_ANY_TYPE, fReadOnly);
}

CDataRecord* CDataRecord::NewChildRecord(DATATYPE wType, LPVOID pData /*=NULL*/, int nIndex /*=-1*/)
{
	CDataManager* pManager = Manager();
	ASSERT(pManager != NULL);

	CDataRecord* pRecord;

	if ((pRecord = pManager->NewRecord(wType, pData)) != NULL)
	{
		// Create a stub for this record.
		CDataRecordStub* pStub = new CDataRecordStub;
		pStub->m_idData = pRecord->Id();
		AddChild(pStub, nIndex);
		pStub->Release();

		pRecord->RecordAddRef();
		Modified();
	}
	return pRecord;
}

CDataRecord* CDataRecord::FindChildRecord(DATAID dwID, BOOL fReadOnly /*=FALSE*/)
{
	int nCount = Children();
	CDataRecord* pChild = NULL;

	for (int i = 0; i < nCount; i++)
	{
		pChild = ChildRecord(i, fReadOnly);

		if (pChild->Id() == dwID)
		{
			break;
		}

		CDataRecord* pFoundChild = pChild->FindChildRecord(dwID, fReadOnly);
		if (fReadOnly)
		{
			pChild->ReleaseReadOnly();
		}
		else
		{
			pChild->Release();
		}

		if (pFoundChild != NULL)
		{
			pChild = pFoundChild;
			break;
		}
		pChild = NULL;
	}
	return pChild;
}

void CDataRecord::DeleteChildRecord(DATAID dwID)
{
	CDataRecord* pChild;
	if ((pChild = FindChildRecord(dwID)) != NULL)
	{
		DeleteChildRecord(pChild);
		pChild->Release();
	}
}

void CDataRecord::DeleteChildRecord(CDataRecord* pChild)
{
/*
// Inform the child that a persistent reference is going away.
// We need to do this first because RemoveChild does a Release() which
// may cause pChild to become invalid.
*/

	DATAID dwID = pChild->Id();
	pChild->RecordRelease();		/* Delete it from the database. */

	int nCount = Children();

	for (int i = 0; i < nCount; i++)
	{
		CDataRecordStub* pStub = (CDataRecordStub*)Child(i);

		if (pStub->Id() == dwID)
		{
			RemoveChild(pStub);
			break;
		}
	}
}

int CDataRecord::GetChildIndex(DATAID dwID)
{
	int nCount = Children();

	for (int i = 0; i < nCount; i++)
	{
		CDataRecordStub* pStub = (CDataRecordStub*)Child(i);

		if (pStub->Id() == dwID)
		{
			return i;
		}
	}
	return -1;
}

void CDataRecord::MoveChildRecord(DATAID dwID, int nIndex)
{
	if (nIndex != -1)
	{
		int nSourceIndex = GetChildIndex(dwID);
		if (nSourceIndex != -1 && nSourceIndex != nIndex)
		{
			MoveChild(nSourceIndex, nIndex);
		}
	}
}

void CDataRecord::MoveChildRecord(CDataRecord* pChild, int nIndex)
{
	MoveChildRecord(pChild->Id(), nIndex);
}

//////////////////////////////////////////////////////////////////////////
// CDataRecordState

CDataRecordState::CDataRecordState()
{
}

CDataRecordState::~CDataRecordState()
{
}

#if 0
void CDataRecordState::Init(void)
{
// Defaults for properties.
	CPropertyState::Init();
	m_dwId = NULL_DATAID;
	m_wType = NULL_DATATYPE;
}
#endif
