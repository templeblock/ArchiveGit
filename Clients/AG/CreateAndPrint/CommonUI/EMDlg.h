#pragma once

#include "resource.h"
#include "DocWindow.h"
#include "AGSym.h"
#include "AGPage.h"
#include "AGLayer.h"
#include "AGDoc.h"
#include "AGMatrix.h"
#include <map>
#include "EMTabControl.h"
#include "ToolTipDialog.h"


class CAGBrush;


#define	ENVELOPE_SIZE			1
#define	ENVELOPE_PICTURE		2
#define	ENVELOPE_ADDRESS		3
#define	ENVELOPE_FINAL			4
#define	MAX_NUMBER_VIEWS		4

class CEMDlg;


///////////////////////////////////////////////////////////////////////////////
class CEMDlg : public CDialogImpl<CEMDlg>,
			   public CWinDataExchange<CEMDlg>,
			   public CToolTipDialog<CEMDlg>

{
public:
	CEMDlg(HWND hWnd, CDocWindow* pDocWindow, CAGDoc* pPrevDoc, AGDOCTYPE DocType, UINT iTabID = IDD_EMADDRESS_TAB);
	~CEMDlg();

	enum { IDD = IDD_EM_DIALOG };

public:
	BEGIN_MSG_MAP(CEMDlg)
		CHAIN_MSG_MAP(CToolTipDialog<CEMDlg>) // ToolTipDialog chain message map on top
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
        MESSAGE_HANDLER(WM_ACTIVATE, OnActivate)
		MESSAGE_HANDLER_EX(WM_USER, OnGetObject);
		COMMAND_HANDLER(IDC_BUTTON_CANCEL, BN_CLICKED, OnFinished)
		COMMAND_HANDLER(IDCANCEL, BN_CLICKED, OnFinished)
        COMMAND_HANDLER(IDC_BUTTON_OK, BN_CLICKED, OnFinished)
		COMMAND_HANDLER(IDC_BUTTON_PRINTPREVIEW, BN_CLICKED, OnPrintPreveiw)
		REFLECT_NOTIFICATIONS()
	END_MSG_MAP()

	BEGIN_DDX_MAP(CEMDlg)
		DDX_CONTROL(IDC_EM_TABCTRL, m_TabControl);
	END_DDX_MAP()

	LRESULT OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnActivate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnFinished(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnPrintPreveiw(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnGetObject(UINT uMsg, WPARAM wParam, LPARAM lParam);


    void UpdateControls();
	void UpdateMailingAddress();
	void FinishImageDrop(bool fDrop, POINTL pt);
	void InitEnvelopeSize();
	bool WriteDialogData();
    bool SetEnvelopeDoc(CAGDoc* pAGDoc);
	void SetImageSize(CAGSymImage* pImage, POINT* pCenterPoint = NULL);
	void SetWidthHeightControls();
	void SetCurrentTab(UINT iTabID);
	void CreateEnvelope(CAGDoc** ppAGDoc);
	void SetPageSize();
	void RecalcTextRects(int nWidth, int nHeight);
	void RecalcGraphic();
	void RecalcBackgroundRect(int nWidth, int nHeight);
	
	AGDOCTYPE GetDocType()
		{ return m_DocType;}

	CAGDoc* GetDoc()
		{ return m_pEnvelopeDoc; }

	CDocWindow* GetDocWin()
		{ return m_pDocWindow; }

	int GetMaxImageSize()
		{ return m_nMaxImageSize; }

	SIZE& GetPageSize()
		{ return m_PageSize; }

	void SetPageSize(SIZE& PageSize)
		{ m_PageSize = PageSize;}
	
	void SetMaxImageSize(int nValue)
		{m_nMaxImageSize = nValue;}


CAGDoc * m_pPrevDoc;
protected:
	CEMTabControl m_TabControl;
	UINT m_iTabID;
	CAGBrush m_Brush;
	int m_nMaxImageSize;

	

public:
	CDocWindow* m_pDocWindow;
	CAGDoc* m_pEnvelopeDoc;
	AGDOCTYPE m_DocType;
	SIZE m_PageSize;
	
};

