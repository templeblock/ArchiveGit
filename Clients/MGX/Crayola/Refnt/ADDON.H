// addon.h - header for addon DLL modules for Crayola products

//----------------------------------------------------------------------
// Misc. defines
//----------------------------------------------------------------------
#define ROOM_ADDON		 16030
#define GWL_IMAGE			 8
#define GWL_IMAGE_TOOLPROC	12
#define MAX_PATH	  		80
#define IDN_ART			  1980
#define GWW_FONT	   		 2

//----------------------------------------------------------------------
// typedefs for function callbacks
//----------------------------------------------------------------------
typedef FARPROC (FAR PASCAL * LPFNGETAPPFUNCTION) (int);
typedef int     (FAR PASCAL * LPFNASTRALDLGEND) (HWND, int);
typedef BOOL    (FAR PASCAL * LPFNASTRALIMAGELOAD) (WORD, LPSTR, int, BOOL);
typedef void    (FAR PASCAL * LPFNGOROOM) (HINSTANCE, ITEMID, BOOL);
typedef void    (FAR PASCAL * LPFNGOPAINTAPP) (HWND, LPSTR);

typedef LPFRAME (FAR PASCAL * LPFNTIFRDR) (LPSTR, int, LPINT, BOOL);
typedef LPFRAME (FAR PASCAL * LPFNTIFFRESOURCE2FRAME) (HINSTANCE, LPSTR);

typedef BOOL    (FAR PASCAL * LPFNSOUNDSTARTRESOURCE) (LPSTR, BOOL, HINSTANCE);
typedef	BOOL	(FAR PASCAL * LPFNSOUNDSTARTID) (ITEMID, BOOL, HINSTANCE);

typedef HBITMAP (FAR PASCAL * LPFNFRAMETOBITMAP) (LPFRAME, LPRECT);
typedef void    (FAR PASCAL * LPFNFRAMECLOSE) (LPFRAME);
typedef int     (FAR PASCAL * LPFNFRAMEDEPTH)(LPFRAME);
typedef int     (FAR PASCAL * LPFNFRAMEYSIZE) (LPFRAME);
typedef int     (FAR PASCAL * LPFNFRAMEXSIZE) (LPFRAME);
typedef LPTR	(FAR PASCAL * LPFNFRAMEPOINTER) (LPFRAME, int, int, BOOL);
typedef void 	(FAR PASCAL * LPFNFRAMESETRGB) (LPFRAME, LPRGB, LPTR, int);
typedef void 	(FAR PASCAL * LPFNFRAMEGETRGB) (LPFRAME, LPTR, LPRGB, int);
typedef LPFRAME	(FAR PASCAL * LPFNSIZEFRAME) (LPFRAME, int, int, int, BOOL);
typedef LPFRAME	(FAR PASCAL * LPFNEXPANDFRAME) (LPFRAME, int, int, int, int, BYTE);

typedef void    (FAR PASCAL * LPFNIMGADDNEWOBJECT) (LPIMAGE, LPOBJECT);
typedef BOOL    (FAR PASCAL * LPFNIMGCOMBINEOBJECTS) (LPIMAGE, BOOL, BOOL, BOOL);
typedef int		(FAR PASCAL * LPFNIMGCOUNTOBJECTS) (LPIMAGE);
typedef LPOBJECT(FAR PASCAL * LPFNIMGGETBASE) (LPIMAGE);
typedef void    (FAR PASCAL * LPFNIMGDELETEOBJECT) (LPIMAGE, LPOBJECT);
typedef LPFRAME (FAR PASCAL * LPFNIMGGETBASEEDITFRAME) (LPIMAGE);
typedef LPOBJECT(FAR PASCAL * LPFNIMGGETNEXTOBJECT) (LPIMAGE, LPOBJECT, BOOL, BOOL);
typedef void	(FAR PASCAL * LPFNIMGREPLACEBASEEDITFRAME) (LPIMAGE, LPFRAME);
typedef void 	(FAR PASCAL * LPFNIMGSETPIXELPROC) (LPBLTPROCESS);

typedef int     (FAR        * LPFNANIMATEPROC) (HWND, LPARAM, UINT);
typedef int     (FAR PASCAL * LPFNSTARTANIMATION) (HWND);
typedef void    (FAR PASCAL * LPFNSTOPANIMATION) (void);
typedef void	(FAR PASCAL * LPFNANIMATEDIBID) (HWND, LPRECT, HINSTANCE,
												ITEMID, LPTR, LPSTR);

typedef LPFRAME (FAR PASCAL * LPFNMASKCREATEALPHAFRAME) (LPFRAME);
typedef LPMASK  (FAR PASCAL * LPFNMASKCREATE) (LPFRAME, int, int, BOOL, BOOL);
typedef BOOL    (FAR PASCAL * LPFNMASKCLOSE) (LPMASK);

typedef LPOBJECT(FAR PASCAL * LPFNOBJCREATEFROMFRAME) (STORAGE_TYPE, LPFRAME,
													LPALPHA, LPRECT, BOOL);
typedef void    (FAR PASCAL * LPFNOBJUNLINKOBJECT) (LPOBJECTLIST, LPPRIMOBJECT);


//----------------------------------------------------------------------
// defines for GetAppFunction function ID's
// used by raddon.c module in the app. also
//----------------------------------------------------------------------
#define A_IMAGE_LOAD		1
#define A_DLGEND			2
#define T_TIFRDR			3
#define F_FTOBMP			4
#define F_FDEPTH			5
#define F_FGETRGB			6
#define F_FPOINTER			7
#define F_FXSIZE			8
#define F_FYSIZE			9
#define F_FCLOSE			10
#define I_GETBASE			11
#define I_GETBASEEDITF		12
#define I_GETNEXTOBJ		13
#define I_ADDNEWOBJ			14
#define I_COMBINEOBJ		15
#define I_DELETEOBJS		16
#define I_SETPIXELPROC		17
#define	I_CHANGESEL			18
#define	I_ADJUSTLSO			19
#define	I_FINDOBJ			20
#define AN_ANIMATEPROC		21
#define R_GOROOM			22
#define R_GOPAINT			23
#define	M_MASKCLOSE			24
#define	M_MASKCREATE		25
#define	M_MASKCREATEALPHAF	26
#define O_CREATEFF			27
#define	O_OBJUNLINK			28
#define	P_PIXMAPFRAME		29
#define	F_ATOWINRECT		30
#define A_STARTANI			31
#define A_STOPANI			32
#define	S_SOUNDSTARTRES		33
#define	S_SOUNDSTARTFILE	34
#define	S_SOUNDSTOP			35
#define S_SOUNDSTOPFREE		36
#define S_MIXSOUNDSTARTRES	37
#define S_MIXSOUNDSTARTFILE	38
#define	S_MIXSOUNDSTOPALL	39
#define F_FSETRGB			40
#define T_TIFFRES2FRAME		41
#define I_DELETEOBJ			42
#define S_SOUNDSTARTID		43
#define	AN_ANIMATEDIBID		44
#define	I_IMGREPBASEEDITF	45
#define	I_IMGCOUNTOBJS		46
#define F_SIZEFRAME			47
#define F_EXPANDFRAME		48
#define	F_FRAMERES			49
#define	F_FRAMEGETBACKGND	50


//----------------------------------------------------------------------
// misc.
//----------------------------------------------------------------------
#ifdef WIN32
#define GET_CLASS_STYLE(hWnd) \
			GetClassLong(hWnd, GCL_STYLE)
#define SET_CLASS_STYLE(hWnd, style) \
			SetClassLong(hWnd, GCL_STYLE, style)
#else
#define GET_CLASS_STYLE(hWnd) \
			GetClassWord(hWnd, GCW_STYLE)
#define SET_CLASS_STYLE(hWnd, style) \
			SetClassWord(hWnd, GCW_STYLE, style)
#endif

#define RectWidth( lpRect )  ( (lpRect)->right - (lpRect)->left + 1 )
#define RectHeight( lpRect ) ( (lpRect)->bottom - (lpRect)->top + 1 )

