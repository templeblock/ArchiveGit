// BandToolBarCtrl.h: interface for the CBandToolBarCtrl class.
//
//////////////////////////////////////////////////////////////////////
#pragma once

#include "resource.h"
#include "BandEditCtrl.h"
#include "JMExplorerBand.h"
#include "MsAddInBase.h"


#define EDIT_WIDTH 200
#define COMBO_HEIGHT 130


const UINT ID_JMBTN1_CMD	= WM_APP + 101;
const UINT ID_JMSEARCH_CMD	= WM_APP + 102;
const UINT ID_JMEDIT_CMD	= WM_APP + 103;



class CBandToolBarCtrl : 
		public CWindowImpl<CBandToolBarCtrl, CToolBarCtrl>,
		public CMsAddInBase
{
public:
	CBandToolBarCtrl();
	virtual ~CBandToolBarCtrl();

    enum TextOptions
    {
        toNone          = -1,
        toTextLabels    =  0,
        toTextOnRight   =  1,
        toNoTextLabels  =  2,
    };


    DECLARE_WND_SUPERCLASS( _T("BandToolBarCtrl"), CToolBarCtrl::GetWndClassName() )
    HWND CreateToolBarCtrl(HWND hWndParent, RECT &rect, DWORD dwStyle = 0, DWORD dwExStyle = 0);
    void UpdateToolBarElement(int nID, bool bEnable);
	BOOL SetBandRebar();
    void SetToolBarButtonInfo(int nID, DWORD dwMask, BYTE fsStyle);
    void SetWebBrowser(IWebBrowser2* pWebBrowser);
	void SetToolBand(CJMToolband *pToolband);
	bool HandleOptionsButton(CRect &rc);
	bool ShowExplorerbar(LPCTSTR szCLSID, bool bShow);

    BEGIN_MSG_MAP(CBandToolBarCtrl)	
        MESSAGE_HANDLER(OCM_COMMAND, OnCommand)
		OCM_COMMAND_ID_HANDLER(ID_JMBTN1_CMD, OnJMButton1)
		OCM_COMMAND_ID_HANDLER(ID_JMSEARCH_CMD, OnJMSearch)
		OCM_NOTIFY_CODE_HANDLER(TBN_DROPDOWN, OnToolbarDropDown)
        NOTIFY_CODE_HANDLER(TTN_NEEDTEXT, OnToolbarNeedText)
        MESSAGE_HANDLER(WM_SIZE, OnSize)
        MESSAGE_HANDLER(WM_MENUSELECT, OnMenuSelect)
        DEFAULT_REFLECTION_HANDLER()
	// handle the rebar chevron messages, does not require reflection
	ALT_MSG_MAP(1)
		NOTIFY_CODE_HANDLER(TBN_DROPDOWN, OnToolbarDropDown)
	END_MSG_MAP()

public:
	// Pure virtual functions that must be implemented by derived classes. 
	virtual bool LaunchMail(bool bAllowReuse, const CString& strDownloadSrc, const CString& strDownloadDst, const CString& strSubject){return true;};
	virtual bool LaunchFeedbackMail(DWORD dwCmd){return true;};
	virtual bool EmbedHTMLAtSelection(const CString& strHTML) {return true;};
	virtual bool EmbedHTMLAtEnd(const CString& strHTMLText){return true;};


public:

    //CBandEditCtrl m_ctlBandEdit;
    CBandComboBoxCtrl m_ctlBandComboBox;
	CContainedWindow m_RebarContainer;
	CReBarCtrl m_ctlRebar;
	TextOptions  m_eTextOptions;
    CComPtr<IWebBrowser2> m_spWebBrowser;

    STDMETHOD(TranslateAcceleratorIO)(LPMSG pMsg);

protected:
	struct _AtlToolBarData
	{
		WORD wVersion;
		WORD wWidth;
		WORD wHeight;
		WORD wItemCount;
		WORD* items()
			{ return (WORD*)(this+1); }
	};


    LRESULT OnCommand(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& bHandled);	
	LRESULT OnJMButton1(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnJMSearch(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
    LRESULT OnToolbarDropDown(int /*idCtrl*/, LPNMHDR pnmh, BOOL& /*bHandled*/);
    LRESULT OnToolbarNeedText(int /*idCtrl*/, LPNMHDR pnmh, BOOL& /*bHandled*/);
	LRESULT OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
    LRESULT OnMenuSelect(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& bHandled); 

	HWND FindRebar(HWND hwndStart);

protected:
	bool m_bExplorerbarVisible;
	//HIMAGELIST m_hImageList;
	CImageList m_ImageList;
    int m_ncy;
    BOOL m_bInitialSeparator;
    UINT m_nResourceID;

};


