// Plugin.cpp : Implementation of CPlugin
#include "stdafx.h"
#include "Plugin.h"
#include "Image.h"
#include "ScaleImage.h"
#include "AGDib.h"
#include "Utility.h"
#include "ColorPicker.h"
#include "MyAtlUrl.h"
#include "ConvertDibToJpg.h"
#include "Spell.h"
#include "Version.h"

#define FL_SILENT		1
#define FL_DEBUG		2
#define FL_NOUPSIZE		4
#define FL_LEAVETEMP	8

#define IsSilent(iFlags) (iFlags & FL_SILENT)
#define IsDebug(iFlags) (iFlags & FL_DEBUG)
#define IsNoUpsize(iFlags) (iFlags & FL_NOUPSIZE)
#define IsLeaveTemp(iFlags) (iFlags & FL_LEAVETEMP)

static int m_iResizeWidth;
static int m_iResizeHeight;

//////////////////////////////////////////////////////////////////////
STDMETHODIMP CPlugin::Version(BSTR* pbstrVersion)
{
	CString strVersion;
	strVersion.Format("%d,%d,%d,%d", VER_PRODUCT_VERSION);
	*pbstrVersion = CComBSTR(strVersion).Copy();
	return S_OK;
}

//////////////////////////////////////////////////////////////////////
STDMETHODIMP CPlugin::SpellCheck(BSTR bstrText, int iFlags, BSTR* pbstrChangedText)
{
	CSpell Spell(false/*bUK*/);
	bool bOK = Spell.Init(false/*bCheckFiles*/, IDR_SSCE5332_DLL, IDR_SSCEAM_TLX, IDR_SSCEAM2_CLX, IDR_CORRECT_TLX);
	CString strResult;
	bOK = Spell.CheckString(CString(bstrText), strResult, iFlags);
	*pbstrChangedText = CComBSTR(strResult).Copy();
	return S_OK;
}

//////////////////////////////////////////////////////////////////////
STDMETHODIMP CPlugin::UploadImages(BSTR bstrURL, BSTR bstrForm, int iMaxWidth, int iMaxHeight, int nQuality, int iFlags, BSTR* pbstrResult)
{
	if (IsDebug(iFlags))
		::MessageBox(NULL, CString(bstrForm), "Form Data passed to the plugin", MB_OK|MB_ICONINFORMATION);

	CWaitCursor Wait;
	
	CString strReturn;
	HRESULT hr = UploadForm(CString(bstrURL), CString(bstrForm), iMaxWidth, iMaxHeight, nQuality, iFlags, strReturn);
	*pbstrResult = CComBSTR(strReturn).Copy();
	return S_OK;
}

// Boundary of fields - be sure this string is not In the source file
const LPCSTR Boundary = "7d33a7820054c";
const CString vbCrLf = "\r\n";

/////////////////////////////////////////////////////////////////////////////
HRESULT CPlugin::UploadForm(const CString& strURL, const CString& strFormDataPairs, int iMaxWidth, int iMaxHeight, int nQuality, int iFlags, CString& strReturn)
{
	// Post the form data to the URL
	CComPtr<IWebBrowser2> m_pBrowser;

	// Set the return value in case we leave early
	strReturn = "Error";

	HRESULT hr = m_pBrowser.CoCreateInstance(CComBSTR("InternetExplorer.Application"));
	if (!m_pBrowser)
	{
		if (!IsSilent(iFlags))
		{
			CString strMsg = String("Could not create an instance of the InternetExplorer.Application.\n\n%0xlx", hr);
			::MessageBox(NULL, strMsg, "Error", MB_OK|MB_ICONSTOP);
		}
		return hr;
	}

	HGLOBAL hMemory = PrepareFormData(strFormDataPairs, iMaxWidth, iMaxHeight, nQuality, iFlags);
	if (!hMemory)
		return E_FAIL;

	BYTE* pFormData = (BYTE*)::GlobalLock(hMemory);
	if (!pFormData)
	{
		if (!IsSilent(iFlags))
		{
			CString strMsg = String("Failed to access the form data.");
			::MessageBox(NULL, strMsg, "Error", MB_OK|MB_ICONSTOP);
		}
		return E_FAIL;
	}
	
	// You can uncomment following line to see the results
	m_pBrowser->put_Visible(false);

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
	hr = m_pBrowser->Navigate(CComBSTR(strURL), &varFlags, &varTargetFrameName, &varPostData, &varHeaders);

	for (int i=0; i<10; i++)
	{
		VARIANT_BOOL bBusy = true; // Initialize this to true if you want to wait for the document to load
		while (bBusy)
		{
			m_pBrowser->get_Busy(&bBusy);
	//j		DoEvents();
		}

		CComPtr<IDispatch> pDisp;
		hr = m_pBrowser->get_Document(&pDisp);
		if (pDisp)
		{
			CComQIPtr<IHTMLDocument2> spHTML = pDisp;
			CComPtr<IHTMLElement> pBody;
			hr = spHTML->get_body(&pBody);
			if (pBody)
			{
				CComBSTR bstrBody;
				hr = pBody->get_outerHTML(&bstrBody);
				strReturn = bstrBody;
				if (IsDebug(iFlags))
					::MessageBox(NULL, CString(bstrBody), "Body of the Response", MB_OK|MB_ICONINFORMATION);
				break;
			}
		}

		// Failed to get the document body - loop back around and retry
	}

	m_pBrowser->Quit();
	m_pBrowser.Release();
	m_pBrowser = NULL;

	::GlobalFree(hMemory);

	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
HGLOBAL CPlugin::PrepareFormData(const CString& strFormDataPairs, int iMaxWidth, int iMaxHeight, int nQuality, int iFlags)
{
	IStream* pStream = NULL;
	HRESULT hr = ::CreateStreamOnHGlobal(NULL, false/*fDeleteOnRelease*/, &pStream);
	if (FAILED(hr) || !pStream)
	{
		if (!IsSilent(iFlags))
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
	while (iStart >= 0)
	{
		int iEnd = strFormDataPairs.Find(_T('|'), iStart);
		if (iEnd < 0)
			iEnd = strFormDataPairs.GetLength();
		if (iEnd <= iStart)
			break;
		int iPair = strFormDataPairs.Find(_T(':'), iStart);
		if (iPair > iStart && iPair < iEnd)
		{
			CString strName = strFormDataPairs.Mid(iStart, iPair-iStart);
			strName.TrimLeft();
			strName.TrimRight();
			if (!strName.IsEmpty())
			{
				iPair++;
				CString strValue = strFormDataPairs.Mid(iPair, iEnd-iPair);

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
					CComBSTR bstrFilePathNew;
					HRESULT hr = ResizeJPG(CComBSTR(strValue), iMaxWidth, iMaxHeight, nQuality, iFlags, &bstrFilePathNew);
					bool bFileReturned = (bstrFilePathNew != "");
					if (!bFileReturned)
					{
						pStream->Release();
						return NULL;
					}

					bool bFileCreated = (bstrFilePathNew != CComBSTR(strValue));

					// Add the image width and height to the form
					strValue.Format("%d", m_iResizeWidth);
					FormData += CString("Content-Disposition: form-data; name=\"") + strName + CString("width\"") + vbCrLf + vbCrLf;
					FormData += strValue + vbCrLf;
					FormData += CString("--") + Boundary + vbCrLf;
					strValue.Format("%d", m_iResizeHeight);
					FormData += CString("Content-Disposition: form-data; name=\"") + strName + CString("height\"") + vbCrLf + vbCrLf;
					FormData += strValue + vbCrLf;
					FormData += CString("--") + Boundary + vbCrLf;

					strValue = CString(bstrFilePathNew);
				//j	EscapeUrl(CString(bstrFilePathNew), strValue);
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

					if (bFileCreated && !IsDebug(iFlags) && !IsLeaveTemp(iFlags))
						::DeleteFile(CString(bstrFilePathNew));
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

	if (!hMemory && !IsSilent(iFlags))
	{
		CString strMsg = String("Could not get memory from the stream.");
		::MessageBox(NULL, strMsg, "Error", MB_OK|MB_ICONSTOP);
	}
	return hMemory;
}

//////////////////////////////////////////////////////////////////////
STDMETHODIMP CPlugin::ResizeJPG(BSTR bstrFilePath, int iMaxWidth, int iMaxHeight, int nQuality, int iFlags, BSTR* pbstrFilePathNew)
{
	CWaitCursor Wait;
	
	// Set the return value in case we leave early
	*pbstrFilePathNew = CComBSTR("").Copy();
	m_iResizeWidth = 0;
	m_iResizeHeight = 0;

	// Create an image object
	CString strFilePath = CString(bstrFilePath);
	CImage Image(strFilePath);
	
	// Get a Dib pointer from the image
	BITMAPINFOHEADER* pDibSrc = Image.GetDibPtr();
	if (!pDibSrc)
	{
		if (!IsSilent(iFlags))
		{
			CString strMsg = String("Could not resize image file.\n\n%s", strFilePath);
			::MessageBox(NULL, strMsg, "Error", MB_OK|MB_ICONSTOP);
		}

		return S_OK;
	}

	bool bNoScale = (IsNoUpsize(iFlags) && (pDibSrc->biWidth <= iMaxWidth && pDibSrc->biHeight <= iMaxHeight));

	// Bail (with the original image) if upsizing is not allowed
	if (bNoScale)
	{
		*pbstrFilePathNew = CComBSTR(strFilePath).Copy();
		m_iResizeWidth = pDibSrc->biWidth;
		m_iResizeHeight = pDibSrc->biHeight;
		return S_OK;
	}

	// Copy the source Dib header into a destination Dib
	BITMAPINFOHEADER DibDst = *pDibSrc;

	// Scale the Dib to fit within the maximum rectangle
	double fxScale = (!DibDst.biWidth ? 1.0 : (double)iMaxWidth / DibDst.biWidth);
	double fyScale = (!DibDst.biHeight ? 1.0 : (double)iMaxHeight / DibDst.biHeight);
	double fScale = min(fxScale, fyScale);
	if (bNoScale)
		fScale = 1.0;

	// Make it fit
	DibDst.biWidth  = (long)(fScale * DibDst.biWidth);
	DibDst.biHeight = (long)(fScale * DibDst.biHeight);
	DibDst.biSizeImage = 0;

	BITMAPINFOHEADER* pDibDst = NULL;
	if (bNoScale)
	{
		pDibDst = DibCopy(pDibSrc);
	}
	else
	{
		// Allocate memory for the new Dib
		pDibDst = (BITMAPINFOHEADER*)malloc(DibSize(&DibDst));
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
		if (!IsSilent(iFlags))
		{
			CString strMsg = String("Could not resize image file.\n\n%s", strFilePath);
			::MessageBox(NULL, strMsg, "Error", MB_OK|MB_ICONSTOP);
		}

		return S_OK;
	}

	// Generate a temporary Jpg file name
	CString strFilePathNew = RandomFileName("cr", ".jpg");

	// Convert the Dib in memory to a Jpg
	bool bOK = ConvertDibToJpg(pDibDst, nQuality, strFilePathNew);
	if (!bOK)
	{
		if (!IsSilent(iFlags))
		{
			CString strMsg = String("Could not resize image file.\n\n%s", strFilePath);
			::MessageBox(NULL, strMsg, "Error", MB_OK|MB_ICONSTOP);
		}
		return S_OK;
	}

	*pbstrFilePathNew = CComBSTR(strFilePathNew).Copy();
	m_iResizeWidth = pDibDst->biWidth;
	m_iResizeHeight = pDibDst->biHeight;

	return S_OK;
}

//////////////////////////////////////////////////////////////////////
STDMETHODIMP CPlugin::ColorPicker(int iFormat, BSTR* pbstrColor)
{
	CString strColor;
	CColorPicker ColorPicker;
	ColorPicker.SetFormat(iFormat);
	int retc = ColorPicker.DoModal();
	if (retc == IDOK)
		strColor = ColorPicker.GetColor();
	*pbstrColor = CComBSTR(strColor).Copy();

	return S_OK;
}

//////////////////////////////////////////////////////////////////////
STDMETHODIMP CPlugin::ColorPicker2(int iFormat, BSTR* pbstrColor)
{
	CString strColor;
	CColorPicker2 ColorPicker;
	ColorPicker.SetFormat(iFormat);
	int retc = ColorPicker.DoModal();
	if (retc == IDOK)
		strColor = ColorPicker.GetColor();
	*pbstrColor = CComBSTR(strColor).Copy();

	return S_OK;
}
