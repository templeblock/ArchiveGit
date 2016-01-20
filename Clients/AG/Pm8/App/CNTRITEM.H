// cntritem.h : interface of the COletestCntrItem class
//

class CPmwDoc;
class CPmwView;
class OleObject;

class CPmwCntrItem : public COleClientItem
{
	DECLARE_SERIAL(CPmwCntrItem)

protected:
	OleObject* m_pObject;			// The object pointing to us.
// Constructors
public:
	CPmwCntrItem(CPmwDoc* pContainer = NULL);
		// Note: pContainer is allowed to be NULL to enable IMPLEMENT_SERIALIZE.
		//  IMPLEMENT_SERIALIZE requires the class have a constructor with
		//  zero arguments.  Normally, OLE items are constructed with a
		//  non-NULL document pointer.

// Attributes
public:
	CPmwDoc* GetDocument()
		{ return (CPmwDoc*)COleClientItem::GetDocument(); }
	CPmwView* GetActiveView()
		{ return (CPmwView*)COleClientItem::GetActiveView(); }
	OleObject* Object(void) const
		{ return m_pObject; }
	void Object(OleObject* pObject)
		{ m_pObject = pObject; }

	BOOL SetExtentAndRects( LPCRECT lpPosRect = NULL, LPCRECT lpClipRect = NULL );

// Implementation
public:
	~CPmwCntrItem();
	BOOL GetColorSet(LPLOGPALETTE* ppLogPalette);
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif
	virtual void Serialize(CArchive& ar);
	virtual void OnGetItemPosition(CRect& rPosition);
	virtual void OnDeactivateUI(BOOL bUndoable);

protected:
	virtual void OnChange(OLE_NOTIFICATION wNotification, DWORD dwParam);
	virtual BOOL OnChangeItemPosition(const CRect& rectPos);
	virtual BOOL CanActivate();
};

/////////////////////////////////////////////////////////////////////////////
