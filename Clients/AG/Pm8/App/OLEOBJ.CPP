/*
// $Header: /PM8/App/OLEOBJ.CPP 1     3/03/99 6:08p Gbeddow $
//
// OLE object routines.
//
// $Log: /PM8/App/OLEOBJ.CPP $
// 
// 1     3/03/99 6:08p Gbeddow
// 
// 6     7/23/98 8:42a Jayn
// Supports CF_BITMAP if CG_METAFILEPICT fails
// 
// 5     7/22/98 8:19p Johno
// 
// 4     4/07/98 8:44a Jayn
// Disabled some experimental code.
// 
// 3     3/24/98 10:52a Jayn
// More serif object. New thumbnail sizes.
// 
// 2     11/25/97 10:57a Jayn
// New and improved OLE/WMF support.
// 
//    Rev 1.0   14 Aug 1997 15:23:12   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:39:28   Fred
// Initial revision.
// 
//    Rev 1.3   12 Nov 1996 10:30:34   johno
// Moved strings to resource DLL
// 
//    Rev 1.2   28 Oct 1996 13:52:24   Jay
// Moved from 3.01 source.
// 
//    Rev 1.3   22 Oct 1996 14:23:46   Jay
// Won't draw object if in-place active.
// 
//    Rev 1.2   23 Oct 1996 00:53:46   Jay
// Improved palette handling. Flipped objects now look right.
// 
//    Rev 1.1   15 Oct 1996 09:02:04   Jay
// is_printing -> IsScreen()
// 
//    Rev 1.0   14 Mar 1996 13:36:52   Jay
// Initial revision.
// 
//    Rev 1.0   16 Feb 1996 12:29:48   FRED
// Initial revision.
// 
//    Rev 1.8   15 Jan 1996 10:26:22   JAY
//  
// 
//    Rev 1.7   15 Nov 1995 15:23:12   JAY
// OLE fixes, etc.
// 
//    Rev 1.6   10 Nov 1995 17:39:16   JAY
// Palette fixes.
// 
//    Rev 1.5   09 Nov 1995 16:13:40   JAY
// OLE improvements
// 
//    Rev 1.4   11 Oct 1995 17:48:26   JAY
// More OLE/logo stuff
// 
//    Rev 1.3   29 Sep 1995 16:01:02   JAY
// More drag/drop/paste/OLE stuff
// 
//    Rev 1.2   28 Sep 1995 12:13:14   JAY
// More drag/drop stuff
// 
//    Rev 1.1   27 Sep 1995 15:26:32   JAY
// New OLE code
// 
//    Rev 1.0   29 Mar 1995 09:18:58   JAY
// Initial revision.
*/

#include "stdafx.h"
#include "pmw.h"
#include "pmwdoc.h"
#include "pmgobj.h"
#include "utils.h"
#include "util.h"
#include "cntritem.h"
#include "cdibdc.h"
#include "bmp.h"
#include "grafobj.h"
#include "wmf.h"
#include "memdev.h"

#include "oleobj.h"

extern CPalette* pOurPal;

/*****************************************************************************/
/*                                OleObject                                  */
/*****************************************************************************/

/*
// The creator for an OLE object.
*/

ERRORCODE OleObject::create(DB_OBJECT_TYPE type, ObjectDatabasePtr owner, VOIDPTR creation_data, PageObjectPtr far *record)
{
	OleObject* pObject;

/* Create the pattern object. */

	if ((pObject = new OleObject(type, owner)) == NULL)
	{
		return ERRORCODE_Memory;
	}
	*record = (PageObjectPtr)pObject;

	if (creation_data != NULL)
	{
		OLEOBJECT_CREATE_STRUCT* ocs = (OLEOBJECT_CREATE_STRUCT*)creation_data;

		pObject->set_bound(ocs->m_Bound);
		pObject->Item(ocs->m_pItem);
		pObject->SetOriginalSize(ocs->m_OriginalSize);
	}

	return ERRORCODE_None;
}

/*
// The constructor for the OLE object.
*/

OleObject::OleObject(DB_OBJECT_TYPE type, ObjectDatabasePtr owner)
		: RectPageObject(type, owner)
{
	record.select_flags = SELECT_FLAG_boundary
 									| SELECT_FLAG_size_handles
									| SELECT_FLAG_move_handle
									| SELECT_FLAG_rotate_handle
									;
	m_orecord.m_dwItemNumber = (DWORD)-1;
	m_orecord.m_OriginalSize.x = 0;
	m_orecord.m_OriginalSize.y = 0;
	m_pItem = NULL;

	refresh_flags |= REFRESH_FLAG_makes_image;
}

/*
// Update an OLE object.
*/

UpdateStatePtr OleObject::update(RedisplayContextPtr rc, PBOX_PTR extent, LPRECT clip, UpdateStatePtr ustate, REFRESH_TYPE refresh_type)
{
	if (m_pItem->IsInPlaceActive() && rc->IsScreen())
	{
		ASSERT(ustate == NULL);
		return NULL;
	}
  
	// Get the data.
	COleDataObject DataObject;
	m_pItem->AttachDataObject(DataObject);
	STGMEDIUM stgmed;
	if (DataObject.GetData(CF_METAFILEPICT, &stgmed))
	{
		ASSERT(stgmed.tymed == TYMED_MFPICT);
		LPMETAFILEPICT pMFP = (LPMETAFILEPICT)GlobalLock(stgmed.hMetaFilePict);
		if (pMFP != NULL)
		{
			MemoryDevice DataDevice;

			HMETAFILE hMetaFile = pMFP->hMF;

			// (1) The header.

			ALDUS_WMF_HEADER Header;

			FillOutAldusHeader(Header, pMFP);

			DataDevice.AddPiece(&Header, sizeof(Header));

			// (2) The WMF data.

			if (hMetaFile != NULL)
			{
				UINT uSize = GetMetaFileBitsEx(hMetaFile, 0, NULL);
				if (uSize != NULL)
				{
					TRY
					{
						LPBYTE pData = new BYTE[uSize];
						GetMetaFileBitsEx(hMetaFile, uSize, pData);

						DataDevice.AddPiece(pData, uSize);

						WMFUpdateState* pState;
						if (ustate == NULL)
						{
							pState = new WMFUpdateState(&DataDevice);
						}
						else
						{
							pState = (WMFUpdateState*)ustate;
						}

						// Setup the parameters for the outline paint.
						COutlinePaintParams Params;

						Params.FromObject(this);
						Params.m_pClip = clip;
						Params.m_pRC = rc;

						// Do the paint.
						ustate = pState->paint(Params, ustate);

						delete [] pData;
					}
					END_TRY
				}
			}
			GlobalUnlock(stgmed.hMetaFilePict);
		}
		ReleaseStgMedium(&stgmed);
	}
	else if (DataObject.GetData(CF_BITMAP, &stgmed))
	{
		ASSERT(stgmed.tymed == TYMED_GDI);
		// Get the size of the bitmap.
		BITMAP bmInfo;
		if (::GetObject(stgmed.hBitmap, sizeof(BITMAP), &bmInfo))
		{
			// We have the bitmap dimensions.
			int nWidth = bmInfo.bmWidth;
			int nHeight = bmInfo.bmHeight;
			CMetaFileDC dc;
			if (dc.Create(NULL))
			{
				int nSave = dc.SaveDC();
				dc.SetMapMode(MM_ANISOTROPIC);
				dc.SetWindowOrg(0, 0);
				dc.SetWindowExt(nWidth, nHeight);
				// Create a DC to use for sourcing the bitmap.
				CDC SourceDC;
				if (SourceDC.CreateCompatibleDC(NULL))
				{
					CBitmap* pOldBitmap = SourceDC.SelectObject(CBitmap::FromHandle(stgmed.hBitmap));
					if (pOldBitmap != NULL)
					{
						// Do a StretchBlt. It is good to do StretchBlt vs. BitBlt
						// for metafile DCs which are scalable.
						dc.StretchBlt(0, 0, nWidth, nHeight, &SourceDC, 0, 0, nWidth, nHeight, SRCCOPY);
						// Out with the old bitmap.
						SourceDC.SelectObject(pOldBitmap);

						dc.RestoreDC(nSave);

						// Setup the metafile pict.
						METAFILEPICT mfp;
						mfp.mm = MM_ANISOTROPIC;
						mfp.xExt = nWidth;
						mfp.yExt = nHeight;
						mfp.hMF = dc.Close();

						// This code duplicates a lot of the code (more or less) in the above
						// case. It would be really cool to unite them someday. Someday...
						if (mfp.hMF != NULL)
						{
							// Draw it!
							// (1) The header.

							ALDUS_WMF_HEADER Header;

							FillOutAldusHeader(Header, &mfp);

							MemoryDevice DataDevice;
							DataDevice.AddPiece(&Header, sizeof(Header));

							HMETAFILE hMetaFile = mfp.hMF;

							// (2) The WMF data.

							UINT uSize = ::GetMetaFileBitsEx(hMetaFile, 0, NULL);
							if (uSize != NULL)
							{
								TRY
								{
									LPBYTE pData = new BYTE[uSize];
									::GetMetaFileBitsEx(hMetaFile, uSize, pData);

									DataDevice.AddPiece(pData, uSize);

									WMFUpdateState* pState;
									if (ustate == NULL)
									{
										pState = new WMFUpdateState(&DataDevice);
									}
									else
									{
										pState = (WMFUpdateState*)ustate;
									}

									// Setup the parameters for the outline paint.
									COutlinePaintParams Params;

									Params.FromObject(this);
									Params.m_pClip = clip;
									Params.m_pRC = rc;

									// Do the paint.
									ustate = pState->paint(Params, ustate);

									delete [] pData;
								}
								END_TRY
							}
						}
					}
				}
			}
		}
		ReleaseStgMedium(&stgmed);
	}

	DataObject.Release();
	return ustate;
}

/*
// Recalc after a size or movement.
*/

void OleObject::calc(PBOX_PTR panel_world /*=NULL*/, FLAGS panel_flags /*=0*/)
{
	RectPageObject::calc(panel_world, panel_flags);
	if (m_pItem->IsInPlaceActive())
	{
		((CPmwCntrItem*)m_pItem)->SetExtentAndRects();
	}
#if 0
	else
	{
		if (m_pItem->IsRunning())
		{
			// Convert bound dimensions to HIMETRIC.
			PBOX Bound = get_bound();
			CSize czExtent;
			czExtent.cx = scale_pcoord(Bound.x1-Bound.x0, 2540, PAGE_RESOLUTION);
			czExtent.cy = scale_pcoord(Bound.y1-Bound.y0, 2540, PAGE_RESOLUTION);
			TRY
				m_pItem->SetExtent(czExtent);
			END_TRY
		}
	}
#endif
}

/*
// Destroy this object.
// The object is being removed from the database.
// Any and all sub-objects must be removed from the database as well.
// This should not FREE anything (in the memory sense); that's the job of the
// destructor.
*/

void OleObject::destroy(VOID)
{
	if (m_pItem != NULL)
	{
		m_pItem->Delete();
		m_pItem = NULL;
	}
}

/*
// read_data()
//
// This is a method invoked by the read method to handle object-specific data.
*/

ERRORCODE OleObject::read_data(StorageDevicePtr device)
{
	ERRORCODE error;

	if ((error = INHERITED::read_data(device)) == ERRORCODE_None)
	{
		error = device->read_record(&m_orecord, sizeof(m_orecord));
		record.select_flags |= SELECT_FLAG_rotate_handle;
	}
	return error;
}

/*
// write_data()
//
// This is a method invoked by the write method to handle object-specific data.
*/

ERRORCODE OleObject::write_data(StorageDevicePtr device)
{
	ERRORCODE error;

	if ((error = INHERITED::write_data(device)) == ERRORCODE_None)
	{
		error = device->write_record(&m_orecord, sizeof(m_orecord));
	}
	return error;
}

/*
// size_data()
//
// This is a method invoked by the size method to handle object-specific data.
*/

ST_MAN_SIZE OleObject::size_data(StorageDevicePtr device)
{
	return INHERITED::size_data(device) + device->size_record(sizeof(m_orecord));
}

/*
// Return the original dimensions of this object.
*/

BOOL OleObject::original_dims(PPNT_PTR p)
{
	if (m_orecord.m_OriginalSize.x != 0
			&& m_orecord.m_OriginalSize.y != 0)
	{
		*p = m_orecord.m_OriginalSize;
		return TRUE;
	}
	return FALSE;
}

/*
// Assign an object to this.
*/

ERRORCODE OleObject::assign(PageObjectRef sobject)
{
	ERRORCODE error;

	if ((error = INHERITED::assign(sobject)) == ERRORCODE_None)
	{
		OleObject& Object = (OleObject&)sobject;
		m_orecord = Object.m_orecord;

		COleClientItem* pItem = Object.Item();
		CPmwDoc* pDoc = (CPmwDoc*)get_database()->GetDocument();
		ASSERT(pDoc != NULL);
		if (pDoc == NULL)
		{
			return ERRORCODE_IntError;
		}
		CPmwCntrItem* pNewItem = new CPmwCntrItem(pDoc);
		if (!pNewItem->CreateCloneFrom(pItem))
		{
			delete pNewItem;
			return ERRORCODE_Memory;			// Memory or ???
		}
		/* Call Item() which will set pointer and dwItemNumber. */
		Item(pNewItem);
		pNewItem->Object(this);
	}
	return error;
}

PageObjectState* OleObject::CreateState(void)
{
	return new OleObjectState;
}

void OleObject::SaveState(PageObjectState* pState)
{
	INHERITED::SaveState(pState);

	((OleObjectState*)pState)->m_ORecord = m_orecord;
}

void OleObject::RestoreState(PageObjectState* pState)
{
	INHERITED::RestoreState(pState);

	m_orecord = ((OleObjectState*)pState)->m_ORecord;
}

void OleObject::SetOriginalSize(const PPNT& Size)
{
	m_orecord.m_OriginalSize = Size;
}

/*
// Get the properties for this object.
*/

void OleObject::GetObjectProperties(CObjectProperties& Properties)
{
}

/*
// Set the properties for this object.
*/

BOOL OleObject::SetObjectProperties(CObjectProperties& Properties)
{
	return FALSE;
}
