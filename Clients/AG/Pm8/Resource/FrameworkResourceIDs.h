// ****************************************************************************
//
//  File Name:			FrameworkResourceIDs.h
//
//  Project:			Renaissance Framework
//
//  Author:				S. Athanas
//
//  Description:		Common command ids.
//
//  Portability:		Platform independent
//
//  Developed by:		Turning Point Software.
//							One Gateway Center, Suite 800
//							Newton, MA 02158
//							(617) 332-0202
//
//  Client:				Broderbund Software, Inc.         
//
//  Copyright (C) 1995-1996 Turning Point Software. All Rights Reserved.
//
//  $Logfile:: /PM8/Resource/FrameworkResourceIDs.h                           $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:28p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#ifndef	_FRAMEWORKRESOURCEIDS_H_
#define	_FRAMEWORKRESOURCEIDS_H_

// ****************************************************************************
//
//		Resource ID range index :
//
//		Dialog IDs										10000 - 10099
//
//		Cursor IDs										10000 - 10099
//
//      Application provided resourcs:
//			Bitmap										10000 - 10099
//			String										10000 -	10099
//          Misc.										10000 - 10999
//
//		String IDs:
//			Localization								10100 - 10149
//			Alert										10150 - 10199
//			Undo/Redo									10200 - 10299
//			Error										10300 -	10499
//			Misc.										10500 -	10899
//			Menu										10900 - 10999
//
//		Menu Command IDs
//			File										 8000 -  8049
//			Edit										 8100 -  8149
//			View										 8200 -  8249
//			Insert										 8300 -  8349
//			Effects										 8400 -  8449
//			Debug										 8900 -  8949
//			Non-standard								10000 - 10999
//			Settings									11000 - 14999
//			Context										18000 - 18499
//
//		Components (For information purposes only): 
//			Graphic/Image							13000 - 13999
//			Headline									14000 - 14499
//			Text										15000 - 15999
//
// ****************************************************************************


// ****************************************************************************
//
//		Dialogs
//
//		Range 10000 - 10099 for dialogs, controls may be anything
//
// ****************************************************************************
#define DIALOG_EDIT_COMPONENT_ATTRIBUTES						10000
#define CONTROL_SELECTABLE										 1000
#define CONTROL_MOVABLE											 1001
#define CONTROL_RESIZABLE										 1002
#define CONTROL_ROTATABLE										 1003
#define CONTROL_ZLAYER											 1004

#define DIALOG_FRAMES											10001
#define CONTROL_STATIC_FRAME_GROUP_BOX							 1000
#define CONTROL_BUTTON_THIN_FRAME								 1001
#define CONTROL_BUTTON_MEDIUM_FRAME								 1002
#define CONTROL_BUTTON_THICK_FRAME								 1003
#define CONTROL_BUTTON_DBL_THIN_FRAME							 1004
#define CONTROL_BUTTON_DBL_MEDIUM_FRAME							 1005
#define CONTROL_BUTTON_DBL_THICK_FRAME							 1006
#define CONTROL_BUTTON_ROUND_CORNER_FRAME						 1007
#define CONTROL_BUTTON_PICTURE_FRAME							 1008
#define CONTROL_BUTTON_DROP_SHADOW_FRAME						 1009
#define CONTROL_BUTTON_REMOVE_FRAME								 1010
#define CONTROL_STATIC_FRAME_DISPLAY							 1011
#define CONTROL_BUTTON_COLOR_FRAME                       		 1022 

#define DIALOG_SCALE											10002
#define CONTROL_STATIC_SCALE									 1002
#define CONTROL_SCALE_SPIN										 1003
#define CONTROL_EDIT_SCALE										 1004
#define CONTROL_STATIC_SCALE_DOTS								 1005
#define CONTROL_RADIO_SCALE_ALL_DIRECTIONS						 1006
#define CONTROL_RADIO_SCALE_HORIZONTAL_ONLY						 1007
#define CONTROL_RADIO_SCALE_VERTICAL_ONLY						 1008
#define CONTROL_SLIDER_SCALE									 1009

#define DIALOG_ALIGN											10003
#define CONTROL_RADIO_ALIGN_HORZ_NONE							 1001
#define CONTROL_RADIO_ALIGN_HORZ_LEFT							 1002
#define CONTROL_RADIO_ALIGN_HORZ_CENTER							 1003
#define CONTROL_RADIO_ALIGN_HORZ_RIGHT							 1004
#define CONTROL_RADIO_ALIGN_VERT_NONE							 1005
#define CONTROL_RADIO_ALIGN_VERT_TOP							 1006
#define CONTROL_RADIO_ALIGN_VERT_CENTER							 1007
#define CONTROL_RADIO_ALIGN_VERT_BOTTOM							 1008
#define CONTROL_STATIC_ALIGN									 1009
																
#define DIALOG_ROTATE											10004
#define CONTROL_EDIT_ROTATE										 1001
#define CONTROL_SPIN_ROTATE										 1002
#define CONTROL_STATIC_ROTATE									 1003
#define CONTROL_DIAL_ROTATE										 1004
#define CONTROL_STATIC_ROTATE_PLACEHOLDER						 1005

#define DIALOG_SPELL_CHECK										10005
#define CONTROL_GROUPBOX_SPELL_CONTEXT							 1001
#define CONTROL_EDIT_SPELL_CONTEXT								 1002
#define CONTROL_STATIC_SPELL_REPLACE_WITH						 1003
#define CONTROL_EDIT_SPELL_REPLACE_WITH							 1004
#define CONTROL_LISTBOX_SPELL_ALTERNATIVES						 1005
#define CONTROL_STATIC_SPELL_NOT_FOUND							 1006
#define CONTROL_EDIT_SPELL_NOT_FOUND							 1007
#define CONTROL_BUTTON_SPELL_ADD_TO_DICTIONARY					 1008
#define CONTROL_EDIT_SPELL_IGNORE								 1009
#define CONTROL_EDIT_SPELL_IGNORE_ALL							 1010
#define CONTROL_EDIT_SPELL_REPLACE								 1011
#define CONTROL_EDIT_SPELL_REPLACE_ALL							 1012
#define CONTROL_EDIT_SPELL_SUGGEST								 1013
#define CONTROL_STATIC_SPELL_ALTERNATIVES						 1014
#define CONTROL_GROUPBOX_SPELL_CHECK							 1015

#define DIALOG_MEM_FAIL											10006
#define DIALOG_FILE_FAIL										 1000
#define IDC_E_FAIL_TIME											 1001
#define IDC_E_NUM_FAILURES										 1002
#define IDC_RD_NEVER_FAIL										 1003
#define IDC_RD_FAIL_AFTER										 1004
#define IDC_S_ALLOCATIONS										 1005
#define IDC_S_NUM_FAILS											 1006
#define IDC_S_FAIL												 1007

#define DIALOG_THESAURUS										10007
#define CONTROL_GROUPBOX_THESAURUS								 1000
#define CONTROL_STATIC_THESAURUS_SELECTED_WORD					 1001
#define CONTROL_STATIC_THESAURUS_SYNONYMS						 1002
#define CONTROL_EDIT_THESAURUS_SELECTED_WORD					 1003
#define CONTROL_LISTBOX_THESAURUS_SYNONYMS						 1004
#define CONTROL_GROUPBOX_THESAURUS_DEFINITION					 1005
#define CONTROL_LISTBOX_THESAURUS_DEFINITIONS					 1006
#define CONTROL_EDIT_THESAURUS_REPLACE							 1007
#define CONTROL_EDIT_THESAURUS_DEFINE							 1008
#define CONTROL_STATIC_THESAURUS_SELECTED_WORD2					 1009

#define DIALOG_COLOR_DIALOG										10008
#define CONTROL_COLOR_MRU1											 2000
#define CONTROL_COLOR_MRU2                    				 2001
#define CONTROL_COLOR_MRU3                    				 2002
#define CONTROL_COLOR_MRU4                    				 2003
#define CONTROL_COLOR_MRU5                    				 2004
#define CONTROL_COLOR_MRU6                    				 2005
#define CONTROL_COLOR_NONE                    				 2006
#define CONTROL_COLOR_PALETTE										 2007
#define CONTROL_COLOR_MORE											 2008

#define DIALOG_COLOR_DIALOG_EX									10009
#define CONTROL_STATIC_COLOR_ANGLE								1008
#define CONTROL_RADIO_COLOR_LINEAR								1009
#define CONTROL_RADIO_COLOR_INVERSE_LINEAR					1010
#define CONTROL_RADIO_COLOR_LINEAR2								1011
#define CONTROL_RADIO_COLOR_INVERSE_LINEAR2					1012
#define CONTROL_RADIO_COLOR_CIRCLEBURST						1013
#define CONTROL_RADIO_COLOR_INVERSE_CIRCLEBURST				1014
#define CONTROL_RADIO_COLOR_SQAUREBURST						1015
#define CONTROL_RADIO_COLOR_INVERSE_SQAUREBURST				1016
#define CONTROL_RADIO_COLOR_GRADIENT1							1017
#define CONTROL_RADIO_COLOR_GRADIENT2							1018
#define CONTROL_RADIO_COLOR_GRADIENT3							1019
#define CONTROL_RADIO_COLOR_GRADIENT4							1020
#define CONTROL_RADIO_COLOR_GRADIENT5							1021
#define CONTROL_RADIO_COLOR_GRADIENT6							1022
#define CONTROL_RADIO_COLOR_GRADIENT7							1023
#define CONTROL_RADIO_COLOR_GRADIENT8							1024
#define CONTROL_RADIO_COLOR_GRADIENT9							1025
#define CONTROL_RADIO_COLOR_GRADIENT10							1026
#define CONTROL_RADIO_COLOR_GRADIENT11							1027

#define BITMAP_BUTTON_TINT_UP										1746
#define BITMAP_BUTTON_TINT_DOWN									1747
#define BITMAP_BUTTON_TINT_DISABLED								1748

#define BITMAP_BUTTON_COLOR_UP									1749
#define BITMAP_BUTTON_COLOR_DOWN									1750
#define BITMAP_BUTTON_COLOR_DISABLED							1751

#define DIALOG_PRINT_STATUS										10011
#define CONTROL_STATIC_STATUS_PRINTER							 1001
#define CONTROL_STATIC_STATUS_PRINTING							 1002
#define CONTROL_STATIC_STATUS_PAGE								 1003
#define CONTROL_STATIC_STATUS_OF								 1004
#define CONTROL_EDIT_STATUS_PRINTER								 1005
#define CONTROL_EDIT_STATUS_PRINTING							 1006
#define CONTROL_EDIT_STATUS_CURRENTPAGE							 1007
#define CONTROL_EDIT_STATUS_TOTALPAGE							 1008
#define CONTROL_PROGRESS_STATUS_INDICATOR						 1009

//
//	Temporary ids for enter test page results dialog
//
#define DIALOG_DEBUG_TESTPAGERESULTS								10012
#define DIALOG_DEBUG_TESTPAGERESULTS_LEFT_CALIBRATION				 1000
#define DIALOG_DEBUG_TESTPAGERESULTS_TOP_CALIBRATION				 1001
#define DIALOG_DEBUG_TESTPAGERESULTS_BOTTOM_CALIBRATION				 1002
#define DIALOG_DEBUG_TESTPAGERESULTS_RIGHT_CALIBRATION				 1003
#define DIALOG_DEBUG_TESTPAGERESULTS_VERTICAL_CALIBRATION			 1004
#define DIALOG_DEBUG_TESTPAGERESULTS_HORIZONTAL_CALIBRATION			 1005
#define DIALOG_DEBUG_TESTPAGERESULTS_LEFT_CALIBRATION_LABEL			 1006
#define DIALOG_DEBUG_TESTPAGERESULTS_TOP_CALIBRATION_LABEL			 1007
#define DIALOG_DEBUG_TESTPAGERESULTS_BOTTOM_CALIBRATION_LABEL		 1008
#define DIALOG_DEBUG_TESTPAGERESULTS_RIGHT_CALIBRATION_LABEL		 1009
#define DIALOG_DEBUG_TESTPAGERESULTS_VERTICAL_CALIBRATION_LABEL		 1010
#define DIALOG_DEBUG_TESTPAGERESULTS_HORIZONTAL_CALIBRATION_LABEL	 1011
#define DIALOG_DEBUG_TESTPAGERESULTS_SIDE_CALIBRATION_LABEL			 1012
#define DIALOG_DEBUG_TESTPAGERESULTS_CENTER_CALIBRATION_LABEL_1		 1013
#define DIALOG_DEBUG_TESTPAGERESULTS_CENTER_CALIBRATION_LABEL_2		 1014
#define DIALOG_DEBUG_TESTPAGERESULTS_TEST1_GROUPBOX					 1015
#define DIALOG_DEBUG_TESTPAGERESULTS_TEST2_GROUPBOX					 1016
#define DIALOG_DEBUG_TESTPAGERESULTS_TEST1_LABEL					 1017
#define DIALOG_DEBUG_TESTPAGERESULTS_TEST2_LABEL					 1018
#define DIALOG_DEBUG_TESTPAGERESULTS_TRUNCATION_TEST_ABC			 1019
#define DIALOG_DEBUG_TESTPAGERESULTS_TRUNCATION_TEST_ABCD			 1020
#define DIALOG_DEBUG_TESTPAGERESULTS_TRANSPARENCY_TEST_CARD			 1021
#define DIALOG_DEBUG_TESTPAGERESULTS_TRANSPARENCY_TEST_CARDS		 1022
#define DIALOG_DEBUG_TESTPAGERESULTS_RIGHT_CALIBRATION_SPIN			 1023
#define DIALOG_DEBUG_TESTPAGERESULTS_TOP_CALIBRATION_SPIN			 1024
#define DIALOG_DEBUG_TESTPAGERESULTS_LEFT_CALIBRATION_SPIN			 1025
#define DIALOG_DEBUG_TESTPAGERESULTS_BOTTOM_CALIBRATION_SPIN		 1026
#define DIALOG_DEBUG_TESTPAGERESULTS_HORIZONTAL_CALIBRATION_SPIN	 1027
#define DIALOG_DEBUG_TESTPAGERESULTS_VERTICAL_CALIBRATION_SPIN	 	 1028

#define DIALOG_TESTPAGERESULTS										10013

#define IDD_COLOR_DIALOG												10014		
#define IDD_COLOR_DIALOG_EX											10015

#define IDD_COLOR_PALETTE												10016
#define CONTROL_GROUPBOX_PALETTE_AREA							    1001
#define CONTROL_BUTTON_COLOR_BLEND								    1002
#define CONTROL_STATIC_COLOR_EX									    1003
#define CONTROL_STATIC_PRINTSHOP_SPLASH							 1004

#define DIALOG_CD_SWAP													10020
#define DIALOG_CD_SWAP_INSTALL										10021
#define DIALOG_CD_SWAP_GB												10022

#define IDD_FLYOUT														10030

#define IDD_FOLDER_DIALOG												10031
#define IDC_FD_FOLDER                   							 1001
#define IDC_FD_CANCEL               								 1002
#define IDC_FD_HTML														 1003
#define IDC_FD_HTM														 1004
#define IDC_FD_PAGE_SIZE							                1005

#define IDD_ADDEDIT_HYPERLINK											10032
#define IDC_AH_WEB_LINK														 1001
#define IDC_AH_EMAIL_LINK													 1002
#define IDC_AH_FILE_LINK													 1003
#define IDC_AH_SOUNDFILE_LINK												 1004
#define IDC_AH_LINK_STATIC_TEXT											 1005
#define IDC_AH_FILE_NAME													 1006
#define IDC_AH_URL															 1007
#define IDC_AH_BROWSE														 1008
#define IDC_AH_REMOVE_LINK													 1009


#define DIALOG_ADDPAGE_TYPE             10033
#define CONTROL_ADDPAGE_TYPE_STRING1    1000
#define CONTROL_ADDPAGE_TYPE_STRING2    1001
#define IDC_BLANK_RADIO                 1002
#define IDC_DUPEDOC_RADIO               1003
#define IDC_QSL_RADIO                   1004

#define DIALOG_ADDPAGE_CONFIRM          10034
#define CONTROL_ADDPAGE_CONF_STRING1    1000
#define CONTROL_ADDPAGE_CONF_STRING2    1001
#define CONTROL_ADDPAGE_CONF_STRING3    1002
#define CONTROL_ADDPAGE_CONF_STRING4    1003
#define IDC_CONF_EDIT_PAGE			    1004
#define IDC_CONF_SPIN_PAGE				1005
#define IDC_CONFPAGENAME_EDIT			1006

#define DIALOG_ADDPAGE_DUPEDOC          10035
#define CONTROL_ADDPAGE_DUPE_STRING1    1000
#define CONTROL_ADDPAGE_DUPE_STRING2    1001
#define CONTROL_ADDPAGE_DUPE_STRING3    1002
#define CONTROL_ADDPAGE_DUPE_STRING4    1003
#define CONTROL_ADDPAGE_DUPE_STRING5    1004
#define IDC_EDIT_PAGENUM                1005
#define IDC_SPIN_PAGENUM                1006
#define IDC_DUPEDOC_EDIT_NUMPAGES       1007
#define IDC_DUPEDOC_SPIN_NUMPAGES       1008
#define IDC_PAGENAME_COMBO              1009

#define DIALOG_ADDPAGE_QSL              10036
#define CONTROL_ADDPAGE_QSL_STRING1     1000
#define CONTROL_ADDPAGE_QSL_STRING2     1001
#define IDC_QSL_GRID                    1002

#define DIALOG_ADDPAGE_PAGENAME         10037
#define CONTROL_ADDPAGE_NAME_STRING1    1000
#define CONTROL_ADDPAGE_NAME_STRING2    1001
#define IDC_PAGENAME_EDIT               1002


//
// The following dialog ids do not refer to a resource, rather they are
// used to allow distinct help ids for common dialogs. -MWH
#define DIALOG_SAVEAS_HELPID														1607
#define	DIALOG_EXPORTHTML_HELPID													1609
// ****************************************************************************
//
//		Cursor IDs
//
//		Range 10000 - 10099
//
// ****************************************************************************

#define CURSOR_RESOURCE_MOVE									10000
#define CURSOR_RESOURCE_ROTATE								10001
#define CURSOR_RESOURCE_COPY									10002
#define CURSOR_RESOURCE_SLASH									10003

// ****************************************************************************
//
//		Application provided resource IDs
//
//		Range 10000 - 10099
//
// ****************************************************************************
																			
#define PALETTE_APPLICATION_256									10000

// ****************************************************************************
//
//		Application provided bitmap IDs
//
//		Range 10000 - 10099
//
// ****************************************************************************

#define BITMAP_APPLICATION_LOGO										10000	 
#define BITMAP_SPLASH_SCREEN											10002
#define BITMAP_COLOR_PALETTE											10003

// ****************************************************************************
//
//		Application provided string resource IDs
//
//		Range 10000 - 10099
//
// ****************************************************************************

#define STRING_OPEN_DEFAULT_PATH										10000
#define STRING_TEXT_COMPONENT_DEFAULT_FONT						10001
#define STRING_HTML_DIRECTORY_NAME								10002


// ****************************************************************************
//
//		Miscellaneous resources for localization
//
//		Range 10100 - 10149
//
// ****************************************************************************

#define STRING_LOCAL_UNIT_TO_INCHES_CONVERSION					10100
#define STRING_FIRST_DAY_IN_CALENDAR_GRID							10101

// ****************************************************************************
//
//		Alert String IDs
//
//		Range 10150 - 10199
//
// ****************************************************************************

#define STRING_ALERT_REVERT										10150
#define STRING_ALERT_FILE_OVERRIGHT								10151
#define STRING_ALERT_PRINT_JOBS_REMAINING						10152
#define STRING_ALERT_OBJECT_CONVERTED							10153

// ****************************************************************************
//
//		Undo/Redo String IDs
//
//		Range 10200 - 10299
//
// ****************************************************************************

#define STRING_CANT_UNDO												10200
#define STRING_CANT_REDO												10201
#define STRING_UNDO_MOVE_COMPONENT									10202
#define STRING_REDO_MOVE_COMPONENT									10203
#define STRING_UNDO_DELETE												10204
#define STRING_REDO_DELETE												10205
#define STRING_UNDO_ROTATE_SELECTION								10206
#define STRING_REDO_ROTATE_SELECTION								10207
#define STRING_UNDO_NEW_COMPONENT									10208
#define STRING_REDO_NEW_COMPONENT									10209
#define STRING_UNDO_GROUP_SELECTION									10210
#define STRING_REDO_GROUP_SELECTION									10211
#define STRING_UNDO_BREAKGROUP										10212
#define STRING_REDO_BREAKGROUP										10213
#define STRING_UNDO_RESIZE_SELECTION								10214
#define STRING_REDO_RESIZE_SELECTION								10215
#define STRING_UNDO_EDIT_COMPONENT_ATTRIBUTES					10216
#define STRING_REDO_EDIT_COMPONENT_ATTRIBUTES					10217
#define STRING_UNDO_PASTE												10218
#define STRING_REDO_PASTE												10219
#define STRING_UNDO_CHANGE_FRAME										10220
#define STRING_REDO_CHANGE_FRAME										10221
#define STRING_UNDO_LOCK												10222
#define STRING_REDO_LOCK												10223
#define STRING_UNDO_UNLOCK												10224
#define STRING_REDO_UNLOCK												10225
#define STRING_UNDO_CHANGE_TINT										10226
#define STRING_REDO_CHANGE_TINT										10227
#define STRING_UNDO_CHANGE_COLOR										10228
#define STRING_REDO_CHANGE_COLOR										10229
#define STRING_UNDO_ALIGN												10230
#define STRING_REDO_ALIGN												10231
#define STRING_UNDO_FLIP_HORIZONTAL									10232
#define STRING_REDO_FLIP_HORIZONTAL									10233
#define STRING_UNDO_FLIP_VERTICAL									10234
#define STRING_REDO_FLIP_VERTICAL									10235
#define STRING_UNDO_FLIP_BOTH											10236
#define STRING_REDO_FLIP_BOTH											10237
#define STRING_UNDO_CHANGE_GRADIENT									10238
#define STRING_REDO_CHANGE_GRADIENT									10239
#define STRING_UNDO_CHANGE_FRAME_COLOR								10240
#define STRING_REDO_CHANGE_FRAME_COLOR								10241
#define STRING_UNDO_ADD_SHADOW										10242
#define STRING_REDO_ADD_SHADOW										10243
#define STRING_UNDO_ADD_GLOW_ON_OFF									10244
#define STRING_REDO_ADD_GLOW_ON_OFF									10245
#define STRING_UNDO_ADD_VIGNETTE_ON_OFF							10246
#define STRING_REDO_ADD_VIGNETTE_ON_OFF							10247
#define STRING_UNDO_BRING_FORWARD									10248
#define STRING_REDO_BRING_FORWARD									10249
#define STRING_UNDO_SEND_BACKWARD									10250
#define STRING_REDO_SEND_BACKWARD									10251
#define STRING_UNDO_BRING_TOFRONT									10252
#define STRING_REDO_BRING_TOFRONT									10253
#define STRING_UNDO_SEND_TOBACK										10254
#define STRING_REDO_SEND_TOBACK										10255
#define STRING_UNDO_AQUIRE_IMAGE										10256
#define STRING_REDO_AQUIRE_IMAGE										10257
#define STRING_UNDO_EDIT_COMPONENT									10258
#define STRING_REDO_EDIT_COMPONENT									10259
#define STRING_UNDO_REPLACETEXT										10260
#define STRING_REDO_REPLACETEXT										10261
#define STRING_UNDO_ADD_EDGEOUTLINE_ON_OFF						10262
#define STRING_REDO_ADD_EDGEOUTLINE_ON_OFF						10263
#define STRING_UNDO_TRANSPARENT_EFFECT								10264
#define STRING_REDO_TRANSPARENT_EFFECT								10265
#define STRING_UNDO_CHANGE_SPECIAL_EFFECT							10266
#define STRING_REDO_CHANGE_SPECIAL_EFFECT							10267
#define STRING_UNDO_REMOVE_SPECIAL_EFFECT							10268
#define STRING_REDO_REMOVE_SPECIAL_EFFECT							10269
#define STRING_UNDO_ADD_URL											10270
#define STRING_REDO_ADD_URL											10271
#define STRING_UNDO_CHANGE_URL										10272
#define STRING_REDO_CHANGE_URL										10273



// ****************************************************************************
//
//		Error String IDs
//
//		Range 10300 - 10499
//
// ****************************************************************************

#define STRING_ERROR_NO_ERROR									10300
#define STRING_ERROR_FILE_NOT_FOUND								10301
#define STRING_ERROR_INVALID_PATH								10302
#define STRING_ERROR_END_OF_FILE								10303
#define STRING_ERROR_TOO_MANY_OPEN_FILES						10304
#define STRING_ERROR_ACCESS_DENIED								10305
#define STRING_ERROR_DIRECTORY_FULL								10306
#define STRING_ERROR_BAD_SEEK									10307
#define STRING_ERROR_DISK_HARDWARE								10308
#define STRING_ERROR_SHARE_VIOLATION							10309
#define STRING_ERROR_LOCK_VIOLATION								10310
#define STRING_ERROR_RESOURCE									10311
#define STRING_ERROR_UNKNOWN_ERROR								10312
#define STRING_ERROR_DATA_FORMAT_INVALID						10313
#define STRING_ERROR_DATA_FORMAT_TOO_OLD						10314
#define STRING_ERROR_ACCUSOFT_ERROR								10315   // Not used in framework
#define STRING_ERROR_MEMORY										10316
#define STRING_ERROR_DISK_FULL									10317
#define STRING_ERROR_GENERIC_ERROR								10318
#define STRING_ERROR_GETTING_CLIPBOARD_DATA						10319
#define STRING_FILE_IN_USE_ERROR								10320   // Not used in framework
#define STRING_FILE_MOVED_ERROR									10321
#define STRING_ERROR_INVALID_FILE								10322
#define STRING_ERROR_STRETCHBLT									10323   // Not used in framework
#define STRING_ERROR_FILE_NOT_OPENED							10324   // Not used in framework
#define STRING_FILE_OPEN_FAILED_ERROR							10325
#define STRING_ERROR_ATMVERSION_INVALID							10326
#define STRING_ERROR_CANNOT_PASTE_DATA							10327
#define STRING_ERROR_WRITE_PROTECT								10328
#define STRING_ERROR_PRINTER_UNAVAILABLE						10329
#define STRING_ERROR_PRINTER_COULD_NOT_MODIFY_PROPERTIES		10330
#define STRING_ERROR_SWAPFILE_FULL								10331
#define STRING_ERROR_DRIVE_NOT_READY							10332

#define STRING_ERROR_ACC_OUT_OF_MEMORY							10333   // Not used in framework
#define STRING_ERROR_ACC_FILE_NOT_FOUND							10334   // Not used in framework
#define STRING_ERROR_ACC_FILE_CORRUPT							10335   // Not used in framework
#define STRING_ERROR_ACC_FILE_SAVE								10336   // Not used in framework
#define STRING_ERROR_ACC_FILE_DISK_FULL							10337   // Not used in framework
#define STRING_ERROR_ACC_FILE_DISK_READ							10338   // Not used in framework
#define STRING_ERROR_ACC_DLL_NOT_LOADED							10339   // Not used in framework
#define STRING_ERROR_ACC_UNKNOWN								10340   // Not used in framework

#define STRING_ERROR_DOCS_TESTPAGE_HOR_CALIBRATION_INVALID		10341
#define STRING_ERROR_DOCS_TESTPAGE_VERT_CALIBRATION_INVALID		10342

//  The following two defines are placeholders for a range of string ID's
//  defined in twainWrapRC.h.  IDS_BTM_BASE_ID in that file should match
//  STRING_ERROR_TWAIN_BASE_ID_START number.
//#define STRING_ERROR_TWAIN_BASE_ID_START						6150  
//#define STRING_ERROR_TWAIN_BASE_ID_END						6199   

#define STRING_ERROR_GENERAL_COMPONENT_MISSING					10345  
#define STRING_ERROR_GENERAL_STARTUP							10346
#define STRING_ERROR_LOAD_COMPONENT_MISSING						10347
#define STRING_ERROR_FILE_ALREADY_OPEN							10348
#define STRING_ERROR_FILE_ALREADY_OPEN_REVERT				10349

// external image file referenced by a collection or QSL-save-by-ref not found
#define STRING_ERROR_OPENIMGERR_FILENOTFOUND						10351

// no partial file path associated with an image in a QSL being saved
// can happen if a 5.x QSL is resaved without double-clicking each image
// into the browser to establish its partial file path
#define STRING_ERROR_SAVE_AS_READYMADE_NO_PARTIAL_PATH		10352

// Hyperlink creation errors.
#define STRING_ERROR_FILENAME_CHARS_INVALID						10353
#define STRING_ERROR_EMAIL_ADDRESS_INVALID						10354
#define STRING_ERROR_FILE_SPECIFIED_NOT_FOUND					10355
#define STRING_ERROR_FILENAME_CHARS_INVALID_URL					10356
#define STRING_ERROR_FILENAME_CHARS_INVALID_EMAIL				10357

// Copy of Linked file errors.
#define STRING_URL_FILE_NOT_FOUND									10358
#define STRING_URL_FILE_COPY_PROBLEM								10359

// ****************************************************************************
//
//		Miscellaneous String Resource IDs
//
//		Range 10500 - 10899
//
// ****************************************************************************

#define STRING_FILETYPE_ALL_DESCRIPTION							10500
#define STRING_FILETYPE_ALL_EXTENSION							10501
#define STRING_MENU_LOCK										10502
#define STRING_MENU_LOCKALL										10503
#define STRING_MENU_UNLOCK										10504
#define STRING_MENU_UNLOCKALL									10505
#define STRING_MENU_GROUP										10506
#define STRING_MENU_UNGROUP									10507

#define STRING_ERROR_CS_ICALLOCERR								10600
#define STRING_ERROR_CS_ICHEADERR								10601
#define STRING_ERROR_CS_ICOPENERR								10602
#define STRING_ERROR_CS_ICREADERR								10603
#define STRING_ERROR_CS_ICSEEKERR								10604 
#define STRING_ERROR_CS_UNKNOWNERR								10605
#define STRING_ERROR_CS_CONVERTERR								10606

#define STRING_NAME_CS_SPELLDB									10607
#define STRING_NAME_CS_HYPHENDB									10608
#define STRING_NAME_CS_PERSONALDB								10609
#define STRING_EFFECTS_FRAME									10610
#define STRING_ERROR_CS_PD_AUTOEXP_WRN							10611
#define STRING_ERROR_CS_PD_AUTODEL_WRN							10612
#define STRING_ERROR_CS_PD_FULL_WRN								10613
#define STRING_ERROR_CS_PD_NOALT_WRN							10614
#define STRING_ERROR_CS_PD_DUP_WRN								10615
#define STRING_ERROR_CS_SLALLOC_ERR								10616
#define STRING_ERROR_CS_SLLOCK_ERR								10617
#define STRING_ERROR_CS_SLTMPFNAME_ERR							10618
#define STRING_ERROR_CS_PD_FULL_ERR								10619
#define STRING_ERROR_CS_PD_OKRET								10620
#define STRING_ERROR_CS_PD_ERRET								10621

#define STRING_APPLICATION_NAME									10622
#define STRING_ERROR_CS_PD_SDINIT_ERR							10623
#define STRING_ERROR_CS_SLOPEN_ERR								10624
#define STRING_ERROR_CS_SLREAD_ERR								10625
#define STRING_ERROR_CS_SLWRITE_ERR								10626 
#define STRING_ERROR_CS_PD_MAXDID_ERR							10627

#define STRING_ERROR_CS_ICCLOSEERR								10628
#define STRING_ERROR_CS_PDCLOSEERR								10629

#define STRING_NAME_IF_THESAURUSDB								10631
#define STRING_ERROR_IF_LOOKUPERR								10632
#define STRING_ERROR_IF_MEMERR									10633  // Not used in framework
#define STRING_ERROR_IF_FINDERR									10634  // Not used in framework
#define STRING_ERROR_IF_ERRACCESSDB								10636

#define STRING_PROMPT_CONVERT_METAFILE_TO_BITMAP			10637  // Not used in framework
#define STRING_WARN_CONVERT_METAFILE_TO_BITMAP				10638  // Not used in framework

#define STRING_PROMPT_DOC_CHANGED								10639

#define STRING_COMPANY_NAME										10640

#define STRING_PRINTER_VENDOR_DB_NAME							10641
#define STRING_PRINTER_USER_DB_NAME								10642
#define STRING_PRINT_TEST_TITLE									10643
#define STRING_PRINT_TEST_PRINT_DOC_NAME						10644
#define STRING_PRINT_TEST_PRINT_LEFT_CALIBRATION			10645
#define STRING_PRINT_TEST_PRINT_TOP_CALIBRATION				10646
#define STRING_PRINT_TEST_PRINT_RIGHT_CALIBRATION			10647
#define STRING_PRINT_TEST_PRINT_BOTTOM_CALIBRATION			10648
#define STRING_PRINT_TEST_PRINT_STRING_1						10649
#define STRING_PRINT_TEST_PRINT_STRING_2						10650
#define STRING_TEST_PAGE_DOC_TITLE								10651

#define STRING_ERROR_CS_PDSAVEERR								10652
#define STRING_ERROR_IF_INITERR									10653

#define STRING_CS_DIALECT_FLAG									10654
#define STRING_IF_DIALECT_FLAG									10655

#define STRING_SOUND_FILE_FILTERS								10656
#define STRING_FILE_LINK											10657
#define STRING_EMAIL_LINK											10658
#define STRING_WEB_LINK												10659
#define STRING_EDIT_HYPERLINK										10660
#define STRING_ADD_HYPERLINK										10661

#define STRING_MISC_NOSUGGESTIONS								10700
#define STRING_MISC_SPELLREPLACEWORDNOTFOUND					10701
#define STRING_MISC_SPELLCHECKDONE								10702

#define STRING_TEST_PAGE_DONE										10703

#define STRING_COLOR_TRANSPARENT									10704

#define STRING_PRINT_TEST_CONFIRMATION_TEXT1					10706
#define STRING_PRINT_TEST_CONFIRMATION_TEXT2					10707

//	Define printer name and orientation strings seperately so that 
//	they can be internationalized properly.
#define STRING_PRINT_PAPER_SIZE_USLETTER_TALL				10710
#define STRING_PRINT_PAPER_SIZE_USLETTER_WIDE				10711
#define STRING_PRINT_PAPER_SIZE_USLEGAL_TALL					10712
#define STRING_PRINT_PAPER_SIZE_USLEGAL_WIDE					10713
#define STRING_PRINT_PAPER_SIZE_USTABLOID_TALL				10714
#define STRING_PRINT_PAPER_SIZE_USTABLOID_WIDE				10715
#define STRING_PRINT_PAPER_SIZE_A4LETTER_TALL				10716
#define STRING_PRINT_PAPER_SIZE_A4LETTER_WIDE				10717
#define STRING_PRINT_PAPER_SIZE_B5LETTER_TALL				10718
#define STRING_PRINT_PAPER_SIZE_B5LETTER_WIDE				10719
#define STRING_PRINT_PAPER_SIZE_A3TABLOID_TALL				10720
#define STRING_PRINT_PAPER_SIZE_A3TABLOID_WIDE				10721
#define STRING_PRINT_PAPER_SIZE_USLETTER						10722
#define STRING_PRINT_PAPER_SIZE_USLEGAL						10723
#define STRING_PRINT_PAPER_SIZE_USTABLOID						10724
#define STRING_PRINT_PAPER_SIZE_A4LETTER						10725
#define STRING_PRINT_PAPER_SIZE_B5LETTER						10726
#define STRING_PRINT_PAPER_SIZE_A3TABLOID						10727

#define STRING_SELECT_DIRECTORY									10728
#define STRING_ADDPAGEWIZ									10729
#define STRING_DONE											10730

// ****************************************************************************
//
//		Menu String Resource IDs
//
//		Range 10900 - 10999
//
// ****************************************************************************

#define STRING_MENU_SHADOW_OFF								10900
#define STRING_MENU_SHADOW_ON									10901

#define STRING_MENU_GLOW_OFF									10902
#define STRING_MENU_GLOW_ON									10903

#define STRING_MENU_VIGNETTE_OFF								10904
#define STRING_MENU_VIGNETTE_ON								10905



// ****************************************************************************
//
//		File Menu Command IDs
//
//		Range 8000 - 8049
//
// ****************************************************************************

#define COMMAND_MENU_FILE_NEW									8000
#define COMMAND_MENU_FILE_OPEN									8001
#define COMMAND_MENU_FILE_CLOSE									8002
#define COMMAND_MENU_FILE_SAVEAS								8003
#define COMMAND_MENU_FILE_REVERT								8004
#define COMMAND_MENU_FILE_PRINTPREVIEW							8006
#define COMMAND_MENU_FILE_SEND									8007
#define COMMAND_MENU_FILE_QUIT									8008
#define COMMAND_MENU_FILE_SAVE									8009
#define COMMAND_MENU_FILE_PRINT									8010
#define COMMAND_MENU_FILE_FASTPRINT								8011
#define COMMAND_MENU_FILE_PRINTTEST								8012

// ****************************************************************************
//
//		Edit Menu Command IDs
//
//		Range 8100 - 8149
//
// ****************************************************************************

#define COMMAND_MENU_EDIT_UNDO									8100
#define COMMAND_MENU_EDIT_REDO									8101
#define COMMAND_MENU_EDIT_CUT										8102
#define COMMAND_MENU_EDIT_COPY									8103
#define COMMAND_MENU_EDIT_PASTE									8104
#define COMMAND_MENU_EDIT_CLEAR									8105
#define COMMAND_MENU_EDIT_SELECTALL								8106
#define COMMAND_MENU_EDIT_DUPLICATE								8107
#define COMMAND_MENU_EDIT_EDIT_SELECTED						8108
#define COMMAND_MENU_EDIT_DELETE									8109
#define COMMAND_MENU_EDIT_PASTE_HERE							8110
#define COMMAND_MENU_EDIT_ADDEDIT_HYPERLINK					8111

// ****************************************************************************
//
//		View Menu Command IDs
//
//		Range 8200 - 8249
//
// ****************************************************************************

#define COMMAND_MENU_VIEW_ZOOMIN								8200
#define COMMAND_MENU_VIEW_ZOOMOUT								8201
#define COMMAND_MENU_VIEW_FITTOWINDOW							8202
#define COMMAND_MENU_VIEW_FITOBJECTSTOWINDOW					8203
#define COMMAND_MENU_VIEW_TOOLBAR_STANDARD						8204
#define COMMAND_MENU_VIEW_TOOLBAR_TEXT							8205
#define COMMAND_MENU_VIEW_TOOLBAR_COMPONENTS					8206
#define COMMAND_MENU_VIEW_TOOLBAR_COLOR							8207
#define COMMAND_MENU_ZOOM_START									8250
#define COMMAND_MENU_ZOOM_END									8269

// ****************************************************************************
//
//		Insert Menu Command IDs
//
//		Range 8300 - 8349
//
// ****************************************************************************

#define COMMAND_MENU_INSERT_SELECT_SOURCE						8300
#define COMMAND_MENU_INSERT_ACQUIRE								8301
#define COMMAND_MENU_INSERT_NONSTANDARD							8302

// ****************************************************************************
//
//		Effects Menu Command IDs
//
//		Range 8400 - 8449
//
// ****************************************************************************

#define COMMAND_MENU_EFFECTS_ADDSHADOW							8400
#define COMMAND_MENU_EFFECTS_COLOR_BEHIND_OBJECT				8401
#define COMMAND_MENU_EFFECTS_SCALE								8403
#define COMMAND_MENU_EFFECTS_ALIGN								8404
#define COMMAND_MENU_EFFECTS_ROTATE								8405
#define COMMAND_MENU_EFFECTS_ORDER_BRINGFORWARD					8409
#define COMMAND_MENU_EFFECTS_ORDER_BRINGTOFRONT					8410
#define COMMAND_MENU_EFFECTS_ORDER_SENDBACKWARD					8411
#define COMMAND_MENU_EFFECTS_ORDER_SENDTOBACK					8412
#define COMMAND_MENU_EFFECTS_FLIP_HORIZONTAL					8413
#define COMMAND_MENU_EFFECTS_FLIP_VERTICAL						8414
#define COMMAND_MENU_EFFECTS_FLIP_BOTH							8415
#define COMMAND_MENU_EFFECTS_FRAME_NONE							8416
#define COMMAND_MENU_EFFECTS_FRAME_THINLINE						8417
#define COMMAND_MENU_EFFECTS_FRAME_MEDIUMLINE					8418
#define COMMAND_MENU_EFFECTS_FRAME_THICKLINE					8419
#define COMMAND_MENU_EFFECTS_FRAME_ROUNDCORNER					8420
#define COMMAND_MENU_EFFECTS_FRAME_DOUBLELINE					8421
#define COMMAND_MENU_EFFECTS_FRAME_DOUBLEMEDIUMLINE				8422
#define COMMAND_MENU_EFFECTS_FRAME_DOUBLETHICKLINE				8423
#define COMMAND_MENU_EFFECTS_FRAME_DROPSHADOW					8424
#define COMMAND_MENU_EFFECTS_FRAME_PICTURE						8425
#define COMMAND_MENU_EFFECTS_FRAME_DIALOG						8426
#define COMMAND_MENU_EFFECTS_LOCK								8427
#define COMMAND_MENU_EFFECTS_SHADOW								8430
#define COMMAND_MENU_EFFECTS_GLOW_ON_OFF							8431
#define COMMAND_MENU_EFFECTS_VIGNETTE_ON_OFF						8432
#define COMMAND_MENU_EFFECTS_ROTATE_RIGHT90						8433
#define COMMAND_MENU_EFFECTS_ROTATE_LEFT90						8434
#define COMMAND_MENU_EFFECTS_CENTER_ON_PANEL					8435
#define COMMAND_MENU_EFFECTS_SIZE_TO_PANEL						8436
#define COMMAND_MENU_EFFECTS_SCALE_TO_PANEL						8437
#define COMMAND_MENU_ARRANGE_GROUP									8438

// ****************************************************************************
//
//		Effects Menu Command IDs
//
//		Range 8450 - 8499
//
// ****************************************************************************
#define COMMAND_MENU_EFFECTS_IMAGE_EFFECTS						8450

#define COMMAND_MENU_EFFECTS_DROPSHADOW							8451
#define COMMAND_MENU_EFFECTS_GLOW								8452
#define COMMAND_MENU_EFFECTS_EDGES								8453

#define COMMAND_MENU_EFFECTS_CROP_STRAIGHTEN					8454
#define COMMAND_MENU_EFFECTS_ADJUST_COLOR						8455
#define COMMAND_MENU_EFFECTS_BRIGHTNESS_FOCUS					8456
#define COMMAND_MENU_EFFECTS_FIX_FLAW							8457
#define COMMAND_MENU_EFFECTS_ARTISTIC_EFFECTS					8458

// ****************************************************************************
//
//		Debug Menu Command IDs
//
//		Range 8900 - 8949
//
// ****************************************************************************

#define COMMAND_MENU_DEBUG_CREATEGROUP							8900
#define COMMAND_MENU_DEBUG_BREAKGROUP							8901
#define COMMAND_MENU_DEBUG_MEMDIALOG							8902
#define COMMAND_MENU_DEBUG_MEM_SUCCEED							8903
#define COMMAND_MENU_DEBUG_MEM_FAIL1							8904
#define COMMAND_MENU_DEBUG_MEM_FAIL2							8905
#define COMMAND_MENU_DEBUG_MEM_FAIL3							8906
#define COMMAND_MENU_DEBUG_MEM_FAIL4							8907
#define COMMAND_MENU_DEBUG_FILEDIALOG							8908
#define COMMAND_MENU_DEBUG_FILE_SUCCEED							8909
#define COMMAND_MENU_DEBUG_FILE_FAIL1							8910
#define COMMAND_MENU_DEBUG_FILE_FAIL2							8911
#define COMMAND_MENU_DEBUG_FILE_FAIL3							8912
#define COMMAND_MENU_DEBUG_FILE_FAIL4							8913
#define COMMAND_MENU_DEBUG_EDIT_COMPONENT_ATTRIBUTES			8914
#define COMMAND_MENU_DEBUG_SCRIPT_RECORD						8915
#define COMMAND_MENU_DEBUG_SCRIPT_PLAYBACK						8916
#define COMMAND_MENU_DEBUG_MEM_PURGE							8917
#define COMMAND_MENU_DEBUG_USE_OFFSCREENS						8920
#define COMMAND_MENU_DEBUG_ALIGN_TO_PANEL_LEFT					8921
#define COMMAND_MENU_DEBUG_ALIGN_TO_PANEL_RIGHT					8922
#define COMMAND_MENU_DEBUG_ALIGN_TO_PANEL_TOP					8923
#define COMMAND_MENU_DEBUG_ALIGN_TO_PANEL_BOTTOM				8924
#define COMMAND_MENU_DEBUG_IMPORT_OLDUI							8925
#define COMMAND_MENU_DEBUG_PMGC_GRAPHIC_BROWSER					8926


// ****************************************************************************
//
//		Non-standard Menu Command IDs
//
//		Range 10000 - 10999
//
// ****************************************************************************

#define COMMAND_MENU_INSERT_NONSTANDARDSTART					10000
#define COMMAND_MENU_INSERT_NONSTANDARDEND						10999

// ****************************************************************************
//
//		Settings Menu Command IDs
//
//		Range 11000 - 14999	(low 50 for each)
//
// ****************************************************************************

#define	MIN_SETTINGS_COMMAND_ID									11000
#define	MAX_SETTINGS_COMMAND_ID									14999

#define COMMAND_MENU_EFFECTS_TINT_0								11000
#define COMMAND_MENU_EFFECTS_TINT_10							11001
#define COMMAND_MENU_EFFECTS_TINT_20							11002
#define COMMAND_MENU_EFFECTS_TINT_30							11003
#define COMMAND_MENU_EFFECTS_TINT_40							11004
#define COMMAND_MENU_EFFECTS_TINT_50							11005
#define COMMAND_MENU_EFFECTS_TINT_60							11006
#define COMMAND_MENU_EFFECTS_TINT_70							11007
#define COMMAND_MENU_EFFECTS_TINT_80							11008
#define COMMAND_MENU_EFFECTS_TINT_90							11009
#define COMMAND_MENU_EFFECTS_TINT_100							11010

#define COMMAND_MENU_EFFECTS_COLOR_DLG							11013
#define COMMAND_MENU_EFFECTS_COLOR_OBJECT						11014
#define COMMAND_MENU_EFFECTS_COLOR_BLACK						11019
#define COMMAND_MENU_EFFECTS_COLOR_WHITE						11020
#define COMMAND_MENU_EFFECTS_COLOR_YELLOW						11021
#define COMMAND_MENU_EFFECTS_COLOR_MAGENTA					11022
#define COMMAND_MENU_EFFECTS_COLOR_RED							11023
#define COMMAND_MENU_EFFECTS_COLOR_CYAN						11024
#define COMMAND_MENU_EFFECTS_COLOR_GREEN						11025
#define COMMAND_MENU_EFFECTS_COLOR_BLUE						11026
#define COMMAND_MENU_EFFECTS_COLOR_LTGRAY						11027
#define COMMAND_MENU_EFFECTS_COLOR_GRAY						11028
#define COMMAND_MENU_EFFECTS_COLOR_DKGRAY						11029


// ****************************************************************************
//
//		Context Menu IDs
//
//		Range 18000 - 18499
//
// ****************************************************************************

#define MENU_CONTEXT_MULTIPLE_SELECTION						18000
#define MENU_CONTEXT_DEFAULT_COMPONENT							18001
#define MENU_CONTEXT_EDIT_VIA_PAINT_SHOP_PRO					18002

#endif	// _FRAMEWORKRESOURCEIDS_H_
