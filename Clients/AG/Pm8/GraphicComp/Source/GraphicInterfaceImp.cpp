//****************************************************************************
//
// File Name:		GraphicSettings.cpp
//
// Project:			Renaissance Graphic Component
//
// Author:			Mike Heydlauf
//
// Description:	Manages Data.
//
// Portability:	Platform Independant
//
// Developed by:	Turning Point Software
//						One Gateway Center, Suite 800
//						Newton, Ma 02158
//						(617)332-0202
//
//  Client:			Broderbund Software, Inc.         
//
//  Copyright (C) 1996 Turning Point Software. All Rights Reserved.
//
//  $Logfile:: /PM8/GraphicComp/Source/GraphicInterfaceImp.cpp                   $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:18p                                                  $
// $Revision:: 1                                                              $
//
//****************************************************************************

#include "GraphicIncludes.h"

ASSERTNAME

#include "GraphicInterfaceImp.h"
#include "GraphicDocument.h"
#include "GraphicView.h"
#include "Psd3SingleGraphic.h"
#include "Psd3TimepieceGraphic.h"
#include "EditGraphicAction.h"
#include "PropertyAttribImp.h"
#include "ChangeColorAction.h"
#include "ChunkyStorage.h"
#include "Stream.h"

#include "MemoryMappedBuffer.h"
#include "DataInterface.h"

// ****************************************************************************
//
//  Function Name:	RGraphicHolderImp::RGraphicHolderImp( )
//
//  Description:		Constructor.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
RGraphicHolderImp::RGraphicHolderImp( RGraphic* pGraphic, BOOLEAN fOwnData )
{
	TpsAssertIsObject( RPsd3SingleGraphic, pGraphic );
	RPsd3SingleGraphic*	pPsd3SingleGraphic = static_cast<RPsd3SingleGraphic*>( pGraphic );
	m_Color = pGraphic->GetMonochromeColor( );

	if (fOwnData)
	{
		m_pGraphic = pPsd3SingleGraphic ; 
	}
	else
	{
		m_pGraphic	= new RPsd3SingleGraphic( *pPsd3SingleGraphic );
	}
}

// ****************************************************************************
//
//  Function Name:	RGraphicHolderImp::~RGraphicHolderImp( )
//	
//  Description:		Destructor.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
RGraphicHolderImp::~RGraphicHolderImp( )
{
	delete m_pGraphic;
}


// ****************************************************************************
//
//  Function Name:	RGraphicHolderImp::GetGraphic( )
//	
//  Description:		Return the Graphic
//
//  Returns:			m_pGraphic
//
//  Exceptions:		None
//
// ****************************************************************************
//
RPsd3SingleGraphic* RGraphicHolderImp::GetGraphic( ) const
{
	return m_pGraphic;
}

// ****************************************************************************
//
//  Function Name:	RGraphicHolderImp::GetColor( )
//	
//  Description:		Return the Color
//
//  Returns:			m_Color
//
//  Exceptions:		None
//
// ****************************************************************************
//
const RColor& RGraphicHolderImp::GetColor( ) const
{
	return m_Color;
}

#ifdef	TPSDEBUG
// ****************************************************************************
//
//  Function Name:	RGraphicHolderImp::Validate( )
//
//  Description:		Validate class.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RGraphicHolderImp::Validate( ) const
{
	RGraphicHolder::Validate( );
	m_pGraphic->Validate( );
}

#endif	//	TPSDEBUG


// ****************************************************************************
//
//  Function Name:	RGraphicInterfaceImp::RGraphicInterfaceImp( )
//
//  Description:		Constructor.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
RGraphicInterfaceImp::RGraphicInterfaceImp( const RGraphicView* pGraphicView )
	: RGraphicInterface( pGraphicView ),
	  m_pGraphicDocument( const_cast<RGraphicDocument*>( pGraphicView->GetGraphicDocument( ) ) ),
	  m_pGraphic( NULL )
{
	NULL;
}

// ****************************************************************************
//
//  Function Name:	RGraphicInterfaceImp::CreateInterface( )
//
//  Description:		Constructor function.
//
//  Returns:			new RInterface
//
//  Exceptions:		kMemory
//
// ****************************************************************************
//
RInterface* RGraphicInterfaceImp::CreateInterface( const RComponentView* pView )
{
	TpsAssertIsObject( RGraphicView, pView );
	return new RGraphicInterfaceImp( static_cast<const RGraphicView*>( pView ) );
}

// ****************************************************************************
//
//  Function Name:	RGraphicInterfaceImp::GetData( )
//
//  Description:		Return the data from the object that this interface references.
//
//  Returns:			new RGraphicHolder (containing the data)
//
//  Exceptions:		kMemory, kUnknownException
//
// ****************************************************************************
//
RComponentDataHolder*	RGraphicInterfaceImp::GetData( )
{
	return GetData( FALSE ) ;
}

// ****************************************************************************
//
//  Function Name:	RGraphicInterfaceImp::GetData( )
//
//  Description:		Return the data from the object that this interface references.
//
//  Returns:			new RGraphicHolder (containing the data)
//
//  Exceptions:		kMemory, kUnknownException
//
// ****************************************************************************
//
RComponentDataHolder*	RGraphicInterfaceImp::GetData( BOOLEAN fRemove )
{
	RGraphic*	pGraphic	= m_pGraphicDocument->GetGraphic( );

	if ( pGraphic )
	{
		TpsAssertIsObject( RPsd3SingleGraphic, pGraphic );
		RPsd3SingleGraphic*	pPsd3SingleGraphic = static_cast<RPsd3SingleGraphic*>( pGraphic );
		
		if (fRemove)
		{
			ClearData() ;
		}

		return new RGraphicHolderImp( pGraphic, fRemove );
	}

	return NULL;
}

// ****************************************************************************
//
//  Function Name:	RGraphicInterfaceImp::EditData( )
//
//  Description:		Edit the object
//
//  Returns:			nothing
//
//  Exceptions:		kMemory, kUnknownException
//
// ****************************************************************************
//
void	RGraphicInterfaceImp::EditData( )
{
	//	If there is a grapyhic, copy it to allow editing.
	//		This is necessary since EditGraphicAction WILL destroy the old graphic....
	m_pGraphic	= static_cast<RPsd3SingleGraphic*>( m_pGraphicDocument->GetGraphic( ) );
	if ( m_pGraphic )
	{
		TpsAssertIsObject( RPsd3SingleGraphic, m_pGraphicDocument->GetGraphic( ) );
		RPsd3SingleGraphic* pTmpGraphic = new RPsd3SingleGraphic( *m_pGraphic );
		m_pGraphicDocument->SetGraphic( pTmpGraphic );
	}

	REditGraphicAction	action( m_pGraphicDocument ); 
	action.Do( );
}

// ****************************************************************************
//
//  Function Name:	RGraphicInterfaceImp::SetData( )
//
//  Description:		Set this data into the object.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RGraphicInterfaceImp::SetData( RComponentDataHolder* pHolder )
{
	if ( pHolder != NULL )
	{
		TpsAssertIsObject( RGraphicHolderImp, pHolder );
		RGraphicHolderImp* pGraphicHolder	= static_cast<RGraphicHolderImp*>( pHolder );
		m_pGraphic	= pGraphicHolder->GetGraphic( );
		m_pGraphicDocument->SetGraphic( m_pGraphic );
		m_pGraphic->SetMonochromeColor( pGraphicHolder->GetColor( ) );
	}
	else
	{
		m_pGraphic = NULL;
	}
}

// ****************************************************************************
//
//  Function Name:	RGraphicInterfaceImp::ClearData( )
//
//  Description:		Free memory associated with this interface
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RGraphicInterfaceImp::ClearData( )
{
	m_pGraphic	= NULL;
	m_pGraphicDocument->SetGraphic( m_pGraphic );
}

// ****************************************************************************
//
//  Function Name:	RGraphicInterfaceImp::Load( )
//
//  Description:		Read in the data into the associated Graphic.from
//							old Print Shop collection format
//
//  Returns:			Nothing
//
//  Exceptions:		kMemory, Disk Errors
//
// ****************************************************************************
//
void RGraphicInterfaceImp::Load( RStorage& storage, EGraphicType graphicType )
{
	RPsd3Graphic* pGraphic = RPsd3Graphic::CreateNewGraphic( graphicType );

	uBYTE*  pGraphicData ;
	uLONG   uGraphicDataSize ;	
	BOOLEAN fMonochrome ;

	const int kMonochromeOffset = 16 ;

	storage.SeekRelative( kMonochromeOffset ) ;
	storage >> fMonochrome ;

	storage.SeekRelative( 3 ) ;   // Gets us to graphic size
	storage >> uGraphicDataSize ;

	uLONG uThumbnailSize ;			// Skip past the thumbnail
	storage >> uThumbnailSize ;
	storage.SeekRelative( uThumbnailSize ) ;

	pGraphicData = storage.GetStorageStream()->GetBuffer( uGraphicDataSize ) ;
	pGraphic->Load( pGraphicData, uGraphicDataSize, fMonochrome ) ;

	m_pGraphicDocument->SetGraphic( dynamic_cast<RGraphic*>( pGraphic ) );
}


// ****************************************************************************
//
//  Function Name:	RGraphicInterfaceImp::Load( )
//
//  Description:		Read in the data into the associated Graphic from
//							a raw data stream.
//
//  Returns:			Nothing
//
//  Exceptions:		kMemory, Disk Errors
//
// ****************************************************************************
//
void RGraphicInterfaceImp::Load( RStorage& storage, uLONG uLength, EGraphicType graphicType )
{
	RPsd3Graphic* pGraphic = RPsd3Graphic::CreateNewGraphic( graphicType );
	
	uBYTE*  pGraphicData = storage.GetStorageStream()->GetBuffer( uLength );
	pGraphic->Load( pGraphicData, uLength, FALSE );

	m_pGraphicDocument->SetGraphic( dynamic_cast<RGraphic*>( pGraphic ) );
}

// ****************************************************************************
//
//  Function Name:	RGraphicInterfaceImp::Load( )
//
//  Description:		Read in the data into the associated Graphic from
//							a raw data stream.
//
//  Returns:			Nothing
//
//  Exceptions:		kMemory, Disk Errors
//
// ****************************************************************************
//
void RGraphicInterfaceImp::Load( IBufferInterface* pIBufferedData, EGraphicType graphicType, BOOLEAN fLinkData )
{
	RMemoryMappedBuffer buffer;
	pIBufferedData->Read( buffer );

	if (kCGM == graphicType)
	{
		RPsd3Graphic* pGraphic = RPsd3Graphic::CreateNewGraphic( graphicType );
		pGraphic->Load( buffer.GetBuffer(), buffer.GetBufferSize(), FALSE );
		m_pGraphicDocument->SetGraphic( dynamic_cast<RGraphic*>( pGraphic ) );
	}
	else
	{
		RBufferStream stream( buffer.GetBuffer(), buffer.GetBufferSize() );
		RStorage storage( &stream );
		LoadFromPretzelCollection( storage, graphicType );
	}

	m_pGraphicDocument->SetLinkedData( fLinkData );
}

// ****************************************************************************
//
//  Function Name:	RGraphicInterfaceImp::LoadFromPretzelCollection( )
//
//  Description:		Read in the data into the associated Graphic.from
//							Print Shop library-of-one format
//
//  Returns:			Nothing
//
//  Exceptions:		kMemory, Disk Errors
//
// ****************************************************************************
//
void RGraphicInterfaceImp::LoadFromPretzelCollection( RStorage& storage, EGraphicType graphicType )
{
// save current compiler struct packing setting, set to 1 (each member on byte boundary)
#pragma pack( push, 1 )
	struct DS_LIBHEAD
	{
		char		FileIdentifier[16];	// file verification---NOT NULL TERMINATED!
		char		LibraryName[32];		// Name of library---NOT NULL TERMINATED!
		short		FileID;					// unique file identifier
		short		Machine;					// machine signature
		char		Type;						// type of file---this is often inaccurate so do not use
		char		Subtype;					// type of graphics in file (we no longer use)
		short		Count;					// number of graphics in file
		short		LayoutClassSize;		// size of class info array (layout only)
		short		LayoutCount;			// number of layouts in file (layout only)
		char		DriverName[8];			// driver on which bitmaps were made (DOS platform only)
		char		CreatedByBB;			// library was created by Broderbund
		char		unused;
		long		magicNumber;			// new for version 3.0---confirms library is 3.x
		char		versionLo;				// new for version 3.0
		char		versionHi;				// new for version 3.0
		long		keywordOffset;			// new for version 3.0---reads in keywords until end of file
		char		reserved[48];			// remainder of 128 bytes
	};
	struct DS_GRAPHICINFO
	{
		char	Name[32];			// name of graphic---NOT NULL TERMINATED!
		long	GraphiclD;			// unique graphic identifier
		long	GraphicSize;		// graphic size
		long	BitmapSize;			// bitmap size
		char	GraphicCompress;	// compression scheme for graphics
		char	BitmapCompress;	// compression scheme for bitmaps
		char	MonoFlag;			// True if monochrome graphic
		char	Screening;			// default screening value, 0-100
		char	BackdropType;		// backdrop subtypes
		char	reserved;
		short	ProjectCode;		// project use code
	};
// restore compiler struct packing setting
#pragma pack( pop )

	uLONG uGraphicSize;
	const int kGraphicSizeOffset = sizeof(DS_LIBHEAD) + 36;
	storage.SeekRelative( kGraphicSizeOffset );
	storage >> uGraphicSize;

	BOOLEAN fMonochrome;
	const int kMonochromeOffset = 6;
	storage.SeekRelative( kMonochromeOffset );
	storage >> fMonochrome;

	const int kSkipRemainder = 5;
	storage.SeekRelative( kSkipRemainder );

	uBYTE* pGraphicData = storage.GetStorageStream()->GetBuffer( uGraphicSize );

	RPsd3Graphic* pGraphic = RPsd3Graphic::CreateNewGraphic( graphicType );
	pGraphic->Load( pGraphicData, uGraphicSize, fMonochrome );

	m_pGraphicDocument->SetGraphic( dynamic_cast<RGraphic*>( pGraphic ) );
}

// ****************************************************************************
//
//  Function Name:	RGraphicInterfaceImp::Read( )
//
//  Description:		Read in the data into the associated Graphic.
//
//  Returns:			Nothing
//
//  Exceptions:		kMemory, Disk Errors
//
// ****************************************************************************
//
void RGraphicInterfaceImp::Read( RChunkStorage& storage )
{
	RColor	color;

	if (!m_pGraphic) m_pGraphic = new RPsd3SingleGraphic;
	m_pGraphic->Read( storage );
	storage >> color;
	m_pGraphic->SetMonochromeColor(color);
	m_pGraphicDocument->SetGraphic( m_pGraphic );
}

// ****************************************************************************
//
//  Function Name:	RGraphicInterfaceImp::Write( )
//
//  Description:		Write the data to the given storage
//
//  Returns:			Nothing
//
//  Exceptions:		kMemory, Disk errors
//
// ****************************************************************************
//
void RGraphicInterfaceImp::Write( RChunkStorage& storage )
{
	if (m_pGraphic) m_pGraphic->Write( storage );
	storage << m_pGraphic->GetMonochromeColor();
}

// ****************************************************************************
//
//  Function Name:	RGraphicInterfaceImp::Render( )
//
//  Description:		Render the data in the view.
//
//  Returns:			Nothing
//
//  Exceptions:		kMemory
//
// ****************************************************************************
//
void RGraphicInterfaceImp::Render( RDrawingSurface& drawingSurface, const R2dTransform& transform,
										  const RIntRect& rcRender, const RIntRect& rcLocation )
{
	R2dTransform	graphicTransform	= transform;
	graphicTransform.PreTranslate( rcLocation.m_Left, rcLocation.m_Top );
	TpsAssert( m_pGraphic != NULL, "Render called with NULL graphic" );
	m_pGraphic->Render( drawingSurface, rcLocation.WidthHeight(), graphicTransform, rcRender, m_pGraphic->GetMonochromeColor(), FALSE );
}


// ****************************************************************************
//
//  Function Name:	RGraphicColorInterfaceImp::RGraphicColorInterfaceImp( )
//
//  Description:		Constructor.
//
//  Returns:			Nothing
//
//  Exceptions:		kMemory
//
// ****************************************************************************
//
RGraphicColorInterfaceImp::RGraphicColorInterfaceImp( const RGraphicView* pView )
	: RGraphicColorInterface( pView ),
	  m_pView( const_cast< RGraphicView* >( pView ) )
	{
	}

// ****************************************************************************
//
//  Function Name:	RGraphicColorInterfaceImp::CreateInterface( )
//
//  Description:		Constructor function.
//
//  Returns:			new RInterface
//
//  Exceptions:		kMemory
//
// ****************************************************************************
//
RInterface* RGraphicColorInterfaceImp::CreateInterface( const RComponentView* pView )
{
	TpsAssertIsObject( RGraphicView, pView );
	return new RGraphicColorInterfaceImp( static_cast<const RGraphicView*>( pView ) );
}

// ****************************************************************************
//
//  Function Name:	RGraphicColorInterfaceImp::IsMonochrome( )
//
//  Description:		Return TRUE if the graphic is monochrome
//
//  Returns:			TRUE if monochrome, FALSE otherwise
//
//  Exceptions:		None
//
// ****************************************************************************
//
BOOLEAN RGraphicColorInterfaceImp::IsMonochrome( ) const
{
	RGraphic*	pGraphic	= m_pView->GetGraphicDocument( )->GetGraphic( );
	if ( pGraphic )
		return pGraphic->IsMonochrome( );
	return FALSE;
}

// ****************************************************************************
//
//  Function Name:	RGraphicColorInterfaceImp::GetColor( )
//
//  Description:		Return the color of the graphic stored in the view
//
//  Returns:			The color
//
//  Exceptions:		kMemory, kUnknownException
//
// ****************************************************************************
//
RColor RGraphicColorInterfaceImp::GetColor( ) const
{
	RGraphic*	pGraphic	= m_pView->GetGraphicDocument( )->GetGraphic( );
	if ( pGraphic )
		return pGraphic->GetMonochromeColor( );
	return RColor( kBlack );
}

// ****************************************************************************
//
//  Function Name:	RGraphicColorInterfaceImp::GetColor( )
//
//  Description:		Return the color of the graphic stored in the view
//
//  Returns:			new RGraphicHolder (containing the data)
//
//  Exceptions:		kMemory, kUnknownException
//
// ****************************************************************************
//
void RGraphicColorInterfaceImp::SetColor( const RColor& color )
{
	RGraphicDocument*	pDocument	= m_pView->GetGraphicDocument( );
	RGraphic*			pGraphic		= pDocument->GetGraphic( );
	if ( pGraphic )
	{
		RChangeColorAction*	pAction		= new RChangeColorAction( pDocument, pGraphic, color );
		pDocument->SendAction( pAction );
	}
}


// ****************************************************************************
//
//  Function Name:	RGraphicDateTimeInterfaceImp::RGraphicDateTimeInterfaceImp( )
//
//  Description:		Constructor
//
//  Returns:			
//
//  Exceptions:		
//
// ****************************************************************************
RGraphicDateTimeInterfaceImp::RGraphicDateTimeInterfaceImp( const RGraphicView* pView )
:	RDateTimeInterface( pView ),
	m_pView( pView )
{
	m_pTimepiece = dynamic_cast<RPsd3TimepieceGraphic*>(static_cast<RGraphicDocument*>(m_pView->GetComponentDocument())->GetGraphic());
}

// ****************************************************************************
//
//  Function Name:	RGraphicDateTimeInterfaceImp::CreateInterface( )
//
//  Description:		
//
//  Returns:			
//
//  Exceptions:		
//
// ****************************************************************************
RInterface* RGraphicDateTimeInterfaceImp::CreateInterface( const RComponentView* pView )
{
	TpsAssertIsObject( RGraphicView, pView );
	return new RGraphicDateTimeInterfaceImp( static_cast<const RGraphicView*>( pView ) );
}
// ****************************************************************************
//
//  Function Name:	RGraphicDateTimeInterfaceImp::GetDateTime( )
//
//  Description:		
//
//  Returns:			
//
//  Exceptions:		
//
// ****************************************************************************
const RDateTime& RGraphicDateTimeInterfaceImp::GetDateTime( ) const
{
	if (m_pTimepiece)
		const_cast<RDateTime&>(m_rDateTime).SetTime(m_pTimepiece->GetTime());		
	else
		TpsAssertAlways("Graphic component does not contain a timepiece.  An RDateTime interface should not have been returned.");

	return m_rDateTime;	
}

// ****************************************************************************
//
//  Function Name:	RGraphicDateTimeInterfaceImp::SetDateTime( )
//
//  Description:		
//
//  Returns:			
//
//  Exceptions:		
//
// ****************************************************************************
void RGraphicDateTimeInterfaceImp::SetDateTime( const RDateTime& rDateTime, BOOLEAN )
{
	if (m_pTimepiece)
		m_pTimepiece->SetTime(rDateTime.GetTime());
	else
		TpsAssertAlways("Graphic component does not contain a timepiece.  An RDateTime interface should not have been returned.");
}

// ****************************************************************************
//
//  Function Name:	RGraphicDateTimeInterfaceImp::SetDateTime()
//
//  Description:		Set the date stored in the interface - Send the action if requested
//
//  Returns:			Nothing
//
//  Exceptions:		kMemory
//
// ****************************************************************************
YResourceId	RGraphicDateTimeInterfaceImp::GetDateFormatId( ) const
{
	return 0;
}


//////////////////////////////////////////////////////////////////////////////////////
// RGraphicPropertiesImp
//////////////////////////////////////////////////////////////////////////////////////

// ****************************************************************************
//
//  Function Name:	RGraphicPropertiesImp::RGraphicPropertiesImp( )
//
//  Description:		Constructor.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
RGraphicPropertiesImp::RGraphicPropertiesImp( const RComponentView* pView )
	: IObjectProperties( pView )
	, m_pDocument( (RGraphicDocument *) pView->GetComponentDocument() )
{
}

// ****************************************************************************
//
//  Function Name:	RGraphicPropertiesImp::CreateInterface( )
//
//  Description:		Constructor function.
//
//  Returns:			new RInterface
//
//  Exceptions:		kMemory
//
// ****************************************************************************
//
RInterface* RGraphicPropertiesImp::CreateInterface( const RComponentView* pView )
{
	TpsAssertIsObject( RGraphicView, pView );
	return new RGraphicPropertiesImp( static_cast<const RComponentView*>( pView ) );
}

BOOLEAN RGraphicPropertiesImp::FindProperty( YInterfaceId iid, RInterface** ppInterface )
{
	switch (iid)
	{
	case kFillColorAttribInterfaceId:
		*ppInterface = new RSolidColorImp( m_pDocument->m_rTintColor, m_pDocument );
		break;

	default:
		*ppInterface = NULL;
	}

	return (*ppInterface != NULL);
}
