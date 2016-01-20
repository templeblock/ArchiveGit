#if !defined(AFX_TARGET_H__A3F4C371_3A40_11D1_BAE5_200604C10000__INCLUDED_)
#define AFX_TARGET_H__A3F4C371_3A40_11D1_BAE5_200604C10000__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// Target.h : header file
//

#include "scancal.h"

#define	NUM_COMP			(3)
#define	TABLE_GRID_POINT	(33)
#define TABLE_ENTRIES		(TABLE_GRID_POINT*TABLE_GRID_POINT*TABLE_GRID_POINT)
#define TABLE_SIZE			(TABLE_ENTRIES*NUM_COMP)

/////////////////////////////////////////////////////////////////////////////
// CTarget

class CTarget : public CWnd
{
public:
	CTarget();           // protected constructor used by dynamic creation

// Attributes
public:
	double m_fTargetData[300*3];
	double m_fPatchData[300*3];
	int m_nTargetPatches;
	double m_fTable[TABLE_SIZE];

// Operations
public:

// Dialog Data
	ScanCal _scancal;
	//{{AFX_DATA(CTarget)
	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTarget)
	//}}AFX_VIRTUAL

protected:
// Generated message map functions
	//{{AFX_MSG(CTarget)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:
// Implementation
	virtual ~CTarget();

	BOOL ReadTargetDataFile( CString pstrFileName );
	BOOL GeneratePatchLocations( CRect* prPatches, CPoint ptUL, CPoint ptUR, CPoint ptLL, CPoint ptLR );
	BOOL ReadPatch( PBITMAPINFO pDib, CRect rRect, double* pfPatchData );
	BOOL ReadPatchData( HGLOBAL hDib, CPoint ptUL, CPoint ptUR, CPoint ptLL, CPoint ptLR );
	void InitTable( double* pfTable );
	BOOL Calibrate( double* pfAvgDeltaE, double* pfMaxDeltaE );
	BOOL SaveProfile(CString szProfileName);
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TARGET_H__A3F4C371_3A40_11D1_BAE5_200604C10000__INCLUDED_)
