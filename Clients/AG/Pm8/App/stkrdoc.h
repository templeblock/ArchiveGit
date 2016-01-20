
#ifndef __STKRDOC_H__
#define __STKRDOC_H__

/////////////////////////////////////////////////////////////////////////////
// CStickerDoc document

class CStickerDoc : public CPmwDoc
{
	DECLARE_SERIAL(CStickerDoc)
protected:
	CStickerDoc();			// protected constructor used by dynamic creation

// Attributes
public:

	static ProjectInfo StandardInfo;
// Operations
public:

	virtual int DefaultTextSize(int nDefault);
  virtual BOOL CanPageNext(void);
	virtual void DoPageNext(void);
// Implementation
protected:
	virtual ~CStickerDoc();

/*
// Resize the document to fit on the paper.
*/

	virtual VOID size_to_paper(VOID);

/*
// Return the paper info to use for tile calculations.
*/

	virtual CPaperInfo* PrintPaperInfo(void);

	// Generated message map functions
protected:
	//{{AFX_MSG(CStickerDoc)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
		
	virtual	BOOL OnOpenDocument(const char *pszPathName);

};

#endif
