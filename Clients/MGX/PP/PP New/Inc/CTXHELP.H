/***************************************************************************** 

    ( c) Copyright 1992-94 MICROGRAFX, Inc., All Rights Reserved. This material
    is confidential and a trade secret. Permission to use this work for any
    purpose must be obtained in writing from:

           MICROGRAFX, 1303 E. Arapaho Rd., Richardson, TX  75081

******************************************************************************

                                  Help Context

*****************************************************************************/

#ifndef CTXHELP_H_
#define CTXHELP_H_

#define HINTS_ON     	( HT_STATIC | HT_FLOATING )
#define HINTBREAK 		_T('~')
#define TITLEBREAK 		_T(';')

// Hint types 
#define HT_OFF              0x0000
#define HT_STATIC           0x0001
#define HT_FLOATING         0x0002
#define HT_TYPE_MASK        0x00FF

// Bubble hint states
#define HL_BUBBLE_ON        0x0100
#define HL_BUBBLE_ACTIVE    0x0200
#define HL_BUBBLE_PENDING   0x0400
#define HL_BUBBLE_SUSPEND   0x0800
#define HL_BUBBLE_HINTTITLE 0x1000
#define HL_BUBBLE_MASK      0xFF00

#define HL_BUBBLE_QUERY     3

#define BUBBLE_COLOR        0x007FFFFF

/************************************ Macros ********************************/
/************************************ Types *********************************/
/*********************************** Classes ********************************/

class CHelpContext
{
private:
    CWnd * pHintName;           //  Pointer to the window for the hint title or name
    CWnd * pHintWnd;            //  Pointer to the control (window) for the hints
    CMDIFrameWnd * pMainWnd;    //  Pointer to the main app window.
    HWND   m_hWndID;
    HWND   hBubbleWnd;          //  Bubble hint window

    CString   CurHintLine;
    CString   CurHintContent;
    CString   CurHintTitle;
    UINT      CurHintID;
    UINT      CurHelpTopic;
    UINT      CurrentTopicHelpTopic;
    UINT      m_HintState;          // Type in LOBYTE, bubble state in HIBYTE
    HINSTANCE hHintLib;             // Handle to the hint string DLL
    TIMERPROC lpfnBubbleTimerProc;

    void process_hint( BOOL bLoadString = TRUE );
    void create_bubble( BOOL bTimer = FALSE );      

public:
    UINT TimerID;

    CHelpContext( UINT HelpTopic = 0, UINT HintID = 0 );
    ~CHelpContext();
    void InitBubbleHints( void );
    void SetHintWindows( CWnd * pName = NULL, CWnd * pContent = NULL, UINT Type = HT_OFF );
    void OnHints( HWND hWnd = NULL, UINT m_HintID = 0 );
    BOOL BubbleHintsOn( BOOL bState = HL_BUBBLE_QUERY );
    void display_bubble( BOOL bDisplay = TRUE, BOOL bTimer = FALSE ); // Access for timerproc
    void SetHintState( UINT HintID );
    void SetHintState( LPCTSTR pString );
};

/****************************** Inline Methods ******************************/
/******************************* Public Data ********************************/

extern CHelpContext HelpContext;

/****************************** Public Functions ****************************/

extern HWND MyChildWindowFromPoint ( HWND hParent, POINT point );

#endif  // CTXHELP_H_


