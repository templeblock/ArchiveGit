//////////////////////////////////////////////////////////////////////////////
// $Header: /PM8/App/UUENCODE.H 1     3/03/99 6:13p Gbeddow $
//
//  "This unpublished source code contains trade secrets which are the
//   property of Mindscape, Inc.  Unauthorized use, copying or distribution
//   is a violation of international laws and is strictly prohibited."
// 
//        Copyright © 1998 Mindscape, Inc. All rights reserved.
//
// $Log: /PM8/App/UUENCODE.H $
// 
// 1     3/03/99 6:13p Gbeddow
// 
// 2     10/05/98 3:03p Jayn
// More content manager cleanup.
// 
// 1     10/05/98 2:33p Jayn
//
//////////////////////////////////////////////////////////////////////////////

#ifndef __UUENCODE_H__
#define __UUENCODE_H__

/////////////////////////////////////////////////////////////////////////////
// CUUEncodeBuffer object

class CUUEncodeBuffer
{
public:
	CUUEncodeBuffer();
	~CUUEncodeBuffer();
	PBYTE GetPtr(void) { return m_pBuf; }
	BOOL Resize(DWORD cNewL);
protected:
	PBYTE m_pBuf;
	DWORD m_cLen;
};

/////////////////////////////////////////////////////////////////////////////
// CUUEncoder object
// uuencodes and decodes

class CUUEncoder
{
public:
	BOOL Encode( BYTE *   bufin, DWORD    nbytes, CUUEncodeBuffer * pbuffEncoded );
	BOOL Decode( char   * bufcoded, CUUEncodeBuffer * pbuffdecoded, DWORD  * pcbDecoded );

	CString EncodeString(LPCSTR p);
	CString DecodeString(LPCSTR p);
};

#endif
