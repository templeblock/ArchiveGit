//
// (c) Copyright 1992 MICROGRAFX, Inc., All Rights Reserved.
// This material is confidential and a trade secret.
// Permission to use this work for any purpose must be obtained
// in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081

//	Performs caching for thmmgr.
// 	Do not mix calls to the two sets of functions.
 
#include "pp.h"
#include "thmmgr.h"
#include "cthmmgr.h"
#include "bsrutil.h"

//***************************Static Protoypes***************************
static int cthmMaxCache;
static int cthmNumCache;
static LPCTHMNAIL lpCThmFirst;
static LPCTHMNAIL lpCThmLast;
static BOOL bCache = NO;
static int iInitCount = 0;

//***************************Static Protoypes***************************
LOCAL LPCTHMNAIL cthm_new(void);
LOCAL void cthm_cache(LPCTHMNAIL lpCThm);
LOCAL void cthm_decache(LPCTHMNAIL lpCThm);
LOCAL LPCTHMNAIL cthm_get_cached(LPFILELOC lpThumb, BOOL bSpecial);
LOCAL void cthm_cleanup_cache(int spaces);

//**************************************************************************
// 	Call BEFORE ANY OTHER CTHM OPERATION!!!. 
//**************************************************************************
void CTHMInitCache(void)
//**************************************************************************
{
	// only allow 1 initialization
	iInitCount++;
	if (iInitCount != 1)
		return;
	GetDefInt(cthmMaxCache, 25);
	cthmNumCache = 0;
	lpCThmFirst = NULL;
	lpCThmLast = NULL;
	bCache = cthmMaxCache ? TRUE:FALSE;
}

//**************************************************************************
// 	Call AFTER DONE WITH ALL CTHM OPERATIONS!!!. 
//**************************************************************************
void CTHMEndCache(void)
//**************************************************************************
{
	LPCTHMNAIL lpCThm, lpNextCThm;
	
	// only unitialize when last user is done
	iInitCount--;
	if (iInitCount)
		return;
	lpCThm = lpCThmFirst;
	while (lpCThm)
	{
		lpNextCThm = lpCThm->next;
		THMClose(&lpCThm->thm);
		FreeUp((LPTR)lpCThm);
		lpCThm = lpNextCThm;
	}
	cthmNumCache = 0;
	lpCThmFirst = NULL;
	lpCThmLast = NULL;
	bCache = FALSE;
}


//**************************************************************************
// 	Returns a CTHMNAIL structure for the thumbnail file specified by lpEntry.
// 	Returns NULL for failure.
//	May NOT fill description or frame part of CTHMNAIL.
// 	Call CTHMClose when done with the thumbnail. 
//**************************************************************************
LPCTHMNAIL	CTHMOpen(LPENTRY lpEntry)
//**************************************************************************
{
	LPCTHMNAIL lpCThm;
	
	// try cache
	lpCThm = cthm_get_cached(&(lpEntry->Thumb), NO);
	if (!lpCThm)
	{
		if (!(lpCThm = cthm_new()))
			return(NULL);
		if (!THMOpen(&lpCThm->thm, lpEntry))
		{
			FreeUp((LPTR)lpCThm);
			return(NULL);			
		}
		cthm_cache(lpCThm);
	}
	lpCThm->iOut++;
	return(lpCThm);
}

//**************************************************************************
// 	Returns a CTHMNAIL structure for the thumbnail file specified by lpFile.
//	ONLY LOOKS AT CACHED THUMBS!
// 	Returns NULL for failure.
//	May NOT fill description or frame part of CTHMNAIL.
// 	Call CTHMClose when done with the thumbnail. 
//**************************************************************************
LPCTHMNAIL	CTHMOpenCached(LPFILELOC lpFile)
//**************************************************************************
{
	LPCTHMNAIL lpCThm;
	
	// try cache
	lpCThm = cthm_get_cached(lpFile, NO);
	if (lpCThm)
		lpCThm->iOut++;
	return(lpCThm);
}
		

//**************************************************************************
//	Closes a thumnail.
//	Destroys frame and memory ONLY if there are too many already cached.
//**************************************************************************
void		CTHMClose(LPCTHMNAIL lpCThm)
//**************************************************************************
{
	if (cthmMaxCache < cthmNumCache)
		CTHMPurge(lpCThm);
	else
		lpCThm->iOut--;
}

//**************************************************************************
//	Closes a thumnail and purges from cache (if not in use).
//	Destroys frame if any and frees all memory associated with the cthmnail.
//**************************************************************************
void		CTHMPurge(LPCTHMNAIL lpCThm)
//**************************************************************************
{
	lpCThm->iOut--;
	if (lpCThm->iOut > 0)
		return;
	cthm_decache(lpCThm);
	THMClose(&lpCThm->thm);
	FreeUp((LPTR)lpCThm);
}

//**************************************************************************
// 	Creates a thumbnail file for the file in lpEntry.  
//	Returns a CTHMNAIL structure for the new thumbnail or NULL for failure.
//	If !bLocal then it is created on the same media as the file (if possible).
//	Fills in lpEntry->szThumb with thw new name.
//	Fills in lpEntry->volume.
//	Does NOT fill in the file with relavent data (use THMReCreateInfo/Frame)
// 	Call THMClose when done with the thumbnail. 
//**************************************************************************
LPCTHMNAIL	CTHMCreate(LPENTRY lpEntry, BOOL bLocal)
//**************************************************************************
{
	LPCTHMNAIL lpCThm;
	
	// get new CTHMNAIL structure
	if (!(lpCThm = cthm_new()))
		return(NULL);
	if (!(THMCreate(&lpCThm->thm, lpEntry, bLocal)))
	{
		FreeUp((LPTR)lpCThm);
		return(NULL);			
	}
	lpCThm->iOut++;
	return(lpCThm);
}


//**************************************************************************
//	Returns the frame associated with the thumbnail.
//	Returns NULL if there is no frame or an error.
// 	Loads frame from thumb if needed.
//**************************************************************************
LPFRAME		CTHMGetFrame(LPCTHMNAIL lpCThm)
//**************************************************************************
{
	return(THMGetFrame(&lpCThm->thm));
}

//**************************************************************************
//	Turns the given frame into a thumbnail and writes it to the given file
//	Updates the info to reflect the new frame, data, and file type.
//	Returns TRUE if successful.
// 	Re-reads thumbnail info.
// 	if lpFrame == NULL then deletes the thumb from the thumb file.
// 	Sets timestamp to be 0, so user should fill in thmtime and follow
//	this routine should be followed by a call to THMReCreateInfo, 
//	to update file size, time etc.
//**************************************************************************
BOOL		CTHMPutFrame(LPCTHMNAIL lpCThm, LPFRAME lpFrame, int width, int height, LONG dataType, LONG fileType, LONG time)
//**************************************************************************
{
	return(THMPutFrame(&lpCThm->thm,lpFrame,width,height,dataType,fileType,time));
}
				
//**************************************************************************
//	Deletes the frame from the thumb file and from memory.
// 	Updates info from thm file.
//**************************************************************************
BOOL		CTHMDeleteFrame(LPCTHMNAIL lpCThm)
//**************************************************************************
{
	return(THMDeleteFrame(&lpCThm->thm));
}

//**************************************************************************
//	Returns the description associated with the thumbnail.
//	Returns "" if there is no description or an error.
// 	Loads description from thumb if needed.
//	Sets the bDescr field of lpCThm->thm.
//**************************************************************************
LPDESC		CTHMGetDescription(LPCTHMNAIL lpCThm)
//**************************************************************************
{
	return(THMGetDescription(&lpCThm->thm));
}

//**************************************************************************
//	Writes the given description into the given thumbnail file.
//	Copies it into lpThm->szDescr
//	Returns TRUE if successful.
// 	Re-reads thumbnail info.
// 	if lpDescription == NULL then deletes the description from the thumb file.
//**************************************************************************
BOOL		CTHMPutDescription(LPCTHMNAIL lpCThm, LPDESC lpDescription)
//**************************************************************************
{
	return(THMPutDescription(&lpCThm->thm,lpDescription));
}

//**************************************************************************
//	Deletes the description from the thumb file and from memory.
// 	Updates info from thm file.
//	Returns true is successful.
//**************************************************************************
BOOL		CTHMDeleteDescription(LPCTHMNAIL lpCThm)
//**************************************************************************
{
	return(THMDeleteDescription(&lpCThm->thm));
}

//**************************************************************************
//	Updates the thumbnail info based on the file given.
//	Only Updates minimal "EASY INFO"
//	Only updates if the file is newer than the thumb OR bAlways==TRUE.	
//	Returns TRUE if sucessful.
//	Failure may occur if the original file is on 
//		unavailable, removable media.
//**************************************************************************
BOOL 		CTHMReCreateEasyInfo(LPCTHMNAIL lpCThm, LPFILELOC lpFile, BOOL bAlways)
//**************************************************************************
{
	return(THMReCreateEasyInfo(&lpCThm->thm,lpFile,bAlways));
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
BOOL 		CTHMReCreateFrame(LPCTHMNAIL lpCThm, LPFILELOC lpFile, BOOL bAlways)
//**************************************************************************
{
	return(THMReCreateFrame(&lpCThm->thm,lpFile,bAlways));
}

//**************************************************************************
//	Copies all available information from one thumbnail to another.
//**************************************************************************
BOOL 		CTHMCopyThumb(LPCTHMNAIL lpDstCThm, LPCTHMNAIL lpSrcCThm, ULONG time)
//**************************************************************************
{
	return(THMCopyThumb(&lpDstCThm->thm, &lpSrcCThm->thm, time));
}

//**************************************************************************
//	Performs the appropriate validation based on level.
//	level is a set of validation flags (see bsrtypes.h)
//	Handles: VL_CLEAN, VL_DATAONLY, VL_NOTIME
//	This may include updateing the info and thumbnail preview.
//	returns TRUE if successful, FALSE for failure, -1 if THUMB SHOULD BE DELETED!!!.
//	DOES NOT CREATE OR DELETE ANY THUMBNAIL.
//	Failure may occur if the original file is 
//		unavailable, or on removable media.
//**************************************************************************
int		CTHMValidate(LPCTHMNAIL lpCThm, LPFILELOC lpFile, WORD level)
//**************************************************************************
{
	return(THMValidate(&lpCThm->thm,lpFile,level));
}

//**************************************************************************
//	Reads a photo_cd thumbnail for the given file.
//	Returns the thumbnail or null.
//	Note: this thumbnail can not be modified
// 	Call CTHMClose when done with the thumbnail. 
//**************************************************************************
LPCTHMNAIL 	CReadPCDThumb(LPTSTR lpFileName)
//**************************************************************************
{
	LPCTHMNAIL lpCThm;
	FILELOC File;
	
	// try cache
	lstrcpy(File.szFile, lpFileName);
	CLEARVOLUME(File.volume);
	lpCThm = cthm_get_cached(&File, YES);
	if (!lpCThm)
	{
		if (!(lpCThm = cthm_new()))
			return(NULL);
		if (!ReadPCDThumb(&lpCThm->thm, lpFileName))
		{
			FreeUp((LPTR)lpCThm);
			return(NULL);			
		}
		lpCThm->thm.Thumb = File;
		cthm_cache(lpCThm);
	}
	lpCThm->iOut++;
	return(lpCThm);
}

//**************************************************************************
//**************************************************************************
//	Reads the thumb from a 3.1 thumbnail (if available) for the file.
//	lpThumbDir is where 3.1 dbf's and thumbs are.
//	Note: this thumbnail can not be modified
// 	Call CTHMClose when done with the thumbnail. 
//**************************************************************************
LPCTHMNAIL	CReadOldThumb(LPTSTR lpFileName, LPTSTR lpThumbDir)
//**************************************************************************
{
	LPCTHMNAIL lpCThm;
	FILELOC File;
	
	// try cache
	lstrcpy(File.szFile, lpFileName);
	CLEARVOLUME(File.volume);
	lpCThm = cthm_get_cached(&File, YES);
	if (!lpCThm)
	{
		if (!(lpCThm = cthm_new()))
			return(NULL);
		if(!ReadOldThumb(&lpCThm->thm, lpFileName, lpThumbDir))
		{
			FreeUp((LPTR)lpCThm);
			return(NULL);			
		}
		lpCThm->thm.Thumb = File;
		cthm_cache(lpCThm);
	}
	lpCThm->iOut++;
	return(lpCThm);
}

//**************************************************************************
//************************STATIC ROUTINES******************************************
//**************************************************************************

//**************************************************************************
//	Gets a new cthm structure.
//	May return NULL.
//**************************************************************************
LOCAL LPCTHMNAIL cthm_new(void)
//**************************************************************************
{
	LPCTHMNAIL lpcthm;
	
	lpcthm = (LPCTHMNAIL)Alloc(sizeof(CTHMNAIL));
	if (!lpcthm)
		return(NULL);
	lpcthm->iOut = 0;
	lpcthm->next = NULL;
	lpcthm->previous = NULL;
	return(lpcthm);
}

//**************************************************************************
//	Adds the thumb to the cache list.
// 	May decache others if needed and possible.
//**************************************************************************
LOCAL void cthm_cache(LPCTHMNAIL lpCThm)
//**************************************************************************
{
	if (!bCache)
		return;
	if (cthmNumCache >= cthmMaxCache)
		cthm_cleanup_cache(1);
	cthmNumCache++;
	lpCThm->next = lpCThmFirst;
	lpCThm->previous = NULL;
	if (lpCThmFirst)
		lpCThmFirst->previous = lpCThm;
	else
		lpCThmLast = lpCThm;
	lpCThmFirst = lpCThm;
}

//**************************************************************************
//	Removes thumbnail from cache.
//	DOES NOT CHECK IF THUMB IS IN USE.
//**************************************************************************
LOCAL void cthm_decache(LPCTHMNAIL lpCThm)
//**************************************************************************
{
	if (!bCache)
		return;
	cthmNumCache--;
	if (lpCThm == lpCThmFirst)
		lpCThmFirst = lpCThm->next;
	if (lpCThm == lpCThmLast)
		lpCThmLast = lpCThm->previous;
	if (lpCThm->previous)
		lpCThm->previous->next = lpCThm->next;
	if (lpCThm->next)
		lpCThm->next->previous = lpCThm->previous;
	lpCThm->next = lpCThm->previous = NULL;
}

//**************************************************************************
// 	Searches for a thumb in the cache list with the file name lpThumb.
//	Returns the thumb found or NULL.
//**************************************************************************
LOCAL LPCTHMNAIL cthm_get_cached(LPFILELOC lpThumb, BOOL bSpecial)
//**************************************************************************
{
	LPCTHMNAIL lpcthm;
	
	if (!bCache)
		return(NULL);
	lpcthm = lpCThmFirst;
	while(lpcthm)
	{
		if (bSpecial == lpcthm->thm.bSpecial)
			if (EQFILELOC(*lpThumb, lpcthm->thm.Thumb))
				return(lpcthm);
		lpcthm = lpcthm->next;
	}
	return(NULL);
}

//**************************************************************************
//	Trys to remove enough unused thumbs to get 
//		(cthmNumCache <= cthmMaxCache-spaces)
//**************************************************************************
LOCAL void cthm_cleanup_cache(int spaces)
//**************************************************************************
{
	LPCTHMNAIL lpcthm, lpprevcthm;
	
	if (!bCache)
		return;
	lpcthm = lpCThmLast;
	while(lpcthm)
	{
		if (cthmNumCache <= cthmMaxCache-spaces)
			return;
		lpprevcthm = lpcthm->previous;
		if (lpcthm->iOut <= 0)
		{
			cthm_decache(lpcthm);
			THMClose(&lpcthm->thm);
			FreeUp((LPTR)lpcthm);
		}
		lpcthm = lpprevcthm;
	}
}

