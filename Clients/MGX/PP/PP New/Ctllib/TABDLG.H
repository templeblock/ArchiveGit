/*-------------------------------------------------------------------------
    %%File:  tabdlg.h
    %%Owner: stevebu
    
-------------------------------------------------------------------------*/

// typedefs used to make our lives easier
typedef unsigned int FTSW;
#ifdef WIN32
typedef int XY;
#else
typedef int	XY;
#endif

// Class Name of Tab Switcher Window
#define szTswClass			"TSWCTRL"

// Class Name of Tab Switcher Border Window
#define szTswBorderClass	"TSWBDRCTRL"


/* tab switcher initialization flags */
#define ftswNil			((FTSW)0x0000)
#define ftswEnabled		((FTSW)0x0001)
#define ftswDisabled	((FTSW)0x0002)
#define ftswOnlyOne		((FTSW)0x0004)
#define ftswDirectAccel	((FTSW)0x0008)
#define ftswValidate	((FTSW)0x0010)
#define ftswHidSelect	((FTSW)0x0020)
#define ftswChiseled	((FTSW)0x0040)				// Use 3D Dialog looks
#define ftswCallOnInit	((FTSW)0x0080)				// Callback with WM_TABSWITCH during WM_CREATE

#define cRowsMaxTsw 4
#define cRowsAutoTsw (-1)
#define cidNull		(0)							// Please don't use this control id


#define cbMaxSzTab	256
#define cchMaxSzTab	255

typedef struct _TSWLPARAM							// Passed to Dialog Proc's lParam
	{	
	union
		{
		struct	// WM_TABTEXT
			{
			unsigned int isz;						// Index of String to Provide (0 based)
			unsigned char szTab[cbMaxSzTab];				// Tab Name filled in by Dialog Proc using isz
			};
		struct	// WM_TABINIT
			{
			unsigned int cRows;						// Number of Tab Rows
			unsigned int iselInit;					// Tab to be visible when dialog is first displayed
			FTSW ftsw;								// Flags described above
			int cidTswBdr;							// Control ID of Tab Switcher Border Window
			};	
		};
	} TSWLPARAM;

typedef TSWLPARAM FAR *LPTSWLPARAM;	
	
#define IRowLastTswh(ptswh) ((ptswh)->cRows - 1)
#define IColLastTswh(ptswh) ((ptswh)->ctswRow - 1)
#define itswNil	((unsigned int)(-1))
#define LptswLParamFromLParam(lParam) ((LPTSWLPARAM)lParam)
#define LszTabFromLParam(_lParam_) (((LPTSWLPARAM)(_lParam_))->szTab)
#define IszTabFromLParam(_lParam_) (((LPTSWLPARAM)(_lParam_))->isz)
#define CtrlIDFromWParam(wParam) (wParam)
#define IselNewFromLParam(lParam) (HIWORD(lParam))
#define IselOldFromLParam(lParam) (LOWORD(lParam))

// Let's hope these stay unique
#define WM_TABMIN	(WM_USER +432)

/*
	WM_TABSWITCH MESSAGE
	HWND:		hwnd of Dialog
	MESSAGE:	WM_TABSWITCH
	WPARAM:		Control ID of Tab Switcher Window
	LPARAM:		Newly Selected Tab (iselNew) and Old Tab (iselOld)
				Uses the macros IselNewFromLParam and IselOldFromLParam to access these
	RETURN VAL:	Ignored
*/
#define WM_TABSWITCH (WM_TABMIN +0)
/* void Cls_OnTabSwitch(HWND hDlg, int idTabSwitcher, int iSelOld, iSelNew); */
#define HANDLE_WM_TABSWITCH(hwnd, wParam, lParam, fn) \
    ((fn)((hwnd), (int)(wParam), (int)LOWORD(lParam), (int)HIWORD(lParam)), 0L)
#define FORWARD_WM_TABSWITCH(hwnd, idTabSwitcher, iSelOld, iSelNew, fn) \
    (void)(fn)((hwnd), WM_TABSWITCH, (WPARAM)(idTabSwitcher), MAKELPARAM((iSelOld),(idSelNew)))


/*
	WM_TABTEXT MESSAGE
	HWND:		hwnd of Dialog
	MESSAGE:	WM_TABTEXT
	WPARAM:		Control ID of Tab Switcher Window
	LPARAM:		Far Pointer to a TSWLPARAM structure.  The lptswlparm->isz is zero based
				index of the tab string we're enumerating. You fill in szTab with a NULL
				terminated string that will be displayed in the tab
	RETURN VAL:	TRUE(1) to keep enumerating tabs, FALSE(0) to stop.  You should return FALSE(0)
				on the next callback after your last tab is enumerated.
*/
#define WM_TABTEXT (WM_TABMIN +1)
/* BOOL Cls_OnTabText(HWND hDlg, int idTabSwitcher, LPTSWLPARAM lptswlparm); */
#define HANDLE_WM_TABTEXT(hwnd, wParam, lParam, fn) \
    (LRESULT)(DWORD)(fn)((hwnd), (int)(wParam), (LPTSWLPARAM)(lParam));
#define FORWARD_WM_TABTEXT(hwnd, idTabSwitcher, lptswlparm, fn) \
    (BOOL)(DWORD)(fn)((hwnd), WM_TABTEXT, (WPARAM)(idTabSwitcher), (LPARAM)(LPTSWLPARAM)(lptswlparm))

/*
	WM_TABINIT MESSAGE
	HWND:		hwnd of Dialog
	MESSAGE:	WM_TABINIT
	WPARAM:		Control ID of Tab Switcher Window
	LPARAM:		Far Pointer to a TSWLPARAM structure. You're responsible for filling out the
				WM_TABINIT structure of TSLPARAM.
				
	RETURN VAL:	Ignored.
*/
#define WM_TABINIT (WM_TABMIN +2)
/* void Cls_OnTabInit(HWND hDlg, int idTabSwitcher, LPTSWLPARAM lptswlparm); */
#define HANDLE_WM_TABINIT(hwnd, wParam, lParam, fn) \
    ((fn)((hwnd), (int)(wParam), (LPTSWLPARAM)(lParam)), 0L)
#define FORWARD_WM_TABINIT(hwnd, idTabSwitcher, lptswlparm, fn) \
    (void)(fn)((hwnd), WM_TABINIT, (WPARAM)(idTabSwitcher), (LPARAM)(LPTSWLPARAM)(lptswlparm))

// Function Prototypes

/* InitTabSwitcher

	Registers Window Classes for hinst and inits DLL.
*/
void FAR PASCAL InitTabSwitcher(HINSTANCE hinst);

void FAR PASCAL ExitTabSwitcher();

/* FTabSwitcherAccel

	Turns fTrue(1) if ch is an accelerator for a tab. fFalse(0) if not.
*/

short FTabSwitcherAccel(HWND hwndDlg, WPARAM ch);

/* TabSwitch

	Switches current tab to itswNew.  If fFocus set the
	focus to the tab switcher window.  cidTswBdr is the Windows
	Control ID for the tab switcher border you passed in WM_INITAB
*/
VOID TabSwitch(UINT cidTswBdr, UINT itswNew, BOOL fFocus);
                                  
/* GetCurrentTab

	Returns the currect tab given a cidTswBdr. cidTswBdr is the Windows
	Control ID for the tab switcher border you passed in WM_INITAB
*/                                                      
UINT GetCurrentTab(UINT cidTswBdr);
