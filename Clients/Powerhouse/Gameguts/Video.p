typedef void (CALLBACK* VIDEO_DRAWDIB_PROC)( PDIB );
typedef void (CALLBACK* VIDEO_DRAWDC_PROC)( HDC );

class VideoObserver;

// Source file: video.cpp
void VideoInit( BOOL bZoomBy2, BOOL bFullScreen );
void VideoDone(void);

BOOL VideoSetObserver(VideoObserver *pObserver);
BOOL VideoClearObserver(VideoObserver *pObserver);
void VideoClearAllObservers(void);

BOOL VideoSetTimer(HWND hNotifyWnd, WORD wId);
BOOL VideoKillTimer(HWND hNotifyWnd, WORD wId);
void VideoKillAllTimers(void);

BOOL MCISetVideoDrawProc( HWND hVideoWnd, WORD wDeviceID );

