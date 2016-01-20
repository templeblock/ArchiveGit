// ****************************************************************************
//
//  File Name:			HeadlineCompResource.h
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
//  $Logfile:: /PM8/Resource/HeadlineCompResource.h                           $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:28p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#ifndef	_HEADLINDCOMPRESOURCE_H_
#define	_HEADLINDCOMPRESOURCE_H_

#ifndef	_FRAMEWORKRESOURCEIDS_H_
	#include "FrameworkResourceIDs.h"
#endif

// ****************************************************************************
//
//		Resource ID range index :
//
//		Dialog IDs:
//			Component									14000 - 14099
//			Interface									14100 - 14199
//
//		Bitmap IDs:										
//			Component									14000 - 14099
//			Interface									14100 - 14299
//
//		String IDs:
//			Component									14000 - 14199
//			Interface									14200 - 14399
//
//		Misc IDs:
//			Component									14000 - 14299
//
// ****************************************************************************

// ****************************************************************************
//
//		Dialogs: Component
//
//		Range 14000 - 14099 for dialogs, controls may be anything
//
// ****************************************************************************

#define IDD_PPG_SHAPE																14000
#define IDC_RD_NON_DISTORT															 1000
#define IDC_RD_FOLLOW_PATH															 1001
#define IDC_RD_WARP_SHAPE															 1002
#define IDC_RD_ALT_BASELINE														 1003
#define IDC_RD_SHEAR																	 1004
#define IDC_RD_ALT_SHEAR															 1005
#define IDC_RD_ALT_SHEAR_SHEAR													 1006
#define IDC_CB_NON_DISTORT															 1107
#define CONTROL_EDIT_SHAPE_SHEAR_ANGLE											 1008
#define CONTROL_EDIT_SHAPE_ROTATION												 1009
#define CONTROL_EDIT_SHAPE_ALT_PERCENT											 1010
#define CONTROL_STATIC_SHAPE_SHEAR_ANGLE										 1011
#define CONTROL_STATIC_SHAPE_ROTATION											 1012
#define CONTROL_STATIC_SHAPE_ALT_PERCENT										 1013
#define IDC_BTN_SHAPE1																 1014
#define IDC_BTN_SHAPE2																 1015
#define IDC_BTN_SHAPE3																 1016
#define IDC_BTN_SHAPE4																 1017
#define IDC_BTN_SHAPE5																 1018
#define IDC_BTN_SHAPE6																 1019
#define IDC_BTN_SHAPE7																 1020
#define IDC_BTN_SHAPE8																 1021
#define IDC_BTN_SHAPE9																 1022
#define IDC_BTN_SHAPE10																 1023
#define IDC_BTN_SHAPE11																 1024
#define IDC_BTN_SHAPE12																 1025
#define IDC_BTN_SHAPE13																 1026
#define IDC_BTN_SHAPE14																 1027
#define IDC_BTN_SHAPE15																 1028
#define IDC_BTN_SELECTED_BMP														 1029 
																						  
#define IDD_PPG_OUTLINE                 										14001
#define IDC_RD_STROKE                   										 1000
#define IDC_RD_FILL                     										 1001
#define IDC_RD_STROKE_FILL              										 1002
#define IDC_RD_HEAVY                    										 1003
#define IDC_RD_DOUBLE                   										 1004
#define IDC_RD_BLURRED                  										 1005
#define IDC_RD_TRIPLE                   										 1006
#define IDC_RD_HAIRLINEDBL              										 1007
#define IDC_RD_DBLHAIRLINE              										 1008
#define IDC_RD_DEBOSSED                 										 1009
#define IDC_RD_EMBOSSED                 										 1010
#define IDC_RD_SOFTEMBOSSED             										 1011
#define IDC_RD_SOFTDEBOSSED             										 1012
#define IDC_RD_ROUNDED                  										 1013
#define IDC_RD_GLINT                    										 1014
#define IDC_RD_SIMPLEFILLSTROKE         										 1015
#define IDC_RD_HAIRLINE                 										 1016
#define IDC_RD_THIN                     										 1017
#define IDC_RD_MED                      										 1018
#define IDC_RD_THICK																	 1019
#define IDC_RD_THICK2																 1020
#define IDC_CMB_STROKE_COLOR														 1021
#define IDC_CMB_FILL_COLOR															 1022
#define IDC_CMB_SHADOW_COLOR														 1023
#define IDC_CMB_HILITE_COLOR														 1024
#define IDC_GP_LINEWEIGHT															 1025
#define IDC_GP_COLORS																 1026
#define IDC_S_STROKE																	 1027
#define IDC_S_FILL																	 1028
#define IDC_S_SHADOW																	 1029
#define IDC_S_HILITE															 		 1030
																						 
#define IDD_PPG_EFFECTS																14002
#define CONTROL_RADIO_BTN_EFFECTS_NO_EFFECTS									 1000
#define CONTROL_RADIO_BTN_EFFECTS_SIMPLE_STACK								 1001
#define CONTROL_RADIO_BTN_EFFECTS_SPIN_STACK									 1002
#define CONTROL_RADIO_BTN_EFFECTS_PSUEDO_PERSPECTIVE_STACK				 1003
#define CONTROL_RADIO_BTN_EFFECTS_SOLID_EXTRUSION							 1004
#define CONTROL_RADIO_BTN_EFFECTS_PSUEDO_PERSPECTIVE_EXTRUSION			 1005
#define CONTROL_RADIO_BTN_EFFECTS_ILLUMINATED_EXTRUSION					 1006
#define CONTROL_EDIT_EFFECTS_STAGES												 1007
#define CONTROL_EDIT_EFFECTS_STACK_DEPTH										 1008
#define CONTROL_EDIT_EFFECTS_VANISHING_POINTX								 1009
#define CONTROL_EDIT_EFFECTS_VANISHING_POINTY								 1010
#define CONTROL_COMBO_EFFECTS_COLOR												 1011
#define CONTROL_CHECKBOX_EFFECTS_BLEND											 1012
#define CONTROL_COMBO_EFFECTS_BLEND_COLOR										 1013
#define CONTROL_RADIO_BTN_EFFECTS_NO_SHADOW									 1014
#define CONTROL_RADIO_BTN_EFFECTS_DROP_SHADOW								 1015
#define CONTROL_RADIO_BTN_EFFECTS_SOFT_DROP_SHADOW							 1016
#define CONTROL_RADIO_BTN_EFFECTS_CAST_SHADOW								 1017
#define CONTROL_RADIO_BTN_EFFECTS_SOFT_CAST_SHADOW							 1018
#define CONTROL_EDIT_EFFECTS_SHADOW_DEPTH										 1019
#define CONTROL_EDIT_FX_SHADOW_VANISHING_POINTX								 1020
#define CONTROL_EDIT_FX_SHADOW_VANISHING_POINTY								 1021
#define CONTROL_COMBO_FX_SHADOW_COLOR											 1022
#define CONTROL_CHECKBOX_EFFECTS_SHADOW_BLEND								 1023
#define CONTROL_COMBO_FX_SHADOW_BLEND_COLOR									 1024
#define CONTROL_STATIC_EFFECTS_SHADOW_DEPTH									 1025
#define CONTROL_STATIC_EFFECTS_SHADOW_VANISHING_PT							 1026
#define CONTROL_STATIC_EFFECTS_SHADOW_COLOR									 1027
#define CONTROL_STATIC_EFFECTS_SHADOW_BLEND_COLOR							 1028
#define CONTROL_STATIC_EFFECTS_EFFECTS_STAGES								 1029
#define CONTROL_STATIC_EFFECTS_EFFECTS_DEPTH									 1030
#define CONTROL_STATIC_EFFECTS_VANISHING_PT									 1031
#define CONTROL_STATIC_EFFECTS_COLOR											 1032
#define CONTROL_GROUPBOX_EFFECTS_EFFECTS										 1033
#define CONTROL_GROUPBOX_EFFECTS_SHADOWS										 1034
#define CONTROL_STATIC_EFFECTS_BLEND_COLOR									 1035
																					
#define DIALOG_PROPERTY_PAGE_ATTRIBUTES_TAB						14003
#define CONTROL_COMBO_ATTRIBUTES_TAB_FONT						 1000
#define CONTROL_COMBO_ATTRIBUTES_TAB_ESCAPEMENT					 1001
#define CONTROL_STATIC_ATTRIBUTES_TAB_ESCAPEMENT				 1002
#define CONTROL_RADIO_ATTRIBUTES_TAB_TIGHT						 1003
#define CONTROL_RADIO_ATTRIBUTES_TAB_LOOSE						 1004
#define CONTROL_RADIO_ATTRIBUTES_TAB_REGULAR					 1005
#define CONTROL_BUTTON_ATTRIBUTES_TAB_EDIT_DEGREE				 1006
#define CONTROL_CHECKBOX_ATTRIBUTES_TAB_KERNING					 1007
#define CONTROL_CHECKBOX_ATTRIBUTES_TAB_LEADING					 1008
#define CONTROL_STATIC_ATTRIBUTES_TAB_SCALING					 1009
#define CONTROL_STATIC_ATTRIBUTES_TAB_COMPENSATION				 1010
#define CONTROL_EDIT_ATTRIBUTES_TAB_SCALE1						 1011
#define CONTROL_EDIT_ATTRIBUTES_TAB_SCALE2						 1012
#define CONTROL_EDIT_ATTRIBUTES_TAB_SCALE3						 1013
#define CONTROL_STATIC_ATTRIBUTES_TAB_SCALE1					 1014
#define CONTROL_STATIC_ATTRIBUTES_TAB_SCALE2					 1015
#define CONTROL_STATIC_ATTRIBUTES_TAB_SCALE3					 1016
#define CONTROL_STATIC_ATTRIBUTES_TAB_FONT						 1017
#define CONTROL_STATIC_ATTRIBUTES_TAB_STYLE						 1018
#define CONTROL_STATIC_ATTRIBUTES_TAB_JUSTIFICATION				 1019
#define CONTROL_BUTTON_ATTRIBUTES_TAB_LEFTJUST					 1020
#define CONTROL_BUTTON_ATTRIBUTES_TAB_CENTERJUST				 1021
#define CONTROL_BUTTON_ATTRIBUTES_TAB_RIGHTJUST					 1022
#define CONTROL_BUTTON_ATTRIBUTES_TAB_FULLJUST					 1023
#define CONTROL_BUTTON_ATTRIBUTES_TAB_BOLD						 1024
#define CONTROL_BUTTON_ATTRIBUTES_TAB_ITALIC					 1025
#define CONTROL_BUTTON_ATTRIBUTES_TAB_UNDERLINE					 1026

#define IDD_PPG_BEHIND_EFFECTS									14004	
#define IDC_RD_NOFRAME											 1000
#define IDC_RD_SILHOUETTE										 1001
#define IDC_RD_EMBOSS											 1002
#define IDC_RD_DEBOSS											 1003
#define IDC_STATIC_FILL_COLOR									 1004
#define IDC_STATIC_HILITE_COLOR									 1005
#define IDC_STATIC_SHADOW_COLOR									 1006

#define DIALOG_EDIT_COMPENSATION_DEGREE							14005
#define CONTROL_STATIC_COMPENSATION_DEGREE						 1000
#define CONTROL_EDIT_COMPENSATION_DEGREE						 1001

// ****************************************************************************
//
//		Dialogs: Interface
//
//		Range 14100 - 14199 for dialogs, controls may be anything
//
// ****************************************************************************

#define IDD_HEADLINE_DLG										14100
#define IDC_HEADLINE_TEXT										 1000
#define IDC_READYMADE											 1001
#define CONTROL_CUSTOMIZE										 1002
#define IDC_STATIC_TEXT											 1003
#define CONTROL_PREVIEW											 1004
#define IDC_READYMADES											 1005
#define IDC_TABS													 1006
#define IDC_HEADLINE_SPLASH									 1007
#define IDC_SAVEAS												 1008
																
#define IDD_FACE_PAGE											14101
#define IDC_COLOR													 1000
#define IDC_IMAGE_LIST											 1001
																
#define IDD_SHAPE_PAGE											14102
#define IDC_DISTORT												 1002
#define IDC_HORIZONTAL											 1003
#define IDC_VERTICAL												 1004
																
#define IDD_OUTLINE_PAGE										14103
#define IDC_OUTLINE_GLOW										 1002
																
#define IDD_DEPTH_PAGE											14104
#define IDC_DEPTH_STATIC										 1002
																
#define IDD_POSITION_PAGE										14105
																
#define IDD_PROPORTION_PAGE									14106
																
#define IDD_HEADLINE_FONT										14107
#define IDC_FONTLIST												 1002
#define IDC_BOLD													 1003
#define IDC_ITALIC												 1004
#define IDC_JUSTIFY_LEFT										 1005
#define IDC_JUSTIFY_CENTER										 1006
#define IDC_JUSTIFY_RIGHT										 1007
#define IDC_JUSTIFY_FULL										 1008


// ****************************************************************************
//
//		Bitmap IDs: Component
//
//		Range 14000 - 14099
//
// ****************************************************************************

#define IDB_PATH_1                      						14000
#define IDB_PATH_2                      						14001
#define IDB_PATH_3                      						14002
#define IDB_PATH_4                      						14003

#define IDB_STYLE_BOLDD                 						14004
#define IDB_STYLE_BOLDU                 						14005
#define IDB_STYLE_ITALICD               						14006
#define IDB_STYLE_ITALICU               						14007
#define IDB_STYLE_UNDERLINED            						14008
#define IDB_STYLE_UNDERLINEU            						14009

#define IDB_JUST_LEFTD                  						14010
#define IDB_JUST_LEFTU                  						14011
#define IDB_JUST_RIGHTD                 						14012
#define IDB_JUST_RIGHTU                 						14013
#define IDB_JUST_CENTERD                						14014
#define IDB_JUST_CENTERU                						14015
#define IDB_JUST_FULLD                  						14016
#define IDB_JUST_FULLU                  						14017

#define IDB_SHAPE_A1                    						14018
#define IDB_SHAPE_A2                    						14019
#define IDB_SHAPE_A3                    						14020

#define IDB_SHAPE_B1                   						14021
#define IDB_SHAPE_B2                   						14022
#define IDB_SHAPE_B3                   						14023

#define IDB_SHAPE_C1                   						14024
#define IDB_SHAPE_C2                    						14025
#define IDB_SHAPE_C3                    						14026

#define IDB_SHAPE_D1                    						14027
#define IDB_SHAPE_D2                    						14028
#define IDB_SHAPE_D3                    						14029

#define IDB_SHAPE_E1                    						14030
#define IDB_SHAPE_E2                   						14031
#define IDB_SHAPE_E3													14032


// ****************************************************************************
//
//		Bitmap IDs: Interface
//
//		Range 14100 - 14299
//
// ****************************************************************************

#define IDB_HEADLINE_FACES											14100
#define IDB_HEADLINE_ORIENTATION									14101
#define IDB_HEADLINE_SHAPES										14102
#define IDB_HEADLINE_SHAPES_VERT									14103
#define IDB_HEADLINE_POSITIONS									14104
#define IDB_HEADLINE_DEPTHS										14105
#define IDB_HEADLINE_DEPTHS2										14106
#define IDB_HEADLINE_OUTLINES										14107
#define IDB_HEADLINE_PROPORTION									14108
#define IDB_HEADLINE_PROPORTION_DISTORT						14109
#define IDB_HEADLINE_PATHS											14110
#define IDB_HEADLINE_PATHS_VERT									14111
#define IDB_HEADLINE_TOOLBAR_TEXT								14112
#define IDB_HEADLINE_GLOW											14113
#define IDB_HEADLINE_SHADOWS										14114


#define IDB_HEADLINE_READYMADE000								14200
#define IDB_HEADLINE_READYMADE001								14201
#define IDB_HEADLINE_READYMADE002								14202
#define IDB_HEADLINE_READYMADE003								14203
#define IDB_HEADLINE_READYMADE004								14204
#define IDB_HEADLINE_READYMADE005								14205
#define IDB_HEADLINE_READYMADE006								14206
#define IDB_HEADLINE_READYMADE007								14207
#define IDB_HEADLINE_READYMADE008								14208
#define IDB_HEADLINE_READYMADE009								14209
#define IDB_HEADLINE_READYMADE010								14210
#define IDB_HEADLINE_READYMADE011								14211
#define IDB_HEADLINE_READYMADE012								14212
#define IDB_HEADLINE_READYMADE013								14213
#define IDB_HEADLINE_READYMADE014								14214
#define IDB_HEADLINE_READYMADE015								14215
#define IDB_HEADLINE_READYMADE016								14216
#define IDB_HEADLINE_READYMADE017								14217
#define IDB_HEADLINE_READYMADE018								14218
#define IDB_HEADLINE_READYMADE019								14219
#define IDB_HEADLINE_READYMADE020								14220
#define IDB_HEADLINE_READYMADE021								14221
#define IDB_HEADLINE_READYMADE022								14222
#define IDB_HEADLINE_READYMADE023								14223
#define IDB_HEADLINE_READYMADE024								14224
#define IDB_HEADLINE_READYMADE025								14225
#define IDB_HEADLINE_READYMADE026								14226
#define IDB_HEADLINE_READYMADE027								14227
#define IDB_HEADLINE_READYMADE028								14228
#define IDB_HEADLINE_READYMADE029								14229
#define IDB_HEADLINE_READYMADE030								14230
#define IDB_HEADLINE_READYMADE031								14231
#define IDB_HEADLINE_READYMADE032								14232
#define IDB_HEADLINE_READYMADE033								14233
#define IDB_HEADLINE_READYMADE034								14234
#define IDB_HEADLINE_READYMADE035								14235
#define IDB_HEADLINE_READYMADE036								14236
#define IDB_HEADLINE_READYMADE037								14237
#define IDB_HEADLINE_READYMADE038								14238
#define IDB_HEADLINE_READYMADE039								14239
#define IDB_HEADLINE_READYMADE040								14240
#define IDB_HEADLINE_READYMADE041								14241
#define IDB_HEADLINE_READYMADE042								14242
#define IDB_HEADLINE_READYMADE043								14243
#define IDB_HEADLINE_READYMADE044								14244
#define IDB_HEADLINE_READYMADE045								14245
#define IDB_HEADLINE_READYMADE046								14246
#define IDB_HEADLINE_READYMADE047								14247
#define IDB_HEADLINE_READYMADE048								14248
#define IDB_HEADLINE_READYMADE049								14249
#define IDB_HEADLINE_READYMADE050								14250
#define IDB_HEADLINE_READYMADE051								14251
#define IDB_HEADLINE_READYMADE052								14252
#define IDB_HEADLINE_READYMADE053								14253
#define IDB_HEADLINE_READYMADE054								14254
#define IDB_HEADLINE_READYMADE055								14255
#define IDB_HEADLINE_READYMADE056								14256
#define IDB_HEADLINE_READYMADE057								14257
#define IDB_HEADLINE_READYMADE058								14258
#define IDB_HEADLINE_READYMADE059								14259
#define IDB_HEADLINE_READYMADE060								14260
#define IDB_HEADLINE_READYMADE061								14261
#define IDB_HEADLINE_READYMADE062								14262
#define IDB_HEADLINE_READYMADE063								14263
#define IDB_HEADLINE_READYMADE064								14264
#define IDB_HEADLINE_READYMADE065								14265
#define IDB_HEADLINE_READYMADE066								14266
#define IDB_HEADLINE_READYMADE067								14267
#define IDB_HEADLINE_READYMADE068								14268
#define IDB_HEADLINE_READYMADE069								14269
#define IDB_HEADLINE_READYMADE070								14270
#define IDB_HEADLINE_READYMADE071								14271
#define IDB_HEADLINE_READYMADE072								14272
#define IDB_HEADLINE_READYMADE073								14273
#define IDB_HEADLINE_READYMADE074								14274
#define IDB_HEADLINE_READYMADE075								14275
#define IDB_HEADLINE_READYMADE076								14276
#define IDB_HEADLINE_READYMADE077								14277
#define IDB_HEADLINE_READYMADE078								14278
#define IDB_HEADLINE_READYMADE079								14279
												
#define IDB_HEADLINE_FIRSTVERTICAL								14300

#define IDB_HEADLINE_READYMADE100								14300
#define IDB_HEADLINE_READYMADE101								14301
#define IDB_HEADLINE_READYMADE102								14302
#define IDB_HEADLINE_READYMADE103								14303
#define IDB_HEADLINE_READYMADE104								14304
#define IDB_HEADLINE_READYMADE105								14305
#define IDB_HEADLINE_READYMADE106								14306
#define IDB_HEADLINE_READYMADE107								14307
#define IDB_HEADLINE_READYMADE108								14308
#define IDB_HEADLINE_READYMADE109								14309
#define IDB_HEADLINE_READYMADE110								14310
#define IDB_HEADLINE_READYMADE111								14311
#define IDB_HEADLINE_READYMADE112								14312
#define IDB_HEADLINE_READYMADE113								14313
#define IDB_HEADLINE_READYMADE114								14314
#define IDB_HEADLINE_READYMADE115								14315
#define IDB_HEADLINE_READYMADE116								14316 
#define IDB_HEADLINE_READYMADE117								14317
#define IDB_HEADLINE_READYMADE118								14318
#define IDB_HEADLINE_READYMADE119								14319

#define IDB_HEADLINE_LASTREADYMADE								14319


// ****************************************************************************
//
//		Misc IDs: Interface
//
//		Range 14000 - 14199
//
// ****************************************************************************

#define ID_SOFTEFFECT_EXCLUSIONS									14000

// ****************************************************************************
//
//		String IDs: Component
//
//		Range 14000 - 14199
//
// ****************************************************************************

#define STRING_UNDO_EDIT_HEADLINE									14000
#define STRING_REDO_EDIT_HEADLINE									14001

#define STRING_HEADLINE_COMPONENT_CREATION						14100
#define STRING_HEADLINE_COMPONENT_DEFAULT_FONT					14101


// ****************************************************************************
//
//		String IDs: Interface
//
//		Range 14200 - 14399
//
// ****************************************************************************

#define STRING_HEADLINE_FACE_TAB										14200	
#define STRING_HEADLINE_SHAPE_TAB									14201
#define STRING_HEADLINE_POSITION_TAB								14202
#define STRING_HEADLINE_OUTLINE_TAB									14203
#define STRING_HEADLINE_SHADOW_TAB									14204
#define STRING_HEADLINE_DEPTH_TAB									14205
#define STRING_HEADLINE_PROPORTION_TAB								14206
#define STRING_HEADLINE_VERTICAL_TEXT_REQUIREMENT				14207


// ****************************************************************************
//
//		Misc IDs: Component
//
//		Range 14000 - 14299
//
// ****************************************************************************

#define WARP_PATH_START_VALUE									14100

#define PATH_SHAPE_10											14103
#define PATH_SHAPE_20											14104
#define PATH_SHAPE_30											14105
#define PATH_SHAPE_40											14106
#define PATH_SHAPE_50											14107
#define PATH_SHAPE_60											14123
#define PATH_SHAPE_70											14124
#define PATH_SHAPE_80											14125
#define PATH_SHAPE_90											14126
#define PATH_SHAPE_100											14127
#define PATH_SHAPE_110											14128
#define PATH_SHAPE_120											14129
#define PATH_SHAPE_130											14130

#define WARP_SHAPE_OVAL											14108
#define WARP_SHAPE_OVALLOWERHALF								14109
#define WARP_SHAPE_OVALUPPERHALF								14110
#define WARP_SHAPE_TRAPEZOIDWIDEBOTTOM							14111
#define WARP_SHAPE_TRAPEZOIDWIDELEFT							14112
#define WARP_SHAPE_TRAPEZOIDWIDERIGHT							14113
#define WARP_SHAPE_TRAPEZOIDWIDETOP								14114
#define WARP_SHAPE_RECTTALL										14115
#define WARP_SHAPE_RECTSLANTLEFT								14116
#define WARP_SHAPE_RECTSLANTRIGHT								14117
#define WARP_SHAPE_DIAMOND										14118
#define WARP_SHAPE_TRIANGLE										14119
#define WARP_SHAPE_BOWTIE										14120
#define WARP_SHAPE_ARC											14121

#define WARP_SHAPE_1_1											14130
#define WARP_SHAPE_1_2											14131
#define WARP_SHAPE_1_3											14132
#define WARP_SHAPE_1_4											14133
#define WARP_SHAPE_1_5											14134
#define WARP_SHAPE_1_6											14135
#define WARP_SHAPE_1_7											14136
#define WARP_SHAPE_1_8											14137
#define WARP_SHAPE_2_1											14138
#define WARP_SHAPE_2_2											14139
#define WARP_SHAPE_2_3											14140
#define WARP_SHAPE_2_4											14141
#define WARP_SHAPE_2_5											14142
#define WARP_SHAPE_2_6											14143
#define WARP_SHAPE_2_7											14144
#define WARP_SHAPE_2_8											14145
#define WARP_SHAPE_3_1											14146
#define WARP_SHAPE_3_2											14147
#define WARP_SHAPE_3_3											14148
#define WARP_SHAPE_3_4											14149
#define WARP_SHAPE_3_5											14150
#define WARP_SHAPE_3_6											14151
#define WARP_SHAPE_3_7											14152
#define WARP_SHAPE_3_8											14153
#define WARP_SHAPE_4_1											14154
#define WARP_SHAPE_4_2											14155
#define WARP_SHAPE_4_3											14156
#define WARP_SHAPE_4_4											14157
#define WARP_SHAPE_4_5											14158
#define WARP_SHAPE_4_6											14159
#define WARP_SHAPE_4_7											14160
#define WARP_SHAPE_4_8											14161
#define WARP_SHAPE_5_1											14162
#define WARP_SHAPE_5_2											14163
#define WARP_SHAPE_5_3											14164
#define WARP_SHAPE_5_4											14165
#define WARP_SHAPE_5_5											14166
#define WARP_SHAPE_5_6											14167
#define WARP_SHAPE_5_7											14168
#define WARP_SHAPE_5_8											14169
#define WARP_SHAPE_6_1											14170
#define WARP_SHAPE_6_2											14171
#define WARP_SHAPE_6_3											14172
#define WARP_SHAPE_6_4											14173
#define WARP_SHAPE_6_5											14174
#define WARP_SHAPE_6_6											14175
#define WARP_SHAPE_6_7											14176
#define WARP_SHAPE_6_8											14177
#define WARP_SHAPE_7_1											14178
#define WARP_SHAPE_7_2											14179
#define WARP_SHAPE_7_3											14180
#define WARP_SHAPE_8_1											14181
#define WARP_SHAPE_8_2											14182
#define WARP_SHAPE_8_3											14183
#define WARP_SHAPE_8_4											14184
#define WARP_SHAPE_8_5											14185
#define WARP_SHAPE_8_6											14186
#define WARP_SHAPE_9_1											14187
#define WARP_SHAPE_9_2											14188
#define WARP_SHAPE_9_3											14189
#define WARP_SHAPE_9_4											14190
#define WARP_SHAPE_9_5											14191
#define WARP_SHAPE_9_6											14192
#define WARP_SHAPE_9_7											14193
#define WARP_SHAPE_10_1											14194
#define WARP_SHAPE_10_2											14195
#define WARP_SHAPE_10_3											14196
#define WARP_SHAPE_10_4											14197
#define WARP_SHAPE_10_5											14198
#define WARP_SHAPE_10_6											14199

#endif // _HEADLINDCOMPRESOURCE_H_
