// ****************************************************************************
//
//  File Name:			HeadlinePersistantObject.h
//
//  Project:			Renaissance Headline component
//
//  Author:				M. Houle
//
//  Description:		Declaration of the RHeadlinePersistantObject class
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
//  $Logfile:: /PM8/HeadlineComp/Include/HeadlinePersistantObject.h           $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:18p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#ifndef		_HEADLINEPERSISTANTOBJECT_H_
#define		_HEADLINEPERSISTANTOBJECT_H_

#ifndef		_PERSISTANTOBJECT_H_
#include		"PersistantObject.h"
#endif



//	Structures for containing the Headline Data.  All information that is used by
//		the headline engine to create headlines is stored in these dialogs.

typedef struct InterfaceData
{
	uLONG		ulData ;			

} InterfaceDataStruct ;

typedef struct AttribData
{
	RMBCString										sFontName;				//Default Font name from combo box
	ETextEscapement								eEscapement;			//Default Escapement from combo box

	BOOLEAN											fBoldChecked;			//Bold text?
	BOOLEAN											fItalicChecked;		//Italicized text?
	BOOLEAN											fUnderlineChecked;	//Underlined text?
	BOOLEAN											fNoBackgroundEnvelop;//do not display an empty headline envelop

	ETextJustification							eSelJust;				//Selected Justification
	ETextCompensation								eSelComp;				//Selected Compensation
	float												flCompDegree;			//Compensation Degree

	BOOLEAN											fKerningChecked;		//Kerning on/off
	BOOLEAN											fLeadingChecked;		//Leading on/off

	uWORD												uwScale1;				//Headline Scale 1
	uWORD												uwScale2;				//Headline Scale 2
	uWORD												uwScale3;				//Headline Scale 3

} AttribDataStruct;

typedef struct ShapeData
{
	EDistortEffects								eDistortEffect;		//Selected Shape Type (ie. NonDistort, FollowPath, WarpShape )
	int												nShearAngle;
	int   											nRotation;
	float 											flAltPercent;
	BOOLEAN											fDistort;
	EVerticalPlacement							eVerticalPlacement;
	float											   flMaxPathScale;

	int												nSelShapeIdx;			//One referenced index to default selected shape button.

} ShapeDataStruct;

typedef struct OutlineData
{
	RHeadlineColor									outlineFillDesc;
	EOutlineEffects								eOutlineEffect;		//Selected Outline Type
	ELineWeight										eLineWeight;			//Selected Line Weight
	RSolidColor										colorStroke;			//Stroke color
	RSolidColor										colorShadow;			//Shadow color
	RSolidColor										colorHilite;			//Hilite color
	RSolidColor										colorBlur2;				//blur 2 background color

} OutlineDataStruct;

typedef struct EffectsData
{
	RHeadlineColor									projectionFillDesc;
	EProjectionEffects							eProjectionEffect;
	int												nNumStages;
	int												nProjectionVanishPtX;
	int												nProjectionVanishPtY;
	float												flProjectionDepth;
	BOOLEAN											fProjectionBlend;		//Internal flag for use in the Headline Dialogs - no need to write it out


	RHeadlineColor									shadowFillDesc;
	EShadowEffects									eShadowEffect;
	int												nShadowVanishPtX;   
	int												nShadowVanishPtY;   
	float												flShadowDepth;
	RSolidColor										colorShadow2;			//shadow 2 background color
	BOOLEAN											fShadowBlend;			//Internal flag for use in the Headline Dialogs - no need to write it out

} EffectsDataStruct;

typedef struct BehindEffectsData
{
	RHeadlineColor									behindFillDesc;
	EFramedEffects									eBehindEffect;			//Selected BehindEffects Type
	RSolidColor										colorShadow;			//This field should use/set the Outline colorShadow
	RSolidColor										colorHilite;			//This field should use/set the Outline colorHilite

} BehindEffectsDataStruct;

typedef struct HeadlineData
{
	AttribDataStruct								structAttribData;				//Data for Attribute tab
	ShapeDataStruct								structShapeData;				//Data for Shape Tab
	OutlineDataStruct					         structOutlineData;			//Data for Outline Tab  
	EffectsDataStruct								structEffectsData;			//Data for Effects Tab
	BehindEffectsDataStruct						structBehindEffectsData;	//Data for FrameEffects Tab
	InterfaceDataStruct							structInterfaceData ;		//Data for user interface use

	RMBCString										sHeadlineText;	   //Headline Text

} HeadlineDataStruct;


class		RHeadlineDocument;

// ****************************************************************************
//
//  Class Name:	RHeadlinePersistantObject
//
//  Description:	The dataobject derivative for maintaining the Stonehand
//						data.
//
// ****************************************************************************
//
class RHeadlinePersistantObject : public RPersistantObject
	{
	// Construction & destruction
	public :
													RHeadlinePersistantObject( );
		virtual									~RHeadlinePersistantObject( );

	//	Operations
	public :
		virtual void							PurgeData( );
		virtual void							Read( RStorage& storage );
		static void								ReadAttribData( AttribDataStruct& attribData, RArchive& archive );
		static void								ReadShapeData( ShapeDataStruct& shapeData, RArchive& archive );
		static void								ReadOutlineData( OutlineDataStruct& outlineData, RArchive& archive );
		static void								ReadEffectsData( EffectsDataStruct& effectsData, RArchive& archive );
		static void								ReadBehindData( BehindEffectsDataStruct& behindData, RArchive& archive );
		static void								ReadInterfaceData( InterfaceDataStruct& interfaceData, RArchive& archive ) ;
		virtual void							Write( RStorage& storage ) const;
		static void								WriteAttribData( const AttribDataStruct& attribData, RArchive& archive );
		static void								WriteShapeData( const ShapeDataStruct& shapeData, RArchive& archive );
		static void								WriteOutlineData( const OutlineDataStruct& outlineData, RArchive& archive );
		static void								WriteEffectsData( const EffectsDataStruct& effectsData, RArchive& archive );
		static void								WriteBehindData( const BehindEffectsDataStruct& behindData, RArchive& archive );
		static void								WriteInterfaceData( const InterfaceDataStruct& interfaceData, RArchive& archive ) ;

		void										SetDocument( RHeadlineDocument* pDocument, BOOLEAN fLoading );
		RHeadlineGraphic*						GetGraphic( BOOLEAN fReconstruct = TRUE );

		//	Get & Set Data...
		void										GetHeadlineData( HeadlineDataStruct* pData );
		void										GetAttribData( AttribDataStruct& attribData );
		void										GetShapeData( ShapeDataStruct& shapeData );
		void										GetOutlineData( OutlineDataStruct& outlineData );
		void										GetEffectData( EffectsDataStruct& effectsData );
		void										GetBehindData( BehindEffectsDataStruct& behindData );
		void										GetInterfaceData( InterfaceDataStruct& interfaceData );
		void										GetText( RMBCString& pData );
		void										SetHeadlineData( HeadlineDataStruct* pData );
		void										SetAttribData( const AttribDataStruct& attribData, BOOLEAN fReconstruct );
		void										SetShapeData( const ShapeDataStruct& shapeData, BOOLEAN fReconstruct );
		void										SetOutlineData( const OutlineDataStruct& outlineData, BOOLEAN fReconstruct );
		void										SetEffectData( const EffectsDataStruct& effectsData, BOOLEAN fReconstruct );
		void										SetBehindData( const BehindEffectsDataStruct& behindData, BOOLEAN fReconstruct );
		void										SetInterfaceData( const InterfaceDataStruct& interfaceData, BOOLEAN fReconstruct );
		void										SetText( const RMBCString& pData, BOOLEAN fReconstruct );

		void										QueueReconstruct();
		void										Reconstruct( );

		BOOLEAN									IsStateValid( ) const;
		void										SetStateValid( BOOLEAN fValid );

	//	Private members
	private :
		RHeadlineDocument*					m_pDocument;
		RHeadlineGraphic*						m_pHeadlineGraphic;
		RMBCString								m_HeadlineText;
		InterfaceDataStruct					m_InterfaceData ;
		YFontId									m_FontId;
		uWORD										m_uwWarpPathShape;
		RPath*									m_pWarpPathPath;
		BOOLEAN									m_fReconstructed;
		BOOLEAN									m_fStateValid;
	

#ifdef	TPSDEBUG
	// Debugging stuff
	public :
		virtual void						Validate( ) const;
#endif	// TPSDEBUG
	} ;

// ****************************************************************************
// 					Inlines
// ****************************************************************************

//
// Mark the graphic as needing reconstruction
inline void RHeadlinePersistantObject::QueueReconstruct()
	{
	if( m_pHeadlineGraphic )
		m_pHeadlineGraphic->Redefine();
	m_fReconstructed	= FALSE;
	}

#endif	//	_HEADLINEPERSISTANTOBJECT_H_

