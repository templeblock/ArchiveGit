// $Workfile: Borders.cpp $
// $Revision: 2 $
// $Date: 3/08/99 1:17p $
//
//  "This unpublished source code contains trade secrets which are the
//   property of Mindscape, Inc.  Unauthorized use, copying or distribution
//   is a violation of international laws and is strictly prohibited."
// 
//        Copyright © 1998 Mindscape, Inc. All rights reserved.
//
//
// $Log: /PM8/App/Borders.cpp $
// 
// 2     3/08/99 1:17p Rgrenfel
// Updated to use component objects in place of graphic objects.
// 
// 27    3/05/99 6:47p Rgrenfel
// Updated to use component objects in plae of graphic objects.
// 
// 26    11/19/98 2:54p Mwilson
// added call to change toolbar for border editor
// 
// 25    10/29/98 4:04p Jayn
// Fixed an MFC DLL crash problem.
// 
// 24    9/23/98 10:27a Jayn
// Converted CollectionManager to COM-style object.
// 
// 23    9/21/98 5:25p Jayn
// Broke PathManager out into a COM-style interface.
// 
// 22    9/14/98 12:05p Jayn
// Removed system_heap. Switched to MFC in DLL.
// 
// 21    8/10/98 5:01p Hforman
// 
// 20    7/21/98 8:34a Jayn
// Fixed a bug in Duplicate().
// 
// 19    7/17/98 6:55p Rlovejoy
// Added code to ensure duplicate would work when databases are not equal.
// 
// 18    7/17/98 6:45p Rlovejoy
// Don't call 'duplicate_record()' to dup a graphic.
// 
// 17    6/02/98 4:12p Rlovejoy
// Code to remove helpful hint from Border Editor.
// 
// 16    6/02/98 1:19p Rlovejoy
// Add DisplayHelpfulHint() to the callback interface.
// 
// 15    4/16/98 7:07p Hforman
// change params to ImportItem()
// 
// 14    4/13/98 12:13p Hforman
// modified params to ImportItem()
// 
// 13    2/27/98 6:44p Hforman
// fixed bug: wasn't deselecting a brush in DC
// 
// 12    2/27/98 2:45p Jayn
// Clipping for border background.
// 
// 11    2/25/98 5:34p Hforman
// fix a couple of problems
// 
// 10    2/24/98 1:36p Hforman
// various fixes, additions, etc.
// 
// 9     2/20/98 8:03p Hforman
// 
// 8     2/19/98 6:44p Hforman
// background color, etc.
// 
// 7     2/17/98 7:26p Hforman
// interrupting, background color, etc.
// 
// 6     2/12/98 1:16p Hforman
// add image caching, save border functionality
// 
// 5     2/03/98 12:49p Hforman
// fix interrupt problems, other minor fixes
// 
// 4     1/13/98 6:13p Hforman
// interim checkin
// 
// 3     1/09/98 6:52p Hforman
// 
// 2     1/09/98 11:05a Hforman
// interim checkin
// 
// 1     1/06/98 6:53p Hforman
//

//
// Borders.cpp -- implements client-side API for the Border Editor
//

#include "stdafx.h"
#include "pmw.h"
#include "refresh.h"
#include "compobj.h"
#include "CompRec.h"
#include "CreateData.h"
#include "objimg.h"
#include "utils.h"
#include "dibbrush.h"
#include "dib2file.h"
#include "svborder.h"
#include "borders.h"
#include "pmwcfg.h"

#ifdef _DEBUG
	#define new DEBUG_NEW
	#undef THIS_FILE
	static char THIS_FILE[] = __FILE__;
#endif

extern CPalette* pOurPal;

// Local function. May be globally useful someday (e.g. in the Mac code).
static void ClipPolyPolygon(CArray<CPoint, CPoint&>& caPoints,
									 CArray<int, int>& caPtsInPoly,
									 CRect crClip);

///////////////////////////////////////////////////////////////////////////
// CBorderEditor class
//
// Used to initialize/shutdown the Border Editor DLL. Create at startup.
//
///////////////////////////////////////////////////////////////////////////
CBorderEditor::CBorderEditor()
{
	m_hLibrary = NULL;
	m_fIsInitialized = FALSE;

	m_pPMInterface  = new CBECallbackInterfaceObj;
}

CBorderEditor::~CBorderEditor()
{
	delete m_pPMInterface;
}

BOOL CBorderEditor::StartupBorderEditor()
{
	m_fIsInitialized = FALSE;

	// load the border editor library -- must be in exe path
	CString libpath = GetGlobalPathManager()->ExpandPath("border.dll");
	m_hLibrary = LoadLibrary(libpath);
	if (m_hLibrary)
	{
		// initialize Border Editor
		typedef int FAR PASCAL BEInitFunc(CBECallbackInterface*, CBEInterface*&);
		BEInitFunc* InitFunc = (BEInitFunc*)GetProcAddress(m_hLibrary, "Initialize");
		if (InitFunc)
		{
			if ((*InitFunc)(m_pPMInterface, m_pBEInterface) == 0)
				m_fIsInitialized = TRUE;
			else
				AfxMessageBox("InitFunc failed");
		}
		else
			AfxMessageBox("InitFunc missing");
	}
	else
	{
		DWORD dwError = ::GetLastError();
		CString csMessage;
		csMessage.Format("Unable to load '%s' (%ld)",
							  (LPCSTR)libpath, dwError);
		AfxMessageBox(csMessage);
	}

	return m_fIsInitialized;
}

void CBorderEditor::ShutdownBorderEditor()
{
	if (m_hLibrary)
	{
		m_pBEInterface->Release();
		FreeLibrary(m_hLibrary);
		m_hLibrary = NULL;
		m_fIsInitialized = FALSE;
	}
}

///////////////////////////////////////////////////////////////////////////
// CBECallbackInterfaceObj class
//
// Functions used by the Border Editor DLL
//
///////////////////////////////////////////////////////////////////////////

CBECallbackInterfaceObj::CBECallbackInterfaceObj()
{
	m_pModuleState = AfxGetModuleState();
}

CBECallbackInterfaceObj::~CBECallbackInterfaceObj()
{
	m_pModuleState = NULL;
}

int CBECallbackInterfaceObj::Release()
{
	AFX_MANAGE_STATE(m_pModuleState);
	// free all our cached images
	POSITION pos = m_ImageCache.GetHeadPosition();
	while (pos != NULL)
	{
		ImageCacheData* pData = m_ImageCache.GetNext(pos);
		delete pData->pImage;
		delete pData;
	}
	m_ImageCache.RemoveAll();

	return BorderSuccess;
}

HBITMAP CBECallbackInterfaceObj::GetToolbarBitmap()
{
	AFX_MANAGE_STATE(m_pModuleState);
	HBITMAP hResult = NULL;
	if(GetConfiguration()->Product() == CPrintMasterConfiguration::plAmericanGreetings)
	{
		CBitmap bmp;
		bmp.LoadBitmap(IDB_BORDER_TOOLBAR);
		hResult = (HBITMAP)bmp;
		bmp.Detach();
		ASSERT(hResult);
	}

	return hResult;
}

int CBECallbackInterfaceObj::NewGraphic(DWORD& dwID, void* pDatabase,
													 LPCSTR pszFileName,
													 LPCSTR pszFriendlyName/*=NULL*/,
													 BOOL fCropCopyright/*=FALSE*/)
{
	AFX_MANAGE_STATE(m_pModuleState);
	if (!pDatabase)
	{
		ASSERT(0);
		return BorderInvalidParams;
	}

	PMGDatabasePtr database = (PMGDatabasePtr)pDatabase;

	GRAPHIC_CREATE_STRUCT gcs;
	gcs.proof = FALSE;
	gcs.m_csFileName = pszFileName;
	gcs.m_csFriendlyName = pszFriendlyName;
	gcs.m_fSpecialCrop = fCropCopyright;

	// Create a component object based on the new graphic data.
	CGraphicData cData( &gcs );
	DatabaseRecordPtr pRecord = NULL;
	ERRORCODE error = database->new_record( RECORD_TYPE_Component, &cData, &pRecord );

	if ((dwID = pRecord->Id()) == 0)
		return BorderError;
	else
		return BorderSuccess;
}
// Duplicate a graphic reference
int CBECallbackInterfaceObj::Duplicate(DWORD& dwDupID, void* pDupDatabase,
													DWORD dwID, void* pDatabase)
{
	AFX_MANAGE_STATE(m_pModuleState);
	if (dwID == 0 || !pDatabase)
	{
		ASSERT(0);
		return BorderInvalidParams;
	}

	PMGDatabasePtr pDupDB = (PMGDatabasePtr)pDupDatabase;
	PMGDatabasePtr pDB = (PMGDatabasePtr)pDatabase;

	ERRORCODE	error;

	// Get our record if we can.
	ComponentPtr pCompRec;
	if ((pCompRec = (ComponentPtr)pDB->get_record(dwID,
						&error, RECORD_TYPE_Component)) != NULL)
	{
		// Duplicate the record.
		// Extreme care must be taken to maintain the correct database at all times.
		ComponentPtr pNewGraphic = (ComponentPtr)pCompRec->duplicate(pDupDB, &error);

		// See if we got the new graphic.
		if (pNewGraphic != NULL)
		{
			// Store the ID in the variable we were provided.
			dwDupID = pNewGraphic->Id();

			pNewGraphic->release();
		}

		// Release our component record.
		pCompRec->release();
	}

	if (dwDupID == 0 || error != ERRORCODE_None)
		return BorderError;
	else
		return BorderSuccess;
}

int CBECallbackInterfaceObj::AddRef(DWORD dwID, void* pDatabase)
{
	AFX_MANAGE_STATE(m_pModuleState);
	if (dwID == 0 || !pDatabase)
	{
		ASSERT(0);
		return BorderInvalidParams;
	}

	PMGDatabasePtr database = (PMGDatabasePtr)pDatabase;
	if (database->inc_component_record(dwID) != ERRORCODE_None)
		return BorderError;
	else
		return BorderSuccess;
}

int CBECallbackInterfaceObj::RemoveRef(DWORD dwID, void* pDatabase)
{
	AFX_MANAGE_STATE(m_pModuleState);
	if (dwID == 0 || !pDatabase)
	{
		ASSERT(0);
		return BorderInvalidParams;
	}

	PMGDatabasePtr database = (PMGDatabasePtr)pDatabase;
	if (database->free_component_record(dwID) != ERRORCODE_None)
		return BorderError;
	else
		return BorderSuccess;
}

// launch the ArtBrowser so user can choose a graphic
int CBECallbackInterfaceObj::ChooseGraphic(HWND hParent, DWORD& dwID, void* pDatabase)
{
	AFX_MANAGE_STATE(m_pModuleState);
	dwID = 0;

	if (!pDatabase)
	{
		ASSERT(0);
		return BorderInvalidParams;
	}

	int retval = BorderSuccess;

	PMGDatabasePtr database = (PMGDatabasePtr)pDatabase;
	CPmwDoc* pDoc = (CPmwDoc*)database->GetDocument();
	if (pDoc)
	{
		if (pDoc->ChoosePicture(NULL, NULL, CWnd::FromHandle(hParent)) == IDOK)
		{
			GRAPHIC_CREATE_STRUCT_PTR p_gcs = pDoc->get_gcs();
			CGraphicData cData( p_gcs );
			DatabaseRecordPtr pRecord = NULL;
			ERRORCODE error = database->new_record( RECORD_TYPE_Component, &cData, &pRecord );
			dwID = pRecord->Id();
			if (dwID == 0)
				retval = BorderError;
		}
	}
	else
		retval = BorderError;

	return retval;
}

// pass back width/height of graphic (in inches)
int CBECallbackInterfaceObj::GetGraphicInfo(DWORD dwID, void* pDatabase,
														  CBEGraphicInfo& Info)
{
	AFX_MANAGE_STATE(m_pModuleState);
	if (dwID == 0 || !pDatabase)
	{
		ASSERT(0);
		return BorderInvalidParams;
	}

	PMGDatabasePtr database = (PMGDatabasePtr)pDatabase;
	ERRORCODE	error;
	ComponentPtr graphic =
		(ComponentPtr)database->get_record(dwID, &error, RECORD_TYPE_Component);
	if (graphic == NULL || error != ERRORCODE_None)
		return BorderError;

	// Attach the record to a component temporarily to allow us to query it.
	ComponentObject *pCompObject = database->create_component_object();
	if (pCompObject == NULL)
		return BorderError;
	pCompObject->SetRecordID( dwID );

	// Get the object size information.
	PPNT ObjectDims;
	pCompObject->original_dims(&ObjectDims);
	Info.m_dXSize = (double)ObjectDims.x; //  / (double)graphic->record.x_resolution;
	Info.m_dYSize = (double)ObjectDims.y; //  / (double)graphic->record.y_resolution;

	// Detach the record from the component.
	pCompObject->DetachRecord();
	pCompObject->destroy();			// Remove from the document.
	delete pCompObject;

	graphic->release();

	return BorderSuccess;
}

//////////////////////////////////////////////////////////////////////////////
// DrawGraphic()
//
// Draw the specific graphic with the given drawing parameters.
//
// This function is used to draw individual graphics within a border object,
// both in PrintMaster and in the Border Editor.
//
// Notes on the image cache used in this function:
//
// The cache is a linked list, with new elements being added to the head of
// the list. The list is limited to MAX_IMAGE_CACHE_SIZE elements, and when
// this is exceeded, the tail of the list is deleted. For this reason, when
// a matching image is found, it is moved to the head of the list.
//
//////////////////////////////////////////////////////////////////////////////
int CBECallbackInterfaceObj::DrawGraphic(DWORD dwID, void* pDatabase,
													  CBEDrawInfo* pDrawInfo)
{
	AFX_MANAGE_STATE(m_pModuleState);
	if (!pDatabase || !pDrawInfo)
	{
		ASSERT(0);
		return BorderInvalidParams;
	}

	PMGDatabasePtr database = (PMGDatabasePtr)pDatabase;

	RedisplayContext rc;

	if (pDrawInfo->m_pClientData)
	{
		// draw request in from PrintMaster, so we have an rc to use
		rc = *(RedisplayContextPtr)pDrawInfo->m_pClientData;
	}
	else
	{
		// Build the redisplay context.
		rc.destination_hdc = pDrawInfo->m_hdc;
		rc.hwnd = pDrawInfo->m_hWnd;
		rc.using_images = TRUE;
		rc.scaled_source_x0 = rc.scaled_source_y0 = 0;
		rc.destination_x0 = rc.destination_y0 = 0;
		rc.destination_rect = pDrawInfo->m_Bounds;

		// Set the resolutions to that of the page.
		extern BYTE screen_gamma_curve[];
		rc.x_resolution = rc.y_resolution = PAGE_RESOLUTION;
		rc.bitmap_gamma_curve = rc.outline_gamma_curve = screen_gamma_curve;

		if (pDrawInfo->m_fAllowInterrupt)
			rc.set_check_interrupt(standard_check_interrupt, (VOIDPTR)&rc);

		// Initialize the redisplay context so we can use it. */
		rc.set_info(pDrawInfo->m_hdc);
	}

	// see if there's anything for us to draw
	if (pDrawInfo->m_pClip)
	{
		// rotate bounds and convert to screen coords
		CRect rcBounds = pDrawInfo->m_Bounds;
		PBOX bound = { rcBounds.left, rcBounds.top, rcBounds.right, rcBounds.bottom };
		PPNT center = { (rcBounds.left + rcBounds.right) / 2,
							 (rcBounds.top + rcBounds.bottom) / 2 };
		PBOX rotbound;
		compute_rotated_bound(bound, pDrawInfo->m_dAngle, &rotbound, center);

		rc.pbox_to_screen(&rotbound, FALSE);
		rc.convert_pbox(&rotbound, &rcBounds, NULL);

		CRect rect;
		if (!rect.IntersectRect(rcBounds, pDrawInfo->m_pClip))
			return BorderSuccess;
	}

	CDC* pDC = CDC::FromHandle(rc.destination_hdc);

	// Realize our palette
	CPalette* pOldPal = pDC->SelectPalette(pOurPal, FALSE);
	pDC->RealizePalette();

	RectPageObjectPtr object = NULL;
	UpdateStatePtr ustate = NULL;
	BOOL fCachedImage = FALSE;

	PBOX objBound;
	CRect scrnRect;

	if (pDrawInfo->m_pDrawState)
	{
		// Re-entering after an interrupt...
		// Extract object info so we can restart where we left off
		SavedState* pSavedState = (SavedState*)pDrawInfo->m_pDrawState;
		object = pSavedState->m_pPageObj;
		ustate = pSavedState->m_pUpdate;
		objBound = pSavedState->m_bound;
		scrnRect = pSavedState->m_scrnRect;

		delete pSavedState;
		pDrawInfo->m_pDrawState = NULL;
	}
	else
	{
		// Draw background color (if not transparent) for the Border Editor.
		// (Background on the PrintMaster side is drawn via DrawBackground())
		if (pDrawInfo->m_pClientData == NULL && pDrawInfo->m_clBackground != -1)
		{
			CRect rect = pDrawInfo->m_Bounds;
			if (pDrawInfo->m_dAngle)
			{
				// border editor can rotate graphics by 90 degrees only
				ANGLE angle = normalize_angle(pDrawInfo->m_dAngle);
				int intangle = int(angle * 100);
				int intpi = 314;	// int(PI * 100)
				if (intangle == intpi / 2 || intangle == intpi / 2 + intpi)
				{
					// rotate 90 degrees
					int diff = (rect.Width() - rect.Height() + 1) / 2;
					rect.SetRect(rect.left + diff, rect.top - diff,
									 rect.right - diff, rect.bottom + diff);
				}
			}

			CDibPatternBrush brush;
			brush.UsePalette(rc.info.type == RASTER_TYPE_PALETTE);
			CBrush* pOldBrush = NULL;
			if (brush.Color(pDrawInfo->m_clBackground))
			{
				pOldBrush = pDC->SelectObject(brush.Brush());
				pDC->FillRect(&rect, brush.Brush());
				pDC->SelectObject(pOldBrush);
			}

			// if no image to draw, restore palette and exit
			if (dwID == 0)
			{
				if (pOldPal)
					pDC->SelectPalette(pOldPal, FALSE);
				return BorderSuccess;
			}
		}

		// Create a graphic object, if we have a valid ID
		if (dwID == 0)
		{
			// we shouldn't be here if ID is 0
			ASSERT(0);
			return BorderInvalidParams;
		}

		object = database->create_component_object();
		if (object == NULL)
			return BorderError;

		// set graphic ID (which will be superfluous if we have a cached image)
		object->SetRecordID(dwID);

//		object->CropIfNeeded();

		RECT r = pDrawInfo->m_Bounds;
		PBOX pageBound = { r.left, r.top, r.right, r.bottom };
		object->set_unrotated_bound(pageBound);
		ANGLE angle = normalize_angle(pDrawInfo->m_dAngle);
		object->set_rotation(angle);

		if (pDrawInfo->m_fXFlipped)
			object->add_flags(OBJECT_FLAG_xflipped);
		if (pDrawInfo->m_fYFlipped)
			object->add_flags(OBJECT_FLAG_yflipped);

		object->calc();
		objBound = object->get_bound();

		// get screen rect
		PBOX pb = objBound;
		rc.pbox_to_screen(&pb, TRUE);
		rc.convert_pbox(&pb, &scrnRect, NULL);

		fCachedImage = FALSE;

		if (rc.using_images)
		{
			//
			// look for a cached image to use
			//
			POSITION pos = m_ImageCache.GetHeadPosition();
			while (pos != NULL && !fCachedImage)
			{
				ImageCacheData* pImageData = m_ImageCache.GetNext(pos);
				CRect bounds = pDrawInfo->m_Bounds;
				if (pImageData->dwID					== dwID							&&
					 pImageData->rcBounds.Width()	== scrnRect.Width()			&&
					 pImageData->rcBounds.Height()== scrnRect.Height()			&&
					 pImageData->pDB					== database						&&
					 pImageData->dAngle				== pDrawInfo->m_dAngle		&&
					 pImageData->fXFlipped			== pDrawInfo->m_fXFlipped	&&
					 pImageData->fYFlipped			== pDrawInfo->m_fYFlipped)
				{
					// we've found a match
					object->set_object_image(pImageData->pImage);
					object->add_refresh_flags(REFRESH_FLAG_has_image);
					object->remove_refresh_flags(REFRESH_FLAG_makes_image);

					// remove it from old position and move it to head of list
					// (unless it's already at the head!)
					if (pImageData != m_ImageCache.GetHead())
					{
						if (pos == NULL)
						{
							 // must be at tail of list
							m_ImageCache.RemoveTail();
						}
						else
						{
							m_ImageCache.GetPrev(pos);
							m_ImageCache.RemoveAt(pos);
						}

						m_ImageCache.AddHead(pImageData);
					}

					fCachedImage = TRUE;
				}
			}
		}
	
		// clip the object bound, if necessary
		if (pDrawInfo->m_pClip != NULL)
		{
			PBOX pbClip;
			rc.screen_to_pbox(pDrawInfo->m_pClip, &pbClip);
			IntersectBox(&objBound, &objBound, &pbClip);
		}
	}

	// set clip rect
	RECT clip;
	if (pDrawInfo->m_pClip == NULL)
		clip = scrnRect;
	else
		clip = *pDrawInfo->m_pClip;

	rc.clip_rect = clip;

	// if we found a cached image, try to draw it
	if (fCachedImage)
	{
		// Draw cached image if one exists
		ERRORCODE error = object->draw_image(&rc, objBound);
		if (error == ERRORCODE_None)
		{
			TRACE("Drew cached image!\n");
		}
		else
		{
	      // If error, fall thru and do normal drawing operation
			TRACE("Failed to draw cached image!\n");

			// make sure we don't remove the image in the object
			object->set_object_image(NULL);
			object->remove_refresh_flags(REFRESH_FLAG_has_image);
			object->add_refresh_flags(REFRESH_FLAG_makes_image);
			fCachedImage = FALSE;
		}
	}

	// normal drawing if no cached image, or cached draw failed
	if (!fCachedImage)
	{
		TRACE("** Drawing with no cache\n");
		ustate = object->update(&rc, &objBound, &clip, ustate, REFRESH_ALL);
	}

	if (ustate && (ustate->type == UPDATE_TYPE_Interrupted))
	{
		// interrupted -- save our state (sent back to Border DLL)
		SavedState* pSavedState = new SavedState;
		pSavedState->m_pUpdate = ustate;
		pSavedState->m_pPageObj = object;
		pSavedState->m_bound = objBound;
		pSavedState->m_scrnRect = scrnRect;

		pDrawInfo->m_pDrawState = pSavedState;
	}
	else if (ustate == NULL)
	{
		// if image wasn't cached, cache it now
		if (!fCachedImage)
		{
			ImageCacheData* pImageData = new ImageCacheData;

			pImageData->pDB = database;
			pImageData->dwID = dwID;
			pImageData->rcBounds = scrnRect;
			pImageData->dAngle = pDrawInfo->m_dAngle;
			pImageData->fXFlipped = pDrawInfo->m_fXFlipped;
			pImageData->fYFlipped = pDrawInfo->m_fYFlipped;
			pImageData->pImage = (CBitmapObjectImage*)object->get_object_image();

			fCachedImage = TRUE;

			// add new image to head of list
			m_ImageCache.AddHead(pImageData);

			// delete oldest from tail of list if list is maxed out
			if (m_ImageCache.GetCount() > MAX_IMAGE_CACHE_SIZE)
			{
				ImageCacheData* pImageDataToDelete = m_ImageCache.GetTail();
				delete pImageDataToDelete->pImage;
				delete pImageDataToDelete;
				m_ImageCache.RemoveTail();
			}
		}
	}
	else
	{
		// some error occurred -- forget it
		ustate = NULL;
	}

	// restore old palette
	if (pOldPal)
		pDC->SelectPalette(pOldPal, FALSE);

	if (ustate == NULL)
	{
		// if image is cached, set image to NULL in the object so it won't get deleted
		if (fCachedImage)
			object->set_object_image(NULL);

		// "Unattach" the record from the object so they won't get deleted
		object->DetachRecord();
		object->destroy();			// Remove from the document.
		delete object;
	}

	return BorderSuccess;
}

int CBECallbackInterfaceObj::DrawBackground(CBEBackgroundInfo* pBackgroundInfo)
{
	AFX_MANAGE_STATE(m_pModuleState);
	RedisplayContextPtr rc = NULL; // always NULL if drawing to BorderEditor
	CDC* pDC = NULL;

	CRect outrect = pBackgroundInfo->m_OuterBounds;
	CRect inrect = pBackgroundInfo->m_InnerBounds;

	if (pBackgroundInfo->m_pClientData)
	{
		// draw request in from PrintMaster, so we have an rc to use
		rc = (RedisplayContextPtr)pBackgroundInfo->m_pClientData;
		pDC = CDC::FromHandle(rc->destination_hdc);

		// convert to screen coords
		PBOX	outpbox = { outrect.left, outrect.top, outrect.right, outrect.bottom };
		rc->pbox_to_screen(&outpbox, TRUE);
		rc->convert_pbox(&outpbox, &outrect, NULL);

		PBOX	inpbox = { inrect.left, inrect.top, inrect.right, inrect.bottom };
		rc->pbox_to_screen(&inpbox, TRUE);
		rc->convert_pbox(&inpbox, &inrect, NULL);
	}
	else
	{
		pDC = CDC::FromHandle(pBackgroundInfo->m_hdc);
	}

	PPNT outppts[4] =
	{
		{outrect.left,  outrect.top},
		{outrect.right, outrect.top},
		{outrect.right, outrect.bottom},
		{outrect.left,  outrect.bottom},
	};

	PPNT inppts[4] =
	{
		{inrect.left,  inrect.top},
		{inrect.right, inrect.top},
		{inrect.right, inrect.bottom},
		{inrect.left,  inrect.bottom},
	};

	// rotate points if needed
	double dAngle = pBackgroundInfo->m_dAngle;
	if (dAngle != 0)
	{
		PPNT center = { (outrect.left + outrect.right) / 2,
							 (outrect.top + outrect.bottom) / 2 };
		rotate_points(outppts, 4, center, dAngle);
		rotate_points(inppts, 4, center, dAngle);
	}

	// create an array to hold our outer/inner rect points
	int pts_per_poly = 5;

	CArray<CPoint, CPoint&> pointArray;
	pointArray.SetSize(10);
	for (int i = 0; i < pts_per_poly; i++)
	{
		if (i < pts_per_poly - 1)
		{
			pointArray[i].x = outppts[i].x;
			pointArray[i].y = outppts[i].y;

			pointArray[i+5].x = inppts[i].x;
			pointArray[i+5].y = inppts[i].y;
		}
		else // last point --  close the rects
		{
			pointArray[i].x = outppts[0].x;
			pointArray[i].y = outppts[0].y;

			pointArray[i+5].x = inppts[0].x;
			pointArray[i+5].y = inppts[0].y;
		}
	}

	CArray<int, int> ptsPerPoly;
	ptsPerPoly.Add(pts_per_poly);
	ptsPerPoly.Add(pts_per_poly);

	if (rc)
	{
		// Clip polygons to the current destination clip rectangle.
		ClipPolyPolygon(pointArray, ptsPerPoly, rc->clip_rect);
	}

	if (ptsPerPoly.GetSize() > 0)
	{
		// Ready to draw our background...
		int nContext = pDC->SaveDC();
		if (nContext != 0)
		{
			// Select and realize our palette
			pDC->SelectPalette(pOurPal, FALSE);
			pDC->RealizePalette();

			CPen pen;
			pen.CreateStockObject(NULL_PEN);
			pDC->SelectObject(&pen);

			CDibPatternBrush brush;
			brush.UsePalette(rc ? (rc->info.type == RASTER_TYPE_PALETTE) : TRUE);
			if (brush.Color(pBackgroundInfo->m_clBackground))
				pDC->SelectObject(brush.Brush());

			if (rc && dAngle != 0)
			{
				// set rect to rotated bound so toggle() works (for PrintMaster side only)
				PBOX rotated_bound;
				compute_enclosing_bound(outppts, 4, &rotated_bound);
				outrect.SetRect(rotated_bound.x0, rotated_bound.y0,
									 rotated_bound.x1, rotated_bound.y1);
			}

			if (rc)
				rc->toggle(FALSE, &outrect);

			pDC->SetPolyFillMode(ALTERNATE);
			pDC->PolyPolygon(pointArray.GetData(), ptsPerPoly.GetData(), ptsPerPoly.GetSize());

			if (rc)
				rc->toggle(TRUE, &outrect);

			// Always use -1 in case of drawing to a metafile DC.
			pDC->RestoreDC(-1);
		}
	}

	return BorderSuccess;
}

HPALETTE CBECallbackInterfaceObj::GetPalette()
{
	AFX_MANAGE_STATE(m_pModuleState);
	return (HPALETTE)pOurPal->GetSafeHandle();
}

//
// Return the "clip sector" for this point.
//
//          |          |
// (-1, -1) | ( 0, -1) | ( 1, -1)
//          |          |
// ---------+----------+---------
//          |  Inside  |
// (-1,  0) | ( 0,  0) | ( 1,  0)
//          |   Clip   |
// ---------+----------+---------
//          |          |
// (-1,  1) | ( 0,  1) | ( 1,  1)
//          |          |

static CPoint GetClipSector(CPoint cp, CPoint& cpClipped, const CRect& crClip)
{
	CPoint cpOut;

	if (cp.x < crClip.left)
	{
		cpOut.x = -1;
		cpClipped.x = crClip.left;
	}
	else if (cp.x > crClip.right)
	{
		cpOut.x = 1;
		cpClipped.x = crClip.right;
	}
	else
	{
		cpOut.x = 0;
		cpClipped.x = cp.x;
	}

	if (cp.y < crClip.top)
	{
		cpOut.y = -1;
		cpClipped.y = crClip.top;
	}
	else if (cp.y > crClip.bottom)
	{
		cpOut.y = 1;
		cpClipped.y = crClip.bottom;
	}
	else
	{
		cpOut.y = 0;
		cpClipped.y = cp.y;
	}

	return cpOut;
}

#if 0
// Useful diagnostic routine.
static void DumpPolyPolygon(CArray<CPoint, CPoint&>& caPoints,
									 CArray<int, int>& caPtsInPoly)
{
	int nPoint = 0;
	for (int nPolygon = 0; nPolygon < caPtsInPoly.GetSize(); nPolygon++)
	{
		TRACE("Polygon %d:\n", nPolygon);
		for (int i = 0; i < caPtsInPoly.GetAt(nPolygon); i++)
		{
			TRACE("Pt %d: %d, %d\n", nPoint, caPoints.GetAt(nPoint));
			nPoint++;
		}
	}
}
#endif

static void ClipPolyPolygon(CArray<CPoint, CPoint&>& caPoints,
									 CArray<int, int>& caPtsInPoly,
									 CRect crClip)
{
	CArray<CPoint, CPoint&> caNewPoints;
	CArray<int, int> caNewPtsInPoly;

//	TRACE("Before\n");
//	DumpPolyPolygon(caPoints, caPtsInPoly);

	// Bump the clip rect out a pixel. This is what we clip to.
	// It keeps clipped segments out of the clip area.

	crClip.InflateRect(1, 1);

	// Loop on source polygons.
	// We may be generating a different number of polygons on exit if some of
	// them go away. This is the only way currently to have a different number.

	int nPoint = 0;
	for (int nPolygon = 0; nPolygon < caPtsInPoly.GetSize(); nPolygon++)
	{
		int nNewPts = 0;

		// Loop through all the points in this polygon.
		int nPoints = caPtsInPoly.GetAt(nPolygon);
		ASSERT(nPoints >= 2);
		if (nPoints >= 2)
		{
			// We have something to do.
			// Grab the first point to prime the loop.
			CPoint cpCurrent = caPoints.GetAt(nPoint++);
			CPoint cpCurrentClipped;
			CPoint cpCurrentSector = GetClipSector(cpCurrent, cpCurrentClipped, crClip);

			// Add this point.
			caNewPoints.Add(cpCurrentClipped);
			nNewPts++;
			int i = 1;

			// Add line segments until done.
			while (i++ < nPoints)
			{
				// Grab the next point.
				CPoint cpNext = caPoints.GetAt(nPoint++);
				CPoint cpNextClipped;
				CPoint cpNextSector = GetClipSector(cpNext, cpNextClipped, crClip);

				// Add line segments for all sector transitions.
				// We want to loop until we run out of line segments to add.
				BOOL fDone = FALSE;
				do
				{
					// Compute the new point to add.
					CPoint cpNew;
					CPoint cpNewClipped;
					CPoint cpNewSector;

					// See if we have any sector transitions to handle.
					// We generate a "time" of intersection which is the 't' value
					// in the standard parametric equations for a line:
					// x(t) = x0 + t*(x1-x0);
					// y(t) = y0 + t*(y1-y0);

					double dT = 2.0;		// No value yet; 't' ranges from 0 to 1.

					// Compute any x transition.
					if (cpNextSector.x != cpCurrentSector.x)
					{
						// Compute the intersection time.
						// This is the next x line we cross.
						int nNewSector;
						int nIntersectX;
						if (cpCurrentSector.x < cpNextSector.x)
						{
							// Moving right.
							if (cpCurrentSector.x < 0)
							{
								nIntersectX = crClip.left;
								nNewSector = 0;
							}
							else
							{
								nIntersectX = crClip.right;
								nNewSector = 1;
							}
						}
						else
						{
							// Moving left.
							if (cpCurrentSector.x > 0)
							{
								nIntersectX = crClip.right;
								nNewSector = 0;
							}
							else
							{
								nIntersectX = crClip.left;
								nNewSector = -1;
							}
						}
						// Compute the "time" where the intersection occurs.
						// From the x(t) equation above, we have:
						// t = (x(t) - x0)/(x1 - x0).
						// Since we are transitioning between sectiors x-wise, we
						// can rest assured that (x0 != x1).
						ASSERT(cpNext.x != cpCurrent.x);
						dT = (double)(nIntersectX-cpCurrent.x)/(double)(cpNext.x-cpCurrent.x);
						cpNewSector.x = nNewSector;
						cpNewSector.y = cpCurrentSector.y;
						ASSERT(dT != 2.0);
					}

					// Compute any y transition.
					if (cpNextSector.y != cpCurrentSector.y)
					{
						// Compute the intersection time.
						// This is the next y line we cross.
						int nNewSector;
						int nIntersectY;
						if (cpCurrentSector.y < cpNextSector.y)
						{
							// Moving down.
							if (cpCurrentSector.y < 0)
							{
								nIntersectY = crClip.top;
								nNewSector = 0;
							}
							else
							{
								nIntersectY = crClip.bottom;
								nNewSector = 1;
							}
						}
						else
						{
							// Moving up.
							if (cpCurrentSector.y > 0)
							{
								nIntersectY = crClip.bottom;
								nNewSector = 0;
							}
							else
							{
								nIntersectY = crClip.top;
								nNewSector = -1;
							}
						}

						// Compute the "time" where the intersection occurs.
						// From the y(t) equation above, we have:
						// t = (y(t) - y0)/(y1 - y0).
						// Since we are transitioning between sectiors y-wise, we
						// can rest assured that (y0 != y1).
						ASSERT(cpNext.y != cpCurrent.y);
						double dYT = (double)(nIntersectY-cpCurrent.y)/(double)(cpNext.y-cpCurrent.y);
						// Use the smaller 't' (the closer one).
						if (dYT < dT)
						{
							dT = dYT;
							// Fix up the sector in case the x case changed it.
							cpNewSector.x = cpCurrentSector.x;
							cpNewSector.y = nNewSector;
						}
						ASSERT(dT != 2.0);
					}

					if (dT == 2.0)
					{
						// No transitions. We are in the same sector.
						// Add the current point and we are done.
						cpNew = cpNext;
						cpNewClipped = cpNextClipped;
						cpNewSector = cpNextSector;

						fDone = TRUE;
					}
					else
					{
						// Compute the point of intersection.
						// Uses the standard parametric equations (see above).
						CPoint cpIntersect;
						cpNew.x = (int)(cpCurrent.x + dT*(cpNext.x - cpCurrent.x));
						cpNew.y = (int)(cpCurrent.y + dT*(cpNext.y - cpCurrent.y));
						cpNewClipped = cpNew;

						// Clip this point for adding purposes.
						if (cpNewSector.x < 0) cpNewClipped.x = crClip.left;
						else if (cpNewSector.x > 0) cpNewClipped.x = crClip.right;
						if (cpNewSector.y < 0) cpNewClipped.y = crClip.top;
						else if (cpNewSector.y > 0) cpNewClipped.y = crClip.bottom;
					}

					if (cpNewClipped != cpCurrentClipped)
					{
						// Add this point.
						caNewPoints.Add(cpNewClipped);
						nNewPts++;
					}

					// The current point is the one we last added.
					cpCurrent = cpNew;
					cpCurrentClipped = cpNewClipped;
					cpCurrentSector = cpNewSector;

				} while (!fDone);
			}
		}
		else
		{
			ASSERT(FALSE);
			nPoint += nPoints;
		}

		// We have a polygon to add.
		// Validate it. Currently, we only ensure we have enough points.
		// At some point in the future, we could also make sure there are
		// actually some points within the clip area.

		if (nNewPts >= 2)
		{
			// Add the count to the "point count" array.
			caNewPtsInPoly.Add(nNewPts);
		}
		else
		{
			// Not enough for a new polygon.
			if (nNewPts == 1)
			{
				// We only generated a single point
				// We need to remove it to keep the arrays consistent.
				caNewPoints.RemoveAt(caNewPoints.GetSize()-1);
			}
		}
	}

	// Now, copy everything back to the original arrays.
	caPoints.Copy(caNewPoints);
	caPtsInPoly.Copy(caNewPtsInPoly);

//	TRACE("After\n");
//	DumpPolyPolygon(caPoints, caPtsInPoly);
}

int CBECallbackInterfaceObj::FreeDrawState(void* pDrawState)
{
	AFX_MANAGE_STATE(m_pModuleState);
	if (pDrawState == NULL)
	{
		ASSERT(0);
		return BorderInvalidParams;
	}

	SavedState* pSavedState = (SavedState*)pDrawState;

	// "Unattach" the record from the object so it won't get deleted
	pSavedState->m_pPageObj->DetachRecord();
	pSavedState->m_pPageObj->destroy();
	delete pSavedState->m_pPageObj;
	delete pSavedState->m_pUpdate;
	delete pSavedState;
	pDrawState = NULL;

	return BorderSuccess;
}

void CBECallbackInterfaceObj::DisplayHelpfulHint(CWnd* pParent /* = NULL*/)
{
	AFX_MANAGE_STATE(m_pModuleState);
	// Put daily tip on top
	GET_PMWAPP()->ShowHelpfulHint("BorderPlus!", pParent);
}

void CBECallbackInterfaceObj::RemoveHelpfulHint()
{
	AFX_MANAGE_STATE(m_pModuleState);
	// Get that guy outta here!
	GET_PMWAPP()->RemoveHelpfulHint();
}

int CBECallbackInterfaceObj::Notify(int nNotification, void* pDatabase, void* pData)
{
	AFX_MANAGE_STATE(m_pModuleState);
	if (!pDatabase)
	{
		ASSERT(0);
		return BorderInvalidParams;
	}

	// get the document from the database
	PMGDatabasePtr pDB = (PMGDatabasePtr)pDatabase;
	CPmwDoc* pDoc = (CPmwDoc*)pDB->GetDocument();
	if (!pDoc)
	{
		ASSERT(0);
		return BorderError;
	}

	// get currently selected object (which better be a border object!)
	PMGPageObjectPtr pObject = pDoc->selected_objects();
	if (pObject->type() != OBJECT_TYPE_Border)
	{
		ASSERT(0);
		return BorderError;
	}

	// OK, now let's do something...
	switch (nNotification)
	{
		case BENotifyBorderChange:
		{
			// just refresh the whole thing for now
			// later, do add_refresh() with bound passed in
			pDoc->refresh_object(pObject);

			break;
		}
		case BESaveBorder:
		{
			CString filter;
			filter.LoadString(IDS_SAVE_BORDER_FILT);
			CString initialDir = GetGlobalPathManager()->ExpandPath("[[P]]");
			CString defExt = "*.pmo";

			CSaveBorderDlg fileDlg(defExt, "", filter, initialDir);

			int error = FALSE;
			if (fileDlg.DoModal() == IDOK)
			{
				CString filename = fileDlg.GetPathName();
				if ((error = SavePMObjectsToFile(filename, pDoc, TRUE)) == ERRORCODE_None)
				{
					if (fileDlg.m_fAddToGallery == TRUE)
					{
						CPMWCollection* pCollection = GetGlobalCollectionManager()->GetUserCollection(CPMWCollection::typeArt);
						if (pCollection != NULL)
						{
							error = pCollection->ImportItem(filename, NULL, NULL, fileDlg.m_strCategory, FALSE);
						}
					}
				}
				if (error != ERRORCODE_None)
					AfxMessageBox(IDS_ERROR_SAVING);
			}
			break;
		}
	}

	return BorderSuccess;
}


// Ensures that the record refered to is of the correct type, and if not
// performs a conversion and resets the value of the ID to that of the converted
// record if a conversion is performed.
void CBECallbackInterfaceObj::MatchupRecordType( DWORD *pID, void* pVoidDatabase )
{
	PMGDatabasePtr pDatabase = (PMGDatabasePtr)pVoidDatabase;

	// If the record referenced is a graphic record, then it is an old style
	// record for the borders.  We need to hook it up to a new component record
	// so that the application runs correctly.
	GraphicRecord *pGraphicRecord = (GraphicRecord*) pDatabase->get_record( *pID, NULL, RECORD_TYPE_Graphic );
	if (pGraphicRecord != NULL)
	{
		// Create a graphic structure that can be used to allocate a new component record.
		GRAPHIC_CREATE_STRUCT GCS;
		GCS.m_csFileName = pGraphicRecord->m_csFileName;
		GCS.m_csFriendlyName = pGraphicRecord->m_csName;
		CPmwDoc* pDoc = (CPmwDoc*)pDatabase->GetDocument();
		GCS.m_fSpecialCrop = ((pGraphicRecord->record.flags & GraphicRecord::FLAG_SpecialCrop) != 0);
		GCS.m_pTempDIB = pGraphicRecord->m_pTempDIB;
		if (pDoc)
		{
			GCS.embed_source = pDoc->GetPathManager()->FileIsRemovable(GCS.m_csFileName);
		}
		else
		{
			GCS.embed_source = FALSE;
		}

		// Build the new component record that will be linked to this graphic record.
		CGraphicData cData( &GCS );
		DatabaseRecordPtr pDatabaseRecord;
		ERRORCODE error = pDatabase->new_record( RECORD_TYPE_Component, &cData, &pDatabaseRecord );
		if (error == ERRORCODE_None)
		{
			// Hook ourselves to the new record.
			*pID = pDatabaseRecord->Id();
		}

		// Release the graphic record now that we are done with it.
		pGraphicRecord->release();
	}
}
