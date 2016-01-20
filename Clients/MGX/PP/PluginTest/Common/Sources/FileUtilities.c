//-------------------------------------------------------------------------------
//
//	File:
//		FileUtilities.c
//
//	Copyright 1996-1998, Adobe Systems Incorporated.
//	All Rights Reserved.
//
//	Description:
//		This library contains sources and functions
//		to simplify the use of extremely common file
//		functions for plug-ins. 
//
//	Use:
//		FileUtilities is intended to group common functions
//		into higher-level macros and routines to simplify
//		plug-in programming.
//
//	Version history:
//		Version 1.0.0	6/1/1996	Created for Photoshop 4.0
//			Written by Andrew Coven
//
//		Version 1.0.1	4/8/1997	Updated for Photoshop 4.0.1
//			Comments fattened out.  Reorganized for clarity.
//
//-------------------------------------------------------------------------------

#include "PIDefines.h"
#include "FileUtilities.h"

//-------------------------------------------------------------------------------
//
//	PISetSaveDirectory
//
//	Inputs:
//
//	Outputs:
//
//-------------------------------------------------------------------------------

Boolean PISetSaveDirectory (Handle alias, Str255 s, 
		   			   		Boolean query, short *rVRefNum)
{
#ifdef __PIMac__	
	CInfoPBRec			pb;
	Boolean				wasChanged;
	FSSpec				spec;
	int32				gotErr = 0;
	
	if (alias != NULL)
	{
	
		ResolveAlias(nil, (AliasHandle) alias, &spec, &wasChanged);
		
		/* set up file param structure so we can get whether we've been handed
		   a file or a directory */
		
		pb.dirInfo.ioCompletion = nil;
		pb.dirInfo.ioNamePtr = spec.name;
		pb.dirInfo.ioVRefNum = spec.vRefNum;
		pb.dirInfo.ioFRefNum = 0;
		pb.dirInfo.ioFDirIndex = 0;
		pb.dirInfo.ioDrDirID = spec.parID;
		
		gotErr = PBGetCatInfoSync( &pb );
		
		/* now check for NO error and this is NOT a directory */
		// nah, don't need to check for error.
		/* even if we got an error, these fields will be valid (enough) */
		
		if (spec.name[0] > 0)
		/* the scripting system passed us something.  Figure out what to do with it. */
		{
			if (pb.dirInfo.ioFlAttrib & ioDirMask)
			{ // the scripting system passed us a directory.  Add filename to it
		 	  // if we're not popping a dialog.
									
				if (!query)
				{ // if we were going to show this, we could not
				  // make it :dir:name since that would be bad for the file dialog
					spec.name[++spec.name[0]] = ':';
					PICopy(&spec.name[ (spec.name[0]+1) ], &s[1], s[0]);
					spec.name[0] += s[0];
					s[0] = spec.name[0]+1;
					s[1] = ':'; // since we're a directory
					PICopy (&s[2], &spec.name[1], spec.name[0]); // copy name to output string
				}
				else
				{ // querying so set dir
					LMSetSFSaveDisk(-pb.dirInfo.ioVRefNum);
					LMSetCurDirStore(pb.dirInfo.ioDrDirID);
				}
			}
			else
			{
				/* we were given a full pathname. Copy the name. */
				PICopy (s, spec.name, (spec.name[0])+1);
				
				if (query)
				{ // we're showing our dialog, set set directory from alias
					LMSetSFSaveDisk(-spec.vRefNum);
					LMSetCurDirStore(spec.parID);
				}
			}
			/* otherwise we'll just use whatever the export parameter block
			   has passed us */
		}

		/* set gStuff->vRefNum as working directory (alias) from vRefNum
		   from hard vRefNum/parID */
		PICloseAndOpenWD(spec.vRefNum, spec.parID, rVRefNum);
		return TRUE;
	}
	else return FALSE; // aliashandle was null

#elif defined(__PIWin__)
	return FALSE; // for now
#endif
}

/*****************************************************************************/

Boolean PICreateFile (Str255 filename, short vRefNum, const ResType creator, 
					  const ResType type, FileHandle *fRefNum, short *result)
{

#ifdef __PIMac__

	/* Mark the file as not being open. */
	
	*fRefNum = 0;
	
	/* Create the file. */

	(void) FSDelete (filename, vRefNum);

	if (!TestAndStoreResult (result, Create (filename, vRefNum, creator, type)))
		return FALSE;
		
	/* Open the file. */

	if (!TestAndStoreResult (result, FSOpen (filename, vRefNum, fRefNum)))
	{
		/* failure */
		(void) FSDelete (filename, vRefNum);
		return FALSE;
	}
	else
		return TRUE; // succeeded
		
#elif defined(__PIWin__)
	return FALSE; // for now
#endif
}

/*****************************************************************************/

Boolean PICloseFile (Str255 filename, short vRefNum,
					 FileHandle fRefNum, Boolean sameNames,
					 Boolean *dirty, AliasHandle *alias, short *result)
{

#ifdef __PIMac__

	FSSpec			spec;
	OSErr			gotErr;
	
	// Close the file if it is open.
	if (fRefNum != 0)
		(void) TestAndStoreResult (result, FSClose (fRefNum));
		
	// Delete the file if we had an error.	
	if (*result != noErr)
		(void) FSDelete (filename, vRefNum);
			
	// Flush the volume anyway.
	if (!TestAndStoreResult (result, FlushVol (NULL, vRefNum))) return FALSE;
	
	// Mark the file as clean.
	*dirty = FALSE;
	
	/* create alias for scripting system */
	
	/* first, make FSSpec record */
	FSMakeFSSpec(vRefNum, 0, filename, &spec);
	
	if (sameNames)
	{ // didn't enter new filename, so set filename to nothing
		spec.name[ (spec.name[0] = 0)+1 ] = 0;
	} // otherwise use filename and store entire reference

	gotErr = NewAlias(nil, &spec, alias);

	return (gotErr == noErr);

#elif defined(__PIWin__)
	return noErr; // for now
#endif
}
	
/*****************************************************************************/
// Creates new working directory and deletes old one if it was valid

void PICloseAndOpenWD(const short vRefNum, const long dirID, short *rVRefNum)
{
#ifdef __PIMac__
	short oldVRefNum = *rVRefNum;
	
	OpenWD(vRefNum, dirID, 0, rVRefNum);
	if (*rVRefNum != oldVRefNum && oldVRefNum < -100)
	{ // small negative numbers are actual volumes (-1, etc.)
	  // large negative numbers (-32766, etc.) are WD's to be closed
		CloseWD(oldVRefNum); // close our old vRefNum
	}
#elif defined(__PIWIn__)
	{
		// Do something win like here.
	}
#endif

}

/*****************************************************************************/

Boolean TestAndStoreResult (short *res, OSErr result)
{
	if (result != noErr)
		{
		if (*res == noErr)
			*res = result;
		return FALSE;
		}
	else
		return TRUE;	
}
	
/*****************************************************************************/

Boolean TestAndStoreCancel (short *res, Boolean tocancel)
{	
	if (tocancel)
		{
		if (*res == noErr)
			*res = userCanceledErr;
		return FALSE;
		}
	else
		return TRUE;	
}


/*****************************************************************************/

/* File I/O Routines. */

#ifdef __PIWin__

/*****************************************************************************/

/* File I/O Routines. */

OSErr FSWrite(int32 refNum, long *count,void *buffPtr)
{
	/* Note:  this routine doesn't work for data larger than 64k. */

	WORD bytes;

	bytes = (WORD)*count;
	if ((*count = _lwrite((int)refNum,(LPSTR)buffPtr,bytes))==0xffff)
		return writErr;

	return noErr;
}

OSErr SetFPos (int32 refNum, short posMode, long posOff)
{
	_llseek((int)refNum, posOff, posMode);
	return noErr;	// just for samples sake. should return error value based on __llseek's
}

OSErr FSRead(int32 refNum, long *count	, void *buffPtr)
{
	WORD bytes;

	bytes = (WORD) *count;

	if ((*count = _lread((int)refNum,(LPSTR)buffPtr,bytes))==0xffff)
		return readErr;

	return noErr;
}

#endif // Only need to define for windows.

/*****************************************************************************/

// end FileUtilities.c
