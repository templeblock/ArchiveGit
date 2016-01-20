// ImageExtractor.cpp : Implementation of CImageExtractor

#include "stdafx.h"
#include "Ctp.h"
#include "AGDoc.h"
#include "AGPage.h"
#include "Image.h"
#include <ShObjIdl.h>
#include <fstream>

#define CImageExtractor CCtp

static const bool bCacheThumbs = false;

/////////////////////////////////////////////////////////////////////////////
static void log(LPCTSTR szString)
{
	CString szMsg(szString);
	HANDLE han1 = CreateFile("C:\\CpLogFile.txt", GENERIC_WRITE, 0, NULL, OPEN_ALWAYS, 0, NULL);
	ULONG lBytesWritten;
	ULONG lMove = 0;
	GetFileSize(han1, &lMove);
	SetFilePointer(han1, NULL, NULL, FILE_END);
	WriteFile(han1,szMsg, (sizeof(char)*szMsg.GetLength()),&lBytesWritten, NULL);
	CloseHandle(han1);
}

/////////////////////////////////////////////////////////////////////////////
static HBITMAP CheckCache(LPCSTR strFileName)
{
	if (!bCacheThumbs)
		return NULL;

	CImage Image(strFileName);
	HBITMAP hBitmap = Image.GetBitmapHandle(true/*bTakeOver*/);

	//if (hBitmap)
	//	log(String("**Cache hit**:: file='%s', hBitmap=%d\r\n", strFileName, hBitmap));
	
	return hBitmap;
}

/////////////////////////////////////////////////////////////////////////////
// IPersistFile
HRESULT CImageExtractor::Load(LPCOLESTR wszFile, DWORD dwMode)
{
	USES_CONVERSION;
	m_strPersistFileName = wszFile;
	//log(String("\r\n **CImageExtractor**::Load file='%s' dwMode=%ld\r\n", m_strPersistFileName, dwMode));
	return S_OK;	
}

/////////////////////////////////////////////////////////////////////////////
// IExtractImage2
HRESULT CImageExtractor::GetDateStamp(FILETIME *pDateStamp)
{
	//log(String("**CImageExtractor**::GetDateStamp file='%s'\r\n", m_strPersistFileName));

	// open the file and get last write time
	HANDLE hFile = CreateFile(m_strPersistFileName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (!hFile)
		return E_FAIL;

	FILETIME ftCreationTime, ftLastAccessTime, ftLastWriteTime;
	GetFileTime(hFile,&ftCreationTime, &ftLastAccessTime, &ftLastWriteTime);
	//log(String("**CImageExtractor**::GetDateStamp file='%s' TIMESTAMP = %d\r\n", m_strPersistFileName, ftLastWriteTime));
	CloseHandle(hFile);
	*pDateStamp = ftLastWriteTime;
	return S_OK; 
}

/////////////////////////////////////////////////////////////////////////////
// IExtractImage
HRESULT CImageExtractor::GetLocation(LPWSTR pszPathBuffer, DWORD cchMax, DWORD *pdwPriority, const SIZE* prgSize, DWORD dwRecClrDepth, DWORD *pdwFlags)
{
	if (!prgSize || !pdwFlags)
		return E_FAIL;
		
	//pszPathBuffer = m_strPersistFileName.AllocSysString();

	//*pdwFlags |= IEIFLAG_ASYNC;
	//*pdwFlags |= IEIFLAG_CACHE;
	//*pdwFlags &= IEIFLAG_REFRESH;
	if (prgSize)
		m_ExtractImageSize = *prgSize;

	LPCSTR strFlag = "";
	CString szPathBuffer(pszPathBuffer);
	//if (*pdwFlags & IEIFLAG_ASYNC)	strFlag = "async";
	//if (*pdwFlags & IEIFLAG_ASPECT)	strFlag = "aspect";
	//if (*pdwFlags & IEIFLAG_SCREEN)	strFlag = "screen";

	//log(String("**CImageExtractor**::GetLocation PATH = %s, cchMax=%ld, flag=%s Size=(%d,%d) dwRecClrDepth=%ld\r\n", szPathBuffer, cchMax, strFlag, m_ExtractImageSize.cx, m_ExtractImageSize.cy, dwRecClrDepth));

	//if (*pdwFlags & IEIFLAG_ASYNC)
	//{
	//	log(_T("ASYCN flag was hit\n"));
	//	*pdwPriority = 3;
	//	return E_PENDING; 
	//}
	//log(_T("ASYCN flag was NOT hit\n"));
	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
HRESULT CImageExtractor::Extract(HBITMAP* phBmpThumbnail)
{ 
	CWaitCursor Wait;
	//log(String("**CImageExtractor**::Extract bOK=%d, file='%s'\r\n", (*phBmpThumbnail ? true : false), m_strPersistFileName));

	if (!phBmpThumbnail)
		return E_FAIL;

	*phBmpThumbnail = NULL;
	if (m_strPersistFileName.IsEmpty())
		return E_FAIL;

	//Look for the file in the temp folder first
	TCHAR szBuffer[MAX_PATH];
	GetTempPath(MAX_PATH, szBuffer);
	CString szTempPath(szBuffer);
	CString szCacheFile(m_strPersistFileName);
	int nFilePos = szCacheFile.ReverseFind(_T('\\'));
	CString szFile = szCacheFile.Right(szCacheFile.GetLength() - (nFilePos + 1));
	nFilePos = szFile.ReverseFind(_T('.'));
	CString szTitle = szFile.Left(nFilePos);
	szCacheFile = szTempPath + szTitle + _T(".bmp");

	*phBmpThumbnail = CheckCache(szCacheFile);
	if (!(*phBmpThumbnail))
	{
		CAGDoc* pAGDoc = new CAGDoc();
		ifstream In(m_strPersistFileName, ios::in | /*ios::nocreate |*/ ios::binary);

		bool bAdjusted = false;
		if (pAGDoc->Read(In, bAdjusted))
		{
			CAGPage* pPage = pAGDoc->GetPage(0);
			if (pPage)
				pPage->CreateThumb(m_ExtractImageSize, (bCacheThumbs ? (LPCSTR)szCacheFile : NULL), "DIB", phBmpThumbnail);
		}

		delete pAGDoc;
	}

	return (*phBmpThumbnail ? S_OK : E_FAIL); 
}
