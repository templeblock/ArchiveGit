#include "stdafx.h"
#include "CurveWindow.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define COLOR_BACK	RGB(255,255,255)
#define COLOR_GRID	RGB(192,192,192)

IMPLEMENT_DYNCREATE(CCurveWindow, CView)

BEGIN_MESSAGE_MAP(CCurveWindow, CWnd)
	//{{AFX_MSG_MAP(CCurveWindow)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_PAINT()
	ON_WM_KEYDOWN()
	ON_WM_SETFOCUS()
	ON_WM_KILLFOCUS()
	ON_WM_GETDLGCODE()
	ON_WM_CREATE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
CCurveWindow::CCurveWindow(BOOL colorImage) :
	CyanCurve(0xC0C000),
	MagentaCurve(0xC000C0),
	YellowCurve(0x00C0C0),
	GammaCurve(0x404040)
{
	Curves[CI_CYAN]    = &CyanCurve;
	Curves[CI_MAGENTA] = &MagentaCurve;
	Curves[CI_YELLOW]  = &YellowCurve;
	Curves[CI_GAMMA]   = &GammaCurve;
	nCurves = 4;

	ShowCurve(CI_CYAN,    FALSE);
	ShowCurve(CI_MAGENTA, FALSE);
	ShowCurve(CI_YELLOW,  FALSE);
	ShowCurve(CI_GAMMA,   FALSE);

	hButtonDownDC = NULL;
	InBox = NULL;
	OutBox = NULL;	
	bPercent = TRUE;
	bEnableEdits = TRUE;
}


/////////////////////////////////////////////////////////////////////////////
CCurveWindow::~CCurveWindow()
{
}


/////////////////////////////////////////////////////////////////////////////
BOOL CCurveWindow::PreCreateWindow(CREATESTRUCT& cs)
{
	cs.dwExStyle=cs.dwExStyle | WS_EX_CLIENTEDGE;	
	return CWnd::PreCreateWindow(cs);
}


/////////////////////////////////////////////////////////////////////////////
void CCurveWindow::SetPercent(BOOL bNewPercent)
{
	bPercent = bNewPercent;
}


/////////////////////////////////////////////////////////////////////////////
void CCurveWindow::SetEnableEdits(BOOL bNewEnableEdits)
{
	bEnableEdits = bNewEnableEdits;
}


/////////////////////////////////////////////////////////////////////////////
void CCurveWindow::OnDraw(CDC* pDC)
{
	if (pDC)
		Draw( pDC->m_hDC );
}


/////////////////////////////////////////////////////////////////////////////
void CCurveWindow::Draw(HDC hDC)
{
	SetSpace(hDC);
	DrawGrid(hDC);
	BOOL bHasFocus = (GetFocus() == this);
	for (int i = 0; i < nCurves; i++)
	{
		if (bCurveShown[i])
			Curves[i]->Draw(hDC, bHasFocus/*bShowSelection*/, bHasFocus && bEnableEdits/*bWantHandles*/);
	}
}


/////////////////////////////////////////////////////////////////////////////
void CCurveWindow::DrawGrid(HDC hDC)
{
	CCurvePen Pen(0/*width*/, R2_COPYPEN, COLOR_GRID);
	HPEN oldPen = Pen.SetInto(hDC);

	CPoint pTop( 0, 0 );
	CPoint pBottom( 0, nSCALE );

	// Draw the vertical lines
	for (int i = 0; i <= 10; i++)
	{
		pTop.x = pBottom.x = ((i * (nSCALE-1)) + 5) / 10;

		POINT pt;
		MoveToEx(hDC, pTop.x, pTop.y, &pt);
		LineTo(hDC, pBottom.x, pBottom.y);
	}
	
	CPoint pLeft( 0, 0 );
	CPoint pRight( nSCALE, 0 );

	// Draw the horizontal lines
	for (i = 0; i <= 10; i++)
	{
		pLeft.y = pRight.y = ((i * (nSCALE-1)) + 5) / 10;

		POINT pt;
		MoveToEx(hDC, pLeft.x, pLeft.y, &pt);
		LineTo(hDC, pRight.x, pRight.y);
	}

	SelectObject(hDC, oldPen);	
}


/////////////////////////////////////////////////////////////////////////////
void CCurveWindow::OnSetFocus(CWnd* pOldWnd) 
{
	CWnd::OnSetFocus(pOldWnd);

	Invalidate();
	UpdateWindow();
}

/////////////////////////////////////////////////////////////////////////////
void CCurveWindow::OnKillFocus(CWnd* pNewWnd)
{
	CWnd::OnKillFocus(pNewWnd);
	
	if (hButtonDownDC)
	{
		ReleaseCapture();
		::ReleaseDC(m_hWnd, hButtonDownDC);
		hButtonDownDC = NULL;
	}

	Invalidate();
	UpdateWindow();
}


/////////////////////////////////////////////////////////////////////////////
void CCurveWindow::OnLButtonDown(UINT nFlags, CPoint point) 
{
	if (GetFocus() != this)
	{
		SetFocus();
		return;
	}

	if (hButtonDownDC)
		return;

	ToLogicalSpace(point,point);
	
	// Look at the selected curves first
	int i;
	for (i = 0; i < nCurves; i++)
	{
		if (!bCurveShown[i])
			continue;
		POINT pointNew;
		if (Curves[i]->Selected() && Curves[i]->CurveWasClicked(point, pointNew))
			break;
	}

	if (i == nCurves) // no selected curve was clicked
	{ // Now look at the un-selected curves
		for (i = 0; i < nCurves; i++)
		{
			if (!bCurveShown[i])
				continue;
			POINT pointNew;
			if (!Curves[i]->Selected() && Curves[i]->CurveWasClicked(point, pointNew))
				break;
		}

		// Unselect all curves
		int nChanged = 0;
		nChanged += SelectCurve(CI_CYAN,    FALSE);
		nChanged += SelectCurve(CI_MAGENTA, FALSE);
		nChanged += SelectCurve(CI_YELLOW,  FALSE);
		nChanged += SelectCurve(CI_GAMMA,   FALSE);
	
		if (i == nCurves) // no curve was clicked; time to get out
		{
			if (nChanged && IsWindow(m_hWnd) && IsWindowVisible())
			{
				Invalidate();
				UpdateWindow();
				UpdateEditBoxes(point.x);
			}
			return;
		}

		// We clicked on an un-selected curve!
		// Select this curve and wait for the next button down to edit
		SelectCurve((CMYK)i, TRUE);

		if (IsWindow(m_hWnd) && IsWindowVisible())
		{
			Invalidate();
			UpdateWindow();
			UpdateEditBoxes(point.x);
		}
		return;
	}

	// We clicked on a selected curve!
	if (!bEnableEdits)
		return;

	// Start the editing process for the selected curve
	if (!(hButtonDownDC = ::GetDC(m_hWnd)))
		return;

	SetSpace(hButtonDownDC);
	SetCapture();

	for (i = 0; i < nCurves; i++)
	{
		if (Curves[i]->Selected())
		{
			Curves[i]->PrepareChange();

			if (IsWindow(m_hWnd) && IsWindowVisible())
			{
				Invalidate();
				UpdateWindow();
				UpdateEditBoxes(point.x);
			}

			Curves[i]->LeftDown(hButtonDownDC, point);

			// Send a message to the parent window
			NotifyParentofChange(i);
			NotifyParentofSelect(i);
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
void CCurveWindow::OnLButtonUp(UINT nFlags, CPoint point) 
{
	if (!hButtonDownDC)
		return;

	ToLogicalSpace(point,point);

	for (int i = 0; i < nCurves; i++)
	{
		if (Curves[i]->Selected())
		{
			Curves[i]->LeftUp(hButtonDownDC,point);
			Curves[i]->FinishChange();

			if (IsWindow(m_hWnd) && IsWindowVisible())
			{
				Invalidate();
				UpdateWindow();
				UpdateEditBoxes(point.x);
			}

			// Send a message to the parent window
			NotifyParentofChange(i);
			NotifyParentofSelect(i);
		}
	}

	ReleaseCapture();
	::ReleaseDC(m_hWnd, hButtonDownDC);
	hButtonDownDC = NULL;
}


/////////////////////////////////////////////////////////////////////////////
void CCurveWindow::OnMouseMove(UINT nFlags, CPoint point)
{
	ToLogicalSpace(point,point);
	
	if (!hButtonDownDC)
	{
		UpdateEditBoxes(point.x);
		return;
	}

	for (int i=0; i<nCurves; i++)
	{
		if (Curves[i]->Selected())
		{
			Curves[i]->LeftDrag(hButtonDownDC, point);

			// Send a message to the parent window
			NotifyParentofChange(i);
			NotifyParentofSelect(i);
		}
	}

	UpdateEditBoxes(point.x);
}


/////////////////////////////////////////////////////////////////////////////
void CCurveWindow::NotifyParentofSelect(int iCurve)
{
	if (iCurve < 0 || iCurve >= nCurves)
		return;
	
	// Send a message to the parent window
	POINT pt;
	int iCurrent = Curves[iCurve]->GetCurrent();
	if (!Curves[iCurve]->GetPoint(iCurrent, pt))
		return;
	WPARAM wParam = MAKELONG(iCurve, pt.x);
	LPARAM lParam = (LPARAM)FALSE;
	GetParent()->SendMessage(WM_USER, wParam, lParam);
	SetFocus();
}

/////////////////////////////////////////////////////////////////////////////
void CCurveWindow::NotifyParentofChange(int iCurve)
{
	if (iCurve < 0 || iCurve >= nCurves)
		return;
	
	// Send a message to the parent window
	int num = Curves[iCurve]->GetNumPoints();
	WPARAM wParam = MAKELONG(iCurve,num);
	LPARAM lParam = (LPARAM)TRUE;
	GetParent()->SendMessage(WM_USER, wParam, lParam);
	SetFocus();
}

/////////////////////////////////////////////////////////////////////////////
void CCurveWindow::OnPaint()
{	
	PAINTSTRUCT	ps;
	::BeginPaint(m_hWnd, &ps);
	Draw( ps.hdc );
	::EndPaint(m_hWnd, &ps);
}


/////////////////////////////////////////////////////////////////////////////
void CCurveWindow::OnArrowKey(UINT nChar) 
{
	int xIncr = 0;
	int yIncr = 0;

	if (nChar == VK_DOWN)
		yIncr = -(nSCALE/100);
	else
	if (nChar == VK_UP)
		yIncr = +(nSCALE/100);
	else
	if (nChar == VK_LEFT)
		xIncr = -(nSCALE/100);
	else
	if (nChar == VK_RIGHT)
		xIncr = +(nSCALE/100);

	if (!yIncr && !xIncr)
		return;

	for (int i = 0; i < nCurves; i++)
	{
		if (!Curves[i]->Selected())
			continue;

		int iCurrent = Curves[i]->GetCurrent();
		POINT pt;
		if (Curves[i]->GetPoint(iCurrent, pt))
		{
			pt.x += xIncr;
			pt.y += yIncr;
			Curves[i]->SetPoint(iCurrent, pt);

			if (IsWindow(m_hWnd) && IsWindowVisible())
			{
				Invalidate();
				UpdateWindow();
				UpdateEditBoxes(pt.x);
			}

			// Send a message to the parent window
			NotifyParentofChange(i);
			NotifyParentofSelect(i);
		}
	}
}

/////////////////////////////////////////////////////////////////////////////
void CCurveWindow::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	if (nChar == VK_DOWN || nChar == VK_UP || nChar == VK_LEFT || nChar == VK_RIGHT)
	{
		OnArrowKey(nChar);
	}
	else			
	if (nChar == VK_DELETE)
	{
		int nDeleted = 0;
		for (int i = 0; i < nCurves; i++)
		{
			if (Curves[i]->Selected())
			{
				int iCurrent = Curves[i]->GetCurrent();
				nDeleted += Curves[i]->DeletePoint(iCurrent);
				// Send a message to the parent window
				NotifyParentofChange(i);
			}
		}

		if (nDeleted)
		{
			Invalidate();
			UpdateWindow();
			UpdateEditBoxes();
		}
	}
	else
	if (nChar == VK_HOME) // Reset all
		SetToDefaults();
			
	CWnd::OnKeyDown(nChar, nRepCnt, nFlags);
}


/////////////////////////////////////////////////////////////////////////////
UINT CCurveWindow::OnGetDlgCode() 
{
	return DLGC_WANTARROWS; //return CWnd::OnGetDlgCode();
}


/////////////////////////////////////////////////////////////////////////////
BOOL CCurveWindow::SelectCurve(CMYK iCurve, BOOL bSelect)
{
	if (iCurve >= nCurves)
		return FALSE; // was not changed

	if ( bSelect != Curves[iCurve]->Selected() )
	{
		Curves[iCurve]->Select(bSelect);
		return TRUE; // was changed
	}
	
	return FALSE; // was not changed
}


/////////////////////////////////////////////////////////////////////////////
void CCurveWindow::ShowCurve(CMYK iCurve, BOOL bShow)
{
	if ( bShow == bCurveShown[iCurve] )
		return;

	bCurveShown[iCurve] = bShow;

	if (bShow)
	{
		// Unselect all curves
		SelectCurve(CI_CYAN,    FALSE);
		SelectCurve(CI_MAGENTA, FALSE);
		SelectCurve(CI_YELLOW,  FALSE);
		SelectCurve(CI_GAMMA,   FALSE);
	}

	SelectCurve(iCurve, bShow/*bSelect*/);

	if (IsWindow(m_hWnd) && IsWindowVisible())
	{
		Invalidate();
		UpdateWindow();
	}
}


/////////////////////////////////////////////////////////////////////////////
void CCurveWindow::ToggleCurve(CMYK iCurve)
{
	ShowCurve( iCurve, !bCurveShown[iCurve] );
}


/////////////////////////////////////////////////////////////////////////////
void CCurveWindow::UpdateEditBoxes(int x)
{
	if (!InBox || !OutBox)
		return;

	if ( x < 0 )
	{ // clear the boxes
		InBox->SetWindowText("");
		OutBox->SetWindowText("");
		return;
	}

	char str[16];
	sprintf(str,"%d%%", ((100 * x) + nSCALE/2) / nSCALE);

	InBox->SetWindowText(str);

	// Find the selected curve
	CCurve* pCurve = NULL;
	for (int i = 0; i < nCurves; i++)
	{
		if (Curves[i]->Selected())
			pCurve = Curves[i];
	}

	// If no selected curve, see if a single curve is displayed
	if ( !pCurve )
	{
		int iShown = -1;
		for (int i = 0; i < nCurves; i++)
		{
			if (!bCurveShown[i])
				continue;
			if (iShown >= 0)
			{ // more than one is shown
				iShown = -1;
				break;
			}
			else
				iShown = i;
		}

		if (iShown >= 0)
			pCurve = Curves[iShown];
	}

	if ( pCurve )
	{
		int y = pCurve->GetOutput(x);
		if (bPercent)
			sprintf(str,"%d%%", ((100 * y) + nSCALE/2) / nSCALE);
		else
		{
			double f = (2.0 * y) / nSCALE;
			sprintf(str,"%.3f", f);
		}
		OutBox->SetWindowText(str);
	}
	else
		OutBox->SetWindowText("");
}


/////////////////////////////////////////////////////////////////////////////
// for initialization, does not store pointers, makes a copy
void CCurveWindow::Init(BOOL colorImage, CEdit* in, CEdit* out)
{
	bColorImage = colorImage;
	InBox = in;
	OutBox = out;
	UpdateEditBoxes();
}


/////////////////////////////////////////////////////////////////////////////
void CCurveWindow::SetPoints( CMYK iCurve, double* x, double* y, int numPoints )
{
	if (iCurve >= nCurves)
		return;

	if (numPoints >= MaxNumPoints)
		numPoints = MaxNumPoints-1;

	// Convert to integer points
	POINT Points[MaxNumPoints];
	for (int i = 0; i < numPoints; i++)
	{
		Points[i].x = int((x[i] * nSCALE) + .5);
		Points[i].y = int((y[i] * nSCALE) + .5);
	}

	Curves[iCurve]->SetPoints(Points, numPoints);

	if (IsWindow(m_hWnd) && IsWindowVisible())
	{
		Invalidate();
		UpdateWindow();
	}
}


/////////////////////////////////////////////////////////////////////////////
int CCurveWindow::GetPoints( CMYK iCurve, double* x, double* y, int numPoints )
{
	if (iCurve >= nCurves)
		return 0;

	POINT Points[MaxNumPoints];
	int nPoints = Curves[iCurve]->GetPoints(Points);
	if (numPoints > nPoints)
		numPoints = nPoints;

	// Convert to doubles
	for (int i = 0; i < numPoints; i++)
	{
		// Put the point in the 0-1.0 range
		x[i] = (double)Points[i].x / nSCALE;
		y[i] = (double)Points[i].y / nSCALE;
		// Only keep 3 digits of accuracy
		int ix = (int)((x[i] * 1000.0) + 0.5);
		int iy = (int)((y[i] * 1000.0) + 0.5);
		x[i] = (double)ix / 1000.0;
		y[i] = (double)iy / 1000.0;
	}	

	return numPoints;
}


/////////////////////////////////////////////////////////////////////////////
// set the curves to defauts, not the selected curves
void CCurveWindow::SetToDefaults()
{
	for (int i = 0; i < nCurves; i++)
	{
		if (bCurveShown[i])
		{
			Curves[i]->SetToDefaults();
			// Send a message to the parent window
			NotifyParentofChange(i);
		}
	}

	if (IsWindow(m_hWnd) && IsWindowVisible())
	{
		Invalidate();
		UpdateWindow();
		UpdateEditBoxes();
	}
}


/////////////////////////////////////////////////////////////////////////////
// set the curves to empty, so no curve will be shown on screen
void CCurveWindow::SetToEmpty()
{
	for (int i = 0; i < nCurves; i++)
	{
		if (bCurveShown[i])
		{
			Curves[i]->SetToEmpty();
			// Send a message to the parent window
			NotifyParentofChange(i);
		}
	}

	if (IsWindow(m_hWnd) && IsWindowVisible())
	{
		Invalidate();
		UpdateWindow();
		UpdateEditBoxes();
	}
}

/////////////////////////////////////////////////////////////////////////////
void CCurveWindow::SetSpace(HDC hdc) const
{
	CRect rSize;
	GetClientRect(rSize);
	SetMapMode(hdc, MM_ANISOTROPIC);
	SetWindowExtEx(hdc, nSCALE, nSCALE, NULL);
	SetViewportExtEx(hdc, rSize.right, -rSize.bottom, NULL);
	SetWindowOrgEx(hdc, 0, nSCALE-1, NULL);
	SetViewportOrgEx(hdc, 0, 0, NULL);	
}


/////////////////////////////////////////////////////////////////////////////
void CCurveWindow::ToLogicalSpace(const CPoint& in, CPoint& out) const
{
	CRect rSize;
	GetClientRect(rSize);

	// Bound the point to the client rectangle (a new feature)
	CPoint inbound = in;
	if (inbound.x < rSize.left)
		inbound.x = rSize.left;
	if (inbound.x > rSize.right)
		inbound.x = rSize.right;
	if (inbound.y < rSize.top)
		inbound.y = rSize.top;
	if (inbound.y > rSize.bottom)
		inbound.y = rSize.bottom;

	// Map the point to the 0-nSCALE
	out.x = (inbound.x * (nSCALE-1)) / (rSize.right-1);
	out.y = ((rSize.bottom - inbound.y) * (nSCALE-1)) / (rSize.bottom-1);
}


/////////////////////////////////////////////////////////////////////////////
// support functions for a custom control
// Most of this is a modified version of
// /msdev/SAMPLES/mfc/GENERAL/CTRLTEST/PAREDIT2.CPP
LRESULT CALLBACK EXPORT
CCurveWindow::WndProcHook(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	// create new item and attach it
	CCurveWindow* theWnd = new CCurveWindow();
	theWnd->Attach(hWnd);
	::SetWindowLong(hWnd, GWL_WNDPROC, (DWORD)AfxWndProc);

	// then call it for this first message
	return ::CallWindowProc(AfxWndProc, hWnd, msg, wParam, lParam);
}


/////////////////////////////////////////////////////////////////////////////
void CCurveWindow::PostNcDestroy()
{
	// needed to clean up the C++ CWnd object

	// Never, never, never delete this!!!
	// Unless you're copying working example code.
	delete this;
}


/////////////////////////////////////////////////////////////////////////////
BOOL CCurveWindow::RegisterControlClass()
{
	WNDCLASS wcls;
	
	// check to see if class already registered
	static const TCHAR szClass[] = _T("CCurveWindow");
	if (::GetClassInfo(AfxGetInstanceHandle(), szClass, &wcls))
	{
		// name already registered - ok if it was us
		return (wcls.lpfnWndProc == (WNDPROC)CCurveWindow::WndProcHook);
	}
	
	// Use standard "edit" control as a template.
	VERIFY(::GetClassInfo(NULL, _T("edit"), &wcls));
	
	LOGBRUSH brush; // brush for painting the background
	brush.lbStyle = BS_SOLID;
	brush.lbColor = COLOR_BACK;

	// set new values
	wcls.style=CS_DBLCLKS | CS_HREDRAW | CS_VREDRAW;
	wcls.lpfnWndProc = CCurveWindow::WndProcHook;
	wcls.hbrBackground = CreateBrushIndirect(&brush);
	wcls.hInstance = AfxGetInstanceHandle();
	wcls.hCursor = LoadCursor(0,MAKEINTRESOURCE(IDC_ARROW));
	wcls.lpszClassName = szClass;
	return (RegisterClass(&wcls) != 0);
}
