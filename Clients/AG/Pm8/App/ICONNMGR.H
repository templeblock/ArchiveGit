//////////////////////////////////////////////////////////////////////////////
// $Header: /PM8/App/ICONNMGR.H 1     3/03/99 6:06p Gbeddow $
//
//  "This unpublished source code contains trade secrets which are the
//   property of Mindscape, Inc.  Unauthorized use, copying or distribution
//   is a violation of international laws and is strictly prohibited."
// 
//        Copyright © 1998 Mindscape, Inc. All rights reserved.
//
// $Log: /PM8/App/ICONNMGR.H $
// 
// 1     3/03/99 6:06p Gbeddow
// 
// 1     10/19/98 10:52a Jayn
// 
//////////////////////////////////////////////////////////////////////////////

#ifndef __ICONNMGR_H__
#define __ICONNMGR_H__

// {BCF2DEC9-49D1-11d2-985D-00A0246D4780}
DEFINE_GUID(IID_IConnectionManager, 0xbcf2dec9, 0x49d1, 0x11d2, 0x98, 0x5d, 0x0, 0xa0, 0x24, 0x6d, 0x47, 0x80);

// Include internet connection status definitions.
#include "INetConnStat.h"

#undef INTERFACE
#define INTERFACE   IConnectionManager

DECLARE_INTERFACE_(IConnectionManager, IUnknown)
{
	// *** IUnknown methods ***
	STDMETHOD(QueryInterface) (THIS_ REFIID riid, LPVOID FAR * lplpObj) PURE;
	STDMETHOD_(ULONG, AddRef) (THIS)  PURE;
	STDMETHOD_(ULONG, Release) (THIS) PURE;

	// *** IConnectionManager methods ***

	STDMETHOD_(long, Connect) (THIS) PURE;
	STDMETHOD_(BOOL, IsConnected) (THIS) PURE;
	STDMETHOD_(long, EditPrefs) (THIS) PURE;
	STDMETHOD_(BOOL, HangUp) (THIS) PURE;
	STDMETHOD_(BOOL, EnableIsConnectedCheck)(THIS_ BOOL bEnable) PURE;
	STDMETHOD_(BOOL, EnableHangUpPrompt) (THIS_ BOOL bEnable) PURE;
	STDMETHOD_(void, SetURL) (THIS_ LPCTSTR szURL) PURE;
};

#endif
