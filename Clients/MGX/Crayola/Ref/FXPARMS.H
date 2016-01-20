typedef LPTR (far *LPTRPROC)();

typedef int (FAR *LPCLOCKPROC)(int part, int whole, BOOL bEscapable);
typedef int (FAR *LPMESSAGEPROC)(int idStr, va_list lpArgs);


typedef struct _FXUIParms
	{							// filled by:
	HINSTANCE	hInstance;		// caller's instance handle	app
	HWND		hWnd;			// caller's window frame	app
	COLORINFO	ActiveColor;	// Active color		app
	COLORINFO	AlternateColor;	// Alternate color		app
	int			src_npix;		// pixels / line in image	app
	int			src_nlin;		// numbers of lines in image	app
	int			src_resolution;	// dpi app
	int			Depth;			// data depth app
	LPMESSAGEPROC	Message;		// Ptr to Message Func app
	LPVOID 		lpUIData;		// Any data setup/used by dll
	} FXUIPARMS;
typedef FXUIPARMS FAR *LPFXUIPARMS;



typedef struct _FXParms
	{							// filled by:
	HINSTANCE		hInstance; 		// caller's instance handle	app
	LPCLOCKPROC AstralClockCursor; // AstralClockCursor proc	app
	LPVOID		lpClockData;	// For clock management (dll's descresion) dll
	LPTRPROC	frame_ptr; 		// frame_ptr proc		app
	LPROC		frame_write; 	// frame_write proc		app
	LPTRPROC	frame_open;		// frame_open proc		app
	LPTRPROC	frame_close;	// frame_close proc		app
	LPMESSAGEPROC	Message;		// Ptr to Message Func 	app
	COLORINFO	ActiveColor;	// Active color			app
	COLORINFO	AlternateColor;	// Alternate color		app
	LPFRAME		lpSrcFrame;		// source frame ptr		app
	LPFRAME		lpDstFrame;		// destination frame ptr 	app
	int			src_npix;		// pixels / line in image	app
	int			src_nlin;		// numbers of lines in image	app
	int			src_resolution;	// dpi					app
	int			Depth;			// data depth			app
	int			dst_npix;		// pixels / line in image	dll
	int			dst_nlin;		// numbers of lines in image	dll
	int			dst_resolution;	// dpi						dll
	int			DstDepth;		// desination frame depth	dll
	LPTR		lpOut;			// output data buffer		app
	RECT		rOut;			// output rectangle			dll
	LPVOID 		lpData;			// Any data setup/used by 	dll
	} FXPARMS;
typedef FXPARMS far *LPFXPARMS;

// see effect.txt for more info on the queries
typedef enum
	{
	FXQ_VERSION,
	FXQ_MULTITHREAD1,
	} FX_QUERY;

// compatibilty FLAG!
#define FXVERSION 1

// for the FXQ_MULTITHREAD1 query
typedef struct _FXThreaddata1
	{
		BOOL bCanMultithread;	// Can the effect be multithreaded
		WORD wNumberOfThreads;	// How Many threads (app fills with suggestion)
		LPRECT lpRectList;		// dll's suggested breakup. (may be NULL, app must free)
		BOOL bMustUseRectList; 	// Must the app use the RectList to thread.
	}FXTHREADDATA1, FAR *LPFXTHREADDATA1;

