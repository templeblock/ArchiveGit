// ****************************************************************************
//
//  File Name:			HeadlinePersistantObject.cpp
//
//  Project:			Renaissance Headline component
//
//  Author:				M. Houle
//
//  Description:		Definition of the DataObject class
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
//  $Logfile:: /PM8/HeadlineComp/Source/HeadlinePersistantObject.cpp          $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:18p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#include	"HeadlineIncludes.h"

ASSERTNAME

#include	"HeadlinePersistantObject.h"
#include "HeadlineGraphic.h"
#include "HeadlineDocument.h"
#include "HeadlinesCanned.h"
#include "Storage.h"
#include "ApplicationGlobals.h"
#include "ResourceManager.h"
#include "HeadlineCompResource.h"
#include "HeadlineApplication.h"
#include "View.h"
#include "ComponentTypes.h"
#include "Configuration.h"

const	YFontSize		kDefaultFontHeight	= ::PrinterPointToLogicalUnit( 72 );
const char*				kDefaultHeadlineFont	= "NewZurica";

// ****************************************************************************
//
//  Function Name:	RHeadlinePersistantObject::RHeadlinePersistantObject( )
//
//  Description:		Constructor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
RHeadlinePersistantObject::RHeadlinePersistantObject( ) 
	: m_pDocument( NULL ),
	  m_uwWarpPathShape( kNoWarpShape ),
	  m_pWarpPathPath( NULL ),
	  m_pHeadlineGraphic( NULL ),
	  m_FontId( kInvalidFontId ),
	  m_fReconstructed( FALSE ),
	  m_fStateValid( TRUE )
{
	m_pHeadlineGraphic	= new RHeadlineGraphic;
	m_pWarpPathPath		= new RPath;

	//	Now Set the initial string data... This will be overwritten if the document is being loaded
	m_HeadlineText		=	GetResourceManager( ).GetResourceString( STRING_HEADLINE_COMPONENT_CREATION );
	//	Don't call GetGraphic since that will try to reconstruct...
	m_pHeadlineGraphic->SetText( m_HeadlineText );

	m_InterfaceData.ulData = MAKELONG( -1, 0 ) ;
}


// ****************************************************************************
//
//  Function Name:	RHeadlinePersistantObject::~RHeadlinePersistantObject( )
//
//  Description:		Destructor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
RHeadlinePersistantObject::~RHeadlinePersistantObject( ) 
{
	delete m_pWarpPathPath;
	delete m_pHeadlineGraphic;
}

// ****************************************************************************
//
//  Function Name:	RHeadlinePersistantObject::PurgeData( )
//
//  Description:		Purge as much data as possible
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RHeadlinePersistantObject::PurgeData( )
{
	if (m_FontId != kInvalidFontId)
		m_pDocument->GetFontList().RemoveFont( m_FontId );
	m_FontId = kInvalidFontId;
	//
	//	Tell the graphic that it will need to reconstruct itself...
	QueueReconstruct();
}

// ****************************************************************************
//
//  Function Name:	RHeadlinePersistantObject::Read( )
//
//  Description:		Read in the data
//
//  Returns:			Nothing
//
//  Exceptions:		File Exceptions
//
// ****************************************************************************
//
void RHeadlinePersistantObject::Read( RStorage& storage )
{
	uLONG								ulValue;
	HeadlineDataStruct			headlineData;

	ReadAttribData( headlineData.structAttribData, storage );
	ReadShapeData( headlineData.structShapeData, storage );
	ReadOutlineData( headlineData.structOutlineData, storage );
	ReadEffectsData( headlineData.structEffectsData, storage );
	ReadBehindData( headlineData.structBehindEffectsData, storage );
	ReadInterfaceData( headlineData.structInterfaceData, storage );

	//	Read in the string
	storage >> headlineData.sHeadlineText;
		storage	>> ulValue;		TpsAssert( ulValue == 0, "Padding field not zero" );
		storage	>> ulValue;		TpsAssert( ulValue == 0, "Padding field not zero" );

   // validatations
 	if ( headlineData.structAttribData.eSelJust != kCenterJust )
		if ( headlineData.structShapeData.eDistortEffect == kFollowPath ||
			  headlineData.structAttribData.eEscapement == kTop2BottomLeft2Right ||
			  headlineData.structAttribData.eEscapement == kTop2BottomRight2Left ||
			  headlineData.structAttribData.eEscapement == kBottom2TopLeft2Right ||
			  headlineData.structAttribData.eEscapement == kBottom2TopRight2Left )
		   headlineData.structAttribData.eSelJust = kCenterJust;
	
	//	Set the data into the headline
	SetHeadlineData( &headlineData );
}

// ****************************************************************************
//
//  Function Name:	RHeadlinePersistantObject::ReadAttribData( )
//
//  Description:		Read in the Attribute data
//
//  Returns:			Nothing
//
//  Exceptions:		File Exceptions
//
// ****************************************************************************
//
void RHeadlinePersistantObject::ReadAttribData( AttribDataStruct& attribData, RArchive& archive )
{
	uLONG								ulValue;
	uWORD								uwValue;

	//	Read in the Headline Attribute Tab Data
	archive	>>	attribData.sFontName;
	archive	>> uwValue;		attribData.eEscapement			= ETextEscapement( uwValue );
	archive	>> uwValue;		attribData.fBoldChecked			= BOOLEAN( uwValue );
	archive	>>	uwValue;		attribData.fItalicChecked		= BOOLEAN( uwValue );
	archive	>> uwValue;		attribData.fUnderlineChecked	= BOOLEAN( uwValue );
	archive	>> uwValue;		attribData.eSelJust				= ETextJustification( uwValue );
	archive	>> uwValue;		attribData.eSelComp				= ETextCompensation( uwValue );
	archive	>> attribData.flCompDegree;
	archive	>> uwValue;		attribData.fKerningChecked		= BOOLEAN( uwValue );
	archive	>> uwValue;		attribData.fLeadingChecked		= BOOLEAN( uwValue );
	archive	>> attribData.uwScale1;
	archive	>> attribData.uwScale2;
	archive	>> attribData.uwScale3;
	archive	>> uwValue;		attribData.fNoBackgroundEnvelop	= BOOLEAN( uwValue );
		archive	>> uwValue;		TpsAssert( uwValue == 0, "Padding field not zero" );
		archive	>> ulValue;		TpsAssert( ulValue == 0, "Padding field not zero" );
		archive	>> ulValue;		TpsAssert( ulValue == 0, "Padding field not zero" );

#ifdef WIN
				if ( !RFont::IsFontOnSystem( attribData.sFontName ) )
					{
					RFontSubstitution	fontSub( attribData.sFontName, "" );
					RFontList::GetFontSubstitutionList().InsertAtEnd( fontSub );
					}
#endif	//	WIN
}

// ****************************************************************************
//
//  Function Name:	RHeadlinePersistantObject::ReadShapeData( )
//
//  Description:		Read in the Shape data
//
//  Returns:			Nothing
//
//  Exceptions:		File Exceptions
//
// ****************************************************************************
//
void RHeadlinePersistantObject::ReadShapeData( ShapeDataStruct& shapeData, RArchive& archive )
{
	uLONG								ulValue;
	uWORD								uwValue;
	sWORD								swValue;
	double							dValue;

	//	Now read in the Shape Tab Data
	archive	>> uwValue;		shapeData.eDistortEffect		= EDistortEffects( uwValue );
	archive	>> uwValue;		shapeData.nSelShapeIdx			= int( uwValue );
	archive	>> swValue;		shapeData.nShearAngle			= int( swValue );
	archive	>> swValue;		shapeData.nRotation				= int( swValue );
	archive	>> dValue;		shapeData.flAltPercent			= float( dValue );
	archive	>> uwValue;		shapeData.fDistort				= BOOLEAN( uwValue );
	archive	>> uwValue;		shapeData.eVerticalPlacement	= ( ( uwValue == 0) ? kPin2Middle : EVerticalPlacement( uwValue ) );
									shapeData.flMaxPathScale		= 0.9;
		archive	>> uwValue;		TpsAssert( uwValue == 0, "Padding field not zero" );
		archive	>> ulValue;		TpsAssert( ulValue == 0, "Padding field not zero" );
}

// ****************************************************************************
//
//  Function Name:	RHeadlinePersistantObject::ReadOutlineData( )
//
//  Description:		Read in the Outline data
//
//  Returns:			Nothing
//
//  Exceptions:		File Exceptions
//
// ****************************************************************************
//
void RHeadlinePersistantObject::ReadOutlineData( OutlineDataStruct& outlineData, RArchive& archive )
{
	uLONG								ulValue;
	uWORD								uwValue;
	//	Now read in the Outline Tab Data
	archive	>> outlineData.outlineFillDesc;
	archive	>> uwValue;		outlineData.eOutlineEffect		= EOutlineEffects( uwValue );
	archive	>> uwValue;		outlineData.eLineWeight			= ELineWeight( uwValue );
	archive	>> outlineData.colorStroke;
	archive	>> outlineData.colorShadow;
	archive	>> outlineData.colorHilite;
	archive	>> outlineData.colorBlur2;
		archive	>> ulValue;		TpsAssert( ulValue == 0, "Padding field not zero" );
		archive	>> ulValue;		TpsAssert( ulValue == 0, "Padding field not zero" );
}

// ****************************************************************************
//
//  Function Name:	RHeadlinePersistantObject::ReadEffectsData( )
//
//  Description:		Read in the Effects data
//
//  Returns:			Nothing
//
//  Exceptions:		File Exceptions
//
// ****************************************************************************
//
void RHeadlinePersistantObject::ReadEffectsData( EffectsDataStruct& effectsData, RArchive& archive )
{
	uLONG								ulValue;
	uWORD								uwValue;
	sWORD								swValue;
	double							dValue;
	//	Now read in the Effect Tab Data
	archive	>> effectsData.projectionFillDesc;
	archive	>> uwValue;		effectsData.eProjectionEffect		= EProjectionEffects( uwValue );
	archive	>> uwValue;		effectsData.nNumStages				= int( uwValue );
	archive	>> swValue;		effectsData.nProjectionVanishPtX	= int( swValue );
	archive	>> swValue;		effectsData.nProjectionVanishPtY	= int( swValue );
	archive	>> dValue;		effectsData.flProjectionDepth		= float( dValue );
		archive	>> ulValue;		TpsAssert( ulValue == 0, "Padding field not zero" );
		archive	>> ulValue;		TpsAssert( ulValue == 0, "Padding field not zero" );
	effectsData.fProjectionBlend	= effectsData.projectionFillDesc.UsesGradient();

	archive	>> effectsData.shadowFillDesc;
	archive	>> uwValue;		effectsData.eShadowEffect			= EShadowEffects( uwValue );
	archive	>> swValue;		effectsData.nShadowVanishPtX		= int( swValue );
	archive	>> swValue;		effectsData.nShadowVanishPtY		= int( swValue );
	archive	>> dValue;		effectsData.flShadowDepth			= float( dValue );
	archive	>> effectsData.colorShadow2;
		archive	>> ulValue;		TpsAssert( ulValue == 0, "Padding field not zero" );
		archive	>> ulValue;		TpsAssert( ulValue == 0, "Padding field not zero" );
	effectsData.fShadowBlend		= effectsData.shadowFillDesc.UsesGradient();
}

// ****************************************************************************
//
//  Function Name:	RHeadlinePersistantObject::ReadBehindData( )
//
//  Description:		Read in the Behind (face) data
//
//  Returns:			Nothing
//
//  Exceptions:		File Exceptions
//
// ****************************************************************************
//
void RHeadlinePersistantObject::ReadBehindData( BehindEffectsDataStruct& behindData, RArchive& archive )
{
	uLONG								ulValue;
	uWORD								uwValue;

	// Now read in the Behind Effect Tab Data
	archive	>> behindData.behindFillDesc;
	archive	>> uwValue;		behindData.eBehindEffect			= EFramedEffects( uwValue );
	archive	>> behindData.colorShadow;
	archive	>> behindData.colorHilite;
		archive	>> ulValue;		TpsAssert( ulValue == 0, "Padding field not zero" );
		archive	>> ulValue;		TpsAssert( ulValue == 0, "Padding field not zero" );
}

// ****************************************************************************
//
//  Function Name:	RHeadlinePersistantObject::ReadInterfaceData( )
//
//  Description:		Read in the inteface data
//
//  Returns:			Nothing
//
//  Exceptions:		File Exceptions
//
// ****************************************************************************
//
void RHeadlinePersistantObject::ReadInterfaceData( InterfaceDataStruct& interfaceData, RArchive& archive )
{
	archive	>> interfaceData.ulData ;		
}

// ****************************************************************************
//
//  Function Name:	RHeadlinePersistantObject::Write( )
//
//  Description:		Write out the data
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RHeadlinePersistantObject::Write( RStorage& storage ) const
{
	uLONG								ulValue;
	HeadlineDataStruct			headlineData;

	//	Get the data into the headline
	RHeadlinePersistantObject*	pThis	= const_cast<RHeadlinePersistantObject*>( this );
	pThis->GetHeadlineData( &headlineData );

	WriteAttribData( headlineData.structAttribData, storage );
	WriteShapeData( headlineData.structShapeData, storage );
	WriteOutlineData( headlineData.structOutlineData, storage );
	WriteEffectsData( headlineData.structEffectsData, storage );
	WriteBehindData( headlineData.structBehindEffectsData, storage );
	WriteInterfaceData( headlineData.structInterfaceData, storage ) ;

	//	Write out the string
	storage << headlineData.sHeadlineText;
		//	Write out padding
		ulValue	= 0;
		storage	<< ulValue;
		storage	<< ulValue;
}

// ****************************************************************************
//
//  Function Name:	RHeadlinePersistantObject::WriteAttribData( )
//
//  Description:		Write out the data
//
//  Returns:			Nothing
//
//  Exceptions:		File Exceptions 
//
// ****************************************************************************
//
void RHeadlinePersistantObject::WriteAttribData( const AttribDataStruct& attribData, RArchive& archive )
{
	uLONG								ulValue;
	uWORD								uwValue;

	//	Write out the Headline Attribute Tab Data
	archive	<<	attribData.sFontName;
	uwValue	= uWORD( attribData.eEscapement );				archive << uwValue;
	uwValue	= uWORD( attribData.fBoldChecked );				archive << uwValue;
	uwValue	= uWORD( attribData.fItalicChecked );			archive << uwValue;
	uwValue	= uWORD( attribData.fUnderlineChecked );		archive << uwValue;
	uwValue	= uWORD( attribData.eSelJust );					archive << uwValue;
	uwValue	= uWORD( attribData.eSelComp );					archive << uwValue;
	archive	<< attribData.flCompDegree;
	uwValue	= uWORD( attribData.fKerningChecked );			archive << uwValue;
	uwValue	= uWORD( attribData.fLeadingChecked );			archive << uwValue;
	archive	<< attribData.uwScale1;
	archive	<< attribData.uwScale2;
	archive	<< attribData.uwScale3;
	uwValue	= uWORD( attribData.fNoBackgroundEnvelop );	archive << uwValue;
		//	Write out padding
		uwValue	= 0;
		ulValue	= 0;
		archive	<< uwValue;
		archive	<< ulValue;
		archive	<< ulValue;
}

// ****************************************************************************
//
//  Function Name:	RHeadlinePersistantObject::WriteShapeData( )
//
//  Description:		Write out the data
//
//  Returns:			Nothing
//
//  Exceptions:		File Exceptions 
//
// ****************************************************************************
//
void RHeadlinePersistantObject::WriteShapeData( const ShapeDataStruct& shapeData, RArchive& archive )
{
	uLONG								ulValue;
	uWORD								uwValue;
	sWORD								swValue;
	double							dValue;

	//	Now Write out the Shape Tab Data
	uwValue	= uWORD( shapeData.eDistortEffect );			archive << uwValue;
	uwValue	= uWORD( shapeData.nSelShapeIdx );				archive << uwValue;
	swValue	= sWORD( shapeData.nShearAngle );				archive << swValue;
	swValue	= sWORD( shapeData.nRotation );					archive << swValue;
	dValue	= double( shapeData.flAltPercent );				archive << dValue;
	uwValue	= uWORD( shapeData.fDistort );					archive << uwValue;
	uwValue	= uWORD( shapeData.eVerticalPlacement );	   archive << uwValue;
	dValue	= double( shapeData.flMaxPathScale );	 
		//	Write out padding
		uwValue	= 0;
		ulValue	= 0;
		archive	<< uwValue;
		archive	<< ulValue;
	}

// ****************************************************************************
//
//  Function Name:	RHeadlinePersistantObject::WriteOutlineData( )
//
//  Description:		Write out the data
//
//  Returns:			Nothing
//
//  Exceptions:		File Exceptions 
//
// ****************************************************************************
//
void RHeadlinePersistantObject::WriteOutlineData( const OutlineDataStruct& outlineData, RArchive& archive )
{
	uLONG								ulValue;
	uWORD								uwValue;

	//	Now Write out the Outline Tab Data
	archive	<< outlineData.outlineFillDesc;
	uwValue	= uWORD( outlineData.eOutlineEffect );			archive << uwValue;
	uwValue	= uWORD( outlineData.eLineWeight );				archive << uwValue;
	archive	<< outlineData.colorStroke;
	archive	<< outlineData.colorShadow;
	archive	<< outlineData.colorHilite;
	archive	<< outlineData.colorBlur2;
		//	Write out padding
		ulValue	= 0;
		archive	<< ulValue;
		archive	<< ulValue;
}

// ****************************************************************************
//
//  Function Name:	RHeadlinePersistantObject::WriteEffectsData( )
//
//  Description:		Write out the data
//
//  Returns:			Nothing
//
//  Exceptions:		File Exceptions 
//
// ****************************************************************************
//
void RHeadlinePersistantObject::WriteEffectsData( const EffectsDataStruct& effectsData, RArchive& archive )
{
	uLONG								ulValue;
	uWORD								uwValue;
	sWORD								swValue;
	double							dValue;

	//	Now Write out the Effect Tab Data
	archive	<< effectsData.projectionFillDesc;
	uwValue	= uWORD( effectsData.eProjectionEffect );			archive << uwValue;
	uwValue	= uWORD( effectsData.nNumStages );					archive << uwValue;
	swValue	= sWORD( effectsData.nProjectionVanishPtX );		archive << swValue;
	swValue	= sWORD( effectsData.nProjectionVanishPtY );		archive << swValue;
	dValue	= double( effectsData.flProjectionDepth );		archive << dValue;
		//	Write out padding
		ulValue	= 0;
		archive	<< ulValue;
		archive	<< ulValue;

	archive	<< effectsData.shadowFillDesc;
	uwValue	= uWORD( effectsData.eShadowEffect );				archive << uwValue;
	swValue	= sWORD( effectsData.nShadowVanishPtX );			archive << swValue;
	swValue	= sWORD( effectsData.nShadowVanishPtY );			archive << swValue;
	dValue	= double( effectsData.flShadowDepth );				archive << dValue;
	archive	<< effectsData.colorShadow2;
		//	Write out padding
		ulValue	= 0;
		archive	<< ulValue;
		archive	<< ulValue;
}

// ****************************************************************************
//
//  Function Name:	RHeadlinePersistantObject::WriteBehindData( )
//
//  Description:		Write out the data
//
//  Returns:			Nothing
//
//  Exceptions:		File Exceptions 
//
// ****************************************************************************
//
void RHeadlinePersistantObject::WriteBehindData( const BehindEffectsDataStruct& behindData, RArchive& archive )
{
	uLONG								ulValue;
	uWORD								uwValue;

	// Now Write out the Behind Effect Tab Data
	archive	<< behindData.behindFillDesc;
	uwValue	= uWORD( behindData.eBehindEffect );				archive << uwValue;
	archive	<< behindData.colorShadow;
	archive	<< behindData.colorHilite;
		//	Write out padding
		ulValue	= 0;
		archive	<< ulValue;
		archive	<< ulValue;
}

// ****************************************************************************
//
//  Function Name:	RHeadlinePersistantObject::WriteInterfaceData( )
//
//  Description:		Write out the inteface data
//
//  Returns:			Nothing
//
//  Exceptions:		File Exceptions
//
// ****************************************************************************
//
void RHeadlinePersistantObject::WriteInterfaceData( const InterfaceDataStruct& interfaceData, RArchive& archive )
{
	archive << interfaceData.ulData ;		
}

// ****************************************************************************
//
//  Function Name:	RHeadlinePersistantObject::SetMainColumn( )
//
//  Description:		Set the MainColumn pointer
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RHeadlinePersistantObject::SetDocument( RHeadlineDocument* pDocument, BOOLEAN fLoading )
{
	m_pDocument = pDocument;

	if( pDocument->GetComponentType( ) == kSpecialHeadlineComponent )
		m_pHeadlineGraphic->SetTrimWhiteSpace( TRUE );

	if ( !fLoading )
	{
		HeadlineDataStruct		headlineData;
		GetHeadlineData( &headlineData );
		SetHeadlineData( &headlineData );
	}
}

// ****************************************************************************
//
//  Function Name:	RHeadlinePersistantObject::GetHeadlineData( )
//
//  Description:		Retrieve the headline data from the HeadlineGraphic
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RHeadlinePersistantObject::GetHeadlineData( HeadlineDataStruct* pData )
	{
	GetAttribData( pData->structAttribData );
	GetShapeData( pData->structShapeData );
	GetOutlineData( pData->structOutlineData );
	GetEffectData( pData->structEffectsData );
	GetBehindData( pData->structBehindEffectsData );
	GetInterfaceData( pData->structInterfaceData ) ;
	GetText( pData->sHeadlineText );
	}

// ****************************************************************************
//
//  Function Name:	RHeadlinePersistantObject::GetAttribData( )
//
//  Description:		Retrieve the headline Attribute Based data
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RHeadlinePersistantObject::GetAttribData( AttribDataStruct& attribData )
	{
	RHeadlineGraphic*				pGraphic		= m_pHeadlineGraphic;

	//	First the attribute structure
	if (m_FontId != kInvalidFontId )
		{
		YFontInfo	fontInfo				= m_pDocument->GetFontList().GetFontInfo( m_FontId );
		attribData.sFontName				= fontInfo.sbName;
		attribData.fBoldChecked			= static_cast<BOOLEAN>(fontInfo.attributes & RFont::kBold);
		attribData.fItalicChecked		= static_cast<BOOLEAN>(fontInfo.attributes & RFont::kItalic);	
		attribData.fUnderlineChecked	= static_cast<BOOLEAN>(fontInfo.attributes & RFont::kUnderline);
		}
	else
		{
		// Try to get the default font
		RMBCString fontName;
		RRenaissanceUserPreferences rConfig;		
		rConfig.GetStringValue(RRenaissanceUserPreferences::kDefaultFont, fontName);
			//	If nothing retrieved, get the value from the resource file
		if ( fontName.IsEmpty() )
			fontName = GetResourceManager( ).GetResourceString( STRING_HEADLINE_COMPONENT_DEFAULT_FONT );
		if ( !fontName.IsEmpty ( ) )
			attribData.sFontName				= fontName;
		else
			attribData.sFontName				= kDefaultHeadlineFont;
		attribData.fBoldChecked			= FALSE;
		attribData.fItalicChecked		= FALSE;	
		attribData.fUnderlineChecked	= FALSE;
		}

	attribData.eEscapement				= pGraphic->GetTextEscapement( );
	attribData.eSelJust					= pGraphic->GetTextJustification( );
	attribData.eSelComp					= pGraphic->GetCharCompensation( );
	attribData.flCompDegree				= pGraphic->GetCompensationAdjustment( );
	attribData.fKerningChecked			= pGraphic->IsCharKerning( );	
	attribData.fLeadingChecked			= pGraphic->IsLineLeading( );	
	pGraphic->GetLineScales( attribData.uwScale1, attribData.uwScale2, attribData.uwScale3 );
	attribData.fNoBackgroundEnvelop	= pGraphic->NotDisplayEnvelop( );	
	}

// ****************************************************************************
//
//  Function Name:	RHeadlinePersistantObject::GetShapeData( )
//
//  Description:		Retrieve the headline Shape specific data
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RHeadlinePersistantObject::GetShapeData( ShapeDataStruct& shapeData )
	{
	RHeadlineGraphic*				pGraphic		= m_pHeadlineGraphic;

	//	Now the shape structure
	shapeData.eDistortEffect			= pGraphic->GetDistortEffect( );
	shapeData.nSelShapeIdx				= m_uwWarpPathShape;
	if ( shapeData.eDistortEffect == kNonDistort )
	{
		//shapeData.eDistortEffect	= kFollowPath;
		shapeData.nSelShapeIdx			= kNoWarpShape;
	}
	shapeData.nShearAngle				= static_cast<int>( pGraphic->GetShearAngle( ) );
	shapeData.nRotation					= static_cast<int>( pGraphic->GetShearRotation( ) );
	shapeData.flAltPercent				= static_cast<float>( pGraphic->GetBaselineAlternate( ) );
	shapeData.fDistort					= static_cast<BOOLEAN>( pGraphic->IsDistortMode( ) );
	shapeData.eVerticalPlacement		= pGraphic->GetVerticalPlacement( );
	shapeData.flMaxPathScale			= static_cast<float>( pGraphic->GetMaxPathHeightScale( ) );
	}

// ****************************************************************************
//
//  Function Name:	RHeadlinePersistantObject::GetOutlineData( )
//
//  Description:		Retrieve the headline Outline Specific data
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RHeadlinePersistantObject::GetOutlineData( OutlineDataStruct& outlineData )
	{
	RHeadlineGraphic*				pGraphic		= m_pHeadlineGraphic;

	// Now the outline structure 
	pGraphic->GetOutlineFill( outlineData.outlineFillDesc );
	outlineData.eOutlineEffect			= pGraphic->GetOutlineEffect();
	outlineData.eLineWeight				= pGraphic->GetLineWeight();
	outlineData.colorStroke				= pGraphic->GetOutlineStrokeColor();
	outlineData.colorShadow				= pGraphic->GetOutlineShadowColor();
	outlineData.colorHilite				= pGraphic->GetOutlineHiliteColor();
	outlineData.colorBlur2				= pGraphic->GetBlur2BackgroundColor();
	}

// ****************************************************************************
//
//  Function Name:	RHeadlinePersistantObject::GetEffectsData( )
//
//  Description:		Retrieve the headline Effects Specific Data
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RHeadlinePersistantObject::GetEffectData( EffectsDataStruct& effectsData )
	{
	RHeadlineGraphic*				pGraphic		= m_pHeadlineGraphic;

	//
	//	Now the Effects
	pGraphic->GetProjectionFill( effectsData.projectionFillDesc );
	effectsData.eProjectionEffect		= pGraphic->GetProjectionEffect( );
	effectsData.nNumStages				= static_cast<int>( pGraphic->GetNumStages( ) );
	effectsData.nProjectionVanishPtX	= pGraphic->GetVanishingPoint( ).m_x;
	effectsData.nProjectionVanishPtY	= pGraphic->GetVanishingPoint( ).m_y;
	effectsData.flProjectionDepth		= static_cast<float>( pGraphic->GetProjectionDepth( ) );
	//	Fill in the helper boolean
	effectsData.fProjectionBlend		= effectsData.projectionFillDesc.UsesGradient();

	pGraphic->GetShadowFill( effectsData.shadowFillDesc );
	effectsData.eShadowEffect			= pGraphic->GetShadowEffect( );
	effectsData.nShadowVanishPtX		= pGraphic->GetShadowingPoint( ).m_x;
	effectsData.nShadowVanishPtY		= pGraphic->GetShadowingPoint( ).m_y;
	effectsData.flShadowDepth			= static_cast<float>( pGraphic->GetShadowDepth( ) );
	effectsData.colorShadow2			= pGraphic->GetShadow2BackgroundColor();
	//	Fill in the helper boolean
	effectsData.fShadowBlend			= effectsData.shadowFillDesc.UsesGradient();
	}

// ****************************************************************************
//
//  Function Name:	RHeadlinePersistantObject::GetBehindData( )
//
//  Description:		Retrieve the headline Behind (face) specific data
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RHeadlinePersistantObject::GetBehindData( BehindEffectsDataStruct& behindData )
	{
	RHeadlineGraphic*				pGraphic		= m_pHeadlineGraphic;

	//
	// Now the Behind Effects
	pGraphic->GetFrameFill( behindData.behindFillDesc );
	behindData.eBehindEffect			= pGraphic->GetFrameEffect( );
		behindData.colorShadow			= pGraphic->GetOutlineShadowColor( );
		behindData.colorHilite			= pGraphic->GetOutlineHiliteColor( );
	}

// ****************************************************************************
//
//  Function Name:	RHeadlinePersistantObject::GetInterfaceData( )
//
//  Description:		Retrieve the headline interface specific data
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RHeadlinePersistantObject::GetInterfaceData( InterfaceDataStruct& interfaceData )
	{
	interfaceData = m_InterfaceData ; 
	}


// ****************************************************************************
//
//  Function Name:	RHeadlinePersistantObject::GetText( )
//
//  Description:		Retrieve the headline Text
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RHeadlinePersistantObject::GetText( RMBCString& data )
	{
	//	Now store the string
	data = m_HeadlineText;
	}

// ****************************************************************************
//
//  Function Name:	RHeadlinePersistantObject::SetHeadlineData( )
//
//  Description:		Stores the headline data into the HeadlineGraphic
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RHeadlinePersistantObject::SetHeadlineData( HeadlineDataStruct* pData )
	{
	SetAttribData( pData->structAttribData, FALSE );
	SetShapeData( pData->structShapeData, FALSE );
	SetOutlineData( pData->structOutlineData, FALSE );
	SetEffectData( pData->structEffectsData, FALSE );
	SetBehindData( pData->structBehindEffectsData, FALSE );
	SetInterfaceData( pData->structInterfaceData, FALSE ) ;
	SetText( pData->sHeadlineText, FALSE );

	QueueReconstruct( );
	}

// ****************************************************************************
//
//  Function Name:	RHeadlinePersistantObject::SetAttribData( )
//
//  Description:		Stores the headline Attribute specific data
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RHeadlinePersistantObject::SetAttribData( const AttribDataStruct& attribData, BOOLEAN fReconstruct )
	{
	RHeadlineGraphic*				pGraphic		= m_pHeadlineGraphic;

	//	Apply the Attribute Data
	pGraphic->ApplyTextEscapement( attribData.eEscapement );
	pGraphic->ApplyTextJustification( attribData.eSelJust );
	pGraphic->ApplyCharCompensation( attribData.eSelComp );
	pGraphic->EnableCharKerning( attribData.fKerningChecked );	
	pGraphic->EnableLineLeading( attribData.fLeadingChecked );	
	pGraphic->AdjustLineScale( attribData.uwScale1, attribData.uwScale2, attribData.uwScale3 );
	pGraphic->NotDisplayEmptyEnvelop( attribData.fNoBackgroundEnvelop );	
	
	//	Reset the font if necessary
	YFont			newFont;

	//	Create new font info	- first init all to 0
	memset( &newFont.info, 0, sizeof(newFont.info) );
	newFont.info.attributes		|= (attribData.fBoldChecked)? RFont::kBold : 0;
	newFont.info.attributes		|= (attribData.fItalicChecked)? RFont::kItalic : 0;
	newFont.info.attributes		|= (attribData.fUnderlineChecked)? RFont::kUnderline : 0;
	newFont.info.height			= kDefaultFontHeight;
	//	Copy the file name,
#ifdef	WIN
		//	What about Multibyte characters
	strncpy( newFont.info.sbName, (LPCTSTR)attribData.sFontName, kMaxFontNameSize );
#else		//	MUST be Mac
	strncpy( newFont.info.sbName, (LPSTR)attribData.sFontName, kMaxFontNameSize );
#endif	//	WIN or MAC
	//	assign black as color...
	newFont.color		= RSolidColor( kBlack );

	RFontList&	fontList	= m_pDocument->GetFontList( );
	BOOLEAN		fNewFont	= FALSE;
	if ( m_FontId == kInvalidFontId )
		fNewFont = TRUE;
	else
		{
		YFontInfo	info	= fontList.GetFontInfo( m_FontId );
		if (info != newFont.info)
			fNewFont = TRUE;
		}

	//	If we need a new font..
	if ( fNewFont )
		{
		if (m_FontId != kInvalidFontId)
			fontList.RemoveFont( m_FontId );
		m_FontId				= kInvalidFontId;	//	incase of failure reset...

		//	Add new font...
		m_FontId	= fontList.AddFont( newFont );
		pGraphic->SetFont( fontList.GetRFont( m_FontId ) );	//	Set font into object
		}

	pGraphic->AdjustCharCompensation( attribData.flCompDegree );

	if ( fReconstruct )
		QueueReconstruct( );
	}

// ****************************************************************************
//
//  Function Name:	RHeadlinePersistantObject::SetShapeData( )
//
//  Description:		Stores the headline Shape Specific data
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RHeadlinePersistantObject::SetShapeData( const ShapeDataStruct& shapeData, BOOLEAN fReconstruct )
	{
	RHeadlineGraphic*				pGraphic		= m_pHeadlineGraphic;

	//	Apply the Shape Data
	BOOLEAN	fWarpSuccess	= FALSE;
	m_uwWarpPathShape	= static_cast<uWORD>( shapeData.nSelShapeIdx );
	switch ( shapeData.eDistortEffect )
		{
		case kNonDistort :
			fWarpSuccess = TRUE;
			pGraphic->ApplyNonDistort( RHeadlineGraphic::GetDefaultHeadlineSize() );
			break;

		case kFollowPath :
			fWarpSuccess = DefineHeadlinePath( m_uwWarpPathShape, pGraphic );
			pGraphic->ApplyVerticalPlacement( shapeData.eVerticalPlacement );
			break;

		case kWarpToEnvelop :
			fWarpSuccess = DefineHeadlineWarp( m_uwWarpPathShape, pGraphic );
			break;

		case kAltBaseline :
			fWarpSuccess = TRUE;
			pGraphic->ApplyAlternateBaselineEffect( shapeData.flAltPercent );
			break;

		case kShear :
			fWarpSuccess = TRUE;
			pGraphic->ApplyShearEffect( shapeData.nShearAngle );
			break;

		case kAltShearRotate :
			pGraphic->ApplyAltShearEffect( shapeData.nShearAngle, shapeData.nRotation );
			fWarpSuccess = TRUE;
			break;

		case kAltShear :
			pGraphic->ApplyAltShearEffect( shapeData.nShearAngle );
			fWarpSuccess = TRUE;
			break;

		default :
			break;
		}

	if ( !fWarpSuccess )
		{
		m_pWarpPathPath->Undefine();
		m_pWarpPathPath->Define( RIntRect( RHeadlineGraphic::GetDefaultHeadlineSize() ) );
		pGraphic->ApplyNonDistort( RHeadlineGraphic::GetDefaultHeadlineSize() );
		}
	else if ( shapeData.eDistortEffect != kFollowPath && shapeData.eDistortEffect != kWarpToEnvelop )
		pGraphic->ApplyDistort( shapeData.fDistort );

	if ( fReconstruct )
		QueueReconstruct( );
	}

// ****************************************************************************
//
//  Function Name:	RHeadlinePersistantObject::SetOutlineData( )
//
//  Description:		Stores the headline Outline Specific data
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RHeadlinePersistantObject::SetOutlineData( const OutlineDataStruct& outlineData, BOOLEAN fReconstruct )
	{
	RHeadlineGraphic*				pGraphic		= m_pHeadlineGraphic;

	//
	//Apply the Outline Data
	pGraphic->ApplyLineWeight( outlineData.eLineWeight );
	pGraphic->ApplyOutlineEffect( outlineData.eOutlineEffect );
	pGraphic->SetOutlineFill( outlineData.eOutlineEffect, outlineData.outlineFillDesc );
	pGraphic->SetOutlineStroke( outlineData.colorStroke );
	pGraphic->SetOutlineShadow( outlineData.colorShadow );
	pGraphic->SetOutlineHilite( outlineData.colorHilite );
	pGraphic->SetBlur2Background( outlineData.colorBlur2 );

	if ( fReconstruct )
		QueueReconstruct( );
	}

// ****************************************************************************
//
//  Function Name:	RHeadlinePersistantObject::SetEffectData( )
//
//  Description:		Stores the headline Effects Specific Data
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RHeadlinePersistantObject::SetEffectData( const EffectsDataStruct& effectsData, BOOLEAN fReconstruct )
	{
	RHeadlineGraphic*				pGraphic		= m_pHeadlineGraphic;

	//
	//	Apply the Effect Data
	RIntPoint	tmpPoint;
	tmpPoint	= RIntPoint( effectsData.nProjectionVanishPtX, effectsData.nProjectionVanishPtY );
	pGraphic->ApplyProjectionEffect(	effectsData.eProjectionEffect,
												effectsData.nNumStages,
												effectsData.flProjectionDepth,
												tmpPoint );
	pGraphic->SetProjectionFill( effectsData.eProjectionEffect, effectsData.projectionFillDesc );

	tmpPoint	= RIntPoint( effectsData.nShadowVanishPtX, effectsData.nShadowVanishPtY );
	pGraphic->ApplyShadowEffect(	effectsData.eShadowEffect,
											effectsData.flShadowDepth,
											tmpPoint );
	pGraphic->SetShadowFill( effectsData.eShadowEffect, effectsData.shadowFillDesc );
	pGraphic->SetShadow2Background( effectsData.colorShadow2 );

	if ( fReconstruct )
		QueueReconstruct( );
	}

// ****************************************************************************
//
//  Function Name:	RHeadlinePersistantObject::SetBehindData( )
//
//  Description:		Stores the headline Behind (face) Specific data
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RHeadlinePersistantObject::SetBehindData( const BehindEffectsDataStruct& behindData, BOOLEAN fReconstruct )
	{
	RHeadlineGraphic*				pGraphic		= m_pHeadlineGraphic;

	//	Apply the Frame (Behind) effects
	pGraphic->ApplyFrameEffect(behindData.eBehindEffect);
	pGraphic->SetFrameFill( behindData.eBehindEffect, behindData.behindFillDesc );
	pGraphic->SetFrameShadow( behindData.eBehindEffect, behindData.colorShadow );
	pGraphic->SetFrameHilite( behindData.eBehindEffect, behindData.colorHilite );

	if ( fReconstruct )
		QueueReconstruct( );
	}

// ****************************************************************************
//
//  Function Name:	RHeadlinePersistantObject::SetInterfaceData( )
//
//  Description:		Stores the headline interface specific data
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RHeadlinePersistantObject::SetInterfaceData( const InterfaceDataStruct& interfaceData, BOOLEAN /*fReconstruct*/ )
	{
	m_InterfaceData = interfaceData ; 
	}

// ****************************************************************************
//
//  Function Name:	RHeadlinePersistantObject::SetText( )
//
//  Description:		Stores the headline text
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RHeadlinePersistantObject::SetText( const RMBCString& data, BOOLEAN fReconstruct )
	{
	RHeadlineGraphic*				pGraphic		= m_pHeadlineGraphic;

	//	Now store the string
	m_HeadlineText		=	data;
	pGraphic->SetText( data );

	if ( fReconstruct )
		QueueReconstruct( );
	}

// ****************************************************************************
//
//  Function Name:	RHeadlinePersistantObject::GetGraphic( )
//
//  Description:		Return the headline graphic
//
//  Returns:			m_HeadlineGraphic
//
//  Exceptions:		None
//
// ****************************************************************************
//
RHeadlineGraphic*	RHeadlinePersistantObject::GetGraphic( BOOLEAN fReconstruct )
	{
	if ( !m_fReconstructed && fReconstruct )
		{
		Reconstruct( );
		m_fReconstructed = TRUE;
		}
	return m_pHeadlineGraphic;
	}

// ****************************************************************************
//
//  Function Name:	RHeadlinePersistantObject::Reconstruct( )
//
//  Description:		Tell the graphic to reconstruct itself
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RHeadlinePersistantObject::Reconstruct( )
	{
	TpsAssert( m_pDocument, "Reconstruct called and there is no document associated with the Object." );

	if ( IsStateValid() )
		{
		//	Get the size of the first view and use it to determine the size of the headline...
		YViewIterator	iterator		= m_pDocument->GetViewCollectionStart( );
		YViewIterator	iteratorEnd	= m_pDocument->GetViewCollectionEnd( );
		for ( ; iterator != iteratorEnd; ++iterator )
			{
			RRealSize			size		= (*iterator)->GetReferenceSize( );
			// reconstruct the headline, Don't call GetGraphic since that will try to call Reconstruct
			m_pHeadlineGraphic->Construct( size, *(*iterator) );
			}
		}
	}

// ****************************************************************************
//
//  Function Name:	RHeadlinePersistantObject::IsStateValid( )
//
//  Description:		Is the state of this persistantobject valid
//
//  Returns:			m_fStateValid
//
//  Exceptions:		None
//
// ****************************************************************************
//
BOOLEAN RHeadlinePersistantObject::IsStateValid( ) const
	{
	return m_fStateValid;
	}

// ****************************************************************************
//
//  Function Name:	RHeadlinePersistantObject::SetStateValid( )
//
//  Description:		Set the state of the persistant object 
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RHeadlinePersistantObject::SetStateValid( BOOLEAN fValid )
	{
	m_fStateValid = fValid;
	}

#ifdef	TPSDEBUG

// ****************************************************************************
//
//  Function Name:	RHeadlinePersistantObject::Validate( )
//
//  Description:		Object Validation
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RHeadlinePersistantObject::Validate( ) const
{
	RPersistantObject::Validate( );
}
#endif	//	TPSDEBUG
