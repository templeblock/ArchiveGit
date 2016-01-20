#pragma once
#include "TabControl.h"
#include "ImageControlConstants.h"
#include "Matrix.h"
#include "ArtGalleryDialog.h"
#include "ToolTipDialog.h"

// For some reason, the 'new' debug macro gets redefined during imports and some includes
#pragma push_macro("new")
#import "progid:ImageControl.ImageControl.1" raw_native_types
#pragma pop_macro("new")
using namespace ImageControlModule;

// CImageEdit dialog
class CImageEdit :	public CAxDialogImpl<CImageEdit>, public CWinDataExchange<CImageEdit>,
						public CDialogResize<CImageEdit>, public CToolTipDialog<CImageEdit>
{
public:
	CImageEdit();
	CImageEdit(const BITMAPINFOHEADER* pDIB, const CMatrix& Matrix, UINT idTab = IDD_IME_MAIN_TAB);
	~CImageEdit();

	BEGIN_MSG_MAP_EX(CImageEdit)
		CHAIN_MSG_MAP(CToolTipDialog<CImageEdit>) // ToolTipDialog chain message map on top
		COMMAND_HANDLER_EX(IDC_IME_STARTOVER, BN_CLICKED, OnStartOver)
//		COMMAND_HANDLER_EX(IDC_IME_STARTOVER1, BN_CLICKED, OnStartOver)
		COMMAND_HANDLER_EX(IDC_IME_IMAGE_SAVE, BN_CLICKED, OnImageSave)
//		COMMAND_HANDLER_EX(IDC_IME_IMAGE_SAVE1, BN_CLICKED, OnImageSave)
		COMMAND_HANDLER_EX(IDC_IME_ZOOMIN, BN_CLICKED, OnZoomIn)
		COMMAND_HANDLER_EX(IDC_IME_ZOOMOUT, BN_CLICKED, OnZoomOut)
		COMMAND_HANDLER_EX(IDC_IME_ZOOMFULL, BN_CLICKED, OnZoomFull)
		COMMAND_HANDLER_EX(IDC_IME_APPLYDONE, BN_CLICKED, OnDone)
		COMMAND_HANDLER_EX(IDC_IME_RESETDONE, BN_CLICKED, OnCancel)
		MSG_WM_INITDIALOG(OnInitDialog)
		MSG_WM_CLOSE(OnClose)
		MESSAGE_HANDLER_EX(WM_USER, OnGetObject);
		CHAIN_MSG_MAP(CAxDialogImpl<CImageEdit>)
		CHAIN_MSG_MAP(CDialogResize<CImageEdit>)
		REFLECT_NOTIFICATIONS()
	END_MSG_MAP()

	BEGIN_DDX_MAP(CImageEdit)
		DDX_CONTROL(IDC_IME_TAB_CONTROL, m_TabControl);
		DDX_CONTROL_HANDLE(IDC_IME_STARTOVER, m_btnStartOver);
		DDX_CONTROL_HANDLE(IDC_IME_IMAGE_SAVE, m_btnSave);
		DDX_CONTROL_HANDLE(IDC_IME_ZOOMFULL, m_iconZoomFull);
    END_DDX_MAP()

	BEGIN_DLGRESIZE_MAP(CImageEdit)
		DLGRESIZE_CONTROL(IDC_IME_IMAGE_CONTROL,	DLSZ_SIZE_X | DLSZ_SIZE_Y)
		DLGRESIZE_CONTROL(IDC_IME_TAB_CONTROL,		DLSZ_SIZE_Y)
		DLGRESIZE_CONTROL(IDC_IME_STARTOVER,		DLSZ_MOVE_Y)
//		DLGRESIZE_CONTROL(IDC_IME_STARTOVER1,		DLSZ_MOVE_Y)
		DLGRESIZE_CONTROL(IDC_IME_IMAGE_SAVE,		DLSZ_MOVE_Y)
//		DLGRESIZE_CONTROL(IDC_IME_IMAGE_SAVE1,		DLSZ_MOVE_Y)
		DLGRESIZE_CONTROL(IDC_IME_ZOOMIN,			DLSZ_MOVE_X | DLSZ_MOVE_Y)
		DLGRESIZE_CONTROL(IDC_IME_ZOOMOUT,			DLSZ_MOVE_X | DLSZ_MOVE_Y)
		DLGRESIZE_CONTROL(IDC_IME_ZOOMFULL,			DLSZ_MOVE_X | DLSZ_MOVE_Y)
		DLGRESIZE_CONTROL(IDC_IME_APPLYDONE,		DLSZ_MOVE_X | DLSZ_MOVE_Y)
		DLGRESIZE_CONTROL(IDC_IME_RESETDONE,		DLSZ_MOVE_X | DLSZ_MOVE_Y)
	END_DLGRESIZE_MAP()

	bool RegisterArtSetupCallback(PFNARTSETUPCALLBACK pfnArtSetupCallback, LPARAM lParam);
	void UnregisterArtSetupCallback();
	void ArtSetup(PFNARTGALLERYCALLBACK pfnArtGalleryCallback, LPARAM lArtGalParam);

	bool DoCommand(LPCTSTR strCommand, LPCTSTR strValue);
	long GetCommand(LPCTSTR strCommand);
	bool GetImage(BITMAPINFOHEADER*& pDIB, CMatrix& ppMatrix);

	CArtGalleryDialog* GetArtGalleryDialog()
		{ return m_pArtGalleryDialog; }
	void SetArtGalleryDialog(CArtGalleryDialog * pArtGalleryDialog)
		{ m_pArtGalleryDialog = pArtGalleryDialog; }
	void SetClipArtInfo(LPCTSTR szValue, bool bInitialState)
		{ m_szClipArtInfo = szValue; m_bIsClipArt = bInitialState; }
	CString& GetClipArtInfo()
		{ return m_szClipArtInfo; }
	bool IsClipArt()
		{ return m_bIsClipArt; }

// Dialog Data
	enum { IDD = IDD_IME_IMAGEEDIT_DIALOG };

protected:
	BOOL OnInitDialog(HWND hWnd, LPARAM lParam);
	bool InitializeImageControl();

// Implementation
protected:
	void OnStartOver(UINT uCode, int nID, HWND hwndCtrl);
	void OnImageSave(UINT uCode, int nID, HWND hwndCtrl);
	void OnZoomIn(UINT uCode, int nID, HWND hwndCtrl);
	void OnZoomOut(UINT uCode, int nID, HWND hwndCtrl);
	void OnZoomFull(UINT uCode, int nID, HWND hwndCtrl);
	void OnDone(UINT uCode, int nID, HWND hwndCtrl);
	void OnCancel(UINT uCode, int nID, HWND hwndCtrl);
	void OnClose();
	LRESULT OnGetObject(UINT uMsg, WPARAM wParam, LPARAM lParam);

private:
	HICON m_hIcon;
	CComQIPtr<IImageControl> m_ImageControl;
	CTabControl m_TabControl;
	CStatic m_iconZoomFull;
	CButton m_btnStartOver;
	CButton m_btnSave;
	const BITMAPINFOHEADER* m_pDIB;
	CMatrix m_Matrix;
	UINT m_idTab;
	CString m_szClipArtInfo;
	bool m_bIsClipArt;
	CArtGalleryDialog* m_pArtGalleryDialog;
	PFNARTSETUPCALLBACK m_pfnArtSetupCallback;
	LPARAM m_pArtSetupCallbackParam;
};
