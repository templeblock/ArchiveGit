#ifndef _ENUMPROC_INC_
#define _ENUMPROC_INC_

#include <windows.h>

typedef BOOL (CALLBACK *PROCENUMPROC)( DWORD, WORD, LPSTR, LPARAM ) ;

BOOL WINAPI EnumProcs( PROCENUMPROC lpProc, LPARAM lParam ) ;

#endif
