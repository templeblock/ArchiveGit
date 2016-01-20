/* @doc
   @module enums.h - enums used by CPubView and other modules |

   This module was created to avoid the baggage of including pubview.h
   in the headers of other mudules that simply need to know the definition
   of enum types used by the view.  It is included in pubview.h.

  Contents include:
  <nl> enum <t EBJust> - justification styles
  <nl> enum <t EBSpace> - line spacing styles
  <nl> enum <t EBViewType> - user view modes

   <cp>Copyright 1995 Brøderbund Software, Inc., all rights reserved
*/

#ifndef ENUMS_H
#define ENUMS_H

// @enum types of projects supported by the program
//
enum EBProjectType
{
    PROJECT_UNKNOWN = -1,   // @emem unspecified
    PROJECT_NEWSLETTER,     // @emem newsletter
    PROJECT_BROCHURE,       // @emem brochure
    PROJECT_REPORT,         // @emem report
    PROJECT_BOOKLET,        // @emem booklet
    PROJECT_LETTER,         // @emem letter/resume
    PROJECT_FLYER,          // @emem flyer
    PROJECT_CUSTOM,         // @emem user defined
    NUMBER_OF_PROJECT_TYPES // @emem must be last element of enum
};

// @enum tags for the text justification styles supported by the UI
//
enum EBJust
{
    JUST_UNKNOWN = -1,      // @emem justification unknown or ambiguous
    JUST_LEFT,              // @emem left justification (ragged right)
    JUST_CENTER,            // @emem center justification (both sides ragged)
    JUST_RIGHT,             // @emem right justification (ragged left)
    JUST_FULL,              // @emem full justification (both sides flush)
    JUST_MIRRORINSIDE,      // @emem header/footer mirrored inside edge
    JUST_MIRROROUTSIDE      // @emem header/footer mirrored outside edge
};

// @enum tags for the line spacing styles supported by the UI
//
enum EBSpace
{
    SPACE_UNKNOWN = -1,     // @emem spacing unknown or ambiguous
    SPACE_TIGHT,            // @emem tight line spacing (single spaced)
    SPACE_MEDIUM,           // @emem medium line spacing (1.5 spaced)
    SPACE_WIDE,             // @emem wide line spacing (double spaced)
    SPACE_CUSTOM            // @emem custom line spacing
};

// @enum tags for the Avoidance gutter used by the TextSlab when avoiding other objects
//
enum EBAvoidance
{
    AVOIDANCE_UNKNOWN= -1,      // @emem avoidance unknown or ambiguous
    AVOIDANCE_TIGHT,            // @emem tight avoidance
    AVOIDANCE_MEDIUM,           // @emem medium avoidance
    AVOIDANCE_LOOSE             // @emem loose avoidance
};

enum EBWrapType
{
    WRAP_TO_OBJECT,
    WRAP_TO_CONTENT,
    WRAP_NOT
};

// @enum tags for the emphasis styles supported by the UI. 
// These atttributes are not mutually exclusive. The enum is used
// when querying for the status of one of the attributes to distinguish
// which one we're querying for.
enum EBEmphasis
{
    EMPHASIS_UNKNOWN = -1,		// @emem emphasis unknown or ambiguous
    EMPHASIS_PLAIN,				// @emem plain text
    EMPHASIS_BOLD,				// @emem bold text
    EMPHASIS_ITALIC,			// @emem italic text
    EMPHASIS_UNDERLINE			// @emem underline text
};

// @enum Supported view modes
//
enum EBViewType
{
    V_NONE = 0,             // @emem used during startup
    V_JOINED,               // @emem view of two consecutive pages
    V_SEPARATED,            // @emem view of two seperated pages
    V_SINGLE,               // @emem full page view
    V_PAGEWIDTH,            // @emem page width fits within window
    V_100,                  // @emem "Actual Size" view
    V_150,                  // @emem magnified to 150%
    V_200,                  // @emem 2x magnified view
    V_400                   // @emem 4x magnified view
};

// @enum active view page
//
enum EBActiveViewPage
{
    AVP_LEFT = 0,           // @emem left page is active
    AVP_RIGHT               // @emem right page is active
};


// @enum Hint types for <mf CBPubDoc::UpdateAllViews>,
//       acted on in <mf CPubView::OnUpdate>
//
enum EBUpdateHints
{
    HINT_INVALIDATE = 1,					// @emem invalidate entire view
    HINT_INVALIDATE_LAYOUT_RECT,			// @emem invalidate specified <c CBLayoutRect>
    HINT_DELETING_DOCUMENT,					// @emem watch out - document is being emptied
    HINT_SHOW_LAYOUT_RECT,					// @emem pHint will contain a CBLayoutRect
    HINT_INVALIDATE_OBJECT,					// @emem invalidate the rect occupied by a given CBLayoutObject
    HINT_NEW_DOCUMENT,						// @emem the document has been re-created
    HINT_CLEAR_SELECTION,					// @emem the layout selection has been cleared
    HINT_SELECT_LAYOUT_OBJECT,				// @emem select a layout object
    HINT_SELECT_POINTER_TOOL,				// @emem select the pointer tool
    HINT_SELECT_ADDITIONAL_LAYOUT_OBJECT,	// @emem add a layout object to the selection list
    HINT_INVALIDATE_PAGE,					// @emem invalidate the specified page
    HINT_FLIP_TO_PAGE,						// @emem flip to the specified page
	HINT_SETUP_TEXTTOOL,					// @emem Setups up text tool.  If not active it makes it active
	HINT_UPDATE_TEXT_SELECTION				// @emem Updates text selection.  Needed because stonehand has a ubg drawing selection when there is a drop cap
};

// @enum Bitmasked enum, used to specify alignment for rectangles
//
enum EBAlignRect
{
    AR_HORZCENTER  = 0x0001,    // @emem horizontal center
    AR_LEFT        = 0x0002,    // @emem left edge
    AR_RIGHT       = 0x0004,    // @emem right edge
    AR_VERTCENTER  = 0x0010,    // @emem vertical center
    AR_TOP         = 0x0020,    // @emem top edge
    AR_BOTTOM      = 0x0040,    // @emem bottom edge
    AR_CENTER      = 0x0011,    // @emem center in both dimensions
    AR_TOPLEFT     = 0x0022,    // @emem top left corner
    AR_TOPRIGHT    = 0x0024,    // @emem top right corner
    AR_BOTTOMLEFT  = 0x0042,    // @emem bottom left corner
    AR_BOTTOMRIGHT = 0x0044     // @emem bottom right corner
};

// @enum Guide types
//
enum EBGuideType
{
	GT_NONE=0,		// @emem no type
	GT_HORZ,		// @emem horizontal guide
	GT_VERT,		// @emem vertical guide
	GT_HORZVERT,	// @emem horizontal and vertical guide
};


// @enum Margin Guide identifiers
//
enum EBMarginGuide
{
	MG_TOP=0,		// @emem top margin guide
	MG_LEFT,		// @emem left margin guide
	MG_BOTTOM,		// @emem bottom margin guide
	MG_RIGHT,		// @emem right margin guide
};


// @enum optional elements that may be rendered to a page
//
enum EBPageDrawElements
{
	PD_NONE                   = 0x0000,		// @emem no optional elements
	PD_PRINTABLE_AREA_OUTLINE = 0x0001,		// @emem outline of printable area
	PD_MARGIN_GUIDES          = 0x0002,		// @emem margin guides
	PD_MOVEABLE_GUIDES        = 0x0004,		// @emem moveable guides
	PD_GUIDES                 = 0x0006,		// @emem all guides
	PD_THIRDPAGE_FOLDLINES	  = 0x000E,		// @emem fold lines which cut the page in thirds. This sets PD_GUIDES true
	PD_HALFPAGE_FOLDLINES	  = 0x0016,		// @emem fold lines which cut the page in half. This sets PD_GUIDES true
	PD_TEXTSLAB_OUTLINE		  = 0x0020		// @mem outline of text boxes
};


// @enum document save load exception types, used in override of ReportSaveLoadException
//
enum EBReportSaveLoadExceptionType
{
	DOC_EXCEPT_DEFAULT=0,	// @emem call base class implementation
	DOC_EXCEPT_PREVIEW		// @emem document preview
};


// @enum type of tracking used by pointer tool
//
enum EBTrackType
{
	TT_NONE							= 0,						// @emem no tracking type
	TT_LINE							= 0x00000001,						// @emem track line
	TT_TEXT							= 0x00000002,						// @emem track text slab
	TT_BOX							= 0x00000004,							// @emem track box
	TT_GRAPHIC						= 0x00000008,						// @emem track graphic
	TT_COMPONENT_KEEP_ASPECT		= 0x00000010,		// @emem track compoent, aspect correct
	TT_COMPONENT_DONT_KEEP_ASPECT	= 0x00000020,	// @emem track compoent, non-aspect correct
	TT_HEADER						= 0x00000040,						// @emem track header
	TT_FOOTER						= 0x00000080,						// @emem track footer
	TT_MULTI						= 0x00000100						// @emem track multiple layout objects
};

// @enum used instead of a BOOL a type which can represent three states, on off and unknown
enum EBThreeState
{
	STATE_UNKNOWN = -1,	// @emem state "unknown"
	STATE_OFF,			// @emem "off" or "FALSE" state
	STATE_ON,			// @emem "on" or "TRUE" state
};

// @enum reflect the paper choices in the drop-down lists in dialogs 
enum EBPaperChoice
{  
	PAPER_LETTER = 0,   
	PAPER_LEGAL,    
	PAPER_A4,
	NUMBER_OF_PAPERS
};

// @enum Reflects a page's relationship to its surrounding pages.  Used for
//			HTML page generation.
enum EBPageRelation
{
	PR_STANDALONE,	// @emem The page has no siblings.
	PR_FIRST,		// @emem This is the first page, it only has a following page.
	PR_MIDDLE,		// @emem This is the middle page, it has a predecessor, and a following page.
	PR_LAST			// @emem This is the last page, it only has a predecessor page.
};

#endif
