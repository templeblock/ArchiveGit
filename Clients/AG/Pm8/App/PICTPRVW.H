//////////////////////////////////////////////////////////////////////////////
// $Header: /PM8/App/PICTPRVW.H 1     3/03/99 6:09p Gbeddow $
// 
//  "This unpublished source code contains trade secrets which are the
//	property of Mindscape, Inc.  Unauthorized use, copying or distribution
//	is a violation of international laws and is strictly prohibited."
// 
//		  Copyright © 1998 Mindscape, Inc. All rights reserved.
//
//
// $Log: /PM8/App/PICTPRVW.H $
// 
// 1     3/03/99 6:09p Gbeddow
// 
// 11    1/25/99 3:45p Rgrenfell
// Added support for displaying an animated GIF in the preview area.
// 
// 10    11/05/98 9:16p Psasse
// A new function to return image size
// 
// 9     5/29/98 5:43p Rlovejoy
// 
// 8     5/28/98 6:37p Rlovejoy
// Changes to accomodate previewing interior pages.
// 
// 7     5/28/98 2:10p Rlovejoy
// Added function to size preview.
// 
// 6     4/27/98 12:30p Dennis
// Added pagedefs.h include for PBOX dependency
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


#ifndef __PICTPRVW_H__
#define __PICTPRVW_H__

#include "pagedefs.h"   // for PBOX

enum
{
	IP_ANIMATION_CONTINUES = 0,
	IP_ANIMATION_COMPLETE
};

class CPGIFAnimator;

/////////////////////////////////////////////////////////////////////////////
// CDocumentPreview - Class for previewing a document.

class CDocumentPreview
{
public:
	CDocumentPreview();
	virtual ~CDocumentPreview();

	BOOL Initialize(CWnd* pDialog, UINT uPrevID, UINT uDescID = 0);
	void NewDocPreview(LPCSTR pszFileName, LPCSTR pszFriendlyName = NULL);
	void NewDocPreview(CPmwDoc* pDoc, BOOL fEncloseObjects = FALSE);
	virtual void SizeDocPreview(void);
	void ForceToFront(BOOL fForce)
	{
		m_fForceToFront = fForce;
	}

	void DrawPreview(LPDRAWITEMSTRUCT lpDrawItemStruct);

protected:
	void CloseDocument(void);
	BOOL GetBounds(PBOX* pBounds);
	void SizePreviewWindow(int nWidth, int nHeight);
	void ErasePreview(void);
	void DrawFrame(HDC hDC);
	void SetDescription(LPCSTR pszFileName, LPCSTR pszFriendlyName);

	CWnd* GetDialog(void);
	CWnd* GetPreviewWnd(void);
	CWnd* GetDescriptionWnd(void);

	CPmwDoc* m_pDoc;					// The document we are previewing.

	CWnd*		m_pDialog;
	UINT		m_uPreviewID;
	UINT		m_uDescriptionID;

	BOOL		m_fCloseDoc;
	BOOL		m_fForceToFront;		// Force document to first page
	BOOL		m_fEncloseObjects;	// Use bound surrounding objects for preview

	CRect		m_crFullPreview;		// Maximum preview rect
	CRect		m_crPreview;			// This preview rect

	int		m_nXResolution;
	int		m_nYResolution;
};

/////////////////////////////////////////////////////////////////////////////
// CPicturePreview - Class for previewing a picture.

class CPicturePreview : public CDocumentPreview
{
public:
	CPicturePreview();
	virtual ~CPicturePreview();

	void NewGraphicPreview(LPCSTR pszName, LPCSTR pszFriendlyName = NULL, LPBITMAPINFO pDib = NULL);
	BOOL GetImageSize(PBOX* pBox);

	// Animated GIF support.
	bool IsAnimatedGIF()
		{ return m_bIsAnimatedGIF; }
	// Renders the next frame of the animated gif.
	UINT Update(LPDRAWITEMSTRUCT lpDrawItemStruct);
	// Informs the caller whether sufficient time has passed for the next frame
	// of the animation to be rendered.
	bool NeedsUpdate();

	// Overloaded to have special preview sizing for GIF animations.
	virtual void SizeDocPreview(void);

protected:
	BOOL CreateDocument(void);

	void CheckForAnimatedGIF();

private:
	bool m_bIsAnimatedGIF;
	CPGIFAnimator *m_pGIFAnimator;
};

#endif						// #ifndef __PICTPRVW_H__
