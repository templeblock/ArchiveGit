/*
// $Workfile: Borders.h $
// $Revision: 2 $
// $Date: 3/08/99 1:16p $
//
//  "This unpublished source code contains trade secrets which are the
//   property of Mindscape, Inc.  Unauthorized use, copying or distribution
//   is a violation of international laws and is strictly prohibited."
// 
//        Copyright © 1998 Mindscape, Inc. All rights reserved.
*/

// Revision History:
//
// $Log: /PM8/App/Borders.h $
// 
// 2     3/08/99 1:16p Rgrenfel
// Implemented the matchup call to ensure correct record IDs in border
// objects.
// 
// 15    3/05/99 6:48p Rgrenfel
// Implemented a fixup prototype for graphic record conversion to
// component records and now have the internal page object stored as a
// RectPageObject pointer.
// 
// 14    11/19/98 2:54p Mwilson
// added call to change toolbar for border editor
// 
// 13    10/29/98 4:04p Jayn
// Fixed an MFC DLL crash problem.
// 
// 12    8/10/98 4:03p Hforman
// add CropCopyright flag
// 
// 11    6/02/98 4:12p Rlovejoy
// Code to remove helpful hint from Border Editor.
// 
// 10    6/02/98 1:20p Rlovejoy
// Add DisplayHelpfulHint() to the callback interface.
// 
// 9     2/27/98 2:45p Jayn
// Clipping for border background.
// 
// 8     2/24/98 1:35p Hforman
// add GetPalette()
// 
// 7     2/19/98 6:44p Hforman
// background color, etc.
// 
// 6     2/17/98 7:26p Hforman
// interrupting, background color, etc.
// 
// 5     2/12/98 1:16p Hforman
// add image caching, save border functionality
// 
// 4     1/13/98 6:13p Hforman
// interim checkin
// 
// 3     1/09/98 6:52p Hforman
// 
// 2     1/09/98 11:05a Hforman
// interim checkin
// 
// 1     1/06/98 6:54p Hforman

#ifndef __BORDERS_H__
#define __BORDERS_H__

#include "border/borddef.h"

// forward declarations
class CBECallbackInterfaceObj;
class CBitmapObjectImage;

// maximum size we'll let our image cache grow
#define MAX_IMAGE_CACHE_SIZE	32

class CBorderEditor
{
public:
			CBorderEditor();
			~CBorderEditor();

	BOOL	StartupBorderEditor();
	void	ShutdownBorderEditor();

	BOOL	IsInitialized()				{ return m_fIsInitialized; }
	CBEInterface* GetBEInterface()	{ return m_pBEInterface; }

private:
	HINSTANCE		m_hLibrary;
	BOOL				m_fIsInitialized;

	CBECallbackInterfaceObj*	m_pPMInterface;	// PrintMaster funcs for BorderEditor
	CBEInterface*					m_pBEInterface;	// BorderEditor funcs for PrintMaster
};

class CBECallbackInterfaceObj : public CBECallbackInterface
{
public:
					CBECallbackInterfaceObj();
					~CBECallbackInterfaceObj();

	virtual int Release(void);
	virtual int NewGraphic(DWORD& dwID, void* pDatabase,
								  LPCSTR pszFileName, LPCSTR pszFriendlyName = NULL,
								  BOOL fCropCopyright = FALSE);

	// Ensures that the record referenced is of the correct type and resets the
	// ID value to a converted record if it isn't.
	virtual void MatchupRecordType( DWORD *pID, void *pVoidDatabase );

	virtual HBITMAP GetToolbarBitmap();
	virtual int Duplicate(DWORD& dwDupID, void* pDupDatabase, DWORD dwID, void* pDatabase);

	virtual int AddRef(DWORD dwID, void* pDatabase);
	virtual int RemoveRef(DWORD dwID, void* pDatabase);

	virtual int ChooseGraphic(HWND hParent, DWORD& dwID, void* pDatabase);
	virtual int GetGraphicInfo(DWORD dwID, void* pDatabase, CBEGraphicInfo& Info);
	virtual int DrawGraphic(DWORD dwID, void* pDatabase, CBEDrawInfo* pDrawInfo);
	virtual int DrawBackground(CBEBackgroundInfo* pBackgroundInfo);
	virtual HPALETTE GetPalette();
	virtual void DisplayHelpfulHint(CWnd* pParent);
	virtual void RemoveHelpfulHint();
	virtual int FreeDrawState(void* pDrawState);
	virtual int Notify(int nNotification, void* pDatabase, void* pData);

protected:
	// data for caching border object images
	struct ImageCacheData
	{
		PMGDatabasePtr	pDB;
		DWORD				dwID;
		CRect				rcBounds;
		double			dAngle;
		BOOL				fXFlipped;
		BOOL				fYFlipped;

		CBitmapObjectImage* pImage;
	};

	// linked list of cached object images
	CList<ImageCacheData*, ImageCacheData*> m_ImageCache;

	// our UpdateState class
	class SavedState
	{
	public:
		SavedState() {}
		virtual ~SavedState() {}

		UpdateStatePtr		m_pUpdate;
		RectPageObjectPtr	m_pPageObj;
		PBOX					m_bound;				// bound of object
		CRect					m_scrnRect;			// screen bound
	};

	SavedState* m_pSavedState;

	AFX_MODULE_STATE* m_pModuleState;
};

#endif
