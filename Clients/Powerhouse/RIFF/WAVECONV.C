/** waveconv.c
 *
     (C) Copyright Microsoft Corp. 1991, 1992.  All rights reserved.

     You have a royalty-free right to use, modify, reproduce and 
     distribute the Sample Files (and/or any modified version) in 
     any way you find useful, provided that you agree that 
     Microsoft has no warranty obligations or liability for any 
     Sample Application Files which are modified. 
	 
     If you did not get this from Microsoft Sources, then it may not be the
     most current version.  This sample code in particular will be updated
     and include more documentation.  

     Sources are:
     	The MM Sys File Transfer BBS: The phone number is 206 936-4082.
	CompuServe: WINSDK forum, MDK section.
	Anonymous FTP from ftp.uu.net vendor\microsoft\multimedia
 *
 *
 **/


//
//  the includes we need
//
#define STRICT
#include <windows.h>
#include <commdlg.h>
#include <shellapi.h>
#include <windowsx.h>
#include <memory.h>
#include <mmsystem.h>
#include <mmreg.h>
#include "pcm.h"
#include "msadpcm.h"
#include "waveconv.h"
#include "riffsup.h"
#include "wavesup.h"
#include "waveio.h"


#if (_MSC_VER < 700)
//
//  this silly stuff is to get around a bug in the C6 compiler's optimizer
//
#ifdef GlobalFreePtr
#undef GlobalFreePtr
#define GlobalFreePtr(x)    GlobalFree((HGLOBAL)SELECTOROF(x))
#endif

#endif

typedef DWORD (FAR PASCAL *CONVERTPROC)(LPWAVEFORMATEX, HPSTR, LPWAVEFORMATEX, HPSTR, DWORD);

typedef struct tWAVECONVCB
{
    HMMIO           hmmioSrc;
    HMMIO           hmmioDst;

    MMCKINFO        ckDst;
    MMCKINFO        ckDstRIFF;

    DWORD           dwSrcSamples;
    DWORD           dwSrcReadSize;

    DWORD           dwDstBufSize;

    LPSTR           lpszFilePathSrc;
    LPWAVEFORMATEX  lpwfxSrc;
    HPSTR           hpSrc;
    DWORD           dwSrcBytes;
    LPSTR           lpszFilePathDst;
    LPWAVEFORMATEX  lpwfxDst;
    HPSTR           hpDst;
    CONVERTPROC     fpConvert;

} WAVECONVCB, *PWAVECONVCB, FAR *LPWAVECONVCB;


//
//  flags for AppGetFileName()...
//
#define AGFN_FLAG_OPEN      0x0000
#define AGFN_FLAG_SAVE      0x0001

//
//  prototypes for good measure
//
BOOL FAR PASCAL DlgProcConvert(HWND, UINT, WPARAM, LPARAM);
BOOL FAR PASCAL DlgProcAbout(HWND, UINT, WPARAM, LPARAM);
LRESULT FAR PASCAL WndProcApp(HWND, UINT, WPARAM, LPARAM);

//
//  globals, no less
//

WAVECONVCB      gwc;


char            gszAppName[]        = "Microsoft Wave Converter";
HINSTANCE       ghInstance;
HWND            ghwnd;

BOOL            gfCancelConvert;

char            gszNone[]           = "(none)";
char            gszUnknown[]        = "(unknown)";


LPWAVEIOCB      glpwio=NULL;
LPWAVEFORMATEX  glpwfx=NULL;
char            gszWaveFilePath[MAX_FILE_PATH_LEN];
char            gszWaveFormat[MAX_FORMAT_LEN];

char            gszSaveFilePath[MAX_FILE_PATH_LEN];

static		LPSTR lpplayerr="Error Playing File: %s";

//
//
//
//
#define WM_CONVERT_BEGIN    (WM_USER + 100)
#define WM_CONVERT_END      (WM_USER + 101)

/* void Cls_OnConvertBegin(HWND hwnd, WAVECONVCB FAR* lpwc) */
#define HANDLE_WM_CONVERT_BEGIN(hwnd, wParam, lParam, fn) \
    ((fn)((hwnd), (WAVECONVCB FAR*)(lParam)), 0L)
#define FORWARD_WM_CONVERT_BEGIN(hwnd, lpwc, fn) \
    (void)(fn)((hwnd), WM_CONVERT_BEGIN, 0, (LPARAM)(WAVECONVCB FAR*)(lpwc))

/* void Cls_OnConvertEnd(HWND hwnd, BOOL fCancel, WAVECONVCB FAR* lpwc) */
#define HANDLE_WM_CONVERT_END(hwnd, wParam, lParam, fn) \
    ((fn)((hwnd), (BOOL)(wParam), (WAVECONVCB FAR*)(lParam)), 0L)
#define FORWARD_WM_CONVERT_END(hwnd, fCancel, lpwc, fn) \
    (void)(fn)((hwnd), WM_CONVERT_END, (WPARAM)(fCancel), (LPARAM)(WAVECONVCB FAR*)(lpwc))

#define	BeginConvert(hwnd, lpwc)	FORWARD_WM_CONVERT_BEGIN((hwnd), (lpwc), PostMessage)
#define	EndConvert(hwnd, fCancel, lpwc)	FORWARD_WM_CONVERT_END((hwnd), (fCancel), (lpwc), PostMessage)


/** void FAR _cdecl AppErrorMsg(LPSTR sz, ...)
 *
 *  DESCRIPTION:
 *      
 *
 *  ARGUMENTS:
 *      (LPSTR sz, ...)
 *
 *  RETURN (void):
 *
 *
 *  NOTES:
 *
 ** */

void FAR _cdecl AppErrorMsg(HWND hwnd, LPCSTR lpszFormat, ...)
{
    char    ach[MAX_ERR_LEN];

    //
    //  format and display the string in a message box...
    //
    wvsprintf(ach, lpszFormat, (LPSTR)(&lpszFormat+1));
    MessageBox(hwnd, ach, gszAppName, MB_OK|MB_ICONEXCLAMATION|MB_TASKMODAL);
} /* AppErrorMsg() */


/** void NEAR PASCAL AppDlgYield(HWND hdlg)
 *
 *  DESCRIPTION:
 *      Yield function for modeless dialogs...
 *
 *  ARGUMENTS:
 *      (HWND hdlg)
 *
 *  RETURN (void NEAR PASCAL):
 *
 *
 *  NOTES:
 *
 ** */

void NEAR PASCAL AppDlgYield(HWND hdlg)
{
    MSG     msg;

    while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
    {
        if ((hdlg == NULL) || !IsDialogMessage(hdlg, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }
} /* AppDlgYield() */


/** void NEAR PASCAL AppHourGlass(BOOL fHourGlass)
 *
 *  DESCRIPTION: 
 *      This function changes the cursor to that of the hour glass or
 *      back to the previous cursor.
 *
 *  ARGUMENTS:
 *      BOOL fHourGlass :   TRUE if we need the hour glass.  FALSE if
 *                          we need the arrow back.
 *
 *  RETURN (void):
 *      On return, the cursor will be what was requested.
 *
 *  NOTES:
 *      This function can be called recursively.
 *
 ** */

void NEAR PASCAL AppHourGlass(BOOL fHourGlass)
{
    static HCURSOR  hCursor;
    static WORD     wWaiting = 0;

    if (fHourGlass)
    {
        if (!wWaiting)
        {
            hCursor = SetCursor(LoadCursor(NULL, IDC_WAIT));
            ShowCursor(TRUE);
        }

        wWaiting++;
    }
    else
    {
        if (!--wWaiting)
        {
            ShowCursor(FALSE);
            SetCursor(hCursor);
        }
    }
} /* AppHourGlass() */


/** BOOL NEAR PASCAL AppGetFileName(HWND hwnd, LPSTR lpszFilePath, LPSTR lpszFileTitle, WORD wFlags)
 *
 *  DESCRIPTION:
 *      
 *
 *  ARGUMENTS:
 *      (HWND hwnd, LPSTR lpszFilePath, LPSTR lpszFileTitle, WORD wFlags)
 *
 *  RETURN (BOOL NEAR PASCAL):
 *
 *
 *  NOTES:
 *
 ** */

BOOL NEAR PASCAL AppGetFileName(HWND hwnd, LPSTR lpszFilePath, LPSTR lpszFileTitle, WORD wFlags)
{
    static char szDefExtension[]    = "WAV";
    static char *szExtFilter[]      =
    {
        "Sound Files (*.wav)", "*.wav",
        "All Files (*.*)", "*.*",
        ""
    };

    OPENFILENAME    ofn;

    _fmemset(&ofn, 0, sizeof(OPENFILENAME));
    
    //
    //  initialize the OPENFILENAME members
    //
    lpszFilePath[0]         = '\0';
    if (lpszFileTitle)
        lpszFileTitle[0]    = '\0';

    ofn.lStructSize         = sizeof(OPENFILENAME);
    ofn.hwndOwner           = hwnd;
    ofn.lpstrFilter         = szExtFilter[0];
    ofn.lpstrCustomFilter   = (LPSTR)NULL;
    ofn.nMaxCustFilter      = 0L;
    ofn.nFilterIndex        = 1L;
    ofn.lpstrFile           = lpszFilePath;
    ofn.nMaxFile            = MAX_FILE_PATH_LEN;
    ofn.lpstrFileTitle      = lpszFileTitle;
    ofn.nMaxFileTitle       = lpszFileTitle ? MAX_FILE_TITLE_LEN : 0;
    ofn.lpstrInitialDir     = (LPSTR)NULL;
    ofn.nFileOffset         = 0;
    ofn.nFileExtension      = 0;
    ofn.lpstrDefExt         = szDefExtension;

    if (wFlags == AGFN_FLAG_OPEN)
    {
        ofn.Flags = OFN_HIDEREADONLY | OFN_FILEMUSTEXIST;
        return (GetOpenFileName(&ofn));
    }
    else
    {
        ofn.Flags = OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT;
        return (GetSaveFileName(&ofn));
    }
} /* AppGetFileName() */


/** BOOL FAR PASCAL AppDialogBox(HWND hwnd, LPCSTR lpszDlg, DLGPROC lpfn, LPARAM lParam)
 *
 *  DESCRIPTION:
 *      
 *
 *  ARGUMENTS:
 *      (HWND hwnd, LPCSTR lpszDlg, DLGPROC lpfn, LPARAM lParam)
 *
 *  RETURN (BOOL NEAR PASCAL):
 *
 *
 *  NOTES:
 *
 ** */

BOOL FAR PASCAL AppDialogBox(HWND hwnd, LPCSTR lpszDlg, DLGPROC lpfn, LPARAM lParam)
{
    BOOL    f;

    f = FALSE;
    if (lpfn = (DLGPROC)MakeProcInstance((FARPROC)lpfn, ghInstance))
    {
        f = DialogBoxParam(ghInstance, lpszDlg, hwnd, lpfn, lParam);
        FreeProcInstance((FARPROC)lpfn);
    }

    return (f);
} /* AppDialogBox() */



/** BOOL NEAR PASCAL WaveConvEnd(HWND hdlg, LPWAVECONVCB lpwc)
 *
 *  DESCRIPTION:
 *      
 *
 *  ARGUMENTS:
 *      (HWND hdlg, LPWAVECONVCB lpwc)
 *
 *  RETURN (BOOL NEAR PASCAL):
 *
 *
 *  NOTES:
 *
 ** */

BOOL NEAR PASCAL WaveConvEnd(HWND hdlg, LPWAVECONVCB lpwc)
{
    if (lpwc->hmmioSrc)
    {
        mmioClose(lpwc->hmmioSrc, 0);
        lpwc->hmmioSrc = NULL;
    }

    if (lpwc->hmmioDst)
    {
        mmioAscend(lpwc->hmmioDst, &lpwc->ckDst, 0);
        mmioAscend(lpwc->hmmioDst, &lpwc->ckDstRIFF, 0);

        mmioClose(lpwc->hmmioDst, 0);
        lpwc->hmmioDst = NULL;
    }

    if (lpwc->hpSrc)
    {
        GlobalFreePtr(lpwc->hpSrc);
        lpwc->hpSrc = NULL;
    }
    
    if (lpwc->hpDst)
    {
        GlobalFreePtr(lpwc->hpDst);
        lpwc->hpDst = NULL;
    }

    return (TRUE);
} /* WaveConvEnd() */


/** BOOL NEAR PASCAL WaveConvBegin(HWND hdlg, LPWAVECONVCB lpwc)
 *
 *  DESCRIPTION:
 *      
 *
 *  ARGUMENTS:
 *      (HWND hdlg, LPWAVECONVCB lpwc)
 *
 *  RETURN (BOOL NEAR PASCAL):
 *
 *
 *  NOTES:
 *
 ** */

BOOL NEAR PASCAL WaveConvBegin(HWND hdlg, LPWAVECONVCB lpwc)
{
    MMCKINFO    ckSrcRIFF;
    MMCKINFO    ck;
    DWORD       dw;

    //
    //  allocate the src and dst buffers for reading/converting data
    //
    lpwc->hpSrc = (HPSTR)GlobalAllocPtr(GHND, lpwc->dwSrcReadSize);
    if (!lpwc->hpSrc)
        goto wcb_Error;
    
    lpwc->hpDst = (HPSTR)GlobalAllocPtr(GHND, lpwc->dwDstBufSize);
    if (!lpwc->hpDst)
        goto wcb_Error;

    //
    //
    //
    lpwc->hmmioDst = mmioOpen(lpwc->lpszFilePathDst, NULL, MMIO_CREATE | MMIO_WRITE | MMIO_ALLOCBUF);
    if (!lpwc->hmmioDst)
        goto wcb_Error;

    //
    //  create the RIFF chunk of form type 'WAVE'
    //
    //
    lpwc->ckDstRIFF.fccType = mmioFOURCC('W', 'A', 'V', 'E');
    lpwc->ckDstRIFF.cksize  = 0L;
    if (mmioCreateChunk(lpwc->hmmioDst, &lpwc->ckDstRIFF, MMIO_CREATERIFF))
        goto wcb_Error;

    //
    //  first try to open the file, etc.. open the given file for reading
    //  using buffered I/O
    //
    lpwc->hmmioSrc = mmioOpen(lpwc->lpszFilePathSrc, NULL, MMIO_READ | MMIO_ALLOCBUF);
    if (!lpwc->hmmioSrc)
        goto wcb_Error;

    //
    //  locate a 'WAVE' form type in a 'RIFF' thing...
    //
    ckSrcRIFF.fccType = mmioFOURCC('W', 'A', 'V', 'E');
    if (mmioDescend(lpwc->hmmioSrc, (LPMMCKINFO)&ckSrcRIFF, NULL, MMIO_FINDRIFF))
        goto wcb_Error;

    //
    //  we found a WAVE chunk--now go through and get all subchunks that
    //  we know how to deal with...
    //
    while (mmioDescend(lpwc->hmmioSrc, &ck, &ckSrcRIFF, 0) == 0)
    {
        //
        //  quickly check for corrupt RIFF file--don't ascend past end!
        //
        if ((ck.dwDataOffset + ck.cksize) > (ckSrcRIFF.dwDataOffset + ckSrcRIFF.cksize))
            goto wcb_Error;

        switch (ck.ckid)
        {
    // explicitly skip these...
	    
            case mmioFOURCC('f', 'm', 't', ' '):
                break;

            case mmioFOURCC('d', 'a', 't', 'a'):
                break;

            case mmioFOURCC('f', 'a', 'c', 't'):
                break;

            case mmioFOURCC('J', 'U', 'N', 'K'):
                break;

	    case mmioFOURCC('P', 'A', 'D', ' '):
		break;

	    case mmioFOURCC('c', 'u', 'e', ' '):
		break;

    // copy chunks that are OK to copy

	    case mmioFOURCC('p', 'l', 's', 't'):
		// although without the 'cue' chunk, it doesn't make much sense
                riffCopyChunk(lpwc->hmmioSrc, lpwc->hmmioDst, &ck);
		break;

	    case mmioFOURCC('D', 'I', 'S', 'P'):
                riffCopyChunk(lpwc->hmmioSrc, lpwc->hmmioDst, &ck);
		break;

		
    // don't copy unknown chunks
		
            default:
                break;
        }

        //
        //  step up to prepare for next chunk..
        //
        mmioAscend(lpwc->hmmioSrc, &ck, 0);
    }
    //
    // now write out possibly editted chunks...
    //
    if(riffWriteINFO(lpwc->hmmioDst, (glpwio->pInfo)))
    {
	goto wcb_Error;
    }


    //
    // go back to beginning of data portion of WAVE chunk
    //
    if (mmioSeek(lpwc->hmmioSrc, ckSrcRIFF.dwDataOffset + sizeof(FOURCC),
	         SEEK_SET) == -1)
        goto wcb_Error;


    ck.ckid = mmioFOURCC('d', 'a', 't', 'a');
    mmioDescend(lpwc->hmmioSrc, &ck, &ckSrcRIFF, MMIO_FINDCHUNK);

    //
    //  now create the destination fmt, fact, and data chunks _in that order_
    //
    //
    //
    //  hmmio is now descended into the 'RIFF' chunk--create the format chunk
    //  and write the format header into it
    //
    if (lpwc->lpwfxDst->wFormatTag == WAVE_FORMAT_PCM)
        dw = sizeof(PCMWAVEFORMAT);
    else
        dw = sizeof(WAVEFORMATEX) + lpwc->lpwfxDst->cbSize;
    lpwc->ckDst.ckid   = mmioFOURCC('f', 'm', 't', ' ');
    lpwc->ckDst.cksize = dw;
    if (mmioCreateChunk(lpwc->hmmioDst, &lpwc->ckDst, 0))
        goto wcb_Error;

    if (mmioWrite(lpwc->hmmioDst, (HPSTR)lpwc->lpwfxDst, dw) != (LONG)dw)
        goto wcb_Error;

    if (mmioAscend(lpwc->hmmioDst, &lpwc->ckDst, 0) != 0)
        goto wcb_Error;

    //
    //  create the 'fact' chunk (not necessary for PCM--but is nice to have)
    //  since we are not writing any data to this file (yet), we set the
    //  samples contained in the file to 0..
    //
    lpwc->ckDst.ckid   = mmioFOURCC('f', 'a', 'c', 't');
    lpwc->ckDst.cksize = 0L;
    if (mmioCreateChunk(lpwc->hmmioDst, &lpwc->ckDst, 0))
        goto wcb_Error;

    if (mmioWrite(lpwc->hmmioDst, (HPSTR)&lpwc->dwSrcSamples, sizeof(DWORD)) != sizeof(DWORD))
        goto wcb_Error;

    if (mmioAscend(lpwc->hmmioDst, &lpwc->ckDst, 0) != 0)
        goto wcb_Error;

    //
    //  create the data chunk AND STAY DESCENDED... for reasons that will
    //  become apparent later..
    //
    lpwc->ckDst.ckid   = mmioFOURCC('d', 'a', 't', 'a');
    lpwc->ckDst.cksize = 0L;
    if (mmioCreateChunk(lpwc->hmmioDst, &lpwc->ckDst, 0))
        goto wcb_Error;

    //
    //  at this point, BOTH the src and dst files are sitting at the very
    //  beginning of their data chunks--so we can READ from the source,
    //  CONVERT the data, then WRITE it to the destination file...
    //

    return (TRUE);


    //
    //
    //
    //
wcb_Error:

    WaveConvEnd(hdlg, lpwc);
    return (FALSE);
} /* WaveConvBegin() */


/** BOOL NEAR PASCAL WaveConvConvert(HWND hdlg, LPWAVECONVCB lpwc)
 *
 *  DESCRIPTION:
 *      
 *
 *  ARGUMENTS:
 *      (HWND hdlg, LPWAVECONVCB lpwc)
 *
 *  RETURN (BOOL NEAR PASCAL):
 *
 *
 *  NOTES:
 *
 ** */

BOOL NEAR PASCAL WaveConvConvert(HWND hdlg, LPWAVECONVCB lpwc)
{
    char    ach[10];
    DWORD   dw;
    DWORD   dwCurrent;
    WORD    w;
    WORD    wCurPercent = (WORD)-1;

    for (dwCurrent = 0; dwCurrent < lpwc->dwSrcBytes; )
    {
        w = (WORD)((dwCurrent * 100) / lpwc->dwSrcBytes);
        if (w != wCurPercent)
        {
            wCurPercent = w;
            wsprintf(ach, "%u%%", wCurPercent);
            SetDlgItemText(hdlg, IDD_TXT_STATUS, ach);
        }

        AppDlgYield(hdlg);
        if (gfCancelConvert)
            goto wcc_Error;

        //
        //
        //
        w = (WORD)min(lpwc->dwSrcReadSize, lpwc->dwSrcBytes - dwCurrent);
        dw = mmioRead(lpwc->hmmioSrc, lpwc->hpSrc, w);
        if (dw == 0L)
            break;

        dwCurrent += dw;

        AppDlgYield(hdlg);
        if (gfCancelConvert)
            goto wcc_Error;

        //
        //  convert data
        //
        dw = (* lpwc->fpConvert)(lpwc->lpwfxSrc, lpwc->hpSrc, 
                                 lpwc->lpwfxDst, lpwc->hpDst, dw);

        AppDlgYield(hdlg);
        if (gfCancelConvert)
            goto wcc_Error;

        //
        //
        //
        if (mmioWrite(lpwc->hmmioDst, lpwc->hpDst, dw) != (LONG)dw)
            goto wcc_Error;
    }

    EndConvert(hdlg, !gfCancelConvert, lpwc);
    return (!gfCancelConvert);

wcc_Error:

    EndConvert(hdlg, FALSE, lpwc);
    return (FALSE);
} /* WaveConvConvert() */



static BOOL Cls_OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam)
{
    HFONT           hFont;
    LPWAVECONVCB    lpwc;

    lpwc = (LPWAVECONVCB)lParam;

    hFont = GetStockFont(ANSI_FIXED_FONT);

    SetWindowFont(GetDlgItem(hwnd, IDD_TXT_INFILEPATH), hFont, FALSE);
    SetWindowFont(GetDlgItem(hwnd, IDD_TXT_OUTFILEPATH), hFont, FALSE);
    SetWindowFont(GetDlgItem(hwnd, IDD_TXT_STATUS), hFont, FALSE);

    SetDlgItemText(hwnd, IDD_TXT_INFILEPATH, lpwc->lpszFilePathSrc);
    SetDlgItemText(hwnd, IDD_TXT_OUTFILEPATH, lpwc->lpszFilePathDst);

    BeginConvert(hwnd, (WAVECONVCB FAR*)lParam);
    return (TRUE);
}

static void Cls_OnConvertBegin(HWND hwnd, WAVECONVCB FAR* lpwc)
{
    gfCancelConvert = FALSE;
    if (WaveConvBegin(hwnd, lpwc))
    {
        WaveConvConvert(hwnd, lpwc);
    }
    else
    {
        EndConvert(hwnd, FALSE, lpwc);
    }
}

static void Cls_OnConvertEnd(HWND hwnd, BOOL fCancel, WAVECONVCB FAR* lpwc)
{
    WaveConvEnd(hwnd, lpwc);
    EndDialog(hwnd, fCancel);
}

static void Cls_OnCommandConvert(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify)
{
    if (id == IDCANCEL)
        gfCancelConvert = TRUE;
}

/** BOOL FAR PASCAL DlgProcConvert(HWND hdlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
 *
 *  DESCRIPTION:
 *      
 *
 *  ARGUMENTS:
 *      (HWND hdlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
 *
 *  RETURN (BOOL FAR PASCAL):
 *
 *
 *  NOTES:
 *
 ** */

BOOL FAR PASCAL DlgProcConvert(HWND hdlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
        case WM_INITDIALOG:
            return (BOOL)(UINT)(DWORD)(LRESULT)HANDLE_WM_INITDIALOG(hdlg, wParam, lParam, Cls_OnInitDialog);

        case WM_CONVERT_BEGIN:
            HANDLE_WM_CONVERT_BEGIN(hdlg, wParam, lParam, Cls_OnConvertBegin);
            break;

        case WM_CONVERT_END:
            HANDLE_WM_CONVERT_END(hdlg, wParam, lParam, Cls_OnConvertEnd);
            break;

        case WM_COMMAND:
            HANDLE_WM_COMMAND(hdlg, wParam, lParam, Cls_OnCommandConvert);
            break;
    }

    return (FALSE);
} /* DlgProcConvert() */


static void Cls_OnCommandAbout(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify)
{
    if ((id == IDOK) || (id == IDCANCEL))
        EndDialog(hwnd, (id == IDOK));
}

/** BOOL FAR PASCAL DlgProcAbout(HWND hdlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
 *
 *  DESCRIPTION:
 *      
 *
 *  ARGUMENTS:
 *      (HWND hdlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
 *
 *  RETURN (BOOL FAR PASCAL):
 *
 *
 *  NOTES:
 *
 ** */

BOOL FAR PASCAL DlgProcAbout(HWND hdlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
        case WM_INITDIALOG:
            return (TRUE);

        case WM_COMMAND:
            HANDLE_WM_COMMAND(hdlg, wParam, lParam, Cls_OnCommandAbout);
            break;
    }

    return (FALSE);
} /* DlgProcAbout() */


/** void NEAR PASCAL WaveConvFileClose(void)
 *
 *  DESCRIPTION:
 *      
 *
 *  ARGUMENTS:
 *      (void)
 *
 *  RETURN (void NEAR PASCAL):
 *
 *
 *  NOTES:
 *
 ** */

void NEAR PASCAL WaveConvFileClose(void)
{
    //
    //  shut down any file we have open, etc.
    //
    if (glpwio)
    {
	wioFileClose(&glpwio,0);
        glpwfx = NULL;
	glpwio=NULL;
    }

    lstrcpy(gszWaveFilePath, gszNone);
    lstrcpy(gszWaveFormat, gszUnknown);
} /* WaveConvFileClose() */


/** BOOL NEAR PASCAL WaveConvFileOpen(HWND hwnd)
 *
 *  DESCRIPTION:
 *      
 *
 *  ARGUMENTS:
 *      (HWND hwnd)
 *
 *  RETURN (BOOL NEAR PASCAL):
 *
 *
 *  NOTES:
 *
 ** */

BOOL NEAR PASCAL WaveConvFileOpen(HWND hwnd, LPSTR lpszFilePath)
{

    LRESULT         lr;
    LPWAVEFORMATEX  lpwfx;

    //
    // close anything that is open
    //
    WaveConvFileClose();
    
    //
    //  we have a valid file name, now attempt to open the file and see
    //  if we can deal with it... currently, this means it must either
    //  be a valid PCM or ADPCM file...
    //
    //  so, first get the wave header (just the common info...) then
    //  verify that it is way-q-zomo...
    //
    lr = wioFileOpen(&glpwio, lpszFilePath, 0L);
    if (lr)
    {
        DPF("WaveConvFileOpen: wioFileOpen failed=%lu", lr);
        AppErrorMsg(hwnd, "%ls is not a valid wave file.", lpszFilePath);
        return (FALSE);
    }

    //
    //  name the thing if we know what type it is
    //
    lpwfx = &glpwio->wfx;
    wsupGetFormatName(lpwfx, gszWaveFormat);

    glpwfx = lpwfx;

    //
    //
    //
    lstrcpy(gszWaveFilePath, lpszFilePath);

    //
    //  something changed...
    //
    return (TRUE);
} /* WaveConvFileOpen() */


/** BOOL NEAR PASCAL WaveConvGetNewFile(HWND hwnd)
 *
 *  DESCRIPTION:
 *      
 *
 *  ARGUMENTS:
 *      (HWND hwnd)
 *
 *  RETURN (BOOL NEAR PASCAL):
 *
 *
 *  NOTES:
 *
 ** */

BOOL NEAR PASCAL WaveConvGetNewFile(HWND hwnd)
{
    BOOL    f;
    char    szFilePath[MAX_FILE_PATH_LEN];

    //
    //  toss up a commdlg file open thing to get a .wav file to work with
    //
    f = AppGetFileName(hwnd, szFilePath, NULL, AGFN_FLAG_OPEN);
    if (!f)
        return (FALSE);

    //
    //  attempt to open the wave file..
    //
    WaveConvFileOpen(hwnd, szFilePath);
   
    //
    //  always say that something changed if a file was selected (regardless
    //  of whether it was a valid wave file or not). the reason for this is
    //  to make sure that display gets updated...
    //
    return (TRUE);
} /* WaveConvGetNewFile() */


/** BOOL NEAR PASCAL WaveConvSaveFileAs(HWND hwnd)
 *
 *  DESCRIPTION:
 *      
 *
 *  ARGUMENTS:
 *      (HWND hwnd)
 *
 *  RETURN (BOOL NEAR PASCAL):
 *
 *
 *  NOTES:
 *
 ** */

BOOL NEAR PASCAL WaveConvSaveFileAs(HWND hwnd)
{
    CONVERTPROC     fpConvert;
    BOOL            f;
    PSTR            psz;
    LPWAVEFORMATEX  lpwfxDst;
    WORD            wHdrSize;


    //
    //  get the output file name... 
    //
    f = AppGetFileName(hwnd, gszSaveFilePath, NULL, AGFN_FLAG_SAVE);
    if (!f)
        return (FALSE);

    //
    //
    //
    psz       = NULL;
    wHdrSize  = 0;
    fpConvert = NULL;
    switch (glpwfx->wFormatTag)
    {
        case WAVE_FORMAT_PCM:
            if (!pcmIsValidFormat(glpwfx))
            {
                psz = "This PCM wave file is not valid for conversion!";
                break;
            }

            //
            //  !!! hack hack hack !!! should have some way of 'determining'
            //  wave header size rather than hardcoding...
            //
            wHdrSize  = sizeof(WAVEFORMATEX) + 32;
            fpConvert = (CONVERTPROC)msadpcmEncode4Bit;
            break;

        case WAVE_FORMAT_ADPCM:
            if (!msadpcmIsValidFormat(glpwfx))
            {
                psz = "This ADPCM wave file is not valid for conversion!";
                break;
            }

            wHdrSize  = sizeof(WAVEFORMATEX);
            fpConvert = (CONVERTPROC)msadpcmDecode4Bit;
            break;

        default:
            psz = "This wave converter application does not know how to convert current format!";
            break;
    }


    //
    //  blast out an error msg if appropriate and fail if we should
    //  
    if (psz)
    {
        AppErrorMsg(hwnd, psz);
        return (FALSE);
    }

    //
    //  
    //
    //
    //
    lpwfxDst = (LPWAVEFORMATEX)GlobalAllocPtr(GHND, wHdrSize);
    if (!lpwfxDst)
    {
        AppErrorMsg(hwnd, "Could not allocate destination conversion header!");
        return (FALSE);
    }

    //
    //  build the destination format header and then call the conversion
    //  dialog box...
    //
    f = msadpcmBuildFormatHeader(glpwfx, lpwfxDst);
    if (f)
    {
        _fmemset(&gwc, 0, sizeof(gwc));

        gwc.dwSrcSamples    = glpwio->dwDataSamples;
        gwc.lpszFilePathSrc = gszWaveFilePath;
        gwc.lpwfxSrc        = glpwfx;
        gwc.lpszFilePathDst = gszSaveFilePath;
        gwc.lpwfxDst        = lpwfxDst;
        gwc.fpConvert       = fpConvert;

        gwc.dwSrcBytes      = glpwio->dwDataBytes;

        //
        //
        //
        if (lpwfxDst->wFormatTag == WAVE_FORMAT_PCM)
        {
            gwc.dwSrcReadSize = glpwfx->nBlockAlign;
            gwc.dwDstBufSize  = ((LPADPCMWAVEFORMAT)glpwfx)->wSamplesPerBlock * lpwfxDst->nBlockAlign;
        }
        else
        {
            gwc.dwSrcReadSize = ((LPADPCMWAVEFORMAT)lpwfxDst)->wSamplesPerBlock * glpwfx->nBlockAlign;
            gwc.dwDstBufSize  = lpwfxDst->nBlockAlign;
        }

        f = AppDialogBox(hwnd, DLG_CONVERT, DlgProcConvert, (LPARAM)(LPVOID)&gwc);
        if (f)
            psz = "Conversion completed successfully!";
        else
            psz = "Conversion aborted--destination file is corrupt!";

        MessageBox(hwnd, psz, gszAppName, MB_OK);
    }
    else
    {
        AppErrorMsg(hwnd, "There is something invalid/corrupt in the source header!");
    }

    //
    //  free the destination format header if it was alloc'd
    //
    if (lpwfxDst)
        GlobalFreePtr(lpwfxDst);

    return (f);
} /* WaveConvSaveFileAs() */

void NEAR PASCAL waveconvDumpWaveFormat(HDC hdc, LPWAVEFORMATEX lpwfx, WORD x, WORD y, WORD nFontHeight)
{
    WORD    w;
    char    ach[128 + 64];
    WORD nLength;
    
    nLength = wsprintf(ach, "          Wave Format: [%u], %ls",
                        glpwfx->wFormatTag, (LPSTR)gszWaveFormat);
    TextOut(hdc, x, y, ach, nLength);  y += nFontHeight;

    nLength = wsprintf(ach, "             Channels: %u", glpwfx->nChannels);
    TextOut(hdc, x, y, ach, nLength);  y += nFontHeight;

    nLength = wsprintf(ach, "   Samples Per Second: %lu", glpwfx->nSamplesPerSec);
    TextOut(hdc, x, y, ach, nLength);  y += nFontHeight;

    nLength = wsprintf(ach, " Avg Bytes Per Second: %lu", glpwfx->nAvgBytesPerSec);
    TextOut(hdc, x, y, ach, nLength);  y += nFontHeight;

    nLength = wsprintf(ach, "      Block Alignment: %u", glpwfx->nBlockAlign);
    TextOut(hdc, 5, y, ach, nLength);  y += nFontHeight;

    nLength = wsprintf(ach, "      Bits Per Sample: %u", glpwfx->wBitsPerSample);
    TextOut(hdc, 5, y, ach, nLength);  y += nFontHeight;

    nLength = wsprintf(ach, "   Extra Header Bytes: %u", glpwfx->cbSize);
    TextOut(hdc, 5, y, ach, nLength);  y += nFontHeight;

    y += 5;

    //
    //  more silly hardcoded stuff to show info about ADPCM file
    //
    if (glpwfx->wFormatTag == WAVE_FORMAT_ADPCM)
    {
        LPADPCMWAVEFORMAT   lpwfxA;

        lpwfxA = (LPADPCMWAVEFORMAT)glpwfx;

        nLength = wsprintf(ach, "    Samples Per Block: %u", lpwfxA->wSamplesPerBlock);
        TextOut(hdc, 5, y, ach, nLength);  y += nFontHeight;

        nLength = wsprintf(ach, "    Coefficient Pairs: %u", lpwfxA->wNumCoef);
        TextOut(hdc, 5, y, ach, nLength);  y += nFontHeight;

        for (w = 0; w < lpwfxA->wNumCoef; w++)
        {
            nLength = wsprintf(ach, "              Pair #%u: (%4d,%4d)", w + 1,
                            lpwfxA->aCoef[w].iCoef1, lpwfxA->aCoef[w].iCoef2);
            TextOut(hdc, 5, y, ach, nLength);  y += nFontHeight;
        }
    }
    
    y += 5;

    //
    //  some other stats
    //
    nLength = wsprintf(ach, "     Total Data Bytes: %lu", glpwio->dwDataBytes);
    TextOut(hdc, 5, y, ach, nLength);  y += nFontHeight;

    nLength = wsprintf(ach, "   Total Data Samples: %lu", glpwio->dwDataSamples);
    TextOut(hdc, 5, y, ach, nLength);  y += nFontHeight;
}

/** void NEAR PASCAL AppPaint(HWND hwnd, HDC hdc)
 *
 *  DESCRIPTION:
 *      
 *
 *  ARGUMENTS:
 *      (HWND hwnd, HDC hdc)
 *
 *  RETURN (void NEAR PASCAL):
 *
 *
 *  NOTES:
 *
 ** */

void NEAR PASCAL AppPaint(HWND hwnd, HDC hdc)
{
    static  int nFontHeight = 0;
    char    ach[128 + 64];
    int     nLength;
    int     y;

    //
    //  this is terribly inefficient--but i don't care right now... this
    //  whole paint routine is poorly written!
    //
    SetTextColor(hdc, GetSysColor(COLOR_WINDOWTEXT));
    SetBkColor(hdc, GetSysColor(COLOR_WINDOW));
    SelectFont(hdc, GetStockFont(ANSI_FIXED_FONT));

    if (nFontHeight == 0)
    {
        TEXTMETRIC  tm;
        GetTextMetrics(hdc, &tm);
        nFontHeight = (int)tm.tmHeight;
    }

    y = 5;
    
    nLength = wsprintf(ach, "       Wave File Path: %ls", (LPSTR)gszWaveFilePath);
    TextOut(hdc, 5, y, ach, nLength);  y += nFontHeight;

    y += 5;

    //
    //  if nothing more to display, then don't 
    //
    if (!glpwfx)
        return;

    waveconvDumpWaveFormat(hdc, glpwfx, 5, y, nFontHeight);
    

} /* AppPaint() */


/** LRESULT NEAR PASCAL AppCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify)
 *
 *  DESCRIPTION:
 *      
 *
 *  ARGUMENTS:
 *      (HWND hwnd, int id, HWND hwndCtl, UINT codeNotify)
 *
 *  RETURN (LRESULT NEAR PASCAL):
 *
 *
 *  NOTES:
 *
 ** */

LRESULT NEAR PASCAL AppCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify)
{
    BOOL    f;
    LRESULT lr;

    switch (id)
    {
	case IDM_PLAY:
	    if(wioIsPlaying(glpwio))
		wioStopWave(glpwio);
	    else
	    {
		lr=wioPlayWave(hwnd, glpwio, gszWaveFilePath, 0);
		switch(lr)
		{
		    case WIOERR_NOERROR:
			// everything OK
			break;
		    case WIOERR_ALLOCATED:
			AppErrorMsg(hwnd, lpplayerr, (LPCSTR)"Output device alread allocated");
			break;
	    
		    case WIOERR_NOTSUPPORTED:
			AppErrorMsg(hwnd, lpplayerr, (LPCSTR)"No available device supports this format");
			break;
		    default:
			AppErrorMsg(hwnd, lpplayerr, (LPCSTR)"Unknown Error");
		}
	    }
	    break;
	    
	case IDM_DISP:
	    break;

	case IDM_FORMAT:
	    break;
	    
	case IDM_STEREO:
	    break;
	    
	case IDM_MONO:
	    break;
	    
	case IDM_SAMPLE:
	    break;
	    
	case IDM_INFO:
	    riffEditINFO(hwnd, glpwio->pInfo, ghInstance);
	    break;
	    
        case IDM_FILE_OPEN:
            //
            //  try to get a new wave file--if something changes, then update
            //  the display...
            //
            f = WaveConvGetNewFile(hwnd);
            if (f)
            {
                InvalidateRect(hwnd, NULL, TRUE);
                UpdateWindow(hwnd);
            }
            break;

        case IDM_FILE_SAVEAS:
            if (glpwio)
            {
                WaveConvSaveFileAs(hwnd);
            }
            break;

        case IDM_FILE_ABOUT:
            AppDialogBox(hwnd, ABOUTBOX, DlgProcAbout, 0L);
            break;

        case IDM_FILE_EXIT:
            FORWARD_WM_CLOSE(hwnd, SendMessage);
            break;
    }

    return (0L);
} /* AppCommand() */


/** void NEAR PASCAL AppInitMenu(HWND hwnd, HMENU hMenu)
 *
 *  DESCRIPTION:
 *      
 *
 *  ARGUMENTS:
 *      (HWND hwnd, HMENU hMenu)
 *
 *  RETURN (void NEAR PASCAL):
 *
 *
 *  NOTES:
 *
 ** */

void NEAR PASCAL AppInitMenu(HWND hwnd, HMENU hMenu)
{
    UINT    mf;

    mf = glpwio ? MF_ENABLED : MF_GRAYED;
    EnableMenuItem(hMenu, IDM_FILE_SAVEAS, mf);
    EnableMenuItem(hMenu, IDM_INFO, mf);
    EnableMenuItem(hMenu, IDM_DISP, mf);

    ModifyMenu(hMenu,IDM_PLAY, MF_BYCOMMAND | MF_STRING | mf, IDM_PLAY, glpwio && wioIsPlaying(glpwio) ? "&Stop" : "&Play");
    

} /* AppInitMenu() */


/** void NEAR PASCAL AppAcceptDroppedFiles(HWND hwnd, HDROP hDrop)
 *
 *  DESCRIPTION:
 *      
 *
 *  ARGUMENTS:
 *      (HWND hwnd, HDROP hDrop)
 *
 *  RETURN (void NEAR PASCAL):
 *
 *
 *  NOTES:
 *
 ** */

void NEAR PASCAL AppAcceptDroppedFiles(HWND hwnd, HDROP hDrop)
{
    char    szFilePath[MAX_FILE_PATH_LEN];
    WORD    wNumFiles;
    WORD    w;
    BOOL    f;
    int     i;

    //
    //  get number of files dropped on our window
    //
    wNumFiles = DragQueryFile(hDrop, (UINT)-1, NULL, 0);

    //
    //  step through each file and stop on the one the user wants or 
    //  the last file (whichever comes first).
    //
    for (w = 0; w < wNumFiles; w++)
    {
        // 
        //  get the next file name and try to open it--if not a valid wave
        //  file, then skip to the next one (if there is one).
        //
        DragQueryFile(hDrop, w, (LPSTR)szFilePath, sizeof(szFilePath));

        f = WaveConvFileOpen(hwnd, szFilePath);

        //
        //  force update to display (even if file is not valid)
        //
        InvalidateRect(hwnd, NULL, TRUE);
        UpdateWindow(hwnd);

        //
        //  if this is NOT the last file in the list of files that are
        //  being dropped on us, then bring up a box asking if we should
        //  continue or stop where we are..
        //
        if (f && (w != (wNumFiles - 1)))
        {
            i = MessageBox(hwnd, "Continue?", gszAppName,
                        MB_YESNO | MB_ICONQUESTION | MB_TASKMODAL);
            if (i == IDNO)
                break;
        }
    }

    //
    //  tell the shell to release the memory it allocated for beaming
    //  the file name(s) over to us...
    //
    DragFinish(hDrop);
} /* AppAcceptDroppedFiles() */


static void Cls_OnPaint(HWND hwnd)
{
    PAINTSTRUCT ps;

    BeginPaint(hwnd, &ps);
    AppPaint(hwnd, ps.hdc);
    EndPaint(hwnd, &ps);
}

/** LRESULT FAR PASCAL WndProcApp(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
 *
 *  DESCRIPTION:
 *      
 *
 *  ARGUMENTS:
 *      (HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
 *
 *  RETURN (LRESULT FAR PASCAL):
 *
 *
 *  NOTES:
 *
 ** */

LRESULT FAR PASCAL WndProcApp(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg) 
    {
        case WM_DESTROY:
            PostQuitMessage(0);
            break;

        case WM_INITMENU:
            HANDLE_WM_INITMENU(hwnd, wParam, lParam, AppInitMenu);
            break;

        case WM_COMMAND:
            return HANDLE_WM_COMMAND(hwnd, wParam, lParam, AppCommand);

        case WM_CLOSE:
            HANDLE_WM_CLOSE(hwnd, wParam, lParam, DestroyWindow);
            break;

        case WM_PAINT:
            HANDLE_WM_PAINT(hwnd, wParam, lParam, Cls_OnPaint);
            break;

        case WM_DROPFILES:
            HANDLE_WM_DROPFILES(hwnd, wParam, lParam, AppAcceptDroppedFiles);
            break;
	    
        case MM_WOM_DONE:
	    wioWaveOutDone(glpwio, (LPWAVEHDR) lParam);
	    return TRUE;


        default:
            return (DefWindowProc(hwnd, uMsg, wParam, lParam));
    }

    return (0L);
} /* WndProcApp() */


/** BOOL NEAR PASCAL AppInit(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdLine, int nCmdShow)
 *
 *  DESCRIPTION:
 *      
 *
 *  ARGUMENTS:
 *      (HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdLine, int nCmdShow)
 *
 *  RETURN (BOOL NEAR PASCAL):
 *
 *
 *  NOTES:
 *
 ** */

BOOL NEAR PASCAL AppInit(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdLine, int nCmdShow)
{
    WNDCLASS    wc;

    ghInstance = hInstance;

    if (!hPrevInstance) 
    {
        wc.style            = CS_HREDRAW | CS_VREDRAW;
        wc.lpfnWndProc      = WndProcApp;
        wc.cbClsExtra       = 0;
        wc.cbWndExtra       = 0;
        wc.hInstance        = hInstance;
        wc.hIcon            = LoadIcon(hInstance, ICON_APP);
        wc.hCursor          = LoadCursor(NULL, IDC_ARROW);
        wc.hbrBackground    = (HBRUSH)(COLOR_WINDOW + 1);
        wc.lpszMenuName     = MENU_APP;
        wc.lpszClassName    = gszAppName;

        if (!RegisterClass(&wc))
            return (FALSE);
    }

    //
    //  init wave file vars, etc..
    //
    WaveConvFileClose();

    //
    //  open the app's window
    //
    ghwnd = CreateWindow(gszAppName, gszAppName, WS_OVERLAPPEDWINDOW,
                          CW_USEDEFAULT, CW_USEDEFAULT,
                          APP_WINDOW_WIDTH, APP_WINDOW_HEIGHT,
                          NULL, NULL, hInstance, NULL);             

    if (!ghwnd)
        return (FALSE);

    //
    //  register the app's window for drag-drop messages from winfile
    //
    DragAcceptFiles(ghwnd, TRUE);

    ShowWindow(ghwnd, nCmdShow);
//  UpdateWindow(ghwnd);

    return (TRUE);
} /* AppInit() */


/** void NEAR PASCAL AppExit(void)
 *
 *  DESCRIPTION:
 *      
 *
 *  ARGUMENTS:
 *      (void)
 *
 *  RETURN (void NEAR PASCAL):
 *
 *
 *  NOTES:
 *
 ** */

void NEAR PASCAL AppExit(void)
{
    //
    //  shut down any file we have open, etc.
    //
    WaveConvFileClose();

} /* AppExit() */


/** int PASCAL WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
 *
 *  DESCRIPTION:
 *      
 *
 *  ARGUMENTS:
 *      (HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdLine, int nCmdShow)
 *
 *  RETURN (int PASCAL):
 *
 *
 *  NOTES:
 *
 ** */

int PASCAL WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdLine, int nCmdShow)
{
    MSG     msg;

    //
    //  init some stuff, create window, etc..
    //
    if (AppInit(hInstance, hPrevInstance, lpszCmdLine, nCmdShow))
    {
        //
        //  dispatch messages
        //
        while (GetMessage(&msg, NULL, 0, 0))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    //
    //  shut things down, clean up, etc.
    //
    AppExit();

    return (msg.wParam);
} /* WinMain() */


/*********************** BEGIN: DEBUGGING ANNEX ****************************/

#ifdef DEBUG

void FAR cdecl dprintf(LPSTR szFormat, ...)
{
//  extern FAR PASCAL OutputDebugStr(LPSTR);
    char ach[512];
    int  s,d;

    s = wvsprintf(ach,szFormat,(LPSTR)(&szFormat+1));
#if 1
    lstrcat(ach,"\n");
    s++;
#endif
    for (d=sizeof(ach)-1; s>=0; s--)
    {
        if ((ach[d--] = ach[s]) == '\n')
            ach[d--] = '\r';
    }

    OutputDebugStr("WAVECONV: ");
    OutputDebugStr(ach+d+1);
}

#endif

/************************ END: DEBUGGING ANNEX *****************************/


/** EOF: waveconv.c **/
