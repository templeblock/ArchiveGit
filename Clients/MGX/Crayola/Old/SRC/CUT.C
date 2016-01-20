//®FD1¯®PL1¯®TP0¯®BT0¯®RM250¯
// (c) Copyright 1991 MICROGRAFX, Inc., All Rights Reserved.
// This material is confidential and a trade secret.
// Permission to use this work for any purpose must be obtained
// in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081
#include <windows.h>
#include "id.h"
#include "data.h"
#include "routines.h"
#include "clipbrd.h"
#include "ppole.h"

// Static prototypes
static BOOL PasteMGXImage( HWND hWindow, LPSTR lpImageFile, LPSTR lpMaskFile );
static BOOL PasteDIB( HWND hWindow, LPSTR lpImageFile, LPSTR lpMaskFile );
static BOOL PasteBitmap( HWND hWindow, LPSTR lpImageFile, LPSTR lpMaskFile );
static BOOL WriteDIBClip( LPBITMAPINFOHEADER lpInfo, LPSTR lpImageFile, LPSTR lpMaskFile );
static BOOL WriteDIB( LPBITMAPINFOHEADER lpInfo, LPSTR lpImageFile );
static BOOL WriteDIBMask( LPBITMAPINFOHEADER lpInfo, LPSTR lpMaskFile );

extern HWND hWndAstral;

// Copy to (write out) the windows clipboard
/************************************************************************/
BOOL RenderToClipboard( HWND hWindow, WORD wFormat )
/************************************************************************/
{
DWORD dwValue;
HGLOBAL hMem;
LPTR lpMem;
LPFRAME lpFrame;
int iDataType, iFileType, fNative;

// This routine assumes that the 2 files to read and render
// are identified by Names.ClipImageFile and Names.ClipMaskFile

fNative = ( pOLE && (wFormat == pOLE->cfNative || !wFormat) ); // Native format is a DIB

if ( wFormat == Control.cfImage || !wFormat )
	{
	if ( hMem = ConstructMGXImage( Names.ClipImageFile, Names.ClipMaskFile ) )
			SetClipboardData( Control.cfImage, hMem );
	else	return( FALSE );
	}

if ( wFormat == CF_DIB || wFormat == CF_BITMAP ||
	 wFormat == CF_METAFILEPICT || !wFormat || fNative )
	{
	if ( !(lpFrame = AstralFrameLoad( Names.ClipImageFile, -1, &iDataType,
		 &iFileType )) )
		return(FALSE);
	}
else
	lpFrame = NULL;

if ( wFormat == CF_DIB || !wFormat || fNative )
	{
	if ( lpMem = FrameToDIB( lpFrame, NULL ) )
		{
#ifdef WIN32		
		hMem = GlobalHandle( lpMem );
#else		
		dwValue = GlobalHandle( HIWORD(lpMem) );
		hMem = (HGLOBAL)LOWORD( dwValue );
#endif
		GlobalUnlock(hMem);
		if ( wFormat == CF_DIB || !wFormat )
			SetClipboardData( CF_DIB, hMem );
		if ( fNative )
			SetClipboardData( pOLE->cfNative, hMem );
		}
	else
		{
		FrameClose( lpFrame );
		return(FALSE);
		}
	}

if ( wFormat == CF_BITMAP || (!wFormat && Control.DoBitmap) )
	{
	if ( hMem = FrameToBitmap( lpFrame, NULL ) )
		SetClipboardData( CF_BITMAP, hMem );
	else
		{
		FrameClose( lpFrame );
		return(FALSE);
		}
	}

if ( wFormat == CF_METAFILEPICT || (!wFormat && Control.DoPicture) )
	{
	if ( hMem = FrameToMetafile( lpFrame, NULL ) )
		SetClipboardData( CF_METAFILEPICT, hMem );
	else
		{
		FrameClose( lpFrame );
		return(FALSE);
		}
	}

if ( lpFrame )
	FrameClose( lpFrame );

if ( wFormat == CF_PALETTE || !wFormat )
	{
	if ( hMem = ConstructPalette( lpBltScreen->hPal ) )
		SetClipboardData( CF_PALETTE, hMem );
	}

return( TRUE );
}


/************************************************************************/
HGLOBAL ConstructMGXImage( LPSTR lpImageFile,  LPSTR lpMaskFile )
/************************************************************************/
{
LPMGX_IMAGEHDR lpMgxHdr;
LPMGX_IMAGENAME lpMgxName;
long lDataSize, lCount, lValue;
LPSTR lpClipMem;
HGLOBAL hMemory;

lDataSize = sizeof(MGX_IMAGENAME) + lstrlen(lpImageFile) + 1 + sizeof(MGX_IMAGENAME) + lstrlen(lpMaskFile) + 1;
lCount = sizeof(MGX_IMAGEHDR) + lDataSize;
// allocate the memory to hold the MGX_IMAGE
if ( !(lpClipMem = (LPSTR)Alloc( lCount )) )
	{
	Message( IDS_EMEMALLOC );
	return( NULL );
	}
#ifdef WIN32		
hMemory = GlobalHandle( lpClipMem );
#else		
lValue = GlobalHandle( HIWORD(lpClipMem) );
hMemory = (HGLOBAL)LOWORD( lValue );
#endif
lpMgxHdr = (LPMGX_IMAGEHDR)lpClipMem;
lpMgxHdr->wHeaderSize = sizeof(MGX_IMAGEHDR);
lpMgxHdr->wVersion = MGX_IMAGE_VERSION;
lpMgxHdr->wImageType = MGX_IMAGEMASKTYPE;
lpMgxHdr->lDataSize = lDataSize;
lpClipMem += lpMgxHdr->wHeaderSize;
lpMgxName = (LPMGX_IMAGENAME)lpClipMem;
lpMgxName->wNameHeaderSize = sizeof(MGX_IMAGENAME);
lpMgxName->wDataType = MGX_IMAGETIFF;
lpMgxName->wNameLength = lstrlen(lpImageFile) + 1;
lpClipMem += sizeof(MGX_IMAGENAME);
lstrcpy(lpClipMem, lpImageFile);
lpClipMem += lpMgxName->wNameLength;
lpMgxName = (LPMGX_IMAGENAME)lpClipMem;
lpMgxName->wNameHeaderSize = sizeof(MGX_IMAGENAME);
lpMgxName->wDataType = MGX_MASKPP;
lpMgxName->wNameLength = lstrlen(lpMaskFile) + 1;
lpClipMem += sizeof(MGX_IMAGENAME);
lstrcpy(lpClipMem, lpMaskFile);
return( hMemory );
}
/************************************************************************/
HGLOBAL ConstructPalette( HPALETTE hPal )
/************************************************************************/
{
if ( !hPal )
	return( NULL );
return( CopyPalette( hPal ) );
}


// Paste from (read in) the windows clipboard
/************************************************************************/
BOOL PasteFromClipboard( HWND hWindow, BOOL fNeedMask )
/************************************************************************/
{
// The MGX format will stuff the path strings since the files already exist
if ( IsClipboardFormatAvailable( Control.cfImage ) )
	return( PasteMGXImage( hWindow, Names.PasteImageFile, Names.PasteMaskFile));

// Other formats need to create the file, therefore we stuff the path
// strings first
GetExtNamePath( Names.PasteImageFile, IDN_CLIPBOARD );
lstrcat( Names.PasteImageFile, "WINCLIP.BMP" );
Names.PasteMaskFile[0] = '\0';

if ( IsClipboardFormatAvailable( CF_DIB ) )
	return( PasteDIB( hWindow, Names.PasteImageFile, NULL ) );
else
if ( IsClipboardFormatAvailable( CF_BITMAP ) )
	return( PasteBitmap( hWindow, Names.PasteImageFile, NULL ) );
return( FALSE );
}


/************************************************************************/
static BOOL PasteMGXImage(
/************************************************************************/
HWND	hWindow,
LPSTR	lpImageFile,
LPSTR	lpMaskFile)
{
LPMGX_IMAGEHDR 	lpMgxHdr;
LPMGX_IMAGENAME lpMgxName;
HGLOBAL		hMemory;
LPTR		lpClipMem;

if ( !OpenClipboard(hWindow) )
	return( FALSE );

if (!(hMemory = GetClipboardData(Control.cfImage)))
	{
	CloseClipboard();
	return(FALSE);
	}

lpClipMem = (LPTR)GlobalLock(hMemory);
lpMgxHdr = (LPMGX_IMAGEHDR)lpClipMem;
if (lpMgxHdr->wHeaderSize != sizeof(MGX_IMAGEHDR) ||
	lpMgxHdr->wVersion != MGX_IMAGE_VERSION ||
	lpMgxHdr->wImageType != MGX_IMAGEMASKTYPE)
	{
	Message(IDS_BADMGXIMAGE);
	GlobalUnlock(hMemory);
	CloseClipboard();
	return(FALSE);
	}
lpClipMem += lpMgxHdr->wHeaderSize;
lpMgxName = (LPMGX_IMAGENAME)lpClipMem;
lpClipMem += lpMgxName->wNameHeaderSize;
lstrcpy(lpImageFile, (LPSTR)lpClipMem);
lpClipMem += lpMgxName->wNameLength;
lpMgxName = (LPMGX_IMAGENAME)lpClipMem;
lpClipMem += lpMgxName->wNameHeaderSize;
lstrcpy(lpMaskFile, (LPSTR)lpClipMem);
GlobalUnlock(hMemory);
CloseClipboard();
return(TRUE);
}


/************************************************************************/
static BOOL PasteDIB(
/************************************************************************/
HWND	hWindow,
LPSTR	lpImageFile,
LPSTR	lpMaskFile)
{
LPBITMAPINFOHEADER lpDIB;
HGLOBAL hMemory;
BOOL bRet;

if ( !OpenClipboard(hWindow) )
	return( FALSE );

if (!(hMemory = GetClipboardData(CF_DIB)))
	{
	CloseClipboard();
	return( NO );
	}

if ( !(lpDIB = (LPBITMAPINFOHEADER)GlobalLock(hMemory)) )
	{
	CloseClipboard();
	return( NO );
	}

bRet = WriteDIBClip( lpDIB, lpImageFile, lpMaskFile );
GlobalUnlock( hMemory );
CloseClipboard();
return( bRet );
}


/************************************************************************/
static BOOL PasteBitmap(
/************************************************************************/
HWND	hWindow,
LPSTR	lpImageFile,
LPSTR	lpMaskFile)
{
BOOL bRet;
HBITMAP hBitmap;
HPALETTE hPal;
LPBITMAPINFOHEADER lpDIB;

if ( !OpenClipboard( hWindow ) )
	return( FALSE );

if ( !(hPal = (HPALETTE)GetClipboardData(CF_PALETTE)) )
	hPal = lpBltScreen->hPal;
if ( !(hBitmap = (HBITMAP)GetClipboardData(CF_BITMAP)) )
	{
	CloseClipboard();
	return( NO );
	}

if ( !(lpDIB = BitmapToDIB( NULL/*hDC*/, hBitmap, hPal )) )
	{
	CloseClipboard();
	return( NO );
	}

bRet = WriteDIBClip( lpDIB, lpImageFile, lpMaskFile );
FreeUp( (LPTR)lpDIB );
CloseClipboard();
return( bRet );
}


/************************************************************************/
static BOOL WriteDIBClip(
/************************************************************************/
LPBITMAPINFOHEADER lpInfo,
LPSTR lpImageFile,
LPSTR lpMaskFile)
{
FNAME szFileName;

if ( lpInfo->biSize != 40 || // make sure it's the correct length
	 lpInfo->biCompression != BI_RGB ) // no compression
	return(FALSE);

if ( lpImageFile )
	{
	if ( !WriteDIB(lpInfo, lpImageFile) )
		return(FALSE);
	}

if ( lpMaskFile )
	{
	if ( !WriteDIBMask(lpInfo, lpMaskFile) )
		{
		lstrcpy(szFileName, lpImageFile);
		FileDelete(szFileName);
		return(FALSE);
		}
	}

return(TRUE);
}


/************************************************************************/
static BOOL WriteDIB(
/************************************************************************/
LPBITMAPINFOHEADER lpInfo,
LPSTR lpImageFile)
{
DWORD lCount;
WORD wCount;
HPTR lpClipPtr;
BITMAPFILEHEADER bmhdr;
int ofh;

bmhdr.bfType = 0x4D42;
bmhdr.bfReserved1 = 0;
bmhdr.bfReserved2 = 0;
bmhdr.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
if (lpInfo->biBitCount == 8)
	bmhdr.bfOffBits += ( 256 * sizeof(RGBQUAD) );
else
if (lpInfo->biBitCount == 4)
	bmhdr.bfOffBits += ( 16 * sizeof(RGBQUAD) );
else
if (lpInfo->biBitCount == 1)
	bmhdr.bfOffBits += ( 2 * sizeof(RGBQUAD) );

lCount = bmhdr.bfOffBits + lpInfo->biSizeImage;
bmhdr.bfSize = lCount/2;

// write the winclip.bmp file
if ( (ofh = _lcreat( lpImageFile, 0 )) < 0 )
	{
	Message (IDS_EOPEN, lpImageFile);
	return(FALSE);
	}
if ( _lwrite( ofh, (LPSTR)&bmhdr, 14 ) != 14 )
	{
	Message (IDS_EWRITE, lpImageFile);
	_lclose(ofh);
	return(FALSE);
	}
lCount -= 14;
lpClipPtr = (HPTR)lpInfo;
while( lCount > 0L )
	{
	wCount = ( ( lCount > 65535L ) ? 65535 : lCount );
	if ( _lwrite( ofh, (LPSTR)lpClipPtr, wCount ) != wCount )
		{
		Message( IDS_EWRITE, lpImageFile );
		_lclose(ofh);
		return(FALSE);
		}
	lpClipPtr += wCount;
	lCount -= wCount;
	}
_lclose(ofh);
return(TRUE);
}


/************************************************************************/
static BOOL WriteDIBMask(
/************************************************************************/
LPBITMAPINFOHEADER lpInfo,
LPSTR lpMaskFile)
{
BOOL bRet;
LPMASK lpMask;

// create the mask
if ( !(lpMask = MaskCreate( NULL, (int)lpInfo->biWidth,
	(int)lpInfo->biHeight, ON, YES )) )
		return(FALSE);

// write out the mask
bRet = WriteMask( lpMaskFile, lpMask, FALSE );
MaskClose( lpMask );

return( bRet );
}


// Builds an ObjectLink and OwnerLink text string for OLE clipboard
// interaction in the format of "classname\0document\0object\0\0"
//  Return Value:
//  HGLOBAL		  Global memory handle to an block containing
//				  the three strings with the appropriate separator.
//
/************************************************************************/
HGLOBAL ConstructObject(LPIMAGE lpImage, LPRECT lpRect)
/************************************************************************/
{
HGLOBAL 	hMem;
WORD 		length;
LPSTR 		lp;
STRING 		szObject;

if ( !lpImage )
	return( NULL );

// Create an object string to describe the masked area
if ( !lpRect ) // If there is no mask/object rectangle...
		lstrcpy( szObject, "." );
else	wsprintf( szObject, "(%d,%d)-(%d,%d)",
		lpRect->left, lpRect->top, lpRect->right, lpRect->bottom );

// Figure out the string length; Extra 4 is for the null-terminators
length = 4 + lstrlen( OBJECT_CLASS ) + lstrlen( lpImage->CurFile )
	+ lstrlen( szObject );

if ( !(hMem = GlobalAlloc( GMEM_SHARE | GMEM_MOVEABLE, (DWORD)length )) )
	return( NULL );

lp = (LPSTR)GlobalLock( hMem );

// The Object Class Name is our standard class name
lstrcpy( lp, OBJECT_CLASS );
lp += lstrlen(lp) + 1;

// The Document name is the image path name
lstrcpy( lp, lpImage->CurFile );
lp += lstrlen(lp) + 1;

// The Object Name describes the masked area (entire image if no mask)
lstrcpy( lp, szObject );
lp += lstrlen(lp) + 1;

// Add a double null terminator
*lp = 0;

GlobalUnlock( hMem );
return( hMem );
}




