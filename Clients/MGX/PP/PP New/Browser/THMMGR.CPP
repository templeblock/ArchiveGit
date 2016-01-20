// (c) Copyright 1992 MICROGRAFX, Inc., All Rights Reserved. 
// This material is confidential and a trade secret.
// Permission to use this work for any purpose must be obtained
// in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081
			  
#include "pp.h"
#include <time.h>
#include "thmmgr.h"
#include "browser.h"	// for validation levels
#include "bsrutil.h"	// for SafeStat
#include "tiffio.h"
#include "id.h"
#include "oldthm.h"
#include "pcdui.h"

static int filesOpen = 0;		

extern LPBSRSTATE lpBsr;	// for 	bDisableCancel
extern bImported;

//***************************Static Protoypes***************************
LOCAL BOOL  THM_write_info(LPTHMINFO lpInfo, FILE_HANDLE hfile);
LOCAL FILE_HANDLE THM_read_info(LPTHMNAIL lpThm, BOOL bWrite);
LOCAL BOOL  THM_delete_frame(LPTHMINFO lpInfo, FILE_HANDLE hfile);
LOCAL BOOL  THM_delete_descr(LPTHMINFO lpInfo, FILE_HANDLE hfile);
LOCAL BOOL  THM_update_easy_info(LPTSTR lpOrgFile, LPTHMINFO lpInfo);
LOCAL LPFRAME THM_make_preview(LPFRAME lpFrame);
LOCAL LPFRAME THM_put_preview_frame(LPTHMINFO lpInfo, LPFRAME lpFrame, FILE_HANDLE hfile);
LOCAL LPFRAME THM_new_thumbnail(LPTSTR lpOrgFile, LPTHMINFO lpInfo);
LOCAL LONG THM_save_thumb(FILE_HANDLE hfile, LPFRAME lpFrame, WORD wCompression);
LOCAL BOOL THM_load_thumb(FILE_HANDLE hfile, LPFRAME lpFrame, WORD wCompression);

void testthmmgr(void)
{
	if (filesOpen == 0)
	 	return;
	MessageBeep(-1);
	MessageBeep(-1);
	MessageBeep(-1);
}

LOCAL FILE_HANDLE lOpen(LPTSTR lptr, int i)
{
	FILE_HANDLE ret;
	UINT errmode;
	
	errmode = SetErrorMode(SEM_FAILCRITICALERRORS);
	ret = FileOpen(lptr, i);
	SetErrorMode(errmode);
	if (ret != FILE_HANDLE_INVALID)
		filesOpen++;
	return(ret);
}

LOCAL BOOL lClose(FILE_HANDLE hf)
{
	if (FileClose(hf))
	{
		filesOpen--;
		return(TRUE);
	}
	return(FALSE);
}

LOCAL FILE_HANDLE lCreat(LPTSTR lptr, int i)
{
	FILE_HANDLE ret;
	ret = FileOpen(lptr, FO_CREATE|i);
	if (ret != FILE_HANDLE_INVALID)
		filesOpen++;
	return(ret);
}

//**************************************************************************
// 	Fills and returns a THMNAIL structure for the thumbnail file specified by lpEntry.
// 	Returns NULL for failure.
//	Does NOT fill description or frame part of THMNAIL.
// 	Call THMClose when done with the thumbnail. 
//**************************************************************************
LPTHMNAIL	THMOpen( LPTHMNAIL lpThm, LPENTRY lpEntry)
//**************************************************************************
{
	FILE_HANDLE hfile;
	
	if (!lpEntry)
		return(NULL);
	if (lpEntry->Thumb.szFile[0] == _T('\0'))
		return(NULL);
	// fill in structure
	lpThm->Thumb = lpEntry->Thumb;
	lpThm->frame = NULL;
	lpThm->bDescr = NO;
	lpThm->szDescr[0] = _T('\0');
	lpThm->bSpecial = FALSE;
	
   	// get info (does volume check)
   	hfile = THM_read_info(lpThm, NO);
	if (hfile == FILE_HANDLE_INVALID)
		return(NULL);
	lClose(hfile); 
	return(lpThm);
}

//**************************************************************************
//	Closes a thumnail.
//	Destroys frame if any.
//**************************************************************************
void		THMClose(LPTHMNAIL lpThm)
//**************************************************************************
{
	if (lpThm->frame)
		FrameClose(lpThm->frame);
}

//**************************************************************************
// 	Creates a thumbnail file for the file in lpEntry.  
//	If !bLocal then it is created on the same media as the file (if possible).
//	Returns a THMNAIL structure for the new thumbnail or NULL for failure.
//	Fills in lpEntry->Thumb with the new name and volume.
//	Does NOT fill in the file with relevent data (use THMReCreateInfo/Frame)
// 	Call THMClose when done with the thumbnail. 
//**************************************************************************
LPTHMNAIL	THMCreate( LPTHMNAIL lpThm, LPENTRY lpEntry, BOOL bLocal)
//**************************************************************************
{
	FILE_HANDLE hfile;
	FNAME szName;
	BOOL result;
	VOLUMEID volume;
		
	if (!lpEntry)
		return(NULL);
	
	// get new file name
	if (bLocal)
	{
   		lstrcpy(szName, Browser.ThumbnailDir);
		GetDriveVolume(szName, &volume, NO);
	}
	else
	{
		lstrcpy(szName, lpEntry->File.szFile);
		stripdir(szName);
   		lstrcat(szName, Browser.DriveThumbnailDir);
		if (!VolumeCheck(&lpEntry->File, &volume))
	   	{	
	   		// dont put the thumb on a different remote volume from 
			//	the original file.
   			lstrcpy(szName, Browser.ThumbnailDir);
			GetDriveVolume(szName, &volume, NO);
		}
	}
	
	if (!GetTempFilename(szName, THUMBNAIL_EXT))
		goto ExitErr; 	
	
   	//	fill in info to write
	lpThm->info.fileXSize = lpThm->info.fileYSize = 0;
	lpThm->info.fileSize = 0;
	lpThm->info.fileDepth = 1;
	lpThm->info.fileRes = 1;
	lpThm->info.dataType = -1;
    lpThm->info.fileType = -1;
	lpThm->info.ctime = 0L;
	lpThm->info.time = 0L;
	lpThm->info.thmtime = 0L;
	lpThm->info.lThumb = 0L;
	lpThm->info.lDescr = 0L;
	lpThm->info.lFree = THMINFOSIZE;	
	lpThm->info.lFlags = THMINFO_50;
	lpThm->info.wCompress = THM_NOCOMPRESS;
	lpThm->info.thmSize = 0L;
	
	// create and open file
	hfile = lCreat(szName, FO_READ|FO_WRITE);
	if (hfile == FILE_HANDLE_INVALID)
		goto ExitErr; 	

   	result = THM_write_info(&(lpThm->info), hfile);
	lClose(hfile);
	if (!result)
		goto ExitErr;
	// fill in structure
	lpEntry->Thumb.volume = volume;
	lstrcpy(lpEntry->Thumb.szFile, szName);
	lpThm->Thumb = lpEntry->Thumb;
	lpThm->frame = NULL;
	lpThm->bDescr = YES;
	lpThm->szDescr[0] = _T('\0');
	lpThm->bSpecial = FALSE;
	
	return(lpThm);
	
ExitErr:
//	result = errno;
	return(NULL);
}


//**************************************************************************
//	Returns the frame associated with the thumbnail.
//	Returns NULL if there is no frame or an error.
// 	Loads frame from thumb if needed.
//**************************************************************************
LPFRAME		THMGetFrame(LPTHMNAIL lpThm)
//**************************************************************************
{
	LPFRAME lpFrame;
	LPTHMINFO info;
	int	 xsize, ysize, depth;
	FILE_HANDLE hfile;
	
	if (lpThm->frame)
		return (lpThm->frame);
	if (lpThm->bSpecial)
		return(NULL);
	info = &(lpThm->info);
	// is there a thumb?
	if (!info->lThumb)
		return(NULL);
	depth = info->thmDepth;
	xsize = info->thmXSize;
	ysize = info->thmYSize;
	// new frame
	lpFrame = FrameOpen((FRMDATATYPE)depth, xsize, ysize, 1); 
	if (!lpFrame)	
		return(NULL);
	
	// open file
	if(!VolumeCheck(&lpThm->Thumb, NULL))
		return(NULL); 
	hfile = lOpen(lpThm->Thumb.szFile, FO_READ);
	if (hfile == FILE_HANDLE_INVALID)
		goto ExitErr;
	if (FileSeek(hfile, info->lThumb, 0/*begining*/) != info->lThumb)
		goto ExitErr;
		
	// load thumb
	if (!THM_load_thumb(hfile, lpFrame, info->wCompress))
		goto ExitErr;

	lClose(hfile);
	// install frame
	lpThm->frame = lpFrame;
	return (lpFrame);
	
ExitErr:
	if (lpFrame)
		FrameClose(lpFrame);
	if (hfile != FILE_HANDLE_INVALID)
		lClose(hfile);
	return(NULL);			
}

//**************************************************************************
//	Turns the given frame into a thumbnail and writes it to the given file
//	Updates the info to reflect the new frame, data, and file type.
//	width&height are size of ORIGINAL frame (not lpFrame)
//	Returns TRUE if successful.
// 	Re-reads thumbnail info.
// 	if lpFrame == NULL then deletes the thumb from the thumb file.
// 	sets timestamp to be time
//	this routine should be followed or proceeded by a call to THMReCreateInfo, 
//	to update file size, time etc.
//**************************************************************************
BOOL		THMPutFrame(LPTHMNAIL lpThm, LPFRAME lpFrame, int width, int height, LONG dataType, LONG fileType, LONG time)
//**************************************************************************
{
	FILE_HANDLE hfile;
	LPFRAME	lpfThm;
	FRMDATATYPE type;
	LPFRAME lpNewFrame;
	DITHER_TYPE DitherType = DT_DEFAULT;
	
	if (lpThm->bSpecial)
		return(FALSE);
	// uninstall old frame
	if (lpThm->frame)
	{
		FrameClose(lpThm->frame);
		lpThm->frame = NULL;
	}
	// get up to date info
 	hfile = THM_read_info(lpThm, YES);
	if (hfile == FILE_HANDLE_INVALID)
		return(FALSE);
	if (dataType == IDC_SAVESP)
	{
		dataType = IDC_SAVELA;
		DitherType = DT_SCATTER;
	}
	lpThm->info.thmtime = time;
	lpThm->info.dataType = dataType - IDC_FIRSTDATATYPE;

    if( bImported )
    	lpThm->info.fileType = fileType - IDN_FIRSTIMPORTFILETYPE;
    else
    	lpThm->info.fileType = fileType - IDN_FIRSTFILETYPE;

	lpThm->info.fileXSize = width;
	lpThm->info.fileYSize = height;
	lpThm->info.fileDepth = lpFrame->Depth;  
	lpThm->info.fileRes = lpFrame->Resolution;
	// make thumbnail preview
	if (!(lpfThm = THM_make_preview(lpFrame)))
		goto ExitFalse;
	// Convert frame
	type = (FRMDATATYPE)-1;
	switch(FrameType(lpFrame))
	{
		case FDT_LINEART:
			// These not need to be converted
		break;
		case FDT_GRAYSCALE: 
			if (dataType == IDC_SAVELA)
				type = FDT_LINEART;
		break;
		case FDT_PALETTECOLOR: 
			if (dataType == IDC_SAVELA)
				type = FDT_LINEART;
			else if (dataType == IDC_SAVECT)
				type = FDT_GRAYSCALE;
			else
				type = FDT_RGBCOLOR;
		break;
		case FDT_RGBCOLOR:     
			if (dataType == IDC_SAVELA)
				type = FDT_LINEART;
			else if (dataType == IDC_SAVECT)
				type = FDT_GRAYSCALE;
		break;
		case FDT_CMYKCOLOR:
			if (dataType == IDC_SAVELA)
				type = FDT_LINEART;
			else if (dataType == IDC_SAVECT)
				type = FDT_GRAYSCALE;
			else
				type = FDT_RGBCOLOR;
		break;
	}
	if (type >= 0)
	{
		lpNewFrame = FrameConvert(lpfThm, type,NULL, DitherType);
		if (lpNewFrame)
		{
			FrameClose(lpfThm);
			lpfThm = lpNewFrame;
		}
	}
	// write thm preview, and new info
	lpThm->frame = THM_put_preview_frame(&(lpThm->info), lpfThm, hfile);
	lClose(hfile);
	return (TRUE);

ExitFalse:	
	if (hfile != FILE_HANDLE_INVALID)
		lClose(hfile);
	return(FALSE);
}
				
//**************************************************************************
//	Deletes the frame from the thumb file and from memory.
// 	Updates info from thm file.
//**************************************************************************
BOOL		THMDeleteFrame(LPTHMNAIL lpThm)
//**************************************************************************
{
	FILE_HANDLE hfile;
	
	if (lpThm->bSpecial)
		return(FALSE);
	// get up to date info
 	hfile = THM_read_info(lpThm, YES);
	if (hfile == FILE_HANDLE_INVALID)
		return(FALSE);
	if (!THM_delete_frame(&lpThm->info, hfile))
	{
		lClose(hfile); 
		return(FALSE);
	}
	if (lpThm->frame)
	{
		FrameClose(lpThm->frame);
		lpThm->frame = NULL;
	}
   	lClose(hfile); 
	return(TRUE);
}

//**************************************************************************
//	Returns the description associated with the thumbnail.
//	Returns "" if there is no description or an error.
// 	Loads description from thumb if needed.
//	Sets the bDescr field of lpThm.
//**************************************************************************
LPDESC		THMGetDescription(LPTHMNAIL lpThm)
//**************************************************************************
{
	LPTHMINFO info;
	FILE_HANDLE hfile;
	UINT size;
	
	info = &(lpThm->info);

	// is it already loaded?
	if (lpThm->bDescr)
		return (&(lpThm->szDescr));
	// default to none
	lpThm->bDescr = TRUE;
	lpThm->szDescr[0] = _T('\0');
	// can we even try?
	if (lpThm->bSpecial)
		return(NULL);
	// is there a description?
	if (!info->lDescr)
		return(&(lpThm->szDescr));
	// open file
	if(!VolumeCheck(&lpThm->Thumb, NULL))
		return(&(lpThm->szDescr)); 
	hfile = lOpen(lpThm->Thumb.szFile, FO_READ);
	if (hfile == FILE_HANDLE_INVALID)
		return(&(lpThm->szDescr));
	if (FileSeek(hfile, info->lDescr, 0/*begining*/) != info->lDescr)
		goto ExitErr;
	// load description
	size = Min(info->descrLen, sizeof(DESC));
	if (FileRead(hfile, lpThm->szDescr ,size) != size)
		goto ExitErr;
	lClose(hfile);
	// install frame
	return (&(lpThm->szDescr));
	
ExitErr:
	lpThm->szDescr[0] = _T('\0');
	if (hfile != FILE_HANDLE_INVALID)
		lClose(hfile);
	return(&(lpThm->szDescr));	
}

//**************************************************************************
//	Writes the given description into the given thumbnail file.
//	Copies it into lpThm->szDescr
//	Returns TRUE if successful.
// 	Re-reads thumbnail info.
// 	if lpDescription == NULL then deletes the description from the thumb file.
//**************************************************************************
BOOL		THMPutDescription(LPTHMNAIL lpThm, LPDESC lpDescription)
//**************************************************************************
{
	FILE_HANDLE hfile;
	LPTHMINFO info;
	long lDescr;
	WORD size;
	
	if (lpThm->bSpecial)
		return(FALSE);
	// intall new description in memory
	lpThm->bDescr = TRUE;
	if (lpDescription)
		lstrcpy(lpThm->szDescr, *lpDescription);
	else
		lpThm->szDescr[0] = _T('\0');
	// get up to date info
 	hfile = THM_read_info(lpThm, YES);
	if (hfile == FILE_HANDLE_INVALID)
		return(FALSE);
	info = &(lpThm->info);
	// is there a new description??
	if (!lpDescription)
	{
		THM_delete_descr(info, hfile);
		lClose(hfile);
		return(TRUE);
	}
	// decide where to put new description
	if (info->lDescr > info->lThumb)
		// write over old descr	
		lDescr = info->lDescr;
	else if (info->lThumb) 
	{
		// put after thumb
		if (!THM_delete_descr(info, hfile))
			goto ExitFalse;
		lDescr = info->lThumb+info->thmSize;
	}
	else 
		// put after info
		lDescr = info->lFree;
	
	// write description		
	if (FileSeek(hfile, lDescr, 0/*begining*/) != lDescr)
		goto ExitFalse;
	size = Min(lstrlen(lpThm->szDescr)+1, sizeof(DESC));
	if (FileWrite(hfile, lpThm->szDescr, size ) != size)
		goto ExitFalse;
	// write new info
	info->lDescr = lDescr;
	info->descrLen = size;
	if (!THM_write_info(info, hfile))
	{
		info->lDescr = 0L;
		goto ExitFalse;
	}
	lClose(hfile);
	return (TRUE);

ExitFalse:	
	if (hfile != FILE_HANDLE_INVALID)
		lClose(hfile);
	return(FALSE);
}

//**************************************************************************
//	Deletes the description from the thumb file and from memory.
// 	Updates info from thm file.
//	Returns true is successful.
//**************************************************************************
BOOL		THMDeleteDescription(LPTHMNAIL lpThm)
//**************************************************************************
{
	FILE_HANDLE hfile;
	
	if (lpThm->bSpecial)
		return(FALSE);
	// get up to date info
 	hfile = THM_read_info(lpThm, YES);
	if (hfile == FILE_HANDLE_INVALID)
		return(FALSE);
	if (!THM_delete_descr(&(lpThm->info), hfile))
		return(FALSE);
	lpThm->szDescr[0] = _T('\0');
	lpThm->bDescr = TRUE;
	return(TRUE);
}

//**************************************************************************
//	Fills the lpThm->info from the thumbnail.
//	Returns the info or NULL for failure. 
// 	Does not do any validation on info.
//**************************************************************************
LPTHMINFO	THMGetInfo(LPTHMNAIL lpThm)
//**************************************************************************
{
	FILE_HANDLE hfile;
	
	if (lpThm->bSpecial)
		return(NULL);
 	hfile =  THM_read_info(lpThm, FALSE);
 	if (hfile == FILE_HANDLE_INVALID)
		return(NULL);
	lClose(hfile);
	return(&(lpThm->info));
}

//**************************************************************************
//	Updates the thumbnail info based on the file given.
//	Only Updates minimal "EASY INFO"
//	Only updates if the file is newer than the thumb OR bAlways==TRUE.	
//	Returns TRUE if sucessful.
//	Failure may occur if the original file is on 
//		unavailable, removable media.
//**************************************************************************
BOOL 		THMReCreateEasyInfo(LPTHMNAIL lpThm, LPFILELOC lpFile, BOOL bAlways)
//**************************************************************************
{
	FILE_HANDLE	hThm;
	ULONG lTime;
	FNAME szFile;
	
	hThm = FILE_HANDLE_INVALID;

	if (lpThm->bSpecial)
		return(FALSE);
	
	// be sure we can access the original file
	if(!VolumeCheck(lpFile, NULL))
		goto ExitFalse; 
		
	// open original file
	if (!bAlways)
	{
		// check file time
		lstrcpy(szFile, lpFile->szFile);
		if (SafeFileStats(szFile, NULL,NULL, &lTime,NULL))
			// is thumb newer?
			if (lTime <= lpThm->info.time)
				goto ExitTrue;
	}
	// get most up to date info on thumb						   
	hThm = THM_read_info(lpThm, YES);
	if (hThm == FILE_HANDLE_INVALID)
		goto ExitFalse;
	// update thumb info which doesn't require reading whole file
	if (!THM_update_easy_info(lpFile->szFile, &(lpThm->info)))
		goto ExitFalse;
	// write info
	if (!THM_write_info(&(lpThm->info), hThm))
		goto ExitFalse;
	// success 
ExitTrue:
	if (hThm != FILE_HANDLE_INVALID)
		lClose(hThm);
	return(TRUE);

ExitFalse:
	if (hThm != FILE_HANDLE_INVALID)
		lClose(hThm);
	return(FALSE);
}

//**************************************************************************
//	Updates the thumbnail preview based on the file given.
//	All other thumb info is updated in the process.
//	Only updates if the file is newer than the thumb OR bAlways==TRUE.	
//	Returns TRUE if completely sucessful.
//	Returns FALSE if none or part of the info was updated.
//	Failure may occur if the original file is on 
//		unavailable, removable media.
//**************************************************************************
BOOL 		THMReCreateFrame(LPTHMNAIL lpThm, LPFILELOC lpFile, BOOL bAlways)
//**************************************************************************
{
	FILE_HANDLE hThm;
	ULONG lTime;
	LPFRAME lpfThm;
	FNAME szFile;
	
	hThm = FILE_HANDLE_INVALID;

	if (lpThm->bSpecial)
		return(FALSE);

	// be sure we can access the original file
	if(!VolumeCheck(lpFile, NULL))
		goto ExitFalse; 

	// open original file
	if (!bAlways)
	{
		// check file time
		lstrcpy(szFile, lpFile->szFile);
		if (SafeFileStats(szFile, NULL,NULL, &lTime,NULL))
			// (no error) is thumb newer?
			if (lTime <= lpThm->info.time)
				goto ExitTrue;
	}
	// get most up to date info on thumb
	hThm = THM_read_info(lpThm, YES);
	if (hThm == FILE_HANDLE_INVALID)
		goto ExitFalse;
	// get new thumbnail and update info
	lpfThm = THM_new_thumbnail(lpFile->szFile, &(lpThm->info));
	if (lpfThm) // got new thumbnail and new info
	{
		// must write info first
		if (!THM_write_info(&(lpThm->info), hThm))
			goto ExitFalse;
		// write thumbnail
		if (lpThm->frame)
			FrameClose(lpThm->frame);
		lpThm->frame = THM_put_preview_frame(&(lpThm->info), lpfThm, hThm);
		if (!lpThm->frame)
		{
			FrameClose(lpfThm);
			goto ExitFalse;
		}
		lClose(hThm); hThm = FILE_HANDLE_INVALID;
	}
	else  // could not create thumb, then at least get easy info and exit FALSE
	{
		if (!THM_update_easy_info(lpFile->szFile, &(lpThm->info)))
			goto ExitFalse;
		THM_write_info(&(lpThm->info), hThm);
		goto ExitFalse;
	}
ExitTrue:
	if (hThm != FILE_HANDLE_INVALID)
		lClose(hThm);
	return(TRUE);

ExitFalse:
	if (hThm != FILE_HANDLE_INVALID)
		lClose(hThm);
	return(FALSE);
}

//**************************************************************************
//	Copies all available information from one thumbnail to another.
//	The dest thumb  preview will be given a timestamp of time.
//**************************************************************************
BOOL 		THMCopyThumb(LPTHMNAIL lpDstThm, LPTHMNAIL lpSrcThm, ULONG time)
//**************************************************************************
{
	LPFRAME lpSrcFrame, lpDstFrame;
	LPDESC	lpSrcDesc;
	FILE_HANDLE 	hfile;

	if (lpSrcThm->bSpecial)
		return(FALSE);
	if (lpDstThm->bSpecial)
		return(FALSE);

	lpSrcFrame = THMGetFrame(lpSrcThm);
	lpSrcDesc = THMGetDescription(lpSrcThm);	
	// copy description
	if (lpSrcDesc[0][0] != _T('\0'))
		THMPutDescription(lpDstThm, lpSrcDesc);
	hfile = THM_read_info(lpDstThm, YES);
	if (hfile == FILE_HANDLE_INVALID)
		return(FALSE);
	// copy preview
	if (lpSrcFrame)
		if (lpDstFrame = FrameCopy(lpSrcFrame, NULL))
			lpDstThm->frame = THM_put_preview_frame(&(lpDstThm->info), lpDstFrame, hfile);
	// copy basic info
	lpDstThm->info.fileXSize = 	lpSrcThm->info.fileXSize;
	lpDstThm->info.fileYSize = 	lpSrcThm->info.fileYSize;
	lpDstThm->info.fileSize = 	lpSrcThm->info.fileSize;
	lpDstThm->info.fileRes = 	lpSrcThm->info.fileRes;
	lpDstThm->info.fileDepth = 	lpSrcThm->info.fileDepth;
	lpDstThm->info.dataType = 	lpSrcThm->info.dataType;
	lpDstThm->info.fileType = 	lpSrcThm->info.fileType;
	lpDstThm->info.ctime = 		lpSrcThm->info.ctime;
	lpDstThm->info.time = 		lpSrcThm->info.time;
	lpDstThm->info.thmtime = time;
	if (!THM_write_info(&lpDstThm->info, hfile))
	{
		lClose(hfile);
		return(FALSE);
	}
	lClose(hfile);
	return(TRUE);
}

//**************************************************************************
//	Performs the appropriate validation based on level.
//	level is a set of validation flags (see bsrtypes.h)
//	Handles: VL_CLEAN, VL_DATAONLY, VL_NOTIME, VL_ESCAPABLE
//	This may include updateing the info and thumbnail preview.
//	returns TRUE if file needs updating. 
//	returns FALSE for failure or file doesnt need updating.
//	returns -1 if THUMB SHOULD BE DELETED!!!.
//	DOES NOT CREATE OR DELETE ANY THUMBNAIL.
//	Failure may occur if the original file is 
//		unavailable, or on removable media.
//**************************************************************************
BOOL		THMValidate(LPTHMNAIL lpThm, LPFILELOC lpFile, WORD level)
//**************************************************************************
{

	UINT type;
	ULONG lTime;
	FNAME szFile;
	VOLUMEID volume;
	DWORD dwError;
	
	if (lpThm->bSpecial)
		return(FALSE);
		
	if (level == VL_NONE)
		return(TRUE);
		
	if ((level & VL_ESCAPABLE) && SAFECANCEL)
		return(FALSE);
	
	// be sure we can access the original file
	if(!VolumeCheck(lpFile, NULL))
		return(FALSE); 
	
	// check file date
	lstrcpy(szFile, lpFile->szFile);
	if (!SafeFileStats(szFile, NULL,NULL, &lTime,NULL))
	{		
		if (level & VL_CLEAN)
		{
			// unchangable MEDIA??
			type = BSRGetDriveType(lpFile->szFile[0]);
			if (type == DRIVE_CDROM || type == 0)
				return(FALSE);
			// check volume with strog match needed
			if (!VolumeCheck(lpFile,&volume))
				return(FALSE);
			// was it a strong match
			if (!StrongEqualVolume(lpFile->szFile[0], &lpFile->volume, &volume))
				return(FALSE);
			// does file exist?
			if (FileExists(lpFile->szFile))
				return(FALSE); 	// just couldn't open it before.
			// why did it fail?? 
			dwError = GetLastError();
			if (dwError == ERROR_BAD_PATHNAME||
				dwError == ERROR_FILE_NOT_FOUND ||
				type == DRIVE_RAMDISK)
			{
				// the file is actually gone
				return (-1);
			}
			return(FALSE);
		}
		return(FALSE);
	}	// endif error getting stats
	// is info out of date 
	if (lTime > lpThm->info.time || (level & VL_NOTIME))
		goto ExitUpdate;
	// does thumb want updating?? (and is it out of date)
	if (!(level & VL_DATAONLY))
		if(lTime > lpThm->info.thmtime || (level & VL_NOTIME))
			goto ExitUpdate;
	return(FALSE);

ExitUpdate:
	if (level & VL_DATAONLY)
	{
		if (!THMReCreateEasyInfo(lpThm, lpFile, YES))
			return(FALSE);
	}
	else if (level != VL_CHECK)
		if (!THMReCreateFrame(lpThm, lpFile, YES))
			return(FALSE);
	return(TRUE);
}

//**************************************************************************
//	Reads a photo_cd thumbnail for the given file.
//	Returns the thumbnail or null.
//	Note: this thumbnail can not be modified
//**************************************************************************
LPTHMNAIL 	ReadPCDThumb(LPTHMNAIL lpThm, LPTSTR lpFileName)
//**************************************************************************
{
	LPFRAME lpFrame;
	CReadImage read(lpFileName);
	
	lpFrame = read.ReadPCDThumbnail();
	if (!lpFrame)
		return(NULL);
	lpThm->info.dataType = IDC_SAVE24BITCOLOR-IDC_FIRSTDATATYPE;

   	//	fill in info to write
	lpThm->info.fileXSize = lpThm->info.fileYSize = 0;
	lpThm->info.fileSize = 0;
	lpThm->info.fileDepth = 1;
	lpThm->info.fileRes = 1;
	lpThm->info.fileType = IDN_PHOTOCD - IDN_FIRSTFILETYPE;
	lpThm->info.ctime = 0L;
	lpThm->info.time = 0L;
	lpThm->info.thmtime = 0L;
	lpThm->info.lThumb = 0L;
	lpThm->info.lDescr = 0L;
	lpThm->info.lFree = 0L;	
	lpThm->info.lFlags = THMINFO_50;
	lpThm->info.wCompress = THM_NOCOMPRESS;
	lpThm->info.thmSize = 0L;
	lpThm->Thumb.szFile[0] = _T('\0');
	lpThm->frame = lpFrame;
	lpThm->bDescr = YES;
	lpThm->szDescr[0] = _T('\0');
	lpThm->bSpecial = TRUE;
	return(lpThm);
}

//**************************************************************************
//	Reads the thumb from a 3.1 thumbnail (if available) for the file.
//	lpThumbDir is where 3.1 dbf's and thumbs are.
//**************************************************************************
LPTHMNAIL	ReadOldThumb(LPTHMNAIL lpThm, LPTSTR lpFileName, LPTSTR lpThumbDir)
//**************************************************************************
{
	if (OldImageDBGetThumb(lpFileName, lpThumbDir, lpThm, YES))
	{
		lpThm->bSpecial = TRUE;
		return(lpThm);
	}
	SAFECANCEL;
	return(NULL);
}

//**************************************************************************
//************************STATIC ROUTINES******************************************
//**************************************************************************

//**************************************************************************
//	Writes all info, including lThumb, lDescr, etc.	This can be dangerous. 
//	Be sure file was opened for writing.
//	Returns TRUE if successful.
//**************************************************************************
LOCAL BOOL  THM_write_info(LPTHMINFO lpInfo, FILE_HANDLE hfile)
//**************************************************************************
{
	LPTSTR lpBuf;
	long bytes;
	WORD Data[THMINFOSIZE/sizeof(WORD)+1];
	LPWORD shtptr;
	int delta;
	
	shtptr = &Data[0];

	// see if we need to convert a pp40 thumbnail
	if (lpInfo->lFlags & THMINFO_40)
	{
		// make room for new data
		delta = THMINFOSIZE - lpInfo->lFree;
		// get file length
		bytes = FileSeek(hfile, 0, FILE_END);
		if (bytes < lpInfo->lFree)
			return(FALSE);
		bytes -= lpInfo->lFree;
		if (delta > 0 && bytes != 0)
		{
			// move evrything by delta
			lpBuf = (LPTSTR)LineBuffer[0];
			if (FileSeek(hfile, lpInfo->lFree, 0) != lpInfo->lFree)
				return(FALSE);
			if (FileRead(hfile, lpBuf, bytes) != bytes)
				return(FALSE);
			if (FileSeek(hfile, THMINFOSIZE, 0) != THMINFOSIZE)
				return(FALSE);
			if (FileWrite(hfile, lpBuf, bytes) != bytes)
				return(FALSE);
			lpInfo->lFree =	THMINFOSIZE; 
			lpInfo->lThumb += delta;
			lpInfo->lDescr += delta;
			// ERROR- if we later fail this could be bad!
		}
	}

	if (FileSeek(hfile, 0, 0/*begining*/) == FILE_DWERR)
		return(FALSE);
		
	// mark file as 50 BETA
	lpInfo->lFlags = (0xffff0000 | THMINFO_50);
	
	// Write data
	PUTLONG(lpInfo->lFlags, shtptr);		// this is out of order in the file
	PUTLONG(lpInfo->fileYSize, shtptr);
	PUTLONG(lpInfo->fileSize, shtptr);
	PUTLONG(lpInfo->fileRes, shtptr);
	PUTLONG(lpInfo->fileDepth, shtptr);
	PUTLONG(lpInfo->dataType, shtptr);
	PUTLONG(lpInfo->fileType, shtptr);
	PUTLONG(lpInfo->ctime, shtptr);
	PUTLONG(lpInfo->time, shtptr);
	PUTLONG(lpInfo->thmtime, shtptr);
	PUTWORD(lpInfo->thmXSize, shtptr);
	PUTWORD(lpInfo->thmYSize, shtptr);
	PUTWORD(lpInfo->thmDepth, shtptr);
	PUTLONG(lpInfo->lFree, shtptr);
	PUTLONG(lpInfo->lThumb, shtptr);
	PUTLONG(lpInfo->lDescr, shtptr);
	PUTWORD(lpInfo->descrLen, shtptr);
	PUTLONG(lpInfo->fileXSize, shtptr);		// this is out of order in the file
	PUTWORD(lpInfo->wCompress, shtptr);
	PUTLONG(lpInfo->thmSize, shtptr);
	
	bytes = FileWrite(hfile, &Data[0], THMINFOSIZE);
	if (bytes != THMINFOSIZE)
		return(FALSE);
	return(TRUE);
}

//**************************************************************************
//	Fills info header from thumbnail file.
//	Uses lpThm->szName and lpThm->volume olny.
//	Returns hfile if successful.
//**************************************************************************
LOCAL FILE_HANDLE THM_read_info(LPTHMNAIL lpThm, BOOL bWrite)
//**************************************************************************
{
	FILE_HANDLE hfile = FILE_HANDLE_INVALID;
	LONG bytes;
	LONG lFlags;
	WORD Data[THMINFOSIZE/sizeof(WORD)+1];
	WORD wHi, wLo;
	LPWORD shtptr = &Data[0];
	LPTHMINFO lpInfo = &lpThm->info;
	
	if(!VolumeCheck(&lpThm->Thumb, NULL))
		goto ExitError;
	hfile = lOpen(lpThm->Thumb.szFile, bWrite ? (FO_READ|FO_WRITE) : FO_READ);
	if (hfile == FILE_HANDLE_INVALID)
		goto ExitError;
	if (FileSeek(hfile, 0, 0/*begining*/) == FILE_DWERR)
		goto ExitError;
	// get data
	bytes = FileRead(hfile, shtptr, THMINFOSIZE);
	if (bytes < sizeof(LONG))
		goto ExitError;
	
	// check compatibilty
	GETLONG(lFlags, shtptr)
	// is old 4.0? (high order byte clear)
	if ((0xffff0000 & lFlags) != 0xffff0000)
		goto PP40;
	// is it compatable?
	if (!(lFlags & THMINFO_50))
		goto ExitError;
	if (bytes != THMINFOSIZE)
		goto ExitError;
	lpInfo->lFlags = lFlags;
	// read in data
	// already read first long--GETLONG(lpInf->fileXSize, shtptr)
	GETLONG(lpInfo->fileYSize, shtptr);
	GETLONG(lpInfo->fileSize, shtptr);
	GETLONG(lpInfo->fileRes, shtptr);
	GETLONG(lpInfo->fileDepth, shtptr);
	GETLONG(lpInfo->dataType, shtptr);
	GETLONG(lpInfo->fileType, shtptr);
	GETLONG(lpInfo->ctime, shtptr);
	GETLONG(lpInfo->time, shtptr);
	GETLONG(lpInfo->thmtime, shtptr);
	GETWORD(lpInfo->thmXSize, shtptr);
	GETWORD(lpInfo->thmYSize, shtptr);
	GETWORD(lpInfo->thmDepth, shtptr);
	GETLONG(lpInfo->lFree, shtptr);
	GETLONG(lpInfo->lThumb, shtptr);
	GETLONG(lpInfo->lDescr, shtptr);
	GETWORD(lpInfo->descrLen, shtptr);
	GETLONG(lpInfo->fileXSize, shtptr); 		// not in order
	GETWORD(lpInfo->wCompress, shtptr);
	GETLONG(lpInfo->thmSize, shtptr);
	return(hfile);

ExitError:
	if (hfile != FILE_HANDLE_INVALID)
		lClose(hfile);
	return(FILE_HANDLE_INVALID);
	
PP40:
	// is it compatable?
	if (bytes < 60)
		goto ExitError;
		
	lpInfo->lFlags = THMINFO_40;
	// read in pp40 data
	lpInfo->fileXSize = lFlags;
	GETLONG(lpInfo->fileYSize, shtptr);
	GETLONG(lpInfo->fileSize, shtptr);
	GETLONG(lpInfo->fileRes, shtptr);
	GETLONG(lpInfo->fileDepth, shtptr);
	GETLONG(lpInfo->dataType, shtptr);
	GETLONG(lpInfo->fileType, shtptr);
	GETLONG(lpInfo->ctime, shtptr);
	GETLONG(lpInfo->time, shtptr);
	GETLONG(lpInfo->thmtime, shtptr);
	GETWORD(lpInfo->thmXSize, shtptr);
	GETWORD(lpInfo->thmYSize, shtptr);
	GETWORD(lpInfo->thmDepth, shtptr);
	GETLONG(lpInfo->lFree, shtptr);
	GETLONG(lpInfo->lThumb, shtptr);
	GETLONG(lpInfo->lDescr, shtptr);
	GETWORD(lpInfo->descrLen, shtptr);
	// defaults
	lpInfo->wCompress = THM_NOCOMPRESS;
	lpInfo->thmSize = lpInfo->thmXSize*lpInfo->thmYSize*lpInfo->thmDepth;
	return(hfile);
}

//**************************************************************************
// Deletes frame from file.
// Info must be up to date wrt lFree, lThumb, lDescr, and descrLen.
// Any changes to other parts of Info will be written by this routine.
// Does not change THMNAIL structure.
//**************************************************************************
LOCAL BOOL  THM_delete_frame(LPTHMINFO lpInfo, FILE_HANDLE hfile)
//**************************************************************************
{
	LPTSTR lpBuf;
	LONG eof;
	
	if (!lpInfo->lThumb)
		return(TRUE);
	lpInfo->thmtime = 0L;
	if (lpInfo->lDescr > lpInfo->lThumb)
	{						
		// move description	to first available space
		lpInfo->lThumb = 0L;
		lpBuf = (LPTSTR)LineBuffer[0];
		if (FileSeek(hfile, lpInfo->lDescr, 0) != lpInfo->lDescr)
			return(FALSE);
		if (FileRead(hfile, lpBuf, lpInfo->descrLen) != lpInfo->descrLen)
			return(FALSE);
		if (FileSeek(hfile, lpInfo->lFree, 0) != lpInfo->lFree)
			return(FALSE);
		if (FileWrite(hfile, lpBuf, lpInfo->descrLen) != lpInfo->descrLen)
			return(FALSE);
		lpInfo->lDescr = lpInfo->lFree;
		eof = lpInfo->lDescr+lpInfo->descrLen;
	}
	else if (!lpInfo->lDescr)
	{
		// no description, just truncate file at end of header
		lpInfo->lThumb = 0L;
		eof = lpInfo->lFree;
	}		
	else 
	{	// description before thumb, just truncate after description
		lpInfo->lThumb = 0L;
		eof =  lpInfo->lDescr + lpInfo->descrLen;
	}
	// write info
	if (!THM_write_info(lpInfo, hfile))
		return(FALSE);
	// truncate file		
	if (FileSeek(hfile, eof, 0/*begining*/) != eof)
		return(FALSE);
	if (!FileTruncate(hfile))
		return(FALSE);
	return(TRUE);
}

//**************************************************************************
// Deletes description from file.
// Info must be up to date wrt lFree, lThumb, lDescr, and descrLen.
// Any changes to other parts of Info will be written by this routine.
// Does not change THMNAIL structure.
//**************************************************************************
LOCAL BOOL  THM_delete_descr(LPTHMINFO lpInfo, FILE_HANDLE hfile)
//**************************************************************************
{
	LPTSTR lpBuf;
	LONG eof, size;
	
	if (!lpInfo->lDescr)
		return(TRUE);
	if (lpInfo->lThumb > lpInfo->lDescr)
	{
		// move thumb to first available space
		lpInfo->lDescr = 0L;
		lpBuf = (LPTSTR)LineBuffer[0];
		size = lpInfo->thmSize;
		size = Min(size, 0xffff); 	// buffer size limitaion,  should not be problem
		if (FileSeek(hfile, lpInfo->lThumb, 0) != lpInfo->lThumb)
			return(FALSE);
		if (FileRead(hfile, lpBuf, (UINT)size) != size)
			return(FALSE);
		if (FileSeek(hfile, lpInfo->lFree, 0) != lpInfo->lFree)
			return(FALSE);
		if (FileWrite(hfile, lpBuf, (UINT)size) != size)
			return(FALSE);
		lpInfo->lThumb = lpInfo->lFree;
		eof = lpInfo->lThumb+size;
	}
	else if (!lpInfo->lThumb)
	{
		// no thumb, just truncate file at end of header
		lpInfo->lDescr = 0L;
		eof = lpInfo->lFree;
	}		
	else 
	{	// thumb before description , just truncate after thumb
		lpInfo->lDescr = 0L;
		size = lpInfo->thmSize;
		eof =  lpInfo->lThumb + size;
	}
	// write info
	if (!THM_write_info(lpInfo, hfile))
		return(FALSE);
	// truncate file		
	if (FileSeek(hfile, eof, 0/*begining*/) != eof)
		return(FALSE);
	if (!FileTruncate(hfile))
		return(FALSE);
	return(TRUE);
}

//***************************************************************************
//	Updates the parts of lpInfo which can be easily determined based on the
//		given file.	(such as file size, date etc.)
//	If the file is newer than the last info update, some additional info
//		MAY be retrieved IF the file type makes it easy (ie fileXsize etc).
// 	Only updates lpInfo in MEMORY.
//	Returns TRUE for success. 
// 	Failure may occur if file is on removable media.
//***************************************************************************
LOCAL BOOL  THM_update_easy_info(LPTSTR lpOrgFile, LPTHMINFO lpInfo)
//***************************************************************************
{
	ULONG ctime, time, size;
	FNAME szFile;
	BOOL bOutOfDate;
	
	// get stats	
	lstrcpy(szFile, lpOrgFile);
	if (!SafeFileStats(szFile, &size,&ctime,&time,NULL))
		return(FALSE); // error
	bOutOfDate = (time > lpInfo->time);
	lpInfo->ctime = ctime;
	lpInfo->time = time;
	lpInfo->fileSize = size;
	// MAY WANTTO GET MORE DATA IN FUTURE
	return(TRUE);	
}

//***************************************************************************
//	Returns a thumbnail frame (or NULL).
//	Updates all of lpInfo (MEMORY ONLY) if it is not NULL.
//***************************************************************************
LOCAL LPFRAME THM_new_thumbnail(LPTSTR lpOrgFile, LPTHMINFO lpInfo)
//***************************************************************************
{
	PREVIEWINFO pi;
	LPFRAME lpOutFrame, lpNewFrame;
	UINT32 	timer;
	UINT32   size;
	FNAME 	szFile, szTemp;
	RECT	Rect;
	BOOL 	bMessageEnable, bSuccess;
	PCDOPTIONS pcdopt;
	LPVOID  lpOptions = NULL;
	
	lpOutFrame = NULL;

	// Get file time and size
	lstrcpy(szFile, lpOrgFile);
	if (SafeFileStats(szFile, &size, NULL , &timer, NULL))
		lpInfo->fileSize = size;
	else
		time((PINT32)&timer); 


	// should we use callback to get preview?
	if (lpBsr->OpenState.lpGetPreview)
	{
		pi.RequestedWidth = DEF_THM_X;
		pi.RequestedHeight = DEF_THM_Y;
		lpOutFrame = (*lpBsr->OpenState.lpGetPreview)(lpOrgFile, lpBsr->OpenState.wExtType, &pi, &bSuccess);
		if (bSuccess)
			goto GotThumbnail;
	}
	
	// special case certain file types
	pi.idFileType = GetPPFileType(lpOrgFile);
	switch (pi.idFileType)
	{
		case IDN_AVI:
			lpOutFrame = AviReadFrame( lpOrgFile, -1/*Depth*/, &pi.idDataType, 0 );
			if (lpOutFrame)
			{
				pi.fileXSize = FrameXSize( lpOutFrame );
				pi.fileYSize = FrameYSize( lpOutFrame );
				pi.fileRes = FrameResolution( lpOutFrame );
			}
		break;
		case IDN_EPS:
		case IDN_DCS:
		case IDN_THUMBNAIL:
			return(NULL);
		break;

		case IDN_PHOTOCD:
			// be sure it is a photo cd CD.
			if (GetPCDOverview( lpOrgFile, szTemp))
			{
				CReadImage read(lpOrgFile);
				lpOutFrame = read.ReadPCDThumbnail();

				pi.idDataType = IDC_SAVE24BITCOLOR;
				if (lpOutFrame)
				{
					pi.fileXSize = 0;
					pi.fileYSize = 0;
					pi.fileRes = 0;
				}
				break;
			}
			// setup options so PCD will not prompt
			clr((LPTR)&pcdopt, sizeof(PCDOPTIONS));
			lpOptions = &pcdopt;
			// fall through to regular open
		default:
			bMessageEnable = -1;
			if (!pi.idFileType)
			{
				bMessageEnable = MessageEnable(FALSE);

                if( bImported )
    				pi.idFileType = IDN_AI;
                else
                    pi.idFileType = IDN_TIFF;
			}
			// get thumbnail directly
			CReadImage Read(lpOrgFile, pi.idFileType);
			Read.SetOptions(lpOptions);
			Rect.left = Rect.top =  0;
			Rect.right = DEF_THM_X-1;
			Rect.bottom = DEF_THM_Y-1;
			Read.SetSampling(&Rect);
			lpOutFrame = Read.ReadFrame();

			if (lpOutFrame)
			{
				pi.idFileType = Read.GetFileType();
				pi.idDataType = Read.GetDataType();
				pi.fileXSize = Read.GetFileWidth();
				pi.fileYSize = Read.GetFileHeight();
				pi.fileRes = Read.GetFileResolution();
			}
				
			if (bMessageEnable >= 0)
				MessageEnable(bMessageEnable);
		break;
	}
	SAFECANCEL; 	// In case any of these were canceled out of

GotThumbnail:
	
	if (lpOutFrame)
	{	// got info 
        if( bImported )
    		lpInfo->fileType = pi.idFileType - IDN_FIRSTIMPORTFILETYPE;
        else
    		lpInfo->fileType = pi.idFileType - IDN_FIRSTFILETYPE;

		lpInfo->dataType = pi.idDataType - IDC_FIRSTDATATYPE;
		lpInfo->time = timer;
	}
	lpInfo->fileXSize = pi.fileXSize;
	lpInfo->fileYSize = pi.fileYSize;
	lpInfo->fileRes = pi.fileRes;
	
	// still need to scale??
	if (lpOutFrame && 
			(FrameXSize(lpOutFrame) > DEF_THM_X || FrameYSize(lpOutFrame) > DEF_THM_Y))
	{
		
		lpInfo->fileDepth = lpOutFrame->Depth;
		lpNewFrame = THM_make_preview(lpOutFrame);
		FrameClose(lpOutFrame);
		lpOutFrame = lpNewFrame;
	}
	
	// if we got a whole frame then convert
	if (lpOutFrame)
	{
		if (FrameType(lpOutFrame) == FDT_PALETTECOLOR)
		{
			lpNewFrame = FrameConvert(lpOutFrame, FDT_RGBCOLOR, NULL);
			FrameClose(lpOutFrame);
			lpOutFrame = lpNewFrame;
		}
	}
	// setup output frame info
	if (lpOutFrame)
	{
		lpInfo->thmtime = timer;
		lpInfo->thmXSize = lpOutFrame->Xsize;
		lpInfo->thmYSize = lpOutFrame->Ysize;
		lpInfo->thmDepth = lpOutFrame->Depth;
	}
	return(lpOutFrame);
}		
	

//***************************************************************************
// 	Returns a thumbnail frame created from the given frame.
//	Returns null for failure.
//***************************************************************************
LOCAL LPFRAME THM_make_preview(LPFRAME lpFrame)
//***************************************************************************
{
	LPFRAME lpOutFrame;
	LFIXED 	rate, line;
	LPTR	lpSrc, lpDst;
	int 	yread;
	int 	y, lin, pix;
	
	// find thumbnail size
	lin = DEF_THM_Y;
	if (lin > lpFrame->Ysize)
		lin = lpFrame->Ysize;
	pix = DEF_THM_X;
	if (pix > lpFrame->Xsize)
		pix = lpFrame->Xsize;
	ScaleToFit(&pix, &lin, lpFrame->Xsize, lpFrame->Ysize);
	lin = Max(1, lin);
	pix = Max(1, pix);	
	rate = FGET(lpFrame->Xsize, pix);
	// get new frame 
	lpOutFrame = FrameOpen(FrameTypeInfo(lpFrame), 
						pix, 
						lin,
						FrameResolution(lpFrame));
	if (!lpOutFrame)
		return(NULL);
	// sample frame
	line = 0L;
	yread = 0;
	for (y=0;y<lin;y++)
	{
		yread = WHOLE( line );
		lpDst = FramePointer(lpOutFrame, 0,y,YES);
		lpSrc = FramePointer(lpFrame, 0,yread,NO);
		if (!lpSrc || !lpDst)
		{
			FrameClose(lpOutFrame);
			return(NULL);
		}
		FrameSample(lpOutFrame, lpSrc, 0, lpDst, 0, pix, rate);
		line += rate; 
	} 	// for y
	return(lpOutFrame);
}

//**************************************************************************
//	Writes the given preview frame into the given thumbnail file.
//	Writes any changes to info.
// 	lpInfo MUST be up to date (without new preview info).
// 	if lpFrame == NULL then deletes the thumb from the thumb file.
//	Returns lpFrame if successful, NULL otherwise.
//	Dont forget to free old frame and install the new frame in the THMNAIL structure.
//**************************************************************************
LOCAL LPFRAME THM_put_preview_frame(LPTHMINFO lpInfo, LPFRAME lpFrame, FILE_HANDLE hfile)
//**************************************************************************
{
	long inSize, outSize, lThumb;
	UINT lineSize;
	WORD wCompression = 0;
	
	// is there a new frame??
	if (!lpFrame)
	{
		THM_delete_frame(lpInfo, hfile);
		return(lpFrame);
	}
	// decide where to put new frame
	inSize = lpInfo->thmXSize*lpInfo->thmYSize*lpInfo->thmDepth;
	lineSize = FrameByteWidth(lpFrame);
	outSize = lineSize*lpFrame->Ysize; 		// NON-Compressed Size

	// write over old frame	if there is no size change and no compression
	if (lpInfo->lThumb 
			&& (inSize==outSize || lpInfo->lThumb>lpInfo->lDescr) 
			&& !wCompression)
		lThumb = lpInfo->lThumb;
	else if (lpInfo->lDescr) 
	{
		// put after description
		if (!THM_delete_frame(lpInfo, hfile))
			return(NULL);
		lThumb = lpInfo->lDescr+lpInfo->descrLen;
	}
	else 
		// put after lpInfo
		lThumb = lpInfo->lFree;
	
	// write frame		
	if (FileSeek(hfile, lThumb, 0/*begining*/) != lThumb)
		return(NULL);
		
	outSize = THM_save_thumb(hfile, lpFrame, wCompression);
	if (!outSize)
		return(NULL);
	
	// write new lpInfo
	lpInfo->thmXSize = lpFrame->Xsize;
	lpInfo->thmYSize = lpFrame->Ysize;
	lpInfo->thmDepth = lpFrame->Depth;
	lpInfo->lThumb = lThumb;
	lpInfo->wCompress = wCompression;
	lpInfo->thmSize = outSize;
	
	if (!THM_write_info(lpInfo, hfile))
	{
		lpInfo->lThumb = 0L;
		return(NULL);
	}
	return (lpFrame);
}

//**************************************************************************
//	Loads the frame from the current file position.
//	Performs Decompression.
//	Does NO conversion - assumes that lpFrame is the correct type.
//	Returns false for error.
//**************************************************************************
LOCAL BOOL THM_load_thumb(FILE_HANDLE hfile, LPFRAME lpFrame, WORD wCompression)
//**************************************************************************
{	
	int ByteWidth;
	int y, ysize;
	LPTR lptr;
	
	ByteWidth = FrameByteWidth(lpFrame);
	ysize = FrameYSize(lpFrame);

	// load thumb
	for (y = ysize-1; y>=0; y--)
	{
		lptr = FramePointerRaw(lpFrame, 0, y,YES);
		if (!lptr)
			return(FALSE);
		if (FileRead(hfile, lptr, ByteWidth) != ByteWidth)
			return(FALSE);	
	}
	return(TRUE);
}

//**************************************************************************
//	Saves the frame at the current file position.
//	Performs specified compression.
//	Returns the number of bytes written- 0 for error.
//**************************************************************************
LOCAL LONG THM_save_thumb(FILE_HANDLE hfile, LPFRAME lpFrame, WORD wCompression)
//**************************************************************************
{	
	int y, ysize, lineSize;
	LPTR lptr;
	LONG bytes = 0;
	
	ysize = FrameYSize(lpFrame);
	lineSize = FrameByteWidth(lpFrame);
	
	for (y = ysize-1; y>=0; y--)
	{
		lptr = FramePointerRaw(lpFrame, 0, y,NO);
		if (!lptr)	
		   	return(FALSE);
		if (FileWrite(hfile, lptr, lineSize ) != lineSize)
		   	return(FALSE);
		bytes += lineSize; 
	}
	
	return(bytes);
}

