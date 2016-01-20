/*
// $Workfile: PMWDOC.H $
// $Revision: 1 $
// $Date: 3/03/99 6:09p $
//
//  "This unpublished source code contains trade secrets which are the
//   property of Mindscape, Inc.  Unauthorized use, copying or distribution
//   is a violation of international laws and is strictly prohibited."
// 
//        Copyright © 1997 Mindscape, Inc. All rights reserved.
*/

/*
// Revision History:
//
// $Log: /PM8/App/PMWDOC.H $
// 
// 1     3/03/99 6:09p Gbeddow
// 
// 80    3/01/99 11:55a Cschende
// added functions to make permanent font substitutions in the document
// after it is read in for font which are no longer going to be
// distributed with PrintMaster
// 
// 79    2/26/99 8:37p Rgrenfel
// Removed the assumption that methods used GraphicObject pointers.
// 
// 78    2/25/99 3:27p Lwilson
// Changed component creation to use a pointer to a CCreationData class or
// variant for in place of an RComponentDocument pointer.
// 
// 77    2/15/99 12:59p Psasse
// ChangerRotation() moves from the View to the Doc class
// 
// 76    2/01/99 5:10p Johno
// Compile update
// 
// 75    2/01/99 4:34p Lwilson
// Added initial support for converting PM objects into component objects.
// 
// 74    2/01/99 2:46p Blee
// Added virtual bool IsMultiPageProjectType()
// Added OnUpdateAddPage()
// Added OnUpdateGotoPage()
// 
// 73    2/01/99 9:38a Johno
// Changes for paneled HTML
// 
// 72    1/30/99 11:31p Psasse
// GetPaperInfo and SizeToInternalPaper become virtual functions
// 
// 71    1/28/99 6:20p Johno
// Got rid of goofy "CDIBInfo is a member of CPmwDoc (not really)" syntax
// Preliminary separate panel backgrounds
// 
// 70    1/27/99 10:50a Johno
// Compile update
// 
// 69    1/26/99 6:34p Psasse
// Added PhotoProjects Project Type (still in progress)
// 
// 68    1/22/99 4:04p Johno
// Prevent ConvertToHTML() with projects that don't support it
// 
// 67    1/21/99 6:13p Johno
// Changes for links, hotspots, and text links!
// 
// 66    1/20/99 4:58p Johno
// Changes for paneled HTML
// 
// 65    1/14/99 2:25p Johno
// Fixed display problem (after creating HTML)
// rearranged code with HTMLCONV.CPP
// 
// 64    1/14/99 11:39a Johno
// Oops! (Compile update)
// 
// 63    1/13/99 6:01p Johno
// Moved some HTML stuff from HTMLCONV to PMWDOC
// 
// 62    1/13/99 10:47a Johno
// Changes for HTML projects
// 
// 61    1/07/99 6:11p Johno
// Preliminary "paneled HTML" for some projects
// Changes to eliminate some h file dependecies
// 
// 60    1/07/99 5:12p Rgrenfel
// Added flag and associated methods to spot that a drop render was
// active.
// 
// 59    1/04/99 5:12p Johno
// Moved CWebView / CWebDoc code into CPmwDoc / CPmwView.
// 
// 58    12/23/98 3:51p Johno
// Changes for projects as HTML
// 
// 57    12/22/98 10:28a Johno
// Moved most HTML conversion stuff here from webdoc.*
// 
// 56    12/19/98 4:38p Psasse
// Revert to Saved support
// 
// 55    12/15/98 3:37p Lwilson
// Initial support for PS component editing.
// 
// 54    12/08/98 5:18p Lwilson
// Initial Print Shop integration.
// 
// 53    10/19/98 10:50a Jayn
// More COM restructuring changes.
// 
// 52    10/05/98 9:53a Mwilson
// changes to print projects utility
// 
// 51    10/01/98 10:25a Mwilson
// added friendly name to instruction page
// 
// 50    9/30/98 4:44p Mwilson
// added extraction of friendly name in SetBrowserPath
// 
// 49    9/25/98 11:07a Mwilson
// added support for instruction page
// 
// 48    9/23/98 10:28a Jayn
// Converted CollectionManager to COM-style object.
// 
// 47    9/21/98 5:27p Jayn
// Broke PathManager out into a COM-style interface.
// 
// 46    8/10/98 3:47p Hforman
// Crop Copyright modifications
// 
// 45    7/18/98 4:08p Johno
// Added GetPanelBoundForPreview() - provides for override (currently used
// by CCalendarDoc)
// 
// 44    7/14/98 5:35p Johno
// Added FLAG_LoadedFromCollection and TestDocumentFlag
// 
// 43    6/29/98 3:22p Johno
// Added IsThisPage() - tests if a page with the given record number
// exists
// 
// 42    6/24/98 7:20p Psasse
// correctly extracting *.pmo and *.dtt files
// 
// 41    6/18/98 7:05p Psasse
// force first page to appear in workspace for templates
// 
// 40    6/05/98 2:33p Fredf
// Added EmbedAllPictures()
// 
// 39    6/05/98 8:47a Fredf
// FLAG_BringUpReceiverList (HACK!)
// 
// 38    6/01/98 1:24p Mwilson
// changed name of embed_all_pictures and added function to embed exteranl
// pictures only
// 
// 37    5/31/98 5:11p Fredf
// More Address Book and merge field changes.
// 
// 36    5/26/98 11:42a Jayn
// 
// 35    4/22/98 4:35p Rlovejoy
// Added virtual DoPostOpen().
// 
// 34    4/21/98 3:39p Fredf
// Reworked "reverse" printing to apply to all projects.
// 
// 33    4/18/98 4:24p Fredf
// T-Shirt printing.
// 
// 32    4/13/98 2:38p Hforman
// add user categories to save
// 
// 31    3/26/98 10:09a Jayn
// Support for DPP (Serif) importing, previewing, etc.
// 
// 30    3/20/98 6:03p Jayn
// Serif Drawing Object
// 
// 29    3/19/98 2:32p Jayn
// Finish up master pages.
// 
// 28    3/18/98 11:15a Jayn
// Page numbers
// 
// 27    3/17/98 11:56a Jayn
// Master-pages, double-sided flag.
// 
// 26    3/10/98 5:24p Jayn
// Master page changes
// 
// 25    3/02/98 3:30p Johno
// Moved web stuff to webdoc.cpp / h
// 
// 24    2/27/98 2:47p Jayn
// "Draw as bitmaps"
// 
// 23    2/24/98 9:13a Dennis
// Added Free() to CDIBInfo()
// 
// 22    2/23/98 11:05a Johno
// 
// 21    2/20/98 7:12p Hforman
// remove param from delete_object()
// 
// 20    2/20/98 12:15p Johno
// Compile update
// 
// 19    2/17/98 11:45a Dennis
// Added Background Object
// 
// 18    2/04/98 4:49p Hforman
// add param to PostPaste()
// 
// 17    1/21/98 9:17a Jstoner
// added select all cells function...for drag/select in table
// 
// 16    1/13/98 6:07p Hforman
// add parent param to ChoosePicture()
// 
// 15    1/13/98 11:15a Jayn
// Transparent GIF support.
// 
// 14    12/18/97 3:20p Dennis
// Added SetDefaultFace() & DefaultFace() methods to allow overiding
// default typeface for a doc. type.
// 
// All object types except Text are converted to HTML.
// 
// 13    12/08/97 2:49p Dennis
// Added dest. directory to ConvertToHTML()
// 
// 12    11/25/97 10:33a Johno
// Got rid of CPmwDoc::CDIBInfo::PrepareData
// 
// 11    11/24/97 8:04a Jstoner
// changed object center on page function to handle top/bottom and
// left/right page calendars
// 
// 10    11/21/97 11:28a Johno
// 
// 9     11/20/97 10:36a Johno
// Added CPmwDoc::CDIBInfo::PrepareData for saving DIBs in other format
// files
// 
// 8     11/19/97 3:27p Hforman
// added function prototype
// 
// 7     11/18/97 1:59p Johno
// BuildPanelDIB and BuildDIB now return ERRORCODE
// 
// 6     11/14/97 12:02p Hforman
// changed get_*_guides()
// 
// 5     11/14/97 11:29a Dennis
// Added options to BuildPanelDIB so it can use a CPtrList
// 
// 4     11/05/97 10:41a Hforman
// add methods for getting guide arrays
//
//3     11/03/97 2:14p Jayn
//New CPtrList DrawObjects() method.
*/

#ifndef __PMWDOC_H__
#define __PMWDOC_H__

// pmwdoc.h : interface of the CPmwDoc class
//
/////////////////////////////////////////////////////////////////////////////

#include "system.h"
#include "pmgdb.h"
#include "objlist.h"
#include "docrec.h"

#include "dbmap.h"
#include "command.h"
#include "icollmgr.h"
#include "ipathmgr.h"
#include "filemgr.h"    // For CFileManager

class CWebPageStats;

#define	HINT_Refresh			1L
#define	HINT_Reset				2L
#define	HINT_CancelEdit		3L
#define	HINT_ReframeOnTxp		4L
#define	HINT_Rulers				5L
#define	HINT_RefreshPreview	6L

// forward declarations
class CPmwDoc;
class CStdMacroServer;
class CPaperInfo;
class ObjectList;
class CPmwCntrItem;
class CFrameObject;
class CPmwView;
class GraphicObject;
class CMlsStringArray;
class Array;
class CCmdRelayerSelects;
class CBackgroundObject;
class StorageFile;
class CDesignCheckerDlg;
class CHTMLDocConverter;
class	CHTMLTagDiv;
class CPhotoPrjData;
class CDIBInfo;
class CCreationData;

class CPmwDocCommandNotify : public CCommandNotify
{
public:
	CPmwDocCommandNotify();
   void SetDocument(CPmwDoc* pDoc)
      { m_pDoc = pDoc; }
	virtual BOOL Notify(CCommand* pCommand, int nNotifyCode);
protected:
   CPmwDoc* m_pDoc;
};

//class CPmwDoc : public COleServerDoc
class CPmwDoc : public COleDocument
{
//	INHERIT(CPmwDoc, COleServerDoc)
	INHERIT(CPmwDoc, COleDocument)

private:

	static VOID refresh_notify(
		VOIDPTR data,
		PBOX_PTR area,
		REFRESH_TYPE type,
		PMGPageObjectPtr object,
		LPRECT lprExtraPixels = NULL);
	static VOID document_modified_handler(VOIDPTR data);

   void size_guides(ObjectListPtr list, PBOX oldbound, PBOX newbound);
   void size_list(ObjectList* list, PCOORD old_xsize, PCOORD old_ysize, PCOORD new_xsize, PCOORD new_ysize);

	void embed_pictures(BOOL bExternalOnly = FALSE);

	void handle_registration_items(void);

	void MakeFontSubstitutions();

protected:

	IPathManager* m_pPathManager;				//	The path manager for this document.
	ICollectionManager* m_pCollectionManager; // The collection manager for this document.
	SHORT file_type;								// Relevant if loaded

	DatabaseMapping	mapping;					// Database mapping.
	COleStreamFile		m_Stream;				// OLE Stream we are based on.

   DocumentPtr			document;				// Locked document object
   ObjectList*			page_objects;
	PMGPageObjectPtr	selected_list;
	SHORT					current_panel;
	FLAGS					panel_flags;			// Xor'd with normal panel flags
	PBOX					portion;					// bound of editing portion
	ProjectInfo			info;						// some project-specific info

	BOOL					m_fAddRefreshes;		// Do we add refreshes?
	BOOL					refreshes_suspended;	// Do we process refreshes?
	BOOL					m_fUserSave;			// Have we been saved?

	CPaperInfo*			m_pPaperInfo;
	CString				m_csBrowserPath;		// Full original loaded path. */

	CString				m_csFriendlyName;

   CBackgroundObject *m_pBackgroundObject;
	// Object creation variables.
	CObjectProperties m_DefaultProperties;

	GRAPHIC_CREATE_STRUCT gcs;

	void AdjustSelects(SHORT id);

	CCommandList m_CommandList;
	CPmwDocCommandNotify m_CommandNotify;

	void DoRelayerCommand(CCmdRelayerSelects* pCommand);
	void DeselectInPlaceObject(void);

public:

	IPathManager* GetPathManager(void)
		{ return m_pPathManager; }

	ICollectionManager* GetCollectionManager(void)
		{ return m_pCollectionManager; }
	
	void EmbedAllPictures(){embed_pictures(FALSE);}
	void EmbedExternalPictures(){embed_pictures(TRUE);}
	virtual void GoToFirstPage();
	CString GetFriendlyName(){return m_csFriendlyName;}

	CPmwView* GetCurrentView(void) const;

	BOOL ExtractFile(CString& csFileName);
	// Are we opening a document within an album?
	enum eDocumentFlag
	{
		FLAG_AlbumDocument = 1,				// Big preview in album
		FLAG_ClipDocument = 2,				// Used for clipboard
		FLAG_TestPrintDocument = 4,		// Used to print test page
		FLAG_AutoLoadDocument = 8,			// Loaded from command line
		FLAG_AddressBookDocument = 16,	// Used by address book
		FLAG_WithHelpDocument = 32,		// Made by With Help wizard
		FLAG_NoMRU = 64,						// Do not add to MRU.
		FLAG_BringUpReceiverList = 128,	// Bring up receiver list when view is created
		FLAG_LoadedFromCollection = 256	// 
	};
	static WORD m_wGlobalDocumentFlags;

	BOOL
	TestDocumentFlag(eDocumentFlag f)
	{
		return (m_wDocumentFlags & f) == 0 ? FALSE : TRUE;
	}

	WORD m_wDocumentFlags;

	virtual CPaperInfo* GetPaperInfo(void)
		{ return m_pPaperInfo; }

	virtual void  SetCurrentSubArea(const int x)
	{   return;  }

	virtual CPhotoPrjData* GetPhotoProjectInfo(void)
	{	return NULL;	}

	virtual int GetTotalSlots(void)
	{	return 0;	}

	void AssignPaperInfo(CPaperInfo& pPaperInfo);

	static BOOL allow_edit_templates;
	static BOOL make_templates;
	static BOOL m_fFixTemplates;

	static BOOL handle_missing_graphic(VOIDPTR data, GraphicPtr graphic, GRAPHIC_CREATE_STRUCT_PTR gcs, int16 *index);
	static WORD wMaxPreviewMemory;
	static BOOL LookupMissingPicture(LPCSTR pszBaseName, LPCSTR pszFullName, LPCSTR pszCollection, int nSubdirs, GRAPHIC_CREATE_STRUCT* gcs);

	// Convenient criteria functions for searching in the object list.
	static BOOL NotBorderCriterion(ListElementPtr element, VOIDPTR data);
	static BOOL LastBorderCriterion(ListElementPtr element, VOIDPTR data);

	// The name of the stream we use in the storage.
	static const char near DocumentStreamName[];

	BOOL AddRefreshes(void) const
		{ return m_fAddRefreshes; }
	void AddRefreshes(BOOL fAddRefreshes)
		{ m_fAddRefreshes = fAddRefreshes; }
	/*
	// Browser category related.
	*/
	const CString& GetBrowserPath(void) const
		{ return m_csBrowserPath; }
	void SetBrowserPath(const CString& csBrowserPath);
	/*
	// Object properties related.
	*/
	CObjectProperties& GetDefaultObjectProperties(void)
		{ return m_DefaultProperties; }
	/*
	// Miscellaneous commands.
	*/
	void SendToBack(void);
	void BackOne(void);
	void ForwardOne(void);
	void SendToFront(void);

	void CenterSelects(void);
	void HCenterSelects(void);
	void VCenterSelects(void);
	void FullPageSelects(void);

	// Frame intersection management.
	BOOL MarkFramesBelow(PMGPageObject* pBelowObject, PMGPageObject* pFirstObject = NULL);
	void MarkFramesBelowSelectedObjects(void);
	void MarkFrame(CFrameObject* pObject);
	void UpdateMarkedFrames(BOOL fDoIt, PMGPageObject* pObject = NULL);

	// Graphic update. This checks the time stamp, etc.
	void UpdateGraphics(ObjectList* pList = NULL);
	/*
	// Command do, undo, and redo support.
	*/

	BOOL AddCommand(CCommand* pCommand, BOOL fDoIt = TRUE);
	void AddCreateCommand(WORD wCmdID);
	void ClearAllCommands(void);

	BOOL CanUndo(void) const
		{ return m_CommandList.CanUndo(); }
	void UndoName(CString& csName) const
		{ m_CommandList.UndoName(csName); }
	CCommand* UndoCommand(void) const
		{ return m_CommandList.UndoCommand(); }
	BOOL Undo(void)
		{ return m_CommandList.Undo(); }

	BOOL CanRedo(void) const
		{ return m_CommandList.CanRedo(); }
	void RedoName(CString& csName) const
		{ m_CommandList.RedoName(csName); }
	CCommand* RedoCommand(void) const
		{ return m_CommandList.RedoCommand(); }
	BOOL Redo(void)
		{ return m_CommandList.Redo(); }

	// Preview overridables.
	virtual BOOL DrawPreview(REFRESH_EXTENT_PTR re, RedisplayContext* rc);
	virtual void GetPreviewBound(PBOX* pBound);
	virtual void GetPanelBoundForPreview(PBOX* pBound)
	{
		get_panel_world(pBound);
	}

	//
	// A routine that used to be in CPMWCollection, but fit better here.
	//

	void GetPreviewInfo(const CRect &crDest, BOOL bDrawPreview, CSize &sizeNew, PBOX &pboxBound);

	/**/
	virtual bool IsMultiPageProjectType() {	return FALSE; }

	BOOL AddPage(DWORD dwAtPage);
	void GotoPage(DWORD dwPage, BOOL fUpdateViews = TRUE);
	void LoadPage(DB_RECORD_NUMBER recPage = 0, BOOL fUpdateMenu = TRUE, BOOL fUpdateViews = TRUE);
	void PostProcessObjects(ObjectList* pList);

	VOID flush(VOID);
	virtual PPNT get_dimensions() { return document->get_dimensions(); }
   VOID set_dimensions(PPNT pPntNewDims) { document->set_dimensions(pPntNewDims); }
	SHORT get_current_panel(VOID) { return current_panel; }
	VOID set_current_panel(SHORT panel) { current_panel = panel; }
	ProjectInfo& GetProjectInfo(void) { return info; }
	PROJECT_TYPE get_project_type(VOID) { return info.project_type; }
	FLAGS get_panel_flags(VOID) { return panel_flags; }
	VOID set_panel_flags(FLAGS flags) { panel_flags = flags; }
	VOID set_portion(PBOX_PTR pbox = NULL);
	PBOX get_portion(VOID) { return portion; }
	GRAPHIC_CREATE_STRUCT_PTR get_gcs() { return &gcs;	}
	SHORT get_file_type() { return file_type; }
   void set_file_type(SHORT fileType) { file_type = fileType; }
   Array* get_horiz_guides() { return page_objects->GetHorizGuides(); }
   Array* get_vert_guides() { return page_objects->GetVertGuides(); }

   // Call UpdatePageProperties() when page properties have changed
   void UpdatePageProperties();
   void UpdatePageProperties(CPageProperties *pPageProp);

	// Code for updating the page number on the master pages when loaded.
   void UpdateMasterPageItems(void);
   
	void UpdateInstructionPageItems(void);
	BOOL UpdateInstructionPages(void);
	
	void ReflowPageMacros(ObjectList* pList);

	SHORT number_of_panels(VOID) { return info.number_of_panels; }
	SHORT frame_layout_id(VOID) { return info.frame_layout_id; }
	SHORT graphic_layout_id(VOID) { return info.graphic_layout_id; }
	SHORT duplicate_layout_id(VOID) { return info.duplicate_layout_id; }

	VOID reset_fonts(VOID);
	int ChoosePicture(LPCSTR pszLastName = NULL, RectPageObjectPtr pLastPicture = NULL, CWnd* pParent = NULL);
	int ChooseExternalPicture(LPCSTR pszLastName, RectPageObjectPtr pLastPicture, LPCSTR pszDefaultPath, BOOL fPhotoCD = FALSE);
	ERRORCODE create_chosen_picture(PBOX_PTR view_pbox = NULL, PPNT* pAlternateOrigin = NULL);
	void handle_new_object(RectPageObjectPtr object, PBOX_PTR view_pbox = NULL, BOOL fAppend = TRUE, PPNT* pAlternateOrigin = NULL);
	
	ERRORCODE create_new_component(CCreationData* pCreationData, PBOX_PTR view_pbox, PPNT* pOrigin = NULL, ComponentObject** ppObject = NULL);
	void replace_component(ComponentObjectPtr pObject, CCreationData* pData, BOOL fResize);

	VOID choose_background(VOID);
	VOID choose_calendar_picture(VOID);
	VOID replace_picture(VOID);
	VOID choose_picture_attributes(GraphicObjectPtr object);
	void DoReplacePicture(RectPageObjectPtr pObject, GRAPHIC_CREATE_STRUCT* pGCS, BOOL fResize, UINT uIDCmd);

	ORIENTATION get_orientation(VOID)
 		{ return document->get_orientation(); }

	virtual ORIENTATION GetPrintOrientation(void)
		{ return get_orientation(); }

	VOID get_panel_world(PBOX_PTR result, SHORT panel = -1);
	VOID get_panel_world_dims(PBOX_PTR world, PCOORD xsize, PCOORD ysize, SHORT this_panel = -1);

	VOID reset_views(VOID);
	VOID ReframeOnTxp(VOID);

	void ChangeRotation(double dNewAngle);

	virtual void DoPostOpen(void) {}

/*
// Link frames support.
*/

	BOOL LinkFrame(DB_RECORD_NUMBER lSourceFrame, DB_RECORD_NUMBER lLinkFrame, DB_RECORD_NUMBER* pPreviousFrame = NULL);
	BOOL UnlinkFrame(DB_RECORD_NUMBER lFrame, BOOL fCreateText = TRUE);
	BOOL DetachFrame(DB_RECORD_NUMBER lFrame, BOOL fCreateText = TRUE);
	
/*
// Resize the document to fit on the paper.
*/

	virtual VOID size_to_paper(VOID);

/*
// Return the paper info to use for tile calculations.
*/

	virtual CPaperInfo* PrintPaperInfo(void);

/*
// Resize the document to fit the paper set within us.
*/

	virtual void SizeToInternalPaper(void);

/*
// Resize the document to fit on the paper.
*/

	virtual void SizeAllPages(PPNT OldSize, PPNT NewSize);
	virtual void SizeCurrentPage(PPNT OldSize, PPNT NewSize);

/*
// Get the default text size for this project.
*/

	virtual int DefaultTextSize(int nDefault);

	void SetDefaultTextSize(void);

   // Get Default Face
	virtual LPCSTR DefaultFace(void);

	void SetDefaultFace(void);

/*
// Special project stuff.
*/
	virtual void GetObjectPageDims(PBOX * pBox, PageObject * pObject);
	virtual CalendarObjectPtr get_current_calendar(BOOL use_global = FALSE);
	virtual SHORT get_date_selected(LPSHORT number = NULL);
	virtual CFrameObject* get_main_frame(VOID);
	virtual BOOL resize_project(CFrameObject* pFrame = NULL);

	static BOOL IsGroupedFrame(PMGPageObject* pObject);

/* Calculate an object taking into account the panel to make layouts work. */

	VOID calc_object(PMGPageObjectPtr object, BOOL ignore_current = FALSE);

/* Necessities. */

	PMGDatabase*		get_database(VOID) { return mapping.database; }
	FontServer*			get_font_server(VOID) { return mapping.font_server; }

	ERRORCODE BuildPanelDIB(CDIBInfo& Info, POINT MaxDims, BOOL bTrueColor=FALSE, COLORREF clBackground = RGB(255, 255, 255));
	ERRORCODE BuildDIB(CDIBInfo& Info, PBOX Bound, POINT MaxDims, CPtrList *plObjectList=NULL, BOOL bTrueColor=FALSE, COLORREF clBackground = RGB(255, 255, 255));
	// WEB page (HTML) stuff
   BOOL
	SupportsHTML(void);
	
	BOOL
	NormalHTML(void);

   virtual CBackgroundObject*
	GetBackgroundObject();
   
	virtual void
	AddBackgroundForDrawing(CPtrList &List);
	// Get properties associated with current page such as background
   // Texture and Text colors used with web pages   
   virtual CPageProperties * 
	GetPageProperties(); 
   // HTML document conversion methods 
   ERRORCODE 
	ConvertToHTML(const CString &csDirectory,
                     CFileManager *pcfmFileList=NULL,
                     CDesignCheckerDlg *pDesignCheckerDlg = NULL)
	{
		ASSERT(SupportsHTML() == TRUE);
		if (SupportsHTML() == FALSE)
			return ERRORCODE_IllegalType;

		if (NormalHTML() != FALSE)
		{
			return ConvertToPagedHTML(csDirectory, pcfmFileList, pDesignCheckerDlg);
		}
		else
		{
			return ConvertToPaneledHTML(csDirectory, pcfmFileList, pDesignCheckerDlg);	
		}
	}
protected:
   ERRORCODE 
	ConvertToPagedHTML(const CString &csDirectory,
                     CFileManager *pcfmFileList=NULL,
                     CDesignCheckerDlg *pDesignCheckerDlg = NULL);
   ERRORCODE 
	ConvertToPaneledHTML(const CString &csDirectory,
                     CFileManager *pcfmFileList=NULL,
                     CDesignCheckerDlg *pDesignCheckerDlg = NULL);

	ERRORCODE 
	BuildHTMLObjectList(ObjectList *pList, CPtrList &HTMLObjects);
	
	ERRORCODE 
	AddPageObject(PMGPageObject *pPMGPageObject, CPtrList &HTMLObjects, BOOL OneBigGraphic = FALSE);
/* Create the macro list. */
	BOOL CreateMacroList(void);
/* Read the macro list (calls create if none exists). */
	BOOL GetMacroList(void);
/* Create the name list. */
	BOOL CreateNameList(void);
/* Read the name list (calls create if none exists). */
	BOOL GetNameList(void);

public:

	CStdMacroServer* GetMacroServer(void)
		{ return mapping.m_pMacroServer; }

	BOOL EditNameList(BOOL fSelectReceivers = FALSE);
	BOOL EditSender(void);
/* Field access. */
	BOOL ViewRecord(DWORD dwKey);
	void ViewSampleRecord(void);

	void ReflowMacros(CWordArray* pMacroList = NULL);
/* Get an array of macros in use. */
	void GetMacrosInUse(CWordArray& cwaMacros);

	BOOL HasDatabaseMacro(void);
/**/
	// Inform the view that a drop render is in progress
	static inline void SetDropRenderActive()
		{ m_fDropRenderActive = TRUE; }
	// Inform the view that the drop render is complete.
	static inline void ClearDropRenderActive()
		{ m_fDropRenderActive = FALSE; }
	// Check to find out if the app is currently performing a drop render.
	static inline bool IsDropRenderActive()
		{ return m_fDropRenderActive; }

	BOOL draw_objects(REFRESH_EXTENT_PTR re, RedisplayContext *rc);

	ObjectList* object_list() { return page_objects; }
	PMGPageObjectPtr objects(void);
	PMGPageObjectPtr selected_objects() { return selected_list; }
	void GetSelectRefreshBound(PBOX_PTR pBounds, RedisplayContext* rc = NULL);
	BOOL GetSelectBound(PBOX_PTR pBounds);

	virtual void FlipProject(FLAGS Flags);
	virtual void FlipPage(DB_RECORD_NUMBER Page, FLAGS Flags);

	void SetImportPictureName(LPCSTR pszFileName, LPCSTR pszFriendlyName = NULL);
	void free_import_picture_name(VOID);

	BackgroundObjectPtr find_background(VOID);

	VOID select_object(PMGPageObjectPtr object, BOOL fUpdateMenu = TRUE);
	VOID select_all(PBOX_PTR area = NULL);
	VOID select_all_cells(PBOX_PTR area = NULL, BOOL bDeselect = FALSE);  // for table object cell selection
	BOOL object_selected(PMGPageObjectPtr object);
	VOID deselect_object(PMGPageObjectPtr object);
	void deselect_all(BOOL fUpdateMenu = TRUE);

	VOID add_refresh(
		PBOX_PTR extent,
		REFRESH_TYPE type = REFRESH_ALL,
		PMGPageObjectPtr object = NULL,
		LPRECT lprExtraPixels = NULL);
	VOID refresh_object(PMGPageObjectPtr object);
	BOOL idle_refresh(VOID);				// called by OnIdle()

	BOOL toggle_selects(CView *view = NULL, HDC hdc = 0, PMGPageObjectPtr object = NULL, SHORT which = 0);
	BOOL toggle_object(PMGPageObjectPtr object = NULL, SHORT which = 0)
		{ return toggle_selects(NULL, 0, object, which); }

	BOOL confirm_deletion(PMGPageObjectPtr object = NULL);
	FLAGS calc_selects(BOOL set_manual_layout = FALSE);
	VOID delete_selects(BOOL fConfirm = TRUE, WORD wCmdID = 0);
	VOID group_selects(VOID);
	VOID ungroup_selects(VOID);
	VOID xflip_selects(VOID);
	VOID yflip_selects(VOID);
	BOOL duplicate_picture(VOID);
	VOID duplicate_selects(VOID);
	void SetSelectedProperties(CObjectProperties& Properties, CPmwView* pView = NULL, WORD wCmdID = 0);
	void GetSelectedProperties(CObjectProperties& Properties);

	void append_object(PMGPageObjectPtr object);
	void attach_background(PMGPageObjectPtr object);
	void detach_object(PMGPageObjectPtr object);
	void delete_object(PMGPageObjectPtr object);

	void insert_object(PMGPageObjectPtr object, int nIndex);
/* Menu UI methods. */
	BOOL can_group(PBOX_PTR bound = NULL);
	BOOL can_ungroup(VOID);
	BOOL something_selected(VOID);
	BOOL one_selection(VOID);
/* Choose a font (actually a font style). */
	int ChooseFont(CTextStyle& Style);
	SHORT choose_color(COLOR &color);

	const BOOL GetSaved(void) const
		{ return m_fUserSave; }
	void SetSaved(const BOOL fSaved)
		{ m_fUserSave = fSaved; }
/*
// Process an object's action.
// This can be overridden by derived document types for special objects!
*/
	virtual VOID process_action(CView *view, PMG_OBJECT_ACTION action, PMGPageObjectPtr object, LPVOID data, PPNT p);
	// Move to next or previous object.
	virtual void SelectNextObject(void);
	virtual BOOL CanSelectNextObject(void);
	virtual void SelectPreviousObject(void);
	virtual BOOL CanSelectPreviousObject(void);

public: // create from serialization only
//protected: // create from serialization only
	CPmwDoc();
	DECLARE_DYNCREATE(CPmwDoc)
// Attributes
public:
// Operations
public:

	BackgroundObjectPtr create_background_object(GRAPHIC_CREATE_STRUCT_PTR gcs)
		{ return mapping.database->create_background_object(gcs); }

	GraphicObjectPtr create_graphic_object(GRAPHIC_CREATE_STRUCT_PTR gcs)
		{ return mapping.database->create_graphic_object(gcs); }

	CFrameObject* create_frame_object(VOIDPTR creation_data)
		{ return mapping.database->create_frame_object(creation_data); }

	PatternObjectPtr create_pattern_object(VOIDPTR creation_data)
		{ return mapping.database->create_pattern_object(creation_data); }

	ComponentObjectPtr create_component_object(VOIDPTR pCreationData)
		{ return mapping.database->create_component_object(pCreationData); }

	BOOL register_used_fonts(BOOL has_reg_items = FALSE);
/*
// Copy/paste support (called by the clipboard and whatever else).
*/
	virtual PMGPageObject* CopyObject(PMGPageObjectPtr pObject, ObjectList* pObjects);
	virtual PMGPageObject* PasteObject(PageObjectPtr pObject, PBOX* pOldWorld, PBOX* pBounds = NULL, BOOL fDuplicate = TRUE);
	// Helpers.
	virtual PMGPageObject* PasteBackground(PageObjectPtr pObject, PBOX* pOldWorld, PBOX* pBounds = NULL, BOOL fDuplicate = TRUE);
	virtual PMGPageObject* PasteBackgroundAsPicture(PageObjectPtr pObject, PBOX* pOldWorld, PBOX* pBounds);
	virtual PMGPageObject* DoPasteObject(PageObjectPtr pObject, PBOX* pOldWorld, PBOX* pBounds = NULL, BOOL fDuplicate = TRUE);
	virtual PMGPageObject* DoDuplicateObject(PageObjectPtr pObject, PBOX* pOldWorld, PBOX* pBounds, BOOL fDuplicate, ERRORCODE* pError);
	virtual void ResizePastedObject(RectPageObjectPtr pNewObject, PBOX* pOldWorld, PBOX* pBounds = NULL, BOOL fProportional = TRUE);
	virtual GraphicObjectPtr ConvertBackgroundToGraphic(BackgroundObjectPtr pObject);
	virtual void PrePaste(void);
	virtual void PostPaste(BOOL fAddUndo = TRUE);
	virtual void AbortPaste(void);

	void PasteSentiment(const CMlsStringArray& csaSentiment);
// Implementation
protected:
	BOOL FlushDocument(void);
	virtual BOOL ReadDocument(void);
	BOOL WriteDocument(void);
public:
	virtual ~CPmwDoc();
	BOOL DoNewDocument(int nOrientation = -1, BOOL fSizeToPaper = TRUE);
	virtual void Serialize(CArchive& ar);	// overridden for document i/o
	virtual	void DeleteContents();
	virtual BOOL SaveModified(); // return TRUE if ok to continue
	virtual COleServerItem* OnGetEmbeddedItem();
	virtual void OnCloseDocument(void);
	BOOL DoSave(const char* pszPathName, BOOL bReplace=TRUE);
	// Actually do the save. Not normally called (except by save code).
	BOOL SaveToFile(LPCSTR pNewName,
						 BOOL fReplace,
						 BOOL fEmbedPictures = FALSE,
						 BOOL fAddToBrowser = FALSE,
						 LPCSTR pszDescription = NULL,
						 LPCSTR pszKeywords = NULL,
						 LPCSTR pszCategory = NULL);

	virtual void SetPathName(const char* pszPathName, BOOL bAddToMRU = TRUE);

	static char tempfile_template[5+1];

	ERRORCODE CreateObjectFromFile(LPCSTR pName, PPNT* pWhere);
	void PasteDropFiles(HDROP hDropInfo, PPNT* pWhere);
	ERRORCODE FinishOleObjectCreate(CPmwCntrItem* pItem, PPNT* pWhere = NULL, int nObjectType = OBJECT_TYPE_OleObject);

#ifdef _DEBUG
	virtual	void AssertValid() const;
	virtual	void Dump(CDumpContext& dc) const;
#endif
protected:
	virtual	BOOL	OnNewDocument();
	virtual	BOOL	OnOpenDocument(const char *pszPathName);
	virtual	BOOL	OnSaveDocument(const char *pszPathName);

	virtual	ERRORCODE CreatePages(void);
	// OnNewDocument helper functions.
	ERRORCODE NewDocument(ORIENTATION orientation = PORTRAIT);
	// OnOpenDocument helper functions.
	HRESULT ConvertFileToStorage(LPCSTR pszPathName);
	HRESULT CopyStorageToTemp(LPCSTR pszPathName);
	HRESULT OpenStorageFile(LPCSTR pszPathName);
	BOOL CreateStorageFile(void);

	HRESULT CreateTemporaryStorage(LPSTORAGE* ppStorage);
// Path of inspiration
public:
   // JRO 5.0 hub buttons
   virtual void
   AddNewStuff (void);

   static void 
   SetNewPictureNames (LPCSTR FileName, LPCSTR FriendlyName, BOOL fEmbed, BOOL fCropCopyright=FALSE)
   {
      m_csNewPictureFileName = FileName;
      m_csNewPictureFriendlyName = FriendlyName;
		m_fEmbedNewPicture = fEmbed;
		m_fCropCopyright = fCropCopyright;
   }
   
   static void
   KillNewPictureNames (void)
   {
      m_csNewPictureFileName.Empty ();
      m_csNewPictureFriendlyName.Empty ();
   }

	static void
   SetNewSentiment (CStringArray &sa)
   {
      //KillNewSentiment ();
      CopySentiment (sa, m_csaNewSentiment);
   }
   
	static void
   GetSentiment (CStringArray &da)
   {
      CopySentiment (m_csaNewSentiment, da);
   }

   static void
   CopySentiment (CStringArray &sa, CStringArray &da)
   {
      da.RemoveAll ();
      for (int i = 0; i < sa.GetSize (); i ++)
         da.SetAtGrow (i, sa [i]);

      
   }

   static void
   KillNewSentiment (void)
   {
      m_csaNewSentiment.RemoveAll ();   
   }

   static void
   KillNewStuff (void)
   {
      KillNewPictureNames ();
      KillNewSentiment ();
   }
   //
private:
   static CString       m_csNewPictureFileName, m_csNewPictureFriendlyName;
   static BOOL          m_fEmbedNewPicture;
	static BOOL				m_fCropCopyright;
   static CStringArray  m_csaNewSentiment;
	// Stores if a drop render is in progress.  Don't delay render when this is true.
	static bool m_fDropRenderActive;
// Generated message map functions
protected:
	//{{AFX_MSG(CPmwDoc)
	afx_msg void OnFileSave();
	afx_msg void OnRevertToSavedFile();
	afx_msg void OnUpdateFileSave(CCmdUI* pCmdUI);
	afx_msg void OnUpdateRevertToSavedFile(CCmdUI* pCmdUI);
	afx_msg void OnFileSaveAs();
	afx_msg void OnUpdateAddPage(CCmdUI* pCmdUI);
	afx_msg void OnUpdateGotoPage(CCmdUI* pCmdUI);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	DWORD CurrentPageIndex(void);
	DB_RECORD_NUMBER CurrentPageRecord(void);

	DWORD NumberOfPages(void);
	DB_RECORD_NUMBER GetPage(DWORD dwPage);

	DWORD NumberOfMasterPages(void);
	DB_RECORD_NUMBER GetMasterPage(DWORD dwPage);
	
	DWORD NumberOfInstructionPages(void);
	DB_RECORD_NUMBER GetInstructionPage(DWORD dwPage);
	
	BOOL
	IsThisPage(DB_RECORD_NUMBER Page);
	// Return the master items for this page.
	ObjectList* GetMasterItems(void);

	ObjectList* GetInstructionItems(void);
	// Get a string for the current page (used by page macros).
	virtual CString GetPageString(DB_RECORD_NUMBER lPage = 0);

	int GetSided(void) const;			// Are we single-sided, double-sided?
	DWORD MasterPageIndex(void);
	BOOL EditingMasterPage(void);
	void GotoMasterPage(DWORD dwPage, BOOL fUpdateViews = TRUE);

	DWORD InstructionPageIndex(void);
	BOOL EditingInstructionPage(void);
	void GotoInstructionPage(DWORD dwPage, BOOL fUpdateViews = TRUE);

	virtual BOOL CanPageBack(void);
	virtual BOOL CanPageNext(void);
	virtual void DoPageBack(void);
	virtual void DoPageNext(void);

	DocumentRecord* DocumentRecord(void)
		{ return document; }

	void InvalidateLastKnownPage(void)
		{ m_lLastKnownPageRecord = 0; }
protected:
	DB_RECORD_NUMBER m_lLastKnownPageRecord;
	DWORD m_lLastKnownPageIndex;
};

extern BOOL DrawObjects(ObjectList* page_objects, REFRESH_EXTENT_PTR pre, RedisplayContext *rc, CPmwDoc* pDoc = NULL);
extern BOOL DrawObjects(CPtrList* page_objects, REFRESH_EXTENT_PTR pre, RedisplayContext *rc, CPmwDoc* pDoc = NULL);
extern CFrameObject* CreatePageHeader(CPmwDoc* pDoc);
// Special draw code for PostScript (and other broken) printers.
// Return value: 1 = terminated, 0 = completed successfully, -1 = error
extern int DrawObjectsAsBitmaps(CPtrList* page_objects, REFRESH_EXTENT_PTR pre, RedisplayContext *rc, CPmwDoc* pDoc);

/////////////////////////////////////////////////////////////////////////////

#define SIGNATURE				0x4e4a
#define FILE_TYPE				0x100
#define TEMPLATE_TYPE		0x101

#define OLD_SIGNATURE		0x43a5
#define OLD_FILE_TYPE		0
#define OLD_TEMPLATE_TYPE	1

#endif
