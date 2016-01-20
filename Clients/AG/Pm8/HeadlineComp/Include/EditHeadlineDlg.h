//****************************************************************************
//
// File Name:  EditHeadlineDlg.h
//
// Project:    Renaissance application framework
//
// Author:     Mike Heydlauf
//
// Description: Definition of REditHeadlineDlg class and HeadlineDlgDataStruct
//
// Portability: Windows Specific
//
// Developed by:   Turning Point Software
//				   One Gateway Center, Suite 800
//				   Newton, Ma 02158
//			       (617)332-0202
//
//  Client:			Broderbund Software, Inc.         
//
//  Copyright (C) 1996 Turning Point Software. All Rights Reserved.
//
//  $Logfile:: /PM8/HeadlineComp/Include/EditHeadlineDlg.h                    $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:18p                                                  $
// $Revision:: 1                                                              $
//
//****************************************************************************
#ifndef _EDITHEADLINEDLG_H_
#define _EDITHEADLINEDLG_H_

#ifdef	TEST_UI
#include "Resource.h"
#include "TPSDefs.h"
#else
#include "HeadlineCompResource.h"
#endif

#ifndef	_DIALOGUTILITIES_H_
#include "DialogUtilities.h"
#endif

#ifndef	_ATTRIBTAB_H_
#include "AttribTab.h"
#endif

#ifndef	_SHAPETAB_H_
#include "ShapeTab.h"
#endif

#ifndef _OUTLINETAB_H_
#include "OutlineTab.h"
#endif

#ifndef _EFFECTSTAB_H_
#include "EffectsTab.h"
#endif

#ifndef _BEHINDEFFECTSTAB_H_
#include "BehindEffectsTab.h"
#endif

#ifndef	_HEADLINESHEET_H_
#include "HeadlineSheet.h"
#endif

class RHeadlinePersistantObject;
class REditHeadlineDlg 
{
// Construction
public:
													REditHeadlineDlg();
													REditHeadlineDlg( HeadlineDataStruct* pHlData , RHeadlinePersistantObject* pHLPersistantObj );
	virtual										~REditHeadlineDlg();
	//
   //Need to declare at least one page as public, so the Apply button
	//can be enabled from HeadlineSheet.cpp
	CAttribTab									m_AttribPage;
// Attributes
public:
	BOOLEAN										DoModal();
	void											FillData( HeadlineDataStruct* pHlData );
   void											ApplyFollowPathModificationsToAttribTab( BOOLEAN fApplying, int m_nSelShapeId );
	void											UpdateHeadlinePreview();
	enum ESelectedTab							{ kAttribTab, kShapeTab, kOutlineTab, kEffectsTab, kBehindEffectsTab };
	void											UpdateTabData( ESelectedTab eSelectedTab );

protected:
	CHeadlineSheet								m_HeadlineSheet;
	CShapeTab									m_ShapePage; 
	COutlineTab									m_OutlinePage;
	CEffectsTab									m_EffectsPage;
	CBehindEffectsTab							m_BehindEffectsPage;

	RHeadlinePersistantObject*				m_pHLPersistantObj;
	HeadlineDataStruct						m_HlData;

	//ESelectedTab								m_eSelectedTab;
};

#endif //_EDITHEADLINEDLG_H_

