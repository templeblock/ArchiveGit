// Plugin.cpp : Implementation of CPlugin
#include "stdafx.h"
#include "Plugin.h"
#include "Utility.h"
#include "MyAtlUrl.h"
#include "Version.h"
#include <shellapi.h>

#define FL_SILENT		1
#define FL_DEBUG		2
#define FL_NOUPSIZE		4
#define FL_LEAVETEMP	8

#define IsSilent(iFlags) (iFlags & FL_SILENT)
#define IsDebug(iFlags) (iFlags & FL_DEBUG)
#define IsNoUpsize(iFlags) (iFlags & FL_NOUPSIZE)
#define IsLeaveTemp(iFlags) (iFlags & FL_LEAVETEMP)

static int m_iVideoLength;


//////////////////////////////////////////////////////////////////////
STDMETHODIMP CPlugin::Version(BSTR* pbstrVersion)
{
	CString strVersion;
	strVersion.Format("%d,%d,%d,%d", VER_PRODUCT_VERSION);
	*pbstrVersion = CComBSTR(strVersion).Copy();
	return S_OK;
}

//////////////////////////////////////////////////////////////////////
STDMETHODIMP CPlugin::UploadVideo(BSTR bstrURL, BSTR bstrForm, BSTR bstrOptions, int iFlags, BSTR* pbstrResult)
{
	if (IsDebug(iFlags)) {
		::MessageBox(NULL, CString(bstrForm), "Form Data passed to the plugin", MB_OK|MB_ICONINFORMATION);
	    ::MessageBox(NULL, CString(bstrOptions), "Options passed to the plugin", MB_OK|MB_ICONINFORMATION);
	}

	CWaitCursor Wait;
	
	CString strReturn;
	HRESULT hr = UploadForm(CString(bstrURL), CString(bstrForm), CString(bstrOptions), iFlags, strReturn);
	*pbstrResult = CComBSTR(strReturn).Copy();
	return S_OK;
}

// Boundary of fields - be sure this string is not In the source file
const LPCSTR Boundary = "7d33a7820054c";
const CString vbCrLf = "\r\n";

/////////////////////////////////////////////////////////////////////////////
HRESULT CPlugin::UploadForm(const CString& strURL, const CString& strFormDataPairs, const CString& strOptions, int iFlags, CString& strReturn)
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

	HGLOBAL hMemory = PrepareFormData(strFormDataPairs, strOptions, iFlags);
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
HGLOBAL CPlugin::PrepareFormData(const CString& strFormDataPairs, const CString& strOptions, int iFlags)
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
					HRESULT hr = PrepareVideo(CComBSTR(strValue), CComBSTR(strOptions), iFlags, &bstrFilePathNew);
					bool bFileReturned = (bstrFilePathNew != "");
					if (!bFileReturned)
					{
						pStream->Release();
						return NULL;
					}

					bool bFileCreated = (bstrFilePathNew != CComBSTR(strValue));

					// Add the video length to the form
					strValue.Format("%d", m_iVideoLength);
					FormData += CString("Content-Disposition: form-data; name=\"") + strName + CString("length\"") + vbCrLf + vbCrLf;
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
STDMETHODIMP CPlugin::PrepareVideo(BSTR bstrFilePath, BSTR bstrOptions, int iFlags, BSTR* pbstrFilePathNew)
{
	// this path has to be consistent with installshield's installation path
	CString ffmpegPath = CString("c:\\Program Files\\Creative Toolbox Video Plug-In\\ffmpeg.exe");
	m_iVideoLength = 0;

	CString strOptions = CString(bstrOptions);

	CString strFilePath = CString(bstrFilePath);
	// Generate a temporary flv file name
	CString strFilePathNew = RandomFileName("cr", ".flv");
	*pbstrFilePathNew = CComBSTR(strFilePathNew).Copy();

	//Generate a temporary name for log file
	CString strFilePathLog = RandomFileName("ffm", ".log");

	// param is what gets passed to ffmpeg
	CString param;
	param.Format(" -stderrlogfile %s -i \"%s\" %s %s", strFilePathLog, CString(bstrFilePath), strOptions, strFilePathNew);
	if (IsDebug(iFlags))
		::MessageBox(NULL, param, "params passed to ffmpeg", MB_OK);
    
	BOOL bFFMPEG=false;

   	SHELLEXECUTEINFO ExecuteInfo;
    memset(&ExecuteInfo, 0, sizeof(ExecuteInfo));  
    ExecuteInfo.cbSize       = sizeof(ExecuteInfo);
    ExecuteInfo.fMask        = SEE_MASK_NOCLOSEPROCESS ;//this process is compulsary to receive process handle.;                
    ExecuteInfo.hwnd         = 0;                
    ExecuteInfo.lpVerb       = "open";     // Operation to perform
	ExecuteInfo.lpFile       = ffmpegPath; //Application name
	ExecuteInfo.lpParameters = param;
    ExecuteInfo.lpDirectory  = 0;         // Default directory
	ExecuteInfo.nShow        = SW_HIDE;
    ExecuteInfo.hInstApp     = 0;
    
	if (ShellExecuteEx(&ExecuteInfo)) {
		DWORD dwret=::WaitForSingleObject(ExecuteInfo.hProcess,INFINITE); //this thread will wait till the process is completed
		if (dwret != WAIT_OBJECT_0) {
			if (IsDebug(iFlags))
				::MessageBox(NULL, "PrepareVideo WaitForSingleObject return failed.", "Error", MB_OK|MB_ICONSTOP);
		} else {
			DWORD dwexitcode;
			BOOL bres = GetExitCodeProcess(ExecuteInfo.hProcess,&dwexitcode);		
			if (!bres) {
				if (IsDebug(iFlags)) {
					::MessageBox(NULL, "PrepareVideo GetExitCodeProcess return failed", "Error", MB_OK|MB_ICONSTOP);
				}
			} else {
				if (dwexitcode != 0) {
					if (IsDebug(iFlags)) {
						TCHAR exitmsgbuf[1024];
						wsprintf(exitmsgbuf,"PrepareVideo GetExitCode Process return exitcode:%d",dwexitcode);
						::MessageBox(NULL, exitmsgbuf, "Error", MB_OK|MB_ICONSTOP);
					}
				} else {
					ScanForVideoLength(strFilePathLog, iFlags);
					if (!IsDebug(iFlags) && !IsLeaveTemp(iFlags))
						::DeleteFile(CString(strFilePathLog));
					//check file size to be > 0
					HANDLE hFile = ::CreateFile(strFilePathNew, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
					if (hFile != INVALID_HANDLE_VALUE) {
						DWORD dwSize = ::GetFileSize(hFile, NULL);
						if (dwSize > 0)
							bFFMPEG = true;
					}
					::CloseHandle(hFile);

				}
			} 
		}
		CloseHandle(ExecuteInfo.hProcess);
	}
	
	if (!bFFMPEG) {
		*pbstrFilePathNew = NULL; 
		CString strMsg = String("Could not upload your video.");
		::MessageBox(NULL, strMsg, "Error", MB_OK|MB_ICONSTOP);
	}
	if (IsDebug(iFlags))
		::MessageBox(NULL, String("PrepareVideo returns."), "INFO", MB_OK);
	return S_OK;
}
HRESULT CPlugin::ScanForVideoLength(const CString& strFilePathLog, int iFlags)
{
	//read log file and parse out the length of the video
	HGLOBAL hLogMem = ReadFileIntoMemory(strFilePathLog);
	if (hLogMem) {
		BYTE* pLogFileMemory = (BYTE*)::GlobalLock(hLogMem);
		if (pLogFileMemory)
		{
			CString logStr = CString(pLogFileMemory);
			if (IsDebug(iFlags))
				::MessageBox(NULL, logStr, "Log file content", MB_OK);
			
			int idx;							
			CString restOfStr = logStr;
			while (restOfStr.Find(TEXT("time=")) > -1) {
				idx = restOfStr.Find(TEXT("time="));
				restOfStr = restOfStr.Mid(idx+1);
			}
			sscanf(restOfStr, "ime=%i", &m_iVideoLength);
			::GlobalUnlock(hLogMem);
			if (IsDebug(iFlags))
				::MessageBox(NULL, restOfStr, "INFO", MB_OK);						
		}
		::GlobalFree(hLogMem);
	}
	return S_OK;
}