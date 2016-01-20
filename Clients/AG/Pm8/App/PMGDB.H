/*
// $Header: /PM8/App/PMGDB.H 1     3/03/99 6:09p Gbeddow $
//
// Definitions for PMG database record objects.
//
// $Log: /PM8/App/PMGDB.H $
// 
// 1     3/03/99 6:09p Gbeddow
// 
// 9     12/08/98 5:18p Lwilson
// Initial Print Shop integration.
// 
// 8     6/30/98 3:02p Johno
// Added SetHyperlinkData()
// 
// 7     3/31/98 10:15a Johno
// Changes for reference counts
// 
// 6     3/27/98 5:45p Johno
// Reference count stuff for hyperlinks
// 
// 5     3/25/98 6:36p Johno
// HyperLink and Hyperlink now all Hyperlink for sanity
// 
// 4     3/11/98 5:52p Johno
// Use HyperlinkData struct
// 
// 3     3/10/98 4:16p Johno
// 
// 2     3/05/98 6:29p Johno
// New register_creation_method for RECORD_TYPE_Hyperlink
// 
//    Rev 1.0   14 Aug 1997 15:23:50   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:35:10   Fred
// Initial revision.
// 
//    Rev 1.14   26 Mar 1997 15:18:48   Fred
// Removed old geronimo stuff
// 
//    Rev 1.13   10 Oct 1996 14:30:36   Jay
// More time stamps
// 
//    Rev 1.12   29 Jul 1996 14:58:18   Jay
// Frame outline control
// 
//    Rev 1.11   28 Jul 1996 11:53:36   Jay
// Got rid of calendar and brochure info records.
// 
//    Rev 1.10   26 Jul 1996 11:38:58   Jay
// Page stuff
// 
//    Rev 1.9   16 Jul 1996 11:25:06   Jay
// Linked frame copy support
// 
//    Rev 1.8   08 Jul 1996 17:51:36   Jay
// Linked frames, etc.
// 
//    Rev 1.7   03 Jul 1996 14:30:30   Jay
//  
// 
//    Rev 1.6   28 Jun 1996 16:18:16   Jay
//  
// 
//    Rev 1.5   26 Jun 1996 14:21:42   Jay
//  
// 
//    Rev 1.4   24 Jun 1996 18:06:10   Jay
//  
// 
//    Rev 1.3   19 Jun 1996 17:32:12   Jay
//  
// 
//    Rev 1.2   25 Apr 1996 12:16:26   Jay
//  
// 
//    Rev 1.1   23 Apr 1996 08:07:30   Jay
// More new stuff
// 
//    Rev 1.0   14 Mar 1996 13:44:32   Jay
// Initial revision.
// 
//    Rev 1.0   16 Feb 1996 12:09:44   FRED
// Initial revision.
// 
//    Rev 2.11   27 Sep 1995 15:25:30   JAY
// Added CDocument member to database.
// 
//    Rev 2.10   07 Jul 1995 16:10:44   JAY
// Misc changes.
// 
//    Rev 2.9   13 May 1995 11:14:20   JAY
//  
// 
//    Rev 2.8   11 May 1995 09:37:34   JAY
// Changed warp field CreateStruct to an external class.
// 
//    Rev 2.7   09 May 1995 08:05:16   JAY
// Added EllipseObject and LineObject support.
// 
//    Rev 2.6   04 May 1995 17:16:04   JAY
// Fixed-point. Drawing objects.
// 
//    Rev 2.5   01 May 1995 15:24:52   JAY
// Restructured the macro stuff
// 
//    Rev 2.4   01 May 1995 08:43:04   JAY
// Access to macro list record.
// 
//    Rev 2.3   28 Apr 1995 17:38:20   FRED
//  
// 
//    Rev 2.2   27 Apr 1995 13:20:16   FRED
// 
//    Rev 2.1   19 Apr 1995 14:46:36   FRED
// Added preliminary support for warped text object
// 
//    Rev 2.0   07 Feb 1995 15:48:52   JAY
// Initial revision.
// 
//    Rev 1.13   06 Oct 1994 13:34:12   JAY
// convert_old_document() now also takes a margin rect pointer.
// 
//    Rev 1.12   09 Jun 1994 11:12:10   JAY
// Hooks for GraphicCache
// 
//    Rev 1.11   01 Jun 1994 16:41:36   JAY
// Moved some code down into Database proper to make it more useful.
// 
//    Rev 1.10   31 May 1994 12:39:40   JAY
// Added prep_graphic_file() hook to the external world.
// 
//    Rev 1.9   23 May 1994 08:36:56   JAY
// Support for embedded file records.
// new_graphic_record() now takes a GRAPHIC_CREATE_STRUCT pointer, not just
// the filename.
// 
//    Rev 1.8   11 May 1994 16:33:28   JAY
// made some LPSTRs into LPCSTRs
// 
//    Rev 1.7   31 Mar 1994 16:03:26   JAY
// Added argument to missing picture handler
// 
//    Rev 1.6   11 Mar 1994 14:25:24   JAY
// Added program instance handle variable.
// 
//    Rev 1.5   09 Mar 1994 09:22:08   MACDUFF
// Added ExtraPixels param to refresh extents
// 
//    Rev 1.4   04 Mar 1994 11:54:16   MACDUFF
// Defined new return codes for typeface callback.
// 
//    Rev 1.3   30 Jan 1994 09:49:30   JAY
//  
// 
//    Rev 1.2   25 Jan 1994 15:18:22   JAY
// More
// 
//    Rev 1.1   05 Jan 1994 08:08:08   JAY
// Restructuring, etc.
// 
//    Rev 1.0   16 Dec 1993 16:25:34   JAY
//  
*/

#ifndef __PMGDB_H__
#define __PMGDB_H__

#include "database.h"

// A forward reference for all

//typedef class PMGDatabase far *PMGDatabasePtr;

#include "pagedefs.h"
#include	"project.h"					/* project info defs. */
#include "refresh.h"
#include "style.h"
//#include "fontsrv.h"					/* Font server definitions. */
//#include "macrosrv.h"				/* Macro server definitions. */

#include "objdb.h"					/* Object database definitions. */
#include "pmgobj.h"					/* PrintMaster Gold object definitions. */

#include "grafrec.h"					/* for graphic create struct. */

#include "tfengine.h"

//#include "gcache.h"					/* Graphic cache interface definition. */

class ObjectList;

class MacroListRecord;
class CMacroServer;
class FontServer;
class GraphicCache;

class CFrameObject;
class CFrameRecord;
class CTextRecord;
class HyperlinkRecord;

struct WarpField_CreateStruct;
struct OutlinePathCreateStruct;

/*
// A PrintMaster Gold database.
*/

class PMGDatabase : public ObjectDatabase
{
	typedef VOID (*REFRESH_NOTIFY)(VOIDPTR, PBOX_PTR, REFRESH_TYPE, PMGPageObjectPtr, LPRECT);
	typedef VOID (*OBJECT_TOGGLE_HANDLER)(VOIDPTR, PMGPageObjectPtr, SHORT);
	typedef BOOL (*MISSING_GRAPHIC_HANDLER)(VOIDPTR, GraphicPtr, GRAPHIC_CREATE_STRUCT_PTR, int16 *);
	typedef LPSTR (*LOAD_STRING_HANDLER)(VOIDPTR, DWORD);
	typedef VOID (*DOCUMENT_MODIFIED_HANDLER)(VOIDPTR);
	typedef BOOL (*PREP_GRAPHIC_FILE)(StorageFilePtr, GraphicPtr, LPVOID);

protected:

	FontServer* font_server;
	CMacroServer* m_pMacroServer;

	REFRESH_NOTIFY refresh_notify;
	VOIDPTR refresh_data;

	MISSING_GRAPHIC_HANDLER	missing_graphic_handler;
	VOIDPTR missing_graphic_data;

	OBJECT_TOGGLE_HANDLER object_toggle_handler;
	VOIDPTR object_toggle_data;

	LOAD_STRING_HANDLER load_string_handler;
	VOIDPTR load_string_data;

	DOCUMENT_MODIFIED_HANDLER document_modified_handler;
	VOIDPTR document_modified_data;

	GraphicCache* m_graphic_cache;

	HINSTANCE m_hInstance;

	CDocument* m_pDocument;				// The document we're a part of.

	DB_RECORD_NUMBER m_lCurrentPage;

	BOOL m_fShowFrameOutlines;

public:

/*
// Create a PMG database.
*/

	PMGDatabase(StorageDevicePtr device, FontServer* fs = NULL);

/*
// Set the document we're bound to.
*/

	void SetDocument(CDocument* pDocument)
		{ m_pDocument = pDocument; }

/*
// Get the document we're bound to.
*/

	CDocument* GetDocument(void) const
		{ return m_pDocument; }

/*
// Set the Windows instance handle.
*/

	void set_instance(HINSTANCE hInstance)
		{ m_hInstance = hInstance; }

/*
// Get the Windows instance handle.
*/

	HINSTANCE get_instance(VOID)
		{ return m_hInstance; }

/*
// Set the refresh notify routine.
*/

	VOID set_refresh_notify(REFRESH_NOTIFY notify, VOIDPTR data = NULL)
		{ refresh_notify = notify; refresh_data = data; }

/*
// Set the missing graphic handler routine.
*/

	VOID set_missing_graphic_handler(MISSING_GRAPHIC_HANDLER handler, VOIDPTR data = NULL)
		{ missing_graphic_handler = handler; missing_graphic_data = data; }

/*
// Set the object toggle handler routine.
*/

	VOID set_object_toggle_handler(OBJECT_TOGGLE_HANDLER handler, VOIDPTR data = NULL)
		{ object_toggle_handler = handler; object_toggle_data = data; }

/*
// Set the load string handler routine.
*/

	VOID set_load_string_handler(LOAD_STRING_HANDLER handler, VOIDPTR data = NULL)
		{ load_string_handler = handler; load_string_data = data; }

/*
// Set the document modified handler routine.
*/

	VOID set_document_modified_handler(DOCUMENT_MODIFIED_HANDLER handler, VOIDPTR data = NULL)
		{ document_modified_handler = handler; document_modified_data = data; }

/*
// Set the graphic cache object.
*/

	VOID set_graphic_cache(GraphicCache* gcache)
		{ m_graphic_cache = gcache; }

/*
// Get the graphic cache object.
*/

	GraphicCache* get_graphic_cache(void) const
		{ return m_graphic_cache; }

/*
// Do a refresh notify. Nothing happens if refresh_notify has not been set.
*/

	VOID do_refresh_notify(
		PBOX_PTR area,
		REFRESH_TYPE type,
		PMGPageObjectPtr object,
		LPRECT lprExtraPixels = NULL);

/*
// Return whether we can notify or not.
*/

	BOOL can_refresh(VOID) { return refresh_notify != NULL; }

/*
// Refresh an object.
*/

	VOID refresh_object(PMGPageObjectPtr object);

/*
// Load a string from the resource file (assumed).
*/

	LPSTR load_string(DWORD which);

/*
// Say the document has been modified.
*/

	VOID document_modified();

/*
// Toggle an object.
*/

	VOID toggle_object(PMGPageObjectPtr object, SHORT which);

/*
// Delete an object.
// The object is assumed to not be in any list.
// This handles updating the object's area when it's gone.
*/

	VOID delete_object(PMGPageObjectPtr object);

/* 
// Record creation methods.
// These return the record, or NULL if error
// (use last_creation_error() to inquire about the last creation error).
//
// A record created here will remain locked until released with
// release_record().
*/

	ProjectPreviewPtr create_project_preview(VOIDPTR creation_data = NULL)
		{ return (ProjectPreviewPtr)new_record(RECORD_TYPE_ProjectPreview, creation_data); }

	DocumentPtr create_document(VOIDPTR creation_data = NULL)
		{ return (DocumentPtr)new_record(RECORD_TYPE_Document, creation_data); }

	FontDataPtr create_font_data(VOIDPTR creation_data = NULL)
		{ return (FontDataPtr)new_record(RECORD_TYPE_FontData, creation_data); }

	PagePtr create_page(VOIDPTR creation_data = NULL)
		{ return (PagePtr)new_record(RECORD_TYPE_Page, creation_data); }

	CFrameRecord* create_frame(VOIDPTR creation_data = NULL)
		{ return (CFrameRecord*)new_record(RECORD_TYPE_Frame, creation_data); }

	ParagraphPtr create_paragraph(VOIDPTR creation_data = NULL)
		{ return (ParagraphPtr)new_record(RECORD_TYPE_Paragraph, creation_data); }

	ObjectLayoutPtr create_object_layout(VOIDPTR creation_data = NULL)
		{ return (ObjectLayoutPtr)new_record(RECORD_TYPE_ObjectLayout, creation_data); }

	WarpFieldRecord* create_warp_field(VOIDPTR creation_data = NULL)
		{ return (WarpFieldRecord*)new_record(RECORD_TYPE_WarpField, creation_data); }

	GraphicPtr create_graphic(VOIDPTR creation_data = NULL)
		{ return (GraphicPtr)new_record(RECORD_TYPE_Graphic, creation_data); }

	ComponentPtr create_component(VOIDPTR creation_data = NULL)
		{ return (ComponentRecord*)new_record(RECORD_TYPE_Component, creation_data); }

	ArrayRecordPtr create_array_record(SHORT element_size = 0)
		{ return (ArrayRecordPtr)new_record(RECORD_TYPE_Array, &element_size); }

	EmbeddedFilePtr create_embedded_file_record(VOIDPTR creation_data = NULL)
		{ return (EmbeddedFilePtr)new_record(RECORD_TYPE_EmbeddedFile, creation_data); }

	OutlinePathRecord* create_outline_path(VOIDPTR creation_data = NULL)
		{ return (OutlinePathRecord*)new_record(RECORD_TYPE_OutlinePath, creation_data); }

	HyperlinkRecord* create_Hyperlink_record(VOIDPTR creation_data = NULL)
		{ return (HyperlinkRecord*)new_record(RECORD_TYPE_Hyperlink, creation_data); }
/* 
// Object creation methods.
// These return the object, or NULL if error
// (use last_creation_error() to inquire about the last creation error).
*/

	PatternObjectPtr create_pattern_object(VOIDPTR creation_data = NULL)
		{ return (PatternObjectPtr)new_object(OBJECT_TYPE_Pattern, creation_data); }

	GraphicObjectPtr create_graphic_object(VOIDPTR creation_data = NULL)
		{ return (GraphicObjectPtr)new_object(OBJECT_TYPE_Graphic, creation_data); }

	ComponentObject* create_component_object(VOIDPTR creation_data = NULL)
		{ return (ComponentObject*)new_object(OBJECT_TYPE_Component, creation_data); }

	GroupObjectPtr create_group_object(VOIDPTR creation_data = NULL)
		{ return (GroupObjectPtr)new_object(OBJECT_TYPE_Group, creation_data); }

	CFrameObject* create_frame_object(VOIDPTR creation_data = NULL)
		{ return (CFrameObject*)new_object(OBJECT_TYPE_Frame, creation_data); }

	BackgroundObjectPtr create_background_object(VOIDPTR creation_data = NULL)
		{ return (BackgroundObjectPtr)new_object(OBJECT_TYPE_Background, creation_data); }

	CalendarObjectPtr create_calendar_object(VOIDPTR creation_data = NULL)
		{ return (CalendarObjectPtr)new_object(OBJECT_TYPE_Calendar, creation_data); }

	CalendarDateObjectPtr create_calendar_date_object(VOIDPTR creation_data = NULL)
		{ return (CalendarDateObjectPtr)new_object(OBJECT_TYPE_CalendarDate, creation_data); }

	WarpTextObject* create_warp_text_object(VOIDPTR creation_data = NULL)
		{ return (WarpTextObject*)new_object(OBJECT_TYPE_WarpText, creation_data); }

	RectangleObject* create_rectangle_object(VOIDPTR creation_data = NULL)
		{ return (RectangleObject*)new_object(OBJECT_TYPE_Rectangle, creation_data); }

	EllipseObject* create_ellipse_object(VOIDPTR creation_data = NULL)
		{ return (EllipseObject*)new_object(OBJECT_TYPE_Ellipse, creation_data); }

	LineObject* create_line_object(VOIDPTR creation_data = NULL)
		{ return (LineObject*)new_object(OBJECT_TYPE_Line, creation_data); }

	DrawingObject* create_drawing_object(VOIDPTR creation_data = NULL)
		{ return (DrawingObject*)new_object(OBJECT_TYPE_DrawingObject, creation_data); }

/*
// Special record management.
// These types of objects are typically created once and used multiple times.
// Some use reference counts, some don't.
//
// You should use these instead of the generic ones above for typical use.
//
// These also include other special utility routines.
*/

/* Paragraph record. */

	VOID delete_paragraph(DB_RECORD_NUMBER);
	C_INDEX last_offset_in_paragraph(DB_RECORD_NUMBER p_record);

/* Frame record. */

	DB_RECORD_NUMBER paragraph_of_frame(DB_RECORD_NUMBER f_record);

/* Graphic record. */

	DB_RECORD_NUMBER new_graphic_record(GRAPHIC_CREATE_STRUCT_PTR gcs);
	ERRORCODE inc_graphic_record(DB_RECORD_NUMBER g_record);
	ERRORCODE free_graphic_record(DB_RECORD_NUMBER g_record);
	BOOL handle_missing_graphic(GraphicPtr graphic, GRAPHIC_CREATE_STRUCT_PTR gcs, int16 *index);
	void update_graphic_record(LPCSTR pszName);

/* Layout record. */

	DB_RECORD_NUMBER new_layout_record(LPSTR name, SHORT type, ListPtr elements);
	ERRORCODE inc_layout_record(DB_RECORD_NUMBER l_record);
	ERRORCODE free_layout_record(DB_RECORD_NUMBER l_record);

/* Warp Field record. */

	DB_RECORD_NUMBER new_warp_field_record(WarpField_CreateStruct* pCreateStruct);
	ERRORCODE inc_warp_field_record(DB_RECORD_NUMBER l_record);
	ERRORCODE free_warp_field_record(DB_RECORD_NUMBER l_record);

/* Outline Path record. */

	DB_RECORD_NUMBER new_outline_path_record(OutlinePathCreateStruct* pCreateStruct);
	ERRORCODE inc_outline_path_record(DB_RECORD_NUMBER l_record);
	ERRORCODE free_outline_path_record(DB_RECORD_NUMBER l_record);

/* Font record. */

	DB_RECORD_NUMBER new_font_record(LPSTR name, BOOL temp = FALSE, LPERRORCODE error = NULL);
	ERRORCODE inc_font_record(DB_RECORD_NUMBER f_record, BOOL temp = FALSE);
	ERRORCODE free_font_record(DB_RECORD_NUMBER f_record, BOOL temp = FALSE);
	ERRORCODE font_data_name(DB_RECORD_NUMBER f_record, LPSTR name);

// Hyperlink Record
   
	DB_RECORD_NUMBER 
   NewHyperlinkRecord(HyperlinkData* p, BOOL temp = FALSE);
	
   ERRORCODE 
   IncrementHyperlinkRecord(DB_RECORD_NUMBER f_record, BOOL temp = FALSE);

   ERRORCODE 
   FreeHyperlinkRecord(DB_RECORD_NUMBER f_record, BOOL temp = FALSE);

   BOOL
   GetHyperlinkData(DB_RECORD_NUMBER RecordNumber, HyperlinkData &Data);

	BOOL
	SetHyperlinkData(DB_RECORD_NUMBER RecordNumber, HyperlinkData &Data);

/* Text style record. */

	DB_RECORD_NUMBER NewTextStyleRecord(LPCSTR name, int nType, CTextStyle* pStyle = NULL);
	ERRORCODE IncTextStyleRecord(DB_RECORD_NUMBER lRecord);
	ERRORCODE FreeTextStyleRecord(DB_RECORD_NUMBER lRecord);
	ERRORCODE TextStyleName(DB_RECORD_NUMBER lRecord, LPSTR name);
	DB_RECORD_NUMBER ImportTextStyleRecord(DB_RECORD_NUMBER, PMGDatabase* pSourceDatabase);

/* Text record. */

	CTextRecord* NewStory(void);
	void DeleteStory(DB_RECORD_NUMBER lStory);
	void AddStory(DB_RECORD_NUMBER lStory, BOOL fCheckExistence = TRUE);
	void RemoveStory(DB_RECORD_NUMBER lStory);

/* Component record. */

	ERRORCODE inc_component_record(DB_RECORD_NUMBER recno);
	ERRORCODE free_component_record(DB_RECORD_NUMBER recno);

/*
// Return the font server for this database.
*/

	FontServer* get_font_server(VOID)
		{ return font_server; }

/*
// Return the macro server for this database.
*/

	CMacroServer* GetMacroServer(void) const
		{ return m_pMacroServer; }

/*
// Set the macro server for this database.
*/

	void SetMacroServer(CMacroServer* pMacroServer)
		{ m_pMacroServer = pMacroServer; }

/*
// Face management. This used to call the font server. No more.
*/

	DB_RECORD_NUMBER allocate_face(LPSTR name, BOOL temp = FALSE)
		{ return new_font_record(name, temp); }

	ERRORCODE free_face(DB_RECORD_NUMBER f_record, BOOL temp = FALSE)
		{ return free_font_record(f_record, temp); }

	ERRORCODE inc_face(DB_RECORD_NUMBER f_record, BOOL temp = FALSE)
		{ return inc_font_record(f_record, temp); }

	ERRORCODE face_name(DB_RECORD_NUMBER f_record, LPSTR name)
		{ return font_data_name(f_record, name); }

/*
// Old document management.
*/

	ERRORCODE convert_old_document(WORD signature, StorageDevicePtr new_device, ProjectInfo *info, LPCRECT margins);

/*
// Macro list support.
*/

	MacroListRecord* GetMacroList(void);

	// What is the currently-loaded page?
	// The database is used as a convenient place to store this value.
	// The document record actually knows.

	DB_RECORD_NUMBER CurrentPage(void) const
		{ return m_lCurrentPage; }
	void CurrentPage(DB_RECORD_NUMBER lPage)
		{ m_lCurrentPage = lPage; }

	static CFrameObject* FindFrameObject(DB_RECORD_NUMBER lFrame, ObjectList* pList);
	CFrameObject* FindFrameObject(DB_RECORD_NUMBER lFrame);

	// Re-link frames after a paste or copy.

	static void MapCopiedFrames(PMGPageObject* pOldObject, PMGPageObject* pNewObject, CMapPtrToPtr& Map);
	void LinkCopiedFrames(PMGDatabase* pOldDatabase, CMapPtrToPtr& Map, BOOL fFlowText);

	void CopyObjectList(ObjectList* pDestList, ObjectList* pSourceList, DB_RECORD_NUMBER lPage = 0);

	// Do we draw frame outlines?
	BOOL ShowFrameOutlines(void) const
		{ return m_fShowFrameOutlines; }
	void ShowFrameOutlines(BOOL fShowFrameOutlines)
		{ m_fShowFrameOutlines = fShowFrameOutlines; }
};

/*
// Hard-coded database record numbers for the two base records.
// These two types of records MUST have these record numbers.
// (This is easily maintained by creating them first in the order shown.)
*/

#define PMG_PREVIEW_RECORD			1
#define PMG_DOCUMENT_RECORD		2

/*
// The callback function passed to init_document_engine() will not be
// invoked during that call.
*/

extern BOOL init_document_engine(GetFontCallback pCallback, LPCSTR pszDllPath);
extern VOID exit_document_engine(void);

#endif						/* #ifndef __PMGDB_H__ */
