/*
// $Header: /PM8/App/DOCREC.H 1     3/03/99 6:05p Gbeddow $
//
// Document record definitions.
//
// $Log: /PM8/App/DOCREC.H $
// 
// 1     3/03/99 6:05p Gbeddow
// 
// 13    1/26/99 6:34p Psasse
// Added PhotoProjects Project Type (still in progress)
// 
// 12    9/29/98 3:21p Mwilson
// fixed instruction page support
// 
// 11    9/25/98 11:07a Mwilson
// added support for instruction page
// 
// 10    7/01/98 4:16p Johno
// Added DocumentRecord::NumberOfHyperlinks()
// 
// 9     4/16/98 4:24p Johno
// Split AppendFullURL() into AppendFullURL() and AppendFullURLString()
// 
// 8     4/03/98 4:23p Johno
// Added AppendFullURL()
// 
// 7     3/25/98 6:30p Johno
// HyperLink and Hyperlink now all Hyperlink for sanity
// 
// 6     3/10/98 5:24p Jayn
// Master page changes
// 
// 5     3/09/98 9:27a Johno
// Compile update
// 
// 4     3/06/98 5:45p Johno
// Hyperlink stuff
// 
// 3     11/12/97 6:19p Hforman
// took out guide arrays -- now in page's object list
// 
// 2     11/05/97 10:41a Hforman
// add arrays for vertical/horizontal guides
// 
//    Rev 1.0   14 Aug 1997 15:20:22   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:33:38   Fred
// Initial revision.
// 
//    Rev 1.8   07 Apr 1997 17:02:36   Jay
// set_project_type()
// 
//    Rev 1.7   26 Jul 1996 11:38:50   Jay
// Page stuff
// 
//    Rev 1.6   25 Jul 1996 19:38:06   Jay
//  
// 
//    Rev 1.5   25 Jul 1996 11:16:24   Jay
// Page stuff
// 
//    Rev 1.4   17 Jul 1996 14:10:06   Jay
// Database bookkeeping
// 
//    Rev 1.3   28 Jun 1996 16:18:12   Jay
//  
// 
//    Rev 1.2   19 Jun 1996 17:32:08   Jay
//  
// 
//    Rev 1.1   23 Apr 1996 08:07:52   Jay
// More new stuff
// 
//    Rev 1.0   14 Mar 1996 13:48:08   Jay
// Initial revision.
// 
//    Rev 1.0   16 Feb 1996 12:07:16   FRED
// Initial revision.
// 
//    Rev 2.8   14 Sep 1995 09:32:16   JAY
// 32-bit changes
// 
//    Rev 2.7   20 Jul 1995 12:48:12   JAY
// Now owns, load and saves a CPaperInfo.
// 
//    Rev 2.6   12 Jul 1995 17:43:22   JAY
// New project sub-types (esp. for cards).
// 
//    Rev 2.5   05 Jul 1995 11:17:26   JAY
// Rearranged includes to reduce symbol count.
// 
//    Rev 2.4   13 May 1995 11:14:20   JAY
//  
// 
//    Rev 2.3   27 Apr 1995 13:20:18   FRED
// 
//    Rev 2.2   19 Apr 1995 12:31:18   JAY
//  
// 
//    Rev 2.1   29 Mar 1995 08:59:08   JAY
// Lots of changes
// 
//    Rev 2.0   07 Feb 1995 15:48:30   JAY
// Initial revision.
// 
//    Rev 1.8   30 Nov 1994 16:33:26   JAY
// stdafx.h; merged database code from album
// 
//    Rev 1.7   22 Jul 1994 16:28:40   JAY
// Added some missing access methods.
// 
//    Rev 1.6   13 Jul 1994 11:58:32   JAY
// New flag to re-initialize graphics.
// 
//    Rev 1.5   10 Jun 1994 10:00:36   JAY
// Added access for arrays.
// 
//    Rev 1.4   01 Jun 1994 16:35:16   JAY
// Restructured 'type' parameter passing
// 
//    Rev 1.3   23 May 1994 08:16:28   JAY
// Made find_graphic() take an LPCSTR instead of an LPSTR.
// 
//    Rev 1.2   13 Jan 1994 17:01:00   JAY
//  
// 
//    Rev 1.1   05 Jan 1994 08:08:12   JAY
// Restructuring, etc.
// 
//    Rev 1.0   16 Dec 1993 16:25:40   JAY
//  
*/

#ifndef __DOCREC_H__
#define __DOCREC_H__

#include "arrayrec.h"
#include	"pagedefs.h"
#include "array.h"

class    ObjectList;
class    CPaperInfo;
struct   HyperlinkData;
/*
// The creation data for a document.
*/

typedef struct
{
/* Project type. */

	SHORT project_type;

/* Project type. */

	SHORT m_nProjectSubtype;

/* Paper orientation. */

	SHORT	orientation;

/* Paper tile dimensions. */

	USHORT		paper_width;	/* Design paper tile width. */
	USHORT		paper_height;	/* Design paper tile height. */

/* Document dimensions. */

	PPNT		dimensions;			/* Document dimensions (1800's of an inch). */

/* Paper info to use for new document. */

	CPaperInfo* m_pPaperInfo;


} DOC_CREATE_STRUCT, far *DOC_CREATE_STRUCT_PTR;

/*
// The document record.
*/

class DocumentRecord : public DatabaseRecord
{
protected:

/*
// Document variables.
*/

	struct
	{
		SHORT	project_type;

		SHORT	orientation;

	/* Paper tile dimensions. */

		USHORT		paper_width;	/* Design paper tile width. */
		USHORT		paper_height;	/* Design paper tile height. */

	/* Document dimensions. */

		PPNT		dimensions;			/* Document dimensions (1800's of an inch). */

	/* The current page's record number. */

		DB_RECORD_NUMBER	last_loaded_page;

	/* The document's information record. */

		DB_RECORD_NUMBER	info_record;

	/* The document's macro list record. */

		DB_RECORD_NUMBER	macro_list_record;

	/* The document's name list record. */

		DB_RECORD_NUMBER	name_list_record;

	/* Flags. */

		WORD m_wFlags;

	/* Project subtype. */

		SHORT m_nProjectSubtype;

		DB_RECORD_NUMBER	m_lPageArray;				/* page records. */
		DB_RECORD_NUMBER	m_lFontArray;				/* fonts available in this document. */
		DB_RECORD_NUMBER	m_lLayoutArray;			/* layouts available in this document. */
		DB_RECORD_NUMBER	m_lGraphicArray;			/* graphics available in this document. */
		DB_RECORD_NUMBER	m_lWarpFieldArray;		/* warp fields available in this document. */
		DB_RECORD_NUMBER	m_lOutlinePathArray;		/* outline paths available in this document. */
		DB_RECORD_NUMBER	m_lTextStyleArray;		/* text styles available in this document. */
		DB_RECORD_NUMBER	m_lStoryArray;				/* stories in this document. */
		DB_RECORD_NUMBER	m_lHyperlinkArray;		/* Hyperlinks in this document. */
		DB_RECORD_NUMBER	m_lMasterPageArray;		/* master pages. */
		SHORT					m_nSided;					/* see Sided enum below */
		DB_RECORD_NUMBER	m_lInstructionPageArray;/* Instruction pages. */
	} record;

	// In-memory pointers for easy access.

	CRecordArrayRecord* m_pPageArray;
	CRecordArrayRecord* m_pFontArray;
	CRecordArrayRecord* m_pLayoutArray;
	CRecordArrayRecord* m_pGraphicArray;
	CRecordArrayRecord* m_pWarpFieldArray;
	CRecordArrayRecord* m_pOutlinePathArray;
	CRecordArrayRecord* m_pTextStyleArray;
	CRecordArrayRecord* m_pStoryArray;
   CRecordArrayRecord* m_pHyperlinkArray;
   CRecordArrayRecord* m_pMasterPageArray;
   CRecordArrayRecord* m_pInstructionPageArray;

	PagePtr				m_pCurrentPage;
	PagePtr				m_pCurrentMasterPage;
	PagePtr				m_pCurrentInstructionPage;
	CPaperInfo*			m_pPaperInfo;

/*
// The constructor for this record.
// This is private. Use the create method.
*/

	DocumentRecord(DB_RECORD_NUMBER number, DB_RECORD_TYPE type, DatabasePtr owner, ST_DEV_POSITION far *where);

/*
// Assign method.
*/

	virtual ERRORCODE assign(DatabaseRecordRef record);

/*
// Destroy method.
*/
	virtual void destroy(void);

/*
// Useful array locking routines.
*/

	void LockPageArray(void);
	void LockFontArray(void);
	void LockLayoutArray(void);
	void LockGraphicArray(void);
	void LockWarpFieldArray(void);
	void LockOutlinePathArray(void);
	void LockTextStyleArray(void);
	void LockStoryArray(void);
	void LockHyperlinkArray(void);
	void LockMasterPageArray(void);
	void LockInstructionPageArray(void);

/*
// Read and convert a RecordArray to the new RecordArrayRecord class.
*/

	void ConvertArray(StorageDevice* pDevice, DB_RECORD_NUMBER& lRecord);

public:

	virtual ~DocumentRecord();

	virtual void ReleaseSubrecords(void);

/*
// Document record flags.
// Naturally, these should all be *NON-OVERLAPPING*.
*/

	enum
	{
		FLAG_HasWarpFields      = 0x01,
		FLAG_HasOutlinePaths    = 0x02,
		FLAG_HasPaperInfo       = 0x04,
		FLAG_HasNewStyleArrays  = 0x08
	};

	// Enum for determining how many master pages to use.
	// This is *not* the same as how many master pages are defined.
	// (E.g. there may be two master pages with only one being used).

	enum Sided
	{
		NotSided = 0,					// No master pages.
		SingleSided = 1,				// Single master page.
		DoubleSided = 2				// Two master pages.
	};

/*
// The creator for this type of record.
// Note that this is static so that we can register it with the database.
*/

	static ERRORCODE create(DB_RECORD_NUMBER number, DB_RECORD_TYPE type, DatabasePtr owner, VOIDPTR creation_data, ST_DEV_POSITION far *where, DatabaseRecordPtr far *record);

/*
// Page management.
*/

	ERRORCODE new_page(DB_RECORD_NUMBER far *, DWORD dwIndex = (DWORD)-1);
	void delete_page(DB_RECORD_NUMBER page_record);

	ERRORCODE AddPage(DB_RECORD_NUMBER lPage, DWORD dwIndex = (DWORD)-1);
	void RemovePage(DB_RECORD_NUMBER lPage);

	DB_RECORD_NUMBER get_last_loaded_page(void);
	ObjectList* load_page(DB_RECORD_NUMBER page_record = 0);
	void unload_page(VOID);

	DWORD NumberOfPages(void);
	DB_RECORD_NUMBER GetPage(DWORD dwPage);
	DWORD IndexOfPage(DB_RECORD_NUMBER lPage);		// Inverse of GetPage().

/*
// Font management.
*/

	void add_font_data(DB_RECORD_NUMBER f_record);
	void remove_font_data(DB_RECORD_NUMBER f_record);
	DB_RECORD_NUMBER find_font_data(LPSTR name);
	ERRORCODE initialize_font_records(BOOL delete_temps);

	DWORD NumberOfFonts(void);
	DB_RECORD_NUMBER GetFont(DWORD dwFont);

/*
// Layout management.
// This code uses reference counts to keep eliminate identical layout
// records.
*/

	void add_layout(DB_RECORD_NUMBER l_record);
	void remove_layout(DB_RECORD_NUMBER l_record);
	DB_RECORD_NUMBER find_layout(LPSTR name, SHORT type);

	DWORD NumberOfLayouts(void);
	DB_RECORD_NUMBER GetLayout(DWORD dwLayout);

/*
// Graphic record management.
// This code uses reference counts to keep eliminate identical graphic
// records.
*/

	void add_graphic(DB_RECORD_NUMBER g_record);
	void remove_graphic(DB_RECORD_NUMBER g_record);
	DB_RECORD_NUMBER find_graphic(LPCSTR name);
	ERRORCODE initialize_graphic_records(BOOL fInitAll);

	DWORD NumberOfGraphics(void);
	DB_RECORD_NUMBER GetGraphic(DWORD dwGraphic);

/*
// Warp field management.
// This code uses reference counts to keep eliminate identical warp
// field records.
*/

	void add_warp_field(DB_RECORD_NUMBER l_record);
	void remove_warp_field(DB_RECORD_NUMBER l_record);
	DB_RECORD_NUMBER find_warp_field(LPCSTR name);

	DWORD NumberOfWarpFields(void);
	DB_RECORD_NUMBER GetWarpField(DWORD dwWarpField);

/*
// Outline path management.
// This code uses reference counts to keep eliminate identical outline
// path records.
*/

	void add_outline_path(DB_RECORD_NUMBER l_record);
	void remove_outline_path(DB_RECORD_NUMBER l_record);
	DB_RECORD_NUMBER find_outline_path(LPCSTR name);

	DWORD NumberOfOutlinePaths(void);
	DB_RECORD_NUMBER GetOutlinePath(DWORD dwOutlinePath);

/*
// Text style management.
// This code uses reference counts to keep eliminate identical text style
// records.
*/

	void AddTextStyle(DB_RECORD_NUMBER lRecord);
	void RemoveTextStyle(DB_RECORD_NUMBER lRecord);
	DB_RECORD_NUMBER FindTextStyle(LPCSTR pName, int nType);

	DWORD NumberOfTextStyles(void);
	DB_RECORD_NUMBER GetTextStyle(DWORD dwTextStyle);

/*
// Story management.
*/

	void AddStory(DB_RECORD_NUMBER lRecord);
	void RemoveStory(DB_RECORD_NUMBER lRecord);
	DWORD FindStory(DB_RECORD_NUMBER lRecord);

	DWORD NumberOfStories(void);
	DB_RECORD_NUMBER GetStory(DWORD dwStory);

// Hyperlink management.

   void					AddHyperlink(DB_RECORD_NUMBER h_record);
   void					RemoveHyperlink(DB_RECORD_NUMBER h_record);
   DB_RECORD_NUMBER	GetHyperlink(DWORD dwHyperlink);
   void					AppendFullURL(struct HyperlinkData &hld, CString &str);
   void					AppendFullURLString(struct HyperlinkData &hld, CString &str);
	DWORD					NumberOfHyperlinks(void);

/*
// Double-sided?
*/
	int GetSided(void) const
		{ return record.m_nSided; }
	void SetSided(int nSided)
		{ record.m_nSided = nSided; }

/*
// Master page management.
*/

	ERRORCODE NewMasterPage(DB_RECORD_NUMBER* plPage = NULL, DWORD dwIndex = (DWORD)-1);
	ERRORCODE AddMasterPage(DB_RECORD_NUMBER lPage, DWORD dwIndex = (DWORD)-1);
	void RemoveMasterPage(DB_RECORD_NUMBER lPage);
	DWORD NumberOfMasterPages(void);
	DB_RECORD_NUMBER GetMasterPage(DWORD dwPage);
	DWORD IndexOfMasterPage(DB_RECORD_NUMBER lPage);	// Inverse of GetMasterPage().

	ObjectList* LoadMasterPageRecord(DB_RECORD_NUMBER lPage);
	ObjectList* LoadMasterPage(DWORD dwPage);
	void UnloadMasterPage(void);

/*
// Instruction page management.
*/

	ERRORCODE NewInstructionPage(DB_RECORD_NUMBER* plPage = NULL, DWORD dwIndex = (DWORD)-1);
	ERRORCODE AddInstructionPage(DB_RECORD_NUMBER lPage, DWORD dwIndex = (DWORD)-1);
	void RemoveInstructionPage(DB_RECORD_NUMBER lPage);
	DWORD NumberOfInstructionPages(void);
	DB_RECORD_NUMBER GetInstructionPage(DWORD dwPage);
	DWORD IndexOfInstructionPage(DB_RECORD_NUMBER lPage);	// Inverse of GetMasterPage().

	ObjectList* LoadInstructionPageRecord(DB_RECORD_NUMBER lPage);
	ObjectList* LoadInstructionPage(DWORD dwPage);
	void UnloadInstructionPage(void);

/*
// Return a pointer to the paper info.
*/

	CPaperInfo* GetPaperInfo(void);

/*
// Return the orientation of the document.
*/

	ORIENTATION get_orientation(VOID) const;

/*
// Set the orientation of the document.
*/

	VOID set_orientation(ORIENTATION orientation);

/*
// Return the size of the document.
*/

	PPNT get_dimensions(VOID) const;

/*
// Set the size of the document.
*/

	VOID set_dimensions(PPNT dimensions);

/*
// Get the info record number.
*/

	DB_RECORD_NUMBER get_info_record(VOID) const;

/*
// Set the info record number.
*/

	VOID set_info_record(DB_RECORD_NUMBER r);

/*
// Get the macro list record number.
*/

	DB_RECORD_NUMBER get_macro_list_record(VOID) const;

/*
// Set the macro list record number.
*/

	VOID set_macro_list_record(DB_RECORD_NUMBER r);

/*
// Get the name list record number.
*/

	DB_RECORD_NUMBER get_name_list_record(VOID) const;

/*
// Set the name list record number.
*/

	VOID set_name_list_record(DB_RECORD_NUMBER r);

/*
// Get the width of the paper.
*/

	USHORT get_paper_width(VOID) const;

/*
// Set the width of the paper.
*/

	 VOID set_paper_width(USHORT width);

/*
// Get the height of the paper.
*/

	USHORT get_paper_height(VOID) const;

/*
// Set the height of the paper.
*/

	 VOID set_paper_height(USHORT height);

/*
// Get the project type.
*/

	PROJECT_TYPE get_project_type(VOID) const;

/*
// Set the project type.
*/

	void set_project_type(PROJECT_TYPE nType);

/*
// Get the project sub-type.
*/

	int GetProjectSubtype(void) const;

/*
// Set the project sub-type.
*/

	void SetProjectSubtype(int nSubtype);

/***************************************************/
/* Methods used exclusively by the StorageManager. */
/***************************************************/

/*
// ReadData()
//
// Read the record.
*/

	virtual ERRORCODE	ReadData(StorageDevicePtr device);

/*
// WriteData()
//
// Write the record.
*/

	virtual ERRORCODE	WriteData(StorageDevicePtr device);

/*
// SizeofData()
//
// Return the size of the record.
*/

	virtual ST_MAN_SIZE SizeofData(StorageDevicePtr device);
};

/*
// Inline-function definitions.
*/

inline ORIENTATION DocumentRecord::get_orientation(VOID) const
	{ return (ORIENTATION)record.orientation; }
inline VOID DocumentRecord::set_orientation(ORIENTATION orientation)
	{ record.orientation = orientation; }
inline PPNT DocumentRecord::get_dimensions(VOID) const
	{ return record.dimensions; }
inline VOID DocumentRecord::set_dimensions(PPNT dimensions)
	{ record.dimensions = dimensions; }
inline DB_RECORD_NUMBER DocumentRecord::get_info_record(VOID) const
	{ return record.info_record; }
inline VOID DocumentRecord::set_info_record(DB_RECORD_NUMBER r)
	{ record.info_record = r; }
inline DB_RECORD_NUMBER DocumentRecord::get_macro_list_record(VOID) const
	{ return record.macro_list_record; }
inline VOID DocumentRecord::set_macro_list_record(DB_RECORD_NUMBER r)
	{ record.macro_list_record = r; }
inline DB_RECORD_NUMBER DocumentRecord::get_name_list_record(VOID) const
	{ return record.name_list_record; }
inline VOID DocumentRecord::set_name_list_record(DB_RECORD_NUMBER r)
	{ record.name_list_record = r; }
inline USHORT DocumentRecord::get_paper_width(VOID) const
	{ return record.paper_width; }
inline VOID DocumentRecord::set_paper_width(USHORT width)
	{ record.paper_width = width; }
inline USHORT DocumentRecord::get_paper_height(VOID) const
	{ return record.paper_height; }
inline VOID DocumentRecord::set_paper_height(USHORT height)
	{ record.paper_height = height; }
inline void DocumentRecord::set_project_type(PROJECT_TYPE nType)
	{ record.project_type = nType; }
inline PROJECT_TYPE DocumentRecord::get_project_type(VOID) const
	{ return (PROJECT_TYPE)record.project_type; }

inline DB_RECORD_NUMBER DocumentRecord::get_last_loaded_page(void)
	{ return record.last_loaded_page; }

inline int DocumentRecord::GetProjectSubtype(void) const
	{ return record.m_nProjectSubtype; }
inline void DocumentRecord::SetProjectSubtype(int nSubtype)
	{ record.m_nProjectSubtype = nSubtype; }

#endif					// #ifndef __DOCREC_H__
