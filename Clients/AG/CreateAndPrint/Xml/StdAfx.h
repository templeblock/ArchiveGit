#pragma once

#define STRICT

//_WIN32_WINNT>=0x0502		Windows Server 2003 family
//_WIN32_WINNT>=0x0501		Windows XP
//_WIN32_WINNT>=0x0500		Windows 2000
//_WIN32_WINNT>=0x0400		Windows NT 4.0
//_WIN32_WINDOWS=0x0490		Windows Me
//_WIN32_WINDOWS>=0x0410	Windows 98
//_WIN32_IE>=0x0600			Internet Explorer 6.0
//_WIN32_IE>=0x0501			Internet Explorer 5.01, 5.5
//_WIN32_IE>=0x0500			Internet Explorer 5.0, 5.0a, 5.0b
//_WIN32_IE>=0x0401			Internet Explorer 4.01
//_WIN32_IE>=0x0400			Internet Explorer 4.0

#define WINVER 0x0400
#define _WIN32_WINNT 0x0400
#define _WIN32_WINDOWS 0x0410
#define _WIN32_IE 0x0501

#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#include <afxdisp.h>        // MFC Automation classes
#include <afxdtctl.h>		// MFC support for Internet Explorer 4 Common Controls
#include <afxcmn.h>			// MFC support for Windows Common Controls
#include <atlbase.h>
#include <atlstr.h>

#include "GdiClasses.h"
#include "Utility.h"

#import "msxml.dll" named_guids raw_interfaces_only
using namespace MSXML;
