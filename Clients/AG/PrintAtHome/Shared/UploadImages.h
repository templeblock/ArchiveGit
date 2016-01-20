#pragma once

#define ULI_SILENT		1
#define ULI_DEBUG		2
#define ULI_NOUPSIZE	4
#define ULI_LEAVETEMP	8

class CUploadImages
{
public:
	CUploadImages(int iFlags);
	~CUploadImages();

	bool UploadImages(LPCSTR strURL, LPCSTR strFormDataPairs, int iMaxWidth, int iMaxHeight, int nQuality, CString& strResult);

private:
	HGLOBAL PrepareFormData(LPCSTR strFormDataPairs);
	bool ResizeJPG(LPCSTR strFilePath, CString& strFilePathResult, int& iWidthResult, int& iHeightResult);

private:
	int m_iMaxWidth;
	int m_iMaxHeight;
	int m_nQuality;
	int m_iFlags;
};
