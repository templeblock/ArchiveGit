#pragma once

#include "resource.h"
#include "DocWindow.h"
#include "AGSym.h"
#include "AGPage.h"
#include "AGLayer.h"
#include "AGDoc.h"
#include "AGMatrix.h"
#include <map>

#define MAX_NUM_PICTURES		25

#define	ENVELOPE_SIZE			1
#define	ENVELOPE_PICTURE		2
#define	ENVELOPE_ADDRESS		3
#define	ENVELOPE_FINAL			4
#define	MAX_NUMBER_VIEWS		4

class CEnvelopeWizard;
class CEnvelopeWizPicture;

///////////////////////////////////////////////////////////////////////////////
class CPictureListView : public CWindowImpl<CPictureListView, CListViewCtrl>
{
public:
	CPictureListView();
	virtual ~CPictureListView();

	void InitDialog(HWND hWnd);
	void SetParent(CEnvelopeWizPicture* pParent);
	void DrawListViewThumbnails();

protected:
	BEGIN_MSG_MAP(CPictureListView)
	END_MSG_MAP()

	CEnvelopeWizPicture* m_pParent;
	CImageList m_ImageList;
	int m_nThumbnailSize;
};

///////////////////////////////////////////////////////////////////////////////
class CEnvelopeWizSize : public CDialogImpl<CEnvelopeWizSize>
{
public:
	enum { IDD = IDD_ENVELOPE_SIZE_SELECT };

	CEnvelopeWizSize(CEnvelopeWizard* pParent);
	virtual ~CEnvelopeWizSize();

	LRESULT OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnEnableBackground(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnPaperType(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	void SetWidthHeightControls();
	void SelectBackground();
	CEnvelopeWizard* GetEnvelopeWizard()
		{ return m_pParent; };

public:
	BEGIN_MSG_MAP(CEnvelopeWizSize)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		COMMAND_HANDLER(IDC_NEWDOC_PAPERTYPE, CBN_SELCHANGE, OnPaperType)
		COMMAND_HANDLER(IDC_ENABLE_BACKGROUND, BN_CLICKED, OnEnableBackground)
	END_MSG_MAP()

protected:
	CEnvelopeWizard* m_pParent;
};

/////////////////////////////////////////////////////////////////////////////
class CEnvelopeWizAddress : public CDialogImpl<CEnvelopeWizAddress>
{
public:
	enum { IDD = IDD_ENVELOPE_ADDRESS_SELECT };
	
	CEnvelopeWizAddress(CEnvelopeWizard* pParent);
	virtual ~CEnvelopeWizAddress();

	LRESULT OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnApplySendAddress(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnApplyReturnAddress(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnSelectSendAddress(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnSelectReturnAddress(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	bool WriteDialogData();
	void UpdateControls();
	CEnvelopeWizard* GetEnvelopeWizard()
		{ return m_pParent; };

public:
	BEGIN_MSG_MAP(CEnvelopeWizAddress)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		COMMAND_HANDLER(IDC_SEND_ADDRESS, EN_CHANGE, OnApplySendAddress)
		COMMAND_HANDLER(IDC_RETURN_ADDRESS, EN_CHANGE, OnApplyReturnAddress)
		COMMAND_HANDLER(IDC_SEND_ADDRESS, EN_SETFOCUS, OnSelectSendAddress)
		COMMAND_HANDLER(IDC_RETURN_ADDRESS, EN_SETFOCUS, OnSelectReturnAddress)
	END_MSG_MAP()

protected:
	CEnvelopeWizard* m_pParent;
	UINT m_nSelectedSym;
};

/////////////////////////////////////////////////////////////////////////////
class CEnvelopeWizPicture : public CDialogImpl<CEnvelopeWizPicture>//, public IDropSource, public IDataObject
{
public:
	enum { IDD = IDD_ENVELOPE_PICT_SELECT };

	CEnvelopeWizPicture(CEnvelopeWizard* pParent);
	virtual ~CEnvelopeWizPicture();

	LRESULT OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnLvnBegindragPictureSelect(int /*idCtrl*/, LPNMHDR pNMHDR, BOOL& bHandled);
	void UpdateControls();
	void FinishImageDrop(bool fDrop, POINTL pt);
	void OnBegindragListThumbnail(NMHDR* pNMHDR, LRESULT* pResult);
	CEnvelopeWizard* GetEnvelopeWizard()
		{ return m_pParent; };
	CPictureListView* GetPictureList()
		{ return &m_PictureList; }

public:
	BEGIN_MSG_MAP(CEnvelopeWizPicture)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		NOTIFY_HANDLER(IDC_PICTURE_SELECT, LVN_BEGINDRAG, OnLvnBegindragPictureSelect)
	END_MSG_MAP()

protected:
	CEnvelopeWizard* m_pParent;
	CPictureListView m_PictureList;
	int m_nDragImage;
};

/////////////////////////////////////////////////////////////////////////////
class CEnvelopeWizFinal : public CDialogImpl<CEnvelopeWizFinal>
{
public:
	enum { IDD = IDD_ENVELOPE_FINAL };

	CEnvelopeWizFinal(CEnvelopeWizard* pParent);
	virtual ~CEnvelopeWizFinal();

	LRESULT OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	CEnvelopeWizard* GetEnvelopeWizard()
		{ return m_pParent; };

public:
	BEGIN_MSG_MAP(CEnvelopeWizFinal)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
	END_MSG_MAP()

protected:
	CEnvelopeWizard* m_pParent;
};

///////////////////////////////////////////////////////////////////////////////
class CEnvelopeWizard : public CDialogImpl<CEnvelopeWizard>
{
public:
	CEnvelopeWizard(HWND hWnd, CDocWindow* pDocWindow, CAGDoc* pPrevDoc, AGDOCTYPE DocType);
	~CEnvelopeWizard();

	enum { IDD = IDD_ENVELOPEWIZARD };

public:
	BEGIN_MSG_MAP(CEnvelopeWizard)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		COMMAND_HANDLER(ID_PREV_VIEW, BN_CLICKED, OnClickedPrev)
		COMMAND_HANDLER(ID_NEXT_VIEW, BN_CLICKED, OnClickedNext)
		COMMAND_HANDLER(IDCANCEL, BN_CLICKED, OnFinished)
		MESSAGE_HANDLER(WM_ACTIVATE, OnActivate)
		REFLECT_NOTIFICATIONS()
	END_MSG_MAP()

	LRESULT OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnActivate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnClickedNext(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnClickedPrev(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnFinished(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);

	void ShowSelectedView(int nSelectedView);
	void CreateEnvelope(CAGDoc** ppAGDoc);
	void SetImageSize(CAGSymImage* pImage, POINT* pCenterPoint = NULL);
	void SetPageSize();
	void InitGraphics(CAGDoc* pAGDoc);
	void SetAddressText(CAGSymText* pText, CString& strText);
	void RecalcTextRects(int nWidth, int nHeight);
	void RecalcGraphic();
	void RecalcBackgroundRect(int nWidth, int nHeight);
	AGDOCTYPE GetDocType()
		{ return m_DocType;}
	void AddDroppedGraphic(int nDragImage, POINT point );
	void FinishImageDrop(bool fDrop, POINTL pt)
		{ m_pEnvelopePicture->FinishImageDrop(fDrop, pt); }
	CAGDoc* GetDoc()
		{ return m_pEnvelopeDoc; }
	CDocWindow* GetDocWin()
		{ return m_pDocWindow; }

protected:
	CEnvelopeWizSize* m_pEnvelopeSize;
	CEnvelopeWizAddress* m_pEnvelopeAddress;
	CEnvelopeWizPicture* m_pEnvelopePicture;
	CEnvelopeWizFinal* m_pEnvelopeFinal;

public:
	CDocWindow* m_pDocWindow;
	CAGDoc* m_pEnvelopeDoc;
	AGDOCTYPE m_DocType;
	CAGSymImage* m_pGraphics[MAX_NUM_PICTURES];
	SIZE m_PageSize;
	int m_nSelectedView;
	int m_PaperType;
	int m_nMaxImageSize;
	bool m_EnableBackground;
	CString m_strSendAddress;
	CString m_strReturnAddress;
};

