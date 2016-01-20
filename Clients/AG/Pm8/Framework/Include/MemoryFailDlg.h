//****************************************************************************
//
// File Name:  MemoryFailDlg.cpp
//
// Project:    Renaissance application framework
//
// Author:     Mike Heydlauf
//
// Description: Defines the Memory Failure Dialog's class.
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
// $Header:: /PM8/Framework $
//
//****************************************************************************
#ifndef _MEMORYFAILDLG_H_
#define _MEMORYFAILDLG_H_

#include "FrameworkResourceIds.h"

#ifdef BYTE_ALIGNMENT
#pragma pack(push, BYTE_ALIGNMENT)
#endif

enum EFailure { kNeverFail, kFailAfter };

class RMemoryFailDlg;

typedef struct MemFailureStruct
{
	EFailure eFail;
	uWORD    uwNumAllocates;
	uWORD    uwNumFailures;
} MemoryFailureStruct;

class RMemoryFailDlg : public CDialog
{
// Construction
public:
	RMemoryFailDlg(CWnd* pParent = NULL);   // standard constructor
	~RMemoryFailDlg();
	// Dialog Data
	//{{AFX_DATA(RMemoryFailDlg)
	enum { IDD = DIALOG_MEM_FAIL };
	UINT	m_unNumFailures;
	UINT	m_unFailTime;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(RMemoryFailDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

//Access
public:
	void SetData ( MemoryFailureStruct* pMFData );
	void FillData( MemoryFailureStruct* pMFData );

// Implementation
protected:
	EFailure	m_eSelFail;
	MemFailureStruct m_MemFailData;
	// Generated message map functions
	//{{AFX_MSG(RMemoryFailDlg)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#ifdef BYTE_ALIGNMENT
#pragma pack(pop)
#endif

#endif //_MEMORYFAILDLG_H_
