//////////////////////////////////////////////////////////////////////////////
// $Header: /PM8/App/PICTPRVW.CPP 1     3/03/99 6:09p Gbeddow $
// 
//  "This unpublished source code contains trade secrets which are the
//	property of Mindscape, Inc.  Unauthorized use, copying or distribution
//	is a violation of international laws and is strictly prohibited."
// 
//		  Copyright © 1998 Mindscape, Inc. All rights reserved.
//
//
// $Log: /PM8/App/PICTPRVW.CPP $
// 
// 1     3/03/99 6:09p Gbeddow
// 
// 17    1/26/99 3:22p Rgrenfell
// Fixed a DC cleanup issue for GIF animation
// 
// 16    1/25/99 3:45p Rgrenfell
// Added support for displaying an animated GIF in the preview area.
// 
// 15    11/22/98 1:08a Psasse
// take out code preventing downloaded ads from being seen
// 
// 14    11/18/98 10:37p Psasse
// Fix blank string crash in PreviewNewGraphic
// 
// 13    11/17/98 4:13p Dennis
// Added check for valid filename
// 
// 12    11/05/98 9:16p Psasse
// A new function to return image size
// 
// 11    9/21/98 5:26p Jayn
// Broke PathManager out into a COM-style interface.
// 
// 10    7/28/98 4:44p Jayn
// Fixed a resource leak (hrgn). Won't try to draw if completely obscured.
// 
// 9     7/23/98 7:28p Hforman
// fixed border redraw problem: not setting rc.terminate_interrupts = TRUE
// 
// 8     5/29/98 4:51p Rlovejoy
// Init to force doc to front page.
// 
// 7     5/28/98 6:37p Rlovejoy
// Changes to accomodate previewing interior pages.
// 
// 6     4/20/98 10:53a Jayn
// Fixed interrupt case where it wasn't invalidating.
// 
// 5     3/26/98 10:09a Jayn
// Support for DPP (Serif) importing, previewing, etc.
// 
// 4     3/17/98 11:58a Rlovejoy
// Changes to handle scratch documents.
// 
// 3     2/03/98 6:38p Hforman
// more cleanup
// 
// 2     2/03/98 12:44p Hforman
// add "pmo" previewing, other significant mods
//
//////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "pmw.h"
#include "pmwdoc.h"
#include "utils.h"
#include "util.h"
#include "grafrec.h"
#include "grafobj.h"
#include "compfn.h"
#include "pmwcfg.h"
#include "prevrec.h"
#include "pictprvw.h"

// Framework file.
#include "GifAnimator.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

extern BYTE far screen_gamma_curve[];
extern BOOL standard_check_interrupt(VOIDPTR data);
extern CPalette * near pOurPal;

/////////////////////////////////////////////////////////////////////////////
// CDocumentPreview
CDocumentPreview::CDocumentPreview()
{
	m_pDoc = NULL;
	m_fCloseDoc = FALSE;
	m_fForceToFront = TRUE;

	m_pDialog = NULL;
	m_uPreviewID = 0;
	m_uDescriptionID = 0;
}

CDocumentPreview::~CDocumentPreview()
{
	CloseDocument();
}

void CDocumentPreview::CloseDocument(void)
{
	// Close any open document
	if (m_pDoc != NULL)
	{
		if (m_fCloseDoc)
		{
			m_pDoc->OnCloseDocument();
		}
		m_pDoc = NULL;
	}
}

void CDocumentPreview::SizeDocPreview(void)
{
	ErasePreview();

	PBOX Bounds;

	if (!GetBounds(&Bounds))
	{
		// Hide the window if we have no record to show
		CWnd* pPreviewWnd = GetPreviewWnd();
		if (pPreviewWnd != NULL)
		{
			pPreviewWnd->ShowWindow(SW_HIDE);
		}

		// Set an empty description.
		SetDescription(NULL, NULL);
	}
	else
	{
		// Set some initial bounds for this object
		int nFullWidth = m_crFullPreview.Width() - 2;
		int nFullHeight = m_crFullPreview.Height() - 2;

		PPNT dim;
		dim.x = scale_pcoord(Bounds.x1 - Bounds.x0, m_nXResolution, PAGE_RESOLUTION);
		dim.y = scale_pcoord(Bounds.y1 - Bounds.y0, m_nYResolution, PAGE_RESOLUTION);

		// Ensure we have legal x and y values (otherwise we can get division by zero errors below.
		if (dim.x == 0)
		{
			dim.x = 1;
		}
		if (dim.y == 0)
		{
			dim.y = 1;
		}

		// Scale to fit in the box
		int nWidth, nHeight;
		nHeight = nFullHeight;
		if ((nWidth = scale_pcoord(nHeight, dim.x, dim.y)) > nFullWidth)
		{
			nWidth = nFullWidth;
			nHeight = scale_pcoord(nWidth, dim.y, dim.x);
		}

		// Size the preview window to the new dimensions.
		SizePreviewWindow(nWidth, nHeight);
	}
}

BOOL CDocumentPreview::GetBounds(PBOX* pBounds)
{
	BOOL fResult = FALSE;

	if (m_pDoc != NULL)
	{
		if (m_fEncloseObjects)
		{
			PMGPageObjectPtr pObject;

			for (pObject = (PMGPageObjectPtr)m_pDoc->objects(); pObject != NULL; pObject = (PMGPageObjectPtr)pObject->next_object())
			{
				PBOX ObjBound;

				ObjBound = ((RectPageObjectPtr)pObject)->get_bound();

				if (fResult)
				{
					UnionBox(pBounds, pBounds, &ObjBound);
				}
				else
				{
					*pBounds = ObjBound;
					fResult = TRUE;
				}
			}
		}
		else
		{
			if (m_fForceToFront)
			{
				m_pDoc->GetPreviewBound(pBounds);
			}
			else
			{
				m_pDoc->CPmwDoc::get_panel_world(pBounds);
			}

			fResult = TRUE;
		}
	}

	// If we got nothing, return an empty bound.
	if (!fResult)
	{
		pBounds->x0 =
			pBounds->y0 =
			pBounds->x1 =
			pBounds->y1 = 0;
	}
	return fResult;
}

BOOL CDocumentPreview::Initialize(CWnd* pDialog, UINT uPrevID, UINT uDescID /*=0*/)
{
	ASSERT(pDialog != NULL);

	// Set the variables

	m_pDialog = pDialog;
	m_uPreviewID = uPrevID;
	m_uDescriptionID = uDescID;

	CWnd* pPreviewWnd = GetPreviewWnd();
	if (pPreviewWnd == NULL)
	{
		ASSERT(FALSE);
		return FALSE;
	}

	// Get the resolution of the dialog.
	CDC *pDC = pDialog->GetDC();
	m_nXResolution = pDC->GetDeviceCaps(LOGPIXELSX);
	m_nYResolution = pDC->GetDeviceCaps(LOGPIXELSY);
	pDialog->ReleaseDC(pDC);

	// Get the bound for the window.
	pPreviewWnd->GetWindowRect(&m_crFullPreview);
	pDialog->ScreenToClient(&m_crFullPreview);
	return TRUE;
}

CWnd* CDocumentPreview::GetDialog(void)
{
	return m_pDialog;
}

//
// Get the preview window.
// This may be temporary, and should not be permanently stored.
//

CWnd* CDocumentPreview::GetPreviewWnd(void)
{
	return m_pDialog->GetDlgItem(m_uPreviewID);
}

//
// Get the description window.
// This may be temporary, and should not be permanently stored.
//

CWnd* CDocumentPreview::GetDescriptionWnd(void)
{
	return (m_uDescriptionID == 0) ? NULL : m_pDialog->GetDlgItem(m_uDescriptionID);
}

void CDocumentPreview::SizePreviewWindow(int nWidth, int nHeight)
{
	m_crPreview.left = (m_crFullPreview.left+m_crFullPreview.right-nWidth)/2;
	m_crPreview.top = (m_crFullPreview.top+m_crFullPreview.bottom-nHeight)/2;
	m_crPreview.right = m_crPreview.left + nWidth;
	m_crPreview.bottom = m_crPreview.top + nHeight;

	CRect r = m_crPreview;

	// We can't both set and show the window at the same time
	// with SetWindowPos. So, we call it twice as needed.
	CWnd* pPreviewWnd = GetPreviewWnd();
	pPreviewWnd->SetWindowPos(NULL, r.left-1, r.top-1, nWidth+2, nHeight+2,
									  SWP_NOZORDER | SWP_NOREDRAW);

	pPreviewWnd->SetWindowPos(NULL, 0, 0, 0, 0,
									  SWP_NOMOVE | SWP_NOSIZE | SWP_NOREDRAW | SWP_SHOWWINDOW);

	// Invalidate ourselves to avoid background draw.
	CWnd* pDialog = GetDialog();
	pDialog->ClientToScreen(&m_crPreview);
	pPreviewWnd->ScreenToClient(&m_crPreview);
	r = m_crPreview;
	r.left--;
	r.top--;
	r.right++;
	r.bottom++;
	pPreviewWnd->InvalidateRect(&r, FALSE);
}

void CDocumentPreview::ErasePreview(void)
{
	CRect r = m_crPreview;
	r.InflateRect(1, 1);

	CWnd* pPreviewWnd = GetPreviewWnd();
	CWnd* pDialog = GetDialog();
	if (pPreviewWnd != NULL && pDialog != NULL)
	{
		pPreviewWnd->ClientToScreen(&r);
		pDialog->ScreenToClient(&r);
		pDialog->InvalidateRect(&r);
	}
}

void CDocumentPreview::DrawFrame(HDC hDC)
{
	HPEN hOldPen = (HPEN)::SelectObject(hDC, ::GetStockObject(BLACK_PEN));
	HBRUSH hOldBrush = (HBRUSH)::SelectObject(hDC, ::GetStockObject(WHITE_BRUSH));
	::Rectangle(hDC,
					m_crPreview.left-1,
					m_crPreview.top-1,
					m_crPreview.right+1,
					m_crPreview.bottom+1);
	::SelectObject(hDC, hOldBrush);
	::SelectObject(hDC, hOldPen);
}

void CDocumentPreview::DrawPreview(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	// Find enclosing bounds for isolating objects
	PBOX Bounds;
	if (GetPreviewWnd() == NULL || !GetBounds(&Bounds))
	{
		// can not continue.
		return;
	}

	HDC hdc = lpDrawItemStruct->hDC;

	// If we are totally clipped, skip it.
	RECT r;
	if (GetClipBox(hdc, &r) == NULLREGION)
	{
		return;
	}

	// Draw a nice frame around our image
	if (!m_fEncloseObjects)
	{
		DrawFrame(hdc);
	}

	PPNT Dims;
	Dims.x = Bounds.x1 - Bounds.x0;
	Dims.y = Bounds.y1 - Bounds.y0;

	RedisplayContext rc;

	rc.destination_hdc = hdc;

	rc.hwnd = lpDrawItemStruct->hwndItem;
	rc.set_info(rc.destination_hdc);

	// Establish the palettes if necessary.
	HPALETTE hOurPal = (HPALETTE)pOurPal->GetSafeHandle();
	HPALETTE hOldPal = NULL;
	if (rc.info.type == RASTER_TYPE_PALETTE)
	{
		hOldPal = SelectPalette(rc.destination_hdc, hOurPal, FALSE);
		RealizePalette(rc.destination_hdc);
	}

	CRect crBounds = m_crPreview;
	CRgn region;
	region.CreateRectRgn(crBounds.left, crBounds.top, crBounds.right, crBounds.bottom);
	HRGN hrgn = region;
	::SelectClipRgn(hdc, hrgn);
	rc.clip_region = hrgn;

	rc.x_resolution = m_nXResolution;
	rc.y_resolution = m_nYResolution;

	rc.scaled_source_x0 =  Bounds.x0*rc.x_resolution;
	rc.scaled_source_y0 =  Bounds.y0*rc.y_resolution;

	rc.destination_x0 = crBounds.left;
	rc.destination_y0 = crBounds.top;

	rc.destination_rect = crBounds;

	rc.clip_rect = rc.destination_rect;

	rc.outline_gamma_curve =
		rc.bitmap_gamma_curve = screen_gamma_curve;

	rc.source_pbox = Bounds;

	PCOORD Scale;
	int n;
	if (Dims.x > Dims.y)
	{
		Scale = scale_pcoord(Dims.x, rc.x_resolution, PAGE_RESOLUTION);
		n = crBounds.Width();
	}
	else
	{
		Scale = scale_pcoord(Dims.y, rc.y_resolution, PAGE_RESOLUTION);
		n = crBounds.Height();
	}

	while (Scale > 32000)
	{
		n >>= 1;
		Scale >>= 1;
	}
	if (n == 0)
	{
		n++;
	}

	rc.SetScaling(n, (USHORT)Scale);

	// Install the abort interrupt check
	rc.set_check_interrupt(standard_check_interrupt, (VOIDPTR)&rc);

	REFRESH_EXTENT re;

	// Build the refresh extent.
	re.extent = rc.source_pbox;
	re.refresh_type = REFRESH_ALL;
	re.my_object = NULL;
	re.update_object = NULL;
	re.update_state = NULL;

	BOOL fDrawn = (m_fForceToFront) ? 
		!m_pDoc->DrawPreview(&re, &rc) : !m_pDoc->CPmwDoc::DrawPreview(&re, &rc);
	if (!fDrawn)
	{
		UpdateState* ustate = re.update_state;
		if (ustate != NULL)
		{
			switch (ustate->type)
			{
				case UPDATE_TYPE_Interrupted:
				{
					// This should not happen...
					delete ustate;
					// Fall through to...
				}
				case UPDATE_TYPE_Terminated:
				{
					// Try again later
					break;
				}
				default:
				{
					// Ignore other types (e.g. "error")
					fDrawn = TRUE;
					break;
				}
			}
		}
		if (!fDrawn)
		{
			::InvalidateRect(lpDrawItemStruct->hwndItem, &m_crPreview, FALSE);
		}
	}

	if (hOldPal != NULL)
	{
		SelectPalette(rc.destination_hdc, hOldPal, FALSE);
	}
}

//
// Set the description.
//

void CDocumentPreview::SetDescription(LPCSTR pszFileName, LPCSTR pszFriendlyName)
{
	CWnd* pDescriptionWnd = GetDescriptionWnd();
	if (pDescriptionWnd != NULL)
	{
		CString csName;
		if (pszFriendlyName != NULL)
		{
			// Use the friendly name since we have one.
			csName = pszFriendlyName;
		}
		else if (pszFileName != NULL)
		{
			// No friendly name. Use the base name of the file.
			Util::SplitPath(pszFileName, NULL, &csName);
		}
		// Set whatever we got.
		pDescriptionWnd->SetWindowText(csName);
	}
}

//
// Here is a new document to preview.
//

void CDocumentPreview::NewDocPreview(LPCSTR pszFileName, LPCSTR pszFriendlyName /*=NULL*/)
{
	// Get rid of any existing preview.
	ErasePreview();

	// Close any existing document.
	if (m_pDoc != NULL)
	{
		CloseDocument();
	}

	// Open the new document.
	CString strName = GetGlobalPathManager()->LocatePath(pszFileName);
	m_pDoc = GET_PMWAPP()->OpenHiddenDocument(strName);
	m_fCloseDoc = TRUE;
	m_fForceToFront = TRUE;
	m_fEncloseObjects = FALSE;
	SetDescription(pszFileName, pszFriendlyName);
	SizeDocPreview();
}

//
// Initialize the preview for a new document (already opened).
//

void CDocumentPreview::NewDocPreview(CPmwDoc* pDoc, BOOL fEncloseObjects /*=FALSE*/)
{
	m_pDoc = pDoc;

	m_fCloseDoc = FALSE;
	m_fEncloseObjects = fEncloseObjects;
	m_fForceToFront = TRUE;
	SetDescription(NULL, NULL);
	SizeDocPreview();
}

/////////////////////////////////////////////////////////////////////////////
// CPicturePreview

CPicturePreview::CPicturePreview()
{
	m_bIsAnimatedGIF = FALSE;
	m_pGIFAnimator = NULL;
}

CPicturePreview::~CPicturePreview()
{
	if (m_pGIFAnimator != NULL)
	{
		delete m_pGIFAnimator;
		m_pGIFAnimator = NULL;
	}
}

//
// Initialize the preview for a new graphic image.
// 'name' may be NULL (indicating no graphic).
//

void CPicturePreview::NewGraphicPreview(LPCSTR pszName, LPCSTR pszFriendlyName /*=NULL*/, LPBITMAPINFO pDib /*=NULL*/)
{
	// If we have no preview window, we have nothing.
	if (GetPreviewWnd() == NULL)
	{
		return;
	}

	// Erase the preview area.
	ErasePreview();

	// Make sure we have a document.
	if (!CreateDocument())
	{
		return;
	}

	//if (lstrcmp(pszName, "") != 0)
	//{
		if (pszName != NULL || pDib != NULL) //&& Util::FileExists(pszName))
		{
			GRAPHIC_CREATE_STRUCT* pGCS = m_pDoc->get_gcs();

			pGCS->proof = FALSE;

			if (pDib != NULL)
			{
				// We have DIB to construct from.
				pGCS->m_pTempDIB = pDib;
				pszFriendlyName = "";
			}

			m_pDoc->SetImportPictureName(pszName, pszFriendlyName);

			// Create the chosen picture.
			m_pDoc->create_chosen_picture();

			// Check to see if the object loaded was an animated GIF image.
			CheckForAnimatedGIF();

			// We want to compute the enclosing bound for the objects.
			m_fEncloseObjects = TRUE;

			// And size the preview window.
			SizeDocPreview();

			// Set the description.
			SetDescription(pszName, pszFriendlyName);
		}
	//}
}

BOOL CPicturePreview::CreateDocument(void)
{
	if (m_pDoc == NULL)
	{
		m_pDoc = GET_PMWAPP()->NewHiddenDocument();
		if (m_pDoc != NULL)
		{
			m_fCloseDoc = TRUE;
			m_pDoc->size_to_paper();
		}
	}
	else
	{
		m_pDoc->deselect_all();
		m_pDoc->object_list()->destroy_objects();
	}
	return (m_pDoc != NULL);
}

BOOL CPicturePreview::GetImageSize(PBOX* pBox)
{
	GetBounds(pBox);

	return TRUE;
}


/* This method checks the current object to see if it is an animated GIF and
	initializes the preview control if it is.
*/
void CPicturePreview::CheckForAnimatedGIF()
{
	// Assume it isn't an animated GIF so we don't have to catch this in multiple locations.
	m_bIsAnimatedGIF = FALSE;

	PMGPageObjectPtr pObject = m_pDoc->objects();
	if (pObject != NULL)
	{
		if (pObject->IsAnimatedGIF())
		{
			// Kill our old GIF animator if present.
			if (m_pGIFAnimator != NULL)
			{
				delete m_pGIFAnimator;
			}
			// Get a new GIF animator from the preview object.
			m_pGIFAnimator = pObject->CreateGIFAnimator();

			m_bIsAnimatedGIF = TRUE;
		}
	}
}


/* This method renders the appropriate frame of the attached animated GIF if
	present.  If sufficient time has not yet gone by to warrent drawing the next
	frame of the animation, this will re-render the current frame.

	Returns : IP_ANIMATION_COMPLETE if the final frame has been rendered and the
				 loop count has expired.
				 IP_ANIMATION_CONTINUES if there are further frames to be rendered.
*/
UINT CPicturePreview::Update(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	// Find enclosing bounds for isolating objects
	if (GetPreviewWnd() == NULL)
	{
		// can not continue.
		return IP_ANIMATION_COMPLETE;
	}

	// If we are totally clipped, skip it.
	RECT r;
	if (GetClipBox(lpDrawItemStruct->hDC, &r) == NULLREGION)
	{
		return IP_ANIMATION_CONTINUES;	// We can continue, we just don't need an update.
	}

	// Draw a nice frame around our image
	if (!m_fEncloseObjects)
	{
		DrawFrame(lpDrawItemStruct->hDC);
	}

	// Create a CDC from the draw item structure that the GIF animator can use
	// to render the current frame for us.
	CDC dcView;
	dcView.Attach( lpDrawItemStruct->hDC );

	// Have the GIF animator render the current frame.
	UINT uContinuationCode;
	if (m_pGIFAnimator->Update( &dcView, m_crPreview ))
	{
		uContinuationCode = IP_ANIMATION_CONTINUES;
	}
	else
	{
		uContinuationCode = IP_ANIMATION_COMPLETE;
	}

	// Release the Device.
	dcView.Detach();

	return uContinuationCode;
}



/* This method checks to see if an update is needed for GIF animation.  This
	checks the system clock to see if sufficient time has passed since the last
	frame was rendered to require the next frame to render.

	Returns : TRUE if an update is needed, FALSE if not.
*/
bool CPicturePreview::NeedsUpdate()
{
	return m_pGIFAnimator->NeedsUpdate();
}


/* Sizes the preview window based on the size of the embedded image.  This is
	over-ridden from the main DocPreview so that GIF animations aren't scaled
	up, only down.  This will allow the animation to be correctly centered.
*/
void CPicturePreview::SizeDocPreview(void)
{
	if (IsAnimatedGIF())
	{
		ErasePreview();

		// Determine our current image animation.
		CSize sizeImage = m_pGIFAnimator->GetGIFScreenSize();
		
		// Set some initial bounds for this object
		int nFullWidth = m_crFullPreview.Width() - 2;
		int nFullHeight = m_crFullPreview.Height() - 2;

		int nWidth, nHeight;

		if ( (sizeImage.cx > m_crFullPreview.Width()) ||
			  (sizeImage.cy > m_crFullPreview.Height()) )
		{
			// Ensure we have legal x and y values (otherwise we can get division by zero errors below.
			if (sizeImage.cx == 0)
			{
				sizeImage.cx = 1;
			}
			if (sizeImage.cy == 0)
			{
				sizeImage.cy = 1;
			}

			// Scale to fit in the box
			nHeight = nFullHeight;
			if ((nWidth = scale_pcoord(nHeight, sizeImage.cx, sizeImage.cy)) > nFullWidth)
			{
				nWidth = nFullWidth;
				nHeight = scale_pcoord(nWidth, sizeImage.cy, sizeImage.cx);
			}
		}
		else
		{
			// Don't scale if we are smaller than the preview area.
			nWidth = sizeImage.cx;
			nHeight = sizeImage.cy;
		}

		// Size the preview window to the new dimensions.
		SizePreviewWindow(nWidth, nHeight);
	}
	else
	{
		// It isn't an animated GIF so call the parent doc sizing method.
		CDocumentPreview::SizeDocPreview();
	}
}
