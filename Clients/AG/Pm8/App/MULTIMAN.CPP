/*
// $Workfile: MULTIMAN.CPP $
// $Revision: 1 $
// $Date: 3/03/99 6:08p $
*/

/*
// Revision History:
//
// $Log: /PM8/App/MULTIMAN.CPP $
// 
// 1     3/03/99 6:08p Gbeddow
// 
//    Rev 1.0   14 Aug 1997 15:22:42   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:39:16   Fred
// Initial revision.
// 
//    Rev 1.1   03 Sep 1996 12:34:26   Jay
// Speed ups, etc.
// 
//    Rev 1.0   14 Mar 1996 13:33:04   Jay
// Initial revision.
// 
//    Rev 1.0   16 Feb 1996 12:29:36   FRED
// Initial revision.
// 
//    Rev 1.0   21 Jun 1995 15:51:18   JAY
// Initial revision.
*/

#include "stdafx.h"
#include "multiman.h"

/*
// Construction.
*/

CMultiDataManager::CMultiDataManager(StorageDevice* pDevice, BOOL fIsSharing /*=FALSE*/)
								: CStandardDataManager(pDevice, fIsSharing)
{
}

/*
// Destruction.
*/

CMultiDataManager::~CMultiDataManager(void)
{
}

/*
// GetRecord()
//
// Get the desired record. The lock count for the record is incremented.
// (i.e. The record is locked.)
// If the record is loaded from the StorageDevice, the record is considered
// "clean" until dirtied by modified_record().
//
// The type of record expected is also passed so we can do some
// basic integrity checking. If DB_ANY_TYPE is passed as the type (or the
// default argument is used), no type checking is done. ** THIS IS NOT
// RECOMMENDED. ***
//
// This calling sequence differs from the rest (ERRORCODE is not the return
// value), but this simplifies calling it greatly.
*/

CDataRecord* CMultiDataManager::GetRecord(DATAID number, DATATYPE type /*=DB_ANY_TYPE*/, BOOL read_only /*=FALSE*/, ERRORCODE* pError /*=NULL*/)
{
	DATAID RealId;
	int nManager;

	SplitDataId(number, RealId, nManager);

	if (nManager == 0)
	{
	/* Manager 0 is just us. */
		return CStandardDataManager::GetRecord(number, type, read_only, pError);
	}
	else
	{
		nManager--;
		if (nManager >= SubManagers())
		{
			if (pError != NULL)
			{
				*pError = ERRORCODE_InvalidRecord;
			}
			return NULL;
		}
		return SubManager(nManager)->GetRecord(RealId, type, read_only, pError);
	}
}

ERRORCODE CMultiDataManager::FlushRecord(DATAID number)
{
	DATAID RealId;
	int nManager;

	SplitDataId(number, RealId, nManager);

	if (nManager == 0)
	{
	/* Manager 0 is just us. */
		return CStandardDataManager::FlushRecord(number);
	}
	else
	{
		nManager--;
		if (nManager >= SubManagers())
		{
			return ERRORCODE_InvalidRecord;
		}
		return SubManager(nManager)->FlushRecord(RealId);
	}
}
