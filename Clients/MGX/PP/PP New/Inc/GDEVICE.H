#include "types.h"

#define DEV_START	1
#define DEV_MSG		2
#define DEV_READ	3
#define DEV_READ0	5
#define DEV_READ1	6
#define DEV_READ2	7
#define DEV_READ3	8
#define DEV_WRITE	9

typedef LPTR (far *LPTRPROC)();

// turn of structure packing 
#pragma pack(1)

typedef struct _DevInit {
	BYTE device_caps;	/* gets device's intents */
	WORD npix;
	WORD nlin;
	WORD bpl;
	WORD xres;
	WORD yres;
	WORD bpp;
	HWND hPw;	/* handle to the picture window(if needed) */
	HINSTANCE	hInst;	/* Instance of the app(if needed) */
	WORD port;
	WORD vgaaddr;
	TCHAR cfname[64];	/* name of the current file */
	BOOL bFile_is_open;	/* is file open when we enter dll? */
	BOOL bImg_type;		/* img_type:True=Contone, False=lineart */
	LPTRPROC FpPtr;	// frame_ptr proc
	LPTRPROC FoPtr;	// frame_open proc
	LPTRPROC FsPtr;	// frame_set proc
	LPROC FcPtr;	// frame_close proc
	LPROC CcPtr;	// Clock cursor proc
	LPROC CmPtr;	// Color mapper proc(2.5 only)
	LPROC H2Ptr;	// Histo2Palette proc(2.5 only)
	LPTR  ColorLut;	// Pointer to the image.ColorLut(2.5 only)
	BOOL firsttime;	// true if this is the first invocation
	BOOL bLineArtAsGray; // Open Line Art as Grayscale
	} DEVINFO;
typedef DEVINFO far *LPDEVINFO;

typedef struct _DevMessage {
	int MsgNo;
	TCHAR szMsg[80];	/* if MsgNo < 0 */
} DEVMSG;
typedef DEVMSG far *LPDEVMSG;

typedef struct _DevImage {
	LPTR ImageAddress;	/* storage location for grabbed image data */
} DEVGET;
typedef DEVGET far *LPDEVGET;

#pragma pack()

typedef BOOL (FAR * DEVNAMEPROC)(LPTSTR);
typedef BOOL (FAR * DEVOPENPROC)(LPDEVINFO);
typedef void (FAR * DEVCLOSEPROC)(void);
typedef BOOL (FAR * DEVCALLPROC)(BYTE,LPTR);

/* the grabber DLL calls in PPGRAB.LIB */
extern BOOL DevName(LPTSTR);
extern BOOL DevOpen(LPDEVINFO);
extern void DevClose(void);
extern BOOL DevCall(BYTE,LPTR);
/* imports from the Grabber driver */
extern BOOL FAR PASCAL AcquireProc(HWND,unsigned,WORD,LONG);
extern VOID AcquireCommand(HWND,WORD);


