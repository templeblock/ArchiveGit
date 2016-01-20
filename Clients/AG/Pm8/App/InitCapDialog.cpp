/*	$Header: /PM8/App/InitCapDialog.cpp 1     3/03/99 6:06p Gbeddow $
//
//	Definition of the RInitCapDialog class.
//
//	This class provides a wrapper for the MFC dialog class.  This dialog allows
// for the specification of an initial capital compound graphic.
//	
// Author:     Mike Heydlauf
//
// Portability: Windows Specific
//
// Developed by:	Turning Point Software
//						One Gateway Center, Suite 800
//						Newton, Ma 02158
//						(617)332-0202
//
// Copyright (C) 1996 Turning Point Software. All Rights Reserved.
//
// $Log: /PM8/App/InitCapDialog.cpp $
// 
// 1     3/03/99 6:06p Gbeddow
// 
// 2     2/11/99 3:01p Rgrenfel
// Forced reconstruction of the graphics collection searcher when
// necessary.
// 
// 1     2/08/99 3:10p Rgrenfel
// Implementation of the initial capitals grouped object dialog.
*/

#include "StdAfx.h"

ASSERTNAME

#include "InitCapDialog.h"
#include "ComponentTypes.h"
#include "ComponentView.h"
#include "ComponentDocument.h"
#include "WinColorDlg.h"
#include "ResourceManager.h"
#include "CompositeSelection.h"

const RIntPoint kShadowVanishPtRight			= RIntPoint(  5, 5 );
const RIntPoint kShadowVanishPtLeft				= RIntPoint( -5, 5 );

const RIntPoint kProjectionVanishPtRight		= RIntPoint(  5, 5 );
const RIntPoint kProjectionVanishPtLeft		= RIntPoint( -5, 5 );
const RIntPoint kProjectionVanishPtBottom		= RIntPoint(  0, 5 );

const RIntPoint kPsuedoPerspectiveVanishPt	= RIntPoint(  0, 5 );

/*
The list of Text Styles for Initial Caps and Numbers is:
        Plain                   Plain distortion headline text
        Blend Down              Face - blend down
        Blend Right             Face - blend color to white to the right
        Blend Left              Face - blend color to white to the left     
        Radial Blend            Face - blend color to white radial burst
        Outline                 Outline - Stroke and Fill Medium
        Ghost(?)                Outline - Hairline White Medium Black
        Neon                    Outline - Thick White Thick Black
        Blur                    Outline - Blur
        Drop Shadow Right       Depth - Drop Shadow Medium/angle lower rt
        Drop Shadow Left        Depth - Drop Shadow Medium/angle lower lf
        Soft Shadow Right       Depth - Soft Drop Shadow/angle lower rt
        Soft Shadow Left        Depth - Soft Drop Shadow/angle lower lf
        Stack Right             Depth - Simple Stack Medium/angle lower rt 
        Stack Left              Depth - Simple Stack Medium/angle lower lf 
        Stack Bottom            Depth - Simple Stack Medium/angle bottom
        Perspective             Depth - Pseudo Persp. Extr. Deep/bottom
*/
#if 0 // Keep this table around for now just in case we decide to implement it somehow..
const RGOHeadlineData RInitCapDialog::m_rCannedInitialEffects[kNumberOfCannedInitialEffects] = 
{							 /*RGOHeadlineData( EOutlineEffects,       RColor,				    ELineWeight,      RSolidColor,        EShadowEffects,     RSolidColor,        RIntPoint,		                YFloatType,                EProjectionEffects, 		RIntPoint,			                YFloatType );*/
							 /*RGOHeadlineData( eOutlineEffect,        rOutlineFillColor,   eLineWeight,      rStrokeColor,       eShadowEffect,      rShadowColor,       rShadowVanishPt,              yShadowDepth,              eProjectionEffect,  		rProjectionVanishPt,              yProjectionDepth );*/
/*Plain*/					RGOHeadlineData( kSimpleStroke,	       RColor(kBlack),      kMediumLine,      RSolidColor(kBlack),     kNoShadow,          RSolidColor(kBlack),     kDefaultShadowVanishPt,       kDefaultShadowDepth,       kNoProjection,      		kDefaultProjectionVanishPt,       kDefaultProjectionDepth ),
/*Blend Down*/				RGOHeadlineData( kSimpleStrokeFill,	    RColor(kBlack),      kMediumLine,      RSolidColor(kBlack),     kNoShadow,          RSolidColor(kBlack),     kDefaultShadowVanishPt,       kDefaultShadowDepth,       kNoProjection,      		kDefaultProjectionVanishPt,       kDefaultProjectionDepth ),
/*Blend Right*/			RGOHeadlineData( kSimpleStrokeFill,	    RColor(kBlack),      kMediumLine,      RSolidColor(kBlack),     kNoShadow,          RSolidColor(kBlack),     kDefaultShadowVanishPt,       kDefaultShadowDepth,       kNoProjection,      		kDefaultProjectionVanishPt,       kDefaultProjectionDepth ),
/*Blend Left*/				RGOHeadlineData( kSimpleStrokeFill,	    RColor(kBlack),      kMediumLine,      RSolidColor(kBlack),     kNoShadow,          RSolidColor(kBlack),     kDefaultShadowVanishPt,       kDefaultShadowDepth,       kNoProjection,      		kDefaultProjectionVanishPt,       kDefaultProjectionDepth ),
/*Radial Blend*/			RGOHeadlineData( kSimpleStrokeFill,     RColor(kBlack),      kMediumLine,      RSolidColor(kBlack),     kNoShadow,          RSolidColor(kBlack),     kDefaultShadowVanishPt,       kDefaultShadowDepth,       kNoProjection,      		kDefaultProjectionVanishPt,       kDefaultProjectionDepth ),
/*Outline*/					RGOHeadlineData( kSimpleStrokeFill,     RColor(kBlack),      kMediumLine,      RSolidColor(kBlack),     kNoShadow,          RSolidColor(kBlack),     kDefaultShadowVanishPt,       kDefaultShadowDepth,       kNoProjection,      		kDefaultProjectionVanishPt,       kDefaultProjectionDepth ),
/*Ghost*/					RGOHeadlineData( kHairlineDoubleOutline,RColor(kBlack),      kMediumLine,      RSolidColor(kWhite),     kNoShadow,          RSolidColor(kBlack),     kDefaultShadowVanishPt,       kDefaultShadowDepth,       kNoProjection,      		kDefaultProjectionVanishPt,       kDefaultProjectionDepth ),
/*Neon*/						RGOHeadlineData( kDoubleOutline,	       RColor(kBlack),      kThickLine,       RSolidColor(kWhite),     kNoShadow,          RSolidColor(kBlack),     kDefaultShadowVanishPt,       kDefaultShadowDepth,       kNoProjection,		      kDefaultProjectionVanishPt,       kDefaultProjectionDepth ),
/*Blur*/						RGOHeadlineData( kBlurredOutline,       RColor(kBlack),      kMediumLine,      RSolidColor(kBlack),     kNoShadow,          RSolidColor(kBlack),     kDefaultShadowVanishPt,       kDefaultShadowDepth,       kNoProjection,      		kDefaultProjectionVanishPt,       kDefaultProjectionDepth ),
/*Drop Shadow Right*/	RGOHeadlineData( kSimpleFill,		       RColor(kBlack),      kMediumLine,      RSolidColor(kBlack),     kDropShadow,        RSolidColor(kBlack),     kShadowVanishPtRight,         kDefaultShadowDepth,       kNoProjection,      		kDefaultProjectionVanishPt,       kDefaultProjectionDepth ),
/*Drop Shadow Left*/		RGOHeadlineData( kSimpleFill,		       RColor(kBlack),      kMediumLine,      RSolidColor(kBlack),     kDropShadow,        RSolidColor(kBlack),     kShadowVanishPtLeft   ,       kDefaultShadowDepth,       kNoProjection,      		kDefaultProjectionVanishPt,       kDefaultProjectionDepth ),
/*Soft Shadow Right*/	RGOHeadlineData( kSimpleFill,		       RColor(kBlack),      kMediumLine,      RSolidColor(kBlack),     kSoftDropShadow,    RSolidColor(kBlack),     kShadowVanishPtRight,         kDefaultShadowDepth,       kNoProjection,      		kDefaultProjectionVanishPt,       kDefaultProjectionDepth ),
/*Soft Shadow Left*/		RGOHeadlineData( kSimpleFill,		       RColor(kBlack),      kMediumLine,      RSolidColor(kBlack),     kSoftDropShadow,    RSolidColor(kBlack),     kShadowVanishPtLeft   ,       kDefaultShadowDepth,       kNoProjection,      		kDefaultProjectionVanishPt,       kDefaultProjectionDepth ),
/*Stack Right*/			RGOHeadlineData( kSimpleStroke,	       RColor(kBlack),      kMediumLine,      RSolidColor(kBlack),     kNoShadow,          RSolidColor(kBlack),     kDefaultShadowVanishPt,       kDefaultShadowDepth,       kSimpleStack,       		kProjectionVanishPtRight,         kDefaultProjectionDepth ),
/*Stack Left*/				RGOHeadlineData( kSimpleStroke,	       RColor(kBlack),      kMediumLine,      RSolidColor(kBlack),     kNoShadow,          RSolidColor(kBlack),     kDefaultShadowVanishPt,       kDefaultShadowDepth,       kSimpleStack,       		kProjectionVanishPtLeft,          kDefaultProjectionDepth ),
/*Stack Bottom*/			RGOHeadlineData( kSimpleStroke,	       RColor(kBlack),      kMediumLine,      RSolidColor(kBlack),     kNoShadow,          RSolidColor(kBlack),     kDefaultShadowVanishPt,       kDefaultShadowDepth,       kSimpleStack,				kProjectionVanishPtBottom,        kDefaultProjectionDepth ),
/*Perspective*/			RGOHeadlineData( kSimpleStroke,	       RColor(kBlack),      kMediumLine,      RSolidColor(kBlack),     kNoShadow,          RSolidColor(kBlack),     kDefaultShadowVanishPt,       kDefaultShadowDepth,       kPseudoPerspectiveStack, kPsuedoPerspectiveVanishPt,       1 )
};
#endif

// ****************************************************************************
//
//  Function Name:	RInitCapDialog::RInitCapDialog
//
//  Description:		Ctor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
RInitCapDialog::RInitCapDialog( RStandaloneDocument* pParentDocument, RComponentDocument* pEditedComponent, int nResourceId )
	: RCommonGroupedObjectDialog(pParentDocument,pEditedComponent,nResourceId),
	m_pGraphicCollectionSearcher( NULL ),
	m_rSolidColorEffect( RColor::kSolid ),
	m_fIsGradient( FALSE )
{
	m_rHeadlineText = ::GetResourceManager().GetResourceString( IDS_DEFAULT_INITCAP );
}

// ****************************************************************************
//
//  Function Name:	RInitCapDialog::~RInitCapDialog
//
//  Description:		Dtor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
RInitCapDialog::~RInitCapDialog()
{
	delete m_pGraphicCollectionSearcher;
}


// ****************************************************************************
//
//  Function Name:	RInitCapDialog::DoDataExchange
//
//  Description:		dialog data exchange
//
//  Returns:			void
//
//  Exceptions:		None
//
// ****************************************************************************
void RInitCapDialog::DoDataExchange(CDataExchange* pDX)
{
	RCommonGroupedObjectDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(RInitCapDialog)
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(RInitCapDialog, RCommonGroupedObjectDialog)
	//{{AFX_MSG_MAP(RInitCapDialog)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// RInitCapDialog message handlers


// ****************************************************************************
//
//  Function Name:	RInitCapDialog::OnInitDialog
//
//  Description:		dialog initialization.
//
//  Returns:			void
//
//  Exceptions:		None
//
// ****************************************************************************
BOOL RInitCapDialog::OnInitDialog() 
{
	// Call the base method
	if( !RCommonGroupedObjectDialog::OnInitDialog() ) return FALSE;

	// Limit the headline text to a single letter
	m_cHeadlineEditCtrl.SetLimitText( 1 );

	// Apply the headline effect and update the preview
	ApplyHeadlineEffect();

	m_fInitializing = FALSE;
	UpdatePreview();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
// ****************************************************************************
//
//  Function Name:	RInitCapDialog::CreateEffectssList
//
//  Description:		Create '|' delimited string of effects names
//
//  Returns:			void
//
//  Exceptions:		None
//
// ****************************************************************************
void RInitCapDialog::CreateEffectsList( RMBCString& rList )
{
	rList = ::GetResourceManager().GetResourceString( IDS_INITCAP_EFFECTS );
}
// ****************************************************************************
//
//  Function Name:	RInitCapDialog::CreateGraphicsList
//
//  Description:		Create '|' delimited string of graphics names
//
//  Returns:			void
//
//  Exceptions:		None
//
// ****************************************************************************
void RInitCapDialog::CreateGraphicsList( RMBCString& rList )
{
	//
	// Only allocate memory for the collection searcher once..
	if( m_pGraphicCollectionSearcher == NULL || m_fRebuiltGraphicArrays )
	{
		// We rebuilt the graphic arrays, so we have to recreate the searcher.
		if (m_pGraphicCollectionSearcher != NULL)
		{
			delete m_pGraphicCollectionSearcher;
		}
		m_pGraphicCollectionSearcher = new 	RCollectionSearcher<RInitCapGraphicSearchCollection>(m_rCollectionArray);
	}
	// Find all the graphics
	RSearchResultArray* pResultArray = m_pGraphicCollectionSearcher->SearchCollections();

	// Now iterate through all the graphics and insert them into
	// the string
	RSearchResultIterator resultIter( pResultArray->Start() );
	RSearchResultIterator resultIterEnd( pResultArray->End() );

	// The index has to start at 1 because the None selection is at zero
	for (int nIdx = 1; resultIter != resultIterEnd; resultIter++, nIdx++ )
	{
		rList += RMBCString( (LPSTR)(*resultIter).namePtr );
		rList += RMBCString( kListFieldSeperator );

		//
		// Keep track of graphics and their indexes...
		m_cGraphicMap.SetAt( (*resultIter).id, (*resultIter) );
		m_cGraphicArray.SetAtGrow( nIdx, (*resultIter) );
   }
}

// ****************************************************************************
//
//  Function Name:	RInitCapDialog::OnButtonGOOtherGraphics
//
//  Description:		Headline/Cap Effect selection changes.
//
//  Returns:			void
//
//  Exceptions:		None
//
// ****************************************************************************
void RInitCapDialog::OnSelchangeListboxEffects() 
{
	//
	// Apply canned headline effect..
	ApplyHeadlineEffect();
	//
	//		Select the headline
	RCompositeSelection* pSelection = m_pControlView->GetSelection( );
	pSelection->UnselectAll( TRUE );
	pSelection->Select( m_pHeadlineView, TRUE );
	//
	//	Updat the preview
	UpdatePreview( TRUE, FALSE );
}

// ****************************************************************************
//
//  Function Name:	RInitCapDialog::ApplyHeadlineEffect
//
//  Description:		Apply selected headline effect
//
//  Returns:			void
//
//  Exceptions:		None
//
// ****************************************************************************
void RInitCapDialog::ApplyHeadlineEffect()
{
	RGOHeadlineData rNewData;
	m_fIsGradient = FALSE;
	int nInitialStyleSelection = m_cEffectsListBox.GetCurSel();
	//
	// Make sure current selection isn't out of canned effects array bounds..
	TpsAssert( nInitialStyleSelection < kNumberOfCannedInitialEffects , "Selection out of array bounds" );

	switch( ECannedInitialEffects( nInitialStyleSelection ) )
	{
	case kPlain:
		break;

	case kBlendDown:
		{
			// Create gradient for blend down...
			RWinColorDlg rColorDlg;
			m_rColorEffect = rColorDlg.ColorToWhite( m_rSolidColorEffect, kLinear, -kPI/2.0, FALSE );
			m_fIsGradient = TRUE;
		}
		break;

	case kBlendRight:
		{
			// Create gradient for blend right...
			RWinColorDlg rColorDlg;
			m_rColorEffect = rColorDlg.ColorToWhite( m_rSolidColorEffect, kLinear, .0, FALSE );
			m_fIsGradient = TRUE;
		}
		break;

	case kBlendLeft:
		{
			// Create gradient for blend left...
			RWinColorDlg rColorDlg;
			m_rColorEffect = rColorDlg.ColorToWhite( m_rSolidColorEffect, kLinear, .0, TRUE );
			m_fIsGradient = TRUE;
		}
		break;

	case kRadialBlend:
		{
			// Create gradient for radial burst...
			RWinColorDlg rColorDlg;
			m_rColorEffect = rColorDlg.ColorToWhite( m_rSolidColorEffect, kRadial, .0, FALSE );
			m_fIsGradient = TRUE;
		}
		break;

	case kOutline:
		{
			rNewData.m_eOutlineEffect			= kSimpleStrokeFill;
			rNewData.m_eLineWeight				= kMediumLine;
			rNewData.m_rOutlineFillColor		= m_rColorEffect;
			rNewData.m_rStrokeColor				= RSolidColor( kBlack );
		}												
		break;

	case kGhost:
		{
			rNewData.m_eOutlineEffect			= kHairlineDoubleOutline;
			rNewData.m_eLineWeight				= kMediumLine;
			rNewData.m_rOutlineFillColor		= m_rColorEffect;
			rNewData.m_rStrokeColor				= RSolidColor( kWhite );
			rNewData.m_rShadowColor				= RSolidColor( kBlack );
		}
		break;

	case kNeon:
		{
			rNewData.m_eOutlineEffect			= kDoubleOutline;
			rNewData.m_eLineWeight				= kThickLine;
			rNewData.m_rOutlineFillColor		= m_rColorEffect;
			rNewData.m_rStrokeColor				= RSolidColor( kWhite );
			rNewData.m_rShadowColor				= RSolidColor( kBlack );
		}
		break;

	case kBlur:
		{
			rNewData.m_eOutlineEffect			= kBlurredOutline;
			rNewData.m_rOutlineFillColor		= m_rColorEffect;
			rNewData.m_eLineWeight				= kHairLine;
		}
		break;

	case kDropShadowRight:
		{
			rNewData.m_eOutlineEffect			= kSimpleFill;
			rNewData.m_eShadowEffect			= kDropShadow;
			rNewData.m_rShadowColor				= RSolidColor( kBlack );
			rNewData.m_rShadowVanishPt			= kShadowVanishPtRight;
			rNewData.m_yShadowDepth				= kDefaultShadowDepth;
		}
		break;

	case kDropShadowLeft:
		{
			rNewData.m_eOutlineEffect			= kSimpleFill;
			rNewData.m_eShadowEffect			= kDropShadow;
			rNewData.m_rShadowColor				= RSolidColor( kBlack );
			rNewData.m_rShadowVanishPt			= kShadowVanishPtLeft;
			rNewData.m_yShadowDepth				= kDefaultShadowDepth;
		}
		break;

	case kSoftShadowRight:
		{
			rNewData.m_eOutlineEffect			= kSimpleFill;
			rNewData.m_eShadowEffect			= kSoftDropShadow;
			rNewData.m_rShadowColor				= RSolidColor( kBlack );
			rNewData.m_rShadowVanishPt			= kShadowVanishPtRight;
			rNewData.m_yShadowDepth				= kDefaultShadowDepth;
		}
		break;

	case kSoftShadowLeft:
		{
			rNewData.m_eOutlineEffect			= kSimpleFill;
			rNewData.m_eShadowEffect			= kSoftDropShadow;
			rNewData.m_rShadowColor				= RSolidColor( kBlack );
			rNewData.m_rShadowVanishPt			= kShadowVanishPtLeft;
			rNewData.m_yShadowDepth				= kDefaultShadowDepth;
		}
		break;

	case kStackRight:
		{
			rNewData.m_eProjectionEffect		= kSimpleStack;
			rNewData.m_rProjectionVanishPt	= kProjectionVanishPtRight;
			rNewData.m_yProjectionDepth		= kDefaultProjectionDepth;
		}
		break;

	case kStackLeft:
		{
			rNewData.m_eProjectionEffect		= kSimpleStack;
			rNewData.m_rProjectionVanishPt	= kProjectionVanishPtLeft;
			rNewData.m_yProjectionDepth		= kDefaultProjectionDepth;
		}
		break;

	case kStackBottom:
		{
			rNewData.m_eProjectionEffect		= kSimpleStack;
			rNewData.m_rProjectionVanishPt	= kProjectionVanishPtBottom;
			rNewData.m_yProjectionDepth		= kDefaultProjectionDepth;
		}
		break;

	case kPerspective:
		{
			rNewData.m_eProjectionEffect		= kPseudoPerspectiveStack;
			rNewData.m_rProjectionVanishPt	= kPsuedoPerspectiveVanishPt;
			rNewData.m_yProjectionDepth		= 1;
		}
		break;

	default:
		{

		}
		break;

	}
	m_rHeadlineData = rNewData;

}

// ****************************************************************************
//
//  Function Name:	RCommonGroupedObjectDialog::OnButtonGOEffectColor
//
//  Description:		Displays color dialog for effect color.
//
//  Returns:			void
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RInitCapDialog::OnButtonGOEffectColor()
{
	//
	// Bring up the color dialog, and don't allow gradients
	RWinColorDlg	rColorDlg( NULL, 0 );//kShowTransparent | kShowGradients | kAllowGradients );
	//
	// Set the current effect color into the dialog
	rColorDlg.SetColor( m_rSolidColorEffect );
	//
	// Pop up the color dialog
	if( rColorDlg.DoModal() == IDOK )
	{
		// Get the selected color, and the selected solid color for creating gradients..
		m_rColorEffect		  = rColorDlg.SelectedColor();
		m_rSolidColorEffect = rColorDlg.SelectedSolidColor();
		if( m_fIsGradient )
		{
			//
			// Apply canned headline effect..
			ApplyHeadlineEffect();
		}
		m_cStyleColorBtn.SetColor( m_rColorEffect );
		UpdatePreview( TRUE, FALSE );
	}
}


// ****************************************************************************
//
//  Function Name:	RInitCapDialog::UpdatePreview
//
//  Description:		Something effecting the preview has changed, so get
//							all pertaining data and redraw it.
//
//  Returns:			void
//
//  Exceptions:		None
//
// ****************************************************************************
void RInitCapDialog::UpdatePreview( BOOLEAN fUpdateHeadline, BOOLEAN fUpdateGraphic )
{
	if( m_fInitializing ) return;
	if( fUpdateHeadline )
	{
		// If the headline is nothing, set it to a space because the headline
		// engine rejects empty headlines.
		if( m_rHeadlineText == RMBCString("") )
		{
			m_rHeadlineData.m_rHeadlineText	= " ";
		}
		else
		{
			m_rHeadlineData.m_rHeadlineText	= m_rHeadlineText;
		}
		m_rHeadlineData.m_rFontName			= m_rFontName;
		//
		// If the current selection is a gradient, use the gradient RColor, otherwise
		// use the RSolidColor..
		if( m_fIsGradient )
		{
			m_rHeadlineData.m_rOutlineFillColor	= m_rColorEffect;
		}
		else
		{
			m_rHeadlineData.m_rOutlineFillColor	= m_rSolidColorEffect;
		}
		m_rHeadlineData.m_yFontAttributes	= m_yFontAttributes;
		//
		// Invalidate the render cache to be sure headline gets updated..
		m_pHeadlineView->InvalidateRenderCache();

		m_rHeadlineData.UpdateHeadline( m_pHeadlineInterface );
	}
	if( fUpdateGraphic )
	{
		if( m_pGraphicDoc )
		{
			RComponentView* pGraphicView = m_pGraphicDoc->GetView( m_pControlView );
			RColorSettingsInterface* pGraphicInterface = dynamic_cast<RColorSettingsInterface*>( pGraphicView->GetInterface(kColorSettingsInterfaceId) );
			if( pGraphicInterface )
			{
				pGraphicInterface->SetColor( m_rColorGraphic );
				delete pGraphicInterface;
			}
		}
	}
	//
	// Invalidate the views to show changes...
	TpsAssert( m_pControlDocument, "Null control document pointer" );
	m_pControlDocument->InvalidateAllViews();
}

// ****************************************************************************
//
//  Function Name:	RInitCapDialog::GetComponentType
//
//  Description:		Gets the type of component that this dialog creates and
//							edits
//
//  Returns:			Component type
//
//  Exceptions:		None
//
// ****************************************************************************
//
const YComponentType& RInitCapDialog::GetComponentType( ) const
	{
	static YComponentType componentType = kInitialCapComponent;

	return componentType;
	}

// ****************************************************************************
//
//  Function Name:	RInitCapDialog::WriteUIContextData
//
//  Description:		Writes this dialogs UI context data
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RInitCapDialog::WriteUIContextData( RUIContextData& contextData ) const
{
	// Write common data..
	RCommonGroupedObjectDialog::WriteUIContextData( contextData );

	// Write out the solid color effects
	contextData << m_rSolidColorEffect;

	// REVIEW MWH -this would go in the base method, but it can't
	// because then we wouldn't be backwards compatiable
	// Write out the selected graphics id
	contextData << m_uSelectedGraphicId;

	//	Write extra data at the current end of the context data.
	RCommonGroupedObjectDialog::WriteVersion2UIContextData( contextData );
}

// ****************************************************************************
//
//  Function Name:	RInitCapDialog::ReadUIContextData
//
//  Description:		Read this dialogs UI context data
//
//  Returns:			pointer to ContextData
//
//  Exceptions:		None
//
// ****************************************************************************
//
RUIContextData* RInitCapDialog::ReadUIContextData( ) 
{
	RUIContextData* pContextData = NULL;
	pContextData = RCommonGroupedObjectDialog::ReadUIContextData();
	// Write common data..
	if ( pContextData )
	{
		try
		{
			// Read in the color effects
			(*pContextData) >> m_rSolidColorEffect;

			// REVIEW MWH -this would go in the base method, but it can't
			// because then we wouldn't be backwards compatiable.
			// Read in the selected graphics id
			(*pContextData) >> m_uSelectedGraphicId;

			// Write common data..
			if ( pContextData )
				pContextData 
				 = RCommonGroupedObjectDialog::ReadVersion2UIContextData( *pContextData );
		}
		catch( ... )
		{
			// Catch exceptions thrown be earlier versions that don't support
			// all current UI info.
		}
	}
	return pContextData;
}
