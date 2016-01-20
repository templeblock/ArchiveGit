// $Workfile: BrowDrop.h $
// $Revision: 1 $
// $Date: 3/03/99 6:03p $
// 
//  "This unpublished source code contains trade secrets which are the
//   property of Mindscape, Inc.  Unauthorized use, copying or distribution
//   is a violation of international laws and is strictly prohibited."
// 
//        Copyright © 1998 Mindscape, Inc. All rights reserved.
//
//////////////////////////////////////////////////////////////////////////////
//
// BrowDrop.h: header file for implementing Drag and Drop in the Browsers
//
//////////////////////////////////////////////////////////////////////////////
//
// Revision History:
//
// $Log: /PM8/App/BrowDrop.h $
// 
// 1     3/03/99 6:03p Gbeddow
// 
// 3     1/29/98 8:12p Hforman
// add support for "pmo" files
// 
// 2     1/22/98 7:30p Hforman
// finished functionality
// 
// 1     1/15/98 3:55p Hforman
//
//////////////////////////////////////////////////////////////////////////////

#ifndef __BROWDROP_H__
#define __BROWDROP_H__

class CArtOleDropTarget : public COleDropTarget
{
public:
	CArtOleDropTarget() {}
	~CArtOleDropTarget() {}

	// Overridden functions. Our COleDropTarget automatically calls these.
	virtual DROPEFFECT OnDragEnter(CWnd* pWnd, COleDataObject* pDataObject, DWORD dwKeyState, CPoint point );
	virtual DROPEFFECT OnDragOver(CWnd* pWnd, COleDataObject* pDataObject, DWORD dwKeyState, CPoint point );
	virtual void OnDragLeave(CWnd* pWnd);
	virtual BOOL OnDrop(CWnd* pWnd, COleDataObject* pDataObject, DROPEFFECT dropEffect, CPoint point );

	BOOL			IsValidFormat(COleDataObject* pDataObject);
	BOOL			IsFixedDrive(COleDataObject* pDataObject);

protected:
	DROPEFFECT	GetDropEffect(COleDataObject* pDataObject, DWORD dwKeyState);
	CString		GetUniqueUserArtFilename(CString ext);

	void	AddUntitledPictToBrowser(const CString& filename, CArtBrowserDialog* pBrowser);
	BOOL	SaveClipObjectsToUserArt(COleDataObject* pDataObject, LPCTSTR pFilename);
	BOOL	Save_CF_METAFILEPICT_ToUserArt(COleDataObject* pDataObject, LPCTSTR pFilename);
	BOOL	Save_CF_DIB_ToUserArt(COleDataObject* pDataObject, LPCTSTR pFilename);
	BOOL	Save_CF_BITMAP_ToUserArt(COleDataObject* pDataObject, LPCTSTR pFilename);
	BOOL	SaveDIBToUserArt(BITMAPINFO* pBitmap, LPCTSTR pFilename);
};

#endif // __BROWDROP_H__
