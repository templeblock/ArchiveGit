/* (c) Copyright 1991 MICROGRAFX, Inc., All Rights Reserved.  This material is
   confidential and a trade secret.  Permission to use this work for any purpose
   must be obtained in writing from:
              MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081

*****************************************************************************

                                  svrview.h

*****************************************************************************
*/

#ifndef _SVRVIEW_H
#define _SVRVIEW_H

/* ******************************* Public Data ***************************** */

/* ********************************* Constants ****************************** */

/* *************************** Class Declarations ************************** */

class CPPDropTarget;
class CPPMDIChildWnd;
class CServerView;
class CUpdateArea; 
class CImage;
class CServerDoc;

/* ******************************* Typedefs ******************************* */

/* ******************************* Classes ******************************** */
class CPPDropTarget : public COleDropTarget
{

	// Overridables
	virtual DROPEFFECT OnDragEnter(CWnd* pWnd, COleDataObject* pDataObject,
		DWORD dwKeyState, CPoint point);
	virtual DROPEFFECT OnDragOver(CWnd* pWnd, COleDataObject* pDataObject,
		DWORD dwKeyState, CPoint point);
	virtual BOOL OnDrop(CWnd* pWnd, COleDataObject* pDataObject,
		DROPEFFECT dropEffect, CPoint point);
	virtual void OnDragLeave(CWnd* pWnd);
	virtual BOOL OnDragScroll(CWnd* pWnd, DWORD dwKeyState, CPoint point);

};

class COneShotTool
{
protected:
	COneShotTool(CServerView *pView, int idCursor);
public:
	~COneShotTool();

	virtual void OnLButtonDown( UINT keyFlags,CPoint point) = 0;
	virtual void OnLButtonUp( UINT keyFlags,CPoint point) = 0;
	virtual void OnMouseMove( UINT keyFlags,CPoint point) = 0;
	virtual BOOL IsToolActive() { return m_fToolActive; }
protected:
	CServerView *m_pView;
	HCURSOR		m_hOldCursor;
	BOOL		m_fToolActive;
};

class CPPMDIChildWnd : public CMDIChildWnd
{
public:
    DECLARE_DYNCREATE(CPPMDIChildWnd)

    CPPMDIChildWnd (); 
    virtual ~CPPMDIChildWnd () {};    

    // overrides    
    virtual BOOL LoadFrame(UINT nIDResource, DWORD dwDefaultStyle,
                    CWnd* pParentWnd, CCreateContext* pContext = NULL);
                    // 'pParentWnd' parameter is required for MDI Child
    
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
    virtual void OnUpdateFrameTitle(BOOL bAddToTitle);

	// MDI message handler member functions
	afx_msg void OnMDIActivate(BOOL bActivate,CWnd* pActivateWnd, 
		CWnd* pDeactivateWnd);

    afx_msg void OnClose(); 
    afx_msg void OnInitMenu(CMenu *pMenu);
    afx_msg int  OnMouseActivate( CWnd *pwndTopLevel, UINT nHitTest, UINT msg);    
	afx_msg void OnPaint();
    afx_msg void OnPaletteChanged(CWnd *pFocusWnd);    
    afx_msg void OnSysCommand(UINT cmd, LPARAM lParam);
    afx_msg void OnWindowPosChanging( LPWINDOWPOS lpWindowPos);
	afx_msg void OnNcLButtonDown(UINT nHitTest, CPoint point);
    DECLARE_MESSAGE_MAP()                 
    
private:                  
    CImage * get_view_imageptr();                
    BOOL handle_syscmd(UINT cmd,LPARAM lParam);
};

                                                     
class CServerView : public CView
{
public:

    struct display_struct * m_lpDisplay;

protected: // create from serialization only
    CServerView();
    DECLARE_DYNCREATE(CServerView)
    virtual ~CServerView();
    
public:
    // special pre-creation and window rect adjustment hooks
    virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

	// Implementation                              
	void DoActivate(BOOL bActivate,CWnd* pActivateWnd, CWnd* pDeactiveWnd );  
    CServerDoc* GetDocument();
	BOOL GetObjectInfo(COleDataObject* pDataObject,CSize* pSize, CSize* pOffset);
	void SetupImageDisplayPalette();
	void UpdateWindowNow();
	BOOL AutoScroll(int x, int y);
	void SetupOneShotTool(COneShotTool *pTool = NULL);
    
	// CView Overridables    
    virtual void OnActivateView( BOOL bActivate, CView* pActivateView, CView* pDeactiveView );
    virtual void OnDraw(CDC* pDC); 
    virtual void OnInitialUpdate();    
    virtual void OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint);

    // OLE 2.0 drag/drop support
    virtual DROPEFFECT OnDragEnter(COleDataObject* pDataObject,
        DWORD dwKeyState, CPoint point);
    virtual DROPEFFECT OnDragOver(COleDataObject* pDataObject,
        DWORD dwKeyState, CPoint point);
    virtual void OnDragLeave();
    virtual BOOL OnDrop(COleDataObject* pDataObject,
        DROPEFFECT dropEffect, CPoint point);

#ifdef _DEBUG
    virtual void AssertValid() const;
    virtual void Dump(CDumpContext& dc) const;
#endif

	// for enabling drag/drop to our window
	CPPDropTarget m_dropTarget;

	//-------------------------------------
	// Command UI update (in viewcmds.cpp & viewcmd2.cpp)
	//-------------------------------------
	afx_msg void OnUpdateStitch(CCmdUI* pCmdUI);
	afx_msg void OnUpdatePaste(CCmdUI* pCmdUI);
	afx_msg void OnUpdatePasteSpecial(CCmdUI* pCmdUI);
	afx_msg void OnUpdateHideMarquee(CCmdUI* pCmdUI);
	afx_msg void OnUpdateHideObjMarquee(CCmdUI* pCmdUI);
	afx_msg void OnUpdateRulerShow(CCmdUI* pCmdUI);
 	afx_msg void OnUpdateEscape(CCmdUI* pCmdUI);
	afx_msg void OnUpdateZoomIn(CCmdUI* pCmdUI);
	afx_msg void OnUpdateZoomOut(CCmdUI* pCmdUI);
	afx_msg void OnUpdateViewFull(CCmdUI* pCmdUI);
	afx_msg void OnUpdateViewLast(CCmdUI* pCmdUI);
	afx_msg void OnUpdateView1To1(CCmdUI* pCmdUI);
	afx_msg void OnUpdateViewAll(CCmdUI* pCmdUI);
	afx_msg void OnUpdateLoadShape(CCmdUI* pCmdUI);
	afx_msg void OnUpdateSaveShape(CCmdUI* pCmdUI);

	afx_msg void OnUpdateRotate90(CCmdUI* pCmdUI);
	afx_msg void OnUpdateRotate180(CCmdUI* pCmdUI);
	afx_msg void OnUpdateRotate270(CCmdUI* pCmdUI);
	afx_msg void OnUpdateRotateAny(CCmdUI* pCmdUI);
	afx_msg void OnUpdateObjLock(CCmdUI* pCmdUI);
	afx_msg void OnUpdateObjUnLock(CCmdUI* pCmdUI);
	afx_msg void OnUpdateObjGroup(CCmdUI* pCmdUI);
	afx_msg void OnUpdateObjUnGroup(CCmdUI* pCmdUI);
	afx_msg void OnUpdatePrintPreviewEnabled(CCmdUI* pCmdUI);
	afx_msg void OnUpdateFeather(CCmdUI* pCmdUI);
	afx_msg void OnUpdateRemoveHoles(CCmdUI* pCmdUI);
	afx_msg void OnUpdateMaskSmooth(CCmdUI* pCmdUI);
	afx_msg void OnUpdateObjAlign(CCmdUI* pCmdUI);
	afx_msg void OnUpdateObjFeather(CCmdUI* pCmdUI);
	afx_msg void OnUpdatePosition(CCmdUI* pCmdUI);
	afx_msg void OnUpdateCombineObj(CCmdUI* pCmdUI);
	afx_msg void OnUpdateCombine(CCmdUI* pCmdUI);
	afx_msg void OnUpdateCombineSel(CCmdUI* pCmdUI);
	afx_msg void OnUpdateSelectAll(CCmdUI* pCmdUI);
	afx_msg void OnUpdateSelectInvert(CCmdUI* pCmdUI);
	afx_msg void OnUpdateDownLevel(CCmdUI* pCmdUI);
	afx_msg void OnUpdateUpLevel(CCmdUI* pCmdUI);
	afx_msg void OnUpdateMoveToBottom(CCmdUI* pCmdUI);
	afx_msg void OnUpdateMoveToTop(CCmdUI* pCmdUI);
	afx_msg void OnUpdateApplyMask(CCmdUI* pCmdUI);
	afx_msg void OnUpdateObjDelete(CCmdUI* pCmdUI);
	afx_msg void OnUpdateMaskFromObject(CCmdUI* pCmdUI);
	afx_msg void OnUpdateObjectFromMask(CCmdUI* pCmdUI);
	afx_msg void OnUpdateChroma(CCmdUI* pCmdUI);
	afx_msg void OnUpdateSizeit(CCmdUI* pCmdUI);
	afx_msg void OnUpdateExpand(CCmdUI* pCmdUI);
	afx_msg void OnUpdateMirrorX(CCmdUI* pCmdUI);
	afx_msg void OnUpdateMirrorY(CCmdUI* pCmdUI);
	afx_msg void OnUpdateMirrorXY(CCmdUI* pCmdUI);
	afx_msg void OnUpdateSpecialFx(CCmdUI* pCmdUI);
	afx_msg void OnUpdateInvert(CCmdUI* pCmdUI);
	afx_msg void OnUpdateSplitRgb(CCmdUI* pCmdUI);
	afx_msg void OnUpdateSplitHsl(CCmdUI* pCmdUI);
	afx_msg void OnUpdateSplitCmyk(CCmdUI* pCmdUI);
	afx_msg void OnUpdateChannelCombine(CCmdUI* pCmdUI);
	afx_msg void OnUpdateConvLa(CCmdUI* pCmdUI);
	afx_msg void OnUpdateConvSp(CCmdUI* pCmdUI);
	afx_msg void OnUpdateConvCt(CCmdUI* pCmdUI);
	afx_msg void OnUpdateConvPalette(CCmdUI* pCmdUI);
	afx_msg void OnUpdateConvRgbColor(CCmdUI* pCmdUI);
	afx_msg void OnUpdateConvCmykColor(CCmdUI* pCmdUI);
	afx_msg void OnUpdateConvColorManaged(CCmdUI* pCmdUI);
	afx_msg void OnUpdateCmdListUndo(CCmdUI* pCmdUI);
	afx_msg void OnUpdateCmdListRedo(CCmdUI* pCmdUI);
	afx_msg void OnUpdateCmdListEdit(CCmdUI* pCmdUI);
	afx_msg void OnUpdateUndo(CCmdUI* pCmdUI);
	afx_msg void OnUpdateApply(CCmdUI* pCmdUI);
	afx_msg void OnUpdateDelete(CCmdUI* pCmdUI);
	afx_msg void OnUpdateMap(CCmdUI* pCmdUI);
	afx_msg void OnUpdateContBrit(CCmdUI* pCmdUI);
	afx_msg void OnUpdateColorBalance(CCmdUI* pCmdUI);
	afx_msg void OnUpdatePosterize(CCmdUI* pCmdUI);
	afx_msg void OnUpdateThreshold(CCmdUI* pCmdUI);
	afx_msg void OnUpdateHue(CCmdUI* pCmdUI);
	afx_msg void OnUpdateHueShift(CCmdUI* pCmdUI);
	afx_msg void OnUpdateToneBalance(CCmdUI* pCmdUI);
	afx_msg void OnUpdateHistogram(CCmdUI* pCmdUI);
	afx_msg void OnUpdatePaletteEdit(CCmdUI* pCmdUI);
	afx_msg void OnUpdateMaskLoad(CCmdUI* pCmdUI);
	afx_msg void OnUpdateMaskSave(CCmdUI* pCmdUI);
	afx_msg void OnUpdateMaskUndo(CCmdUI* pCmdUI);
	afx_msg void OnUpdateMaskRemove(CCmdUI* pCmdUI);
	afx_msg void OnUpdateMaskInvert(CCmdUI* pCmdUI);
	afx_msg void OnUpdateMaskStroke(CCmdUI* pCmdUI);
	afx_msg void OnUpdateCrop(CCmdUI* pCmdUI);
	afx_msg void OnUpdateMoveObjects(CCmdUI* pCmdUI);
	afx_msg void OnUpdateCalibrateApply(CCmdUI* pCmdUI);
	afx_msg void OnUpdateCut(CCmdUI* pCmdUI);
	afx_msg void OnUpdateCopy(CCmdUI* pCmdUI);
	afx_msg void OnUpdateCopyToFile(CCmdUI* pCmdUI);
	afx_msg void OnUpdateExportFile(CCmdUI* pCmdUI);

	afx_msg void OnUpdateQuickZoom(CCmdUI* pCmdUI);
	afx_msg void OnUpdateProbePoint(CCmdUI* pCmdUI);
	afx_msg void OnUpdateProbeRect(CCmdUI* pCmdUI);

	//-------------------------------------
	// Command targets (in viewcmds.cpp & viewcmd2.cpp)
	//-------------------------------------
	afx_msg void OnStitch();
	afx_msg void OnPaste();
	afx_msg void OnPasteSpecial();
	afx_msg void OnHideMarquee();
	afx_msg void OnHideObjMarquee();
	afx_msg void OnRulerShow();
	afx_msg void OnEscape();
	afx_msg void OnViewLast();
	afx_msg void OnViewFull();
    afx_msg void OnView1To1();
	afx_msg void OnViewAll();
	afx_msg void OnZoomIn();
	afx_msg void OnZoomOut();
	afx_msg void OnLoadShape();
	afx_msg void OnSaveShape();
    
    afx_msg void OnRotate90();
	afx_msg void OnRotate180();
	afx_msg void OnRotate270();
    afx_msg void OnRotateAny();
	afx_msg void OnObjLock();
	afx_msg void OnObjUnLock();
	afx_msg void OnObjUnGroup();
	afx_msg void OnObjGroup();
	afx_msg void OnPrintPreview();
	afx_msg void OnFeather();
	afx_msg void OnRemoveHoles();
	afx_msg void OnMaskSmooth();
	afx_msg void OnObjFeather();
	afx_msg void OnObjAlign();
	afx_msg void OnObjPosition();
	afx_msg void OnCombineObj();
	afx_msg void OnCombine();
	afx_msg void OnCombineSel();
	afx_msg void OnSelectAll();
	afx_msg void OnSelectInvert();
	afx_msg void OnDownLevel();
	afx_msg void OnUpLevel();
	afx_msg void OnMoveToBottom();
	afx_msg void OnMoveToTop();
	afx_msg void OnApplyMask();
	afx_msg void OnDelete();
	afx_msg void OnChroma();
	afx_msg void OnMaskFromObject();
	afx_msg void OnObjectFromMask();
	afx_msg void OnSizeit();
	afx_msg void OnExpand();
	afx_msg void OnMirrorX();
	afx_msg void OnMirrorY();
	afx_msg void OnMirrorXY();
	afx_msg void OnSpecialFx();
	afx_msg void OnInvert();
	afx_msg void OnSplitRgb();
	afx_msg void OnSplitHsl();
	afx_msg void OnSplitCmyk();
	afx_msg void OnChannelCombine();
	afx_msg void OnConvLa();
	afx_msg void OnConvSp();
	afx_msg void OnConvCt();
	afx_msg void OnConvPalette();
	afx_msg void OnConvRgbColor();
	afx_msg void OnConvCmykColor();
	afx_msg void OnConvColorManaged();
	afx_msg void OnCmdListUndo();
	afx_msg void OnCmdListRedo();
	afx_msg void OnCmdListEdit();
	afx_msg void OnUndo();
	afx_msg void OnApply();
	afx_msg void OnObjDelete();
	afx_msg void OnMap();
	afx_msg void OnContBritStick();
	afx_msg void OnColorBalStick();
	afx_msg void OnContBritVisual();
	afx_msg void OnColorBalVisual();
	afx_msg void OnPosterize();
	afx_msg void OnThreshold();
	afx_msg void OnHue();
	afx_msg void OnHueShift();
	afx_msg void OnToneBalance();
	afx_msg void OnHistogram();
	afx_msg void OnPaletteEdit();
	afx_msg void OnMaskLoad();
	afx_msg void OnMaskSave();
	afx_msg void OnMaskUndo();
	afx_msg void OnMaskRemove();
	afx_msg void OnMaskInvert();
	afx_msg void OnMaskStroke();
	afx_msg void OnCrop();
	afx_msg void OnMoveUp();
	afx_msg void OnMoveDown();
	afx_msg void OnMoveLeft();
	afx_msg void OnMoveRight();
	afx_msg void OnCalibrateApply();
	afx_msg void OnCut();
	afx_msg void OnCopy();
	afx_msg void OnCopyToFile();
	afx_msg void OnExportFile();
 
    afx_msg void OnChar( UINT ch, UINT cRepeat, UINT nFlags);
    afx_msg int  OnCreate( LPCREATESTRUCT lpCreateStruct );
    afx_msg void OnDestroy();          
    afx_msg BOOL OnEraseBkgnd(CDC *cdc);
    afx_msg void OnHScroll(UINT code, UINT pos,CScrollBar *pScrollBar);
    afx_msg void OnKeyDown( UINT nChar, UINT cRepeat, UINT flags);
    afx_msg void OnKeyUp( UINT nChar, UINT cRepeat, UINT flags);
    afx_msg void OnKillFocus(CWnd *pNewWnd);
    afx_msg void OnLButtonDblClk( UINT keyFlags,CPoint point);
    afx_msg void OnLButtonDown( UINT keyFlags,CPoint point);
    afx_msg void OnLButtonUp( UINT keyFlags,CPoint point);  
    afx_msg void OnMouseMove( UINT keyFlags,CPoint point);
    afx_msg void OnMove( int x, int y);        
    afx_msg void OnNcLButtonDown(UINT nHitTest,CPoint point);
    afx_msg void OnPaletteChanged(CWnd *pFocusWnd);    
	afx_msg void OnPaint();
    afx_msg void OnRButtonDown( UINT keyFlags,CPoint point);
    afx_msg void OnRButtonUp( UINT keyFlags,CPoint point);
    afx_msg BOOL OnSetCursor( CWnd *pWnd, UINT nHitTest, UINT msg);
    afx_msg void OnSetFocus(CWnd *pOldWnd);
    afx_msg void OnSize( UINT state, int cx, int cy);
    afx_msg void OnTimer( UINT nIDEvent );
    afx_msg void OnVScroll(UINT code, UINT pos,CScrollBar *pScrollBar);
	afx_msg LONG OnDropActivate(UINT wParam, LONG lParam);  // user defined msg

    afx_msg void OnCustomView();
    afx_msg void OnQuickZoom();
    afx_msg void OnProbePoint();
    afx_msg void OnProbeRect();

	DECLARE_MESSAGE_MAP()   

private:       

	LPOBJECT get_clipboard_object(FRMTYPEINFO TypeInfo);
	void init_paste(LPOBJECT lpObject,CPoint *pt = NULL);
	void draw_it (CDC *pDC,LPRECT lpRepairRect,LPRECT lpInvalidRect = NULL);
    void setup_rates(CImage * lpImage);    
	void on_zoom(int idCmd);
	void on_update_zoom(int idCmd, CCmdUI* pCmdUI);
	void do_split(UINT idCmd);
	void move_obj(UINT idCmd);

	BOOL	m_bAllowInterrupt;

	// drag/drop vars
	CPoint m_dragPoint;
	CSize m_dragSize;
	CSize m_dragOffset;
	DROPEFFECT m_prevDropEffect;
	BOOL m_fCapture;
	int m_iSizeCallDepth;
	// flag to determine whether we have activated or deactivated
	BOOL m_fActivated;
	BOOL m_fPositioned;
	COneShotTool *m_pOneShotTool;
};

class CUpdateArea : public CObject
{
    DECLARE_DYNAMIC(CUpdateArea)
public:

	CUpdateArea (CRect rUpdateArea) { m_rUpdateArea = rUpdateArea; };

	LPCRECT GetUpdateArea () {return &m_rUpdateArea;};

private:

	CRect m_rUpdateArea;
};

/* *************************** Inline Methods ****************************** */

#ifndef _DEBUG  // debug version in svrview.cpp
inline CServerDoc* CServerView::GetDocument()
{ 
	return (CServerDoc*)m_pDocument; 
}
#endif

/* *************************** Global Data ********************************* */

/* *************************** Global Function ***************************** */

/* ************************************************************************* */

#endif // _SVRVIEW_H
