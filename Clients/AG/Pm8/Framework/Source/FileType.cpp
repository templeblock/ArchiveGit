// ****************************************************************************
//
//  File Name:			FileType.cpp
//
//  Project:			Renaissance Application framework
//
//  Author:				S. Athanas
//
//  Description:		Definition of the RFileType and RFileTypeCollection class
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
//  $Logfile:: /PM8/Framework/Source/FileType.cpp                             $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:17p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#include	"FrameworkIncludes.h"

ASSERTNAME

#include	"FileType.h"

#undef	FrameworkLinkage
#define	FrameworkLinkage

//*****************************************************************************
//
// Function Name:	RFileType::RFileType
//
// Description:	Constructor
//
// Returns:			Nothing
//
// Exceptions:		None
//
//*****************************************************************************
//
RFileType::RFileType( const RMBCString& description, const RMBCString& extension )
	: m_Description( description ),
	  m_Extension( extension )
	{
	;
	}

//*****************************************************************************
//
// Function Name:	RFileType::GetDescription
//
// Description:	Accessor
//
// Returns:			File type description
//
// Exceptions:		None
//
//*****************************************************************************
//
const RMBCString& RFileType::GetDescription( ) const
	{
	return m_Description;
	}

//*****************************************************************************
//
// Function Name:	RFileType::GetExtension
//
// Description:	Accessor
//
// Returns:			File type extension
//
// Exceptions:		None
//
//*****************************************************************************
//
const RMBCString& RFileType::GetExtension( ) const
	{
	return m_Extension;
	}

//*****************************************************************************
//
// Function Name:	RFileTypeCollection::~RFileTypeCollection
//
// Description:	Destructor
//
// Returns:			Nothing
//
// Exceptions:		None
//
//*****************************************************************************
//
RFileTypeCollection::~RFileTypeCollection( )
	{
	::DeleteAllItems( *this );
	}

//*****************************************************************************
//
// Function Name:	RFileTypeCollection::Merge
//
// Description:	Merges a file type into this collection. The collection
//						adopts ownership of the type.
//
// Returns:			Nothing
//
// Exceptions:		None
//
//*****************************************************************************
//
void RFileTypeCollection::Merge( RFileType* pFileType )
	{
	// If the collection is empty, just insert it and leave
	if( Count( ) == 0 )
		{
		InsertAtEnd( pFileType );
		return;
		}

	// Check to see if the extension already exists in the collection
	const RMBCString& extension = pFileType->GetExtension( );
	YFileTypeIterator iterator = Start( );
	for( ; iterator != End( ); ++iterator )
		if( extension == ( *iterator )->GetExtension( ) )
			{
			// Found a matching extension. Just kill it
			delete pFileType;
			return;
			}

	// Look for a matching description
	const RMBCString& description = pFileType->GetDescription( );
	iterator = End( );
	do
		{
		--iterator;
		if( description == ( *iterator )->GetDescription( ) )
			{
			// Found a matching description. Insert here
			InsertAt( iterator, pFileType );
			return;
			}
		}
	while( iterator != Start( ) );

	// Didnt find a match anywhere, insert at the end
	InsertAtEnd( pFileType );
	}

//*****************************************************************************
//
// Function Name:	RFileTypeCollection::GetFileDialogFilterString
//
// Description:	Creates a filter string from the supported file types
//
// Returns:			VOID
//
// Exceptions:		None
//
//*****************************************************************************
//
void RFileTypeCollection::GetFileDialogFilterString( RMBCString& filterString, BOOLEAN fGetRawString ) const
	{
	const RCharacter kExtensionSeperator = _TCHAR( ';' );
	//RMBCString filterString;
	RMBCString lastDescription;

	// Iterate the file types, getting the description and extension for each one
	YFileTypeIterator iterator = Start( );
	for( ; iterator != End( ); ++iterator )
		{
		if( lastDescription == ( *iterator )->GetDescription( ) )
			{
			filterString += kExtensionSeperator;
			filterString += "*.";
			filterString += ( *iterator )->GetExtension( );
			}

		else
			{
			if( iterator != Start( ) )
				filterString += kNullSubstitute;

			filterString += ( *iterator )->GetDescription( );
			filterString += kNullSubstitute;
			filterString += "*.";
			filterString += ( *iterator )->GetExtension( );

			lastDescription = ( *iterator )->GetDescription( );
			}
		}

	filterString += kNullSubstitute;
	filterString += kNullSubstitute;
	//
	// The fGetRawString flag allows the string to be returned before the NULLs are substituted.
	if( fGetRawString ) return;
	// Convert all kNullSubstitutes to '\0's
	int stringLength = filterString.GetStringLength( );
	LPUBYTE	pString	= new uBYTE[ stringLength + 1 ];
	strcpy( (char*)pString, (LPCSZ)filterString );

	filterString.Empty( );
	for( int i = 0; i < stringLength; ++i )
		{
		if( pString[ i ] == kNullSubstitute )
			filterString += RCharacter( '\0' );
		else
			filterString += RCharacter( pString[i] );
		}

	delete [] pString;

	//return stringLength;
	}

#ifdef TPSDEBUG

// ****************************************************************************
//
//  Function Name:	RFileType::Validate( )
//
//  Description:		Validation
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RFileType::Validate( ) const
	{
	RObject::Validate( );
	TpsAssertIsObject( RFileType, this );
	}

#endif	// TPSDEBUG
