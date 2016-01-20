// ****************************************************************************
//
//  File Name:			TextCompResource.h
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
//  $Logfile:: /PM8/Resource/TextCompResource.h                           $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:34p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#ifndef	_TEXTCOMPRESOURCE_H_
#define	_TEXTCOMPRESOURCE_H_

#ifndef	_FRAMEWORKRESOURCEIDS_H_
	#include "FrameworkResourceIDs.h"
#endif

// ****************************************************************************
//
//		Resource ID range index :
//
//		Dialog IDs:
//			Component									15000 - 15099
//			Interface									15100 - 15199
//
//		Bitmap IDs:										
//			Component									15000 - 15099
//			Interface									15100 - 15299
//
//		String IDs:
//			Component									15000 - 15199
//			Interface									15200 - 15399
//
//		Misc IDs:
//			Component									15000 - 15299
//
// ****************************************************************************

// ****************************************************************************
//
//		Text component resources
//
//		Range 15000 - 15299
//
// ****************************************************************************

#define CURSOR_TEXT_DRAG_DROP											15000

#define  TEXT_SENTENCE_END_CHARACTER_LIST							15031
#define	TEXT_SHAPE_MORE_TEXT_INDICATOR							15299

#define	TEXT_SHAPE_START_VALUE										15000

#define	TEXT_SHAPE_NO_SHAPE											15000
#define	TEXT_SHAPE_1_1													15001
#define	TEXT_SHAPE_1_2													15002
#define	TEXT_SHAPE_2_1													15003
#define	TEXT_SHAPE_2_2													15004
#define	TEXT_SHAPE_3_1													15005
#define	TEXT_SHAPE_3_2													15006
#define	TEXT_SHAPE_4_1													15007
#define	TEXT_SHAPE_4_2													15008
#define	TEXT_SHAPE_5_1													15009
#define	TEXT_SHAPE_5_2													15010
#define	TEXT_SHAPE_6_1													15011
#define	TEXT_SHAPE_6_2													15012
#define	TEXT_SHAPE_7_1													15013
#define	TEXT_SHAPE_7_2													15014
#define	TEXT_SHAPE_8_1													15015
#define	TEXT_SHAPE_8_2													15016
#define	TEXT_SHAPE_9_1													15017
#define	TEXT_SHAPE_9_2													15018
#define	TEXT_SHAPE_10_1												15019
#define	TEXT_SHAPE_10_2												15020
#define	TEXT_SHAPE_11_1												15021
#define	TEXT_SHAPE_11_2												15022
#define	TEXT_SHAPE_12_1												15023
#define	TEXT_SHAPE_12_2												15024
#define	TEXT_SHAPE_13_1												15025
#define	TEXT_SHAPE_13_2												15026
#define	TEXT_SHAPE_14_1												15027
#define	TEXT_SHAPE_14_2												15028
#define	TEXT_SHAPE_15_1												15029
#define	TEXT_SHAPE_15_2												15030
#define	TEXT_SHAPE_16_1												15031
#define	TEXT_SHAPE_16_2												15032
#define	TEXT_SHAPE_17_1												15033
#define	TEXT_SHAPE_B_1_1												15034
#define	TEXT_SHAPE_B_1_2												15035
#define	TEXT_SHAPE_B_1_3												15036
#define	TEXT_SHAPE_B_1_4												15037
#define	TEXT_SHAPE_B_1_5												15038
#define	TEXT_SHAPE_B_1_6												15039
#define	TEXT_SHAPE_B_1_7												15040
#define	TEXT_SHAPE_B_1_8												15041
#define	TEXT_SHAPE_B_2_1												15042
#define	TEXT_SHAPE_B_2_2												15043
#define	TEXT_SHAPE_B_2_3												15044
#define	TEXT_SHAPE_B_2_4												15045
#define	TEXT_SHAPE_B_2_5												15046
#define	TEXT_SHAPE_B_2_6												15047
#define	TEXT_SHAPE_B_2_7												15048
#define	TEXT_SHAPE_18_1												15049
#define	TEXT_SHAPE_18_2												15050


// ****************************************************************************
//
//		Dialogs: Component
//
//		Range 15000 - 15099 for dialogs, controls may be anything
//
// ****************************************************************************
#define DIALOG_LINESPACE_CUSTOM         15000
#define CONTROL_STATIC_PLACEHOLDER		1001
#define CONTROL_SPIN                    1002
#define CONTROL_RADIO_CUSTOM            1003
#define CONTROL_RADIO_1                 1004
#define CONTROL_RADIO_15                1005
#define CONTROL_RADIO_2                 1006
#define CONTROL_EDIT                    1007
#define IDC_STATIC_GB                   1010
#define DIALOG_TEXT_SETTINGS						15001
#define CONTROL_CHECKBOX_TEXT_SETTINGS_BOLD			1100
#define CONTROL_CHECKBOX_TEXT_SETTINGS_ITALIC		1101
#define CONTROL_CHECKBOX_TEXT_SETTINGS_UNDERLINE	1102
#define CONTROL_COMBO_TEXT_SETTINGS_SIZE			1103
#define CONTROL_COMBO_TEXT_SETTINGS_COLOR			1104
#define CONTROL_COMBO_TEXT_SETTINGS_FONT			1105
#define CONTROL_EDIT_TEXT_SETTINGS_TEXT_SAMPLE		1106
#define CONTROL_BUTTON_TEXT_SETTINGS_CANCEL			1107
#define CONTROL_BUTTON_TEXT_SETTINGS_OK				1108
#define CONTROL_STATIC_TEXT_SETTINGS_FONT			1109 
#define CONTROL_STATIC_TEXT_SETTINGS_SIZE			1110 
#define CONTROL_STATIC_TEXT_SETTINGS_COLOR			1111 
#define CONTROL_STATIC_TEXT_SETTINGS_FONT_STYLES	1112 
#define CONTROL_STATIC_TEXT_SETTINGS_GROUPBOX		1113
#define CONTROL_STATIC_TEXT_SETTINGS_LINE			1114
#define CONTROL_STATIC_TEXT_SETTINGS_FONTSTYLE		1115
#define CONTROL_BUTTON_TEXT_SETTINGS_COLOR			1116

#define DIALOG_TEXTATTRIB_WRAPAROUND								15002
#define CONTROL_STATIC_TEXTATTRIB_WRAPPREVIEW					1101
#define CONTROL_RADIO_TEXTATTRIB_WRAP_BORDER						1102
#define CONTROL_RADIO_TEXTATTRIB_WRAP_GRAPHIC					1103
#define CONTROL_RADIO_TEXTATTRIB_IGNORE_GRAPHIC					1104
#define CONTROL_STATIC_TEXTATTRIB_WRAP								1105
#define CONTROL_STATIC_TEXTATTRIB_OPTION							1106
#define CONTROL_STATIC_TEXTATTRIB_LINE1							1107
#define CONTROL_STATIC_TEXTATTRIB_LINE2							1108
#define CONTROL_RADIO_TEXTATTRIB_TIGHT_TOLERANCE				1109
#define CONTROL_RADIO_TEXTATTRIB_MEDIUM_TOLERANCE				1110
#define CONTROL_RADIO_TEXTATTRIB_LOOSE_TOLERANCE				1111
#define CONTROL_GROUP_TEXTATTRIB_BOX								1112

#define DIALOG_INSERT_SYMBOL											15003
#define CONTROL_STATIC_SYMBOL_OUTLINE								1000
#define CONTROL_STATIC_SYMBOL_SYMBOLGRID							1001
#define CONTROL_STATIC_SYMBOL_SIDEBAR								1002
#define CONTROL_BUTTON_SYMBOL_PREVIEW								1003
#define CONTROL_BUTTON_SYMBOL_INSERT								1004

#define DIALOG_PPG_TEXTSHAPE											15004
#define DIALOG_PPG_TEXTBALLOON										15005
// The following IDs must always maintain the same relationship to one another
// (ie. B1 is 1 greater than A1 and A2 is 1 greater than C1 etc).  If they don't,
// the Text block attrib dialog will get messed up.
#define CONTROL_RADIO_TEXTSHAPE_A1									1001
#define CONTROL_RADIO_TEXTSHAPE_B1									1002
#define CONTROL_RADIO_TEXTSHAPE_C1									1003
#define CONTROL_RADIO_TEXTSHAPE_A2									1004
#define CONTROL_RADIO_TEXTSHAPE_B2									1005
#define CONTROL_RADIO_TEXTSHAPE_C2									1006
#define CONTROL_RADIO_TEXTSHAPE_A3									1007
#define CONTROL_RADIO_TEXTSHAPE_B3									1008
#define CONTROL_RADIO_TEXTSHAPE_C3									1009
#define CONTROL_RADIO_TEXT_SHAPE_NOSHAPE							1010
#define CONTROL_STATIC_TEXTSHAPE_SHAPEPREVIEW					1011
#define CONTROL_BUTTON_TEXTSHAPE_COLOR_OUTLINE					1012
#define CONTROL_STATIC_TEXTSHAPE_COLOR_OUTLINE					1013
#define CONTROL_BUTTON_TEXTSHAPE_COLOR_BACKGROUND				1014
#define CONTROL_STATIC_TEXTSHAPE_COLOR_BACKGROUND				1015
#define CONTROL_SCROLLBAR_TEXTSHAPE_SHAPES						1018
#define CONTROL_CHKBOX_TEXTSHAPE_FLIPHORIZ						1021
#define CONTROL_CHKBOX_TEXTSHAPE_FLIPVERT							1022


// ****************************************************************************
//
//		Bitmap IDs: Component
//
//		Range 15000 - 15099
//
// ****************************************************************************

// ****************************************************************************
//
//		Bitmap IDs: Interface
//
//		Range 15100 - 15299
//
// ****************************************************************************
#define BITMAP_INSERT_SYMBOL_SIDEBAR								15101

// Text shape bitmap ids
#define BMP_TEXTSHAPE_00												15160
#define BMP_TEXTSHAPE_01                                    15161
#define BMP_TEXTSHAPE_02                                    15162
#define BMP_TEXTSHAPE_03                                    15163
#define BMP_TEXTSHAPE_04                                    15164
#define BMP_TEXTSHAPE_05                                    15165
#define BMP_TEXTSHAPE_06                                    15166
#define BMP_TEXTSHAPE_07                                    15167
#define BMP_TEXTSHAPE_08                                    15168
#define BMP_TEXTSHAPE_09                                    15169
#define BMP_TEXTSHAPE_10                                    15170
#define BMP_TEXTSHAPE_11                                    15171
#define BMP_TEXTSHAPE_12                                    15172
#define BMP_TEXTSHAPE_13                                    15173
#define BMP_TEXTSHAPE_14                                    15174
#define BMP_TEXTSHAPE_15                                    15175
#define BMP_TEXTSHAPE_16                                    15176
#define BMP_TEXTSHAPE_17                                    15177
#define BMP_TEXTSHAPE_18                                    15178
#define BMP_TEXTSHAPE_19                                    15179
#define BMP_TEXTSHAPE_20                                    15180
#define BMP_TEXTSHAPE_21                                    15181 
#define BMP_TEXTSHAPE_22                                    15182
#define BMP_TEXTSHAPE_23                                    15183
#define BMP_TEXTSHAPE_24                                    15184
#define BMP_TEXTSHAPE_25                                    15185 
#define BMP_TEXTSHAPE_26                                    15186
#define BMP_TEXTSHAPE_27                                    15187
#define BMP_TEXTSHAPE_28                                    15188 
#define BMP_TEXTSHAPE_29                                    15189
#define BMP_TEXTSHAPE_30                                    15190
#define BMP_TEXTSHAPE_31                                    15191
#define BMP_TEXTSHAPE_32                                    15192
#define BMP_TEXTSHAPE_33                                    15193
#define BMP_TEXTSHAPE_34                                    15194
#define BMP_TEXTSHAPE_35                                    15195
#define BMP_TEXTSHAPE_36                                    15196
#define BMP_TEXTSHAPE_37                                    15197
#define BMP_TEXTSHAPE_38                                    15198
#define BMP_TEXTSHAPE_39                                    15199
#define BMP_TEXTSHAPE_40                                    15200
#define BMP_TEXTSHAPE_41                                    15201
#define BMP_TEXTSHAPE_42                                    15202
#define BMP_TEXTSHAPE_43                                    15203
#define BMP_TEXTSHAPE_44                                    15204
#define BMP_TEXTSHAPE_45                                    15205
#define BMP_TEXTSHAPE_46                                    15206
#define BMP_TEXTSHAPE_47                                    15207
#define BMP_TEXTSHAPE_48                                    15208
#define BMP_TEXTSHAPE_49                                    15209
#define BMP_TEXTSHAPE_50                                    15210
#define BMP_TEXTSHAPE_OUTLINE											15211
#define BMP_TEXTSHAPE_BACKGND											15212

#define BMP_TEXTAVOIDANCE_IGNORE										15250
#define BMP_TEXTAVOIDANCE_WRAPCONTOUR								15251
#define BMP_TEXTAVOIDANCE_WRAPBORDER								15252
#define BMP_TEXTAVOIDANCE_WRAPCONTOUR_NORMAL						15253
#define BMP_TEXTAVOIDANCE_WRAPBORDER_NORMAL						15254
#define BMP_TEXTAVOIDANCE_WRAPCONTOUR_LOOSE						15255
#define BMP_TEXTAVOIDANCE_WRAPBORDER_LOOSE						15256

// ****************************************************************************
//
//		String IDs: Component
//
//		Range 15000 - 15199
//
// ****************************************************************************

#define STRING_UNDO_TYPING											15000
#define STRING_REDO_TYPING											15001
#define STRING_UNDO_INSERT_MERGE_FIELD								15002
#define STRING_REDO_INSERT_MERGE_FIELD								15003
#define STRING_UNDO_ALTER_FONT										15004
#define STRING_REDO_ALTER_FONT										15005
#define STRING_UNDO_ALTER_PARAGRAPH									15006
#define STRING_REDO_ALTER_PARAGRAPH									15007
#define STRING_DEFAULT_CHARACTER_SETTINGS_TEXT						15008
#define STRING_UNDO_CUTTEXT											15009
#define STRING_REDO_CUTTEXT											15010
#define STRING_UNDO_DELETE_TYPING									15011
#define STRING_REDO_DELETE_TYPING									15012
#define STRING_UNDO_DELETE_TEXT_SELECTION							15013
#define STRING_REDO_DELETE_TEXT_SELECTION							15014
#define STRING_UNDO_TEXT_MOVE										15015
#define STRING_REDO_TEXT_MOVE										15016
#define STRING_UNDO_CHANGE_RETURN_ADDRESS							15017
#define STRING_REDO_CHANGE_RETURN_ADDRESS							15018
#define STRING_UNDO_INSERT_QUOTE									15019
#define STRING_REDO_INSERT_QUOTE									15020
#define STRING_UNDO_INSERT_SYMBOL									15021
#define STRING_REDO_INSERT_SYMBOL									15022
#define STRING_UNDO_LINE_SPACEING									15023
#define STRING_REDO_LINE_SPACEING									15024
#define STRING_UNDO_PASTETEXT										15025
#define STRING_REDO_PASTETEXT										15026
#define STRING_RETURN_ADDRESS_FORMAT								15028
#define STRING_UNDO_SETTEXTBLOCK									15029
#define STRING_REDO_SETTEXTBLOCK									15030
#define STRING_TEXT_COMPONENT_CREATION								15034
#define STRING_TEXT_COMPONENT_BULLET_FONT							15035
#define STRING_AVOIDANCE_DIALOG_DEFAULT_TEXT						15036


// ****************************************************************************
//
//		String IDs: Interface
//
//		Range 15200 - 15399
//
// ****************************************************************************

// ****************************************************************************
//
//		Text Menu Command IDs
//
//		Range 8550 - 8599
//
// ****************************************************************************

#define COMMAND_MENU_TEXT_QUOTESANDVERSES							8550
#define COMMAND_MENU_TEXT_INSERT_FIELD								8551

#define MENU_CONTEXT_TEXT_NOT_ACTIVE								15100
#define MENU_CONTEXT_TEXT_ACTIVE_NO_SELECTION					15101
#define MENU_CONTEXT_TEXT_ACTIVE_SELECTION						15102

#define COMMAND_MENU_TEXT_SETTINGS									11050
#define COMMAND_MENU_TEXT_SIZE                              11051
#define COMMAND_MENU_TEXT_BOLD										11052
#define COMMAND_MENU_TEXT_ITALIC										11053
#define COMMAND_MENU_TEXT_UNDERLINE									11054
#define COMMAND_MENU_TEXT_JUSTIFICATION							11055
#define COMMAND_MENU_TEXT_JUSTIFICATION_LEFT						11056
#define COMMAND_MENU_TEXT_JUSTIFICATION_CENTER					11057
#define COMMAND_MENU_TEXT_JUSTIFICATION_RIGHT					11058
#define COMMAND_MENU_TEXT_JUSTIFICATION_FULL						11059
#define COMMAND_MENU_TEXT_BULLET										11060
#define COMMAND_MENU_TEXT_DROPCAPS									11061
#define COMMAND_MENU_TEXT_HYPHENATION								11062
#define COMMAND_MENU_TEXT_PLACEMENT									11063
#define COMMAND_MENU_TEXT_PLACEMENT_TOP							11064
#define COMMAND_MENU_TEXT_PLACEMENT_CENTER						11065
#define COMMAND_MENU_TEXT_PLACEMENT_BOTTOM						11066
#define COMMAND_MENU_TEXT_PLACEMENT_FULL							11067
#define COMMAND_MENU_TEXT_ATTRIBUTES								11068
#define COMMAND_MENU_TEXT_AVOIDANCE									11069
#define COMMAND_MENU_TEXT_INSERT_SYMBOL							11070

#define COMMAND_MENU_TEXT_BLOCKSHAPE								11073
#define COMMAND_MENU_TEXT_BLOCKSHAPE_NONE							11074
#define COMMAND_MENU_TEXT_BLOCKSHAPE_OVAL							11075
#define COMMAND_MENU_TEXT_BLOCKSHAPE_OVALLOWERHALF				11076
#define COMMAND_MENU_TEXT_BLOCKSHAPE_OVALUPPERHALF				11077
#define COMMAND_MENU_TEXT_BLOCKSHAPE_TRAPEZOIDWIDEBOTTOM		11078
#define COMMAND_MENU_TEXT_BLOCKSHAPE_TRAPEZOIDWIDELEFT		11079
#define COMMAND_MENU_TEXT_BLOCKSHAPE_TRAPEZOIDWIDERIGHT		11080
#define COMMAND_MENU_TEXT_BLOCKSHAPE_TRAPEZOIDWIDETOP			11081
#define COMMAND_MENU_TEXT_BLOCKSHAPE_RECTTALL					11082
#define COMMAND_MENU_TEXT_BLOCKSHAPE_RECTSLANTLEFT				11083
#define COMMAND_MENU_TEXT_BLOCKSHAPE_RECTSLANTRIGHT			11084
#define COMMAND_MENU_TEXT_BLOCKSHAPE_DIAMOND						11085
#define COMMAND_MENU_TEXT_BLOCKSHAPE_TRIANGLE					11086
#define COMMAND_MENU_TEXT_SINGLE_SPACE								11087
#define COMMAND_MENU_TEXT_ONE_ONE_HALF_SPACE						11088
#define COMMAND_MENU_TEXT_DOUBLE_SPACE								11089
#define COMMAND_MENU_TEXT_CUSTOM_SPACE								11090

#define COMMAND_MENU_TEXT_WORD_BALLOONS							11091
#define COMMAND_MENU_TEXT_AROUND_GRAPHICS							11092

// ****************************************************************************
//
//		Misc IDs: Component
//
//		Range 15000 - 15299
//
// ****************************************************************************


#endif // _TEXTCOMPRESOURCE_H_
