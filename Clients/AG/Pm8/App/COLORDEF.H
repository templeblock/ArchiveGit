/*
// $Workfile: COLORDEF.H $
// $Revision: 1 $
// $Date: 3/03/99 6:04p $
//
//  "This unpublished source code contains trade secrets which are the
//   property of Mindscape, Inc.  Unauthorized use, copying or distribution
//   is a violation of international laws and is strictly prohibited."
// 
//        Copyright © 1997-1998 Mindscape, Inc. All rights reserved.
*/

/*
// Revision History:
//
// $Log: /PM8/App/COLORDEF.H $
// 
// 1     3/03/99 6:04p Gbeddow
// 
// 5     6/30/98 5:57p Hforman
// remove old browser colors
// 
// 4     4/20/98 7:39p Fredf
// Added colors for helpful hint backgrounds.
// 
// 3     3/10/98 5:24p Jayn
// Master page changes
// 
// 2     11/19/97 2:06p Hforman
// added GUIDELINE_COLOR
// 
//    Rev 1.0   14 Aug 1997 15:19:22   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:33:10   Fred
// Initial revision.
// 
*/

// Standard Colors
#define  RGB_COLOR_RED               PALETTERGB(255, 0, 0)
#define  RGB_COLOR_GREEN             PALETTERGB(0, 255, 0)
#define  RGB_COLOR_BLUE              PALETTERGB(0, 0, 255)
#define  RGB_COLOR_WHITE             PALETTERGB(255, 255, 255)
#define  RGB_COLOR_BLACK             PALETTERGB(0, 0, 0)
#define  RGB_COLOR_MAGENTA           PALETTERGB(255, 0, 255)

/////////////////////////////////////////////////////////////////////////////
// No color matte in dialogs derived from CPmwDialogColor
#define  INVALID_DIALOG_COLOR    (COLORREF)-1
// HUB colors
#define  HUB_FILL_COLOR          PALETTERGB (0, 0, 0)//PALETTERGB (102, 153, 255)
// Currently used in all dialogs derived from CPmwDialogColor
#define  BRAND_NEW_BOX_COLOR     PALETTERGB (0, 0, 0)       
// Ready made path dialog colors
#define  READY_MADE_COLOR        PALETTERGB (204, 102, 51)//PALETTERGB (255, 153, 51)
// With help path dialog colors
#define  WITH_HELP_COLOR         PALETTERGB (153, 0, 51)//PALETTERGB (255, 102, 102)
// Brand new path dialog colors
#define  BRAND_NEW_COLOR         PALETTERGB (102, 51, 153)//PALETTERGB (255, 102, 204)
// Your projects path dialog colors
#define  YOUR_PROJ_COLOR         PALETTERGB (153, 102, 0)//PALETTERGB (153, 255, 51)
// Art Gallery path dialog colors
#define  ART_GALLERY_COLOR       PALETTERGB (0, 102, 51)//PALETTERGB (51, 204, 102)
// Sentiment Gallery path dialog colors
#define  SENTIMENT_GALLERY_COLOR PALETTERGB (204, 51, 255)
// Internet path dialog colors
#define  INTERNET_COLOR          PALETTERGB (51, 51, 153)//PALETTERGB (51, 204, 204)
// Address book path dialog colors
#define  ADDRESS_BOOK_COLOR      PALETTERGB (0, 102, 204)//PALETTERGB (51, 153, 255)
// Workspace welcome
#define  WORKSPACE_COLOR         PALETTERGB (57, 57, 57)
// Guide color
#define  GUIDE_COLOR					PALETTERGB(0, 153, 255)
#define  MASTER_GUIDE_COLOR		PALETTERGB(0, 255, 255)
// Helpful Hint Background
#define	HELPFUL_HINT_COLOR		PALETTERGB(255, 255, 204)
// design Hint Background
#define	DESIGN_HINT_COLOR			PALETTERGB(204, 255, 255)

// Common Browser colors
#define BROWSER_CAPTION_COLOR             RGB_COLOR_BLACK
#define BROWSER_SELECTED_CAPTION_COLOR    RGB_COLOR_BLUE
#define BROWSER_SELECTED_CAPTION_BK_COLOR RGB_COLOR_WHITE
#define BROWSER_UNLICENSED_CAPTION_COLOR  RGB_COLOR_RED

