#ifndef _Videofx_h
#define _Videofx_h

#include "vidobsrv.h"
#include "DirSnd.h"

#define VFX_TYPE_TURN 1

enum FxState
{
	StartState,
	MidState,
	EndState
};

class VideoFx : public VideoObserver {
public:
	VideoFx(BOOL bType);
	~VideoFx();
	BOOL DoFx(HWND hVideoWnd, int iEventCode, LPSHOT lpShot);
	virtual void OnVideoDraw(LPBITMAPINFOHEADER lpFormat, LPTR lpData);
	BOOL DoingFx() { return (BOOL)iEvent; }

	void DrawDibText(LPBITMAPINFOHEADER lpbi, LPTR lpDestBits);
	BOOL TextOn(HWND hSceneWnd, char *pszText, COLORREF cColor = 0, 
		int nPointSize = 0, int nMills = 0, int nPriority = 1);
	void TextOff(void);
	BOOL TextUp(void);

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
	CDirSnd *pSndSmall;
	CDirSnd *pSndBig;

	CDirSnd *pCurSnd;

	LPSHOT m_lpShot;
	STRING m_szText;
	PDIB m_pTextDib;
	HPALETTE m_hPal;
	COLORREF m_cColor;
	int m_nPointSize;
	int m_nMills;
	DWORD m_dwStartTime;
	int m_nPriority;
};

#endif
