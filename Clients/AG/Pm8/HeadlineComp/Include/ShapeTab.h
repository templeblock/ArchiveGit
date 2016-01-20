//****************************************************************************
//
// File Name:  ShapeTab.h	
//
// Project:    Renaissance application framework
//
// Author:     Mike Heydlauf
//
// Description: Definition of CShapeTab class
//
// Portability: Windows Specific
//
// Developed by:   Turning Point Software
//				   One Gateway Center, Suite 800
//				   Newton, Ma 02158
//			       (617)332-0202
//
// Client:		   Broderbund
//
// Copyright(C)1995, Turning Point Software, All Rights Reserved.
//
// $Header:: /PM8/HeadlineC $
//
//****************************************************************************
#ifndef _SHAPETAB_H_
#define _SHAPETAB_H_

#define kMaxNumShapeBmpBtns  15

#define kFollowBtn1Bmp  IDB_PATH_1
#define kFollowBtn2Bmp  IDB_PATH_2
#define kFollowBtn3Bmp  IDB_PATH_4
#define kFollowBtn4Bmp  IDB_PATH_3

#define kWarpBtn1Bmp  IDB_SHAPE_A1
#define kWarpBtn2Bmp  IDB_SHAPE_B1
#define kWarpBtn3Bmp  IDB_SHAPE_C1
#define kWarpBtn4Bmp  IDB_SHAPE_D1
#define kWarpBtn5Bmp  IDB_SHAPE_E1
#define kWarpBtn6Bmp  IDB_SHAPE_A2
#define kWarpBtn7Bmp  IDB_SHAPE_B2
#define kWarpBtn8Bmp  IDB_SHAPE_C2
#define kWarpBtn9Bmp  IDB_SHAPE_D2
#define kWarpBtn10Bmp  IDB_SHAPE_E2
#define kWarpBtn11Bmp  IDB_SHAPE_A3
#define kWarpBtn12Bmp  IDB_SHAPE_B3
#define kWarpBtn13Bmp  IDB_SHAPE_C3
#define kWarpBtn14Bmp  IDB_SHAPE_D3
#define kWarpBtn15Bmp  IDB_SHAPE_E3

#define kShearAngleMin				-89
#define kDefaultShearAngle			45
#define kDefaultAltShearAngle    30
#define kShearAngleMax				89

#define kRotationMin					-89
#define kDefaultRotation			30
#define kRotationMax					89


#define kAltPercentMin				0
#define kDefaultAltPercent			25
#define kAltPercentMax				100

class REditHeadlineDlg;

class CShapeTab : public CPropertyPage
{
	DECLARE_DYNCREATE(CShapeTab)

// Construction
public:
	CShapeTab();
	CShapeTab( struct ShapeData* pHlData );
	~CShapeTab();
	void SetPointerToOwner( REditHeadlineDlg* pParentPropSheet );
	void FillData( struct ShapeData* pShapeData );
	virtual BOOL OnKillActive();
	void SetData( ShapeDataStruct* pShapeData );

// Dialog Data
	//{{AFX_DATA(CShapeTab)
	enum { IDD = IDD_PPG_SHAPE };
	CBitmapButton	m_ctrlSelShapeBmp;
	CBitmapButton	m_btnShape9;
	CBitmapButton	m_btnShape8;
	CBitmapButton	m_btnShape7;
	CBitmapButton	m_btnShape6;
	CBitmapButton	m_btnShape5;
	CBitmapButton	m_btnShape4;
	CBitmapButton	m_btnShape3;
	CBitmapButton	m_btnShape2;
	CBitmapButton	m_btnShape15;
	CBitmapButton	m_btnShape14;
	CBitmapButton	m_btnShape13;
	CBitmapButton	m_btnShape12;
	CBitmapButton	m_btnShape11;
	CBitmapButton	m_btnShape10;
	CBitmapButton	m_btnShape1;
	int		m_nShearAngle;
	int		m_nRotation;
	int		m_nAltPercent;
	CEdit		m_ctrlShearAngle;
	CEdit		m_ctrlRotation;
	CEdit		m_ctrlAltPercent;
	CButton	m_ctrlNonDistort;
	BOOL		m_fNonDistort;
	//}}AFX_DATA
	
	EDistortEffects	m_eDistortEffect;


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CShapeTab)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	int m_nSelShapeId;

	BOOLEAN m_fIsShear;
	BOOLEAN m_fIsAltShear;
	BOOLEAN m_fIsAltBaseline;

	void LoadSelectedShapeBmp( int nSelShapeId );
	void ShowButtons( int nBtnType );
	void LoadShapeButtonBitmaps( EDistortEffects nBtnType );
	void HideShapeButtons();
	void ShowAngleEditControls();
	REditHeadlineDlg* m_pParentPropSheet;
	// Generated message map functions
	//{{AFX_MSG(CShapeTab)
	virtual BOOL OnInitDialog();
	virtual BOOL OnApply( );
	afx_msg void OnRdFollowPath();
	afx_msg void OnRdWarpShape();
	afx_msg void OnBtnShape1();
	afx_msg void OnBtnShape10();
	afx_msg void OnBtnShape11();
	afx_msg void OnBtnShape12();
	afx_msg void OnBtnShape13();
	afx_msg void OnBtnShape14();
	afx_msg void OnBtnShape15();
	afx_msg void OnBtnShape2();
	afx_msg void OnBtnShape3();
	afx_msg void OnBtnShape4();
	afx_msg void OnBtnShape5();
	afx_msg void OnBtnShape6();
	afx_msg void OnBtnShape7();
	afx_msg void OnBtnShape8();
	afx_msg void OnBtnShape9();
	afx_msg void OnRdShear();
	afx_msg void OnRdAltBaseline();
	afx_msg void OnRdAltShearRotate();
	afx_msg void OnRdAltShearShear();
	afx_msg void OnRdNonDistort();
	afx_msg void OnCbNonDistort();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};
#endif //_SHAPETAB_H_
