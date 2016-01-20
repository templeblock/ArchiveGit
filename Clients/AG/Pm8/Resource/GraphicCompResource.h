// ****************************************************************************
//
//  File Name:			GraphicCompResource.h
//
//  Project:			Renaissance Framework
//
//  Author:				Lance Wilson
//
//  Description:		Common command ids.
//
//  Portability:		Platform independent
//
//  Developed by:		Broderbund Software, Inc. 
//						500 Redwood Blvd
//						Novato, CA 94948
//
//  Copyright (C) 1995-1997 Broderbund Software. All Rights Reserved.
//
//  $Logfile:: /PM8/Resource/GraphicCompResource.h                            $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:28p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

// ****************************************************************************
//
//		Resource ID range index :
//
//		Flyout IDs:
//			Toolbar										13000 - 13099
//			Buttons										13100 - 13199
//
//		Dialog IDs:
//			Component									13000 - 13099
//			Interface									13100 - 13199
//
//		Bitmap IDs:										
//			Component									13000 - 13099
//			Interface									13100 - 13199
//
//		String IDs:
//			Component									13000 - 13199
//			Interface									13200 - 13399
//
// ****************************************************************************

#ifndef	_GRAPHICCOMPRESOURCE_H_
#define	_GRAPHICCOMPRESOURCE_H_


// ****************************************************************************
//
//		FLYOUT: Toolbar
//
//		Range 13000 - 13099 for Toolbars
//
// ****************************************************************************
//
//		13000 - 13004 used by Photo Workshop resources, moved to
//							PhotoWorkshopResource.h
//



// ****************************************************************************
//
//		CURSORS: 
//
//		Range 13100 - 13199 for cursors
//
// ****************************************************************************
//
//		13100 - 13103 used by Photo Workshop resources, moved to
//							PhotoWorkshopResource.h
//



// ****************************************************************************
//
//		FLYOUT: Buttons
//
//		Range 13100 - 13199 for buttons
//
// ****************************************************************************
//
//		13100 - 13123 used by Photo Workshop resources, moved to
//							PhotoWorkshopResource.h
//


// ****************************************************************************
//
//		Dialogs: Component
//
//		Range 13000 - 13099 for dialogs, controls may be anything
//
// ****************************************************************************

#define DIALOG_SELECT_GRAPHIC									13000
#define	CONTROL_GRAPHIC_LIST									 1000

#define DIALOG_PSD_CLIPBOARD_VIEWER								13001
#define	CONTROL_STATIC_GRAPHIC_PREVIEW							 1004
#define	CONTROL_RADIO_PSD3_GRAPHIC								 1005
#define	CONTROL_RADIO_PSD5_GRAPHIC								 1006


// ****************************************************************************
//
//		Graphic Browser Import Image Controls
//
// ****************************************************************************

#define IDC_FILE_TREE												3001
#define IDC_FILE_TYPES												3002


// ****************************************************************************
//
//		Dialogs: Interface
//
//		Range 13100 - 13199 for dialogs, controls may be anything
//
// ****************************************************************************

#define DIALOG_IMPORTIMAGE_HELPID									13100
																					
#define DIALOG_IMPORT_IMAGE											13101
#define CONTROL_BUTTON_EDIT_IMAGE										1000
#define CONTROL_BUTTON_SHOW_PREVIEW										1001
#define CONTROL_STATIC_IMPORT_PREVIEW									1002
#define CONTROL_STATIC_IMPORT_FRAME										1003
																					
#define DIALOG_EXPORT_IMAGE_DATA									13102
#define CONTROL_EDIT_EXPORT_WIDTH										1000
#define CONTROL_EDIT_EXPORT_HEIGHT										1001
#define CONTROL_CHKBOX_EXPORT_ASPECT									1002
																					
#define DIALOG_EDIT_IMAGE											13103
#define CONTROL_SLIDER_BRIGHTNESS										1000
#define CONTROL_SLIDER_CONTRAST											1001
#define CONTROL_SLIDER_SHARPNESS										1002
#define CONTROL_STATIC_EDIT_PREVIEW										1003
#define CONTROL_STATIC_LIGHTEN											1004
#define CONTROL_STATIC_DARKEN											1005
#define CONTROL_STATIC_LOW_CONTRAST										1006
#define CONTROL_STATIC_HIGH_CONTRAST									1007
#define CONTROL_STATIC_SHARPEN											1008
#define CONTROL_STATIC_BLUR												1009
#define CONTROL_STATIC_FRAME1											1010
#define CONTROL_STATIC_FRAME2											1011
#define CONTROL_STATIC_CROP_TEXT										1012
																					
#define IDD_GRC_BROWSER												13104
#define		IDC_GRC_TAB													1000
#define		IDC_LOOK_IN													1001
#define		IDC_EXPORT													1002
#define		IDC_GALLERY_VIEW											1003
#define		IDC_LARGE_VIEW												1004
#define		IDC_LIST_VIEW												1005
#define		IDC_RESULTS_LB												1006
#define		IDC_IMPORT_LB												1007
#define		IDC_STATIC_PREVIEW										1008
#define		IDC_KEYWORD_PREVIEW										1009
#define		IDC_EDITIMAGE												1010
#define		IDC_STATIC_TOTAL_GRID_ITEMS							3001
#define		IDC_STATIC_SELECTED_NAME								3002
#define		IDC_VIEWMODE_LB											3003
#define		IDC_SORTORDER_LB											3004
																					
#define IDD_GRC_CATEGORY_TAB										13105
#define		IDC_CATEGORY_TREE											2000

#define IDD_GRC_KEYWORD_TAB										13106
#define		IDC_KEYWORD_EDIT											1000
#define		IDC_KEYWORD_CB												1001
#define		IDC_TYPES_CB												1002
#define		IDC_PROGRESS_BAR											1003
#define		IDC_NUMFOUND												1004
#define		IDC_START_SEARCH											1005
#define		IDC_SEARCH_OR												1006
#define		IDC_SEARCH_AND												1007
#define		IDC_EXPAND_SEARCH											1008

#define		IDC_SEARCH_NAME_RADIO									2000
#define		IDC_SEARCH_KEYWORD_RADIO								2001
#define		IDC_SEARCH_THEME_RADIO									2002
#define		IDC_SEARCH_NAME_EDIT										2003
#define		IDC_SEARCH_KEYWORD_EDIT									2004
#define		IDC_SEARCH_THEME_COMBO									2005

#define IDD_GRC_IMPORT_TAB											13107
// the control ids are in resource.h because the tab subclasses off a base class
// that references ids shared for leapfrog also - Lisa Wu


#define IDD_PMGC_BROWSER												13124
#define		IDC_PMGC_TAB													1000
#define		IDC_PMGC_LOOK_IN												1001
#define		IDC_PMGC_EXPORT												1002
#define		IDC_PMGC_GALLERY_VIEW										1003
#define		IDC_PMGC_LARGE_VIEW											1004
#define		IDC_PMGC_LIST_VIEW											1005
#define		IDC_PMGC_RESULTS_LB											1006
#define		IDC_PMGC_IMPORT_LB											1007
#define		IDC_PMGC_STATIC_PREVIEW										1008
#define		IDC_PMGC_KEYWORD_PREVIEW									1009
#define		IDC_PMGC_EDITIMAGE											1010
#define		IDC_PMGC_STATIC_TOTAL_GRID_ITEMS							3001
#define		IDC_PMGC_STATIC_SELECTED_NAME								3002
#define		IDC_PMGC_VIEWMODE_LB											3003
																					
#define IDD_PMGC_CATEGORY_TAB											13125
#define		IDC_PMGC_CATEGORY_TREE										2000

#define IDD_PMGC_KEYWORD_TAB											13126
#define		IDC_PMGC_KEYWORD_EDIT										1000
#define		IDC_PMGC_KEYWORD_CB											1001
#define		IDC_PMGC_TYPES_CB												1002
#define		IDC_PMGC_PROGRESS_BAR										1003
#define		IDC_PMGC_NUMFOUND												1004
#define		IDC_PMGC_START_SEARCH										1005
#define		IDC_PMGC_SEARCH_NAME_RADIO									2000
#define		IDC_PMGC_SEARCH_KEYWORD_RADIO								2001
#define		IDC_PMGC_SEARCH_THEME_RADIO								2002
#define		IDC_PMGC_SEARCH_NAME_EDIT									2003
#define		IDC_PMGC_SEARCH_KEYWORD_EDIT								2004
#define		IDC_PMGC_SEARCH_THEME_COMBO								2005

#define IDD_PMGC_IMPORT_TAB											13127
// the control ids are in resource.h because the tab subclasses off a base class
// that references ids shared for leapfrog also - Lisa Wu / Greg Beddow

																					
#define DIALOG_BACKDROP_BROWSER										13108
#define		CONTROL_TREE_BACKDROP_CATEGORIES							1000
#define		CONTROL_LIST_BACKDROP_PREVIEW								1001
#define		CONTROL_COMBOBOX_BACKDROP_TINT								1002
#define		CONTROL_RADIO_BACKDROP_GALLERY								1003
#define		CONTROL_RADIO_BACKDROP_LIST									1004
#define		CONTROL_RADIO_BACKDROP_VIEW									1005
#define		CONTROL_BUTTON_BACKDROP_COLOR								1006
#define		CONTROL_BUTTON_BACKDROP_EXPORT								1007
#define		CONTROL_LIST_BACKDROP_LIST									1008
#define		CONTROL_STATIC_BACKDROP_LOGO								1009
#define		CONTROL_COMBOBOX_BACKDROP_TYPE								1010

#define DIALOG_PSD4_LIBRARY_BROWSER									13109
#define		CONTROL_LIST_AVAILABLE_GRAPHICS								1001
#define		CONTROL_STATIC_GENERICGO_PREVIEW							2001  // Duplicate of common id, so
																						 // don't change it.
#define	IDD_BORDERS													13110
#define		IDC_BORDERS_NORMAL											1000
#define		IDC_BORDERS_WIDE											1001
#define		IDC_BORDERS_CATEGORY_TREE									1002
#define		IDC_BORDERS_GRID											1003
#define		IDC_BORDERS_STATIC_PREVIEW									1004
#define		IDC_BORDERS_GALLERY											1005
#define		IDC_BORDERS_LARGE											1006
#define		IDC_BORDERS_LIST											1007
#define		IDC_BORDERS_EXPORT											1008
#define		CONTROL_STATIC_TOTAL_GRID_ITEMS								3001
#define		CONTROL_STATIC_SELECTED_NAME								3002

#define	IDD_LINES													13111
#define		IDC_LINES_CATEGORY_TREE										1002
#define		IDC_LINES_GRID												1003
#define		IDC_LINES_STATIC_PREVIEW									1004
#define		IDC_LINES_GALLERY											1005
#define		IDC_LINES_LARGE												1006
#define		IDC_LINES_LIST												1007
#define		IDC_LINES_HORIZONTAL										2000
#define		IDC_LINES_VERTICAL											2001
#define		IDC_LINES_EXPORT											2002

//
//		13112 - 13119 used by Photo Workshop resources, moved to
//							PhotoWorkshopResource.h
//

#define DIALOG_FRAMES_BROWSER										13118
#define		CONTROL_TREE_FRAMES_CATEGORIES					 1000
#define		CONTROL_LIST_FRAMES_PREVIEW						 1001
#define		CONTROL_COMBO_FRAMES_MODE							 1003
#define		CONTROL_BUTTON_FRAMES_EXPORT						 1007
#define		CONTROL_LIST_FRAMES_LIST							 1008
#define		CONTROL_CHECK_FRAMES_PHOTOHOLDER					 1009
#define     CONTROL_BUTTON_FRAMES_COLOR						 1010

// ===========================================================================




// ****************************************************************************
//
//		Bitmap and Placeholder IDs: Component
//
//		Range 13000 - 13099
//
// ****************************************************************************

#define PLACEHOLDER_SQUARE_GRAPHIC									13000
#define PLACEHOLDER_COLUMN_GRAPHIC									13001
#define PLACEHOLDER_ROW_GRAPHIC										13002
#define PLACEHOLDER_RULED_LINE_GRAPHIC								13003
#define PLACEHOLDER_BORDER_GRAPHIC									13004
#define PLACEHOLDER_IMAGE_COMPONENT									13005

// ****************************************************************************
//
//		Icon IDs: Interface
//
//		Range 13100 - 13199
//
// ****************************************************************************
//
//		13110 - 13115 used by Photo Workshop resources, moved to
//							PhotoWorkshopResource.h
//


// ****************************************************************************
//
//		Bitmap IDs: Interface
//
//		Range 13100 - 13199
//
// ****************************************************************************

#define BITMAP_NO_IMAGE_PREVIEW_AVAILABLE							13100
//
//		13110 - 13129 used by Photo Workshop resources, moved to
//							PhotoWorkshopResource.h
//

// ****************************************************************************
//
//		String IDs: Component
//
//		Range 13000 - 13199
//
// ****************************************************************************

#define STRING_UNDO_EDIT_GRAPHIC										13000	
#define STRING_REDO_EDIT_GRAPHIC										13001	
#define STRING_UNDO_IMAGE_EFFECTS									13002
#define STRING_REDO_IMAGE_EFFECTS									13003

#define STRING_ERROR_IMAGE_INSUFFICIENT_MEMORY					13010
#define STRING_ERROR_IMAGE_UNSUPPORTED_FORMAT					13011
#define STRING_ERROR_IMAGE_UNSUPPORTED_COMPRESSION				13012
#define STRING_ERROR_IMAGE_FILE_NOT_ACCESSIBLE					13013
#define STRING_ERROR_IMAGE_INVALID_PATH							13014
#define STRING_ERROR_IMAGE_DISK_FULL								13015
#define STRING_ERROR_IMAGE_GENERIC									13016

#define STRING_ERROR_MIE_INSUFFICIENT_MEMORY						13020
#define STRING_ERROR_MIE_DISK_FULL									13021
#define STRING_ERROR_MIE_GENERIC										13022

// moved to PhotoWorkshopResource.h - still in use
//#define STRING_ERROR_CROP_AREA_TOO_SMALL							13023


// ****************************************************************************
//
//		String IDs: Interface
//
//		Range 13200 - 13399
//
// ****************************************************************************

#define STRING_BACKDROP_ALL_CATEGORIES								13200

#define STRING_IMPORT_IMAGE_FILE_FILTER 							13210
#define STRING_IMPORT_IMAGE_DIALOG_TITLE 							13211
#define STRING_EXPORT_IMAGE_FILE_FILTER 							13212
#define STRING_EXPORT_IMAGE_DIALOG_TITLE 							13213

#define STRING_BROWSER_CATEGORIES       							13220
#define STRING_BROWSER_KEYWORDS         							13221
#define STRING_BROWSER_START_SEARCH     							13222
#define STRING_BROWSER_STOP_SEARCH      							13223
#define STRING_BROWSER_IMPORT										13224

#define STRING_FILE_IMPORT_GRAPHIC_FILTER							13230
#define STRING_ERROR_BAD_LIBRARY									13231

#define STRING_FILTER_BMP											13240
#define STRING_FILTER_JPG											13241
#define STRING_FILTER_TIF											13242
#define STRING_FILTER_FPX											13243
#define STRING_FILTER_GIF											13244
#define	STRING_FILTER_WMF											13245

#define STRING_IMPORT_ALL_FILES										13250
#define STRING_IMPORT_BMP											13251
#define STRING_IMPORT_JPEG											13252
#define STRING_IMPORT_PCD               							13253
#define STRING_IMPORT_TIF               							13254
#define STRING_IMPORT_WMF               							13255
#define STRING_IMPORT_FPX               							13256
#define STIRNG_IMPORT_PNG               							13257

//
//		13260 - 13269 used by Photo Workshop resources, moved to
//							PhotoWorkshopResource.h
//

#endif // _GRAPHICCOMPRESOURCE_H_
