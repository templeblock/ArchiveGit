#ifndef TYPES
    #include "types.h"
#endif

#define DEV_START	1
#define DEV_MSG		2
#define DEV_READ	3
#define DEV_READ0	5
#define DEV_READ1	6
#define DEV_READ2	7
#define DEV_READ3	8
#define DEV_WRITE	9

typedef struct _DevInit {
	BYTE device_caps;	/* gets device's intents */
	WORD npix;
	WORD nlin;
	WORD bpl;
	WORD xres;
	WORD yres;
	WORD bpp;
	HWND hPw;	/* handle to the picture window(if needed) */
	HANDLE	hInst;	/* Instance of the app(if needed) */
	WORD port;
	WORD vgaaddr;
	char cfname[64];	/* name of the current file */
	char cproghome[64];	/* name of pp home directory */
	char cramdisk[64];	/* name of pp ramdisk directory */
	BOOL bFile_is_open;	/* is file open when we enter dll? */
	BOOL bImg_type;		/* img_type:True=Contone, False=lineart */
	} DEVINFO;
typedef DEVINFO far *LPDEVINFO;

typedef struct _DevMessage {
	int MsgNo;
	char szMsg[80];	/* if MsgNo < 0 */
} DEVMSG;
typedef DEVMSG far *LPDEVMSG;

typedef struct _DevImage {
	LPTR ImageAddress;	/* storage location for grabbed image data */
} DEVGET;
typedef DEVGET far *LPDEVGET;

/* the grabber DLL calls in PPGRAB.LIB */
extern  int DevName(LPTR);
extern  int DevOpen(LPDEVINFO);
extern  void DevClose(void);
extern  int DevCall(BYTE,LPTR);
