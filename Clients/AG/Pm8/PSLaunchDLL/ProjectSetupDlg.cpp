// ProjectSetupDlg.cpp : implementation file
//

#include "stdafx.h"
#include "LaunchResource.h"
#include "PSLaunchDLL.h"

#include <winspool.h>

#include "ProjectSetupDlg.h"
#include "SetupConstants.h"
#include "CommonTypes.h"

// Framework Support
#include "DcDrawingSurface.h"
#include "Printer.h"

using RPrinter::kPortrait;
using RPrinter::kLandscape;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSetupPreview

const int kPreviewMargin = 10;

CSetupPreview::CSetupPreview()
{
}

CSetupPreview::~CSetupPreview()
{
}


BEGIN_MESSAGE_MAP(CSetupPreview, CStatic)
	//{{AFX_MSG_MAP(CSetupPreview)
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSetupPreview message handlers

void CSetupPreview::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	
	// Determine page rects
	CRect rect;
	GetClientRect( rect );

	if (_ProjectInfo.sDocDesc.nPages > 1)
	{
		// Compute the two rectangles
		//
		CRect rect2( rect );

		if (_ProjectInfo.nProjectFormat == kPortrait)
			rect2.left += rect.Width() / 2;
		else
			rect2.top  += rect.Height() / 2;

		rect.SubtractRect( rect, rect2 );

		// Draw the second page
		DrawPage( &dc, rect2, 2 );
	}

	DrawPage( &dc, rect, 1 );
}

void CSetupPreview::DrawPage( CDC* pDC, CRect rect, int nPage ) 
{
	// Deflate the rect some so it doesn't draw right up to the edge of the control
	rect.DeflateRect( kPreviewMargin, kPreviewMargin );

	// If we are adding page numbers, allow room for the text
	CFont* pFont = GetParent()->GetFont();
	CFont* pOldFont = pDC->SelectObject( pFont );

	if (_ProjectInfo.sDocDesc.nPages > 1)
	{
		CSize szExtent = pDC->GetTextExtent ("Page 2", 6) ;
		rect.bottom -= (szExtent.cy + 5) ;
	}

	// Determine page rect
	//
	double xScale = (double) rect.Width()  / _ProjectInfo.sDocDesc.szPage.cx;
	double yScale = (double) rect.Height() / _ProjectInfo.sDocDesc.szPage.cy;

	if (xScale > yScale)
		xScale = yScale;
	else
		yScale = xScale;

	CRect cPageRect( 0, 0,
		_ProjectInfo.sDocDesc.szPage.cx * xScale, 
		_ProjectInfo.sDocDesc.szPage.cy * yScale );

	// Draw page 
	cPageRect.OffsetRect( rect.CenterPoint() - cPageRect.CenterPoint() );
	pDC->Rectangle( cPageRect );

	try
	{
		// Draw page dressing
		//
		CPen  cPen( PS_SOLID, 1, RGB( 128, 128, 128 ) ); 
		CPen* pOldPen = pDC->SelectObject( &cPen );

		pDC->MoveTo( cPageRect.right, cPageRect.top + 2 );
		pDC->LineTo( cPageRect.right, cPageRect.bottom );
		pDC->LineTo( cPageRect.left + 2, cPageRect.bottom );

		CPen  cPen2( PS_SOLID, 1, RGB( 196, 196, 196 ) ); 
		pDC->SelectObject( &cPen2 );

		pDC->MoveTo( cPageRect.right - 2, cPageRect.top + 1 );
		pDC->LineTo( cPageRect.right - 2, cPageRect.bottom - 1 );
		pDC->SelectObject( pOldPen );

		// Draw page text
		//
		if (_ProjectInfo.sDocDesc.nPages > 1)
		{
			CString strPage;
			strPage.Format( IDS_SETUP_PAGE, nPage );
			CSize szExtent = pDC->GetTextExtent( strPage, 6 );

			int cxPos = cPageRect.left + (cPageRect.Width() - szExtent.cx) / 2;
			pDC->TextOut( cxPos, cPageRect.bottom + 5, strPage );
		}

		// Compute margin rect
		CRect cMarginRect( 
			cPageRect.left + _ProjectInfo.sDocDesc.rcMargins.left * xScale,
			cPageRect.top  + _ProjectInfo.sDocDesc.rcMargins.top  * xScale,
			cPageRect.left + _ProjectInfo.sDocDesc.rcMargins.right * xScale,
			cPageRect.top  + _ProjectInfo.sDocDesc.rcMargins.bottom * xScale );

		// Copy the margin rect into page rect, as this rect
		// will be used and modified when drawing all slabs.
		cPageRect = cMarginRect;

		// Create a brush for rendering the slabs
		CBrush cbrSlab( RGB( 196, 196, 196 ) );

		// Draw Header
		//
		if (_ProjectInfo.sDocDesc.bHasHeaders && nPage >= _ProjectInfo.sDocDesc.nHeaderStartPage)
		{
			CRect cRect = cPageRect;
			cRect.bottom = cRect.top + (_ProjectInfo.sDocDesc.nHeaderHeight * yScale);
			
			// Draw the header rectangle. 
			pDC->FrameRect( cRect, &cbrSlab );  

			// Adjust page rect for header and seperator
			cPageRect.top = cRect.bottom + _ProjectInfo.sDocDesc.nVerticalGap * yScale;
		}

		// Draw Masthead
		//
		if (_ProjectInfo.sDocDesc.bHasMasthead && nPage == 1)
		{
			CRect cRect = cPageRect;
			cRect.bottom = cRect.top + (_ProjectInfo.sDocDesc.nMastheadHeight * yScale);
			
			// Draw the masthead rectangle. 
			pDC->FrameRect( cRect, &cbrSlab );  

			// Adjust page rect for masthead and seperator
			cPageRect.top = cRect.bottom + _ProjectInfo.sDocDesc.nVerticalGap * yScale;
		}

		// Draw Footer
		//
		if (_ProjectInfo.sDocDesc.bHasFooters && nPage >= _ProjectInfo.sDocDesc.nFooterStartPage)
		{
			CRect cRect = cPageRect;
			cRect.top = cRect.bottom - _ProjectInfo.sDocDesc.nFooterHeight * yScale;
			
			// Draw the footer rectangle.  
			pDC->FrameRect( cRect, &cbrSlab );  

			// Adjust page rect for footer and seperator
			cPageRect.bottom = cRect.top - _ProjectInfo.sDocDesc.nVerticalGap * yScale;
		}

		// Draw Columns
		//
		int& nColumns  = _ProjectInfo.sDocDesc.nColumns;
		int  nGapWidth = _ProjectInfo.sDocDesc.nColumnGap * xScale;
		int  nColWidth = (cPageRect.Width() - (nGapWidth * (nColumns - 1))) / nColumns;

		CRect cColRect( cPageRect );
		cColRect.right = cColRect.left + nColWidth;
		nColWidth += nGapWidth;
		
		for (int i = 0; i < nColumns; i++)
		{
			pDC->FrameRect( cColRect, &cbrSlab );  

			if (_ProjectInfo.sDocDesc.bHasColumnLines && i < nColumns - 1)
			{
				// Draw Column line
				int xPos = cColRect.right + nGapWidth / 2;

				pDC->MoveTo( xPos, cColRect.top );
				pDC->LineTo( xPos, cColRect.bottom );
			}

			cColRect.OffsetRect( nColWidth, 0 );
		}

		// Draw Margins
		//
		CBrush cbrMargins( RGB( 0, 247, 247 ) ); 
		pDC->FrameRect( cMarginRect, &cbrMargins );  

	}
	catch (CResourceException&)
	{
	}


	pDC->SelectObject( pOldFont );
}

/////////////////////////////////////////////////////////////////////////////
// CProjectSetupDlg dialog


CProjectSetupDlg::CProjectSetupDlg(CWnd* pParent /*=NULL*/)
	: CProjectDialog(CProjectSetupDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CProjectSetupDlg)
	m_fBottom = 1.0;
	m_fLeft = 1.0;
	m_fRight = 1.0;
	m_fTop = 1.0;
	//}}AFX_DATA_INIT

	// TODO: change to user units
//  m_fMinTop    = ::LogicalUnitsToInches( rect.top );
//  m_fMinLeft   = ::LogicalUnitsToInches( rect.left );
//  m_fMinBottom = ::LogicalUnitsToInches( _ProjectInfo.sDocDesc.szPage.cy - rect.bottom );
//  m_fMinRight  = ::LogicalUnitsToInches( _ProjectInfo.sDocDesc.szPage.cx - rect.right );

    // set 3 inches as default maximum margin
    m_fMax = 3; //CBDevice::LogToUUnits(CBD_LOGINCH * 3);
}


void CProjectSetupDlg::DoDataExchange(CDataExchange* pDX)
{
	CProjectDialog::DoDataExchange(pDX);

	//{{AFX_DATA_MAP(CProjectSetupDlg)
	DDX_Control(pDX, IDC_STATIC_PREVIEW, m_cPreview);
	DDX_Control(pDX, IDC_TOPSPIN, m_spinTop);
	DDX_Control(pDX, IDC_TOPMARGIN, m_editTop);
	DDX_Control(pDX, IDC_RIGHTSPIN, m_spinRight);
	DDX_Control(pDX, IDC_RIGHTMARGIN, m_editRight);
	DDX_Control(pDX, IDC_LEFTSPIN, m_spinLeft);
	DDX_Control(pDX, IDC_LEFTMARGIN, m_editLeft);
	DDX_Control(pDX, IDC_BOTTOMSPIN, m_spinBottom);
	DDX_Control(pDX, IDC_BOTTOMMARGIN, m_editBottom);
	//}}AFX_DATA_MAP

	// Update/Validate Appearence Fields
	//
	SBDocDescription& desc = _ProjectInfo.sDocDesc;

	DDX_Text(pDX, IDC_PAGES, desc.nPages);
	DDV_MinMaxInt(pDX, desc.nPages, 1, 100);
	DDX_Text(pDX, IDC_COLUMNS, desc.nColumns);
	DDV_MinMaxInt(pDX, desc.nColumns, 1, 4);
	DDX_Check(pDX, IDC_FOOTER, desc.bHasFooters);
	DDX_Check(pDX, IDC_HEADER, desc.bHasHeaders);
	DDX_Check(pDX, IDC_LINES, desc.bHasColumnLines);
	DDX_Check(pDX, IDC_MASTHEAD, desc.bHasMasthead);

	int nHeaderStartPage = desc.nHeaderStartPage - 1;
	int nFooterStartPage = desc.nFooterStartPage - 1;

	DDX_CBIndex(pDX, IDC_HEADER_START, nHeaderStartPage);
	DDX_CBIndex(pDX, IDC_FOOTER_START, nFooterStartPage);

	if (pDX->m_bSaveAndValidate)
	{
		desc.nHeaderStartPage = nHeaderStartPage + 1;
		desc.nFooterStartPage = nFooterStartPage + 1;
	}

	// Update/Validate Margin Fields
	//

	// TODO: use user units.
	if (!pDX->m_bSaveAndValidate)
	{
		// Determine the values to display
		m_fLeft   = ::LogicalUnitsToInches( desc.rcMargins.left );
		m_fTop    = ::LogicalUnitsToInches( desc.rcMargins.top );
		m_fRight  = ::LogicalUnitsToInches( desc.szPage.cx - desc.rcMargins.right );
		m_fBottom = ::LogicalUnitsToInches( desc.szPage.cy - desc.rcMargins.bottom );
	}

#if 1
	// Validate Margin values
	DDX_Text(pDX, IDC_TOPMARGIN, m_fTop);
	DDV_MinMaxDouble(pDX, m_fTop, m_fMinTop, m_fMax);
	DDX_Text(pDX, IDC_BOTTOMMARGIN, m_fBottom);
	DDV_MinMaxDouble(pDX, m_fBottom, m_fMinBottom, m_fMax);
	DDX_Text(pDX, IDC_LEFTMARGIN, m_fLeft);
	DDV_MinMaxDouble(pDX, m_fLeft, m_fMinLeft, m_fMax);
	DDX_Text(pDX, IDC_RIGHTMARGIN, m_fRight);
	DDV_MinMaxDouble(pDX, m_fRight, m_fMinRight, m_fMax);
#else
	ExchangeValidTextNumber( pDX,IDC_TOPMARGIN,    &m_fTop,    m_fMinBottom, m_fMax ); 
	ExchangeValidTextNumber( pDX,IDC_BOTTOMMARGIN, &m_fBottom, m_fMinBottom, m_fMax ); 
	ExchangeValidTextNumber( pDX,IDC_LEFTMARGIN,   &m_fLeft,   m_fMinBottom, m_fMax ); 
	ExchangeValidTextNumber( pDX,IDC_RIGHTMARGIN,  &m_fRight,  m_fMinBottom, m_fMax ); 
#endif

	if (pDX->m_bSaveAndValidate)
	{
		// Update document description with new values
		desc.rcMargins.left   = ::InchesToLogicalUnits( m_fLeft );
		desc.rcMargins.top    = ::InchesToLogicalUnits( m_fTop );
		desc.rcMargins.right  = desc.szPage.cx - ::InchesToLogicalUnits( m_fRight );
		desc.rcMargins.bottom = desc.szPage.cy - ::InchesToLogicalUnits( m_fBottom );
	}

	if (!pDX->m_bSaveAndValidate)
	{
		// Disable masthead option if masthead height is 0
		GetDlgItem( IDC_MASTHEAD )->EnableWindow( desc.nMastheadHeight );

		// Disable header option if header height is 0
		GetDlgItem( IDC_HEADER )->EnableWindow( desc.nHeaderHeight );
		
		// Disable footer option if footer height is 0
		GetDlgItem( IDC_FOOTER )->EnableWindow( desc.nFooterHeight );
	}
}


BEGIN_MESSAGE_MAP(CProjectSetupDlg, CProjectDialog)
	//{{AFX_MSG_MAP(CProjectSetupDlg)
	ON_WM_CTLCOLOR()
	ON_BN_CLICKED(IDC_FOOTER, OnFooter)
	ON_BN_CLICKED(IDC_HEADER, OnHeader)
	ON_BN_CLICKED(IDC_LINES, OnDataChange)
	ON_EN_KILLFOCUS(IDC_COLUMNS, OnKillfocusColumns)
	ON_EN_KILLFOCUS(IDC_PAGES, OnKillfocusPages)
	ON_BN_CLICKED(IDC_MASTHEAD, OnDataChange)
	ON_CBN_SELCHANGE(IDC_HEADER_START, OnDataChange)
	ON_CBN_SELCHANGE(IDC_FOOTER_START, OnDataChange)
	ON_WM_VSCROLL()
	ON_EN_KILLFOCUS(IDC_BOTTOMMARGIN, OnKillfocusBottommargin)
	ON_EN_KILLFOCUS(IDC_LEFTMARGIN, OnKillfocusLeftmargin)
	ON_EN_KILLFOCUS(IDC_RIGHTMARGIN, OnKillfocusRightmargin)
	ON_EN_KILLFOCUS(IDC_TOPMARGIN, OnKillfocusTopmargin)
	ON_MESSAGE( UM_UPDATE_DATA, OnUpdateData )
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////
// @mfunc Helper function to initialize document settings for documents
//        created "from scratch". Initial values for these variables are
//        defined in the Publisher specification in section 2.1.
//
// @devnote The real magic at work here is the operator= defined in the
//          SBDocDescription struct which initializes with values from a
//          SBDocConstants struct.
//
// @xref  <t SBDocDescription>, <t SBDocConstants>
//
void CProjectSetupDlg::InitDocDesc( BOOL fFirstInit )
{
    // index into project constants array
	ASSERT( _ProjectInfo.nPaperType < 3 );
    int nIndex = (_ProjectInfo.nPaperType * 2) + _ProjectInfo.nProjectFormat;

	SBDocDescription& sDocDesc = _ProjectInfo.sDocDesc;

    // select the correct defaults struct and initialize m_pPWData with it
    switch (_ProjectInfo.nProjectType - kPressWriterProjectStart)
    {
    default:
    case PROJECT_UNKNOWN:
    case PROJECT_NEWSLETTER:
	    sDocDesc = kNewsletterArray[nIndex];
        break;
    case PROJECT_BROCHURE:
        sDocDesc = kBrochureArray[nIndex];
        break;
    case PROJECT_REPORT:
        sDocDesc = kReportArray[nIndex];
        break;
    case PROJECT_BOOKLET:
        sDocDesc = kBookletArray[nIndex];
        break;
    case PROJECT_LETTER:
        sDocDesc = kLetterArray[nIndex];
        break;
    case PROJECT_FLYER:
        sDocDesc = kFlyerArray[nIndex];
        break;
    case PROJECT_CUSTOM:
        sDocDesc = kCustomArray[nIndex];
        break;
    }

	if (!fFirstInit)
	{
		// If this is not the first initialization, then we just over wrote
		// the current user settings.  So, load them back in from the controls.
		UpdateData( TRUE );
	}
}

/////////
// @mfunc <c CProjectSetupDlg> function to calculate the printable area of the page.
//        The CPrintDialog class is invoked to obtain a DC for the current
//        printer.  The DC is queried for printable area information, which
//        is converted into twips by our <c CBDevice> class.
//
// @rdesc m_rcPrintable, a rectangle describing the printable area in twips.
//
CRect CProjectSetupDlg::GetPrintableArea()
{
	// For convenience use a shorter descriptor
	SBDocDescription& desc = _ProjectInfo.sDocDesc;

	// Update printer 
	static short _DMPaperMap[]  = { DMPAPER_LETTER, DMPAPER_LEGAL, DMPAPER_A4, DMPAPER_B5, DMPAPER_TABLOID };
	static short _DMOrientMap[] = { DMORIENT_PORTRAIT, DMORIENT_LANDSCAPE };

	((CPSLaunchDLLApp *) AfxGetApp())->SetPrintFormat( 
		_DMPaperMap[_ProjectInfo.nPaperType], 
		_DMOrientMap[_ProjectInfo.nProjectFormat] );


    // make a default one to start with
    CRect rcPrint( 0, 0, desc.szPage.cx, desc.szPage.cy);
        
    // subtract an eighth inch all around the page
    int nEdge = ::InchesToLogicalUnits( .125 );
    rcPrint.InflateRect( -nEdge, -nEdge);

    // ok, let's get real...
    // code curtesy of John Hornick (jhornick@MICROSOFT.COM)
    CPrintDialog cDlg(FALSE, PD_RETURNDEFAULT | PD_RETURNDC, GetParent());

    PRINTDLG pdlg;
    ::ZeroMemory(&pdlg, sizeof(PRINTDLG));
    AfxGetApp()->GetPrinterDeviceDefaults(&pdlg);
    LPDEVMODE pOrigDevMode = (LPDEVMODE)::GlobalLock(pdlg.hDevMode);

    // the dialog won't appear because of the PD_RETURNDEFAULT flag
    cDlg.DoModal();

    if (cDlg.m_pd.hDC == NULL)      // no printer installed?
    {
        return rcPrint;				// return the default 1/8" inset
    }

    LPDEVMODE pDevMode   = (LPDEVMODE)::GlobalLock(cDlg.m_pd.hDevMode); 
    LPDEVNAMES pDevNames = (LPDEVNAMES)::GlobalLock(cDlg.m_pd.hDevNames);

    LPTSTR lpszDriverName = (LPTSTR)pDevNames + pDevNames->wDriverOffset;
    LPTSTR lpszDeviceName = (LPTSTR)pDevNames + pDevNames->wDeviceOffset;
    LPTSTR lpszPortName   = (LPTSTR)pDevNames + pDevNames->wOutputOffset;

    HANDLE  hPrinter;
    ::OpenPrinter( lpszDeviceName, &hPrinter, NULL );
    ::DocumentProperties( NULL, hPrinter, lpszDeviceName, pDevMode,
                                pOrigDevMode, DM_IN_BUFFER|DM_OUT_BUFFER );

    HDC hDC = ::ResetDC( cDlg.m_pd.hDC, pDevMode );

    ::GlobalUnlock( pdlg.hDevMode );
    ::GlobalUnlock( cDlg.m_pd.hDevMode );
    ::GlobalUnlock( cDlg.m_pd.hDevNames );

    CDC cDC;            // use a CDC class for convenience
    cDC.Attach(hDC);

    // get upper left in device units
    POINT pt;
    cDC.Escape(GETPRINTINGOFFSET, 0, NULL, &pt);

    // get lower right in device units
	RIntPoint ptUL( pt.x, pt.y );
    RIntPoint ptLR( 
		cDC.GetDeviceCaps(HORZRES) + ptUL.m_x, 
		cDC.GetDeviceCaps(VERTRES) + ptUL.m_y );

	RRealSize dpi( 
		::GetDeviceCaps( cDC.m_hAttribDC, LOGPIXELSX ),
		::GetDeviceCaps( cDC.m_hAttribDC, LOGPIXELSY ) );

	// Create the scaling factor
	RRealSize scalingFactor( kSystemDPI / dpi.m_dx, kSystemDPI / dpi.m_dy );
	ptUL.Scale( scalingFactor );
	ptLR.Scale( scalingFactor );

    // we need a CBDevice to set up for our logical units (TWIPS) 
//    CBDevice cbDevice(&cDC);
//    cbDevice.SetScaling(1.0);

    // translate both points to logical units
//    cbDevice.DPtoLP(&ptUL);
//    cbDevice.DPtoLP(&ptLR);

    // set the rectangle
    rcPrint.SetRect(ptUL.m_x, ptUL.m_y, ptLR.m_x, ptLR.m_y);

    // clean up the mess
//    cDC.Detach();
//	DeleteDC(hDC) ;

    return rcPrint;
}

// quick hack to round a floating point number
static double round2(double d)
{
    d *= 100;
    d += .5;
    int n = int(d);
    return double(n) / 100.;
}

/////////
// @mfunc Public <c CProjectSetupDlg> function called by <c CProjectSetupDlg> to
//        initialize or update the margins outer limits.
//
// @rdesc TRUE if the margin values have changed as a result of this call
//
BOOL CProjectSetupDlg::SetOuterMarginLimits(
    const CRect &rcLimit,   // @parm the "printable area" rectangle in twips
    const CSize &szPage)    // @parm size of the page that contains it in twips
{
	SBDocDescription& desc = _ProjectInfo.sDocDesc;
	CRect rcMargins( desc.rcMargins );

	m_fMinTop = round2( ::LogicalUnitsToInches( rcLimit.top ) );
	m_fMinLeft = round2( ::LogicalUnitsToInches( rcLimit.left ) );
	m_fMinRight = round2( ::LogicalUnitsToInches( szPage.cx - rcLimit.right ) );
	m_fMinBottom = round2( ::LogicalUnitsToInches( szPage.cy - rcLimit.bottom ) );

	if (m_fTop < m_fMinTop)
		desc.rcMargins.top    = ::InchesToLogicalUnits( m_fMinTop );

	if (m_fLeft < m_fMinLeft)
		desc.rcMargins.left   = ::InchesToLogicalUnits( m_fMinLeft );

	if (m_fRight < m_fMinRight)
		desc.rcMargins.right  = szPage.cx - ::InchesToLogicalUnits( m_fMinRight );

	if (m_fBottom < m_fMinBottom)
		desc.rcMargins.bottom = szPage.cy - ::InchesToLogicalUnits( m_fMinBottom );

    // update controls if window has been created
    if (::IsWindow(m_hWnd))
    {
        UpdateData(FALSE);                              // member data to controls
    }

    return rcMargins != desc.rcMargins;
}

/////////////////////////////////////////////////////////////////////////////
// CProjectSetupDlg message handlers

BOOL CProjectSetupDlg::OnInitDialog() 
{
	// Initialize document description.  Note: this must occur
	// before the base classes OnInitDialog() is called
	//
	InitDocDesc();

    // set half an inch as default minimum margin
	CRect rect = GetPrintableArea();
	SetOuterMarginLimits( rect, _ProjectInfo.sDocDesc.szPage );

	CProjectDialog::OnInitDialog();
	
    // set ranges of spinner controls (same for all doc types, per spec)
    ((CSpinButtonCtrl *) GetDlgItem( IDC_PAGES_CTRL ))->SetRange( 1, 100 );
    ((CSpinButtonCtrl *) GetDlgItem( IDC_COLUMNS_CTRL ))->SetRange( 1, 4 );

    // Disable the window while we set up the spinners
    // to prevent annoying flashes.
    // 8/13/96, MCK
    EnableWindow( FALSE );

	// set initial range of top margin spinner
    m_spinTop.SetPrecision(2);
    m_spinTop.SetRange(m_fMinTop, m_fMax, 0.05);
    m_spinTop.SetPosition(m_fTop);
    m_editTop.AttachSpinner(&m_spinTop);

    // set initial range of left margin spinner
    m_spinLeft.SetPrecision(2);
    m_spinLeft.SetRange(m_fMinLeft, m_fMax, 0.05);
    m_spinLeft.SetPosition(m_fLeft);
    m_editLeft.AttachSpinner(&m_spinLeft);

    // set initial range of right margin spinner
    m_spinRight.SetPrecision(2);
    m_spinRight.SetRange(m_fMinRight, m_fMax, 0.05);
    m_spinRight.SetPosition(m_fRight);
    m_editRight.AttachSpinner(&m_spinRight);

    // set initial range of bottom margin spinner
    m_spinBottom.SetPrecision(2);
    m_spinBottom.SetRange(m_fMinBottom, m_fMax, 0.05);
    m_spinBottom.SetPosition(m_fBottom);
    m_editBottom.AttachSpinner(&m_spinBottom);

    EnableWindow();

#if 0
	// TODO: User units not supported yet
    // display the text describing current units of measurement (gUnits)
	if (units != UU_INCH)
	{
		SetDlgItemText(IDC_STATIC_UNITS1, "cm");
		SetDlgItemText(IDC_STATIC_UNITS2, "cm");
		SetDlgItemText(IDC_STATIC_UNITS3, "cm");
		SetDlgItemText(IDC_STATIC_UNITS4, "cm");
	}
#endif

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

HBRUSH CProjectSetupDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
	HBRUSH hbr = (HBRUSH) ::GetStockObject( WHITE_BRUSH );
	
	return hbr;
}


BOOL CSetupPreview::OnEraseBkgnd(CDC* pDC) 
{
	CRect rect;
	GetClientRect( rect );
	pDC->FillSolidRect( rect, GetSysColor( COLOR_WINDOW ) );
	
	return TRUE; //CStatic::OnEraseBkgnd(pDC);
}

void CProjectSetupDlg::OnFooter() 
{
    if (UpdateData(TRUE))       // from controls to member variables
	{
		GetDlgItem( IDC_FOOTER_START )->EnableWindow( _ProjectInfo.sDocDesc.bHasFooters );
		m_cPreview.Invalidate( FALSE );
	}
}

void CProjectSetupDlg::OnHeader() 
{
    if (UpdateData(TRUE))       // from controls to member variables
	{
		GetDlgItem( IDC_HEADER_START )->EnableWindow( _ProjectInfo.sDocDesc.bHasHeaders );
		m_cPreview.Invalidate( FALSE );
	}
}

void CProjectSetupDlg::OnDataChange() 
{
    if (UpdateData(TRUE))		// from controls to member variables
		m_cPreview.Invalidate( FALSE );
}

void CProjectSetupDlg::OnKillfocusColumns() 
{
	//
	// First check to see if the control was losing
	// focus to the cancel button.  If so, we need
	// to return so no updating of validating occurs.
	//
	if (GetFocus() == GetParent()->GetDlgItem(IDCANCEL))
		return ;

	int nOldValue = _ProjectInfo.sDocDesc.nColumns ;

	if (UpdateData (TRUE) && nOldValue != _ProjectInfo.sDocDesc.nColumns)
	{
		m_cPreview.Invalidate( FALSE );
	}
}

void CProjectSetupDlg::OnKillfocusPages() 
{
	//
	// First check to see if the control was losing
	// focus to the cancel button.  If so, we need
	// to return so no updating of validating occurs.
	//
	if (GetFocus() == GetDlgItem(IDCANCEL))
		return ;

	int nOldValue = _ProjectInfo.sDocDesc.nPages ;

	if (UpdateData (TRUE) && nOldValue != _ProjectInfo.sDocDesc.nPages)
	{
		BOOL fErase = nOldValue == 1 || _ProjectInfo.sDocDesc.nPages == 1;
		m_cPreview.Invalidate( fErase );
	}
}

void CProjectSetupDlg::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	ASSERT (pScrollBar) ;	

	//
	// First, determine whether or not the scroll change was
	// accepted.  Note:  UpdateData() will not do this, as 
	// the data will not have changed due to the  spinner 
	// control rejecting the new value.  This test here
	// will prevent unnecessary update messages from being
	// sent to the parent.
	//

	// Only process thumb position messages
	if (nSBCode != SB_THUMBPOSITION)
		return ;

	UINT nCtlID = pScrollBar->GetDlgCtrlID();

	double fValue = 0.0;

	if (dynamic_cast<CBDoubleSpinner *>( pScrollBar ))
		fValue = ((CBDoubleSpinner *) pScrollBar)->GetPosition() ;

	// Check for changes.
	if ((IDC_COLUMNS_CTRL == nCtlID && int(nPos) == _ProjectInfo.sDocDesc.nColumns)	||
		(IDC_PAGES_CTRL   == nCtlID && int(nPos) == _ProjectInfo.sDocDesc.nPages)		||   
		(IDC_TOPSPIN      == nCtlID && fValue	 == m_fTop)								||
		(IDC_BOTTOMSPIN   == nCtlID && fValue	 == m_fLeft)							||   
		(IDC_LEFTSPIN     == nCtlID && fValue	 == m_fRight)							||
		(IDC_RIGHTSPIN    == nCtlID && fValue	 == m_fBottom))   
		return ;

	//
	// Ok, there was actually a change, so update the
	// data and send the data modified message off.
	//
	int nOldValue = _ProjectInfo.sDocDesc.nPages ;

	if (UpdateData (TRUE))
	{
		BOOL fErase = (IDC_PAGES_CTRL == nCtlID && (nOldValue == 1 || _ProjectInfo.sDocDesc.nPages == 1));
		m_cPreview.Invalidate( fErase );
	}
}

void CProjectSetupDlg::OnKillfocusBottommargin() 
{
	//
	// First check to see if the control was losing
	// focus to the cancel button.  If so, we need
	// to return so no updating of validating occurs.
	//
	if (GetFocus() == GetParent()->GetDlgItem(IDCANCEL))
		return ;

	double fOldValue = m_fBottom ;
	
	if (UpdateData (TRUE))
	{
		if (m_fBottom != fOldValue)
			m_cPreview.Invalidate( FALSE );
	}
}

void CProjectSetupDlg::OnKillfocusLeftmargin() 
{
	//
	// First check to see if the control was losing
	// focus to the cancel button.  If so, we need
	// to return so no updating of validating occurs.
	//
	if (GetFocus() == GetParent()->GetDlgItem(IDCANCEL))
		return ;

	double fOldValue = m_fLeft ;
	
	if (UpdateData (TRUE))
	{
		if (m_fLeft != fOldValue)
			m_cPreview.Invalidate( FALSE );
	}
}

void CProjectSetupDlg::OnKillfocusRightmargin() 
{
	//
	// First check to see if the control was losing
	// focus to the cancel button.  If so, we need
	// to return so no updating of validating occurs.
	//
	if (GetFocus() == GetParent()->GetDlgItem(IDCANCEL))
		return ;

	double fOldValue = m_fRight ;
	
	if (UpdateData (TRUE))
	{
		if (m_fRight != fOldValue)
			m_cPreview.Invalidate( FALSE );
	}
}

void CProjectSetupDlg::OnKillfocusTopmargin() 
{
	//
	// First check to see if the control was losing
	// focus to the cancel button.  If so, we need
	// to return so no updating of validating occurs.
	//
	if (GetFocus() == GetParent()->GetDlgItem(IDCANCEL))
		return ;

	double fOldValue = m_fTop ;
	
	if (UpdateData (TRUE))
	{
		if (m_fTop != fOldValue)
			m_cPreview.Invalidate( FALSE );
	}
}

LONG CProjectSetupDlg::OnUpdateData( WPARAM, LPARAM )
{
	InitDocDesc( FALSE );

	CRect rect = GetPrintableArea();
	SetOuterMarginLimits( rect, _ProjectInfo.sDocDesc.szPage );

    m_spinTop.SetRange(m_fMinTop, m_fMax, 0.05);
    m_spinLeft.SetRange(m_fMinLeft, m_fMax, 0.05);
    m_spinRight.SetRange(m_fMinRight, m_fMax, 0.05);
    m_spinBottom.SetRange(m_fMinBottom, m_fMax, 0.05);

    m_spinTop.SetPosition(m_fTop);
    m_spinLeft.SetPosition(m_fLeft);
    m_spinRight.SetPosition(m_fRight);
    m_spinBottom.SetPosition(m_fBottom);

	return 0L;
}
