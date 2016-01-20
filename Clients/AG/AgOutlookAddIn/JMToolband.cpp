// JMToolband.cpp : Implementation of CJMToolband

#include "stdafx.h"
#include "JMToolband.h"


const WCHAR TITLE_CJMToolband[] = L"";

// CJMToolband
CJMToolband::CJMToolband()
{
}
CJMToolband::~CJMToolband()
{
}
/**************************************************************************

   CJMToolband::RegisterAndCreateWindow()
   
**************************************************************************/

BOOL CJMToolband::RegisterAndCreateWindow()
{  
	if (m_hWndParentRebar == NULL)
		return false;

	RECT rectClientParent;
	::GetClientRect(m_hWndParentRebar, &rectClientParent);

    // We need to create an Invisible Child Window using the Parent Window, this will also be used to reflect Command
    // messages from the rebar
    HWND hWndChild = m_ReflectWnd.Create(m_hWndParentRebar, rectClientParent, NULL, WS_CHILD);
    CBandToolBarCtrl* pToolBar = m_ReflectWnd.GetToolBar();
   
    
    // Set the web Broser
    if (m_spWebBrowser)
    {
		pToolBar->SetWebBrowser(m_spWebBrowser);
		pToolBar->m_ctlBandComboBox.m_pBand = this;

        // Call this after you get a valid WebBrowser
        pToolBar->SetBandRebar();
    }


	return pToolBar->IsWindow();
    
}
/**************************************************************************

   CJMToolband::GetClassID()
   
**************************************************************************/
void CJMToolband::FocusChange(bool bFocus)
{
	CComPtr<IUnknown> spBand;
	try
	{
		QueryInterface(IID_IUnknown, (void**)&spBand);
		m_spSite->OnFocusChangeIS(spBand, (BOOL)bFocus);
	}
	catch(...)
	{

	}
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// IPersistStream implementations
// 
// This is only supported to allow the desk band to be dropped on the 
// desktop and to prevent multiple instances of the desk band from showing 
// up in the shortcut menu. This desk band doesn't actually persist any data.
//

/**************************************************************************

   CJMToolband::GetClassID()
   
**************************************************************************/

STDMETHODIMP CJMToolband::GetClassID(LPCLSID pClassID)
{
    *pClassID = CLSID_JMToolband;
    
    return S_OK;
}

/**************************************************************************

   CJMToolband::IsDirty()
   
**************************************************************************/

STDMETHODIMP CJMToolband::IsDirty(void)
{
    return S_FALSE;
}

/**************************************************************************

   CJMToolband::Load()
   
**************************************************************************/

STDMETHODIMP CJMToolband::Load(LPSTREAM pStream)
{
    return S_OK;
}

/**************************************************************************

   CJMToolband::Save()
   
**************************************************************************/

STDMETHODIMP CJMToolband::Save(LPSTREAM pStream, BOOL fClearDirty)
{
    return S_OK;
}

/**************************************************************************

   CJMToolband::GetSizeMax()
   
**************************************************************************/

STDMETHODIMP CJMToolband::GetSizeMax(ULARGE_INTEGER *pul)
{
    return E_NOTIMPL;
}
///////////////////////////////////////////////////////////////////////////
//
// IOleWindow Implementation
//

/**************************************************************************

   CJMToolband::GetWindow()
   
**************************************************************************/

STDMETHODIMP CJMToolband::GetWindow(HWND *phWnd)
{
    HRESULT hr = S_OK;
	if (NULL == phWnd)
	{
		hr = E_INVALIDARG;
	}
	else
	{
		*phWnd = m_ReflectWnd.GetToolBar()->m_hWnd;
	}
	return hr;
}

/**************************************************************************

   CJMToolband::ContextSensitiveHelp()
   
**************************************************************************/

STDMETHODIMP CJMToolband::ContextSensitiveHelp(BOOL fEnterMode)
{
    m_bEnterHelpMode = fEnterMode;
	return S_OK;;
}

///////////////////////////////////////////////////////////////////////////
//
// IDockingWindow Implementation
//

/**************************************************************************

   CJMToolband::ShowDW()
   
**************************************************************************/

STDMETHODIMP CJMToolband::ShowDW(BOOL fShow)
{
    HRESULT hr = S_OK;
	m_bShow = fShow;
	CBandToolBarCtrl* pToolBar = m_ReflectWnd.GetToolBar();
	if (pToolBar)
		::ShowWindow(pToolBar->m_hWnd, m_bShow ? SW_SHOW : SW_HIDE);
	return hr;
}

/**************************************************************************

   CJMToolband::CloseDW()
   
**************************************************************************/

STDMETHODIMP CJMToolband::CloseDW(DWORD dwReserved)
{
    return S_OK;
}

/**************************************************************************

   CJMToolband::ResizeBorderDW()
   
**************************************************************************/

STDMETHODIMP CJMToolband::ResizeBorderDW(LPCRECT prcBorder, 
                                          IUnknown* punkSite, 
                                          BOOL fReserved)
{
    // This method is never called for Band Objects.
    return E_NOTIMPL;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// IInputObject Implementation
//

/**************************************************************************

   CJMToolband::UIActivateIO()
   
**************************************************************************/

STDMETHODIMP CJMToolband::UIActivateIO(BOOL fActivate, LPMSG pMsg)
{
    if(fActivate)
	{
		CBandToolBarCtrl* pToolBar = m_ReflectWnd.GetToolBar();
		if (pToolBar)
			pToolBar->m_ctlBandComboBox.SetFocus();
	}
    
    return S_OK;
}

/**************************************************************************

   CJMToolband::HasFocusIO()
   
   If this window or one of its decendants has the focus, return S_OK. Return 
   S_FALSE if neither has the focus.

**************************************************************************/

STDMETHODIMP CJMToolband::HasFocusIO(void)
{
    HWND hwnd = GetFocus();

    // See if the focus has been set to any of the children
    //
    HWND hChild = ::GetWindow(m_ReflectWnd.GetToolBar()->m_hWnd, GW_CHILD);
    while (hChild) 
    {
        // check its child
        HWND hGrandChild = ::GetWindow(hChild, GW_CHILD);

        if (hwnd == hChild || hwnd == hGrandChild)
            return S_OK;

        hChild = ::GetWindow(hChild, GW_HWNDNEXT);
    }

    return S_FALSE;
    
}

/**************************************************************************

   CJMToolband::TranslateAcceleratorIO()
   
   If the accelerator is translated, return S_OK or S_FALSE otherwise.

**************************************************************************/
STDMETHODIMP CJMToolband::TranslateAcceleratorIO(LPMSG pMsg)
{
	CBandToolBarCtrl* pToolBar = m_ReflectWnd.GetToolBar();
	if (pToolBar)
		return pToolBar->TranslateAcceleratorIO(pMsg);   
	
	return S_FALSE;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// IObjectWithSite implementations
//
/**************************************************************************

   CJMToolband::SetSite()
   
**************************************************************************/
STDMETHODIMP CJMToolband::SetSite(IUnknown *punkSite)
{
    HRESULT hr;

	//If a site is being held, release it.
    if(m_spSite)
		m_spSite = NULL;

   

	//If punkSite is not NULL, a new site is being set.
	if(punkSite)
	{
		//Get the parent window.
		CComPtr<IOleWindow>  spOleWindow;

        m_hWndParentRebar = NULL;

        hr = punkSite->QueryInterface(IID_IOleWindow, (LPVOID*)&spOleWindow);
        if (SUCCEEDED(hr))
		{
            spOleWindow->GetWindow(&m_hWndParentRebar);
            spOleWindow = NULL;
        }


		if(!::IsWindow(m_hWndParentRebar))
			return E_FAIL;

		// Minimum of Explorer 4 required
        CComQIPtr<IServiceProvider> spServiceProvider = punkSite;
		if (m_spWebBrowser)
			m_spWebBrowser = NULL;
		
        if (FAILED(spServiceProvider->QueryService(SID_SWebBrowserApp, IID_IWebBrowser2, (void**)&m_spWebBrowser)))
            return E_FAIL;


         if(!RegisterAndCreateWindow())
            return E_FAIL;

		//Get and keep the IInputObjectSite pointer.
        hr = punkSite->QueryInterface(IID_IInputObjectSite, (LPVOID*)&m_spSite);
		if (FAILED(hr))
            return E_FAIL;
		
	}
	return S_OK;
}

/**************************************************************************

   CJMToolband::GetSite()
   
**************************************************************************/
STDMETHODIMP CJMToolband::GetSite(REFIID riid, LPVOID *ppvReturn)
{
    *ppvReturn = NULL;
    
    if(m_spSite)
        return m_spSite->QueryInterface(riid, ppvReturn);
    
    return E_FAIL;
}
///////////////////////////////////////////////////////////////////////////
//
// IDeskBand implementation
//

/**************************************************************************

   CExplorerBar::GetBandInfo()
   
**************************************************************************/

STDMETHODIMP CJMToolband::GetBandInfo(DWORD dwBandID, 
                                       DWORD dwViewMode, 
                                       DESKBANDINFO* pdbi)
{
    m_dwBandID = dwBandID;
	m_dwViewMode = dwViewMode;

	CBandToolBarCtrl* pToolBar = m_ReflectWnd.GetToolBar();
	if (!pToolBar)
		return S_OK;

	if (pdbi)
	{
		if (pdbi->dwMask & DBIM_MINSIZE)
		{
            pdbi->ptMinSize.x = 20; //sizeBar.cx; //(250)
			pdbi->ptMinSize.y = 22; // 20
		}
		if (pdbi->dwMask & DBIM_MAXSIZE)
		{
			pdbi->ptMaxSize.x = 0; // ignored (0)
			pdbi->ptMaxSize.y = -1;	// width
		}
		if (pdbi->dwMask & DBIM_INTEGRAL)
		{
			pdbi->ptIntegral.x = 1; // ignored (1)
			pdbi->ptIntegral.y = 1; // not sizeable 1
		}
		if (pdbi->dwMask & DBIM_ACTUAL)
		{
            CRect rc;
            int nWidth(0);
            int nCount = pToolBar->GetButtonCount();
            for(int i = 0; i < nCount; i++)
            {
                pToolBar->GetItemRect(i, &rc);
                nWidth += rc.right - rc.left;
            }

            pdbi->ptActual.x = nWidth; 
			pdbi->ptActual.y = 22; // 22
		}
		if (pdbi->dwMask & DBIM_TITLE)
		{
			wcscpy_s(pdbi->wszTitle,  _countof(pdbi->wszTitle), TITLE_CJMToolband);
		}
		if (pdbi->dwMask & DBIM_BKCOLOR)
		{
			//Use the default background color by removing this flag.
			pdbi->dwMask &= ~DBIM_BKCOLOR;
		}
		if (pdbi->dwMask & DBIM_MODEFLAGS)
		{
            // Add Chevron
            pdbi->dwModeFlags = DBIMF_NORMAL | DBIMF_VARIABLEHEIGHT | DBIMF_USECHEVRON | DBIMF_BREAK;
		}
	}
	return S_OK;
}
