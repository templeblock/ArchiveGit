#ifndef PRINT_MAIN_H
#define PRINT_MAIN_H


/************************************************************************************
*  Define MBAR IDs                                                                  *
**************************************************************************************/
#define MENUBAR					1800

/************************************************************************************
*  Define MENUS                                                                  *
**************************************************************************************/
#define APPLE_MENU		128
#define FILE_MENU		1801
#define EDIT_MENU		1804


// Define MENU ITEMS for APPLE_MENU
#define ABOUT_ITEM			1

// Define MENU ITEMS for FILE_MENU
#define NEW_ITEM			1
#define CLOSE_ITEM			3
#define QUIT_ITEM			11

#define OPEN_PROFILE_ITEM   2
#define NEW_PROFILE_ITEM	1
#define DISPLAY_ITEM		3
#define CLOSE_PROFILE_ITEM  4
#define SAVE_PATCH_DATA		6
#define OPEN_G_ITEM			8
#define CLOSE_G_ITEM		9
#define PRNT_PATCHES		-1

// edit menu 

#define EDIT_UNDO			1
#define EDIT_CUT			3
#define EDIT_COPY			4
#define EDIT_PASTE			5
#define EDIT_PREF			-1

// device menu

#define DEVICES_MENU		1805
#define DEVICE_MENU			130
#define PORT_MENU			131

#define DEVICE_NONE			1
#define DEVICE_DTP51		2
#define DEVICE_TECH_CP300	3
#define DEVICE_TECH_SP820	4
#define DEVICE_TECH_TCR_CP300	5
#define DEVICE_TECH_TCR_SP820	6
#define DEVICE_GRE_HAND		7
#define DEVICE_GRE_TABLE	8
/************************************************************************************
*  Define constants                                                                 *
**************************************************************************************/
#define NIL_PTR					0L
#define IN_FRONT				(WindowPtr)-1L
#define SLEEP_TICKS				2L
#define MOUSE_REGION			0L
#define NIL						0
#define REMOVE_EVENTS			0

#endif //NEW_BATCH_H
