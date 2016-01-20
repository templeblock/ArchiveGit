//////////////////////////////////////////////////////////////////////////////
// $Header: /PM8/Download/imedntfy.h 1     3/03/99 6:15p Gbeddow $
//
//  "This unpublished source code contains trade secrets which are the
//   property of Mindscape, Inc.  Unauthorized use, copying or distribution
//   is a violation of international laws and is strictly prohibited."
// 
//        Copyright © 1998 Mindscape, Inc. All rights reserved.
//
// $Log: /PM8/Download/imedntfy.h $
// 
// 1     3/03/99 6:15p Gbeddow
// 
// 1     10/02/98 7:09p Jayn
// 
// 2     5/31/98 5:19p Hforman
// 
//////////////////////////////////////////////////////////////////////////////

#ifndef __IMEDNOT_H__
#define __IMEDNOT_H__

//// {BCF2DEC5-49D1-11d2-985D-00A0246D4780}
DEFINE_GUID(IID_IMediaPromptNotify, 0xbcf2dec5, 0x49d1, 0x11d2, 0x98, 0x5d, 0x0, 0xa0, 0x24, 0x6d, 0x47, 0x80);

/*
// The media prompt notify interface.
*/

#undef INTERFACE
#define INTERFACE   IMediaPromptNotify

DECLARE_INTERFACE_(IMediaPromptNotify, IUnknown)
{
	// *** IUnknown methods ***
	STDMETHOD(QueryInterface) (THIS_ REFIID riid, LPVOID FAR * lplpObj) PURE;
	STDMETHOD_(ULONG, AddRef) (THIS) PURE;
	STDMETHOD_(ULONG, Release) (THIS) PURE;

	// *** IMediaPromptNotify methods ***

	//
	// Prompt for the home CD.
	//

	STDMETHOD_(BOOL, PromptForHomeCD)(THIS) PURE;

	//
	// Prompt for a particular volume (CD or diskette).
	// The drive type is one specified in idrvmgr.h.
	//

	STDMETHOD_(BOOL, PromptForVolume)(THIS_ int nDriveType, LPCSTR pszVolume, LPCSTR pszMediaName, LPCSTR pszFileName) PURE;
};

#endif

