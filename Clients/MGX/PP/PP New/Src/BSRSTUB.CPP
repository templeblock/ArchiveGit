#include "pp.h"
#include "id.h"
#include "browser.h"
#include "bsrport.h"
#include "bsrid.h"
#include "mgxrx.h"
#include "mgxrx.rh"
#include "bsrtypes.h"
#include "handlebs.h"
#include "bsritems.h"
#include "bsrutil.h"
#include "bsrdlgs.h"
#include "thmbsr.h"
#include "cthmmgr.h"

//************************************************************************/
BOOL DoOpenDlg( HWND hParent, LPOPENSTATE lpState)
/************************************************************************/
{
return(FALSE);
}

//************************************************************************/
BOOL DoMiniOpenDlg( HWND hParent, LPOPENSTATE lpState)
/************************************************************************/
{
return(FALSE);
}

//************************************************************************/
//	Given a LOCAL (IDT_) type sets the string to be the extension.
//	DO NOT use this as a real extension unless a 'real' file type is
//		used ie: TIFF, JPG etc.  not  ALLFILES etc.
//************************************************************************/
BOOL LookupExtension(ITEMID idLocalFileType, LPTR lpExtension )
/************************************************************************/
{
LPTR lpFileType;

if (idLocalFileType == IDT_ALLFILES)
	idLocalFileType = IDS_ALLFILES;

if (idLocalFileType == IDT_ALLIMAGES)
	idLocalFileType = IDS_ALLIMAGES;
	

if (!AstralStr(idLocalFileType, &lpFileType))
	lstrcpy(lpExtension, ".TIF");
lstrcpy( lpExtension, lpFileType );
return(YES);
}

//************************************************************************/
// Loads file paths into temp file paths in the .INI
//************************************************************************/
void LoadFileLocations()
/************************************************************************/
{
	FNAME szFileName, szExtension;
	int i;
	int items[] =
		{IDT_TIFF, IDT_TARGA, IDT_GIF, IDT_BMP, IDT_DIB, IDT_PCX, IDT_JPEG, IDT_PHOTOCD, IDT_AVI, IDT_PP, IDT_ALLFILES};

	for ( i=0; i<sizeof(items)/sizeof(int); i++ )
	{
		if ( !LookupExtension( items[i], szExtension ) )
			continue;
		lstrcpy (szExtension, extension(szExtension));
		GetDefaultString(szExtension, "", szFileName, sizeof(FNAME) );
		lstrcat( szExtension, "temp" );
		PutDefaultString( szExtension, szFileName );
	}
}

//************************************************************************/
BOOL GetFileLocation(ITEMID idFileType, LPTR lpPath )
//************************************************************************/
{
	FNAME szExtension, szFileName, szTemp;

	if ( !LookupExtension( idFileType, szExtension ) )
		return( NO );

	/* Find out where the user was last, so we can change over to that directory */
	lstrcpy( szTemp, szExtension );
	lstrcat( szTemp, "temp" );
	GetDefaultString( szTemp, szExtension, szFileName, sizeof(FNAME) );
	stripfile( szFileName );
	FixPath( szFileName );
	if (idFileType == IDT_PHOTOCD)
		GetPCDLocation(szFileName);
	lstrcat( szFileName, szExtension );
	lstrcpy( lpPath, szFileName );
	return( YES );
}

//************************************************************************/
BOOL NonBSRVolumeCheck(LPTR lpFile, LPVOLUMEID lpVolume, LPINT lpBoolRemovable)
//************************************************************************/
{
return(FALSE);
}

/************************************************************************/
LPTR SizeToAscii(DWORD dwSize, LPTR lpString )
/************************************************************************/
{
	int l;
	BYTE c, num[32];
	DWORD dwDivisor;
	LPTR lpLabel;
	#define GIG (1L<<30)
	#define MEG (1L<<20)
	#define KB (1L<<10)

	if ( dwSize < KB )
		{
		dwSize;
		itoa( (int)dwSize, num, 10 );
		lstrcpy( lpString, num );
		if ( AstralStr( IDS_BYTES, &lpLabel ) )
			lstrcat( lpString, lpLabel );
		return( lpString );
		}

	if ( dwSize >= GIG )
		{
		dwDivisor = GIG;
		if ( !AstralStr( IDS_GIGABYTES, &lpLabel ) )
			lpLabel = "";
		}
	else
	if ( dwSize >= MEG )
		{
		dwDivisor = MEG;
		if ( !AstralStr( IDS_MEGABYTES, &lpLabel ) )
			lpLabel = "";
		}
	else
	//if ( dwSize >= KB )
		{
		dwDivisor = KB;
		if ( !AstralStr( IDS_KILOBYTES, &lpLabel ) )
			lpLabel = "";
		}

	dwSize += (dwDivisor/20);
	dwSize /= (dwDivisor/10);
	itoa( (int)dwSize, num, 10 );
	l = lstrlen( num );
	c = num[l-1]; // get the decimal place value
	if ( l > 3 || c == '0' ) // has 4 significant digits or no decimal value
		num[l-1] = '\0'; // Don't do any decimal places
	else	{
		num[l-1] = '.';
		num[l] = c;
		num[l+1] = '\0';
		}
	lstrcpy( lpString, num );
	lstrcat( lpString, lpLabel );
	return( lpString );
}

//************************************************************************
void UpdateThumbnail(LPTR lpFile, LPFRAME lpFrame, int width, int height, LONG dataType, LONG fileType)
//************************************************************************
{
}

