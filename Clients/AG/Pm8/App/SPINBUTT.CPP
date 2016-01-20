/*
// $Workfile: SPINBUTT.CPP $
// $Revision: 1 $
// $Date: 3/03/99 6:11p $
*/

/*
// Revision History:
//
// $Log: /PM8/App/SPINBUTT.CPP $
// 
// 1     3/03/99 6:11p Gbeddow
// 
//    Rev 1.0   14 Aug 1997 15:25:54   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:40:32   Fred
// Initial revision.
// 
//    Rev 1.2   22 Oct 1996 15:26:48   johno
// Added CS_GLOBALCLASS to style (RegisterMyClass)
// 
//    Rev 1.1   31 Jul 1996 11:57:02   Fred
// 16-bit compatibility
// 
//    Rev 1.0   14 Mar 1996 13:34:58   Jay
// Initial revision.
// 
//    Rev 1.0   16 Feb 1996 12:31:10   FRED
// Initial revision.
// 
//    Rev 2.4   13 Oct 1995 14:20:04   FRED
// 32 bit changes
// 
//    Rev 2.3   06 Oct 1995 15:23:58   FRED
// Win32 SendMessage() changes
// 
//    Rev 2.2   27 Aug 1995 18:29:46   FRED
// Changes to make install program not reference OLE
// 
//    Rev 2.1   08 Feb 1995 13:35:10   JAY
// Reverted. New series.
// 
//    Rev 1.2   30 Nov 1994 16:59:24   JAY
//  
// 
//    Rev 1.1   04 Aug 1994 08:30:58   JAY
// Large model and 32-bit changes
// 
//    Rev 1.0   15 Jun 1994 09:22:50   FRED
// Initial revision.
*/ 

#include "stdafx.h"

#include "spinbutt.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSpinButton

WNDPROC CSpinButton::m_pfnSuperWndProc = NULL;

LONG CALLBACK AFX_EXPORT CSpinButton::FirstMsgOnlyWndProc(HWND hWnd, UINT msg, UINT wParam, LONG lParam)
{
	/*
	// Construct the object and then attach the hWnd to it.
	*/
	
	CSpinButton* pSpinButton = new CSpinButton;
	pSpinButton->Attach(hWnd) ;
	
	pSpinButton->m_fCreatedByClassName = TRUE ;
	
	/*
	// Subclass the window by changing the window procedure to AfxWndProc
	// and pass the current message to it.
	*/
	
	::SetWindowLong(hWnd, GWL_WNDPROC, (DWORD)AfxWndProc);

#ifdef STRICT
	return ::CallWindowProc((WNDPROC)AfxWndProc, hWnd, msg, wParam, lParam);
#else
	return ::CallWindowProc((FARPROC)AfxWndProc, hWnd, msg, wParam, lParam);
#endif
}

BOOL CSpinButton::RegisterMyClass()
{
	WNDCLASS wcls;
	
	m_pfnSuperWndProc = ::DefWindowProc;
	
	static const char szClassName[] = "MlsSpinButton";
	
	if (::GetClassInfo(AfxGetInstanceHandle(), szClassName, &wcls))
	{
		return (wcls.lpfnWndProc == CSpinButton::FirstMsgOnlyWndProc);
	}
	
	// JRO
  // wcls.style = 0;
  wcls.style = CS_GLOBALCLASS;
	wcls.cbClsExtra = 0;
	wcls.cbWndExtra = 0;
	wcls.lpfnWndProc = CSpinButton::FirstMsgOnlyWndProc;
	wcls.hIcon = NULL;
	wcls.hCursor = ::LoadCursor(NULL, IDC_ARROW);
	wcls.hInstance = AfxGetInstanceHandle();
	wcls.hbrBackground = (HBRUSH)(COLOR_BACKGROUND+1);
	wcls.lpszMenuName = NULL;
	wcls.lpszClassName = szClassName;
	
	return (RegisterClass(&wcls) != 0);
}

void CSpinButton::PostNcDestroy()
{
	if (m_fCreatedByClassName)
	{
		delete this;
	}
}

CSpinButton::CSpinButton()
{
	m_fCreatedByClassName = FALSE;
}

CSpinButton::~CSpinButton()
{
}

BEGIN_MESSAGE_MAP(CSpinButton, CWnd)
	//{{AFX_MSG_MAP(CSpinButton)
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
	ON_WM_CREATE()
	ON_WM_ENABLE()
	ON_WM_SHOWWINDOW()
	ON_WM_CANCELMODE()
	ON_WM_TIMER()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_LBUTTONDOWN()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONUP()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSpinButton message handlers


void CSpinButton::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	
	WORD wSystemColor[CCOLORS] =
	{
		COLOR_BTNFACE,
		COLOR_BTNTEXT,
		COLOR_BTNSHADOW,
#ifdef WIN32
		COLOR_BTNHILIGHT,
#else
		COLOR_BTNHIGHLIGHT,
#endif
		COLOR_WINDOWFRAME
	};

	/*
	// Get colors that we'll need.  We do not want to cache these
	// items since we may our top-level parent window may have
	// received a WM_WININICHANGE message at which time the control
	// is repainted.  Since this control never sees that message,
	// we cannot assume that colors will remain the same throughout
	// the life of the control.
	//
	// We use the system color if m_crColor[i] is -1, otherwise we
	// use the color in m_crColor[i].
	*/
	
	COLORREF crColor[CCOLORS];
	CPen cpPen[CCOLORS];
    
	for (int i = 0; i < CCOLORS; i++)
	{
		if (m_crColor[i] == (COLORREF)-1)
		{
			crColor[i] = GetSysColor(wSystemColor[i]);
		}
		
		else
		{
		   crColor[i] = m_crColor[i];
		}
			
		cpPen[i].CreatePen(PS_SOLID, 1, crColor[i]);
	}
	
	CBrush cbArrow;
	CBrush cbFace;

	cbFace.CreateSolidBrush(crColor[SBCOLOR_FACE]);
	cbArrow.CreateSolidBrush(crColor[SBCOLOR_ARROW]);
	
	/*
	// These values are extremely cheap to calculate for the amount
	// we are going to use them.
	*/
	
	CRect crClient;
	GetClientRect(&crClient);

	int cx,  cy;    // whole dimensions
	int cx2, cy2;   // half dimensions
	int cx4, cy4;   // quarter dimensions
	
	cx  = crClient.Width();
	cy  = crClient.Height();
	cx2 = cx/2;
	cy2 = cy/2;
	cx4 = cx2/2;
	cy4 = cy2/2;
	
	/*
	// If one half is depressed, set the x/yAdd varaibles that we use
	// to shift the small arrow image down and right.
	*/
	
	int xAdd1 = 0, yAdd1 = 0;
	int xAdd2 = 0, yAdd2 = 0;

	if (!StateTest(STATE_mouseout))
	{
		if (StateTest(STATE_upclick | STATE_leftclick))
		{
			xAdd1 = 1;
			yAdd1 = 1;
		}
	
		if (StateTest(STATE_downclick | STATE_rightclick))
		{
			xAdd2 = 1;
			yAdd2 = 1;
		}
	}
	
	int nContext = dc.SaveDC();
	
	// draw the face color and the outer frame
	dc.SelectObject(&cbFace);
	dc.SelectObject(&cpPen[SBCOLOR_FRAME]);
	dc.Rectangle(&crClient);
	
	// draw the arrows depending on the orientation
	if ((SBS_VERTICAL & m_dwStyle) != 0)
	{
		// draw the horizontal center line
		dc.MoveTo(0,  cy2);
		dc.LineTo(cx, cy2);
	
		/*
		// We do one of three modifications for drawing the borders:
		//  1) Both halves un-clicked.
		//  2) Top clicked,   bottom unclicked.
		//  3) Top unclicked, bottom clicked.
		//
		// Case 1 is xAdd1==xAdd2==0
		// Case 2 is xAdd1==1, xAdd2=0
		// Case 3 is xAdd1==0, xAdd2==1
		//
		*/
		
		// draw top and bottom buttons borders
		Draw3DButtonRect(&dc, &cpPen[SBCOLOR_HIGHLIGHT], &cpPen[SBCOLOR_SHADOW], 0, 0,   cx-1, cy2,  xAdd1);
		Draw3DButtonRect(&dc, &cpPen[SBCOLOR_HIGHLIGHT], &cpPen[SBCOLOR_SHADOW], 0, cy2, cx-1, cy-1, xAdd2);
			
		// select default line color
		dc.SelectObject(&cpPen[SBCOLOR_ARROW]);
	
		// draw the arrows depending on the enable state
		if (StateTest(STATE_grayed))
		{
			/*
			// Draw arrow color lines in the upper left of the
			// top arrow and on the top of the bottom arrow.
			// Pen was already selected as a default.
			*/
			
			dc.MoveTo(cx2,   cy4-2);      // top arrow
			dc.LineTo(cx2-3, cy4+1);
			dc.MoveTo(cx2-3, cy2+cy4-2);  // bottom arrow
			dc.LineTo(cx2+3, cy2+cy4-2);
				
			/*
			// Draw highlight color lines in the bottom of the
			// top arrow and on the upper right of the bottom arrow.
			*/
			
			dc.SelectObject(&cpPen[SBCOLOR_HIGHLIGHT]);
			dc.MoveTo(  cx2-3, cy4+1);      // top arrow
			dc.LineTo(  cx2+3, cy4+1);
			dc.MoveTo(  cx2+3, cy2+cy4-2);  // bottom arrow
			dc.LineTo(  cx2,   cy2+cy4+1);
			dc.SetPixel(cx2,   cy2+cy4+1, crColor[SBCOLOR_HIGHLIGHT]);
		}
		
		else
		{
			POINT rgpt1[3];
			POINT rgpt2[3];

			// top arrow polygon
			rgpt1[0].x = xAdd1+cx2;
			rgpt1[0].y = yAdd1+cy4-2;
			rgpt1[1].x = xAdd1+cx2-3;
			rgpt1[1].y = yAdd1+cy4+1;
			rgpt1[2].x = xAdd1+cx2+3;
			rgpt1[2].y = yAdd1+cy4+1;
				
			// bottom arrow polygon
			rgpt2[0].x = xAdd2+cx2;
			rgpt2[0].y = yAdd2+cy2+cy4+1;
			rgpt2[1].x = xAdd2+cx2-3;
			rgpt2[1].y = yAdd2+cy2+cy4-2;
			rgpt2[2].x = xAdd2+cx2+3;
			rgpt2[2].y = yAdd2+cy2+cy4-2;
				
			// draw the arrows
			dc.SelectObject(&cbArrow);
			dc.Polygon((LPPOINT)rgpt1, 3);
			dc.Polygon((LPPOINT)rgpt2, 3);
		}
	}
	
	else
	{
		// draw the vertical center line, assume the frame color is selected.
		dc.MoveTo(cx2, 0);
		dc.LineTo(cx2, cy);
				
		/*
		// We do one of three modifications for drawing the borders:
		//  1) Both halves un-clicked.
		//  2) Left clicked,   right unclicked.
		//  3) Left unclicked, right clicked.
		//
		// Case 1 is xAdd1==xAdd2==0
		// Case 2 is xAdd1==1, xAdd2=0
		// Case 3 is xAdd1==0, xAdd2==1
		//
		*/
				
		// draw left and right buttons borders
		Draw3DButtonRect(&dc, &cpPen[SBCOLOR_HIGHLIGHT], &cpPen[SBCOLOR_SHADOW], 0,   0, cx2,  cy-1, xAdd1);
		Draw3DButtonRect(&dc, &cpPen[SBCOLOR_HIGHLIGHT], &cpPen[SBCOLOR_SHADOW], cx2, 0, cx-1, cy-1, xAdd2);
				
		// select default line color
		dc.SelectObject(&cpPen[SBCOLOR_ARROW]);
			
		// draw the arrows depending on the enable state
		if (StateTest(STATE_grayed))
		{
			/*
			// Draw arrow color lines in the upper left of the
			// left arrow and on the left of the right arrow.
			// Pen was already selected as a default.
			*/
			
			dc.MoveTo(cx4-2,     cy2);        // left arrow
			dc.LineTo(cx4+1,     cy2-3);
			dc.MoveTo(cx2+cx4-2, cy2-3);      // right arrow
			dc.LineTo(cx2+cx4-2, cy2+3);
				
			/*
			// Draw highlight color lines in the bottom of the
			// top arrow and on the ;pwer right of the bottom arrow.
			*/
			
			dc.SelectObject(&cpPen[SBCOLOR_HIGHLIGHT]);
			dc.MoveTo(cx4+1,     cy2-3);
			dc.LineTo(cx4+1,     cy2+3);
			dc.MoveTo(cx2+cx4+1, cy2);
			dc.LineTo(cx2+cx4-2, cy2+3);
		}
		
		else
		{
			POINT rgpt1[3];
			POINT rgpt2[3];

			// left arrow polygon
			rgpt1[0].x = xAdd1+cx4-2;
			rgpt1[0].y = yAdd1+cy2;
			rgpt1[1].x = xAdd1+cx4+1;
			rgpt1[1].y = yAdd1+cy2+3;
			rgpt1[2].x = xAdd1+cx4+1;
			rgpt1[2].y = yAdd1+cy2-3;
				
			// right arrow polygon
			rgpt2[0].x = xAdd2+cx2+cx4+1;
			rgpt2[0].y = yAdd2+cy2;
			rgpt2[1].x = xAdd2+cx2+cx4-2;
			rgpt2[1].y = yAdd2+cy2+3;
			rgpt2[2].x = xAdd2+cx2+cx4-2;
			rgpt2[2].y = yAdd2+cy2-3;
				
			// draw the arrows
			dc.SelectObject(&cbArrow);
			dc.Polygon((LPPOINT)rgpt1, 3);
			dc.Polygon((LPPOINT)rgpt2, 3);
		}
	}
	
	dc.RestoreDC(nContext);
}

/*
// Draw3DButtonRect
//
// Purpose:
//  Draws the 3D button look within a given rectangle.  This rectangle
//  is assumed to be bounded by a one pixel black border, so everything
//  is bumped in by one.
//
// Parameters:
//  pcdcDraw     CDC* to draw to
//  pcpHighlight CPen* for highlight color
//  pcpShadow    CPen* for shadow color
//  x1           int upper left corner x
//  y1           int upper left corner y
//  x2           int lower right corner x
//  y2           int lower right corner y
//  fClicked     BOOL specifies if the button is down or not (TRUE==DOWN)
*/

void CSpinButton::Draw3DButtonRect(CDC *pcdcDraw, CPen *pcpHighlight, CPen *pcpShadow, int x1, int y1, int x2, int y2, BOOL fClicked)
{
	// shrink the rectangle to account for borders
	x1 += 1;
	x2 -= 1;
	y1 += 1;
	y2 -= 1;
	
	CPen *pcpOld = pcdcDraw->SelectObject(pcpShadow);
	
	if (fClicked)
	{
		// shadow on left and top edge when clicked
		pcdcDraw->MoveTo(x1, y2);
		pcdcDraw->LineTo(x1, y1);
		pcdcDraw->LineTo(x2+1, y1);
	}
	else
	{
		// lowest shadow line
		pcdcDraw->MoveTo(x1, y2);
		pcdcDraw->LineTo(x2, y2);
		pcdcDraw->LineTo(x2, y1-1);
		
		// upper shadow line
		pcdcDraw->MoveTo(x1+1, y2-1);
		pcdcDraw->LineTo(x2-1, y2-1);
		pcdcDraw->LineTo(x2-1, y1);
		
		pcdcDraw->SelectObject(pcpHighlight);
		
		// upper highlight line
		pcdcDraw->MoveTo(x1, y2-1);
		pcdcDraw->LineTo(x1, y1);
		pcdcDraw->LineTo(x2, y1);
	}

	pcdcDraw->SelectObject(pcpOld);
}
    
BOOL CSpinButton::OnEraseBkgnd(CDC* pDC)
{
	return TRUE;
}

int CSpinButton::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
	{
		return -1;
	}
	
	// by default, associate with our parent
	m_pcwAssociate = GetParent();
	
	// copy style DWORD
	m_dwStyle = lpCreateStruct->style;
	
	/*
	// Enforce exclusive SBS_VERTICAL and SBS_HORIZONTAL,
	// defaulting to SBS_VERTICAL.
	*/
	
	if (((SBS_VERTICAL & m_dwStyle) != 0) && ((SBS_HORIZONTAL & m_dwStyle) != 0))
	{
		m_dwStyle &= ~SBS_HORIZONTAL;
	}
	
	// either parse the text or use defaults for initial range
	int nMin, nMax, nPos;
	BOOL fTextRange = FALSE;
	
	if ((SBS_TEXTHASRANGE & m_dwStyle) != 0)
	{
		fTextRange = ParseText((LPSTR)lpCreateStruct->lpszName, &nMin, &nMax, &nPos);
	
		/*
		// Verify that the position is in the given range.  If
		// the position is outside the range, force it to the
		// middle.
		*/

		if (fTextRange)
		{		
			if ((nPos < nMin) || (nPos > nMax))
			{
				nPos = (nMin+nMax)/2;
			}
		}
	}
	
	/*
	// Use defaults if we never had SBS_TEXTHASRANGE or
	// ParseText failed.
	*/
	m_nMin = (fTextRange ? nMin : DEFAULT_MIN);
	m_nMax = (fTextRange ? nMax : DEFAULT_MAX);
	m_nPos = (fTextRange ? nPos : DEFAULT_POS);
	
	// clear out all initial states
	StateClear(STATE_all);
	
	// initialize colors to use system defaults
	for (int i = 0; i < CCOLORS; i++)
	{
		m_crColor[i] = (COLORREF)-1L;
	}
	
	return 0;
}

/*
// ParseText
//
// Purpose:
//  Parses window text for a valid range and initial position.
//  This function is used when creating the control or setting the
//  window text to set the initial range and position but is also
//  used to validate text entered in the Style dialog in the Dialog
//  Editor interface if the SBS_TEXTHASRANGE style is selected.
//
//  The range and position must all be valid for any change to
//  occur in pnMin, pnMax, and pnPos.
//
// Parameters:
//  lpszText        LPSTR pointer to the window text to parse out
//                  the range and the position.
//  lpnMin          LPINT pointer to location to store minimum.
//  lpnMax          LPINT pointer to location to store maximum.
//  lpnPos          LPINT pointer to location to store position.
//
// Return Value:
//  BOOL            TRUE if the function successfully initializes
//                  the range and position. FALSE if any part of
//                  the text is not a valid number between comma
//                  delimeters.
*/

BOOL CSpinButton::ParseText(LPSTR lpszText, LPINT pnMin, LPINT pnMax, LPINT pnPos)
{
	int nMin;
	int nMax;
	int nPos;

	if (lpszText == NULL)
	{
		return FALSE;
	}

	/*
	// Parse off the bottom of the range.  Note that we depend
	// on TranslateUpToChar to modify lpszText to point to the character
	// after the delimeter which is why we pass &lpszText.
	*/
	
	nMin = TranslateUpToChar(&lpszText, ',');

	// check for valid value AND that there's text remaining
	if ((nMin == -1) || (*lpszText == '\0'))
	{
		return FALSE;
	}

	// parse off the top of the range.
	nMax = TranslateUpToChar(&lpszText, ',');

	// check for valid value AND that there's text remaining
	if ((nMax == -1) || (*lpszText == '\0'))
	{
		return FALSE;
	}

	// parse off the position and validate it
	nPos = TranslateUpToChar(&lpszText, ',');
	
	if (nPos == -1)
	{
		return FALSE;
	}

	// store the parsed values in the return locations
	*pnMin = nMin;
	*pnMax = nMax;
	*pnPos = nPos;

	return TRUE;
}

/*
// TranslateUpToChar
//
// Purpose:
//  Scans a string for digits, converting the series of digits to
//  an integer value as the digits are scanned.  Scanning stops
//  at chDelimeter or the end of the string.
//
//  If the scan sees a non-digit character, -1 is returned to indicate
//  error.  If the scan sees a null-terminator before any text, we
//  return 0.
//
// Parameters:
//  lplpszText      LPSTR * pointer to pointer to the string to scan.
//                  On return, the pointer will point to the character
//                  after the delimeter OR the NULL terminator.
//
//                  We want a pointer to the pointer so we can modify
//                  that pointer for the calling function since we are
//                  using the return value for the parsed value
//
//  cDelimiter      char delimeter at which the scanning stops.
//
// Return Value:
//  WORD            -1 if the string contains non-digits excluding
//                  the comma.  Otherwise the converted value is
//                  returned and the pointer to the address after
//                  the comma is stored at lplpszText.
*/

WORD CSpinButton::TranslateUpToChar(LPSTR FAR *lplpszText, char cDelimeter)
{
	WORD wResult = 0;
	LPSTR lpszText = *lplpszText;

	// fail if there's no more string to parse
	if (*lpszText == '\0')
	{
		return ((WORD)-1);
	}

	// scan string, stopping at 0 or cDelimeter
	char ch;
	
	while ((ch = *lpszText) != '\0')
	{
		if (ch == cDelimeter)
		{
			lpszText++;	// skip delimeter
			break;
		}
	
		// check for digits, returning -1 on a non-digit
		ch -= '0';
		
		if ((ch < 0) || (ch > 9))
		{
			return ((WORD)-1);
		}
		
		// Add up the value as we scan
		wResult = (wResult*10) + (WORD)ch;
		
		/*
		// We increment lpszText here so if we hit a null-terminator
		// lpszText is always valid.  If we incremented in the while
		// statement then lpszText might be past the null-terminator
		// and possibly invalid.
		*/
		
		lpszText++;
	}

	/*
	// Store the new pointer and the value.  Note that the *lpszText++
	// already incremented lpszText past the delimeter AND the zero.
	*/
	
	*lplpszText = lpszText;
	
	return wResult;
}
    
void CSpinButton::OnEnable(BOOL bEnable)
{
	CWnd::OnEnable(bEnable);
	
	/*
	// Handles disabling/enabling case.  Example of a
	// change-state-and-repaint strategy since we let the
	// painting code take care of the visuals.
	*/
	
	if (bEnable)
	{
		StateClear(STATE_grayed);
	}
	
	else
	{
		StateSet(STATE_grayed);
	}
	
	// force a repaint since the control will look different
	InvalidateRect(NULL);
	UpdateWindow();
}

void CSpinButton::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CWnd::OnShowWindow(bShow, nStatus);
	
	/*
	// Set or clear the hidden flag. Windows will
	// automatically force a repaint if we become visible.
	*/
   
	if (bShow)
	{
		StateClear(STATE_hidden);
	}
	
	else
	{
		StateSet(STATE_hidden);
	}
}

void CSpinButton::OnCancelMode()
{
	CWnd::OnCancelMode();
	
	/*
	// IMPORTANT MESSAGE!  WM_CANCELMODE means that a
	// dialog or some other modal process has started.
	// we must make sure that we cancel any clicked state
	// we are in, kill the timers, and release the capture.
	*/
   
	StateClear(STATE_downclick | STATE_upclick);
	KillTimer(IDT_FIRSTCLICK);
	KillTimer(IDT_HOLDCLICK);
	ReleaseCapture();
}

void CSpinButton::OnTimer(UINT nIDEvent)
{
	/*
	// We run two timers:  the first is the initial delay
	// after the first click before we begin repeating, the
	// second is the repeat rate.
	*/
	
	if (nIDEvent == IDT_FIRSTCLICK)
	{
		KillTimer(nIDEvent);
		SetTimer(IDT_HOLDCLICK, CTICKS_HOLDCLICK, NULL);
	}

	/*
	// Send a new scroll message if the mouse is still in the
	// originally clicked area.
	*/
	
	if (!StateTest(STATE_mouseout))
	{
		PositionChange();
	}
	
	CWnd::OnTimer(nIDEvent);
}

void CSpinButton::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	OnLButtonDown(nFlags, point);
	CWnd::OnLButtonDblClk(nFlags, point);
}

void CSpinButton::OnLButtonDown(UINT nFlags, CPoint point)
{
	/*
	// When we get a mouse down message, we know that the mouse
	// is over the control.  We then do the following steps
	// to set up the new state:
	//  1.  Hit-test the coordinates of the click to
	//      determine in which half the click occurred.
	//  2.  Set the appropriate STATE_*CLICK state
	//      and repaint that clicked half.  This is another
	//      example of a change-state-and-repaint strategy.
	//  3.  Send an initial scroll message.
	//  4.  Set the mouse capture.
	//  5.  Set the initial delay timer before repeating
	//      the scroll message.
	//
	// A WM_LBUTTONDBLCLK message means that the user clicked
	// the control twice in succession which we want to treat
	// like WM_LBUTTONDOWN.  This is safe since we will receive
	// WM_LBUTTONUP before the WM_LBUTTONDBLCLK.
	*/

	/*
	// Determine the type of click.
	//
	// Only need to hit-test the upper half for a vertical
	// control or the left half for a horizontal control.
	*/
	
	CRect crClient;
   GetClientRect(&crClient);
   
	WORD wClick;
	
	if ((SBS_VERTICAL & m_dwStyle) != 0)
	{
		wClick = (point.y > (crClient.bottom/2)) ? STATE_downclick : STATE_upclick;
	}
	
	else
	{
		wClick = (point.x > (crClient.right/2)) ? STATE_rightclick : STATE_leftclick;
	}

   // change-state-and-repaint
   
   StateSet(wClick);
   ClickedRectCalc(crClient);
   InvalidateRect(&crClient);
   UpdateWindow();

   PositionChange();
   SetCapture();
   SetTimer(IDT_FIRSTCLICK, CTICKS_FIRSTCLICK, NULL);
	
	CWnd::OnLButtonDown(nFlags, point);                                               
}

void CSpinButton::OnMouseMove(UINT nFlags, CPoint point)
{
	/*
	// On WM_MOUSEMOVE messages we want to know if the mouse
	// has moved out of the control when the control is in
	// a clicked state.  If the control has not been clicked,
	// then we have nothing to do.  Otherwise we want to set
	// the STATE_MOUSEOUT flag and repaint so the button
	// visually comes up.
	*/
	
	if (StateTest(STATE_clicked))
	{
		// Get the area we originally clicked and the new POINT
		CRect crRect;
		ClickedRectCalc(crRect);
		
		WORD wPreviousState = m_wState;
		
		// hit-test the rectangle and change the state if necessary
		if (crRect.PtInRect(point))
		{
			StateClear(STATE_mouseout);
		}
		
		else
		{
			StateSet(STATE_mouseout);
		}
		
		/*
		// If the state changed, repaint the appropriate part of
		// the control.
		*/
		
		if (m_wState != wPreviousState)
		{
			InvalidateRect(&crRect);
			UpdateWindow();
		}
	}
	
	CWnd::OnMouseMove(nFlags, point);
}

void CSpinButton::OnLButtonUp(UINT nFlags, CPoint point)
{
	/*
	// A mouse button up event is much like WM_CANCELMODE since
	// we have to clean out whatever state the control is in:
	//  1.  Kill any repeat timers we might have created.
	//  2.  Release the mouse capture.
	//  3.  Clear the clicked states and repaint, another example
	//      of a change-state-and-repaint strategy.
	*/
	
	KillTimer(IDT_FIRSTCLICK);
	KillTimer(IDT_HOLDCLICK);
	
	ReleaseCapture();
	
	/*
	// Repaint if necessary, only if we are clicked AND the mouse
	// is still in the boundaries of the control.
	*/
	
	if (StateTest(STATE_clicked) && StateTest((WORD)(~STATE_mouseout)))
	{
		// Calculate the rectangle before clearing states
		CRect crRect;
		ClickedRectCalc(crRect);
			
		// Clear the states so we repaint properly
		StateClear(STATE_mouseout);
		StateClear(STATE_clicked);
			
		InvalidateRect(&crRect);
		UpdateWindow();
	}
	
	CWnd::OnLButtonUp(nFlags, point);
}

/*
// ClickedRectCalc
//
// Purpose:
//  Calculates the rectangle of the clicked region based on the
//  state flags STATE_upclick, STATE_downclick, STATE_leftclick,
//  and STATE_rightclick, depending on the style.
//
// Parameter:
//  crRect          CRect structure to fill.
*/

void CSpinButton::ClickedRectCalc(CRect &crRect)
{
	GetClientRect(&crRect);
	
	int cx = crRect.right/2;
	int cy = crRect.bottom/2;

	if ((SBS_VERTICAL & m_dwStyle) != 0)
	{
		// vertical
		if (StateTest(STATE_downclick))
		{
			crRect.top = cy;
		}

		if (StateTest(STATE_upclick))
		{
			crRect.bottom = 1+cy;
		}
	}
	
	else
	{
		// horizontal
		if (StateTest(STATE_rightclick))
		{
			crRect.left = cx;
		}

		if (StateTest(STATE_leftclick))
		{
			crRect.right = 1+cx;
		}
	}
}
    
/*
// PositionChange
//
// Purpose:
//  Checks what part of the control is clicked, modifies the current
//  position accordingly (taking SBS_INVERTRANGE into account) and
//  sends an appropriate message to the associate.  For SBS_VERTICAL
//  controls we send WM_VSCROLL messages and for SBS_HORIZONTAL controls
//  we send WM_HSCROLL.
//
//  The scroll code in the message is always SB_LINEUP for the upper
//  or left half of the control (vertical and horizontal, respectively)
//  and SB_LINEDOWN for the bottom or right half.
//
//  This function does not send a message if the position is pegged
//  on the minimum or maximum of the range if SBS_NOPEGSCROLL is
//  set in the style bits.
*/

void CSpinButton::PositionChange(void)
{
	WORD wScrollCode;
	
	if (StateTest(STATE_upclick | STATE_leftclick))
	{
		wScrollCode = SB_LINEUP;
	}

	if (StateTest(STATE_downclick | STATE_rightclick))
	{
		wScrollCode = SB_LINEDOWN;
	}

	WORD wScrollMsg = ((SBS_VERTICAL & m_dwStyle) != 0) ? WM_VSCROLL : WM_HSCROLL;

	/*
	// Modify the current position according to the following rules:
	//
	// 1. On SB_LINEUP with an inverted range, increment the position.
	//    If the position is already at the maximum, set the pegged flag.
	//
	// 2. On SB_LINEUP with an normal range, decrement the position.
	//    If the position is already at the minimum, set the pegged flag.
	//
	// 3. On SB_LINEDOWN with an inverted range, treat like SB_LINEUP
	//    with a normal range.
	//
	// 4. On SB_LINEDOWN with an normal range, treat like SB_LINEUP
	//    with an inverted range.
	*/

	BOOL fPegged = FALSE;

	if (wScrollCode == SB_LINEUP)
	{
		if ((SBS_INVERTRANGE & m_dwStyle) != 0)
		{
			if (m_nPos == m_nMax)
			{
				fPegged = TRUE;
			}
			
			else
			{
				m_nPos++;
			}
		}
		
		else
		{
			if (m_nPos == m_nMin)
			{
				fPegged = TRUE;
			}
			
	   	else
	   	{
				m_nPos--;
			}
		}
	}
	
	else
	{
		if ((SBS_INVERTRANGE & m_dwStyle) != 0)
		{
			if (m_nPos == m_nMin)
			{
				fPegged = TRUE;
			}
			
			else
			{
				m_nPos--;
			}
		}
		
		else
		{
			if (m_nPos == m_nMax)
			{
				fPegged = TRUE;
			}
			
	   	else
	   	{
				m_nPos++;
			}
		}
	}

	/*
	// Send a message if we changed and are not pegged, or did not change
	// and SBS_NOPEGSCROLL is clear.
	*/
	
	if ((!fPegged) || ((SBS_NOPEGSCROLL & m_dwStyle) != 0))
	{
#ifdef WIN32
		m_pcwAssociate->SendMessage(wScrollMsg, MAKEWPARAM(wScrollCode, m_nPos), (LPARAM)m_hWnd);
#else
		m_pcwAssociate->SendMessage(wScrollMsg, wScrollCode, MAKELONG(m_nPos, m_hWnd));
#endif
	}
}
