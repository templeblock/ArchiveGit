#if !defined(AFX_SETTINGSDLG_H__9E4DFFC1_9635_11D1_8680_006008661BA9__INCLUDED_)
#define AFX_SETTINGSDLG_H__9E4DFFC1_9635_11D1_8680_006008661BA9__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// SettingsDlg.h : header file
//

#include "beobjd.h"


class CSettingsDlg;

/////////////////////////////////////////////////////////////////////////////
// CSettings class. Keeps track of changes in a CSettingsDlg

class CSettings
{
	friend class CSettingsDlg;

	// Keep the border editor object to change
	CBEObjectD* m_pBEObj;
	
	BOOL m_bSizeChanged;	// Keeps track of whether edge size field was changed
	BOOL m_bForceTile;		// TRUE to force tiling to a specific number
	int	m_nTiles;
	int m_nMaxTiles;
	double	m_dEdgeSize;

public:
	
// Operations	
	void TransferSettings(BOOL aRegions[]);
};

/////////////////////////////////////////////////////////////////////////////
// CSettingsDlg dialog

class CSettingsDlg : public CDialog
{
	BOOL m_bValidSize;		// Keeps track of whether edge size field is valid
	BOOL m_bValidMaxTiles;	// Keeps track of whether max tiles field is valid
	BOOL m_bIgnoreSizeChange;	// TRUE to avoid recording text changes as valid
	BOOL m_bIgnoreMaxChange;	// TRUE to avoid recording text changes as valid

	// Changes
public:
	CSettings m_Settings;

// Construction
public:
	CSettingsDlg(CBEObjectD* pBEObject, CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CSettingsDlg)
	enum { IDD = IDD_SETTINGS };
	int		m_nTiles;
	double	m_dEdgeSize;
	int		m_nMaxTiles;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSettingsDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CSettingsDlg)
	virtual void OnOK();
	afx_msg void OnUseTiling();
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg void OnChangeEdSize();
	afx_msg void OnChangeEdMaxTiles();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SETTINGSDLG_H__9E4DFFC1_9635_11D1_8680_006008661BA9__INCLUDED_)
