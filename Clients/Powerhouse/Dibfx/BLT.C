#define STRICT 1
#pragma warning (disable : 4001)
#include <windows.h>
#include <windowsx.h>
#include <stdlib.h>

//
// NOTE, this file doesn't use dib.h
//
// I got sick of debugging the macros between long's int's and so on.
//

typedef LPBITMAPINFOHEADER PDIB;

#define DibPitch(pdib)	((((pdib->biBitCount * pdib->biWidth) + 31) & ~31) / 8)
#define DibHeight(pdib)	(labs(pdib->biHeight))
#define DibWidth(pdib)	(pdib->biWidth)

void FAR PASCAL CopyBits(WORD selDst, DWORD offDst, DWORD pitchDst, WORD selSrc, DWORD offSrc, DWORD pitchSrc, DWORD cx, DWORD cy);
extern void PASCAL far Copy32Bytes( WORD DestSelector, DWORD DestOffset, WORD SrcSelector, DWORD SrcOffset, DWORD cBytes);


void FAR PASCAL CopyBytes(LPBYTE pDst, LPBYTE pSrc, DWORD cb)
{
	WORD DestSelector = HIWORD (pDst);
	DWORD DestOffset = LOWORD (pDst);
	WORD SourceSelector = HIWORD (pSrc);
	DWORD SourceOffset = LOWORD (pSrc);

	Copy32Bytes (DestSelector, DestOffset, SourceSelector, SourceOffset, cb);
}

BOOL WINAPI DibBlt(PDIB pdibDst, LPBYTE pbitsDst, int xDst, int yDst, PDIB pdibSrc, LPBYTE pbitsSrc, int xSrc, int ySrc, int cx, int cy)
{
	RECT rcDst, rcSrc;
	RECT rcBltDst, rcBltSrc;	
	long cxBlt, cyBlt;
	DWORD offDst, offSrc;
	WORD selDst, selSrc;
	long pitchDst, pitchSrc;
	
	SetRect(&rcDst, 0, 0, (int)DibWidth(pdibDst), (int)DibHeight(pdibDst));
	SetRect(&rcSrc, 0, 0, (int)DibWidth(pdibSrc), (int)DibHeight(pdibSrc));

	SetRect(&rcBltDst, xDst, yDst, xDst + cx, yDst + cy);
	SetRect(&rcBltSrc, xSrc, ySrc, xSrc + cx, ySrc + cy);

	if ((IntersectRect(&rcBltDst, &rcDst, &rcBltDst) == FALSE)
	||  (IntersectRect(&rcBltSrc, &rcSrc, &rcBltSrc) == FALSE))
	{
		return FALSE;
	}

	cxBlt = min(rcBltDst.right - rcBltDst.left, rcBltSrc.right - rcBltSrc.left);
	cyBlt = min(rcBltDst.bottom - rcBltDst.top, rcBltSrc.bottom - rcBltSrc.top);

	selDst = HIWORD(pbitsDst);
	selSrc = HIWORD(pbitsSrc);
	
	if (pdibDst->biHeight < 0)
	{
		pitchDst = (long)DibPitch(pdibDst);
		offDst = LOWORD(pbitsDst) + DibPitch(pdibDst) * rcBltDst.top + rcBltDst.left;
	}
	else
	{
		pitchDst = - (long)DibPitch(pdibDst);
		offDst = LOWORD(pbitsDst) + DibPitch(pdibDst) * (DibHeight(pdibDst) - rcBltDst.top - 1) + rcBltDst.left;
	}

	if (pdibSrc->biHeight < 0)
	{
		pitchSrc = (long)DibPitch(pdibSrc);
		offSrc = LOWORD(pbitsSrc) + DibPitch(pdibSrc) * rcBltSrc.top + rcBltSrc.left;
	}
	else
	{
		pitchSrc = - (long)DibPitch(pdibSrc);
		offSrc = LOWORD(pbitsSrc) + DibPitch(pdibSrc) * (DibHeight(pdibSrc) - rcBltSrc.top - 1) + rcBltSrc.left;
	}

	CopyBits(selDst, offDst, pitchDst, selSrc, offSrc, pitchSrc, cxBlt, cyBlt);
	return TRUE;
}
