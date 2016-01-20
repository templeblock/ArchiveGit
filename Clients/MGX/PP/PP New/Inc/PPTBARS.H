#ifndef _PPTOOLS_H
#define _PPTOOLS_H

#include "forum.h"
#include "message.h"
#include "octools.h"

class COCRibbon;
class COCStatus;

class CPPToolBar : public COCToolBar
{
	DECLARE_DYNAMIC(CPPToolBar)
public:
	CPPToolBar();
	virtual ~CPPToolBar();
	virtual BOOL Load();
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	HWND GetColorPatch();
	BOOL LoadMain();
	void InvalidateColorPatches(BOOL Erase);
	void NotifyColorPatches(int what);
	void OnActiveColor(HWND hDlg, int id, UINT codeNotify);
	void UpdateActiveTool(int id);
	BOOL PreTranslateMessage(MSG* pMsg);
protected:
	//{{AFX_MSG(CPPToolBar)
	afx_msg void OnOK();
	afx_msg void OnMeasureItem(int nIDCtl, LPMEASUREITEMSTRUCT lpMeasureItem);
	afx_msg void OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItem);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnInitMenuPopup(CMenu* pPopupMenu, UINT nIndex,
										BOOL bSysMenu);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

class  COCWinButton : public COCButton
{
public:
	virtual ~COCWinButton();
	COCWinButton();
	COCWinButton(UINT id, CString Str, CString ClassName, DWORD Style, CSize S);
	virtual BOOL Create(CWnd *pParent);
	virtual void Move(CPoint Pt, BOOL Horz);
	virtual COCButton* Duplicate();
	virtual BOOL Notify(int nCode);
	virtual void Serialize(CArchive &ar);
	virtual void  CalcSize(BOOL Large);
	BOOL Init(CWnd *pParent);
public:
	CFontCombo	FontCombo;
	CZoomCombo	m_ZoomCombo;
	CStatusMonitor	m_StatusMonitor;
 	DECLARE_SERIAL(COCWinButton);
};

class COCRibbon : public COCToolBar
{
	DECLARE_DYNAMIC(COCRibbon)
public:
	COCRibbon();
	void LoadRibbon();
	virtual BOOL Create(CWnd* pParentWnd,
			DWORD dwStyle = WS_CHILD | WS_VISIBLE | CBRS_TOP,
			UINT nID = AFX_IDW_TOOLBAR);
	BOOL AttachRibbon(HWND	hWnd);
	virtual CSize GetDockedSize(BOOL bHorz, BOOL Update = TRUE);
	virtual BOOL InsertButton(COCButton *pButton, int loc = -1, BOOL ReBuild = TRUE){return FALSE;};
	virtual BOOL DeleteButton(int loc, BOOL ReBuild = TRUE, BOOL DelWin = TRUE){return FALSE;};
	virtual BOOL SetButtons(const OCBUTTON* lpIDArray, int nIDCount){return FALSE;};
	virtual void InsertSeparator(int loc){};
	virtual BOOL DeleteSeparator(int loc){return FALSE;};
protected:
	//{{AFX_MSG(COCRibbon)
		afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

class COCStatus : public COCToolBar
{
	DECLARE_DYNAMIC(COCStatus)
public:
	COCStatus();
	~COCStatus();
	void LoadStatus();
	virtual BOOL Create(CWnd* pParentWnd,
			DWORD dwStyle = WS_CHILD | WS_VISIBLE | CBRS_BOTTOM,
			UINT nID = AFX_IDW_TOOLBAR);
	BOOL AttachRibbon(HWND	hWnd);
	virtual BOOL ShowToolBar(CFrameWnd *pFrame, CWnd *pOwner = NULL);
	virtual CSize GetDockedSize(BOOL bHorz, BOOL Update = TRUE);
	virtual BOOL InsertButton(COCButton *pButton, int loc = -1, BOOL ReBuild = TRUE){return FALSE;};
	virtual BOOL DeleteButton(int loc, BOOL ReBuild = TRUE, BOOL DelWin = TRUE){return FALSE;};
	virtual BOOL SetButtons(const OCBUTTON* lpIDArray, int nIDCount){return FALSE;};
	virtual void InsertSeparator(int loc){};
	virtual BOOL DeleteSeparator(int loc){return FALSE;};
protected:
	//{{AFX_MSG(COCStatus)
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	CRect	m_rect;
	CStatusMonitor	m_StatusMonitor;
};

class CPPToolList : public COCToolList
{
public:
	CPPToolList();
	virtual ~CPPToolList();
	DECLARE_DYNAMIC(CPPToolList)
	BOOL CreateStatus(CFrameWnd *pFrame, CWnd *pOwner){return m_pStatus->ShowToolBar(pFrame, pOwner);};
	BOOL CreateMain(CFrameWnd *pFrame, CWnd *pOwner = NULL){return m_pMain->ShowToolBar(pFrame, pOwner);}
	BOOL CreateRibbon(CFrameWnd *pFrame, CWnd *pOwner = NULL){return m_pRibbon->ShowToolBar(pFrame, pOwner);}
	void ResetStatus(CFrameWnd *pFrame, CWnd *pOwner = NULL);
	void ResetMain(CFrameWnd *pFrame, CWnd *pOwner = NULL);
	virtual BOOL Save();
	virtual BOOL Load(CFrameWnd *pFrame, CWnd *pOwner = NULL);
	virtual COCToolBar* NewToolBar();
	CFile* GetFile(LPCTSTR lpszFileName, UINT nOpenFlags);
	COCRibbon*	GetRibbon();
	COCStatus*	GetStatus();
	CPPToolBar* GetMain();
	void InvalidateColorPatches(BOOL Erase);
	void NotifyOfActiveTool(int cat, int id);
	void NotifyColorPatches(int what);
	void UpdateStatusButton(int id, BOOL b);
	BOOL IsMainUp();
	void ShowBars(BOOL bSHow);
	void SetDlgCtrl(HWND hDlg, HWND hControl, UINT codeNotify);
	BOOL Remove(CPPToolBar *pBox);

protected:
	//{{AFX_MSG(CPPToolList)
	 afx_msg void OnUpdateZoomCombo(CCmdUI* pCmdUI);
    afx_msg void OnUpdateRMCheck(CCmdUI* pCmdUI);
    afx_msg void OnUpdateRMEnable(CCmdUI* pCmdUI);
    afx_msg void OnUpdateRMEnableAndCheck(CCmdUI* pCmdUI);
    afx_msg void OnFreeHand();
    afx_msg void OnAutoMask();
    afx_msg void OnDrawLine();
    afx_msg void OnDrawBezier();
    afx_msg void OnIsBezier();
    afx_msg void OnIsPoint();
    afx_msg void OnMovePoints();
    afx_msg void OnAddPoints();
    afx_msg void OnDelPoint();
    afx_msg void OnPointMode();

    afx_msg void OnMaskClick();
    afx_msg void OnMaskDblClk();
    afx_msg void OnRetouchClick();
    afx_msg void OnRetouchDblClk();
    afx_msg void OnFilterClick();
    afx_msg void OnFilterDblClk();
    afx_msg void OnFillClick();
    afx_msg void OnFillDblClk();
    afx_msg void OnDrawClick();
    afx_msg void OnDrawDblClk();

    afx_msg void OnSelViewTextProbe(int ID);
    afx_msg void OnSelector();
    afx_msg void OnZoom();
    afx_msg void OnText();
    afx_msg void OnProbe();
    afx_msg void OnLastTool();
	afx_msg void OnMaskTool(int ID);

    afx_msg void OnMaskFree();
    afx_msg void OnMaskSquare();
    afx_msg void OnMaskCircle();
    afx_msg void OnMaskPainton();
    afx_msg void OnMaskMagic();
    afx_msg void OnMaskShield();
    afx_msg void OnMaskTransformer();
    afx_msg void OnMaskPointer();
    afx_msg void OnMaskCrop();

    afx_msg void OnRetouchTool(int ID);
    afx_msg void OnRetouchPaint();
    afx_msg void OnRetouchSpray();
    afx_msg void OnRetouchClone();
    afx_msg void OnRetouchTexture();
    afx_msg void OnRetouchSmear();
    afx_msg void OnRetouchEraser();
	
	afx_msg void OnFilterTool(int ID);
    afx_msg void OnFilterSharp();
    afx_msg void OnFilterSmooth();
    afx_msg void OnFilterLighten();
    afx_msg void OnFilterDarken();

    afx_msg void OnFillTool(int ID);
    afx_msg void OnFillVignette();
    afx_msg void OnFillTexture();
    afx_msg void OnFillTintFill();
    afx_msg void OnFillFlood();

    afx_msg void OnDrawTool(int ID);
    afx_msg void OnDrawPencil();
    afx_msg void OnDrawSquare();
    afx_msg void OnDrawFree();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	COCRibbon*		m_pRibbon;
	COCStatus*		m_pStatus;
	CPPToolBar*		m_pMain;
    HWND        	m_hDlg;
    HWND        	m_hControl;
    UINT        	m_codeNotify;
};

#endif
/////////////////////////////////////////////////////////////////////////////
