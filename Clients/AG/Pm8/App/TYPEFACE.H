/*
// $Header: /PM8/App/TYPEFACE.H 1     3/03/99 6:12p Gbeddow $
//
// Typeface server definitions.
//
// $Log: /PM8/App/TYPEFACE.H $
// 
// 1     3/03/99 6:12p Gbeddow
// 
// 4     9/14/98 12:11p Jayn
// Removed system_heap. Switched to MFC in DLL.
// 
// 3     4/29/98 4:27p Fredf
// New PMW fonts. Required changes to PMW.FFL.
// 
// 2     12/18/97 3:23p Dennis
// Added Font Origin logic used by HTML generator to determine if font is
// a standard windows font.
// 
//    Rev 1.0   14 Aug 1997 15:26:44   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:36:22   Fred
// Initial revision.
// 
//    Rev 1.4   09 Apr 1997 14:51:44   Fred
// Start of new font caching (fonts and metrics)
// 
//    Rev 1.3   03 Apr 1997 09:32:16   Fred
// More text cleanup
// 
//    Rev 1.2   07 Feb 1997 09:02:02   Fred
// Got rid of font types
// 
//    Rev 1.1   27 Aug 1996 17:46:12   Fred
// Fixed bug where WM_FONTCHANGE could crash PM
// 
//    Rev 1.0   14 Mar 1996 13:31:16   Jay
// Initial revision.
// 
//    Rev 1.0   16 Feb 1996 12:36:46   FRED
// Initial revision.
// 
//    Rev 2.2   15 Mar 1995 10:48:26   JAY
// Support for new text editing
// 
//    Rev 2.1   08 Feb 1995 13:36:44   JAY
// Reverted. New series.
// 
//    Rev 1.6   16 Sep 1994 14:17:24   JAY
// Only "stuffs" font file lists if CD_VERSION.
// 
//    Rev 1.5   04 Aug 1994 08:30:52   JAY
// Large model and 32-bit changes
// 
//    Rev 1.4   10 Jun 1994 10:17:58   JAY
// Some methods changed to "const".
// New handling for registration fonts.
// 
//    Rev 1.3   01 Jun 1994 16:46:46   JAY
// Added new variations to the face entry structure and the code to allow
// virtual file (from Font File Lists) and a preview font.
// 
//    Rev 1.2   04 Mar 1994 12:00:24   MACDUFF
// Added BuildLogFont() method.
// 
//    Rev 1.1   13 Jan 1994 17:14:06   JAY
//  
// 
//    Rev 1.0   16 Dec 1993 16:29:00   JAY
//  
*/

#ifndef __TYPEFACE_H__
#define __TYPEFACE_H__

// Utility functions in TYPEFACE.CPP
SHORT typeface_for_fstyle(SHORT index, FONT_STYLE fstyle);
SHORT typeface_index(SHORT typeface);
FONT_STYLE typeface_style(SHORT typeface);

/*
// Defines for flags for styles supported.
*/

typedef enum
{
	FEStyle_normal,				// 0
	FEStyle_bold,					// 1
	FEStyle_italic,				// 2
	FEStyle_bolditalic,			// 3
	FEStyle_MAX
} FEStyleIndex;

/*
// Font variation types.
// Each variation of each face in the list has one of these types ('vtype').
*/

typedef enum
{
	VARIATION_TYPE_NONE = 0,
	VARIATION_TYPE_WIN31,					/* From Windows 3.1. (OBSOLETE)*/
	VARIATION_TYPE_WIN40,					/* From Windows 4.0 and above. */
	VARIATION_TYPE_VIRTUAL,					/* From font file list (FFL). */
	VARIATION_TYPE_REGISTRATION,			/* From font file list (FFL). */
	VARIATION_TYPE_PREVIEW					/* Uses preview font. */
} VARIATION_TYPE;

/*
// An entry in the face list.
*/

class FaceEntry : public ListElement
{
public:
	// These definitions are used by the HTML generator (Web Project)
	// to determine which fonts are standard windows fonts and which
	// are not. This is important because for a Web Project to be
	// properly viewed, the fonts used in the project need to be
	// installed on the client system. Non standard fonts should
	// probably be turned into a graphic to preserve WYSIWYG.
   enum FontOriginType
      {
         fotWindows95Standard,
         fotWindows95Extra,
         fotPrintMaster,
         fotReserved
      };
private:

	////////////////////////////////////////////////////////////////
	// The following information is built when the typeface entry
	// is built -- it is always valid.

	// The name of this face (for display purposes)
	CString m_csName;

	// The origin of the font (used for HTML conversion -- see above.)
   long  m_lFontOrigin;

	// TrueType font variations, indexed by font style:
	//
	//		0 = normal
	//		1 = bold
	//		2 = italic
	//		3 = bold italic

	struct
	{
		VARIATION_TYPE vtype;				// Does this variation actually exist?
		union
		{
			struct
			{
				USHORT italic : 1;			// Log font italic value
				USHORT underline : 1;		// Log font underline value
				USHORT strikeout : 1;		// Log font strikeout value
				USHORT quality : 2;			// Log font quality
				USHORT outPrecision : 3;	// Log font output precision
				BYTE charSet;					// Log font character set
				BYTE pitchAndFamily;			// Log font pitch and family
				BYTE clipPrecision;			// Log font clip precision
				SHORT weight;					// Log font weight value
				SHORT width;					// Log font width value
				SHORT height;					// Log font height value
			} s;					// for fonts installed in Windows
			struct
			{
				int preview_font_number;	// logical index of face in preview font
				LPSTR file_name;				// 8.3 file name (in PM fonts directory)
			} v;					// for "virtual" fonts (from PMW.FFL)
		};
	} variation[FEStyle_MAX];

	// Initialize a StorageFile (or LOGFONT) to access font file data for a variation.
	int setup_file_variation(SHORT i, LOGFONT* plf, StorageFilePtr file);

	// Build the style preference array.
	BOOL build_preference_array(SHORT style_index, SHORT preference[]);

	////////////////////////////////////////////////////////////////



	////////////////////////////////////////////////////////////////
	// The following fields require information that is not easily
	// available to the typeface server, but is typeface specific.
	// In addition, computing this information requires opening the
	// font file up or some other "large" operation which we don't
	// necessarily want to do for each font. Instead, this information
	// is computed at the time the font is first used (GetFontCacheItem)
	// and then cached in these fields in the typeface entry structure.

	// True if extended information fields below are valid.
	BOOL m_fIsExtendedInformationValid;

	// Minimum recommended pixel height (used for greeking.)
	uint16 m_uMinimumPixelHeight;

	// Is this a text font (used for character mapping.)
	BOOL m_fIsTextFont;

	////////////////////////////////////////////////////////////////


public:

	// Constructor
	FaceEntry();

	// Destructor
	~FaceEntry();

	// The standard variation type.
	static VARIATION_TYPE standard_variation_type;

	// Silliness to keep from having to cast all over the place.
	FaceEntry* next_face()
		{ return (FaceEntry*)next_element(); }

	// Return whether this is a chooseable face or not.
	BOOL is_chooseable(BOOL is_registered) const;

	// Set the face name.
	BOOL set_name(LPCSTR new_name);

	// Get the face name.
	LPCSTR get_name(void) const
		{ return m_csName; }

	// Set the file name for a virtual (PMW.FFL) font.
	BOOL set_file_name(LPCSTR new_file_name, SHORT i);

	// Setup the file for this file and variation.
	int setup_file(SHORT style_index, LOGFONT* plf, StorageFilePtr file);

	// Add a variation for a Windows font.
	void add_standard_variation(SHORT style_index, LOGFONT* plf);

	// Add a virtual variation (from PMW.FFL)
	void add_virtual_variation(SHORT style_index, LPCSTR pszFileName, int nPreviewFontNumber, BOOL registration = FALSE);

	// Set all preview variations.
	void set_preview_variations(void);

	// Get the type of a variation.
	VARIATION_TYPE get_variation_type(SHORT style_index);

	// Get the logical index of the face in the preview font
	int get_preview_font_number(SHORT style_index);

   // Set the origin of this font family
   void SetFontOrigin(FontOriginType enumFontType)
      {
         m_lFontOrigin = (long) enumFontType;
      }

   // Get the origin of this font family
   FontOriginType GetFontOrigin()
      {
         return (FontOriginType) m_lFontOrigin;
      }

	// Build a logfont structure for a face style.
	void BuildLogFont(int nStyleIndex, LOGFONT* plf);

	// Fake the registration of a face.
	// This makes each variation "virtual" instead of "registration".
	// It returns TRUE if any variation is "registration".
	BOOL fake_register(void);


	////////////////////////////////////////////////////////////////
	// Access methods for extended information fields. The extended
	// are computed the first time the face is used (GetFontCacheItem)
	// and cached in the face structure.

	BOOL GetIsExtendedInformationValid(void) const
		{ return m_fIsExtendedInformationValid; }
	void SetIsExtendedInformationValid(BOOL fIsExtendedInformationValid = TRUE)
		{ m_fIsExtendedInformationValid = fIsExtendedInformationValid; }

	uint16 GetMinimumPixelHeight(void) const
		{ return m_uMinimumPixelHeight; }
	void SetMinimumPixelHeight(uint16 uMinimumPixelHeight)
		{ m_uMinimumPixelHeight = uMinimumPixelHeight; }

	BOOL GetIsTextFont(void) const
		{ return m_fIsTextFont; }
	void SetIsTextFont(BOOL fIsTextFont = TRUE)
		{ m_fIsTextFont = fIsTextFont; }

};

/*
// A list of faces.
*/

class FaceList : public List
{
private:

/*
// Build the family list.
*/

	SHORT build_family_list(HDC hdc);

/*
// Get the variations.
*/

	SHORT get_face_variations(HDC hdc);

/*
// Stuff an individual font file list.
*/

	BOOL stuff_font_file_list(PCSTR name);

/*
// Stuff all font file lists in the main directory.
*/

	void stuff_font_file_lists(void);

// 
	void DetermineFontSources();

public:
/* Constructor. */

	FaceList() : List(0, TRUE) {}

/* Silliness to keep from having to cast all over the place. */

	FaceEntry* get_face(SHORT which)
		{ return (FaceEntry*)element_in_list(which); }

	FaceEntry* first_face(void)
		{ return (FaceEntry*)first_element(); }

/*
// Find the face in the face list. Returns -1 if the face is not in the list.
*/

	SHORT find_face(LPCSTR face_name);

/* Add a new entry to the list. */

	BOOL add_entry(LPCSTR name, FaceEntry** pentry = NULL);

/* Build the face list. */

	BOOL build(void);
};

/*
// The typeface server class.
*/

class TypefaceServer
{
private:

	CString m_font_preview_filename;

	// Initialized flag.

	BOOL m_fInitialized;

/*
// Add the preview face to the list.
*/

	void add_preview_face(void);

public:

/*
// Constructor.
*/

	TypefaceServer();

/*
// Destructor.
*/

	~TypefaceServer();

/*
// The system face list.
*/

	FaceList face_list;

/*
// The virtual path for font file lists (FFLs).
*/

	CString m_csVirtualFontPath;

/*
// A handle for getting a physical font from a DC.
*/

	static HANDLE (FAR PASCAL * lpGetPhysicalFontHandle)(HDC hDC);

/*
// The default face.
*/

	char const *default_face_name;

/*
// Read the list of faces.
*/

	BOOL build_face_list(void);

/*
// Find the face in the face list.
// If the face is not in the list, the replacement face is returned.
*/

	SHORT find_face(LPCSTR face_name, BOOL fReplace = TRUE);

/*
// Call this to initialize the font server.
*/

	BOOL startup(char const *default_face);

/*
// This returns the name of the preview font.
// If returns NULL if there is none (so check!)
*/

	PCSTR preview_face_name(void);

/*
// This returns the name of the font preview filename.
// If returns NULL if there is none (so check!)
*/

	PCSTR font_preview_filename(void);

/*
// This is a routine we can pass the the TrueType engine constructor
// to enable it to get the path name for a font.
//
// The typeface number should be as follows:
// bits			meaning
// ----			-------
//  15			 italic
//  14			 bold
// 13-0			 face list index
//
*/

	static int AFX_EXPORT get_face_path(int typeface, LOGFONT* plf, StorageFilePtr file);

/*
// The face index for a replacement face.
*/

	SHORT replacement_face;

/*
// Return the face entry for this face index.
*/
	FaceEntry* get_face(int which);

};

inline FaceEntry* TypefaceServer::get_face(int which)
	{ return face_list.get_face(which); }

extern TypefaceServer near typeface_server;

#endif					// #ifndef __TYPEFACE_H__
