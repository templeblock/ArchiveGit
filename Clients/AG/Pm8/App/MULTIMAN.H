/*
// $Workfile: MULTIMAN.H $
// $Revision: 1 $
// $Date: 3/03/99 6:08p $
*/

/*
// Revision History:
//
// $Log: /PM8/App/MULTIMAN.H $
// 
// 1     3/03/99 6:08p Gbeddow
// 
//    Rev 1.0   14 Aug 1997 15:22:44   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:34:40   Fred
// Initial revision.
// 
//    Rev 1.2   03 Sep 1996 12:34:26   Jay
// Speed ups, etc.
// 
//    Rev 1.1   15 Aug 1996 15:57:48   Jay
// Make Build, Split, ... static.
// 
//    Rev 1.0   14 Mar 1996 13:29:04   Jay
// Initial revision.
// 
//    Rev 1.0   16 Feb 1996 12:34:58   FRED
// Initial revision.
// 
//    Rev 1.0   21 Jun 1995 15:51:18   JAY
// Initial revision.
*/

class CMultiDataManager;

#ifndef __MULTIMAN_H__
#define __MULTIMAN_H__

#include "cmanstd.h"

class CMultiDataManager : public CStandardDataManager
{
public:
	CMultiDataManager(StorageDevice* pDevice = NULL, BOOL fIsSharing = FALSE);
	virtual ~CMultiDataManager();

	static void SplitDataId(DATAID Id, DATAID& RealId, int& nManager)
		{ RealId = (Id & 0x00FFFFFFL); nManager = (int)((Id & 0xFF000000L) >> 24); }

	static DATAID BuildDataId(DATAID RealId, int nManager)
		{ return RealId | (((DATAID)nManager) << 24); }

	// Return the number of sub managers.
	int SubManagers(void)
		{ return m_SubManagers.Objects(); }

	// Return the sub manager by index.
	CDataManager* SubManager(int nManager)
		{ return (CDataManager*)m_SubManagers.Object(nManager); }

	// Add a sub manager. Returns the index of the manager.
	int AddSubManager(CDataManager* pManager)
		{ m_SubManagers.Add(pManager); return SubManagers()-1; }

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

	virtual CDataRecord* GetRecord(DATAID number, DATATYPE type = DB_ANY_TYPE, BOOL read_only = FALSE, ERRORCODE* pError = NULL);

	virtual ERRORCODE FlushRecord(DATAID Id);

protected:
	CBaseObjects m_SubManagers;
};

#endif			// __MULTIMAN_H__
