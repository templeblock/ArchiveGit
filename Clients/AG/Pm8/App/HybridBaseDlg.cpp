// HybridBaseDlg.cpp : implementation file
//

#include "stdafx.h"
#include "HybridBaseDlg.h"
#include "Resource.h"
#include "ImageUtils.h"
#include "imgctrl.h"
#include "TransBtn.h"
#include "CustCtrlDef.h"
#include "pmwcfg.h"
#include "pmw.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CHybridBaseDlg dialog


CHybridBaseDlg::CHybridBaseDlg(UINT nIDTemplate, CWnd* pParentWnd,
									int nBorderAdjust, int nTitleAdjust):
		CDialog					(nIDTemplate, pParentWnd), 
		m_nBdrAdjust			(nBorderAdjust),
		m_nTitleAdjust			(nTitleAdjust),
		m_pBkgdImageCtrl		(NULL),
		m_pBorderImageCtrl	(NULL),
		m_pCornerImageCtrl	(NULL),
		m_czBrdThickness		(0,0),
		m_pDlogFont				(NULL),
		m_fTileBkgdImage		(FALSE),
		m_fCustomLook			(FALSE),
		m_pAppImageManager	(NULL)
{
	m_strBkgdImageName.Empty();
}

CHybridBaseDlg::~CHybridBaseDlg()
{
}

void CHybridBaseDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CHybridBaseDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}



BEGIN_MESSAGE_MAP(CHybridBaseDlg, CDialog)
	//{{AFX_MSG_MAP(CHybridBaseDlg)
	ON_MESSAGE(WM_SETTEXT, OnSetText)
	ON_WM_NCACTIVATE()
	ON_WM_NCPAINT()
	ON_WM_SYSCOLORCHANGE()
	ON_WM_SETTINGCHANGE()
	ON_WM_NCCALCSIZE()
	ON_WM_NCHITTEST()
	ON_WM_NCCREATE()
	ON_WM_NCLBUTTONDOWN()
	ON_WM_NCLBUTTONDBLCLK()
	ON_WM_DESTROY()
   ON_WM_PAINT()
	ON_WM_SIZE()
   ON_WM_CTLCOLOR()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


void CHybridBaseDlg::DrawBorder(CDC* pDC, CRect& theRect)
{
	//pDC->DrawEdge(theRect, EDGE_RAISED, BF_RECT | BF_MIDDLE);
	
	if (m_pCornerImageCtrl)
		DrawCornerPieces(pDC, theRect);

	if (m_pBorderImageCtrl)
		DrawSidePieces(pDC, theRect);
}

void CHybridBaseDlg::DrawCornerPieces(CDC* pDC, CRect& theRect)
{
	//===== do the corners
	CRect rcCorner;
	CRect rcCPiece(0,0,m_czBrdThickness.cx, m_czBrdThickness.cy);
	m_pCornerImageCtrl->SetControlSize(rcCPiece);

	// top left
	rcCorner.SetRect(theRect.left, theRect.top, 
				theRect.left + m_czBrdThickness.cx, theRect.top + m_czBrdThickness.cy);
	if (m_pCornerImageCtrl->Draw(CPImageControl::TOPLEFT, *pDC, rcCPiece, rcCorner) == ERRORCODE_Fail)
		return;

	// bottom left
	rcCorner.SetRect(theRect.left, theRect.bottom - m_czBrdThickness.cy,
				theRect.left + m_czBrdThickness.cx, theRect.bottom);
	if (m_pCornerImageCtrl->Draw(CPImageControl::BOTTOMLEFT, *pDC, rcCPiece, rcCorner) == ERRORCODE_Fail)
		return;

	// top right
	rcCorner.SetRect(theRect.right - m_czBrdThickness.cx, theRect.top,
				theRect.right, theRect.top + m_czBrdThickness.cy);
	if (m_pCornerImageCtrl->Draw(CPImageControl::TOPRIGHT, *pDC, rcCPiece, rcCorner) == ERRORCODE_Fail)
		return;

	// bottom right
	rcCorner.SetRect(theRect.right - m_czBrdThickness.cx, theRect.bottom - m_czBrdThickness.cy,
				theRect.right, theRect.bottom);
	if (m_pCornerImageCtrl->Draw(CPImageControl::BOTTOMRIGHT, *pDC, rcCPiece, rcCorner) == ERRORCODE_Fail)
		return;
}

void CHybridBaseDlg::DrawSidePieces(CDC* pDC, CRect& theRect)
{
	// now do the sides stretched
	CRect rcSides;

	// horizontal piece size
	CRect rcSPiece(0,0,
				theRect.Width() - (2*m_czBrdThickness.cx),
				m_czBrdThickness.cy);
	m_pBorderImageCtrl->SetControlSize(rcSPiece);

	// top
	rcSides.SetRect(theRect.left + m_czBrdThickness.cx, theRect.top,
				theRect.right - m_czBrdThickness.cx, theRect.top + m_czBrdThickness.cy);
	m_pBorderImageCtrl->Draw(CPImageControl::TOP, *pDC, rcSPiece, rcSides);

	// bottom
	rcSides.top = theRect.bottom - m_czBrdThickness.cy;
	rcSides.bottom = theRect.bottom;
	m_pBorderImageCtrl->Draw(CPImageControl::BOTTOM, *pDC, rcSPiece, rcSides);

	// vertical side piece
	rcSPiece.SetRect(0,0,
					m_czBrdThickness.cx,
					theRect.Height() - (2*m_czBrdThickness.cy));
	m_pBorderImageCtrl->SetControlSize(rcSPiece);
	// left
	rcSides.SetRect(theRect.left, theRect.top + m_czBrdThickness.cy,
				theRect.left + m_czBrdThickness.cx, theRect.bottom - m_czBrdThickness.cy);
	m_pBorderImageCtrl->Draw(CPImageControl::LEFT, *pDC, rcSPiece, rcSides);

	// right
	rcSides.left = theRect.right - m_czBrdThickness.cx;
	rcSides.right = theRect.right;
	m_pBorderImageCtrl->Draw(CPImageControl::RIGHT, *pDC, rcSPiece, rcSides);
}

BOOL CHybridBaseDlg::LoadDlogBkImages()
{
	//get list of control info
	CtrlInfoList* pListCtrls = m_Page.GetControls();

	//the first control is always the background
	POSITION pos = pListCtrls->GetHeadPosition();
	if(!pos)
		return FALSE;
	
	CPControlInfo* pInfo; 
	CPImageControl* pImageControl;

	// now i need to take care of the border pieces and the titlebar
	while(pos)
	{
		//get info
		pInfo = pListCtrls->GetNext(pos);
		
		//create wnd ctrl base on type
		switch(pInfo->nControlType)
		{
			case CPCompPageCollBuilder::BKGND:
				{
					//create background image ctrl
					if (m_strBkgdImageName.IsEmpty())
					{
						// m_strBkgdImageName and m_fTileBkgdImage are a protected members
						// subclassed dialogs have the chance to set this in order to get a
						// different background image and the tiled flag
						m_strBkgdImageName = pInfo->csBmpUp;
						m_fTileBkgdImage = pInfo->bTiled;
					}
					pImageControl = new CPImageControl(m_strBkgdImageName, 
																	pInfo->csBmpDown, pInfo->csBmpGlow, pInfo->csBmpSelGlow,
																	pInfo->csBmpDisabled, pInfo->csBmpDefault,
																	m_fTileBkgdImage,
																	NULL,
																	*m_pAppImageManager);
					
					//set ptr to bkgnd for easy access
					m_pBkgdImageCtrl = pImageControl;
				}
				break;

			case CPCompPageCollBuilder::BORDER:
				{
					//create img ctrl
					pImageControl = new CPImageControl(pInfo->csBmpUp, 
																pInfo->csBmpDown, pInfo->csBmpGlow, pInfo->csBmpSelGlow,
																pInfo->csBmpDisabled, pInfo->csBmpDefault,
																pInfo->bTiled,
																NULL,
																*m_pAppImageManager);

					m_pBorderImageCtrl = pImageControl;
				}
				break;

			case CPCompPageCollBuilder::CORNER:
				{
					//create img ctrl
					pImageControl = new CPImageControl(pInfo->csBmpUp, 
																pInfo->csBmpDown, pInfo->csBmpGlow, pInfo->csBmpSelGlow,
																pInfo->csBmpDisabled, pInfo->csBmpDefault,
																pInfo->bTiled,
																NULL,
																*m_pAppImageManager);

					m_pCornerImageCtrl = pImageControl;
				}
				break;

			case CPCompPageCollBuilder::TITLEBAR:
				{
					//create img ctrl
					pImageControl = new CPImageControl(pInfo->csBmpUp, 
																pInfo->csBmpDown, pInfo->csBmpGlow, pInfo->csBmpSelGlow,
																pInfo->csBmpDisabled, pInfo->csBmpDefault,
																pInfo->bTiled,
																NULL,
																*m_pAppImageManager);

					m_capp.SetTitlebarImgCtrl(pImageControl);
				}
				break;

			case CPCompPageCollBuilder::BUTTON:
				{
					switch (pInfo->nBtnType)
					{
						case CPCompPageCollBuilder::SYSCLOSEBTN:
						case CPCompPageCollBuilder::SYSMINBTN:
						case CPCompPageCollBuilder::SYSMAXBTN:
						case CPCompPageCollBuilder::SYSRESTOREBTN:
						{
							pImageControl = new CPImageControl(pInfo->csBmpUp, 
																	pInfo->csBmpDown, pInfo->csBmpGlow, pInfo->csBmpSelGlow,
																	pInfo->csBmpDisabled, pInfo->csBmpDefault,
																	pInfo->bTiled,
																	NULL,
																	*m_pAppImageManager);

							CRect rcBtn(pInfo->cpPosition, pInfo->czSize);
							m_capp.SetTBarCtrlImgCtrl(pImageControl, pInfo->nBtnType, rcBtn, pInfo->nCtrlID);
						}
						break;
						
						default:
							break;
					}
				}
				break;
		}
		
		if (pImageControl != NULL)
		{
			//add image ctrl to list.  The page manages all the image ctrls
			m_listImageControls.AddTail(pImageControl);
			if(pInfo->nControlType == CPCompPageCollBuilder::BKGND)
			{
				if(!m_fTileBkgdImage )
				{
					m_rcBkgndImage.SetRectEmpty();
				}
				else
				{
					CPoint cpPos = pInfo->cpPosition;
					CSize czSize = pInfo->czSize;
					m_rcBkgndImage.SetRect(cpPos, cpPos + czSize);
				}
			}
			else
			{
				CPoint cpPos = pInfo->cpPosition;
				CSize czSize = pInfo->czSize;
				CRect rcImage(cpPos, czSize);
				m_pBkgdImageCtrl->SetControlSize(rcImage);
			}
		}

		pImageControl = NULL;	// null this out for the next timearound

	}
	return TRUE;
}


BOOL CHybridBaseDlg::CreateCustomCtrl(HWND childHWnd, int nCtrlType)
{
	CtrlInfoList* pListCtrls = m_Page.GetControls();;

	POSITION pos = pListCtrls->GetHeadPosition();
	if(!pos)
		return FALSE;

	while(pos)
	{
		CPControlInfo* pInfo = pListCtrls->GetNext(pos);

		//create wnd ctrl base on type
		switch(pInfo->nControlType)
		{
			case CPCompPageCollBuilder::BUTTON:
				if (nCtrlType == CPCompPageCollBuilder::BUTTON)
				{
					ASSERT(childHWnd);
					if (CreateButton(childHWnd, pInfo))
						return TRUE;
				}
				break;

			case CPCompPageCollBuilder::RADIO:
			{
				break;
			}
			case CPCompPageCollBuilder::CHECKBOX:
			{
				break;
			}
			case CPCompPageCollBuilder::STATIC:
			{
				break;
			}
			case CPCompPageCollBuilder::LISTBOX:
			{
				break;
			}
			case CPCompPageCollBuilder::COMBOBOX:
			{
				break;
			}
			default:
				//ASSERT(0);
				break;
		
		}
	}
	return TRUE;
}

BOOL CHybridBaseDlg::CreateButton(HWND hWnd, CPControlInfo* pInfo)
{
	if (pInfo->nBtnType == CPCompPageCollBuilder::STDBTN)	// create standard btn
	{
		//create window ctrl
		CTransBtn* pButton = new CTransBtn;
		
		// rect of the control is based on the window rect
		CRect rcWnd;
		::GetWindowRect(hWnd, &rcWnd);

		pButton->SubclassWindow(hWnd);
		
		//create img ctrl
		CPImageControl* pImageControl = new CPImageControl(pInfo->csBmpUp, 
													pInfo->csBmpDown, pInfo->csBmpGlow, pInfo->csBmpSelGlow,
													pInfo->csBmpDisabled, pInfo->csBmpDefault,
													pInfo->bTiled,
													m_pBkgdImageCtrl,
													*m_pAppImageManager);

		//add imgctrl to list
		m_listImageControls.AddTail(pImageControl);
		
		//init windows img ctrl
		pButton->SetImageControl(pImageControl);
		pImageControl->SetControlSize(rcWnd);
		
		//add ctrl to list
		m_listControls.AddTail(pButton);
		if (m_pDlogFont)
			pButton->SendMessage(WM_SETFONT, (WPARAM)m_pDlogFont->GetSafeHandle(), 0L);
		return TRUE;
	}
	return FALSE;
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
// CHybridBaseDlg message handlers

BOOL CHybridBaseDlg::PreTranslateMessage(MSG* pMsg) 
{
	if (m_fCustomLook)
	{
		// did the caption handle the message? if yes, return
		if (m_capp.RelayEvent(pMsg) == TRUE)
			return TRUE;
	}

	return CDialog::PreTranslateMessage(pMsg);
}

/////////////////////////////////////////////////////////////////////////////
void CHybridBaseDlg::OnDestroy() 
{
	while(!m_listImageControls.IsEmpty())
		delete m_listImageControls.RemoveHead();
	
	while(!m_listControls.IsEmpty())
		delete m_listControls.RemoveHead();

	if (m_pDlogFont)
	{
		delete m_pDlogFont;
		m_pDlogFont = NULL;
	}

	CDialog::OnDestroy();

}

/////////////////////////////////////////////////////////////////////////////
BOOL CHybridBaseDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	if (!m_pDlogFont)
	{
		m_pDlogFont = new CFont;
		
		LOGFONT lf;                        // Used to create the CFont.
		memset(&lf, 0, sizeof(LOGFONT));   // Clear out structure.
		lf.lfHeight = 16;                  
		strcpy(lf.lfFaceName, "Arial");    //    with face name "Arial".
		m_pDlogFont->CreateFontIndirect(&lf);    // Create the font.
	
	}

	if (m_fCustomLook)
	{

		//Load the page from the database
		// we want the "Generic" portion of the page layout 
		CPUIPageComponentManager* pPgMngr = GET_PMWAPP()->GetPageManager();
		pPgMngr->LoadPage(m_Page, "Generic");

		m_pAppImageManager = GET_PMWAPP()->GetImageManager();
		EnumChildWindows(GetSafeHwnd(), CustomizeChildWindow, (LPARAM)this);

		// Install caption painter
		m_capp.Install(this, 16, "Arial", m_nBdrAdjust, m_nTitleAdjust);
		
		LoadDlogBkImages();
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
} 

/////////////////////////////////////////////////////////////////////////////
BOOL CALLBACK CHybridBaseDlg::CustomizeChildWindow(HWND hWindow, LPARAM lData)
{
	CHybridBaseDlg* pDialog = (CHybridBaseDlg*)lData;
	
	char szClassName[64];
	::GetClassName(hWindow, szClassName, sizeof(szClassName));
	if (lstrcmpi(szClassName, "BUTTON") == 0)
	{
		LONG theStyle = GetWindowLong(  hWindow, GWL_STYLE );
		if ( (theStyle & BS_AUTORADIOBUTTON) == BS_AUTORADIOBUTTON  ||
				(theStyle & BS_RADIOBUTTON) == BS_RADIOBUTTON	)
				;	// might do something for radio buttons?
		else if ((theStyle & BS_AUTOCHECKBOX) == BS_AUTOCHECKBOX  ||
				(theStyle & BS_CHECKBOX) == BS_CHECKBOX )
				;	// might do something for checkbox
		else	// just the regular button
			pDialog->CreateCustomCtrl(hWindow, CPCompPageCollBuilder::BUTTON);
	}
	
	return TRUE;
}




/////////////////////////////////////////////////////////////////////////////
void CHybridBaseDlg::OnPaint() 
{
	if (m_fCustomLook)
	{
		CPaintDC dc(this);
		DrawBkgnd(&dc);
	}
	else
		CDialog::OnPaint();
}

void CHybridBaseDlg::DrawBkgnd(CDC* pDC)
{
	CRect rcClient;
	GetClientRect(rcClient);
	if(m_rcBkgndImage.IsRectEmpty())
		m_rcBkgndImage = rcClient;
	m_pBkgdImageCtrl->SetControlSize(m_rcBkgndImage);
	m_pBkgdImageCtrl->Draw(CPImageControl::UP, *pDC, rcClient, rcClient);
}
/////////////////////////////////////////////////////////////////////////////
BOOL CHybridBaseDlg::OnNcActivate(BOOL bActive )
{
	if (m_fCustomLook)
		return m_capp.OnNcActivate(bActive);
	else
		return CDialog::OnNcActivate(bActive);
	
}

/////////////////////////////////////////////////////////////////////////////
void CHybridBaseDlg::OnNcPaint() 
{
	if (m_fCustomLook)
	{
		// Get window DC that we will be drawing into.
		CWindowDC dc(this);

		// Compute the rectangle we will be updating.
		CRect windowRect;
		GetWindowRect(windowRect);
		ScreenToClient(windowRect);

		// make it zero based
		windowRect.OffsetRect(-windowRect.left, -windowRect.top);

		// draw the border
		DrawBorder(&dc, windowRect);

		// do the title bar
		CRgn	myUpdateRgn;
		if (GetUpdateRgn( &myUpdateRgn, TRUE ) !=  NULLREGION )
			m_capp.OnNcPaint((HRGN)myUpdateRgn.m_hObject);
	}
	else
		CDialog::OnNcPaint();
}


/////////////////////////////////////////////////////////////////////////////
void CHybridBaseDlg::OnSysColorChange() 
{
	if (m_fCustomLook)
		m_capp.Redraw();

	CDialog::OnSysColorChange();
}


/////////////////////////////////////////////////////////////////////////////
void CHybridBaseDlg::OnSettingChange(UINT uFlags, LPCTSTR lpszSection) 
{
	if (m_fCustomLook)
		m_capp.Redraw();

	CDialog::OnSettingChange(uFlags, lpszSection);
}

/////////////////////////////////////////////////////////////////////////////
LRESULT CHybridBaseDlg::OnSetText(WPARAM wParam,LPARAM lParam)
{
	if (m_fCustomLook)
		m_capp.OnSetText((LPCTSTR)lParam);
	return 0;
}

/////////////////////////////////////////////////////////////////////////////
void CHybridBaseDlg::OnNcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS FAR* lpncsp) 
{
	if (m_fCustomLook)
	{
		// decide how to muck with this client rect

		// Modify the first rectangle
		LONG	style = GetStyle();
		BOOL fCustThickness = FALSE;

		if (style & CAC_THICKFRAME) 
		{
			::InflateRect(lpncsp->rgrc, -m_nBdrAdjust, -m_nBdrAdjust);
			fCustThickness = TRUE;
		}
		else if (style & WS_THICKFRAME)
		{
			::InflateRect(lpncsp->rgrc, -GetSystemMetrics(SM_CXFRAME), 
				-GetSystemMetrics(SM_CYFRAME));
		}
		else
		{
			::InflateRect(lpncsp->rgrc, -GetSystemMetrics(SM_CXBORDER),
				-GetSystemMetrics(SM_CYBORDER));
		}
		
		if ((style & WS_CAPTION) == WS_CAPTION)
		{
			lpncsp->rgrc[0].top += GetSystemMetrics(SM_CYCAPTION) + // standard height of caption
										((fCustThickness)? 0: GetSystemMetrics(SM_CYBORDER)) + // that border height for bottom
										m_nTitleAdjust;										// that extra for custom height
		}
	}
	else
		CDialog::OnNcCalcSize(bCalcValidRects, lpncsp);
}

/////////////////////////////////////////////////////////////////////////////

BOOL CHybridBaseDlg::OnNcCreate(LPCREATESTRUCT lpCreateStruct) 
{
   if(GetConfiguration()->Product() == CPrintMasterConfiguration::plAmericanGreetings)
	{
		// set this flag that determines if we do any of those customize look controls etc
		m_fCustomLook = TRUE;

		DWORD dwStyle = GetStyle();
		dwStyle |= CAC_THICKFRAME;
		::SetWindowLong(m_hWnd, GWL_STYLE, dwStyle);

		m_nTitleAdjust = 4;
		m_nBdrAdjust = 6;

		
		m_czBrdThickness = (dwStyle & WS_THICKFRAME) ?
			CSize(GetSystemMetrics(SM_CXSIZEFRAME),
					GetSystemMetrics(SM_CYSIZEFRAME)) :
			CSize(GetSystemMetrics(SM_CXFIXEDFRAME),
					GetSystemMetrics(SM_CYFIXEDFRAME));

		if ( dwStyle & CAC_THICKFRAME)
		{
			m_czBrdThickness.cx += m_nBdrAdjust / 2;
			m_czBrdThickness.cy += m_nBdrAdjust / 2;
		}
	}

	return CDialog::OnNcCreate(lpCreateStruct);
}

/////////////////////////////////////////////////////////////////////////////
UINT CHybridBaseDlg::OnNcHitTest(CPoint point) 
{
	if (m_fCustomLook)
		return m_capp.OnNcHitTest(point);
	else
		return CDialog::OnNcHitTest(point);
}

/////////////////////////////////////////////////////////////////////////////
void CHybridBaseDlg::OnNcLButtonDown(UINT nHitTest, CPoint point) 
{
	if (m_fCustomLook)
	{
		if (m_capp.OnNcLBtnDown(nHitTest, point) == TRUE)
			return;
	}

	CDialog::OnNcLButtonDown(nHitTest, point);
}


/////////////////////////////////////////////////////////////////////////////
void CHybridBaseDlg::OnNcLButtonDblClk(UINT nHitTest, CPoint point) 
{
	if (m_fCustomLook)
		m_capp.OnNcLBtnDblClk(nHitTest, point);
	
	CDialog::OnNcLButtonDblClk(nHitTest, point);

}


/////////////////////////////////////////////////////////////////////////////
void CHybridBaseDlg::OnSize( UINT nType, int cx, int cy )
{
	CDialog::OnSize(nType, cx, cy);
}

/////////////////////////////////////////////////////////////////////////////
HBRUSH CHybridBaseDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
    switch (nCtlColor)
    {
        case CTLCOLOR_STATIC:
        case CTLCOLOR_BTN:
            //
            // set transfer mode to be transparent
            //
            pDC->SetBkMode(TRANSPARENT);

            return (HBRUSH)GetStockObject(HOLLOW_BRUSH);
    }
    
    return CDialog::OnCtlColor(pDC, pWnd, nCtlColor);
}
