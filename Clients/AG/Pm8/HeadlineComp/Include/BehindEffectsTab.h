//****************************************************************************
//
// File Name:  BehindEffectsTab.h	
//
// Project:    Renaissance application framework
//
// Author:     Mike Heydlauf
//
// Description: Definition of CBehindEffectsTab class
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

#ifndef _BEHINDEFFECTSTAB_H_
#define _BEHINDEFFECTSTAB_H_

class REditHeadlineDlg;
class CBehindEffectsTab : public CPropertyPage
{
// Construction
public:
	CBehindEffectsTab();
	CBehindEffectsTab( struct BehindEffectsData* pBehindEffectsData );
	void CBehindEffectsTab::FillData( struct BehindEffectsData* pBehindEffectsData );
	void SetPointerToOwner( REditHeadlineDlg* pParentPropSheet );
	virtual BOOL OnKillActive();
	void SetData(  struct BehindEffectsData* pBehindEffectsData );
// Dialog Data
	//{{AFX_DATA(CBehindEffectsTab)
	enum { IDD = IDD_PPG_BEHIND_EFFECTS };
	CComboBox	m_cmbShadowColor;
	CComboBox	m_cmbHiliteColor;
	CComboBox	m_cmbFillColor;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CBehindEffectsTab)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	EFramedEffects								m_eBehindEffect;		//Selected BehindEffects Type
	RSolidColor									m_colorFill;			//Fill color
	RSolidColor									m_colorShadow;			//Shadow color
	RSolidColor									m_colorHilite;			//Hilite color

	void SetupColorComboBoxes();
	EColors GetEColorFromRColor( RSolidColor* pColor );

	REditHeadlineDlg* m_pParentPropSheet;
	// Generated message map functions
	//{{AFX_MSG(CBehindEffectsTab)
	virtual BOOL OnInitDialog();
	virtual BOOL OnApply( );
	afx_msg void OnCmbHiliteColorSelChange();
	afx_msg void OnCmbShadowColorSelChange();
	afx_msg void OnCmbFillColorSelChange();
	afx_msg void OnRdSilhouette();
	afx_msg void OnRdNoFrame();
	afx_msg void OnRdEmboss();
	afx_msg void OnRdDeboss();
	
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#endif  //_BEHINDEFFECTSTAB_H_
