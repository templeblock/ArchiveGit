// ****************************************************************************
//
//  File Name:			ResourceManager.cpp
//
//  Project:			Renaissance Application Framework
//
//  Author:				S. Athanas
//
//  Description:		Implementation of the RResourceManager class
//
//  Portability:		Platform independent
//
//  Developed by:		Turning Point Software.
//							One Gateway Center, Suite 800
//							Newton, MA 02158
//							(617) 332-0202
//
//  Client:				Broderbund Software, Inc.         
//
//  Copyright (C) 1996 Turning Point Software. All Rights Reserved.
//
//  $Logfile:: /PM8/Framework/Source/ResourceManager.cpp                      $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:17p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#include	"FrameworkIncludes.h"

ASSERTNAME

#include "ResourceManager.h"

#undef	FrameworkLinkage
#define	FrameworkLinkage

#include "StandaloneApplication.h"
#include "ApplicationGlobals.h"

// ****************************************************************************
//
//  Function Name:	RResourceManager::RResourceManager( )
//
//  Description:		Constructor.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
RResourceManager::RResourceManager(  )
	{
	;
	}

// ****************************************************************************
//
//  Function Name:	RResourceManager::~RResourceManager( )
//
//  Description:		Destructor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
RResourceManager::~RResourceManager( )
	{
	TpsAssert( m_ResourceFileList.Count() == 0, "There are still Resource files in the File List." );
	}


// ****************************************************************************
//
//  Function Name:	RResourceManager::AddFileReference( YResourceFileReference fileRef )
//
//  Description:		Add the given file reference to the list of files
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RResourceManager::AddResourceFile( YResourceFileReference fileRef )
	{
#ifdef	TPSDEBUG
	if (m_ResourceFileList.Count() > 1)
		{
		TpsAssert( m_ResourceFileList.Find( fileRef) == m_ResourceFileList.End(), "This file reference is already in the file list." );
		}
#endif	//	TPSDEBUG
	m_ResourceFileList.InsertAtEnd( fileRef );

	MacCode( UseResFile( fileRef ) );
	}

// ****************************************************************************
//
//  Function Name:	RResourceManager::RemoveResourceFile( YResourceFileReference fileRef )
//
//  Description:		Remove the given file reference to the list of files
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RResourceManager::RemoveResourceFile( YResourceFileReference fileRef )
	{
	TpsAssert( m_ResourceFileList.Find( fileRef) != m_ResourceFileList.End(), "Could not find the file reference to add" );
	m_ResourceFileList.Remove( fileRef );

#ifdef	MAC
	//	If there is any files, left in the list, make the last one active...
	if (m_ResourceFileList.Count() > 0)
		{
		RResourceFileList::YIterator	iterator = m_ResourceFileList.End() - 1;
		UseResFile( *iterator );
		}
#endif
	}

// ****************************************************************************
//
//  Function Name:	RResourceManager::GetResourceCursor( YResourceId cursorId )
//
//  Description:		Load the given cursor from an open resource file
//
//  Returns:			A Reference to the resource
//
//  Exceptions:		None
//
// ****************************************************************************
//
YResourceCursor RResourceManager::GetResourceCursor( YResourceId cursorId )
	{
	YResourceCursor	cursor = NULL;

#ifdef	_WINDOWS
	//	First, check the generic system default resources
	if ( (cursor = ::LoadCursor( NULL, MAKEINTRESOURCE(cursorId) )) == NULL )
		{
		//	Since the cursor might be in any of the 'ResourceFiles,' scan until one is found
		RResourceFileList::YIterator	iterator = m_ResourceFileList.Start();
		for ( ; iterator != m_ResourceFileList.End(); ++iterator )
			{
			if (( cursor = ::LoadCursor(*iterator, MAKEINTRESOURCE(cursorId)) ) != NULL)
				break;
			}
		}

#else		//	!_WINDOWS
	cursor = ::GetCursor( cursorId );
#endif	//	_WINDOWS

	TpsAssert( cursor != NULL, "Cursor could not be found in any open resource file.");
	return cursor;
	}

// ****************************************************************************
//
//  Function Name:	RResourceManager::GetResourceBitmap( YResourceId cursorId )
//
//  Description:		Load the given bitmap from an open resource file
//
//  Returns:			A Reference to the resource, NULL if not found or out of memory
//
//  Exceptions:		None
//
// ****************************************************************************
//
YResourceBitmap RResourceManager::GetResourceBitmap( YResourceId bitmapId )
	{
	YResourceBitmap	bitmap = NULL;

#ifdef	_WINDOWS
	//	First, check the generic system default resources
//	if ( (bitmap = ::LoadBitmap( NULL, MAKEINTRESOURCE(bitmapId) )) == NULL )
	if ( (bitmap = (YResourceBitmap)::LoadImage( NULL, MAKEINTRESOURCE(bitmapId), IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION )) == NULL )
		{
		//	Since the cursor might be in any of the 'ResourceFiles,' scan until one is found
		RResourceFileList::YIterator	iterator = m_ResourceFileList.Start();
		for ( ; iterator != m_ResourceFileList.End(); ++iterator )
			{
//			if (( bitmap = ::LoadBitmap(*iterator, MAKEINTRESOURCE(bitmapId)) ) != NULL)
			if ( (bitmap = (YResourceBitmap)::LoadImage( *iterator, MAKEINTRESOURCE(bitmapId), IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION )) != NULL )
				break;
			}
		}

#else		//	!_WINDOWS
	bitmap = ::GetPicture( bitmapId );
#endif	//	_WINDOWS
	return bitmap;
	}

// ****************************************************************************
//
//  Function Name:	RResourceManager::FreeResourceBitmap( YResourceBitmap bitmap ) )
//
//  Description:		Release a bitmap resource loaded with GetResourceBitmap
//
//  Returns:			None
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RResourceManager::FreeResourceBitmap( YResourceBitmap bitmap )
	{
	TpsAssert( bitmap != NULL, "Bitmap is NULL.");
#ifdef	_WINDOWS
	::DeleteObject( bitmap );
#else		//	!_WINDOWS
	::ReleaseResource( bitmap );
#endif	//	_WINDOWS
	}

// ****************************************************************************
//
//  Function Name:	RResourceManager::GetHTBitResourceBitmap( YResourceId cursorId )
//
//  Description:		Load the given bitmap from an open resource file
//
//  Returns:			an HTBit bitmap that's internally a DIB, or NULL if not
//							 found or out of memory
//
//  Exceptions:		None
//
// ****************************************************************************
//
HTBIT RResourceManager::GetHTBitResourceBitmap( YResourceId bitmapId )
	{
	YResourceBitmap	bitmap = NULL;
	HTBIT					htBitmap = NULL;
	BOOLEAN				fSuccess = FALSE;

	bitmap = GetResourceBitmap( bitmapId );
	if(NULL == bitmap)
		goto LFail4;

#ifdef	_WINDOWS
	{
		BITMAP	bm ;
		TRECT		trc;
		HDC		hdcSrc;
		HGDIOBJ	hOldBitmap;

		::GetObject (bitmap, sizeof (BITMAP), (LPSTR) &bm);
		trcMaketrc(&trc, 0, 0, bm.bmWidth, bm.bmHeight);
		htBitmap = tbitAlloc ( static_cast<uWORD>( bm.bmPlanes * bm.bmBitsPixel ),
									  &trc,
									  TBIT_DefaultColorTable | TBIT_ZeroInit | TBIT_LongAlignRows );
		if(NULL == htBitmap)
			goto LFail3;
		if( !tbitLock( htBitmap ) )
			goto LFail2;
		hdcSrc = CreateCompatibleDC (NULL) ;
		if(NULL == hdcSrc)
			goto LFail1;
		hOldBitmap = SelectObject (hdcSrc, bitmap) ;
		tbitBlitFromDevice (htBitmap, (uLONG)hdcSrc, &trc, &trc, TBIT_SrcCopy, FALSE);
		SelectObject (hdcSrc, hOldBitmap) ;
		DeleteDC ( hdcSrc );
		fSuccess = TRUE;
	}
#else		//	!_WINDOWS
	UnimplementedCode();
#endif	//	_WINDOWS

LFail1:
	tbitUnlock( htBitmap );
LFail2:
	if(!fSuccess)
	{
		tbitFree( htBitmap );
		htBitmap = NULL;
	}
LFail3:
	FreeResourceBitmap( bitmap );
LFail4:
	return htBitmap;
	}

// ****************************************************************************
//
//  Function Name:	RResourceManager::GetResourceString( YResourceId stringId )
//
//  Description:		Get a resource string
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
RMBCString RResourceManager::GetResourceString( YResourceId stringId )
	{
	LPSZ	lpszString = (LPSZ)GetGlobalBuffer();
	*lpszString = 0;		//	Make sure the string is null terminated...
#ifdef	_WINDOWS
		//	Since the cursor might be in any of the 'ResourceFiles,' scan until one is found
	RResourceFileList::YIterator	iterator = m_ResourceFileList.Start();
	for ( ; iterator != m_ResourceFileList.End(); ++iterator )
		{
		if ( ::LoadString( *iterator, stringId, lpszString, GetGlobalBufferSize() ) != 0 )
			break;
		}

#else		//	!_WINDOWS
	UnimplementedCode();
	//	For now, this is NULL on the Mac.
#endif	//	_WINDOWS
	RMBCString	returnString(lpszString);
	ReleaseGlobalBuffer( (unsigned char*)lpszString );
	return returnString;
	}

// ****************************************************************************
//
//  Function Name:	RResourceManager::GetResourceDialogTemplate( YResourceId cursorId )
//
//  Description:		Loads a dialog template resource
//
//  Returns:			The dialog template
//
//  Exceptions:		None
//
// ****************************************************************************
//
YResourceDialogTemplate RResourceManager::GetResourceDialogTemplate( YResourceId dialogId )
	{
	YResourceDialogTemplate dialog = NULL;

#ifdef	_WINDOWS
	RResourceFileList::YIterator	iterator = m_ResourceFileList.Start();
	for ( ; iterator != m_ResourceFileList.End(); ++iterator )
		{
		HRSRC hResource = ::FindResource( *iterator, MAKEINTRESOURCE( dialogId ), RT_DIALOG );

		if( hResource )
			{
			dialog = ::LoadResource( *iterator, hResource );
			break;
			}
		}

#else		//	!_WINDOWS
	UnimplementedCode();
#endif	//	_WINDOWS

	TpsAssert( dialog != NULL, "Dialog could not be found in any open resource file.");
	return dialog;
	}

// ****************************************************************************
//
//  Function Name:	RResourceManager::GetResourceMenu
//
//  Description:		Loads a menu resource
//
//  Returns:			YResourceMenu
//
//  Exceptions:		None
//
// ****************************************************************************
//
YResourceMenu RResourceManager::GetResourceMenu( YResourceId menuId )
	{
	YResourceMenu menu = NULL;

#ifdef	_WINDOWS

	RResourceFileList::YIterator	iterator = m_ResourceFileList.Start( );
	for ( ; iterator != m_ResourceFileList.End( ) ; ++iterator )
		{
		menu = ::LoadMenu( *iterator, MAKEINTRESOURCE( menuId ) );
		if( menu )
			break;
		}

#else		//	!_WINDOWS
	UnimplementedCode();
#endif	//	_WINDOWS

	TpsAssert( menu != NULL, "Menu could not be found in any open resource file.");
	return menu;
	}

// ****************************************************************************
//
//  Function Name:	RResourceManager::GetResource( YResourceType yrt, YResourceId yrid )
//
//  Description:		Loads a resource of an arbitrary type
//
//  Returns:			YResource (HNATIVE) of the resource, or NULL
//
//  Exceptions:		None
//
// ****************************************************************************
//
YResource	RResourceManager::GetResource( YResourceType yrt, YResourceId yrid )
{
#ifdef	_WINDOWS
	char			lpResType[ kResTypeSize ];
	YResource	htr = NULL;

	tmemZeroBlock(lpResType, sizeof(lpResType));
	*((YResourceType*)lpResType) = yrt;

	RResourceFileList::YIterator	iterator = m_ResourceFileList.Start();
	for ( ; iterator != m_ResourceFileList.End(); ++iterator )
		{
		HRSRC hResource = ::FindResource( *iterator, MAKEINTRESOURCE( yrid ), lpResType );

		if( hResource )
			{
			htr = ::LoadResource( *iterator, hResource );
			break;
			}
		}
#else		//	!_WINDOWS
	::GetResource( yrt, yrid );	
#endif	//	_WINDOWS
	// Note (DCS) - Commented out the line below because the ResourceManager does not export
	// any functions equivalent to FindResource to check for the existence of a resource.
	//	TpsAssert( htr != NULL, "Resource could not be found in any open resource file.");
	return htr;
}


// ****************************************************************************
//
//  Function Name:	RResourceManager::GetResourceSize( YResourceType yrt, YResourceId yrid )
//
//  Description:		Returns the size (in bytes) of a resource.
//
//  Returns:			(DWORD) size of the resource.
//
//  Exceptions:		None
//
// ****************************************************************************
//
DWORD		RResourceManager::GetResourceSize( YResourceType yrt, YResourceId yrid )
{
#ifdef	_WINDOWS
	char			lpResType[ kResTypeSize ];
	YResource	htr = NULL;
	DWORD dwSize = 0;

	tmemZeroBlock(lpResType, sizeof(lpResType));
	*((YResourceType*)lpResType) = yrt;

	RResourceFileList::YIterator	iterator = m_ResourceFileList.Start();
	for ( ; iterator != m_ResourceFileList.End(); ++iterator )
		{
		HRSRC hResource = ::FindResource( *iterator, MAKEINTRESOURCE( yrid ), lpResType );

		if( hResource )
			{
			dwSize = ::SizeofResource( *iterator, hResource );
			break;
			}
		}
#else		//	!_WINDOWS
	TpsAssertAlways("Not implemented for Mac.");
#endif	//	_WINDOWS
	TpsAssert( dwSize > 0, "Resource could not be found in any open resource file.");
	return dwSize;
}


// ****************************************************************************
//
//  Function Name:	RResourceManager::FreeResource( YResource htr )
//
//  Description:		Free resource loaded with call RResourceManager::GetResource
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void	RResourceManager::FreeResource( YResource htr )
{
	TpsAssert( htr != NULL, "NULL resource passed to free.");
#ifdef	_WINDOWS
	::FreeResource( htr );
#else		//	!_WINDOWS
	::ReleaseResource( htr );
#endif	//	_WINDOWS

}

#ifdef	TPSDEBUG

// ****************************************************************************
//
//  Function Name:	RResourceManager::Validate( )
//
//  Description:		Validate the object
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RResourceManager::Validate( ) const
	{
	RObject::Validate( );
	}

#endif	TPSDEBUG
