//****************************************************************************
//
// File Name:  EditHeadlineDlg.cpp
//
// Project:    Renaissance application framework
//
// Author:     Mike Heydlauf
//
// Description: Defines class REditHeadlineDlg used to provide a simple
//				programmer's interface to the Headline UI.
//
// Portability: Windows Specific
//
// Developed by:   Turning Point Software
//						 One Gateway Center, Suite 800
//						 Newton, Ma 02158
//						 (617)332-0202
//
//  Client:			Broderbund Software, Inc.         
//
//  Copyright (C) 1996 Turning Point Software. All Rights Reserved.
//
//  $Logfile:: /PM8/HeadlineComp/Source/EditHeadlineDlg.cpp                   $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:18p                                                  $
// $Revision:: 1                                                              $
//
//****************************************************************************


#include "HeadlineIncludes.h"
ASSERTNAME

#ifndef	WIN
	#error	This file must be compilied only on Windows
#endif	//	WIN

#include "EditHeadlineDlg.h"

#include "HeadlinesCanned.h"
//#include "HeadlineSheet.h"

//****************************************************************************
//
// Function Name: REditHeadlineDlg
//
// Description:   Constructor that accepts a pointer to a HeadlineDataStruct	
//                used for dialog initialization 
//
// Returns:       Nothing 
//
// Exceptions:	  None
//
//****************************************************************************
REditHeadlineDlg::REditHeadlineDlg( HeadlineDataStruct* pHlData, RHeadlinePersistantObject* pHLPersistantObj )
	: m_HeadlineSheet( &(pHlData->sHeadlineText.operator CString( ) ) ),
	  m_AttribPage( &pHlData->structAttribData ),
	  m_ShapePage( &pHlData->structShapeData ),
	  m_OutlinePage( &pHlData->structOutlineData ),
	  m_EffectsPage( &pHlData->structEffectsData ),
	  m_BehindEffectsPage( &pHlData->structBehindEffectsData ),
	  m_HlData( *pHlData ),
	  m_pHLPersistantObj( pHLPersistantObj )
{
	m_HeadlineSheet.SetPointerToOwner( this );
	m_AttribPage.SetPointerToOwner( this );
	m_ShapePage.SetPointerToOwner( this );
	m_OutlinePage.SetPointerToOwner( this );
	m_EffectsPage.SetPointerToOwner( this );
	m_BehindEffectsPage.SetPointerToOwner( this );
}


//****************************************************************************
//
// Function Name: REditHeadlineDlg::~REditHeadlineDlg
//
// Description:   Destructor 
//
// Returns:       Nothing 
//
// Exceptions:	  None
//
//****************************************************************************
REditHeadlineDlg::~REditHeadlineDlg()
{
	NULL;
}

//****************************************************************************
//
// Function Name: DoModal
//
// Description:   Programmer front end for drawing the Headline dialog
//
// Returns:       TRUE if user pressed OK
//
// Exceptions:    None 
//
//****************************************************************************
BOOLEAN REditHeadlineDlg::DoModal()
{
#ifdef	WIN
	int nOkorCancel;
	m_HeadlineSheet.AddPage( &m_AttribPage );
	m_HeadlineSheet.AddPage( &m_ShapePage );
	m_HeadlineSheet.AddPage( &m_OutlinePage );
	m_HeadlineSheet.AddPage( &m_EffectsPage );
	m_HeadlineSheet.AddPage( &m_BehindEffectsPage );

	nOkorCancel = m_HeadlineSheet.DoModal() ;
	if ( nOkorCancel == IDOK )
	{
		CString	string;
		m_AttribPage.FillData( &m_HlData.structAttribData );
		m_ShapePage.FillData( &m_HlData.structShapeData );
		m_OutlinePage.FillData( &m_HlData.structOutlineData );
		m_EffectsPage.FillData( &m_HlData.structEffectsData );
		m_BehindEffectsPage.FillData( &m_HlData.structBehindEffectsData );
		m_HeadlineSheet.FillData( &string );
		m_HlData.sHeadlineText	= string;
	}
	return( static_cast<BOOLEAN>( nOkorCancel == IDOK ) );
#else
	return FALSE;
#endif
}

////////////////////////////////////////////////////////////////////////////////////
//Function: ApplyFollowPathModificationsToAttribTab
//
//Description: Called from Shape Tab when follow path is selected/deselected.  
//					Passes info on to	Attrib tab so controls can be set up properly.
//				   
//Paramaters: fApplying -indicates whether follow path was selected(TRUE) or deselected(FALSE).
//				  m_nSelShapeId - the selected follow path shape.
//
//Returns:    VOID
//
////////////////////////////////////////////////////////////////////////////////////
void REditHeadlineDlg::ApplyFollowPathModificationsToAttribTab( BOOLEAN fApplying, 	int m_nSelShapeId)
{
	m_AttribPage.ApplyFollowPathModifications( fApplying, m_nSelShapeId );
}

void REditHeadlineDlg::UpdateHeadlinePreview()
{
	HeadlineDataStruct HlData;
	CString	string;
	m_AttribPage.FillData( &HlData.structAttribData );
	m_ShapePage.FillData( &HlData.structShapeData );
	m_OutlinePage.FillData( &HlData.structOutlineData );
	m_EffectsPage.FillData( &HlData.structEffectsData );
	m_BehindEffectsPage.FillData( &HlData.structBehindEffectsData );
	m_HeadlineSheet.FillData( &string );
	HlData.sHeadlineText	= string;

	m_HeadlineSheet.UpdatePreviewHeadline( &HlData );

}

void REditHeadlineDlg::UpdateTabData( ESelectedTab eSelectedTab )
{
	HeadlineDataStruct HlData;
	CString	string;
	m_HeadlineSheet.FillData( &string );
	//HlData.sHeadlineText	= string;
	m_pHLPersistantObj->SetText( RMBCString( string ), TRUE );//HlData.sHeadlineText, TRUE );

	switch( eSelectedTab )
	{
		case kAttribTab:
			{
				m_AttribPage.FillData( &HlData.structAttribData );
				m_pHLPersistantObj->SetAttribData( HlData.structAttribData, TRUE );
			}
			break;

		case kShapeTab:
			{
				m_ShapePage.FillData( &HlData.structShapeData );
				m_pHLPersistantObj->SetShapeData( HlData.structShapeData, TRUE );

			}
			break;

		case kOutlineTab:
			{
				m_OutlinePage.FillData( &HlData.structOutlineData );
				m_pHLPersistantObj->SetOutlineData( HlData.structOutlineData, TRUE );

			}
			break;

		case kEffectsTab:
			{
				m_EffectsPage.FillData( &HlData.structEffectsData );
				m_pHLPersistantObj->SetEffectData( HlData.structEffectsData, TRUE );

			}
			break;

		case kBehindEffectsTab:
			{
				m_BehindEffectsPage.FillData( &HlData.structBehindEffectsData );
				m_pHLPersistantObj->SetBehindData( HlData.structBehindEffectsData, TRUE );

			}
			break;

		default:
			{
				TpsAssertAlways( "Unknown Headline Tab encountered" );
			}
			break;

	}
	//
	// CompDegree is not persistant in Headline engine, so I need to preserve it manually...
	float flTempDegree = HlData.structAttribData.flCompDegree;
	m_pHLPersistantObj->GetAttribData( HlData.structAttribData );
	HlData.structAttribData.flCompDegree = flTempDegree;

	m_pHLPersistantObj->GetShapeData( HlData.structShapeData );
	m_pHLPersistantObj->GetOutlineData( HlData.structOutlineData );
	m_pHLPersistantObj->GetEffectData( HlData.structEffectsData );
	m_pHLPersistantObj->GetBehindData( HlData.structBehindEffectsData );

	m_AttribPage.SetData( &HlData.structAttribData );
	m_ShapePage.SetData( &HlData.structShapeData );
	m_OutlinePage.SetData( &HlData.structOutlineData );
	m_EffectsPage.SetData( &HlData.structEffectsData );
	m_BehindEffectsPage.SetData( &HlData.structBehindEffectsData );
}


//****************************************************************************
//
// Function Name: FillData
//
// Description:   Fills a HeadlineDataStruct	 with the Headline Dlg's data
//
// Returns:       VOID
//
// Exceptions:	  None
//
//****************************************************************************
void REditHeadlineDlg::FillData( HeadlineDataStruct* pHlData )
{
	*pHlData = m_HlData;
}