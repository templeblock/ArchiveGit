/******************************************************************************
 *
 * MOPEN.C - Minerva MDI File/Open menu command handler
 *
 * Copyright (C) 1996, 1997 Pegasus Imaging Corporation
 *
 * This source code is provided to you as sample source code.  Pegasus Imaging
 * Corporation grants you the right to use this source code in any way you
 * wish. Pegasus Imaging Corporation assumes no responsibility and explicitly
 * disavows any responsibility for any and every use you make of this source
 * code.
 *
 * Global functions:
 *     OpenOnFileOpen      - frame window File/Open menu command handler
 *     OpenOnFileMruOpen   - frame window File/Mru Open menu command handler
 *     OpenImageWindow     - creates a window and opens the image into the new window
 *     OpenImage           - opens the image into a pre-existing window
 *     OpenCleanup         - free window instance data and all subordinate data
 *     OpenFileExtractDib  - extract full-size, full-image dib from image file
 *     OpenMemoryExtractDib - extract full-size, full-image dib from image file in memory
 *
 * Local functions:
 *     OpenAndMruShared    - create child window for image in pszFilename and open the image
 *     LoadImageFile       - allocate a buffer and load the image file
 *     QueryImageFile      - determine the image type and locate the open opcode to use
 *     CreateChildWindow   - create the MDI child window for the pszFilename image file
 *     OpenImageIntoWindow - set up window instance data and open image (construct dib)
 *
 * Revision History:
 *   04-21-97  1.00.21  jrb  added multiple select to file open box
 *                           set child window size before setting scroll range so the
 *                           scroll bar doesn't flash for the new window
 *   11-11-96  1.00.00  jrb  created
 *
 ******************************************************************************/



#define STRICT
/* system includes */
#pragma warning(disable:4115 4201 4214)
#include <windows.h>
#include <windowsx.h>
#pragma warning(default:4115 4201 4214)
#include <commdlg.h>
#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <limits.h>

#pragma warning(disable:4001)
/* pic includes */
#include "pic.h"
#include "errors.h"

/* minerva includes */
#include "resource.h"
#include "mopcodes.h"
#include "mctrl.h"
#include "minerva.h"
#include "mchild.h"
#include "mmenu.h"
#include "merror.h"
#include "mopen.h"
#include "mmru.h"
#include "mmisc.h"



/******************************************************************************
 *  local functions declared
 ******************************************************************************/



static BOOL OpenAndMruShared(LPCSTR pszFilename, DWORD dwParam);
static BOOL LoadImageFile(LPCSTR pszFilename, BYTE PICHUGE* *ppImage, DWORD *pdwSize);
static BOOL QueryImageFile(
    LPCSTR        pszFilename,
    BYTE PICHUGE* pImage,
    DWORD         dwSize,
    OPENOP      **ppOpen,
    PIC_PARM     *pPicParm);
static HWND CreateChildWindow(LPCSTR pszFilename);
static BOOL OpenImageIntoWindow(
    LPCSTR        pszFilename,
    HWND          hwnd,
    BYTE PICHUGE* pImage,
    DWORD         dwSize,
    OPENOP       *pOpen,
    PIC_PARM     *pPicParm,
    DWORD         dwOpFlags);



/******************************************************************************
 *  global functions
 ******************************************************************************/



/******************************************************************************
 *  BOOL OpenFileQuery(
 *      LPCSTR pszFilename,
 *      BYTE PICHUGE* PICFAR* ppDib,
 *      DWORD PICFAR* pdwSizeDib,
 *      OPENOP **ppOpenOp,
 *      PICPARM PICFAR* pDibPicParm)
 * 
 *  query an image in a file
 *
 *  parameters:
 *      pszFilename  - name of image file (for error messages)
 *      ppDib        - receive pointer to Dib buffer
 *      pdwSizeDib   - receive size of Dib buffer
 *      ppOpenOp     - receive pointer to open opcdoe def data for image
 *      pDibPicParm  - receives picparm data for Dib
 *
 *  returns:
 *      FALSE if some error (already reported), else TRUE
 *
 *  notes:
 *      this function allows us to determine we can probably process the
 *      image without expanding the whole thing.   It's used in mtutil82.c
 *      when filling the image file list box.
 ******************************************************************************/
BOOL OpenFileQuery(
    LPCSTR                pszFilename,
    BYTE PICHUGE*        *ppDib,
    DWORD                *pdwDibSize,
    OPENOP              **ppOpenOp,
    PIC_PARM             *pDibPicParm)
{
    if ( !LoadImageFile(pszFilename, ppDib, pdwDibSize) )
        return ( FALSE );
    if ( !QueryImageFile(pszFilename, *ppDib, *pdwDibSize, ppOpenOp, pDibPicParm) )
        {
        MiscGlobalFreePtr(ppDib);
        return ( FALSE );
        }
    return ( TRUE );
}



/******************************************************************************
 *  BOOL OpenFileExtractDib
 *      LPCSTR          pszFilename,
 *      BYTE PICHUGE* PICFAR* ppDib,
 *      DWORD PICFAR*   pdwSizeDib,
 *      PICPARM PICFAR* pDibPicParm)
 * 
 *  extract a dib from a file by determining the image type, locating the
 *  appropriate open opcode to use, and calling the opcode extract entry
 *  to get a full-size, full-detail dib
 *
 *  parameters:
 *      pszFilename - name of image file (for error messages)
 *      ppDib       - receive pointer to Dib buffer
 *      pdwSizeDib  - receive size of Dib buffer
 *      pDibPicParm - receives picparm data for Dib
 *
 *  returns:
 *      FALSE if some error (already reported), else TRUE
 ******************************************************************************/
BOOL OpenFileExtractDib(
    LPCSTR                pszFilename,
    BYTE PICHUGE* PICFAR* ppDib,
    DWORD PICFAR*         pdwDibSize,
    PIC_PARM PICFAR*      pDibPicParm)
{
    BYTE PICHUGE* pImage;
    DWORD dwSize;
    BOOL bRet;

    if ( !LoadImageFile(pszFilename, &pImage, &dwSize) )
        return ( FALSE );
    bRet = OpenMemoryExtractDib(pszFilename, pImage, dwSize, ppDib, pdwDibSize, pDibPicParm);
    MiscGlobalFreePtr(&pImage);
    return ( bRet );    
}



/******************************************************************************
 *  BOOL OpenMemoryExtractDib
 *      LPCSTR          pszFilename,
 *      BYTE PICHUGE*   pImage,
 *      DWORD           dwSize,
 *      BYTE PICHUGE* PICFAR* ppDib,
 *      DWORD PICFAR*   pdwSizeDib,
 *      PICPARM PICFAR* pDibPicParm)
 * 
 *  extract a dib from a memory image by determining the image type, locating the
 *  appropriate open opcode to use, and calling the opcode extract entry
 *  to get a full-size, full-detail dib
 *
 *  parameters:
 *      pszFilename - name of image file (for error messages)
 *      pImage      - pointer to memory image
 *      lSize       - size of memory image
 *      ppDib       - receive pointer to Dib buffer
 *      pdwSizeDib  - receive size of Dib buffer
 *      pDibPicParm - receives picparm data for Dib
 *
 *  returns:
 *      FALSE if some error (already reported), else TRUE
 ******************************************************************************/
BOOL OpenMemoryExtractDib(
    LPCSTR                pszFilename,
    BYTE PICHUGE*         pImage,
    DWORD                 dwSize,
    BYTE PICHUGE* PICFAR* ppDib,
    DWORD PICFAR*         pdwDibSize,
    PIC_PARM PICFAR*      pDibPicParm)
{
    OPENOP *pOpenOp;
    PIC_PARM PicParm;

    if ( !QueryImageFile(pszFilename, pImage, dwSize, &pOpenOp, &PicParm) )
        return ( FALSE );
    return ( (*pOpenOp->pfnExtractDib)(pImage, dwSize, &PicParm, ppDib, pdwDibSize, pDibPicParm) );
}



/******************************************************************************
 *  void OpenOnFileOpen(void)
 * 
 *  frame window File/Open menu command handler
 *
 *  notes:
 *      uses common file open dialog
 *
 *      constructs file filter list on the fly from open opcodes whose opcode
 *      dll is present.  Currently the image open code uses the first
 *      acceptable open opcode whose dll is present.  If none are present, it
 *      takes the first which wuld have been used if all dll's were present and
 *      displays an error message about that one.
 *
 *      all the code in this function basically just does the file/open dialog.
 *      Local function OpenAndMruShared() actually loads the file, determines
 *      the open opcode, creates the window and creates the dib
 ******************************************************************************/
void OpenOnFileOpen(void)
{
    MiscImageFileOpenDialog(hwndFrame, OpenAndMruShared, 0);
}



/******************************************************************************
 *  void OpenOnFileMruOpen(int nMruNum)
 * 
 *  frame window File/Mru Open menu command handler
 ******************************************************************************/
void OpenOnFileMruOpen(int nMruNum)
{
    char sz[_MAX_PATH + 1];

    MruGetFilename(APPLICATION_INIFILENAME, nMruNum, sz, sizeof(sz));
    OpenAndMruShared(sz, 0);
}



/******************************************************************************
 *  void OpenImageWindow(LPCSTR pszFilename, BYTE PICHUGE* pImage, DWORD dwSize, DWORD dwOpFlags)
 * 
 *  creates a window and opens the image into the new window
 *
 *  parameters:
 *      pszFilename - file containing image (note file is already loaded into pImage)
 *      pImage      - memory image of image file
 *      nSize       - length of image file
 *      dwOpFlags   - bit mask of open opcode parameters
 ******************************************************************************/
static void DoOpenImage(HWND hwnd, LPCSTR pszFilename, BYTE PICHUGE* pImage, DWORD dwSize, DWORD dwOpFlags)
{
    if ( !OpenImage(hwnd, pszFilename, pImage, dwSize, dwOpFlags) )
        {
        MiscGlobalFreePtr(&pImage);
        MruDeleteFilename(hwndFrame, APPLICATION_INIFILENAME, pszFilename);
        FORWARD_WM_MDIDESTROY(hwndMDIClient, hwnd, SendMessage);
        return;
        }
    MruSetFirstFilename(hwndFrame, APPLICATION_INIFILENAME, pszFilename);
}

#ifdef _MT
struct _DoOpenImageParams
{
    HWND hwnd;
    LPSTR pszFilename;
    BYTE PICHUGE* pImage;
    DWORD dwSize;
    DWORD dwOpFlags;
    DWORD dwInputThreadId;
};

static DWORD WINAPI CallDoOpenImage(LPVOID pv)
{
    struct _DoOpenImageParams *p = (struct _DoOpenImageParams *)pv;

    PegasusLibThreadInit();
    AttachThreadInput(GetCurrentThreadId(), p->dwInputThreadId, TRUE);
    DoOpenImage(p->hwnd, p->pszFilename, p->pImage, p->dwSize, p->dwOpFlags);
    free(p);
    PegasusLibThreadTerm();
    return ( 0 );
}
#endif

void OpenImageWindow(LPCSTR pszFilename, BYTE PICHUGE* pImage, DWORD dwSize, DWORD dwOpFlags)
{
    char sz[_MAX_PATH + 1];
    HWND hwnd;
    
    lstrcpy(sz, pszFilename);
    hwnd = CreateChildWindow(sz);
    if ( hwnd == NULL )
        {
        MiscGlobalFreePtr(&pImage);
        MruDeleteFilename(hwndFrame, APPLICATION_INIFILENAME, sz);
        return;
        }
#ifdef _MT
    {
        HANDLE h;
        DWORD ThreadId;
        struct _DoOpenImageParams *p = malloc(sizeof(struct _DoOpenImageParams) + strlen(sz) + 1);

        if ( p != 0 )
        {
            p->hwnd        = hwnd;
            p->pszFilename = (LPSTR)( p + 1 );
            p->pImage      = pImage;
            p->dwSize      = dwSize;
            p->dwOpFlags   = dwOpFlags;
            p->dwInputThreadId = GetCurrentThreadId();
            strcpy(p->pszFilename, sz);
            h = CreateThread(NULL, 0, CallDoOpenImage, p, 0, &ThreadId);
            if ( h != NULL )
            {
                CloseHandle(h);
                return;
            }
            free(p);
        }
        ErrorMessage(STYLE_ERROR, IDS_THREADERROR, sz);
    }
#endif
    DoOpenImage(hwnd, sz, pImage, dwSize, dwOpFlags);
}



/******************************************************************************
 *  void OpenImage(HWND hwnd, LPCSTR pszFilename, BYTE PICHUGE* pImage, DWORD dwSize, DWORD dwOpFlags)
 * 
 *  opens the image into a pre-existing window
 *
 *  parameters:
 *      hwnd        - child window into which to open the image
 *      pszFilename - file containing image (note file is already loaded into pImage)
 *      pImage      - memory image of image file
 *      nSize       - length of image file
 *      dwOpFlags   - bit mask of open opcode parameters
 *
 *  notes:
 *      Determines the open opcode on the fly (in QueryImageFile) according to
 *      the biCompression returned by PegasusQuery
 *
 *      The window has to already be there, but the instance data pointer may
 *      be NULL (i.e. it could be the first image opened into the child window).
 ******************************************************************************/
BOOL OpenImage(HWND hwnd, LPCSTR pszFilename, BYTE PICHUGE* pImage, DWORD dwSize, DWORD dwOpFlags)
{
    char sz[_MAX_PATH + 1];
    CHILDINSTANCEDATA PICFAR* pInstance = ChildGetInstanceData(hwnd);
    OPENOP *pOpen;
    PIC_PARM PicParm;
    
    lstrcpy(sz, pszFilename);
    if ( !QueryImageFile(sz, pImage, dwSize, &pOpen, &PicParm) )
        return ( FALSE );

    if ( !OpenImageIntoWindow(sz, hwnd, pImage, dwSize, pOpen, &PicParm, dwOpFlags) )
        return ( FALSE );
        
    if ( pInstance != 0 )
        {
        /* if the old instance source image is also the new instance
            source image, don't let OpenCleanup free it.  */
        if ( pInstance->pSrc == pImage )
            pInstance->pSrc = 0;
        OpenCleanup(pInstance); /* OpenImageIntoWindow will have set a new instance pointer */
        }
    return ( TRUE );
}



/******************************************************************************
 *  void OpenCleanup(CHILDINSTANCEDATA PICFAR* pInstance)
 * 
 *  free window instance data and all subordinate data
 *
 *  parameters:
 *      pInstance - instance data to clean up
 *
 *  notes:
 *      Determines the open opcode on the fly (in QueryImageFile) according to
 *      the biCompression returned by PegasusQuery
 *
 *      The window has to already be there, but the instance data pointer may
 *      be NULL as it is for the first image opened into the child window.
 ******************************************************************************/
void OpenCleanup(CHILDINSTANCEDATA PICFAR* pInstance)
{    
    if ( pInstance == 0 )
        return;
    if ( pInstance->pOpenOp != 0 )
        (*pInstance->pOpenOp->pfnCleanup)(
            pInstance->pDib,
            &pInstance->DibPicParm,
            pInstance->pOpenData);
    if ( pInstance->hPalette != NULL )
        DeleteObject(pInstance->hPalette);
    MiscGlobalFreePtr(&pInstance->pSrc);
    MiscGlobalFreePtr(&pInstance);
}    



/******************************************************************************
 *  local functions
 ******************************************************************************/



/******************************************************************************
 *  void OpenAndMruShared(LPCSTR pszFilename, DWORD dwParam)
 * 
 *  create child window for image in pszFilename and open the image
 *
 *  parameters:
 *      pszFilename - name of image file
 *      dwParam - not used, but allows this function to be passed to
 *                MiscImageFileOpenDialog
 ******************************************************************************/
static BOOL OpenAndMruShared(LPCSTR pszFilename, DWORD dwParam)
{
    BYTE PICHUGE* pImage;
    DWORD dwSize;

    NOREFERENCE(dwParam);
    if ( !LoadImageFile(pszFilename, &pImage, &dwSize) )
        {
        MruDeleteFilename(hwndFrame, APPLICATION_INIFILENAME, pszFilename);
        return ( FALSE );
        }

    OpenImageWindow(pszFilename, pImage, dwSize, OP_INISETTINGS);
    return ( TRUE );
}



/******************************************************************************
 *  BOOL LoadImageFile(LPCSTR pszFilename, BYTE PICHUGE* ppImage, DWORD *pdwSize)
 * 
 *  allocate a buffer and load the image file
 *
 *  parameters:
 *      pszFilename - name of image file
 *      ppImage     - receive pointer to buffer containing image file
 *      plSize      - receive length of image file
 *
 *  returns:
 *      FALSE if a buffer can't be allocated or a file error occurs, else TRUE
 *
 *  notes:
 *      it's up to LoadImageFile to display an appropriate error message
 *      if it fails
 ******************************************************************************/
static BOOL LoadImageFile(LPCSTR pszFilename, BYTE PICHUGE* *ppImage, DWORD *pdwSize)
{
    HFILE hFile;
    OFSTRUCT of;
    DWORD lPos;

    memset(&of, 0, sizeof(of));
    of.cBytes = sizeof(of);
    hFile = OpenFile(pszFilename, &of, OF_READ | OF_SHARE_DENY_WRITE);
    if ( hFile == HFILE_ERROR )
        {
        _lclose(hFile);
        ErrorFileMessage(IDS_IMAGEOPENFILE, of.nErrCode, (LPCSTR)pszFilename);
        /* "The file %s could not be opened." */
        return ( FALSE );
        }

    /* determine the size of the file */
    *pdwSize = _llseek(hFile, 0, SEEK_END);
    assert(*pdwSize != (DWORD)HFILE_ERROR);
    if ( *pdwSize == 0 )
        {
        _lclose(hFile);
        ErrorMessage(STYLE_ERROR, IDS_IMAGEEMPTY, (LPCSTR)pszFilename);
        /* "The file %s is empty." */
        return ( FALSE );
        }
    lPos = _llseek(hFile, 0, SEEK_SET); /* can't leave file position set to eof */
    assert(lPos != (DWORD)HFILE_ERROR);

    /* allocate a buffer for the memory image */
    *ppImage = GlobalAllocPtr(GMEM_MOVEABLE, *pdwSize);
    if ( *ppImage == 0 )
        {
        _lclose(hFile);
        ErrorMessage(STYLE_ERROR, IDS_IMAGEMEMORY, (LPCSTR)pszFilename);
        /* "There was not enough memory to load the file %s." */
        return ( FALSE );
        }

    /* read it into the buffer */
    lPos = _hread(hFile, *ppImage, *pdwSize);
    if ( lPos != *pdwSize )
        {
        _lclose(hFile);
        MiscGlobalFreePtr(ppImage);
        ErrorMessage(STYLE_ERROR, IDS_IMAGEREAD, (LPCSTR)pszFilename);
        /* "An unexpected error occurred reading the file %s.  "
            "Another program may be accessing the file." */
        return ( FALSE );
        }

    hFile = _lclose(hFile);
    assert(hFile != HFILE_ERROR);
    return ( TRUE );
}



/******************************************************************************
 *  BOOL QueryImageFile(
 *      LPCSTR        pszFilename,
 *      BYTE PICHUGE* pImage,
 *      DWORD         dwSize,
 *      OPENOP      **ppOpen,
 *      PIC_PARM     *pPicParm)
 * 
 *  determine the image type and locate the open opcode to use
 *
 *  parameters:
 *      pszFilename - name of image file (for error messages)
 *      pImage      - pointer to image buffer
 *      lSize       - size of image buffer
 *      ppOpen      - receives pointer to opcode definition data
 *      pPicParm    - receives PegasusQuery picparm data
 *
 *  returns:
 *      FALSE if PegasusQuery returns an error, the biCompression isn't valid
 *      for any of our open opcodes or the appropriate opcode dll isn't present
 *
 *  notes:
 *      if the selected image can only be opened by a dll which isn't present,
 *      the displayed error message includes the dll name whose construction
 *      is hardcoded (PICN%.2lu%.2u) in this code.  Derivation of the dll number from
 *      the opcode is also hard-coded here (and in the about box)
 ******************************************************************************/
static BOOL QueryImageFile(
    LPCSTR        pszFilename,
    BYTE PICHUGE* pImage,
    DWORD         dwSize,
    OPENOP      **ppOpen,
    PIC_PARM     *pPicParm)
{    
    int i;
    OPENOP *pFoundOp;

    /* call PegasusQuery to determine the image type, the bit depth (# colors), the
        width and height in pixels and, if present, the color palette */
    memset(pPicParm, 0, sizeof(*pPicParm));
    pPicParm->ParmSize     = sizeof(*pPicParm);
    pPicParm->ParmVer      = CURRENT_PARMVER;
    pPicParm->ParmVerMinor = 0;
    pPicParm->Get.Start    = pImage;                /* start of input queue buffer */
    pPicParm->Get.End      = pImage + dwSize;       /* end of input queue buffer */
    pPicParm->Get.Front    = pPicParm->Get.Start;   /* start of input queue data */
    pPicParm->Get.Rear     = pPicParm->Get.End;     /* end of input queue data */
    pPicParm->u.QRY.BitFlagsReq =           /* requested query data from image */
        QBIT_BICOMPRESSION | QBIT_BIBITCOUNT |
        QBIT_BIHEIGHT | QBIT_BIWIDTH |
        QBIT_BICLRUSED | QBIT_BICLRIMPORTANT | QBIT_PALETTE;
    /* PegasusQuery returns FALSE if an error occurred or if it couldn't retrieve all
        the requested information.  In the latter case, Status == 0 (no error)
        and we just make sure we have the barest minimum information we require,
        also make sure that quantities we have to deal with as signed integers
        aren't (quite unexpectedly) larger than INT_MAX.  I think the expand
        and pack dll's would choke anyway for Width > INT_MAX */
    if ( ( !PegasusQuery(pPicParm) &&
           ( pPicParm->Status != 0 ||
             ( pPicParm->u.QRY.BitFlagsAck &
               ( QBIT_BICOMPRESSION | QBIT_BIBITCOUNT | QBIT_BIHEIGHT | QBIT_BIWIDTH ) ) !=
               ( QBIT_BICOMPRESSION | QBIT_BIBITCOUNT | QBIT_BIHEIGHT | QBIT_BIWIDTH )
           )
         ) ||
         pPicParm->Head.biHeight       > INT_MAX ||
         pPicParm->Head.biWidth        > INT_MAX ||
         pPicParm->Head.biClrUsed      > INT_MAX ||
         pPicParm->Head.biClrImportant > INT_MAX
       )
        {
        assert(pPicParm->Status == 0 || pPicParm->Status == ERR_BAD_IMAGE_TYPE);
        ErrorMessage(STYLE_ERROR, IDS_IMAGEQUERY, (LPCSTR)pszFilename);
        /* "The file %s is not an image file or it is not a supported type of image file." */
        return ( FALSE );
        }

    /* find the right Op for the image file, the right one is the first one
        with a present opcode which can handle the file biCompression, else
        it's the first not-present opcode which could have handled the file
        type if it were present */
    pFoundOp = 0;
    for ( i = 0; pOpenOpTable[i] != 0; i++ )
        {
        if ( MiscIsImageType(pPicParm->Head.biCompression, pOpenOpTable[i]) )
            {
            if ( pFoundOp == 0 )
                pFoundOp = pOpenOpTable[i];
            if ( pOpenOpTable[i]->nFoundParmVer != 0 )
                break;
            }
        }

    if ( pOpenOpTable[i] == 0 )
        {
        if ( pFoundOp == 0 )
            {   /* unknown (to this version of Minerva) image type */
            ErrorMessage(STYLE_ERROR, IDS_IMAGEUNKNOWN, (LPCSTR)pszFilename);
            /* "The file %s is an image type which is unknown to this version of Minerva." */
            return ( FALSE );
            }
        if ( pFoundOp->pszAbout == NULL )
            {
            /* look for an open opcode with the same opcode as pFoundOp, but with a
                pszAbout box string.  This provides a descriptive legend of the opcode
                whose dll isn't present. */
            for ( i = 0; pOpenOpTable[i] != 0; i++ )
                if ( pOpenOpTable[i]->lOpcode == pFoundOp->lOpcode &&
                     pOpenOpTable[i]->pszAbout != NULL )
                    break;
            pFoundOp = pOpenOpTable[i];
            assert(pFoundOp != 0);
            }
        /* note the string resource includes printf formatting */
        ErrorMessage(
            STYLE_ERROR,
            IDS_IMAGEOPCODENOTPRESENT,
            (LPCSTR)pszFilename,
            (LPCSTR)pFoundOp->pszAbout,
            pFoundOp->lOpcode / 1000,
            CURRENT_PARMVER);
        /* "The file %s requires the %s DLL (PICN%.2lu%.2u ) which was not found."
            "The image cannot be loaded." */
        return ( FALSE );
        }

    *ppOpen = pOpenOpTable[i];
    return ( TRUE );
}



/******************************************************************************
 *  HWND CreateChildWindow(LPCSTR pszFilename)
 * 
 *  create the MDI child window for the pszFilename image file
 *
 *  parameters:
 *      pszFilename - name of image file (for title and error messages)
 *
 *  returns:
 *      HWND of child window else NULL
 ******************************************************************************/
static HWND CreateChildWindow(LPCSTR pszFilename)
{    
    MDICREATESTRUCT mcs;
    HWND hwnd;

    /* Create MDI child window */
    mcs.szClass    = MDICHILD_CLASSNAME;
    mcs.szTitle    = pszFilename;
    mcs.hOwner     = hinstThis;
    mcs.x          = CW_USEDEFAULT;
    mcs.y          = CW_USEDEFAULT;
    mcs.cx         = CW_USEDEFAULT;
    mcs.cy         = CW_USEDEFAULT;
    mcs.style      = WS_HSCROLL | WS_VSCROLL;
    mcs.lParam     = 0;
    if ( ChildGetActive() != NULL && IsZoomed(ChildGetActive()) )
        mcs.style |= WS_MAXIMIZE;

    // tell the MDI Client to create the child 
    LockWindowUpdate(hwndMDIClient);
    hwnd = FORWARD_WM_MDICREATE(hwndMDIClient, (LPMDICREATESTRUCT)&mcs, SendMessage);
    if ( hwnd == NULL )
        {
        LockWindowUpdate(NULL);
        ErrorMessage(STYLE_ERROR, IDS_IMAGECREATEWINDOW, (LPCSTR)pszFilename);
        /* "An unexpected error occurred creating a window to display the image." */
        return ( NULL );
        }
    if ( !IsZoomed(hwnd) )
        ShowWindow(hwnd, SW_HIDE);
    LockWindowUpdate(NULL);
    InvalidateRect(hwndMDIClient, NULL, FALSE);
    UpdateWindow(hwndMDIClient);

    //####UpdateWindow(hwnd);
    return ( hwnd );
}



/******************************************************************************
 *  BOOL OpenImageIntoWindow(
 *      LPCSTR        pszFilename,
 *      HWND          hwnd,
 *      BYTE PICHUGE* pImage,
 *      DWORD         dwSize,
 *      OPENOP       *pOpen,
 *      PIC_PARM     *pPicParm,
 *      DWORD         dwOpFlags)
 * 
 *  set up window instance data and open image (construct dib)
 *
 *  parameters:
 *      pszFilename - filename of image
 *      hwnd        - hwnd of child window
 *      pImage      - pointer to memory image of image
 *      lSize       - size of memory image
 *      pOpen       - open opcode definition data
 *      pPicParm    - PegasusQuery picparm for image
 *      dwOpFlags   - bit mask of open opcode parameters
 *
 *  returns:
 *      FALSE if instance data can't be allocated and if open fails
 ******************************************************************************/
static BOOL OpenImageIntoWindow(
    LPCSTR        pszFilename,
    HWND          hwnd,
    BYTE PICHUGE* pImage,
    DWORD         dwSize,
    OPENOP       *pOpen,
    PIC_PARM     *pPicParm,
    DWORD         dwOpFlags)
{
    CHILDINSTANCEDATA PICFAR* pInstance;
    DWORD dwOldInstance = GetWindowLong(hwnd, 0);

    /* set up window instance data */
    pInstance = (CHILDINSTANCEDATA PICFAR*)
        GlobalAllocPtr(GMEM_MOVEABLE, sizeof(CHILDINSTANCEDATA) + lstrlen(pszFilename) + 1);
            /* There was not enough memory to load the file %s. */
    if ( pInstance == 0 )
        {
        ErrorMessage(STYLE_ERROR, IDS_IMAGEMEMORY, (LPCSTR)pszFilename);
        /* "There was not enough memory to load the file %s." */
        return ( FALSE );
        }
    _fmemset(pInstance, 0, sizeof(CHILDINSTANCEDATA));
    pInstance->pszFilename     = (LPSTR)pInstance + sizeof(CHILDINSTANCEDATA);
    lstrcpy(pInstance->pszFilename, pszFilename);
    pInstance->pSrc            = pImage;
    pInstance->dwSrcLen        = dwSize;
    pInstance->SrcPicParm      = *pPicParm;
    pInstance->pOpenOp         = pOpen;
    // sorry about overloading this parameter like this.  It's just that there are
    // so _many_ args already to pfnOpen.
    pInstance->pOpenData       = (LPVOID)dwOpFlags;
    
    SetScrollPos(hwnd,   SB_HORZ, 0, FALSE);
    SetScrollPos(hwnd,   SB_VERT, 0, FALSE);
    SetScrollRange(hwnd, SB_HORZ, 0, 0, FALSE);
    SetScrollRange(hwnd, SB_VERT, 0, 0, FALSE);
    SetWindowLong(hwnd, 0, (LONG)pInstance);

    /* open the image */
    ChildWaitCursorOn(hwnd);
    if ( !(*pInstance->pOpenOp->pfnOpen)(
            hwnd,
            pImage,
            dwSize,
            &pInstance->SrcPicParm,
            &pInstance->pDib,
            &pInstance->dwDibLen,
            &pInstance->DibPicParm,
            &pInstance->pOpenData) )
        {
        ChildWaitCursorOff(hwnd);
        MiscGlobalFreePtr(&pInstance);
        SetWindowLong(hwnd, 0, dwOldInstance);        
        return ( FALSE );
        }
    ChildWaitCursorOff(hwnd);

    assert(labs(pInstance->DibPicParm.Head.biHeight) <= INT_MAX &&
           pInstance->DibPicParm.Head.biWidth        <= INT_MAX &&
           pInstance->DibPicParm.Head.biClrUsed      <= INT_MAX &&
           pInstance->DibPicParm.Head.biClrImportant <= INT_MAX);

    ChildSetDibHeight(hwnd, labs(pInstance->DibPicParm.Head.biHeight));

    /* make sure the window knows to use the new palette and discard the old palette (if any) */
    ChildInvalidatePalette(hwnd);

    /* make sure the window isn't bigger than the image */
    ChildSetWindowSize(hwnd);

    return ( TRUE );
}
