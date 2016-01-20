#ifndef Videofx_h
#define Videofx_h

#include "vidobsrv.h"
#include "vsndmix.h"

enum FxState
{
	StartState,
	MidState,
	EndState
};

class VideoFx : public VideoObserver {
public:
	VideoFx();
	~VideoFx();
	BOOL DoFx(HWND hVideoWnd, int iEventCode);
	virtual void OnVideoDraw(LPBITMAPINFOHEADER lpFormat, LPTR lpData);

protected:
	void Turn(LPBITMAPINFOHEADER lpFormat, LPTR lpData, int iEventCode);
	void Invert(LPBITMAPINFOHEADER lpFormat, LPTR lpData);
	void PlayFxSound( ITEMID id );

private:
	int iEvent;
	FxState State;
	HWND hWnd;
	int nCount;
	int nMidCount;
	int nMaxCount;
	int nSize;
	VSndMix *pSnd;
};

typedef VideoFx FAR *LPVIDEOFX;

#endif
