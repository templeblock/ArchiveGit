// ****************************************************************************
//
//  File Name:			FileFormat.cpp
//
//  Project:			Renaissance Application framework
//
//  Author:				S. Athanas
//
//  Description:		Declaration of the RFileFormat class
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
//  $Logfile:: /PM8/Framework/Include/FileFormat.h                            $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:15p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#ifndef		_FILEFORMAT_H_
#define		_FILEFORMAT_H_

#ifdef BYTE_ALIGNMENT
#pragma pack(push, BYTE_ALIGNMENT)
#endif

class RStandaloneDocument;
class RFileType;
class RFileTypeCollection;
class RBitmapImage;

//	chunk tag for the document type
const uLONG kDocumentTypeTag = 'DCTP';

// ****************************************************************************
//
//  Class Name:		RFileFormat
//
//  Description:		The base class for file formats.
//
// ****************************************************************************
//
class FrameworkLinkage RFileFormat : public RObject
	{
	// Operations
	public :
		virtual RStandaloneDocument*	Load( const RMBCString& filename ) = 0;
		virtual void						Save( const RMBCString& filename ) const = 0;
		virtual void						Revert( const RMBCString& filename, RStandaloneDocument* pDocument ) const = 0;
		virtual BOOLEAN					CheckFile( const RMBCString& filename ) const = 0;
		virtual RFileType*				GetSaveFileType( ) const = 0;
		virtual void						MergeLoadFileTypes( RFileTypeCollection& fileTypeCollection ) const = 0;
		virtual BOOLEAN					IsSymmetric( ) const = 0;
		virtual RBitmapImage*			GetPreviewBitmap( const RMBCString& filename ) const = 0;

#ifdef	TPSDEBUG
	// Debugging stuff
	public :
		virtual void						Validate( ) const;
#endif
	};

// ****************************************************************************
//
//  Class Name:		RNativeFileFormat
//
//  Description:		The base class for native file formats supported by the
//							framework.
//
// ****************************************************************************
//
class FrameworkLinkage RNativeFileFormat : public RFileFormat
	{
	// Constructor
	public :
												RNativeFileFormat( RStandaloneDocument* pDocument );

	// Operations
	public :
		virtual RStandaloneDocument*	Load( const RMBCString& filename );
		virtual RStandaloneDocument*	Load( RChunkStorage& storage );
		virtual void						Save( const RMBCString& filename ) const;
		virtual void						Save( const RMBCString& filename, EWriteReason reason ) const;
		virtual void						Revert( const RMBCString& filename, RStandaloneDocument* pDocument ) const;
		virtual BOOLEAN					CheckFile( const RMBCString& filename ) const;
		virtual RFileType*				GetSaveFileType( ) const;
		virtual void						MergeLoadFileTypes( RFileTypeCollection& fileTypeCollection ) const;
		virtual BOOLEAN					IsSymmetric( ) const;
		virtual RBitmapImage*			GetPreviewBitmap( const RMBCString& filename ) const;
		virtual void						WriteData( RChunkStorage& storage, EWriteReason reason ) const;

	// Implementation
	private :
		virtual void						ReadDocument( RStandaloneDocument* pDocument, RChunkStorage& storage, EReadReason reason ) const;

	// Data
	protected :
		RStandaloneDocument*				m_pDocument;
	};

// ****************************************************************************
//
//  Class Name:		RFileFormatCollection
//
//  Description:		Class to encapsulate a collection of file formats
//
// ****************************************************************************
//
class FrameworkLinkage RFileFormatCollection : public RArray<RFileFormat*>
	{
	// Construction & Destruction
	public :
												~RFileFormatCollection( );

	// Operations
	public :
		void									GetSaveFileTypes( RFileTypeCollection& fileTypeCollection ) const;
		void									GetLoadFileTypes( RFileTypeCollection& fileTypeCollection ) const;
		RFileFormat*						GetMatchingFileFormat( const RMBCString& filename ) const;
	};

typedef RFileFormatCollection::YIterator YFileFormatIterator;

// ****************************************************************************
// 					Inlines
// ****************************************************************************

#ifdef BYTE_ALIGNMENT
#pragma pack(pop)
#endif

#endif	// _FILEFORMAT_H_
