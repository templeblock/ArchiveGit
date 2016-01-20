// FlashManager.h : Declaration of the CFlashManager

#pragma once

// ShockwaveFlashObjects
#import "libid:D27CDB6B-AE6D-11CF-96B8-444553540000" named_guids exclude("IServiceProvider")
#include "Timer.h"
#include <vector>

struct ElementInfo
{
	BOOL Updated;
	CComPtr<IDispatch> pFlashDisp;
	CComPtr<IDispatch> pTagDisp;
};
typedef std::vector<ElementInfo> ELEMENTLIST;


class CFlashManager : public CWindowImpl<CFlashManager>
{
public:
	CFlashManager();
	~CFlashManager();

BEGIN_MSG_MAP(CFlashManager)
	MESSAGE_HANDLER(WM_TIMER, OnTimer)
END_MSG_MAP()

enum FlashState
{
    FLASH_ERROR = 0,
    FLASH_ELEM_NOTREADY,
    FLASH_ACTIVATED,
    FLASH_LOADING
};


public:
	bool Activate(IHTMLDocument2Ptr pDoc, bool bOn, bool bContentEditable = false);
	void FlashCallback(IDispatch * pDisp1, IDispatch * pDisp2, DISPID id, VARIANT* pVarResult);
	BOOL Flash_WMode(IDispatch* pDisp, LPCTSTR szValue);
	BOOL Flash_ShowMenu(IDispatch* pDisp, BOOL bShow=TRUE);
	BOOL Flash_Start(IDispatch* pDisp);
	BOOL Flash_Play(IDispatch* pDisp);
	BOOL Flash_Rewind(IDispatch* pDisp);
	BOOL ShowMenu(IHTMLDocument2* pDoc, BOOL bShow);
	BOOL Enable(IHTMLDocument2* pDoc, BOOL bContentEditable);
	BOOL IsWsShellUsed(IDispatch* pDisp);
	BOOL IsWsShellUsed(){ return m_bWSShellUsed;};
	LRESULT OnTimer(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
//	static void CALLBACK ActivateFlashTimerProc(UINT wTimerID, UINT msg, DWORD dwUser, DWORD dwReserved1, DWORD dwReserved2);
//	void OnActivateFlashTimer();

protected:
	BOOL SetMouseOverCallback(ElementInfo &ElemInfo, BOOL bReset);
	BOOL SetMouseDownCallback(ElementInfo &ElemInfo, BOOL bReset);
	BOOL SetDragEndCallback(ElementInfo &ElemInfo, BOOL bReset);
	BOOL SetDocMouseDownCallback(IHTMLDocument2 *pDoc, BOOL bReset);
	BOOL SetDocPasteCallback(IHTMLDocument2 *pDoc, BOOL bReset);
	BOOL SetContentEditable(IDispatch* pDisp, BOOL bContentEditable);
	BOOL SetFlashAttributes(ElementInfo &ElemInfo, BOOL bContentEditable);
	BOOL SetAllFlashAttributes(IHTMLDocument2* pDoc, BOOL bContentEditable);
	BOOL ResetAllFlashCallbacks(IHTMLDocument2* pDoc);
	BOOL GetFlashElements(IHTMLDocument2* pDoc);
	FlashState FlashFocus(ElementInfo& eInfo);

protected:	
//	CTimer m_FlashTimer;
	CComPtr<IHTMLDocument2> m_pPrePasteTimerDoc;
	ELEMENTLIST m_FlashElementList;
	BOOL m_bWSShellUsed;
	BOOL m_bFlashTimerInUse;
	BOOL m_bPasteTimerInUse;
};
