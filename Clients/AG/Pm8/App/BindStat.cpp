#include "stdafx.h"
#include "wininet.h"
#include "bindstat.h"
#include "resource.h"
#include "ProgBar.h"
#include "ProgStat.h"
#include "file.h"
#include <mmsystem.h>  //for timeGetTime

#define EDIT_BOX_LIMIT 0x7FFF    //  The Edit box limit
#define SAFECAST(_obj, _type) (((_type)(_obj)==(_obj)?0:0), (_type)(_obj))
// ===========================================================================
//                     CBindStatusCallback Implementation
// ===========================================================================

// ---------------------------------------------------------------------------
// %%Function: CBindStatusCallback::CBindStatusCallback
// ---------------------------------------------------------------------------
CBindStatusCallback::CBindStatusCallback()
{
    m_pbinding = NULL;
    m_pstm = NULL;
    m_cRef = 1;
    m_cbOld = 0;

    m_fProgressShown = FALSE;
	m_fShowProgress = FALSE;
	m_pProgressDialog = new CProgressStatusDlg;
	//CreateProgressControl(m_pProgressDialog);

	m_dwStartTime = timeGetTime();
	m_ulStartTime = 0;
	m_ulProgressLast = 0;
	m_ulTimeLast = 0;
	m_ulTimeLeftLast = 0; //xffffffff;

}  // CBindStatusCallback

// ---------------------------------------------------------------------------
// %%Function: CBindStatusCallback::~CBindStatusCallback
// ---------------------------------------------------------------------------
CBindStatusCallback::~CBindStatusCallback()
{
	//if(m_fProgressShown)
	  m_pProgressDialog->DestroyWindow();

	delete m_pProgressDialog;
}  // ~CBindStatusCallback

// ---------------------------------------------------------------------------
// %%Function: CBindStatusCallback::SetWndText
// ---------------------------------------------------------------------------
 inline void
CBindStatusCallback::SetWndText(CWnd* pWnd,LPCWSTR szText)
{
    if (pWnd)
        {
        char    rgchBuf[INTERNET_MAX_PATH_LENGTH];
        WideCharToMultiByte(CP_ACP, 0, szText, -1, rgchBuf, INTERNET_MAX_PATH_LENGTH, 0, 0);
		pWnd->SetWindowText(rgchBuf);
	}
}  // CBindStatusCallback::SetWndText

// ---------------------------------------------------------------------------
// %%Function: CBindStatusCallback::QueryInterface
// ---------------------------------------------------------------------------
 STDMETHODIMP
CBindStatusCallback::QueryInterface(REFIID riid, void** ppv)
{
    *ppv = NULL;

    REFIID temp = IID_IBindStatusCallback;

	if (riid==IID_IUnknown || riid==IID_IBindStatusCallback)
        {
			*ppv = SAFECAST(this, IBindStatusCallback*);
			AddRef();
			return S_OK;
        }
	*ppv = NULL;
    return E_NOINTERFACE;
}  // CBindStatusCallback::QueryInterface

// ---------------------------------------------------------------------------
// %%Function: CBindStatusCallback::OnStartBinding
// ---------------------------------------------------------------------------
 STDMETHODIMP
CBindStatusCallback::OnStartBinding(DWORD dwReserved, IBinding* pbinding)
{

	if (m_pbinding != NULL)
        m_pbinding->Release();
    m_pbinding = pbinding;
    if (m_pbinding != NULL)
        {
        m_pbinding->AddRef();
	    if(m_fProgressShown) 
          SetStatus(L"Status: Starting to bind...");
        }

	m_ulStartTime = timeGetTime();

    return S_OK;
}  // CBindStatusCallback::OnStartBinding

// ---------------------------------------------------------------------------
// %%Function: CBindStatusCallback::GetPriority
// ---------------------------------------------------------------------------
 STDMETHODIMP
CBindStatusCallback::GetPriority(LONG* pnPriority)
{
	return E_NOTIMPL;
}  // CBindStatusCallback::GetPriority

// ---------------------------------------------------------------------------
// %%Function: CBindStatusCallback::OnLowResource
// ---------------------------------------------------------------------------
 STDMETHODIMP
CBindStatusCallback::OnLowResource(DWORD dwReserved)
{
    return E_NOTIMPL;
}  // CBindStatusCallback::OnLowResource

// ---------------------------------------------------------------------------
// %%Function: CBindStatusCallback::OnProgress
// ---------------------------------------------------------------------------
 STDMETHODIMP
CBindStatusCallback::OnProgress(ULONG ulProgress, ULONG ulProgressMax, ULONG ulStatusCode, LPCWSTR szStatusText)
{
    //char    sz[255];

	if(m_pProgressDialog->GetAbortStatus())
		return E_NOTIMPL;

	
	//if(szStatusText!=NULL)
	  //  WideCharToMultiByte(CP_ACP, 0, szStatusText,-1, sz, 255, 0,0);

    char    msg[100];
    WCHAR   out[200];

    //wsprintf(msg,"Progress: %s %d of %d bytes", sz, ulProgress, (ulProgress>ulProgressMax)?ulProgress:ulProgressMax);
    ULONG ulTimeNow = timeGetTime(); //in milliseconds
	ULONG ulRate = 0; // in Bytes/millisecond
	ULONG ulTimeLeft = 0; // in seconds
	ULONG ulCopied = 0; // in KiloBytes

	if((m_ulStartTime > 0) && ((ulTimeNow - m_ulStartTime) > 0)){
	  //note: do NOT use instantaneous rate - stick with avg. rate
		ulRate = (ulProgress) / (ulTimeNow - m_ulStartTime);
	}else 
	  ulRate = 0;

	if(ulRate > 0){
	  if(ulTimeLeft <= m_ulTimeLeftLast)
	  	 ulTimeLeft = ((ulProgressMax - ulProgress) / ulRate) / 1000;
	  else
		 ulTimeLeft = m_ulTimeLeftLast;
	}else 
	  ulTimeLeft = 0;

	
	if(ulProgress > 0)
		ulCopied = ulProgress / 1000;
	else
		ulCopied = 0;

	if(ulTimeLeft <= 59) // less than a minute
	  wsprintf(msg,"Estimated time left: %d sec ( %d KB copied )", ulTimeLeft, ulCopied);
	else if(ulTimeLeft <= 3559) // less than an hour
	  wsprintf(msg,"Estimated time left: %d min %d sec ( %d KB copied )", ((ulTimeLeft % 3600) / 60), (ulTimeLeft % 60), ulCopied);
	else // more than an hour
	  wsprintf(msg,"Estimated time left: %d hr %d min %d sec ( %d KB copied )", (ulTimeLeft / 3600), ((ulTimeLeft % 3600) / 60), ((ulTimeLeft % 3600) % 60), ulCopied);


    MultiByteToWideChar(CP_ACP, 0, msg, -1, out, sizeof(out));

//	  if(timeGetTime() >= m_dwStartTime + 2000) 
	  {
	      if(!m_fProgressShown){
			CreateProgressControl(m_pProgressDialog);
		  }
		  m_pProgressDialog->ShowWindow(SW_SHOW);
		  m_fShowProgress = TRUE;
	  }

	  if(m_fProgressShown && m_fShowProgress){
        SetStatus(L"Status: Downloading File.");
		SetProgress(out);
		SetProgressBar(ulProgress, ulProgressMax);
	  }

	  m_ulProgressLast = ulProgress;
	  m_ulTimeLast = ulTimeNow;
	  m_ulTimeLeftLast = ulTimeLeft;
    return(NOERROR);
}  // CBindStatusCallback::OnProgress

// ---------------------------------------------------------------------------
// %%Function: CBindStatusCallback::OnStopBinding
// ---------------------------------------------------------------------------
 STDMETHODIMP
CBindStatusCallback::OnStopBinding(HRESULT hrStatus, LPCWSTR pszError)
{
	 if (hrStatus){
	   if(m_fProgressShown) 
         SetStatus(L"Status: File downloaded.");
		 m_fShowProgress = FALSE;
	 }

	if (m_pbinding)
	{
		m_pbinding->Release();
		m_pbinding = NULL;
	}

    return S_OK;
}  // CBindStatusCallback::OnStopBinding

// ---------------------------------------------------------------------------
// %%Function: CBindStatusCallback::GetBindInfo
// ---------------------------------------------------------------------------
 STDMETHODIMP
CBindStatusCallback::GetBindInfo(DWORD* pgrfBINDF, BINDINFO* pbindInfo)
{
    *pgrfBINDF = BINDF_ASYNCHRONOUS | BINDF_ASYNCSTORAGE | BINDF_PULLDATA;
    *pgrfBINDF |= BINDF_GETNEWESTVERSION | BINDF_NOWRITECACHE;
    pbindInfo->cbSize = sizeof(BINDINFO);
    pbindInfo->szExtraInfo = NULL;
    memset(&pbindInfo->stgmedData, 0, sizeof(STGMEDIUM));
    pbindInfo->grfBindInfoF = 0;
    pbindInfo->dwBindVerb = BINDVERB_GET;
    pbindInfo->szCustomVerb = NULL;
    return S_OK;
}  // CBindStatusCallback::GetBindInfo

// ---------------------------------------------------------------------------
// %%Function: CBindStatusCallback::OnDataAvailable
// ---------------------------------------------------------------------------
 STDMETHODIMP
CBindStatusCallback::OnDataAvailable(DWORD grfBSCF, DWORD dwSize, FORMATETC* pfmtetc, STGMEDIUM* pstgmed)
{
	 HRESULT hr=S_OK;
	 DWORD dStrlength=0;

	// Get the Stream passed
    if (BSCF_FIRSTDATANOTIFICATION & grfBSCF)
    {
        if (!m_pstm && pstgmed->tymed == TYMED_ISTREAM)
	    {
		    m_pstm = pstgmed->pstm;
            if (m_pstm)
                m_pstm->AddRef();
    	}
    }

    // If there is some data to be read then go ahead and read them
    if (m_pstm && dwSize > m_cbOld)
	{
        DWORD dwRead = dwSize - m_cbOld; // Minimum amount available that hasn't been read
        DWORD dwActuallyRead = 0;            // Placeholder for amount read during this pull

		if (dwRead > 0)
		do
		{
	        char * pNewstr = new char[dwRead + 1];
			if (pNewstr==NULL)
				return S_FALSE;
			hr=m_pstm->Read(pNewstr,dwRead,&dwActuallyRead);
		    pNewstr[dwActuallyRead] = 0;

			// If we really read something then lets add it to the Edit box
			if (dwActuallyRead>0)
			{
				m_cbOld += dwActuallyRead;
			}
			delete[] pNewstr;

                } while (!(hr == E_PENDING || hr == S_FALSE) && SUCCEEDED(hr));
	}//     if (m_pstm && dwSize > m_cbOld)

	if (BSCF_LASTDATANOTIFICATION & grfBSCF)
	{
        if (m_pstm)
            m_pstm->Release();
		hr=S_OK;  // If it was the last data then we should return S_OK as we just finished reading everything

		if(m_fProgressShown) // && m_fShowProgress)
          SetStatus(L"Status: File downloaded.");
	}

    return hr;
}  // CBindStatusCallback::OnDataAvailable

// ---------------------------------------------------------------------------
// %%Function: CBindStatusCallback::OnObjectAvailable
// ---------------------------------------------------------------------------
 STDMETHODIMP
CBindStatusCallback::OnObjectAvailable(REFIID riid, IUnknown* punk)
{
    return E_NOTIMPL;
}  // CBindStatusCallback::OnObjectAvailable


void CBindStatusCallback::SetProgress(LPCWSTR szProgress)
{ 
	SetWndText((CWnd*)&m_pProgressDialog->m_staProgress, szProgress); 
}

void CBindStatusCallback::SetStatus(LPCWSTR szStatus)
{ 
	//SetWndText((CWnd*)&m_pProgressDialog->m_staDisplay, szStatus); 
}

void CBindStatusCallback::SetProgressBar(ULONG cProgress, ULONG maxProgress) 
{
	// Set the range and increment of the progress bar. 
	m_pProgressDialog->m_ctrlProgressBar.SetRange(0, 100); 
	m_pProgressDialog->m_ctrlProgressBar.SetPos(maxProgress ? cProgress * 100 / maxProgress : 0);

}

void CBindStatusCallback::CreateProgressControl(CProgressStatusDlg *pProgressDialog)
{
	// Create Progress Bar control
	CString csResource;
	CString csMessage;
	BOOL bCreated = FALSE;

	bCreated = pProgressDialog->Create(IDD_PROGRESS_STATUS, NULL);
	if(!bCreated)
	{
		csResource.LoadString(IDS_PROGRESS_CREATE_ERROR);
		ASSERT(!csResource.IsEmpty());
		AfxMessageBox(csResource);
		return; // ERRORCODE_Fail;
	}

	csResource.LoadString(IDS_PUBLISHING_MESSAGE);
	ASSERT(!csResource.IsEmpty());
	csMessage.Format("Loading %s", "File"); 
	pProgressDialog->SetWindowText(csMessage);
	pProgressDialog->m_ctrlProgressBar.SetRange(0, 100);
	// End Progress Bar control

	m_fProgressShown = TRUE;

}

