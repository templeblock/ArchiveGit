// ****************************************************************************
//
//  File Name:			FileType.cpp
//
//  Project:			Renaissance Application framework
//
//  Author:				S. Athanas
//
//  Description:		Declaration of the RFileType class
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
//  $Logfile:: /PM8/Framework/Include/FileType.h                              $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:15p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#ifndef		_FILETYPE_H_
#define		_FILETYPE_H_

#ifdef BYTE_ALIGNMENT
#pragma pack(push, BYTE_ALIGNMENT)
#endif

class RStandaloneDocument;

// ****************************************************************************
//
//  Class Name:		RFileType
//
//  Description:		The base class for file types.
//
// ****************************************************************************
//
const RCharacter kNullSubstitute		= _TCHAR( '|' );
const _TCHAR     kNullSubString[]	= "|";
class FrameworkLinkage RFileType : public RObject
	{
	// Construction
	public :
												RFileType( const RMBCString& description, const RMBCString& extension );

	// Operations
	public :
		const RMBCString&					GetDescription( ) const;
		const RMBCString&					GetExtension( ) const;

	// Members
	private :
		RMBCString							m_Description;
		RMBCString							m_Extension;

#ifdef	TPSDEBUG
	// Debugging stuff
	public :
		virtual void						Validate( ) const;
#endif
	};

// ****************************************************************************
//
//  Class Name:		RFileTypeCollection
//
//  Description:		Class to encapsulate a collection of file types
//
// ****************************************************************************
//
class FrameworkLinkage RFileTypeCollection : public RArray<RFileType*>
	{
	// Construction & Destruction
	public :
												~RFileTypeCollection( );

	// Operations
	public :
		void									Merge( RFileType* pFileType );
		//RMBCString							GetFileDialogFilterString( ) const;
		void									GetFileDialogFilterString( RMBCString& filterString, BOOLEAN fGetRawString = FALSE ) const;
	};

typedef RFileTypeCollection::YIterator YFileTypeIterator;

// ****************************************************************************
// 					Inlines
// ****************************************************************************

#ifdef BYTE_ALIGNMENT
#pragma pack(pop)
#endif

#endif	// _FILETYPE_H_
