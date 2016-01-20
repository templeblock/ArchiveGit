/*
// Text import classes
//
// $Header: /PM8/App/TIMPORT.H 1     3/03/99 6:12p Gbeddow $
//
// $Log: /PM8/App/TIMPORT.H $
// 
// 1     3/03/99 6:12p Gbeddow
// 
//    Rev 1.0   14 Aug 1997 15:26:36   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:36:18   Fred
// Initial revision.
// 
//    Rev 1.0   24 Dec 1996 12:06:52   Jay
// Initial revision.
*/

class CFileRTFSource;

#ifndef __TIMPORT_H__
#define __TIMPORT_H__

#include "rtfparse.h"

class StorageFile;

enum
{
	TEXTTYPE_Unknown	= -1,
	TEXTTYPE_Plain		= 0,
	TEXTTYPE_RTF,
};

/////////////////////////////////////////////////////////////////////////////
// CFileRTFSource

class CFileRTFSource : public CRTFSource
{
public:
	CFileRTFSource(StorageFile* pFile);
	virtual ~CFileRTFSource();

	// Functions.
	virtual int GetChar(void);
	virtual void UnGetChar(int nChar);

	// Variable data.
	StorageFile* m_pFile;
};

#endif
