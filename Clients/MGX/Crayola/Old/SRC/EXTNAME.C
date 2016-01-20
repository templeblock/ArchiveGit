//®PL1¯®FD1¯®TP0¯®BT0¯®RM255¯
// (c) Copyright 1991 MICROGRAFX, Inc., All Rights Reserved.
// This material is confidential and a trade secret.
// Permission to use this work for any purpose must be obtained
// in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081
#include <windows.h>
#include "id.h"
#include "data.h"
#include "routines.h"

//************************************************************************
// extname.c    Extended name handling routines
//************************************************************************

typedef int (*LPADDPROC)(LPSTR, LPSTR, WORD, long);

// Static prototypes
static int AddToComboBox(LPSTR lpExtName, LPSTR lpFileName, WORD wType, long lData);
static int AddToListBox(LPSTR lpExtName, LPSTR lpFileName, WORD wType, long lData);
static BOOL ListExtFiles(WORD wType, LPADDPROC lpAddFunction, long lData);
static BOOL GetFileFullPath( LPSTR lpFileName, WORD wType );
static LPSTR GetType( WORD wType );
static WORD GetTypeWord( LPSTR lpType );
static BOOL IsCombo( HWND hWnd );
static BOOL LoadTypeStrings( WORD wType );
static BOOL GetNextTypeString( WORD wType, LPSTR lpNameString, LPSTR lpValueString );
static BOOL GetTypeString( WORD wType, LPSTR lpNameString, LPSTR lpValueString );
static BOOL MakeIniFileName( LPSTR lpIniFileName );

// Static Data
static LPSTR lpLoadedName;
static FNAME szIniFile;


/***********************************************************************/
void InitExtName(
/***********************************************************************/
HWND 	hWnd,
int 	idControl,
int		idStatic,
LPSTR 	lpExtName,
WORD 	wType)
{
InitExtNameN(hWnd, idControl, idStatic, lpExtName, wType, 0, NO);
}


/***********************************************************************/
void InitExtNameN(
/***********************************************************************/
HWND 	hWnd,
int 	idControl,
int		idStatic,
LPSTR 	lpExtName,
WORD 	wType,
int 	idEntry1,
BOOL 	fCategorize)
{
int i;
LPSTR lp;
FNAME szExtName, szFileName;
HWND hControl;
BOOL bCombo;

if ( !(hControl = GetDlgItem(hWnd, idControl)) )
	return;
bCombo = IsCombo( hControl );
SendMessage( hControl, (bCombo ? CB_RESETCONTENT : LB_RESETCONTENT), 0, 0L );
if ( bCombo )
	{
	SendMessage( hControl, CB_LIMITTEXT, MAX_STR_LEN-2, 0L );
	if ( idEntry1 && AstralStr( idEntry1, &lp ) )
		AddToComboBox( lp, NULL, 0, (long)(LPTR)hControl );
	ListExtFiles( wType, AddToComboBox, (long)(LPTR)hControl );
	}
else
	{
	if ( idEntry1 && AstralStr( idEntry1, &lp ) )
		AddToListBox( lp, NULL, 0, (long)(LPTR)hControl );
	ListExtFiles( wType, AddToListBox, (long)(LPTR)hControl );
	}

if ( idStatic )
	SetDlgItemText( hWnd, idStatic, "" );

if ( !(i = SendMessage( hControl, (bCombo ? CB_GETCOUNT : LB_GETCOUNT),0,0L)) )
	{
	*lpExtName = '\0';
	return;
	}

while ( --i >= 0 )
	{
	SendMessage( hControl, (bCombo ? CB_GETLBTEXT : LB_GETTEXT),
		i, (long)(LPSTR)szExtName);
	if ( !i || StringsEqual(lpExtName, szExtName) )
		{
		SendMessage( hControl, (bCombo ? CB_SETCURSEL : LB_SETCURSEL), i, 0L);
		break;
		}
	}

*lpExtName = '\0';
SendMessage( hControl, (bCombo ? CB_GETLBTEXT : LB_GETTEXT), i,
	(long)lpExtName );
if ( idStatic && (!idEntry1 || i) &&
	 LookupExtFileN( lpExtName, szFileName, wType, NO ))
		SetDlgItemText( hWnd, idStatic, Lowercase(filename(szFileName)) );
}

/***********************************************************************/
static int AddToComboBox(
/***********************************************************************/
LPSTR 	lpExtName,
LPSTR	lpFileName,
WORD 	wType,
long 	lData)
{
return( SendMessage( (HWND)lData, CB_ADDSTRING, 0, (long)lpExtName ) );
}


/***********************************************************************/
static int AddToListBox(
/***********************************************************************/
LPSTR 	lpExtName,
LPSTR	lpFileName,
WORD 	wType,
long 	lData)
{
return( SendMessage( (HWND)lData, LB_ADDSTRING, 0, (long)lpExtName ) );
}


/***********************************************************************/
int GetExtName(
/***********************************************************************/
HWND 	hWnd,
int 	idControl,
int		idStatic,
LPSTR 	lpExtName,
WORD 	wType,
WORD 	wMsg)
{
return( GetExtNameN( hWnd, idControl, idStatic, lpExtName, wType, wMsg, 0, NO));
}

/***********************************************************************/
int GetExtNameN(
/***********************************************************************/
HWND 	hWnd,
int 	idControl,
int		idStatic,
LPSTR 	lpExtName,
WORD 	wType,
WORD 	wMsg,
int 	idEntry1,
BOOL 	fCategorize)
{
int i = -1;
FNAME szFileName;
HWND hControl;
BOOL bCombo;

if ( !(hControl = GetDlgItem(hWnd, idControl)) )
	return(i);

bCombo = IsCombo( hControl );
if ( bCombo && wMsg == CBN_EDITCHANGE )
	GetDlgItemText( hWnd, idControl, lpExtName, MAX_FNAME_LEN );
else
if ( (bCombo && wMsg == CBN_SELCHANGE) || (!bCombo && wMsg == LBN_SELCHANGE) )
	{
	*lpExtName = '\0';
	i = SendMessage( hControl, (bCombo ? CB_GETCURSEL:LB_GETCURSEL), 0, 0L );
	SendMessage( hControl, (bCombo ? CB_GETLBTEXT : LB_GETTEXT), i,
		(long)lpExtName);
	if ( !idStatic )
		return(i);
	if ( !i && idEntry1 )
		{
		SetDlgItemText( hWnd, idStatic, "" );
		return(i);
		}
	if ( LookupExtFileN( lpExtName, szFileName, wType, YES ))
		SetDlgItemText( hWnd, idStatic, Lowercase(filename(szFileName)) );
	else
		SetDlgItemText( hWnd, idStatic, "" );
	}
return(i);
}


/************************************************************************/
int GetExtNameCount(WORD wType)
/************************************************************************/
{
FNAME szValueString, szNameString;
int count;

if ( !LoadTypeStrings( wType ) )
	return( -1 );
count  = 0;
while ( GetNextTypeString(wType, szNameString, szValueString) )
	count++;
return( count );
}

/************************************************************************/
BOOL GetExtNameString(WORD wType, int index, LPSTR lpName)
/************************************************************************/
{
FNAME szValueString, szNameString;
int count;

if (index >= GetExtNameCount( wType ))
	return( FALSE );

if ( !LoadTypeStrings( wType ) )
	return( FALSE );

count = 0;
while ( GetNextTypeString(wType, szNameString, szValueString) )
	{
	if (count == index)
		{
		lstrcpy(lpName, szNameString);
		return( TRUE );
		}
	count++;
	}

return( FALSE );
}

/************************************************************************/
BOOL LookupExtFile( LPSTR lpExtName, LPSTR lpFileName, WORD wType )
/************************************************************************/
{
return( LookupExtFileN( lpExtName, lpFileName, wType, YES ) );
}

/************************************************************************/
BOOL LookupExtFileN( LPSTR lpExtName, LPSTR lpFileName, WORD wType, BOOL bFileCheck )
/************************************************************************/
{
LPSTR lpTypeName;
FNAME szValueString, szNameString;

if ( !lstrlen( lpExtName ) )
	return( FALSE );

if ( !LoadTypeStrings( wType ) )
	return(FALSE);

while ( GetNextTypeString(wType, szNameString, szValueString) )
	{
	if ( !StringsEqual(szNameString, lpExtName) )
		continue;
	if ( !GetFileFullPath( szValueString, wType ) && bFileCheck )
		{ // The file doesn't exist...
		lpTypeName = GetType( wType );
		AstralAffirm( IDS_EXTNAME_BADFILE, lpTypeName, lpExtName,
			Lowercase(szValueString), lpTypeName );
		return( NO );
		}
	lstrcpy( lpFileName, szValueString );
	return(TRUE);
	}

return(FALSE);
}


/************************************************************************/
static BOOL ListExtFiles(WORD wType, LPADDPROC lpAddFunction, long lData)
/************************************************************************/
{
FNAME szValueString, szNameString;
int num_files;

if (!LoadTypeStrings( wType ))
	return(FALSE);

num_files = 0;
while ( GetNextTypeString(wType, szNameString, szValueString) )
	{
	(*lpAddFunction)((LPSTR)szNameString, (LPSTR)szValueString, (WORD)wType,
		lData);
	++num_files;
	}

return( num_files != 0 );
}


/************************************************************************/
static BOOL GetFileFullPath(
/************************************************************************/
LPSTR 	lpFileName,
WORD 	wType)
{
FNAME path, szFileName;

lstrcpy(szFileName, lpFileName);
if ( !lstrchr(szFileName, ':') && !lstrchr(szFileName, '\\') &&
	 GetExtNamePath( path, wType ) )
	{ // if it is not a full path already, start with the standard location
	lstrcpy( lpFileName, path );
	lstrcat( lpFileName, szFileName );
	}

return( FileExists( lpFileName ) );
}


/************************************************************************/
BOOL LoadAllExtNamePaths( BOOL bClose )
/************************************************************************/
{
// Don't load the paths, so we always default to PouchPath
return( TRUE );
}


/************************************************************************/
BOOL GetExtNamePath( LPSTR lpPath, WORD wType )
/************************************************************************/
{
	switch (wType)
	{
		case IDN_CLIPBOARD:
		case IDN_MASK:
			lstrcpy( lpPath, Control.WorkPath );
			break;
		default:
			lstrcpy( lpPath, Control.PouchPath );
	}
	return( TRUE );
}

/***********************************************************************/
static LPSTR GetType( WORD wType )
/***********************************************************************/
{
switch ( wType )
    {
    case IDN_BGMAP:
		return( "BGMap" );
    case IDN_CALMAP:
		return( "CalMap" );
    case IDN_HALFTONE:
		return( "Halftone" );
    case IDN_SEPARATE:
		return( "Separation" );
    case IDN_TEXTURE:
		return( "Texture" );
    case IDN_EFFECTS:
		return( "Effects" );
    case IDN_STYLE:
		return( "Style" );
    case IDN_CLIPBOARD:
		return( "Clipboard" );
    case IDN_MASK:
		return( "Mask" );
    case IDN_CUSTOM:
		return( "Custom" );
    case IDN_PALETTE:
		return( "Palette" );
    case IDN_STAMP:
		return( "Stamp" );
    case IDN_TOY:
		return( "Toy" );
    case IDN_PAINTSTYLE:
		return( "Paint" );
    case IDN_CRAYONSTYLE:
		return( "Crayon" );
    case IDN_MARKERSTYLE:
		return( "Marker" );
    case IDN_LINESTYLE:
		return( "Lines" );
    case IDN_SHAPESTYLE:
		return( "Shapes" );
    case IDN_ERASESTYLE:
		return( "Eraser" );
    default:
		return( "Unknown" );
    }
}


/***********************************************************************/
static WORD GetTypeWord( LPSTR lpType )
/***********************************************************************/
{
if ( StringsEqual( lpType, "BGMap" ) )
	return( IDN_BGMAP );
if ( StringsEqual( lpType, "CalMap" ) )
	return( IDN_CALMAP );
if ( StringsEqual( lpType, "Halftone" ) )
	return( IDN_HALFTONE );
if ( StringsEqual( lpType, "Separation" ) )
	return( IDN_SEPARATE );
if ( StringsEqual( lpType, "Texture" ) )
	return( IDN_TEXTURE );
if ( StringsEqual( lpType, "Effects" ) )
	return( IDN_EFFECTS );
if ( StringsEqual( lpType, "Style" ) )
	return( IDN_STYLE );
if ( StringsEqual( lpType, "Clipboard" ) )
	return( IDN_CLIPBOARD );
if ( StringsEqual( lpType, "Mask" ) )
	return( IDN_MASK );
if ( StringsEqual( lpType, "Custom" ) )
	return( IDN_CUSTOM );
if ( StringsEqual( lpType, "Palette" ) )
	return( IDN_PALETTE );
if ( StringsEqual( lpType, "Stamp" ) )
	return( IDN_STAMP );
if ( StringsEqual( lpType, "Toy" ) )
	return( IDN_TOY );
if ( StringsEqual( lpType, "Paint" ) )
	return( IDN_PAINTSTYLE );
if ( StringsEqual( lpType, "Crayon" ) )
	return( IDN_CRAYONSTYLE );
if ( StringsEqual( lpType, "Marker" ) )
	return( IDN_MARKERSTYLE );
if ( StringsEqual( lpType, "Lines" ) )
	return( IDN_LINESTYLE );
if ( StringsEqual( lpType, "Shapes" ) )
	return( IDN_SHAPESTYLE );
if ( StringsEqual( lpType, "Eraser" ) )
	return( IDN_ERASESTYLE );
return( 1 ); // An invalid type
}


/***********************************************************************/
static BOOL IsCombo( HWND hWnd )
/***********************************************************************/
{
STRING szClass;

GetClassName(hWnd, szClass, sizeof(szClass));
return( lstrfind( szClass, "combobox" ) != NULL );
}


/************************************************************************/
static BOOL LoadTypeStrings( WORD wType )
/************************************************************************/
{
	static LPSTR lpLoadedNameStrings;
#define SIZEOF_NAME_BUFFER 1600

	if ( !MakeIniFileName( szIniFile ) )
		return( NO );

	if ( !lpLoadedNameStrings ) // One and only one alloc; it never gets freed
		if ( !(lpLoadedNameStrings = (LPSTR)Alloc( (long)SIZEOF_NAME_BUFFER )) )
			return( NO );

	if ( !GetPrivateProfileString( GetType(wType), NULL, NULL, lpLoadedNameStrings,
		SIZEOF_NAME_BUFFER, szIniFile ) )
			return( NO );

	lpLoadedName = lpLoadedNameStrings;
	return( YES );
}


/************************************************************************/
static BOOL GetNextTypeString( WORD wType, LPSTR lpNameString, LPSTR lpValueString )
/************************************************************************/
{
BOOL bRet;

if ( !lpLoadedName || *lpLoadedName == '\0' )
	return( NO );

if ( bRet = GetTypeString( wType, lpLoadedName, lpValueString ) )
	lstrcpy( lpNameString, lpLoadedName );
lpLoadedName += lstrlen( lpLoadedName );
lpLoadedName++;
return( bRet );
}


/************************************************************************/
static BOOL GetTypeString( WORD wType, LPSTR lpNameString, LPSTR lpValueString )
/************************************************************************/
{
if ( !MakeIniFileName( szIniFile ) )
	return( NO );

GetPrivateProfileString( GetType(wType), lpNameString, ""/*Default*/,
	lpValueString, sizeof(STRING), szIniFile );

return( TRUE /*lpValueString[0] != '\0'*/ );
}

/************************************************************************/
static BOOL MakeIniFileName( LPSTR lpIniFileName )
/************************************************************************/
{
FNAME szFileName;
LPSTR lp;

if ( !AstralStr( IDS_NAMESFILE, &lp ) )
	return( NO );

#ifdef _MAC
lstrcpy( lpIniFileName, lp );
#else
lstrcpy( szFileName, Control.PouchPath );
lstrcat( szFileName, lp );
if ( !FileExists( szFileName ) )
	{
	lstrcpy( szFileName, lp );
	GetIniPath( szFileName );
	lstrcpy( Control.PouchPath, stripfile(szFileName) );
	}

lstrcpy( lpIniFileName, szFileName );
#endif // _MAC
return( YES );
}
