/*
// $Header: /PM8/App/CRECORD.CPP 1     3/03/99 6:04p Gbeddow $
//
// Database record routines.
//
// $Log: /PM8/App/CRECORD.CPP $
// 
// 1     3/03/99 6:04p Gbeddow
// 
//    Rev 1.0   14 Aug 1997 15:19:36   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:37:50   Fred
// Initial revision.
// 
//    Rev 1.4   07 Oct 1996 16:59:04   Jay
// Got rid of some memory leaks.
// 
//    Rev 1.3   07 Oct 1996 10:50:40   Jay
// Wasn't deleting subcategories and items properly.
// 
//    Rev 1.2   04 Oct 1996 10:43:56   Jay
// Improvements, etc.
// 
//    Rev 1.1   30 Aug 1996 10:45:22   Jay
// Keyword/item stuff
// 
//    Rev 1.0   14 Mar 1996 13:36:06   Jay
// Initial revision.
// 
//    Rev 1.0   16 Feb 1996 12:27:48   FRED
// Initial revision.
// 
//    Rev 2.3   04 Aug 1995 15:19:48   JAY
// Fixed a bug.
// 
//    Rev 2.2   13 Jun 1995 08:25:48   JAY
// Shareable album
// 
//    Rev 2.1   08 Feb 1995 13:36:00   JAY
// Reverted. New series.
// 
//    Rev 1.2   11 Nov 1994 13:01:44   JAY
// Now uses local buffered ReadOnlyFiles to speed up reads.
// 
//    Rev 1.1   09 Nov 1994 08:51:40   JAY
// Preallocates arrays before reading them.
// 
//    Rev 1.0   03 Nov 1994 12:54:10   FRED
//  
// 
//    Rev 1.1   01 Nov 1994 09:58:28   JAY
// New data manager routines
// 
//    Rev 1.0   25 Oct 1994 16:23:40   JAY
// Initial revision.
// 
// *** Incarnation in PMW database ***
//
//    Rev 1.3   09 Jun 1994 11:05:46   JAY
// Added const typedefs for all types.
// Removed initialize().
// Added release_read_only().
// Added zap() method.
// 
//    Rev 1.2   31 May 1994 12:43:20   JAY
// Added a default destroy() method.
// 
//    Rev 1.1   13 Feb 1994 15:23:42   MACDUFF
// Added modification count to database record.
// 
//   Rev 1.0   16 Dec 1993 16:25:24   JAY
//  
*/

#include "stdafx.h"
#include "crecord.h"
#include "cmanstd.h"
#include "file.h"

/////////////////////////////////////////////////////////////////////////////
// CStandardDataRecordState

BEGIN_OBJECT_PROPERTIES(CStandardDataRecordState)
	OBJECT_PROPERTY_INHERIT(CDataRecordState)
END_OBJECT_PROPERTIES()

CStandardDataRecordState::CStandardDataRecordState()
{
}

CStandardDataRecordState::~CStandardDataRecordState()
{
}

void CStandardDataRecordState::Init(void)
{
	CDataRecordState::Init();
}

/////////////////////////////////////////////////////////////////////////////
// CStandardDataRecord

/*
// The constructor for the standard record.
*/

CStandardDataRecord::CStandardDataRecord()
{
	m_nType = 0;
	m_pWhere = NULL;
	m_pManager = NULL;
	m_nModifications = 0;
	m_dwRecordRef = 0;
}

CStandardDataRecord::~CStandardDataRecord()
{
}

BOOL CStandardDataRecord::Init(DATAID record_number,
										 DATATYPE record_type,
										 CStandardDataManager* pManager,
										 LPVOID pData,
										 ST_DEV_POSITION far *in_where)
{
	CDataRecord::Init();		// Any properties.
	StorageElement::Init(ELEMENT_TYPE_RECORD, record_number, pManager->GetStorageManager());

	ASSERT(m_pManager == NULL);		// We don't support multiple Init's yet.

	m_nType = record_type;
	m_pWhere = in_where;
	m_pManager = pManager;

	ASSERT(m_nModifications == 0);
	return TRUE;
}

CDataManager* CStandardDataRecord::Manager(void) const
{
	return m_pManager;
}

/*
// Return the id for this guy. It turns out that the id is the same as the
// one in the storage element so we just pass it up.
*/

DATAID CStandardDataRecord::Id(void) const
{
	return StorageElement::Id();
}

/*
// This Type() is virtual within CDataRecord; the static version of Type()
// (defined in StorageElement) is called when the record is cast to a
// StorageElement (and this returns the *storage* type which isn't useful
// to anyone but the storage manager).
*/

DATATYPE CStandardDataRecord::Type(void) const
{
	return m_nType;
}

/*
// Add a ref to this record.
*/

DWORD CStandardDataRecord::AddRef(void)
{
	m_pManager->AddRefRecord(this);
	return CDataRecord::AddRef();			/* Object's ref count. */
}

/*
// Release this record.
*/

DWORD CStandardDataRecord::Release(void)
{
	m_pManager->ReleaseRecord(this);
	return CDataRecord::Release();			/* Object's ref count. */
}

/*
// Release this record in the database.
// This is like 'release' except it take a 'const' 'this'.
*/

void CStandardDataRecord::ReleaseReadOnly(void) const
{
	m_pManager->ReleaseRecord((CStandardDataRecord*)this, TRUE);
	((CDataRecord*)this)->CDataRecord::Release();						/* Object's ref count. */
}

/*
// Say we're modified.
*/

void CStandardDataRecord::Modified(VOID)
{
	++m_nModifications;
	m_pManager->ModifiedRecord(this);
}

DWORD CStandardDataRecord::RecordAddRef(void)
{
	return ++m_dwRecordRef;
}

DWORD CStandardDataRecord::RecordRelease(void)
{
//	ASSERT(m_dwRecordRef > 0);

	if (m_dwRecordRef > 0)
	{
		--m_dwRecordRef;
	}
	if (m_dwRecordRef == 0)
	{
		return CDataRecord::RecordRelease();
	}
	return m_dwRecordRef;
}

DWORD CStandardDataRecord::RecordRefCount(void) const
{
	return m_dwRecordRef;
}

void CStandardDataRecord::OnDelete(void)
{
/* Default removes all children. */

	int nChildren = Children();

	while (nChildren != 0)
	{
		CDataRecord* pChild = ChildRecord(0);
		if (pChild != NULL)
		{
			DeleteChildRecord(pChild);
			pChild->Release();
		}

		// Be certain that things are decreasing.
		int nNewChildren = Children();
		if (nNewChildren == nChildren)
		{
			// Not decreasing. Break out!
			ASSERT(FALSE);
			break;
		}
		nChildren = nNewChildren;
	}
}

void CStandardDataRecord::OnFlush(void)
{
// Default does nothing.
}

ERRORCODE CStandardDataRecord::ReadData(StorageDevicePtr device)
{
// We call this a "record" even though it's only a single DWORD.
	ERRORCODE error;

	if ((error = device->read_record(&m_dwRecordRef, sizeof(m_dwRecordRef))) == ERRORCODE_None)
	{
	/*
	// Read the children. Each child is saved as its record id. To read the
	// children, we read the each id and lock the child it corresponds to.
	// This means all children are in memory at once.
	*/
		WORD wChildren;

		if ((error = device->read(&wChildren, sizeof(wChildren))) == ERRORCODE_None)
		{
			if (wChildren != 0)
			{
				PreallocateChildren(wChildren);			// Optimization.

				DATAID* pData = NULL;
				TRY
					pData = new DATAID[wChildren];
				END_TRY

				if (pData == NULL)
				{
					error = ERRORCODE_Memory;
				}
				else
				{
					if ((error = device->read(pData, wChildren*sizeof(DATAID))) == ERRORCODE_None)
					{
						for (WORD w = 0; w < wChildren; w++)
						{
							// All children belong to 'this' data manager.

							// Create a stub for this record.
							CDataRecordStub* pStub = new CDataRecordStub;
							pStub->m_idData = pData[w];

							AddChild(pStub);			// Does its own AddRef().
							pStub->Release();			// Release our hold on it
						}
					}
					delete pData;
				}
			}
		}
	}
	return error;
}

ERRORCODE CStandardDataRecord::WriteData(StorageDevicePtr device)
{
	ERRORCODE error;

	if ((error = device->write_record(&m_dwRecordRef, sizeof(m_dwRecordRef))) == ERRORCODE_None)
	{
		WORD wChildren = Children();

		if ((error = device->write(&wChildren, sizeof(wChildren))) == ERRORCODE_None)
		{
			for (WORD w = 0; w < wChildren; w++)
			{
				CDataRecordStub* pStub = (CDataRecordStub*)Child(w);
				ASSERT(pStub != NULL);
				if (pStub != NULL)
				{
					DATAID dwID = pStub->Id();
					if ((error = device->write(&dwID, sizeof(dwID))) != ERRORCODE_None)
					{
						break;
					}
				}
			}
		}
	}
	return error;
}

ST_MAN_SIZE CStandardDataRecord::SizeofData(StorageDevicePtr device)
{
	WORD wChildren = Children();

	return device->size_record(sizeof(m_dwRecordRef))
				+ sizeof(wChildren)
				+ wChildren*sizeof(DATAID);
}
