//////////////////////////////////////////////////////////////////////////////
// $Header: /PM8/App/USERSET.H 1     3/03/99 6:13p Gbeddow $
//
//  "This unpublished source code contains trade secrets which are the
//   property of Mindscape, Inc.  Unauthorized use, copying or distribution
//   is a violation of international laws and is strictly prohibited."
// 
//        Copyright © 1998 Mindscape, Inc. All rights reserved.
//
// $Log: /PM8/App/USERSET.H $
// 
// 1     3/03/99 6:13p Gbeddow
// 
// 3     9/21/98 5:27p Jayn
// Broke PathManager out into a COM-style interface.
// 
// 2     9/15/98 2:49p Jayn
// Conversion of DriveManager to COM format.
// Added UserSettings.
// 
// 1     9/15/98 11:45a Jayn
//////////////////////////////////////////////////////////////////////////////

#ifndef __USERSET_H__
#define __USERSET_H__

#include "iuserset.h"

/////////////////////////////////////////////////////////////////////////////
// CPMUserSettings
//  PrintMaster-specific implementation of IUserSettings
//

class CPMUserSettings : public CCmdTarget
{
protected:
	DECLARE_INTERFACE_MAP()

	// This defines the interface that is passed to others.
	// It will have name m_XUserSettings.
	// To see comments on the functions, see the IUserSettings definition
	// in IUSERSET.H.
	BEGIN_INTERFACE_PART(UserSettings, IUserSettings)
		INIT_INTERFACE_PART(CPMUserSettings, UserSettings)
		STDMETHOD_(CString, GetString) (THIS_ LPCSTR pszSection, LPCSTR pszEntry, LPCSTR pszDefault, int nSettingType);
		STDMETHOD_(void, WriteString) (THIS_ LPCSTR pszSection, LPCSTR pszEntry, LPCSTR pszString, int nSettingType);
		STDMETHOD_(int, GetInteger) (THIS_ LPCSTR pszSection, LPCSTR pszEntry, int nDefault, int nSettingType);
		STDMETHOD_(void, WriteInteger) (THIS_ LPCSTR pszSection, LPCSTR pszEntry, int nValue, int nSettingType);
	END_INTERFACE_PART(UserSettings)
};

#endif
