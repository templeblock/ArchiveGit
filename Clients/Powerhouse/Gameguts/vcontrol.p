#ifndef VCONTROL_P
#define VCONTROL_P

#include "vcontrol.h"

// Source file: vcontrol.cpp
BOOL Video_Register( HINSTANCE hInstance );
void Video_SetDefaults( int iZoomFactor, SHOTID lShot, BOOL bShortHotspots = NO, BOOL bUseMCIDrawProc = NO );
LPSHOT Video_GetShot( LPVIDEO lpVideo, SHOTID lShot, long lCount = 0 );
LPSHOT Video_FindSubShot( LPVIDEO lpVideo, SHOTID lShot, long lCount, long lFrame );
LPSHOT Video_FindShot( HWND hWindow, SHOTID lShot, long lCount );
LPSHOT Video_FindShotFlag( HWND hWindow, BYTE cFlagNum );
LPSHOT Video_FindShotFlags( HWND hWindow, DWORD dwFlags );
LPSHOT Video_FindShotName( HWND hWindow, LPSTR lpString );
LPSHOT Video_FindShotPrefix( HWND hWindow, LPSTR lpString );
void Video_GotoShot( HWND hWindow, LPVIDEO lpVideo, SHOTID lShot, long lCount, int iFrames, int iEventCode );
void Video_OnSoundOnOff( HWND hWindow, BOOL bOn );
void Video_ProcessEvent( HWND hWindow, LPVIDEO lpVideo, int iEventCode );
long WINPROC EXPORT VideoControl( HWND hWindow, unsigned message, WPARAM wParam, LPARAM lParam );
void Video_HookMouseButtons( LPLBUTTONHOOK lpLeftProc, LPRBUTTONHOOK lpRightProc, LPLUBUTTONHOOK lpLeftUProc );
void Video_UnhookMouseButtons();
void Video_RemapShotTable(HWND hWindow);
void Video_Close( HWND hWindow, LPVIDEO lpVideo );
BOOL Video_Open( HWND hWindow, LPVIDEO lpVideo, WORD wDisk, long lStartFrame );
long Video_JumpFrame(void);
LPSHOT Video_GetCurrentShot( LPVIDEO lpVideo );
#endif
