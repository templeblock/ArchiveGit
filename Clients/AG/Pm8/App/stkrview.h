
#ifndef __STKRVIEW_H__
#define __STKRVIEW_H__

/////////////////////////////////////////////////////////////////////////////
// CStickerView view

class CStickerView : public CPmwView
{
	INHERIT(CStickerView, CPmwView)

	DECLARE_DYNCREATE(CStickerView)
protected:
	CStickerView();			// protected constructor used by dynamic creation

// Attributes
public:

	public:
	// Gadget interface.
	virtual BOOL GadgetVisible(UINT uID);
	virtual void GadgetText(UINT uID, CString& csText);
	virtual LPCSTR GadgetMenuName(UINT uID, UINT uPart);
// Operations
public:

// Implementation
protected:
	virtual ~CStickerView();

	virtual void OnDraw(CDC* pDC);				// overridden to draw this view 
	virtual void OnDraw(PrintContext* pPC);	// overridden to print this view 
	virtual SHORT OnPrintDocument(PrintContext &pc,
											int32 aCurrentDocument, int32 aNumDocuments);
	virtual void CreateEditDesktop(void);

	virtual CPmwPrint* CreatePrintDialog(void);

	// Generated message map functions
protected:
	//{{AFX_MSG(CStickerView)
	afx_msg void OnStickerType();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#endif
