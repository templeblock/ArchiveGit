// printmain_p.h
// The menu definitions for the polaroid project

#ifndef PRINT_MAIN_P_H
#define PRINT_MAIN_P_H


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
#define CLOSE_ITEM			4
#define QUIT_ITEM			12

#define OPEN_PROFILE_ITEM   2
#define NEW_PROFILE_ITEM	1
#define DISPLAY_ITEM		-1
#define CLOSE_PROFILE_ITEM  4
#define SAVE_PROFILE		6
#define SAVE_PROFILE_AS		7
#define REVERT_PROFILE		8
#define OPEN_G_ITEM			10


// edit menu 

#define EDIT_UNDO			1
#define EDIT_CUT			3
#define EDIT_COPY			4
#define EDIT_PASTE			5
#define EDIT_PREF			7

// device menu

#define DEVICES_MENU		1805
#define DEVICE_MENU			130
#define PORT_MENU			131

#define DEVICE_NONE			1
#define DEVICE_DTP51		2
#define DEVICE_DTP41		3
#define DEVICE_XRITE408		4
#define DEVICE_TECH_CP300	5
#define DEVICE_TECH_SP820	6
#define DEVICE_TECH_TCR_CP300	7
#define DEVICE_TECH_TCR_SP820	8
#define DEVICE_GRE_HAND		9
#define DEVICE_GRE_TABLE	10
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
