#pragma once

class CColorMap
{
public:
	CColorMap();
	~CColorMap();

	void MakePoster(int nLevels);
	void MakeStretch(int iMinIndex, int iMaxIndex);
	void MakeContrastBrightness(int iContrast, int iBrightness);
	void MakeHueSaturation(int iHueAmount, int iHue, int iSaturation);
	void MakeGamma(int iRGamma, int iGGamma, int iBGamma);
	void MakeSepiaTone(int iIntensity);
	bool ProcessImage(BITMAPINFOHEADER* pSrcDib, BITMAPINFOHEADER* pDstDib, bool bColorize = false);

protected:
	BYTE m_R[256];
	BYTE m_G[256];
	BYTE m_B[256];
	bool m_bUsingHSL;
};
