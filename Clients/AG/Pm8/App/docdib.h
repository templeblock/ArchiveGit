//////////////////////////////////////////////////////////////////////////////
// $Header: /PM8/App/docdib.h 1     3/03/99 6:04p Gbeddow $
//
//  "This unpublished source code contains trade secrets which are the
//   property of Mindscape, Inc.  Unauthorized use, copying or distribution
//   is a violation of international laws and is strictly prohibited."
// 
//        Copyright © 1999 Mindscape, Inc. All rights reserved.
//
// $Log: /PM8/App/docdib.h $
// 
// 1     3/03/99 6:04p Gbeddow
// 
// 1     1/28/99 6:11p Johno
// Got rid of goofy "CDIBInfo is a member of CPmwDoc (not really)" syntax
// 
// 
//////////////////////////////////////////////////////////////////////////////
#ifndef __HEADER_H__
#define __HEADER_H__

class CDIBInfo
{
public:
	// Constructor.
	CDIBInfo();
	// Destructor.
	~CDIBInfo();

	// Allocate memory.
	void Allocate(int nHeaderSize, DWORD dwDataSize);

	// Free memory.
	void Free();

	// Access the bitmap info.
	BITMAPINFO* BitmapInfo(void)
		{ return m_pBI; }

	// Access the data.
	LPBYTE BitmapData(void)
		{ return m_pData; }
/*
   void
   PrepareData (DWORD Rows, DWORD RowBytes, BOOL BGRtoRGB = FALSE, BOOL RemoveDWORDPad = FALSE);
*/
	LONG
	GetWidth(void);

	LONG
	GetHeight(void);

protected:
	BITMAPINFO* m_pBI;
	LPBYTE m_pData;
};
#endif
