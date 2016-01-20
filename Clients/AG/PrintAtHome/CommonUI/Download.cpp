#include "stdafx.h"
#include "Download.h"
#include "MessageBox.h"
#include "Internet.h"
#include "Ctp.h"
#include <strstream>

#ifdef _DEBUG
	#define new DEBUG_NEW
#endif _DEBUG

/////////////////////////////////////////////////////////////////////////////
CDownload::CDownload()
{
	m_pClientSite = NULL;
	m_iAppType = -1;
	m_iFileType = -1;
	m_fnCallback = NULL;
	m_lParam = NULL;
	m_pData = NULL;
	m_dwSize = 0;
	m_List.clear();
	m_bGoOnline = false;
	m_bShowProgress = false;

	m_pDataSaved = NULL;
	m_dwSizeSaved = 0;
	m_fnCallbackSaved = NULL;
}

/////////////////////////////////////////////////////////////////////////////
CDownload::~CDownload()
{
	Free();
	FreeSavedData();
}

/////////////////////////////////////////////////////////////////////////////
void CDownload::SetAppInfo(IOleClientSite* pClientSite, int iAppType)
{
	m_pClientSite = pClientSite;
	m_iAppType = iAppType;
}

/////////////////////////////////////////////////////////////////////////////
void CDownload::Free()
{
	m_ProgressDialog.Destroy();
    m_bShowProgress = false;

	m_iFileType = -1;
	m_fnCallback = NULL;
	m_lParam = NULL;

	if (m_pData)
		free(m_pData);

	m_pData = NULL;
	m_dwSize = 0;
	m_List.clear();
}

/////////////////////////////////////////////////////////////////////////////
void CDownload::FreeSavedData()
{
	if (m_pDataSaved)
		free(m_pDataSaved);

	m_pDataSaved = NULL;
	m_dwSizeSaved = 0;
	m_fnCallbackSaved = NULL;
}

/////////////////////////////////////////////////////////////////////////////
bool CDownload::Init(int iFileType, FNDOWNLOADCALLBACK fnCallback, LPARAM lParam, DWORD dwFlags)
{
	if (InProgress())
	{
		if (!(dwFlags & DL_PROMPTUSER))
			return !!(dwFlags & DL_KILLINPROGRESS);

		if (CMessageBox::Message("Download already in progress.\n\nDo you want to abort the current download?", MB_YESNO) == IDNO)
			return false;
	}
	
	Free();

	m_iFileType = iFileType;
	m_fnCallback = fnCallback;
	m_lParam = lParam;
	return true;
}

/////////////////////////////////////////////////////////////////////////////
void CDownload::AddFile(CString& strSrcURL, CString& strDstPath, int iData)
{
	CString strSrcPath = strSrcURL;
	//j Temporary
	//if (strSrcPath.Find("stage.") >= 0)
		//Replace(strSrcPath, "stage.", "agstage:t3stm3@stage.");

	DOWNLOAD d;
	d.strSrcURL = strSrcPath;
	d.strDstPath = strDstPath;
	d.iData = iData;
	d.pData = NULL;
	d.dwSize = 0;
	m_List.push_back(d);
}

/////////////////////////////////////////////////////////////////////////////
bool CDownload::Start(bool bGoOnline, LPCTSTR szProgressTitle)
{
	m_bGoOnline = bGoOnline;

	if (szProgressTitle)
	{
		bool bOK = m_ProgressDialog.Create();
		if (bOK)
		{
			m_bShowProgress = true;
			m_ProgressDialog.SetTitle(szProgressTitle);
		//j	m_ProgressDialog.SetAnimation(_AtlBaseModule.GetResourceInstance(), IDR_PROGRESS);
			m_ProgressDialog.SetCancelMessage("Download is being cancelled...");
			m_ProgressDialog.ShowModeless(NULL/*hwndParentWindow*/);
		}
	}
	if (Next())
		return true;

	CMessageBox::Message("Error starting the download.  Please try again.");
	Free();
	return false;
}

/////////////////////////////////////////////////////////////////////////////
bool CDownload::Next()
{
	if (m_List.empty())
		return false;

	DOWNLOAD& Download = m_List.front();
	CString strSrcURL = Download.strSrcURL;
	if (strSrcURL.GetLength() <= 0)
		return false;

	if (m_bShowProgress)
	{
		m_ProgressDialog.SetLineText(1/*dwLine*/, strSrcURL);
	    m_ProgressDialog.UpdateProgress(0, 100/*dwMax*/);
		m_ProgressDialog.ResetTimer();
	}

	if (m_bGoOnline)
	{
		CInternet Internet;
		if (!Internet.GoOnline(strSrcURL, NULL/*hwndParentWindow*/, false/*bForceLANOnlineSilently*/))
			return false;
	}

	
	CBindStatusCallback2<CCtp>::Download(CComBSTR(strSrcURL), this, m_pClientSite);

	return true;
}

/////////////////////////////////////////////////////////////////////////////
HRESULT CDownload::OnData(CString& strSrcURL, BYTE* pBytes, DWORD dwSize)
{
	if (pBytes)
		FileData(pBytes, dwSize);
	else
	{
		HRESULT hr = (HRESULT)dwSize;
		bool bError = FAILED(hr);
		if (bError && hr != E_ABORT)
			CMessageBox::Message(String("%s (0x%lx)\nError downloading %s.", HRString(hr), hr, strSrcURL));

		FileEnd(bError);
	}

	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
HRESULT CDownload::OnProgress(ULONG ulProgress, ULONG ulProgressMax, ULONG ulStatusCode, LPCWSTR szStatusText)
{
	static bool bUsingCache = false;
	bool bEndOperation = false;
	bool bBeginOperation = false;
	LPCSTR strCode = NULL;
	switch (ulStatusCode)
	{
		case BINDSTATUS_FINDINGRESOURCE:		   strCode = "Finding resource"; break;
		case BINDSTATUS_CONNECTING:				   strCode = "Connecting"; break; //!
		case BINDSTATUS_REDIRECTING:			   strCode = "Redirecting"; break;
		case BINDSTATUS_BEGINDOWNLOADDATA:		   strCode = (bUsingCache ? "Copying from cache" : "Downloading data"); bBeginOperation = true; break; //!
		case BINDSTATUS_DOWNLOADINGDATA:		   strCode = (bUsingCache ? "Copying from cache" : "Downloading data"); break; //!
		case BINDSTATUS_ENDDOWNLOADDATA:		   strCode = (bUsingCache ? "Copying from cache" : "Downloading data"); bEndOperation = true; break; //!
		case BINDSTATUS_BEGINDOWNLOADCOMPONENTS:   strCode = "Installing components"; bBeginOperation = true; break;
		case BINDSTATUS_INSTALLINGCOMPONENTS:	   strCode = "Installing components"; break;
		case BINDSTATUS_ENDDOWNLOADCOMPONENTS:	   strCode = "Installing components"; bEndOperation = true; break;
		case BINDSTATUS_USINGCACHEDCOPY:		   strCode = NULL; bUsingCache = true; break;
		case BINDSTATUS_SENDINGREQUEST:			   strCode = "Sending request"; break; //!
		case BINDSTATUS_CLASSIDAVAILABLE:		   strCode = "Classid available"; break;
		case BINDSTATUS_MIMETYPEAVAILABLE:		   strCode = NULL; break; //!
		case BINDSTATUS_CACHEFILENAMEAVAILABLE:	   strCode = NULL; break; //!
		case BINDSTATUS_BEGINSYNCOPERATION:		   strCode = "Sync operation"; bBeginOperation = true; break;
		case BINDSTATUS_ENDSYNCOPERATION:		   strCode = "Sync operation"; bEndOperation = true; break;
		case BINDSTATUS_BEGINUPLOADDATA:		   strCode = "Uploading data"; bBeginOperation = true; break;
		case BINDSTATUS_UPLOADINGDATA:			   strCode = "Uploading data"; break;
		case BINDSTATUS_ENDUPLOADDATA:			   strCode = "Uploading data"; bEndOperation = true; break;
		case BINDSTATUS_PROTOCOLCLASSID:		   strCode = "Protocol classid"; break;
		case BINDSTATUS_ENCODING:				   strCode = "Encoding"; break;
		case BINDSTATUS_VERIFIEDMIMETYPEAVAILABLE: strCode = "Verified mime type available"; break;
		case BINDSTATUS_CLASSINSTALLLOCATION:	   strCode = "Class install location"; break;
		case BINDSTATUS_DECODING:				   strCode = "Decoding"; break;
		case BINDSTATUS_LOADINGMIMEHANDLER:		   strCode = "Loading mime handler"; break;
	};

	if (bEndOperation)
		bUsingCache = false;

	if (!strCode)
		return S_OK;

	CString strMessage = strCode;
	int iValue = 0;
	if (!ulProgressMax)
		strMessage += "...";
	else
	{
		if (!ulProgressMax) ulProgressMax = 1;
		iValue = ((ulProgress * 100) + 50) / ulProgressMax;
		strMessage += String(" %d%%", iValue);
	}

	// Output the message
	bool bAbort = false;
	if (m_bShowProgress)
	{
		if (m_ProgressDialog.HasUserCanceled())
		{
			Free();
			bAbort = true; // Abort the current download
		}
		else
		{
			m_ProgressDialog.SetLineText(2/*dwLine*/, strMessage);
			m_ProgressDialog.UpdateProgress(iValue);
		}
	}

	CCtp* pCtp = (CCtp*)m_lParam;
	if (pCtp && pCtp->GetDocWindow() && pCtp->GetDocWindow()->IsProgressDlgUp())
	{
		pCtp->GetDocWindow()->DrawProgressDlg(strMessage, false/*bCreateIfNeeded*/);

		if (bEndOperation)
			pCtp->GetDocWindow()->DrawProgressDlg("Download Complete.\nYour project will appear in a moment...", false/*bCreateIfNeeded*/);
	}

	return (!bAbort ? S_OK : E_ABORT);
}

/////////////////////////////////////////////////////////////////////////////
void CDownload::FileData(BYTE* pBytes, DWORD dwLen)
{
	if (!InProgress())
		return;
	if (!pBytes || !dwLen)
		return;

	if (!m_pData)
		m_pData = (BYTE*)malloc(dwLen);
	else
		m_pData = (BYTE*)realloc(m_pData, m_dwSize + dwLen);

	if (!m_pData)
		return;

	::memcpy(m_pData + m_dwSize, pBytes, dwLen);
	m_dwSize += dwLen;
}

/////////////////////////////////////////////////////////////////////////////
void CDownload::FileEnd(bool bError)
{
	if (m_bShowProgress)
	{
	    m_ProgressDialog.Destroy();
	    m_bShowProgress = false;
	}

	if (!InProgress())
		return;

#ifdef NOTUSED //j Code to examine the data for the MIME type
	CComObject<CAsyncDownload>* pbsc = NULL;
	HRESULT hr = CComObject<CAsyncDownload>::CreateInstance(&pbsc);
	if (SUCCEEDED(hr))
	{
		LPWSTR pMimeOut = NULL;
		hr = ::FindMimeFromData(pbsc->m_spBindCtx,
			NULL/*pwzUrl*/,
			m_pData/*pBuffer*/,
			m_dwSize/*cbSize*/,
			NULL/*pwzMimeProposed*/,
			0/*dwMimeFlags*/,
			&pMimeOut/*ppwzMimeOut*/,
			0/*dwReserved*/);

		int i = 0;
	}
#endif NOTUSED

	bool bPickingUpLeftovers = false;
	if (m_iFileType > 0) // Anything but a project file
	{
		// Is there any data to process?
		if (m_pData)
		{
			FileSave();
			free(m_pData);
			m_pData = NULL;
			m_dwSize = 0;
		}

		// Pop the download item off the list
		m_List.erase(m_List.begin());

		// Are there any more files in the list to download?
		if (!bError && Next())
			return; // Yes, more files to download

		// No more files to download, so make the final callback after we do the Free()
		FNDOWNLOADCALLBACK fnCallback = m_fnCallback;
		LPARAM lParam = m_lParam;
		if (!m_pDataSaved) // Is there any leftover project data to pick up?
			Free();
		if (fnCallback)
			fnCallback(lParam, NULL);
		if (!m_pDataSaved) // Is there any leftover project data to pick up?
			return; // No more files to download or process

		// Fall through to pick up any leftover project data
		bPickingUpLeftovers = true;
		m_pData = m_pDataSaved; m_pDataSaved = NULL;
		m_dwSize = m_dwSizeSaved; m_dwSizeSaved = 0;
		m_fnCallback = m_fnCallbackSaved; m_fnCallbackSaved = NULL;
		m_iFileType = 0; // A project file
	}

	// If this is not a project file, we are done
	if (m_iFileType != 0)
		return;

	// Is there any data to process?
	if (!m_pData)
	{
		Free();
		return;
	}

	CAGDoc* pAGDoc = new CAGDoc();
	if (!pAGDoc)
	{
		Free();
		return;
	}

	// Set the Context License for the document. This must be called 
	// prior to Read() below.
	CCtp* pCtp = (CCtp*)m_lParam;
	int iContextLicense = pCtp->GetContextLicense();
	pAGDoc->SetContextLicense(iContextLicense);

	istrstream input((char*)m_pData, m_dwSize);
	bool bAdjusted = false;
	if (!pAGDoc->Read(input, bAdjusted))
	{
		delete pAGDoc;
		CMessageBox::Message("Error reading the project data.");
		Free();
		return;
	}

	// See if we need to start downloading any fonts
	if (!bPickingUpLeftovers)
	{
		// Pop the download item off the list
		m_List.erase(m_List.begin());

		LOGFONTLIST lfProjectFontList;
		pAGDoc->GetFonts(lfProjectFontList);

		FONTDOWNLOADLIST List;
		pCtp->GetFontList().GetProjectFontList(lfProjectFontList, List, true/*bMissingOnly*/);
		if (!List.empty())
		{
			delete pAGDoc;

			// Save the project data so we can pick it up later
			m_pDataSaved = m_pData; m_pData = NULL;
			m_dwSizeSaved = m_dwSize; m_dwSize = 0;
			m_fnCallbackSaved = m_fnCallback; m_fnCallback = NULL;

			// A whole new Init() process will begin, with the saved data being picked up when it's over
			Free();

			// Download the fonts
			pCtp->FontListDownload(List);
			return;
		}
	}

	// Set the document name
	CComBSTR bstrSrcURL;
	pCtp->GetSrc(&bstrSrcURL);
	pAGDoc->SetFileName(CString(bstrSrcURL));

	// Set the document as active
	CDocWindow* pDocWindow = pCtp->GetDocWindow();
	if (pDocWindow)
		pDocWindow->SetDoc(pAGDoc);

	// No more files to download, so make the final callback after we do the Free()
	FNDOWNLOADCALLBACK fnCallback = m_fnCallback;
	LPARAM lParam = m_lParam;
	Free();
	if (fnCallback)
		fnCallback(lParam, NULL);

	// If necessary, send the print command
	if (pCtp->AutoPrintEnabled())
		pCtp->m_pDocWindow->SendMessage(WM_COMMAND, IDC_DOC_PRINT);
}

/////////////////////////////////////////////////////////////////////////////
void CDownload::FileSave()
{
	if (!InProgress())
		return;

	// Is there any file data to process?
	if (!m_pData)
		return;

	// If there is a destination file defined, write the data to disk
	DOWNLOAD Download = m_List.front();
	CString& strDstPath = Download.strDstPath;
	if (!strDstPath.IsEmpty())
	{
		// Write the file to the destination path
		HANDLE hFile = ::CreateFile(strDstPath, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
		if (hFile == INVALID_HANDLE_VALUE)
		{
			CMessageBox::Message(String("Error creating file '%s' after download.", strDstPath));
			return;
		}

		DWORD dwBytesWritten = 0;
		bool bFailed = !::WriteFile(hFile, (LPCVOID)m_pData, m_dwSize, &dwBytesWritten, NULL);
		::CloseHandle(hFile);

		if (bFailed)
		{
			CMessageBox::Message(String("Error writing to '%s' after download.", strDstPath));
			return;
		}
	}

	// If there is a callback defined, make the 'success' callback
	if (m_fnCallback)
	{
		Download.pData = m_pData;
		Download.dwSize = m_dwSize;
		m_fnCallback(m_lParam, (void*)&Download);
	}
}
