// (c) Copyright 1991 MICROGRAFX, Inc., All Rights Reserved.
//    This material is confidential and a trade secret.
//    Permission to use this work for any purpose must be obtained
//    in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081  
//-------------------------------------------------------------------------
//                            MDIframe.h
//-------------------------------------------------------------------------

#ifndef _MDIFRAME_H_
#define _MDIFRAME_H_

class CPPDropTarget;
class CPPToolList;

class CMDIFrame : public CMDIFrameWnd
{
public:
    DECLARE_DYNAMIC(CMDIFrame)

    // COnstructors and Destructors
    CMDIFrame ();
    virtual ~CMDIFrame ();

    // Implementation
	int GetNumMDIChildWindows();
	ULONG GetClipboardID() { return m_ClipboardChangeID; };

    // Overides        
	afx_msg void OnInitMenuPopup(CMenu* pPopupMenu, UINT nIndex,
										BOOL bSysMenu);
    virtual WNDPROC* CMDIFrame::GetSuperWndProcAddr();
    virtual BOOL PreTranslateMessage( MSG* pMsg );
    virtual LONG WindowProc (UINT message, UINT wParam, LONG lParam);

    // OLE 2.0 drag/drop support
	CPPDropTarget *m_pDropTarget;
	BOOL GetObjectInfo(COleDataObject* pDataObject,CSize* pSize, CSize* pOffset);
    virtual DROPEFFECT OnDragEnter(COleDataObject* pDataObject,
        DWORD dwKeyState, CPoint point);
    virtual DROPEFFECT OnDragOver(COleDataObject* pDataObject,
        DWORD dwKeyState, CPoint point);
    virtual void OnDragLeave();
    virtual BOOL OnDrop(COleDataObject* pDataObject,
        DROPEFFECT dropEffect, CPoint point);
	virtual void RecalcLayout(BOOL bNotify = TRUE);

    // Message Maps
    DECLARE_MESSAGE_MAP()    
	afx_msg void OnToolbars();
	afx_msg void OnUpdateToolbars(CCmdUI* pCmdUI);
	afx_msg void OnOCCustomize();
	afx_msg void OnUpdateOCCustomize(CCmdUI* pCmdUI);
	afx_msg void OnActivateApp( BOOL  bActive, HTASK  hTask ); 
	afx_msg void OnChangeCbChain( HWND, HWND );
    afx_msg void OnClose();
    afx_msg int OnCreate( LPCREATESTRUCT lpCreateStruct );  
    afx_msg void OnDestroy();
	afx_msg void OnDrawClipboard( );
	afx_msg void OnEndSession(BOOL bEnding);
    afx_msg BOOL OnQueryEndSession();
    afx_msg void OnSysColorChange();
	
public:                     
   CPPToolList    *m_pListCustTools;
	BOOL m_fFrameClosing;			// set while closing childern documents due to close of mdiframe
private:                     
    BOOL do_close();
    void do_freeup();    

	// Clipboard Stuff to assist with tracking on Paste Operations
	HWND    m_hwndNextViewer;  // next viewer in the clipboard viewer list
	ULONG   m_ClipboardChangeID;

	// drag/drop vars
	CPoint m_dragPoint;
	CSize m_dragSize;
	CSize m_dragOffset;
	DROPEFFECT m_prevDropEffect;
};  


#endif // _MDIFRAME_H_
