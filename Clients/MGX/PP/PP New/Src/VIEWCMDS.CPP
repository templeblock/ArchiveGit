/* (c) Copyright 1991 MICROGRAFX, Inc., All Rights Reserved.  This material is
   confidential and a trade secret.  Permission to use this work for any purpose
   must be obtained in writing from:
              MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081

********************************************************************************
********************************************************************************

                                viewcmds.cpp

********************************************************************************
********************************************************************************

*/

#include "pp.h"
#include "ppafx.h"
#include "afxodlgs.h"
#include "id.h"
#include "svrdoc.h"
#include "svritem.h"
#include "svrview.h"
#include "ipframe.h"
#include "cmdundo.h"

/* ********************************* History ******************************** */
/* ******************************** Constants ******************************* */

#if 0
    #define IFDEBUG(s) {s;}
    #define IFCOPY(s) s
#else
    #define IFDEBUG(s)
    #define IFCOPY(s)
#endif

/******************************* EXPORTED Functions *****************************/
/* ********************************** Macros ******************************** */

#define STOP ((m_lpDisplay==NULL) || (!GetDocument()) || ((GetDocument()->GetImage())==NULL))

/* ********************************** Types ********************************* */
/* ******************************* Local Data ******************************* */
/* ******************************* Public Data ****************************** */

/* *************************** Private Functions **************************** */
/* *************************** Private Methods **************************** */

void CServerView::init_paste(
LPOBJECT lpObject,
CPoint *pt)
{
	RECT rImage, rObjects;

    CServerDoc* pDoc = GetDocument();
	ASSERT(pDoc);

   	LPIMAGE lpImage = pDoc->m_lpImage;
	ASSERT(lpImage);
	
	int xData = RectWidth(&lpObject->rObject);
	int yData = RectHeight(&lpObject->rObject);

	// get location rect to center to object on
	rImage = m_lpDisplay->FileRect;

	int x, y;
	// center the object around point
	if (pt)
	{
		x = pt->x;
		y = pt->y;
		Display2File(GetSafeHwnd(), (LPINT)&x, (LPINT)&y);
		x = x - (xData/2);
		y = y - (yData/2);
	}
	// center the object in the location rectangle
	else
	{
		x = ( rImage.left + rImage.right - xData ) / 2;
		y = ( rImage.top + rImage.bottom - yData ) / 2;
	}

	if (x < rImage.left)
		x = rImage.left;
	if (y < rImage.top)
		y = rImage.top;

	OffsetRect(&lpObject->rObject, x, y);

	RemoveObjectMarquee(lpImage);
	// make this object temporary, so it will go away if
	// the paste is cancelled
	lpObject->StorageType = ST_TEMPORARY;
	// save the selection state in case of cancel
	SaveSelectedObjects(lpImage);
	ImgAddNewObject(lpImage, lpObject);
	ImgGetSelObjectRect(lpImage, &rObjects, YES);
	UpdateImage(lpImage, &rObjects, YES);
}

/* ***************************** Public Methods ***************************** */

afx_msg void CServerView::OnUpdateStitch(CCmdUI* pCmdUI)
{
	BOOL state = IsCmdEnabled(0, this);
	if (state)
	{
		CImage *pImage = GetDocument()->GetImage();
		FRMTYPEINFO TypeInfo;
		pImage->GetTypeInfo(&TypeInfo);
		if (TypeInfo.DataType == FDT_PALETTECOLOR || pImage->InMaskEditMode())
			state = FALSE;
	}
    pCmdUI->Enable(	state );
}

afx_msg void CServerView::OnUpdateHideObjMarquee(CCmdUI* pCmdUI)
{
	BOOL state = IsCmdEnabled(0, this);
	if (state)
	{
		CImage *pImage = GetDocument()->GetImage();
		if (pImage->MultipleObjects())
		{
        	if( !pCmdUI->m_pOther )      
    	    	OnUpdateShowHideMenu( IDM_OBJ_HIDEMARQUEE, 
                                  	!pImage->UseObjectMarquee, pCmdUI );
		}
		else
			state = FALSE;
	}
	pCmdUI->Enable(state);
}

afx_msg void CServerView::OnUpdatePaste(CCmdUI* pCmdUI)
{
	BOOL state = IsCmdEnabled(0, this);
	if (state)
	{
		state = FALSE;
		if ( OpenClipboard() )
	   	{
		    if ( IsClipboardFormatAvailable(CF_METAFILEPICT) || // new jtm
                 IsClipboardFormatAvailable(CF_DIB) ||
	    	     IsClipboardFormatAvailable(CF_BITMAP) ||
	        	 IsClipboardFormatAvailable(Control.cfImage) ||
	        	 IsClipboardFormatAvailable(Control.cfImagePP4) )
		       	state = TRUE;
		    CloseClipboard();
	    }
	}
	pCmdUI->Enable( state );
}
 
afx_msg void CServerView::OnUpdatePasteSpecial(CCmdUI* pCmdUI)
{
	if (IsCmdEnabled(0, this))
	{
   		COleDataSource *pDataSource = COleDataSource::GetClipboardOwner();
		if (pDataSource)
        	pCmdUI->Enable(FALSE);
		else
	    	OnUpdatePaste(pCmdUI);
	}
	else
    	pCmdUI->Enable(FALSE);
}

afx_msg void CServerView::OnUpdateHideMarquee(CCmdUI* pCmdUI)
{
	BOOL state = IsCmdEnabled(0, this);
	CImage *pImage = GetDocument()->GetImage();

	if (state && !pImage->GetMask())
    	state = FALSE;
	if( !pCmdUI->m_pOther )      
	   	OnUpdateShowHideMenu( IDM_HIDEMARQUEE, 
                          	pImage && !IsMarqueeVisible( pImage ), pCmdUI );
    pCmdUI->Enable(	state );
}

afx_msg void CServerView::OnUpdateRulerShow(CCmdUI* pCmdUI)
{
	BOOL state = IsCmdEnabled(0, this);

    // Toggle show/hide menu item to HIDE rulers if currently displayed. 
    // Otherwise, SHOW them.
    if( !pCmdUI->m_pOther )      
    	OnUpdateShowHideMenu( IDM_RULERSHOW, 
			!( m_lpDisplay && m_lpDisplay->HasRulers ), pCmdUI );

	pCmdUI->Enable (state);
}

afx_msg void CServerView::OnUpdateEscape(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(IsCmdEnabled(0, this));
}

afx_msg void CServerView::OnUpdateZoomIn(CCmdUI* pCmdUI)
{
	on_update_zoom(IDC_ZOOMIN, pCmdUI);
}

afx_msg void CServerView::OnUpdateZoomOut(CCmdUI* pCmdUI)
{
	on_update_zoom(IDC_ZOOMOUT, pCmdUI);
}


afx_msg void CServerView::on_update_zoom(int idCmd, CCmdUI* pCmdUI)
{
    if (IsCmdEnabled(0, this) && m_lpDisplay->ViewPercentage)
	{
		int inc;

		if (idCmd == IDC_ZOOMIN)
			inc = 100;
		else
			inc = -100;
    	pCmdUI->Enable(	ZoomEnable(GetSafeHwnd(), inc, YES) );
	}
	else
		pCmdUI->Enable( FALSE );
}

afx_msg void CServerView::OnUpdateViewFull(CCmdUI* pCmdUI)
{
	pCmdUI->Enable( IsCmdEnabled(0, this) );
}

afx_msg void CServerView::OnUpdateViewLast(CCmdUI* pCmdUI)
{
	pCmdUI->Enable( IsCmdEnabled(0, this) );
}

afx_msg void CServerView::OnUpdateView1To1(CCmdUI* pCmdUI)
{
	pCmdUI->Enable( IsCmdEnabled(0, this) );
}

afx_msg void CServerView::OnUpdateViewAll(CCmdUI* pCmdUI)
{
	pCmdUI->Enable( IsCmdEnabled(0, this) );
}

afx_msg void CServerView::OnUpdateLoadShape(CCmdUI* pCmdUI)
{
	pCmdUI->Enable( IsCmdEnabled(0, this) );
}

afx_msg void CServerView::OnUpdateSaveShape(CCmdUI* pCmdUI)
{
	pCmdUI->Enable( IsCmdEnabled(0, this) );
}

afx_msg void CServerView::OnStitch()
{
    if (STOP) return; 
    CServerDoc* pDoc = GetDocument();
    LPIMAGE lpImage = pDoc->GetImage();

    DeactivateTool();
    if (ImgMultipleObjects(lpImage))
    {
        Message(IDS_OBJECTSNEEDCOMBINE);
        return;
    }
    ActivateTool( IDC_STITCH );
}

afx_msg void CServerView::OnHideObjMarquee()
{
    if (!Control.Retail )
        return;

    if (STOP) return; 
    CServerDoc* pDoc = GetDocument();
    LPIMAGE lpImage = pDoc->GetImage();

    RemoveObjectMarquee(lpImage);
    lpImage->UseObjectMarquee = !lpImage->UseObjectMarquee;
}

afx_msg void CServerView::OnPaste()
{
	FNAME	szFileName;
	LPOBJECT lpObject;
	DWORD	dwImageID;
	LPIMAGE lpImage = NULL;
	FRMTYPEINFO SrcTypeInfo;

    CServerDoc* pDoc = GetDocument();
	ASSERT(pDoc);

   	lpImage = pDoc->GetImage();
	if (!lpImage)
		return;
	
    DeactivateTool();

	lpObject = GetClipboardObject(lpImage, szFileName, &dwImageID, &SrcTypeInfo);
	if (!lpObject)
		return;

	init_paste(lpObject);

	TransformObjectsStart(lpImage, PasteEndProc, IDC_OBJECTTRANSFORM, YES,
         	IDS_UNDOPASTE, YES, szFileName, dwImageID, FALSE, &SrcTypeInfo);
}

afx_msg void CServerView::OnPasteSpecial()
{
    COlePasteSpecialDialog psd;
    psd.AddFormat(CF_METAFILEPICT,TYMED_MFPICT,AFX_IDS_METAFILE_FORMAT,
        FALSE,FALSE);
    psd.AddFormat(CF_DIB,TYMED_HGLOBAL,AFX_IDS_DIB_FORMAT,
        FALSE,FALSE);
    psd.AddFormat(CF_BITMAP,TYMED_GDI,AFX_IDS_BITMAP_FORMAT,
        FALSE,FALSE);
    psd.AddFormat(Control.cfImage,TYMED_HGLOBAL,IDS_MGXIMAGE5_FORMAT,
        FALSE,FALSE);
    psd.AddFormat(Control.cfImagePP4,TYMED_HGLOBAL,IDS_MGXIMAGE_FORMAT,
        FALSE,FALSE);
    if (psd.DoModal() == IDOK)
    {
        int index = psd.GetPasteIndex();        
        if (index >= 0 && index <= psd.m_ps.cPasteEntries)
        {
           	FNAME	szFileName;
           	LPOBJECT lpObject;
           	DWORD	dwImageID;
           	LPIMAGE lpImage = NULL;
           	FRMTYPEINFO SrcTypeInfo;
            
            CServerDoc* pDoc = GetDocument();
        	ASSERT(pDoc);
        
           	lpImage = pDoc->GetImage();
        	if (!lpImage)
        		return;
        	
            DeactivateTool();
        
        	lpObject = GetClipboardObject(lpImage, szFileName, &dwImageID, 
                &SrcTypeInfo,psd.m_ps.arrPasteEntries[index].fmtetc.cfFormat);

        	if (!lpObject)
        		return;
        
        	init_paste(lpObject);
        
        	TransformObjectsStart(lpImage, PasteEndProc, IDC_OBJECTTRANSFORM, YES,
                 	IDS_UNDOPASTE, YES, szFileName, dwImageID, FALSE, &SrcTypeInfo);
        }        
    }
}

afx_msg void CServerView::OnHideMarquee()
{
    if (STOP) return; 
    CServerDoc* pDoc = GetDocument();
    LPIMAGE lpImage = pDoc->GetImage();
	if (lpImage)
	    ShowMarquee(lpImage, !IsMarqueeVisible(lpImage));
}

afx_msg void CServerView::OnRulerShow()
{
    if (STOP) return; 
    CServerDoc* pDoc = GetDocument();
    LPIMAGE lpImage = pDoc->GetImage();

    if (lpImage)
	{
		HWND hActiveWnd = GetSafeHwnd();
	    if (m_lpDisplay->HasRulers)
    	    DestroyRulers(hActiveWnd);
	    else    
			CreateRulers(hActiveWnd);
		
	    ComputeDispRect(hActiveWnd);
	    ComputeFileRect(hActiveWnd, 0, 0, m_lpDisplay->FileRate );
	    SetupRulers(hActiveWnd);
	    InvalidateRect(NULL, TRUE);
	}
}

afx_msg void CServerView::OnEscape()
{
	// always try to kill the one shot tool
	SetupOneShotTool();
    if (STOP) return; 
    CServerDoc* pDoc = GetDocument();
    LPIMAGE lpImage = pDoc->GetImage();

    if (lpImage)
	{
		HWND hActiveWnd = GetSafeHwnd();
	    if (hActiveWnd == hZoomWindow)
	        return;
	    if ( Tool.bActive && Tool.lpToolProc )
    	    DestroyProc( hActiveWnd, 1L );
	}
}

afx_msg void CServerView::OnViewLast()
{
    if (STOP) return; 
    RevertLastView(GetActiveDoc());
}

afx_msg void CServerView::OnViewFull()
{
    if (STOP) return; 
    AstralDlg( NO, PictPubApp.GetResourceHandle(), GetSafeHwnd(), IDD_VIEWFULL, DlgFullScreenViewProc );
}

afx_msg void CServerView::OnView1To1()
{
    if (STOP) return; 
    View1TO1();
}

afx_msg void CServerView::OnViewAll()
{
    if (STOP) return; 
    ViewAll(GetSafeHwnd());
}

afx_msg void CServerView::OnZoomIn()
{
	on_zoom(IDC_ZOOMIN);
}

afx_msg void CServerView::OnZoomOut()
{
	on_zoom(IDC_ZOOMOUT);
}

void CServerView::on_zoom(int idCmd)
{
	int x, y, inc;

    if (STOP) return; 
    
    if (!m_lpDisplay->ViewPercentage)
        return;

    x = ( m_lpDisplay->FileRect.left 
            + m_lpDisplay->FileRect.right ) / 2;
    y = ( m_lpDisplay->FileRect.top 
            + m_lpDisplay->FileRect.bottom ) / 2;
    SaveLastView(GetSafeHwnd());
	if (idCmd == IDC_ZOOMIN)
		inc = 100;
	else
		inc = -100;
    Zoom(GetSafeHwnd(), x,y, inc, YES,( View.ZoomWindow ^ CONTROL ) );
}

afx_msg void CServerView::OnLoadShape()
{
    CommandProc( GetSafeHwnd(), IDM_LOADSHAPE );
}

afx_msg void CServerView::OnSaveShape()
{
    CommandProc( GetSafeHwnd(), IDM_SAVESHAPE );
}
