// ****************************************************************************
//
//  File Name:			PSDGraphicLibrary.cpp
//
//  Project:			Renaissance Framework
//
//  Author:				S. Athanas
//
//  Description:		Definition of the RPsd3GraphicLibrary class
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
//  $Logfile:: /PM8/GraphicComp/Source/Psd3GraphicLibrary.cpp                 $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:18p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#include	"GraphicIncludes.h"

ASSERTNAME

#include "Psd3GraphicLibrary.h"
#include "Stream.h"

#ifdef IMAGER
	#ifdef MAC
		extern short gGraphicType;
	#endif	// MAC
#endif	// IMAGER

// ****************************************************************************
//
// Function Name:		RPsd3GraphicLibrary::RPsd3GraphicLibrary( )
//
// Description:		Constructor
//
// Returns:				nothing
//
// Exceptions:			None
//
// ****************************************************************************
//
RPsd3GraphicLibrary::RPsd3GraphicLibrary( )
	: m_GraphicInfoArray( NULL ),
	  m_pGraphicNames( NULL ),
	  m_pGraphicIDs(NULL)
	{
	;
	}

// ****************************************************************************
//
// Function Name:		RPsd3GraphicLibrary::~RPsd3GraphicLibrary( )
//
// Description:		Destructor
//
// Returns:				nothing
//
// Exceptions:			None
//
// ****************************************************************************
//
RPsd3GraphicLibrary::~RPsd3GraphicLibrary( )
{
	delete [] m_GraphicInfoArray;

	// delete the graphic name array

	if (m_GraphicInfoArray)
	{
		for(int i = 0; i < m_LibraryHeader.m_Count; i++) delete [] m_pGraphicNames[i];
	}

	delete [] m_pGraphicNames;

	// Delete the graphic IDs array
	delete [] m_pGraphicIDs;
}

// ****************************************************************************
//
// Function Name:		RPsd3GraphicLibrary::OpenLibrary( )
//
// Description:		Loads the header and index of a PSD graphic library out
//							of the given stream
//
// Returns:				TRUE if the library was successfully loaded.
//
//							FALSE if the given stream is not a PSD graphic library,
//
// Exceptions:			Memory exception, File exceptions
//
// ****************************************************************************
//
BOOLEAN RPsd3GraphicLibrary::OpenLibrary( RStream& stream )
{
	TpsAssertValid( (&stream) );

	// Save the library start position
	m_LibraryStartPosition = stream.GetPosition( );

	// Read the library header
	stream.Read( sizeof(m_LibraryHeader), (uBYTE*)&m_LibraryHeader );

	// byte swap data on mac
#ifdef MAC
	tintSwapWords( (uWORD *)&(m_LibraryHeader.m_FileID), 2 );
	tintSwapWords( (uWORD *)&(m_LibraryHeader.m_Count), 3 );
#endif

	const char* kVerifyString = "PSDELUXE.";

	// Check for the string "PSDELUXE." If this string is not present, it is not a valid PSD library
#ifdef WIN 
	// REVIEW TBD write xplatform equiv of strnicmp
	if( _strnicmp( m_LibraryHeader.m_FileIdentity, kVerifyString, strlen( kVerifyString ) ) != 0 )
		return FALSE;
#endif

	// Read in the array of graphic data structures
	m_GraphicInfoArray = new RPsd3GraphicInfo[ m_LibraryHeader.m_Count ];
	
	int i;
	for( i = 0; i < m_LibraryHeader.m_Count; i++ )
	{
		stream.Read( sizeof( RPsd3GraphicInfo ), (uBYTE*)&m_GraphicInfoArray[ i ] );

	// byte swap data on mac
#ifdef MAC
		tintSwapLongs( (uLONG *)&(m_GraphicInfoArray[ i ].m_GraphicID), 3 );
		m_GraphicInfoArray[ i ].m_Code = tintSwapWord( m_GraphicInfoArray[ i ].m_Code );
#endif
	}

	// Make sure we have a recognizable library type
	if (GetLibraryType() == kUnknownLibrary) return FALSE;

	// Now go through and validate the graphics
	// Init the position to the graphic data
	YStreamPosition position = m_LibraryStartPosition + sizeof( m_LibraryHeader ) + ( m_LibraryHeader.m_Count * sizeof( RPsd3GraphicInfo ) );

	// Go through the graphic information, getting pointers to the names of valid graphics
	for( i = 0; i < m_LibraryHeader.m_Count; i++ )
	{
		// Seek to the correct place in the file
		stream.SeekAbsolute( position );

		// Allocate a buffer to hold the graphic data
		uBYTE* pBuffer = new uBYTE[ m_GraphicInfoArray[ i ].m_GraphicSize ];

		// Read the graphic data into the buffer
		stream.Read( m_GraphicInfoArray[ i ].m_GraphicSize, pBuffer );

	// byte swap data on mac
#ifdef MAC
		tintSwapStructs( pBuffer, sizeof(RPsd3GraphicHeader), 1, 0x000000A54AL );
#endif
		
		// Increment the position
		position += m_GraphicInfoArray[i].m_GraphicSize;

		delete [] pBuffer;
	}

	// Allocate an array of NULL terminated graphic names. (Names are not NULL
	// terminated in the library file.) Only do for valid graphics.
	// Also create an array of graphic IDs.	
	m_pGraphicNames = new char*[ m_LibraryHeader.m_Count ];
	m_pGraphicIDs = new YPsd3GraphicID[m_LibraryHeader.m_Count];	
	for( i = 0; i < m_LibraryHeader.m_Count; i++ )
	{
		// Copy name
		m_pGraphicNames[ i ] = new char[ kGraphicNameLength + 1 ];
		strncpy( m_pGraphicNames[ i ], m_GraphicInfoArray[ i ].m_Name, kGraphicNameLength );

		// Copy ID
		m_pGraphicIDs[ i ] = m_GraphicInfoArray[ i ].m_GraphicID;

		// Validate some info

/*		// Removed this check because too many libraries were not correct and QA would have*/
/*		// been extremly difficult due to the large amount of asserts that would be triggered.*/
/*		// Compression*/
/*		TpsAssert( ((m_GraphicInfoArray[i].m_GraphicCompressionScheme >= 3) && (m_GraphicInfoArray[i].m_GraphicCompressionScheme <= 7)) ||*/
/*					m_GraphicInfoArray[i].m_GraphicCompressionScheme == 0,*/
/*					"Invalid Library Graphic compression scheme, value is not 0 or between 3 and 7.");*/
		// mono flag
		TpsAssert( (0 == m_GraphicInfoArray[i].m_Mono) || (1 == m_GraphicInfoArray[i].m_Mono), "Invalid value for MonoFlag.");

		// screening value
		TpsAssert( 100 == m_GraphicInfoArray[i].m_Screen, "Screening value is not 100.");
	}

#ifdef	TPSDEBUG
		// Make sure the PSD library data is in the valid ranges
		Validate();
#endif	// TPSDEBUG

	return TRUE;
}

// ****************************************************************************
//
// Function Name:		RPsd3GraphicLibrary::GetGraphicType( )
//
// Description:		Figures out the type of this library
//
// Returns:				The type
//
// Exceptions:			None
//
// ****************************************************************************
//
RPsd3GraphicLibrary::ELibraryType RPsd3GraphicLibrary::GetLibraryType( ) const
{
	// For reasons beyond my knowledge, you cannot trust the library type field
	// of the library header to be correct. Instead, you have to figure it out
	// based on the extension of the file. I am assuming that the extension of the
	// m_FileIdentity field is correct; if not we have to do some more work to get
	// the real file name.

	// See Psd3 source code, Libarray.cpp

	const int kTypeExtensionLength = 3;

	// Struct used in the library key array
	struct RGraphicKey
	{
		 char											szExtension[ kTypeExtensionLength + 1 ];
		 RPsd3GraphicLibrary::ELibraryType	Type;
	};

	// List of library extensions, and their corresponding library types.
	static const RGraphicKey graphicKeyArray[ ] = 
		{	{ "PBK", kBackdropLibrary },
			{ "PBR", kBorderLibrary },
			{ "CBR", kCertificateBorderLibrary },
			{ "PSG", kSquareGraphicLibrary },
			{ "PCG", kColumnGraphicLibrary },
			{ "PRG", kRowGraphicLibrary },
			{ "PRL", kRuledLineLibrary },
//			{ "CLY", LIB_COMPLAYOUT},
//			{ "PLY", LIB_PSDLAYOUT},
			{ "PSE", kSealExteriorLibrary },
			{ "PSI", kSealInteriorLibrary },
//			{ "PSG", LIB_INITIALCAPS},
			{ "PCT", kTimepieceLibrary },
			{ "PAU", kSignatureLibrary },
			{ "PSB", kSpeechBubbleLibrary },
//			{ "PBK", LIB_MINIBACKDROP}
		};

	// Get the library type extension
	const char* szExtension = strchr( m_LibraryHeader.m_FileIdentity, '.' );

	TpsAssert( strlen( szExtension ) == kTypeExtensionLength + 1, "Invalid library type extension." );
	szExtension++;

	// Loop through the library key array looking for the extension
	ELibraryType eLibraryType = kUnknownLibrary;

	int nNumKeys = sizeof( graphicKeyArray ) / sizeof( RGraphicKey );
	for( int i = 0; i < nNumKeys; i++ )
	{
		if( strcmp( szExtension, graphicKeyArray[ i ].szExtension ) == 0 )
		{
			eLibraryType = graphicKeyArray[ i ].Type;
			break;
		}
	}

	return eLibraryType;
}

// ****************************************************************************
//
// Function Name:		RPsd3GraphicLibrary::GetGraphicByIndex( )
//
// Description:		Loads a graphic out of this graphic library.
//
// Returns:				A pointer to an RPsdGraphic object. NULL if the graphic
//							was not found.
//
// Exceptions:			Memory exception, File exceptions
//
// ****************************************************************************
//
RPsd3Graphic* RPsd3GraphicLibrary::GetGraphicByIndex( RStream& stream, uWORD uwGraphicIndex, BOOLEAN ) const
	{
	TpsAssert( uwGraphicIndex < m_LibraryHeader.m_Count, "Invalid graphic index." );
	
	// Allocate a buffer to hold the graphic data
	uBYTE* pBuffer = NULL;
	pBuffer = new uBYTE[ m_GraphicInfoArray[ uwGraphicIndex ].m_GraphicSize ];

	// Seek to the correct place in the stream
	YStreamPosition position = m_LibraryStartPosition + sizeof( m_LibraryHeader ) + ( m_LibraryHeader.m_Count * sizeof( RPsd3GraphicInfo ) );
	for( uWORD i = 0; i < uwGraphicIndex; i++ )
		position += m_GraphicInfoArray[ i ].m_GraphicSize;

	stream.SeekAbsolute( position );

	// Read the graphic data into the buffer
	stream.Read( m_GraphicInfoArray[ uwGraphicIndex ].m_GraphicSize, pBuffer );

	RPsd3Graphic* pPsd3Graphic = NULL;

	try
		{
#ifdef IMAGER	
		// We need to know what kind of graphic we have for the Mac Imager
 		gGraphicType = GetGraphicType( GetLibraryType( ), m_GraphicInfoArray[ uwGraphicIndex ].m_SubprojectType );
#endif	// IMAGER

		// Create a new graphic
		pPsd3Graphic = RPsd3Graphic::CreateNewGraphic( GetGraphicType( GetLibraryType( ), m_GraphicInfoArray[ uwGraphicIndex ].m_SubprojectType ) );

		// Try to load it
		if(!pPsd3Graphic->Load(pBuffer, m_GraphicInfoArray[ uwGraphicIndex ].m_GraphicSize, m_GraphicInfoArray[ uwGraphicIndex ].m_Mono))
			throw;
		}

	catch(...)
		{
		delete pPsd3Graphic;		
		delete [] pBuffer;
		
		return NULL;
		}
	
	// Delete the buffer
	delete [] pBuffer;
	
	return pPsd3Graphic;
	}

// ****************************************************************************
//
// Function Name:		RPsd3GraphicLibrary::GetGraphicType( )
//
// Description:		Gets the graphic type from the library type
//
// Returns:				Graphic type
//
// Exceptions:			None
//
// ****************************************************************************
//
RGraphicInterface::EGraphicType RPsd3GraphicLibrary::GetGraphicType( ELibraryType libraryType, YPsd3Subproject subProject ) const
	{
	struct RLibraryToGraphic
		{
		RPsd3GraphicLibrary::ELibraryType	m_LibraryType;
		RGraphicInterface::EGraphicType		m_GraphicType;
		};

	RLibraryToGraphic libraryToGraphicMap[ ] =
		{
			{ kSquareGraphicLibrary,		RGraphicInterface::kSquare			},
			{ kRowGraphicLibrary,			RGraphicInterface::kRow				},
			{ kColumnGraphicLibrary,		RGraphicInterface::kColumn			},
			{ kBorderLibrary,					RGraphicInterface::kBorder			},
			{ kCertificateBorderLibrary,	RGraphicInterface::kBorder			},
			{ kRuledLineLibrary,				RGraphicInterface::kLine			},
			{ kSealInteriorLibrary,			RGraphicInterface::kSeal			},
			{ kSealExteriorLibrary,			RGraphicInterface::kSeal			}, 
			{ kTimepieceLibrary,				RGraphicInterface::kTimepiece		},
			{ kBackdropLibrary,				RGraphicInterface::kBackdrop		}
		};

	struct RSubProjectToGraphic
		{
		YPsd3Subproject						m_SubProject;
		RGraphicInterface::EGraphicType	m_GraphicType;
		};

	RSubProjectToGraphic backgroundToGraphicMap[ ] =
		{
			{ kHorizontalBanner,			RGraphicInterface::kHorzBannerBackdrop		},
			{ kVerticalBanner,			RGraphicInterface::kVertBannerBackdrop		},
			{ kTile,							RGraphicInterface::kTiledBannerBackdrop	}
		};

	// If its a backdrop, we need to check against the subproject type first
	if( libraryType == kBackdropLibrary )
		{
		for( int i = 0; i < NumElements( backgroundToGraphicMap ); ++i )
			if( backgroundToGraphicMap[ i ].m_SubProject == subProject )
				return backgroundToGraphicMap[ i ].m_GraphicType;
		}
	// Otherwise look through the library map for a match
	for( int i = 0; i < NumElements( libraryToGraphicMap ); ++i )
		{
		if( libraryToGraphicMap[ i ].m_LibraryType == libraryType )
			return libraryToGraphicMap[ i ].m_GraphicType;

		}
	TpsAssertAlways( "Graphic type not found." );

	return RGraphicInterface::kSquare; 
	}

// ****************************************************************************
//
// Function Name:		RPsd3GraphicLibrary::GetGraphic( )
//
// Description:		Loads a graphic out of this graphic library.
//
// Returns:				A pointer to an RPsdGraphic object.
//
// Exceptions:			Memory exception, File exceptions
//
// ****************************************************************************
//
RPsd3Graphic* RPsd3GraphicLibrary::GetGraphic( RStream& stream, const char* pszGraphicName, BOOLEAN fKeepRawData ) const
	{
	RPsd3Graphic* pGraphic = NULL;

	// Look for the graphic
	for( uWORD i = 0; i < m_LibraryHeader.m_Count; i++ )
		{
		if( strncmp( pszGraphicName, m_GraphicInfoArray[ i ].m_Name, kGraphicNameLength ) == 0 )
			{
			// Found the graphic. Now load it
			pGraphic = GetGraphicByIndex( stream, i, fKeepRawData );
			break;
			}
		}

	return pGraphic;
	}

// ****************************************************************************
//
// Function Name:		RPsd3GraphicLibrary::GetGraphic( )
//
// Description:		Loads a graphic out of this graphic library.
//
// Returns:				A pointer to an RPsdGraphic object. NULL if the graphic
//							was not found.
//
// Exceptions:			Memory exception, File exceptions
//
// ****************************************************************************
//
RPsd3Graphic* RPsd3GraphicLibrary::GetGraphic( RStream& stream, YPsd3GraphicID graphicID, BOOLEAN fKeepRawData ) const
	{
	RPsd3Graphic* pGraphic = NULL;

	// Look for the graphic
	for( uWORD i = 0; i < m_LibraryHeader.m_Count; i++ )
		{
		if( graphicID == m_GraphicInfoArray[ i ].m_GraphicID )
			{
			// Found the graphic. Now load it
			pGraphic = GetGraphicByIndex( stream, i, fKeepRawData );
			break;
			}
		}

	return pGraphic;
	}

// ****************************************************************************
//
// Function Name:		RPsd3GraphicLibrary::GetRawDataByIndex( )
//
// Description:		Get the raw graphic data as stored in the library.
//
// Returns:				A pointer to raw graphic data; caller is responsible for
//							deleting this pointer.
//
// Exceptions:			Memory exception, File exceptions
//
// ****************************************************************************
//
uBYTE* RPsd3GraphicLibrary::GetRawDataByIndex( RStream& stream, uWORD uwGraphicIndex, uLONG& ulRawDataLength ) const
	{
	uWORD	i;
	// Seek to the correct place in the stream
	YStreamPosition position = m_LibraryStartPosition + sizeof( m_LibraryHeader ) + ( m_LibraryHeader.m_Count * sizeof( RPsd3GraphicInfo ) );
	for( i = 0; i < uwGraphicIndex; i++ )
		position += m_GraphicInfoArray[ i ].m_GraphicSize;

	stream.SeekAbsolute( position );

	// Allocate memory for the library header, a graphic info structure, and the graphic data
	ulRawDataLength = sizeof( RPsd3GraphicLibraryHeader ) + sizeof( RPsd3GraphicInfo ) + m_GraphicInfoArray[ i ].m_GraphicSize;
	uBYTE* pGraphicData = new uBYTE[ ulRawDataLength ];

	// Copy the library header
	memcpy( pGraphicData, &m_LibraryHeader, sizeof( m_LibraryHeader ) );

	// Get a pointer to the library header, and fix up some fields
	RPsd3GraphicLibraryHeader* pLibraryHeader = (RPsd3GraphicLibraryHeader*)pGraphicData;
	pLibraryHeader->m_Count = 1;

	// Copy the graphic info struct
	memcpy( pGraphicData + sizeof( RPsd3GraphicLibraryHeader ),
			  &m_GraphicInfoArray[ i ],
			  sizeof( RPsd3GraphicInfo ) );

	// Seek to the graphic data
	stream.SeekAbsolute( position );

	// Read the graphic data
	stream.Read( m_GraphicInfoArray[ uwGraphicIndex ].m_GraphicSize,
					 pGraphicData + sizeof( RPsd3GraphicLibraryHeader ) + sizeof( RPsd3GraphicInfo ) );
					
	position += m_GraphicInfoArray[ i ].m_GraphicSize;

	return pGraphicData;
	}

// ****************************************************************************
//
// Function Name:		RPsd3GraphicLibrary::GetRawData( )
//
// Description:		Get the raw graphic data as stored in the library.
//
// Returns:				A pointer to raw graphic data; caller is responsible for
//							deleting this pointer.
//
// Exceptions:			Memory exception, File exceptions
//
// ****************************************************************************
//
uBYTE* RPsd3GraphicLibrary::GetRawData( RStream& stream, const char* pszGraphicName, uLONG& ulRawDataLength ) const
	{
	uBYTE* pGraphic = NULL;

	// Look for the graphic
	for( uWORD i = 0; i < m_LibraryHeader.m_Count; i++ )
		{
		if( strncmp( pszGraphicName, m_GraphicInfoArray[ i ].m_Name, kGraphicNameLength ) == 0 )
			{
			// Found the graphic. 
			pGraphic =  GetRawDataByIndex( stream, i, ulRawDataLength );

			break;
			}
		}

	return pGraphic;
	}

// ****************************************************************************
//
// Function Name:		RPsd3GraphicLibrary::GetRawData( )
//
// Description:		Get the raw graphic data as stored in the library.
//
// Returns:				A pointer to raw graphic data; caller is responsible for
//							deleting this pointer.
//
// Exceptions:			Memory exception, File exceptions
//
// ****************************************************************************
//
uBYTE* RPsd3GraphicLibrary::GetRawData( RStream& stream, YPsd3GraphicID graphicID, uLONG& ulRawDataLength ) const
	{
	uBYTE* pGraphic = NULL;

	// Look for the graphic
	for( uWORD i = 0; i < m_LibraryHeader.m_Count; i++ )
		{
		if( graphicID == m_GraphicInfoArray[ i ].m_GraphicID )
			{
			// Found the graphic. 
			pGraphic =  GetRawDataByIndex( stream, i, ulRawDataLength );

			break;
			}
		}

	return pGraphic;
	}

// ****************************************************************************
//
// Function Name:		RPsd3GraphicLibrary::GetGraphicNames( )
//
// Description:		Creates an array of graphic names. The array contains
//							ulNumGraphics entries, each one of which is a NULL
//							terminated string. The caller is responsible for freeing
//							the array.
//
//							The char*'s in the array are only valid for as long as
//							this RPsd3GraphicLibrary object is valid
//
// Returns:				Nothing
//
// Exceptions:			Memory exception
//
// ****************************************************************************
//
char** RPsd3GraphicLibrary::GetGraphicNames( uWORD& numGraphics ) const
	{
	TpsAssert( m_pGraphicNames, "OpenLibrary not called." );

	// Fill in the output parameter
	numGraphics = m_LibraryHeader.m_Count;

	// Return a pointer to the name array
	return m_pGraphicNames;
	}

// ****************************************************************************
//
// Function Name:		RPsd3GraphicLibrary::GetGraphicIDs)
//
// Description:		Returns a pointer to an array of the graphic IDs
//							contained in the library.  The pointer is only valid
//							as long as the library instance exists.
//
// Returns:				Nothing
//
// Exceptions:			
//
// ****************************************************************************
//
YPsd3GraphicID* RPsd3GraphicLibrary::GetGraphicIDs( uWORD& numGraphics ) const
	{
	TpsAssert( m_pGraphicIDs, "OpenLibrary not called." );

	// Fill in the output parameter
	numGraphics = m_LibraryHeader.m_Count;

	// Return a pointer to the name array
	return m_pGraphicIDs;
	}


#ifdef	TPSDEBUG

// ****************************************************************************
//
// Function Name:		RPsd3GraphicLibrary::Validate( )
//
// Description:		Validates the object
//
// Returns:				nothing
//
// Exceptions:			None
//
// ****************************************************************************
//
void RPsd3GraphicLibrary::Validate( ) const
{
	RObject::Validate( );
	TpsAssertIsObject( RPsd3GraphicLibrary, this );

	// GraphicLibraryHeader
	const int kMinGraphicCount	= 1;
	// const int kMaxGraphicCount	= 1000;
	char*	  fName				= "PSDeluxe";
	 	
	TpsAssert( m_LibraryHeader.m_Count >= kMinGraphicCount, "Graphic count out of range, less than 1." );
	// TpsAssert( m_LibraryHeader.m_Count <= kMaxGraphicCount, "Graphic count out of range, greater than 1000." );
	
	TpsAssert( 0 == memcmp(fName, m_LibraryHeader.m_FileIdentity, 8), "File name is not \"PSDeluxe.xxx\" format.");

//	Following assert removed because too many PSD3 and PSD4 libraries failed it which
//	caused problems for automated testing (BDR 4/17/97)
//	TpsAssert( 0x0100 == m_LibraryHeader.m_Machine, "Library was not built on a Macintosh.");

//	Removed to fix bug 1508 since old libraries didn't always conform.
//	TpsAssert( TRUE == m_LibraryHeader.m_BroderbundCreated, "Library was not created by Broderbund.");
}

#endif	// TPSDEBUG
