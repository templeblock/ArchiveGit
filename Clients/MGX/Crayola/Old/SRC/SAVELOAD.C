//®PL1¯®FD1¯®TP0¯®BT0¯
// (c) Copyright 1991 MICROGRAFX, Inc., All Rights Reserved.
// This material is confidential and a trade secret.
// Permission to use this work for any purpose must be obtained
// in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081
#include <windows.h>
#include "string.h"
#include "id.h"
#include "data.h"
#include "routines.h"

// Static prototypes
static void DelRecallImageName( LPSTR lpFileName );
static void MakeRecallName(LPSTR lpFileName);

extern HWND hWndAstral;
extern SAVE Save;

/***********************************************************************/
LPFRAME AstralFrameLoad(
/***********************************************************************/
LPSTR 	szName,
int 	depth,
LPINT 	lpDataType,
LPINT 	lpFileType)
{
int Type;
LPFRAME lpFrame;

// Force the file type if the extension is a known one
Type = GetFileExtentionType(szName);
if ( !Type && lpFileType )
	Type = *lpFileType;

if ( Type == IDN_TIFF )								/* Load TIFF image */
	lpFrame = tifrdr(szName, depth, lpDataType, NO/*bReadOnly*/);
else
if ( Type == IDN_BMP )
	lpFrame = bmprdr(szName, depth, lpDataType);	/* Load BMP image */
else
	lpFrame = NULL;
if ( lpFileType )
	*lpFileType = Type;

return( lpFrame );
}

/***********************************************************************/
BOOL AstralFrameSave(
/***********************************************************************/
LPSTR 		lpFileName,
LPFRAME 	lpFrame,
LPRECT 		lpRect,
int 		DataType,
int			FileType,
BOOL 		fCompressed)
{
long lMaxSize;
int iResult;
FNAME szName, szTempName;
BOOL bTemp;

if (!lpFrame)
    return(FALSE);

if ( lpRect )
	{
	BoundRect(lpRect, 0, 0, FrameXSize(lpFrame)-1, FrameYSize(lpFrame)-1);
	lMaxSize = (long)RectWidth(lpRect) * RectHeight(lpRect);
	}
else	lMaxSize = (long)FrameXSize(lpFrame) * FrameYSize(lpFrame);

lstrcpy(szName, lpFileName);
if ( DataType == IDC_SAVE32BITCOLOR )
	lMaxSize *= 4;
else
if ( DataType == IDC_SAVE24BITCOLOR )
	lMaxSize *= 3;
else
if ( DataType == IDC_SAVELA || DataType == IDC_SAVESP )
	lMaxSize /= 8;
lMaxSize += 5000;

if ( lMaxSize > OSDiskSpace(szName) )
	{
	Message( IDS_EDISKSPACE, Lowercase(szName),
		(lMaxSize+512)/1024, (OSDiskSpace(szName)+512)/1024 );
	return( FALSE );
	}

// if file already exists, copy it into a temporary
// file in case write fails
bTemp = NO;
if (FileExists(szName))  // is this an overwrite?
	{ 
	// make sure we have write access to destination file
	if (!FilePermission(szName,  2 /*WRITE*/))  // write access?
		{
		Message(IDS_EWRITEPROTECT, lpFileName);
		return(FALSE);
		}
	// create temporary file name
	lstrcpy(szTempName, szName);
	stripfile(szTempName);
	lstrcat(szTempName, "ppXXXXXX");
	if (MkTemp(szTempName))
		// temp name creation was successful
		bTemp = !RenameFile(szName, szTempName); // rename original
	}

if ( FrameDepth(lpFrame) < 3 && DataType == IDC_SAVE8BITCOLOR )
	DataType = IDC_SAVECT;

switch (FileType)
    {
    case IDN_TIFF:
	iResult = tifwrt( szName, NULL, lpFrame, lpRect, DataType, fCompressed );
	break;
    case IDN_BMP:
	iResult = bmpwrt( szName, NULL, lpFrame, lpRect, DataType, fCompressed );
	break;
    default:
	break;
    }

// see if we created a temporary save file
if (bTemp)
	{
	if (iResult >= 0)  // successful write, bag temp file
		FileDelete(szTempName); // don't care if error
	else		// unsuccessful write, rename temp to original
		{
		if (rename(szTempName, szName)) // do rename
			FileDelete(szTempName);	// error, bag temp file
		}
	}
//if (iResult >= 0)
//	HandleImageModified(szName, lpFrame, DataType);

return(iResult >= 0);
}

/***********************************************************************/
BOOL AstralObjectListSave(
	LPSTR     lpFileName,
	LPOBJECT lpObject,
	LPRECT   lpRect,
	int      DataType,
	int		 FileType,
	BOOL     fCompressed)
/***********************************************************************/
{
	long  lMaxSize;
	int   iResult;
	FNAME szName, szTempName;
	BOOL  bTemp;
	LPFRAME lpFrame;

	if (!lpObject)
		return(FALSE);

	lpFrame = ObjGetEditFrame(lpObject);

	if (!lpFrame)
		return(FALSE);

	if ( lpRect )
	{
		BoundRect(lpRect, 0, 0, FrameXSize(lpFrame)-1, FrameYSize(lpFrame)-1);
		lMaxSize = (long)RectWidth(lpRect) * RectHeight(lpRect);
	}
	else
		lMaxSize = (long)FrameXSize(lpFrame) * FrameYSize(lpFrame);

	lstrcpy(szName, lpFileName);

	if ( DataType == IDC_SAVE32BITCOLOR )
		lMaxSize *= 4;
	else
	if ( DataType == IDC_SAVE24BITCOLOR )
		lMaxSize *= 3;
	else
	{
		if ( DataType == IDC_SAVELA || DataType == IDC_SAVESP )
			lMaxSize /= 8;
		lMaxSize += 5000;
	}

	if ( lMaxSize > OSDiskSpace(szName) )
	{
		Message( IDS_EDISKSPACE, Lowercase(szName),
			(lMaxSize+512)/1024, (OSDiskSpace(szName)+512)/1024 );
		return( FALSE );
	}

	// if file already exists, copy it into a temporary
	// file in case write fails
	bTemp = NO;

	if (FileExists(szName))  // is this an overwrite?
	{ 
		// make sure we have write access to destination file
		if (!FilePermission(szName,  2 /*WRITE*/))  // write access?
		{
			Message(IDS_EWRITEPROTECT, lpFileName);
			return(FALSE);
		}

		// create temporary file name
		lstrcpy(szTempName, szName);
		stripfile(szTempName);
		lstrcat(szTempName, "ppXXXXXX");

		if (MkTemp(szTempName))
		{
			// temp name creation was successful
			bTemp = !rename(szName, szTempName); // rename original
		}
	}

	if ( FrameDepth(lpFrame) < 3 && DataType == IDC_SAVE8BITCOLOR )
		DataType = IDC_SAVECT;

	switch (FileType)
	{
		case IDN_TIFF:
			iResult = tifwrt( szName, lpObject, lpFrame, lpRect, DataType, fCompressed );
		break;

		case IDN_BMP:
			iResult = bmpwrt( szName, lpObject, lpFrame, lpRect, DataType, fCompressed );
		break;

		default:
		break;
	}

	// see if we created a temporary save file
	if (bTemp)
	{
		if (iResult >= 0)
		{
			// successful write, bag temp file
			FileDelete(szTempName); // don't care if error
		}
		else
		{
			// unsuccessful write, rename temp to original
			if (RenameFile(szTempName, szName)) // do rename
				FileDelete(szTempName);	// error, bag temp file
		}
	}
	//if (iResult >= 0)
	//	HandleImageModified(szName, lpFrame, DataType);

	return(iResult >= 0);
}

/***********************************************************************/
BOOL CALLBACK EXPORT AstralImageLoad(
/***********************************************************************/
WORD 	idFileType,
LPSTR 	lpFileName,
int 	iDoZoom,
BOOL 	bCheckDuplicates)
{
int idDataType;
LPSTR lpNewName;
LPFRAME lpFrame;
OBJECTLIST ObjList;
LPOBJECT lpBase;
LPALPHA lpAlpha;
LPPRIMOBJECT lpObject;
int Type;

if ( !bCheckDuplicates )
	lpNewName = lpFileName;
else
	{
	if ( !OKToOpen( lpFileName, &lpNewName ) )
		return(FALSE);
	}

Type = GetFileExtentionType(lpFileName);
if ( !Type )
	Type = idFileType;

if (Type == IDN_ART)
	{
	ProgressBegin(Control.MultipleObjects ? 1 : 2, 0);
	idFileType = Type;
	if (!objrdr(lpFileName, -1, &idDataType, &ObjList))
		{
		ProgressEnd();
		if (!FileExists(lpFileName))
			DelRecallImageName(lpFileName);
		return( FALSE );
		}
	lpBase = (LPOBJECT)ObjList.lpHead;
	lpFrame = lpBase->Pixmap.EditFrame;
	lpBase->Pixmap.EditFrame = NULL;

	lpAlpha = lpBase->lpAlpha;
	lpBase->lpAlpha = NULL;

	ObjUnlinkObject(&ObjList, (LPPRIMOBJECT)lpBase);
	ObjFreeUp(lpBase);

	// Setup the new image and bring up the new image window
	NewImageWindow(
		NULL,		// lpOldFrame
		lpNewName,	// Name
		lpFrame, 	// lpNewFrame
		idFileType,	// lpImage->FileType
		idDataType, // lpImage->DataType
		FALSE,		// New view?
		IMG_DOCUMENT,	// lpImage->DocumentType
		NULL,		// lpImage->ImageName
		iDoZoom		// Maximized?
		);
	if (lpImage && (ImgGetBaseEditFrame(lpImage) == lpFrame))
		{
		ImgSetMask(lpImage, lpAlpha);
		while (lpObject = ObjGetNextObject(&ObjList, NULL, YES))
			{
			ObjUnlinkObject(&ObjList, lpObject);
			ObjAddTail(&lpImage->ObjList, lpObject);
			}
		if (!Control.MultipleObjects)
			{
			ImgCombineObjects(lpImage, YES, NO, YES);
			lpImage->fUntitled = YES;
			}
		}
	else
		{
		while (lpObject = ObjGetNextObject(&ObjList, NULL, YES))
			{
			ObjUnlinkObject(&ObjList, lpObject);
			ObjFreeUp((LPOBJECT)lpObject);
			}
		}
	ProgressEnd();
	SetupMiniViews(NULL, NO);
	}
else
	{
	lpFrame = AstralFrameLoad( lpFileName, -1/*depth*/, &idDataType, (LPINT)&idFileType );
	if ( !lpFrame )
		{
		if (!FileExists(lpFileName))
			DelRecallImageName(lpFileName);
		return( FALSE );
		}

	// Setup the new image and bring up the new image window
	NewImageWindow(
		NULL,		// lpOldFrame
		lpNewName,	// Name
		lpFrame, 	// lpNewFrame
		idFileType, 	// lpImage->FileType
		idDataType, 	// lpImage->DataType
		FALSE,		// New view?
		IMG_DOCUMENT,	// lpImage->DocumentType
		NULL,		// lpImage->ImageName
		iDoZoom		// Maximized?
		);
	}

// Add the file name to our recall list
AddRecallImageName( lpFileName );
return( TRUE );
}


/***********************************************************************/
BOOL OKToOpen(
/***********************************************************************/
LPSTR 		lpFile,
LPSTR FAR *	lppName)
{
HWND hWnd, hOldWnd;
int ret, i;

*lppName = lpFile;
if ( !lpImage )
	return( YES );

hOldWnd = lpImage->hWnd;
ret = YES;
for (i = 0; i < NumDocs(); ++i)
	{
	if ( !(hWnd = GetDoc(i)) )
		continue;
	if ( !SetCurView( hWnd, NULL ) )
		continue;
	if ( !StringsEqual( lpFile, (LPSTR)lpImage->CurFile ) )
		continue;
	*lppName = NULL;
	ret = (AstralOKCancel( IDS_OKTOOPEN, (LPSTR)lpImage->CurFile ) == IDOK );
	break;
	}

SetCurView( hOldWnd, NULL );
return( ret );
}


/***********************************************************************/
void AddRecallImageName( LPSTR lpFileName )
/***********************************************************************/
{
int i, j;
FNAME szFileName;

lstrcpy(szFileName, lpFileName);
MakeRecallName(szFileName);
if (Control.RecallCount > 0)
	if (StringsEqual(szFileName, Control.RecallImage[0]))
		return;

// Allow only known extension names
if (!GetFileExtentionType(szFileName))
	return;

// Remove all duplicates from recall list
for ( i=0; i<Control.RecallCount; i++ )
	{
	// If so, remove duplicate from list
	if ( StringsEqual( szFileName, Control.RecallImage[i] ) )
		{
		for (j = i+1; j < Control.RecallCount; ++j)
			lstrcpy(Control.RecallImage[j-1],
			Control.RecallImage[j]);
		--i;
		--Control.RecallCount;
		}
	}
// move all in list down one to make space for new name
i = Control.RecallCount;  // current amount in list
if (i >= MAX_RECALL) // too many in list?
	--i;		 // yup
Control.RecallCount = i + 1; // new amount in list
--i;		     // form index of last item in list to be moved
for (; i >= 0; --i)
	lstrcpy(Control.RecallImage[i+1], Control.RecallImage[i]);
// Otherwise, add it to the list
Lowercase( szFileName );
lstrcpy( Control.RecallImage[ 0 ], szFileName );
}

/***********************************************************************/
static void DelRecallImageName( LPSTR lpFileName )
/***********************************************************************/
{
int i, j;
FNAME szFileName;

lstrcpy(szFileName, lpFileName);
MakeRecallName(szFileName);
// Remove from recall list
for ( i=0; i<Control.RecallCount; i++ )
	{
	// If so, remove duplicate from list
	if ( StringsEqual( szFileName, Control.RecallImage[i] ) )
		{
		for (j = i+1; j < Control.RecallCount; ++j)
			lstrcpy(Control.RecallImage[j-1], Control.RecallImage[j]);
		--i;
		--Control.RecallCount;
		}
	}
}

/***********************************************************************/
//	formats the file name so it can go into recall list
/***********************************************************************/
static void MakeRecallName(LPSTR lpFileName)
/***********************************************************************/
{
	FNAME szName;
	LPSTR lpOld, lpNew;
	
	lpOld = lpFileName;
	lpNew = szName;
	
	while(*lpOld)
	{
		*lpNew++ = *lpOld;
		if (*lpOld++ == '&')
			*lpNew++ = '&';
	}
	*lpNew = '\0';
	lstrcpy(lpFileName, szName);
}

/***********************************************************************/
//	formats the recall list entry file name so it can be opened
/***********************************************************************/
void GetRecallFileName(LPSTR lpRecallName)
/***********************************************************************/
{
	LPSTR lpOld, lpNew;
	
	lpOld = lpRecallName;
	lpNew = lpRecallName;
	
	while(*lpOld)
	{
		*lpNew++ = *lpOld;
		if (*++lpOld == '&')
			++lpOld;
	}
	*lpNew = '\0';
}


/***********************************************************************/
BOOL AstralImageSave(
	WORD 	idFileType,
	WORD 	idDataType,
	LPSTR 	lpFileName)
/***********************************************************************/
{
	if ( !lpImage )
		return( FALSE );

	if ( Control.Crippled )
	{
		Message( Control.Crippled );
		return( FALSE );
	}

	if (idFileType == IDN_ART)
	{
		if (objwrt(lpImage, lpFileName, idFileType, NULL, idDataType,
			Save.Compressed) < 0)
		{
			if (!lpImage->fUntitled && !FileExists(lpImage->CurFile))
				lpImage->fUntitled = YES;
		}
	}
	else
	{
		LPOBJECT lpObject = ImgGetBase(lpImage);

		if (ImgCountObjects( lpImage ) <= 1)
			lpObject = NULL;

		if (!lpObject)
		{
			if (!AstralFrameSave(lpFileName, ImgGetBaseEditFrame(lpImage), (LPRECT)NULL,
				idDataType, idFileType, Save.Compressed))
			{
				if (!lpImage->fUntitled && !FileExists(lpImage->CurFile))
					lpImage->fUntitled = YES;
				return( FALSE );
			}
		}
		else
		{
			if (!AstralObjectListSave(lpFileName, lpObject, (LPRECT)NULL,
				idDataType, idFileType, Save.Compressed))
			{
				if (!lpImage->fUntitled && !FileExists(lpImage->CurFile))
					lpImage->fUntitled = YES;
				return( FALSE );
			}
		}
	}

	SetImgChanged(lpImage, NO);

	return( TRUE );
}

/***********************************************************************/
BOOL LoadMap(
/***********************************************************************/
LPMAP 	lpMap0,
LPMAP	lpMap1,
LPMAP	lpMap2,
LPMAP	lpMap3,
LPSTR	lpFileName)
{
#include <string.h>
#define MAX_MAP_SIZE ((4*sizeof(MAP))+3+10)
LPMAP lpMap;
int fp, len;
char buffer[MAX_MAP_SIZE];

if ( ( fp = _lopen( lpFileName, OF_READ ) ) < 0 )
	{
	Message( IDS_EOPEN, Lowercase(lpFileName) );
	return( FALSE );
	}

AstralCursor( IDC_WAIT );	/* Put up the wait cursor */
len = _lread( fp, buffer, sizeof(buffer) );
_lclose( fp );
AstralCursor( NULL );		/* Revert back to the old cursor */

if (strncmp( "MP4", buffer, 3 ) )
	{
	Message( IDS_EBADMAP, Lowercase(lpFileName) );
	return( FALSE );
	}

len -= 3;
if ( len == (4 * sizeof(MAP)) )
	{
	lpMap = (LPMAP)&buffer[3];
	if ( lpMap0 )
		copy( (LPTR)lpMap, (LPTR)lpMap0, sizeof(MAP) );
	lpMap++;
	if ( lpMap1 )
		copy( (LPTR)lpMap, (LPTR)lpMap1, sizeof(MAP) );
	lpMap++;
	if ( lpMap2 )
		copy( (LPTR)lpMap, (LPTR)lpMap2, sizeof(MAP) );
	lpMap++;
	if ( lpMap3 )
		copy( (LPTR)lpMap, (LPTR)lpMap3, sizeof(MAP) );
	return( TRUE );
	}

Message( IDS_EREAD, Lowercase(lpFileName) );
return( FALSE );
}

/***********************************************************************/
BOOL SaveMap(
/***********************************************************************/
LPMAP	lpMap0,
LPMAP	lpMap1,
LPMAP	lpMap2,
LPMAP	lpMap3,
LPSTR	lpFileName)
{
int fp, len;
MAP NullMap0, NullMap1, NullMap2, NullMap3;

if ( ( fp = _lcreat( lpFileName, 0 ) ) < 0 )
	{
	Message( IDS_ECREATE, Lowercase(lpFileName) );
	return( FALSE );
	}

AstralCursor( IDC_WAIT );	/* Put up the wait cursor */

if ( lpMap0 )
	MakeMap( lpMap0 );
else	{
	ResetMap( &NullMap0, MAPPOINTS, NO );
	lpMap0 = &NullMap0;
	}
if ( lpMap1 )
	MakeMap( lpMap1 );
else	{
	ResetMap( &NullMap1, MAPPOINTS, NO );
	lpMap1 = &NullMap1;
	}
if ( lpMap2 )
	MakeMap( lpMap2 );
else	{
	ResetMap( &NullMap2, MAPPOINTS, NO );
	lpMap2 = &NullMap2;
	}
if ( lpMap3 )
	MakeMap( lpMap3 );
else	{
	ResetMap( &NullMap3, MAPPOINTS, NO );
	lpMap3 = &NullMap3;
	}
len  = _lwrite( fp, "MP4", 3 );
len += _lwrite( fp, (LPSTR)lpMap0, sizeof(MAP) );
len += _lwrite( fp, (LPSTR)lpMap1, sizeof(MAP) );
len += _lwrite( fp, (LPSTR)lpMap2, sizeof(MAP) );
len += _lwrite( fp, (LPSTR)lpMap3, sizeof(MAP) );
_lclose( fp );
AstralCursor( NULL );		/* Revert back to the old cursor */

if ( len != (3+(4*sizeof(MAP))) )
	{
	Message( IDS_EWRITE, Lowercase(lpFileName) );
	return( FALSE );
	}

return( TRUE );
}


/***********************************************************************/
int GetFileExtentionType(LPSTR lpFileName)
/***********************************************************************/
{
int Type;
LPSTR lp;
char tmp[4];

tmp[0] = tmp[1] = tmp[2] = tmp[3] = '\0';
lp = stripdir(lpFileName);
while ( *lp )
	{ // check for an extension
	if ( *lp++ != '.' )
		continue;
	// put extension into local buffer, can make lower case
	if ( *lp )
	{
		tmp[0] = tolower(*lp);
		lp++;
	}
	if ( *lp )
	{
		tmp[1] = tolower(*lp);
		lp++;
	}
	if ( *lp )
	{
		tmp[2] = tolower(*lp);
		lp++;
	}
	break;
	}

// Force the file type if the extension is a known one
Type = 0;
if ( StringsEqual( "art", tmp ) ) Type = IDN_ART;	else
if ( StringsEqual( "pp4", tmp ) ) Type = IDN_ART;	else
if ( StringsEqual( "tif", tmp ) ) Type = IDN_TIFF;	else
if ( StringsEqual( "bmp", tmp ) ) Type = IDN_BMP;	else
if ( StringsEqual( "dib", tmp ) ) Type = IDN_BMP;

return(Type);
}

/***********************************************************************/
ITEMID GetFrameFileDataType(LPFRAME lpFrame)
/***********************************************************************/
{
	ITEMID idDataType;

	switch(FrameType(lpFrame))
	{
		case FDT_LINEART :
			idDataType = IDC_SAVELA;
		break;

		case FDT_GRAYSCALE :
			idDataType = IDC_SAVECT;
		break;

		case FDT_PALETTECOLOR :
			idDataType = IDC_SAVE8BITCOLOR;
		break;

		case FDT_RGBCOLOR :
			idDataType = IDC_SAVE24BITCOLOR;
		break;

		case FDT_CMYKCOLOR :
			idDataType = IDC_SAVE32BITCOLOR;
		break;
	}

	return(idDataType);
}
