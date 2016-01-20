
/*
    mopcodes.h

    Revision History:
     04-21-97  1.00.21  jrb  added extract procedure pointer to open def.  Extract
                             procedure is used to extract a full-detail, full-size,
                             24-bit DIB from a given image.  Extract procedures are
                             used by save opcodes and by make color palette.
*/



#define OP_INISETTINGS  ( 0x0L )
#define OP_SILENT       ( 0x1L )
#define OP_BESTSETTINGS ( 0x2L )



//####TODO: commenting
typedef struct
    {
    HWND          hWnd;
	BYTE PICHUGE *pGet0;
    BYTE PICHUGE *pGet;
    DWORD         dwGetLen;
	DWORD		  dwGetSize;
    BYTE PICHUGE *pPut0;        /* original pPut */
    BYTE PICHUGE *pPut;
    DWORD         dwPutLen;
    DWORD         dwPutSize;
    DWORD         dwTime[20];   /* can time up to 20 intervals */
    } APPDATA;



/* options dialogs use this data structure as their WM_COMMAND user data
    as it has all the information needed to validate the image settings and to
    redisplay the images if needed when the APPLY button is pressed */
typedef struct _OptionsUserData
    {
    HWND                   hWnd;            /* mdi child window handle containing image */
    LPCSTR                 pszFilename;     /* file name for save-as image */
    BYTE PICHUGE* PICFAR*  ppSrc;           /* source image data */
    DWORD PICFAR*          pdwSrcLen;       /* source image length */
    PIC_PARM PICFAR*       pSrcPicParm;     /* source image PegasusQuery PicParm */
    BYTE PICHUGE* PICFAR*  ppDib;           /* old dib, set to new dib if changed */
    DWORD PICFAR*          pdwDibLen;       /* old dib length, set to new dib length if changed */
    PIC_PARM PICFAR*       pDibPicParm;     /* old dib picparm, set to new if changed */
    BOOL                   bTested;         /* TRUE if some options have been applied */
    LPCSTR                 pszType;         /* name where applicable (BMP, PCX, TIF, TGA) */
    } OPTIONSUSERDATA;



typedef void (*PFNVOIDPROCVOID)(void);



/******************************************************************************
    Save opcode definitions
*/

/* define function to save an image */
typedef BOOL (*PFNSAVE)(
    HWND                   hWnd,
    LPCSTR                 pszFilename,
    const BYTE PICHUGE*    pSrcImage,
    DWORD                  dwSrcLen,
    const PIC_PARM PICFAR* pSrcPicParm,
    const BYTE PICHUGE*    pDib,
    DWORD                  dwDibLen,
    const PIC_PARM PICFAR* pDibPicParm,
    BYTE PICHUGE* PICFAR*  ppSavedImage,
    DWORD PICFAR*          pdwSavedLen);

/* save opcode data definition */
typedef struct _SaveOp
    {
    long            lOpcode;            /* opcode to use in picparm */
    char           *pszAbout;           /* string describing opcode dll,
                                            one and only one entry with this lOpcode will have
                                            a pszAbout defined. */
    char           *pszDefaultsMenu;    /* string for defaults menu if a defaults menu dialog
                                            is available */
    char           *pszFilter;          /* string for save dialog box if applicable */
    PFNVOIDPROCVOID pfnInit;            /* if != 0 && pszAbout is defined, and opcode dll
                                            can be loaded, then this is called (see msutil80.c) */
    PFNSAVE         pfnSave;            /* does the saving */
    PFNVOIDPROCVOID pfnDefaultsDialog;  /* defaults dialog or 0 */
    int             nFoundParmVer;      /* != 0 if opcode dll is present */
    } SAVEOP;



/******************************************************************************
    Open opcode definitions
*/

/* define helper function to extract a full-size, full-detail DIB image */
typedef BOOL (*PFNEXTRACTDIB)(
    const BYTE PICHUGE*    pSrcImage,
    DWORD                  dwSrcLen,
    const PIC_PARM PICFAR* pSrcPicParm,
    BYTE PICHUGE* PICFAR*  ppDib,
    DWORD PICFAR*          pdwDibLen,
    PIC_PARM PICFAR*       pDibPicParm);

/* define function to open an image with user options*/
typedef BOOL (*PFNOPEN)(
    HWND                   hWnd,
    const BYTE PICHUGE*    pSrcImage,
    DWORD                  dwSrcLen,
    const PIC_PARM PICFAR* pSrcPicParm,
    BYTE PICHUGE* PICFAR*  ppDib,
    DWORD PICFAR*          pdwDibLen,
    PIC_PARM PICFAR*       pDibPicParm,
    VOID PICFAR* PICFAR*   ppOpenData);

/* define function for image options dialog */
typedef BOOL (*PFNOPENOPTIONSDIALOG)(
    HWND                   hWnd,
    const BYTE PICHUGE*    pSrcImage,
    DWORD                  dwSrcLen,
    const PIC_PARM PICFAR* pSrcPicParm,
    BYTE PICHUGE* PICFAR*  ppDib,
    DWORD PICFAR*          pdwDibLen,
    PIC_PARM PICFAR*       pDibPicParm,
    VOID PICFAR* PICFAR*   ppOpenData);

/* define function for properties dialog */
typedef void (*PFNOPENPROPERTIESDIALOG)(
    HWND                   hWnd,
    LPCSTR                 pszFilename,
    const BYTE PICHUGE*    pSrcImage,
    DWORD                  dwSrcLen,
    const PIC_PARM PICFAR* pSrcPicParm,
    const BYTE PICHUGE*    pDib,
    DWORD                  dwDibLen,
    const PIC_PARM PICFAR* pDibPicParm,
    const VOID PICFAR*     pOpenData);

/* define function to release any image resources at end */
typedef void (*PFNOPENCLEANUP)(
    BYTE PICHUGE *pDib,
    PIC_PARM PICFAR *pDibPicParm,
    VOID PICFAR* pOpenData);

/* define function to duplicate picparm data */
typedef BOOL (*PFNOPENDUPPICPARM)(
    const PIC_PARM PICFAR* pSrcPicParm,
    PIC_PARM PICFAR*       pDupPicParm);

/* open opcode data definition */
typedef struct _OpenOp
    {
    long                   lOpcode;             /* opcode to use in picparm */
    char                   *pszAbout;           /* string describing opcode dll,
                                                    one and only one entry with this lOpcode
                                                    will have a pszAbout defined. */
    char                   *pszDefaultsMenu;    /* string for defaults menu if a defaults menu
                                                    dialog is available */
    char                   *pszFilter;          /* string for save dialog box if applicable */
    DWORD                  *pbiCompression;     /* list of biCompression values handled by
                                                    this opcode */
    PFNVOIDPROCVOID pfnInit;                    /* if != 0 && pszAbout is defined, and opcode dll
                                                    can be loaded, then this is called
                                                    (see moutil81.c) */
    PFNOPEN                 pfnOpen;            /* open an image into a window */
    PFNEXTRACTDIB           pfnExtractDib;      /* extract a full-size, full-detail, 24-bit DIB */
    PFNOPENOPTIONSDIALOG    pfnOptionsDialog;   /* set image options for an open image */
    PFNOPENPROPERTIESDIALOG pfnPropertiesDialog;/* show image properties for an open image */
    PFNVOIDPROCVOID         pfnDefaultsDialog;  /* set default image options */
    PFNOPENCLEANUP          pfnCleanup;         /* cleanup image resources */
    PFNOPENDUPPICPARM       pfnDupPicParm;      /* duplicate image pic_parm */
    SAVEOP                 *pSaveOp;            /* save opcode data definition to use
                                                    for File/Save */
    int                     nFoundParmVer;      /* != 0 if opcode dll is present */
    } OPENOP;



/******************************************************************************
    Transform opcode definitions
*/

/* define function to transform an image */
typedef BOOL (*PFNTRANSFORM)(
    HWND                   hWnd,
    LPCSTR                 pszFilename,
    BYTE PICHUGE* PICFAR*  ppSrcImage,
    DWORD PICFAR*          pdwSrcLen,
    PIC_PARM PICFAR*       pSrcPicParm,
    const BYTE PICHUGE*    pDib,
    DWORD                  dwDibLen,
    const PIC_PARM PICFAR* pDibPicParm);

typedef void (*PFNTRANSFORMDEFAULTS)(void);

typedef struct _TransformOp
    {
    long            lOpcode;            /* opcode to use in picparm */
    char           *pszAbout;           /* string describing opcode dll,
                                            one and only one entry with this lOpcode will have
                                            a pszAbout defined. */
    char           *pszDefaultsMenu;    /* string for defaults menu if a defaults menu dialog
                                            is available */
    char           *pszMenuItem;        /* string for tools menu menu item if any */
    PFNVOIDPROCVOID pfnInit;            /* if != 0 && pszAbout is defined, and opcode dll
                                            can be loaded, then this is called */
    PFNTRANSFORM    pfnTransform;       /* transform function */
    PFNVOIDPROCVOID pfnDefaultsDialog;  /* set default options for tool */
    int             nFoundParmVer;      /* != 0 if tool opcode dll is present */
    } TRANSFORMOP;



/******************************************************************************
    Tool opcode definitions
*/

typedef struct _ToolsOp
    {
    long            lOpcode;            /* opcode to use in picparm */
    char           *pszAbout;           /* string describing opcode dll,
                                            one and only one entry with this lOpcode will have
                                            a pszAbout defined. */
    char           *pszDefaultsMenu;    /* string for defaults menu if a defaults menu dialog
                                            is available */
    char           *pszMenuItem;        /* string for tools menu menu item if any */
    PFNVOIDPROCVOID pfnInit;            /* if != 0 && pszAbout is defined, and opcode dll
                                            can be loaded, then this is called */
    PFNVOIDPROCVOID pfnTool;            /* do the tool function */
    PFNVOIDPROCVOID pfnDefaultsDialog;  /* set default options for tool */
    int             nFoundParmVer;      /* != 0 if tool opcode dll is present */
    } TOOLSOP;
