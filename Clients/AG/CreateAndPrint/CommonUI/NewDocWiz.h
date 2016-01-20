#pragma once

#include "PropertyPage.h"
#include "AGDoc.h"
#include "Resource.h"

#define MAX_ITEMS 40
#define ITEM_WIDTH 120
#define ITEM_HEIGHT 33

/////////////////////////////////////////////////////////////////////////////
// CNewDocData
class CNewDocData
{
public:
	int m_nSelectedItem;
	AGDOCTYPE m_DocType;
	int m_PaperType;
	bool m_bFinished;
	bool m_bIsPortrait;
	bool m_bStdSize;
	bool m_bStdNumPages;
	bool m_bStdPageNames;

	double m_fStdWidth;
	double m_fWidth;

	double m_fStdHeight;
	double m_fHeight;

	int m_iStdNumPages;
	int m_iNumPages;

	CString m_strStdPageNames[MAX_AGPAGE];
	CString m_strPageNames[MAX_AGPAGE];

public:
	CNewDocData();
	void Init();
	void SetNewDocPageData();
	void SetNewDocSizeData();
};

/////////////////////////////////////////////////////////////////////////////
// CDocList
class CDocList : public CWindowImpl<CDocList>
{
public:
	void Reset();
	int GetSelectedItem();
	void SelectItem(int idIcon);
	void AddItem(AGDOCTYPE idDocType, int idIcon, bool bEnable);
	LRESULT OnDrawItem(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnMeasureItem(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnKeyUp(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnLButtonDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnLButtonDblClk(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

public:
	BEGIN_MSG_MAP(CDocList)
		MESSAGE_HANDLER(WM_MEASUREITEM, OnMeasureItem)
		MESSAGE_HANDLER(WM_DRAWITEM, OnDrawItem)
		MESSAGE_HANDLER(WM_KEYUP, OnKeyUp)
		MESSAGE_HANDLER(WM_LBUTTONDOWN, OnLButtonDown)
		MESSAGE_HANDLER(WM_LBUTTONDBLCLK, OnLButtonDblClk)
	END_MSG_MAP()

protected:
	int m_nItems;
	int m_idIcons[MAX_ITEMS];
	bool m_bIconsEnabled[MAX_ITEMS];
};

/////////////////////////////////////////////////////////////////////////////
// CNewDocWizIntro
class CNewDocWizIntro : public CPropertyPageEx<CNewDocWizIntro>
{
public:
	enum { IDD = IDD_NEWDOCWIZINTRO };

	CNewDocWizIntro();
	BOOL OnSetActive();
	BOOL OnKillActive();
	BOOL OnWizardNext();
	CNewDocData* GetFinishedData()
		{ return (m_NewDocData.m_bFinished ? &m_NewDocData : NULL); }

	LRESULT OnClickList(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnDoubleClickList(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);

	LRESULT OnDrawItem(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		return m_DocList.OnDrawItem(uMsg, wParam, lParam, bHandled);
	}

	LRESULT OnMeasureItem(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		return m_DocList.OnMeasureItem(uMsg, wParam, lParam, bHandled);
	}

public:
	BEGIN_MSG_MAP(CNewDocWizIntro)
		MESSAGE_HANDLER(WM_MEASUREITEM, OnMeasureItem)
		MESSAGE_HANDLER(WM_DRAWITEM, OnDrawItem)
		COMMAND_HANDLER(IDC_NEWDOC_LIST, BN_CLICKED, OnClickList)
		COMMAND_HANDLER(IDC_NEWDOC_LIST, 999/*BN_DBLCLK*/, OnDoubleClickList)
		CHAIN_MSG_MAP(CPropertyPageEx<CNewDocWizIntro>)
	END_MSG_MAP()

protected:
	CNewDocData m_NewDocData;
	CNewDocData* m_Data;
	CDocList m_DocList;
};


/////////////////////////////////////////////////////////////////////////////
// CNewDocWizStep1
class CNewDocWizStep1 : public CPropertyPageEx<CNewDocWizStep1>
{
public:
	enum { IDD = IDD_NEWDOCWIZSTEP1 };
	
	CNewDocWizStep1();
	BOOL OnSetActive();
	BOOL OnKillActive();
	BOOL OnWizardNext();

	void SwapSizeCheck(bool bSetControls);
	LRESULT OnLandscape(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnPortrait(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnStdSize(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnPaperType(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	void SwapOrientationCheck(bool bSetControls);
	void UpdateSample(int idSample);
	LRESULT OnWidth(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnHeight(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);

public:
	BEGIN_MSG_MAP(CNewDocWizStep1)
		COMMAND_HANDLER(IDC_NEWDOC_LANDSCAPE, BN_CLICKED, OnLandscape)
		COMMAND_HANDLER(IDC_NEWDOC_PORTRAIT, BN_CLICKED, OnPortrait)
		COMMAND_HANDLER(IDC_NEWDOC_PAPERTYPE, CBN_SELCHANGE, OnPaperType)
		COMMAND_HANDLER(IDC_NEWDOC_STDSIZE, BN_CLICKED, OnStdSize)
		COMMAND_HANDLER(IDC_NEWDOC_WIDTH, EN_CHANGE, OnWidth)
		COMMAND_HANDLER(IDC_NEWDOC_HEIGHT, EN_CHANGE, OnHeight)
		CHAIN_MSG_MAP(CPropertyPageEx<CNewDocWizStep1>)
	END_MSG_MAP()

protected:
	CNewDocData* m_Data;
	RECT m_SampleRect;
};

/////////////////////////////////////////////////////////////////////////////
// CNewDocWizStep2
class CNewDocWizStep2 : public CPropertyPageEx<CNewDocWizStep2>
{
public:
	enum { IDD = IDD_NEWDOCWIZSTEP2 };

	CNewDocWizStep2();
	BOOL OnSetActive();
	BOOL OnKillActive();
	BOOL OnWizardNext();

	LRESULT OnStdNumPages(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnNumPages(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnStdPageNames(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnPageName1(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnPageName2(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnPageName3(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnPageName4(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnPageName5(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnPageName6(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnPageName7(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnPageName8(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);

public:
	BEGIN_MSG_MAP(CNewDocWizStep2)
		COMMAND_HANDLER(IDC_NEWDOC_STDNUMPAGES, BN_CLICKED, OnStdNumPages)
		COMMAND_HANDLER(IDC_NEWDOC_NUMPAGES, EN_CHANGE, OnNumPages)
		COMMAND_HANDLER(IDC_NEWDOC_STDPAGENAMES, BN_CLICKED, OnStdPageNames)
		COMMAND_HANDLER(IDC_NEWDOC_PAGENAME1, EN_CHANGE, OnPageName1)
		COMMAND_HANDLER(IDC_NEWDOC_PAGENAME2, EN_CHANGE, OnPageName2)
		COMMAND_HANDLER(IDC_NEWDOC_PAGENAME3, EN_CHANGE, OnPageName3)
		COMMAND_HANDLER(IDC_NEWDOC_PAGENAME4, EN_CHANGE, OnPageName4)
		COMMAND_HANDLER(IDC_NEWDOC_PAGENAME5, EN_CHANGE, OnPageName5)
		COMMAND_HANDLER(IDC_NEWDOC_PAGENAME6, EN_CHANGE, OnPageName6)
		COMMAND_HANDLER(IDC_NEWDOC_PAGENAME7, EN_CHANGE, OnPageName7)
		COMMAND_HANDLER(IDC_NEWDOC_PAGENAME8, EN_CHANGE, OnPageName8)
		CHAIN_MSG_MAP(CPropertyPageEx<CNewDocWizStep2>)
	END_MSG_MAP()

protected:
	CNewDocData* m_Data;
};

/////////////////////////////////////////////////////////////////////////////
// CNewDocWizEnd
class CNewDocWizEnd : public CPropertyPageEx<CNewDocWizEnd>
{
public:
	enum { IDD = IDD_NEWDOCWIZEND };

	CNewDocWizEnd();
	BOOL OnSetActive();
	BOOL OnKillActive();
	BOOL OnWizardFinish();

public:
	BEGIN_MSG_MAP(CNewDocWizEnd)
		CHAIN_MSG_MAP(CPropertyPageEx<CNewDocWizEnd>)
	END_MSG_MAP()

protected:
	CNewDocData* m_Data;
};
