//®PL1¯®FD1¯®TP0¯®BT0¯®RM255¯
// (c) Copyright 1991 MICROGRAFX, Inc., All Rights Reserved.
// This material is confidential and a trade secret.
// Permission to use this work for any purpose must be obtained
// in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081
#include "pp.h"
#include "ppafx.h"
#ifndef WIN32
#include <ctype.h>
#endif
#include "id.h"

//************************************************************************
// extname.cpp    Extended name handling routines
//************************************************************************

#define CAT_DELIM _T('>')
#define SIZEOF_NAME_BUFFER 1600

// Static prototypes
LOCAL void CategorizeExtFiles(HWND hControl, BOOL bCombo);
LOCAL int AddToComboBox(LPTSTR lpExtName, LPTSTR lpFileName, ITEMID wType, long lData);
LOCAL int AddToListBox(LPTSTR lpExtName, LPTSTR lpFileName, ITEMID wType, long lData);
LOCAL void AddExtName( HWND hWnd, int idControl, int idStatic, LPTSTR lpExtName, ITEMID wType, int idEntry1 );
LOCAL void RenameExtName( HWND hWnd, int idControl, int idStatic, LPTSTR lpExtName, ITEMID wType, int idEntry1 );
LOCAL void DeleteExtName( HWND hWnd, int idControl, int idStatic, LPTSTR lpExtName, ITEMID wType, int idEntry1 );
LOCAL BOOL DuplicateFiles( LPTSTR lpFileName );
LOCAL BOOL RenameExtFile( LPTSTR lpExtName, LPTSTR lpNewExtName, ITEMID wType );
LOCAL BOOL GetFileFullPath( LPTSTR lpFileName, ITEMID wType );
LOCAL LPTSTR GetType( ITEMID wType );
LOCAL ITEMID GetTypeWord( LPTSTR lpType );
LOCAL BOOL IsCombo( HWND hWnd );
LOCAL LPTSTR LoadTypeString( ITEMID wType );
LOCAL LPTSTR GetNextTypeString( LPTSTR lpLoadedName, ITEMID wType, LPTSTR lpNameString, LPTSTR lpValueString );
LOCAL BOOL GetTypeString( ITEMID wType, LPTSTR lpNameString, LPTSTR lpValueString );
LOCAL BOOL PutTypeString( ITEMID wType, LPTSTR lpNameString, LPTSTR lpValueString );
LOCAL BOOL DeleteTypeString( ITEMID wType, LPTSTR lpNameString );
LOCAL BOOL MakeIniFileName( LPTSTR lpIniFileName );
LOCAL void FixFileName( LPTSTR lpFileName, BOOL bBaseIsFilename );
LOCAL void FixFileName( CString *FileName, BOOL bBaseIsFilename );

// Static Data
static LPTSTR lpLoadedNamePaths;
static FNAME szIniFile, szAddExtName;
static ITEMID wAddType;

/***********************************************************************/
void InitExtName( HWND hWnd, int idControl, int idStatic, LPTSTR lpExtName,
                     ITEMID wType )
/***********************************************************************/
{
InitExtNameN(hWnd, idControl, idStatic, lpExtName, wType, 0, NO);
}


/***********************************************************************/
void InitExtNameN( HWND hWnd, int idControl, int idStatic, LPTSTR lpExtName,
                     ITEMID wType, int idEntry1, BOOL fCategorize )
/***********************************************************************/
{
int i;
LPTSTR lp;
FNAME szExtName, szFileName;
HWND hControl;
BOOL bCombo;

if ( !(hControl = GetDlgItem(hWnd, idControl)) )
	return;
bCombo = IsCombo( hControl );
SendMessage( hControl, (bCombo ? CB_RESETCONTENT : LB_RESETCONTENT), 0, 0L );
if ( bCombo )
	{
    ComboBox_LimitText( hControl, MAX_EXTNAME_LEN - 1 );

	if ( idEntry1 && AstralStr( idEntry1, &lp ) )
		AddToComboBox( lp, NULL, 0, (long)hControl );

	ListExtFiles( wType, AddToComboBox, (long)hControl );
	}
else
	{
	if ( idEntry1 && AstralStr( idEntry1, &lp ) )
		AddToListBox( lp, NULL, 0, (long)hControl );
	ListExtFiles( wType, AddToListBox, (long)hControl );
	}

if ( idStatic )
	SetDlgItemText( hWnd, idStatic, _T("") );

if ( !(i = SendMessage( hControl, (bCombo ? CB_GETCOUNT : LB_GETCOUNT),0,0L)) )
	{
	*lpExtName = _T('\0');
	return;
	}

if ( fCategorize )
	CategorizeExtFiles( hControl, bCombo );

i = SendMessage( hControl, (bCombo ? CB_GETCOUNT : LB_GETCOUNT),0,0L);
while ( --i >= 0 )
	{
	SendMessage( hControl, (bCombo ? CB_GETLBTEXT : LB_GETTEXT),
		i, (long)(LPTSTR)szExtName);

	if ( !i || StringsEqual(lpExtName, szExtName) )
		{
		SendMessage( hControl, (bCombo ? CB_SETCURSEL : LB_SETCURSEL), i, 0L);
		break;
		}
	}

*lpExtName = _T('\0');
SendMessage( hControl, (bCombo ? CB_GETLBTEXT : LB_GETTEXT), i,	(long)lpExtName );
if ( idStatic && (!idEntry1 || i) &&
	 LookupExtFileN( lpExtName, szFileName, wType, NO ))
	{
	SetDlgItemText( hWnd, idStatic, Lowercase(filename(szFileName)) );
	}
}


/***********************************************************************/
LOCAL void CategorizeExtFiles(HWND hControl, BOOL bCombo)
/***********************************************************************/
{
int	i, iCount, length;
STRING  szExtName, szCategory, szNewCategory;

szCategory[0] = _T('\0');
iCount = SendMessage( hControl, (bCombo ? CB_GETCOUNT : LB_GETCOUNT), 0, 0L);

for ( i=0; i<iCount; i++ )
	{
	SendMessage( hControl, (bCombo ? CB_GETLBTEXT : LB_GETTEXT),
		i, (long)(LPTSTR)szExtName);
	if ( IsCategory(szExtName) )
		{
		GetCategory(szExtName, szCategory);
		continue;
		}
	if ( !GetCategory(szExtName, szNewCategory) ||
		 StringsEqual(szNewCategory, szCategory) )
		continue;

	lstrcpy(szCategory, szNewCategory);

	// put on the delimiter so this is recognized as a category name
	length = lstrlen(szNewCategory);
	szNewCategory[length] = CAT_DELIM;
	szNewCategory[length+1] = _T('\0');

	SendMessage( hControl, (bCombo ? CB_INSERTSTRING : LB_INSERTSTRING),
		i, (long)(LPTSTR)szNewCategory);
	++iCount;
	++i;
	}
}


/***********************************************************************/
LOCAL int AddToComboBox( LPTSTR lpExtName, LPTSTR lpFileName, ITEMID wType,
                              long lData )
/***********************************************************************/
{
return( SendMessage( (HWND)lData, CB_ADDSTRING, 0, (long)lpExtName ) );
}


/***********************************************************************/
LOCAL int AddToListBox( LPTSTR lpExtName, LPTSTR lpFileName, ITEMID wType,
                              long lData )
/***********************************************************************/
{
return( SendMessage( (HWND)lData, LB_ADDSTRING, 0, (long)lpExtName ) );
}


/***********************************************************************/
int GetExtName( HWND hWnd, int idControl, int idStatic, LPTSTR lpExtName,
                     ITEMID wType, UINT wMsg )
/***********************************************************************/
{
return( GetExtNameN( hWnd, idControl, idStatic, lpExtName, wType, wMsg, 0, NO));
}


/***********************************************************************/
int GetExtNameN( HWND hWnd, int idControl, int idStatic, LPTSTR lpExtName,
                     ITEMID wType, UINT wMsg, int idEntry1,	BOOL fCategorize )
/***********************************************************************/
{
int i = -1;
FNAME szFileName;
HWND hControl;
BOOL bCombo;

if ( !(hControl = GetDlgItem(hWnd, idControl)) )
	return(i);

bCombo = IsCombo( hControl );
if ( bCombo && wMsg == CBN_EDITCHANGE )
{
	GetDlgItemText( hWnd, idControl, lpExtName, MAX_FNAME_LEN );
    FixExtName( lpExtName );
}
else
if ( (bCombo && wMsg == CBN_SELCHANGE) || (!bCombo && wMsg == LBN_SELCHANGE) )
	{
	*lpExtName = _T('\0');
	i = SendMessage( hControl, (bCombo ? CB_GETCURSEL:LB_GETCURSEL), 0, 0L );
	SendMessage( hControl, (bCombo ? CB_GETLBTEXT : LB_GETTEXT), i,
		(long)lpExtName);
	if ( !idStatic )
		return(i);
	if ( !i && idEntry1 )
		{
		SetDlgItemText( hWnd, idStatic, _T("") );
		return(i);
		}
	if ( LookupExtFileN( lpExtName, szFileName, wType, YES ))
		SetDlgItemText( hWnd, idStatic, Lowercase(filename(szFileName)) );
	else
		{
		SetDlgItemText( hWnd, idStatic, _T("") );
		// re fill the combo
		InitExtNameN(hWnd,idControl,idStatic,lpExtName,wType,idEntry1,fCategorize);
		}
	}
return(i);
}


/***********************************************************************/
void ExtNameManager( HWND hWnd, int idControl, int idStatic, LPTSTR lpExtName,
                        ITEMID wType, int idFunction, int idEntry1 )
/***********************************************************************/
{
if ( !MakeIniFileName( szIniFile ) )
	return;

if (!FilePermission(szIniFile,  2 /*WRITE*/))  // write access?
	{
	Message( IDS_INI_NOWRITE, Lowercase(szIniFile) );
	return;
	}

switch ( idFunction )
    {
    case IDC_ADDEXT:
		AddExtName( hWnd, idControl, idStatic, lpExtName, wType, idEntry1 );
		break;
    case IDC_DELETEEXT:
		DeleteExtName( hWnd, idControl, idStatic, lpExtName, wType, idEntry1 );
		break;
    case IDC_RENAMEEXT:
		RenameExtName( hWnd, idControl, idStatic, lpExtName, wType, idEntry1 );
		break;
    default:
		break;
    }
}


/***********************************************************************/
LOCAL void AddExtName( HWND hWnd, int idControl, int idStatic,
                           LPTSTR lpExtName, ITEMID wType, int idEntry1 )
/***********************************************************************/
{
	int i;
	FNAME szFileName;
	OPENSTATE info;
	LPTSTR lpString, lpExtension;

	// Bring up the modal 'File Open' box 
	clr( (LPTR)&info, sizeof(info) );
	if ( AstralStr(IDS_FILEADD_TITLE, &lpString) )
		lstrcpy(info.szTitle, lpString);

	if ( AstralStr(IDS_FILEADD_BUTTON, &lpString) )
		lstrcpy(info.szOK, lpString);

	info.bImported = FALSE;
	info.bSaving = FALSE;
	info.idFileType = wType;
	info.szFileSpec[0] = info.szAlbum[0] = info.szPath[0] = _T('\0');
	info.wFileAttr = -1;
	info.idDataType = IDC_SAVELA;

	GetExtNamePath( info.szPath, wType );
	if ( !AstralStr( wType, &lpExtension ) )
		lstrcpy(info.szFileSpec, _T("*.*") );
	else
		lstrcpy(info.szFileSpec, lpExtension);

	if ( wType == IDN_TEXTURE || wType == IDN_CLIPBOARD || wType == IDN_MASK )
	{
		if ( !DoOpenDlg( hWnd, &info ) )
			return;
	}
	else
	{
		if ( !DoMiniOpenDlg( hWnd, &info ) )
			return;
	}

	wAddType = wType;
	for (i=0; i<info.numFiles;i++)
	{
		lstrcpy( szFileName, info.lpFileList[i] );
		// what about info.idFileType ?
		lstrcpy( szAddExtName, Lowercase(filename(szFileName)) );
		stripext( szAddExtName );

		if( IsDBCSLeadByte(( BYTE )*szAddExtName ))
			*(( LPWORD )szAddExtName )= MBToUpperW( szAddExtName );
		else
			*szAddExtName = MBToUpper( szAddExtName );

		if( !FileExists( szFileName ))
		{
			Message(IDS_BADFILE, (LPTSTR)szAddExtName);
			continue;
		}
		if ( !AstralDlg( NO, PictPubApp.GetResourceHandle(), hWnd, IDD_ADDEXTNAME, DlgAddExtNameProc ))
			break;
		if ( !AddExtFile( szAddExtName, szFileName, wType ) )
			break;
		lstrcpy( lpExtName, szAddExtName ); // pass back the name
		InitExtNameN( hWnd, idControl, idStatic, lpExtName, wType, idEntry1, NO );
		}

	FreeUp(info.lpFileList);
}


/***********************************************************************/
LOCAL void RenameExtName( HWND hWnd, int idControl, int idStatic,
                              LPTSTR lpExtName, ITEMID wType, int idEntry1 )
/***********************************************************************/
{
FNAME szFileName;

if ( !LookupExtFileN( lpExtName, szFileName, wType, YES ) )
	{
	InitExtNameN( hWnd, idControl, idStatic, lpExtName, wType, idEntry1, NO );
	return;
	}

if (!FilePermission(szFileName,  2 /*WRITE*/))  // write access?
	{
	Message( IDS_EWRITEPROTECT, Lowercase(szFileName) );
	return;
	}

wAddType = wType;
lstrcpy( szAddExtName, lpExtName );
if ( !AstralDlg( NO, PictPubApp.GetResourceHandle(), hWnd, IDD_ADDEXTNAME, DlgAddExtNameProc ) )
	return;
if ( RenameExtFile( lpExtName, szAddExtName, wType ) )
	{
	if ( wType == IDN_CLIPBOARD )
		RenameExtFile( lpExtName, szAddExtName, IDN_MASK );
	lstrcpy( lpExtName, szAddExtName ); // pass back the name
	}
InitExtNameN( hWnd, idControl, idStatic, lpExtName, wType, idEntry1, NO );
}


/***********************************************************************/
LOCAL void DeleteExtName( HWND hWnd, int idControl, int idStatic,
                              LPTSTR lpExtName, ITEMID wType, int idEntry1 )
/***********************************************************************/
{
FNAME szFileName;

if ( !LookupExtFileN( lpExtName, szFileName, wType, NO ) )
	{
	InitExtNameN( hWnd, idControl, idStatic, lpExtName, wType, idEntry1, NO );
	return;
	}

if (!FilePermission(szFileName,  2 /*WRITE*/))  // write access?
	{
	Message( IDS_EWRITEPROTECT, Lowercase(szFileName) );
	return;
	}

if ( AstralOKCancel( IDS_DELETEEXTFILE, GetType(wType), lpExtName,
	Lowercase(szFileName) ) != IDOK )
		return;
if ( DeleteExtFile( lpExtName, wType ) )
	{
	if ( wType == IDN_CLIPBOARD )
		DeleteExtFile( lpExtName, IDN_MASK );
	}
InitExtNameN( hWnd, idControl, idStatic, lpExtName, wType, idEntry1, NO );
}


/***********************************************************************/
BOOL PromptForExtName( HWND hWnd, LPTSTR lpExtName, LPTSTR lpFileName,
                           ITEMID wType )
/***********************************************************************/
{
if ( *lpExtName )
	lstrcpy( szAddExtName, lpExtName );
else
	lstrcpy( szAddExtName, _T("") );
wAddType = wType;
if ( !AstralDlg( NO, PictPubApp.GetResourceHandle(), hWnd, IDD_ADDEXTNAME, DlgAddExtNameProc ) )
	return(FALSE);
lstrcpy( lpExtName, szAddExtName );
if ( !LookupExtFileN( lpExtName, lpFileName, wType, NO ) )
	if ( !CreateFileName( lpExtName, wType, lpFileName ) )
		return( FALSE );
return(TRUE);
}


// LOCAL prototypes
LOCAL BOOL AddExtName_OnInitDialog(HWND hDlg, HWND hWndFocus, LPARAM lParam);
LOCAL void AddExtName_OnCommand(HWND hDlg, int id, HWND hControl, UINT codeNotify);

/***********************************************************************/
BOOL WINPROC EXPORT DlgAddExtNameProc( HWND hDlg, UINT msg, WPARAM wParam,
                                    LPARAM lParam )
/***********************************************************************/
{
switch (msg)
	{
	HANDLE_DLGRET(hDlg, WM_INITDIALOG, AddExtName_OnInitDialog);  
	HANDLE_DLGMSG(hDlg, WM_COMMAND, AddExtName_OnCommand);

	default:
	return Dialog_HandleDefaults(hDlg, msg, wParam, lParam, IDD_ADDEXTNAME);
	}
}

/************************************************************************/
LOCAL BOOL AddExtName_OnInitDialog(HWND hDlg, HWND hWndFocus, LPARAM lParam)
/************************************************************************/
{
LPTSTR lpTitle;
FNAME szFileName;

CenterPopup( hDlg );
if ( AstralStr( wAddType + MAX_FILETYPES, &lpTitle ) )
	SetWindowText( hDlg, lpTitle );
szFileName[0] = _T('\0');
InitExtName( hDlg, IDC_EXTNAMES, IDC_FILENAME, szFileName, wAddType);
SetDlgItemText( hDlg, IDC_EXTNAMES, szAddExtName );
return(TRUE);
}

/************************************************************************/
LOCAL void AddExtName_OnCommand(HWND hDlg, int id, HWND hControl, UINT codeNotify)
/************************************************************************/
{
	LPTSTR lp;
	FNAME szFileName;

	switch (id)
	{
		case IDC_EXTNAMES:
			GetExtName( hDlg, IDC_EXTNAMES, IDC_FILENAME,
				szAddExtName, wAddType, codeNotify );
			break;

		case IDOK:
			// Remove leading and trailing spaces first
			StripTrailer( szAddExtName, _T(' ') );
			lstrcpy( szAddExtName, SkipSpaces( szAddExtName ));

			// Make sure there are no illegal extended name characters
			while( lp = lStrChr( szAddExtName, _T('=') ))
				*lp = _T('-');

			if ( LookupExtFileN( szAddExtName, szFileName, wAddType, NO ) )
			{
				if (AstralOKCancel(IDS_OVERWRITEEXTFILE,
			    	(LPTSTR)szAddExtName) == IDCANCEL)
					break;
			}
			AstralDlgEnd( hDlg, TRUE );
			break;

		case IDCANCEL:
			AstralDlgEnd( hDlg, FALSE );
			break;

	   	default:
			break;
   	}
}

/************************************************************************/
int GetExtNameCount(ITEMID wType)
/************************************************************************/
{
LPTSTR lpTypeStrings, lpTypeNext;
FNAME szValueString, szNameString;
int count;

if ( !(lpTypeStrings = LoadTypeString( wType )) )
	return(-1);
lpTypeNext = lpTypeStrings;

count  = 0;
while ( lpTypeNext = GetNextTypeString(lpTypeNext, wType, szNameString, szValueString) )
	count++;
FreeUp(lpTypeStrings);
return( count );
}

/************************************************************************/
BOOL GetExtNameString(ITEMID wType, int index, LPTSTR lpName)
/************************************************************************/
{
LPTSTR lpTypeStrings, lpTypeNext;
FNAME szValueString, szNameString;
int count;

if (index >= GetExtNameCount( wType ))
	return( FALSE );

if ( !(lpTypeStrings = LoadTypeString( wType )) )
	return(FALSE);
lpTypeNext = lpTypeStrings;

count = 0;
while ( lpTypeNext = GetNextTypeString(lpTypeNext, wType, szNameString, szValueString) )
	{
	if (count == index)
		break;
	count++;
	}

FreeUp(lpTypeStrings);
if (count == index)
	{
	lstrcpy(lpName, szNameString);
	return( TRUE );
	}
else
	return( FALSE );
}

/************************************************************************/
BOOL LookupExtFile( LPTSTR lpExtName, LPTSTR lpFileName, ITEMID wType )
/************************************************************************/
{
return( LookupExtFileN( lpExtName, lpFileName, wType, YES ) );
}

/************************************************************************/
BOOL LookupExtFileN( LPTSTR lpExtName, LPTSTR lpFileName, ITEMID wType, BOOL bFileCheck )
/************************************************************************/
{
LPTSTR lpTypeName, lpTypeStrings, lpTypeNext;
FNAME szValueString, szNameString;
BOOL fRet = FALSE;

if ( !lstrlen( lpExtName ) )
	return( FALSE );

if ( !(lpTypeStrings = LoadTypeString( wType )) )
	return(FALSE);
lpTypeNext = lpTypeStrings;

while ( lpTypeNext = GetNextTypeString(lpTypeNext, wType, szNameString, szValueString) )
	{
	if ( !StringsEqual(szNameString, lpExtName) )
		continue;
	if ( !GetFileFullPath( szValueString, wType ) && bFileCheck )
		{ // The file doesn't exist...
		lpTypeName = GetType( wType );
		if ( AstralAffirm( IDS_EXTNAME_BADFILE, lpTypeName, lpExtName,
				Lowercase(szValueString), lpTypeName ) &&
			 AstralOKCancel( IDS_DELETEEXTFILE, lpTypeName, lpExtName,
				Lowercase(szValueString) ) == IDOK )
					DeleteExtFile( lpExtName, wType );
		break;
		}
	lstrcpy( lpFileName, szValueString );
	fRet = TRUE;
	break;
	}
FreeUp((LPTR)lpTypeStrings);
return(fRet);
}


/************************************************************************/
BOOL AddExtFile( LPTSTR lpExtName, LPTSTR lpFileName, ITEMID wType )
/************************************************************************/
{
FNAME szFileName, szNewPath, szOldPath;

if ( LookupExtFileN( lpExtName, szFileName, wType, NO ) )
	{ // If the name exists already...
	if ( StringsEqual( szFileName, lpFileName ) ) // and same file name too...
		return( TRUE );
	DeleteExtFile( lpExtName, wType );
	}

if ( Control.bNetworkInstall )
	{ // All new files created in the pouch path (name.ini path)
	lstrcpy( szNewPath, Control.PouchPath );
	}
else
	{ // Otherwise, use the normal location for this file type
	if ( !GetExtNamePath( szNewPath, wType ) )
		lstrcpy( szNewPath, Control.PouchPath );
	}

lstrcpy( szOldPath, lpFileName );
stripfile( szOldPath );

if ( StringsEqual( szOldPath, szNewPath ) )
	{ // If its already in the pouch...
	lstrcpy(szFileName, lpFileName);
	}
else
	{ // If its not already in the pouch...
	if ( !CreateFileName( lpFileName, wType|0x8000, szFileName ) )
		{
		Message( IDS_EXTNAME_CREATE, (LPTSTR)szFileName );
		return(FALSE);
		}
	if ( !FileCopy(lpFileName, szFileName) )
		{
		Message( IDS_EXTNAME_COPY, lpFileName, (LPTSTR)szFileName );
		return(FALSE);
		}
	}
return( PutTypeString( wType, lpExtName, filename(szFileName) ) );
}


/************************************************************************/
BOOL DeleteExtFile( LPTSTR lpExtName, ITEMID wType )
/************************************************************************/
{
FNAME szValueString, szNameString;
LPTSTR lpTypeStrings, lpTypeNext;

if ( !(lpTypeStrings = LoadTypeString( wType )) )
	return(FALSE);
lpTypeNext = lpTypeStrings;

while ( lpTypeNext = GetNextTypeString(lpTypeNext, wType, szNameString, szValueString) )
	{ // Delete all occurances of the same type
	if ( !StringsEqual(szNameString, lpExtName) )
		continue;
	DeleteTypeString( wType, szNameString);
	// Look for another usage of this file before deleting it
	if ( GetFileFullPath( szValueString, wType ) &&
		!DuplicateFiles( szValueString ) )
			FileDelete( szValueString );
	}

FreeUp((LPTR)lpTypeStrings);
return(TRUE);
}


/************************************************************************/
LOCAL BOOL DuplicateFiles( LPTSTR lpFileName )
/************************************************************************/
{
LPTSTR lpTypeStrings, lpTypeNext;
FNAME szValueString, szNameString;
ITEMID wType;

for ( wType = IDN_FIRSTEXTTYPE; wType <= IDN_LASTEXTTYPE; wType++ )
	{
	if ( !(lpTypeStrings = LoadTypeString( wType )) )
		return(FALSE);
	lpTypeNext = lpTypeStrings;

	while ( lpTypeNext = GetNextTypeString(lpTypeNext, wType, szNameString, szValueString) )
		{
		if ( GetFileFullPath( szValueString, wType ) &&
			 StringsEqual( lpFileName, szValueString ) )
			 	{
				FreeUp(lpTypeStrings);
				return( YES );
				}
		}
	FreeUp(lpTypeStrings);
	}

return( NO );
}


/************************************************************************/
LOCAL BOOL RenameExtFile( LPTSTR lpExtName, LPTSTR lpNewExtName, ITEMID wType )
/************************************************************************/
{
FNAME szValueString, szNameString;
LPTSTR lpTypeStrings, lpTypeNext;

if ( StringsEqual( lpExtName, lpNewExtName ) )
	return( TRUE );

if ( !(lpTypeStrings = LoadTypeString( wType )) )
	return(FALSE);
lpTypeNext = lpTypeStrings;

while ( lpTypeNext = GetNextTypeString(lpTypeNext, wType, szNameString, szValueString) )
	{ // Rename all occurances of the same type
	if ( !StringsEqual( lpExtName, szNameString ) )
		continue;
	if ( !PutTypeString( wType, lpNewExtName, szValueString ) )
		{
		FreeUp(lpTypeStrings);
		return(FALSE);
		}
	DeleteTypeString( wType, lpExtName );
  	}
FreeUp(lpTypeStrings);
return(TRUE);
}


/************************************************************************/
BOOL ListExtFiles(ITEMID wType, EXTADDPROC lpAddFunction, long lData)
/************************************************************************/
{
FNAME szValueString, szNameString;
LPTSTR lpTypeStrings, lpTypeNext;
int num_files;

if ( !(lpTypeStrings = LoadTypeString( wType )) )
	return(FALSE);
lpTypeNext = lpTypeStrings;

num_files = 0;
while ( lpTypeNext = GetNextTypeString(lpTypeNext, wType, szNameString, szValueString) )
	{
	(*lpAddFunction)((LPTSTR)szNameString, (LPTSTR)szValueString, (ITEMID)wType,
		lData);
	++num_files;
	}
FreeUp(lpTypeStrings);
return( num_files != 0 );
}


/************************************************************************/
BOOL CreateFileName( LPTSTR lpBaseName, int wType, LPTSTR lpFileName,
					LPTSTR lpPathName)
/************************************************************************/
{
	FNAME szExtension;
	FILE_HANDLE ofp;
	FNAME outfile, num_string, path, outname;
	int num, len;
	BOOL bBaseIsFilename;
	STRING szFileType;

	bBaseIsFilename = wType & 0x8000;
	wType &= 0x7FFF;

	if (lpPathName) // someone passed in a pathname
		lstrcpy(path, lpPathName);
	else
	if ( Control.bNetworkInstall )	// All new files created in the pouch path (name.ini path)
		lstrcpy( path, Control.PouchPath );
	else  							// Otherwise, use the normal location for this file type
	{ 
		if ( !GetExtNamePath( path, wType ) )
			lstrcpy( path, Control.PouchPath );
	}

	lstrcpy( outname, filename(lpBaseName) );
	FixFileName( outname, bBaseIsFilename );
	lstrcpy( szExtension, extension(outname) );
	stripext(outname);
	if ( !szExtension[0] ) // if no extension...
	{
		if ( !AstralStrEx( wType, szFileType, sizeof(szFileType) ) )
			lstrcpy( szExtension, _T("*.*") );
		else	
			lstrcpy( szExtension, szFileType );

		lstrcpy( szExtension, extension(szExtension) );
	}

	num = 0;
	num_string[0] = _T('\0');
	len = lstrlen( outname );
	while ( TRUE )
	{
		while ( len && (len + lstrlen(num_string) > 8 ))
			outname[--len] = _T('\0');
		if ( !len )
			return(FALSE);
		lstrcpy(outfile, path);
		lstrcat(outfile, outname);
		lstrcat(outfile, num_string);
		lstrcat(outfile, szExtension);
		if ( (ofp = FileOpen(outfile, FO_READ)) == FILE_HANDLE_INVALID)
			break;
		FileClose(ofp);
		sprintf(num_string, "%d", num);
		num++;
	}

	lstrcpy( lpFileName, outfile );
	return(TRUE);
}


/************************************************************************/
BOOL FormFileName( LPTSTR lpOutPathFileName, int wType,  LPTSTR lpFileName)
/************************************************************************/
{
	FNAME path;
	ASSERT(lpFileName);
	ASSERT(lpOutPathFileName);

	wType &= 0x7FFF;
	if ( Control.bNetworkInstall )
	{ // All new files created in the pouch path (name.ini path)
		lstrcpy( path, Control.PouchPath );
	}
	else
	{ // Otherwise, use the normal location for this file type
		if ( !GetExtNamePath( path, wType ) )
			lstrcpy( path, Control.PouchPath );
	}

	lstrcpy( lpOutPathFileName, path );
	lstrcat( lpOutPathFileName, filename(lpFileName));

	return(TRUE);
}

/************************************************************************/
BOOL FileName( LPTSTR lpBaseName, int wType, LPTSTR lpFileName,
					LPTSTR lpPathName)
/************************************************************************/
{
	FNAME szExtension;
	FILE_HANDLE ofp;
	FNAME outfile, num_string, path, outname;
	int num, len;
	BOOL bBaseIsFilename;
	STRING szFileType;

	bBaseIsFilename = wType & 0x8000;
	wType &= 0x7FFF;

	if (lpPathName) // someone passed in a pathname
		lstrcpy(path, lpPathName);
	else
	if ( Control.bNetworkInstall )	// All new files created in the pouch path (name.ini path)
		lstrcpy( path, Control.PouchPath );
	else							// Otherwise, use the normal location for this file type
	{ 
		if ( !GetExtNamePath( path, wType ) )
			lstrcpy( path, Control.PouchPath );
	}

	lstrcpy( outname, filename(lpBaseName) );
	FixFileName( outname, bBaseIsFilename );
	lstrcpy( szExtension, extension(outname) );
	stripext(outname);
	if ( !szExtension[0] ) // if no extension...
	{
		if ( !AstralStrEx( wType, szFileType, sizeof(szFileType) ) )
			lstrcpy( szExtension, _T("*.*") );
		else	
			lstrcpy( szExtension, szFileType );

		lstrcpy( szExtension, extension(szExtension) );
	}

	num = 0;
	num_string[0] = _T('\0');
	len = lstrlen( outname );
	while ( TRUE )
	{
		while ( len && (len + lstrlen(num_string) > 8) )
			outname[--len] = _T('\0');
		if ( !len )
			return(FALSE);

		lstrcpy(outfile, path);
		lstrcat(outfile, outname);
		lstrcat(outfile, num_string);
		lstrcat(outfile, szExtension);
		if ( (ofp = FileOpen(outfile, FO_READ)) == FILE_HANDLE_INVALID)
			break;
		FileClose(ofp);
		sprintf(num_string, "%d", num);
		num++;
	}

	lstrcpy( lpFileName, outfile );
	return(TRUE);
}


/************************************************************************/
LOCAL BOOL GetFileFullPath( LPTSTR lpFileName, ITEMID wType )
/************************************************************************/
{
	FNAME path, szFileName;
	BOOL bExists;

	lstrcpy(szFileName, lpFileName);

		
	if ( !lStrChr(szFileName, _T(':')) && !lStrChr(szFileName, _T('\\')) &&
	 	GetExtNamePath( path, wType ) )
	{ // if it is not a full path already, start with the standard location
		lstrcpy( lpFileName, path );
		lstrcat( lpFileName, szFileName );
	}

	if ( !(bExists = FileExists( lpFileName )) )
	{ // if it's not in the standard location, try the pouch
		lstrcpy( path, lpFileName ); // Remember the standard location
		lstrcpy( lpFileName, Control.PouchPath );
		lstrcat( lpFileName, szFileName );
		if ( !(bExists = FileExists( lpFileName )) )
			lstrcpy( lpFileName, path ); // Restore the default location b4 error
	}

	return( bExists );
}


/************************************************************************/
BOOL LoadAllExtNamePaths( BOOL bClose )
/************************************************************************/
{
ITEMID wType;
long lSize;
LPTSTR lp;
LPTSTR lpDefault;
PSTR pIniName;
FNAME szTemp;


#ifdef ID_MAGIC
// Don't load the paths, so we always default to PouchPath
	return( TRUE );
#endif

if ( lpLoadedNamePaths )
	FreeUp( lpLoadedNamePaths );
if ( bClose )
	{
	lpLoadedNamePaths = NULL;
	return( TRUE );
	}
lSize = (IDN_LASTEXTTYPE - IDN_FIRSTEXTTYPE + 1) * sizeof(FNAME);
if ( !(lpLoadedNamePaths = (LPTSTR)Alloc( lSize )) )
	return( FALSE );

lp = lpLoadedNamePaths;
for ( wType = IDN_FIRSTEXTTYPE; wType <= IDN_LASTEXTTYPE; wType++ )
	{
	lpDefault = Control.PouchPath;
	switch ( wType )
	    {
	    case IDN_TEXTURE:
			pIniName = _T("Texture");
			break;
	    case IDN_SPECIAL:
			pIniName = _T("Effects");
			break;
	    case IDN_CLRMGR:
	    case IDN_SCANNER:
	    case IDN_GRABBER:
	    case IDN_SCANMAP:
	    case IDN_EXPORT:
			pIniName = _T("Device");
			break;
	    case IDN_HALFTONE:
	    case IDN_SEPARATE:
	    case IDN_BGMAP:
	    case IDN_CALMAP:
	    case IDN_STYLE:
	    case IDN_MAP:
		case IDN_PCDLUT:
			pIniName = _T("PrnStyle");
			break;
	    case IDN_CLIPBOARD:
	    case IDN_MASK:
			pIniName = _T("Clipbit");
			break;
		case IDN_SHAPE:
			// default to Clipbits
			PictPubApp.GetLocalProfileString( _T("Clipbit"), Control.PouchPath, szTemp, sizeof(FNAME) );
			pIniName = _T("ShapePath");
			lpDefault = szTemp;
			break;
	    case IDN_CUSTOM:
	    case IDN_PAINTSTYLE:
	    case IDN_SPRAYSTYLE:
	    case IDN_CLONESTYLE:
		case IDN_TEXTURESTYLE:
	    case IDN_SMEARSTYLE:
	    case IDN_ERASESTYLE:
		case IDN_SHARPSTYLE:
		case IDN_SMOOTHSTYLE:
		case IDN_LIGHTENSTYLE:
		case IDN_DARKENSTYLE:
	    case IDN_DRAWSHAPESTYLE:
	    case IDN_DRAWFREESTYLE:
	    case IDN_PENCILSTYLE:
	    case IDN_STROKESTYLE:
			pIniName = _T("Brush");
			break;
	    case IDN_PALETTE:
			pIniName = _T("Palette");
			break;
	    case IDN_MACRO:
			pIniName = _T("Macros");
			break;
        case IDN_IMPORTFILTERS:
			pIniName = _T("Filters");
			break;
	    default:
			pIniName = _T("");
			break;
	    }

	PictPubApp.GetLocalProfileString( pIniName, Control.PouchPath, lp, sizeof(FNAME) );
	FixPath( lp );
	lp += sizeof(FNAME);
	}

return( TRUE );
}


/************************************************************************/
BOOL GetExtNamePath( LPTSTR lpPath, ITEMID wType )
/************************************************************************/
{
if ( !lpLoadedNamePaths )
	LoadAllExtNamePaths( NO );

if ( !lpLoadedNamePaths || wType < IDN_FIRSTEXTTYPE || wType > IDN_LASTEXTTYPE )
	{
	lstrcpy( lpPath, Control.PouchPath );
	return( TRUE );
	}

wType -= IDN_FIRSTEXTTYPE;
lstrcpy( lpPath, lpLoadedNamePaths + (wType * sizeof(FNAME)) );
return( TRUE );
}


/************************************************************************/
BOOL IsCategory(LPTSTR lpString)
/************************************************************************/
{
	STRING	szString;
	LPTSTR	lpTemp;

	lstrcpy( szString, lpString );
	lpTemp = lStrChr( szString, CAT_DELIM );

	if( lpTemp && ( *MyCharNext( lpTemp ) == _T('\0') ))
		return( YES );

	return(NO);
}


/************************************************************************/
BOOL GetCategory(LPTSTR lpString, LPTSTR lpCategory)
/************************************************************************/
{
	STRING	szString;
	LPTSTR	lpTemp;

	lstrcpy( szString, lpString );

	if( lpTemp = lStrChr( szString, CAT_DELIM ))
	{
		*lpTemp = _T('\0');

		if( lpCategory )
			lstrcpy( lpCategory, szString );
		return( lstrlen( szString ) > 0 );
	}
	return( FALSE );
}


/************************************************************************/
BOOL GetCategoryName(LPTSTR lpString, LPTSTR lpName)
/************************************************************************/
{
	STRING	szString;
	LPTSTR	lpTemp;

	lstrcpy( szString, lpString );
	if( lpTemp = lStrChr( szString, CAT_DELIM ))
	{
		if( lpName )
			lstrcpy( lpName, MyCharNext( lpTemp ));
		return( lstrlen( MyCharNext( lpTemp )) > 0 );
	}
	if( lpName )
		lstrcpy( lpName, lpString );
	return( FALSE );
}


/***********************************************************************/
LOCAL LPTSTR GetType( ITEMID wType )
/***********************************************************************/
{
switch ( wType )
    {
    case IDN_MAP:
		return( _T("Map") );
    case IDN_BGMAP:
		return( _T("BGMap") );
    case IDN_SCANMAP:
		return( _T("ScanMap") );
    case IDN_CALMAP:
		return( _T("CalMap") );
    case IDN_HALFTONE:
		return( _T("Halftone") );
    case IDN_SEPARATE:
		return( _T("Separation") );
    case IDN_TEXTURE:
		return( _T("Texture") );
    case IDN_SPECIAL:
		return( _T("Special") );
    case IDN_SCANNER:
		return( _T("Scanner") );
    case IDN_GRABBER:
		return( _T("Grabber") );
    case IDN_STYLE:
		return( _T("Style") );
    case IDN_CLIPBOARD:
		return( _T("Clipboard") );
    case IDN_MASK:
		return( _T("Mask") );
    case IDN_SHAPE:
		return( _T("Shape") );
    case IDN_CUSTOM:
		return( _T("Custom") );
    case IDN_PALETTE:
		return( _T("Palette") );
    case IDN_PAINTSTYLE:
		return( _T("PaintStyle") );
    case IDN_SPRAYSTYLE:
		return( _T("SprayStyle") );
    case IDN_CLONESTYLE:
		return( _T("CloneStyle") );
    case IDN_TEXTURESTYLE:
		return( _T("TextureStyle") );
    case IDN_SMEARSTYLE:
		return( _T("SmearStyle") );
    case IDN_SHARPSTYLE:
		return( _T("SharpStyle") );
    case IDN_SMOOTHSTYLE:
		return( _T("SmoothStyle") );
    case IDN_LIGHTENSTYLE:
		return( _T("LightenStyle") );
    case IDN_DARKENSTYLE:
		return( _T("DarkenStyle") );
    case IDN_ERASESTYLE:
		return( _T("EraserStyle") );
    case IDN_DRAWSHAPESTYLE:
		return( _T("DrawShapeStyle") );
    case IDN_DRAWFREESTYLE:
		return( _T("DrawFreeStyle") );
    case IDN_PENCILSTYLE:
		return( _T("PencilStyle") );
    case IDN_STROKESTYLE:
		return( _T("StrokeStyle") );
    case IDN_MACRO:
		return( _T("Macro") );
    case IDN_CLRMGR:
		return( _T("ColorManager") );
    case IDN_PCDLUT:
		return( _T("PCDTables") );
    default:
		return( _T("Unknown") );
    }
}


/***********************************************************************/
LOCAL ITEMID GetTypeWord( LPTSTR lpType )
/***********************************************************************/
{
if ( StringsEqual( lpType, _T("Map") ) )
	return( IDN_MAP );
if ( StringsEqual( lpType, _T("BGMap") ) )
	return( IDN_BGMAP );
if ( StringsEqual( lpType, _T("ScanMap") ) )
	return( IDN_SCANMAP );
if ( StringsEqual( lpType, _T("CalMap") ) )
	return( IDN_CALMAP );
if ( StringsEqual( lpType, _T("Halftone") ) )
	return( IDN_HALFTONE );
if ( StringsEqual( lpType, _T("Separation") ) )
	return( IDN_SEPARATE );
if ( StringsEqual( lpType, _T("Texture") ) )
	return( IDN_TEXTURE );
if ( StringsEqual( lpType, _T("Special") ) )
	return( IDN_SPECIAL );
if ( StringsEqual( lpType, _T("Scanner") ) )
	return( IDN_SCANNER );
if ( StringsEqual( lpType, _T("Grabber") ) )
	return( IDN_GRABBER );
if ( StringsEqual( lpType, _T("Style") ) )
	return( IDN_STYLE );
if ( StringsEqual( lpType, _T("Clipboard") ) )
	return( IDN_CLIPBOARD );
if ( StringsEqual( lpType, _T("Mask") ) )
	return( IDN_MASK );
if ( StringsEqual( lpType, _T("Shape") ) )
	return( IDN_SHAPE );
if ( StringsEqual( lpType, _T("Custom") ) )
	return( IDN_CUSTOM );
if ( StringsEqual( lpType, _T("Palette") ) )
	return( IDN_PALETTE );
if ( StringsEqual( lpType, _T("PaintStyle") ) )
	return( IDN_PAINTSTYLE );
if ( StringsEqual( lpType, _T("SprayStyle") ) )
	return( IDN_SPRAYSTYLE );
if ( StringsEqual( lpType, _T("TextureStyle") ) )
	return( IDN_TEXTURESTYLE );
if ( StringsEqual( lpType, _T("CloneStyle") ) )
	return( IDN_CLONESTYLE );
if ( StringsEqual( lpType, _T("SmearStyle") ) )
	return( IDN_SMEARSTYLE );
if ( StringsEqual( lpType, _T("SharpStyle") ) )
	return( IDN_SHARPSTYLE );
if ( StringsEqual( lpType, _T("SmoothStyle") ) )
	return( IDN_SMOOTHSTYLE );
if ( StringsEqual( lpType, _T("LightenStyle") ) )
	return( IDN_LIGHTENSTYLE );
if ( StringsEqual( lpType, _T("DarkenStyle") ) )
	return( IDN_DARKENSTYLE );
if ( StringsEqual( lpType, _T("EraserStyle") ) )
	return( IDN_ERASESTYLE );
if ( StringsEqual( lpType, _T("DrawShapeStyle") ) )
	return( IDN_DRAWSHAPESTYLE );
if ( StringsEqual( lpType, _T("DrawFreeStyle") ) )
	return( IDN_DRAWFREESTYLE );
if ( StringsEqual( lpType, _T("PencilStyle") ) )
	return( IDN_PENCILSTYLE );
if ( StringsEqual( lpType, _T("StrokeStyle") ) )
	return( IDN_STROKESTYLE );
if ( StringsEqual( lpType, _T("Macro") ) )
	return( IDN_MACRO );
if ( StringsEqual( lpType, _T("ColorManager") ) )
	return( IDN_CLRMGR );
if ( StringsEqual( lpType, _T("PCDTables") ) )
	return( IDN_PCDLUT );
return( 1 ); // An invalid type
}


/***********************************************************************/
LOCAL BOOL IsCombo( HWND hWnd )
/***********************************************************************/
{
STRING szClass;

GetClassName(hWnd, szClass, sizeof(szClass));
return( lstrfind( szClass, _T("combobox") ) != NULL );
}


/************************************************************************/
LOCAL LPTSTR LoadTypeString( ITEMID wType )
/************************************************************************/
{
LPTSTR lpTypeStrings;
STRING szIniFile;

if ( !MakeIniFileName( szIniFile ) )
	return( NULL );

if ( !(lpTypeStrings = (LPTSTR)Alloc( (long)SIZEOF_NAME_BUFFER )) )
	return( NULL );

if ( !GetPrivateProfileString( GetType(wType), NULL, NULL, lpTypeStrings,
	SIZEOF_NAME_BUFFER, szIniFile ) )
		return( NO );

return( lpTypeStrings );
}


/************************************************************************/
LOCAL LPTSTR GetNextTypeString( LPTSTR lpLoadedName, ITEMID wType, LPTSTR lpNameString, LPTSTR lpValueString )
/************************************************************************/
{
	if( !lpLoadedName || *lpLoadedName == _T('\0') )
		return( NO );

	if( GetTypeString( wType, lpLoadedName, lpValueString ))
	{
		lstrcpy( lpNameString, lpLoadedName );
		lpLoadedName += lstrlen( lpLoadedName );
		lpLoadedName++;
		return( lpLoadedName );
	}
	return(NULL);
}


/************************************************************************/
LOCAL BOOL GetTypeString( ITEMID wType, LPTSTR lpNameString, LPTSTR lpValueString )
/************************************************************************/
{
	STRING szIniFile;

	if( !MakeIniFileName( szIniFile ))
		return( NO );

	GetPrivateProfileString( GetType(wType), lpNameString, _T("")/*Default*/,
		lpValueString, sizeof(STRING), szIniFile );

	return( TRUE /*lpValueString[0] != _T('\0')*/ );
}


/************************************************************************/
LOCAL BOOL PutTypeString( ITEMID wType, LPTSTR lpNameString, LPTSTR lpValueString )
/************************************************************************/
{
if ( !MakeIniFileName( szIniFile ) )
	return( NO );

return( WritePrivateProfileString( GetType(wType), lpNameString,
	lpValueString, szIniFile ));
}


/************************************************************************/
LOCAL BOOL DeleteTypeString( ITEMID wType, LPTSTR lpNameString )
/************************************************************************/
{
return( PutTypeString( wType, lpNameString, NULL ) );
}


/************************************************************************/
LOCAL BOOL MakeIniFileName( LPTSTR lpIniFileName )
/************************************************************************/
{
FNAME szFileName;
STRING szNamesFile;

if ( !AstralStrEx( IDS_NAMESFILE, szNamesFile, sizeof(szNamesFile) ) )
	return( NO );

lstrcpy( szFileName, Control.PouchPath );
lstrcat( szFileName, szNamesFile );
if ( !FileExists( szFileName ) )
	{
	lstrcpy( szFileName, szNamesFile );
	GetIniPath( szFileName );
	lstrcpy( Control.PouchPath, stripfile(szFileName) );
	}

lstrcpy( lpIniFileName, szFileName );
return( YES );
}

///////////////////////////////////////////////////////////////////////////
LOCAL void FixFileName( LPTSTR lpFileName, BOOL bBaseIsFilename )
///////////////////////////////////////////////////////////////////////////
{
    FNAME 	szFileName;
    LPTSTR 	lpi, lpj;
	WORD 	wChar;

    lstrcpyn( szFileName, lpFileName, MAX_FNAME_LEN );
    lpi = lpj = szFileName;

    while( *lpi )
    {
		if( IsDBCSLeadByte(( BYTE )*lpi ))
			wChar = *(( LPWORD )lpi );
		else
			wChar = *lpi;

	    if(( wChar != _T(' ') ) &&
   	       ( wChar != _T('*') ) &&
   	       ( wChar != _T('?') ) &&
	       ( wChar != _T(':') ) &&
	       ( wChar != _T(';') ) &&
	       ( wChar != _T('+') ) &&
	       ( wChar != _T(',') ) &&
	       ( bBaseIsFilename || wChar != _T('.') ))  
        {
            if( IsDBCSLeadByte(( BYTE )*lpi ))
    		    *(( LPWORD )lpj )= *(( LPWORD )lpi );
            else
    		    *lpj = *lpi;
            lpj = MyCharNext( lpj );
        }
        lpi = MyCharNext( lpi );
    }
    *lpj = _T('\0');                        // NULL-terminate string

    lstrcpy( lpFileName, szFileName );  // pass back corrected file name
}


///////////////////////////////////////////////////////////////////////////
LOCAL void FixFileName( CString *FileName, BOOL bBaseIsFilename )
///////////////////////////////////////////////////////////////////////////
{
    FNAME szFileName; 

    lstrcpyn( szFileName, *FileName, sizeof( FNAME ));
    FixFileName( szFileName, bBaseIsFilename );
    *FileName = szFileName;
}


//************************************************************************
// 		LPGETFILEPROC
// PARAMETERS:
//					LPTSTR lpFileName- String to return file name in.
//					LPTSTR lpExtName- The extended name to look up.
//					WORD wType- The type to look up.
// DESCRIPTION:
// 				Callback for extended browser.
//				Gets a full file name from the extended name.
//				Does NOT prompt user if there is no such File.
// RETURNS:
//			FALSE if there is no such extended name, or if there is no file 
//			associated with it.
//************************************************************************
BOOL GetExtFileName_Bsr(LPTSTR lpFileName, LPTSTR lpExtName, WORD wType)
//************************************************************************
{
	lpFileName[0] = _T('\0');
	if (!LookupExtFileN(lpExtName, lpFileName, wType, NO ))
		return(FALSE);
	return(TRUE);
}


///////////////////////////////////////////////////////////////////////////
void FixExtName( LPTSTR lpExtName )
///////////////////////////////////////////////////////////////////////////
{
    TCHAR 	szExtendedName[ MAX_EXTNAME_LEN ];
    LPTSTR 	lpi, lpj;
	WORD 	wChar;

    lpi = lpj = szExtendedName;

    lstrcpyn( szExtendedName, lpExtName, MAX_EXTNAME_LEN );
    while( *lpi )
    {
		if( IsDBCSLeadByte(( BYTE )*lpi ))
			wChar = *(( LPWORD )lpi );
		else
			wChar = *lpi;

	    if(( wChar != _T('*') ) &&
   	       ( wChar != _T('?') ) &&
	       ( wChar != _T(':') ) &&
	       ( wChar != _T(';') ) &&
	       ( wChar != _T('+') ) &&
	       ( wChar != _T(',') ) &&
	       ( wChar != _T('.') ))  
        {
            if( IsDBCSLeadByte(( BYTE )*lpi ))
    		    *(( LPWORD )lpj )= *(( LPWORD )lpi );
            else
    		    *(( LPBYTE )lpj )= *(( LPBYTE )lpi );
            lpj = MyCharNext( lpj );
        }
        lpi = MyCharNext( lpi );
    }
    *lpj = _T('\0');

    lpj = SkipSpaces( szExtendedName );	// strip off leading blank spaces

    lstrcpy( lpExtName, lpj );  // pass back corrected extended name
}

///////////////////////////////////////////////////////////////////////////
void FixExtName( CString *ExtName )
///////////////////////////////////////////////////////////////////////////
{
    FNAME szExtendedName; 

    lstrcpyn( szExtendedName, *ExtName, MAX_EXTNAME_LEN );
    FixExtName( szExtendedName );
    *ExtName = szExtendedName;
}


