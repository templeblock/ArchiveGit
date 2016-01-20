// CustomTTWnd.cpp : implementation file
//

#include "stdafx.h"
#include "CustomTTip.h"
#include "CustomTTWnd.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif



/////////////////////////////////////////////////////////////////////////////
// CTipObj

CTipObj::CTipObj(CWnd* pWnd, CString& theText, int index):
	m_pCtrlWnd			(pWnd),
	m_strCtrlTipText	(theText),
	m_nIndex				(index)
{
}

CTipObj::~CTipObj()
{
}

/////////////////////////////////////////////////////////////////////////////
// CCustomTTWnd

LPCSTR CCustomTTWnd::m_pszWndClass = NULL;

/* ============================================
		CCustomTTWnd::CCustomTTWnd()
		
		Construction
=============================================== */
CCustomTTWnd::CCustomTTWnd():
	m_bActivate			(FALSE),
	m_nShowDelay		(1000),
	m_nHideDelay		(2000),
	m_bmpSize			(0,0),
	m_LastMouseMovePoint(0,0),
	m_nToolCount		(0),
	m_nCurToolIndex	(-1),
	m_nTimerId			(0),
	m_nHideTimerId		(0),
	m_nBitmapID			(-1),
	m_pTipFont			(NULL),
	m_pTransbmpBubble	(NULL),
	m_pParentCtrl		(NULL),
	m_pCurTipObj		(NULL)
{
    // Register the window class if it has not already been registered by
    // previous instantiation of CCustomTTWnd.    
	if (m_pszWndClass == NULL)    
	{
        m_pszWndClass = AfxRegisterWndClass(
            CS_SAVEBITS | CS_HREDRAW | CS_VREDRAW);    
	}

	m_rcDisplayRect.SetRectEmpty();
	m_textRect.SetRectEmpty();
	m_rcMargin.SetRectEmpty();

	m_crTextColor = kDefFontColor;

}

/* ============================================
		CCustomTTWnd::~CCustomTTWnd()
		
		Destruction
=============================================== */
CCustomTTWnd::~CCustomTTWnd()
{
	if (m_pTransbmpBubble)	
		delete m_pTransbmpBubble;
	
	if (m_pTipFont)
		delete m_pTipFont;

	if (!m_pTipObjList->IsEmpty())
	{
		POSITION pListPos = m_pTipObjList->GetHeadPosition();
		while (pListPos != NULL)
		{
			CTipObj *pTipObj = (CTipObj*)m_pTipObjList->GetNext(pListPos);
			delete pTipObj;
		}
		m_pTipObjList->RemoveAll();
	}
	delete m_pTipObjList;

}


/* ============================================
		CCustomTTWnd::Create()
		
		Create the window, and the font
=============================================== */
BOOL CCustomTTWnd::Create(CWnd*		pParentWnd,
								  int			nBitmapID,
								  LPCRECT	rcMargin,
								  BOOL		bActivate,
								  CString	strTipFont,
								  int			nTipFontSize)
{    
	ASSERT_VALID(pParentWnd);
	m_pParentCtrl = pParentWnd;
    
	m_nBitmapID = nBitmapID;
	m_rcMargin = rcMargin;

	DWORD dwStyle = WS_POPUP;
	BOOL retVal = CreateEx(0, m_pszWndClass, NULL,         
								dwStyle, 0, 0, 0, 0,
								pParentWnd->GetSafeHwnd(), NULL, NULL);

	if (retVal != 0)
	{
		// load in the help bubble bitmap
		m_pTransbmpBubble = new CTransBmp;
		m_pTransbmpBubble->LoadBitmap(m_nBitmapID);
		m_bmpSize.cx = m_pTransbmpBubble->GetWidth();
		m_bmpSize.cy = m_pTransbmpBubble->GetHeight();

		// create the font
		SetTipFont(strTipFont, nTipFontSize);
	}

	m_pTipObjList = new CPtrList;

	m_bActivate = bActivate;

	return retVal;
}

/* ============================================
		CCustomTTWnd::SetTipFont()
		
		Set a new font to the tip
=============================================== */
void CCustomTTWnd::SetTipFont(CString	strTipFont, 
										int		nTipFontSize)
{
	if (m_pTipFont)
		delete m_pTipFont;

	// create the font
	m_pTipFont = new CFont;
	// size is in 10ths of a pt so multiply
	VERIFY (m_pTipFont->CreatePointFont (nTipFontSize*10, strTipFont, NULL));
}

/* ============================================
		CCustomTTWnd::AddTool()
		
		Called by parent window to add a tool to the list
=============================================== */
BOOL CCustomTTWnd::AddTool(CWnd *pWnd, CString tipText) 
{
	CTipObj *pTipObj = new CTipObj(pWnd, tipText, ++m_nToolCount);
	ASSERT(pTipObj);
	m_pTipObjList->AddHead(pTipObj);

	return TRUE;
}

/* ============================================
		CCustomTTWnd::AddTool()
		
		Call this to add tool if the tip text is
		embedded in the caption of the control separated
		by the token of "*"
=============================================== */
BOOL CCustomTTWnd::AddTool(CWnd *pWnd)
{
	// get the text out of the control, it should contain the 
	// tool tip text too
	CString myCaption;
	pWnd->GetWindowText(myCaption);

	int helpTextStart = myCaption.Find( '*' );// found the marker for tool tip
	ASSERT (helpTextStart != -1);	
	
	CString myHelpText = myCaption;
	myCaption = myCaption.Left( helpTextStart ) ;
	myHelpText = myHelpText.Right(myHelpText.GetLength() - helpTextStart - 1);
	// set the caption back after tool tip has been stripped
	pWnd->SetWindowText(myCaption);

	return AddTool(pWnd, myHelpText);
}


/* ============================================
		CCustomTTWnd::DelTool()
		
		Take this tool out of my list
=============================================== */
void CCustomTTWnd::DelTool(CWnd *pWnd)
{
	ASSERT(m_pTipObjList);
	if (m_pTipObjList->IsEmpty())
		return;

	POSITION pListPos, pListPos2;

	for( pListPos = m_pTipObjList->GetHeadPosition(); ( pListPos2 = pListPos ) != NULL; )   
	{
		CTipObj *pTipObj = (CTipObj*)m_pTipObjList->GetNext(pListPos);
		CWnd* pCtrlWnd = pTipObj->GetTipCtrlWnd();
		if ( pCtrlWnd == pWnd )
		{
			pTipObj = (CTipObj*)m_pTipObjList->GetAt( pListPos2 ); // Save the old pointer for deletion.
			m_pTipObjList->RemoveAt( pListPos2 );
			delete pTipObj; // Deletion avoids memory leak.
			m_nToolCount--;
			return;
		}   
	} while (pListPos != NULL);
}

/* ============================================
		CCustomTTWnd::SetTip()
		
		Sets the text string and calculates the textbox
=============================================== */
void CCustomTTWnd::SetTip(CString theString) 
{
	// make sure the bubble has been loaded
	// so i can get the size from that to adjust the text rect
	ASSERT(m_pTransbmpBubble);

	m_strTip = theString;

    // see what kind a rectangle this text will take up
	m_textRect.SetRect(0,0, m_bmpSize.cx, m_bmpSize.cy);
	
	// create dc, set font, draw text
	CClientDC DC(this);
	ASSERT_VALID(m_pTipFont);
	CFont* pOldFont = DC.SelectObject(m_pTipFont);
	int txtHeight = DC.DrawText(m_strTip, m_textRect, DT_CALCRECT );
	DC.SelectObject(pOldFont);

	// now try to get the new rectangle's aspect ratio
	// close to the bitmap or do a max of 3 lines
	double aspectRatio = (double)m_bmpSize.cx / (double)m_bmpSize.cy;
	int lineCount = 0;
	double rectWidth = m_textRect.Width();
	double myRatio = 0;
	double width = 0, height = 0;
	do
	{
		lineCount++;
		width = rectWidth / lineCount;
		height = txtHeight * lineCount;
		myRatio = width / height;

	} while ( aspectRatio != myRatio  && lineCount < 3);
	m_textRect.right = width;
	m_textRect.bottom = txtHeight * (lineCount + 1);	// add one line just in case

	// adjust for the bubble transparent edges, make zero based rect
	m_textRect.InflateRect(m_rcMargin);
	m_textRect.OffsetRect(m_rcMargin.left, m_rcMargin.top);

}

/* ============================================
		CCustomTTWnd::AdjustWindowRect()

		Move the display rectangle around relative to the mouse pos
		resize the rect based on the amount of text
=============================================== */
void CCustomTTWnd::AdjustWindowRect()
{
	ASSERT_VALID(m_pCurTipObj);

	// get the control's rect, in screen coordinates
	CRect myWinRect = m_textRect;	// text rect was set up in SetTip
	CRect ctrlRect;
	CWnd* pCtrlWnd = m_pCurTipObj->GetTipCtrlWnd();
	pCtrlWnd->GetWindowRect(ctrlRect);

	// get the current mouse position
	CPoint curPos;
	VERIFY(GetCursorPos(&curPos));

	// get the size of the cursor
	int curHeight = ::GetSystemMetrics(SM_CYCURSOR);

	// offset the rect to the mouse pos and the cursor height
	myWinRect.OffsetRect(CPoint(curPos.x, curPos.y + (curHeight/2)));


	// now adjust the actual size of the rectangle based on the 
	// amount of text if necessary
    if (!m_strTip.IsEmpty())
    {
        // m_textRect is calculated when the tip string is set
		int textRcDim = m_textRect.Width();
		if (textRcDim > m_bmpSize.cx)
			myWinRect.right += textRcDim - m_bmpSize.cx;
        
		textRcDim = m_textRect.Height();
		if (textRcDim > m_bmpSize.cy)
			myWinRect.bottom += textRcDim - m_bmpSize.cy;
	}

	// Adjust the rect for being near the edge of screen.
	int nScreenWidth = ::GetSystemMetrics(SM_CXFULLSCREEN);
	if (myWinRect.right > nScreenWidth)
        myWinRect.OffsetRect(nScreenWidth - myWinRect.right, 0);

	// this shouldn't happen
	if (myWinRect.left < 0)
        myWinRect.OffsetRect(-myWinRect.left, 0);            

	// adjust for the bottom
	int nScreenHeight = ::GetSystemMetrics(SM_CYFULLSCREEN);
	if (myWinRect.bottom > nScreenHeight)
        myWinRect.OffsetRect(0, nScreenHeight - myWinRect.bottom);

	// assign to the member variable
	m_rcDisplayRect = myWinRect;
}

/* ============================================
		CCustomTTWnd::Show()

		Move the window around and set the timer delay
=============================================== */
void CCustomTTWnd::Show()
{
	if (!m_bActivate)
		return;

	// if there is a timer for show, kill it
	if (m_nTimerId == TM_SHOWTIP)
	{
		VERIFY (KillTimer(m_nTimerId));
		m_nTimerId = 0;
	}
	// if there is a timer for hide, kill it
	if (m_nHideTimerId == TM_HIDETIP )
	{
		VERIFY (KillTimer(m_nHideTimerId));
		m_nHideTimerId = 0;
	}

	// move the window around and set new show timer
	AdjustWindowRect();
	m_nTimerId = SetTimer(TM_SHOWTIP, m_nShowDelay, NULL);
	ASSERT(m_nTimerId != 0);

}

/* ============================================
		CCustomTTWnd::Hide()

		Kill the timer and hide the window
=============================================== */

void CCustomTTWnd::Hide()
{    
	if (!m_bActivate)
		return;

	// if there is a timer for show, kill it
	if (m_nTimerId > 0)
	{
		VERIFY (KillTimer(m_nTimerId));
		m_nTimerId = 0;
	}
	
	// if there is a timer for hide, kill it
	if (m_nHideTimerId > 0)
	{
		VERIFY (KillTimer(m_nHideTimerId));
		m_nHideTimerId = 0;
	}
	
	// hide the window
	ASSERT(::IsWindow(m_hWnd));    
	ShowWindow(SW_HIDE);
	m_rcDisplayRect.SetRectEmpty();

}

/* ============================================
		CCustomTTWnd::NewMousePos()

		i don't need to know what the new position is
		but just that it has moved
		Kill the hide timer and start it new again
=============================================== */

void CCustomTTWnd::NewMousePos()
{
	// if there is a timer for hide, kill it
	if (m_nHideTimerId > 0)
	{
		VERIFY (KillTimer(m_nHideTimerId));
		m_nHideTimerId = 0;
	}
	
	// now, set the timer for it to hide again
	m_nHideTimerId = SetTimer(TM_HIDETIP, m_nHideDelay, NULL);
	ASSERT(m_nHideTimerId != 0);
}


/* ============================================
		CCustomTTWnd::RelayEvent()
		
		Called by the parent in PreTranslateMessage
=============================================== */
void CCustomTTWnd::RelayEvent(MSG* pMsg) 
{
	if (!m_bActivate)	// don't care if we are not activated
		return;

	switch (pMsg->message)
	{
		case WM_MOUSEMOVE: 
			HandleMouseMove(CPoint(pMsg->pt));
			break;

		case WM_LBUTTONDOWN:
		case WM_LBUTTONUP: 
		case WM_RBUTTONDOWN: 
		case WM_RBUTTONUP: 
		case WM_MBUTTONDOWN:
		case WM_MBUTTONUP: 
			ResetTip();
			break;
	}
}

/* ============================================
		CCustomTTWnd::HandleMouseMove()
		
		Show and Hide as appropriate
=============================================== */
void CCustomTTWnd::HandleMouseMove(POINT point)
{
	CPoint curPoint(point);
	if (curPoint != m_LastMouseMovePoint )
	{
		m_LastMouseMovePoint = point;
		CWnd* myWnd = WindowFromPoint( point );
		
		ASSERT(m_pTipObjList);
		BOOL		bFoundMatch = FALSE;
		int		tipIndex = -1;
		if (!m_pTipObjList->IsEmpty())
		{
			// loop through and see if i can find a match in my list
			POSITION pListPos = m_pTipObjList->GetHeadPosition();
			while (pListPos != NULL)
			{
				CTipObj *pTipObj = (CTipObj*)m_pTipObjList->GetNext(pListPos);
				CWnd* pCtrlWnd = pTipObj->GetTipCtrlWnd();
				if ( pCtrlWnd == myWnd)
				{
					
					m_pCurTipObj = pTipObj;	// set member var for reference later
					tipIndex = pTipObj->GetIndex();
					SetTip(pTipObj->GetTipText());// found a match, set the string from that control
					bFoundMatch = TRUE;
					break;
				}

			}
		}
		// mouse is over something that has a tool tip
		if (bFoundMatch)
		{
			// moved to a new tool
			if (m_nCurToolIndex != tipIndex)
			{
				m_nCurToolIndex = tipIndex;	// set this member var
				// if there is a tool up already,
				if (IsWindowVisible())
					Hide();	// hide the thing
				else
					Show();	// call show to set the show timer
			}
			// at the same tool, but new mouse position
			else
				NewMousePos();
		}
		else
		{
			ResetTip();
		}
	}

}

/* ============================================
		CCustomTTWnd::ResetTip()
		
		Reset all member vars and hide the window
=============================================== */
void CCustomTTWnd::ResetTip()
{
	m_nCurToolIndex = -1;
	m_pCurTipObj = NULL;
	m_strTip.Empty();
	Hide();
}

/* ============================================
		CCustomTTWnd::UpdateTipText()
		
		Set new tooltip text to tool that has been added already
=============================================== */
void CCustomTTWnd::UpdateTipText( LPCTSTR lpszText, CWnd* pWnd )
{
	ASSERT(m_pTipObjList);
	if (m_pTipObjList->IsEmpty())
		return;

	// loop through and see if i can find a match in my list
	POSITION pListPos = m_pTipObjList->GetHeadPosition();
	while (pListPos != NULL)
	{
		CTipObj *pTipObj = (CTipObj*)m_pTipObjList->GetNext(pListPos);
		CWnd* pCtrlWnd = pTipObj->GetTipCtrlWnd();
		if ( pCtrlWnd == pWnd )
		{
			pTipObj->SetTipText(CString(lpszText));// found a match, set the string from that control
			return;
		}

	}

}

/* ============================================
		CCustomTTWnd::GetText()
		
		Get the tooltip text for that tool
=============================================== */
void CCustomTTWnd::GetText( CString& str, CWnd* pWnd ) const
{
	ASSERT(m_pTipObjList);
	if (m_pTipObjList->IsEmpty())
		return;

	// loop through and see if i can find a match in my list
	POSITION pListPos = m_pTipObjList->GetHeadPosition();
	while (pListPos != NULL)
	{
		CTipObj *pTipObj = (CTipObj*)m_pTipObjList->GetNext(pListPos);
		CWnd* pCtrlWnd = pTipObj->GetTipCtrlWnd();
		if ( pCtrlWnd == pWnd )
		{
			str = pTipObj->GetTipText();// found a match, set the string from that control
			return;
		}

	}

}


BEGIN_MESSAGE_MAP(CCustomTTWnd, CWnd)
	//{{AFX_MSG_MAP(CCustomTTWnd)
	ON_WM_PAINT()
	ON_WM_TIMER()
	ON_WM_ERASEBKGND()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CCustomTTWnd message handlers

void CCustomTTWnd::OnPaint() 
{
	CPaintDC DC(this);
    int nSavedDC = DC.SaveDC();    
	
	CRect clientRect;
	GetClientRect(clientRect);

	// draw the bubble
    m_pTransbmpBubble->DrawTransStretched( 
				DC.m_hDC, 0, 0,
				0, 0, m_rcDisplayRect.Width(),m_rcDisplayRect.Height());

	ASSERT_VALID(m_pTipFont);
	DC.SelectObject(m_pTipFont);
    
	// Draw text of item 
	ASSERT(!m_strTip.IsEmpty());
	CRect myTextRect = m_textRect;
	myTextRect.DeflateRect(m_rcMargin);
	DC.SetBkMode(TRANSPARENT);
	DC.SetTextColor(m_crTextColor);
	DC.DrawText(m_strTip,myTextRect, DT_WORDBREAK);

	DC.RestoreDC(nSavedDC);

	// now, set a timer for it to hide
	m_nHideTimerId = SetTimer(TM_HIDETIP, m_nHideDelay, NULL);
	ASSERT(m_nHideTimerId != 0);


}

void CCustomTTWnd::OnTimer(UINT nIDEvent) 
{
	if (nIDEvent == m_nTimerId)
	{
		KillTimer(nIDEvent);
		m_nTimerId = 0;

		ASSERT(::IsWindow(m_hWnd));

		// determind if the mouse is still in the control
		// when we handle this timer message
		CRect ctrlRect;
		ASSERT_VALID(m_pCurTipObj);
		CWnd* pCtrlWnd = m_pCurTipObj->GetTipCtrlWnd();
		pCtrlWnd->GetWindowRect(ctrlRect);
		// get the current mouse position
		CPoint curPos;
		VERIFY(GetCursorPos(&curPos));
		// if not, don't do anything, just return
		if (!ctrlRect.PtInRect(curPos))
			return;

		ASSERT(!m_rcDisplayRect.IsRectEmpty());   // Make sure there is a rect
		Invalidate();										// Invalidate the client rect

		// move the window around
		int nSWPFlags = SWP_SHOWWINDOW | SWP_NOACTIVATE;
		VERIFY(SetWindowPos(&wndTopMost, m_rcDisplayRect.left, m_rcDisplayRect.top,
							m_bmpSize.cx, m_bmpSize.cy, nSWPFlags));

		
	}
	else if (nIDEvent == m_nHideTimerId)
	{
		KillTimer(nIDEvent);
		m_nHideTimerId = 0;

		Hide();
	}

	CWnd::OnTimer(nIDEvent);
}

BOOL CCustomTTWnd::OnEraseBkgnd(CDC* pDC) 
{
	return TRUE;//CWnd::OnEraseBkgnd(pDC);
}
