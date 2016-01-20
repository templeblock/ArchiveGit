/* (c) Copyright 1991 MICROGRAFX, Inc., All Rights Reserved.  This material is
   confidential and a trade secret.  Permission to use this work for any purpose
   must be obtained in writing from:
              MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081

*****************************************************************************

                                  ppmodal.h

*****************************************************************************
*/

#ifndef _PPMODAL_H
#define _PPMODAL_H

/* ***************************** Public Data ***************************** */

/* ****************************** Constants ****************************** */

// Image Forum Notification codes
#define IMAGE_DESTROYED 1





/* ************************** Class Declarations ************************* */

typedef LRESULT (CALLBACK* IMAGEREGPROC)(HWND,LPIMAGE,UINT);

class CServerDoc;
class CServerView;

class CImageForum
{
public:

	CImageForum ();
	~CImageForum ();

	long Register(HWND hDlg, LPIMAGE lpImage,IMAGEREGPROC lpCallBackProc); 
	BOOL DeRegister (long RegId);	
	
	void NotifyImageChange(LPIMAGE lpImage,UINT Code);

protected:

private:

	CObList m_Attendees;	
	long m_NextId;
};

class CPPModalDlg : public CDialog
{
	DECLARE_DYNAMIC(CPPModalDlg);	
	// do not use;  we need an id on dialogs for hinting purposes
	CPPModalDlg(LPCTSTR lpszTemplateName, CWnd* pParentWnd = NULL) {ASSERT(FALSE);};
	
public:
	CPPModalDlg (UINT nIDTemplate, CWnd* pParentWnd = NULL );
	
	virtual BOOL PreDoModal (LPVOID lpData = NULL) {return TRUE;};
	virtual LPVOID PostDoModal () {return NULL;};
	virtual void DoDataExchange(CDataExchange* pDX);
	virtual void OnCancel();

	// CDialog Override for 
	virtual BOOL OnInitDialog();
		
	// Window Messages						 CWnd
	afx_msg void OnClose();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct);
	afx_msg void OnMeasureItem(int nIDCtl, LPMEASUREITEMSTRUCT lpMeasureItemStruct);
	afx_msg void OnMenuSelect(UINT nItemID, UINT nFlags, HMENU hSysMenu);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg void OnHScroll( UINT nSBCode, UINT nPos, CScrollBar* pScrollBar );
	afx_msg void OnVScroll( UINT nSBCode, UINT nPos, CScrollBar* pScrollBar );
	afx_msg void OnDeltapos( NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg BOOL OnDeltaposRange(UINT nID, NMHDR* pNMHDR, LRESULT* pResult);

	DECLARE_MESSAGE_MAP()


protected:

	UINT m_nIDTemplate;

protected:
	
private:

public:

};


class CPPViewModalDlg : public CPPModalDlg
{
		
public:

	CPPViewModalDlg (CServerView *pView, UINT nIDTemplate, CWnd* pParentWnd = NULL );
	~CPPViewModalDlg();
	LPIMAGE GetImage ();
	
	
	
	protected:
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	virtual void OnCancel();
private:
	LPIMAGE m_lpImage;
	long m_ImageForumId;
	IMAGEREGPROC m_lpfnImageForumProc;
protected:	
	CServerView *m_pView;
public:
	HWND GetViewHwnd ();

	DECLARE_DYNAMIC(CPPViewModalDlg)
	
};


extern CImageForum ImageForum;

/************************************ The End ************************************/
#endif // _PPMODAL_H
