#include "Resource.h"

// Dialogs
//
#define IDD_ABOUTBOX							 100

#define IDD_PSLAUNCH_DIALOG						1000

#define IDD_CATEGORY_CARD_SHOP					1001
#define IDD_CATEGORY_PHOTO_PROJECTS				1002
#define IDD_CATEGORY_CREATIVE_CORNER			1003
#define IDD_CATEGORY_ALL						1004
#define IDD_CATEGORY_INTERNETWORLD				1005
#define IDD_CATEGORY_CLASSROOM					1006
#define IDD_CATEGORY_WORKPLACE					1007
#define IDD_CATEGORY_OPEN						1008


#define IDD_PROJECT_METHOD						2000
#define IDD_PROJECT_METHOD_PRESSWRITER			2001
//#define IDD_PROJECT_GREETINGCARD_METHOD		2001
//#define IDD_PROJECT_LABEL_METHOD				2002
#define IDD_PROJECT_CALENDAR_METHOD				2003
#define IDD_PROJECT_WEBSITE_METHOD				2500

#define IDD_PROJECT_FORMAT						3000
//#define IDD_PROJECT_NEWSLETTER_FORMAT			3002
#define IDD_PROJECT_FORMAT_GREETINGCARD			3500
#define IDD_PROJECT_FORMAT_LABEL				3501
#define IDD_PROJECT_FORMAT_ENVELOPE				3502		
#define IDD_PROJECT_FORMAT_PRESSWRITER			3503

#define IDD_PROJECT_THEME						4000
#define IDD_PROJECT_GREETINGCARD_THEME			4001

#define IDD_PROJECT_QSL							5000
//#define IDD_PROJECT_GREETINGCARD_QSL			5001
#define IDD_PROJECT_QSL_PRESSWRITER				5500

#define IDD_PROJECT_TYPE						6000
#define IDD_PROJECT_CALENDAR_TYPE				6503

#define IDD_PROJECT_SETUP						7000


// System Data
//
#define ID_PROJECT_MAP							1000
#define ID_PROJECT_PAPER_MAP					1001

// PSButton Flags
//
#define IMAGEDATA								0x0001
#define IMAGEDATA_wFOCUS						0x0003
#define IMAGEDATA_ONDOWN						0x0004
#define IMAGEDATA_INFOTEXT						0x0008
#define IMAGEDATA_TEXT							0x0010

// The following types map to the first project entry
// in their respective groups.  These defines are needed
// because the specific type of project isn't known until
// part way down the project path.
#define kCalendar									4 
#define	kGreetingCard								8
#define kEnvelope									24
#define kLabels										32
#define kNameTag									54

// Misc types - These are project types that map to other 
// PrintShop project types, but need to have a unique value
// for some reason or another.
#define kBlankPage									301
#define kTrifoldBrochureTall						302
#define kTrifoldBrochureWide						303

// PressWriter types
//
#define kPressWriterProjectStart					150

// Leapfrog types
//
#define kLeapFrogProjectStart						160
#define kWebsite									161

#define TALL										0
#define WIDE										1

// Misc
#define kAllPaperTypes								-1
#define kPaperMapDefault							100
