typedef void (CALLBACK* VIDEO_DRAWDIB_PROC)( PDIB );
typedef void (CALLBACK* VIDEO_DRAWDC_PROC)( HDC );

// Source file: video.cpp
void VideoInit( BOOL bZoomBy2, BOOL bFullScreen );
void VideoDone(void);
BOOL MCISetVideoDrawProc( WORD wDeviceID, VIDEO_DRAWDIB_PROC lpDrawDibProc = NULL, VIDEO_DRAWDC_PROC lpDrawDCProc = NULL );

