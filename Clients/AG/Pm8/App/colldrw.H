//////////////////////////////////////////////////////////////////////////////
// $Header: /PM8/App/colldrw.H 1     3/03/99 6:04p Gbeddow $
//
//  "This unpublished source code contains trade secrets which are the
//   property of Mindscape, Inc.  Unauthorized use, copying or distribution
//   is a violation of international laws and is strictly prohibited."
// 
//        Copyright © 1998 Mindscape, Inc. All rights reserved.
//
// $Log: /PM8/App/colldrw.H $
// 
// 1     3/03/99 6:04p Gbeddow
// 
// 3     2/09/99 1:48p Gbeddow
// Support in collection building for creating online collection GIF and
// JPEG thumbnail files, and related changes, per spec from Alexei
// 
// 2     10/19/98 10:48a Jayn
// More COM restructuring changes.
// 
// 1     10/14/98 12:35p Jayn
// 
//////////////////////////////////////////////////////////////////////////////

#ifndef __COLLDRW_H__
#define __COLLDRW_H__

#include "icolldrw.h"
#include "ipathmgr.h"
#include "pmwcoll.h"

#ifdef NEW_THUMBNAILS
//#include "pmgobj.h"
#endif

class CPmwDoc;
class StorageFile;
class TmpFile;

struct REFRESH_EXTENT;

//
// A structure to conveniently encapsulate the general functionality of the
// BuildDocumentPreview function.
//

class CCollectionDrawContext
{
public:
	CCollectionDrawContext()
	{
		m_clrFillColor = UNSPECIFIED_COLOR;
		m_fDrawTransparent = FALSE;
	}

	// Setup to draw a thumbnail.
	void SetThumbnail(CThumbnailDataEntry* pThumbnailData, CSize czPreviewDims)
	{
		m_nDrawType = Thumbnail;
		m_Thumbnail.m_pThumbnailData = pThumbnailData;
		m_Thumbnail.m_czPreviewDims = czPreviewDims;
	}

	// Setup to draw a view.
	void SetView(CDC* pDC, CRect* pDestRect, HRGN hrgnClip, REFRESH_EXTENT* pRefreshExtent)
	{
		m_nDrawType = View;
		m_View.m_pDC = pDC;
		m_View.m_pDestRect = pDestRect;
		m_View.m_hrgnClip = hrgnClip;
		m_View.m_pRefreshExtent = pRefreshExtent;
	}

public:
	// The type of draw to do.
	enum
	{
		Thumbnail,
		View,
	} m_nDrawType;

	// General draw parameters.
	COLORREF m_clrFillColor;
	BOOL m_fDrawTransparent;

	// Specific draw parameters.
	union
	{
		// Thumbnail parameters.
		struct
		{
			CThumbnailDataEntry* m_pThumbnailData;
			SIZE m_czPreviewDims;
		} m_Thumbnail;

		// View parameters.
		struct
		{
			CDC* m_pDC;
			CRect* m_pDestRect;
			HRGN m_hrgnClip;
			REFRESH_EXTENT* m_pRefreshExtent;
		} m_View;
	};
};

//
// This is the "collection draw" class.
// There is still work to do here if we ever want to separate the drawing
// functions out of the browser.
// This gives us a good start as far as generalizing the thumbnail drawing
// in the main collection code.
//

class CPMCollectionDraw : public CCmdTarget
{
public:
	CPMCollectionDraw();
	virtual ~CPMCollectionDraw();

	void Init(IPathManager* pPathManager);

   // Document management.
	int CreateDocument(LPCSTR pszName, int nType);
	CPmwDoc* GetDocument(void) const
		{ return m_pDoc; }
	void CloseDocument(void);

	// Get the name of the document.
	LPCSTR GetFileName(void) const
		{ return m_csFileName; }

	int Draw(int nType, LPCSTR pszContentFileName,
				CDC* pDC, CRect& crDest, HRGN hrgnClip,
				COLORREF clrFillColor = UNSPECIFIED_COLOR,
				REFRESH_EXTENT* pRefreshExtent = NULL);
public:
	// Implementation of the interfaces functions.
	int BuildThumbnail(int nType, CPMWCollectionData* pData, CThumbnailDataEntry* pThumbnailData);

protected:
	// Basic implementation.
	void SetDocument(CPmwDoc *pDoc, LPCSTR pszFileName);

	int DrawDocument(CCollectionDrawContext &DrawContext, CPMWCollectionData* pClientCD = NULL);

	int CreateTempProjectFile(LPCSTR pszFileName, StorageDevice *pSource, TmpFile *pDestFile);

	int DrawPicture(LPCSTR pszContentFileName, StorageDevice* pSource, CCollectionDrawContext& DrawContext, CPMWCollectionData* pClientCD = NULL);
	int DrawProject(LPCSTR pszContentFileName, LPCSTR pszOriginalFileName, CCollectionDrawContext& DrawContext);

	int GetPictureDocument(LPCSTR pszContentFileName, StorageDevice* pSource = NULL);
	int GetProjectDocument(LPCSTR pszContentFileName, LPCSTR pszOriginalFileName = NULL);

	int BuildPictureThumbnail(CPMWCollectionData* pData, CThumbnailDataEntry* pThumbnailData);
	int BuildProjectThumbnail(CPMWCollectionData* pData, CThumbnailDataEntry* pThumbnailData);

protected:
   // Data
	CThumbnailDataEntry* m_pThumbnailData;
	CPmwDoc *            m_pDoc;
	CString					m_csFileName;
	REFRESH_EXTENT*      m_pRefreshExtent;
	IPathManager*			m_pPathManager;

protected:
	DECLARE_INTERFACE_MAP()

	// This defines the interface that is passed to others.
	// It will have name m_XCollectionDraw.
	// To see comments on the functions, see the ICollectionDraw definitions
	// in ICOLLDRW.H.
	BEGIN_INTERFACE_PART(CollectionDraw, ICollectionDraw)
		INIT_INTERFACE_PART(CPMCollectionDraw, CollectionDraw)
		STDMETHOD_(int, BuildThumbnail) (THIS_ int nType, CPMWCollectionData* pData, CThumbnailDataEntry* pThumbnailData);
	END_INTERFACE_PART(CollectionDraw)
};

struct REFRESH_EXTENT;

#if 0
      int               GetPictureDocument(CPMWDocumentPreviewData &docData,
                           CString &strContentFileName,
                           StorageDevice *pSource = NULL);
      int               GetProjectDocument(CPMWDocumentPreviewData &docData,
                           CString &strContentFileName,
                           CString *pstrOriginalFileName = NULL);
      int               BuildProjectThumbnail(CPMWDocumentPreviewData &docData);
      int               BuildPictureThumbnail(CPMWDocumentPreviewData &docData);

      int               Draw(CPMWDocumentPreviewData &docData,
                           CString &strContentFileName,
                           CDC *pDC, CRect &crDest, HRGN hrgnClip,
                           COLORREF crFillColor = UNSPECIFIED_COLOR);
      int               DrawPicture(CPMWDocumentPreviewData &docData,
                           CString &strContentFileName,
                           StorageDevice *pSource,
									CThumbnailDataEntry* pThumbnailData,
                           CDC *pDC = NULL, CRect *DestRect = NULL,
                           COLORREF crFillColor = UNSPECIFIED_COLOR);
      int               GetDocument(CPMWDocumentPreviewData &docData,
                           CString &strContentFileName);
      // DrawProject using default parms creates thumbnail
      int               DrawProject(CPMWDocumentPreviewData &docData,
												CString &strContentFileName,
												CString* pstrOriginalFileName,
												CThumbnailDataEntry* pThumbnailData,
												CDC *pDC = NULL, CRect *pDestRect=NULL,
												BOOL bDrawPreview=TRUE, HRGN hrgnClip=NULL,
												COLORREF crFillColor = UNSPECIFIED_COLOR);
      int               CreateTempProjectFile(CString &csFileName,
                           StorageDevice *pSource, TmpFile *pDestFile);
#endif

#endif
