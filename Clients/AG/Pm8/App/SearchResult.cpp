// ****************************************************************************
//
//  File Name:			SearchResult.cpp
//
//  Project:			Renaissance Application
//
//  Author:				S. Athanas
//
//  Description:		Implmentation of the SearchResult class
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
//  $Logfile:: /PM8/App/SearchResult.cpp                                      $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:11p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#include "StdAfx.h"

ASSERTNAME

#include "SearchResult.h"
#include "ComponentTypes.h"
#include "ImageInterface.h"
#include "ApplicationGlobals.h"
#include "ComponentAttributes.h"
#include "ComponentDocument.h"
#include "ComponentView.h"
#include "ChunkyStorage.h"
#include "SearchCollection.h"
#include "ComponentInfoInterface.h"
#include "ComponentManager.h"
#include "RenaissanceResource.h"
#include "FrameworkResourceIDs.h"

SearchResult::RShapeToComponentMapEntry SearchResult::m_ShapeToComponentMap[ ] = 
	{
	RShapeToComponentMapEntry( stSquare,			0,		kSquareGraphicComponent ),
	RShapeToComponentMapEntry( stRow,				0,		kRowGraphicComponent ),
	RShapeToComponentMapEntry( stColumn,			0,		kColumnGraphicComponent ),
	RShapeToComponentMapEntry( stBackground,		0,		kBackdropGraphicComponent ),
	RShapeToComponentMapEntry( stPSDEnvBD,			0,		kBackdropGraphicComponent ),
	RShapeToComponentMapEntry( stImage,				0,		kPSDPhotoComponent ),
	RShapeToComponentMapEntry( stImageRef,			0,		kPSDPhotoComponent ),
	RShapeToComponentMapEntry( stFineArt,			0,		kPSDPhotoComponent ),
	RShapeToComponentMapEntry( stFineArtRef,		0,		kPSDPhotoComponent ),
	RShapeToComponentMapEntry( stBorder,			0,		kBorderComponent ),
	RShapeToComponentMapEntry( stBorder,			1,		kMiniBorderComponent ),
	RShapeToComponentMapEntry( stRule,				0,		kHorizontalLineComponent ),
	RShapeToComponentMapEntry( stRule,				1,		kVerticalLineComponent ),
	RShapeToComponentMapEntry( stTimepiece,		0,		kSquareGraphicComponent ),
	RShapeToComponentMapEntry( stInitcap,			0,		kSquareGraphicComponent ),
	RShapeToComponentMapEntry( stSeal,				0,		kSquareGraphicComponent ),
	RShapeToComponentMapEntry( stSpot,				0,		kSquareGraphicComponent ),
	RShapeToComponentMapEntry( stNumber,			0,		kSquareGraphicComponent ),
	RShapeToComponentMapEntry( stLogo,				0,		kSquareGraphicComponent ),
	RShapeToComponentMapEntry( stAutograph,		0,		kRowGraphicComponent ),
	RShapeToComponentMapEntry( stCalendarpart,	0,		kSquareGraphicComponent ),

	// Leapfrog-specific shape types
	RShapeToComponentMapEntry( stNavButton,		0,	  	kNavigationButtonSetComponent ),
	RShapeToComponentMapEntry( stIconButton,		0,		kIconButtonComponent ),
	RShapeToComponentMapEntry( stBullet,			0,		kBulletComponent ),
	RShapeToComponentMapEntry( stPagedivider,		0,		kPageDividerComponent ),
	RShapeToComponentMapEntry( stImage,				btTiled,kBackgroundComponent ),
//	RShapeToComponentMapEntry( stWebBackground,	0,		kBackgroundComponent ),
	RShapeToComponentMapEntry( stAnimation,		0,		kAnimationComponent )
	};
		   
const YFloatType kMaxBorderAspect = 1.3;
const YFloatType kMinBorderAspect = 1 / kMaxBorderAspect;

const YFloatType kMaxBannerAspect = 4;
const YFloatType kMinBannerAspect = 1 / kMaxBannerAspect;

//	amoust to shrink the component size of the line when previewing to make
//		the internal lines pieces larger.
const YRealDimension	kPreviewLineAdjstment	= 0.5F;

// ****************************************************************************
//
//  Function Name:	SearchResult::GetComponent( )
//
//  Description:		Gets a component identified by this search result data,
//
//  Returns:			Component document
//
//  Exceptions:		None
//
// ****************************************************************************
//
RComponentDocument* SearchResult::GetComponent( IComponentInfo* pInfo, BOOLEAN fPreview, int nSubType, const RRealSize& backdropSize ) const
{
	const YRealDimension kDefaultWidth = ::InchesToLogicalUnits( 2 );

	// Get the storage the item is in
	RStorage& storage = *( collectionStorage->GetDataParent( ) );

	TpsAssertValid( &storage );

	// Save the current storage position, so we can go back when we are done
	YStreamLength pos	= storage.GetPosition( );

	// Seek to the location in the storage of the item
	storage.SeekAbsolute( dataOffset );

	// Read in the graphic header
	GRAF_Header	grHeader;
	storage.Read( sizeof(grHeader), (uBYTE*) &grHeader );

	// Get the component type 
	const YComponentType& componentType = ComponentTypeFromShapeType( (cbSHAPETYPES)grHeader.graphicType, nSubType );

	// Compute component size
	RRealSize componentSize;

	// If it is a backdrop, use the size passed in. We have to pass it in, as path to project doesnt have a view yet.
	if( componentType == kBackdropGraphicComponent )
	{
		TpsAssert( backdropSize != RRealSize( 0, 0 ), "Backdrops must specify a size." );

		// If it is a tiled backdrop or a banner backdrop, just used the passed in size.
		if( grHeader.bkType == btTiled || grHeader.bkType == btHorzBanner || grHeader.bkType == btVertBanner )
		{
			componentSize = backdropSize;

			// Get the aspect ratio of that size
			YFloatType aspect = componentSize.m_dx / componentSize.m_dy;

			// Constrain the aspect to be within our allowable range. Otherwise long banners
			// will have absurd aspects
			if( aspect > kMaxBannerAspect )
				componentSize.m_dx /= ( aspect / kMaxBannerAspect );
			else if( aspect < kMinBannerAspect )
				componentSize.m_dy /= ( kMinBannerAspect / aspect );
		}

		// Otherwise use the size of the artwork
		else
		{
			// Load the DS_CLIPART header
			DS_CLIPART clipArtHeader;
			storage.Read( sizeof( clipArtHeader ), (uBYTE*) &clipArtHeader );

			// Get the artwork size
			componentSize.m_dx = clipArtHeader.xSize;
			componentSize.m_dy = clipArtHeader.ySize;
		}
	}
	// If its a border, get the size of our parent. We dont have the same problem as with backdrops,
	// as they are not selected through path-to-project
	else if( componentType == kBorderComponent )
	{
		// Get the default size for the border.
		TpsAssert( pInfo, "Invalid component info interface!" ) ;
		componentSize = pInfo->GetDefaultObjectSize( componentType ) ;

		// Get the aspect ratio of that size
		YFloatType aspect = componentSize.m_dx / componentSize.m_dy;

		// Constrain the aspect to be within our allowable range. If we dont, some projects,
		// like labels, will have borders so small that they wont show up
		if( aspect > kMaxBorderAspect )
			componentSize.m_dx /= ( aspect / kMaxBorderAspect );
		else if( aspect < kMinBorderAspect )
			componentSize.m_dy /= ( kMinBorderAspect / aspect );
	}
	//	If a vertical line or horizontal line, shrink the component size to
	//		make the pieces stand out more
	//			Main part of the coded copied from the else section below 12/8/97 MDH
	else if( componentType == kHorizontalLineComponent ||
				componentType == kVerticalLineComponent )
	{
		if (!pInfo && backdropSize != RRealSize( 0, 0 ))
		{
			TRACE( "Warning: Using backdropSize instead of pInfo->GetDefaultObjectSize\n" );
			componentSize = backdropSize;
		}
		else
		{
			TpsAssert( pInfo, "Invalid component info interface!" ) ;
			componentSize = pInfo->GetDefaultObjectSize( componentType ) ;
		}
		//	If this is a preview, adjust so the pieces will be larger
		if( fPreview )
		{
			if ( componentSize.m_dx > componentSize.m_dy )
				componentSize.m_dx *= kPreviewLineAdjstment;
			else
				componentSize.m_dy *= kPreviewLineAdjstment;
		}
	}
	// Otherwise, get aspect ratio for the component, and use a default size. Our caller will resize
	// later if necessary.
	else
	{
		// GCB 12/5/97 - for Print Shop path to project backdrop browser when
		// browsing bitmap backdrop, use backdropSize passed in
		if (!pInfo && backdropSize != RRealSize( 0, 0 ))
		{
			TRACE( "Warning: Using backdropSize instead of pInfo->GetDefaultObjectSize\n" );
			componentSize = backdropSize;
		}
		else
		{
			TpsAssert( pInfo, "Invalid component info interface!" ) ;
			componentSize = pInfo->GetDefaultObjectSize( componentType ) ;
		}

		// REVIEW LJW 09/19/97 - Can't the application provided default size work?
		//YFloatType aspectRatio = componentSize.m_dx / componentSize.m_dy;
		//componentSize = RRealSize( kDefaultWidth, kDefaultWidth / aspectRatio );
	}

	// REVIEW LJW 09/19/97 - Have the caller set the parent document and parent view
	// RDocument* pParentDocument = pParentView ? pParentView->GetRDocument( ) : NULL;

	RComponentAttributes attr ;

	if (pInfo)
	{
		attr = pInfo->GetDefaultObjectAttributes( componentType );
	}

	// Create a new component.
	RComponentDocument* pComponentDocument =
		::GetComponentManager( ).CreateNewComponent( componentType,
			NULL,
			NULL,
			attr,
			componentSize,
			FALSE );

	// Get the component view
	RComponentView* pComponentView = static_cast<RComponentView*>( pComponentDocument->GetActiveView( ) );

	// Try to get a graphic interface
	RGraphicInterface* pGraphicInterface = static_cast<RGraphicInterface*>( pComponentView->GetInterface( kGraphicInterfaceId ) );

	// If we got a graphic image, load with it
	if( pGraphicInterface )
	{
		// Seek to the location in the storage of the item
		storage.SeekAbsolute( dataOffset );

		// Load the graphic
		pGraphicInterface->Load( storage, GetGraphicType( (cbSHAPETYPES)grHeader.graphicType, grHeader.bkType ) );
		delete pGraphicInterface;

		// If its a tiled backdrop, set the tile attributes. Let a static function in the panel view do it,
		// as thats where the component is going to live.
		if( grHeader.bkType == btTiled )
		{
			//
			// REVIEW LJW 9/19/97 - Might as well set the tile attributes
			// here, but provide a static member function to set the
			// percentage if necessary.
			//
			// REVIEW LJW 8/13/97 - Set the default tile attriubtes
			// and then leave it up to the caller to set the correct
			// tile attributes.
			//
			SetTileAttributes( pComponentView );
		}
	}
	// No graphic interface, try to get an image interface
	else
	{
		RImageInterface* pImageInterface = static_cast<RImageInterface*>( pComponentView->GetInterface( kImageInterfaceId ) );

		// If we got an image interface, load with it
		if( pImageInterface )
		{
			// If we are previewing, load the thumbnail
			if( fPreview && grHeader.thumbnailSize > 0 )
			{
				YException hException;
				pImageInterface->Load( storage, grHeader.thumbnailSize, hException );
			}
			else // not previewing - load image into document
			{
				YStreamLength saveStoragePos = storage.GetPosition();
				collectionStorage->SelectRootChunk();
				RChunkStorage::RChunkSearcher yIter = collectionStorage->Start( 'CLHD', FALSE );
				TpsAssert( !yIter.End(), "Not a collection file!" );
				CollectionHeader header;
				*collectionStorage >> header.yCollectionID;
				*collectionStorage >> header.yLangLocale;
				*collectionStorage >> header.yCurFormatVersion;
				*collectionStorage >> header.yMinFormatVersion;
				*collectionStorage >> header.yCharEncoding;
				*collectionStorage >> header.yCollectionType;
				storage.SeekAbsolute( saveStoragePos );

				// if the collection is by-reference get path to file and load from it
				if (header.yCollectionType == utGraphicWithRef ||
					header.yCollectionType == utBackdropWithRef)
				{
					RWaitCursor waitCursor;
					RMBCString jpegPathPrefix;
					BOOL success = TRUE;
					BOOL keepTrying = TRUE;
					try
					{
						RCollectionManager::TheCollectionManager().UpdateImageByReferencePathPrefix( collectionStorage );
						for (; keepTrying; )
						{
							if (RCollectionManager::TheCollectionManager().GetImageByReferencePathPrefix( collectionStorage, jpegPathPrefix ))
								keepTrying = FALSE;
							else
							{
								if (RAlert().AskUserForCdSwap() == kAlertCancel)
									keepTrying = success = FALSE;
								else
									RCollectionManager::TheCollectionManager().UpdateImageByReferencePathPrefix( collectionStorage );
							}
						}
					}
					catch (...)
					{
						RAlert().AlertUser( STRING_ERROR_CORRUPT_COLLECTION_FILE );
					}

					try
					{
						// If we succeeded, load the image
						if (success)
						{
							// Seek past the thumbnail and the DS_CLIPIMAGE structure
							// REVIEW STA - When we have a declaration for DS_CLIPIMAGE, use sizeof it instead
							storage.SeekRelative( grHeader.thumbnailSize + 24 );
							if (grHeader.grDataSize < 0 || grHeader.grDataSize >= 256)
							{
								RAlert().AlertUser( STRING_ERROR_CORRUPT_COLLECTION_FILE );
							}
							else
							{
								unsigned char buf[256];
								storage.Read( grHeader.grDataSize, buf );
								buf[grHeader.grDataSize] = '\0';
								RMBCString jpegPathSuffix( (LPCSZ)buf );
								RMBCString fullPathToJPEG(jpegPathPrefix);
								fullPathToJPEG += jpegPathSuffix;
								RStorage jpegStorage( fullPathToJPEG, kRead );
								YStreamLength jpegFileSize = jpegStorage.GetStorageStream()->GetSize();
								YException hException;
								pImageInterface->Load( jpegStorage, jpegFileSize, hException, &jpegPathSuffix );
							}
						}
						// Otherwise the user canceled, delete the image and return NULL
						else
						{
							delete pComponentDocument;
							pComponentDocument = NULL;
						}
					}
					catch (...)
					{
						RAlert().AlertUser( STRING_ERROR_OPENIMGERR_FILENOTFOUND );
//						AfxMessageBox( STRING_ERROR_OPENIMGERR_FILENOTFOUND );
					}
				}
				else // image is by-value - load directly from collection file
				{
					// Seek past the thumbnail and the DS_CLIPIMAGE structure
					// REVIEW STA - When we have a declaration for DS_CLIPIMAGE, use sizeof it instead
					storage.SeekRelative( grHeader.thumbnailSize + 24 );

					// We are now positioned at the beginning of image data. Load from it.
					YException hException;
					pImageInterface->Load( storage, grHeader.grDataSize, hException );
				}
			}

			delete pImageInterface;
		}
		else
			TpsAssertAlways( "No useful interfaces available." );
	}

	// Get back to where we started in the storage
	storage.SeekAbsolute( pos );

	// If we are previewing, set the render threshold to zero
	if( fPreview && pComponentDocument )
		static_cast<RComponentView*>( pComponentDocument->GetActiveView( ) )->SetRenderCacheThreshold( 0 );

	return pComponentDocument;
}

// ****************************************************************************
//
//  Function Name:	SearchResult::ComponentTypeFromShapeType( )
//
//  Description:		Returns the type of Renaissance component that must be
//							created to hold an object of the specified shape type.
//
//  Returns:			Component type
//
//  Exceptions:		None
//
// ****************************************************************************
//
const YComponentType& SearchResult::ComponentTypeFromShapeType( cbSHAPETYPES shapeType, int nSubType ) const
	{
	for( int i = 0; i < NumElements( m_ShapeToComponentMap ); ++i )
		{
		if( m_ShapeToComponentMap[ i ].m_ShapeType == shapeType && m_ShapeToComponentMap[ i ].m_nSubType == nSubType )
			{
			return m_ShapeToComponentMap[ i ].m_ComponentType;
			}
		}

// GCB 12/2/97 allow for 6.0 expandability of graphic collection shape types
// by assuming that new shape types will always map to kPSDPhotoComponent
#if 0
	TpsAssertAlways( "Component type not found for shape type." );
	return kSquareGraphicComponent;
#else
	return kPSDPhotoComponent;
#endif
	}

// ****************************************************************************
//
//  Function Name:	SearchResult::GetGraphicType( )
//
//  Description:		Gets the type of graphic that must be created
//
//  Returns:			graphic type
//
//  Exceptions:		None
//
// ****************************************************************************
//
RGraphicInterface::EGraphicType SearchResult::GetGraphicType( cbSHAPETYPES shape, uBYTE backdropType ) const
	{
	struct RShapeToGraphic
		{
		cbSHAPETYPES							m_Shape;
		RGraphicInterface::EGraphicType	m_GraphicType;
		};

	static RShapeToGraphic shapeToGraphicMap[ ] =
		{
			{ stSquare,			RGraphicInterface::kSquare						},
			{ stRow,				RGraphicInterface::kRow							},
			{ stColumn,			RGraphicInterface::kColumn						},
			{ stBorder,			RGraphicInterface::kBorder						},
			{ stRule,			RGraphicInterface::kLine						},
			{ stSeal,			RGraphicInterface::kSeal						},
			{ stBackground,	RGraphicInterface::kBackdrop					},
			{ stPSDEnvBD,		RGraphicInterface::kBackdrop					},
			{ stLogo,			RGraphicInterface::kSquare						},
			{ stInitcap,		RGraphicInterface::kSquare						},
			{ stSpot,			RGraphicInterface::kSquare						},
			{ stNumber,			RGraphicInterface::kSquare						},
			{ stTimepiece,		RGraphicInterface::kTimepiece					},
			{ stAutograph,		RGraphicInterface::kRow							},
			{ stNavButton,		RGraphicInterface::kNavButtonSet				},
			{ stIconButton,	RGraphicInterface::kIconButton				},
			{ stCalendarpart,	RGraphicInterface::kSquare						},
			// { stWebBackground,RGraphicInterface::kWebBackground			},
			{ stBullet,			RGraphicInterface::kBullet						},
			{ stPagedivider,	RGraphicInterface::kPageDivider				},
			{ stAnimation,		RGraphicInterface::kAnimation					}
		};
		

	struct RBackgroundToGraphic
		{
		cbBACKGROUNDTYPES						m_Background;
		RGraphicInterface::EGraphicType	m_GraphicType;
		};

	static RBackgroundToGraphic backgroundToGraphicMap[ ] =
		{
			{ btHorzBanner,	RGraphicInterface::kHorzBannerBackdrop		},
			{ btVertBanner,	RGraphicInterface::kVertBannerBackdrop		},
			{ btTiled,			RGraphicInterface::kTiledBannerBackdrop	}
		};

	// If its a backdrop, we need to check against the backdrop type first
	if( shape == stBackground )
		for( int i = 0; i < NumElements( backgroundToGraphicMap ); ++i )
			if( backgroundToGraphicMap[ i ].m_Background == backdropType )
				return backgroundToGraphicMap[ i ].m_GraphicType;

	// Otherwise look through the shape map for a match
	for( int i = 0; i < NumElements( shapeToGraphicMap ); ++i )
		if( shapeToGraphicMap[ i ].m_Shape == shape )
			return shapeToGraphicMap[ i ].m_GraphicType;

	TpsAssertAlways( "Graphic type not found." );

	return RGraphicInterface::kSquare; 
	}

// ****************************************************************************
//
//  Function Name:	SearchResult::RShapeToComponentMapEntry::RShapeToComponentMapEntry
//
//  Description:		Constructor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
SearchResult::RShapeToComponentMapEntry::RShapeToComponentMapEntry( cbSHAPETYPES shape, int nSubType, const YComponentType& component )
	: m_ShapeType( shape ),
	  m_nSubType( nSubType ),
	  m_ComponentType( component )
	{
	;
	}

// ****************************************************************************
//
//  Function Name:	SearchResult::SetTileAttributes( )
//
//  Description:		Sets the tile attributes for a component
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void SearchResult::SetTileAttributes( RComponentView* pComponent ) const 
{
	const YFloatType kTileSizePercentage = 0.31;

	// Get the component size
	RRealSize size = pComponent->GetReferenceSize( );

	// Create the tile attributes
	RTileAttributes tileAttributes;

	if( size.m_dx > size.m_dy )
		tileAttributes.m_rTileSize.m_dx = tileAttributes.m_rTileSize.m_dy = kTileSizePercentage * size.m_dy;
	else
		tileAttributes.m_rTileSize.m_dx = tileAttributes.m_rTileSize.m_dy = kTileSizePercentage * size.m_dx;

	tileAttributes.m_rTileSpacing.m_dx = tileAttributes.m_rTileSpacing.m_dy = 0;

	// Tile the view
	pComponent->TileView( tileAttributes );
}

int SearchResult::CompareString( LCID Locale, DWORD dwCmpFlags, LPCTSTR lpString1, 
	int cchCount1, LPCTSTR lpString2, int cchCount2 )
{
	CString strSort1, strSort2 ;

	try
	{
		// Determine the length of the buffer needed to hold the 
		// destination sort key for the first string
		int i = LCMapString( Locale, dwCmpFlags | LCMAP_SORTKEY,
			lpString1, cchCount1, (LPTSTR) NULL, 0 ); 

		// Map the first string to a sort key
		i = LCMapString( Locale, dwCmpFlags | LCMAP_SORTKEY, 
			lpString1, cchCount1, (LPTSTR) strSort1.GetBuffer( i ), i ); 
		if (!i) throw kUnknownError ;
		
		// Determine the length of the buffer needed to hold the 
		// destination sort key for the second string
		i = LCMapString( Locale, dwCmpFlags | LCMAP_SORTKEY, 
			lpString2, cchCount2, (LPTSTR) NULL, 0 ); 

		// Map the second string to a sort key
		i = LCMapString( Locale, dwCmpFlags | LCMAP_SORTKEY, 
			lpString2, cchCount2, (LPTSTR) strSort2.GetBuffer( i ), i ); 
		if (!i) throw kUnknownError ;

		LPBYTE lpSort1 = (LPBYTE)(LPCTSTR) strSort1 ;
		LPBYTE lpSort2 = (LPBYTE)(LPCTSTR) strSort2 ;

		for (i = 0; (int) lpSort1[i] != 0x01 && (int) lpSort2[i] != 0x01; i++)
		{
			if (lpSort1[i] != lpSort2[i])
				break ;
		}

		if (lpSort1[i] < lpSort2[i])
			return 1; // Less than

		else if (lpSort1[i] > lpSort2[i])
			return 3; // Greater than

		return 2 ;	 // Equal
	}
	catch (YException)
	{
		return 0 ;
	}
}