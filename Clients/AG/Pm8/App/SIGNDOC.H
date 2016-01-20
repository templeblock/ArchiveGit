/*
// $Workfile: SIGNDOC.H $
// $Revision: 1 $
// $Date: 3/03/99 6:11p $
// 
//  "This unpublished source code contains trade secrets which are the
//   property of Mindscape, Inc.  Unauthorized use, copying or distribution
//   is a violation of international laws and is strictly prohibited."
// 
//        Copyright © 1998 Mindscape, Inc. All rights reserved.
*/

/*
// Revision History:
//
// $Log: /PM8/App/SIGNDOC.H $
// 
// 1     3/03/99 6:11p Gbeddow
// 
// 4     8/17/98 5:36p Mwilson
// changed Tshirts to be based off of newsletters instead of posters
// 
// 3     8/05/98 10:57a Mwilson
// added a goto first page for multipage t-shirts
// 
// 2     4/17/98 4:47p Fredf
// T-Shirts
*/

#ifndef __SIGNDOC_H__
#define __SIGNDOC_H__

/////////////////////////////////////////////////////////////////////////////
// CPosterDoc document

class CPosterDoc : public CPmwDoc
{
	DECLARE_SERIAL(CPosterDoc)
protected:
	CPosterDoc();			// protected constructor used by dynamic creation

// Attributes
public:

	static ProjectInfo StandardInfo;
// Operations
public:

// Implementation
protected:
	virtual ~CPosterDoc();

	// Generated message map functions
protected:
	//{{AFX_MSG(CPosterDoc)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

class CCertificateDoc : public CPosterDoc
{
	DECLARE_SERIAL(CCertificateDoc)
protected:
	CCertificateDoc();			// protected constructor used by dynamic creation

// Attributes
public:

	static ProjectInfo StandardInfo;
// Operations
public:

// Implementation
protected:
	virtual ~CCertificateDoc();

	// Generated message map functions
protected:
	//{{AFX_MSG(CCertificateDoc)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

class CFaxCoverDoc : public CPosterDoc
{
	DECLARE_SERIAL(CFaxCoverDoc)
protected:
	CFaxCoverDoc();			// protected constructor used by dynamic creation

// Attributes
public:

	static ProjectInfo StandardInfo;
// Operations
public:

// Implementation
protected:
	virtual ~CFaxCoverDoc();

	// Generated message map functions
protected:
	//{{AFX_MSG(CFaxCoverDoc)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

class CStationeryDoc : public CPosterDoc
{
	DECLARE_SERIAL(CStationeryDoc)
protected:
	CStationeryDoc();			// protected constructor used by dynamic creation

// Attributes
public:

	static ProjectInfo StandardInfo;
// Operations
public:

// Implementation
protected:
	virtual ~CStationeryDoc();

	// Generated message map functions
protected:
	//{{AFX_MSG(CStationeryDoc)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


#endif
