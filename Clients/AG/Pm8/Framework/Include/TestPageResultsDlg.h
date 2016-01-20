//****************************************************************************
//
// File Name:		TestPageResultsDlg.cpp
//
// Project:			Renaissance application framework
//
// Author:			Eric VanHelene
//
// Description:	Defines the test page results Dialog's class.
//				
// Portability:	Windows Specific
//
// Developed by:  Turning Point Software
//						One Gateway Center, Suite 800
//						Newton, Ma 02158
//						(617)332-0202
//
// Client:			Broderbund Software, Inc.         
//
// Copyright(C)1995, Turning Point Software, All Rights Reserved.
//
//  $Logfile:: /PM8/Framework/Include/TestPageResultsDlg.h                   $
//   $Author:: Gbeddow                                                       $
//     $Date:: 3/03/99 6:16p                                                 $
// $Revision:: 1                                                             $
//
//****************************************************************************

#ifndef _TESTPAGERESULTSDLG_H_
#define _TESTPAGERESULTSDLG_H_ 

#ifdef BYTE_ALIGNMENT
#pragma pack(push, BYTE_ALIGNMENT)
#endif

class RTestPageResultsDlg;
class RPrinter;
class RTestPageProject;

typedef struct TagTestPageResultsStruct
{
	YIntDimension		yTopCalibration;
	YIntDimension		yLeftCalibration;
	YIntDimension		yRightCalibration;
	YIntDimension		yBottomCalibration;
} 
TestPageResultsStruct;

class RTestPageResultsDlg : public CDialog
{
// Construction
public:
	RTestPageResultsDlg( RPrinter* pPrinter, CWnd* pParent = NULL);   // standard constructor
	~RTestPageResultsDlg();
	virtual BOOL							OnInitDialog( );
	virtual void							OnOK( );
	// Dialog Data
	//{{AFX_DATA(RTestPageResultsDlg)
	TestPageResultsStruct m_TestPageResultsData;
	//}}AFX_DATA

	enum	ESpinCalRange {
				kMinCalRange		=	1,
				kDefaultRotation	=	17,
				kMaxCalRange		=	32
			};

	CSpinButtonCtrl	m_TopCalibrationSpinRotate;
	CSpinButtonCtrl	m_LeftCalibrationSpinRotate;
	CSpinButtonCtrl	m_RightCalibrationSpinRotate;
	CSpinButtonCtrl	m_BottomCalibrationSpinRotate;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(RTestPageResultsDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

//Access
public:
	void					SetData ( TestPageResultsStruct* pMFData );
	void					FillData( TestPageResultsStruct* pMFData );
	RIntRect				GetPrintableArea( );
	
	//	Enums
	enum ETruncationTest { kABC, kABCD };
	enum ETransparencyTest { kCARD, kCARDS };

// Implementation
protected:
	CString m_VerticalCalibration;
	CString m_HorizontalCalibration;
	RPrinter* m_pPrinter;
	YIntDimension	m_yVerticalOffset;
	YIntDimension	m_yHorizontalOffset;
	ETruncationTest	m_eTruncTest;
	ETransparencyTest m_eTransTest;

	afx_msg void		OnTextChange();

	// Generated message map functions
	//{{AFX_MSG(RTestPageResultsDlg)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#ifdef BYTE_ALIGNMENT
#pragma pack(pop)
#endif

#endif //_TESTPAGERESULTSDLG_H_
