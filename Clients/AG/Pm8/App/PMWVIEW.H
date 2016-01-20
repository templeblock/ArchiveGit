// $Workfile: PMWVIEW.H $
// $Revision: 1 $
// $Date: 3/03/99 6:09p $
//
//  "This unpublished source code contains trade secrets which are the
//   property of Mindscape, Inc.  Unauthorized use, copying or distribution
//   is a violation of international laws and is strictly prohibited."
// 
//   Copyright © 1997 Mindscape, Inc. All rights reserved.
//
// Revision History:
//
// $Log: /PM8/App/PMWVIEW.H $
// 
// 1     3/03/99 6:09p Gbeddow
// 
// 94    2/26/99 2:20p Lwilson
// Added support for image component replacement.
// 
// 93    2/25/99 3:28p Lwilson
// Changed component creation to use a pointer to a CCreationData class or
// variant for in place of an RComponentDocument pointer.
// 
// 92    2/25/99 10:35a Cschende
// added apply drop cap function
// 
// 91    2/19/99 11:32a Johno
// Moved OnWebSiteProperties here from webview.cpp; Enable / disable menu
// item according to project's HTML support
// 
// 90    2/15/99 1:02p Psasse
// ChangeRotation moved from the view to the doc class
// 
// 89    2/11/99 11:55a Cschende
// Added code for Drop Caps implementation
// 
// 88    2/09/99 4:44p Rgrenfel
// Added support for the Made To Order toolbar popup menu for seals and
// timepieces.
// 
// 87    2/08/99 3:17p Rgrenfel
// Added the new commands for logos, initial capitals, smart numbers and
// signatures.
// 
// 86    2/01/99 5:37p Rgrenfel
// Added Seal dialog command to the view.
// 
// 85    2/01/99 2:46p Blee
// Removed OnUpdateAddPage() (now in Doc)
// 
// 84    2/01/99 11:26a Rgrenfel
// Added command for custom Timepieces.
// 
// 83    1/25/99 4:35p Johno
// Moved OnPageProperties() here
// 
// 82    1/22/99 4:05p Johno
// Added CmdUI handlers for web stuff
// 
// 81    1/20/99 5:00p Johno
// 
// 80    1/20/99 4:59p Blee
// Added OnUpdateTextBanner to gray out the "Edit Banner Text" command
// when the project type is not a banner.
// 
// 79    1/19/99 3:18p Lwilson
// Fixed comments
// 
// 78    1/19/99 1:14p Jfleischhauer
// added command processing for the Photo Workshop popup menu items
// 
// 77    1/14/99 3:44p Jfleischhauer
// additional command handlers for Photo Workshop button and popup window
// 
// 76    1/13/99 1:23p Jfleischhauer
// replaced PhotoEnhancer with Photo Workshop
// 
// 75    1/07/99 5:16p Rgrenfel
// Added drag activation definition and methods to track where a drag is
// dropped.
// 
// 74    1/07/99 3:58p Cschende
// added command handlers for InsertDate
// 
// 73    1/05/99 5:46p Cschende
// added Insert Symbol command handler
// 
// 72    1/04/99 5:12p Johno
// Moved CWebView / CWebDoc code into CPmwDoc / CPmwView.
// 
// 71    12/23/98 12:03p Cschende
// uncommented bullet menu command functions
// 
// 70    12/23/98 11:43a Johno
// Compile update
// 
// 69    12/22/98 10:29a Johno
// Moved most HTML conversion stuff here from webview.*
// 
// 68    12/17/98 3:23p Cschende
// removed temporary bullet selection menu commands
// 
// 67    12/17/98 2:33p Lwilson
// Added double click edit support for component objects
// 
// 66    12/15/98 3:37p Lwilson
// Initial support for PS component editing.
// 
// 65    12/11/98 1:53p Lwilson
// Added headline dialog support.
// 
// 64    12/10/98 4:11p Cschende
// Added bullet, numbered list and multilevel list support in text
// 
// 63    12/08/98 5:18p Lwilson
// Initial Print Shop integration.
// 
// 62    12/01/98 12:27p Johno
// Fix for close minimized print preview crash
// From 7.01
// 
// 61    11/30/98 5:35p Johno
// Foo
// 
// 60    10/28/98 11:15a Mwilson
// fixed paste bug
// 
// 59    9/30/98 4:43p Mwilson
// more changes for instrucition page support
// 
// 58    9/29/98 1:07p Mwilson
// added support for view instruction pages in HTML help window
// 
// 57    9/25/98 11:07a Mwilson
// added support for instruction page
// 
// 56    7/21/98 8:39a Jayn
// ChooseFont now supports tables
// 
// 55    7/19/98 6:27p Jayn
// Merge cells. Applying font attributes to cells.
// 
// 54    7/12/98 5:33p Jayn
// Changed how Serif DrawPlus tool bar button worked (add vs. edit)
// 
// 53    7/10/98 6:08p Rlovejoy
// Added command hooks for 'MergeNames'.
// 
// 52    6/18/98 4:06p Mwilson
// added static variable for use when printing a half fold as a quarter
// fold card.  Used for ag remote print
// 
// 51    6/05/98 7:46p Hforman
// add guides from menu functions
// 
// 50    6/05/98 2:12p Jayn
// Overlapping object flashing rectangle(s) in web projects.
// 
// 49    6/05/98 8:53a Fredf
// Changes to automatically bring up the receiver list for the with help
// labels.
// 
// 48    6/04/98 7:52p Jayn
// Table fill down and right plus a lot of "fit to text" stuff
// 
// 47    6/03/98 9:52a Fredf
// More changes for address book and merge names.
// 
// 46    6/01/98 1:25p Mwilson
// added some functions to help with personal delivery
// 
// 45    5/30/98 6:43p Jayn
// Table cell contents; cell formatting undo
// 
// 44    5/30/98 5:03p Psasse
// With Help Labels support - new message to post for address book
// 
// 43    5/29/98 6:39p Jayn
// Insert column, etc.
// 
// 42    5/26/98 2:39p Jayn
// New table code.
// 
// 41    5/11/98 9:04p Psasse
// added online menu to postertype
// 
// 40    5/05/98 4:44p Rlovejoy
// Gradient button switch.
// 
// 39    4/30/98 7:56p Jayn
// Changes for Border Plus and Add Picture From Disk.
// 
// 38    4/24/98 6:33p Fredf
// Fixed bug with simultaneous print previews. Moved preview refresh
// extent pointer into CPmwView... it was a global variable.
// 
// 37    3/20/98 6:03p Jayn
// Serif Drawing Object
// 
// 36    3/19/98 2:32p Jayn
// Finish up master pages.
// 
// 35    3/18/98 11:15a Jayn
// Page numbers
// 
// 34    3/17/98 2:43p Fredf
// New workspace toolbars.
// 
// 33    3/17/98 11:56a Jayn
// Master-pages, double-sided flag.
// 
// 32    3/11/98 5:13p Johno
// Disadle hyperlink menu item (for non webviews)
// 
// 31    3/10/98 5:24p Jayn
// Master page changes
// 
// 30    3/10/98 10:57a Rlovejoy
// Messages for YYWB dialog.
// 
// 29    3/06/98 5:15p Johno
// Added virtual SetObjectHyperLink methods.
// 
// 28    2/27/98 4:51p Johno
// Moved all web view stuff to webview.cpp / h
// 
// 27    2/25/98 2:27p Hforman
// add EditBorder functions
// 
// 26    2/25/98 10:40a Jstoner
// table functionality updates...merge/split cells
// row/col selection done
// 
// 25    2/24/98 10:49a Dennis
// Renamed Page Properties control
// 
// 24    2/17/98 4:58p Johno
// Added AllowDoubleSided parameter
// 
// 23    2/11/98 12:21p Dennis
// Added Web Background Properties menu handler
// 
// 22    2/11/98 11:36a Jstoner
// changes for table object
// 
// 21    2/11/98 9:29a Dennis
// Added Web Site Properties
// 
// 20    2/03/98 10:34a Jstoner
// saving incremental progress 
// 
// 19    1/28/98 10:39a Jstoner
// incremental update to table project
// 
// 18    1/21/98 9:14a Jstoner
// added cell selection by dragging, merge menu handlers
// 
// 17    12/23/97 7:39p Hforman
// add border commands
// 
// 16    12/16/97 12:13p Jstoner
// added auto format menu item and handler
// 
// 15    12/16/97 9:18a Jstoner
// added table support
// 
// 14    12/11/97 4:54p Dennis
// Added Publish to Web... methods
// 
// 13    11/21/97 4:42p Hforman
// added TextBox Outline show/hide functions; modify DrawGuideline()
// params
// 
// 12    11/19/97 6:42p Hforman
// add a guide variable

#include "pagedefs.h"
#include "event.h"
#include "marquee.h"
#include	"ctxp.h"
#include	"pmwprint.h"
#include "filemgr.h"
#include "urlproc.h"    // For processing URL's when previewing web pages

#define TXP_CURSOR_TIMER	157			// A nice random number.

#define  FLAGTEST(f, t) ((f & t) == 0 ? FALSE : TRUE)
#define  FLAGSET(f, t)  (f |= t)
#define  FLAGCLR(f, t)  (f = (f & ~t))

class CCmdChangeSelects;
class CDocEditDesktop;
class CPmwPrint;
struct PrintContext;
class CFrameObject;
class CPopupPalette;
class CShapeEntry;
class CRuler;
class GraphicRecord;
class CCardDoc;
class CCreationData;
class RComponentDocument;

typedef enum
{
	VIEW_NONE = -1,
	VIEW_LAST = 0,
	VIEW_CUSTOM,
	VIEW_AREA,
	VIEW_25,
	VIEW_50,
	VIEW_100,
	VIEW_200,
	VIEW_400,
	VIEW_FULL_PAGE,
	VIEW_FULL_WIDTH,
	VIEW_FULL_HEIGHT,
	VIEW_ZOOM_IN,
	VIEW_ZOOM_OUT
} VIEW_TYPE;

VOID near blit_scale_pbox(PBOX_PTR pbox, SHORT x_num, SHORT x_denom, SHORT y_num, SHORT y_denom);

/*
// Flags for position_view() and reframe_view().
*/
#define PVIEW_FLAG_scroll			1
#define PVIEW_FLAG_center			2
#define PVIEW_FLAG_set_scrolls	4

// number of pixels on either side of Guides to use in PtOnGuide()
#define GUIDE_AREA_MOVE          3
#define GUIDE_AREA_SNAP          10

class RComponentView;
class RComponentDocument;

class CDragDropState
{
public:
	CDragDropState();

	PPNT m_DragSize;
	PPNT m_DragOffset;
	PPNT m_DragPoint;

	BOOL m_fShowFeedback;
	DROPEFFECT m_prevDropEffect;
	CRect m_crDrawnRect;
};

class CViewEditState;

class CPmwView : public CView
{
	INHERIT(CPmwView, CView)

	friend CPmwDoc;

public:
	typedef VOID (CPmwView::* EDIT_EVENT_HANDLER)(EVENT_INFO *);
	typedef BOOL (CPmwView::* STATE_HANDLER)(EVENT_INFO *);
   // guideline types
   // NOTE: these are also used as return flags from SnapPosition()
   enum { GUIDE_HORIZ=1, GUIDE_VERT=2 };
	// guide colors (normal and master page)
	static COLORREF m_clrGuides;
	static COLORREF m_clrMasterGuides;
	// Web publishing
	static int		s_nPublishWarn;
	static int		s_nWebCheck;
   static BOOL		m_fGotDestinationStrings;
   static CString	m_csDestURL, m_csSiteName;
protected:
	static CTempFileManager m_cfmPublishTempFiles;
	
	BOOL
	QueryPublish(void);
   
	BOOL
   GetDestinationStrings(void);
   
   void
   SaveDestinationStrings(LPCSTR Dest, LPCSTR Name);

   void
   DoURLErrorDialog(URLProcessorRetCode rc);

   BOOL
   SelectFullHyperlink(void);

   void
   DoHyperlinkText(CPmwDoc* pDoc);

   void
   DoHyperlinkObject(CPmwDoc* pDoc, PMGPageObject* pObject);

   BOOL
   IsJustOneObjectSelected(CPmwDoc **pDoc);

   BOOL
   IsAnyTextObjectSelected(DWORD &Flags, BOOL &AllAlike);

   void
   IsAnyTextObjectSelected(DWORD &Flags, BOOL &AllAlike, ObjectList* pList, BOOL &FoundText);

   void
   TestObject(PMGPageObject *pObject, DWORD &Flags, BOOL &AllAlike, BOOL &FoundText);
////
protected:
	RedisplayContext	rc;					/* Screen context */
	PPNT					spacing;				/* origin offset */
	PBOX					view_extent;		/* Limits of view (doc + spacing). */
	PBOX					view_portion;		/* Doc "bound" */
	CTxp					m_Txp;
	BOOL					m_fDefaultStretching;	// Default stretching.
	HBITMAP				m_edit_background;
	RECT					m_rEditBackground;

	CDocEditDesktop*	m_pDesktop;

	PrintContext		m_pc;

	COleDropTarget*	m_pDropTarget;
	CDragDropState		m_DragDropState;

	CWnd*					m_pLinkFrameButton;
	CWnd*					m_pNextFrameButton;
	CWnd*					m_pPreviousFrameButton;

	BOOL					m_fStuckTool;
	CShapeEntry*		m_pShapeEntry;		// The shape selected for the tool.

	CRuler*				m_pXRuler;
	CRuler*				m_pYRuler;
	CRuler*				m_pXYRuler;

   static CTempFileManager m_cfmHelpTempFiles;
	
	DWORD					m_dwPushedPageIndex;
	BOOL m_fBringUpReceiverList;

public:
	REFRESH_EXTENT* m_pPreviewExtent;

protected:
/* View management. */

	VIEW_TYPE current_view;
	VOID position_view(PCOORD new_x0, PCOORD new_y0, PCOORD new_x1, PCOORD new_y1, FLAGS flags);
	VOID view_scale(PCOORD numerator, PCOORD denominator, PPNT *dimensions, PBOX *pbox);

	void GetUsableClientRect(LPRECT pRect);
	// calendar mgmt
	void ReplaceCalendarInfo(CALENDAR_INFO info, CalendarObjectPtr pObject, CPmwDoc * pDoc);
	// Ruler management.
	BOOL CreateRulers(void);
	void UpdateRulers(void);
	BOOL ShowRulers(void);
	void SetShowRulers(BOOL fShowRulers);
	BOOL SnapToRulers(void);
	void SetSnapToRulers(BOOL fSnapToRulers);
   // Guideline management
	BOOL        ShowGuides();
	void        SetShowGuides(BOOL fShowGuides);
	BOOL        SnapToGuides();
	void			AddGuide(int nType);
	static BOOL guide_toggle(RedisplayContextPtr rc, BOOL turn_on, LPRECT clip, VOIDPTR data);

   int            m_GuideType;
   int            m_GuideIndex;
   CPoint         m_LastGuidePt;

/* Scroll bar management. */

	VOID update_scrolls(RECT *client, PBOX *pbox, BOOL horz, BOOL vert);
	BOOL horizontal_scroll_shown;
	BOOL vertical_scroll_shown;

	VOID OnScroll(int nBar, UINT nSBCode, UINT nPos);

/* Print code. */

	void add_cut_mark(PCOORD x0, PCOORD y0, PCOORD x1, PCOORD y1, SHORT pattern);
	void add_cut_marks(BOOL left, BOOL top, BOOL right, BOOL bottom);
	void remove_cut_marks(void);

/* Edit event handling mechanism. */

	EDIT_EVENT_HANDLER edit_event_handler;
	SHORT edit_state;

	BOOL check_escape_event(EVENT_INFO *info);

	void invoke_event_handler(VOID);
	void signal_mouse_event(FLAGS nFlags, CPoint point, EVENT event);
	void signal_internal_event(EVENT event);
	void signal_mode_event(EVENT event, BOOL fInternal = FALSE, BOOL fInit = TRUE);

	// Set the edit mode.
	VOID set_mode(EDIT_EVENT_HANDLER handler, BOOL fInternal = FALSE, BOOL fInit = TRUE);

/* Resize mode variables and routines. */

	PMGPageObjectPtr current_object;
	PMGPageObjectPtr m_pLastSelectedObject;		/* for shift-click */
	CCmdChangeSelects* m_pCommand;
	OBJECT_HANDLE current_handle;
	PBOX original_bound;
	PBOX original_refresh_bound;
	ANGLE original_angle;
	FLAGS original_select_flags;
	PPNT grab_offset;
	Marquee *marquee;
	PPNT select_start;

	BOOL cursor_visible;
	BOOL cursor_on;

	BOOL toggle_object(SHORT which = 0, PMGPageObjectPtr object = NULL);

	VOID move_selected_objects(DRAG_STATE state, PPNT vector, PPNT current_xy, bool fAllowDrag = TRUE);

	VOID process_right_button(POINT where);

/* Mode number bases. Leave room for range! */

	enum
	{
		STATE_INIT = 0,			/* All modes support this. */
		STATE_NORMAL = 1,			/* All modes support this. */
		STATE_RELEASE = 2,		/* All modes support this. */
		STATE_BASE_END = 3,		/* Last common mode. */

		RESIZE_STATE_BASE = 100,
		RESIZE_STATE_HANDLE = RESIZE_STATE_BASE,
		RESIZE_STATE_MOVING,
		RESIZE_STATE_SELECT,
		RESIZE_STATE_MOVING_GUIDE,
		RESIZE_STATE_SELECT_CELLS,
		RESIZE_STATE_RESIZE_COL,
		RESIZE_STATE_RESIZE_ROW,
		RESIZE_STATE_SELECT_COL,
		RESIZE_STATE_SELECT_ROW,

		CREATE_STATE_BASE = 110,
		CREATE_STATE_DRAGGING = CREATE_STATE_BASE,

		TEXT_EDIT_STATE_BASE = 120,
		TEXT_EDIT_STATE_DRAGGING = TEXT_EDIT_STATE_BASE,

		PLACE_PICTURE_STATE_BASE = 130,
		PLACE_PICTURE_STATE_DRAGGING = PLACE_PICTURE_STATE_BASE,

		ZOOM_STATE_BASE = 140,
		ZOOM_STATE_DRAGGING = ZOOM_STATE_BASE,

		LINK_STATE_BASE = 150,

		DRAW_STATE_BASE = 160,
		DRAW_STATE_DRAWING = DRAW_STATE_BASE,

		CROP_STATE_BASE = 170,
		CROP_STATE_HANDLE = CROP_STATE_BASE,
		CROP_STATE_PANNING,

	};

	BOOL standard_state_release(EVENT_INFO *info);

/* Resize mode. */

	BOOL resize_state_init(EVENT_INFO *info);
	BOOL resize_state_normal(EVENT_INFO *info);
	BOOL resize_state_handle(EVENT_INFO *info);
	BOOL resize_state_moving(EVENT_INFO *info);
	BOOL resize_state_select(EVENT_INFO *info);
	BOOL resize_state_moving_guide(EVENT_INFO *info);
	BOOL resize_state_select_cell(EVENT_INFO *info);
	BOOL resize_state_resize_table_col(EVENT_INFO *info);
	BOOL resize_state_resize_table_row(EVENT_INFO *info);
	BOOL resize_state_select_table_col(EVENT_INFO *info);
	BOOL resize_state_select_table_row(EVENT_INFO *info);
	VOID resize_key(SHORT dx, SHORT dy, EVENT_INFO *info, PPNT p);

	static STATE_HANDLER resize_state[];
	VOID mode_resize(EVENT_INFO *info);

	// A hook function to handle things after an object has changed position.
	virtual void AfterPositionChange(void);

	// Creation mode.

	BOOL create_state_init(EVENT_INFO *info);
	BOOL create_state_normal(EVENT_INFO *info);
	BOOL create_state_release(EVENT_INFO *info);
	BOOL create_state_dragging(EVENT_INFO *info);

	static STATE_HANDLER create_state[];
	void mode_create(EVENT_INFO *info);

	UINT m_uUndoCreate;
	DB_OBJECT_TYPE m_nTypeToCreate;
	PMGPageObject* m_pCreatedObject;

	void OnTool(DB_OBJECT_TYPE nType);
	void OnUpdateTool(CCmdUI* pCmdUI, DB_OBJECT_TYPE nType);
	void AbortCreate(BOOL fMoving = TRUE);

	ERRORCODE create_initial_frame(VOID);

	// Drawing mode.

	BOOL draw_state_init(EVENT_INFO *info);
	BOOL draw_state_normal(EVENT_INFO *info);
	BOOL draw_state_drawing(EVENT_INFO *info);

	static STATE_HANDLER draw_state[];
	void mode_draw(EVENT_INFO *info);

	void AbortDraw(void);

	// Text box editing mode.

	VOID toggle_square(PBOX pbox, LPRECT pClip = NULL, CFrameObject* pFrameObject = NULL);
	void ToggleSelect(CTxp* pTxp, LPRECT pClip = NULL);
	static void TxpToggleFunction(CTxp* pTxp, int nWhich, LPVOID pData);
	static BOOL redisp_toggle_function(RedisplayContextPtr rc, BOOL turn_on, LPRECT clip, VOIDPTR data);
	VOID text_edit_keystroke(EVENT_INFO *info);
	BOOL text_edit_state_init(EVENT_INFO *info);
	BOOL text_edit_state_normal(EVENT_INFO *info);
	BOOL text_edit_state_dragging(EVENT_INFO *info);

	void update_horz_align(CCmdUI* pCmdUI, ALIGN_TYPE type);
	void update_vert_align(CCmdUI* pCmdUI, VERT_ALIGN_TYPE type);
	void realign_frame(ALIGN_TYPE align_type);
	void reposition_frame(VERT_ALIGN_TYPE align_type);

	BOOL UpdateUndoTyping(CTxp* pTxp, CHARACTER c, int nCount);
	BOOL UpdateUndoDelete(CTxp* pTxp, int nCount);
	BOOL UpdateUndoBackspace(CTxp* pTxp, int& nCount);

	PBOX text_cursor_pbox;
	static STATE_HANDLER text_edit_state[];
	VOID mode_text_edit(EVENT_INFO *info);

	void ToggleTxpCursor(void);
	void TxpCursorOn(void);

	void UpdateEditBackground(void);
	VOID create_edit_background(void);
public:
	void free_edit_background(void);
protected:

	void RefreshInstructionItems(void);
	void ViewInstructionsInHelpWnd(BOOL bShowWindow);
	void ChangeFrame(int nDirection);

#if 0
	// Place picture mode.

	BOOL place_picture_state_init(EVENT_INFO *info);
	BOOL place_picture_state_normal(EVENT_INFO *info);
	BOOL place_picture_state_release(EVENT_INFO *info);
	BOOL place_picture_state_dragging(EVENT_INFO *info);

	static STATE_HANDLER place_picture_state[];
	VOID mode_place_picture(EVENT_INFO *info);
#endif

	// Zoom mode.

	BOOL zoom_state_init(EVENT_INFO *info);
	BOOL zoom_state_normal(EVENT_INFO *info);
	BOOL zoom_state_release(EVENT_INFO *info);
	BOOL zoom_state_dragging(EVENT_INFO *info);
	void zoom_cleanup(void);

	static STATE_HANDLER zoom_state[];
	VOID mode_zoom(EVENT_INFO *info);

	void SetZoomCursor(EVENT_INFO *info);

	BOOL m_fZooming;

	double ZoomFactor(void);
	BOOL CanZoomIn(void);
	BOOL CanZoomOut(void);

	// Link mode.

	BOOL link_state_init(EVENT_INFO *info);
	BOOL link_state_normal(EVENT_INFO *info);
	BOOL link_state_release(EVENT_INFO *info);

	void AbortLink(void);

	static STATE_HANDLER link_state[];
	void mode_link(EVENT_INFO *info);

	DB_RECORD_NUMBER m_lLinkSource;
	DB_RECORD_NUMBER m_lLinkSourceText;
	DB_RECORD_NUMBER m_lOldLink;

	void IllegalCursor(void);
	enum
	{
		LINK_STATUS_NotHere,
		LINK_STATUS_Link,
		LINK_STATUS_Unlink
	};
	int GetLinkStatus(PMGPageObject* pObject, BOOL fComplain = FALSE);

	// Crop mode.

	BOOL crop_state_init(EVENT_INFO *info);
	BOOL crop_state_normal(EVENT_INFO *info);
	BOOL crop_state_handle(EVENT_INFO *info);
	BOOL crop_state_panning(EVENT_INFO *info);

	static STATE_HANDLER crop_state[];
	void mode_crop(EVENT_INFO *info);
	BOOL CanCrop(void);

	// Refresh management code.

	BOOL m_fRefreshSemaphore;
	CRefreshArray m_RefreshArray;

	void add_refresh(PBOX_PTR extent,
		REFRESH_TYPE type = REFRESH_ALL,
		PMGPageObjectPtr object = NULL,
		LPRECT lprExtraPixels = NULL);

	static BOOL marquee_toggle(RedisplayContextPtr rc, BOOL turn_on, LPRECT clip, VOIDPTR data);

/**/

	void adjust_objects(SHORT id);
/*
// Desktop support.
*/
public:
	CDocEditDesktop* GetEditDesktop(void)
		{ return m_pDesktop; }
						
protected:
	virtual void CreateEditDesktop(void);
/*
// Drop target support.
*/
public:
	COleDropTarget* GetDropTarget(void)
		{ return m_pDropTarget; }

	BOOL CreateChosenPicture(PPNT* pOrigin = NULL, BOOL bAddUndo = TRUE);
	BOOL CreateNewComponent(CCreationData* pCreationData, PPNT* pOrigin = NULL, BOOL bAddUndo = TRUE );

protected:
	virtual void CreateDropTarget(void);
	
	// Overridden functions. Our COleDropTarget automatically calls these.
	virtual DROPEFFECT OnDragEnter( COleDataObject* pDataObject, DWORD dwKeyState, CPoint point );
	virtual DROPEFFECT OnDragOver(COleDataObject* pDataObject, DWORD dwKeyState, CPoint point );
	virtual void OnDragLeave(void);
	virtual BOOL OnDrop( COleDataObject* pDataObject, DROPEFFECT dropEffect, CPoint point );
	virtual BOOL OnScrollBy( CSize sizeScroll, BOOL bDoScroll = TRUE );

	void InitiateDrag();

	DROPEFFECT ComputeDropEffect(COleDataObject* pDataObject, DWORD dwKeyState);
	static BOOL ToggleDragFeedback(RedisplayContext* pContext, BOOL fTurnOn, LPRECT pcrArea, LPVOID pData);
	void UpdateDragFeedback(DROPEFFECT de = DROPEFFECT_NONE,
									const PPNT* point = NULL);

protected: // create from serialization only
	CPmwView();
	DECLARE_DYNCREATE(CPmwView)

// Attributes
public:
	CPmwDoc* GetDocument() const;
	RedisplayContext* get_rc(void)
		{ return &rc; }

	void InsertCharacter(CHARACTER c);			// Misc function (macro.cpp)
	BOOL EditingText(void)
		{ return m_Txp.Valid(); }

	void TxpOn(void);
	CTxp* GetTxp(void)
		{ return &m_Txp; }

	BOOL IsActiveView(void);
	
	virtual BOOL CanInsertPictures(void);

	CShapeEntry* GetShapeEntry(void) const
		{ return m_pShapeEntry; }
	void SetShapeEntry(CShapeEntry* pShapeEntry)
		{ m_pShapeEntry = pShapeEntry; }

// Operations
public:

	virtual void SetPanel(int nPanel, BOOL fUpdateMenu = TRUE);

	SHORT choose_picture_color(GraphicObjectPtr object, WORD wSoundID = 0);
	SHORT choose_object_position(int nStartPage = 0);

	VOID change_view(VIEW_TYPE new_view, BOOL force = FALSE, PBOX_PTR bound = NULL);
	VOID reframe_view(PBOX cursor);
	VOID reframe_on_txp(VOID);
	BOOL SetTxpFrame(DB_RECORD_NUMBER lFrame, CHARACTER_INDEX lCharacter = -1);
	VOID set_arrow_tool(BOOL fInternal = FALSE, BOOL fInit = TRUE) { set_mode(mode_resize, fInternal, fInit); }
	void ZoomCommand(VIEW_TYPE new_view);
//	VOID set_text_create_tool() { set_mode(mode_text_create); }
	VOID set_text_edit_tool() { set_mode(mode_text_edit); }
//	VOID set_place_picture_tool() { set_mode(mode_place_picture); }
	VOID set_zoom_tool() { set_mode(mode_zoom); }

	VOID edit_frame(CFrameObject* pObject, PPNT_PTR p = NULL);
	void stop_editing_frame(void);

	void EditComponentObject( ComponentObject* pObject, PPNT_PTR p = NULL );
	
	// Edit state and view manipulation.
	virtual CViewEditState* CreateEditState(void);
	virtual void SaveEditState(CViewEditState* pState);
	virtual void RestoreEditState(CViewEditState* pState);

	virtual void ViewObject(PMGPageObject* pObject, PBOX* pWhere = NULL);
	virtual void ViewText(DB_RECORD_NUMBER lFrame, CHARACTER_INDEX lStartCharacter, CHARACTER_INDEX lEndCharacter, BOOL Zoom = TRUE);

	void BeginFrameEdit(CFrameObject* pFrame, CHARACTER_INDEX lCharacter);

	VOID select_object(PMGPageObjectPtr object);
	VOID deselect_object(PMGPageObjectPtr object);
	void deselect_all(BOOL fUpdateMenu = TRUE);

	VOID set_cursor(HCURSOR);

	BOOL refresh(VOID);

	BOOL toggle_selects(SHORT which, PMGPageObjectPtr object, HDC hdc = 0);

	static BOOL refresh_toggle(RedisplayContextPtr rc, BOOL turn_on, LPRECT clip, VOIDPTR data);

	VOID removing_object(PMGPageObjectPtr object = NULL);

	// Get the current cursor position in page coordinates.
	void GetCursorPagePos(PPNT* p);

	// Snap functions.
	int SnapPosition(PPNT* p);

   // Guide functions
   void CreateGuide(int type);
   void DeleteGuide(int type, int index);
   void DrawGuideLine(RedisplayContext* rc, int type, POINT ptPos, BOOL fLastPt = FALSE, COLORREF Color = (COLORREF)-1);
   BOOL PtOnGuide(POINT screenpt, int extrapix, int type, int* index);
	int PtOnHGuide(POINT screenpt, Array* pGuides, int extrapix);
	int PtOnVGuide(POINT screenpt, Array* pGuides, int extrapix);

	// Stretch helper.
	void GetStretchState(BOOL& fEnable, int& nCheck, int& nType);

	/*
	// Print project main entry point.
	*/

	int PrintProject(CPrintInfo* pInfo, WORD wSoundID = 0, BOOL AllowDoubleSided = TRUE);
 
public:
	// Gadget interface.
	virtual BOOL GadgetVisible(UINT uID);
	virtual BOOL GadgetEnabled(UINT uID, UINT uPart);
	virtual void GadgetClicked(UINT uID, UINT uPart, CWnd* pGadget);
	virtual void GadgetText(UINT uID, CString& csText);
	virtual LPCSTR GadgetMenuName(UINT uID, UINT uPart);

	virtual void BeforePageChange(BOOL fToMasterPage = FALSE);

// Implementation
public:
	virtual ~CPmwView();
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view 
	virtual void OnDraw(PrintContext* pPC);  // overridden to print this view 
	virtual void SetInitialTextStyle (void);
   virtual void OnInitialUpdate();
	virtual void OnActivateView(BOOL bActivate, CView *pActivateView, CView *pDeactivateView);

#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	void UpdateFrameButtons(void);

	virtual void OnUpdate(CView *sender, LPARAM lHint, CObject *pHint);

	// Printing support
protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnPrepareDC(CDC* pDC, CPrintInfo* pInfo = NULL);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);
   virtual BOOL DoPreparePrinting (CPrintInfo* pInfo );
   virtual BOOL IsGoodPrinter ();

	virtual CPmwPrint* CreatePrintDialog(void);

	virtual VOID generate_print_vars(PrintContext *pc, RECT r);
	void generate_preview_vars(CDC *pDC, CPrintInfo* pInfo);

	virtual UINT GetPrintPreviewToolbar(void);
	virtual int PrintPreviewPageCount(void);
	virtual void OnEndPrintPreview(CDC* pDC, CPrintInfo* pInfo, POINT point, CPreviewView* pView);


   void
   DoFileSaveAsGraphic (void);

public:
	virtual int SetPrintPreviewPage(int nPage);
protected:
	virtual void OnBeginPrinting(CDC *pDC, CPrintInfo* pInfo);
	void OnPrint(CDC* pDC, CPrintInfo* pInfo);
	// My version of OnPrint.
	virtual int OnPrint(PrintContext *pc, CPrintInfo* pInfo);
	// My version of OnBeginPrinting.
	void OnBeginPrinting(PrintContext *pc, CPrintInfo* pInfo);

	virtual SHORT OnPrintDocument(PrintContext &pc,
							int32 aCurrentDocument, int32 aNumDocuments);

	// Multi-page entry-point (newsletter, brochure, etc).
	virtual int OnPrintMultiPageDocument(PrintContext &pc,
							int32 aCurrentDocument, int32 aNumDocuments);

	// Print the current page.
	virtual int PrintCurrentPage(PrintContext& pc, int nPage, BOOL fMakeBlank = FALSE);

	// Double-sided printing. Key page printing
	virtual int PrintKeySheetFront(PrintContext& pc);
	virtual int PrintKeySheetBack(PrintContext& pc);
	int PrintBlankPage(PrintContext& pc);

	// Double-sided printing. Document printing
	virtual int PrintDocumentFront(PrintContext& pc);
	virtual int PrintDocumentBack(PrintContext& pc);
	virtual int PrintSingleSided(int nFromPage, int nToPage, PrintContext& pc);

	virtual int PrintDocumentForward(PrintContext& pc, int nSide);
	virtual int PrintDocumentBackward(PrintContext& pc, int nSide);

	/*
	// My virtual print functions.
	*/

	virtual void SetupToPrint(PrintContext& pc);
	virtual int StartPrintDoc(PrintContext& pc);
	virtual int GrindOutPrintJob(PrintContext& pc);
	virtual void EndPrintDoc(PrintContext& pc, int nError);
	virtual void FinishPrinting(PrintContext& pc, int nError);

	// OLE Client support
	virtual BOOL IsSelected(const CObject* pDocItem) const;

	// text formatting functions
	void ChangeFont(CTextStyle* pStyle, WORD wCmdID);
	void ChangeTextSize(int nNewSize, WORD wCmdID);
	void ApplyDropCap( int nLine );

	CTextStyle& GetCurrentTextStyle(void);

	// Special printing functions.
public:
	//flag for ag remote printing.  They print HF as QF so we convert the .hcr to a .car and then 
	//print the .car
	static BOOL m_bPrintingHFasQF;

	void PrintNoDialog(void);
	void PrintHFtoQF(void);
	void InsertAgBarCode(CCardDoc* pDoc);
	BOOL m_fNoPrintDialog;

	BOOL CreateKeyFrame(WORD wDoubleSidedType);
	BOOL CreateKeyArrow(WORD wDoubleSidedType);

	void OnModifyPalette(UINT uID, UINT uPaletteID, CPopupPalette& Window, BOOL fHasGradient = FALSE);

	void DoIndicatorMenu(int nWhichMenu);
	void DoSetRotation(int nDegrees);
	void DoUpdateRotate(CCmdUI* pCmdUI, int nDegrees = -1);
	BOOL CanRotate(void);

	// Sentiment support.
	virtual void PasteSentiment(const CMlsStringArray& csaSentiment);
	void PasteSentiment(const CString& csSentiment, int nWhere, BOOL fOnCard, BOOL fSelect);
   // Web Publishing
   virtual BOOL
   SetObjectHyperLink(PMGPageObject* po, LPCSTR str)
   {
      ASSERT(FALSE); // Only do this with a CWebView
      return FALSE;
   }
   virtual BOOL
   SetObjectHyperLink(CPmwDoc* pDoc, PMGPageObject* po, LPCSTR str)
   {
      ASSERT(FALSE); // Only do this with a CWebView
      return FALSE;
   }
	// Is a table selected?
	BOOL TableIsSelected(void);
	BOOL TableCellsSelected(void);

// Generated message map functions
protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	//{{AFX_MSG(CPmwView)
	afx_msg void OnInsertObject();	// OLE support
	afx_msg void OnCancelEdit();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnViewFullPage();
	afx_msg void OnViewFullWidth();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnPictureImport();
	afx_msg void OnAddSentiment();
	afx_msg void OnEditDelete();
	afx_msg void OnEditGroup();
	afx_msg void OnEditUngroup();
	afx_msg void OnUpdateEditUngroup(CCmdUI* pCmdUI);
	afx_msg void OnUpdateEditGroup(CCmdUI* pCmdUI);
	afx_msg void OnEditBorder();
	afx_msg void OnUpdateEditBorder(CCmdUI* pCmdUI);
	afx_msg void OnXflip();
	afx_msg void OnYflip();
	afx_msg void OnTextChoosefont();
	afx_msg void OnUpdateTextChoosefont(CCmdUI* pCmdUI);
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnEditingText(CCmdUI* pCmdUI);
	afx_msg void OnEditDuplicate();
	afx_msg void OnFilePrintPreview();
	afx_msg void OnCalendarTitle();
	afx_msg void OnCalendarWhen();
	afx_msg void OnCalendarPicture();
	afx_msg void OnUpdateCalendarPicture(CCmdUI* pCmdUI);
	afx_msg void OnUpdateCalendarText(CCmdUI* pCmdUI);
	afx_msg void OnCalendarText();
	afx_msg void OnCalendarClear();
	afx_msg void OnUpdateCalendarClear(CCmdUI* pCmdUI);
	afx_msg void OnViewFullHeight();
	afx_msg void OnCalendarColor();
	afx_msg void OnUpdateCalendarColor(CCmdUI* pCmdUI);
	afx_msg void OnCalendarStyle();
	afx_msg void OnCalendarNames();
	afx_msg void OnCalendarNumber();
	afx_msg void OnHaveCalendar(CCmdUI* pCmdUI);
	afx_msg void OnTextBanner();
	afx_msg void OnUpdateTextBanner(CCmdUI* pCmdUI);
	afx_msg void OnUpdateViewFullHeight(CCmdUI* pCmdUI);
	afx_msg void OnUpdateViewFullPage(CCmdUI* pCmdUI);
	afx_msg void OnUpdateViewFullWidth(CCmdUI* pCmdUI);
	afx_msg void OnSomethingSelected(CCmdUI* pCmdUI);
	afx_msg void OnPicturesReplace();
	afx_msg void OnUpdateEditFrame(CCmdUI* pCmdUI);
	afx_msg void OnEditFrame();
	afx_msg void OnPicturesColor();
	afx_msg void OnOnePictureSelection(CCmdUI* pCmdUI);
	afx_msg void OnReplaceObject();
	afx_msg void OnUpdateReplaceObject(CCmdUI* pCmdUI);
	afx_msg void OnFilePrint();
	afx_msg void OnEditCopy();
	afx_msg void OnEditCut();
	afx_msg void OnEditPaste();
	afx_msg void OnUpdateEditCopy(CCmdUI* pCmdUI);
	afx_msg void OnUpdateEditCut(CCmdUI* pCmdUI);
	afx_msg void OnUpdateEditPaste(CCmdUI* pCmdUI);
	afx_msg void OnView50View();
	afx_msg void OnUpdateView50View(CCmdUI* pCmdUI);
	afx_msg void OnView100View();
	afx_msg void OnUpdateView100View(CCmdUI* pCmdUI);
	afx_msg void OnView200View();
	afx_msg void OnUpdateView200View(CCmdUI* pCmdUI);
	afx_msg void OnViewAreaView();
	afx_msg void OnView25View();
	afx_msg void OnUpdateView25View(CCmdUI* pCmdUI);
	afx_msg void OnView400View();
	afx_msg void OnUpdateView400View(CCmdUI* pCmdUI);
	afx_msg void OnPictureAttributes();
	afx_msg void OnSendToBack();
	afx_msg void OnBackOne();
	afx_msg void OnForwardOne();
	afx_msg void OnSendToFront();
	afx_msg void OnUpdateLayerToBack(CCmdUI* pCmdUI);
	afx_msg void OnUpdateLayerBackOne(CCmdUI* pCmdUI);
	afx_msg void OnUpdateLayerForwardOne(CCmdUI* pCmdUI);
	afx_msg void OnUpdateLayerToFront(CCmdUI* pCmdUI);
	afx_msg void OnStandardPosition();
	afx_msg void OnStandardSize();
	afx_msg void OnRotate0();
	afx_msg void OnUpdateRotate0(CCmdUI* pCmdUI);
	afx_msg void OnRotate45();
	afx_msg void OnUpdateRotate45(CCmdUI* pCmdUI);
	afx_msg void OnRotate90();
	afx_msg void OnUpdateRotate90(CCmdUI* pCmdUI);
	afx_msg void OnRotate135();
	afx_msg void OnUpdateRotate135(CCmdUI* pCmdUI);
	afx_msg void OnRotate180();
	afx_msg void OnUpdateRotate180(CCmdUI* pCmdUI);
	afx_msg void OnRotate225();
	afx_msg void OnUpdateRotate225(CCmdUI* pCmdUI);
	afx_msg void OnRotate270();
	afx_msg void OnUpdateRotate270(CCmdUI* pCmdUI);
	afx_msg void OnRotate315();
	afx_msg void OnUpdateRotate315(CCmdUI* pCmdUI);
	afx_msg void OnRotateMore();
	afx_msg void OnUpdateRotateMore(CCmdUI* pCmdUI);
	afx_msg void OnVcenter();
	afx_msg void OnOneSelection(CCmdUI* pCmdUI);
	afx_msg void OnHcenter();
	afx_msg void OnCenter();
	afx_msg void OnFullPage();
	afx_msg void OnCalendarAttributes();
	afx_msg void OnAlignBottom();
	afx_msg void OnUpdateAlignBottom(CCmdUI* pCmdUI);
	afx_msg void OnAlignCenter();
	afx_msg void OnUpdateAlignCenter(CCmdUI* pCmdUI);
	afx_msg void OnAlignLeft();
	afx_msg void OnUpdateAlignLeft(CCmdUI* pCmdUI);
	afx_msg void OnAlignMiddle();
	afx_msg void OnUpdateAlignMiddle(CCmdUI* pCmdUI);
	afx_msg void OnAlignRight();
	afx_msg void OnUpdateAlignRight(CCmdUI* pCmdUI);
	afx_msg void OnAlignJustify();
	afx_msg void OnUpdateAlignJustify(CCmdUI* pCmdUI);
	afx_msg void OnAlignTop();
	afx_msg void OnUpdateAlignTop(CCmdUI* pCmdUI);
	afx_msg void OnEditSelectAll();
	afx_msg void OnUpdateEditSelectAll(CCmdUI* pCmdUI);
	afx_msg void OnInitMenu(CMenu* pMenu);
	afx_msg void OnInitMenuPopup(CMenu* pPopupMenu, UINT nIndex, BOOL bSysMenu);
	afx_msg void OnUpdatePositionStandard(CCmdUI* pCmdUI);
	afx_msg void OnUpdateEditDelete(CCmdUI* pCmdUI);
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg void OnEditUndo();
	afx_msg void OnUpdateEditUndo(CCmdUI* pCmdUI);
	afx_msg void OnEditRedo();
	afx_msg void OnUpdateEditRedo(CCmdUI* pCmdUI);
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnTextBold();
	afx_msg void OnUpdateTextBold(CCmdUI* pCmdUI);
	afx_msg void OnTextItalic();
	afx_msg void OnUpdateTextItalic(CCmdUI* pCmdUI);
	afx_msg void OnTextUnderline();
	afx_msg void OnUpdateTextUnderline(CCmdUI* pCmdUI);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnViewAddressBook();
	afx_msg void OnViewSmartFields();
	afx_msg void OnUpdateViewSmartFields(CCmdUI* pCmdUI);
	afx_msg void OnDrawingObject();
	afx_msg void OnUpdateDrawingObject(CCmdUI* pCmdUI);
	afx_msg void OnUpdatePictureAttributes(CCmdUI* pCmdUI);
	afx_msg void OnInsertField();
	afx_msg void OnInsertMailMergeField();
	afx_msg void OnSelectMergeNames();
	afx_msg void OnUpdateSelectMergeNames(CCmdUI* pCmdUI);
	afx_msg void OnUpdateViewZoomIn(CCmdUI* pCmdUI);
	afx_msg void OnUpdateViewZoomOut(CCmdUI* pCmdUI);
	afx_msg void OnViewZoomIn();
	afx_msg void OnViewZoomOut();
	afx_msg void OnFormatParagraph();
	afx_msg void OnFormatTabs();
	afx_msg void OnFormatFrame();
	afx_msg void OnUpdateFormatFrame(CCmdUI* pCmdUI);
	afx_msg void OnTextWrap();
	afx_msg void OnUpdateTextWrap(CCmdUI* pCmdUI);
	afx_msg void OnEditDeletePage();
	afx_msg void OnUpdateEditDeletePage(CCmdUI* pCmdUI);
	afx_msg void OnSpelling();
	afx_msg void OnUpdateSpelling(CCmdUI* pCmdUI);
	afx_msg void OnAddPage();
	afx_msg void OnAddCalendarPage();
	afx_msg void OnUpdateAddCalendarPage(CCmdUI* pCmdUI);
	afx_msg void OnGotoPage();
	afx_msg void OnUpdateGotoPage(CCmdUI* pCmdUI);
	afx_msg void OnGotoFirstPage();
	afx_msg void OnGotoLastPage();
	afx_msg void OnGotoNextPage();
	afx_msg void OnUpdateGotoNextPage(CCmdUI* pCmdUI);
	afx_msg void OnGotoPreviousPage();
	afx_msg void OnUpdateGotoPreviousPage(CCmdUI* pCmdUI);
	afx_msg void OnUpdateGotoFirstPage(CCmdUI* pCmdUI);
	afx_msg void OnUpdateGotoLastPage(CCmdUI* pCmdUI);
	afx_msg void OnTextChoosecolor();
	afx_msg void OnAcquire();
	afx_msg void OnUpdateAcquire(CCmdUI* pCmdUI);
	afx_msg void OnSelectSource();
	afx_msg void OnUpdateSelectSource(CCmdUI* pCmdUI);
	afx_msg void OnImportText();
	afx_msg void OnUpdateImportText(CCmdUI* pCmdUI);
	afx_msg void OnFormatSpacing();
   afx_msg void OnUpdateTextEditReplace(CCmdUI* pCmdUI);
   afx_msg void OnTextEditReplace();
	afx_msg void OnRulers();
	afx_msg void OnUpdateRulers(CCmdUI* pCmdUI);
	afx_msg void OnSnapToRulers();
	afx_msg void OnUpdateSnapToRulers(CCmdUI* pCmdUI);
	afx_msg void OnGuides();
	afx_msg void OnUpdateGuides(CCmdUI* pCmdUI);
	afx_msg void OnSnapToGuides();
	afx_msg void OnUpdateSnapToGuides(CCmdUI* pCmdUI);
	afx_msg void OnLockGuides();
	afx_msg void OnUpdateLockGuides(CCmdUI* pCmdUI);
	afx_msg void OnAddHorizontalGuide();
	afx_msg void OnAddVerticalGuide();
	afx_msg void OnTextBoxOutlines();
	afx_msg void OnUpdateTextBoxOutlines(CCmdUI* pCmdUI);
	afx_msg void OnNfxToon();
	afx_msg void OnUpdateNfxToon(CCmdUI* pCmdUI);
	afx_msg void OnPhotoWorkshopCropOrientation();
	afx_msg void OnPhotoWorkshopAdjustColor();
	afx_msg void OnPhotoWorkshopBrightnessFocus();
	afx_msg void OnPhotoWorkshopFixFlaw();
	afx_msg void OnPhotoWorkshopArtisticEffects();
	afx_msg void OnUpdatePhotoWorkshop(CCmdUI* pCmdUI);
	afx_msg void OnPhotoWorkshopButton();
	afx_msg void OnPhotoWorkshopMenuPalette();
	afx_msg void OnMadeToOrderButton();
   afx_msg void OnMadeToOrderMenuPalette();
	afx_msg void OnFileSaveAsGraphic();
	afx_msg void OnZoomToSelected();
	afx_msg void OnAddBorder();
	afx_msg void OnUpdateAddBorder(CCmdUI* pCmdUI);
	afx_msg void OnBorderPlus();
	afx_msg void OnUpdateBorderPlus(CCmdUI* pCmdUI);
	afx_msg void OnYywb();
	afx_msg void OnUpdateYywb(CCmdUI* pCmdUI);
   afx_msg void OnUpdateWebHyperlink(CCmdUI* pCmdUI);
	afx_msg void OnWebSiteProperties();
	afx_msg void OnUpdateWebSiteProperties(CCmdUI* pCmdUI); 
	afx_msg void OnAddPageNumber();
	afx_msg void OnUpdateAddPageNumber(CCmdUI* pCmdUI);
	afx_msg void OnDrawPlus();
	afx_msg void OnAddDrawingObject();
	afx_msg void OnUpdateAddDrawingObject(CCmdUI* pCmdUI);
	afx_msg void OnEditDrawingObject();
	afx_msg void OnUpdateEditDrawingObject(CCmdUI* pCmdUI);
	afx_msg void OnAddHeadline();
	afx_msg void OnAddPictureFromDisk();
	afx_msg void OnOnlineCreateAccount();
	afx_msg void OnOnlineChangeAccounts();
	afx_msg void OnOnlineUpdateAccount();
	afx_msg void OnOnlineDeleteAllDownloadedFiles();
	afx_msg void OnTableSelected(CCmdUI* pCmdUI);
	afx_msg void OnTableInsert();
	afx_msg void OnUpdateTableInsert(CCmdUI* pCmdUI);
	afx_msg void OnTableDelete();
	afx_msg void OnUpdateTableDelete(CCmdUI* pCmdUI);
	afx_msg void OnTableFillDown();
	afx_msg void OnUpdateTableFillDown(CCmdUI* pCmdUI);
	afx_msg void OnTableFillRight();
	afx_msg void OnUpdateTableFillRight(CCmdUI* pCmdUI);
	afx_msg void OnTableMergeCells();
	afx_msg void OnUpdateTableMergeCells(CCmdUI* pCmdUI);
	afx_msg void OnTableSplitCells();
	afx_msg void OnUpdateTableSplitCells(CCmdUI* pCmdUI);
	afx_msg void OnTableFitToText();
	afx_msg void OnUpdateTableFitToText(CCmdUI* pCmdUI);
	afx_msg void OnGotoInstructionPage();
	afx_msg void OnUpdateGotoInstructionPage(CCmdUI* pCmdUI);
	afx_msg void OnFormatBullets();
	afx_msg void OnUpdateFormatBullets(CCmdUI* pCmdUI);
	afx_msg void OnPublishToFolder();
	afx_msg void OnPublishToWeb();
	afx_msg void OnWebHyperlink();
	afx_msg void OnHotSpot();
	afx_msg void OnPreviewWebSite();
	afx_msg void OnWebChecker();
	afx_msg void OnWebAuto();
	afx_msg void OnUpdateWebAuto(CCmdUI* pCmdUI);
	afx_msg void OnWebKeepText();
	afx_msg void OnWebAsGraphic();
	afx_msg void OnFormatInsertSymbol();
	afx_msg void OnUpdateFormatInsertSymbol(CCmdUI* pCmdUI);
	afx_msg void OnFormatInsertdate();
	afx_msg void OnUpdateFormatInsertdate(CCmdUI* pCmdUI);
	afx_msg void OnPageProperties();
	afx_msg void OnUpdateIfHTML(CCmdUI* pCmdUI); 
	afx_msg void OnUpdateIfHTMLText(CCmdUI* pCmdUI); 
	afx_msg void OnFormatDropcapsFourlines();
	afx_msg void OnUpdateFormatDropcapsFourlines(CCmdUI* pCmdUI);
	afx_msg void OnFormatDropcapsNone();
	afx_msg void OnUpdateFormatDropcapsNone(CCmdUI* pCmdUI);
	afx_msg void OnFormatDropcapsThreelines();
	afx_msg void OnUpdateFormatDropcapsThreelines(CCmdUI* pCmdUI);
	afx_msg void OnFormatDropcapsTwolines();
	afx_msg void OnUpdateFormatDropcapsTwolines(CCmdUI* pCmdUI);
	//}}AFX_MSG
// JRO
	afx_msg void OnDropFiles(HDROP hDropInfo);
	afx_msg void OnEditPasteSpecial();
	afx_msg void OnUpdateEditPasteSpecial(CCmdUI* pCmdUI);
	afx_msg void OnTextEditDone();
	afx_msg void OnTextStretching();
	afx_msg void OnUpdateTextStretching(CCmdUI* pCmdUI);
	afx_msg void OnTextTypeface();
	afx_msg void OnUpdateTextTypeface(CCmdUI* pCmdUI);
	afx_msg void OnTextSizeKillFocus();
	afx_msg void OnUpdateTextSize(CCmdUI* pCmdUI);
	afx_msg void OnStartupSmartFields();
	afx_msg void OnTextLink();
	afx_msg void OnTextPreviousFrame();
	afx_msg void OnTextNextFrame();
	afx_msg void OnClickedRuler();
	afx_msg void OnPositionIndicator();
	afx_msg void OnUpdatePositionIndicator(CCmdUI* pCmdUI);
	afx_msg void OnSizeIndicator();
	afx_msg void OnUpdateSizeIndicator(CCmdUI* pCmdUI);
	afx_msg void OnRotateIndicator();
	afx_msg void OnUpdateRotateIndicator(CCmdUI* pCmdUI);
	afx_msg void OnTextTool();
	afx_msg void OnUpdateTextTool(CCmdUI* pCmdUI);
	afx_msg void OnFreeDrawTool();
	afx_msg void OnUpdateFreeDrawTool(CCmdUI* pCmdUI);
	afx_msg void OnLineTool();
	afx_msg void OnUpdateLineTool(CCmdUI* pCmdUI);
	afx_msg void OnRectangleTool();
	afx_msg void OnUpdateRectangleTool(CCmdUI* pCmdUI);
	afx_msg void OnEllipseTool();
	afx_msg void OnUpdateEllipseTool(CCmdUI* pCmdUI);
	afx_msg void OnFillColor();
	afx_msg void OnFillColorPalette();
	afx_msg void OnUpdateFillColor(CCmdUI* pCmdUI);
	afx_msg void OnLineWidth();
	afx_msg void OnLineWidthPalette();
	afx_msg void OnUpdateLineWidth(CCmdUI* pCmdUI);
	afx_msg void OnLineColor();
	afx_msg void OnLineColorPalette();
	afx_msg void OnUpdateLineColor(CCmdUI* pCmdUI);
	afx_msg void OnShadow();
	afx_msg void OnShadowPalette();
	afx_msg void OnUpdateShadow(CCmdUI* pCmdUI);
	afx_msg void OnShadowColor();
	afx_msg void OnShadowColorPalette();
	afx_msg void OnUpdateShadowColor(CCmdUI* pCmdUI);
	afx_msg void OnTextShape();
	afx_msg void OnTextShapePalette();
	afx_msg void OnUpdateTextShape(CCmdUI* pCmdUI);
	afx_msg void OnNextObject();
	afx_msg void OnUpdateNextObject(CCmdUI* pCmdUI);
	afx_msg void OnPreviousObject();
	afx_msg void OnUpdatePreviousObject(CCmdUI* pCmdUI);
	afx_msg void OnCrop();
	afx_msg void OnUpdateCrop(CCmdUI* pCmdUI);
	afx_msg void OnResetCropping();
	afx_msg void OnUpdateResetCropping(CCmdUI* pCmdUI);
	afx_msg void OnAddTable();
	afx_msg void OnUpdateAddTable(CCmdUI* pCmdUI);
	afx_msg void OnAutoFormat();
	afx_msg void OnUpdateAutoFormat(CCmdUI* pCmdUI);
	afx_msg void OnCellFormatting();
	afx_msg void OnUpdateCellFormatting(CCmdUI* pCmdUI);
	afx_msg void OnAddTimePiece();
	afx_msg void OnAddSeal();
	afx_msg void OnAddLogo();
	afx_msg void OnAddInitCap();
	afx_msg void OnAddNumber();
	afx_msg void OnAddSignature();
	
	afx_msg void OnWithHelpViewAddressBook();

	DECLARE_MESSAGE_MAP()

	void
	PublishToFolder(void);

	void
	PublishToWeb();

   void
   PreviewWebSite(BOOL DoCheck, BOOL JustCheck);

   void
   SetClearObjectFlags(DWORD Set, DWORD Clear, BOOL &Changed, PMGPageObject *pObject);

   void
   SetClearSelectedTextObjectsFlags(DWORD Set, DWORD Clear);

	void
	SetClearSelectedTextObjectsFlags(DWORD Set, DWORD Clear, BOOL &Changed, ObjectList* pList);

   void
   DoMenuEnableAndCheck(CCmdUI* pCmdUI, DWORD Flags);
#ifdef   MODIFY_LAYER_MENU
   BOOL
   AnyNonHotspotsSelected(CPmwDoc *pDoc);
#endif

	// An array of PBOXs for the currently shown overlapping extents.
	CVarArray m_OverlappingExtents;
public:

   // Dummy function; I don't think we want to do this
	// unless we are a CWebDoc / CWebView
	virtual void
   SetWarningRect(PBOX pb, DB_RECORD_NUMBER PageNumber);

	// Component edit functions
	CCreationData *	HeadlineDialog( RComponentView* pComponent, const RRealPoint& mousePoint, uLONG ulEditInfo ) const;
	CCreationData *	ImageGallery( RComponentView* pComponent, const RRealPoint& mousePoint, uLONG ulEditInfo ) const;
	CCreationData *	TimePieceDialog( RComponentView* pComponent, const RRealPoint& mousePoint, uLONG ulEditInfo ) const;
	CCreationData *	SealDialog( RComponentView* pComponent, const RRealPoint& mousePoint, uLONG ulEditInfo ) const;
	CCreationData *	LogoDialog( RComponentView* pComponent, const RRealPoint& mousePoint, uLONG ulEditInfo ) const;
	CCreationData *	InitCapDialog( RComponentView* pComponent, const RRealPoint& mousePoint, uLONG ulEditInfo ) const;
	CCreationData *	NumberDialog( RComponentView* pComponent, const RRealPoint& mousePoint, uLONG ulEditInfo ) const;
	CCreationData *	SignatureDialog( RComponentView* pComponent, const RRealPoint& mousePoint, uLONG ulEditInfo ) const;

private:
	// Stores if the current drag opperation was started by us.
	bool m_fDragActive;
	// Stores if we have dropped onto ourselves.
	bool m_fLocalDragDrop;
	
	// Photo Workshop functionality
	void DoUpdatePhotoWorkshop(CCmdUI* pCmdUI);
	void DoPhotoWorkshop( int nTabIndex = 0 );
	RComponentDocument*	PhotoWorkshopDialog( RComponentDocument* pComponent, int nTabIndex );
};

class CViewEditState
{
public:
	CViewEditState();
	virtual ~CViewEditState();

	// Where we are.
	DB_RECORD_NUMBER m_lPage;
	PBOX m_SourceBox;
	VIEW_TYPE m_nView;
	int m_nPanel;

	// What we are doing.
	CPmwView::EDIT_EVENT_HANDLER m_EditEventHandler;
	int m_nState;

	// Text editing info.
	CFrameObject* m_pTextObject;
	CHARACTER_INDEX m_lTextIndex;
	CHARACTER_INDEX m_lTextSelectStart;
	CHARACTER_INDEX m_lTextSelectEnd;
};

#ifndef _DEBUG	// debug version in pmwview.cpp
inline CPmwDoc* CPmwView::GetDocument() const
   { return (CPmwDoc*) m_pDocument; }
#endif

typedef CCreationData* (CPmwView::* YComponentEditFunction)( RComponentView*, const RRealPoint&, uLONG ) const;

struct SPComponentDataMapEntry
{
public:

									SPComponentDataMapEntry(  YResourceId menuStringId,
																	  YResourceId insertUndoRedoPairId,
																	  YResourceId editUndoRedoPairId,
																	  YComponentEditFunction pFunction );

	YResourceId					m_MenuStringId;
	YResourceId					m_InsertUndoRedoPairId;
	YResourceId					m_EditUndoRedoPairId;
	YComponentEditFunction	m_pFunction;
};

class CPComponentDataMap : public CMapStringToPtr
{
public:

	CPComponentDataMap();
};

/////////////////////////////////////////////////////////////////////////////
