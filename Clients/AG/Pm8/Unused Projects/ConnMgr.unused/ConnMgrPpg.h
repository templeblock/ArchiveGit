#if !defined(AFX_CONNMGRPPG_H__94FB2C55_512E_11D2_8BEF_00A0C9B12C3D__INCLUDED_)
#define AFX_CONNMGRPPG_H__94FB2C55_512E_11D2_8BEF_00A0C9B12C3D__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

// ConnMgrPpg.h : Declaration of the CConnMgrPropPage property page class.

////////////////////////////////////////////////////////////////////////////
// CConnMgrPropPage : See ConnMgrPpg.cpp.cpp for implementation.

class CConnMgrPropPage : public COlePropertyPage
{
	DECLARE_DYNCREATE(CConnMgrPropPage)
	DECLARE_OLECREATE_EX(CConnMgrPropPage)

// Constructor
public:
	CConnMgrPropPage();

// Dialog Data
	//{{AFX_DATA(CConnMgrPropPage)
	enum { IDD = IDD_PROPPAGE_CONNMGR };
		// NOTE - ClassWizard will add data members here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DATA

// Implementation
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Message maps
protected:
	//{{AFX_MSG(CConnMgrPropPage)
		// NOTE - ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CONNMGRPPG_H__94FB2C55_512E_11D2_8BEF_00A0C9B12C3D__INCLUDED)
