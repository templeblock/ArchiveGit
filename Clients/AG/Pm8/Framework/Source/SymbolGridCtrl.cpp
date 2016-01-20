// SymbolGridCtrl.cpp : implementation file
//
#include	"FrameworkIncludes.h"

ASSERTNAME

#include "SymbolGridCtrl.h"

#undef	FrameworkLinkage
#define	FrameworkLinkage


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// RSymbolGridCtrl


RSymbolGridCtrl::RSymbolGridCtrl(int numCol, int numRow,
					BYTE charSet, CString fontName)
	:m_symbolChar	(32),
	m_nNumCol		(numCol),
	m_nNumRow		(numRow),
	m_charSet		(charSet)
{
	m_nTotalNumSym = m_nNumCol * m_nNumRow;
    m_fontName = fontName;
	
}

RSymbolGridCtrl::~RSymbolGridCtrl()
{
}


BEGIN_MESSAGE_MAP(RSymbolGridCtrl, CStatic)
	//{{AFX_MSG_MAP(RSymbolGridCtrl)
	ON_WM_PAINT()
	ON_WM_NCHITTEST()
	ON_WM_KEYDOWN( )
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_KILLFOCUS( )
	ON_WM_SETFOCUS( )
	ON_WM_GETDLGCODE( )
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

//*****************************************************************************
//
// Function:		RSymbolGridCtrl::PreSubclassWindow()
//
// Description:    set up the rect and the font
//
// Returns:		   none
//
// Exceptions:	   None
//
//*****************************************************************************
void RSymbolGridCtrl::PreSubclassWindow( )
{
    GetClientRect( &m_gridRect );

    m_rectWidth  = m_gridRect.Width()  / m_nNumCol;
    m_rectHeight = m_gridRect.Height() / m_nNumRow;

    
	LOGFONT     logFont;
    memset(&logFont, 0, sizeof(LOGFONT));

    logFont.lfHeight   = m_rectHeight - 4;
    logFont.lfCharSet  = m_charSet;
    strcpy (logFont.lfFaceName, m_fontName );

    m_symbolFont.CreateFontIndirect( &logFont );

}



/////////////////////////////////////////////////////////////////////////////
// RSymbolGridCtrl message handlers

void RSymbolGridCtrl::OnPaint() 
{
	CPaintDC dc(this);      // DC for painting

    int     i, j, x, y, cx, cy;
    
    cx = (m_gridRect.Width()  - m_nNumCol * m_rectWidth) / 2;
    cy = (m_gridRect.Height() - m_nNumRow * m_rectHeight) / 2;

    int     nChar = 32;

    char    buf[] = "\0\0";

    CFont *  pOldFont = dc.SelectObject( &m_symbolFont );
    dc.SetBkMode( TRANSPARENT );

    for (j = 0; j < m_nNumRow; j++)
    {
        for (i = 0; i < m_nNumCol; i++)
        {
            x = i * m_rectWidth  + m_gridRect.left + cx;
            y = j * m_rectHeight + m_gridRect.top  + cy;

            dc.Draw3dRect( x, y, m_rectWidth-1, m_rectHeight-1,
                           GetSysColor( COLOR_3DDKSHADOW ),
                           GetSysColor( COLOR_3DHILIGHT ));
			
			if (m_charSet == SYMBOL_CHARSET)
				buf[0] = char(nChar);
			else
			{
				if (i == 0 && j == 0)
					buf[0] = 32; // the space character for first box
				else
					buf[0] = (j * m_nNumCol) + i + kSymbolCharStart - 1;	// 161 is the starting char but -1 to adjust for the cell with space character
			}

            CRect   clipRect( x+1, y+1,  x+m_rectWidth-2, y+m_rectHeight-2 );
            m_rectArray[nChar-32] = clipRect;

            nChar++;
            dc.DrawText( buf, 1, &clipRect, DT_CENTER );
        }
    }

    dc.SelectObject( pOldFont );
    HighlightSymbol( m_symbolChar );
	GetParent()->SendMessage( UM_SYMBOL_CHANGED, GetDlgCtrlID(), (LPARAM)m_symbolChar ) ;

	if (GetFocus() == this)
	{
		// Color must be set back before focus is drawn
		dc.SetTextColor(GetSysColor(COLOR_WINDOWTEXT));
		dc.DrawFocusRect( m_gridRect );
	}
	else
	{
		CBrush Brush;
		Brush.CreateSolidBrush( GetSysColor(COLOR_BTNFACE) );
		dc.FrameRect( &m_gridRect, &Brush );
	}


	// Do not call CStatic::OnPaint() for painting messages
}


//*****************************************************************************
//
// Function:		RSymbolGridCtrl::OnNcHitTest()
//
// Description:    Test to see if cursor is in client area
//
// Returns:		   Result of call to base class
//
// Exceptions:	   None
//
//*****************************************************************************
UINT RSymbolGridCtrl::OnNcHitTest( CPoint point )
{
	// Get the actual cursor position.  This is in screen coordinates.  If we
	// try to use the point given, it is old and unreliable.
	CPoint ptCursor;
	::GetCursorPos( &ptCursor );

	CRect   rcWndRect;
	GetClientRect(&rcWndRect);
	ClientToScreen( &rcWndRect );
	
	// Test to see if the cursor is in the client area.
	if(rcWndRect.PtInRect(ptCursor))
		return HTCLIENT;
	
	return CStatic::OnNcHitTest(point);

}
//*****************************************************************************
//
// Function:		RSymbolGridCtrl::OnLButtonDown()
//
// Description:   Override to OnLButtonDown 
//
// Returns:		   Result of call to base class
//
// Exceptions:	   None
//
//*****************************************************************************
void RSymbolGridCtrl::OnLButtonDown(UINT nFlags, CPoint point) 
{
	if (GetFocus() != this)
		SetFocus();

    for (int i=0; i < m_nTotalNumSym; i++)
    {
        if (m_rectArray[i].PtInRect(point))
        {
            HighlightSymbol( i+32 );
			GetParent()->SendMessage( UM_SYMBOL_CHANGED, GetDlgCtrlID(), (LPARAM)m_symbolChar ) ;
        }
    }
}

//*****************************************************************************
//
// Function:		RSymbolGridCtrl::OnLButtonDblClk()
//
// Description:   Override to OnLButtonDown 
//
// Returns:		   Result of call to base class
//
// Exceptions:	   None
//
//*****************************************************************************
void RSymbolGridCtrl::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
    for (int i=0; i < m_nTotalNumSym; i++)
    {
        if (m_rectArray[i].PtInRect(point) && m_symbolChar != 0)
        {
			GetParent()->SendMessage( UM_SYMBOL_DBLCLICKED, GetDlgCtrlID(), 0L ) ;
			return;
        }
    }
    ::MessageBeep((UINT)-1);
}

//*****************************************************************************
//
// Function:		RSymbolGridCtrl::OnSetFocus()
//
// Description:   control has got focus
//
// Returns:		   
//
// Exceptions:	   None
//
//*****************************************************************************
void RSymbolGridCtrl::OnSetFocus(CWnd* /*pOldWnd*/) 
{
	InvalidateRect( m_gridRect, FALSE );
}

//*****************************************************************************
//
// Function:		RSymbolGridCtrl::OnKillFocus()
//
// Description:   control lost focus
//
// Returns:		   
//
// Exceptions:	   None
//
//*****************************************************************************
void RSymbolGridCtrl::OnKillFocus(CWnd* /*pNewWnd*/) 
{
	InvalidateRect( m_gridRect, FALSE );
}

//*****************************************************************************
//
// Function:		RSymbolGridCtrl::OnKeyDown()
//
// Description:   Take the arrow keys and pass them to member func
//
// Returns:		   
//
// Exceptions:	   None
//
//*****************************************************************************
void RSymbolGridCtrl::OnKeyDown( UINT nChar, UINT nRepCnt, UINT nFlags )
{
	switch (nChar)
	{
		case VK_UP:
		case VK_DOWN:
		case VK_LEFT:
		case VK_RIGHT:
			HandleArrowKeys(nChar);
		
		default:
			return;
	}
}


//*****************************************************************************
//
// Function:		RSymbolGridCtrl::HighlightSymbol()
//
// Description:   
//
// Returns:		   
//
// Exceptions:	   None
//
//*****************************************************************************
void RSymbolGridCtrl::HighlightSymbol(int nSymbol)
{
    char buf[] = "\0\0";
 
    CDC * pDC = GetDC();

    CFont * pOldFont = pDC->SelectObject( &m_symbolFont );
    pDC->SetBkMode( TRANSPARENT );
 
	// invert the last cell
    if (m_symbolChar != 0)
    {
        int index = m_symbolChar - 32;
        pDC->FillSolidRect( &m_rectArray[index],
                            GetSysColor( COLOR_BTNFACE ));
 		if (m_charSet == SYMBOL_CHARSET)
			buf[0] = char( m_symbolChar );
		else
		{
			if (index == 0)
				buf[0] = 32; // the space character for first box
			else
				buf[0] = index + kSymbolCharStart - 1;	// 161 is the starting char but -1 to adjust for the cell with space character
		}

        pDC->DrawText( buf, 1, &m_rectArray[index], DT_CENTER );
    }

	// select the new cell
    if (nSymbol != 0)
    {
  		int i = nSymbol - 32;

/*		if (m_charSet == ANSI_CHARSET && i != 0)
		{
			i = kSymbolCharStart - nSymbol + 1;
		}*/

//		ASSERT(i >= 0 && i < m_nTotalNumSym );

        pDC->FillSolidRect( &m_rectArray[i],
                            GetSysColor( COLOR_HIGHLIGHT ));

 		if (m_charSet == SYMBOL_CHARSET)
		{
			buf[0] = char( nSymbol );
		}
		else
		{
			if (i == 0)
				buf[0] = 32; // the space character for first box
			else
				buf[0] = i + kSymbolCharStart - 1;	// 161 is the starting char but -1 to adjust for the cell with space character
		}
		m_symbolChar = nSymbol;

        pDC->SetTextColor( GetSysColor(COLOR_HIGHLIGHTTEXT));
        pDC->DrawText( buf, 1, &m_rectArray[i], DT_CENTER );

    }

	pDC->SelectObject( pOldFont );
	ReleaseDC(pDC);

}

// ****************************************************************************
//
//  Function Name:	RSymbolGridCtrl::OnGetDlgCode
//
//  Description:  	
//
//  Returns:			
//
//  Exceptions:		None
//
// ****************************************************************************
//
UINT RSymbolGridCtrl::OnGetDlgCode( )
{
	return DLGC_WANTARROWS; 
}


// ****************************************************************************
//
//  Function Name:	RSymbolGridCtrl::HandleArrowKeys
//
//  Description: Decide where to go 	
//
//  Returns:			
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RSymbolGridCtrl::HandleArrowKeys( WORD theKey )
{
	int index = m_symbolChar - 32;

	// check out where we are currently before preceding
	int curColumn = index % m_nNumCol;
	int curRow = index / m_nNumCol;

	switch (theKey)
	{
		case VK_UP:
			{
				curRow--;
				if ( curRow < 0 )
					HighlightSymbol( (m_nNumCol * (m_nNumRow-1)) + curColumn + 32);	// take it to last row
				else
					HighlightSymbol( m_symbolChar - m_nNumCol );
			}
			break;

		case VK_DOWN:
			{
				curRow++;
				if ( curRow > m_nNumRow - 1 )
					HighlightSymbol( curColumn + 32 );	// take it to first row
				else
					HighlightSymbol( m_symbolChar + m_nNumCol);
			}
			break;
		
		case VK_LEFT:
			{
				curColumn--;	// decrement
				if ( curColumn < 0 ) // did we go past 1st column?
					HighlightSymbol( m_symbolChar + (m_nNumCol - 1));
				else
					HighlightSymbol( m_symbolChar - 1);
			}
			break;
		
		case VK_RIGHT:
			{
				curColumn++;	// increment
				if ( curColumn > m_nNumCol - 1 ) // did we exceed the bounds?
					HighlightSymbol( m_symbolChar - (curColumn - 1));
				else
					HighlightSymbol( m_symbolChar + 1);

			}
			break;

		default:
			return;
	}
	
	// m_symbolChar gets updated in HighlightSymbol
	GetParent()->SendMessage( UM_SYMBOL_CHANGED, GetDlgCtrlID(), (LPARAM)m_symbolChar ) ;

}