#pragma once

class CDib
{
public:
	static void OrientChangeState(BYTE& iState, int iRotateDirection, bool bFlipX, bool bFlipY);
	static bool OrientDecode(BYTE iState, int& iRotateDirection, bool& bFlipX, bool& bFlipY);
	static BITMAPINFOHEADER* OrientRestore(const BITMAPINFOHEADER* pSrcDib, BYTE iState);
	static BITMAPINFOHEADER* Orient(const BITMAPINFOHEADER* pSrcDib, int iRotateDirection, bool bFlipX, bool bFlipY, BYTE* pSrcBits = NULL);
};
