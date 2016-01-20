/* (c) Copyright 1991 MICROGRAFX, Inc., All Rights Reserved.  This material is
   confidential and a trade secret.  Permission to use this work for any purpose
   must be obtained in writing from:
              MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081

********************************************************************************
********************************************************************************

                                ppmodal.cpp

********************************************************************************
********************************************************************************

*/

#include "pp.h"
#include "svrdoc.h"
#include "svrview.h"
#include "ppmodal.h"
#include "ctxhelp.h"


IMPLEMENT_DYNAMIC(CPPModalDlg, CDialog)
IMPLEMENT_DYNAMIC(CPPViewModalDlg, CPPModalDlg)

/***************************** Classes *************************************/

class CImageReg : public CObject
{
public:
	CImageReg (HWND hDlg,LPIMAGE lpImage,IMAGEREGPROC lpCallBackProc, long Id) 
		{
			ASSERT(hDlg!=NULL);
			ASSERT(lpImage!=NULL);
			ASSERT(lpCallBackProc!=NULL);
			m_hDlg = hDlg;
			m_lpImage = lpImage;
			m_lpCallBackProc = lpCallBackProc;
			m_Id = Id;
		};

	long GetId() {return m_Id;};

	void NotifyImageChange(LPIMAGE lpImage,UINT Code)
		{
			ASSERT(m_lpCallBackProc != NULL);
			if (lpImage == m_lpImage)
			{
				if (Code == IMAGE_DESTROYED)
					(*m_lpCallBackProc)(m_hDlg,m_lpImage,IMAGE_DESTROYED);
			}
		};

private:
	HWND m_hDlg;
	LPIMAGE m_lpImage;
	IMAGEREGPROC m_lpCallBackProc;
	long m_Id;
};

/************************* Data ******************************************/

CImageForum ImageForum;


/************************* Local Procedure *******************************/

LRESULT ImageForumCallBack(HWND hDlg, LPIMAGE lpImage, UINT Code)
{
	ASSERT(Code == IMAGE_DESTROYED);
	if (Code == IMAGE_DESTROYED)
	{
		::EndDialog(hDlg,IDABORT);
		::MessageBox (NULL,"Image Destroyed aborting dialog session.",
            "Micrografx Picture Publisher",MB_OK);
	}

	return TRUE;
}

/***************************** Methods ************************************/

/////////////////////////////////////////////////////////////////////////
//  CPPModalDlg Methods

BEGIN_MESSAGE_MAP(CPPModalDlg,CDialog)
    ON_WM_CLOSE()	
    ON_WM_CREATE()
    ON_WM_DESTROY()
    ON_WM_DRAWITEM()
    ON_WM_MEASUREITEM()
    ON_WM_MENUSELECT()
    ON_WM_SETCURSOR()
	ON_WM_HSCROLL()
	ON_WM_VSCROLL()
//    ON_NOTIFY(UDN_DELTAPOS, OnDeltapos)  let the actual user override this
    ON_NOTIFY_EX_RANGE(UDN_DELTAPOS, 0, 0xFFFF, OnDeltaposRange) 
END_MESSAGE_MAP()

CPPModalDlg::CPPModalDlg (UINT nIDTemplate, CWnd* pParentWnd) :	CDialog(nIDTemplate,pParentWnd)
{
	m_nIDTemplate = nIDTemplate;
}


afx_msg void CPPModalDlg::OnClose()
{
	CDialog::OnClose();
	//EndDialog(FALSE);
}

afx_msg int CPPModalDlg::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	SetWindowLong(GetSafeHwnd(),DWL_USER, 0L);
	return(TRUE);
}

afx_msg void CPPModalDlg::OnDestroy()
{
	LPTR lptr;
	
	lptr = (LPTR)GetWindowLong(GetSafeHwnd(),DWL_USER);
	if (lptr)
		FreeUp(lptr);
}

afx_msg void CPPModalDlg::OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	DrawItem( GetSafeHwnd(), lpDrawItemStruct, 0 );
}

void CPPModalDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	// TODO here.  Ie,) This is a great place to write out Profile settings,
	// validate data,	etc.
	//....
}

BOOL CPPModalDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO here.  Ie,) This is a great place to read in Profile settings etc.

	return TRUE;
}

afx_msg void CPPModalDlg::OnMeasureItem(int nIDCtl, LPMEASUREITEMSTRUCT lpMeasureItemStruct)
{
	MeasureItem( GetSafeHwnd(), lpMeasureItemStruct, NO );
}

afx_msg void CPPModalDlg::OnMenuSelect(UINT nItemID, UINT nFlags, HMENU hSysMenu)
{
	HelpContext.OnHints( GetSafeHwnd(), 0 );
	HintLine(nItemID);
}

afx_msg BOOL CPPModalDlg::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
	HWND hWndCursor = pWnd->GetSafeHwnd();
	CPoint pt;
	HWND hChild;

	// check for disabled control
	GetCursorPos( &pt );
	if( hChild = MyChildWindowFromPoint( hWndCursor, pt ))
	{
	    hWndCursor = hChild;
	}
		
	return( SetupCursor( hWndCursor, nHitTest, m_nIDTemplate ) ? TRUE :
		CDialog::OnSetCursor(pWnd,nHitTest,message));
}



///////////////////////////////////////////////////////////////////////
/// CImageForum Methods



CImageForum::CImageForum () 
{
	m_NextId=-1;
}

CImageForum::~CImageForum ()
{

	m_Attendees.RemoveAll();
	
}

long CImageForum::Register(HWND hDlg,LPIMAGE lpImage,IMAGEREGPROC lpCallBackProc) 
{
	long retId=-1;

	if(m_Attendees.AddTail(new CImageReg(hDlg,lpImage,lpCallBackProc,
		m_NextId+1)) != 0) 
		retId = ++m_NextId;

	return retId;
}

BOOL CImageForum::DeRegister (long RegId)	
{
	BOOL bResult = FALSE;
	CImageReg* pImageReg;
	int nAttendeeCount = m_Attendees.GetCount();
	POSITION pos = m_Attendees.GetHeadPosition();
	for(int i=0; i< nAttendeeCount && bResult==FALSE; i++)
	{
		pImageReg = (CImageReg*) m_Attendees.GetAt(pos);
		if(pImageReg->GetId() == RegId)
		{
		 	m_Attendees.RemoveAt(pos);
			delete pImageReg;
			bResult = TRUE;
		}
		m_Attendees.GetNext(pos);
	}
return bResult;
}
	
	
void CImageForum::NotifyImageChange(LPIMAGE lpImage,UINT Code)
{
	CImageReg* pImageReg;
	POSITION pos;
	for( pos = m_Attendees.GetHeadPosition(); pos != NULL; )
	{
		pImageReg = (CImageReg*) m_Attendees.GetNext(pos);
		if (pImageReg)
			pImageReg->NotifyImageChange(lpImage,Code);
	}
}


void CPPModalDlg::OnCancel() 
{
	CDialog::OnCancel();
}


/////////////////////////////////////////////////////////////////////////
// CPPViewModalDlg Methods


CPPViewModalDlg::CPPViewModalDlg (CServerView *pView, UINT nIDTemplate, 
		CWnd* pParentWnd) : CPPModalDlg (nIDTemplate,pParentWnd)
{
	m_pView = pView;
	m_lpfnImageForumProc = NULL;
}

HWND CPPViewModalDlg::GetViewHwnd()
{
	return m_pView ? m_pView->GetSafeHwnd() : NULL;
}

CPPViewModalDlg::~CPPViewModalDlg ()
{
    if (m_lpfnImageForumProc)
    	ImageForum.DeRegister(m_ImageForumId);
}

LPIMAGE CPPViewModalDlg::GetImage () 
{
   	if (m_pView && m_pView->GetDocument())
   		return m_pView->GetDocument()->m_lpImage;
	return NULL;
}

BOOL CPPViewModalDlg::OnInitDialog()
{
    if (GetImage())
    {
     	m_lpfnImageForumProc = (IMAGEREGPROC)MakeProcInstance((FARPROC)ImageForumCallBack,
								::AfxGetInstanceHandle());
    	if(m_lpfnImageForumProc == NULL)
    		return FALSE;
    	ASSERT(m_lpfnImageForumProc);
    	HWND hWnd = GetSafeHwnd(); ASSERT(hWnd != NULL);
    	m_ImageForumId = ImageForum.Register(hWnd, GetImage(), m_lpfnImageForumProc);
    }

	return CPPModalDlg::OnInitDialog(); 
}


void CPPViewModalDlg::OnOK()
{
CDialog::OnOK();
}

void CPPViewModalDlg::OnCancel()
{
CDialog::OnCancel();
}

void CPPModalDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	HWND hControl = pScrollBar->GetSafeHwnd();
	STRING szClass;
	GetClassName(hControl, szClass, sizeof(szClass)-1);
	if(StringsEqual(szClass, TRACKBAR_CLASS))
		SliderOnScroll(hControl, nSBCode, nPos);
}

void CPPModalDlg::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	HWND hControl = pScrollBar->GetSafeHwnd();
	STRING szClass;
	GetClassName(hControl, szClass, sizeof(szClass)-1);
	if(StringsEqual(szClass, TRACKBAR_CLASS))
		SliderOnScroll(hControl, nSBCode, nPos);
}		

void CPPModalDlg::OnDeltapos(NMHDR* pNMHDR, LRESULT* pResult)
{
	if (pNMHDR->code == UDN_DELTAPOS)
	{
		CSpinFixedCtrl *pSpin = (CSpinFixedCtrl *)CWnd::FromHandlePermanent(pNMHDR->hwndFrom);
		if (pSpin && pSpin->IsKindOf(RUNTIME_CLASS(CSpinFixedCtrl)))
			pSpin->OnNotifyDeltaPos((LPNM_UPDOWN)pNMHDR);
	}
}

BOOL CPPModalDlg::OnDeltaposRange(UINT nID, NMHDR* pNMHDR, LRESULT* pResult)
{
    OnDeltapos( pNMHDR, pResult);
    return TRUE;
}
