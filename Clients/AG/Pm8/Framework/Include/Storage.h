// ****************************************************************************
//
//  File Name:			Storage.h
//
//  Project:			Renaissance Framework
//
//  Author:				E. VanHelene
//
//  Description:		Implementation of the RStorage class
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
//  Copyright (C) 1995-1996 Turning Point Software. All Rights Reserved.
//
//  $Logfile:: /PM8/Framework/Include/Storage.h                               $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:16p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#ifndef		_STORAGE_H_
#define		_STORAGE_H_

//	enumerated definitions and types
enum EPlatformTag	{ kMac='MMMM', kWin='IIII' };
typedef EPlatformTag YPlatformTag;

class RStream;
class RFileStream;
class RMemoryStream;

class FrameworkLinkage RStorage : public RArchive
	{
	// Construction, Destruction & Initialization
	public :
										RStorage( );
										RStorage( RStream* pStream );
										RStorage( RFileStream* pStream );
										RStorage( RMemoryStream* pStream );
										RStorage( const RMBCString& rFilename, EAccessMode eOpenMode );
		virtual						~RStorage( );

		// Operations
		public :
		virtual void				Read( YStreamLength yBufferSize, uBYTE* pubBuffer );
		virtual void				Write( YStreamLength yBufferSize, const uBYTE* pubBuffer );
		virtual void				SeekAbsolute( const YStreamLength ySeekPosition );
		virtual void				SeekRelative( const YStreamOffset ySeekOffset );
		virtual YStreamLength	GetPosition( ) const;
		virtual void				Flush( );
		void							Copy( RStream* pStream );
		RStream *					GetStorageStream( ) const ;
		static YPlatformTag		GetPlatformTag( ) ;
		void							DeleteStreamOnClose( );
		void							WriteFailed( const BOOLEAN fDeleteOnClose );
		virtual void				PreserveFileDate( const BOOLEAN fPreserveFileDate = TRUE );
		virtual BOOLEAN			ShouldPreserveFileDate( );

#ifdef TPSDEBUG
	// Debugging Support
	public :
		virtual void 				Validate( ) const;
#endif

	// Implementation
	private :
	
	// Member data
	protected :
		RStream*						m_pStream;						//	data storage
		BOOLEAN	 					m_fDeleteStreamOnClose;		//	delete storage when chunk is deleted
	};


// ****************************************************************************
// 					Inlines
// ****************************************************************************

#endif //	_STORAGE_H_
