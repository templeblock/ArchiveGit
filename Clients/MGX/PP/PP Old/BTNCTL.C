/* Check boxes and radio buttons */

/*% MOVEDS - NK */

#define LSTRING
#define CTLMGR
#define NOMENUS
#define NOICON
#define NOSYSCOMMANDS
#define NOSYSMETRICS
#define NOCLIPBOARD
#define NOMETAFILE
#define NOSOUND
#define NOWH
#define NOKANJI
#include "user.h"

/* CalcBtnRect codes */
#define CBR_CLIENTRECT      0
#define CBR_CHECKBOX        1
#define CBR_CHECKTEXT       2
#define CBR_GROUPTEXT       3
#define CBR_GROUPFRAME      4

/* DrawBtnText codes */
#define DBT_TEXT    0x0001
#define DBT_FOCUS   0x0002

#define SRCNOTXOR           0x00990066L
#define DPO 0x00fa0089  /* destination, pattern, or */

/* these values are assumed for bit shifting operations */
#define BFCHECK     0x0003
#define BFSTATE     0x0004
#define BFFOCUS     0x0008
#define BFINCLICK   0x0010      /* inside click code */
#define BFCAPTURED  0x0020      /* we have mouse capture */
#define BFMOUSE     0x0040      /* mouse-initiated */
#define BFDONTCLICK 0x0080	/* don't check on get focus */

#define BUTTONSTATE(hwnd)   ( *((BYTE *)(hwnd + 1)) )

#define WBUTTONSTYLE(hwnd)  ((LOBYTE(hwnd->style)) & (LOWORD(~BS_LEFTTEXT)))
#define BUTTONSTYLE(hwnd)   ((LOBYTE(hwnd->style)) & (LOBYTE(~BS_LEFTTEXT)))

extern HBRUSH hbrWhite, hbrBlack, hbrGray;
extern int cxBorder, cyBorder;
extern SYSCLROBJECTS sysClrObjects;
extern SYSCOLORS sysColors;
extern OEMBITMAPINFO obiCorner;
extern int FAR LRCCFrame(HDC, LPRECT, HBRUSH, long);
extern int cxBorder, cyBorder;
extern FAR UnRealizeObject(HBRUSH);


void far DefSetText(HWND, LPSTR);
long ButtonCtlMsg();
void FAR PASCAL DrawPushButton();
HBRUSH FAR GetControlBrush();
void FAR BltColor();
void FAR FillWindow();
void FAR PaintRect();
BOOL FAR PrefixTextOut();
void BtnTextOut();
DWORD BtnGetTextExtent();

char mpStyleCbr[] = {
    CBR_CLIENTRECT,     /* BS_PUSHBUTTON     */
    CBR_CLIENTRECT,     /* BS_DEFPUSHBUTTON  */
    CBR_CHECKTEXT,      /* BS_CHECKBOX       */
    CBR_CHECKTEXT,      /* BS_AUTOCHECKBOX   */
    CBR_CHECKTEXT,      /* BS_RADIOBUTTON    */
    CBR_CHECKTEXT,      /* BS_3STATE         */
    CBR_CHECKTEXT,      /* BS_AUTO3STATE     */
    CBR_GROUPTEXT,      /* BS_GROUPBOX       */
    CBR_CLIENTRECT,     /* BS_USERBUTTON     */
    CBR_CHECKTEXT,      /* BS_AUTORADIOBUTTON */
    CBR_CLIENTRECT,     /* BS_PUSHBOX        */
};

HBRUSH hbrBtn;


HDC BtnGetDC(hwnd)
register HWND hwnd;
{
    register HDC hdc;

    if (IsWindowVisible(hwnd)) {
        hdc = GetDC(hwnd);
        BtnInitDC(hwnd, hdc);
        return(hdc);
    }
    return(NULL);
}

BtnInitDC(hwnd, hdc)
register HWND hwnd;
register HDC hdc;
{

    hbrBtn = GetControlBrush(hwnd, hdc, CTLCOLOR_BTN);
    SetBkMode(hdc, OPAQUE);
}


/*********************** START OF SPECIFICATION ***********************/
#ifdef COMMENT_GLOBAL

ButtonWndProc - WndProc for buttons, check boxes, etc.

LINKAGE:    FAR PLM

ENTRY:      hwnd, message, wParam, lParam on stack.

EXIT:       long in AX,DX = result.

EXIT-ERROR:

EFFECTS:    AX,BX,CX,DX,ES are modified.

INTERNAL:
            SetCursorPos
            BeginPaint
            BtnInitDC
            EndPaint
            GetDC
            ReleaseDC
            NotifyParent
            ReleaseBtnCapture
            SetBtnCapture
            GetClientRect
            SetBtnState
            BtnGetDC
            DrawNewState
            DrawCheck
            InvalidateRect
            DefSetText
            ButtonPaint
            DrawBtnText
            DefWindowProc

EXTERNAL:

WARNINGS:

REVISION HISTORY:
            4/4/86      jpk     Added LEFTTEXT button style.
            4/7/86      jpk     Added this sublogue.
            4/7/86      jpk     Added PUSHBOX button style.
            6/15/87     edf     Removed fErase flag to ButtonPaint.

IMPLEMENTATION:

#endif
/*********************** END OF SPECIFICATION *************************/

long far ButtonWndProc(hwnd, message, wParam, lParam)
register HWND hwnd;
unsigned message;
register WORD wParam;
LONG lParam;
{
    WORD bsWnd;
    int sOld;
    RECT rc;
    HDC hdc;
    PAINTSTRUCT ps;

    bsWnd = BUTTONSTYLE(hwnd);

    switch (message) {
    case WM_NCHITTEST:
        if (bsWnd == BS_GROUPBOX) {
            return((LONG)-1);       /* HTTRANSPARENT */
        } else {
            goto CallDWP;
        }
        break;

    case WM_ERASEBKGND:
        /* do nothing, but don't let defwndproc do it either.
         * it will be erased in buttonpaint().
         * 20-Jul-1987. davidhab.
         */
        return ((LONG)TRUE);
        break;

    case WM_PAINT:
        /* If wParam != NULL, then this is a subclassed paint */
        if ((hdc = (HDC)wParam) == NULL)
            hdc = BeginPaint(hwnd, (PAINTSTRUCT far *)&ps);
        BtnInitDC(hwnd, hdc);
        ButtonPaint(hwnd, hdc);
        if (wParam == NULL)
            EndPaint(hwnd, (PAINTSTRUCT far *)&ps);

        break;


    case WM_SETFOCUS:
        BUTTONSTATE(hwnd) |= BFFOCUS;
        if ((hdc = BtnGetDC(hwnd)) != NULL) {
            DrawBtnText(hdc, hwnd, DBT_FOCUS);
            ReleaseDC(hwnd, hdc);
        }

        if (!(BUTTONSTATE(hwnd) & BFINCLICK)) {
            switch (bsWnd) {
            case BS_RADIOBUTTON:
            case BS_AUTORADIOBUTTON:
		if (!(BUTTONSTATE(hwnd) & BFDONTCLICK)) {
		    if (!(BUTTONSTATE(hwnd) & BFCHECK))
			NotifyParent(hwnd, BN_CLICKED);
		    break;
		}
            default:
                break;
            }
        }
        break;

    case WM_KILLFOCUS:
	/* If we are loosing the focus and we are in "capture mode" click
	 * the button.	This allows tab and space keys to overlap for
	 * fast toggle of a series of buttons.	(bobgu 8/6/87)
	 */
	ReleaseBtnCapture(hwnd, TRUE);
        BUTTONSTATE(hwnd) &= ~BFFOCUS;
        if ((hdc = BtnGetDC(hwnd)) != NULL) {
            DrawBtnText(hdc, hwnd, DBT_FOCUS);
            ReleaseDC(hwnd, hdc);
        }
        break;

    /* Double click messages are recognized for BS_RADIOBUTTON and
     * BS_USERBUTTON sytles.  For all other buttons, double click is
     * handled like a normal button down.  (bobgu 6/4/87)
     */
    case WM_LBUTTONDBLCLK:
        switch (bsWnd) {
        case BS_USERBUTTON:
	case BS_RADIOBUTTON:
            if (GetAppVer() >= VER) {
                NotifyParent(hwnd, BN_DOUBLECLICKED);
		break;
            }
	/* fall through if old version app */
        default:
	    goto btnclick;	/* go do a nornal click */
        }
        break;

    case WM_LBUTTONUP:
        if (!(BUTTONSTATE(hwnd) & BFMOUSE))
            break;
        ReleaseBtnCapture(hwnd, TRUE);
        break;

    case WM_MOUSEMOVE:
        if (!(BUTTONSTATE(hwnd) & BFMOUSE))
            break;
        /* fall thru */
    case WM_LBUTTONDOWN:
btnclick:
        SetBtnCapture(hwnd, BFMOUSE);
        GetClientRect(hwnd, (LPRECT)&rc);
        SetBtnState(hwnd, PtInRect((LPRECT)&rc, MAKEPOINT(lParam)));
        break;

    case BM_CLICK:
        SendMessage(hwnd, WM_LBUTTONDOWN, 0, 0L);
        SendMessage(hwnd, WM_LBUTTONUP, 0, 0L);

    case WM_KEYDOWN:
        if (BUTTONSTATE(hwnd) & BFMOUSE)
            break;
        if (wParam == VK_SPACE) {
            SetBtnCapture(hwnd, 0);
            SetBtnState(hwnd, TRUE);
        } else {
            ReleaseBtnCapture(hwnd, FALSE);
        }
        break;

    case WM_KEYUP:
    case WM_SYSKEYUP:
        if (BUTTONSTATE(hwnd) & BFMOUSE)
            goto CallDWP;
	/* Don't cancel the capture mode on the up of the tab in case the
	 * guy is overlapping tab and space keys. (bobgu 8/6/87)
	 */
	if (wParam == VK_TAB)
	    goto CallDWP;
        ReleaseBtnCapture(hwnd, (wParam == VK_SPACE));
        if (message == WM_SYSKEYUP)
            goto CallDWP;
        break;

    case BM_GETSTATE:     /* get state */
        return((long)BUTTONSTATE(hwnd));
        break;

    case BM_SETSTATE:     /* set state */
        sOld = (BUTTONSTATE(hwnd) & BFSTATE);
        if (wParam)
            BUTTONSTATE(hwnd) |= BFSTATE;
        else
            BUTTONSTATE(hwnd) &= ~BFSTATE;
        if ((hdc = BtnGetDC(hwnd)) != NULL) {
            if (bsWnd == BS_USERBUTTON)
                NotifyParent(hwnd, (wParam ? BN_HILITE : BN_UNHILITE));
            else
                DrawNewState(hdc, hwnd, sOld);
            ReleaseDC(hwnd, hdc);
        }
        break;

    case BM_GETCHECK:        /* Get check state */
        return((long)(BUTTONSTATE(hwnd) & BFCHECK));
        break;

    case BM_SETCHECK:        /* set check state */
        switch (bsWnd) {
        case BS_RADIOBUTTON:
        case BS_AUTORADIOBUTTON:
            if (wParam)
                hwnd->style |= WS_TABSTOP;
            else
                hwnd->style &= ~WS_TABSTOP;
            /* fall thru */
        case BS_CHECKBOX:
        case BS_AUTOCHECKBOX:
            if (wParam)
                wParam = 1;
            goto CheckIt;

        case BS_3STATE:
        case BS_AUTO3STATE:
            if (wParam > 2)
                wParam = 2;
CheckIt:
            if ((BUTTONSTATE(hwnd) & BFCHECK) != wParam) {
                BUTTONSTATE(hwnd) = ((BUTTONSTATE(hwnd) & ~BFCHECK) | wParam);
                if ((hdc = BtnGetDC(hwnd)) != NULL) {
                    DrawCheck(hdc, hwnd);
                    ReleaseDC(hwnd, hdc);
                }
            }
            break;
        }
        break;

    case BM_SETSTYLE:
        LOWORD(hwnd->style) = wParam;
        if (lParam)
            InvalidateRect(hwnd, (LPRECT)NULL, TRUE);
        break;

    case WM_GETDLGCODE:
        switch (bsWnd) {
        case BS_DEFPUSHBUTTON:
            wParam = DLGC_DEFPUSHBUTTON;
            break;
        case BS_PUSHBUTTON:
        case BS_PUSHBOX:
            wParam = DLGC_UNDEFPUSHBUTTON;
            break;
        case BS_AUTORADIOBUTTON:
        case BS_RADIOBUTTON:
            wParam = DLGC_RADIOBUTTON;
            break;
        case BS_GROUPBOX:
            return((long)DLGC_STATIC);
            break;
        default:
            wParam = 0;
            break;
        }
        return((long)(wParam | DLGC_BUTTON));
        break;

    case WM_SETTEXT:
        DefSetText(hwnd, (LPSTR)lParam);
        /* Fall through */
    case WM_ENABLE:
        if ((hdc = BtnGetDC(hwnd)) != NULL) {
            ButtonPaint(hwnd, hdc);
            ReleaseDC(hwnd, hdc);
        }
        break;

    default:
CallDWP:
        return(DefWindowProc(hwnd, message, wParam, lParam));
        break;
    }
    return(0L);
}

SetBtnCapture(hwnd, codeMouse)
register HWND hwnd;
WORD codeMouse;
{
    BUTTONSTATE(hwnd) |= codeMouse;
    if (!(BUTTONSTATE(hwnd) & BFCAPTURED)) {
        BUTTONSTATE(hwnd) |= BFCAPTURED;
        SetCapture(hwnd);
        /* To prevent redundant CLICK messages, we set the INCLICK bit so
           the WM_SETFOCUS code will not do a NotifyParent(BN_CLICKED)
        */
        BUTTONSTATE(hwnd) |= BFINCLICK;
        SetFocus(hwnd);
        BUTTONSTATE(hwnd) &= ~BFINCLICK;
    }
}

/*********************** START OF SPECIFICATION ***********************/
#ifdef COMMENT_LOCAL

ReleaseBtnCapture

LINKAGE:    NEAR PLM

ENTRY:      hwnd, fCheck on stack.

EXIT:       none

EXIT-ERROR:

EFFECTS:    AX,BX,CX,DX,ES are modified.

INTERNAL:
            SetBtnState
            SendMessage
            GetNextDlgGroupItem
            NotifyParent
            ReleaseCapture

EXTERNAL:

WARNINGS:

REVISION HISTORY:
            4/4/86      jpk     Added LEFTTEXT button style.
            4/7/86      jpk     Added this sublogue.

IMPLEMENTATION:

#endif
/*********************** END OF SPECIFICATION *************************/

ReleaseBtnCapture(hwnd, fCheck)
register HWND hwnd;
BOOL fCheck;
{
    register HWND hwndT;
    unsigned check;

    if (BUTTONSTATE(hwnd) & BFSTATE) {
        SetBtnState(hwnd, FALSE);
        if (fCheck) {
            switch (BUTTONSTYLE(hwnd)) {
            case BS_AUTORADIOBUTTON:
                hwndT = hwnd;
                do {
                    if ((WORD)SendMessage(hwndT, WM_GETDLGCODE, 0, 0L) & DLGC_RADIOBUTTON)
                        SendMessage(hwndT, BM_SETCHECK, (hwnd == hwndT), 0L);
                    hwndT = GetNextDlgGroupItem(hwndT->hwndParent, hwndT, FALSE);
                } while (hwndT != hwnd);
                goto DoNotify;
                break;

            case BS_AUTOCHECKBOX:
            case BS_AUTO3STATE:
                check = (BUTTONSTATE(hwnd) & BFCHECK) + 1;
                if (check > (BUTTONSTYLE(hwnd) == BS_AUTO3STATE ? 2 : 1))
                    check = 0;
                SendMessage(hwnd, BM_SETCHECK, check, 0L);
                /* fall thru */
            default:
DoNotify:
                NotifyParent(hwnd, BN_CLICKED);
                break;
            }
        }
    }
    if (BUTTONSTATE(hwnd) & BFCAPTURED) {
        BUTTONSTATE(hwnd) &= ~(BFCAPTURED | BFMOUSE);
        ReleaseCapture();
    }
}

SetBtnState(hwnd, fHilite)
register HWND hwnd;
register BOOL fHilite;
{
    SendMessage(hwnd, BM_SETSTATE, fHilite, 0L);
}

/*********************** START OF SPECIFICATION ***********************/
#ifdef COMMENT_LOCAL

ButtonPaint

LINKAGE:    NEAR PLM

ENTRY:      hwnd, hdc on stack.

EXIT:       none

EXIT-ERROR:

EFFECTS:    AX,BX,CX,DX,ES are modified.

INTERNAL:
            FillWindow
            DrawBtnText
            DrawNewState
            GetClientRect
            DrawPushButton
            DrawCheck
            NotifyParent
            CalcBtnRect
            FillRect
            DrawFrame

EXTERNAL:
            SelectObject

WARNINGS:

REVISION HISTORY:
            4/4/86      jpk     Added LEFTTEXT button style.
            4/7/86      jpk     Added this sublogue.
            4/7/86      jpk     Added PUSHBOX button style.
            6/15/87     edf     Removed fErase flag.

IMPLEMENTATION:

#endif
/*********************** END OF SPECIFICATION *************************/

ButtonPaint(hwnd, hdc)
register HWND hwnd;
register HDC hdc;
{
    WORD bsWnd;
    BOOL far FChildVisible(HWND);
    RECT rc;
    HBRUSH hbrBtnSave;

    /* Don't do all this if the control is not visible. (bobgu 5/20/87) */
    if (!FChildVisible(hwnd))
	return;

    FarSemWriteEnter(SEMRW_GDIOBJECTS);

    if ((bsWnd=BUTTONSTYLE(hwnd)) != BS_GROUPBOX)
        FillWindow(NULL, hwnd, hdc, hbrBtn);

    hbrBtnSave = SelectObject(hdc, hbrBtn);

    switch (bsWnd) {
    case BS_PUSHBUTTON:
    case BS_DEFPUSHBUTTON:
        DrawBtnText(hdc, hwnd, DBT_TEXT | (GetFocus()==hwnd ? DBT_FOCUS : 0));
        DrawNewState(hdc, hwnd, 0);
        GetClientRect(hwnd, (LPRECT)&rc);
        DrawPushButton(hdc, (LPRECT)&rc, TestWF(hwnd, WFDISABLED) ? LOWORD(BS_PUSHBUTTON) : bsWnd, FALSE);
        break;

    case BS_PUSHBOX:
        DrawBtnText(hdc, hwnd, DBT_TEXT | (GetFocus()==hwnd ? DBT_FOCUS : 0));
        DrawNewState(hdc, hwnd, 0);
        break;

    case BS_CHECKBOX:
    case BS_RADIOBUTTON:
    case BS_AUTORADIOBUTTON:
    case BS_3STATE:
    case BS_AUTOCHECKBOX:
    case BS_AUTO3STATE:
        DrawBtnText(hdc, hwnd, DBT_TEXT | (GetFocus()==hwnd ? DBT_FOCUS : 0));
        DrawCheck(hdc, hwnd);
        break;

    case BS_USERBUTTON:
        NotifyParent(hwnd, BN_PAINT);
        if (BUTTONSTATE(hwnd) & BFSTATE)
            NotifyParent(hwnd, BN_HILITE);
        if (TestWF(hwnd, WFDISABLE))
            NotifyParent(hwnd, BN_DISABLE);
        if (GetFocus() == hwnd)
            DrawBtnText(hdc, hwnd,  DBT_FOCUS);
	break;

    case BS_GROUPBOX:
        CalcBtnRect(hwnd, hdc, (LPRECT)&rc, CBR_GROUPFRAME);
        DrawFrame(hdc, (LPRECT)&rc, 1, DF_WINDOWFRAME);
        CalcBtnRect(hwnd, hdc, (LPRECT)&rc, CBR_GROUPTEXT);
        FillRect(hdc, (LPRECT)&rc, hbrBtn);
        DrawBtnText(hdc, hwnd, DBT_TEXT);
        break;
    }
    SelectObject(hdc, hbrBtnSave);

    FarSemWriteLeave(SEMRW_GDIOBJECTS);
}

/*********************** START OF SPECIFICATION ***********************/
#ifdef COMMENT_LOCAL

DrawNewState

LINKAGE:    NEAR PLM

ENTRY:      hdc, hwnd, sOld on stack.

EXIT:       none

EXIT-ERROR:

EFFECTS:    AX,BX,CX,DX,ES are modified.

INTERNAL:
            GetClientRect
            DrawPushButton
            DrawCheck

EXTERNAL:

WARNINGS:

REVISION HISTORY:
            4/4/86      jpk     Added LEFTTEXT button style.
            4/7/86      jpk     Added this sublogue.
            4/7/86      jpk     Added PUSHBOX button style.

IMPLEMENTATION:

#endif
/*********************** END OF SPECIFICATION *************************/

DrawNewState(hdc, hwnd, sOld)
register HDC hdc;
register HWND hwnd;
int sOld;
{
    RECT rc;

    if (sOld)
        sOld = BFSTATE;

    if (sOld != (BUTTONSTATE(hwnd) & BFSTATE)) {
        switch (BUTTONSTYLE(hwnd)) {
        case BS_GROUPBOX:
            break;
        case BS_PUSHBUTTON:
        case BS_DEFPUSHBUTTON:
        case BS_PUSHBOX:
            GetClientRect(hwnd, (LPRECT)&rc);
            DrawPushButton(hdc, (LPRECT)&rc, WBUTTONSTYLE(hwnd), TRUE);
            break;
        default:
            DrawCheck(hdc, hwnd);
            break;
        }
    }
}

/*********************** START OF SPECIFICATION ***********************/
#ifdef COMMENT_LOCAL

DrawCheck

LINKAGE:    NEAR PLM

ENTRY:      hdc, hwnd on stack.

EXIT:       none

EXIT-ERROR:

EFFECTS:    AX,BX,CX,DX,ES are modified.

INTERNAL:
            CalcBtnRect
            SetRect
            PaintRect
            BltColor

EXTERNAL:

WARNINGS:

REVISION HISTORY:
            4/4/86      jpk     Added LEFTTEXT button style.
            4/7/86      jpk     Added this sublogue.

IMPLEMENTATION:

#endif
/*********************** END OF SPECIFICATION *************************/

DrawCheck(hdc, hwnd)
register HDC  hdc;
register HWND hwnd;
{
    RECT rc;
    int ibm;
    int cxOff, cyOff;

    CalcBtnRect(hwnd, hdc, (LPRECT)&rc, CBR_CHECKBOX);

    ibm = 0;
    switch (BUTTONSTYLE(hwnd)) {
    case BS_AUTORADIOBUTTON:
    case BS_RADIOBUTTON:
        ibm = 1;
        break;
    case BS_3STATE:
    case BS_AUTO3STATE:
        if ((BUTTONSTATE(hwnd) & BFCHECK) == 2)
            ibm = 2;
        break;
    }
    cxOff = ( ((BUTTONSTATE(hwnd) & BFCHECK) ? 1 : 0) |
               ((BUTTONSTATE(hwnd) & BFSTATE) ? 2 : 0) ) * oemInfo.cxbmpChk;
#ifdef DISABLE
    cyOff = ibm * oemInfo.cybmpChk;
#endif

    /* 10-Mar-1987. 3 row button bitmap now stored as one long row */
    cxOff += (ibm * oemInfo.bmbtnbmp.cx);
    cyOff = 0;

    /* fill in this area with background brush */
    rc.right = rc.left + oemInfo.cxbmpChk;
    rc.bottom = rc.top + oemInfo.cybmpChk;

    /* semaphore already convered in PaintRect */
    PaintRect(hwnd->hwndParent, hwnd, hdc, hbrBtn, (LPRECT)&rc);

    BltColor(hdc, NULL, oemInfo.bmbtnbmp.hBitmap, rc.left,
        rc.top, oemInfo.cxbmpChk, oemInfo.cybmpChk, resInfo.dxCheckBoxes+cxOff, cyOff, TRUE);
}

/*********************** START OF SPECIFICATION ***********************/
#ifdef COMMENT_LOCAL

DrawBtnText

LINKAGE:    NEAR PLM

ENTRY:      hdc, hwnd, dbt

EXIT:       none

EXIT-ERROR:

EFFECTS:    AX,BX,CX,DX,ES are modified.

INTERNAL:
            CalcBtnRect
            PSGetTextExtent
            SetRect
            FillRect
            GrayString
            BtnTextOut

EXTERNAL:
            lstrlen
            SetBkMode

WARNINGS:

REVISION HISTORY:
            4/4/86      jpk     Added LEFTTEXT button style.
            4/7/86      jpk     Added this sublogue.

IMPLEMENTATION:

#endif
/*********************** END OF SPECIFICATION *************************/

DrawBtnText(hdc, hwnd, dbt)
HDC hdc;
register HWND hwnd;
BOOL dbt;
{
    RECT rc, rcClient;
    int cbr;
    int cch;
    HBRUSH hbr;
    register int x;
    int y, temp;
    long extent;

    if (dbt == DBT_FOCUS && BUTTONSTYLE(hwnd) == BS_GROUPBOX)
	    return;
    
    cbr = mpStyleCbr[BUTTONSTYLE(hwnd)];
    CalcBtnRect(hwnd, hdc, (LPRECT)&rc, cbr);

    cch = lstrlen((LPSTR)hwnd->szName);
    extent = BtnGetTextExtent(hdc, (LPSTR)hwnd->szName, cch);
    x = rc.left;
    if (cbr != CBR_CHECKTEXT)
        x += (int)(rc.right - rc.left - LOWORD(extent)) / 2;
    y = rc.top + (int)(rc.bottom - rc.top - HIWORD(extent)) / 2;

    /* Draw Button Text */
    if (dbt & DBT_TEXT) {
        if (TestWF(hwnd, WFDISABLED)) {
	    GrayString(hdc, NULL, (FARPROC)PrefixTextOut, (DWORD)(LPSTR)hwnd->szName, cch, x, y, 0, 0);
        } else {
	    SetBkMode(hdc, OPAQUE);
            BtnTextOut(hdc, x, y, (LPSTR)hwnd->szName, cch);
        }
    }
    /* Draw Button Focus */
    if (dbt & DBT_FOCUS) {
	GetClientRect(hwnd, (LPRECT)&rcClient);
	if ((rc.top = y - cyBorder) < 0)
	    rc.top = 0;
	if ((rc.bottom = rc.top + (cyBorder << 1) + HIWORD(extent) + cyBorder) > rcClient.bottom)
	    rc.bottom = rcClient.bottom;
	/* don't draw the grey frame in the push button outline. */
	if (BUTTONSTYLE(hwnd) == BS_PUSHBUTTON || BUTTONSTYLE(hwnd) == BS_DEFPUSHBUTTON) {
	    if ((temp = rcClient.bottom - (cyBorder << 1)) < rc.bottom)
		rc.bottom = temp;
	}
	if ((rc.left = x - (cxBorder << 1)) < 0)
	    rc.left = 0;
	rc.right = rc.left + (cxBorder << 2) + LOWORD(extent);

        if (rc.right > rcClient.right)
            rc.right = rcClient.right;

        UnRealizeObject(hbrGray);
        LRCCFrame(hdc, (BUTTONSTYLE(hwnd) == BS_USERBUTTON)? (LPRECT)&rcClient : (LPRECT)&rc,
            hbrGray, PATINVERT);
    }
}

NotifyParent(hwnd, code)
register HWND hwnd;
unsigned code;
{
    HWND FAR GetWindowCreator(HWND);

    SendMessage(GetWindowCreator(hwnd),
	WM_COMMAND, (WORD)hwnd->hMenu, MAKELONG(hwnd, code));
}

/*********************** START OF SPECIFICATION ***********************/
#ifdef COMMENT_LOCAL

CalcBtnRect

LINKAGE:    NEAR PLM

ENTRY:      hwnd, hdc, lprc, code on stack.

EXIT:       none

EXIT-ERROR:

EFFECTS:    AX,BX,CX,DX,ES are modified.

INTERNAL:
            GetClientRect
            PSGetTextExtent

EXTERNAL:
            lstrlen
            GetTextExtent

WARNINGS:

REVISION HISTORY:
            4/4/86      jpk     Added LEFTTEXT button style.
            4/7/86      jpk     Added this sublogue.

IMPLEMENTATION:

#endif
/*********************** END OF SPECIFICATION *************************/

CalcBtnRect(hwnd, hdc, lprc, code)
register HWND hwnd;
register HDC hdc;
LPRECT lprc;
int code;
{
    extern int cxBorder;
    extern int cxSysFontChar;


    int cch;
    long extent;

    GetClientRect(hwnd, lprc);
    switch (code) {
    case CBR_CLIENTRECT:
        break;

    case CBR_CHECKBOX:
        lprc->top += ((lprc->bottom - lprc->top - oemInfo.cybmpChk) / 2);
        if (hwnd->style & (LOWORD(BS_LEFTTEXT)))
            lprc->left = lprc->right - oemInfo.cxbmpChk;
        break;

    case CBR_CHECKTEXT:
        if (!(hwnd->style & (LOWORD(BS_LEFTTEXT))))
            lprc->left += oemInfo.cxbmpChk + 4;
	else
	    lprc->left += cxBorder;
        break;

    case CBR_GROUPTEXT:
        cch = lstrlen((LPSTR)hwnd->szName);
        if (!cch) {
            SetRectEmpty(lprc);
            break;
        }
        extent = BtnGetTextExtent(hdc, (LPSTR)hwnd->szName, cch);
        lprc->left  += cxSysFontChar - cxBorder;
        lprc->right  = lprc->left + LOWORD(extent) + 4;
        lprc->bottom = lprc->top  + HIWORD(extent) + 4;
        break;

    case CBR_GROUPFRAME:
        extent = BtnGetTextExtent(hdc, (LPSTR)&szOneChar, 1);
        lprc->top += ((int)HIWORD(extent) / 2);
        break;
    }
}

/*********************** START OF SPECIFICATION ***********************/
#ifdef COMMENT_LOCAL

PrefixTextOut

LINKAGE:    FAR PLM

ENTRY:	    hwnd, lpstr, cch

EXIT:	    none

EXIT-ERROR:

EFFECTS:

INTERNAL:
            BtnTextOut

EXTERNAL:

WARNINGS:

REVISION HISTORY:
	    12/11/86	bobgu	    Initial version.

IMPLEMENTATION:
    This function is used as the target of the function called by
    GrayString.  It simply does a BtnTextOut into the hdc specified.
    This function handles strings with a prefix character.


#endif
/*********************** END OF SPECIFICATION *************************/

BOOL far PrefixTextOut(hdc, lpstr, cch)
HDC hdc;
LPSTR lpstr;
int cch;
{
    BtnTextOut(hdc, 0, 0, lpstr, cch);
    return(TRUE);
}


void BtnTextOut(hdc, xLeft, yTop, lpsz, cch)
register HDC     hdc;
register int     xLeft;
int     yTop;
LPSTR   lpsz;
int     cch;
{
    int FAR StripPrefix(LPSTR, LPSTR);
    char tempBuff[255];

    /* for old apps, we need to strip out the "&" characters before
     * we print in case someone is using a new printer driver with
     * an old app. (Tue 27-Oct-1987 : bobgu)
     */
    if (GetAppVer() < VER) {
	cch -= StripPrefix((LPSTR)tempBuff, lpsz);
	TextOut(hdc, xLeft, yTop, (LPSTR)tempBuff, cch);
    } else {
	PSMTextOut(hdc, xLeft, yTop, lpsz, cch);
    }
}


DWORD BtnGetTextExtent(hdc, lpstr, cch)
HDC hdc;
LPSTR   lpstr;
register int cch;
{
    /* For old apps, we need to compute the length as if the "&" characters
     * were not there, since we strip them out in BtnTextOut.  So, just use
     * PSMGetTextExtent for both. (Tue 27-Oct-1987 : bobgu)
     */

    return(PSMGetTextExtent(hdc, lpstr, cch));
}
