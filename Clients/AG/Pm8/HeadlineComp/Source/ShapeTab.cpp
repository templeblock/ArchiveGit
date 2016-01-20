//****************************************************************************
//
// File Name: ShapeTab.cpp
//
// Project:    Renaissance application framework
//
// Author:     Mike Heydlauf
//
// Description: Manages CShapeTab class which is used for the Shape 
//				Property Page
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
//  $Logfile:: /PM8/HeadlineComp/Source/ShapeTab.cpp                          $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:18p                                                  $
// $Revision:: 1                                                              $
//
//****************************************************************************

#include "HeadlineIncludes.h"
ASSERTNAME

#ifndef	WIN
	#error	This file can only be compilied on Windows
#endif

//#include "ShapeTab.h"
#include "EditHeadlineDlg.h"

#include "HeadlinesCanned.h"

/////////////////////////////////////////////////////////////////////////////
// CShapeTab property page

IMPLEMENT_DYNCREATE(CShapeTab, CPropertyPage)

CShapeTab::CShapeTab() : CPropertyPage(CShapeTab::IDD)
{
	//{{AFX_DATA_INIT(CShapeTab)
	//}}AFX_DATA_INIT

	m_eDistortEffect	= kFollowPath;
	m_nSelShapeId		= kNoWarpShape;
}

CShapeTab::CShapeTab( ShapeDataStruct* pShapeData ) : CPropertyPage(CShapeTab::IDD)
{
	SetData( pShapeData );
}

void CShapeTab::SetData( ShapeDataStruct* pShapeData )
{
	m_eDistortEffect	= pShapeData->eDistortEffect;
	if( m_eDistortEffect == kNonDistort )
	{
		m_nSelShapeId		= kWarpPath10;
	}

	m_fNonDistort		= pShapeData->fDistort;
	m_nSelShapeId		= pShapeData->nSelShapeIdx;
	m_nShearAngle     = pShapeData->nShearAngle;    
	m_nRotation			= pShapeData->nRotation;
	m_nAltPercent     = ::Round((pShapeData->flAltPercent)*100);   
}

CShapeTab::~CShapeTab()
{
}

void CShapeTab::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CShapeTab)
	DDX_Control(pDX, IDC_BTN_SELECTED_BMP, m_ctrlSelShapeBmp);
	DDX_Control(pDX, IDC_BTN_SHAPE9, m_btnShape9);
	DDX_Control(pDX, IDC_BTN_SHAPE8, m_btnShape8);
	DDX_Control(pDX, IDC_BTN_SHAPE7, m_btnShape7);
	DDX_Control(pDX, IDC_BTN_SHAPE6, m_btnShape6);
	DDX_Control(pDX, IDC_BTN_SHAPE5, m_btnShape5);
	DDX_Control(pDX, IDC_BTN_SHAPE4, m_btnShape4);
	DDX_Control(pDX, IDC_BTN_SHAPE3, m_btnShape3);
	DDX_Control(pDX, IDC_BTN_SHAPE2, m_btnShape2);
	DDX_Control(pDX, IDC_BTN_SHAPE15, m_btnShape15);
	DDX_Control(pDX, IDC_BTN_SHAPE14, m_btnShape14);
	DDX_Control(pDX, IDC_BTN_SHAPE13, m_btnShape13);
	DDX_Control(pDX, IDC_BTN_SHAPE12, m_btnShape12);
	DDX_Control(pDX, IDC_BTN_SHAPE11, m_btnShape11);
	DDX_Control(pDX, IDC_BTN_SHAPE10, m_btnShape10);
	DDX_Control(pDX, IDC_BTN_SHAPE1, m_btnShape1);
	DDX_Check(pDX, IDC_CB_NON_DISTORT, m_fNonDistort);
	//}}AFX_DATA_MAP

	//
	//REVIEW -Mike Heydlauf -This whole validation method could stand vast improvement
	//			 and when its not throw away, I'll improve it.
	CString sGetInput;
	//
	//Take care of Shear validation (if neccessary)...
	DDX_Control(pDX, CONTROL_EDIT_SHAPE_SHEAR_ANGLE, m_ctrlShearAngle);
	sGetInput.Format( "%d", m_nShearAngle );
  	DDX_Text(pDX, CONTROL_EDIT_SHAPE_SHEAR_ANGLE, sGetInput );//m_nShearAngle);
	//
	//Check to see if field is blank.  If it is, force failure.
	if  ( sGetInput == "" ) sGetInput = "fail";
	m_nShearAngle = atoi( sGetInput );
	if ( m_nShearAngle == 0 && sGetInput.GetAt(0) != '0' )
	{
		//
		//Invalid number entered, so I force DDV to fail...
		DDV_MinMaxInt(pDX, kShearAngleMin - 1, kShearAngleMin, kShearAngleMax);
	}
	else
	{
		DDV_MinMaxInt(pDX, m_nShearAngle, kShearAngleMin, kShearAngleMax);
	}
	//
	//Take care of Rotation validation (if neccessary)...
	DDX_Control(pDX, CONTROL_EDIT_SHAPE_ROTATION, m_ctrlRotation);
	sGetInput.Format( "%d", m_nRotation );
  	DDX_Text(pDX, CONTROL_EDIT_SHAPE_ROTATION, sGetInput );//m_nRotation);
	//
	//Check to see if field is blank.  If it is, force failure.
	if  ( sGetInput == "" ) sGetInput = "fail";
	m_nRotation = atoi( sGetInput );
	if ( m_nRotation == 0 && sGetInput.GetAt(0) != '0' )
	{
		//
		//Invalid number entered, so I force DDV to fail...
		DDV_MinMaxInt(pDX, kRotationMin - 1, kRotationMin, kRotationMax);
	}
	else
	{
		DDV_MinMaxInt(pDX, m_nRotation, kRotationMin, kRotationMax);
	}
	//
	//Take care of Alternate Baseline validation (if neccessary)...
	DDX_Control(pDX, CONTROL_EDIT_SHAPE_ALT_PERCENT, m_ctrlAltPercent);
  	DDX_Text(pDX, CONTROL_EDIT_SHAPE_ALT_PERCENT, m_nAltPercent);
	DDV_MinMaxInt(pDX, m_nAltPercent, kAltPercentMin, kAltPercentMax);

	DDX_Control(pDX, IDC_CB_NON_DISTORT, m_ctrlNonDistort);
	//
	//Attach member variable to Radio Button...
	int	nConvertShapeType = int(m_eDistortEffect);
	DDX_Radio( pDX, IDC_RD_NON_DISTORT, nConvertShapeType );
	m_eDistortEffect = EDistortEffects(nConvertShapeType);
}								

BEGIN_MESSAGE_MAP(CShapeTab, CPropertyPage)
	//{{AFX_MSG_MAP(CShapeTab)
	ON_BN_CLICKED(IDC_RD_FOLLOW_PATH, OnRdFollowPath)
	ON_BN_CLICKED(IDC_RD_WARP_SHAPE, OnRdWarpShape)
	ON_BN_CLICKED(IDC_BTN_SHAPE1, OnBtnShape1)
	ON_BN_CLICKED(IDC_BTN_SHAPE10, OnBtnShape10)
	ON_BN_CLICKED(IDC_BTN_SHAPE11, OnBtnShape11)
	ON_BN_CLICKED(IDC_BTN_SHAPE12, OnBtnShape12)
	ON_BN_CLICKED(IDC_BTN_SHAPE13, OnBtnShape13)
	ON_BN_CLICKED(IDC_BTN_SHAPE14, OnBtnShape14)
	ON_BN_CLICKED(IDC_BTN_SHAPE15, OnBtnShape15)
	ON_BN_CLICKED(IDC_BTN_SHAPE2, OnBtnShape2)
	ON_BN_CLICKED(IDC_BTN_SHAPE3, OnBtnShape3)
	ON_BN_CLICKED(IDC_BTN_SHAPE4, OnBtnShape4)
	ON_BN_CLICKED(IDC_BTN_SHAPE5, OnBtnShape5)
	ON_BN_CLICKED(IDC_BTN_SHAPE6, OnBtnShape6)
	ON_BN_CLICKED(IDC_BTN_SHAPE7, OnBtnShape7)
	ON_BN_CLICKED(IDC_BTN_SHAPE8, OnBtnShape8)
	ON_BN_CLICKED(IDC_BTN_SHAPE9, OnBtnShape9)
	ON_BN_CLICKED(IDC_RD_SHEAR, OnRdShear)
	ON_BN_CLICKED(IDC_RD_ALT_BASELINE, OnRdAltBaseline)
	ON_BN_CLICKED(IDC_RD_ALT_SHEAR, OnRdAltShearRotate)
	ON_BN_CLICKED(IDC_RD_ALT_SHEAR_SHEAR, OnRdAltShearShear)
	ON_BN_CLICKED(IDC_RD_NON_DISTORT, OnRdNonDistort)
	ON_BN_CLICKED(IDC_CB_NON_DISTORT, OnCbNonDistort)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CShapeTab message handlers


BOOL CShapeTab::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	//
	//Make sure the data coming in is valid...
	ASSERT( m_nShearAngle >= kShearAngleMin && m_nShearAngle <= kShearAngleMax );
	ASSERT( m_nRotation >= kRotationMin && m_nRotation <=	kRotationMax );
	ASSERT( m_nAltPercent >= kAltPercentMin && m_nAltPercent <= kAltPercentMax );

	HideShapeButtons();

	LoadShapeButtonBitmaps( m_eDistortEffect );
	ShowButtons( m_eDistortEffect );
	ShowAngleEditControls();

	if ( m_eDistortEffect == kNonDistort || 
		  m_eDistortEffect == kAltBaseline || 
		  m_eDistortEffect == kShear || 
		  m_eDistortEffect == kAltShearRotate ||
		  m_eDistortEffect == kAltShear )
	{
		m_ctrlSelShapeBmp.ShowWindow( SW_HIDE );	
	}
	else
	{
		LoadSelectedShapeBmp( m_nSelShapeId );
	}

	if ( m_eDistortEffect == kFollowPath )
	{
		// Disable non distort check box, but mark it checked
		m_fNonDistort = TRUE;
		m_ctrlNonDistort.SetCheck( TRUE );
		m_ctrlNonDistort.EnableWindow( FALSE );
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

////////////////////////////////////////////////////////////////////////////////////
//Function: ShowAngleEditControls
//
//Description: Switches on the distort effect and hides or shows the appropriate 
//					angle edit controls for that effect.
//
//Paramaters:  None
//
//Returns:	   VOID
//
////////////////////////////////////////////////////////////////////////////////////
void CShapeTab::ShowAngleEditControls()
{
	//
	//Start by hiding all the edit controls, and then only showing them if necessary.
	m_ctrlShearAngle.ShowWindow( SW_HIDE );
	m_ctrlRotation.ShowWindow( SW_HIDE );
	m_ctrlAltPercent.ShowWindow( SW_HIDE );

	switch( m_eDistortEffect )
	{
	case kNonDistort:
		break;

	case kFollowPath:
		break;

	case kWarpToEnvelop:
		break;

	case kAltBaseline:
		{
			m_ctrlAltPercent.ShowWindow( SW_SHOW );
		}
		break;

	case kShear:
		{
			m_ctrlShearAngle.ShowWindow( SW_SHOW );
		}
		break;

	case kAltShearRotate:
		{
			m_ctrlRotation.ShowWindow( SW_SHOW );
			m_ctrlShearAngle.ShowWindow( SW_SHOW );
		}
		break;

	case kAltShear:
		{
			m_ctrlShearAngle.ShowWindow( SW_SHOW );
		}
		break;

	default:
		ASSERT(FALSE);
		break;
	}
}
////////////////////////////////////////////////////////////////////////////////////
//Function: LoadSelectedShapeBmp
//
//Description: Loads the specified bitmap into the Shape Property Page's current
//			   selection bitmap button.
//
//Paramaters:  nSelShapeIdx -0 based index to the button whose shape is being
//						     loaded into the static control
//
//Returns:	   VOID
//
////////////////////////////////////////////////////////////////////////////////////
void CShapeTab::LoadSelectedShapeBmp( int nSelShapeId )
{
	m_nSelShapeId	= nSelShapeId;

	m_ctrlSelShapeBmp.Invalidate();

	switch( nSelShapeId )
	{
	case kWarpPath10 :
		if( m_eDistortEffect == kFollowPath )
		{
			m_ctrlSelShapeBmp.LoadBitmaps( kFollowBtn1Bmp );
			m_pParentPropSheet->ApplyFollowPathModificationsToAttribTab( TRUE, nSelShapeId );
		}
		else
		{
			ASSERT( m_eDistortEffect == kWarpToEnvelop );
			m_ctrlSelShapeBmp.LoadBitmaps( kWarpBtn8Bmp );
		}
		break;

	case kWarpPath20 :
		ASSERT( m_eDistortEffect == kFollowPath );
		m_ctrlSelShapeBmp.LoadBitmaps( kFollowBtn2Bmp );
		m_pParentPropSheet->ApplyFollowPathModificationsToAttribTab( TRUE, nSelShapeId );
		break;

	case kWarpPath30 :
		ASSERT( m_eDistortEffect == kFollowPath );
		m_ctrlSelShapeBmp.LoadBitmaps( kFollowBtn3Bmp );
		m_pParentPropSheet->ApplyFollowPathModificationsToAttribTab( TRUE, nSelShapeId );
		break;

	case kWarpPath40 :
		ASSERT( m_eDistortEffect == kFollowPath );
		m_ctrlSelShapeBmp.LoadBitmaps( kFollowBtn4Bmp );
		m_pParentPropSheet->ApplyFollowPathModificationsToAttribTab( TRUE, nSelShapeId );
		break;

	case kWarpPath50 :
		ASSERT( m_eDistortEffect == kFollowPath );
		m_ctrlSelShapeBmp.LoadBitmaps( kFollowBtn4Bmp );
		m_pParentPropSheet->ApplyFollowPathModificationsToAttribTab( TRUE, nSelShapeId );
		break;

	case kWarpPath60 :
		ASSERT( m_eDistortEffect == kFollowPath );
		m_ctrlSelShapeBmp.LoadBitmaps( kFollowBtn4Bmp );
		m_pParentPropSheet->ApplyFollowPathModificationsToAttribTab( TRUE, nSelShapeId );
		break;

	case kWarpPath70 :
		ASSERT( m_eDistortEffect == kFollowPath );
		m_ctrlSelShapeBmp.LoadBitmaps( kFollowBtn4Bmp );
		m_pParentPropSheet->ApplyFollowPathModificationsToAttribTab( TRUE, nSelShapeId );
		break;

	case kWarpPath80 :
		ASSERT( m_eDistortEffect == kFollowPath );
		m_ctrlSelShapeBmp.LoadBitmaps( kFollowBtn4Bmp );
		m_pParentPropSheet->ApplyFollowPathModificationsToAttribTab( TRUE, nSelShapeId );
		break;

	case kWarpPath90 :
		ASSERT( m_eDistortEffect == kFollowPath );
		m_ctrlSelShapeBmp.LoadBitmaps( kFollowBtn4Bmp );
		m_pParentPropSheet->ApplyFollowPathModificationsToAttribTab( TRUE, nSelShapeId );
		break;

	case kWarpPath100:
		ASSERT( m_eDistortEffect == kFollowPath );
		m_ctrlSelShapeBmp.LoadBitmaps( kFollowBtn4Bmp );
		m_pParentPropSheet->ApplyFollowPathModificationsToAttribTab( TRUE, nSelShapeId );
		break;

	case kWarpPath110:
		ASSERT( m_eDistortEffect == kFollowPath );
		m_ctrlSelShapeBmp.LoadBitmaps( kFollowBtn4Bmp );
		m_pParentPropSheet->ApplyFollowPathModificationsToAttribTab( TRUE, nSelShapeId );
		break;

	case kNoWarpShape :		//	No Warp - Rectangle
		ASSERT( m_eDistortEffect == kWarpToEnvelop );
		m_ctrlSelShapeBmp.LoadBitmaps( kWarpBtn1Bmp );
		break;

	case kWarpShape1 :		//	Oval
		ASSERT( m_eDistortEffect == kWarpToEnvelop );
		m_ctrlSelShapeBmp.LoadBitmaps( kWarpBtn6Bmp );
		break;

	case kWarpShape2 :		//	Half oval (upper)
		ASSERT( m_eDistortEffect == kWarpToEnvelop );
		m_ctrlSelShapeBmp.LoadBitmaps( kWarpBtn3Bmp );
		break;

	case kWarpShape3 :		//	Half oval (lower)
		ASSERT( m_eDistortEffect == kWarpToEnvelop );
		m_ctrlSelShapeBmp.LoadBitmaps( kWarpBtn8Bmp );
		break;

	case kWarpShape4 :		//	trapazoid (wide bottom)
		ASSERT( m_eDistortEffect == kWarpToEnvelop );
		m_ctrlSelShapeBmp.LoadBitmaps( kWarpBtn13Bmp );
		break;

	case kWarpShape5 :		//	trapazoid (wide left)
		ASSERT( m_eDistortEffect == kWarpToEnvelop );
		m_ctrlSelShapeBmp.LoadBitmaps( kWarpBtn2Bmp );
		break;

	case kWarpShape6 :		//	trapazoid (wide right)
		ASSERT( m_eDistortEffect == kWarpToEnvelop );
		m_ctrlSelShapeBmp.LoadBitmaps( kWarpBtn7Bmp );
		break;

	case kWarpShape7 :		//	trapazoid (wide top)
		ASSERT( m_eDistortEffect == kWarpToEnvelop );
		m_ctrlSelShapeBmp.LoadBitmaps( kWarpBtn12Bmp );
		break;

	case kWarpShape8 :		//	tall rect
		ASSERT( m_eDistortEffect == kWarpToEnvelop );
		m_ctrlSelShapeBmp.LoadBitmaps( kWarpBtn11Bmp );
		break;

	case kWarpShape9 :		//	slant Right
		ASSERT( m_eDistortEffect == kWarpToEnvelop );
		m_ctrlSelShapeBmp.LoadBitmaps( kWarpBtn5Bmp );
		break;

	case kWarpShape10 :		//	slant Left
		ASSERT( m_eDistortEffect == kWarpToEnvelop );
		m_ctrlSelShapeBmp.LoadBitmaps( kWarpBtn4Bmp );
		break;

	case kWarpShape11 :		//	Diamond
		ASSERT( m_eDistortEffect == kWarpToEnvelop );
		m_ctrlSelShapeBmp.LoadBitmaps( kWarpBtn9Bmp );
		break;

	case kWarpShape12 :		//	Triangle
		ASSERT( m_eDistortEffect == kWarpToEnvelop );
		m_ctrlSelShapeBmp.LoadBitmaps( kWarpBtn10Bmp );
		break;

	default:
		ASSERT( FALSE );
		break;
	}
m_ctrlSelShapeBmp.ShowWindow( SW_SHOW );
}
////////////////////////////////////////////////////////////////////////////////////
//Function: HideShapeButtons
//
//Description: Hides all the buttons on the Shape Page
//
//Paramaters:  VOID
//
//Returns:	   VOID
//
////////////////////////////////////////////////////////////////////////////////////
void CShapeTab::HideShapeButtons() 
{
 	m_btnShape1.ShowWindow( SW_HIDE );
	m_btnShape2.ShowWindow( SW_HIDE );
	m_btnShape3.ShowWindow( SW_HIDE );
	m_btnShape4.ShowWindow( SW_HIDE );
	m_btnShape5.ShowWindow( SW_HIDE );
	m_btnShape6.ShowWindow( SW_HIDE );
	m_btnShape7.ShowWindow( SW_HIDE );
	m_btnShape8.ShowWindow( SW_HIDE );
	m_btnShape9.ShowWindow( SW_HIDE );
	m_btnShape10.ShowWindow( SW_HIDE );
	m_btnShape11.ShowWindow( SW_HIDE );
	m_btnShape12.ShowWindow( SW_HIDE );
	m_btnShape13.ShowWindow( SW_HIDE );
	m_btnShape14.ShowWindow( SW_HIDE );
	m_btnShape15.ShowWindow( SW_HIDE );

}

void CShapeTab::OnCbNonDistort() 
{
	m_fNonDistort = !m_fNonDistort;
	SetModified( TRUE );
}

void CShapeTab::OnRdNonDistort() 
{
	m_ctrlNonDistort.EnableWindow( TRUE );
	HideShapeButtons();
	m_ctrlSelShapeBmp.ShowWindow( SW_HIDE );
	SetModified( TRUE );
}

void CShapeTab::OnRdFollowPath() 
{
	if ( m_eDistortEffect != kFollowPath )
	{
		m_pParentPropSheet->ApplyFollowPathModificationsToAttribTab( TRUE, kWarpPath10 );
		HideShapeButtons();
		LoadShapeButtonBitmaps( kFollowPath );
		ShowButtons( kFollowPath );
		LoadSelectedShapeBmp( kWarpPath10 );

		SetModified( TRUE );
	}

	// Disable non distort check box, but mark it checked
	m_fNonDistort = TRUE;
	m_ctrlNonDistort.SetCheck( TRUE );
	m_ctrlNonDistort.EnableWindow( FALSE );
	
	//
	//Hide/Show the appropriate angle edit controls.  
	m_ctrlShearAngle.ShowWindow( SW_HIDE );
	m_ctrlRotation.ShowWindow( SW_HIDE );
	m_ctrlAltPercent.ShowWindow( SW_HIDE );
	//
	//Reset the Angle Values
	m_nShearAngle = kDefaultShearAngle;
	m_nRotation = kDefaultRotation;
	m_nAltPercent = kDefaultAltPercent;

	UpdateData( FALSE );
}

void CShapeTab::OnRdWarpShape() 
{
	if ( m_eDistortEffect != kWarpToEnvelop )
	{
		HideShapeButtons();
		LoadShapeButtonBitmaps( kWarpToEnvelop );
		ShowButtons( kWarpToEnvelop );
		LoadSelectedShapeBmp( kNoWarpShape );
		m_pParentPropSheet->ApplyFollowPathModificationsToAttribTab( FALSE, 0 );
		SetModified( TRUE );
	}

	// Disable non distort check box, but mark it unchecked
	m_fNonDistort = FALSE;
	m_ctrlNonDistort.SetCheck( FALSE );
	m_ctrlNonDistort.EnableWindow( FALSE );

	//
	//Hide/Show the appropriate angle edit controls.  
	m_ctrlShearAngle.ShowWindow( SW_HIDE );
	m_ctrlRotation.ShowWindow( SW_HIDE );
	m_ctrlAltPercent.ShowWindow( SW_HIDE );
	//
	//Reset the Angle Values
	m_nShearAngle = kDefaultShearAngle;
	m_nRotation = kDefaultRotation;
	m_nAltPercent = kDefaultAltPercent;
	UpdateData( FALSE );
}

void CShapeTab::OnRdShear() 
{
	if ( m_eDistortEffect != kShear )
	{
		HideShapeButtons();
		LoadShapeButtonBitmaps( kShear  );
		ShowButtons( kShear  );
		m_ctrlSelShapeBmp.ShowWindow( SW_HIDE );
		m_pParentPropSheet->ApplyFollowPathModificationsToAttribTab( FALSE, 0 );
		SetModified( TRUE );

	}

	// Enable non distort check box
	m_ctrlNonDistort.EnableWindow( TRUE );
	
	//
	//Hide/Show the appropriate angle edit controls.  
	m_ctrlShearAngle.ShowWindow( SW_SHOW );
	m_ctrlRotation.ShowWindow( SW_HIDE );
	m_ctrlAltPercent.ShowWindow( SW_HIDE );
	//
	//Reset the Angle Values
	//m_nShearAngle = kDefaultShearAngle;
	m_nShearAngle = kDefaultShearAngle;
	m_nRotation = kDefaultRotation;
	m_nAltPercent = kDefaultAltPercent;
	UpdateData( FALSE );
}

void CShapeTab::OnRdAltBaseline() 
{
	if ( m_eDistortEffect != kAltBaseline )
	{
		HideShapeButtons();
		LoadShapeButtonBitmaps( kAltBaseline  );
		ShowButtons( kAltBaseline  );
		m_ctrlSelShapeBmp.ShowWindow( SW_HIDE );
		m_pParentPropSheet->ApplyFollowPathModificationsToAttribTab( FALSE, m_nSelShapeId );
		SetModified( TRUE );

	}

	// Enable non distort check box
	m_ctrlNonDistort.EnableWindow( TRUE );
	
	//
	//Hide/Show the appropriate angle edit controls
	m_ctrlShearAngle.ShowWindow( SW_HIDE );
	m_ctrlRotation.ShowWindow( SW_HIDE );
	m_ctrlAltPercent.ShowWindow( SW_SHOW );
	//
	//Reset the Angle Values
	m_nShearAngle = kDefaultShearAngle;
	m_nRotation = kDefaultRotation;
	//m_nAltPercent = kDefaultAltPercent;
	UpdateData( FALSE );

}

void CShapeTab::OnRdAltShearRotate() 
{
	if ( m_eDistortEffect != kAltShearRotate )
	{
		HideShapeButtons();
		LoadShapeButtonBitmaps( kAltShearRotate  );
		ShowButtons( kAltShearRotate  );
		m_ctrlSelShapeBmp.ShowWindow( SW_HIDE );
		m_pParentPropSheet->ApplyFollowPathModificationsToAttribTab( FALSE, m_nSelShapeId );
		SetModified( TRUE );

	}

	// Enable non distort check box
	m_ctrlNonDistort.EnableWindow( TRUE );
	
	//
	//Hide/Show the appropriate angle edit controls
	m_ctrlShearAngle.ShowWindow( SW_SHOW );
	m_ctrlRotation.ShowWindow( SW_SHOW );
	m_ctrlAltPercent.ShowWindow( SW_HIDE );
	//
	//Reset the Angle Values
	m_nShearAngle = kDefaultAltShearAngle;
	//m_nRotation = kDefaultRotation;
	m_nAltPercent = kDefaultAltPercent;
	UpdateData( FALSE );

}

void CShapeTab::OnRdAltShearShear() 
{
	if ( m_eDistortEffect != kAltShear )
	{
		HideShapeButtons();
		LoadShapeButtonBitmaps( kAltShear );
		ShowButtons( kAltShear  );
		m_ctrlSelShapeBmp.ShowWindow( SW_HIDE );
		m_pParentPropSheet->ApplyFollowPathModificationsToAttribTab( FALSE, m_nSelShapeId );
		SetModified( TRUE );

	}

	// Enable non distort check box
	m_ctrlNonDistort.EnableWindow( TRUE );
	
	//
	//Hide/Show the appropriate angle edit controls
	m_ctrlShearAngle.ShowWindow( SW_SHOW );
	m_ctrlRotation.ShowWindow( SW_HIDE );
	m_ctrlAltPercent.ShowWindow( SW_HIDE );
	//
	//Reset the Angle Values
	m_nShearAngle = kDefaultAltShearAngle;
	m_nRotation = kDefaultRotation;
	m_nAltPercent = kDefaultAltPercent;
	UpdateData( FALSE );

}

////////////////////////////////////////////////////////////////////////////////////
//Function: LoadShapeButtonBitmaps
//
//Description: Switches on the shape type (ie. NonDistort, FollowPath, or WarpShape)
//			   and loads the appropriate buttons with the appropriate bitmaps	
//
//Paramaters:  nBtnType -enum specifying which shape type
//
//Returns:     VOID
//
////////////////////////////////////////////////////////////////////////////////////
void CShapeTab::LoadShapeButtonBitmaps( EDistortEffects nBtnType )
{
	int nButtonBmpIdx = 0;

	switch( nBtnType )
	{
	case kNonDistort:
		m_eDistortEffect = kNonDistort;
		break;

	case kFollowPath:
		m_eDistortEffect = kFollowPath;

		m_btnShape1.LoadBitmaps(kFollowBtn1Bmp);
		m_btnShape2.LoadBitmaps(kFollowBtn2Bmp);
		m_btnShape3.LoadBitmaps(kFollowBtn3Bmp);
		m_btnShape4.LoadBitmaps(kFollowBtn4Bmp);
		m_btnShape4.LoadBitmaps(kFollowBtn4Bmp);
		m_btnShape5.LoadBitmaps(kFollowBtn4Bmp);
		m_btnShape6.LoadBitmaps(kFollowBtn4Bmp);
		m_btnShape7.LoadBitmaps(kFollowBtn4Bmp);
		m_btnShape8.LoadBitmaps(kFollowBtn4Bmp);
		m_btnShape9.LoadBitmaps(kFollowBtn4Bmp);
		m_btnShape10.LoadBitmaps(kFollowBtn4Bmp);
		m_btnShape11.LoadBitmaps(kFollowBtn4Bmp);
		break;

	case kWarpToEnvelop:
		m_eDistortEffect = kWarpToEnvelop;
		m_btnShape1.LoadBitmaps(kWarpBtn1Bmp);
		m_btnShape2.LoadBitmaps(kWarpBtn2Bmp);
		m_btnShape3.LoadBitmaps(kWarpBtn3Bmp);
		m_btnShape4.LoadBitmaps(kWarpBtn5Bmp);		//	This is easier than changing the resource, enum
		m_btnShape5.LoadBitmaps(kWarpBtn4Bmp);		//	and rebuilding all files.	MDH 8/5/96
		m_btnShape6.LoadBitmaps(kWarpBtn6Bmp);
		m_btnShape7.LoadBitmaps(kWarpBtn7Bmp);
		m_btnShape8.LoadBitmaps(kWarpBtn8Bmp);
		m_btnShape9.LoadBitmaps(kWarpBtn9Bmp);
		m_btnShape10.LoadBitmaps(kWarpBtn10Bmp);
		m_btnShape11.LoadBitmaps(kWarpBtn11Bmp);
		m_btnShape12.LoadBitmaps(kWarpBtn12Bmp);
		m_btnShape13.LoadBitmaps(kWarpBtn13Bmp);
		m_btnShape14.LoadBitmaps(kWarpBtn14Bmp);
		m_btnShape15.LoadBitmaps(kWarpBtn15Bmp);
		break;

	case kAltBaseline:
		m_eDistortEffect = kAltBaseline;
		break;

	case kShear:
		m_eDistortEffect = kShear;
		break;

	case kAltShearRotate:
		m_eDistortEffect = kAltShearRotate;
		break;

	case kAltShear:
		m_eDistortEffect = kAltShear;
		break;

	default:
		ASSERT( FALSE );
	}
}

////////////////////////////////////////////////////////////////////////////////////
//Function: ShowButtons
//
//Description:Switches on the shape type (ie. NonDistort, FollowPath, or WarpShape)
//			  and shows the appropriate buttons .
//
//Paramaters: nBtnType -enum specifying which shape type
//
//Returns:    VOID
//
////////////////////////////////////////////////////////////////////////////////////
void CShapeTab::ShowButtons( int nBtnType )
{
	switch( nBtnType )
	{
	case kNonDistort:
		break;

	case kFollowPath:
		m_btnShape1.ShowWindow( SW_SHOW );
		m_btnShape2.ShowWindow( SW_SHOW );
		m_btnShape3.ShowWindow( SW_SHOW );
		m_btnShape4.ShowWindow( SW_SHOW );
		m_btnShape5.ShowWindow( SW_SHOW );
		m_btnShape6.ShowWindow( SW_SHOW );
		m_btnShape7.ShowWindow( SW_SHOW );
		m_btnShape8.ShowWindow( SW_SHOW );
		m_btnShape9.ShowWindow( SW_SHOW );
		m_btnShape10.ShowWindow( SW_SHOW );
		m_btnShape11.ShowWindow( SW_SHOW );
		break;

	case kWarpToEnvelop:
		m_btnShape1.ShowWindow( SW_SHOW );
		m_btnShape2.ShowWindow( SW_SHOW );
		m_btnShape3.ShowWindow( SW_SHOW );
		m_btnShape4.ShowWindow( SW_SHOW );
		m_btnShape5.ShowWindow( SW_SHOW );
		m_btnShape6.ShowWindow( SW_SHOW );
		m_btnShape7.ShowWindow( SW_SHOW );
		m_btnShape8.ShowWindow( SW_SHOW );
		m_btnShape9.ShowWindow( SW_SHOW );
		m_btnShape10.ShowWindow( SW_SHOW );
		m_btnShape11.ShowWindow( SW_SHOW );
		m_btnShape12.ShowWindow( SW_SHOW );
		m_btnShape13.ShowWindow( SW_SHOW );
		//REVIEW Mike H
		//Following two buttons disabled because of duplicate bitmaps
		/*
		m_btnShape14.ShowWindow( SW_SHOW );
		m_btnShape15.ShowWindow( SW_SHOW );
		*/

		break;

	case kAltBaseline:
		break;
	case kShear:
		break;
	case kAltShearRotate:
		break;
	case kAltShear:
		break;
	default:
		ASSERT( FALSE );
	}
}

void CShapeTab::OnBtnShape1() 
{
	LoadSelectedShapeBmp( (m_eDistortEffect == kFollowPath)? (int)kWarpPath10 : (int)kNoWarpShape );
	SetModified( TRUE );
}

void CShapeTab::OnBtnShape2() 
{
	LoadSelectedShapeBmp( (m_eDistortEffect == kFollowPath)? (int)kWarpPath20 : (int)kWarpShape5 );
	SetModified( TRUE );
}

void CShapeTab::OnBtnShape3() 
{
	LoadSelectedShapeBmp( (m_eDistortEffect == kFollowPath)? (int)kWarpPath30 : (int)kWarpShape2 );
	SetModified( TRUE );

}

void CShapeTab::OnBtnShape4() 
{
	LoadSelectedShapeBmp( (m_eDistortEffect == kFollowPath)? (int)kWarpPath40 : (int)kWarpShape9 );
	SetModified( TRUE );

}

void CShapeTab::OnBtnShape5() 
{
	LoadSelectedShapeBmp( (m_eDistortEffect == kFollowPath)? (int)kWarpPath50 : (int)kWarpShape10 );
	SetModified( TRUE );

}

void CShapeTab::OnBtnShape6() 
{
	LoadSelectedShapeBmp( (m_eDistortEffect == kFollowPath)? (int)kWarpPath60 : (int)kWarpShape1 );
	SetModified( TRUE );

}

void CShapeTab::OnBtnShape7() 
{
	LoadSelectedShapeBmp( (m_eDistortEffect == kFollowPath)? (int)kWarpPath70 : (int)kWarpShape6 );
	SetModified( TRUE );

}

void CShapeTab::OnBtnShape8() 
{
	LoadSelectedShapeBmp( (m_eDistortEffect == kFollowPath)? (int)kWarpPath80 : (int)kWarpShape3 );
	SetModified( TRUE );

}

void CShapeTab::OnBtnShape9() 
{
	LoadSelectedShapeBmp( (m_eDistortEffect == kFollowPath)? (int)kWarpPath90 : (int)kWarpShape11 );
	SetModified( TRUE );

}

void CShapeTab::OnBtnShape10() 
{
	LoadSelectedShapeBmp( (m_eDistortEffect == kFollowPath)? (int)kWarpPath100 : (int)kWarpShape12 );
	SetModified( TRUE );

}

void CShapeTab::OnBtnShape11() 
{
	LoadSelectedShapeBmp( (m_eDistortEffect == kFollowPath)? (int)kWarpPath110 : (int)kWarpShape8 );
	SetModified( TRUE );
}

void CShapeTab::OnBtnShape12() 
{
	LoadSelectedShapeBmp( (int)kWarpShape7 );
	SetModified( TRUE );

}

void CShapeTab::OnBtnShape13() 
{										   
	LoadSelectedShapeBmp( (int)kWarpShape4 );
	SetModified( TRUE );

}

void CShapeTab::OnBtnShape14() 
{
	LoadSelectedShapeBmp( (int)kWarpShape1 );
	SetModified( TRUE );

}

void CShapeTab::OnBtnShape15() 
{
	LoadSelectedShapeBmp( (int)kWarpShape6 );
	SetModified( TRUE );

}

////////////////////////////////////////////////////////////////////////////////////
//Function: SetPointerToOwner
//
//Description: Stores a pointer to the REditHeadlineDlg class that called it so I can
//					call methods in REditHeadlineDlg
//
//Paramaters: pParentPropSheet - pointer to REditHeadlineDlg that instantiated this 
//											instance.
//
//Returns:    VOID
//
////////////////////////////////////////////////////////////////////////////////////
void CShapeTab::SetPointerToOwner( REditHeadlineDlg* pParentPropSheet )
{
	ASSERT( pParentPropSheet );
	m_pParentPropSheet = pParentPropSheet;

}

////////////////////////////////////////////////////////////////////////////////////
//Function: FillData
//
//Description:Loads data from the property page into a ShapeTabDataStruct
//
//Paramaters: pShapeData -ShapeTabDataStruct to accept Shape Page data.
//
//Returns:    VOID
//
////////////////////////////////////////////////////////////////////////////////////
void CShapeTab::FillData( ShapeDataStruct* pShapeData )
{
	pShapeData->eDistortEffect	= m_eDistortEffect;
	pShapeData->nSelShapeIdx	= m_nSelShapeId;

	pShapeData->nShearAngle			= m_nShearAngle;
	pShapeData->nRotation			= m_nRotation;
	float flAltPercent				= (float)m_nAltPercent;
	pShapeData->flAltPercent	   = flAltPercent/100.0f;
	pShapeData->fDistort			= (BOOLEAN)m_fNonDistort;
}

BOOL CShapeTab::OnApply()
{
	m_pParentPropSheet->UpdateHeadlinePreview();
	return CPropertyPage::OnApply();
}

BOOL CShapeTab::OnKillActive() 
{
	UpdateData( TRUE );
	m_pParentPropSheet->UpdateTabData( REditHeadlineDlg::kShapeTab );
	return CPropertyPage::OnKillActive();
}
