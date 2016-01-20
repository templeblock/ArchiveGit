/** RIFFsup.c
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
 ** */


//
//  the includes we need
//
#define STRICT
#include <windows.h>
#include <windowsx.h>
#include <mmsystem.h>
#include "mmreg.h"
#include "riffsup.h"


#if (_MSC_VER < 700)
//
//  this silly stuff is to get around a bug in the C6 compiler's optimizer
//
#ifdef GlobalFreePtr
#undef GlobalFreePtr
#define GlobalFreePtr(x)    GlobalFree((HGLOBAL)SELECTOROF(x))
#endif

#endif

#define MAXSTR      255

static char        szBuf[MAXSTR];



LRESULT NEAR PASCAL riffParseINFO(const LPINFOCHUNK lpInfo);


/** BOOL RIFFAPI riffCopyChunk(HMMIO hmmioSrc, HMMIO hmmioDst, const LPMMCKINFO lpck)
 *
 *  DESCRIPTION:
 *      
 *
 *  ARGUMENTS:
 *      (LPWAVECONVCB lpwc, LPMMCKINFO lpck)
 *
 *  RETURN (BOOL NEAR PASCAL):
 *
 *
 *  NOTES:
 *
 **  */

BOOL RIFFAPI riffCopyChunk(HMMIO hmmioSrc, HMMIO hmmioDst, const LPMMCKINFO lpck)
{
    MMCKINFO    ck;
    HPSTR       hpBuf;

    //
    //
    //
    hpBuf = (HPSTR)GlobalAllocPtr(GHND, lpck->cksize);
    if (!hpBuf)
        return (FALSE);

    ck.ckid   = lpck->ckid;
    ck.cksize = lpck->cksize;
    if (mmioCreateChunk(hmmioDst, &ck, 0))
	goto rscc_Error;
	
    if (mmioRead(hmmioSrc, hpBuf, lpck->cksize) != (LONG)lpck->cksize)
        goto rscc_Error;

    if (mmioWrite(hmmioDst, hpBuf, lpck->cksize) != (LONG)lpck->cksize)
        goto rscc_Error;

    if (mmioAscend(hmmioDst, &ck, 0))
        goto rscc_Error;

    if (hpBuf)
        GlobalFreePtr(hpBuf);

    return (TRUE);

rscc_Error:

    if (hpBuf)
        GlobalFreePtr(hpBuf);

    return (FALSE);
} /* RIFFSupCopyChunk() */

/** BOOL RIFFAPI riffCopyList(HMMIO hmmioSrc, HMMIO hmmioDst, const LPMMCKINFO lpck)
 *
 *  DESCRIPTION:
 *      
 *
 *  ARGUMENTS:
 *  (HMMIO hmmioSrc, HMMIO hmmioDst, LPMMCKINFO lpck)
 *
 *  RETURN (BOOL NEAR PASCAL):
 *
 *
 *  NOTES:
 *
 ** */

BOOL RIFFAPI riffCopyList(HMMIO hmmioSrc, HMMIO hmmioDst, const LPMMCKINFO lpck)
{
    MMCKINFO    ck;
    HPSTR       hpBuf;
    DWORD	dwCopySize;

    hpBuf = (HPSTR)GlobalAllocPtr(GHND, lpck->cksize);
    if (!hpBuf)
        return (FALSE);

    dwCopySize=lpck->cksize;
    
    // mmio leaves us after LIST ID
	
    ck.ckid   = lpck->ckid;
    ck.cksize = dwCopySize;
    ck.fccType= lpck->fccType;
	
    if (mmioCreateChunk(hmmioDst, &ck, MMIO_CREATELIST))
	goto rscl_Error;

    // we already wrote 'LIST' ID, so reduce byte count
    dwCopySize-=sizeof(FOURCC);

    if (mmioRead(hmmioSrc, hpBuf, dwCopySize) != (LONG)dwCopySize)
        goto rscl_Error;

    if (mmioWrite(hmmioDst, hpBuf, dwCopySize) != (LONG)dwCopySize)
        goto rscl_Error;

    if (mmioAscend(hmmioDst, &ck, 0))
        goto rscl_Error;

    if (hpBuf)
        GlobalFreePtr(hpBuf);

    return (TRUE);

rscl_Error:

    if (hpBuf)
        GlobalFreePtr(hpBuf);

    return (FALSE);
} /* RIFFSupCopyList() */


/////////////////////////////////////////////////////////////////////////////

typedef struct tINFO
{
    char * pFOURCC;
    char * pShort;
    char * pLong;
} INFO;

static INFO aINFO[]=
{

"IARL",	"Archival Location", "Indicates where the subject of the file is archived.",
"IART",	"Artist", "Lists the artist of the original subject of the file. For example, \"Michaelangelo.\"",
"ICMS",	"Commissioned", "Lists the name of the person or organization that commissioned the subject of the file. For example, \"Pope Julian II.\"",
"ICMT",	"Comments", "Provides general comments about the file or the subject of the file. If the comment is several sentences long, end each sentence with a period. Do not include newline characters.",
"ICOP",	"Copyright", "Records the copyright information for the file. For example, \"Copyright Encyclopedia International 1991.\" If there are multiple copyrights, separate them by a semicolon followed by a space.",
"ICRD",	"Creation date", "Specifies the date the subject of the file was created. List dates in year-month-day format, padding one-digit months and days with a zero on the left. For example, \"1553-05-03\" for May 3, 1553.",
"ICRP",	"Cropped", "Describes whether an image has been cropped and, if so, how it was cropped. For example, \"lower right corner.\"",
"IDIM",	"Dimensions", "Specifies the size of the original subject of the file. For example, \"8.5 in h, 11 in w.\"",
"IDPI",	"Dots Per Inch", "Stores dots per inch setting of the digitizer used to produce the file, such as \"300.\"",
"IENG",	"Engineer", "Stores the name of the engineer who worked on the file. If there are multiple engineers, separate the names by a semicolon and a blank. For example, \"Smith, John; Adams, Joe.\"",
"IGNR",	"Genre", "Describes the original work, such as, \"landscape,\" \"portrait,\" \"still life,\" etc.",
"IKEY",	"Keywords", "Provides a list of keywords that refer to the file or subject of the file. Separate multiple keywords with a semicolon and a blank. For example, \"Seattle; aerial view; scenery.\"",
"ILGT",	"Lightness", "Describes the changes in lightness settings on the digitizer required to produce the file. Note that the format of this information depends on hardware used.",
"IMED",	"Medium", "Describes the original subject of the file, such as, \"computer image,\" \"drawing,\" \"lithograph,\" and so forth.",
"INAM",	"Name", "Stores the title of the subject of the file, such as, \"Seattle From Above.\"",
"IPLT",	"Palette Setting", "Specifies the number of colors requested when digitizing an image, such as \"256.\"",
"IPRD",	"Product", "Specifies the name of the title the file was originally intended for, such as \"Encyclopedia of Pacific Northwest Geography.\"",
"ISBJ",	"Subject", "Describes the contents of the file, such as \"Aerial view of Seattle.\"",
"ISFT",	"Software", "Identifies the name of the software package used to create the file, such as \"Microsoft WaveEdit.\"",
"ISHP",	"Sharpness", "Identifies the changes in sharpness for the digitizer required to produce the file (the format depends on the hardware used).",
"ISRC",	"Source", "Identifies the name of the person or organization who supplied the original subject of the file. For example, \"Trey Research.\"",
"ISRF",	"Source Form", "Identifies the original form of the material that was digitized, such as \"slide,\" \"paper,\" \"map,\" and so forth. This is not necessarily the same as IMED.",
"ITCH",	"Technician", "Identifies the technician who digitized the subject file. For example, \"Smith, John.\"",

NULL, NULL, NULL

};


void NEAR PASCAL riffInsertINFO(LPINFOCHUNK lpInfo, const PINFODATA pInfo)
{
    PINFODATA pI;
    
    if(!lpInfo)
	return;
    
    if(!lpInfo->pHead)
    {
	lpInfo->pHead=pInfo;
	return;
    }
    
    pI=lpInfo->pHead;
    while(pI->pnext)
    {
	pI=pI->pnext;
    }
    // insert at end
    pI->pnext=pInfo;
    
    return;
}

PINFODATA NEAR PASCAL riffCreateINFO(WORD id, WORD wFlags, DWORD dwInfoOffset, LPCSTR lpText)
{
    PINFODATA pI;
    pI=(PINFODATA)LocalAlloc(LPTR,sizeof(INFODATA));
    if(!pI)
	return NULL;
    
    pI->index=id;
    pI->wFlags=wFlags;
    pI->dwINFOOffset=dwInfoOffset;
    pI->lpText=lpText;
    
    return pI;
}

LRESULT RIFFAPI riffInitINFO(INFOCHUNK FAR * FAR * lplpInfo)
{
    LPINFOCHUNK lpInfo;
    WORD	id;
    PINFODATA	pI;
    
    lpInfo=(LPINFOCHUNK)GlobalAllocPtr(GHND, sizeof(INFOCHUNK));
    if(!lpInfo)
	return RIFFERR_NOMEM;
    *lplpInfo=lpInfo;

    for (id=0;aINFO[id].pFOURCC;id++)
    {
	pI=riffCreateINFO(id, 0, 0L, NULL);   // create empty INFO
	riffInsertINFO(lpInfo,pI);
    }
    return RIFFERR_NOERROR;
}

LRESULT RIFFAPI riffReadINFO(HMMIO hmmio, const LPMMCKINFO lpck, LPINFOCHUNK lpInfo)
{
    DWORD	dwInfoSize;

    dwInfoSize=lpck->cksize - sizeof(FOURCC);	// take out 'INFO'

    lpInfo->cksize=dwInfoSize;
    lpInfo->lpChunk=GlobalAllocPtr(GHND, dwInfoSize);
    if(!lpInfo->lpChunk)
	return RIFFERR_NOMEM;
    
    if (mmioRead(hmmio, (HPSTR)lpInfo->lpChunk, dwInfoSize) != (LONG)dwInfoSize)
	return RIFFERR_FILEERROR;
    else
        return riffParseINFO(lpInfo);
}

PINFODATA NEAR PASCAL riffFindPIINFO(const LPINFOCHUNK lpInfo, FOURCC fcc)
{
    PINFODATA pI;

    pI=lpInfo->pHead;
    while(pI)
    {
	if(mmioStringToFOURCC(aINFO[pI->index].pFOURCC,0)==fcc)
	    return(pI);
	pI=pI->pnext;
    }
    return NULL;
}

void NEAR PASCAL riffModifyINFO(const LPINFOCHUNK lpInfo, PINFODATA pI, WORD wFlags, DWORD dw, LPCSTR lpText)
{
    if(!pI)
	return;
    
    pI->wFlags=wFlags;
    if(!(wFlags&INFOCHUNK_MODIFIED))
	pI->dwINFOOffset=dw;
    
    if(pI->lpText)
    {
	if(lpText)
	{
	    if(!lstrcmp(lpText,pI->lpText))
	    {
		// they are the same, don't bother changing...
		GlobalFreePtr(lpText);
	    }
	    else
	    {
		GlobalFreePtr(pI->lpText);
		pI->lpText=lpText;
	    }
	}
	else if(wFlags&INFOCHUNK_REVERT)
	{
	    GlobalFreePtr(pI->lpText);
	    pI->lpText=NULL;
	}
    }
    else if(lpText)
    {
	// if no read data, don't bother to check....
	if(!lpInfo->lpChunk && *lpText)
	{
	    pI->lpText=lpText;
	}
	else if(lstrcmp(lpText, (LPSTR)lpInfo->lpChunk+pI->dwINFOOffset))
	{ 	// new text...
	    if(*lpText)
		// NOT the same, set...
		pI->lpText=lpText;
	    else
		// new is blank, do nothing...
		GlobalFreePtr(lpText);
	}
	else
	    // the same, don't bother...
	    GlobalFreePtr(lpText);
    }
}

WORD NEAR PASCAL riffFindaINFO(FOURCC fcc)
{
    WORD    id;

    for (id=0;aINFO[id].pFOURCC;id++)
    {
	if(mmioStringToFOURCC(aINFO[id].pFOURCC,0)==fcc)
	    return id;
    }
    return 0xFFFF;
}



LRESULT NEAR PASCAL riffParseINFO(const LPINFOCHUNK lpInfo)
{
    LPSTR   lpBuf;
    DWORD   dwCurInfoOffset;
    PINFODATA pI;
    LPCHUNK lpck;

    lpBuf=lpInfo->lpChunk;
    for(dwCurInfoOffset=0;dwCurInfoOffset<lpInfo->cksize;)
    {
	lpck=(LPCHUNK)((LPSTR)(lpBuf+dwCurInfoOffset));
	dwCurInfoOffset+=sizeof(CHUNK);   // dwCurInfoOffset is offset of data
	pI=riffFindPIINFO(lpInfo,lpck->fcc);
	if(!pI)
	{
            UINT    u;

	    // file contains unknown INFO chunk
	    wsprintf(szBuf,"This wave file contains an unknown item in the INFO chunk: '%4.4s'.  Open anyway?",
		(LPCSTR)(lpck));
            u = MessageBox(NULL, szBuf,
                            "riffParseINFO", MB_YESNO | MB_ICONEXCLAMATION | MB_TASKMODAL);
            if (u == IDNO)
	    {
		return RIFFERR_BADFILE;
	    }
	    
	    
	}
	else
	{
	    // modify entry to show text (data) from file...
	    riffModifyINFO(lpInfo, pI, 0, dwCurInfoOffset, NULL);
	}
	dwCurInfoOffset+=lpck->cksize+(lpck->cksize&1);  // skip past data
    }

    return RIFFERR_NOERROR;
}

LRESULT RIFFAPI riffFreeINFO(INFOCHUNK FAR * FAR * lplpInfo)
{
    PINFODATA	pI;
    PINFODATA	pIT;
    LPINFOCHUNK lpInfo;
    LRESULT     lr;

    lr    = RIFFERR_BADPARAM;

    if(!lplpInfo)
	goto riff_FI_Error;
    
    lpInfo=*lplpInfo;
    if(!lpInfo)
	goto riff_FI_Error;
    
    if(lpInfo->lpChunk)
	GlobalFreePtr(lpInfo->lpChunk);


    pI=lpInfo->pHead;
    
    while(pI)
    {
	pIT=pI;
	pI=pI->pnext;
	LocalFree((HANDLE)pIT);
    }

    
    //

    GlobalFreePtr(lpInfo);
    *lplpInfo=NULL;
    return RIFFERR_NOERROR;
    
riff_FI_Error:    
    return lr;
}

static BOOL NEAR PASCAL riffSetupEditBoxINFO(HWND hdlg, const LPINFOCHUNK lpInfo, WORD wFlags)
{
    PINFODATA	pI;
    WORD	iSel;
    HWND	hLB;
    PSTR szFormat = "%-4s%c %-25s";
    
    hLB=GetDlgItem(hdlg, IDD_INFOLIST);
    if(wFlags&INFOCHUNK_MODIFIED)
    {
        iSel = ComboBox_GetCurSel(hLB);
	
    }
    else
        iSel = 0;

    ComboBox_ResetContent(hLB);
    
    pI=lpInfo->pHead;
    
    while(pI)
    {
	wsprintf(szBuf,szFormat,
	    (LPCSTR)aINFO[pI->index].pFOURCC,
	    (pI->dwINFOOffset || ( (pI->lpText) && (pI->lpText[0]) ) ) ?
			'*' : ' ',
	    (LPCSTR)aINFO[pI->index].pShort
	    );

	ComboBox_AddString(hLB, szBuf);
	pI=pI->pnext;
    }
    ComboBox_SetCurSel(hLB, iSel);

    if(!(wFlags&INFOCHUNK_MODIFIED))
    {
	// FIRST time only
	pI=lpInfo->pHead;
	if(pI)
	    if(pI->lpText)
		// Modified text
		SetDlgItemText(hdlg, IDD_INFOTEXT, (LPCSTR)pI->lpText);
	    else if(pI->dwINFOOffset)
		// default text
		SetDlgItemText(hdlg, IDD_INFOTEXT, (LPCSTR)(lpInfo->lpChunk+pI->dwINFOOffset));
	    else
		// no text
		SetDlgItemText(hdlg, IDD_INFOTEXT, (LPCSTR)"");
	SetDlgItemText(hdlg, IDD_INFOINFO, (LPCSTR)aINFO[0].pLong);
    }
    return TRUE;
}


static BOOL Cls_OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam)
{
    LPINFOCHUNK lpInfo;
    HFONT       hFont;
    HWND	hLB;

    lpInfo = (LPINFOCHUNK)lParam;
    if(!lpInfo)
        return FALSE;

    SetWindowLong(hwnd, DWL_USER, (LONG)lpInfo);
	    
    hFont = GetStockFont(ANSI_FIXED_FONT);

    hLB=GetDlgItem(hwnd, IDD_INFOLIST);
    SetWindowFont(hLB, hFont, FALSE);

    riffSetupEditBoxINFO(hwnd, lpInfo, 0);

    return TRUE;
}

static void Cls_OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify)
{
    LPINFOCHUNK lpInfo;
    PINFODATA   pI;
    WORD        iSel;
    int         i;
    LPSTR       lpstr;

    lpInfo=(LPINFOCHUNK)GetWindowLong(hwnd, DWL_USER);
            
    switch(id)
    {
        case IDOK:
        case IDCANCEL:
            EndDialog(hwnd, (id == IDOK));
            break;
        case IDD_INFOLIST:
            switch(codeNotify)
            {
                case CBN_SELCHANGE:

                    iSel = ComboBox_GetCurSel(GetDlgItem(hwnd, id));
                    SetDlgItemText(hwnd, IDD_INFOINFO, (LPCSTR)aINFO[iSel].pLong);

                    pI=lpInfo->pHead;
                    while(pI)
                    {
                        if(pI->index==iSel)
                            break;
                        pI=pI->pnext;
                    }
                    if(pI)
                    {
                        if(pI->lpText)
                            // Modified text
                            SetDlgItemText(hwnd, IDD_INFOTEXT, (LPCSTR)pI->lpText);
                        else if(pI->dwINFOOffset)
                            // default text
                            SetDlgItemText(hwnd, IDD_INFOTEXT, (LPCSTR)(lpInfo->lpChunk+pI->dwINFOOffset));
                        else
                            // no text
                            SetDlgItemText(hwnd, IDD_INFOTEXT, (LPCSTR)"");
                    }
                        else
                            SetDlgItemText(hwnd, IDD_INFOINFO, (LPCSTR)"Can't FIND iSel");
                    break;
            }

        case IDD_INFOTEXT:
            switch(codeNotify)
            {
                case EN_KILLFOCUS:
                    // get text out and give to current id
                    iSel=(WORD)SendDlgItemMessage(hwnd,IDD_INFOLIST,CB_GETCURSEL,0,0L);
                    pI=lpInfo->pHead;
                    while(pI)
                    {
                        if(pI->index==iSel)
                            break;
                        pI=pI->pnext;
                    }
                    if(pI)
                    {
                        i=GetDlgItemText(hwnd, IDD_INFOTEXT, szBuf,sizeof(szBuf));
                        lpstr=(LPSTR)GlobalAllocPtr(GHND,(DWORD)i+1);
                        if(!lpstr)
                            break;

                        lstrcpy(lpstr,szBuf);

                        riffModifyINFO(lpInfo, pI, INFOCHUNK_MODIFIED, 0, lpstr);

                        riffSetupEditBoxINFO(hwnd, lpInfo, INFOCHUNK_MODIFIED);
                    }
                    else
                        SetDlgItemText(hwnd, IDD_INFOINFO, (LPCSTR)"Can't FIND iSel");
                    break;

            }
            break;
        case IDD_INFOINFO:
            break;
    }

}                           

BOOL FAR PASCAL _export DlgProcINFOEdit(HWND hdlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
        case WM_INITDIALOG:
            return (BOOL)(UINT)(DWORD)(LRESULT)HANDLE_WM_INITDIALOG(hdlg, wParam, lParam, Cls_OnInitDialog);

        case WM_COMMAND:
            HANDLE_WM_COMMAND(hdlg, wParam, lParam, Cls_OnCommand);
            break;
    }

    return FALSE;
}


LRESULT RIFFAPI riffEditINFO(HWND hwnd, LPINFOCHUNK lpInfo, HINSTANCE hInst)
{
    LRESULT     lr;
    DLGPROC	lpfn;
#ifdef DEBUG    
    int		i;
#endif
    
    lr    = RIFFERR_BADPARAM;

    if(!lpInfo)
	goto riff_EI_Error;

    if (lpfn = (DLGPROC)MakeProcInstance((FARPROC)DlgProcINFOEdit, hInst))
    {
#ifdef DEBUG
	i=
#endif
        DialogBoxParam(hInst, DLG_INFOEDIT, hwnd, lpfn, (LPARAM)(LPVOID)lpInfo);
        FreeProcInstance((FARPROC)lpfn);
	lr=RIFFERR_NOERROR;
#ifdef DEBUG
	if(i==-1)
	{
	    MessageBox(hwnd, "INFO Edit Error: DLG_INFOEDIT not found.  Check .RC file.", "RIFF SUP module", MB_OK|MB_ICONEXCLAMATION|MB_TASKMODAL);
	    lr=RIFFERR_ERROR;
	}
#endif
	
    }
#ifdef DEBUG
    else
    {
	MessageBox(hwnd, "INFO Edit Error: Can't MakeProcInstace()", "RIFF SUP module", MB_OK|MB_ICONEXCLAMATION|MB_TASKMODAL);
	lr=RIFFERR_ERROR;
    }
#endif
    
riff_EI_Error:    
    return lr;
}

LRESULT RIFFAPI riffWriteINFO(HMMIO hmmioDst, LPINFOCHUNK lpInfo)
{
    LRESULT     lr;
    MMCKINFO    ck;
    MMCKINFO    ckINFO;
    PINFODATA	pI;
    LPSTR	lpstr;
    BOOL	fList=FALSE;

    lr    = RIFFERR_BADPARAM;

    if(!hmmioDst || !lpInfo)
	goto riff_SI_Error;

    lr=RIFFERR_FILEERROR;
    
    ckINFO.ckid   = mmioFOURCC('L', 'I', 'S', 'T');
    ckINFO.cksize = 0;  // mmio fill fill it in later
    ckINFO.fccType= mmioFOURCC('I', 'N', 'F', 'O');
	
    pI=lpInfo->pHead;
    
    while(pI)
    {
	if(pI->lpText)
	    // Modified text
	    lpstr=pI->lpText;
	else if(pI->dwINFOOffset)
	    // default text
	    lpstr=(lpInfo->lpChunk+pI->dwINFOOffset);
	else
	    // no text
	    lpstr=NULL;
	if(lpstr)
	{
	    if(!fList)
	    {
		// only create if needed...
		if (mmioCreateChunk(hmmioDst, &ckINFO, MMIO_CREATELIST))
		    goto riff_SI_Error;
		fList=TRUE;
	    }
    
	    ck.ckid=mmioStringToFOURCC(aINFO[pI->index].pFOURCC,0);
	    ck.cksize=lstrlen(lpstr)+1;
	    ck.fccType=0;
	    if (mmioCreateChunk(hmmioDst, &ck, 0))
		goto riff_SI_Error;

	    if (mmioWrite(hmmioDst, lpstr, ck.cksize) != (LONG)(ck.cksize))
		goto riff_SI_Error;

	    if (mmioAscend(hmmioDst, &ck, 0))
		goto riff_SI_Error;

	}
	pI=pI->pnext;
    }
    
    if(fList)
    {
	if (mmioAscend(hmmioDst, &ckINFO, 0))
	    goto riff_SI_Error;
    }

    return RIFFERR_NOERROR;
    
riff_SI_Error:    
    return lr;
    
}


///////////////////////////////////////////////////////////////////////////////

LRESULT RIFFAPI riffReadDISP(HMMIO hmmio, LPMMCKINFO lpck, DISP FAR * FAR * lpnpDisp)
{
    LRESULT     lr;
    lr    = RIFFERR_ERROR;
   
    return lr;
}

LRESULT RIFFAPI riffFreeDISP(DISP FAR * FAR * lpnpDisp)
{
    LRESULT     lr;
    lr    = RIFFERR_ERROR;
    
    return lr;
}

LRESULT RIFFAPI riffWriteDISP(HMMIO hmmio, DISP FAR * FAR * lpnpDisp)
{
    LRESULT     lr;
    lr    = RIFFERR_ERROR;
    
    return lr;
}


/** EOF: RIFFsup.c **/
