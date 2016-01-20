#ifndef _VidObsrv_h
#define _VidObsrv_h

class VideoObserver
{
public:
	virtual void OnVideoOpen()  { return; }
	virtual void OnVideoClose() { return; }
	virtual void OnVideoDraw(LPBITMAPINFOHEADER lpFormat, LPTR lpData) { return; }
};

#endif
