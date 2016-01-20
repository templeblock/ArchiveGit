// ****************************************************************************
//
//  File Name:			ResourceManager.h
//
//  Project:			Renaissance Application Framework
//
//  Author:				M. Houle
//
//  Description:		Declaration of the RResourceManager class
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
//  $Logfile:: /PM8/Framework/Include/ResourceManager.h                       $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:16p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#ifndef		_RESOURCEMANAGER_H_
#define		_RESOURCEMANAGER_H_

#ifdef BYTE_ALIGNMENT
#pragma pack(push, BYTE_ALIGNMENT)
#endif

const		kResTypeSize =	5;			//	size of 4 character ID for resource type

#define	SetResourceType(trt, a,b,c,d)	{((uBYTE*)&(trt))[0] = a, ((uBYTE*)&(trt))[1] = b, ((uBYTE*)&(trt))[2] = c, ((uBYTE*)&(trt))[3] = d;}

// ****************************************************************************
//
//  Class Name:	RResourceManager
//
//  Description:	All resources will be accessed through this class. This will
//						us to abstract the resource format and platform information
//
// ****************************************************************************
//
class FrameworkLinkage RResourceManager : public RObject
	{
	// Construction, destruction & initialization
	public :
													RResourceManager( );
		virtual									~RResourceManager( );


	//	Operations
	public :
		void										AddResourceFile( YResourceFileReference fileRef );
		void										RemoveResourceFile( YResourceFileReference fileRef );

		YResourceCursor						GetResourceCursor( YResourceId cursorId );
		YResourceBitmap						GetResourceBitmap( YResourceId bitmapId );
		HTBIT										GetHTBitResourceBitmap( YResourceId bitmapId );
		void										FreeResourceBitmap( YResourceBitmap bitmap );
		RMBCString								GetResourceString( YResourceId stringId );
		YResourceDialogTemplate				GetResourceDialogTemplate( YResourceId dialogId );
		YResourceMenu							GetResourceMenu( YResourceId menuId );
		YResource								GetResource( YResourceType yrt, YResourceId yrid );
		void										FreeResource( YResource htr );

		uLONG										GetResourceSize( YResourceType yrt, YResourceId yrid );

	//	Members
	private :
		typedef	RArray<YResourceFileReference>		RResourceFileList;

		RResourceFileList						m_ResourceFileList;

#ifdef	TPSDEBUG
	public :
		virtual	void							Validate( ) const;
#endif	//	TPSDEBUG
	} ;

#ifdef BYTE_ALIGNMENT
#pragma pack(pop)
#endif

#endif	//	_RESOURCEMANAGER_H_



