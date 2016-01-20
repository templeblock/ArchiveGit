//////////////////////////////////////////////////////////////////////////////
// $Header: /PM8/App/BaseCustomPage.cpp 1     3/03/99 6:03p Gbeddow $
//
//  "This unpublished source code contains trade secrets which are the
//   property of Mindscape, Inc.  Unauthorized use, copying or distribution
//   is a violation of international laws and is strictly prohibited."
// 
//        Copyright © 1998 Mindscape, Inc. All rights reserved.
//
// $Log: /PM8/App/BaseCustomPage.cpp $
// 
// 1     3/03/99 6:03p Gbeddow
// 
// 11    3/02/99 3:41p Lwu
// changed to use the apply's page components
// 
// 10    3/01/99 12:02p Mwilson
// added new states
// 
// 9     2/22/99 4:31p Lwu
// changed calls to image ctrl draw to include srcRect
// 
// 8     2/19/99 12:32p Mwilson
// modified constructor to take page name
// 
// 7     2/17/99 4:28p Lwu
// added extra params to the imagecontrol call
// 
// 6     2/16/99 5:29p Mwilson
// added scaling for tiles
// 
// 5     2/11/99 1:55p Mwilson
// 
// 4     2/11/99 1:38p Lwu
// sizing message
// 
// 3     2/11/99 12:05p Lwu
// handle preTranslate message and message handler for mouse moves
// 
// 2     2/11/99 11:37a Mwilson
// Updated
// 
// 1     2/10/99 3:07p Mwilson
// 
// 2     5/31/98 5:19p Hforman
// 
//    Rev 1.0   14 Aug 1997 15:21:16   Fred
// Initial revision.
// 
//////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "BaseCustomPage.h"

#include "imgctrl.h"
#include "dcdrawingsurface.h"
#include "ImageUtils.h"
#include "CtrlIDs.h"
#include "CustCtrlDef.h"
#include "TransBtn.h"
#include "pmw.h"


CPBaseCustomPage::CPBaseCustomPage(CString csPageName, CWnd* pParent /*= NULL*/) : 
	CDialog(CPBaseCustomPage::IDD, pParent),
	m_pImageControl		(NULL),
	m_czDesignedRes		(800,600),
	m_pWndMouseEntered	(NULL),
	m_czCurRes(m_czDesignedRes),
	m_csPageName(csPageName),
	m_pAppImageManager	(NULL)
{
   //{{AFX_DATA_INIT(CAboutDlg)
   //}}AFX_DATA_INIT
}

CPBaseCustomPage::~CPBaseCustomPage()
{
	while(!m_listImageControls.IsEmpty())
		delete m_listImageControls.RemoveHead();
	
	while(!m_listControls.IsEmpty())
		delete m_listControls.RemoveHead();

}


void CPBaseCustomPage::DoDataExchange(CDataExchange* pDX)
{
   CDialog::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(CPBaseCustomPage)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CPBaseCustomPage, CDialog)
   //{{AFX_MSG_MAP(CPBaseCustomPage)
   ON_WM_PAINT()
	ON_WM_SIZE()
   //}}AFX_MSG_MAP
	ON_MESSAGE( UM_MOUSEMOVE, OnItemMouseMove )
END_MESSAGE_MAP()

BOOL CPBaseCustomPage::OnInitDialog()
{
   CDialog::OnInitDialog();

	//Load the page from the database
	CPUIPageComponentManager* pPgMngr = GET_PMWAPP()->GetPageManager();
	pPgMngr->LoadPage(m_Page, m_csPageName);

	m_pAppImageManager = GET_PMWAPP()->GetImageManager();

	if(!ChangeResolution())
		return FALSE;

	//get list of control info
	CtrlInfoList* pListControls = m_Page.GetControls();

	//the first control is always the background
	POSITION pos = pListControls->GetHeadPosition();
	if(!pos)
		return FALSE;
	//get background info
	CPControlInfo* pInfo = pListControls->GetNext(pos);

	//create background image ctrl
	CPImageControl* pImageControl = new CPImageControl(pInfo->csBmpUp, 
													pInfo->csBmpDown, pInfo->csBmpGlow,
													pInfo->csBmpSelGlow, pInfo->csBmpDisabled,
													pInfo->csBmpDefault,	pInfo->bTiled,
													NULL,
													*m_pAppImageManager);
	//add image ctrl to list.  The page manages all the image ctrls
	m_listImageControls.AddTail(pImageControl);
	//set ptr to bkgnd for easy access
	m_pImageControl = pImageControl;

	CPoint cpPos = pInfo->cpPosition;
	CSize czSize = pInfo->czSize;
	CRect rcImage(cpPos, czSize);
	BOOL bOffset = !((cpPos.x == 0) & (cpPos.y == 0));
	ScaleRect(rcImage, bOffset);
	m_pImageControl->SetControlSize(rcImage);
	//if the background is tiled this will scale the background accordingly
	m_pImageControl->SetOutputResolution(m_czCurRes);

	//loop through and create rest of the controls that go on the page
	while(pos)
	{
		pInfo = pListControls->GetNext(pos);
		//create img ctrl
		pImageControl = new CPImageControl(pInfo->csBmpUp, 
													pInfo->csBmpDown, pInfo->csBmpGlow,
													pInfo->csBmpSelGlow, pInfo->csBmpDisabled,
													pInfo->csBmpDefault,	pInfo->bTiled,
													m_pImageControl,
													*m_pAppImageManager);

		//add imgctrl to list
		m_listImageControls.AddTail(pImageControl);
		//create wnd ctrl base on type
		switch(pInfo->nControlType)
		{
			case CPCompPageCollBuilder::BUTTON:
			{
				//creat window ctrl
				CTransBtn* pButton = new CTransBtn;
				CRect rcWnd(pInfo->cpPosition, pInfo->czSize);
				ScaleRect(rcWnd);
				pButton->Create(pInfo->csCaption, BS_PUSHBUTTON | WS_CHILD | WS_VISIBLE, rcWnd, this, pInfo->nCtrlID);
				//init windows img ctrl
				pButton->SetImageControl(pImageControl);
				pImageControl->SetControlSize(rcWnd);

				//add ctrl to list
				m_listControls.AddTail(pButton);
				break;
			}
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
				ASSERT(0);
				break;
		
		}
	}
	
   return TRUE;  // return TRUE  unless you set the focus to a control
}

BOOL CPBaseCustomPage::ChangeResolution() 
{
	//get current device resolution
	CDC* pDC = GetDC();
	int nXRes = pDC->GetDeviceCaps(HORZRES);
	int nYRes = pDC->GetDeviceCaps(VERTRES);
	ReleaseDC(pDC);

	//get scale factors based on intended resolution
	m_fXScaleFactor = (float)nXRes / m_czDesignedRes.cx;
	m_fYScaleFactor = (float)nYRes / m_czDesignedRes.cy;
	m_czCurRes.cx = nXRes;
	m_czCurRes.cy = nYRes;

	CtrlInfoList* pListControls = m_Page.GetControls();

	//get background info
	CPControlInfo* pInfo = pListControls->GetHead();
	if(!pInfo)
		return FALSE;

	//Setup window size and placement
	CPoint cpPos = pInfo->cpPosition;
	CSize czSize = pInfo->czSize;
	CRect rcWindow(cpPos, czSize);
	BOOL bOffset = !((cpPos.x == 0) & (cpPos.y == 0));
	ScaleRect(rcWindow, bOffset);

	MoveWindow(rcWindow, TRUE);
	return TRUE;

}

void CPBaseCustomPage::OnPaint() 
{
	CPaintDC dc(this);
	CRect rcClient;
	GetClientRect(rcClient);
	m_pImageControl->Draw(CPImageControl::UP, dc, rcClient, rcClient);
}

void CPBaseCustomPage::OnSize( UINT nType, int cx, int cy )
{
	//image control hasn't been created yet we are still initializing
	if(!m_pImageControl)
		return;

	m_pImageControl->SetControlSize( CRect(0,0, cx, cy) );
	//if the background is tiled this will scale the background accordingly
	m_pImageControl->SetOutputResolution(m_czCurRes);

	CRect rcWnd;
	//the first control is always the background
	POSITION pos = m_listControls.GetHeadPosition();
	while(pos != NULL)
	{	
		CWnd* pWnd = m_listControls.GetNext(pos);
		if(pWnd)
		{
			pWnd->GetWindowRect(&rcWnd);
			ScaleRect(rcWnd);
			pWnd->SendMessage(UM_SIZECTRL, 0, (LPARAM)&rcWnd);
			pWnd->MoveWindow(rcWnd);
		}
	}
}

void CPBaseCustomPage::ScaleRect(CRect& rcScale, BOOL bOffset /*=TRUE*/)
{
	int nXInflate = (int)(rcScale.Width() * m_fXScaleFactor + .5) - rcScale.Width();
	int nYInflate = (int)(rcScale.Height() * m_fYScaleFactor + .5) - rcScale.Height();
	rcScale.InflateRect(0,0, nXInflate, nYInflate);
	if(bOffset)
	{
		CPoint ptTemp = rcScale.TopLeft();
		CPoint ptOffset;
		ptOffset.x = (int)(ptTemp.x * m_fXScaleFactor + .5) - ptTemp.x;
		ptOffset.y = (int)(ptTemp.y * m_fYScaleFactor + .5) - ptTemp.y;
		rcScale.OffsetRect(ptOffset);
	}
}

LRESULT CPBaseCustomPage::OnItemMouseMove( WPARAM wParam, LPARAM lParam)
{
	m_pWndMouseEntered = (CWnd*)lParam;
	return 0L;
}


BOOL CPBaseCustomPage::PreTranslateMessage(MSG* pMsg) 
{
	// was there a button that the mouse had entered before?
	if (m_pWndMouseEntered)
		ValidatePoint(pMsg->message, pMsg->pt);
	
	return CDialog::PreTranslateMessage(pMsg);
}

void CPBaseCustomPage::ValidatePoint(UINT message, POINT pt)
{
	if (message	== WM_MOUSEMOVE)
	{
		RECT	rcBtn;
		::GetWindowRect(m_pWndMouseEntered->m_hWnd, &rcBtn);
		
		// here i'm only testing the rect instead of the
		// clickable region coz everything within the rect
		// is testing within the btn's own mouse move message
		if (::PtInRect(&rcBtn, pt))
			return;

		m_pWndMouseEntered->SendMessage(UM_MOUSEEXIT);
	}
}

