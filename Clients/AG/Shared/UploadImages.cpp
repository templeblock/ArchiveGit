// UploadImages.cpp : Implementation of CUploadImages
#include "stdafx.h"
#include "UploadImages.h"
#include "Image.h"
#include "ScaleImage.h"
#include "Dib.h"
#include "Utility.h"
#include "ConvertDibToJpg.h"
//#include "MyAtlUrl.h"

#define IsSilent(iFlags) (iFlags & ULI_SILENT)
#define IsDebug(iFlags) (iFlags & ULI_DEBUG)
#define IsNoUpsize(iFlags) (iFlags & ULI_NOUPSIZE)
#define IsLeaveTemp(iFlags) (iFlags & ULI_LEAVETEMP)

//////////////////////////////////////////////////////////////////////
CUploadImages::CUploadImages(int iFlags)
{
	m_iMaxWidth = 0;
	m_iMaxHeight = 0;
	m_nQuality = 0;
	m_iFlags = iFlags;
}

//////////////////////////////////////////////////////////////////////
CUploadImages::~CUploadImages()
{
}

/////////////////////////////////////////////////////////////////////////////
// Boundary of fields - be sure this string is not In the source file
const LPCSTR Boundary = "7d33a7820054c";
const CString vbCrLf = "\r\n";

/////////////////////////////////////////////////////////////////////////////
// strFormDataPairs is a string of name/value pairs as follows "name1:value1|\name2:value2|\n"
bool CUploadImages::UploadImages(LPCSTR strURL, LPCSTR strFormDataPairs, int iMaxWidth, int iMaxHeight, int nQuality, CString& strResult)
{
	m_iMaxWidth = iMaxWidth;
	m_iMaxHeight = iMaxHeight;
	m_nQuality = nQuality;

	if (IsDebug(m_iFlags))
		::MessageBox(NULL, strFormDataPairs, "Form Data passed to UploadImages", MB_OK|MB_ICONINFORMATION);

	CWaitCursor Wait;
	
	// Post the form data to the URL
	CComPtr<IWebBrowser2> pBrowser;

	// Set the return value in case we leave early
	strResult = "Error";

	HRESULT hr = pBrowser.CoCreateInstance(CComBSTR("InternetExplorer.Application"));
	if (!pBrowser)
	{
		if (!IsSilent(m_iFlags))
		{
			CString strMsg = String("Could not create an instance of the InternetExplorer.Application.\n\n%0xlx", hr);
			::MessageBox(NULL, strMsg, "Error", MB_OK|MB_ICONSTOP);
		}
		return SUCCEEDED(hr);
	}

	HGLOBAL hMemory = PrepareFormData(strFormDataPairs);
	if (!hMemory)
		return false;

	BYTE* pFormData = (BYTE*)::GlobalLock(hMemory);
	if (!pFormData)
	{
		if (!IsSilent(m_iFlags))
		{
			CString strMsg = String("Failed to access the form data.");
			::MessageBox(NULL, strMsg, "Error", MB_OK|MB_ICONSTOP);
		}
		return false;
	}
	
	// You can uncomment following line to see the results
	pBrowser->put_Visible(false);

	// Send the form data To URL As a POST request
	CComVariant varFlags = navNoHistory;
	CComVariant varTargetFrameName = "_self";
	CComSafeArray<BYTE> Array;
	Array.Add(::GlobalSize(hMemory), pFormData);
	CComVariant varPostData = Array;

	// For simple form data, use the following
	//pFormData = "pn1605=Photo+Album+Title&pn1629=Photo+Caption+1&pn1631=Photo+Caption+2&rights=Y&agree=Y&mode=submit&FilePers=1628&CapPers=1629&TitlePers=1605";
	//CComVariant varHeaders = CComBSTR(CString("Content-Type: application/x-www-form-urlencoded") + vbCrLf);

	CComVariant varHeaders = CComBSTR(CString("Content-Type: multipart/form-data; boundary=") + Boundary + vbCrLf);
	hr = pBrowser->Navigate(CComBSTR(strURL), &varFlags, &varTargetFrameName, &varPostData, &varHeaders);

	for (int i=0; i<10; i++)
	{
		VARIANT_BOOL bBusy = true; // Initialize this to true if you want to wait for the document to load
		while (bBusy)
		{
			pBrowser->get_Busy(&bBusy);
	//j		DoEvents();
		}

		CComPtr<IDispatch> pDisp;
		hr = pBrowser->get_Document(&pDisp);
		if (pDisp)
		{
			CComQIPtr<IHTMLDocument2> spHTML = pDisp;
			CComPtr<IHTMLElement> pBody;
			hr = spHTML->get_body(&pBody);
			if (pBody)
			{
				CComBSTR bstrBody;
				hr = pBody->get_innerHTML(&bstrBody);
				strResult = bstrBody;
				if (IsDebug(m_iFlags))
					::MessageBox(NULL, CString(bstrBody), "Body of the Response", MB_OK|MB_ICONINFORMATION);
				break;
			}
		}

		// Failed to get the document body - loop back around and retry
	}

	pBrowser->Quit();
	pBrowser.Release();
	pBrowser = NULL;

	::GlobalFree(hMemory);

	return true;
}

/////////////////////////////////////////////////////////////////////////////
// strFormDataPairs is a string of name/value pairs as follows "name1:value1|\name2:value2|\n"
HGLOBAL CUploadImages::PrepareFormData(LPCSTR strFormDataPairs)
{
	IStream* pStream = NULL;
	HRESULT hr = ::CreateStreamOnHGlobal(NULL, false/*fDeleteOnRelease*/, &pStream);
	if (FAILED(hr) || !pStream)
	{
		if (!IsSilent(m_iFlags))
		{
			CString strMsg = String("Could not create a stream.\n\n%0xlx", hr);
			::MessageBox(NULL, strMsg, "Error", MB_OK|MB_ICONSTOP);
		}
		return NULL;
	}
	
	// Pull the name/value pairs off of the form string
	CString FormData;
	int iStart = 0;
	ULONG ulWritten;
	CString strFormData = strFormDataPairs;
	while (iStart >= 0)
	{
		int iEnd = strFormData.Find(_T('|'), iStart);
		if (iEnd < 0)
			iEnd = strFormData.GetLength();
		if (iEnd <= iStart)
			break;
		int iPair = strFormData.Find(_T(':'), iStart);
		if (iPair > iStart && iPair < iEnd)
		{
			CString strName = strFormData.Mid(iStart, iPair-iStart);
			strName.TrimLeft();
			strName.TrimRight();
			if (!strName.IsEmpty())
			{
				iPair++;
				CString strValue = strFormData.Mid(iPair, iEnd-iPair);

				bool bValueIsFileName = (strValue.Find('~') == 0);
				if (bValueIsFileName)
					strValue = strValue.Mid(1);

				FormData += CString("--") + Boundary + vbCrLf;
				if (!bValueIsFileName)
				{
					FormData += CString("Content-Disposition: form-data; name=\"") + strName + CString("\"") + vbCrLf + vbCrLf;
					FormData += strValue + vbCrLf;
				}
				else
				{
					CString strFilePathResult;
					int iWidthResult = 0;
					int iHeightResult = 0;
					bool bOK = ResizeJPG(strValue, strFilePathResult, iWidthResult, iHeightResult);
					bool bFileReturned = (strFilePathResult != "");
					if (!bFileReturned)
					{
						pStream->Release();
						return NULL;
					}

					bool bFileCreated = (strFilePathResult != strValue);

					// Add the image width and height to the form
					strValue.Format("%d", iWidthResult);
					FormData += CString("Content-Disposition: form-data; name=\"") + strName + CString("width\"") + vbCrLf + vbCrLf;
					FormData += strValue + vbCrLf;
					FormData += CString("--") + Boundary + vbCrLf;
					strValue.Format("%d", iHeightResult);
					FormData += CString("Content-Disposition: form-data; name=\"") + strName + CString("height\"") + vbCrLf + vbCrLf;
					FormData += strValue + vbCrLf;
					FormData += CString("--") + Boundary + vbCrLf;

					strValue = strFilePathResult;
				//j	EscapeUrl(strFilePathResult, strValue);
					FormData += CString("Content-Disposition: form-data; name=\"") + strName + CString("\"; filename=\"") + strValue + CString("\"") + vbCrLf;
					FormData += CString("Content-Type: image/jpeg") + vbCrLf + vbCrLf;
				//j	FormData += CString("Content-Transfer-Encoding: binary") + vbCrLf + vbCrLf;
					pStream->Write(FormData, FormData.GetLength(), &ulWritten);
					FormData.Empty();

					HGLOBAL hFileMemory = ReadFileIntoMemory(strValue);
					if (hFileMemory)
					{
						BYTE* pFileMemory = (BYTE*)::GlobalLock(hFileMemory);
						if (pFileMemory)
						{
							pStream->Write(pFileMemory, ::GlobalSize(hFileMemory), &ulWritten);
							::GlobalUnlock(hFileMemory);
						}

						::GlobalFree(hFileMemory);
					}

					FormData += vbCrLf;

					if (bFileCreated && !IsDebug(m_iFlags) && !IsLeaveTemp(m_iFlags))
						::DeleteFile(strFilePathResult);
				}
			}
		}

		iStart = iEnd + 1;
	}

	FormData += CString("--") + Boundary + CString("--") + vbCrLf + vbCrLf;
	pStream->Write(FormData, FormData.GetLength(), &ulWritten);

	HGLOBAL hMemory = NULL;
	hr = ::GetHGlobalFromStream(pStream, &hMemory);
	pStream->Release();

	if (!hMemory && !IsSilent(m_iFlags))
	{
		CString strMsg = String("Could not get memory from the stream.");
		::MessageBox(NULL, strMsg, "Error", MB_OK|MB_ICONSTOP);
	}

	return hMemory;
}

//////////////////////////////////////////////////////////////////////
bool CUploadImages::ResizeJPG(LPCSTR bstrFilePath, CString& strFilePathResult, int& iWidthResult, int& iHeightResult)
{
	CWaitCursor Wait;
	
	// Set the return value in case we leave early
	strFilePathResult.Empty();
	iWidthResult = 0;
	iHeightResult = 0;

	// Create an image object
	CString strFilePath = CString(bstrFilePath);
	CImage Image(strFilePath);
	
	// Get a Dib pointer from the image
	BITMAPINFOHEADER* pDibSrc = Image.GetDibPtr();
	if (!pDibSrc)
	{
		if (!IsSilent(m_iFlags))
		{
			CString strMsg = String("Could not resize image file.\n\n%s", strFilePath);
			::MessageBox(NULL, strMsg, "Error", MB_OK|MB_ICONSTOP);
		}

		return true;
	}

	bool bNoScale = (IsNoUpsize(m_iFlags) && (pDibSrc->biWidth <= m_iMaxWidth && pDibSrc->biHeight <= m_iMaxHeight));

	// Bail (with the original image) if upsizing is not allowed
	if (bNoScale)
	{
		strFilePathResult = strFilePath;
		iWidthResult = pDibSrc->biWidth;
		iHeightResult = pDibSrc->biHeight;
		return true;
	}

	// Copy the source Dib header into a destination Dib
	BITMAPINFOHEADER DibDst = *pDibSrc;

	// Scale the Dib to fit within the maximum rectangle
	double fxScale = (!DibDst.biWidth ? 1.0 : (double)m_iMaxWidth / DibDst.biWidth);
	double fyScale = (!DibDst.biHeight ? 1.0 : (double)m_iMaxHeight / DibDst.biHeight);
	double fScale = min(fxScale, fyScale);
	if (bNoScale)
		fScale = 1.0;

	// Make it fit
	DibDst.biWidth  = (long)(fScale * DibDst.biWidth);
	DibDst.biHeight = (long)(fScale * DibDst.biHeight);
	DibSizeImageRecompute(&DibDst);

	BITMAPINFOHEADER* pDibDst = NULL;
	if (bNoScale)
	{
		pDibDst = DibCopy(pDibSrc);
	}
	else
	{
		// Allocate memory for the new Dib
		pDibDst = DibAlloc(DibSize(&DibDst));
		if (pDibDst)
		{
			// Copy the Dib header
			*pDibDst = DibDst;
			// Resize the Dib
			ScaleImage(pDibSrc, pDibDst);
		}
	}

	if (!pDibDst)
	{
		if (!IsSilent(m_iFlags))
		{
			CString strMsg = String("Could not resize image file.\n\n%s", strFilePath);
			::MessageBox(NULL, strMsg, "Error", MB_OK|MB_ICONSTOP);
		}

		return true;
	}

	// Generate a temporary Jpg file name
	CString strFilePathOut = RandomFileName("cr", ".jpg");

	// Convert the Dib in memory to a Jpg
	bool bOK = ConvertDibToJpg(pDibDst, m_nQuality, strFilePathOut);
	if (!bOK)
	{
		if (!IsSilent(m_iFlags))
		{
			CString strMsg = String("Could not resize image file.\n\n%s", strFilePathOut);
			::MessageBox(NULL, strMsg, "Error", MB_OK|MB_ICONSTOP);
		}
		return true;
	}

	strFilePathResult = strFilePathOut;
	iWidthResult = pDibDst->biWidth;
	iHeightResult = pDibDst->biHeight;

	return true;
}
