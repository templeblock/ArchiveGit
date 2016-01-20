//****************************************************************************
//
// File Name:		HeadlineInterfaceImp.cpp
//
// Project:			Renaissance Headline Component
//
// Author:			Mike Houle
//
// Description:	Manages Headline settings.
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
//  $Logfile:: /PM8/HeadlineComp/Source/HeadlineInterfaceImp.cpp           $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:18p                                                  $
// $Revision:: 1                                                              $
//
//****************************************************************************

#include "HeadlineIncludes.h"

ASSERTNAME

#include "HeadlineInterfaceImp.h"
#include "HeadlineView.h"
#include "HeadlinePersistantObject.h"
#include "Storage.h"

// ****************************************************************************
//
//  Function Name:	RHeadlineHolderImp::RHeadlineHolderImp( )
//
//  Description:		Constructor.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
RHeadlineHolderImp::RHeadlineHolderImp( RHeadlinePersistantObject& persistantObject )
{
	persistantObject.GetHeadlineData( &m_HeadlineData );
}

// ****************************************************************************
//
//  Function Name:	RHeadlineHolderImp::Read( )
//
//  Description:		Read in the data
//
//  Returns:			Nothing
//
//  Exceptions:		File Exceptions
//
// ****************************************************************************
//
void RHeadlineHolderImp::Read( RStorage& storage )
{
	RHeadlinePersistantObject::ReadAttribData( m_HeadlineData.structAttribData, storage );
	RHeadlinePersistantObject::ReadShapeData( m_HeadlineData.structShapeData, storage );
	RHeadlinePersistantObject::ReadOutlineData( m_HeadlineData.structOutlineData, storage );
	RHeadlinePersistantObject::ReadEffectsData( m_HeadlineData.structEffectsData, storage );
	RHeadlinePersistantObject::ReadBehindData( m_HeadlineData.structBehindEffectsData, storage );
	RHeadlinePersistantObject::ReadInterfaceData( m_HeadlineData.structInterfaceData, storage );

	//	Read in the string
	storage >> m_HeadlineData.sHeadlineText;

	// Read and verify padding
	uLONG ulValue ;
	storage	>> ulValue;		TpsAssert( ulValue == 0, "Padding field not zero" );
	storage	>> ulValue;		TpsAssert( ulValue == 0, "Padding field not zero" );
}

// ****************************************************************************
//
//  Function Name:	RHeadlineHolderImp::Write( )
//
//  Description:		Write out the data
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RHeadlineHolderImp::Write( RStorage& storage ) const
{
	RHeadlinePersistantObject::WriteAttribData( m_HeadlineData.structAttribData, storage );
	RHeadlinePersistantObject::WriteShapeData( m_HeadlineData.structShapeData, storage );
	RHeadlinePersistantObject::WriteOutlineData( m_HeadlineData.structOutlineData, storage );
	RHeadlinePersistantObject::WriteEffectsData( m_HeadlineData.structEffectsData, storage );
	RHeadlinePersistantObject::WriteBehindData( m_HeadlineData.structBehindEffectsData, storage );
	RHeadlinePersistantObject::WriteInterfaceData( m_HeadlineData.structInterfaceData, storage ) ;

	//	Write out the string
	storage << m_HeadlineData.sHeadlineText;
		
	//	Write out padding
	uLONG ulValue = 0;
	storage	<< ulValue;
	storage	<< ulValue;
}

// ****************************************************************************
//
//  Function Name:	RHeadlineHolderImp::RHeadlineHolderImp( )
//
//  Description:		Constructor.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
HeadlineDataStruct& RHeadlineHolderImp::GetHeadlineData( )
{
	return m_HeadlineData;
}

#ifdef	TPSDEBUG
// ****************************************************************************
//
//  Function Name:	RHeadlineHolderImp::Validate( )
//
//  Description:		Constructor.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RHeadlineHolderImp::Validate( ) const
{
	return;
}
#endif	//	TPSDEBUG

// ****************************************************************************
//
//  Function Name:	RHeadlineInterfaceImp::RHeadlineInterfaceImp( )
//
//  Description:		Constructor.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
RHeadlineInterfaceImp::RHeadlineInterfaceImp( const RHeadlineView* pView )
	: RHeadlineInterface( pView ),
	  m_pDocument( pView->GetHeadlineDocument() ),
	  m_PersistantObject( (const_cast<RHeadlineView*>(pView))->GetHeadlineDocument()->HeadlineObject() )
{
	pView->GetShadowSettings( m_ShadowSettings );
	m_PersistantObject.GetHeadlineData( &m_HeadlineData );
}

// ****************************************************************************
//
//  Function Name:	RHeadlineInterfaceImp::CreateInterface( )
//
//  Description:		Constructor.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
RInterface* RHeadlineInterfaceImp::CreateInterface( const RComponentView* pView )
{
	TpsAssertIsObject( RHeadlineView, pView );
	return new RHeadlineInterfaceImp( static_cast<const RHeadlineView*>( pView ) );
}

// ****************************************************************************
//
//  Function Name:	RHeadlineInterfaceImp::GetData( )
//
//  Description:		Return a pointer to the given headline data.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
RComponentDataHolder* RHeadlineInterfaceImp::GetData( )
{
	return new RHeadlineHolderImp( m_PersistantObject );
}

// ****************************************************************************
//
//  Function Name:	RHeadlineInterfaceImp::EditData( )
//
//  Description:		Let the component edit itself
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RHeadlineInterfaceImp::EditData( )
{
	UnimplementedCode( );
}

// ****************************************************************************
//
//  Function Name:	RHeadlineInterfaceImp::SetData( )
//
//  Description:		Set the given data pointer into the data.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RHeadlineInterfaceImp::SetData( RComponentDataHolder* pHolder )
{
	TpsAssertIsObject( RHeadlineHolderImp, pHolder );
	RHeadlineHolderImp*	pHeadlineHolder	= static_cast<RHeadlineHolderImp*>( pHolder );
	m_HeadlineData	= pHeadlineHolder->GetHeadlineData();
	Commit( );
}


// ****************************************************************************
//
//  Function Name:	RHeadlineInterfaceImp::ClearData( )
//
//  Description:		Reset all data in the component to defaults
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RHeadlineInterfaceImp::ClearData( )
{
	UnimplementedCode( );
}

// ****************************************************************************
//
//  Function Name:	RHeadlineInterfaceImp::Read( )
//
//  Description:		Read the data from the given storage
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RHeadlineInterfaceImp::Read( RChunkStorage& )
{
	UnimplementedCode( );
}

// ****************************************************************************
//
//  Function Name:	RHeadlineInterfaceImp::Write( )
//
//  Description:		Write the data to the given storage
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RHeadlineInterfaceImp::Write( RChunkStorage& )
{
	UnimplementedCode( );
}

// ****************************************************************************
//
//  Function Name:	RHeadlineInterfaceImp::Render( )
//
//  Description:		Render the data.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RHeadlineInterfaceImp::Render( RDrawingSurface& , const R2dTransform&, const RIntRect&, const RIntRect& )
{
	UnimplementedCode( );
}

// ****************************************************************************
//
//  Function Name:	RHeadlineInterfaceImp::Commit( )
//
//  Description:		Commit the changes to the headline interface 
//							to the persistantobject.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void	RHeadlineInterfaceImp::Commit( )
{
	m_PersistantObject.SetHeadlineData( &m_HeadlineData );
	m_pDocument->InvalidateAllViews() ;
}

// ****************************************************************************
//
//  Function Name:	RHeadlineInterfaceImp::GetFontName( )
//
//  Description:		Return the Font name.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void	RHeadlineInterfaceImp::GetFontName( RMBCString& fontName )
{
	fontName = m_HeadlineData.structAttribData.sFontName;
}

// ****************************************************************************
//
//  Function Name:	RHeadlineInterfaceImp::GetEscapement( )
//
//  Description:		Return the text escapement.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void	RHeadlineInterfaceImp::GetEscapement( ETextEscapement& escapement )
{
	escapement = m_HeadlineData.structAttribData.eEscapement;
}

// ****************************************************************************
//
//  Function Name:	RHeadlineInterfaceImp::GetFontAttributes( )
//
//  Description:		Return the font attributes (Bold, Italic, Underline).
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void	RHeadlineInterfaceImp::GetFontAttributes( YFontAttributes& attributes )
{
	attributes	= 0;
	attributes	|= (m_HeadlineData.structAttribData.fBoldChecked? RFont::kBold : 0);
	attributes	|= (m_HeadlineData.structAttribData.fItalicChecked? RFont::kItalic : 0);
	attributes	|= (m_HeadlineData.structAttribData.fUnderlineChecked? RFont::kUnderline : 0);
}

// ****************************************************************************
//
//  Function Name:	RHeadlineInterfaceImp::GetJustification( )
//
//  Description:		Return the type of justification.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void	RHeadlineInterfaceImp::GetJustification( ETextJustification& justification )
{
	justification = m_HeadlineData.structAttribData.eSelJust;
}

// ****************************************************************************
//
//  Function Name:	RHeadlineInterfaceImp::GetCompensation( )
//
//  Description:		Return the compensation type.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void	RHeadlineInterfaceImp::GetCompensation( ETextCompensation& compensation )
{
	compensation = m_HeadlineData.structAttribData.eSelComp;
}

// ****************************************************************************
//
//  Function Name:	RHeadlineInterfaceImp::GetCompensationDegree( )
//
//  Description:		Get the compensation Degree.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void	RHeadlineInterfaceImp::GetCompensationDegree( float& flCompensation )
{
	flCompensation = m_HeadlineData.structAttribData.flCompDegree;
}

// ****************************************************************************
//
//  Function Name:	RHeadlineInterfaceImp::GetKerning( )
//
//  Description:		Return if Kerning is on or off.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void	RHeadlineInterfaceImp::GetKerning( BOOLEAN& fKerning )
{
	fKerning = m_HeadlineData.structAttribData.fKerningChecked;
}

// ****************************************************************************
//
//  Function Name:	RHeadlineInterfaceImp::GetLeading( )
//
//  Description:		Return if leading is on or off.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void	RHeadlineInterfaceImp::GetLeading( BOOLEAN& fLeading )
{
	fLeading = m_HeadlineData.structAttribData.fLeadingChecked;
}

// ****************************************************************************
//
//  Function Name:	RHeadlineInterfaceImp::GetScale1( )
//
//  Description:		Return the scale of line 1.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void	RHeadlineInterfaceImp::GetScale1( uWORD& uwScale )
{
	uwScale = m_HeadlineData.structAttribData.uwScale1;
}

// ****************************************************************************
//
//  Function Name:	RHeadlineInterfaceImp::GetScale2( )
//
//  Description:		Return the scale of line 2.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void	RHeadlineInterfaceImp::GetScale2( uWORD& uwScale )
{
	uwScale = m_HeadlineData.structAttribData.uwScale2;
}

// ****************************************************************************
//
//  Function Name:	RHeadlineInterfaceImp::GetScale3( )
//
//  Description:		Return the scale of line 3.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void	RHeadlineInterfaceImp::GetScale3( uWORD& uwScale )
{
	uwScale = m_HeadlineData.structAttribData.uwScale3;
}

// ****************************************************************************
//
//  Function Name:	RHeadlineInterfaceImp::GetNoDisplayEnvelop( )
//
//  Description:		Return if display of envelop is on or off.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void	RHeadlineInterfaceImp::GetNoDisplayEnvelop( BOOLEAN& fNotDisplay )
{
	fNotDisplay = m_HeadlineData.structAttribData.fNoBackgroundEnvelop;
}
// ****************************************************************************
//
//  Function Name:	RHeadlineInterfaceImp::GetNumLines( uWORD& numLines )
//
//  Description:		Returns the number of lines of text in the headline.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void	RHeadlineInterfaceImp::GetNumLines( uWORD& numLines )
{
	RHeadlineGraphic* headlineGraphic = m_PersistantObject.GetGraphic();
	TpsAssert( headlineGraphic, "Can't get headline graphic!" );
	if (headlineGraphic)
		numLines = headlineGraphic->GetNumLines();
	else
		numLines = 1;
}

// ****************************************************************************
//
//  Function Name:	RHeadlineInterfaceImp::SetFontName( )
//
//  Description:		set the name of the font.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void	RHeadlineInterfaceImp::SetFontName( const RMBCString& fontName )
{
	m_HeadlineData.structAttribData.sFontName = fontName;
}

// ****************************************************************************
//
//  Function Name:	RHeadlineInterfaceImp::SetEscapement( )
//
//  Description:		Set the font escapement.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void	RHeadlineInterfaceImp::SetEscapement( ETextEscapement escapement )
{
	m_HeadlineData.structAttribData.eEscapement = escapement;
}

// ****************************************************************************
//
//  Function Name:	RHeadlineInterfaceImp::SetFontAttributes( )
//
//  Description:		Set the font attributes (bold, italic, underline).
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void	RHeadlineInterfaceImp::SetFontAttributes( YFontAttributes attributes )
{
	m_HeadlineData.structAttribData.fBoldChecked = static_cast<BOOLEAN>(attributes & RFont::kBold);
	m_HeadlineData.structAttribData.fItalicChecked = static_cast<BOOLEAN>(attributes & RFont::kItalic);
	m_HeadlineData.structAttribData.fUnderlineChecked = static_cast<BOOLEAN>(attributes & RFont::kUnderline);
}

// ****************************************************************************
//
//  Function Name:	RHeadlineInterfaceImp::SetJustification( )
//
//  Description:		set the headline justification.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void	RHeadlineInterfaceImp::SetJustification( ETextJustification justification )
{
	m_HeadlineData.structAttribData.eSelJust	= justification;
}

// ****************************************************************************
//
//  Function Name:	RHeadlineInterfaceImp::SetCompensation( )
//
//  Description:		Set the type of compensation.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void	RHeadlineInterfaceImp::SetCompensation( ETextCompensation compensation )
{
	m_HeadlineData.structAttribData.eSelComp = compensation;
}

// ****************************************************************************
//
//  Function Name:	RHeadlineInterfaceImp::SetCompensationDegree( )
//
//  Description:		Set the compensation degre.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void	RHeadlineInterfaceImp::SetCompensationDegree( float flCompensation )
{
	m_HeadlineData.structAttribData.flCompDegree = flCompensation;
}

// ****************************************************************************
//
//  Function Name:	RHeadlineInterfaceImp::SetKerning( )
//
//  Description:		Set kerning on or off.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void	RHeadlineInterfaceImp::SetKerning( BOOLEAN fKerning )
{
	m_HeadlineData.structAttribData.fKerningChecked = fKerning;
}

// ****************************************************************************
//
//  Function Name:	RHeadlineInterfaceImp::SetLeading( )
//
//  Description:		Turn the leading on or off.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void	RHeadlineInterfaceImp::SetLeading( BOOLEAN fLeading )
{
	m_HeadlineData.structAttribData.fLeadingChecked = fLeading;
}

// ****************************************************************************
//
//  Function Name:	RHeadlineInterfaceImp::SetScale1( )
//
//  Description:		Set the line 1 scale.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void	RHeadlineInterfaceImp::SetScale1( uWORD uwScale )
{
	m_HeadlineData.structAttribData.uwScale1 = uwScale;
}

// ****************************************************************************
//
//  Function Name:	RHeadlineInterfaceImp::SetScale2( )
//
//  Description:		Set the line 2 scale.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void	RHeadlineInterfaceImp::SetScale2( uWORD uwScale )
{
	m_HeadlineData.structAttribData.uwScale2 = uwScale;
}

// ****************************************************************************
//
//  Function Name:	RHeadlineInterfaceImp::SetScale3( )
//
//  Description:		Set the line3 scale.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void	RHeadlineInterfaceImp::SetScale3( uWORD uwScale )
{
	m_HeadlineData.structAttribData.uwScale3 = uwScale;
}

// ****************************************************************************
//
//  Function Name:	RHeadlineInterfaceImp::SetNoDisplayEnvelop( )
//
//  Description:		Set display envelop on or off.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void	RHeadlineInterfaceImp::SetNoDisplayEnvelop( BOOLEAN fNotDisplay )
{
	m_HeadlineData.structAttribData.fNoBackgroundEnvelop = fNotDisplay;
}

// ****************************************************************************
//
//  Function Name:	RHeadlineInterfaceImp::GetDistortEffect( )
//
//  Description:		Return the distort Effect.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void	RHeadlineInterfaceImp::GetDistortEffect( EDistortEffects& effect )
{
	effect = m_HeadlineData.structShapeData.eDistortEffect;
}

// ****************************************************************************
//
//  Function Name:	RHeadlineInterfaceImp::GetShearAngle( )
//
//  Description:		Return the shear angle.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void	RHeadlineInterfaceImp::GetShearAngle( int& shearAngle )
{
	shearAngle = m_HeadlineData.structShapeData.nShearAngle;
}

// ****************************************************************************
//
//  Function Name:	RHeadlineInterfaceImp::GetRotation( )
//
//  Description:		Get the rotation part of the shape.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void	RHeadlineInterfaceImp::GetRotation( int& rotation )
{
	rotation = m_HeadlineData.structShapeData.nRotation;
}

// ****************************************************************************
//
//  Function Name:	RHeadlineInterfaceImp::GetAltPercent( )
//
//  Description:		Return the AltPercent (?).
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void	RHeadlineInterfaceImp::GetAltPercent( float& percent )
{
	percent = m_HeadlineData.structShapeData.flAltPercent;
}

// ****************************************************************************
//
//  Function Name:	RHeadlineInterfaceImp::GetDistort( )
//
//  Description:		Return TRUE if the headline is distorted.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void	RHeadlineInterfaceImp::GetDistort( BOOLEAN& fDistort )
{
	fDistort = m_HeadlineData.structShapeData.fDistort;
}

// ****************************************************************************
//
//  Function Name:	RHeadlineInterfaceImp::GetShapeIndex( )
//
//  Description:		Return the shape index.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void	RHeadlineInterfaceImp::GetShapeIndex( int& shapeIndex )
{
	shapeIndex = m_HeadlineData.structShapeData.nSelShapeIdx;
}

// ****************************************************************************
//
//  Function Name:	RHeadlineInterfaceImp::GetVerticalPlacement( )
//
//  Description:		Return the vertical placement.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void	RHeadlineInterfaceImp::GetVerticalPlacement( EVerticalPlacement& placement )
{
	placement = m_HeadlineData.structShapeData.eVerticalPlacement;
}

// ****************************************************************************
//
//  Function Name:	RHeadlineInterfaceImp::SetDistortEffect( )
//
//  Description:		Set the distort effect of the shape.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void	RHeadlineInterfaceImp::SetDistortEffect( EDistortEffects effect )
{
	m_HeadlineData.structShapeData.eDistortEffect = effect;
}

// ****************************************************************************
//
//  Function Name:	RHeadlineInterfaceImp::SetShearAngle( )
//
//  Description:		Set the shear angle for a shear shape.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void	RHeadlineInterfaceImp::SetShearAngle( int shearAngle )
{
	m_HeadlineData.structShapeData.nShearAngle = shearAngle;
}

// ****************************************************************************
//
//  Function Name:	RHeadlineInterfaceImp::SetRotation( )
//
//  Description:		Set the rotation of the compensation.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void	RHeadlineInterfaceImp::SetRotation( int rotation )
{
	m_HeadlineData.structShapeData.nRotation = rotation;
}

// ****************************************************************************
//
//  Function Name:	RHeadlineInterfaceImp::SetAltPercent( )
//
//  Description:		Set the Alt Percent(?).
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void	RHeadlineInterfaceImp::SetAltPercent( float percent )
{
	m_HeadlineData.structShapeData.flAltPercent = percent;
}

// ****************************************************************************
//
//  Function Name:	RHeadlineInterfaceImp::SetDistort( )
//
//  Description:		Set the distort flag for the shape.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void	RHeadlineInterfaceImp::SetDistort( BOOLEAN fDistort )
{
	m_HeadlineData.structShapeData.fDistort = fDistort;
}

// ****************************************************************************
//
//  Function Name:	RHeadlineInterfaceImp::SetShapeIndex( )
//
//  Description:		Set the index of the shape.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void	RHeadlineInterfaceImp::SetShapeIndex( int shapeIndex )
{
	m_HeadlineData.structShapeData.nSelShapeIdx	= shapeIndex;
}

// ****************************************************************************
//
//  Function Name:	RHeadlineInterfaceImp::SetVerticalPlacement( )
//
//  Description:		Set the vertical placement of the shape.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void	RHeadlineInterfaceImp::SetVerticalPlacement( EVerticalPlacement placement )
{
	m_HeadlineData.structShapeData.eVerticalPlacement = placement;
}

// ****************************************************************************
//
//  Function Name:	RHeadlineInterfaceImp::GetOutlineFillColor( )
//
//  Description:		Return the outline fill color.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void	RHeadlineInterfaceImp::GetOutlineFillColor( RColor& color )
{
	RHeadlineColor fillColor = m_HeadlineData.structOutlineData.outlineFillDesc;
	fillColor.InvertBaseTransform();
	color = fillColor.GetColor();

	if (RColor::kGradient == color.GetFillMethod())
	{
		// The outline fill color uses a slightly different coordinate
		// system then the rest of the app.  Everywhere else, pi is added
		// to the angle to account for a downward positive y-axis, but the 
		// outline fill color just needs to negate the angle, thus flipping 
		// it about the x-axis.  So, negate the angle and then add the default
		// adjustment.
		//
		// 10/30/97 Lance - Ok, something changed in the gradient code, so
		// now we don't need to add pi into the adjustment, we only need
		// to negate the angle.
		//
		YAngle angle = -color.m_pGradient->GetGradientAngle() /*+ kPI*/ ;
		color.m_pGradient->SetGradientAngle( angle ) ;
	}
}

// ****************************************************************************
//
//  Function Name:	RHeadlineInterfaceImp::GetOutlineEffect( )
//
//  Description:		Return the effect for this outline.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void	RHeadlineInterfaceImp::GetOutlineEffect( EOutlineEffects& effect )
{
	effect = m_HeadlineData.structOutlineData.eOutlineEffect;
}

// ****************************************************************************
//
//  Function Name:	RHeadlineInterfaceImp::GetLineWeight( )
//
//  Description:		Return the weight of the line.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void	RHeadlineInterfaceImp::GetLineWeight( ELineWeight& weight )
{
	weight = m_HeadlineData.structOutlineData.eLineWeight;
}

// ****************************************************************************
//
//  Function Name:	RHeadlineInterfaceImp::GetStrokeColor( )
//
//  Description:		Return the color of the stroke.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void	RHeadlineInterfaceImp::GetStrokeColor( RSolidColor& color )
{
	color = m_HeadlineData.structOutlineData.colorStroke;
}

// ****************************************************************************
//
//  Function Name:	RHeadlineInterfaceImp::GetShadowColor( )
//
//  Description:		Return the shadow color.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void	RHeadlineInterfaceImp::GetShadowColor( RSolidColor& color )
{
	color = m_HeadlineData.structOutlineData.colorShadow;
}

// ****************************************************************************
//
//  Function Name:	RHeadlineInterfaceImp::GetHiliteColor( )
//
//  Description:		Get the hilite color of the outline.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void	RHeadlineInterfaceImp::GetHiliteColor( RSolidColor& color )
{
	color = m_HeadlineData.structOutlineData.colorHilite;
}

// ****************************************************************************
//
//  Function Name:	RHeadlineInterfaceImp::GetBlur2Color( )
//
//  Description:		Return the color the outline is bluring 2.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void	RHeadlineInterfaceImp::GetBlur2Color( RSolidColor& color )
{
	color = m_HeadlineData.structOutlineData.colorBlur2;
}

// ****************************************************************************
//
//  Function Name:	RHeadlineInterfaceImp::SetOutlineFillColor( )
//
//  Description:		Set the outline Fill color.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void	RHeadlineInterfaceImp::SetOutlineFillColor( const RColor& color )
{
	if (RColor::kGradient == color.GetFillMethod())
	{
		// The outline fill color uses a slightly different coordinate
		// system then the rest of the app.  Everywhere else, pi is added
		// to the angle to account for a downward positive y-axis, but the 
		// outline fill color just needs to negate the angle, thus flipping 
		// it about the x-axis.  So, undo the previous adjustment and negate 
		// the result.
		//
		// 10/30/97 Lance - Ok, something changed in the gradient code, so
		// now we don't need to undo the previous adjustment, we only need
		// to negate the angle.
		//
		RColor crTemp = color ;
		YAngle angle  = -(crTemp.m_pGradient->GetGradientAngle() /*- kPI*/) ;
		crTemp.m_pGradient->SetGradientAngle( angle ) ;
		m_HeadlineData.structOutlineData.outlineFillDesc = RHeadlineColor( crTemp );

		return ;
	}

	m_HeadlineData.structOutlineData.outlineFillDesc = RHeadlineColor( color );
	m_HeadlineData.structOutlineData.outlineFillDesc.InvertBaseTransform();
}

// ****************************************************************************
//
//  Function Name:	RHeadlineInterfaceImp::SetOutlineEffect( )
//
//  Description:		Set the outline effect type.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void	RHeadlineInterfaceImp::SetOutlineEffect( EOutlineEffects effect )
{
	m_HeadlineData.structOutlineData.eOutlineEffect = effect;
}

// ****************************************************************************
//
//  Function Name:	RHeadlineInterfaceImp::SetLineWeight( )
//
//  Description:		Set the line weight of the outline.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void	RHeadlineInterfaceImp::SetLineWeight( ELineWeight weight )
{
	m_HeadlineData.structOutlineData.eLineWeight = weight;
}

// ****************************************************************************
//
//  Function Name:	RHeadlineInterfaceImp::SetStrokeColor( )
//
//  Description:		Set the color of the outline stroke.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void	RHeadlineInterfaceImp::SetStrokeColor( const RSolidColor& color )
{
	m_HeadlineData.structOutlineData.colorStroke = color;
}

// ****************************************************************************
//
//  Function Name:	RHeadlineInterfaceImp::SetShadowColor( )
//
//  Description:		Set the Shadow Color.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void	RHeadlineInterfaceImp::SetShadowColor( const RSolidColor& color )
{
	m_HeadlineData.structOutlineData.colorShadow = color;
}

// ****************************************************************************
//
//  Function Name:	RHeadlineInterfaceImp::SetHiliteColor( )
//
//  Description:		Set the hilite color of the outline.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void	RHeadlineInterfaceImp::SetHiliteColor( const RSolidColor& color )
{
	m_HeadlineData.structOutlineData.colorHilite = color;
}

// ****************************************************************************
//
//  Function Name:	RHeadlineInterfaceImp::SetBlur2Color( )
//
//  Description:		Set the outline Blur 2 color.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void	RHeadlineInterfaceImp::SetBlur2Color( const RSolidColor& color )
{
	m_HeadlineData.structOutlineData.colorBlur2 = color;
}

// ****************************************************************************
//
//  Function Name:	RHeadlineInterfaceImp::GetNumStages( )
//
//  Description:		Return the number of stages in the projection.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void	RHeadlineInterfaceImp::GetNumStages( int& stages )
{
	stages = m_HeadlineData.structEffectsData.nNumStages;
}

// ****************************************************************************
//
//  Function Name:	RHeadlineInterfaceImp::GetProjectionFillColor( )
//
//  Description:		Return the Projection Fill Color.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void	RHeadlineInterfaceImp::GetProjectionFillColor( RColor& color )
{
	color = m_HeadlineData.structEffectsData.projectionFillDesc.GetColor();
}

// ****************************************************************************
//
//  Function Name:	RHeadlineInterfaceImp::GetProjectionEffect( )
//
//  Description:		Return the projection effect.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void	RHeadlineInterfaceImp::GetProjectionEffect( EProjectionEffects& effect )
{
	effect = m_HeadlineData.structEffectsData.eProjectionEffect;
}

// ****************************************************************************
//
//  Function Name:	RHeadlineInterfaceImp::GetProjectionVanishPoint( )
//
//  Description:		Return the vanishing point of the projection.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void	RHeadlineInterfaceImp::GetProjectionVanishPoint( RIntPoint& vanishPt )
{
	vanishPt.m_x = m_HeadlineData.structEffectsData.nProjectionVanishPtX;
	vanishPt.m_y = m_HeadlineData.structEffectsData.nProjectionVanishPtY;
}

// ****************************************************************************
//
//  Function Name:	RHeadlineInterfaceImp::GetProjectionDepth( )
//
//  Description:		Return the depth of the projection.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void	RHeadlineInterfaceImp::GetProjectionDepth( float& flDepth )
{
	flDepth = m_HeadlineData.structEffectsData.flProjectionDepth;
}

// ****************************************************************************
//
//  Function Name:	RHeadlineInterfaceImp::GetShadowFillColor( )
//
//  Description:		Return the shadow fill color (only valid if effect is
//							solid color or fill).
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void	RHeadlineInterfaceImp::GetShadowFillColor( RColor& color )
{
	color = m_ShadowSettings.m_fShadowColor;
//	color = m_HeadlineData.structEffectsData.shadowFillDesc.GetColor();
}

// ****************************************************************************
//
//  Function Name:	RHeadlineInterfaceImp::GetShadowEffect( )
//
//  Description:		Return the Shadow effect.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void	RHeadlineInterfaceImp::GetShadowEffect( EShadowEffects& effect )
{
	if (!m_ShadowSettings.m_fShadowOn)
		effect = kNoShadow;
	else if (m_ShadowSettings.m_fShadowEdgeSoftness > 0.0)
		effect = kSoftDropShadow;
	else
		effect = kDropShadow;

//	effect = m_HeadlineData.structEffectsData.eShadowEffect;
}

// ****************************************************************************
//
//  Function Name:	RHeadlineInterfaceImp::GetShadowVanishPoint( )
//
//  Description:		Return the shadow vanishing point.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void	RHeadlineInterfaceImp::GetShadowVanishPoint( RIntPoint& vanishPt )
{
	vanishPt.m_x = m_HeadlineData.structEffectsData.nShadowVanishPtX;
	vanishPt.m_y = m_HeadlineData.structEffectsData.nShadowVanishPtY;
}

// ****************************************************************************
//
//  Function Name:	RHeadlineInterfaceImp::GetShadowDepth( )
//
//  Description:		Return the depth of the shadow.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void	RHeadlineInterfaceImp::GetShadowDepth( float& flDepth )
{
	flDepth = m_HeadlineData.structEffectsData.flShadowDepth;
}

// ****************************************************************************
//
//  Function Name:	RHeadlineInterfaceImp::GetShadowColor2( )
//
//  Description:		Get the Blend color of the shadow (only if the color 
//							is a gradient blend.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void	RHeadlineInterfaceImp::GetShadowColor2( RSolidColor& color )
{
	color = m_HeadlineData.structEffectsData.colorShadow2;
}

// ****************************************************************************
//
//  Function Name:	RHeadlineInterfaceImp::SetNumStages( )
//
//  Description:		Set the number of stages in the project.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void	RHeadlineInterfaceImp::SetNumStages( int stages )
{
	m_HeadlineData.structEffectsData.nNumStages = stages;
}

// ****************************************************************************
//
//  Function Name:	RHeadlineInterfaceImp::SetProjectionFillColor( )
//
//  Description:		Set the Projection Fill Color.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void	RHeadlineInterfaceImp::SetProjectionFillColor( const RColor& color )
{
	if (RColor::kGradient == color.GetFillMethod())
	{
		//
		// Convert gradient into a headline "blend" color
		//
		RSolidColor crStart = color.GetSolidColor( 0.0 ) ;
		RSolidColor crFinish = color.GetSolidColor( 100.0 ) ;

		m_HeadlineData.structEffectsData.projectionFillDesc = 
			RHeadlineColor( crStart, crFinish, kSouth ) ;

		return ;
	}

	m_HeadlineData.structEffectsData.projectionFillDesc = RHeadlineColor( color );
}

// ****************************************************************************
//
//  Function Name:	RHeadlineInterfaceImp::SetProjectionEffect( )
//
//  Description:		Set the projection effect.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void	RHeadlineInterfaceImp::SetProjectionEffect( EProjectionEffects effect )
{
	m_HeadlineData.structEffectsData.eProjectionEffect = effect;
}

// ****************************************************************************
//
//  Function Name:	RHeadlineInterfaceImp::SetProjectionVanishPoint( )
//
//  Description:		Set the Projection vanishing point.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void	RHeadlineInterfaceImp::SetProjectionVanishPoint( const RIntPoint& vanishPt )
{
	m_HeadlineData.structEffectsData.nProjectionVanishPtX = vanishPt.m_x;
	m_HeadlineData.structEffectsData.nProjectionVanishPtY = vanishPt.m_y;
}

// ****************************************************************************
//
//  Function Name:	RHeadlineInterfaceImp::SetProjectionDepth( )
//
//  Description:		Set the depth of the projection.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void	RHeadlineInterfaceImp::SetProjectionDepth( float flDepth )
{
	m_HeadlineData.structEffectsData.flProjectionDepth = flDepth;
}

// ****************************************************************************
//
//  Function Name:	RHeadlineInterfaceImp::SetShadowFillColor( )
//
//  Description:		Set the fill color for the shadow (if not a gradient).
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void	RHeadlineInterfaceImp::SetShadowFillColor( const RColor& color )
{
	RComponentView* pView = (RComponentView *) m_pDocument->GetActiveView();

	m_ShadowSettings.m_fShadowColor = color.GetSolidColor();
	pView->SetShadowSettings( m_ShadowSettings );

//	m_HeadlineData.structEffectsData.shadowFillDesc = RHeadlineColor( color );
}

// ****************************************************************************
//
//  Function Name:	RHeadlineInterfaceImp::SetShadowEffect( )
//
//  Description:		Set the effect of the shadow.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void	RHeadlineInterfaceImp::SetShadowEffect( EShadowEffects effect )
{
	if (kNoShadow == effect)
		m_ShadowSettings.m_fShadowOn = FALSE;
	else
	{
		m_ShadowSettings.m_fShadowOn = TRUE;

		if (kDropShadow == effect)
		{
			m_ShadowSettings.m_fShadowOpacity      = 1.0;
			m_ShadowSettings.m_fShadowEdgeSoftness = 0.0;
		}
		else
		{
			m_ShadowSettings.m_fShadowOpacity      = 0.65;
			m_ShadowSettings.m_fShadowEdgeSoftness = 0.40;
		}
	}

//	m_HeadlineData.structEffectsData.eShadowEffect = effect;
}

// ****************************************************************************
//
//  Function Name:	RHeadlineInterfaceImp::SetShadowVanishPoint( )
//
//  Description:		Set the vanishing point for the shadow.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void	RHeadlineInterfaceImp::SetShadowVanishPoint( const RIntPoint& vanishPt )
{
	m_ShadowSettings.m_fShadowAngle = -atan2( vanishPt.m_y,	vanishPt.m_x );

	RComponentView* pView = (RComponentView *) m_pDocument->GetActiveView();
	pView->SetShadowSettings( m_ShadowSettings );

	m_HeadlineData.structEffectsData.nShadowVanishPtX = vanishPt.m_x;
	m_HeadlineData.structEffectsData.nShadowVanishPtY = vanishPt.m_y;
}

// ****************************************************************************
//
//  Function Name:	RHeadlineInterfaceImp::SetShadowDepth( )
//
//  Description:		Set the depth of the shadow.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void	RHeadlineInterfaceImp::SetShadowDepth( float flDepth )
{
	// Determine shadow depth
	if (flDepth > 0.666)
	  m_ShadowSettings.m_fShadowOffset = 0.86;
	else if (flDepth > 0.333)
	  m_ShadowSettings.m_fShadowOffset = 0.47;
	else
	  m_ShadowSettings.m_fShadowOffset = 0.25;

	RComponentView* pView = (RComponentView *) m_pDocument->GetActiveView();
	pView->SetShadowSettings( m_ShadowSettings );

	m_HeadlineData.structEffectsData.flShadowDepth = flDepth;
}

// ****************************************************************************
//
//  Function Name:	RHeadlineInterfaceImp::SetShadowColor2( )
//
//  Description:		Set the shadow blend 2 color
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void	RHeadlineInterfaceImp::SetShadowColor2( const RSolidColor& color )
{
	m_HeadlineData.structEffectsData.colorShadow2 = color;
}

// ****************************************************************************
//
//  Function Name:	RHeadlineInterfaceImp::GetBehindColor( )
//
//  Description:		Return the Behind Color.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void	RHeadlineInterfaceImp::GetBehindColor( RColor& color )
{
	color = m_HeadlineData.structBehindEffectsData.behindFillDesc.GetColor();
}

// ****************************************************************************
//
//  Function Name:	RHeadlineInterfaceImp::GetBehindEffect( )
//
//  Description:		Set the behind Effect.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void	RHeadlineInterfaceImp::GetBehindEffect( EFramedEffects& effect )
{
	effect = m_HeadlineData.structBehindEffectsData.eBehindEffect;
}

// ****************************************************************************
//
//  Function Name:	RHeadlineInterfaceImp::GetBehindShadowColor( )
//
//  Description:		Get the behind Shadow Color.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void	RHeadlineInterfaceImp::GetBehindShadowColor( RSolidColor& color )
{
	color = m_HeadlineData.structBehindEffectsData.colorShadow;
}

// ****************************************************************************
//
//  Function Name:	RHeadlineInterfaceImp::GetBehindHilite( )
//
//  Description:		Return the Behind Hilite Color.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void	RHeadlineInterfaceImp::GetBehindHilite( RSolidColor& color )
{
	color = m_HeadlineData.structBehindEffectsData.colorHilite;
}

// ****************************************************************************
//
//  Function Name:	RHeadlineInterfaceImp::SetBehindColor( )
//
//  Description:		Set the behind color
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void	RHeadlineInterfaceImp::SetBehindColor( const RColor& color )
{
	m_HeadlineData.structBehindEffectsData.behindFillDesc = RHeadlineColor( color );
}

// ****************************************************************************
//
//  Function Name:	RHeadlineInterfaceImp::SetBehindEffect( )
//
//  Description:		Set the behind Effect.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void	RHeadlineInterfaceImp::SetBehindEffect( EFramedEffects effect )
{
	m_HeadlineData.structBehindEffectsData.eBehindEffect = effect;
}

// ****************************************************************************
//
//  Function Name:	RHeadlineInterfaceImp::SetBehindShadowColor( )
//
//  Description:		Set the behind Shadow color.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void	RHeadlineInterfaceImp::SetBehindShadowColor( const RSolidColor& color )
{
	m_HeadlineData.structBehindEffectsData.colorShadow = color;
}

// ****************************************************************************
//
//  Function Name:	RHeadlineInterfaceImp::SetBehindHilite( )
//
//  Description:		Sets the Behind Hilite Color.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void	RHeadlineInterfaceImp::SetBehindHilite( const RSolidColor& color )
{
	m_HeadlineData.structBehindEffectsData.colorHilite = color;
}

// ****************************************************************************
//
//  Function Name:	RHeadlineInterfaceImp::GetInterfaceData( )
//
//  Description:		Return the headline interface data.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void	RHeadlineInterfaceImp::GetInterfaceData( uLONG& ulData )
{
	ulData = m_HeadlineData.structInterfaceData.ulData;
}

// ****************************************************************************
//
//  Function Name:	RHeadlineInterfaceImp::SetInterfaceData( )
//
//  Description:		Sets the Interface data.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void	RHeadlineInterfaceImp::SetInterfaceData( uLONG ulData )
{
	m_HeadlineData.structInterfaceData.ulData = ulData;
}

// ****************************************************************************
//
//  Function Name:	RHeadlineInterfaceImp::GetHeadlineText( )
//
//  Description:		Returns the headline text.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void	RHeadlineInterfaceImp::GetHeadlineText( RMBCString& text )
{
	text = m_HeadlineData.sHeadlineText;
}

// ****************************************************************************
//
//  Function Name:	RHeadlineInterfaceImp::SetHeadlineText( )
//
//  Description:		Sets the Headline Text.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void	RHeadlineInterfaceImp::SetHeadlineText( const RMBCString& text )
{
	m_HeadlineData.sHeadlineText = text;
}
