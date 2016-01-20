/****************************************************************************
 
	(c) Copyright 1993 MICROGRAFX, Inc., All Rights Reserved.  This material is
   	confidential and a trade secret.  Permission to use this work for any purpose
   	must be obtained in writing from MICROGRAFX, 1303 E Arapaho Rd.,
   	Richardson, Tx.  75081.

*****************************************************************************
*****************************************************************************

                                     TextIME

*****************************************************************************
****************************************************************************/

#include "pp.h"
#include "ppafx.h"
#include "id.h"
#include "ime.h"
#include "textime.h"

// Global Data
extern HINSTANCE hInstIMELib;
TEXTIME TextIME;	// contains handle to global memory block

/***************************************************************************/
DWORD FAR PASCAL ImeMessage( unsigned Msg, UINT wParam, long lParam )
/***************************************************************************/
{
	if( !hInstIMELib )
		return( 0 );

    DWORD dwReturn = 0L;
	LPTSTR lpStr;
	WORD wChar;
	
    switch( Msg )
    {
        case WM_IME_REPORT:
            switch( wParam )
            {
                case IR_OPENCONVERT:
                    ImeSetFont( &TextIME );
                    break;

                case IR_STRING:
				case IR_CLOSECONVERT: 
				case IR_STRINGSTART:  
				case IR_STRINGEND:    
				case IR_CHANGECONVERT:
				case IR_FULLCONVERT:  
				case IR_IMESELECT:    
				case IR_DBCSCHAR:        // New for 3.1
				case IR_UNDETERMINE:     // New for 3.1
				case IR_STRINGEX:        // New for 3.1
                default:
                    break;
            }
            break;

        default:
            break;
    }
    return( dwReturn );
}

/************************************************************************/
BOOL FAR PASCAL SetupIME( HWND hWnd, LPLOGFONT lpLogFont )
/************************************************************************/
{
	ImeInit( &TextIME, hWnd );
    ImeCreateFont( &TextIME, lpLogFont );
	return( TRUE );
}

/************************************************************************/
BOOL FAR PASCAL TerminateIME( void )
// put it back like you found it
/************************************************************************/
{
	ImeMoveConvertWin( &TextIME, -1, -1 ); 	 
	ImeSendVKey( &TextIME, VK_DBE_FLUSHSTRING );
    ImeCreateFont( &TextIME, NULL );
	ImeTerminate( &TextIME );
	return( TRUE );
}

/***************************************************************************/
void FAR PASCAL ImeInit( LPTEXTIME lpTextIME, HWND hWnd )
// Called at the beginning of an edit session to take control of the IME.
// User's IME settings are saved now for restoration.
/***************************************************************************/
{
	if( !hInstIMELib )
		return;

	LPIMESTATE  lpIMEState;
	LPIMESTRUCT lpIME;
    MESSAGEPROC	lpfnMessageProc = ( MESSAGEPROC )GetProcAddress( hInstIMELib, "SendIMEMessageEx" );

    if( !lpTextIME->hIMEState )
    {
        // Allocate the IME state structure.
        lpTextIME->hIMEState = GlobalAlloc( GHND | GMEM_DDESHARE | GMEM_ZEROINIT, 
										  ( LONG )sizeof( IMESTATE ));
    }

    if( lpIMEState = ( LPIMESTATE )GlobalLock( lpTextIME->hIMEState ))
    {
        if( lpIMEState->hIME )
            GlobalFree( lpIMEState->hIME );

        lpIMEState->hIMEWnd   	= hWnd;
        lpIMEState->hIME      	= GlobalAlloc( GHND | GMEM_DDESHARE | GMEM_ZEROINIT, 
											 ( LONG )sizeof( IMESTRUCT ));
        lpIMEState->lIMEParam 	= MAKELPARAM( lpIMEState->hIME, 0 );
        lpIMEState->wCharSet = IME_MODE_UNINITIALIZED;

        if( lpIME = ( LPIMESTRUCT )GlobalLock( lpIMEState->hIME )) 
        {
			// save current mode at startup
            lpIME->fnc = IME_GETCONVERSIONMODE;
			lpTextIME->wPrevIMEMode = lpIMEState->wPrevIMEMode = ( WORD )(( *lpfnMessageProc )( lpIMEState->hIMEWnd, lpIMEState->lIMEParam ));

            if( IS_MODE_JIS( lpTextIME->wPrevIMEMode ))
            {
                // We're starting with a non-single-byte-alphanumeric mode;
                // save it as the last one used.
                lpTextIME->wLastJISMode = lpTextIME->wPrevIMEMode;
            }
            else
            {
                // Choose a default JIS mode
                lpTextIME->wLastJISMode = IME_MODE_HIRAGANA
                                        | IME_MODE_DBCSCHAR
                                        | IME_MODE_ROMAN
                                        | IME_MODE_NOCODEINPUT;
            }
            GlobalUnlock( lpIMEState->hIME );
        }
        GlobalUnlock( lpTextIME->hIMEState );
    }
}

/***************************************************************************/
void FAR PASCAL ImeTerminate( LPTEXTIME lpTextIME )
// Destroy memory objects and freeup memory.
/***************************************************************************/
{
	if( !hInstIMELib )
		return;

    LPIMESTATE lpIMEState;

    if(( lpTextIME ) && ( lpTextIME->hIMEState ) &&
         ( lpIMEState = ( LPIMESTATE )GlobalLock( lpTextIME->hIMEState )))
    {
        if( lpIMEState->hIME )
            lpIMEState->hIME = GlobalFree( lpIMEState->hIME );

        if( lpIMEState->hLastIMEFont )
            DeleteObject( lpIMEState->hLastIMEFont );

        if( lpIMEState->hIMEFont )
            DeleteObject( lpIMEState->hIMEFont );

        GlobalUnlock( lpTextIME->hIMEState );
        GlobalFree( lpTextIME->hIMEState );
        lpTextIME->hIMEState = NULL;
    }
}

/***************************************************************************/
void FAR PASCAL ImeMoveConvertWin( LPTEXTIME lpTextIME, int x, int y )
// Move IME window to the specified location. If x == -1 and y == -1, 
// IME window is restored to its default position.
/***************************************************************************/
{
	if( !hInstIMELib || !lpTextIME->hIMEState )
		return;

    LPIMESTATE  lpIMEState;
    LPIMESTRUCT lpIME;
    MESSAGEPROC	lpfnMessageProc = ( MESSAGEPROC )GetProcAddress( hInstIMELib, "SendIMEMessageEx" );

    if( lpIMEState = ( LPIMESTATE )GlobalLock( lpTextIME->hIMEState ))
    {
        if( lpIME = ( LPIMESTRUCT )GlobalLock( lpIMEState->hIME )) 
        {
            lpIME->fnc = IME_SETCONVERSIONWINDOW;
            if( x == -1 && y == -1 )
                lpIME->wParam = MCW_DEFAULT;
            else
            {
                lpIME->wParam  = MCW_WINDOW;
                lpIME->lParam1 = MAKELPARAM( x, y );
            }
            GlobalUnlock( lpIMEState->hIME );

			( *lpfnMessageProc)( lpIMEState->hIMEWnd, lpIMEState->lIMEParam );
        }
        GlobalUnlock( lpTextIME->hIMEState );
    }
    return;
}

/***************************************************************************/
WORD FAR PASCAL ImeSendVKey( LPTEXTIME lpTextIME, WORD vKey )
// Send escapes to the IME
/***************************************************************************/
{
	if( !hInstIMELib || !lpTextIME->hIMEState )
		return( 0 );

    LPIMESTATE  lpIMEState;
    LPIMESTRUCT lpIME;
    MESSAGEPROC	lpfnMessageProc = ( MESSAGEPROC )GetProcAddress( hInstIMELib, "SendIMEMessageEx" );

    if( lpIMEState = ( LPIMESTATE )GlobalLock( lpTextIME->hIMEState ))
    {
        if( lpIME = ( LPIMESTRUCT )GlobalLock( lpIMEState->hIME )) 
        {
            lpIME->fnc = IME_SENDVKEY;
            lpIME->wParam = vKey;
            GlobalUnlock( lpIMEState->hIME );

			( *lpfnMessageProc )( lpIMEState->hIMEWnd, lpIMEState->lIMEParam );
        }
        GlobalUnlock( lpTextIME->hIMEState );
    }
    return( 0 );
}

/***************************************************************************/
WORD FAR PASCAL ImeSetMode( LPTEXTIME lpTextIME, int tmCharSet )
// Set IME mode based on current font. 
/***************************************************************************/
{
	if( !hInstIMELib || !lpTextIME->hIMEState )
		return( 0 );

    LPIMESTATE  lpIMEState;
    LPIMESTRUCT lpIME;
    WORD wIMEMode = IME_MODE_NOCODEINPUT | IME_MODE_ROMAN;
    MESSAGEPROC	lpfnMessageProc = ( MESSAGEPROC )GetProcAddress( hInstIMELib, "SendIMEMessageEx" );

    if( lpIMEState = ( LPIMESTATE )GlobalLock( lpTextIME->hIMEState ))
    {
	    if(( tmCharSet == SHIFTJIS_CHARSET )
            == ( lpIMEState->wCharSet == SHIFTJIS_CHARSET )
	        && lpIMEState->wCharSet != IME_MODE_UNINITIALIZED )
        {
            // Character set has not changed, bail out
            wIMEMode = lpIMEState->wPrevIMEMode;
            GlobalUnlock( lpTextIME->hIMEState );
            return( wIMEMode );
        }

		ImeSendVKey( lpTextIME, VK_DBE_FLUSHSTRING );

        switch( tmCharSet )
        {

            case SHIFTJIS_CHARSET:
                wIMEMode = lpTextIME->wLastJISMode;
                break;

            case HANGEUL_CHARSET:
            case CHINESEBIG5_CHARSET: 
                // This values may may require changes for Chinese, Korean, 
                // and other far east versions.
                wIMEMode |= IME_MODE_DBCSCHAR | IME_MODE_KATAKANA;
                break;

            case ANSI_CHARSET:
            case SYMBOL_CHARSET:
            case OEM_CHARSET:
                // Set to roman SBCS mode
                wIMEMode |= IME_MODE_SBCSCHAR | IME_MODE_ALPHANUMERIC;
                break;

            default:
                wIMEMode |= IME_MODE_DBCSCHAR | IME_MODE_HIRAGANA;
                break;
        }

        if( lpIME = ( LPIMESTRUCT )GlobalLock( lpIMEState->hIME )) 
        {
            _fmemset( lpIME, 0, sizeof( IMESTRUCT ));
            lpIME->fnc    = IME_SETCONVERSIONMODE;
            lpIME->wParam = wIMEMode;

            if( wIMEMode = ( WORD )(( *lpfnMessageProc )( lpIMEState->hIMEWnd, lpIMEState->lIMEParam )))
            {
                if( IS_MODE_JIS( wIMEMode ))
                    lpTextIME->wLastJISMode = wIMEMode;

                lpIMEState->wPrevIMEMode = wIMEMode;
   	            lpIMEState->wCharSet = tmCharSet;
            }
            GlobalUnlock( lpIMEState->hIME );
        }
        GlobalUnlock( lpTextIME->hIMEState );
    }
    return( wIMEMode );
}

/***************************************************************************/
WORD FAR PASCAL ImeGetMode( LPTEXTIME lpTextIME )
/***************************************************************************/
{
	if( !hInstIMELib || !lpTextIME->hIMEState )
		return 0;

	LPIMESTATE  lpIMEState;
	LPIMESTRUCT lpIME;
	DWORD wReturn = 0;
    MESSAGEPROC	lpfnMessageProc = ( MESSAGEPROC )GetProcAddress( hInstIMELib, "SendIMEMessageEx" );

    if( lpIMEState = ( LPIMESTATE )GlobalLock( lpTextIME->hIMEState ))
    {
        lpIMEState->lIMEParam 	= MAKELPARAM( lpIMEState->hIME, 0 );
        if( lpIME = ( LPIMESTRUCT )GlobalLock( lpIMEState->hIME )) 
        {
            lpIME->fnc = IME_GETCONVERSIONMODE;

			wReturn = ( WORD )(( *lpfnMessageProc )( lpIMEState->hIMEWnd, lpIMEState->lIMEParam ));

            GlobalUnlock( lpIMEState->hIME );
        }
        GlobalUnlock( lpTextIME->hIMEState );
    }
	return( wReturn );
}

/***************************************************************************/
void FAR PASCAL ImeCreateFont( LPTEXTIME lpTextIME, LPLOGFONT lpLogFont )
/***************************************************************************/
{
	if( !hInstIMELib || !lpTextIME->hIMEState )
		return;

    LPIMESTATE  lpIMEState;
    LPIMESTRUCT lpIME;
    MESSAGEPROC	lpfnMessageProc = ( MESSAGEPROC )GetProcAddress( hInstIMELib, "SendIMEMessageEx" );

    if( lpIMEState = ( LPIMESTATE )GlobalLock( lpTextIME->hIMEState ))
    {
        if( lpIME = ( LPIMESTRUCT )GlobalLock( lpIMEState->hIME )) 
        {
            _fmemset( lpIME, 0, sizeof( IMESTRUCT ));
            lpIME->fnc = IME_SETCONVERSIONFONT;

            if( lpLogFont )
            {
                lpIMEState->hLastIMEFont = lpIMEState->hIMEFont;

                // Create a new font to set in the IME.
                lpIMEState->hIMEFont = CreateFontIndirect( lpLogFont );
				lpIME->wParam = ( WPARAM )( lpIMEState->hIMEFont );
				if( *lpLogFont->lfFaceName )
	                ImeSetMode( lpTextIME, lpLogFont->lfCharSet );
            }
            else
            {
                // Restore the system defaults.
                lpIMEState->wPrevIMEMode = lpTextIME->wLastJISMode;
				lpIME->wParam = ( WPARAM )GetStockObject( SYSTEM_FONT );
				ImeSetMode( lpTextIME, SHIFTJIS_CHARSET );
            }

            if((( *lpfnMessageProc )( lpIMEState->hIMEWnd, lpIMEState->lIMEParam )))
            {
                if( lpIMEState->hLastIMEFont )
                {
                    DeleteObject( lpIMEState->hLastIMEFont );
                    lpIMEState->hLastIMEFont = NULL;
                }
            }
            GlobalUnlock( lpIMEState->hIME );
        }
        GlobalUnlock( lpTextIME->hIMEState );
    }
}

/***************************************************************************/
void FAR PASCAL ImeSetFont( LPTEXTIME lpTextIME )
/***************************************************************************/
{
	if( !hInstIMELib || !lpTextIME->hIMEState )
		return;

	LPIMESTATE  lpIMEState;
    LPIMESTRUCT lpIME;
    MESSAGEPROC	lpfnMessageProc = ( MESSAGEPROC )GetProcAddress( hInstIMELib, "SendIMEMessageEx" );

    if( lpIMEState = ( LPIMESTATE )GlobalLock( lpTextIME->hIMEState ))
    {
        if( lpIMEState->hIMEFont && ( lpIME = ( LPIMESTRUCT )GlobalLock( lpIMEState->hIME )) ) 
        {
            _fmemset( lpIME, 0, sizeof( IMESTRUCT ));
            lpIME->fnc = IME_SETCONVERSIONFONT;
            lpIME->wParam = ( WPARAM )lpIMEState->hIMEFont;
            ( *lpfnMessageProc )( lpIMEState->hIMEWnd, lpIMEState->lIMEParam );
            GlobalUnlock( lpIMEState->hIME );
        }
        GlobalUnlock( lpTextIME->hIMEState );
    }
}

