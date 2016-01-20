// Crop.cpp : implementation file
//

#include "stdafx.h"
#include "MonacoScan.h"
#include "Crop.h"
#include "dibapi.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCrop dialog
CCrop::CCrop(CString szImageFile, CWnd* pParent /*=NULL*/)
	: CDialog(CCrop::IDD, pParent)
{
	//{{AFX_DATA_INIT(CCrop)
	m_szImageFile = szImageFile;
	m_ptMouseOnImage = 0;
	m_ptCornerUR = -1;
	m_ptCornerUL = -1;
	m_ptCornerLR = -1;
	m_ptCornerLL = -1;
	m_ptOutputUR = -1;
	m_ptOutputUL = -1;
	m_ptOutputLR = -1;
	m_ptOutputLL = -1;
	m_hDib = NULL;
	//}}AFX_DATA_INIT
}


/////////////////////////////////////////////////////////////////////////////
void CCrop::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCrop)
	DDX_Control(pDX, IDC_CROP_IMAGE, m_ctlImage);
	//}}AFX_DATA_MAP
}


/////////////////////////////////////////////////////////////////////////////
BEGIN_MESSAGE_MAP(CCrop, CDialog)
	ON_WM_QUERYNEWPALETTE()
	ON_WM_PALETTECHANGED()
	ON_WM_CREATE()
	//{{AFX_MSG_MAP(CCrop)
	ON_BN_CLICKED(IDC_UNZOOM, OnUnzoom)
	ON_BN_CLICKED(IDOK, OnCropOK)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCrop message handlers


/////////////////////////////////////////////////////////////////////////////
BEGIN_EVENTSINK_MAP(CCrop, CDialog)
    //{{AFX_EVENTSINK_MAP(CCrop)
	ON_EVENT(CCrop, IDC_CROP_IMAGE, -601 /* DblClick */, OnImageDoubleClick, VTS_NONE)
	ON_EVENT(CCrop, IDC_CROP_IMAGE, -606 /* MouseMove */, OnImageMouseMove, VTS_I2 VTS_I2 VTS_I4 VTS_I4)
	ON_EVENT(CCrop, IDC_CROP_IMAGE, -605 /* MouseDown */, OnImageMouseDown, VTS_I2 VTS_I2 VTS_I4 VTS_I4)
	ON_EVENT(CCrop, IDC_CROP_IMAGE, -607 /* MouseUp */, OnImageMouseUp, VTS_I2 VTS_I2 VTS_I4 VTS_I4)
	ON_EVENT(CCrop, IDC_CROP_IMAGE, -600 /* Click */, OnImageClick, VTS_NONE)
	ON_EVENT(CCrop, IDC_CROP_IMAGE, 6 /* ViewStatusChanged */, OnImageViewStatusChanged, VTS_I4 VTS_I4 VTS_I4)
	ON_EVENT(CCrop, IDC_CROP_IMAGE, 5 /* BufStatusChanged */, OnImageBufStatusChanged, VTS_I4 VTS_I4 VTS_I4)
	ON_EVENT(CCrop, IDC_CROP_IMAGE, 7 /* TimerTick */, OnImageTimerTick, VTS_NONE)
	//}}AFX_EVENTSINK_MAP
END_EVENTSINK_MAP()


/////////////////////////////////////////////////////////////////////////////
BOOL CCrop::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	m_ptMouseOnImage = 0;

	// The license to distribute the Imagn control
	m_ctlImage.UL( 136509823, 1211332160, 1341321342, 6802 );
	m_ctlImage.SetScrollbars(true);
	m_ctlImage.SetFilename(m_szImageFile);
	m_ctlImage.SetViewScale(-2); // Shrink to fit
	m_ctlImage.SetPopupMenu(FALSE);
	m_ctlImage.SetToolControl(0/*TOOL_NONE*/);
	m_ctlImage.SetToolShift(0/*TOOL_NONE*/);
	m_ctlImage.SetToolControlShift(0/*TOOL_NONE*/);
	m_ctlImage.SetTool(2/*TOOL_ZOOM_RECT*/);
	m_ctlImage.SetDither(2/*Dither Fast*/);
	m_ctlImage.SetRotate(0/*Normal (no rotation)*/);
	m_ctlImage.SetTimer(1000); // draw the marks every second
	m_ctlImage.SetEnabled(TRUE);

	DrawAllMarks(TRUE);

	// Check to see if it's OK to enable OK
	BOOL bSet = (
		m_ptCornerUL.x > 0 && m_ptCornerUL.y > 0 &&
		m_ptCornerUR.x > 0 && m_ptCornerUR.y > 0 &&
		m_ptCornerLL.x > 0 && m_ptCornerLL.y > 0 &&
		m_ptCornerLR.x > 0 && m_ptCornerLR.y > 0 );

	if (!bSet)
	{
		CWnd* pControl = GetDlgItem(IDOK);
		if (pControl)
			pControl->ModifyStyle(NULL/*dwRemove*/, WS_DISABLED/*dwAdd*/, NULL/*nFlags*/);
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

/////////////////////////////////////////////////////////////////////////////
CWnd* CCrop::GetImageWindow(int idControl)
{
	CWnd* pControl = GetDlgItem(idControl);
	if (!pControl)
		return NULL;

	CWnd* pWnd = pControl->GetTopWindow();
	while (pWnd)
	{
		char szClassName[100];
		int i = GetClassName(pWnd->GetSafeHwnd(), szClassName, sizeof(szClassName)-1);
		if (i > 0 && !strncmp(szClassName, "IMW", 3))
			break;
		pWnd = pWnd->GetNextWindow(GW_HWNDNEXT);
	}

	return pWnd;
}

/////////////////////////////////////////////////////////////////////////////
void CCrop::UndrawMark(int iType, POINT pt)
{
	CWnd* pWnd = GetImageWindow(IDC_CROP_IMAGE);
	if (!pWnd)
		return;

	// See if the image is flipped vertically (some DIB's)
	int yTop = 0;
	int yBottom = m_ctlImage.GetSizeY()-1;
	if (m_ctlImage.PixelToViewY(yTop) > m_ctlImage.PixelToViewY(yBottom))
		pt.y = yBottom - pt.y;
	// Convert to screen coordinates
	CRect rect;
	rect.left = rect.right = m_ctlImage.PixelToViewX(pt.x);
	rect.top = rect.bottom = m_ctlImage.PixelToViewY(pt.y);
	rect.InflateRect(10, 10);
	CWnd* pControl = GetDlgItem(IDC_CROP_IMAGE);
	if (pControl)
		pControl->MapWindowPoints(pWnd, &rect);
	pWnd->InvalidateRect(&rect, FALSE);
	pWnd->UpdateWindow();
}

/////////////////////////////////////////////////////////////////////////////
void CCrop::DrawMark(int iType, POINT pt, BOOL bDrawText)
{
	if (bDrawText)
	{
		CString sz;
		sz.Format("(%ld,%ld)", pt.x, pt.y);
		CStatic* pStaticText = (CStatic*)GetDlgItem(iType);
		if (pStaticText)
			pStaticText->SetWindowText(sz);
	}

	CWnd* pWnd = GetImageWindow(IDC_CROP_IMAGE);
	if (!pWnd)
		return;

	CDC* pDC = pWnd->GetDC();
	if (!pDC)
		return;
	CPen Pen(PS_SOLID, 1/*iWidth*/, RGB(0,255,0));
	CPen* pOldPen = pDC->SelectObject(&Pen);

	// See if the image is flipped vertically (some DIB's)
	int yTop = 0;
	int yBottom = m_ctlImage.GetSizeY()-1;
	if (m_ctlImage.PixelToViewY(yTop) > m_ctlImage.PixelToViewY(yBottom))
		pt.y = yBottom - pt.y;
	// Convert to screen coordinates
	pt.x = m_ctlImage.PixelToViewX(pt.x);
	pt.y = m_ctlImage.PixelToViewY(pt.y);
	CWnd* pControl = GetDlgItem(IDC_CROP_IMAGE);
	if (pControl)
		pControl->MapWindowPoints(pWnd, &pt, 1);

	if (iType == IDC_CORNER_UL)
	{
		pDC->MoveTo(pt.x-7, pt.y+1);
		pDC->LineTo(pt.x+1, pt.y+1);
		pDC->LineTo(pt.x+1, pt.y-7-1);
		pDC->MoveTo(pt.x-7, pt.y-1);
		pDC->LineTo(pt.x-1, pt.y-1);
		pDC->LineTo(pt.x-1, pt.y-7-1);
	}
	else
	if (iType == IDC_CORNER_LL)
	{
		pDC->MoveTo(pt.x-7, pt.y-1);
		pDC->LineTo(pt.x+1, pt.y-1);
		pDC->LineTo(pt.x+1, pt.y+7+1);
		pDC->MoveTo(pt.x-7, pt.y+1);
		pDC->LineTo(pt.x-1, pt.y+1);
		pDC->LineTo(pt.x-1, pt.y+7+1);
	}
	else
	if (iType == IDC_CORNER_UR)
	{
		pDC->MoveTo(pt.x+7, pt.y+1);
		pDC->LineTo(pt.x-1, pt.y+1);
		pDC->LineTo(pt.x-1, pt.y-7-1);
		pDC->MoveTo(pt.x+7, pt.y-1);
		pDC->LineTo(pt.x+1, pt.y-1);
		pDC->LineTo(pt.x+1, pt.y-7-1);
	}
	else
	if (iType == IDC_CORNER_LR)
	{
		pDC->MoveTo(pt.x+7, pt.y-1);
		pDC->LineTo(pt.x-1, pt.y-1);
		pDC->LineTo(pt.x-1, pt.y+7+1);
		pDC->MoveTo(pt.x+7, pt.y+1);
		pDC->LineTo(pt.x+1, pt.y+1);
		pDC->LineTo(pt.x+1, pt.y+7+1);
	}

	pDC->SelectObject(pOldPen);
	pWnd->ReleaseDC(pDC);
}

/////////////////////////////////////////////////////////////////////////////
void CCrop::DrawAllMarks(BOOL bDrawText)
{
	if ( m_ptCornerUL.x > 0 && m_ptCornerUL.y > 0 )
		DrawMark(IDC_CORNER_UL, m_ptCornerUL, bDrawText);

	if ( m_ptCornerLL.x > 0 && m_ptCornerLL.y > 0 )
		DrawMark(IDC_CORNER_LL, m_ptCornerLL, bDrawText);

	if ( m_ptCornerUR.x > 0 && m_ptCornerUR.y > 0 )
		DrawMark(IDC_CORNER_UR, m_ptCornerUR, bDrawText);

	if ( m_ptCornerLR.x > 0 && m_ptCornerLR.y > 0 )
		DrawMark(IDC_CORNER_LR, m_ptCornerLR, bDrawText);
}

/////////////////////////////////////////////////////////////////////////////
void CCrop::OnUnzoom() 
{
	m_ctlImage.SetViewScale(0); // Shrink to fit
}

/////////////////////////////////////////////////////////////////////////////
void CCrop::OnCropOK() 
{
	BeginWaitCursor();

	// Free any previously allocated DIB
	if (m_hDib)
	{
		GlobalFree(m_hDib);
		m_hDib = NULL;
	}

	// Create the DIB
	m_hDib = (HGLOBAL)m_ctlImage.CreateDIB(m_ctlImage.GetViewBufferId());

	EndWaitCursor();

	// Make sure that the DIB is legit
	if (!m_hDib)
		return;
	PBITMAPINFO pDib = (PBITMAPINFO)GlobalLock(m_hDib);
	if (!pDib)
		return;

	int iWidth = DIBWidth((LPSTR)pDib);
	int iHeight = DIBHeight((LPSTR)pDib);

	// We are done with the DIB
	GlobalUnlock(m_hDib);
	pDib = NULL;

	int iControlWidth = 0;
	CRect rect;
	HWND hWnd = (HWND)m_ctlImage.GetHWnd();
	if (hWnd)
	{
		::GetClientRect(hWnd, &rect);
		iControlWidth = rect.Width();
	}

	if (!iControlWidth)
		iControlWidth = 500;

	// Make sure the points we got from the crop dialog are fairly square
	int iThreshold;
	iThreshold = iWidth / iControlWidth; // Size of a screen pixel
	if (iThreshold < 1) iThreshold = 1;
	iThreshold *= 5; // 5 screen pixels
	if (abs(m_ptCornerUL.x - m_ptCornerLL.x) > iThreshold ||
		abs(m_ptCornerUR.x - m_ptCornerLR.x) > iThreshold ||
		abs(m_ptCornerUL.y - m_ptCornerUR.y) > iThreshold ||
		abs(m_ptCornerLL.y - m_ptCornerLR.y) > iThreshold)
		{
			MessageBox("Your crop points do not appear to form a rectanglular area.  Please try to relocate them.", AfxGetApp()->m_pszAppName);
			return;
		}

	// Make sure the aspect ratio is about 1.8
	double fAspect = double(m_ptCornerUR.x - m_ptCornerUL.x + 1) /
					 double(m_ptCornerLL.y - m_ptCornerUL.y + 1);
	if (fAspect < 1.7 || fAspect > 1.9)
	{
		MessageBox("One or more of your crop points do not appear to line up with the marks in the target image.  Please try to relocate them.", AfxGetApp()->m_pszAppName);
		return;
	}

	// Create the output points;
	// Move them out since the marks are inside the actual area to be measured
	m_ptOutputUL = m_ptCornerUL;
	m_ptOutputLL = m_ptCornerLL;
	m_ptOutputUR = m_ptCornerUR;
	m_ptOutputLR = m_ptCornerLR;

	int iCropWidth  = min( (m_ptOutputUR.x - m_ptOutputUL.x),
						   (m_ptOutputLR.x - m_ptOutputLL.x) );
	int iCropHeight = min( (m_ptOutputLL.y - m_ptOutputUL.y),
						   (m_ptOutputLR.y - m_ptOutputUR.y) );

	int iOffsetX = (iCropWidth + 23) / 46;
	m_ptOutputUL.x -= iOffsetX;
	m_ptOutputLL.x -= iOffsetX;
	m_ptOutputUR.x += iOffsetX;
	m_ptOutputLR.x += iOffsetX;
	
	int iOffsetY = (iCropHeight + 13) / 26;
	m_ptOutputUL.y -= iOffsetY;
	m_ptOutputUR.y -= iOffsetY;

	iOffsetY = ((5*iCropHeight) + 13) / 26;
	m_ptOutputLL.y += iOffsetY;
	m_ptOutputLR.y += iOffsetY;

	// Make sure the crop point are within the image
	if (m_ptOutputUL.x < 0 || m_ptOutputLL.x < 0 || m_ptOutputUL.y < 0 || m_ptOutputUR.y < 0 ||
		m_ptOutputUR.x >= iWidth  || m_ptOutputLR.x >= iWidth  ||
		m_ptOutputLL.y >= iHeight || m_ptOutputLR.y >= iHeight )
	{
		MessageBox("One or more of your crop points appear to be off the image.  Please try to relocate them.", AfxGetApp()->m_pszAppName);
		return;
	}

	CDialog::OnOK();
}

/////////////////////////////////////////////////////////////////////////////
void CCrop::OnImageMouseMove(short Button, short Shift, long X, long Y) 
{
	X = m_ctlImage.ViewToPixelX(X);
	Y = m_ctlImage.ViewToPixelY(Y);

	CString sz;

	if (X < 0 || X >= m_ctlImage.GetSizeX() ||
		Y < 0 || Y >= m_ctlImage.GetSizeY())
		sz = "";
	else
	{
		m_ptMouseOnImage.x = X;
		m_ptMouseOnImage.y = Y;
		sz.Format("(%ld,%ld)", X, Y);
	}

	CWnd* pControl = GetDlgItem(IDC_MOUSE_MOVE);
	if (pControl)
		pControl->SetWindowText(sz);
}


/////////////////////////////////////////////////////////////////////////////
void CCrop::OnImageClick() 
{
	int id = NULL;
	long xSize = m_ctlImage.GetSizeX();
	long ySize = m_ctlImage.GetSizeY();
	long x = m_ptMouseOnImage.x;
	long y = m_ptMouseOnImage.y;
	if (y < ySize/2 && x < xSize/2)
	{ // Set the upper left point
		id = IDC_CORNER_UL;
		UndrawMark(id, m_ptCornerUL);
		m_ptCornerUL = m_ptMouseOnImage;
		DrawMark(id, m_ptCornerUL, TRUE);
	}
	else
	if (y < ySize/2 && x > xSize/2)
	{ // Set the upper right point
		id = IDC_CORNER_UR;
		UndrawMark(id, m_ptCornerUR);
		m_ptCornerUR = m_ptMouseOnImage;
		DrawMark(id, m_ptCornerUR, TRUE);
	}
	else
	if (y > ySize/2 && x < xSize/2)
	{ // Set the lower left point
		id = IDC_CORNER_LL;
		UndrawMark(id, m_ptCornerLL);
		m_ptCornerLL = m_ptMouseOnImage;
		DrawMark(id, m_ptCornerLL, TRUE);
	}
	else
	if (y > ySize/2 && x > xSize/2)
	{ // Set the lower right point
		id = IDC_CORNER_LR;
		UndrawMark(id, m_ptCornerLR);
		m_ptCornerLR = m_ptMouseOnImage;
		DrawMark(id, m_ptCornerLR, TRUE);
	}

	// Check to see if it's OK to enable OK
	BOOL bSet = (
		m_ptCornerUL.x > 0 && m_ptCornerUL.y > 0 &&
		m_ptCornerUR.x > 0 && m_ptCornerUR.y > 0 &&
		m_ptCornerLL.x > 0 && m_ptCornerLL.y > 0 &&
		m_ptCornerLR.x > 0 && m_ptCornerLR.y > 0 );

	if (bSet)
	{
		CWnd* pControl = GetDlgItem(IDOK);
		if (pControl)
		{
			pControl->ModifyStyle(WS_DISABLED/*dwRemove*/, NULL/*dwAdd*/, NULL);
			pControl->Invalidate();
		}
	}
}

/////////////////////////////////////////////////////////////////////////////
void CCrop::OnImageDoubleClick() 
{
}

/////////////////////////////////////////////////////////////////////////////
void CCrop::OnImageMouseDown(short Button, short Shift, long X, long Y) 
{
}

/////////////////////////////////////////////////////////////////////////////
void CCrop::OnImageMouseUp(short Button, short Shift, long X, long Y) 
{
}

/////////////////////////////////////////////////////////////////////////////
void CCrop::OnImageViewStatusChanged(long ViewId, long TypeCode, long Data) 
{
}

/////////////////////////////////////////////////////////////////////////////
void CCrop::OnImageBufStatusChanged(long BufId, long Status, long Data) 
{
}

/////////////////////////////////////////////////////////////////////////////
void CCrop::OnImageTimerTick() 
{
	DrawAllMarks(FALSE);
}
