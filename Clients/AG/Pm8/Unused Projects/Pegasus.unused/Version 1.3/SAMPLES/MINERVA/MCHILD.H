
/*
    mchild.h
*/



/* MDI child window instance data.  Accessed by
    (CHILDINSTANCEDATA PICFAR*)GetWindowLong(hwnd, 0) -- but typically using the macro
    ChildGetInstanceData(hwnd)
    
   This structure is pretty freely accessed by mframe.c, mopen.c, msave.c and mmenu.c (as
   well as mchild.c).  I hope to keep the opcode modules and any functions they call (thus
   mmisc.c, mdlg.c and mdlgctrl.c) free from having to know anything about it so the opcode
   modules are a little more generic. */
typedef struct _ChildInstanceData
    {
    LPSTR          pszFilename;     /* source file name */
    BYTE PICHUGE*  pSrc;            /* -> source file image in memory */
    DWORD          dwSrcLen;        /* len of source file image (and source file) */
    PIC_PARM       SrcPicParm;      /* -> source file PIC_PARM after PegasusQuery */
    BYTE PICHUGE*  pDib;            /* -> Dib output from Open opcode */
    DWORD          dwDibLen;        /* len of Dib */
    PIC_PARM       DibPicParm;      /* PIC_PARM after Pegasus() in Open opcode */
    BOOL           bSrcModified;    /* possibly TRUE after a transformation opcode is applied,
                                        FALSE after Save or Save As */
    BOOL           bModified;       /* TRUE after a transformation opcode is applied,
                                        FALSE after Save or Save As */
    BOOL           bFormatModified; /* TRUE after a transformation opcode is applied if it
                                        results in a different source buffer format,
                                        FALSE after Save or Save As */
    int            xWindowOrg;      /* for scrolling, left image coordinate at window left 0 */
    int            yWindowOrg;      /* for scrolling, top image coordinate at window top 0 */
    BOOL           bLButtonDown;    /* TRUE after mouse left down in window, before mouse up */
    BOOL           bRButtonDown;    /* TRUE after mouse right down in window, before mouse up */
    POINT          ptMouse;         /* Mouse position at LButtonDown or last MouseMove as
                                        bLButtonDown == TRUE */
    HPALETTE       hPalette;        /* NULL until hpalette created from colortable in DibPicParm */
    OPENOP PICFAR* pOpenOp;         /* -> OPENOP corresponding to source file */
    BOOL           bDisableSetScrollRanges;
    BOOL           bSizing;
    int            nLineScrollIncrement;
    DWORD          dwTimeNextLineScrollIncrement;
    int            nWaitCursor;
    HCURSOR        hOldCursor;
    LONG           lDibHeight;
    LPVOID         pOpenData;           /* received from Open Opcode for its private use */
    } CHILDINSTANCEDATA;



#define ChildGetInstanceData(hwnd) ( IsWindow(hwnd) ? \
    (CHILDINSTANCEDATA PICFAR*)GetWindowLong(hwnd, 0) : 0 )



LRESULT CALLBACK ChildWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
BOOL ChildIsModified(HWND hwnd);
BOOL ChildQueryCloseAll(void);
BOOL ChildQueryClose(HWND hwnd);
void ChildInvalidatePalette(HWND hwnd);
void ChildSetScrollRanges(HWND hWnd);
void ChildSetWindowSize(HWND hWnd);
HWND ChildGetActive(void);
void ChildWaitCursorOn(HWND hwnd);
void ChildWaitCursorOff(HWND hwnd);
void ChildSetDibHeight(HWND hwnd, LONG lDibHeight);
