////////////////////////////////////////////////////////////////////////////// 

    (c) Copyright 1994 MICROGRAFX, Inc., All Rights Reserved. This material
    is confidential and a trade secret. Permission to use this work for any
    purpose must be obtained in writing from:

           MICROGRAFX, 1303 E. Arapaho Rd., Richardson, TX  75081

//////////////////////////////////////////////////////////////////////////////

#include "pp.h"
#include "ppafx.h"
#include "id.h"
#include "pptbars.h"
#include "svrview.h"
#include "svrdoc.h"
#include "ctxhelp.h"

extern HINSTANCE hInstHintLib;   
extern BOOL fAppActive;
////////////////////////////////// Constants /////////////////////////////////

// new color metrics
#define COLOR_INFOTEXT			23
#define COLOR_INFOBK			24

#define ERROR_BUFFER_LENGTH     256
#define HINT_BUFFER_LENGTH      256
#define HINT_TITLE_LENGTH       40

#define CX_BUFFER               8
#define CY_BUFFER               6
#define SPACE_BUFFER            8

#define ID_POSTBUBBLE           1
#define START_DELAY             2000  
#define STOP_DELAY              1000

#define RIBBON_ICON_CLASS       2

//////////////////////////////////// Macros //////////////////////////////////
//////////////////////////////////// Types ///////////////////////////////////
///////////////////////////////// Local Data /////////////////////////////////

LOCAL char szBubble[] = "BubbleHelp";      // Class name for bubble hints
BOOL bResult = FALSE;
static HWND LastWnd;

///////////////////////////////// Public Data ////////////////////////////////

CHelpContext HelpContext;

/////////////////////////////// Local Functions //////////////////////////////

LRESULT /* CALFIX _export */ CALLBACK BubbleWndProc( HWND, UINT, WPARAM, LPARAM );
void    /* CALFIX _export */ CALLBACK BubbleTimerProc( HWND, UINT, UINT, DWORD );

//////////////////////////////////////////////////////////////////////////////

LRESULT /* CALFIX _export */ CALLBACK BubbleWndProc( HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam )
//////////////////////////////////////////////////////////////////////////////
//  This is the bubble windowproc. It's sole purpose in life is to display  //
//  the title text in an attractive manner.                                 //
//////////////////////////////////////////////////////////////////////////////
{
    LRESULT     lResult = 0L;
    HDC         hDC;
    PAINTSTRUCT ps;
    RECT        rect;
    char        szTemp[HINT_BUFFER_LENGTH];
    HGDIOBJ     hPrevPen;
    HGDIOBJ     hPrevBrush;    
	COLORREF	oldColor;
    POINT       pt[3];

    switch( Message )
    {
        case WM_PAINT:
            hDC = BeginPaint( hWnd, ( LPPAINTSTRUCT )&ps );
            GetClientRect( hWnd, ( LPRECT )&rect );    
            GetWindowText( hWnd, ( LPTSTR )szTemp, HINT_BUFFER_LENGTH );
            pt[0].x = pt[2].y = 0;
            pt[0].y = pt[1].y = rect.bottom - 1;
            pt[1].x = pt[2].x = rect.right - 1;

            hPrevBrush = SelectObject( hDC, GetStockObject( NULL_BRUSH ));
            SelectObject( hDC, GetStockObject( Window.iLabelFont ));
            Rectangle( hDC, 0, 0, rect.right - 1, rect.bottom - 1 );

		    hPrevPen = SelectObject( hDC, CreatePen( PS_SOLID, 1, 0x00808080L ));
            Polyline( hDC, ( LPPOINT )&pt, 3 );

            SetBkMode( hDC, TRANSPARENT );

			if (Control.WinVer == WV_WINDOWS95)
				oldColor = SetTextColor( hDC, GetSysColor(COLOR_INFOTEXT) );
            TextOut( hDC, CX_BUFFER / 2, CY_BUFFER / 2, 
                ( LPTSTR )szTemp, GetWindowTextLength( hWnd ));
			if (Control.WinVer == WV_WINDOWS95)
				SetTextColor( hDC, oldColor );

            SelectObject( hDC, hPrevBrush );
            DeleteObject( SelectObject( hDC, hPrevPen ));
            EndPaint( hWnd, ( LPPAINTSTRUCT )&ps );
            break;

        default:
            lResult = DefWindowProc( hWnd, Message, wParam, lParam );
            break;
    }
    return( lResult );
}

void /* CALFIX _export */ CALLBACK BubbleTimerProc( HWND hWnd, UINT Message, UINT id, DWORD dwTime )
//////////////////////////////////////////////////////////////////////////////
//  This is the callback for the start/stop bubble mode timer               //
//////////////////////////////////////////////////////////////////////////////
{
    if(( Message == WM_TIMER ) &&( id == HelpContext.TimerID ))
    {
        CPoint pt;
        RECT rect;

        GetCursorPos( &pt );

        if( IsWindow( LastWnd ))
        {
            GetWindowRect( LastWnd, &rect );

            if(( WITHIN( pt.x, rect.left, rect.right )) &&
            ( WITHIN( pt.y, rect.top, rect.bottom )))
            {
                // Create a window
                HelpContext.display_bubble( TRUE, TRUE );
            }
            else
                // kill window, reset clock
                HelpContext.display_bubble( FALSE );
        }
        else
            HelpContext.display_bubble( FALSE );
    }
}

///////////////////////////// CHelpContextMethods ////////////////////////////

//
//  Private methods:
//

void CHelpContext::process_hint( BOOL bLoadString )
//////////////////////////////////////////////////////////////////////////////
//  This function loads the hint string from the hint DLL and parses out    //
//  the title and content components of the hint. The title will be set     //
//  in the title hint window while the content will be set the main hint    //
//  window. These windows are set via the SetHintWindows function. The      //
//  format of the hint is as follows:                                       //
//                                                                          //
//      <title>TITLEBREAK<content>                                          //
//              Display a title and context but no bubble hint window.      //
//                                                                          //
//      <title>HINTBREAK<content>                                           //
//              Display a title and context and use the title in a bubble   //
//              hint window.                                                //
//                                                                          //
//      <title><content>                                                    //
//              Display entire hint string in main( content ) hint window   //
//              using the default application title.                        //
//                                                                          //
//  See above for a current definition of the TITLEBREAK and HINTBREAK      //
//  constants.                                                              //
//////////////////////////////////////////////////////////////////////////////
{
    int iOffset  = 0;

    // reset valid hint attribute.
    m_HintState &= ~HL_BUBBLE_HINTTITLE;    

    // Load up the raw hint string from the DLL.
    if( bLoadString )
    {
        if( hInstHintLib == NULL )
        {
            display_bubble( FALSE );
            return;
        }

        LoadString( hInstHintLib, CurHintID, 
                    CurHintLine.GetBuffer( HINT_BUFFER_LENGTH ), 
                    HINT_BUFFER_LENGTH );
        CurHintLine.ReleaseBuffer();
    }

    if( !CurHintLine.IsEmpty())
    {   
        if(( iOffset = CurHintLine.Find( HINTBREAK )) == -1 ) 
        {
            // No bubble hints here, so delete any bubble hints
            display_bubble( FALSE );

            // Is there an explicit title?
            if(( iOffset = CurHintLine.Find( TITLEBREAK )) != -1 )
            {
                if( CurHintLine.Find( ' ' ) == 0 )  // strip off leading blank
                {
                    CurHintTitle = CurHintLine.Mid( 1 );
                    CurHintTitle.GetBufferSetLength( iOffset -1 );
                }
                else
                {
                    CurHintTitle = CurHintLine;
                    CurHintTitle.GetBufferSetLength( iOffset );
                }
                CurHintContent = CurHintLine.Mid( iOffset + 1 );    
            }    
        }
        else
        {
            // There is a title string, use it as the bubble hint
            if( CurHintLine.Find( ' ' ) == 0 )      // strip off leading blank
            {
                CurHintTitle = CurHintLine.Mid( 1 );                
                CurHintTitle.GetBufferSetLength( iOffset - 1 );
            }
            else
            {
                CurHintTitle = CurHintLine;
                CurHintTitle.GetBufferSetLength( iOffset );         
            }
            CurHintContent = CurHintLine.Mid( iOffset + 1 );        

            m_HintState |= HL_BUBBLE_HINTTITLE;
            display_bubble( TRUE );   
        }
/*
        // Add the accelerator key
        CurHintContent += GetAcceleratorHint( m_CtlId );

        if( pHintName )
            pHintName->SetWindowText(( const char * ) CurHintTitle );

        if( pHintWnd )
            pHintWnd->SetWindowText(( const char * ) CurHintContent );
*/
    }
    else
        display_bubble( FALSE );
}

void CHelpContext::create_bubble( BOOL bTimer )
//////////////////////////////////////////////////////////////////////////////
//  This function will create the bubble hint window. Any existing window   //
//  is destroyed. The size is based on the extent of the title string and   //
//  the position of the window is based on the browsed control window.      //
//  The position is then adjusted if necessary to ensure that it will be    //
//  visible on the screen.                                                  //
//////////////////////////////////////////////////////////////////////////////
{
    RECT  rect;
    HDC   hDC;
    SIZE  size;
    int   x, y, iCtrlExt, iScreenExtent;

    // Destroy the old window.
    if( hBubbleWnd )
    {    
        DestroyWindow( hBubbleWnd );
        hBubbleWnd = NULL;
    }

    // Calculate the size of the window based on the title

    hDC = GetDC( NULL );

    SelectObject( hDC, GetStockObject( Window.iLabelFont ));
    GetTextExtentPoint( hDC, ( const char * )CurHintTitle, 
        CurHintTitle.GetLength(), ( LPSIZE )&size );

    size.cx += CX_BUFFER;
    size.cy += CY_BUFFER;
    ReleaseDC( NULL, hDC );

    // Calculate the X/Y position of the window
    if( !( IsWindow( m_hWndID )))
        return;

    GetWindowRect( m_hWndID, &rect );

    char szClassName[20];
    GetClassName( GetParent( m_hWndID ), szClassName, sizeof( szClassName ));

    // if ribbon or status bar, place hint below or above button
    if(( StringsEqual( szClassName, "ppribbon" )) || 
      (( StringsEqual( szClassName, "ppdialog" )) && ( AstralDlgGet( IDD_STATUS ) == GetParent( m_hWndID ))))
    {
        iCtrlExt = rect.right - rect.left;

        x = max( CX_BUFFER, rect.left -(( size.cx - iCtrlExt ) / 2 ));

        if(( x + size.cx ) >( iScreenExtent = GetSystemMetrics( SM_CXSCREEN )) )
            // Won't fit on screen, move it to the left.
            x = ( iScreenExtent - ( size.cx + CX_BUFFER ));

        y = rect.bottom + SPACE_BUFFER;

        if(( y + size.cy ) >( iScreenExtent= GetSystemMetrics( SM_CYSCREEN )))
        {
            y = rect.top - ( SPACE_BUFFER + size.cy );
        }
    }
    else                // it may be a custom toolbar
    {
        CWnd *pWnd = CWnd::FromHandle( ::GetParent( m_hWndID ));
        if( !pWnd )
            return;

        if( !pWnd->IsKindOf( RUNTIME_CLASS( CPPToolBar )))        
            return;     // no hint window created if not a toolbar

        // locate hint to the left or right of tool
        x = rect.right + CX_BUFFER / 2;

        if(( x + size.cx ) >( iScreenExtent = GetSystemMetrics( SM_CXSCREEN )))
            // Won't fit on screen, move it to the left.
            x = rect.left - ( size.cx + CX_BUFFER / 2 );

        iCtrlExt = rect.bottom - rect.top;
        y = rect.top + (( iCtrlExt - size.cy ) / 2 );   // bubble centered vertically next to button
    }

    hBubbleWnd = ::CreateWindowEx( 0, ( LPTSTR )szBubble, 
        ( const char * )CurHintTitle, WS_POPUP | WS_CHILD, 
        x, y, size.cx, size.cy, ::AfxGetMainWnd()->GetSafeHwnd(), NULL, 
        AfxGetInstanceHandle(), NULL );

    if( hBubbleWnd )
        ShowWindow( hBubbleWnd, SW_SHOWNOACTIVATE );
}

//
//  Public methods:
//

CHelpContext::CHelpContext( 
//////////////////////////////////////////////////////////////////////////////
//  Constructor                                                             //
//////////////////////////////////////////////////////////////////////////////
UINT HelpTopic,
UINT HintID) :
pHintName( NULL ), 
pHintWnd( NULL ), 
pMainWnd( NULL ), 
CurHintID( 0 ), 
CurHelpTopic( 0 ), 
CurrentTopicHelpTopic( 0 ), 
m_HintState( HT_OFF )
{
    CurHintLine = "";
    CurHintTitle = "";
    CurHintContent = "";
    hBubbleWnd = NULL;
    TimerID = 0;
    lpfnBubbleTimerProc = NULL;     
}
    
CHelpContext::~CHelpContext()
//////////////////////////////////////////////////////////////////////////////
// Destructor                                                               //
//////////////////////////////////////////////////////////////////////////////
{
    if( lpfnBubbleTimerProc )
        FreeProcInstance(( FARPROC )lpfnBubbleTimerProc );

    if( TimerID )
        KillTimer( NULL, TimerID );
}

void CHelpContext::InitBubbleHints()
////////    //////////////////////////////////////////////////////////////////////
//  Function will register the window class for the bubble hint window      //
//////////////////////////////////////////////////////////////////////////////
{
    WNDCLASS wc;

    wc.style         = CS_SAVEBITS;
    wc.lpfnWndProc   = BubbleWndProc;
    wc.cbClsExtra    = 0;
    wc.cbWndExtra    = 0;
    wc.hInstance     = AfxGetInstanceHandle();
    wc.hIcon         = NULL;
    wc.hCursor       = LoadCursor( NULL, MAKEINTRESOURCE( IDC_ARROW ));
	if (Control.WinVer == WV_WINDOWS95)
	    wc.hbrBackground = (HBRUSH)(COLOR_INFOBK+1);
	else
	    wc.hbrBackground = CreateSolidBrush( BUBBLE_COLOR );
    wc.lpszMenuName  = NULL;
    wc.lpszClassName =( LPTSTR )szBubble;

    RegisterClass(( LPWNDCLASS )&wc );
}

void CHelpContext::SetHintWindows( CWnd * pName, CWnd * pContent, UINT Type )
//////////////////////////////////////////////////////////////////////////////
//  Turns bubble hints ON/OFF                                               //
//////////////////////////////////////////////////////////////////////////////
{
    if( Type == HT_OFF )
    {    
        m_HintState &= ~HT_TYPE_MASK;   
        pHintName = NULL; 
        pHintWnd  = NULL; 

        CurHintID = 0;
        display_bubble( FALSE );

        if( lpfnBubbleTimerProc )
        {
            if( TimerID )
               KillTimer( NULL, TimerID );
            TimerID = 0;
            
            FreeProcInstance(( FARPROC )lpfnBubbleTimerProc );
            lpfnBubbleTimerProc = NULL;
        }
    }    
    else    // start up hints
    {   
        pHintName = pName; 
        pHintWnd  = pContent; 

        if( CurHintID )
            process_hint();

        lpfnBubbleTimerProc = 
            ( TIMERPROC ) MakeProcInstance(( FARPROC )BubbleTimerProc, AfxGetInstanceHandle());
    }
    m_HintState = ( m_HintState & HL_BUBBLE_MASK ) | Type;
}

void CHelpContext::OnHints( HWND hWnd, UINT HintId )
//////////////////////////////////////////////////////////////////////////////
//  Bubble hint handler. This function is called when windows are browsed   //
//  with the mouse, allowing hint strings to be displayed.                  //
//////////////////////////////////////////////////////////////////////////////
{
	BOOL bInPlaceActive = PictPubApp.IsActiveDocInPlaceActive();

    if( !bInPlaceActive && ( !Hints.fBubbleHintsOn || !fAppActive ))
        return;

    if( CurHintID != HintId )
    {
        LastWnd = m_hWndID = hWnd;
        CurHintID = HintId;

        if( m_HintState & HINTS_ON )
        {
            // parse the string and set the window text
            process_hint();
        }
    }
}

BOOL CHelpContext::BubbleHintsOn( BOOL bState )
//////////////////////////////////////////////////////////////////////////////
//   This function may be used to query the current state of bubble hints   //
//   or to set the new state.                                               //
//////////////////////////////////////////////////////////////////////////////
{
    if( bState != HL_BUBBLE_QUERY )
    {
        if( bState )
        {
            m_HintState |= HL_BUBBLE_ON;
        }
        else
        {
            m_HintState &= ~HL_BUBBLE_ON;
        }
    }    
    return( m_HintState & HL_BUBBLE_ON );
}

void CHelpContext::SetHintState( UINT HintID )
//////////////////////////////////////////////////////////////////////////////
//   Immediately update the displayed hint string with the supplied ID.     //
//////////////////////////////////////////////////////////////////////////////
{
    if( HintID != CurHintID )
    {
        CurHintID = HintID;

        if( m_HintState & HINTS_ON )
        {
            // parse the string and set the window text
            process_hint();
        }
    }
}

void CHelpContext::SetHintState( const char * pString )
//////////////////////////////////////////////////////////////////////////////
//   Immediately update the displayed hint string the supplied string.      //
//////////////////////////////////////////////////////////////////////////////
{
    if( CurHintLine.CompareNoCase( pString ))
    {
        if( m_HintState & HINTS_ON )
        {
            CurHintLine = pString;
            process_hint( FALSE );
        }
    }
}

void CHelpContext::display_bubble( BOOL bDisplay, BOOL bTimer )
//////////////////////////////////////////////////////////////////////////////
//  This function maintains the "state" of bubble hint windows, if they are //
//  enabled. A Windows timer is used to add start and stop delays. The      //
//  start delay causes the bubble to pop up when the mouse lingers above    //
//  a control window beyond the defined timeout. Once enabled popups will   //
//  continue to display until the mouse remains over a non-bubble window    //
//  such as the drawing area for the duration of the stop timeout. The      //
//  delay times are defined as constants above. There are 3 basic states    //
//  and one attribute.                                                      //
//                                                                          //
//  STATES                                                                  //
//      HL_BUBBLE_PENDING       A delay timer has been started but has      //    
//                              not been received. No bubbles are created   //
//                              in this state.                              //
//      HL_BUBBLE_ACTIVE        Bubble hint windows are being shown as      //
//                              encountered. See the process_hint function  //
//                              for criteria for bubble display.            //
//      HL_BUBBLE_SUSPEND       The stop timeout has occurred, but cursor   //
//                              remains over a bubble hintable window.      //
//                                                                          //
//  ATTRIBUTE                                                               //
//      HL_BUBBLE_HINTTITLE     The current contents of the data member     //
//                              CurHintTitle is a valid bubble hint         //
//////////////////////////////////////////////////////////////////////////////
{
    if( !( m_HintState & HL_BUBBLE_ON ))
        return;

    if( bDisplay )
    {
        if( m_HintState & HL_BUBBLE_ACTIVE )    // bubble hint displayed
        {
            if( bTimer )    
            {
                // Bubble hints are currently active and the exit
                // mode timer has been received. Kill the timer and
                // reset the mode.
                m_HintState &= ~HL_BUBBLE_ACTIVE;
                m_HintState &= ~HL_BUBBLE_PENDING;

                if( TimerID )
                {
                    bResult = KillTimer( NULL, TimerID );
                    ASSERT( bResult );
                    TimerID = 0;
                }

                // If a bubble hint is currently being displayed,  
                // set the suspend state.
                if( hBubbleWnd )
                    m_HintState |= HL_BUBBLE_SUSPEND;
                else
                    m_HintState &= ~HL_BUBBLE_SUSPEND;
            }
            else
            {
                // Kill the exit mode timer.
                if( TimerID )
                {
                    bResult = KillTimer( NULL, TimerID );
                    ASSERT( bResult );
                    TimerID = 0;
                }
                create_bubble();
                m_HintState |= HL_BUBBLE_ACTIVE;

                // Restart the exit mode timer.
                TimerID = SetTimer( NULL, ID_POSTBUBBLE, STOP_DELAY, 
                    lpfnBubbleTimerProc );
                ASSERT( TimerID );
            }
        }
        else 
        if( m_HintState & HL_BUBBLE_PENDING ) 
        {
            // Stop the pending timer.
            if( TimerID )
            {
                bResult = KillTimer( NULL, TimerID );
                ASSERT( bResult );
                TimerID = 0;
            }

            if( bTimer && ( m_HintState & HL_BUBBLE_HINTTITLE ))
            {
                // Timer expired, display the bubble hint.

                // Go into active mode
                m_HintState &= ~HL_BUBBLE_PENDING;  // reset pending bit
                create_bubble();
                m_HintState |= HL_BUBBLE_ACTIVE;    // set active bit

                // Start the exit timer.
                TimerID = SetTimer( NULL, ID_POSTBUBBLE, STOP_DELAY, 
                    lpfnBubbleTimerProc );
                ASSERT( TimerID );
            }
            else
            {
                // A new window has been browsed. Restart the delay timer
                // and stay in the pending mode.
                TimerID = SetTimer( NULL, ID_POSTBUBBLE, START_DELAY, lpfnBubbleTimerProc );
                ASSERT( TimerID );
            }
        }
        else if( m_HintState & HL_BUBBLE_SUSPEND ) 
        {
            if( bTimer && TimerID )
            {
                bResult = KillTimer( NULL, TimerID );
                ASSERT( bResult );
                TimerID = 0;
            }

            // Go back into active mode
            m_HintState &= ~HL_BUBBLE_SUSPEND;
            create_bubble();
            m_HintState |= HL_BUBBLE_ACTIVE;

            // Restart the exit bubble mode timer.
            TimerID = SetTimer( NULL, ID_POSTBUBBLE, STOP_DELAY, 
                lpfnBubbleTimerProc );
            ASSERT( TimerID );
        }
        else
        {
            // Start the delay timer to display bubbles 
            m_HintState |= HL_BUBBLE_PENDING;
            TimerID = SetTimer( NULL, ID_POSTBUBBLE, START_DELAY, lpfnBubbleTimerProc );
            ASSERT( TimerID );
        }
    }
    else
    {
        // reset hint data strings
        CurHintLine = "";
        CurHintTitle = "";
        CurHintContent = "";
        CurHintID = 0;

        // Remove the current hint popup window.
        if( hBubbleWnd )
        {    
            DestroyWindow( hBubbleWnd );
            hBubbleWnd = NULL;
        }

        if( !( m_HintState & HL_BUBBLE_ACTIVE ))
        {
            if( TimerID )
            {
                bResult = KillTimer( NULL, TimerID );
                ASSERT( bResult );
                TimerID = 0;
            }

            // Start the delay timer to display bubbles 
            m_HintState &= ~HL_BUBBLE_SUSPEND; 
            m_HintState |= HL_BUBBLE_PENDING;
            TimerID = SetTimer( NULL, ID_POSTBUBBLE, START_DELAY, lpfnBubbleTimerProc );
            ASSERT( TimerID );
        }
    }
}

/////////////////////////////// Public Functions /////////////////////////////

HWND MyChildWindowFromPoint (
HWND hParent,   // Parent of children to search thru.
POINT point )   // Point in screen coordinates.
//////////////////////////////////////////////////////////////////////////////
//  This differs from the SDK function in that it will look through windows //
//  that indicate that they are transparent. Also the point is in screen    //
//  coordinates rather than in parent client coordinates. This is needed    //
//  because of the frame and group box classes that may be the first child  //
//  in the list but have other siblings within their borders.               //
//////////////////////////////////////////////////////////////////////////////
{      
    BOOL bFound = FALSE;
    HWND hChild = GetWindow( hParent, GW_CHILD );
    RECT rChild;
    
    while( IsWindow( hChild ) && GetParent( hChild ) == hParent )
    {
        GetWindowRect( hChild, &rChild );

        if( PtInRect( &rChild, point ))
        {
            long lResult;
            
            lResult = ::SendMessage ( hChild, WM_NCHITTEST, 0,
                MAKELONG( point.x, point.y ));

            if( lResult != HTTRANSPARENT )
            {
                bFound = TRUE;
                break;
            }
        }
        hChild = GetWindow ( hChild, GW_HWNDNEXT );
    }
    return bFound ? hChild : NULL;
}



