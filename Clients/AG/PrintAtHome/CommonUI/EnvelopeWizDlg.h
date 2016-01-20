#pragma once

#include "resource.h"
#include "DocWindow.h"
#include "AGSym.h"
#include "AGPage.h"
#include "AGLayer.h"
#include "AGDoc.h"
#include "AGMatrix.h"
#include <map>

class CAGBrush;

#define MAX_NUM_PICTURES		25

#define	ENVELOPE_SIZE			1
#define	ENVELOPE_PICTURE		2
#define	ENVELOPE_ADDRESS		3
#define	ENVELOPE_FINAL			4
#define	MAX_NUMBER_VIEWS		4

class CEnvelopeWizard;

///////////////////////////////////////////////////////////////////////////////
class CPictureListView : public CWindowImpl<CPictureListView, CListViewCtrl>
{
public:
	CPictureListView();
	virtual ~CPictureListView();

	void InitDialog(HWND hWnd);
	void SetParent(CEnvelopeWizard* pParent);
	void DrawListViewThumbnails();

protected:
	BEGIN_MSG_MAP(CPictureListView)
	END_MSG_MAP()

	CEnvelopeWizard* m_pParent;
	CImageList m_ImageList;
	int m_nThumbnailSize;
};
///////////////////////////////////////////////////////////////////////////////
class CEnvelopeWizard : public CDialogImpl<CEnvelopeWizard>
{
public:
	CEnvelopeWizard(HWND hWnd, CDocWindow* pDocWindow, CAGDoc* pPrevDoc, AGDOCTYPE DocType);
	~CEnvelopeWizard();

	enum { IDD = IDD_ENVELOPESETTINGS_DIALOG };

public:
	BEGIN_MSG_MAP(CEnvelopeWizard)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
        MESSAGE_HANDLER(WM_ACTIVATE, OnActivate)
		MESSAGE_HANDLER(WM_CTLCOLORSTATIC, OnCtlColor)
		COMMAND_HANDLER(IDCANCEL, BN_CLICKED, OnFinished)
        COMMAND_HANDLER(IDOK, BN_CLICKED, OnFinished)
        COMMAND_HANDLER(IDC_NEWDOC_PAPERTYPE, CBN_SELCHANGE, OnPaperType)
		COMMAND_HANDLER(IDC_ENABLE_BACKGROUND, BN_CLICKED, OnEnableBackground)
        COMMAND_HANDLER(IDC_SEND_ADDRESS, EN_CHANGE, OnApplySendAddress)
		COMMAND_HANDLER(IDC_RETURN_ADDRESS, EN_CHANGE, OnApplyReturnAddress)
		COMMAND_HANDLER(IDC_SEND_ADDRESS, EN_SETFOCUS, OnSelectSendAddress)
		COMMAND_HANDLER(IDC_RETURN_ADDRESS, EN_SETFOCUS, OnSelectReturnAddress)
        NOTIFY_HANDLER(IDC_PICTURE_SELECT, LVN_BEGINDRAG, OnLvnBegindragPictureSelect)
		REFLECT_NOTIFICATIONS()
	END_MSG_MAP()

	LRESULT OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnActivate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnCtlColor(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnClickedNext(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnClickedPrev(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnFinished(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
    LRESULT OnEnableBackground(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnPaperType(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
    LRESULT OnApplySendAddress(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnApplyReturnAddress(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnSelectSendAddress(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnSelectReturnAddress(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnLvnBegindragPictureSelect(int /*idCtrl*/, LPNMHDR pNMHDR, BOOL& bHandled);
	
    void UpdateControls();
	void FinishImageDrop(bool fDrop, POINTL pt);
	void OnBegindragListThumbnail(NMHDR* pNMHDR, LRESULT* pResult);
    bool WriteDialogData();
	void SetWidthHeightControls();
	void SelectBackground();
	void ShowSelectedView(int nSelectedView);
    bool SetEnvelopeDoc(CAGDoc* pAGDoc);
	void CreateEnvelope(CAGDoc** ppAGDoc);
	void SetImageSize(CAGSymImage* pImage, POINT* pCenterPoint = NULL);
	void SetPageSize();
	void InitGraphics(CAGDoc* pAGDoc);
    void InitEnvelopeSize();
    void InitEnvelopeAddress();
    void InitPictureList();
	void SetAddressText(CAGSymText* pText, CString& strText, bool bClearText = true);
	void RecalcTextRects(int nWidth, int nHeight);
	void RecalcGraphic();
	void RecalcBackgroundRect(int nWidth, int nHeight);

	AGDOCTYPE GetDocType()
		{ return m_DocType;}

	void AddDroppedGraphic(int nDragImage, POINT point );

	CAGDoc* GetDoc()
		{ return m_pEnvelopeDoc; }

	CDocWindow* GetDocWin()
		{ return m_pDocWindow; }

	CPictureListView* GetPictureList()
		{ return &m_PictureList; }

private:
	HBRUSH GetBgBrush(COLORREF Color)
	{
		HBRUSH hBrush = m_Brush.GetHBrush();
		if (hBrush && m_LastColor == Color)
			return hBrush;

		hBrush = m_Brush.Create(Color);
		if (!hBrush)
			return NULL;

		m_LastColor = Color;
		return hBrush;
	}

public:
    CAGSymImage* m_pGraphics[MAX_NUM_PICTURES];

protected:
    UINT m_nSelectedSym;
    CPictureListView m_PictureList;
	int m_nDragImage;
	CAGBrush m_Brush;
	COLORREF m_LastColor;

public:
    HFONT m_hHdrFont;
    HFONT m_hHdrFont2;
	CDocWindow* m_pDocWindow;
	CAGDoc* m_pEnvelopeDoc;
	AGDOCTYPE m_DocType;
	SIZE m_PageSize;
	int m_nSelectedView;
	int m_PaperType;
	int m_nMaxImageSize;
	bool m_EnableBackground;
	CString m_strSendAddress;
	CString m_strReturnAddress;
};

