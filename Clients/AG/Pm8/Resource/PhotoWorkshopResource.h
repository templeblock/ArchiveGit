

#ifndef _PHOTOWORKSHOPRESOURCE_H_
#define _PHOTOWORKSHOPRESOURCE_H_

// ****************************************************************************
//
//		FLYOUT: Toolbar
//
//		Range 13000 - 13099 for Toolbars
//
// ****************************************************************************
#define TOOLBAR_REDEYE                  13000
#define TOOLBAR_PETEYE                  13001
#define TOOLBAR_DUST			        13002
#define TOOLBAR_SHINE	                13003
#define TOOLBAR_CUTOUT	                13004

// ****************************************************************************
//
//		CURSORS: 
//
//		Range 13100 - 13199 for cursors
//
// ****************************************************************************
#define CURSOR_FREEHAND						13100
#define IDC_FIXFLAW_CIRCLE30           13101
#define IDC_FIXFLAW_CIRCLE20           13102
#define IDC_FIXFLAW_CIRCLE10           13103


// ****************************************************************************
//
//		FLYOUT: Buttons
//
//		Range 13100 - 13199 for buttons
//
// ****************************************************************************
#define ID_REDEYE_LG					13100
#define ID_REDEYE_MED					13101
#define ID_REDEYE_SM					13102
#define ID_PETEYE_LG					13103
#define ID_PETEYE_MED					13104
#define ID_PETEYE_SM					13105
#define ID_DUST_CIR_LG                  13106
#define ID_DUST_CIR_MED                 13107
#define ID_DUST_CIR_SM                  13108
#define ID_DUST_OVAL_LG                 13109
#define ID_DUST_OVAL_MED                13110
#define ID_DUST_OVAL_SM                 13111
#define ID_SHINE_LG						13112
#define ID_SHINE_MED					13113
#define ID_SHINE_SM						13114
#define ID_CUTOUT_CIRCLE				13115
#define ID_CUTOUT_SQUARE				13116
#define ID_CUTOUT_HEART					13117
#define ID_CUTOUT_ARCH					13118
#define ID_CUTOUT_SQUARE_NOTCHED		13119
#define ID_CUTOUT_SQUARE_SLANT			13120
#define ID_CUTOUT_SQUARE_RND			13121
#define ID_CUTOUT_STAR					13122
#define ID_CUTOUT_TRIANGLE				13123

// ****************************************************************************
//
//		Dialogs: Interface
//
//		Range 13100 - 13199 for dialogs, controls may be anything
//
// ****************************************************************************

// the image edit tabbed dialog and each tab window resource
#define IDD_EDITIMAGE_TABBED										13112
#define		IDC_EDIT_TAB												1000
#define		IDC_EDIT_RESET												1001
#define		IDC_EDIT_SAVEAS												1002

#define IDD_EDIT_CROP_TAB											13113
#define		IDC_SHAPES_BTN												1000
#define		IDC_FREEHAND_BTN											1001
#define		IDC_CUTIT_BTN												1002
#define		IDC_ROTATE_BTN												1003
#define		IDC_REMOVE_CROP_BTN             							1004
#define		IDC_UNDO_BTN												2000
#define		IDC_ZOOMIN_BTN												2001
#define		IDC_ZOOMOUT_BTN												2002
#define		IDC_PREVIEW_AREA											2003

#define IDD_EDIT_ADJUSTCOLOR_TAB									13114
#define		IDC_FINE_RADIO												2004
#define		IDC_COARSE_RADIO               								2005
#define		IDC_CONVERTBW_BTN              								1001
#define		IDC_CUR_PREVIEW                								1002
#define		IDC_MORE_GRN                 								1003
#define		IDC_MORE_YEL               									1004
#define		IDC_MORE_RED                   								1005
#define		IDC_MORE_MAG	              								1006
#define		IDC_MORE_BLUE                  								1007
#define		IDC_MORE_CYAN                  								1008
#define		IDC_BEFORE_PREVIEW              							1009
#define		IDC_ADJUST_COLOR_BMP            							1010

#define IDD_EDIT_BRIGHTNESS_TAB										13115
#define		IDC_BRIGHTNESS_SLIDER										1000
#define		IDC_CONTRAST_SLIDER											1001
#define		IDC_BLURSHARP_SLIDER										1002
#define		IDC_BEFORE_AREA												1003
#define		IDC_AFTER_AREA												1004

#define IDD_EDIT_FIXFLAW_TAB										13116
#define		IDC_REDEYE_BTN												1000
#define		IDC_PETEYE_BTN												1001
#define		IDC_DUST_BTN												1002
#define		IDC_SHINE_BTN												1003

#define IDD_EDIT_ARTEFFECTS_TAB										13117
#define		IDC_EFFECTS_GRID											1000
#define		IDC_NO_ARTEFFECT											1001

// ****************************************************************************
//
//		Icon IDs: Interface
//
//		Range 13100 - 13199
//
// ****************************************************************************

#define IDI_INFOSYMBOL_GRAPHICCOMP									13100
#define IDI_IE_BRIGHT                   13110
#define IDI_EI_SHRP                     13111
#define IDI_EI_DARK                     13112
#define IDI_IE_HIGHCONTRAST             13113
#define IDI_IE_LOWCONTRAST              13114
#define IDI_EI_BLUR                     13115


// ****************************************************************************
//
//		Bitmap IDs: Interface
//
//		Range 13100 - 13199
//
// ****************************************************************************

#define IDB_ARTEFFECT_01                4500
#define IDB_ARTEFFECT_02                4501
#define IDB_ARTEFFECT_03                4502
#define IDB_ARTEFFECT_04                4503
#define IDB_ARTEFFECT_05                4504
#define IDB_ARTEFFECT_06                4505
#define IDB_ARTEFFECT_07                4506
#define IDB_ARTEFFECT_08                4507
#define IDB_ARTEFFECT_09                4508
#define IDB_ARTEFFECT_10                4509
#define IDB_ARTEFFECT_11                4510
#define IDB_ARTEFFECT_12                4511
#define IDB_ARTEFFECT_13                4512
#define IDB_ARTEFFECT_14                4513
#define IDB_ARTEFFECT_15                4514
#define IDB_ARTEFFECT_16                4515
#define IDB_ARTEFFECT_17                4516
#define IDB_ARTEFFECT_18                4517
#define IDB_ARTEFFECT_19                4518
#define IDB_ARTEFFECT_20                4519
#define IDB_ARTEFFECT_21                4520

#define IDB_PHOTOWORKSHOP_SHAPES_U										13110
#define IDB_PHOTOWORKSHOP_SHAPES_D										13111
#define IDB_PHOTOWORKSHOP_SHAPES_F										13112
#define IDB_PHOTOWORKSHOP_SHAPES_X										13113
#define IDB_PHOTOWORKSHOP_FREEHAND_U									13114
#define IDB_PHOTOWORKSHOP_FREEHAND_D									13115
#define IDB_PHOTOWORKSHOP_FREEHAND_F									13116
#define IDB_PHOTOWORKSHOP_FREEHAND_X									13117
#define IDB_PHOTOWORKSHOP_CROPIT_U										13118
#define IDB_PHOTOWORKSHOP_CROPIT_D										13119
#define IDB_PHOTOWORKSHOP_CROPIT_F										13120
#define IDB_PHOTOWORKSHOP_CROPIT_X										13121
#define IDB_PHOTOWORKSHOP_REMOVECROP_U									13122
#define IDB_PHOTOWORKSHOP_REMOVECROP_D									13123
#define IDB_PHOTOWORKSHOP_REMOVECROP_F									13124
#define IDB_PHOTOWORKSHOP_REMOVECROP_X									13125
#define IDB_PHOTOWORKSHOP_ROTATE_U										13126
#define IDB_PHOTOWORKSHOP_ROTATE_D										13127
#define IDB_PHOTOWORKSHOP_ROTATE_F										13128
#define IDB_PHOTOWORKSHOP_ROTATE_X										13129
#define IDB_PHOTOWORKSHOP_ZOOMIN_U										13130
#define IDB_PHOTOWORKSHOP_ZOOMIN_D										13131
#define IDB_PHOTOWORKSHOP_ZOOMIN_F										13132
#define IDB_PHOTOWORKSHOP_ZOOMIN_X										13133
#define IDB_PHOTOWORKSHOP_ZOOMOUT_U										13134
#define IDB_PHOTOWORKSHOP_ZOOMOUT_D										13135
#define IDB_PHOTOWORKSHOP_ZOOMOUT_F										13136
#define IDB_PHOTOWORKSHOP_ZOOMOUT_X										13137
#define IDB_PHOTOWORKSHOP_UNDO_U											13138
#define IDB_PHOTOWORKSHOP_UNDO_D											13139
#define IDB_PHOTOWORKSHOP_UNDO_F											13140
#define IDB_PHOTOWORKSHOP_UNDO_X											13141

#define IDB_PHOTOWORKSHOP_ADJUST_COLOR									13142
#define IDB_PHOTOWORKSHOP_PETEYE											13143
#define IDB_PHOTOWORKSHOP_REDEYE											13144
#define IDB_PHOTOWORKSHOP_DUST_SCRATCH									13145
#define IDB_PHOTOWORKSHOP_SHINE											13146
#define BITMAP_FORMAT_NOT_SUPPORTED										13147

// ****************************************************************************
//
//		String IDs
//
// ****************************************************************************

#define IDS_AE_01                       4500
#define IDS_AE_02                       4501
#define IDS_AE_03                       4502
#define IDS_AE_04                       4503
#define IDS_AE_05                       4504
#define IDS_AE_06                       4505
#define IDS_AE_07                       4506
#define IDS_AE_08                       4507
#define IDS_AE_09                       4508
#define IDS_AE_10                       4509
#define IDS_AE_11                       4510
#define IDS_AE_12                       4511
#define IDS_AE_13                       4512
#define IDS_AE_14                       4513
#define IDS_AE_15                       4514
#define IDS_AE_16                       4515
#define IDS_AE_17                       4516
#define IDS_AE_18                       4517
#define IDS_AE_19                       4518
#define IDS_AE_20                       4519
#define IDS_AE_21                       4520

#define STRING_ERROR_CROP_AREA_TOO_SMALL							13023

// strings for each of the tab in the tabbed image edit dialog
#define STRING_IED_CROP												13260
#define STRING_IED_ADJUSTCOLOR									13261
#define STRING_IED_BRIGHTFOCUS									13262
#define STRING_IED_FIXFLAW											13263
#define STRING_IED_ARTEFFECTS										13264

#define IDS_PHOTOWORKSHOP_MENU_CROP								13265
#define IDS_PHOTOWORKSHOP_MENU_ADJUSTCOLOR					13266
#define IDS_PHOTOWORKSHOP_MENU_BRIGHTFOCUS					13267
#define IDS_PHOTOWORKSHOP_MENU_FIXFLAW							13268
#define IDS_PHOTOWORKSHOP_MENU_ARTEFFECTS						13269


#endif // _PHOTOWORKSHOPRESOURCE_H_
