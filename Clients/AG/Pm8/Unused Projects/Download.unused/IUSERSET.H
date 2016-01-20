//////////////////////////////////////////////////////////////////////////////
// $Header: /PM8/Download/IUSERSET.H 1     3/03/99 6:15p Gbeddow $
//
//  "This unpublished source code contains trade secrets which are the
//   property of Mindscape, Inc.  Unauthorized use, copying or distribution
//   is a violation of international laws and is strictly prohibited."
// 
//        Copyright © 1998 Mindscape, Inc. All rights reserved.
//
// $Log: /PM8/Download/IUSERSET.H $
// 
// 1     3/03/99 6:15p Gbeddow
// 
// 3     9/21/98 5:26p Jayn
// Broke PathManager out into a COM-style interface.
// 
// 2     9/15/98 2:49p Jayn
// Conversion of DriveManager to COM format.
// Added UserSettings.
// 
// 1     9/15/98 10:49a Jayn
//////////////////////////////////////////////////////////////////////////////

#ifndef __IUSERSET_H__
#define __IUSERSET_H__

// {BCF2DEC0-49D1-11d2-985D-00A0246D4780}
DEFINE_GUID(IID_IUserSettings, 0xbcf2dec0, 0x49d1, 0x11d2, 0x98, 0x5d, 0x0, 0xa0, 0x24, 0x6d, 0x47, 0x80);

#undef INTERFACE
#define INTERFACE   IUserSettings

DECLARE_INTERFACE_(IUserSettings, IUnknown)
{
	// Setting values.
	enum
	{
		AnySetting		= 0,
		GlobalSetting	= 1,
		UserSetting		= 2
	};

	// *** IUnknown methods ***
	STDMETHOD(QueryInterface) (THIS_ REFIID riid, LPVOID FAR * lplpObj) PURE;
	STDMETHOD_(ULONG, AddRef) (THIS)  PURE;
	STDMETHOD_(ULONG, Release) (THIS) PURE;

	// *** IUserSettings methods ***

	//
	// Read a string from settings.
	// pszDefault can be NULL (which defaults to the empty string).
	//

	STDMETHOD_(CString, GetString) (THIS_ LPCSTR pszSection, LPCSTR pszEntry, LPCSTR pszDefault, int nSettingType = AnySetting) PURE;

	//
	// Write a string to settings.
	//

	STDMETHOD_(void, WriteString) (THIS_ LPCSTR pszSection, LPCSTR pszEntry, LPCSTR pszString, int nSettingType = UserSetting) PURE;

	//
	// Read an int from settings.
	//

	STDMETHOD_(int, GetInteger) (THIS_ LPCSTR pszSection, LPCSTR pszEntry, int nDefault, int nSettingType = AnySetting) PURE;

	//
	// Write an int to settings.
	//

	STDMETHOD_(void, WriteInteger) (THIS_ LPCSTR pszSection, LPCSTR pszEntry, int nValue, int nSettingType = UserSetting) PURE;
};

#endif
