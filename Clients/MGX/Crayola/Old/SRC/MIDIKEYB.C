
/** midikeyb.c
 *
 *  DESCRIPTION: 
 *      This is a custom control that implements a MIDI style keyboard.
 *      It's not exactly the best keyboard for a jam session; don't expect
 *      to hit a bunch of hemidemisemiquavers at correct tempo...
 *
 *      In any case, this custom control responds to and sends MIDI short
 *      messages.  This could be expanded on, but this is a good start.
 *      The right mouse button is used for 'sticky keys' and will toggle
 *      the state appropriately.  If you drag the pointer off of a key
 *      with the left mouse button pressed, you will notice that the key
 *      stays down.  This is because I do not SetCapture--and I should.
 *      Another enhancement might be to add CTRL key and SHIFT key 
 *      modifiers to allow key selection like a multi-select list box.
 *
 *      Before you can use this control, you MUST first export the window
 *      procedure for the control:
 *
 *          EXPORTS     midiKeyBProc
 *
 *      You then need initialize the class before you use it:
 *
 *          if ( !midiKeyBInit( hInst ) )
 *              die a horrible death
 *          else
 *              you are good to go
 *
 *      The colors used by the control default to black and white (black
 *      for the accidental keys and white for the normal keys).  This should
 *      work just dandy on all displays.
 *
 *      To select your own colors, you can send the KEYB_SETFGCOLOR and
 *      KEYB_SETBKCOLOR messages to set the foreground (accidental) and
 *      background (normal) key colors.  The lParam is the RGB() value--
 *      wParam is a BOOL telling the control to repaint immediately if
 *      set to TRUE.
 *
 *      The layout of the keyboard is set using the MIDI key values with
 *      C0 being MIDI note number 0, C2 is 48 (middle C), etc.  The default
 *      layout is: start = 48 (middle C), for 36 keys (3 octaves), ending
 *      on 83.  I chose this by throwing darts at my MIDI poster...
 *
 *      You can set the layout to whatever you want by sending the 
 *      KEYB_SETLAYOUT message to the control.  The HIWORD() of lParam
 *      is the starting key, the LOWORD() of lParam is the number of keys.
 *      wParam is a BOOL telling the control to repaint immediately if
 *      set to TRUE (actually non-zero...).  You can get the current 
 *      layout by sending the KEYB_GETLAYOUT message--the LONG return
 *      value can be decoded as the lParam of KEYB_SETLAYOUT is encoded.
 *
 *      You also have the option of having labels printed on the keys
 *      for MIDI note referencing.  Set the KEYBS_LABELS style flag to
 *      get labels.
 *      
 *      The font used for the label text can be set using the standard
 *      WM_SETFONT message.  You can get the current font at any time with
 *      the WM_GETFONT message.
 *
 *      For other messages and the documentation, see the header block
 *      for midiKeyBProc at the end of this source file.
 *
 *
 **/
// COPYRIGHT:
//
//   (C) Copyright Microsoft Corp. 1993.  All rights reserved.
//
//   You have a royalty-free right to use, modify, reproduce and
//   distribute the Sample Files (and/or any modified version) in
//   any way you find useful, provided that you agree that
//   Microsoft has no warranty obligations or liability for any
//   Sample Application Files which are modified.
//


/* get the includes we need */
#include <windows.h>
#include "id.h"
#include "data.h"
#include "routines.h"
#include "app.h"
#include "midikeyb.h"


/* static global variables */
    static char gszmidiKeyBClass[] = "midiKeyB";

    typedef HANDLE      HMIDIKEYB; 


/* I wonder why this isn't defined? */
#ifndef MAKEWORD
    #define MAKEWORD(bLow, bHigh)   ((WORD)((bHigh<<8)|bLow))
#endif


/* for debuggin' */
    #ifdef DEBUG
        #define D(x)    {x;}
        #define ODS(x)  OutputDebugString(x)
    #else
        #define D(x)
        #define ODS(x)
    #endif


/* window word position definitions */
    #define KEYB_WW_HMIDIKEYB   0
    #define KEYB_WW_EXTRABYTES  2

    #define KEYB_GMEM_FLAGS     (GMEM_MOVEABLE | GMEM_ZEROINIT)


/* defaults for midiKeyB controls */
    #define KEYB_DEF_NUMKEYS    36  /* total number of keys             */
    #define KEYB_DEF_FIRSTKEY   48  /* 60 is middle C (inc semi-tones)  */
    #define KEYB_DEF_VELOCITY   48  /* default note velocity            */
    #define KEYB_DEF_BKCOLOR    RGB( 255, 255, 255 )
    #define KEYB_DEF_FGCOLOR    RGB( 0, 0, 0 )

    #define KEYB_MAX_KEYS       128 /* maximum number of MIDI notes     */


/* typedefs for the keyboard */
    typedef struct tKEYBKEY
    {
        RECT        rc;             /* the key's area                   */
        BYTE        bMIDINote;      /* which midi key number            */
        BYTE        bNote;          /* which (alphabetical) note        */
        BOOL        fNoteOn;        /* is the note on?                  */

    } KEYBKEY, *PKEYBKEY, FAR *LPKEYBKEY;

    typedef struct tMIDIKEYB
    {
        BYTE        bNumKeys;       /* number of keys on the keyboard   */
        BYTE        bFirstKey;      /* first key on the keyboard        */
        BYTE        bWhiteKeys;     /* number of white keys             */
        BYTE        bLastNote;      /* last note played                 */
        BYTE        bChannel;       /* channel keyboard is assigned to  */
        BYTE        bVelocity;      /* velocity for keys                */
        WORD        wTMHeight;      /* height from textmetric struct    */
        WORD        wTMAvgWidth;    /* width from textmetric struct     */
        WORD        wKeyAdd;        /* fudge factor                     */
        DWORD       rgbFgColor;     /* foreground color                 */
        DWORD       rgbBkColor;     /* background color                 */
        HANDLE      hMidiOut;       /* midi output handle for msg       */
        HWND        hWnd;           /* optional window handle to post   */
        WORD        wWidth;         /* client window width              */
        WORD        wHeight;        /* client window height             */
        WORD        wMsgDown;       /* message to send on key down event*/
        WORD        wMsgUp;         /* message to send on key up event  */
        HFONT       hFont;          /* font to use for labels           */
        KEYBKEY     rgKeyBKeys[ KEYB_MAX_KEYS ];

    } MIDIKEYB, *PMIDIKEYB, FAR *LPMIDIKEYB;


/* internal function prototypes */
    long FAR PASCAL midiKeyBProc( HWND, UINT, WPARAM, LPARAM );
    void NEAR PASCAL midiKeyBPaint( HWND hWnd, HDC hDC );
    BYTE NEAR PASCAL midiKeyBGetNote( LPMIDIKEYB lpKeyB, LONG pos );
    BYTE NEAR PASCAL midiKeyBNormalKey( BYTE bKey );
    void NEAR PASCAL midiKeyBNoteChanged( HWND hWnd, BYTE bNote );
    void NEAR PASCAL midiKeyBNoteOn( HWND hWnd, BYTE bNote );
    void NEAR PASCAL midiKeyBNoteOff( HWND hWnd, BYTE bNote );



//************************************************************************
/** BOOL FAR PASCAL midiKeyBInit( HANDLE hInst )
 *
 *  DESCRIPTION: 
 *      Registers the window class for the midiKeyB control.  This must
 *      be done before the midiKeyB control is used--or it will fail
 *      and your dialog box will not open!
 *
 *  ARGUMENTS:
 *      HANDLE hInst    :   Instance handle to register class with.
 *
 *  RETURN (BOOL):
 *      The return value is TRUE if the midiKeyB class was successfully
 *      registered.  It is FALSE if the initialization failed.
 *
 *  NOTES:
 *
 **/

#ifndef DEBUG
    /* codeview pukes if you have this in--why? */
#ifdef WHEN_CODEVIEW_STOPS_PUKING	
    #pragma alloc_text( init, midiKeyBInit )
#endif
#endif

//************************************************************************
BOOL FAR PASCAL midiKeyBInit( HANDLE hInst )
//************************************************************************
{
    static BOOL fRegistered = FALSE;
    WNDCLASS    rClass;
    
    /* assume already registered if not first instance */
    if ( fRegistered )
        return ( TRUE );

    /* fill in the class structure for the midiKeyB control */
    if ( !(rClass.hCursor = LoadCursor( (HINSTANCE)hInst, "CURSOR_MIDIKEYB" )) )
        rClass.hCursor = LoadCursor( NULL, IDC_UPARROW );

    rClass.hIcon        = NULL;
    rClass.lpszMenuName = NULL;
    rClass.lpszClassName= gszmidiKeyBClass;
    rClass.hbrBackground= (HBRUSH)GetStockObject( WHITE_BRUSH );
    rClass.hInstance    = (HINSTANCE)hInst;

#ifdef MIDIKEYB_DLL
    rClass.style        = CS_HREDRAW | CS_VREDRAW | CS_GLOBALCLASS;
#else
    rClass.style        = CS_HREDRAW | CS_VREDRAW;
#endif

    rClass.lpfnWndProc  = midiKeyBProc;
    rClass.cbClsExtra   = 0;
    rClass.cbWndExtra   = KEYB_WW_EXTRABYTES;

    /* attempt to register it--create abs Boolean in fRegistered */
    return ( fRegistered = (RegisterClass( &rClass ) != NULL) );
} /* midiKeyBInit() */


//************************************************************************
/** BYTE NEAR PASCAL midiKeyBNormalKey( BYTE bKey )
 *
 *  DESCRIPTION: 
 *      This function returns the normal (white) key name (alphabetic).
 *      All accidental (black) keys return 0.
 *
 *  ARGUMENTS:
 *      BYTE bKey           :   The key in question.
 *
 *  RETURN (BYTE):
 *      If the key is a normal key (white), then its alphabetic name is 
 *      returned (character).  If the key is an accidental key, then 0 is
 *      returned.
 *
 *  NOTES:
 *
 **/

//************************************************************************
BYTE NEAR PASCAL midiKeyBNormalKey( BYTE bKey )
//************************************************************************
{
    /* assumes C0 is 0, ..., C4 is 60 (middle C)... */
    switch ( bKey % 12 )
    {
        case 0:
            return ( 'C' );
        case 2:
            return ( 'D' );
        case 4:
            return ( 'E' );
        case 5:
            return ( 'F' );
        case 7:
            return ( 'G' );
        case 9:
            return ( 'A' );
        case 11:
            return ( 'B' );
    }

    /* not a normal key--must be accidental */
    return ( 0 );
} /* midiKeyBNormalKey() */


//************************************************************************
/** void NEAR PASCAL midiKeyBNoteOn( HWND hWnd, BYTE bNote )
 *
 *  DESCRIPTION: 
 *      This function is used to turn on a MIDI note.  This includes sending
 *      a message to the parent window and setting the state of the key to
 *      ON.
 *
 *  ARGUMENTS:
 *      HWND hWnd   :   Handle to control's window.
 *
 *      BYTE bNote  :   The note to send the 'ON' or 'DOWN' message for.
 *
 *  RETURN (void):
 *      The note will have been turned on.
 *
 *  NOTES:
 *
 **/

//************************************************************************
void NEAR PASCAL midiKeyBNoteOn( HWND hWnd, BYTE bNote )
//************************************************************************
{
    HMIDIKEYB    hKeyB;
    LPMIDIKEYB   lpKeyB;

    /* get the handle to the keyboard's control block */
    hKeyB = (HMIDIKEYB)GetWindowWord( hWnd, KEYB_WW_HMIDIKEYB );

    /* get pointer to the control's control block */
    if ( !(lpKeyB = (LPMIDIKEYB)GlobalLock( hKeyB )) )
        return;

    /* set the current key state */
    lpKeyB->bLastNote = bNote;

    /* don't send note on message if the key is already down */
    if ( !lpKeyB->rgKeyBKeys[ bNote ].fNoteOn )
    {
        lpKeyB->rgKeyBKeys[ bNote ].fNoteOn = TRUE;

        /* determine window to send message to */
        hWnd = (lpKeyB->hWnd) ? lpKeyB->hWnd : GetParent( hWnd );

        SendMessage( hWnd, lpKeyB->wMsgDown, (WPARAM)lpKeyB->hMidiOut,
                    MAKELONG( MAKEWORD( (BYTE)0x90 + lpKeyB->bChannel, bNote ),
                            MAKEWORD( lpKeyB->bVelocity, 0 ) ) );
    }

    /* unlock it */
    GlobalUnlock( hKeyB );
} /* midiKeyBNoteOn() */


//************************************************************************
/** void NEAR PASCAL midiKeyBNoteOff( HWND hWnd, BYTE bNote )
 *
 *  DESCRIPTION: 
 *      This function is used to turn off a MIDI note.  This includes sending
 *      a message to the parent window and setting the state of the key to
 *      OFF.
 *
 *  ARGUMENTS:
 *      HWND hWnd   :   Handle to control's window.
 *
 *      BYTE bNote  :   The note to send the 'OFF' or 'UP' message for.
 *
 *  RETURN (void):
 *      The note will have been turned off.
 *
 *  NOTES:
 *
 **/

//************************************************************************
void NEAR PASCAL midiKeyBNoteOff( HWND hWnd, BYTE bNote )
//************************************************************************
{
    HMIDIKEYB    hKeyB;
    LPMIDIKEYB   lpKeyB;

    /* get the handle to the keyboard's control block */
    hKeyB = (HMIDIKEYB)GetWindowWord( hWnd, KEYB_WW_HMIDIKEYB );

    /* get pointer to the control's control block */
    if ( !(lpKeyB = (LPMIDIKEYB)GlobalLock( hKeyB )) )
        return;

    /* don't send note off message if the key is already up */
    if ( lpKeyB->rgKeyBKeys[ bNote ].fNoteOn )
    {
        /* set the current key state */
        lpKeyB->rgKeyBKeys[ bNote ].fNoteOn = FALSE;

        /* determine window to send message to */
        hWnd = (lpKeyB->hWnd) ? lpKeyB->hWnd : GetParent( hWnd );

        SendMessage( hWnd, lpKeyB->wMsgUp, (WPARAM)lpKeyB->hMidiOut,
                    MAKELONG( MAKEWORD( (BYTE)0x80 + lpKeyB->bChannel, bNote ),
                            MAKEWORD( 0, 0 ) ) );
    }

    /* unlock it */
    GlobalUnlock( hKeyB );
} /* midiKeyBNoteOff() */


//************************************************************************
/** BYTE NEAR PASCAL midiKeyBGetNote( LPMIDIKEYB lpKeyB, LONG lPos )
 *
 *  DESCRIPTION: 
 *      This function does the 'hit-testing' for the keyboard.  Although
 *      there is probably a better solution to approximate the key, this
 *      works well enough for now.
 *
 *  ARGUMENTS:
 *      LPMIDIKEYB lpKeyB   :   Pointer to keyboard layout information.
 *
 *      LONG lPos           :   Point to hit-test against (lParam of mouse
 *                              message).
 *
 *  RETURN (BYTE):
 *      The return value is the MIDI note number of the key hit.  It is
 *      0xFF if no key was 'hit.'
 *
 *  NOTES:
 *
 **/

    #define KEYB_GUESS_ERROR    6       /* no >5 w/ current algorithm   */

//************************************************************************
BYTE NEAR PASCAL midiKeyBGetNote( LPMIDIKEYB lpKeyB, LONG lPos )
//************************************************************************
{
    POINT   pt;
    BYTE    i;
    WORD    dx;
    WORD    wKey;

    pt.x = LOWORD( lPos );
    pt.y = HIWORD( lPos );

    /* dx is the width of a white key */
    dx = lpKeyB->wWidth / lpKeyB->bWhiteKeys;

    /*  Now _approximate_ the key hit (guess *LOW*).  This is probably not
     *  the most efficient approximation--but it works.  And it's a whole
     *  bunch better than doing a bajillion PtInRect() calls!  Currently,
     *  the error will be no >5 keys off.
     *
     *  If you are trying to figure out what this calculation is doing, it
     *  might help if I said that there are 12 keys per octave; 7 white and
     *  5 black.  So it uses this ratio to zero in on our key.  If you can
     *  simplify this, tell me how.
     */
    wKey = ((lpKeyB->bFirstKey / 12) * 7) +
                (((lpKeyB->bFirstKey % 12) * 5) / 7) +
                (pt.x / dx);
    wKey += (wKey / 7) * 5;

    /* for computing error when debugging (overload dx...) */
    D( dx = 0 );

    /* step from the guess, up... */
    for ( i = (BYTE)wKey; (i <(BYTE)( lpKeyB->bFirstKey + lpKeyB->bNumKeys)) &&
                          (i < (BYTE)(wKey + KEYB_GUESS_ERROR)); i++ )
    {
        /* black notes are 'over' white ones */
        if ( !lpKeyB->rgKeyBKeys[i].bNote )
        {
            if ( PtInRect( &(lpKeyB->rgKeyBKeys[i].rc), pt ) )
            {
#if 0
                D(  char    tBuf[ 80 ];
                    wsprintf( tBuf, "Black Error: %d\r\n", dx );
                    ODS( tBuf );
                 );
#endif

                return ( (BYTE)(i + lpKeyB->wKeyAdd) );
            }
        }

        /* for computing error when debugging */
        D( dx++ );
    }
    
    /* for computing error when debugging */
    D( dx = 0 );

    for ( i = (BYTE)wKey; (i < (BYTE)(lpKeyB->bFirstKey + lpKeyB->bNumKeys)) &&
                          (i < (BYTE)(wKey + KEYB_GUESS_ERROR)); i++ )
    {
        /* look at white keys */
        if ( lpKeyB->rgKeyBKeys[i].bNote ) 
        {
            if ( PtInRect( &(lpKeyB->rgKeyBKeys[i].rc), pt ) )
            {
#if 0
                D(  char    tBuf[ 80 ];
                    wsprintf( tBuf, "White Error: %d\r\n", dx );
                    ODS( tBuf );
                 );
#endif

                return ( (BYTE)(i + lpKeyB->wKeyAdd) );
            }
        }

        /* for computing error when debugging */
        D( dx++ );
    }

    /* out of bounds */
    return ( 0xFF );
} /* midiKeyBGetNote() */


//************************************************************************
/** void midiKeyBPaintKey( hWnd, hDC, lpKeyB, hPenLine, i )
 *
 *  DESCRIPTION: 
 *      This chunk of junk paints a single key.  It is used by both paint
 *      routines.  There is no sense duplicating code everywhere.
 *
 *  ARGUMENTS:
 *      HWND hWnd           :   The window to munge in.
 *
 *      HDC hDC             :   The DC to munge.
 *
 *      LPMIDIKEYB lpKeyB   :   Pointer to keyboard info.
 *
 *      HPEN hPenLine       :   Pen for drawing key separators.
 *
 *      BYTE i              :   The MIDI key note number to paint.
 *
 *  RETURN (void):
 *      The key will have been painted.
 *
 *  NOTES:
 *
 **/

//************************************************************************
void midiKeyBPaintKey( HWND hWnd, HDC hDC, LPMIDIKEYB lpKeyB, HPEN hPenLine, BYTE i )
//************************************************************************
{
    LPKEYBKEY   lpKeyBKey = &lpKeyB->rgKeyBKeys[ i ];
    HFONT       hOldFont = NULL;
    DWORD       dwOldText, dwOldBk;
    char        buf[ 4 ];
    char        buf2[ 4 ];
    RECT        rc, rc2;
    int         nSeparatorTop;
    int         nLen;

    /* don't munge original (CopyRect is silly and slow!) */
    rc2 = rc = lpKeyBKey->rc;
    rc2.top = rc.top = rc.bottom - (2 * lpKeyB->wTMHeight);

    /* paint white keys */
    if ( lpKeyBKey->bNote )
    {
        /* set the colors into for the keyboard into the control */
        dwOldText = SetTextColor( hDC, lpKeyB->rgbFgColor );
        dwOldBk = SetBkColor( hDC, lpKeyB->rgbBkColor );

        /* for labels */
        if ( GetWindowLong(hWnd, GWL_STYLE) & KEYBS_LABELS )
        {
            if ( lpKeyB->hFont )
                hOldFont = (HFONT)SelectObject( hDC, (HFONT)lpKeyB->hFont );

            nLen = wsprintf( buf, "%u", i + lpKeyB->wKeyAdd );
            wsprintf( buf2, "%c", lpKeyBKey->bNote );
        }

        /* zero len strings if no labels */
        else *buf2 = *buf = '\0', nLen = 0;

        InflateRect( &rc2, -1, -1 );

        /* set top of ON rect to not overlap black keys */
	rc2.top = max( rc2.top, (int)(lpKeyB->wHeight * 2/3) );

        ExtTextOut( hDC, lpKeyBKey->rc.left + (lpKeyBKey->rc.right -
                    lpKeyBKey->rc.left - lpKeyB->wTMAvgWidth * nLen) / 2,
                    lpKeyB->wHeight - 2 * lpKeyB->wTMHeight, 
                    ETO_CLIPPED | ETO_OPAQUE, &rc2,
                    (LPSTR)buf, nLen, NULL );

        ExtTextOut( hDC, lpKeyBKey->rc.right - (lpKeyBKey->rc.right -
                    lpKeyBKey->rc.left) / 2 - lpKeyB->wTMAvgWidth / 2,
                    lpKeyB->wHeight - lpKeyB->wTMHeight, NULL, NULL,
                    (LPSTR)buf2, lstrlen( buf2 ), NULL );

        /* set top of ON rect to not overlap black keys */
	rc.top = max( rc.top, (int)(lpKeyB->wHeight * 2/3) );

        /* is the next key an accidental key? adjust separator if so */
        if ( (i < 127) && lpKeyB->rgKeyBKeys[ i + 1 ].bNote )
            nSeparatorTop = lpKeyBKey->rc.top;
        else
            nSeparatorTop = lpKeyB->rgKeyBKeys[ i + 1 ].rc.bottom;

        hPenLine = (HPEN)SelectObject( hDC, hPenLine );
        MoveToEx( hDC, lpKeyBKey->rc.right, nSeparatorTop, NULL );
        LineTo( hDC, lpKeyBKey->rc.right, lpKeyBKey->rc.bottom );
        hPenLine = (HPEN)SelectObject( hDC, hPenLine );

        if ( hOldFont )
            SelectObject( hDC, (HFONT)hOldFont );
    }

    /* paint black keys */
    else
    {
        /* set the colors into for the keyboard into the control */
        dwOldBk = SetBkColor( hDC, lpKeyB->rgbFgColor );
        dwOldText = SetTextColor( hDC, lpKeyB->rgbBkColor );

        ExtTextOut( hDC, lpKeyBKey->rc.left, lpKeyBKey->rc.top,
                    ETO_CLIPPED | ETO_OPAQUE, &lpKeyBKey->rc, NULL,
                    0, NULL );

    }

    /* put these back */
    SetTextColor( hDC, dwOldText );
    SetBkColor( hDC, dwOldBk );

    if ( lpKeyB->rgKeyBKeys[ i + lpKeyB->wKeyAdd ].fNoteOn )
    {
        InflateRect( &rc, -2, -2 );
        InvertRect( hDC, &rc );
    }
} /* midiKeyBPaintKey() */


//************************************************************************
/** void NEAR PASCAL midiKeyBPaintSingleKey( HWND hWnd, BYTE bNote )
 *
 *  DESCRIPTION: 
 *      This function is used to paint a *single* key when it changes state.
 *      Even though GDI will only paint what has been InvalidateRect()'d,
 *      it must do clipping which is slow when dealing with a BIG keyboard.
 *      This is about 100 times faster than defaulting through WM_PAINT.
 *
 *  ARGUMENTS:
 *      HWND hWnd   :   Window handle for the custom control.
 *
 *      BYTE bNote  :   The note (MIDI note number) that changed state.
 *
 *  RETURN (void):
 *      The key will have been painted.
 *
 *  NOTES:
 *
 **/

//************************************************************************
void NEAR PASCAL midiKeyBPaintSingleKey( HWND hWnd, BYTE bNote )
//************************************************************************
{
    HMIDIKEYB   hKeyB;
    LPMIDIKEYB  lpKeyB;
    HPEN        hPenLine;
    HDC         hDC;

    /* get the handle to the keyboard's control block */
    hKeyB = (HMIDIKEYB)GetWindowWord( hWnd, KEYB_WW_HMIDIKEYB );

    /* get pointer to the control's control block */
    if ( lpKeyB = (LPMIDIKEYB)GlobalLock( hKeyB ) )
    {
        /* get the hDC and prepare for painting the key */
        if ( hDC = GetDC( hWnd ) )
        {
            /* build pen here so it isn't built for *each* key we paint */
            if ( hPenLine = CreatePen( PS_SOLID, 1, lpKeyB->rgbFgColor ) )
            {
                /* paint it */
                midiKeyBPaintKey( hWnd, hDC, lpKeyB, hPenLine, bNote );

                /* get rid of the pen */
                DeleteObject( hPenLine );
            }

            ReleaseDC( hWnd, hDC );
        }

        /* now unlock the control block */
        GlobalUnlock( hKeyB );
    }
} /* midiKeyBPaintSingleKey() */


//************************************************************************
/** void NEAR PASCAL midiKeyBNoteChanged( HWND hWnd, BYTE bNote )
 *
 *  DESCRIPTION: 
 *      This function is called to change the state of a key.  The key
 *      will be repainted in accordance to the new state.
 *
 *  ARGUMENTS:
 *      HWND hWnd           :   Window handle of control.
 *
 *      LPMIDIKEYB lpKeyB   :   Pointer to keyboard layout information.
 *
 *      BYTE bNote          :   MIDI note number to toggle.
 *
 *  RETURN (void):
 *      The MIDI note 'bNote' will have been toggled.
 *
 *  NOTES:
 *
 **/

//************************************************************************
void NEAR PASCAL midiKeyBNoteChanged( HWND hWnd, BYTE bNote )
//************************************************************************
{
    HMIDIKEYB   hKeyB;
    LPMIDIKEYB  lpKeyB;
    RECT        rc;

    /* get the handle to the keyboard's control block */
    hKeyB = (HMIDIKEYB)GetWindowWord( hWnd, KEYB_WW_HMIDIKEYB );

    /* get pointer to the control's control block */
    if ( !(lpKeyB = (LPMIDIKEYB)GlobalLock( hKeyB )) )
        return;

    /* don't munge original copy (CopyRect is silly and slow!) */
    rc = lpKeyB->rgKeyBKeys[ bNote ].rc;
    rc.top = rc.bottom - lpKeyB->wTMHeight * 2;

    /* set top of ON rect to not overlap black keys */
    if ( midiKeyBNormalKey( bNote ) )
	rc.top = max( rc.top, (int)(lpKeyB->wHeight * 2 / 3) );

    /* a little bit _inside_ of the key */
    InflateRect( &rc, -2, -2 );

    /* paint a single key */
    midiKeyBPaintSingleKey( hWnd, bNote );

    /* unlock it */
    GlobalUnlock( hKeyB );
} /* midiKeyBNoteChanged() */


//************************************************************************
/** void NEAR PASCAL midiKeyBPaint( HWND hWnd, HDC hDC )
 *
 *  DESCRIPTION: 
 *      This function is responsible for painting the midiKeyB control.
 *
 *  ARGUMENTS:
 *      HWND hWnd   :   The window handle for the keyboard.
 *
 *      HDC hDC     :   The DC for the keyboard's window.
 *
 *  RETURN (void):
 *      The control will have been painted.
 *
 *  NOTES:
 *
 **/

//************************************************************************
void NEAR PASCAL midiKeyBPaint( HWND hWnd, HDC hDC )
//************************************************************************
{
    HMIDIKEYB   hKeyB;
    LPMIDIKEYB  lpKeyB;
    WORD        i;
    WORD        wLastKey;
    HPEN        hPenLine;

    /* get the handle to the keyboard's control block */
    hKeyB = (HMIDIKEYB)GetWindowWord( hWnd, KEYB_WW_HMIDIKEYB );

    /* get pointer to the control's control block */
    if ( lpKeyB = (LPMIDIKEYB)GlobalLock( hKeyB ) )
    {
        /* build pen here so it isn't built for *each* key we paint */
        if ( hPenLine = CreatePen( PS_SOLID, 1, lpKeyB->rgbFgColor ) )
        {
            /* precompute the last MIDI key to paint */
            wLastKey = lpKeyB->bFirstKey + lpKeyB->bNumKeys;

            for ( i = lpKeyB->bFirstKey; i < wLastKey; i++ )
            {
                /* paint it */
                midiKeyBPaintKey( hWnd, hDC, lpKeyB, hPenLine, (BYTE)i );
            }

            /* get rid of the pen */
            DeleteObject( hPenLine );
        }

        /* now unlock the control block */
        GlobalUnlock( hKeyB );
    }
} /* midiKeyBPaint() */

  
//************************************************************************
/** void midiKeyBSize( HWND hWnd, LPMIDIKEYB lpKeyB )
 *
 *  DESCRIPTION: 
 *      This function is used to recalculate the rectangles used for painting
 *      and hit testing the keys on the keyboard.
 *
 *  ARGUMENTS:
 *      HWND hWnd           :   The window to size the control to.
 *
 *      LPMIDIKEYB lpKeyB   :   Keyboard layout information.
 *
 *  RETURN (void):
 *      The rectangles for each key will have been recalculated.
 *
 *  NOTES:
 *
 **/

//************************************************************************
void midiKeyBSize( HWND hWnd, LPMIDIKEYB lpKeyB )
//************************************************************************
{
    WORD    i, x, dx;

    /* count the number of white keys visible */
    lpKeyB->bWhiteKeys = 0;
    for ( i = lpKeyB->bFirstKey; i < (BYTE)(lpKeyB->bFirstKey + lpKeyB->bNumKeys); i++ )
    {
        if ( midiKeyBNormalKey( (BYTE)i ) )
            lpKeyB->bWhiteKeys++;
    }

    /* x is the right side of the white, dx is the width of a white key */
    x = dx = lpKeyB->wWidth / lpKeyB->bWhiteKeys;

    for ( i = lpKeyB->bFirstKey; i < (BYTE)(lpKeyB->bFirstKey + lpKeyB->bNumKeys); i++ )
    {
        if ( lpKeyB->rgKeyBKeys[i].bNote )
        {
            /* it's a normal (white) key */
            SetRect( &(lpKeyB->rgKeyBKeys[i].rc), x - dx, 0, x, lpKeyB->wHeight );
            x += dx;
        }

        else
        {                            
            /* it's an accidental (black) key */
            SetRect( &(lpKeyB->rgKeyBKeys[i].rc), x-dx/3-dx, 0, x+dx/3-dx, lpKeyB->wHeight*2/3 );
        }
    }
} /* midiKeyBSize() */


//************************************************************************
/** BOOL NEAR PASCAL midiKeyBShortMsg( HWND hWnd, LPMIDIKEYB lpKeyB, DWORD dwMsg )
 *
 *  DESCRIPTION:
 *      This function is responsible for parsing and performing the MIDI
 *      short message passed in dwMsg.  At this time, NOTE ON and NOTE OFF
 *      are the only messages that we respond to--this would be easy to
 *      expand upon.
 *
 *  ARGUMENTS:
 *      HWND hWnd           :   Window handle of midiKeyB control.
 *
 *      LPMIDIKEYB lpKeyB   :   Pointer to midiKeyB instance data.
 *
 *      DWORD dwMsg         :   The MIDI short message to interpret.
 *
 *  RETURN (BOOL):
 *      Currently always returns TRUE.
 *
 *  NOTES:
 *
 **/

//************************************************************************
BOOL NEAR PASCAL midiKeyBShortMsg( HWND hWnd, LPMIDIKEYB lpKeyB, DWORD dwMsg )
//************************************************************************
{
    BYTE    bCmd    = (BYTE)(((BYTE)dwMsg) & 0xF0);
    BYTE    bNote   = (BYTE)(((WORD)dwMsg) >> 8);

    switch ( bCmd )
    {
        /* key up */
        case 0x80:
            /* don't do a bunch of silly stuff if it is already up */
            if ( lpKeyB->rgKeyBKeys[ bNote ].fNoteOn )
            {
                /* set the current key state */
                lpKeyB->rgKeyBKeys[ bNote ].fNoteOn = FALSE;

                /* show it */
                midiKeyBNoteChanged( hWnd, bNote );
            }
        break;

        /* key down */
        case 0x90:
            /* don't do a bunch of silly stuff if it is already down */
            if ( !lpKeyB->rgKeyBKeys[ bNote ].fNoteOn )
            {
                /* set the current key state */
                lpKeyB->rgKeyBKeys[ bNote ].fNoteOn = TRUE;

                /* show it */
                midiKeyBNoteChanged( hWnd, bNote );
            }
        break;
    }

    /* always succeed for now... */
    return ( TRUE );
} /* midiKeyBShortMsg() */


//************************************************************************
/** LONG FAR PASCAL midiKeyBProc( HWND hWnd, WORD wMsg, WORD wParam, LONG lParam )
 *
 *  DESCRIPTION: 
 *      This is the control's window procedure.  Its purpose is to handle
 *      special messages for this custom control.
 *
 *      The special control messages for the midiKeyB control are:
 *
 *          KEYB_GETLAYOUT  :   Gets the current keyboard layout.  HIWORD()
 *                              is the first key; LOWORD() is number of keys.
 *
 *          KEYB_SETLAYOUT  :   Sets the keyboard layout.  HIWORD(lParam) is
 *                              the first key; LOWORD(lParam) is the number
 *                              of keys for the keyboard.  wParam is a BOOL
 *                              telling the control to repaint now (TRUE) or
 *                              later (FALSE).
 *
 *          KEYB_GETCHANNEL :   Gets the current channel used in the MIDI
 *                              short message (in lParam) sent to the parent
 *                              window.
 *
 *          KEYB_SETCHANNEL :   Sets the current channel used in the MIDI
 *                              short message (in lParam) sent to the parent
 *                              window.  wParam should be the new channel.
 *                              lParam is ignored.
 *
 *          KEYB_GETHMIDIOUT:   Gets the current hMidiOut used in the MIDI
 *                              short message (in lParam) sent to the parent
 *                              window.
 *
 *          KEYB_SETHMIDIOUT:   Sets the current hMidiOut used in the MIDI
 *                              short message (in lParam) sent to the parent
 *                              window.  wParam should be the new hMidiOut.
 *                              lParam is ignored.
 *
 *          KEYB_GETHWND    :   Gets the current 'parent' window that will
 *                              receive note on and off messages from the
 *                              control.
 *
 *          KEYB_SETHWND    :   Sets the current 'parent' window that will
 *                              receive note on and off messages.  If NULL,
 *                              then the GetParent() of the control will
 *                              receive the messages (default).
 *
 *          KEYB_GETVELOCITY:   Gets the current velocity used in the MIDI
 *                              short message (in lParam) sent to the parent
 *                              window.
 *
 *          KEYB_SETVELOCITY:   Sets the current velocity used in the MIDI
 *                              short message (in lParam) sent to the parent
 *                              window.  wParam should be the new velocity.
 *                              lParam is ignored.
 *
 *          KEYB_GETFGCOLOR :   Gets the current foreground color of keyboard.
 *                              This is used for the accidental keys, text,
 *                              and separator lines.
 *
 *          KEYB_SETFGCOLOR :   Sets the foreground color for the keyboard.
 *                              This is used for the accidental keys, text,
 *                              and separator lines.  lParam should be the
 *                              RGB() value.  wParam is a BOOL telling the
 *                              control to repaint immediately (TRUE) or
 *                              later (FALSE).
 *
 *          KEYB_GETBKCOLOR :   Gets the current background color of keyboard.
 *                              This is used for the normal keys.
 *
 *          KEYB_SETBKCOLOR :   Sets the background color for the keyboard.
 *                              This is used for the normal keys. lParam
 *                              should be the RGB() value.  wParam is a BOOL
 *                              telling the control to repaint immediately
 *                              (TRUE) or later (FALSE).
 *
 *          WM_SETFONT      :   Sets the font to use for the labels on keys.
 *
 *          WM_GETFONT      :   Gets the current font in use for labels.
 *
 *  NOTES:
 *
 **/

//************************************************************************
LONG WINPROC EXPORT midiKeyBProc( HWND hWnd, UINT wMsg, WPARAM wParam, LPARAM lParam )
//************************************************************************
{
    PAINTSTRUCT ps;
    HMIDIKEYB   hKeyB;
    LPMIDIKEYB  lpKeyB;
    WORD        i;
    BYTE        bNote;
    HFONT       hFont;

    /* break to get DefWindowProc() */
    switch( wMsg )
    {
        case WM_CREATE:
            /* need to allocate a control block */
            if ( hKeyB = GlobalAlloc(KEYB_GMEM_FLAGS, sizeof(MIDIKEYB)) )
            {
                /* hang on to this control block */
                SetWindowWord( hWnd, KEYB_WW_HMIDIKEYB, (WORD)hKeyB );

                /* lock the control block so we can fill it with defaults */
                if ( lpKeyB = (LPMIDIKEYB)GlobalLock( hKeyB ) )
                {
                    HDC         hDC;
                    TEXTMETRIC  tm;

                    /* grab the text metrics */
                    hDC = GetDC( hWnd );
                    GetTextMetrics( hDC, &tm );

                    /* fill control block with defaults */
                    lpKeyB->wTMHeight   = tm.tmHeight;
                    lpKeyB->wTMAvgWidth = tm.tmAveCharWidth;
                    lpKeyB->bVelocity   = KEYB_DEF_VELOCITY;
                    lpKeyB->rgbFgColor  = GetNearestColor( hDC, KEYB_DEF_FGCOLOR );
                    lpKeyB->rgbBkColor  = GetNearestColor( hDC, KEYB_DEF_BKCOLOR );
                    lpKeyB->wMsgDown    = WM_MIDIKEYBKEYDOWN;
                    lpKeyB->wMsgUp      = WM_MIDIKEYBKEYUP;

                    ReleaseDC( hWnd, hDC );

                    /* initialize key array */
                    for ( i = 0; i < KEYB_MAX_KEYS; i++ )
                    {
                        lpKeyB->rgKeyBKeys[ i ].fNoteOn = FALSE;
                        lpKeyB->rgKeyBKeys[ i ].bNote = midiKeyBNormalKey( (BYTE)i );
                        lpKeyB->rgKeyBKeys[ i ].bMIDINote = (BYTE)i;
                    }

                    SendMessage( hWnd, KEYB_SETLAYOUT, FALSE,
                            MAKELONG( KEYB_DEF_NUMKEYS, KEYB_DEF_FIRSTKEY ) );

                    /* unlock the control block */
                    GlobalUnlock( hKeyB );

                    /* go to DefWindowProc() to finish the job */
                    break;
                }

                /* free the memory (why couldn't I lock it???) */
                else GlobalFree( hKeyB );
            }

            /* uh-oh! problems with memory, so fail! */
            return ( 0L );
        break;


        case WM_DESTROY:
            /* get rid of the control's hunk 'o junk */
            if ( hKeyB = (HMIDIKEYB)GetWindowWord( hWnd, KEYB_WW_HMIDIKEYB ) )
                GlobalFree( hKeyB );
        break;


        case WM_ERASEBKGND:
            hKeyB = (HMIDIKEYB)GetWindowWord( hWnd, KEYB_WW_HMIDIKEYB );
            if ( lpKeyB = (LPMIDIKEYB)GlobalLock( hKeyB ) )
            {
                HBRUSH  hNewBk = CreateSolidBrush( lpKeyB->rgbBkColor );
                HBRUSH  hOldBk;

                GlobalUnlock( hKeyB );

                if ( !hNewBk )
                    break;

                hOldBk = (HBRUSH)SET_CLASS_HBRBACKGROUND( hWnd, (UINT)hNewBk );
                lParam = DefWindowProc( hWnd, wMsg, wParam, lParam );
                SET_CLASS_HBRBACKGROUND( hWnd, (UINT)hOldBk );
                DeleteObject( hNewBk );
                return ( lParam );
            }
        break;


        case WM_SIZE:
            hKeyB = (HMIDIKEYB)GetWindowWord( hWnd, KEYB_WW_HMIDIKEYB );
            if ( lpKeyB = (LPMIDIKEYB)GlobalLock( hKeyB ) )
            {
                lpKeyB->wWidth = LOWORD( lParam );
                lpKeyB->wHeight = HIWORD( lParam );
                midiKeyBSize( hWnd, lpKeyB );
                GlobalUnlock( hKeyB );
            }
        break;


        case WM_RBUTTONDOWN:
            hKeyB = (HMIDIKEYB)GetWindowWord( hWnd, KEYB_WW_HMIDIKEYB );
            if ( lpKeyB = (LPMIDIKEYB)GlobalLock( hKeyB ) )
            {
                bNote = midiKeyBGetNote( lpKeyB, lParam );
                if ( bNote != 0xFF )
                {
                    /* toggle the key */
                    if ( lpKeyB->rgKeyBKeys[ bNote ].fNoteOn )
                    {
                        midiKeyBNoteOff( hWnd, bNote );
                    }

                    else
                    {
                        midiKeyBNoteOn( hWnd, bNote );
                    }

                    midiKeyBNoteChanged( hWnd, bNote );
                }

                GlobalUnlock( hKeyB );
            }
        break;

        case WM_LBUTTONDOWN:
            hKeyB = (HMIDIKEYB)GetWindowWord( hWnd, KEYB_WW_HMIDIKEYB );
            if ( lpKeyB = (LPMIDIKEYB)GlobalLock( hKeyB ) )
            {
                bNote = midiKeyBGetNote( lpKeyB, lParam );
                if ( bNote != 0xFF )
                {
                    midiKeyBNoteOn( hWnd, bNote );

keybLButtonDone:
                    midiKeyBNoteChanged( hWnd, bNote );
                }
                GlobalUnlock( hKeyB );
            }
        break;

        case WM_LBUTTONUP:
            hKeyB = (HMIDIKEYB)GetWindowWord( hWnd, KEYB_WW_HMIDIKEYB );
            if ( lpKeyB = (LPMIDIKEYB)GlobalLock( hKeyB ) )
            {
                bNote = midiKeyBGetNote( lpKeyB, lParam );
                if ( bNote != 0xFF )
                {
                    midiKeyBNoteOff( hWnd, bNote );
                    goto keybLButtonDone;
                }
            }
        break;


        /*  It would be really neat if the keyboard would allow CTRL
         *  and SHIFT key modifiers like a multi-select list box.  The
         *  following code was used to slide the keyboard up and down
         *  in MIDI note messages--it is no longer used and probably
         *  does not work.
         */
#if 0
        case WM_KEYDOWN:
        {
            RECT        rc;

            hKeyB = GetWindowWord( hWnd, KEYB_WW_HMIDIKEYB );
            if ( lpKeyB = (LPMIDIKEYB)GlobalLock( hKeyB ) )
            {
                GetWindowRect( hWnd, &rc );
                rc.right = rc.right - rc.left;
                rc.left = 0;
                rc.top = lpKeyB->wHeight - (2 * lpKeyB->wTMHeight);
                rc.bottom = lpKeyB->wHeight - lpKeyB->wTMHeight;
                if ( wParam == VK_CONTROL )
                {
                    lpKeyB->wKeyAdd = -lpKeyB->bNumKeys;
                    if ( !(lParam & 0x40000000) )
                        InvalidateRect( hWnd, &rc, TRUE );
                }

                else if ( wParam == VK_SHIFT )
                {
                    lpKeyB->wKeyAdd = lpKeyB->bNumKeys;
                    if ( !(lParam & 0x40000000) )
                        InvalidateRect( hWnd, &rc, TRUE );
                }
                GlobalUnlock( hKeyB );
            }
        }
        break;


        case WM_KEYUP:
        {
            RECT        rc;

            hKeyB = GetWindowWord( hWnd, KEYB_WW_HMIDIKEYB );
            if ( lpKeyB = (LPMIDIKEYB)GlobalLock( hKeyB ) )
            {
                GetWindowRect( hWnd, &rc );
                rc.right = rc.right - rc.left;
                rc.left = 0;
                rc.top = lpKeyB->wHeight - (2 * lpKeyB->wTMHeight);
                rc.bottom = lpKeyB->wHeight - lpKeyB->wTMHeight;
                lpKeyB->wKeyAdd = 0;
                InvalidateRect( hWnd, &rc, TRUE );
                GlobalUnlock( hKeyB );
            }
        }
        break;
#endif

        case WM_MOUSEMOVE:
            if ( wParam & MK_LBUTTON )
            {
                hKeyB = (HMIDIKEYB)GetWindowWord( hWnd, KEYB_WW_HMIDIKEYB );
                if ( lpKeyB = (LPMIDIKEYB)GlobalLock( hKeyB ) )
                {
                    bNote = midiKeyBGetNote( lpKeyB, lParam );
                    if ( (bNote != 0xFF) && ( bNote != lpKeyB->bLastNote ) )
                    {
                        midiKeyBNoteOff( hWnd, lpKeyB->bLastNote );
                        midiKeyBNoteChanged( hWnd, lpKeyB->bLastNote );

                        midiKeyBNoteOn( hWnd, bNote );
                        midiKeyBNoteChanged( hWnd, bNote );
                    }
                    GlobalUnlock( hKeyB );
                }
            }
        break;


        case WM_PAINT:
            BeginPaint( hWnd, &ps );
            midiKeyBPaint( hWnd, ps.hdc );
            EndPaint( hWnd, &ps );
            return ( 0L );


        case KEYB_GETMSGDOWN:
            hKeyB = (HMIDIKEYB)GetWindowWord( hWnd, KEYB_WW_HMIDIKEYB );
            if ( lpKeyB = (LPMIDIKEYB)GlobalLock( hKeyB ) )
            {
                wParam = lpKeyB->wMsgDown;

keybReturnwParam:
                GlobalUnlock( hKeyB );
                return ( (LONG)wParam );
            }
            return ( 0L );

        case KEYB_GETMSGUP:
            hKeyB = (HMIDIKEYB)GetWindowWord( hWnd, KEYB_WW_HMIDIKEYB );
            if ( lpKeyB = (LPMIDIKEYB)GlobalLock( hKeyB ) )
            {
                wParam = lpKeyB->wMsgUp;
                goto keybReturnwParam;
            }
            return ( 0L );

        case KEYB_GETVELOCITY:
            hKeyB = (HMIDIKEYB)GetWindowWord( hWnd, KEYB_WW_HMIDIKEYB );
            if ( lpKeyB = (LPMIDIKEYB)GlobalLock( hKeyB ) )
            {
                wParam = (WORD)lpKeyB->bVelocity;
                goto keybReturnwParam;
            }
            return ( 0L );

        case KEYB_GETCHANNEL:
            hKeyB = (HMIDIKEYB)GetWindowWord( hWnd, KEYB_WW_HMIDIKEYB );
            if ( lpKeyB = (LPMIDIKEYB)GlobalLock( hKeyB ) )
            {
                wParam = (WORD)lpKeyB->bChannel;
                goto keybReturnwParam;
            }
            return ( 0L );

        case KEYB_GETHWND:
            hKeyB = (HMIDIKEYB)GetWindowWord( hWnd, KEYB_WW_HMIDIKEYB );
            if ( lpKeyB = (LPMIDIKEYB)GlobalLock( hKeyB ) )
            {
                wParam = (WORD)lpKeyB->hWnd;
                goto keybReturnwParam;
            }
            return ( 0L );

        case KEYB_GETHMIDIOUT:
            hKeyB = (HMIDIKEYB)GetWindowWord( hWnd, KEYB_WW_HMIDIKEYB );
            if ( lpKeyB = (LPMIDIKEYB)GlobalLock( hKeyB ) )
            {
                wParam = (WPARAM)lpKeyB->hMidiOut;
                goto keybReturnwParam;
            }
            return ( 0L );



        case KEYB_SETMSGUP:
            hKeyB = (HMIDIKEYB)GetWindowWord( hWnd, KEYB_WW_HMIDIKEYB );
            if ( lpKeyB = (LPMIDIKEYB)GlobalLock( hKeyB ) )
            {
                lpKeyB->wMsgUp = wParam;
keybReturn0L:
                GlobalUnlock( hKeyB );
            }
            return ( 0L );

        case KEYB_SETMSGDOWN:
            hKeyB = (HMIDIKEYB)GetWindowWord( hWnd, KEYB_WW_HMIDIKEYB );
            if ( lpKeyB = (LPMIDIKEYB)GlobalLock( hKeyB ) )
            {
                lpKeyB->wMsgDown = wParam;
                goto keybReturn0L;
            }
            return ( 0L );

        case KEYB_SETCHANNEL:
            hKeyB = (HMIDIKEYB)GetWindowWord( hWnd, KEYB_WW_HMIDIKEYB );
            if ( lpKeyB = (LPMIDIKEYB)GlobalLock( hKeyB ) )
            {
                lpKeyB->bChannel = (BYTE)wParam;
                goto keybReturn0L;
            }
            return ( 0L );

        case KEYB_SETVELOCITY:
            hKeyB = (HMIDIKEYB)GetWindowWord( hWnd, KEYB_WW_HMIDIKEYB );
            if ( lpKeyB = (LPMIDIKEYB)GlobalLock( hKeyB ) )
            {
                lpKeyB->bVelocity = (BYTE)wParam;
                goto keybReturn0L;
            }
            return ( 0L );

        case KEYB_SETHWND:
            hKeyB = (HMIDIKEYB)GetWindowWord( hWnd, KEYB_WW_HMIDIKEYB );
            if ( lpKeyB = (LPMIDIKEYB)GlobalLock( hKeyB ) )
            {
                lpKeyB->hWnd = (HWND)wParam;
                goto keybReturn0L;
            }
            return ( 0L );

        case KEYB_SETHMIDIOUT:
            hKeyB = (HMIDIKEYB)GetWindowWord( hWnd, KEYB_WW_HMIDIKEYB );
            if ( lpKeyB = (LPMIDIKEYB)GlobalLock( hKeyB ) )
            {
                lpKeyB->hMidiOut = (HANDLE)wParam;
                goto keybReturn0L;
            }
            return ( 0L );



        case KEYB_GETBKCOLOR:
            hKeyB = (HMIDIKEYB)GetWindowWord( hWnd, KEYB_WW_HMIDIKEYB );
            if ( lpKeyB = (LPMIDIKEYB)GlobalLock( hKeyB ) )
            {
                lParam = lpKeyB->rgbFgColor;

keybReturnlParam:
                GlobalUnlock( hKeyB );
                return ( lParam );
            }
            return ( 0L );

        case KEYB_GETFGCOLOR:
            hKeyB = (HMIDIKEYB)GetWindowWord( hWnd, KEYB_WW_HMIDIKEYB );
            if ( lpKeyB = (LPMIDIKEYB)GlobalLock( hKeyB ) )
            {
                lParam = lpKeyB->rgbBkColor;
                goto keybReturnlParam;
            }
            return ( 0L );


        case KEYB_SETBKCOLOR:
            hKeyB = (HMIDIKEYB)GetWindowWord( hWnd, KEYB_WW_HMIDIKEYB );
            if ( lpKeyB = (LPMIDIKEYB)GlobalLock( hKeyB ) )
            {
                HDC hDC = GetDC( hWnd );
                lpKeyB->rgbBkColor = GetNearestColor( hDC, lParam );
                ReleaseDC( hWnd, hDC );
                GlobalUnlock( hKeyB );
            }

            goto keybRepaintTheThing;

        case KEYB_SETFGCOLOR:
            hKeyB = (HMIDIKEYB)GetWindowWord( hWnd, KEYB_WW_HMIDIKEYB );
            if ( lpKeyB = (LPMIDIKEYB)GlobalLock( hKeyB ) )
            {
                HDC hDC = GetDC( hWnd );
                lpKeyB->rgbFgColor = GetNearestColor( hDC, lParam );
                ReleaseDC( hWnd, hDC );
                GlobalUnlock( hKeyB );
            }

            goto keybRepaintTheThing;


        case KEYB_GETLAYOUT:
            hKeyB = (HMIDIKEYB)GetWindowWord( hWnd, KEYB_WW_HMIDIKEYB );
            if ( lpKeyB = (LPMIDIKEYB)GlobalLock( hKeyB ) )
            {
                lParam = MAKELONG( lpKeyB->bNumKeys, lpKeyB->bFirstKey );
                GlobalUnlock( hKeyB );
            }

            else lParam = 0L;

            /* return the current number of keys and starting number */
            return ( lParam );


        case KEYB_SETLAYOUT:
            hKeyB = (HMIDIKEYB)GetWindowWord( hWnd, KEYB_WW_HMIDIKEYB );
            if ( lpKeyB = (LPMIDIKEYB)GlobalLock( hKeyB ) )
            {
                lpKeyB->bNumKeys = (BYTE)LOWORD( lParam );
                lpKeyB->bFirstKey = (BYTE)HIWORD( lParam );
                midiKeyBSize( hWnd, lpKeyB );
                GlobalUnlock( hKeyB );
            }

keybRepaintTheThing:
            /* redraw if indicated in message */
            if ( (BOOL)wParam )
            {
                InvalidateRect( hWnd, NULL, TRUE );
                UpdateWindow( hWnd );
            }
            return ( 0L );


        case KEYB_MIDISHORTMSG:
            hKeyB = (HMIDIKEYB)GetWindowWord( hWnd, KEYB_WW_HMIDIKEYB );
            if ( lpKeyB = (LPMIDIKEYB)GlobalLock( hKeyB ) )
            {
                /* decode the short message */
                midiKeyBShortMsg( hWnd, lpKeyB, lParam );

                GlobalUnlock( hKeyB );

                /* repaint the WHOLE THING if wParam is non-zero */
                goto keybRepaintTheThing;
            }
            return ( 0L );


        case KEYB_RESET:
            hKeyB = (HMIDIKEYB)GetWindowWord( hWnd, KEYB_WW_HMIDIKEYB );
            if ( lpKeyB = (LPMIDIKEYB)GlobalLock( hKeyB ) )
            {
                short   i;

                /* set all key states to off */
                for ( i = 0; i < KEYB_MAX_KEYS; i++ )
                {
                    lpKeyB->rgKeyBKeys[ i ].fNoteOn = FALSE;
                }

                GlobalUnlock( hKeyB );

                /* repaint the WHOLE THING if wParam is non-zero */
                goto keybRepaintTheThing;
            }
            return ( 0L );


        case WM_GETFONT:
            hKeyB = (HMIDIKEYB)GetWindowWord( hWnd, KEYB_WW_HMIDIKEYB );
            if ( lpKeyB = (LPMIDIKEYB)GlobalLock( hKeyB ) )
            {
                hFont = lpKeyB->hFont;
                GlobalUnlock( hKeyB );
            }

            else hFont = NULL;

            /* if system font, then return NULL handle */
            return ( (hFont == GetStockObject(SYSTEM_FONT)) ? NULL : (long)(LPTR)hFont );

        case WM_SETFONT:
            /* if NULL hFont, use system font */
            if ( !(hFont = (HFONT)wParam) )
                hFont = (HFONT)GetStockObject( SYSTEM_FONT );

            hKeyB = (HMIDIKEYB)GetWindowWord( hWnd, KEYB_WW_HMIDIKEYB );
            if ( lpKeyB = (LPMIDIKEYB)GlobalLock( hKeyB ) )
            {
                HDC         hDC;
                TEXTMETRIC  tm;

                /* grab the text metrics */
                hDC = GetDC( hWnd );
                if ( lpKeyB->hFont = hFont )
                    SelectObject( hDC, hFont );
                GetTextMetrics( hDC, &tm );
                ReleaseDC( hWnd, hDC );

                /* fill control block with defaults */
                lpKeyB->wTMHeight   = tm.tmHeight;
                lpKeyB->wTMAvgWidth = tm.tmAveCharWidth;

                GlobalUnlock( hKeyB );
            }

            /* set wParam to lParam BOOL so it will repaint appropriately */
            wParam = (WORD)lParam;
            goto keybRepaintTheThing;

    } /* switch () */

    /* let the dialog mangler take care of this message */
    return ( DefWindowProc( hWnd, wMsg, wParam, lParam ) );
} /* midiKeyBProc() */


/** EOF: midikeyb.c **/
