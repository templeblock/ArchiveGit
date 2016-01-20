#pragma once
#include "resource.h"       // main symbols

class CMobilize
{
public:

	CMobilize()
	{
	}

	bool UploadFiles(CSimpleArray<CString> arrFiles);

private:
	HGLOBAL PrepareFormData(const CString& strFormDataPairs, int iMaxWidth, int iMaxHeight, int nQuality, int bSilent);
	bool UploadForm(const CString& strURL, BYTE* pFormData, DWORD dwFormDataSize, int bSilent, CString& strResponseBody);
	bool ResizeJPG(const CString& strFilePath, int iMaxWidth, int iMaxHeight, int nQuality, int iFlags, CString& strFilePathNew, int& iResizeWidth, int& iResizeHeight);

public:
	static CString GetContentType(const CString& strExtension, bool& bIsImage);
};
