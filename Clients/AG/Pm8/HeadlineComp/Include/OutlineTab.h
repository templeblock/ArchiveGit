//****************************************************************************
//
// File Name:  OutlineTab.h	
//
// Project:    Renaissance application framework
//
// Author:     Mike Heydlauf
//
// Description: Definition of COutlineTab class
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

#ifndef _OUTLINETAB_H_
#define _OUTLINETAB_H_

class REditHeadlineDlg;
class COutlineTab : public CPropertyPage
{
// Construction
public:
	COutlineTab();
	COutlineTab( struct OutlineData* pOutlineData );
	void COutlineTab::FillData( struct OutlineData* pOutlineData );
	void SetPointerToOwner( REditHeadlineDlg* pParentPropSheet );
	virtual BOOL OnKillActive();
	void SetData( OutlineDataStruct* pOutlineData );
// Dialog Data
	//{{AFX_DATA(COutlineTab)
	enum { IDD = IDD_PPG_OUTLINE };
	CComboBox	m_cmbStrokeColor;
	CComboBox	m_cmbShadowColor;
	CComboBox	m_cmbHiliteColor;
	CComboBox	m_cmbFillColor;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(COutlineTab)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	EOutlineEffects								m_eOutlineEffect;		//Selected Outline Type
	ELineWeight										m_eLineWeight;			//Selected Pen width
	RSolidColor										m_colorStroke;			//Stroke color
	RSolidColor										m_colorFill;			//Fill color
	RSolidColor										m_colorShadow;			//Shadow color
	RSolidColor										m_colorHilite;			//Hilite color

	void SetupColorComboBoxes();
	void UncheckRadioIfNecessary();
	EColors GetEColorFromRColor( RSolidColor* pColor );

	REditHeadlineDlg* m_pParentPropSheet;
	// Generated message map functions
	//{{AFX_MSG(COutlineTab)
	virtual BOOL OnInitDialog();
	virtual BOOL OnApply( );
	afx_msg void OnRdStroke();
	afx_msg void OnRdFill();
	afx_msg void OnRdStrokeAndFill();
	afx_msg void OnRdHeavy();
	afx_msg void OnRdDouble();
	afx_msg void OnRdBlurred();
	afx_msg void OnRdBeveled();
	afx_msg void OnCmbStrokeColorSelChange();
	afx_msg void OnCmbHiliteColorSelChange();
	afx_msg void OnCmbShadowColorSelChange();
	afx_msg void OnCmbFillColorSelChange();
	afx_msg void OnRdTriple();
	afx_msg void OnRdHairlineDouble();
	afx_msg void OnRdDoubleHairline();
	
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#endif  //_OUTLINETAB_H_
