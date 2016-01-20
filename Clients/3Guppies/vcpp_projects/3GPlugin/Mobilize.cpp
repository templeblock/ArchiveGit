#include "stdafx.h"
#include "Mobilize.h"
#include "Image.h"
#include "ScaleImage.h"
#include "AGDib.h"
#include "Utility.h"
#include "MyAtlUrl.h"
#include "ConvertDibToJpg.h"
#include <atlctrlx.h>	// CWaitCursor

enum NewBrowserNavConstants
{
	navOpenInNewTab	= 0x800,
	navOpenInBackgroundTab	= 0x1000,
} NewBrowserNavConstants;


// Boundary of fields - be sure this string is not in the source file
const LPCSTR szBoundary = "---------------------------7d71de52e0ac4";
const LPCSTR szCrLf = "\r\n";

#define FL_SILENT		1
#define FL_DEBUG		2
#define FL_RESIZE		4
#define FL_NOUPSIZE		8
#define FL_LEAVETEMP	16
#define FL_BINARYFILES	32
#define DEFAULT_FLAGS	(FL_BINARYFILES|FL_NOUPSIZE);

#define IsSilent(iFlags) (iFlags & FL_SILENT)
#define IsDebug(iFlags) (iFlags & FL_DEBUG)
#define IsResize(iFlags) (iFlags & FL_RESIZE)
#define IsNoUpsize(iFlags) (iFlags & FL_NOUPSIZE)
#define IsLeaveTemp(iFlags) (iFlags & FL_LEAVETEMP)
#define IsBinaryFiles(iFlags) (iFlags & FL_BINARYFILES)

#define NAME_VALUE_SEPARATOR _T(':')
#define NAME_VALUE_TERMINATOR _T('|')
#define FILE_MARKER _T('~')

/////////////////////////////////////////////////////////////////////////////
bool CMobilize::UploadFiles(CSimpleArray<CString> arrFiles)
{
	try
	{
		CString strFilePath = arrFiles[0];
		CString strFormDataPairs;
		// name="MAX_FILE_SIZE" value="57671680"
		strFormDataPairs += CString("MAX_FILE_SIZE") + NAME_VALUE_SEPARATOR + CString("57671680") + NAME_VALUE_TERMINATOR + "\n";
		// name="userfile" type="file"
		strFormDataPairs += CString("userfile") + NAME_VALUE_SEPARATOR + FILE_MARKER + strFilePath + NAME_VALUE_TERMINATOR + "\n";
		// name="EditOptions[Global.NoFormat]" type="checkbox"
		strFormDataPairs += CString("EditOptions[Global.NoFormat]") + NAME_VALUE_SEPARATOR + CString("on") + NAME_VALUE_TERMINATOR + "\n";

		int iFlags = DEFAULT_FLAGS;
		HGLOBAL hFormDataMemory = PrepareFormData(strFormDataPairs, 0/*350 iMaxWidth*/, 0/*206 iMaxHeight*/, 70/*nQuality*/, iFlags);
		if (!hFormDataMemory)
			return false;

		DWORD dwFormDataSize = (DWORD)::GlobalSize(hFormDataMemory);
		BYTE* pFormData = (BYTE*)::GlobalLock(hFormDataMemory);
		CString strURL = "http://www.3guppies.com/ringtones/module/OpenLocker/action/Mobilize";
//j		CString strURL = "http://gg.internal.mixxer.com/~dbartosh/143278/ringtones/module/OpenLocker/action/Mobilize";
		CString strResponseBody;
		bool bOK = UploadForm(strURL, pFormData, dwFormDataSize, iFlags, strResponseBody);
		::GlobalUnlock(hFormDataMemory);
		::GlobalFree(hFormDataMemory);
		return bOK;
	}
	catch (...)
	{
	}

	return true;
}

/////////////////////////////////////////////////////////////////////////////
HGLOBAL CMobilize::PrepareFormData(const CString& strFormDataPairs, int iMaxWidth, int iMaxHeight, int nQuality, int iFlags)
{
	CWaitCursor Wait;
	
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
	
	// Process each name/value pairs in the form string
	CString sFormData;
	int iStart = 0;
	ULONG ulWritten;
	while (iStart >= 0)
	{
		int iEnd = strFormDataPairs.Find(NAME_VALUE_TERMINATOR, iStart);
		if (iEnd < 0)
			iEnd = strFormDataPairs.GetLength();
		if (iEnd <= iStart)
			break;
		int iPair = strFormDataPairs.Find(NAME_VALUE_SEPARATOR, iStart);
		if (iPair > iStart && iPair < iEnd)
		{
			CString strName = strFormDataPairs.Mid(iStart, iPair-iStart);
			strName.TrimLeft();
			strName.TrimRight();
			if (!strName.IsEmpty())
			{
				iPair++;
				CString strValue = strFormDataPairs.Mid(iPair, iEnd-iPair);

				bool bValueIsFileName = (strValue.Find(FILE_MARKER) == 0);
				if (bValueIsFileName)
					strValue = strValue.Mid(1);

				if (!bValueIsFileName)
				{
					sFormData += CString("--") + szBoundary + szCrLf;
					sFormData += CString("Content-Disposition: form-data; name=\"") + strName + CString("\"") + szCrLf;
//j					sFormData += CString("Content-Type: text/plain; charset=utf-8") + szCrLf;
					sFormData += szCrLf;
					sFormData += strValue + szCrLf;
				}
				else
				{
					bool bFileCreated = false;
					bool bIsImage = false;
					CString strFilePath = strValue;
					CString strExtension = StrExtension(strFilePath);
					CString strContentType = GetContentType(strExtension, bIsImage);
					if (bIsImage && IsResize(iFlags))
					{
						CString strFilePathNew;
						int iResizeWidth = 0;
						int iResizeHeight = 0;
						if (ResizeJPG(strFilePath, iMaxWidth, iMaxHeight, nQuality, iFlags, strFilePathNew, iResizeWidth, iResizeHeight))
						{
							bFileCreated = (strFilePathNew.CompareNoCase(strFilePath) != 0);
							strFilePath = strFilePathNew;

							// Add the image width and height to the form
							strValue.Format("%d", iResizeWidth);
							sFormData += CString("--") + szBoundary + szCrLf;
							sFormData += CString("Content-Disposition: form-data; name=\"") + strName + CString("width\"") + szCrLf;
//j							sFormData += CString("Content-Type: text/plain; charset=utf-8") + szCrLf;
							sFormData += szCrLf;
							sFormData += strValue + szCrLf;

							strValue.Format("%d", iResizeHeight);
							sFormData += CString("--") + szBoundary + szCrLf;
							sFormData += CString("Content-Disposition: form-data; name=\"") + strName + CString("height\"") + szCrLf;
//j							sFormData += CString("Content-Type: text/plain; charset=utf-8") + szCrLf;
							sFormData += szCrLf;
							sFormData += strValue + szCrLf;
						}
					}

					strValue = strFilePath;  //EscapeUrl(strFilePath, strValue);
					sFormData += CString("--") + szBoundary + szCrLf;
					sFormData += CString("Content-Disposition: form-data; name=\"") + strName + CString("\"; filename=\"") + strValue + CString("\"") + szCrLf;
					if (!strContentType.IsEmpty())
						sFormData += CString("Content-Type: ") + strContentType + szCrLf;
//j					if (IsBinaryFiles(iFlags))
//j						sFormData += CString("Content-Transfer-Encoding: binary") + szCrLf;
					sFormData += szCrLf;
					ULONG ulBytes = sFormData.GetLength();
					hr = pStream->Write(sFormData, ulBytes, &ulWritten);
					sFormData.Empty();

					HGLOBAL hFileMemory = ReadFileIntoMemory(strValue);
					if (hFileMemory)
					{
						BYTE* pFileMemory = (BYTE*)::GlobalLock(hFileMemory);
						if (pFileMemory)
						{
							if (!IsBinaryFiles(iFlags))
								int i=0;
							ULONG ulBytes = (ULONG)::GlobalSize(hFileMemory);
							hr = pStream->Write(pFileMemory, ulBytes, &ulWritten);
							::GlobalUnlock(hFileMemory);
						}

						::GlobalFree(hFileMemory);
					}

					sFormData += szCrLf;

					if (bFileCreated && !IsLeaveTemp(iFlags))
						::DeleteFile(strFilePath);
				}
			}
		}

		iStart = iEnd + 1;
	}

	sFormData += CString("--") + szBoundary + CString("--") + szCrLf;
	sFormData += szCrLf;
	ULONG ulBytes = sFormData.GetLength();
	hr = pStream->Write(sFormData, ulBytes, &ulWritten);

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

/////////////////////////////////////////////////////////////////////////////
bool CMobilize::UploadForm(const CString& strURL, BYTE* pFormData, DWORD dwFormDataSize, int iFlags, CString& strResponseBody)
{
	CWaitCursor Wait;
	
	if (!pFormData || !dwFormDataSize)
	{
		if (!IsSilent(iFlags))
		{
			CString strMsg = String("Failed to access the form data.");
			::MessageBox(NULL, strMsg, "Error", MB_OK|MB_ICONSTOP);
		}

		return false;
	}
	
	if (IsDebug(iFlags))
		::MessageBox(NULL, (LPTSTR)pFormData, "Form Data", MB_OK|MB_ICONINFORMATION);

	CComPtr<IWebBrowser2> spWebBrowser2;
	HRESULT hr = spWebBrowser2.CoCreateInstance(CLSID_InternetExplorer) ; // (CComBSTR("InternetExplorer.Application"));
	if (FAILED(hr) || !spWebBrowser2)
	{
		if (!IsSilent(iFlags))
		{
			CString strMsg = String("Could not create an instance of Internet Explorer.\n\n%0xlx", hr);
			::MessageBox(NULL, strMsg, "Error", MB_OK|MB_ICONSTOP);
		}

		return false;
	}

	// Send the form data to the URL as a POST request
	CComVariant varFlags;
	varFlags.vt = VT_I4;
	varFlags = navNoHistory; //| navOpenInNewTab | navOpenInBackgroundTab | navBrowserBar;
	CComVariant varTargetName; // = "_self" "_blank";

#ifdef ALT
	CComSafeArray<BYTE> byteArray;
	byteArray.Add(dwFormDataSize, pFormData);
	CComVariant varPostData = byteArray;
#else
	CComSafeArray<BYTE> byteArray(dwFormDataSize);
	::CopyMemory(((LPSAFEARRAY)byteArray)->pvData, pFormData, dwFormDataSize);
	CComVariant varPostData;
	varPostData.vt = VT_ARRAY | VT_UI1;
	varPostData.parray = byteArray.Detach();
#endif ALT

	// For simple form data, use the following
	//pFormData = "pn1605=Photo+Album+Title&pn1629=Photo+Caption+1&pn1631=Photo+Caption+2&rights=Y&agree=Y&mode=submit&FilePers=1628&CapPers=1629&TitlePers=1605";
	//CComVariant varHeaders = CComBSTR(CString("Content-Type: application/x-www-form-urlencoded") + szCrLf);

	CComVariant varHeaders = CComBSTR(
		CString("Referer: ") + strURL + szCrLf +
		CString("Content-Type: multipart/form-data; boundary=") + szBoundary + szCrLf +
		szCrLf);
	hr = spWebBrowser2->Navigate(CComBSTR(strURL), &varFlags, &varTargetName, &varPostData, &varHeaders);
	hr = spWebBrowser2->put_Visible(true);

	// Bring the newly launched window into the foreground
	HWND hWnd = NULL;
	hr = spWebBrowser2->get_HWND((LONG_PTR*)&hWnd);
	if (hWnd)
	{
		// Find the parent application
		HWND hWndParent = NULL;;
		while (hWndParent = ::GetParent(hWnd))
			hWnd = hWndParent;
		::SetForegroundWindow(hWnd);
	}

	VARIANT_BOOL bBusy = true; // Initialize this to true if you want to wait for the document to load
	int iTimeToPoll = 500;
	while (bBusy)
	{
		// Wait for a few second timeout, or for any notification messages
		DWORD dwEventTriggered = ::MsgWaitForMultipleObjects(0/*nCount*/, NULL/*pHandles*/, false/*fWaitAll*/, iTimeToPoll, QS_ALLINPUT|QS_ALLPOSTMESSAGE);
		if (dwEventTriggered != WAIT_TIMEOUT) // such as WAIT_OBJECT_0...
			int i = 0;// a Windows message was received;

		READYSTATE ReadyState = READYSTATE_UNINITIALIZED;
		hr = spWebBrowser2->get_ReadyState(&ReadyState);
		bBusy = (ReadyState != READYSTATE_COMPLETE && ReadyState != READYSTATE_UNINITIALIZED);
//j		hr = spWebBrowser2->get_Busy(&bBusy);
	}

	if (IsDebug(iFlags))
	{
		// Get the current document object from browser
		CComPtr<IDispatch> spDispDoc;
		hr = spWebBrowser2->get_Document(&spDispDoc);
		CComQIPtr<IHTMLDocument2> spHTMLDocument2 = spDispDoc;
		if (spHTMLDocument2)
		{
			CComPtr<IHTMLElement> spBody;
			hr = spHTMLDocument2->get_body(&spBody);
			if (spBody)
			{
				CComBSTR bstrBody;
				hr = spBody->get_outerHTML(&bstrBody);
				strResponseBody = bstrBody;
				::MessageBox(NULL, strResponseBody, "Body of the Response", MB_OK|MB_ICONINFORMATION);
			}
		}
	}

	hr = spWebBrowser2->put_Visible(true);
//j	hr = spWebBrowser2->Quit();
	spWebBrowser2.Release();

	return true;
}

//////////////////////////////////////////////////////////////////////
bool CMobilize::ResizeJPG(const CString& strFilePath, int iMaxWidth, int iMaxHeight, int nQuality, int iFlags, CString& strFilePathNew, int& iResizeWidth, int& iResizeHeight)
{
	CWaitCursor Wait;
	
	// Set the return value in case we leave early
	strFilePathNew.Empty();
	iResizeWidth = 0;
	iResizeHeight = 0;

	// Create an image object
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

		return false;
	}

	bool bNoScale = (IsNoUpsize(iFlags) && (pDibSrc->biWidth <= iMaxWidth && pDibSrc->biHeight <= iMaxHeight));

	// Bail (with the original image) if upsizing is not allowed
	if (bNoScale)
	{
		strFilePathNew = strFilePath;
		iResizeWidth = pDibSrc->biWidth;
		iResizeHeight = pDibSrc->biHeight;
		return true;
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

		return false;
	}

	// Generate a temporary Jpg file name
	CString strFilePathTemp = RandomFileName("cr", ".jpg");

	// Convert the Dib in memory to a Jpg
	bool bOK = ConvertDibToJpg(pDibDst, nQuality, strFilePathTemp);
	if (!bOK)
	{
		if (!IsSilent(iFlags))
		{
			CString strMsg = String("Could not resize image file.\n\n%s", strFilePath);
			::MessageBox(NULL, strMsg, "Error", MB_OK|MB_ICONSTOP);
		}

		return false;
	}

	strFilePathNew = strFilePathTemp;
	iResizeWidth = pDibDst->biWidth;
	iResizeHeight = pDibDst->biHeight;
	return true;
}

//////////////////////////////////////////////////////////////////////
CString CMobilize::GetContentType(const CString& strExtension, bool& bIsImage)
{
	bIsImage = false;

	CString strContentType;
	// Audio files
		 if (strExtension.Find(".mp3")  >= 0)	strContentType = "audio/mpeg";
	else if (strExtension.Find(".wma")  >= 0)	strContentType = "audio/x-ms-wma";
	else if (strExtension.Find(".aac")  >= 0)	strContentType = "application/octet-stream";
	else if (strExtension.Find(".wav")  >= 0)	strContentType = "audio/wav";
	else if (strExtension.Find(".smaf") >= 0)	strContentType = "audio/smaf";
	else if (strExtension.Find(".mel")  >= 0)	strContentType = "audio/imelody";
	else if (strExtension.Find(".qcp")  >= 0)	strContentType = "audio/vnd.qcelp";
	else if (strExtension.Find(".awb" ) >= 0)	strContentType = "audio/amr";
	else if (strExtension.Find(".m4a")  >= 0)	strContentType = "audio/mp4";
	else if (strExtension.Find(".m4p")  >= 0)	strContentType = "audio/mp4";
	else if (strExtension.Find(".mid")  >= 0)	strContentType = "audio/midi";
	else if (strExtension.Find(".ogg")  >= 0)	strContentType = "application/ogg";
	else if (strExtension.Find(".rm")   >= 0)	strContentType = "audio/x-realaudio";

	// Video files
	else if (strExtension.Find(".flv")  >= 0)	strContentType = "video/x-flv";
	else if (strExtension.Find(".asf")  >= 0)	strContentType = "video/x-ms-asf";
	else if (strExtension.Find(".wmv")  >= 0)	strContentType = "video/x-ms-wmv";
	else if (strExtension.Find(".avi")  >= 0)	strContentType = "video/avi";
	else if (strExtension.Find(".mov")  >= 0)	strContentType = "video/quicktime";
	else if (strExtension.Find(".mp4")  >= 0)	strContentType = "video/mp4";
	else if (strExtension.Find(".mpg")  >= 0)	strContentType = "video/mpeg";
	else if (strExtension.Find(".mpeg") >= 0)	strContentType = "video/mpeg";
	else if (strExtension.Find(".qt")   >= 0)	strContentType = "video/quicktime";
	else if (strExtension.Find(".3gp")  >= 0)	strContentType = "video/3gpp";
	else if (strExtension.Find(".ogm")  >= 0)	strContentType = "application/ogg";

	// Image files
	else if (strExtension.Find(".gif")  >= 0)	{ bIsImage = true; strContentType = "image/gif"; }
	else if (strExtension.Find(".jpg")  >= 0)	{ bIsImage = true; strContentType = "image/jpeg"; }
	else if (strExtension.Find(".jpeg") >= 0)	{ bIsImage = true; strContentType = "image/jpeg"; }
	else if (strExtension.Find(".png")  >= 0)	{ bIsImage = true; strContentType = "image/png"; }
	else if (strExtension.Find(".dib")  >= 0)	{ bIsImage = true; strContentType = "image/bmp"; }
	else if (strExtension.Find(".bmp")  >= 0)	{ bIsImage = true; strContentType = "image/bmp"; }
	else if (strExtension.Find(".qtif") >= 0)	{ bIsImage = true; strContentType = "image/qtif"; }

	// Other file types (for completeness)
//j	else if (strExtension.Find(".exe")  >= 0)	strContentType = "application/exe";
	else if (strExtension.Find(".jad")  >= 0)	strContentType = "text/plain";
	else if (strExtension.Find(".jar")  >= 0)	strContentType = "application/zip";
	else if (strExtension.Find(".swf")  >= 0)	strContentType = "application/x-shockwave-flash";
	else if (strExtension.Find(".txt")  >= 0)	strContentType = "text/plain";

	return strContentType;
}
