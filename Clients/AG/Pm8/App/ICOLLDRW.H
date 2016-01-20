//////////////////////////////////////////////////////////////////////////////
// $Header: /PM8/App/ICOLLDRW.H 1     3/03/99 6:06p Gbeddow $
//
//  "This unpublished source code contains trade secrets which are the
//   property of Mindscape, Inc.  Unauthorized use, copying or distribution
//   is a violation of international laws and is strictly prohibited."
// 
//        Copyright © 1998 Mindscape, Inc. All rights reserved.
//
// $Log: /PM8/App/ICOLLDRW.H $
// 
// 1     3/03/99 6:06p Gbeddow
// 
// 2     10/19/98 10:48a Jayn
// More COM restructuring changes.
// 
// 1     10/14/98 12:35p Jayn
//
//////////////////////////////////////////////////////////////////////////////

#ifndef __ICOLLDRW_H__
#define __ICOLLDRW_H__

class CPMWCollectionData;
class CThumbnailDataEntry;

// {BCF2DEC7-49D1-11d2-985D-00A0246D4780}
DEFINE_GUID(IID_ICollectionDraw, 0xbcf2dec7, 0x49d1, 0x11d2, 0x98, 0x5d, 0x0, 0xa0, 0x24, 0x6d, 0x47, 0x80);

#undef INTERFACE
#define INTERFACE   ICollectionDraw

DECLARE_INTERFACE_(ICollectionDraw, IUnknown)
{
	// *** IUnknown methods ***
	STDMETHOD(QueryInterface) (THIS_ REFIID riid, LPVOID FAR * lplpObj) PURE;
	STDMETHOD_(ULONG, AddRef) (THIS)  PURE;
	STDMETHOD_(ULONG, Release) (THIS) PURE;

	// *** ICollectionDraw methods ***

	//
	// Build a thumbnail.
	//

	STDMETHOD_(int, BuildThumbnail) (THIS_ int nType, CPMWCollectionData* pData, CThumbnailDataEntry* pThumbnailData) PURE;
};

#endif
