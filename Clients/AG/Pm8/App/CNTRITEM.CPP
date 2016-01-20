// cntritem.cpp : implementation of the COletestCntrItem class
//

#include "stdafx.h"
#include "pmw.h"

#include "pmwdoc.h"
#include "pmwview.h"
#include "cntritem.h"
#include "oleobj.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPmwCntrItem implementation

IMPLEMENT_SERIAL(CPmwCntrItem, COleClientItem, 0)

CPmwCntrItem::CPmwCntrItem(CPmwDoc* pContainer)
	: COleClientItem(pContainer)
{
	// TODO: add one-time construction code here
	m_pObject = NULL;
}

CPmwCntrItem::~CPmwCntrItem()
{
	// TODO: add cleanup code here
}

BOOL CPmwCntrItem::GetColorSet(LPLOGPALETTE* ppLogPalette)
{
	*ppLogPalette = NULL;
	LPVIEWOBJECT lpViewObject = NULL;
	m_lpObject->QueryInterface(IID_IViewObject, (void**)&lpViewObject);
	if (lpViewObject != NULL)
	{
		TRACE("Got ViewObject: %08lx\n", lpViewObject);
		lpViewObject->GetColorSet(m_nDrawAspect, -1, NULL, NULL, NULL, ppLogPalette);
		lpViewObject->Release();
		TRACE("Got palette: %08lx\n", *ppLogPalette);
	}
	return (*ppLogPalette != NULL);
}

void CPmwCntrItem::OnChange(OLE_NOTIFICATION nCode, DWORD dwParam)
{
	ASSERT_VALID(this);
	ASSERT(m_pObject != NULL);

	COleClientItem::OnChange(nCode, dwParam);

	// When an item is being edited (either in-place or fully open)
	//  it sends OnChange notifications for changes in the state of the
	//  item or visual appearance of its content.

	m_pObject->changed_image();
	GetDocument()->refresh_object(m_pObject);
}

BOOL CPmwCntrItem::OnChangeItemPosition(const CRect& rectPos)
{
	ASSERT_VALID(this);

	// During in-place activation CPmwCntrItem::OnChangeItemPosition
	//  is called by the server to change the position on of the in-place
	//  window.  Usually, this is a result of the data in the server
	//  document changing such that the extent has changed or as a result
	//  of in-place resizing.
	//
	// The default here is to call the base class, which will call
	//  COleClientItem::SetItemRects to move the item
	//  to the new position.

	if (!COleClientItem::OnChangeItemPosition(rectPos))
		return FALSE;

	TRACE("OnChangeItemPosition: %d, %d, %d, %d\r\n", *(LPCRECT)rectPos);

	// TODO: update any cache you may have of the item's rectangle/extent

	CPmwView* pView = GetActiveView();
	CPmwDoc* pDoc = GetDocument();

	RedisplayContext* rc = pView->get_rc();
	PBOX Bound;

	rc->screen_to_pbox((RECT*)(LPCRECT)rectPos, &Bound);

	pDoc->refresh_object(m_pObject);
	m_pObject->set_bound(Bound);
	m_pObject->calc();
	pDoc->refresh_object(m_pObject);

	return TRUE;
}

void CPmwCntrItem::OnGetItemPosition(CRect& rPosition)
{
	ASSERT_VALID(this);
	ASSERT(m_pObject != NULL);

	// During in-place activation, CPmwCntrItem::OnGetItemPosition
	//  will be called to determine the location of this item.
	//  You can obtain the view by calling CPmwCntrItem::GetActiveView.

	// TODO: return correct rectangle (in pixels) in rectPos

	CPmwView* pView = GetActiveView();

	PBOX Bound = m_pObject->get_bound();
	RedisplayContext* rc = pView->get_rc();
	RECT r;

	rc->pbox_to_screen(&Bound, TRUE);
	rc->convert_pbox(&Bound, &r, NULL);

	rPosition = r;
}

void CPmwCntrItem::OnDeactivateUI(BOOL bUndoable)
{
	COleClientItem::OnDeactivateUI(bUndoable);

	// Close an in-place active item whenever it removes the user
	//  interface.  The action here should match as closely as possible
	//  to the handling of the escape key in the view.

	Deactivate();   // nothing fancy here -- just deactivate the object
}

void CPmwCntrItem::Serialize(CArchive& ar)
{
	ASSERT_VALID(this);

	// Call base class first to read in COleClientItem data.
	// Since this sets up the m_pDocument pointer returned from
	//  CPmwCntrItem::GetDocument, it is a good idea to call
	//  the base class Serialize first.
	COleClientItem::Serialize(ar);

	// now store/retrieve data specific to CPmwCntrItem
	if (ar.IsStoring())
	{
		// TODO: add storing code here
	}
	else
	{
		// TODO: add loading code here
	}
}

BOOL CPmwCntrItem::CanActivate()
{
	// Editing in-place while the server itself is being edited in-place
	//  does not work and is not supported.  So, disable in-place
	//  activation in this case.
	CPmwDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

//	ASSERT(pDoc->IsKindOf(RUNTIME_CLASS(COleServerDoc)));
//	if (pDoc->IsInPlaceActive())
//		return FALSE;

	// otherwise, rely on default behavior
	return COleClientItem::CanActivate();
}

#ifdef WIN32
/*
// Create those OLE helper functions that are not defined in 32-bit.
// First, we create a helper function for the other two...
*/

static int XformPixelsToHimetric(HDC hDCIn, int nDim, int nMetricID)
{
	HDC hDC = hDCIn;
	if (hDC == NULL)
	{
		hDC = ::CreateCompatibleDC(NULL);
		if (hDC == NULL)
		{
			return 0;
		}
	}
	int nMetric = ::GetDeviceCaps(hDC, nMetricID);
	if (hDCIn == NULL)
	{
		::DeleteDC(hDC);
	}

	return MulDiv(nDim, 2540, nMetric);
}

int XformHeightInPixelsToHimetric(HDC hDC, int nHeight)
{
	return XformPixelsToHimetric(hDC, nHeight, LOGPIXELSY);
}
int XformWidthInPixelsToHimetric(HDC hDC, int nWidth)
{
	return XformPixelsToHimetric(hDC, nWidth, LOGPIXELSX);
}

#endif

BOOL CPmwCntrItem::SetExtentAndRects( LPCRECT lpPosRect /*=NULL*/, LPCRECT lpClipRect /*=NULL*/ )
{
	CRect crPosition;

	/*
	// Set up for a call to SetExtent().
	*/
	OnGetItemPosition(crPosition);

	CSize size = crPosition.Size();

	// now convert this size to HIMETRIC units
	size.cx = XformHeightInPixelsToHimetric(NULL, size.cx);
	size.cy = XformWidthInPixelsToHimetric(NULL, size.cy);

	// use this size to set the extent of the object
	SetExtent(size);

	return SetItemRects(lpPosRect, lpClipRect);
}

/////////////////////////////////////////////////////////////////////////////
// CPmwCntrItem diagnostics

#ifdef _DEBUG
void CPmwCntrItem::AssertValid() const
{
	COleClientItem::AssertValid();
}

void CPmwCntrItem::Dump(CDumpContext& dc) const
{
	COleClientItem::Dump(dc);
}
#endif

/////////////////////////////////////////////////////////////////////////////
