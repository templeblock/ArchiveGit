//****************************************************************************
//
// File Name:  FileFailDlg.cpp
//
// Project:    Renaissance application framework
//
// Author:     Eric VanHelene
//
// Description: Defines the file failure Dialog's class.
//				
// Portability: Windows Specific
//
// Developed by:   Turning Point Software
//						 One Gateway Center, Suite 800
//						 Newton, Ma 02158
//						(617)332-0202
//
// Client:		   Broderbund
//
// Copyright(C)1995, Turning Point Software, All Rights Reserved.
//
// $Header:: /PM8/Framework $
//
//****************************************************************************
#ifndef _FILEFAILDLG_H_
#define _FILEFAILDLG_H_

#include "FrameworkResourceIds.h"

#include "MemoryFailDlg.h"

#ifdef BYTE_ALIGNMENT
#pragma pack(push, BYTE_ALIGNMENT)
#endif

class RFileFailDlg;

struct FileFailureStruct
{
	EFailure eFail;
	uWORD    uwNumAllocates;
	uWORD    uwNumTimesToFail;
};

class RFileFailDlg : public CDialog
{
// Construction
public:
	RFileFailDlg(CWnd* pParent = NULL);   // standard constructor
	~RFileFailDlg();
	// Dialog Data
	//{{AFX_DATA(RFileFailDlg)
	enum { IDD = DIALOG_FILE_FAIL };
	UINT	m_unFailTime;
	UINT	m_unNumTimesToFail;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(RFileFailDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

//Access
public:
	void SetData ( FileFailureStruct* pMFData );
	void FillData( FileFailureStruct* pMFData );

// Implementation
protected:
	EFailure		m_eSelFail;
	FileFailureStruct m_FileFailData;
	// Generated message map functions
	//{{AFX_MSG(RFileFailDlg)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#ifdef BYTE_ALIGNMENT
#pragma pack(pop)
#endif

#endif //_FILEFAILDLG_H_
