// Plugin prototypes
BOOL SetupPluginMenus( HMENU hImageMenu );
long HandlePluginCommand( LPIMAGE lpImage, WPARAM wParam, LPARAM lParam );
long HandlePluginMessage( LPIMAGE lpImage, WPARAM wParam, LPARAM lParam );
void FreeUpPlugin();

BOOL SetupAdobeFileImportMenu( HMENU hFileMenu, int position );
BOOL SetupAdobeFileExportMenu( HMENU hFileMenu, int position );
BOOL SetupAdobeImageMenu( HMENU hImageMenu );
long HandleAdobePluginCommand( LPIMAGE lpImage, int idCmd );
void FreeUpAdobePlugin();
BOOL PSFilterImage(LPIMAGE lpImage, LPPSFILTER_PARMS lpParms);

// Reserve 4000 - 4149 for plugin menu items
#define PS_FIRST_PLUGIN				    4000
#define PS_LAST_PLUGIN				    4149

// Reserve 4150 - 4300 for Adobe Plugin menu items
#define PS_FIRST_ADOBEPLUGIN            4150
#define PS_LAST_ADOBEPLUGIN             4300

#define PS_SERVICE_ABORT_BEGIN          4347 //0x10FB
#define PS_SERVICE_ABORT_CHECK          4348 //0x10FC
#define PS_SERVICE_ABORT_END            4349 //0x10FD

#define PS_SERVICE_ABORT_BEGIN_NOESC    4352 //0x1100
#define PS_SERVICE_BUF_DELETE           4353 //0x1101

#define PS_SERVICE_GET_IMAGE_INFO       4356 //0x1104

#define PS_SERVICE_PRIVATE_INI          4361 //0x1109
#define PS_SERVICE_BUF_ALLOC            4362 //0x110A
#define PS_SERVICE_BUF_LOCK1            4363 //0x110B
#define PS_SERVICE_BUF_UNLOCK1          4364 //0x110C
#define PS_SERVICE_BUF_NEXTLINE         4365 //0x110D
#define PS_SERVICE_BUF_LINEPTR          4366 //0x110E

#define PS_SERVICE_BUF_LINE_TABLE       4368 //0x1110

typedef enum    {
	PIT_FILTER,
	PIT_EXPORT,
    PIT_ACQUIRE
} PLUGIN_TYPE;

typedef struct  {
	BOOL     bDelAfterUsed;
	int      nBitsPerPixel;
	int      nPixelsPerLine;
	int      nLines;
	int      nReserved[6];
} PS_BUF;
typedef PS_BUF far *LPPS_BUF;


//----- BufType
#define BUF_FILE    1
#define BUF_MEM     2
#define BUF_EMS     3

typedef struct
	{
	int      nBitsPerPixel;
	int      nPixelsPerLine;
	int      nLines;
	int      nPlanes; /* Must 1 */
	//---------------------------
	WORD     xx02;
	WORD     BytesPerBank;
	WORD     SizePerBank;
	WORD     Banks;
	WORD     LinesPerBank;
	WORD     LinesInLastBank;
	//---------------------------
	int      nBytesPerLine;
	//---------------------------
	WORD     BufDirty;
	WORD     BufType;   // BUF_FILE(1), BUF_MEM(2), BUF_EMS(3), BUF_UNKNOWN(4)
	WORD     xx1;
	//---------------------------
	int      xx2;
	int      xx3;
	int      xx4;
	
	int      xx5;
	TCHAR     BufFileName[120];
	int      nReserved3[8];
	//---------------------------
	int      nxRes;
	int      nyRes;
	int      nImgClass;
	HPALETTE hDocPalette;
	//---------------------------
	TCHAR     cReserved2[31];
	} BUFFER;
typedef HANDLE      HBUFFER;
typedef BUFFER far *LPBUFFER;

typedef LPBYTE far *LPLPBYTE;

/* Plug-in Menu Position Operator */
#define PO_BEFORE       0x0100
#define PO_AFTER        0x0200
#define PO_SUB_MENU     0x0300

/* Plug-in Menu Position Indicator */
#define PI_SMOOTHING_FILTERS    0x0001
#define PI_SHARPENING_FILTERS   0x0002
#define PI_SPECIAL_FILTERS      0x0003
#define PI_2D_EFFECTS           0x0004
#define PI_3D_EFFECTS           0x0005

/* Image Type */
#define DOC_IMG_RGB24           1       /* True Color */
#define DOC_IMG_GRAY8           2       /* Grayscale */

typedef struct  {
	WORD   wType;
	TCHAR   szMenuString[32];
	WORD   wTypeInfo;           // if wType != 0, The ordinal of the effect
} PIMENUITEM, far *LP_PIMENUITEM;

#define  ID_PIMENU   0xD0010100

typedef struct  {
    DWORD        dwDataID;      //- ID_PIMENU
	int          nMenuItems;    //- no. of  menu items
	PIMENUITEM   piMenu[1];     //- data structure for menu items
} PIMENU, far *LP_PIMENU;

