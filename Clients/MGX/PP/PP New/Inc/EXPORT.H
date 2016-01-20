
typedef LPTR (far *LPTRPROC)();

typedef struct _ExpParms {
	HANDLE     hInstance; 	      // caller's instance handle app
	HWND       hWnd;		      // caller's window frame app
	LPROC      StatusCallback;    // AstralClockCursor proc app
	LPTRPROC   DataPointer;       // CachePtr proc app
	LPTSTR     lpszFileName;      // Image Filename
	int        XSize;             // Image XSize
	int        YSize;             // Image YSize
	int        Resolution;        // Image Resolution
	int        Depth;             // Image Depth
} EXPPARMS, far *LPEXPPARMS;

typedef int (FAR * EXPORTPROC)(LPEXPPARMS);
