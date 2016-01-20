#ifndef _JM_HOOK_H
#define _JM_HOOK_H
#if _MSC_VER > 1000
#pragma once
#endif
#ifdef __cplusplus
extern "C" {
#endif // __cplusplus
#ifdef _COMPILING_44E531B1_14D3_11d5_A025_006067718D04
#define LIBSPEC __declspec(dllexport)
#else
#define LIBSPEC __declspec(dllimport)
#endif // _COMPILING_44E531B1_14D3_11d5_A025_006067718D04
   LIBSPEC BOOL InstallCBTHook(const DWORD dwThreadID);
   LIBSPEC BOOL InstallSHELLHook(bool bUseCBT);
   LIBSPEC BOOL RemoveCBTHook();
   LIBSPEC BOOL RemoveSHELLHook();
   LIBSPEC BOOL RemoveToolbars(HWND hwndRebar);
   LIBSPEC BOOL ShowToolbars();
   LIBSPEC BOOL ForceShutdown();

#undef LIBSPEC
#ifdef __cplusplus
}
#endif // __cplusplus
#define UWM_OECREATE_MSG _T("UWM_OECREATE_MSG-33E531B1_14D3_11d5_A025_006067718D04")
#define UWM_OECOMPOSE_MSG _T("UWM_OECOMPOSE_MSG-33E531B1_14D3_11d5_A025_006067718D04")
#define UWM_AOLCOMPOSE_MSG _T("UWM_AOLCOMPOSE_MSG-3D72DCD5-742A-44d2-B2A9-3E7700919871")
#define UWM_SHUTDOWN_MSG _T("UWM_SHUTDOWN_MSG-33E531B1_14D3_11d5_A025_006067718D04")
#define UWM_TBREMOVE_MSG _T("UWM_TBREMOVE_MSG-33E531B1_14D3_11d5_A025_006067718D04")
#define UWM_TBSHOW_MSG _T("UWM_TBSHOW_MSG-33E531B1_14D3_11d5_A025_006067718D04")
#define UWM_TBDESTROY_MSG _T("UWM_TBDESTROY_MSG-33E531B1_14D3_11d5_A025_006067718D04")
#define UWM_MOVEBAND_MSG _T("UWM_MOVEBAND_MSG-33E531B1_14D3_11d5_A025_006067718D04")

static const TCHAR OE_BROWSER_CLASS[]			= _T("Outlook Express Browser Class");
static const TCHAR OE_COMPOSEWINDOW_CLASS[]		= _T("ATH_Note");
static const TCHAR AOL_BROWSER_CLASS[]			= _T("AOL Frame25");
static const TCHAR AOL_COMPOSEWINDOW_CLASS[]	= _T("AOL Child");
static const TCHAR AOL_BROWSER_TITLE[16]			= _T("America Online");

// These declarations are because the latest SDK isn't installed...
#ifndef GET_X_LPARAM
#define GET_X_LPARAM(p) ((int)(short)LOWORD(p))
#define GET_Y_LPARAM(p) ((int)(short)HIWORD(p))
#endif



#endif // _JM_HOOK_H