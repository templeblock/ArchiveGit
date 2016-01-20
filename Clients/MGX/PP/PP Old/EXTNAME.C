// (c) Copyright 1991 MICROGRAFX, Inc., All Rights Reserved.
// This material is confidential and a trade secret.
// Permission to use this work for any purpose must be obtained
// in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081
//®PL1¯®FD1¯®TP0¯®BT0¯
#include <windows.h>
#include "astral.h"
#include "data.h"
#include "routines.h"
#include "mclib.h"

/************************************************************************/
/* extname.c    Extended name handling routines				*/
/************************************************************************/

#define EXT_NAME "POUCH.TXT"

extern HANDLE hInstAstral;
extern HWND hWndAstral;

static FILE *fp;
static FNAME szAddExtName;
static WORD wAddType;

/***********************************************************************/
void InitExtName( hWnd, idControl, idStatic, lpExtName, wType )
/***********************************************************************/
HWND hWnd;
WORD idControl, idStatic;
LPTR lpExtName;
WORD wType;
{
int i;
LPTR lp;
FNAME szExtName, szFileName;
HWND hControl;
BOOL bCombo;

if ( !(hControl = GetDlgItem(hWnd, idControl)) )
	return;
bCombo = IsCombo( hControl );
SendMessage( hControl, (bCombo ? CB_RESETCONTENT : LB_RESETCONTENT), 0, 0L );
if ( bCombo )
	ListExtFiles( wType, AddToComboBox, (long)hControl );
else	{
	if ( AstralStr( IDS_UNTITLED, &lp ) )
		{
		szFileName[0] = '-';
		lstrcpy( &szFileName[1], lp );
		SendMessage( hControl, LB_ADDSTRING, 0, (long)(LPTR)szFileName);
		}
	ListExtFiles( wType, AddToListBox, (long)hControl );
	}

i = SendMessage( hControl, (bCombo ? CB_GETCOUNT : LB_GETCOUNT), 0, 0L);
while ( --i >= 0 )
	{
	SendMessage( hControl, (bCombo ? CB_GETLBTEXT : LB_GETTEXT),
		i, (long)(LPTR)szExtName);
	if ( !i || StringsEqual(lpExtName, szExtName) )
		{
		SendMessage( hControl, (bCombo ? CB_SETCURSEL : LB_SETCURSEL),
			i, 0L);
		break;
		}
	}

SendMessage( hControl, (bCombo ? CB_GETLBTEXT : LB_GETTEXT),
	i, (long)lpExtName );
if ( !idStatic )
	return;
if ( LookupExtFile( lpExtName, szFileName, wType ) )
	SetDlgItemText( hWnd, idStatic, Lowercase(stripdir(szFileName)) );
else	SetDlgItemText( hWnd, idStatic, "" );
}


/***********************************************************************/
static void AddToComboBox(lpExtName, lpFileName, wType, lData)
/***********************************************************************/
LPTR lpExtName, lpFileName;
WORD wType;
long lData;
{
SendMessage( (HWND)lData, CB_ADDSTRING, 0, (long)lpExtName );
}


/***********************************************************************/
static void AddToListBox(lpExtName, lpFileName, wType, lData)
/***********************************************************************/
LPTR lpExtName, lpFileName;
WORD wType;
long lData;
{
SendMessage( (HWND)lData, LB_ADDSTRING, 0, (long)lpExtName );
}


/***********************************************************************/
void GetExtName( hWnd, idControl, idStatic, lpExtName, wType, wMsg )
/***********************************************************************/
HWND hWnd;
WORD idControl, idStatic;
LPTR lpExtName;
WORD wType;
WORD wMsg;
{
int item;
FNAME szFileName;
HWND hControl;
BOOL bCombo;

if ( !(hControl = GetDlgItem(hWnd, idControl)) )
	return;

bCombo = IsCombo( hControl );
if ( bCombo && wMsg == CBN_EDITCHANGE )
	GetDlgItemText( hWnd, idControl, lpExtName, MAX_FNAME_LEN );
else
if ( (bCombo && wMsg == CBN_SELCHANGE) || (!bCombo && wMsg == LBN_SELCHANGE) )
	{
	*lpExtName = '\0';
	item = SendMessage( hControl, (bCombo ? CB_GETCURSEL : LB_GETCURSEL),
		0, 0L );
	SendMessage( hControl, (bCombo ? CB_GETLBTEXT : LB_GETTEXT),
		item, (long)lpExtName);
	if ( !idStatic )
		return;
	if ( LookupExtFile( lpExtName, szFileName, wType ) )
	      SetDlgItemText( hWnd, idStatic, Lowercase(stripdir(szFileName)) );
	else  SetDlgItemText( hWnd, idStatic, "" );
	}
}


/***********************************************************************/
void ExtNameManager( hWnd, idControl, idStatic, lpExtName, wType, idFunction, fLinkedFiles )
/***********************************************************************/
HWND hWnd;
WORD idControl, idStatic, idFunction;
LPTR lpExtName;
WORD wType;
BOOL fLinkedFiles;
{
switch ( idFunction )
    {
    case IDC_ADDEXT:
	AddExtName( hWnd, idControl, idStatic, lpExtName, wType );
	break;
    case IDC_DELETEEXT:
	DeleteExtName( hWnd, idControl, idStatic, lpExtName, wType,
		fLinkedFiles);
	break;
    case IDC_RENAMEEXT:
	RenameExtName( hWnd, idControl, idStatic, lpExtName, wType,
		fLinkedFiles);
	break;
    default:
	break;
    }
}


/***********************************************************************/
static void AddExtName( hWnd, idControl, idStatic, lpExtName, wType )
/***********************************************************************/
HWND hWnd;
WORD idControl, idStatic;
LPTR lpExtName;
WORD wType;
{
FNAME szFileName;
WORD idDlg, idType;

if ( wType == IDN_TEXTURE || wType == IDN_CLIPBOARD )
	{
	idDlg = IDD_OPEN;
	idType = IDC_SAVEBMP;
	}
else	{
	idDlg = IDD_EXTOPEN;
	idType = 0;
	}
if ( !DoOpenDlg( hWnd, idDlg, idType, szFileName, NO ) )
	return;
wAddType = wType;
lstrcpy( szAddExtName, Lowercase(stripdir(szFileName)) );
stripext( szAddExtName );
if ( islower(*szAddExtName) )
	*szAddExtName -= ('a'-'A');
if ( !AstralDlg( NO, hInstAstral, hWnd, IDD_ADDEXTNAME, DlgAddExtNameProc ) )
	return;
if ( !AddExtFile( szAddExtName, szFileName, wType ) )
	return;
lstrcpy( lpExtName, szAddExtName ); // pass back the name
InitExtName( hWnd, idControl, idStatic, lpExtName, wType );
}


/***********************************************************************/
static void RenameExtName( hWnd, idControl, idStatic, lpExtName, wType, fLinkedFiles )
/***********************************************************************/
HWND hWnd;
WORD idControl, idStatic;
LPTR lpExtName;
WORD wType;
BOOL fLinkedFiles;
{
FNAME szFileName;

if ( !LookupExtFile( lpExtName, szFileName, wType ) )
	return;
wAddType = wType;
lstrcpy( szAddExtName, lpExtName );
if ( !AstralDlg( NO, hInstAstral, hWnd, IDD_ADDEXTNAME, DlgAddExtNameProc ) )
	return;
if ( !RenameExtFile( lpExtName, szAddExtName, wType, fLinkedFiles) )
	return;
lstrcpy( lpExtName, szAddExtName ); // pass back the name
InitExtName( hWnd, idControl, idStatic, lpExtName, wType );
}


/***********************************************************************/
static void DeleteExtName( hWnd, idControl, idStatic, lpExtName, wType, fLinkedFiles )
/***********************************************************************/
HWND hWnd;
WORD idControl, idStatic;
LPTR lpExtName;
WORD wType;
BOOL fLinkedFiles;
{
int retc;
FNAME szFileName;

if ( !LookupExtFile( lpExtName, szFileName, wType ) )
	return;
retc = AstralOKCancel( IDS_DELETEEXTFILE, lpExtName, Lowercase(szFileName));
if ( retc != IDOK )
	return;
DeleteExtFile( lpExtName, wType, fLinkedFiles );
InitExtName( hWnd, idControl, idStatic, lpExtName, wType );
}


/***********************************************************************/
BOOL PromptForExtName(hWnd, lpExtName, lpFileName, wType )
/***********************************************************************/
HWND hWnd;
LPTR lpExtName, lpFileName;
WORD wType;
{
if ( *lpExtName )
	lstrcpy( szAddExtName, lpExtName );
else	lstrcpy( szAddExtName, "" );
wAddType = wType;
if ( !AstralDlg( NO, hInstAstral, hWnd, IDD_ADDEXTNAME, DlgAddExtNameProc ) )
	return(FALSE);
lstrcpy( lpExtName, szAddExtName );
if ( !LookupExtFile( lpExtName, lpFileName, wType ) )
	if ( !GetExtFile( lpExtName, NULL, wType, lpFileName ) )
		return( FALSE );
return(TRUE);
}


/***********************************************************************/
BOOL FAR PASCAL DlgAddExtNameProc( hDlg, msg, wParam, lParam)
/***********************************************************************/
HWND hDlg;
unsigned msg;
WORD wParam;
long lParam;
{
BOOL Bool;
LPTR lpTitle;
FNAME szFileName;

switch (msg)
    { // This proc requires that wAddType and szAddExtName be initialized
    case WM_INITDIALOG:
	CenterPopup( hDlg );
	if ( AstralStr( wAddType + MAX_FILETYPES, &lpTitle ) )
		SetWindowText( hDlg, lpTitle );
	szFileName[0] = '\0';
	InitExtName( hDlg, IDC_EXTNAMES, NULL, szFileName, wAddType);
	SetDlgItemText( hDlg, IDC_EXTNAMES, szAddExtName );
	break;

    case WM_CLOSE:
	AstralDlgEnd( hDlg, FALSE );
	break;

    case WM_CTLCOLOR:
	return( SetControlColors( (HDC)wParam, hDlg, LOWORD(lParam),
		HIWORD(lParam) ) );

    case WM_ERASEBKGND:
	EraseDialogBackground( wParam /*hDC*/, hDlg );
	break;

    case WM_COMMAND:
	if ( HELP ) { Help( HELP_CONTEXT, (long)wParam ); }
	switch (wParam)
	    {
	    case IDC_EXTNAMES:
		GetExtName( hDlg, IDC_EXTNAMES, NULL, szAddExtName, wAddType,
			HIWORD(lParam) );
		break;

	    case IDOK:
		if ( LookupExtFile( szAddExtName, szFileName, wAddType ) )
			{
			if (AstralOKCancel(IDS_OVERWRITEEXTFILE,
			    (LPTR)szAddExtName) == IDCANCEL)
				break;
			}
		AstralDlgEnd( hDlg, TRUE );
		break;

	    case IDCANCEL:
		AstralDlgEnd( hDlg, FALSE );
		break;

	    default:
		return( FALSE );
	    }
    default:
	return( FALSE );
    }

return( TRUE );
}


/************************************************************************/
BOOL AddExtFile(lpExtName, lpFileName, wType)
/************************************************************************/
LPTR lpExtName, lpFileName;
WORD wType;
{
char buffer[256];
FNAME outfile, path, outpath;
int err;

if ( LookupExtFile(lpExtName, outfile, wType) )
	{ // If the name exists already...
	if ( StringsEqual( outfile, lpFileName ) )
		return( TRUE );
	DeleteExtFile(lpExtName, wType, YES);
	}
lstrcpy(outpath, lpFileName);
stripfile(outpath);

if (!GetExtNamePath(path))
	return(FALSE);
if ( StringsEqual(outpath, path) )
	{
	lstrcpy(outfile, lpFileName);
	}
else	{ // If its not already in the pouch...
	if (!GetExtFile(lpExtName, lpFileName, 0, outfile))
		return(FALSE);
	if (!CopyFile(lpFileName, outfile))
		return(FALSE);
	}

if (!OpenExtNameFile("a"))
	return(FALSE);
lstrcpy(buffer, lpExtName);
lstrcat(buffer, "|");
lstrcat(buffer, stripdir(outfile));
lstrcat(buffer, "|");
lstrcat(buffer, GetTypeString(wType) );
lstrcat(buffer, "\n");

err = fputs(buffer, fp);
CloseExtNameFile();
return( err ? FALSE : TRUE );
}


/************************************************************************/
BOOL LookupExtFile(lpExtName, lpFileName, wType)
/************************************************************************/
LPTR lpExtName, lpFileName;
WORD wType;
{
FNAME file_name, ext_name;
WORD type;

if ( !lstrlen( lpExtName ) )
	return( FALSE );

if (!OpenExtNameFile("r"))
	return(FALSE);

while (type = GetExtNameData(ext_name, file_name))
	{
	if (wType && (wType != type) )
		continue;
	if (!StringsEqual(ext_name, lpExtName))
		continue;
	GetExtFilePath(file_name);
	lstrcpy(lpFileName, file_name);
	CloseExtNameFile();
	return(TRUE);
	}

CloseExtNameFile();
return(FALSE);
}

/************************************************************************/
BOOL DeleteExtFile(lpExtName, wType, fLinkedFiles)
/************************************************************************/
LPTR lpExtName;
WORD wType;
BOOL fLinkedFiles;
{
FNAME file_name, ext_name, name;
WORD type;
int iFileCount;

if ( !fLinkedFiles )
	{ // Check for the existance of a linked file
	if (!OpenExtNameFile("r"))
		return(FALSE);
	while (type = GetExtNameData(ext_name, file_name))
		{ // see if we can find same name, different type
		if ( type == wType )
			continue;
		if (!StringsEqual(ext_name, lpExtName))
			continue;
		CloseExtNameFile();
		Print("Can't delete linked files");
		return(FALSE);
		}
	CloseExtNameFile();
	}

if (!OpenExtNameFile("r"))
	return(FALSE);

while (type = GetExtNameData(ext_name, file_name))
	{ // Delete all occurances
	if (!StringsEqual(ext_name, lpExtName))
		continue;
	if (!DeleteExtFileEntry(ext_name, type))
		{
		CloseExtNameFile();
		return(FALSE);
		}
	// Look for another usage of this file before deleting it
	if (!OpenExtNameFile("r"))
		return(FALSE);
	GetExtFilePath(file_name);
	strcpy(name, file_name);
	iFileCount = NO;
	while (GetExtNameData(ext_name, file_name))
		{
		GetExtFilePath(file_name);
		if (StringsEqual(file_name, name))
			{
			iFileCount = YES;
			break;
			}
		}
	if (!iFileCount)
        	unlink(name);
	if (!OpenExtNameFile("r"))
		return(FALSE);
	}

CloseExtNameFile();
return(TRUE);
}

/************************************************************************/
BOOL RenameExtFile(lpExtName, lpNewExtName, wType, fLinkedFiles)
/************************************************************************/
LPTR lpExtName;
LPTR lpNewExtName;
WORD wType;
BOOL fLinkedFiles;
{
FNAME file_name, ext_name;
WORD type;

if ( !fLinkedFiles )
	{ // Check for the existance of a linked file
	if (!OpenExtNameFile("r"))
		return(FALSE);
	while (type = GetExtNameData(ext_name, file_name))
		{ // see if we can find same name, different type
		if ( type == wType )
			continue;
		if (!StringsEqual(ext_name, lpExtName))
			continue;
		CloseExtNameFile();
		Print("Can't rename linked files");
		return(FALSE);
		}
	CloseExtNameFile();
	}

if (!OpenExtNameFile("r"))
	return(FALSE);

while (type = GetExtNameData(ext_name, file_name))
	{ // Rename all occurances
	if (!StringsEqual(ext_name, lpExtName))
		continue;
	GetExtFilePath(file_name);
    	if ( !AddExtFile( lpNewExtName, file_name, type ) )
		return(FALSE);
    	if (!DeleteExtFileEntry( lpExtName, type ))
		return(FALSE);
	if (!OpenExtNameFile("r"))
		return(FALSE);
    	}

CloseExtNameFile();
return(TRUE);
}

/************************************************************************/
BOOL ListExtFiles(wType, lpAddFunction, lData)
/************************************************************************/
WORD wType;
LPROC lpAddFunction;
long lData;
{
FNAME file_name, ext_name;
WORD type;
int num_files = 0;

if (!OpenExtNameFile("r"))
	return(FALSE);

while (type = GetExtNameData(ext_name, file_name))
	{
	if ( type != wType )
		continue;
        (*lpAddFunction)((LPTR)ext_name, (LPTR)file_name, (WORD)type, lData);
        ++num_files;
	}

CloseExtNameFile();
return( num_files != 0 );
}

/************************************************************************/
BOOL GetExtFile(lpExtName, lpFileName, idExtension, lpNewFileName)
/************************************************************************/
LPTR lpExtName, lpFileName;
int idExtension;
LPTR lpNewFileName;
{
FNAME szExtension;
char *p;
FILE *ofp;
FNAME outfile, num_string, path, outname;
int num, i, j, len;

if (!GetExtNamePath(path))
	return(FALSE);

if (lpFileName) // If a file name was passed in...
	{
	lstrcpy(outname, stripdir(lpFileName));
	GetExtension(outname, szExtension);
	}
else	{
	lstrcpy(outname, stripdir(lpExtName));
	if ( !LookupExtension( idExtension, szExtension ) )
		return(FALSE);
	if ((p = strchr(szExtension, '.')))
		lstrcpy(szExtension, p);
	}

stripext(outname);
for (i = 0, j = 0; i < lstrlen(outname); ++i)
    if (!isspace(outname[i]))
	outname[j++] = outname[i];

num = 0;
num_string[0] = '\0';
len = lstrlen( outname );
while ( 1 )
	{
	while ( len && (len + lstrlen(num_string) > 8) )
		outname[--len] = '\0';
	if ( !len )
		return(FALSE);
	lstrcpy(outfile, path);
	lstrcat(outfile, outname);
	lstrcat(outfile, num_string);
	lstrcat(outfile, szExtension);
	if ( !(ofp = fopen(outfile, "r")) )
		break;
	fclose(ofp);
	sprintf(num_string, "%d", num);
	num++;
	}

lstrcpy(lpNewFileName, outfile);
return(TRUE);
}


/************************************************************************/
static BOOL OpenExtNameFile(mode)
/************************************************************************/
char *mode;
{
FNAME name;
char themode[5];
int theCH;

if (!GetExtNamePath(name))
	return(FALSE);
CloseExtNameFile();
lstrcat(name, EXT_NAME);
if (*mode == 'a')
    lstrcpy(themode, "r+");
else
    lstrcpy(themode, mode);
if (!(fp = fopen(name, themode)))
    return(FALSE);
if (*mode == 'a')
    {
    while ((theCH = fgetc(fp)) != EOF)
	if (theCH == '\032') // looking for Ctrl Z's because stream
	    {		     // IO doesn't take care of this properly
	    fseek(fp, -1L, SEEK_CUR);
	    break;
	    }
    }
return(TRUE);
}

/************************************************************************/
static BOOL CloseExtNameFile()
/************************************************************************/
{
if (fp)
    fclose(fp);
fp = NULL;
}

/************************************************************************/
static BOOL GetExtNamePath(path)
/************************************************************************/
char *path;
{
int length;

lstrcpy(path, Control.PouchPath);
if ( !(length = lstrlen(path)) )
	return(FALSE);
while (length && path[length-1] == '\n' || path[length-1] == '\r')
	path[--length] = '\0';
if (!length)
	return(FALSE);
if (path[length-1] != '\\')
	lstrcat(path, "\\");
return(TRUE);
}

/************************************************************************/
static WORD GetExtNameData(ext_name, file_name)
/************************************************************************/
char *ext_name, *file_name;
{
char buffer[256];
BOOL found = FALSE;
STRING TypeName;
char *p1, *p2, *p3;

while (TRUE)
    {
    if (!fgets(buffer, MAX_FNAME_LEN, fp))
    	return( NULL );
    if (!(p1 = strchr(buffer, '|')))
	continue;
    if (buffer >= p1)
    	continue;
    copy(buffer, ext_name, p1-buffer);
    ext_name[p1-buffer] = '\0';
    while (lstrlen(ext_name) && isspace(ext_name[lstrlen(ext_name)-1]))
	ext_name[lstrlen(ext_name)-1] = '\0';
    ++p1;
    if (!(p2 = strchr(p1, '|')))
	continue;
    if (p1 >= p2)
    	continue;
    copy(p1, file_name, p2-p1);
    file_name[p2-p1] = '\0';
    while (lstrlen(file_name) && isspace(file_name[lstrlen(file_name)-1]))
	file_name[lstrlen(file_name)-1] = '\0';
    ++p2;
    p3 = buffer + lstrlen(buffer);
    if (p2 >= p3)
    	continue;
    copy(p2, TypeName, p3-p2);
    TypeName[p3-p2] = '\0';
    while (lstrlen(TypeName) && (isspace(TypeName[lstrlen(TypeName)-1]) || TypeName[lstrlen(TypeName)-1] == '|'))
	TypeName[lstrlen(TypeName)-1] = '\0';
    if (lstrlen(ext_name) > 0 && lstrlen(file_name) > 0 && lstrlen(TypeName) > 0)
	return( GetTypeWord( TypeName ) );
    }
}

/************************************************************************/
static BOOL DeleteExtFileEntry( lpExtName, wType )
/************************************************************************/
LPTR lpExtName;
WORD wType;
{
FNAME name, oldname, path, ext_name, file_name;
WORD type;
FILE *newfp, *oldfp;
char buffer[256];

if (!GetExtNamePath(name))
	return(FALSE);
CloseExtNameFile();
lstrcat(name, EXT_NAME);
lstrcpy(oldname, name);
stripext(oldname);
lstrcat(oldname, ".old");
if (!CopyFile(name, oldname))
    return(FALSE);
if (!OpenExtNameFile("w"))
	return(FALSE);
newfp = fp;
if (!(oldfp = fopen(oldname, "r")))
    {
    CloseExtNameFile();
    CopyFile(oldname, name);
    return(FALSE);
    }
fp = oldfp;
if (!GetExtNamePath(path))
    { 
    CloseExtNameFile();
    fp = newfp;
    CloseExtNameFile();
    CopyFile(oldname, name);
    return(FALSE);
    }
while (type = GetExtNameData(ext_name, file_name))
    {
    if (StringsEqual(ext_name, lpExtName) && (type == wType))
	continue;
    lstrcpy(buffer, ext_name);
    lstrcat(buffer, "|");
    lstrcat(buffer, file_name);
    lstrcat(buffer, "|");
    lstrcat(buffer, GetTypeString(type) );
    lstrcat(buffer, "\n");
    if (fputs(buffer, newfp))
        {
        CloseExtNameFile();
        fp = newfp;
        CloseExtNameFile();
        CopyFile(oldname, name);
        return(FALSE);
        }
    }
CloseExtNameFile();
fp = newfp;
CloseExtNameFile();
return(TRUE);
}

/************************************************************************/
static VOID GetExtFilePath(lpFileName)
/************************************************************************/
LPTR lpFileName;
{
FNAME path, file_name;

lstrcpy(file_name, lpFileName);
if (!strchr(file_name, '\\') && !strchr(file_name, ':'))
	{
	if (GetExtNamePath(path))
		{
		strcat(path, file_name);
		strcpy(file_name, path);
		}
	}
lstrcpy(lpFileName, file_name);
}

/************************************************************************/
LPTR stripfile( lpPathName )
/************************************************************************/
LPTR lpPathName;
{
LPTR ptr;

ptr = lpPathName;
ptr += lstrlen( lpPathName );
while( ptr >= lpPathName && *ptr != '\\' && *ptr != ':' )
	ptr--;
ptr++;
*ptr = '\0';
return( lpPathName );
}

/************************************************************************/
LPTR stripext(lpFileName)
/************************************************************************/
LPTR lpFileName;
{
FNAME file_name;
char *p;

lstrcpy(file_name, lpFileName);
if ((p = strchr(file_name, '.')))
    *p = '\0';
lstrcpy(lpFileName, file_name);
return(lpFileName);
}


/************************************************************************/
static BOOL GetExtension(file_name, extension)
/************************************************************************/
char *file_name;
char *extension;
{
char *p;

if ((p = strchr(file_name, '.')))
	{
	lstrcpy(extension, p);
	return(TRUE);
	}
else	{
	lstrcpy(extension, "");
	return(FALSE);
	}
}


/***********************************************************************/
static PTR GetTypeString( wType )
/***********************************************************************/
WORD wType;
{
switch ( wType )
    {
    case IDN_MAP:
	return( "Map" );
    case IDN_BGMAP:
	return( "BGMap" );
    case IDN_CALMAP:
	return( "CalMap" );
    case IDN_SCANMAP:
	return( "ScanMap" );
    case IDN_TEXTURE:
	return( "Texture" );
    case IDN_SMOOTH:
	return( "Smooth" );
    case IDN_SHARP:
	return( "Sharp" );
    case IDN_EDGE:
	return( "Edge" );
    case IDN_SPECIAL:
	return( "Special" );
    case IDN_SCANNER:
	return( "Scanner" );
    case IDN_GRABBER:
	return( "Grabber" );
    case IDN_STYLE:
	return( "Style" );
    case IDN_CLIPBOARD:
	return( "Clipboard" );
    case IDN_MASK:
	return( "Mask" );
    case IDN_CUSTOM:
	return( "Custom" );
    default:
	return( "Unknown" );
    }
}


/***********************************************************************/
static WORD GetTypeWord( lpType )
/***********************************************************************/
LPTR lpType;
{
if ( StringsEqual( lpType, "Map" ) )
	return( IDN_MAP );
if ( StringsEqual( lpType, "BGMap" ) )
	return( IDN_BGMAP );
if ( StringsEqual( lpType, "CalMap" ) )
	return( IDN_CALMAP );
if ( StringsEqual( lpType, "ScanMap" ) )
	return( IDN_SCANMAP );
if ( StringsEqual( lpType, "Texture" ) )
	return( IDN_TEXTURE );
if ( StringsEqual( lpType, "Smooth" ) )
	return( IDN_SMOOTH );
if ( StringsEqual( lpType, "Sharp" ) )
	return( IDN_SHARP );
if ( StringsEqual( lpType, "Edge" ) )
	return( IDN_EDGE );
if ( StringsEqual( lpType, "Special" ) )
	return( IDN_SPECIAL );
if ( StringsEqual( lpType, "Scanner" ) )
	return( IDN_SCANNER );
if ( StringsEqual( lpType, "Grabber" ) )
	return( IDN_GRABBER );
if ( StringsEqual( lpType, "Style" ) )
	return( IDN_STYLE );
if ( StringsEqual( lpType, "Clipboard" ) )
	return( IDN_CLIPBOARD );
if ( StringsEqual( lpType, "Mask" ) )
	return( IDN_MASK );
if ( StringsEqual( lpType, "Custom" ) )
	return( IDN_CUSTOM );
return( 1 ); // An invalid type
}


/***********************************************************************/
static BOOL IsCombo( hWnd )
/***********************************************************************/
HWND hWnd;
{
return( GetWindowLong( hWnd, GWL_STYLE ) & CBS_AUTOHSCROLL );
}
