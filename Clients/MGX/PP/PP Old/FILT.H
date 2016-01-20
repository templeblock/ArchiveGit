
typedef LPTR (far *LPTRPROC)();

typedef struct _FiltParms
	{							// filled by:
	HANDLE		hInstance; 	// caller's instance handle	app
	HWND		hWnd;		// caller's window frame	app
	LPROC		AstralClockCursor; // AstralClockCursor proc	app
	LPTRPROC	frame_set;	// frame_set proc		app
	LPTRPROC	CachePtr; 	// CachePtr proc		app
	LPROC		FilterWrite; 	// CacheWrite proc		app
	LPROC		frame_getRGB;	// frame_getRGB proc		app
	LPROC		frame_putRGB;	// frame_putRGB proc		app
	RGBS		ActiveRGB;	// Active RGB color		app
	RGBS		AlternateRGB;	// Alternate RGB color		app
	LPTR		lpSrcFrame;	// source frame ptr		app
	LPTR		lpDstFrame;	// destination frame ptr 	app
	int		src_npix;	// pixels / line in image	app
	int		src_nlin;	// numbers of lines in image	app
	int		src_resolution;	// dpi				app
	int		Depth;		// data depth			app
	RECT		rMask;		// mask rectangle		app
	LPTR		lpOut;		// output data buffer		app
	int		dst_npix;	// pixels / line in image	dll
	int		dst_nlin;	// numbers of lines in image	dll
	int		dst_resolution;	// dpi				dll
	RECT		rOut;		// output rectangle		dll
	} FILTPARMS;
typedef FILTPARMS far *LPFILTPARMS;
