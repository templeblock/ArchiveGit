// ****************************************************************************
//
//  File Name:			ComponentManager.cpp
//
//  Project:			Renaissance Application Framework
//
//  Author:				S. Athanas
//
//  Description:		Implementation of the RComponentManager class
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
//  $Logfile:: /PM8/Framework/Source/ComponentManager.cpp                     $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:16p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#include	"FrameworkIncludes.h"

ASSERTNAME

#include	"ComponentManager.h"

#undef	FrameworkLinkage
#define	FrameworkLinkage

#include "ComponentApplication.h"
#include "ComponentDocument.h"
#include "ComponentView.h"
#include "FrameworkResourceIds.h"
#include "ApplicationGlobals.h"
#include "Menu.h"
#include "StandaloneApplication.h"
#include "ResourceManager.h"

// ****************************************************************************
//
//  Function Name:	RComponentManager::RComponentManager( )
//
//  Description:		Constructor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
RComponentManager::RComponentManager( )
	: m_fLoadComponentsCalled( FALSE )
	{
	;
	}

// ****************************************************************************
//
//  Function Name:	RComponentManager::CreateNewComponent( )
//
//  Description:		Creates a new component
//
//  Returns:			Pointer to the component
//
//  Exceptions:		Memory Exception
//
// ****************************************************************************
//
RComponentDocument* RComponentManager::CreateNewComponent( const YComponentType& componentType, 
																			  RDocument* pParentDocument,
																			  const RComponentAttributes& componentAttributes,
																			  BOOLEAN fLoading )
	{
	RComponentDocument* pComponentDocument	= NULL;

	// Iterate the server list looking for the appropriate server
	for( YIterator i = Start( ); i != End( ); ++i )
		{
		// Get the supported component types
		RComponentTypeCollection componentTypeCollection;
		( *i ).m_pApplication->GetSupportedComponentTypes( componentTypeCollection );

		//	Look if we have the correct type of component
		if( componentTypeCollection.Find( componentType ) != componentTypeCollection.End( ) )
			{
			// Create a new component
			RDocument* pNewDocument = ( *i ).m_pApplication->CreateNewDocument( componentAttributes, componentType, fLoading );
			pComponentDocument = dynamic_cast<RComponentDocument*>(pNewDocument);
			TpsAssert( pComponentDocument, "The created document was not a Component Document");

			// Set its parent. I really didnt want to have to do this, but the text component needs to
			// know its parent at creation time
			pComponentDocument->SetParentDocument( pParentDocument );

			return pComponentDocument;
			}
		}

	TpsAssertAlways( "Component server not available." );
	return NULL;
	}

// ****************************************************************************
//
//  Function Name:	RComponentManager::CreateNewComponent( )
//
//  Description:		Creates a new component and creates a view for it
//
//  Returns:			Pointer to the component
//
//  Exceptions:		Memory Exception
//
// ****************************************************************************
//
RComponentDocument* RComponentManager::CreateNewComponent( const YComponentType& componentType, 
																			  RDocument* pParentDocument,
																			  RView* pParentView,
																			  const RComponentAttributes& componentAttributes,
																			  const YComponentBoundingRect& boundingRect,
																			  BOOLEAN fLoading )
	{
	// Create a new component
	RComponentDocument* pComponentDocument = CreateNewComponent( componentType,
																					 pParentDocument,
																					 componentAttributes,
																					 fLoading );

	// Create a new view for the component
	RComponentView* pComponentView = pComponentDocument->CreateView( boundingRect, pParentView );

	// Add the view to the document
	pComponentDocument->AddRView( pComponentView );

	return pComponentDocument;
	}

// ****************************************************************************
//
//  Function Name:	RComponentManager::IsComponentAvailable( )
//
//  Description:		Determines if the specified component is available.
//
//  Returns:			TRUE if the component is available
//
//  Exceptions:		None
//
// ****************************************************************************
//
BOOLEAN RComponentManager::IsComponentAvailable( const YComponentType& componentType ) const
	{
	// Iterate the server list looking for the appropriate server
	for( YIterator i = Start( ); i != End( ); ++i )
		{
		// Get the supported component types
		RComponentTypeCollection componentTypeCollection;
		( *i ).m_pApplication->GetSupportedComponentTypes( componentTypeCollection );

		// See if the requested type is supported
		if( componentTypeCollection.Find( componentType ) != componentTypeCollection.End( ) )
			return TRUE;
		}

	return FALSE;
	}

// ****************************************************************************
//
//  Function Name:	RComponentManager::LoadComponentServers( )
//
//  Description:		This routine searches the component path looking for
//							Renaissance component servers. It loads and initializes
//							any servers which it finds.
//
//  Returns:			Nothing
//
//  Exceptions:		FALSE if the app should terminate
//
// ****************************************************************************
//
#pragma optimize("",off)
void RComponentManager::LoadComponentServers( const RMBCString& componentPath, YMenuIndex componentSubMenuId )
	{
	TpsAssert( m_fLoadComponentsCalled == FALSE, "LoadComponentServers has already been called" );
	m_fLoadComponentsCalled = TRUE;
	int numNonStandardComponents = 0;

	// Get the component submenu
	RMenu* pComponentMenu = NULL;
	YMenuIndex nonstandardIndex = 0;

	if( ::GetApplication( ) )
		{
		pComponentMenu = ::GetApplication( ).GetApplicationMenu( ).GetSubMenu( componentSubMenuId );

		// Get the index of the nonstandard placeholder menu item
		nonstandardIndex = pComponentMenu->GetIndexFromCommand( COMMAND_MENU_INSERT_NONSTANDARD );
		}

#ifdef	_WINDOWS
	typedef RApplication* (FAR WINAPI *INITPROC)( RApplicationGlobals* pGlobals );

	WIN32_FIND_DATA FindFileData;

	// Create the path to pass to FindFirstFile
	RMBCString searchPath = componentPath;
	searchPath += "\\*.dll";

#ifdef	HIPROF 	//Wynn for HiProf
	searchPath = "\\development\\Renaissance\\Bins\\*compdebug.dll";
#endif

	// Find the first matching file
	HANDLE hFind = ::FindFirstFile( searchPath, &FindFileData );

	while( hFind != INVALID_HANDLE_VALUE )
		{
		// Attempt to load the .dll
		searchPath = componentPath;
		searchPath += "\\";

#ifdef	HIPROF	//Wynn for HiProf
		searchPath = "\\development\\Renaissance\\Bins\\";
#endif
		searchPath += FindFileData.cFileName;

		HINSTANCE hInstance = ::LoadLibrary( searchPath );
		if( hInstance )
			{
			// Look for our init proc
			INITPROC InitProc = (INITPROC)::GetProcAddress( hInstance, "InitializeComponent" );
			if( InitProc )
				{
				// We found our init proc, this is a Renaissance component server. Create
				// a new server info struct, and add it to the list
				RComponentServerInfo componentServerInfo;
				componentServerInfo.m_hInstance = hInstance;
				componentServerInfo.m_pApplication = (RComponentApplication*)InitProc( RApplication::GetApplicationGlobals( ) );
				TpsAssertIsObject( RComponentApplication, componentServerInfo.m_pApplication );

				// Do we need to append the component to the component menu?
				if( componentServerInfo.m_pApplication->AppendToComponentMenu( ) )
					{
					// Get the supported component types
					RComponentTypeCollection componentTypeCollection;
					componentServerInfo.m_pApplication->GetSupportedComponentTypes( componentTypeCollection );
					TpsAssert( componentTypeCollection.Count( ) > 0, "No component types." );

					// Add the component types to the menu
					RComponentTypeCollection::YIterator iterator = componentTypeCollection.Start( );
					for( ; iterator != componentTypeCollection.End( ); ++iterator )
						{
						TpsAssert( pComponentMenu, "No menu to add non-standard component to." );

						pComponentMenu->InsertItemByIndex( nonstandardIndex + numNonStandardComponents,
																	  (YCommandID)( COMMAND_MENU_INSERT_NONSTANDARDSTART + numNonStandardComponents ),
																	  *iterator );

						++numNonStandardComponents;
						}

					// Add the server in the correct position
					int i = 0;
					for( YIterator serverIterator = Start( ); serverIterator != End( ); ++serverIterator, ++i )
						if( i == numNonStandardComponents - 1 )
							{
							InsertAt( serverIterator, componentServerInfo );
							break;
							}
					}

				else
					{
					// Add the server to the end of the server list
					InsertAtEnd( componentServerInfo );
					}

				GetResourceManager().AddResourceFile( hInstance );
				}
			}
		// Look for the next file
		if( !::FindNextFile( hFind, &FindFileData ) )
			{
			::FindClose( hFind );
			hFind = INVALID_HANDLE_VALUE;
			}
		}

#endif	// _WINDOWS

	if( Count( ) == 0 )
		{
		RAlert( ).AlertUser( STRING_ERROR_GENERAL_COMPONENT_MISSING );
		throw FALSE;
		}

	// Remove the nonstandard placeholder menu item
	if( pComponentMenu )
		pComponentMenu->RemoveItemByCommand( COMMAND_MENU_INSERT_NONSTANDARD );

	// Delete the submenu
	delete pComponentMenu;
	}
#pragma optimize("",on)

// ****************************************************************************
//
//  Function Name:	RComponentManager::UnloadComponentServers( )
//
//  Description:		Frees the component servers, and any associated memory.
//
//  Returns:			Nothing
//
//  Exceptions:		Nothing
//
// ****************************************************************************
//
void RComponentManager::UnloadComponentServers( )
	{
	TpsAssert( m_fLoadComponentsCalled == TRUE, "Load Component Servers has not been called");

#ifdef	_WINDOWS

	// Iterate the component list, freeing the servers
	for( YIterator i = Start( ); i != End( ); ++i )
		{
		GetResourceManager().RemoveResourceFile( ( *i ).m_hInstance );
		WinCode( ::FreeLibrary( ( *i ).m_hInstance ) );
		}

#endif	//	_WINDOWS
	}

// ****************************************************************************
//
//  Function Name:	RComponentManager::GetNonstandardComponentType( )
//
//  Description:		Gets the component types of the non standard compnent
//							identified by the specified index.
//
//  Returns:			See above
//
//  Exceptions:		None
//
// ****************************************************************************
//
const YComponentType RComponentManager::GetNonstandardComponentType( YComponentIndex componentIndex ) const
	{
	for( YIterator i = Start( ); i != End( ); ++i )
		{
		TpsAssert( componentIndex >= 0, "Negative component index not allowed." );

		RComponentTypeCollection componentTypeCollection;
		( *i ).m_pApplication->GetSupportedComponentTypes( componentTypeCollection );
		if( componentIndex < componentTypeCollection.Count( ) )
			return componentTypeCollection.Start( )[ componentIndex ];
		else
			componentIndex -= componentTypeCollection.Count( );
		}

	TpsAssertAlways( "Component server not found." );

	return YComponentType( );
	}

// ****************************************************************************
//																												 
//  Function Name:	RComponentManager::MapDataFormatToComponentType( )
//
//  Description:		Function which maps a data format the its associated
//							Renaissance component type
//
//  Returns:			TRUE if there is a mapping between the specified format
//							and a Renaissance component type.
//
//  Exceptions:		None
//
// ****************************************************************************
//
BOOLEAN RComponentManager::MapDataFormatToComponentType( YDataFormat dataFormat, YComponentType& componentType ) const
	{
	// Iterate the server list looking for a component that can handle that data
	for( YIterator i = Start( ); i != End( ); ++i )
		{
		// See if the component type can paste this data format
		if( ( *i ).m_pApplication->CanPasteDataFormat( dataFormat, componentType ) )
			{
			return TRUE;
			}
		}

	return FALSE;
	}
