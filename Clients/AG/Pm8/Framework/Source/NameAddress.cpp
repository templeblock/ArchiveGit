// ****************************************************************************
//
//  File Name:			NameAddress.cpp
//
//  Project:			Renaissance
//
//  Author:				Michael Houle
//
//  Description:		Implementation RAddress And Address Formating
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
//  $Logfile:: /PM8/Framework/Source/NameAddress.cpp                          $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:17p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#include	"FrameworkIncludes.h"

ASSERTNAME

#include	"NameAddress.h"

#undef	FrameworkLinkage
#define	FrameworkLinkage

#include "MergeData.h"

const RCharacter	kFormatIdentifierTag( '%' );
const RCharacter	kFirstNameIdentifierTag( 'f' );
const RCharacter	kLastNameIdentifierTag( 'l' );
const RCharacter	kAddressIdentifierTag( 'a' );
const RCharacter	kCityIdentifierTag( 'c' );
const RCharacter	kStateIdentifierTag( 's' );
const RCharacter	kZipIdentifierTag( 'z' );
const RCharacter	kCountryIdentifierTag( 'C' );

const RCharacter	kSpaceTag( ' ' );
const RCharacter	kReturnTag( '\n' );
const RCharacter	kTabTag( '\t' );
const RCharacter	kCommaTag( ',' );

// ****************************************************************************
//
//  Function Name:	RNameAddress::FormatNameAddress( )
//
//  Description:		Perform the formatting described in the format string to
//							the given inputs and put the result into output
//
//  Returns:			Nothing
//
//  Exceptions:		kMemory
//
// ****************************************************************************
//
void RNameAddress::FormatNameAddress( RMBCString& output, const RMBCString& format,
									const RMBCString& firstName, const RMBCString& lastName, const RMBCString& address,
									const RMBCString& city, const RMBCString& state, const RMBCString& zip,
									const RMBCString& country )
{
	output.Empty( );

	RMBCStringIterator	iterator		= format.Start( );
	RMBCStringIterator	iteratorEnd	= format.End( );

	for ( ; iterator != iteratorEnd; ++iterator )
	{
		RCharacter	ch	= ( *iterator );
		if ( ch == kFormatIdentifierTag )
		{
			++iterator;
			TpsAssert( iterator!=iteratorEnd, "Format Identifier found at end of stream" );
			ch = ( *iterator );
			if ( ch == kFormatIdentifierTag )
				output	+= kFormatIdentifierTag;
			else if ( ch == kFirstNameIdentifierTag )
				output	+= firstName;
			else if ( ch == kLastNameIdentifierTag )
				output	+= lastName;
			else if ( ch == kAddressIdentifierTag )
				output	+= address;
			else if ( ch == kCityIdentifierTag )
				output	+= city;
			else if ( ch == kStateIdentifierTag )
				output	+= state;
			else if ( ch == kZipIdentifierTag )
				output	+= zip;
			else if ( ch == kCountryIdentifierTag )
				output	+= country;
			else
			{
				TpsAssertAlways( "Unknown format tag found" );
			}
		}
		else
		{
			if ( ch == kSpaceTag )
				output	+= ch;
			else if ( ch == kReturnTag )
				output	+= ch;
			else if ( ch == kTabTag )
				output	+= ch;
			else if ( ch == kCommaTag )
				output	+= ch;
			else
			{
				//TpsAssertAlways( "Unsupported Character, If it is necessary add it to the MergeField info and here" );
				output	+= ch;
			}
		}
	}

}

// ****************************************************************************
//
//  Function Name:	RNameAddress::FormatNameAddress( )
//
//  Description:		Perform the formatting described in the format string to
//							the given inputs and put the result into address
//
//  Returns:			Nothing
//
//  Exceptions:		kMemory
//
// ****************************************************************************
//
void RNameAddress::FormatNameAddress( RMergeFieldContainer& output, const RMBCString& format,
										YMergeId firstName, YMergeId lastName, YMergeId address,
										YMergeId city, YMergeId state, YMergeId zip, YMergeId country )
{
	output.Empty( );

	RMBCStringIterator	iterator		= format.Start( );
	RMBCStringIterator	iteratorEnd	= format.End( );

	for ( ; iterator != iteratorEnd; ++iterator )
	{
		RCharacter	ch	= ( *iterator );
		if ( ch == kFormatIdentifierTag )
		{
			++iterator;
			TpsAssert( iterator!=iteratorEnd, "Format Identifier found at end of stream" );
			ch = ( *iterator );
			if ( ch == kFirstNameIdentifierTag )
				output.InsertAtEnd( firstName );
			else if ( ch == kLastNameIdentifierTag )
				output.InsertAtEnd( lastName );
			else if ( ch == kAddressIdentifierTag )
				output.InsertAtEnd( address );
			else if ( ch == kCityIdentifierTag )
				output.InsertAtEnd( city );
			else if ( ch == kStateIdentifierTag )
				output.InsertAtEnd( state );
			else if ( ch == kZipIdentifierTag )
				output.InsertAtEnd( zip );
			else if ( ch == kCountryIdentifierTag )
				output.InsertAtEnd( country );
			else
			{
				TpsAssertAlways( "Unknown format tag found" );
			}
		}
		else
		{
			if ( ch == kSpaceTag )
				output.InsertAtEnd( kMergeFieldSpace );
			else if ( ch == kReturnTag )
				output.InsertAtEnd( kMergeFieldReturn );
			else if ( ch == kTabTag )
				output.InsertAtEnd( kMergeFieldTab );
			else if ( ch == kCommaTag )
				output.InsertAtEnd( kMergeFieldComma );
			else
			{
				TpsAssertAlways( "Unsupported Character, If it is necessary add it to the MergeField info and here" );
			}
		}
	}

}

