/*
// $Header: /PM8/App/CLIP.H 1     3/03/99 6:04p Gbeddow $
//
// Definitions for clipboard object.
//
// $Log: /PM8/App/CLIP.H $
// 
// 1     3/03/99 6:04p Gbeddow
// 
// 8     1/25/99 3:38p Rgrenfell
// Added support for object descriptors in Drag/Drop
// 
// 7     1/07/99 5:09p Rgrenfel
// Added new clip render methods.
// 
// 6     9/14/98 12:06p Jayn
// Removed system_heap. Switched to MFC in DLL.
// 
// 5     7/23/98 6:45p Jayn
// SavePMObjectsTo... can skip special objects (like calendars)
// 
// 4     2/06/98 11:35a Jayn
// Broke some functions out to use elsewhere.
// 
// 3     1/29/98 8:02p Hforman
// add Selected param to CopyObjectsToDatabase()
// 
// 2     1/27/98 6:42p Hforman
// cleanup, made some methods global, changed the way H_BITMAP data is
// read, etc.
// 
//    Rev 1.0   14 Aug 1997 15:19:08   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:33:04   Fred
// Initial revision.
// 
//    Rev 1.9   16 Jun 1997 14:28:28   Jay
// Uses database of document during copy if exists (fixes crash).
// 
//    Rev 1.8   12 May 1997 18:08:38   Jay
// Browser drag & drop
// 
//    Rev 1.7   16 Apr 1997 12:12:32   Jay
// Fixes for text undo
// 
//    Rev 1.6   13 Dec 1996 13:20:16   JAY
// RTF Parser
// 
//    Rev 1.5   18 Oct 1996 10:49:44   Jay
// Now sets the update time duirng a drop.
// 
//    Rev 1.4   11 Jul 1996 11:35:44   Jay
//  
// 
//    Rev 1.3   03 Jul 1996 14:31:10   Jay
//  
// 
//    Rev 1.2   02 Jul 1996 08:55:30   Jay
//  
// 
//    Rev 1.1   05 Jun 1996 08:21:44   Jay
//  
// 
//    Rev 1.0   14 Mar 1996 13:27:58   Jay
// Initial revision.
// 
//    Rev 1.0   16 Feb 1996 12:33:18   FRED
// Initial revision.
// 
//    Rev 2.9   15 Nov 1995 15:23:06   JAY
// OLE fixes, etc.
// 
//    Rev 2.8   29 Sep 1995 16:00:08   JAY
// More drag/drop/paste/OLE stuff
// 
//    Rev 2.7   28 Sep 1995 12:13:10   JAY
// More drag/drop stuff
// 
//    Rev 2.6   27 Sep 1995 15:26:26   JAY
// New OLE code
// 
//    Rev 2.5   19 Sep 1995 15:45:14   JAY
// 32-bit changes.
// 
//    Rev 2.4   14 Aug 1995 17:17:48   JAY
// Extensions for cut and paste from warp text dialog.
// 
//    Rev 2.3   15 Mar 1995 10:48:54   JAY
// Support for new text editing
// 
//    Rev 2.2   17 Feb 1995 16:47:22   JAY
// Text editing undo and redo.
// 
//    Rev 2.1   08 Feb 1995 13:39:06   JAY
// Reverted. New series.
// 
//    Rev 1.4   30 Jan 1995 09:34:46   JAY
// Cut and paste
// 
//    Rev 1.3   09 Jan 1995 17:01:02   JAY
// Cut & paste, etc.
// 
//    Rev 1.2   05 Jan 1994 17:15:42   JAY
// Cut and paste!
// 
//    Rev 1.1   05 Jan 1994 08:12:06   JAY
// Restructuring, cut/paste, images, etc.
// 
//    Rev 1.0   16 Dec 1993 16:29:04   JAY
//  
*/

#ifndef __CLIP_H__
#define __CLIP_H__

// forward declarations
class CUpdatingClipboard;
class CCommandList;
class CTxp;

//#define NO_PASTE_OBJECTS				// for now...

enum CLIPBOARD_TYPE
{
	CLIPBOARD_TYPE_unknown				= 0,
	CLIPBOARD_TYPE_text					= 1,
	CLIPBOARD_TYPE_objects				= 2,
	CLIPBOARD_TYPE_object_document	= 3
};

enum UNDO_STATE
{
	UNDO_None,
	UNDO_Normal,
	UNDO_Deleting
};

typedef struct
{
	SHORT m_Type;					// CLIPBOARD_TYPE
	DB_RECORD_NUMBER m_dwRecord;
	FLAGS m_uFlags;
	char m_cbName[_MAX_PATH];	// Used to be exact (for older "text" types).
	PBOX m_World;					// Not guaranteed to exist for "text" types.
} CLIPBOARD_FILENAME, far *CLIPBOARD_FILENAME_PTR;

/*
// A state to hold the open state during pasting.
*/

class CClipboardOpenState
{
public:
	CClipboardOpenState();
	~CClipboardOpenState();

	PMGDatabase* GetDatabase(void);

	CPmwDoc* m_pDoc;
	PagePtr m_pPage;
	DatabaseMapping m_Mapping;
	ObjectList* m_pList;
};

class Clipboard
{
private:
	Array m_Formats;
	DWORD m_dwUpdateTime;					// When were the formats enumerated?
protected:

/*
// Information about the current clipboard contents.
// This information is mirrored in the "CLIPBOARD_FILENAME" record placed
// on the Windows clipboard for other apps (or ours!).
*/

	CLIPBOARD_TYPE m_Type;
	DB_RECORD_NUMBER m_dwRecord;
	FLAGS m_uFlags;
	char* m_pszClipboardName;			// For the external file.
	PBOX m_World;							// World space for objects.
	BOOL m_fRenderedMetafile;
	BOOL m_fRenderedBitmap;
	BOOL m_fRenderedPalette;

	BOOL OpenClipboard(VOID)
		{ return AfxGetApp()->m_pMainWnd->OpenClipboard(); }
	BOOL EmptyClipboard(VOID)
		{ return ::EmptyClipboard(); }
	BOOL CloseClipboard(VOID)
		{ return ::CloseClipboard(); }

	ERRORCODE PrepareClipboardForCopy(CClipboardOpenState* pState, BOOL fNeedDocument = FALSE);
	VOID CreateStandardClipboard(CLIPBOARD_TYPE type, DB_RECORD_NUMBER record, PBOX* pWorld);

	ERRORCODE PrepareObjectPaste(CClipboardOpenState* pState);
	ERRORCODE PrepareDatabasePaste(DatabaseMapping *mapping);
	ERRORCODE PrepareDocumentPaste(CPmwDoc** ppDoc);

	ERRORCODE PasteTextFromDatabase(CTxp* pTxp, BOOL fWarpTextPaste /*=FALSE*/);
	ERRORCODE PasteTextFromClipboard(CTxp* pTxp);

	ERRORCODE PasteObjectsFromClipboard(CPmwView* pView);
	ERRORCODE CopyObjectsToClipboard(CPmwDoc* pDoc);
	
	// Helper function to get local info.
	ERRORCODE EnsureLocalClipboard(LPSTR pName);

	BOOL NeedDocumentClipboard(CPmwDoc* pDoc);
	BOOL NeedDocumentClipboard(PMGPageObjectPtr pObject);

#ifndef NO_PASTE_OBJECTS
	ERRORCODE RenderAsWMF(CUpdatingClipboard* pDialog = NULL);
	ERRORCODE RenderAsBitmap(CUpdatingClipboard* pDialog = NULL);
#endif

public:
	Clipboard();
	~Clipboard();

	VOID DestroyContents(VOID);			// In response to WM_DESTROYCLIPBOARD
	VOID UpdateContents(HWND hWnd);		// In response to WM_DRAWCLIPBOARD

	CLIPBOARD_TYPE TypeOfMlsClipboard(BOOL is_open);
	BOOL FormatIsAvailable(UINT uFormat);

	BOOL CanPasteText(VOID);
	ERRORCODE CopyText(CPmwView* pView);
	ERRORCODE CutText(CPmwView* pView, CCommandList* pCommandList = NULL);
	ERRORCODE PasteText(CPmwView* pView, CCommandList* pCommandList = NULL, BOOL fWarpTextPaste = FALSE);

	BOOL CanPasteObjects(VOID);
	ERRORCODE CopyObjects(CPmwDoc* pDoc);
	ERRORCODE CutObjects(CPmwDoc* pDoc);
	ERRORCODE PasteObjects(CPmwView* pView);
	ERRORCODE PasteObjectsFromDatabase(CPmwDoc* pDoc);

	VOID RenderFormat(UINT nFormat, CUpdatingClipboard* pDialog = NULL);
	VOID RenderAllFormats(VOID);

	// Paste from a data object.
	ERRORCODE PasteFromOleDataObject(CPmwView* pView, UINT uFormat, COleDataObject* pDataObject, PPNT* pOrigin = NULL, BOOL fUpdateTime = FALSE);

	void BuildPasteName(LPCSTR pszExtension, LPSTR pszName);
	void SetUpdateTime(void);

	static UINT m_uFilenameFormat;				// Our registered format.
	static UINT m_uRichTextFormat;				// Rich text format.

	static char m_cbClipboardTemplate[5+1];
	static BOOL m_fRenderOnExit;

	// Clipboard formats (used to be in _oleData before the switch to DLL MFC)
	static UINT m_cfEmbeddedObject;
	static UINT m_cfEmbedSource;
	static UINT m_cfLinkSource;
	static UINT m_cfFileName;
	static UINT m_cfObjectDescriptor;
};

// *** Global externs *** //

extern Clipboard clipboard;

// Convenient routines for copying text around.
extern ERRORCODE CopyTextToDatabase(CTxp* pTxp, PMGDatabasePtr dst_database,
												DB_RECORD_NUMBER *new_precord, UNDO_STATE nUndo = UNDO_None);
extern ERRORCODE PasteTextFromText(CTxp* pTxp, PMGDatabase* pSrcDatabase,
											  DB_RECORD_NUMBER src_record, UNDO_STATE nUndo = UNDO_None,
											  BOOL fWarpTextPaste = FALSE);
extern void CopyTextToText(CTextRecord* pDestText, CHARACTER_INDEX lDestination,
									CTextRecord* pSourceText, CHARACTER_INDEX lStart,
									CHARACTER_INDEX lEnd, UNDO_STATE nUndo = UNDO_None,
									DB_RECORD_NUMBER lFrame = 0);

// Utility to get info from a data object.
extern BOOL GetObjectInfo(COleDataObject* pDataObject, PPNT* pSize, PPNT* pOffset);

// Helper functions for copying/pasting objects
extern ERRORCODE CopyObjectsToDatabase(CPmwDoc* pDoc, CClipboardOpenState* pState,
													DB_RECORD_NUMBER* new_precord, BOOL fSelectedOnly = TRUE, BOOL fSkipSpecial = FALSE);
extern ERRORCODE PasteObjectList(CPmwDoc* pDoc, ObjectList* pList, PBOX pbxWorld);

extern HGLOBAL CreateMetafilePictFromObjects(ObjectList* pList, CUpdatingClipboard* pDialog = NULL);
extern HGLOBAL CreateMetafilePictFromObjects(CPtrList* pList, CUpdatingClipboard* pDialog = NULL);
extern HGLOBAL CreateEnhMetafilePictFromObjects(CPtrList* pList, CUpdatingClipboard* pDialog = NULL);
extern HGLOBAL CreateObjectDescriptorFromObjects(CPtrList *pList);
extern HGLOBAL RenderObjectsAsBitmap(CPtrList* pList, CUpdatingClipboard* pDialog = NULL);
extern HGLOBAL RenderObjectsAsDIB(CPtrList* pList, CUpdatingClipboard* pDialog =NULL);

#endif // __CLIP_H__
