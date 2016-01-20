/*
// $Header: /PM8/App/PROJECT.H 1     3/03/99 6:11p Gbeddow $
//
// Project table and other description definitions.
//
// $Log: /PM8/App/PROJECT.H $
// 
// 1     3/03/99 6:11p Gbeddow
// 
// 15    2/24/99 1:39p Johno
// Fix for icon / file association errors
// 
// 14    2/20/99 9:21p Psasse
// Support for Vertical Banners
// 
// 13    2/04/99 3:06p Johno
// Changes for new "Blank Page" (poster) project type
// 
// 12    12/24/98 5:06p Psasse
// PROJECT_TYPE_PhotoProjects type
// 
// 11    12/23/98 5:33p Psasse
// support for new project type - easyprints
// 
// 10    10/20/98 10:59a Mwilson
// added more craft types
// 
// 9     8/24/98 3:30p Hforman
// add CRAFT_TYPE_BigCard, fix typo
// 
// 8     8/20/98 12:51p Hforman
// add CRAFT_TYPE
// 
// 7     8/06/98 4:33p Mwilson
// added craft project type
// 
// 6     4/17/98 4:47p Fredf
// T-Shirts
// 
// 5     1/07/98 7:49a Jstoner
// added table panel enum
// 
// 4     12/22/97 5:25p Hforman
// add PROJECT_TYPE_Last
// 
// 3     12/05/97 5:13p Hforman
// include pagedefs.h
// 
// 2     12/01/97 1:27p Jstoner
// created sticker project type
// 
//    Rev 1.1   19 Sep 1997 17:32:12   johno
// Changes for new Post Card project type
// (preliminary)
// 
//    Rev 1.0   14 Aug 1997 15:25:02   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:35:34   Fred
// Initial revision.
// 
//    Rev 1.3   04 Apr 1997 15:35:38   Jay
// Half-fold card and web projects
// 
//    Rev 1.2   01 Apr 1997 16:33:42   Jay
// Beginning of half-fold cards.
// 
//    Rev 1.1   23 Jul 1996 10:39:10   Jay
// Added newsletter and brochure
// 
//    Rev 1.0   14 Mar 1996 13:44:24   Jay
// Initial revision.
// 
//    Rev 1.0   16 Feb 1996 12:07:00   FRED
// Initial revision.
// 
//    Rev 2.3   31 Jul 1995 10:22:30   JAY
// Addes stationery type.
// 
//    Rev 2.2   20 Jul 1995 12:47:52   JAY
// Added types for labels and envelopes.
// 
//    Rev 2.1   12 Jul 1995 17:43:34   JAY
// New project sub-types (esp. for cards).
// 
//    Rev 2.0   07 Feb 1995 15:48:54   JAY
// Initial revision.
// 
//    Rev 1.1   06 Oct 1994 13:35:10   JAY
// get_panel_world() now also take a margin rect pointer.
// 
//    Rev 1.0   16 Dec 1993 16:26:08   JAY
//  
*/

#ifndef __PROJECT_H__
#define __PROJECT_H__

#ifndef __PAGEDEFS_H__
#include "pagedefs.h"
#endif

//
// Types of documents.
//
// NOTE: the string IDS_PROJECT_TYPES must correspond to this enum!
//
typedef enum
{
	PROJECT_TYPE_Generic = 0,		// 0
	PROJECT_TYPE_Poster,				// 1
	PROJECT_TYPE_Card,				// 2
	PROJECT_TYPE_Banner,				// 3
	PROJECT_TYPE_Calendar,			// 4
	PROJECT_TYPE_Label,				// 5
	PROJECT_TYPE_Envelope,			// 6
	PROJECT_TYPE_BusinessCard,		// 7
	PROJECT_TYPE_Certificate,		// 8
	PROJECT_TYPE_NoteCard,			// 9
	PROJECT_TYPE_FaxCover,			// 10
	PROJECT_TYPE_Stationery,		// 11
	PROJECT_TYPE_Newsletter,		// 12
	PROJECT_TYPE_Brochure,			// 13
	PROJECT_TYPE_HalfCard,			// 14
	PROJECT_TYPE_WebPub,				// 15
	PROJECT_TYPE_PostCard,			// 16
	PROJECT_TYPE_Sticker,			// 17
	PROJECT_TYPE_TShirt,				// 18
	PROJECT_TYPE_Craft,				// 19
	PROJECT_TYPE_PhotoProjects,	//	20
	PROJECT_TYPE_BlankPage,			// 21

	PROJECT_TYPE_Last
} PROJECT_TYPE;

//
// Types of Crafts
//
// NOTE: the string IDS_CRAFT_TYPES must correspond to this enum!
//
enum CRAFT_TYPE
{
	CRAFT_TYPE_Generic = 0,
	CRAFT_TYPE_Album,
	CRAFT_TYPE_AttachmentCards,
	CRAFT_TYPE_Bag,
	CRAFT_TYPE_Basket,
	CRAFT_TYPE_BigCards,
	CRAFT_TYPE_Bookmark,
	CRAFT_TYPE_Box,
	CRAFT_TYPE_Calendar,
	CRAFT_TYPE_Centerpiece,
	CRAFT_TYPE_Certificate,
	CRAFT_TYPE_CutoutCard,
	CRAFT_TYPE_Cutouts,
	CRAFT_TYPE_DeskPlaques,
	CRAFT_TYPE_Doorhanger,
	CRAFT_TYPE_Envelopes,
	CRAFT_TYPE_FrameCard,
	CRAFT_TYPE_Frame,
	CRAFT_TYPE_Garland,
	CRAFT_TYPE_GiftEnclosures,
	CRAFT_TYPE_Hat,
	CRAFT_TYPE_Invitation,
	CRAFT_TYPE_KidsToys,
	CRAFT_TYPE_Mask,
	CRAFT_TYPE_Mat,
	CRAFT_TYPE_Mobile,
	CRAFT_TYPE_NapkinHolder,
	CRAFT_TYPE_Ornament,
	CRAFT_TYPE_PackageDecorations,
	CRAFT_TYPE_PlaceCard,
	CRAFT_TYPE_PlaceMat,
	CRAFT_TYPE_PopupCard,
	CRAFT_TYPE_Poster,
	CRAFT_TYPE_Puzzle,
	CRAFT_TYPE_RecipeCard,
	CRAFT_TYPE_Scrapbook,
	CRAFT_TYPE_SelfMailer,
	CRAFT_TYPE_SliderCards,
	CRAFT_TYPE_Stationery,
	CRAFT_TYPE_Stencil,

	CRAFT_TYPE_Last
};


enum PHOTO_PROJECT_TYPE
{
	PHOTO_PROJECT_TYPE_Generic = 0,
	PHOTO_PROJECT_TYPE_EasyPrints,

	PHOTO_PROJECT_TYPE_Last
};

/*
// Information defined for a panel.
*/

typedef struct
{
	RECT	panel_pos;						/* What part of the page does the panel */
												/* occupy (as 10 * %age of page) */
	RECT	panel_margin;					/* Margin within pos (as 1000ths of in.) */
	FLAGS	flags;							/* flags for the objects to have */
	SHORT	border_scale;					/* The scale for the border. */
	SHORT m_nPageIndex;					/* What page are we on? (default = 0) */
} PANEL_INFO;

/*
// Information defined for a project.
*/

struct ProjectInfo
{
	PROJECT_TYPE	project_type;
	int				project_sub_type;
	ORIENTATION		default_orientation;		/* The default orientation. */
	SHORT				number_of_panels;			/* The number of project panels */
	SHORT				frame_layout_id;			/* ID of frame layout file name */
	SHORT				graphic_layout_id;		/* ID of graphic layout file name */
	SHORT				duplicate_layout_id;		/* ID of dup layout file name */
	PANEL_INFO		*pip;							/* Panel information */

	VOID get_panel_world(PBOX_PTR world,
								PCOORD xsize,
								PCOORD ysize,
								SHORT this_panel,
								LPCRECT margins = NULL);
};

/*
// Banner panels.
*/

enum
{
	MAIN_BANNER_PANEL = 100
};

/*
// Calendar panels.
*/

enum
{
	MAIN_CALENDAR_PANEL = 100
};

/*
// tablel panels
*/
enum 
{
	MAIN_TABLE_PANEL = 150
};

/*
// Card panels.
*/

enum
{
// New panels.
	CARD_PANEL_Front				= 0,
	CARD_PANEL_Inside				= 1,
	CARD_PANEL_Back				= 2,
// Old panels.
	CARD_PANEL_OldInsideLeft	= 1,
	CARD_PANEL_OldInsideRight	= 2,
	CARD_PANEL_OldBack			= 3
};

/*
// Sub-types for cards.
*/

enum
{
	CARD_Unknown		= 0,
	CARD_SideFold		= 1,			// Quarter-fold side-fold card
	CARD_TopFold		= 2,			// Quarter-fold top-fold card
};

/*
// Sub-types for banners.
*/

enum
{
	BANNER_Unknown			= 0,
	BANNER_Horizontal		= 1,			// horizontal banner
	BANNER_Vertical		= 2,			// vertical banner
};

#endif						// #ifndef __PROJECT_H__
