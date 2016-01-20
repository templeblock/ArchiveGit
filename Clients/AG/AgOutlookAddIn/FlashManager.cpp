// CFlashManager.cpp : Implementation of COEAddin
#include "stdafx.h"
#include "FlashManager.h"
#include "FunctionObject.h"
#include "HelperFunctions.h"

static UINT IDT_FLASH_TIMER = ::RegisterWindowMessage("Creata Mail Flash Timer");
static UINT IDT_PASTE_TIMER	= ::RegisterWindowMessage("Creata Mail Paste Timer");

static LPCTSTR FLASH_PLAYER_CLASSID = "D27CDB6E-AE6D-11CF-96B8-444553540000";

//////////////////////////////////////////////////////////////////////////////
CFlashManager::CFlashManager()
{
	m_bFlashTimerInUse	= false;
	m_bPasteTimerInUse	= false;
	m_bWSShellUsed		= false;

	// Create a	hidden window (using CWindowImpl)
	RECT rect;
	rect.left =	rect.right = rect.top =	rect.bottom	= 0;
	Create(NULL, rect, _T("FlashManagerHiddenWindow"),	WS_POPUP);
}

//////////////////////////////////////////////////////////////////////////////
CFlashManager::~CFlashManager()
{
	if (m_bFlashTimerInUse)
		KillTimer(IDT_FLASH_TIMER);
	if (m_bPasteTimerInUse)
		KillTimer(IDT_PASTE_TIMER);
	
	m_FlashElementList.clear();
	DestroyWindow();
	
}

//////////////////////////////////////////////////////////////////////////////
bool CFlashManager::Activate(IHTMLDocument2Ptr spDoc, bool bOn, bool bContentEditable)
{
	CFuncLog log(g_pLog, "CFlashManager::Activate()");
	
	// spDoc could be NULL if the document is being closed
	if ((spDoc == NULL) && bOn)
	{
		CString szMsg;
		szMsg.Format("(spDoc == NULL) && bOn) FAILED, spDoc == %X, bOn == %x", spDoc, bOn); 
		log.LogString(LOG_WARNING, szMsg.GetBuffer());
		return false;
	}
				
	if ((spDoc != NULL) && !IsDocInEditMode(spDoc))
	{
		CString szMsg;
		szMsg.Format("(spDoc != NULL) && !IsDocInEditMode(spDoc) FAILED, spDoc == %X", spDoc); 
		log.LogString(LOG_WARNING, szMsg.GetBuffer());
		return false;
	}

	if (bOn)
	{
		Enable(spDoc, bContentEditable);
		SetDocMouseDownCallback(spDoc, false);
		SetDocPasteCallback(spDoc, false);
	}
	else
	{
		SetDocMouseDownCallback(spDoc, true);
		SetDocPasteCallback(spDoc, true);
		ResetAllFlashCallbacks(spDoc);
	}

	return true;
}

//////////////////////////////////////////////////////////////////////////////
void CFlashManager::FlashCallback(IDispatch * pDisp1, IDispatch * pDisp2, DISPID id, VARIANT* pVarResult)
{
	CFuncLog log(g_pLog, "CFlashManager::FlashCallback()");
	
	if (!pDisp1)
		return;

	if (id == DISPID_MOUSEOVER)
	{
		// Play flash
		//Flash_Play(pDisp2); // Interferes with interactive flash, i.e. Care Bear game

		// Set contentEditable false (true if alt key pressed)
		CComQIPtr<IHTMLElement> spElem(pDisp1);
		if (!spElem)
			return;
			
		HRESULT hr = S_OK;
		CComPtr<IDispatch> spDisp;
		hr = spElem->get_document(&spDisp);
		if (FAILED(hr) || !spDisp)
			return;

		CComQIPtr<IHTMLDocument2> spDoc(spDisp);
		if (!spDoc)
			return;

		CComPtr<IHTMLWindow2> spWnd;
		hr = spDoc->get_parentWindow(&spWnd);
		if (FAILED(hr) || !spWnd)
			return;
		
		CComPtr<IHTMLEventObj> spEventObj;
		hr = spWnd->get_event(&spEventObj);
		if (FAILED(hr) || !spEventObj)
			return;
		
		VARIANT_BOOL bVal;
		spEventObj->get_altKey(&bVal);

		if ( bVal ) 
			SetContentEditable(pDisp1, TRUE);
		else
			SetContentEditable(pDisp1, FALSE);	
	}
	else if (id == DISPID_MOUSEDOWN)
	{
		CComQIPtr<IHTMLElement> spElem(pDisp1);
		if (!spElem)
			return;
		
		CComPtr<IDispatch> spDisp;
		HRESULT hr = spElem->get_document(&spDisp);
		CComQIPtr<IHTMLDocument2> spDoc(spDisp);
		if (FAILED(hr) || !spDoc)
			return;
		
		CComPtr<IHTMLWindow2> spWnd;
		hr = spDoc->get_parentWindow(&spWnd);
		if (FAILED(hr) || !spWnd)
			return;
		
		CComPtr<IHTMLEventObj> spEventObj;
		hr = spWnd->get_event(&spEventObj);
		if (FAILED(hr) || !spEventObj)
			return;
		
		long lVal;
		spEventObj->get_button(&lVal);
		if ( lVal == 1) // 1 = left button is pressed 		
			hr = spElem->click();		
	}
	else if (id == DISPID_DRAGEND)
	{
		// New flash elements added, re-enable.
		CComQIPtr<IHTMLElement> spElem(pDisp1);
		if (!spElem)
			return;

		HRESULT hr = S_OK;
		CComPtr<IDispatch> spDisp;
		hr = spElem->get_document(&spDisp);
		if (FAILED(hr) || !spDisp)
			return;

		CComQIPtr<IHTMLDocument2> spDoc(spDisp);
		if (!spDoc)
			return;
		
		Enable(spDoc, FALSE);
	}
	else if (id == DISPID_PASTE) 
	{
		CComQIPtr<IHTMLDocument2> spDoc(pDisp1);
		if (!spDoc)
			return;

		if (m_bPasteTimerInUse)
			return;

		m_pPrePasteTimerDoc = spDoc;
		SetTimer(IDT_PASTE_TIMER, 300, NULL);
		m_bPasteTimerInUse = true;
	}
	else if (id == DISPID_DOCMOUSEDOWN) 
	{
		CComQIPtr<IHTMLDocument2> spDoc(pDisp1);
		if (!spDoc)
			return;

		CComPtr<IHTMLWindow2> spWnd;
		HRESULT hr = spDoc->get_parentWindow(&spWnd);
		if (FAILED(hr) || !spWnd)
			return;
		
		CComPtr<IHTMLEventObj> spEventObj;
		hr = spWnd->get_event(&spEventObj);
		if (FAILED(hr) || !spEventObj)
			return;
		
		VARIANT_BOOL bVal = VARIANT_FALSE;
		spEventObj->get_ctrlKey(&bVal);

		if (bVal) 
			Activate((IHTMLDocument2*)spDoc, true, true);

	}
}

//////////////////////////////////////////////////////////////////////////////
BOOL CFlashManager::SetMouseOverCallback(ElementInfo &ElemInfo, BOOL bReset)
{
	CFuncLog log(g_pLog, "CFlashManager::SetMouseOverCallback()");

	CComQIPtr<IHTMLElement> spElem(ElemInfo.pTagDisp);
	if (!spElem)
		return FALSE;
		
	if (bReset)
	{
		VARIANT varNull; 
		varNull.vt = VT_NULL;
		HRESULT hr = spElem->put_onmouseover(varNull);
		if (FAILED(hr))
			return FALSE;
	}
	else
	{
		VARIANT varFO;
		varFO.vt = VT_DISPATCH;
		varFO.pdispVal = 
		CFunctionObject<CFlashManager>::CreateEventFunctionObject
										(this, 
										&CFlashManager::FlashCallback,
										ElemInfo.pTagDisp,
										ElemInfo.pFlashDisp,
										DISPID_MOUSEOVER);
		
		HRESULT hr = spElem->put_onmouseover(varFO);
		if (FAILED(hr))
			return FALSE;
	}
	
	return TRUE;
}
//////////////////////////////////////////////////////////////////////////////
BOOL CFlashManager::SetMouseDownCallback(ElementInfo &ElemInfo, BOOL bReset)
{
	CFuncLog log(g_pLog, "CFlashManager::SetMouseDownCallback()");

	CComQIPtr<IHTMLElement> spElem(ElemInfo.pTagDisp);
	if (!spElem)
		return FALSE;
		
	if (bReset)
	{
		VARIANT varNull; 
		varNull.vt = VT_NULL;
		HRESULT hr = spElem->put_onmousedown(varNull);
		if (FAILED(hr))
			return FALSE;
	}
	else
	{
		VARIANT varFO;
		varFO.vt = VT_DISPATCH;
		varFO.pdispVal = 
		CFunctionObject<CFlashManager>::CreateEventFunctionObject
										(this, 
										&CFlashManager::FlashCallback,
										ElemInfo.pTagDisp,
										ElemInfo.pFlashDisp,
										DISPID_MOUSEDOWN);
		
		HRESULT hr = spElem->put_onmousedown(varFO);
		if (FAILED(hr))
			return FALSE;
	}
	
	return TRUE;
}

//////////////////////////////////////////////////////////////////////////////
BOOL CFlashManager::SetDragEndCallback(ElementInfo &ElemInfo,  BOOL bReset)
{
	CFuncLog log(g_pLog, "CFlashManager::SetDragEndCallback()");

	CComQIPtr<IHTMLElement2> spElem2(ElemInfo.pTagDisp);
	if (!spElem2)
		return FALSE;
	
	if (bReset)
	{
		VARIANT varNull; 
		varNull.vt = VT_NULL;
		HRESULT hr = spElem2->put_ondragend(varNull);
		if (FAILED(hr))
			return FALSE;
	}
	else
	{
		VARIANT varFO;
		varFO.vt = VT_DISPATCH;
		varFO.pdispVal = 
		CFunctionObject<CFlashManager>::CreateEventFunctionObject
										(this, 
										&CFlashManager::FlashCallback,
										ElemInfo.pTagDisp,
										NULL,
										DISPID_DRAGEND);
		
		HRESULT hr = spElem2->put_ondragend(varFO);
		if (FAILED(hr))
			return FALSE;
	}
	return TRUE;
}

//////////////////////////////////////////////////////////////////////////////
BOOL CFlashManager::SetDocPasteCallback(IHTMLDocument2 *pDoc, BOOL bReset)
{
	CFuncLog log(g_pLog, "CFlashManager::SetDocPasteCallback()");

	CComPtr<IHTMLDocument2> spDoc(pDoc);
	if (!spDoc)
		return FALSE;
		
	CComPtr<IHTMLElement> spElement;
	HRESULT hr = spDoc->get_body(&spElement);
	if (!spElement)
		return FALSE;

	CComQIPtr<IHTMLElement2> spElem2(spElement);
	if (spElem2 == NULL)
		return FALSE;

	if (bReset)
	{
		VARIANT varNull; 
		varNull.vt = VT_NULL;
		hr = spElem2->put_onpaste(varNull);
		if (FAILED(hr))
			return FALSE;
	}
	else
	{
		VARIANT varFO;
		varFO.vt = VT_DISPATCH;
		varFO.pdispVal = 
		CFunctionObject<CFlashManager>::CreateEventFunctionObject
										(this, 
										&CFlashManager::FlashCallback,
										spDoc,
										NULL,
										DISPID_PASTE);
		
		HRESULT hr = spElem2->put_onpaste(varFO);
		if (FAILED(hr))
			return FALSE;
	}
	
	return TRUE;
}

//////////////////////////////////////////////////////////////////////////////
BOOL CFlashManager::SetDocMouseDownCallback(IHTMLDocument2 *pDoc, BOOL bReset)
{
	CFuncLog log(g_pLog, "CFlashManager::SetDocMouseDownCallback()");

	CComPtr<IHTMLDocument2> spDoc(pDoc);
	if (!spDoc)
		return FALSE;
		
	if (bReset)
	{
		VARIANT varNull; 
		varNull.vt = VT_NULL;
		HRESULT hr = spDoc->put_onmousedown(varNull);
		if (FAILED(hr))
			return FALSE;
	}
	else
	{
		VARIANT varFO;
		varFO.vt = VT_DISPATCH;
		varFO.pdispVal = 
		CFunctionObject<CFlashManager>::CreateEventFunctionObject
										(this, 
										&CFlashManager::FlashCallback,
										spDoc,
										NULL,
										DISPID_DOCMOUSEDOWN);
		
		HRESULT hr = spDoc->put_onmousedown(varFO);
		if (FAILED(hr))
			return FALSE;
	}
	
	return TRUE;
}

//////////////////////////////////////////////////////////////////////////////
BOOL CFlashManager::SetContentEditable(IDispatch* pDisp, BOOL bContentEditable)
{
	CFuncLog log(g_pLog, "CFlashManager::SetContentEditable()");
	return SetAttribute(pDisp, CComBSTR(HTML_ATTRIBUTE_CONTENTEDITABLE), CComVariant(bContentEditable ? "true" : "false"));
}

//////////////////////////////////////////////////////////////////////////////
BOOL CFlashManager::ShowMenu(IHTMLDocument2* pDoc, BOOL bShow)
{
	CComQIPtr<IHTMLDocument2> spHTML(pDoc);
	if (!spHTML)
		return FALSE;

	if (GetFlashElements(spHTML))
	{	
		ELEMENTLIST::iterator itr;
		itr = m_FlashElementList.begin();
		while(itr != m_FlashElementList.end())
		{
			if (!Flash_ShowMenu((*itr).pFlashDisp, bShow))
				return FALSE;
			itr++;
		}	
	}
	
	return TRUE;
}

//////////////////////////////////////////////////////////////////////////////
BOOL CFlashManager::Flash_WMode(IDispatch* pDisp, LPCTSTR szValue)
{
	HRESULT hr = S_OK;
	CComQIPtr<ShockwaveFlashObjects::IShockwaveFlash> spSWF(pDisp);
	if (!spSWF)
		return FALSE;

	CComBSTR bstrVal(szValue);
	hr = spSWF->put_WMode(bstrVal);
	if (FAILED(hr))
		return FALSE;
	
	return TRUE;
}

//////////////////////////////////////////////////////////////////////////////
BOOL CFlashManager::Flash_ShowMenu(IDispatch* pDisp, BOOL bShow)
{
	HRESULT hr = S_OK;
	CComQIPtr<ShockwaveFlashObjects::IShockwaveFlash> spSWF(pDisp);
	if (!spSWF)
		return FALSE;

	VARIANT_BOOL bVal = bShow;
	hr = spSWF->put_Menu(bShow);
	if (FAILED(hr))
		return FALSE;
	
	return TRUE;
}
//////////////////////////////////////////////////////////////////////////////
BOOL CFlashManager::IsWsShellUsed(IDispatch* pDisp)
{
	CComQIPtr<ShockwaveFlashObjects::IShockwaveFlash> spSWF(pDisp);
	if (!spSWF)
		return FALSE;

	CComBSTR bsMovie;
	HRESULT hr = spSWF->get_Movie(&bsMovie);
	if (FAILED(hr))
		return FALSE;

	CString csMovie = bsMovie;
	if (csMovie.Find("wsShell") < 0)
		return FALSE;

	return TRUE;
}
//////////////////////////////////////////////////////////////////////////////
BOOL CFlashManager::Flash_Start(IDispatch* pDisp)
{
	CFuncLog log(g_pLog, "CFlashManager::Flash_Start()");
	CComQIPtr<ShockwaveFlashObjects::IShockwaveFlash> spSWF(pDisp);
	if (!spSWF)
	{
		log.LogString(LOG_ERROR, " FAILURE: CComQIPtr<ShockwaveFlashObjects::IShockwaveFlash> spSWF == NULL)");	
		return FALSE;
	}

	// If using the wsShell.swf loader then to play the 
	// flash we must actually call Rewind(). This fixes the 
	// problem where flash types with the wsShell loader were 
	// failing to play. For all other flash types we call Play().
	HRESULT hr = S_OK;
	if (IsWsShellUsed(pDisp))
		hr = spSWF->Rewind();
	else
		hr = spSWF->Play();

	if (FAILED(hr))
	{
		CString szMsg;
		szMsg.Format("Play/Rewind FAILED(%X)", hr); 
		GetError(szMsg);
		log.LogString(LOG_ERROR, szMsg.GetBuffer());
		return FALSE;
	}

	if (g_pLog)
	{
		CString szMsg;
		szMsg.Format("SUCCESS"); 
		log.LogString(LOG_INFO, szMsg.GetBuffer());
	}

	return TRUE;
}
//////////////////////////////////////////////////////////////////////////////
BOOL CFlashManager::Flash_Play(IDispatch* pDisp)
{
	CFuncLog log(g_pLog, "CFlashManager::Flash_Play()");

	CComQIPtr<ShockwaveFlashObjects::IShockwaveFlash> spSWF(pDisp);
	if (!spSWF)
		return FALSE;
					

	VARIANT_BOOL bVal = VARIANT_FALSE;
	bVal = spSWF->IsPlaying();
    if (bVal)
	{
		//spSWF->StopPlay();
		return FALSE;
	}

	HRESULT hr = spSWF->Play();
	if (FAILED(hr))
		return FALSE;
	
	return TRUE;
}
//////////////////////////////////////////////////////////////////////////////
BOOL CFlashManager::Flash_Rewind(IDispatch* pDisp)
{
	CFuncLog log(g_pLog, "CFlashManager::Flash_Rewind()");

	CComQIPtr<ShockwaveFlashObjects::IShockwaveFlash> spSWF(pDisp);
	if (!spSWF)
		return FALSE;
	
	HRESULT hr;
	VARIANT_BOOL bVal = VARIANT_FALSE;
	bVal = spSWF->IsPlaying();
    if (bVal)
		spSWF->Stop();

	hr = spSWF->Rewind();

	if (FAILED(hr))
		return FALSE;
	
	return TRUE;
}
//////////////////////////////////////////////////////////////////////////////
BOOL CFlashManager::SetFlashAttributes(ElementInfo &ElemInfo, BOOL bContentEditable)
{
	CFuncLog log(g_pLog, "CFlashManager::SetFlashAttributes()");

	if (!ElemInfo.pTagDisp)
		return FALSE;
	
	CComQIPtr<IHTMLElement> spElem(ElemInfo.pTagDisp);
	if (!spElem)
		return FALSE;

	SetContentEditable(ElemInfo.pTagDisp, bContentEditable);
	//Flash_WMode(ElemInfo.pFlashDisp, _T("transparent")); // Content will handle Transparency?
	Flash_Start(ElemInfo.pFlashDisp);
	//SetMouseOverCallback(ElemInfo, false);
	SetDragEndCallback(ElemInfo, false);
	
	return TRUE;
}

//////////////////////////////////////////////////////////////////////////////
BOOL CFlashManager::SetAllFlashAttributes(IHTMLDocument2* pDoc, BOOL bContentEditable)
{
	CFuncLog log(g_pLog, "CFlashManager::SetAllFlashAttributes()");
	
	// This function sets the contentEditable attribute for Flash elements.
	// By default the current flash elements are retrieved from pDoc and 
	// the contentEditable value is set to "true". This behavior is overriden
	// by the bEdit and pElist parameters.
	CComQIPtr<IHTMLDocument2> spHTML(pDoc);
	if (!spHTML)
		return false;

	ELEMENTLIST::iterator itr;
	itr = m_FlashElementList.begin();
	while(itr != m_FlashElementList.end())
	{
		SetFlashAttributes(*itr, bContentEditable);	
		itr++;
	}

	return true;
}

//////////////////////////////////////////////////////////////////////////////
BOOL CFlashManager::Enable(IHTMLDocument2* pDoc, BOOL bContentEditable)
{
	CFuncLog log(g_pLog, "CFlashManager::Enable()");

	CComQIPtr<IHTMLDocument2> spHTML(pDoc);
	if (spHTML && m_hWnd)
	{
		KillTimer(IDT_FLASH_TIMER);
		m_bWSShellUsed = true; // assume true to use as default.
		m_bFlashTimerInUse = false;
		if (GetFlashElements(spHTML))
		{		
			SetAllFlashAttributes(spHTML, bContentEditable);
			SetTimer(IDT_FLASH_TIMER, 200, NULL);
			m_bFlashTimerInUse = true;
			log.LogString(LOG_INFO, "SUCCESS");
			return TRUE;
		}
	}
	
	log.LogString(LOG_WARNING, "No Flash Elements?");
	return FALSE;
}

//////////////////////////////////////////////////////////////////////////////
BOOL CFlashManager::ResetAllFlashCallbacks(IHTMLDocument2* pDoc)
{
	CFuncLog log(g_pLog, "CFlashManager::ResetAllFlashCallbacks()");

	KillTimer(IDT_FLASH_TIMER);
	m_bFlashTimerInUse = false;
	KillTimer(IDT_PASTE_TIMER);
	m_bPasteTimerInUse = false;

	CComQIPtr<IHTMLDocument2> spHTML(pDoc);
	if (!spHTML)
		return FALSE;
	
	if (GetFlashElements(spHTML))
	{		
		ELEMENTLIST::iterator itr;
		itr = m_FlashElementList.begin();
		while(itr != m_FlashElementList.end())
		{
			//SetMouseOverCallback(*itr, TRUE);
			SetDragEndCallback(*itr, TRUE);
			itr++;
		}
	}
	m_FlashElementList.clear();

	return TRUE;
}

//////////////////////////////////////////////////////////////////////////////
BOOL CFlashManager::GetFlashElements(IHTMLDocument2* pDoc)
{
	CFuncLog log(g_pLog, "CFlashManager::GetFlashElements()");

	m_FlashElementList.clear();

	CComQIPtr<IHTMLDocument2> spHTML(pDoc);
	if (!spHTML)
	{
		CString szMsg;
		szMsg.Format("CComQIPtr<IHTMLDocument2> spHTML(pDoc) FAILED: pDoc==%X", pDoc); 
		GetError(szMsg);
		log.LogString(LOG_ERROR, szMsg.GetBuffer());
		return FALSE;
	}

	CComPtr<IHTMLElementCollection> spAll;
	HRESULT hr = spHTML->get_all(&spAll);
	if (FAILED(hr) || !spAll)
	{
		CString szMsg;
		szMsg.Format("spHTML->get_all(&spAll) FAILED: HR == [%X], spAll == %X", hr, spAll); 
		GetError(szMsg);
		log.LogString(LOG_ERROR, szMsg.GetBuffer());
		return FALSE;
	}

	// Find all the OBJECT tags in the document
    CComPtr<IDispatch> spTagsDisp;
	hr = spAll->tags(CComVariant(CComBSTR("object")), &spTagsDisp);
	if (FAILED(hr) || !spTagsDisp)
	{
		CString szMsg;
		szMsg.Format("if (FAILED(hr) || !spTagsDisp) FAILED: hr==%X", hr, spTagsDisp); 
		GetError(szMsg);
		log.LogString(LOG_ERROR, szMsg.GetBuffer());
		return FALSE;
	}

	CComQIPtr<IHTMLElementCollection> spTags(spTagsDisp);
	if (!spTags)
		return FALSE;

	long nCnt = 0;
	hr = spTags->get_length(&nCnt);
	if (FAILED(hr))
	{
		CString szMsg;
		szMsg.Format("spTags->get_length(&nCnt) FAILED: nCnt==%d", nCnt); 
		log.LogString(LOG_WARNING, szMsg.GetBuffer());
		return FALSE;
	}

	bool bSuccess = false;
	for (long i=0; i < nCnt; i++)
	{
		CComPtr<IDispatch> spTagDisp;
		hr = spTags->item(CComVariant(i), CComVariant(i), &spTagDisp);
		if (FAILED(hr) || !spTagDisp)
			continue;
		
		CComQIPtr<IHTMLObjectElement> spObject(spTagDisp);
		if (!spObject)
			continue;

		CComBSTR bsClassID;
		hr = spObject->get_classid(&bsClassID);
		CString szClassID(bsClassID);
		if (FAILED(hr) || !bsClassID)
		{
			CString szMsg;
			szMsg.Format("if (FAILED(hr) || !bsClassID) FAILED: hr==%X, bsClassID == %s", hr, bsClassID.m_str); 
			log.LogString(LOG_ERROR, szMsg.GetBuffer());
			continue;
		}

		szClassID.MakeUpper();
		if (szClassID.Find(FLASH_PLAYER_CLASSID) >= 0)
		{
			log.LogString(LOG_INFO, "if (szClassID.Find(FLASH_PLAYER_CLASSID) >= 0) is TRUE");
		
			ElementInfo ElemInfo;
			ElemInfo.Updated = FALSE;
			CComPtr<IDispatch> spFlashDisp;
			hr = spObject->get_object(&spFlashDisp);
			if (SUCCEEDED(hr) && spFlashDisp)
			{
				ElemInfo.pFlashDisp = spFlashDisp;
				log.LogString(LOG_INFO, "if (SUCCEEDED(hr) && spFlashDisp) Succeeded");
			}
			
			ElemInfo.pTagDisp = spTagDisp;
			m_FlashElementList.push_back(ElemInfo);
			bSuccess = true;
		}
	}
	
	if (g_pLog)
	{
		CString szMsg;
		szMsg.Format("RESULTS: bSuccess == %X, nCnt==%d", bSuccess, nCnt); 
		log.LogString(LOG_INFO, szMsg.GetBuffer());
	}

	return bSuccess;
}

//////////////////////////////////////////////////////////////////////////////
CFlashManager::FlashState CFlashManager::FlashFocus(ElementInfo& eInfo)
{
	CFuncLog log(g_pLog, "CFlashManager::FlashFocus()");
	
	HRESULT hr;
	CComPtr<IDispatch> spDisp = (IDispatch*)eInfo.pTagDisp;
	if (!spDisp)
		return FLASH_ERROR;
	
	// Activet flash element
	CComQIPtr<IHTMLElement2> spElem2(spDisp);
	
	if (!spElem2)
		return FLASH_ERROR;
    

	// Check the ready state for the flash element.
	// If not Interactive (3) or Complete (4) then
	// return and try again next time.
	enum  { ST_Uninitialized = 0, ST_Loading = 1, ST_Loaded = 2, ST_Interactive = 3, ST_Complete = 4 };
	long lState = 0;
	hr = spElem2->get_readyStateValue(&lState);
	if (lState < ST_Interactive) 
		return FLASH_ELEM_NOTREADY;

	// Make sure flash movie has completed loading.
	CComPtr<IDispatch> spFlashDisp = (IDispatch*)eInfo.pFlashDisp;
	CComQIPtr<ShockwaveFlashObjects::IShockwaveFlash> spSWF(spFlashDisp);
	if (!spSWF)
		return FLASH_ERROR;

	long lPctLoaded = spSWF->PercentLoaded();
	if (lPctLoaded < 100)
		return FLASH_LOADING;
	
		
	// Set flash element to active to ensure that it's
	// viewable (i.e. black box problem)
	CComQIPtr<IHTMLElement3> spElem3(spDisp);
	if (spElem3) // Will fail for verions of IE less than IE5.5
		hr = spElem3->setActive();
	else 
		hr = spElem2->focus();

	ATLASSERT(SUCCEEDED(hr));

	return FLASH_ACTIVATED;
}
//////////////////////////////////////////////////////////////////////////////
LRESULT CFlashManager::OnTimer(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	CFuncLog log(g_pLog, "CFlashManager::OnTimer()");

	// This logic will activate one flash element at a time -
	// each time the timer expires until all elements are activated.
	// The reason the timer is needed is because time needs to elapse 
	// to process the setActive command to get the desired effect.
	if (IDT_FLASH_TIMER == wParam) 
	{		
		if (!m_FlashElementList.empty())
		{
			// Get element starting with back of list.
			FlashState eState = FLASH_ERROR;
			ElementInfo& eInfo = m_FlashElementList.back();
			CComPtr<IDispatch> spDisp = (IDispatch*)eInfo.pTagDisp;
		
			// Activate flash element
			eState = FlashFocus(eInfo);
	
			// Flag for WS shell use.
			CComPtr<IDispatch> spFlashDisp = (IDispatch*)eInfo.pFlashDisp;
			m_bWSShellUsed = IsWsShellUsed(spFlashDisp);

			// Remove the activated flash element from the list.
			if (eState == FLASH_ACTIVATED || eState == FLASH_ERROR)
				m_FlashElementList.pop_back();

			log.LogString(LOG_INFO, log.FormatString("Flash Element State: State == %d, m_bWSShellUsed == %d", eState, m_bWSShellUsed));
		}
		else
		{
			// Kill timer when we have updated the last flash element in the list.
			KillTimer(IDT_FLASH_TIMER);
			m_bFlashTimerInUse = false;
		}
	}
	else
	if (IDT_PASTE_TIMER == wParam) 
	{
		KillTimer(IDT_PASTE_TIMER);
		m_bPasteTimerInUse = false;
		Enable(m_pPrePasteTimerDoc, false/*bContentEditable*/);
		m_pPrePasteTimerDoc = NULL;
	}

	return 0;
}
