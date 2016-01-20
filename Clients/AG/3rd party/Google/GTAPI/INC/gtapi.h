//  Copyright 2004 Google Inc.
//  All Rights Reserved.
//
//  Google Toolbar Partner API

#ifndef GOOGLE_TOOLBAR_API_H__
#define GOOGLE_TOOLBAR_API_H__

extern "C" {

//  ToolbarCompatibilityCheck()
//    Returns TRUE if the system can support Google Toolbar installation or
//    FALSE otherwise.
typedef BOOL (__stdcall *TCC_FN)(DWORD*);
BOOL __stdcall ToolbarCompatibilityCheck(DWORD *reasons);

#define TCC_ERROR_TOOLBARPRESENT                0x01
#define TCC_ERROR_ACCESSDENIED                  0x02
#define TCC_ERROR_IENOTDEFAULTBROWSER           0x04
#define TCC_ERROR_BHODISABLED                   0x08

}

#endif // GOOGLE_TOOLBAR_API_H__
