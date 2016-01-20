// PMWCompressDoc.h : interface of the CPMWCompressDoc class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_PMWCOMPRESSDOC_H__2A823AEB_C7DE_11D1_8680_0060089672BE__INCLUDED_)
#define AFX_PMWCOMPRESSDOC_H__2A823AEB_C7DE_11D1_8680_0060089672BE__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000


class CPMWCompressDoc : public CDocument
{
protected: // create from serialization only
	CPMWCompressDoc();
	DECLARE_DYNCREATE(CPMWCompressDoc)

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPMWCompressDoc)
	public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CPMWCompressDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CPMWCompressDoc)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PMWCOMPRESSDOC_H__2A823AEB_C7DE_11D1_8680_0060089672BE__INCLUDED_)
