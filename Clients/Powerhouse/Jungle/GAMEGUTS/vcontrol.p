// Source file: vcontrol.cpp
void Video_SetDefaults( int iZoomFactor, SHOTID lShot, BOOL bShortHotspots );
LPSHOT Video_GetShot( LPVIDEO lpVideo, SHOTID lShot, long lCount = 0 );
LPSHOT Video_FindSubShot( LPVIDEO lpVideo, SHOTID lShot, long lCount, long lFrame );
LPSHOT Video_FindShot( HWND hWindow, SHOTID lShot, long lCount );
LPSHOT Video_FindShotFlag( HWND hWindow, BYTE cFlagNum );
LPSHOT Video_FindShotName( HWND hWindow, LPSTR lpString );
void Video_GotoShot( HWND hWindow, LPVIDEO lpVideo, SHOTID lShot, long lCount, int iFrames, int iEventCode );
long WINPROC EXPORT VideoControl( HWND hWindow, unsigned message, WPARAM wParam, LPARAM lParam);
