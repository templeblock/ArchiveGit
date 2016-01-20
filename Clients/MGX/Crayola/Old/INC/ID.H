// (c) Copyright	1991 MICROGRAFX, Inc., All Rights Reserved.
// This material is confidential and a trade secret.
// Permission to use this work for any purpose must be obtained
// in writing from: MICROGRAFX,	1303 E Arapaho, Richardson, TX

#ifndef ID_H
#define ID_H

//************************************************************************
// THIS FILE CONTAIN ID'S FOR ALL WINDOWS, MENUS, DIALOGS, AND CONTROLS
// NOTHING IN THIS FILE SHOULD BE MODIFIED FOR LOCALIZATION
//************************************************************************

#ifdef ADVENT
#define ID_APPCLASS			"ADVENT_MAIN"
#else
#define ID_APPCLASS			"STUDIO_MAIN"
#endif

#define	ID_MAGIC			100
#define ID_ACCEL			101

/* Print cancel controls */
#define ID_NAME				110
#define ID_PRINTER			111
#define ID_PORT				112
#define ID_FILENAME			113

// Miscellaneous cursors
#define ID_ARROW			32512
#define ID_NULL				500
#define ID_CLOCK01			501
#define ID_CLOCK02			502
#define ID_CLOCK03			503
#define ID_CLOCK04			504
#define ID_CLOCK05			505
#define ID_CLOCK06			506
#define ID_CLOCK07			507
#define ID_CLOCK08			508
#define ID_CLOCK09			509
#define ID_CLOCK10			510
#define ID_CLOCK11			511
#define ID_CLOCK12			512
#define ID_BALL1			513
#define ID_BALL2			514
#define ID_BALL3			515
#define ID_BALL4			516
#define ID_HAND01			517
#define ID_HAND02			518

// The tool cursors
#define ID_LINES			530
#define ID_SHAPES			531
#define ID_TOY				532
#define ID_EFFECTS			533
#define ID_TRANSFORMER		534
#define ID_ERASE			536
#define ID_TEXT				537
#define ID_FILL				538
#define ID_STAMP			539

#define ID_PLAY_OBJECT		540
#define ID_NO_ACTION		541
#define ID_CUT				542

#define LAST_CURSOR			600

#define ID_MOVE_OBJECT		32640 // Windows ID_SIZE
#define LAST_ICON			31999

//	1960-1990 id's for full names for file types starting at	1990
// The image file types
#define IDN_FIRSTFILETYPE	1980
#define IDN_ART				1980
#define IDN_TIFF			1981
#define IDN_BMP				1982

// The extended name id's
#define IDN_FIRSTEXTTYPE	IDN_STYLE
#define IDN_STYLE			2000
#define IDN_HALFTONE		2001
#define IDN_CALMAP			2002
#define IDN_SEPARATE		2003
#define IDN_BGMAP			2004
#define IDN_EFFECTS			2005
#define IDN_TEXTURE			2006
#define IDN_CLIPBOARD		2007
#define IDN_MASK			2008
#define IDN_CUSTOM			2009
#define IDN_PALETTE			2010
#define IDN_STAMP			2011
#define IDN_TOY				2012
#define IDN_FIRST_RETOUCHSTYLE IDN_PAINTSTYLE
#define IDN_PAINTSTYLE		2013
#define IDN_CRAYONSTYLE		2014
#define IDN_MARKERSTYLE		2015
#define IDN_LINESTYLE		2016
#define IDN_SHAPESTYLE		2017
#define IDN_ERASESTYLE		2018
#define IDN_LASTEXTTYPE		IDN_ERASESTYLE

/* Leave an equal number of slots below for "add" strings */
#define MAX_FILETYPES		100 // 2100-2199 reserved

// Popup dialog box ID's
#define IDD_OPEN			4100
#define IDD_SAVEAS			4101
#define IDD_ABORT			4102

#define IDD_PREF			4105
#define IDD_ABOUT			4106
#define IDD_WARNING			4107
#define IDD_VIEWFULL		4108
#define IDD_PRINTSETUP		4109

#define IDD_HERALD			4114
#define IDD_COMBO			4115
#define IDD_MAIN			4116
#define IDD_ROOM			4117
#define IDD_STATUS			4118
#define IDD_ASK_REGISTER	4119
#define IDD_ASK_UNLOCK		4120
#define IDD_SLIDE_EFFSETUP	4121
#define IDD_SAVEFLOPPY		4122
#define IDD_LOADFLOPPY		4123

// Tool Option Ribbon ID's
#define IDD_PAINT			4200
#define IDD_CRAYON			4201
#define IDD_MARKER			4202
#define IDD_LINES			4203
#define IDD_SHAPES			4204
#define IDD_ERASER			4205
#define IDD_STAMP			4206
#define IDD_FILL			4207
#define IDD_TOY				4208
#define IDD_EFFECTS			4209
#define IDD_TEXT			4210
#define IDD_TRANSFORMER		4211
#define IDD_GALLERY			4212

// Room controls go from 5000-10000

// Generic hotspot id's				5000-
#define HS_EXIT             5000
#define HS_GALLERY          5001
#define HS_HORN             5002
#define HS_EASEL            5003
#define HS_TOUR             5004
#define HS_BACKBOOKS        5005
#define HS_BACKART          5006
#define HS_COLORME          5007
#define HS_COLORME_ME       HS_COLORME
#define HS_COVER            5008
#define HS_SHOWME           5009
#define HS_SHOW_ME          HS_SHOWME
#define HS_TAB1             5010
#define HS_TAB2             5011
#define HS_TAB3             5012
#define HS_TAB4             5013
#define HS_TAB5             5014
#define HS_TAB6             5015
#define HS_TAB7             5016
#define HS_TAB8             5017
#define HS_NEXT             5018
#define HS_GIVEUP           5019
#define HS_RESTART          5020
#define IDC_TIMER_BLK       5022
#define IDC_TIMER_TEXT      5023
#define HS_BACKSHOW         5024
#define IDC_ADDONROOM		5025

#define IDC_COVER_ANIMATE1	5050
#define IDC_COVER_ANIMATE2	5051
#define IDC_COVER_ANIMATE3	5052
#define IDC_COVER_ANIMATE4	5053
#define IDC_COVER_ANIMATE5	5054
#define IDC_COVER_ANIMATE6	5055
#define IDC_COVER_ANIMATE7	5056
#define IDC_COVER_ANIMATE8	5057
#define IDC_COVER_ANIMATE9	5058
#define IDC_COVER_ANIMATE10	5059

// Opening "room"					5100-
#define HS_OPENING       	5100
#define IDC_MGX_LOGO        5101
#define IDC_FUTURE_LOGO     5102
#define IDC_PRESENTS_LOGO   5103

// Who are you "room"				5200-
#define HS_WHO              5200
#define HS_WHO_UPARROW		5201
#define HS_WHO_DNARROW		5202
#define IDC_WHO_BUTTON1		5203
#define IDC_WHO_BUTTON2		5204
#define IDC_WHO_BUTTON3		5205
#define IDC_WHO_BUTTON4		5206
#define IDC_WHO_NEWBUTTON	5207
#define IDC_WHO_EDIT		5208
#define IDC_WHO_DELNAME		5209

// Gallery "room"					5300-
#define HS_GALLERY_GOPAINT  5300
#define IDC_GALLERY_SW1		5301
#define IDC_GALLERY_SW1a	5302
#define HS_GALLERY_TRASH    5303
#define HS_GALLERY_SLIDES   5304
#define HS_GALLERY_BEFORE   5305
#define HS_GALLERY_NEXT     5306
// Do not change the order of the following ID's
#define HS_GALLERY_TEXT1	5307
#define HS_GALLERY_TEXT2	5308
#define HS_GALLERY_TEXT3	5309
#define HS_GALLERY_TEXT4	5310
#define HS_GALLERY_TEXT5	5311
#define HS_GALLERY_TEXT6	5312
#define HS_GALLERY_TEXT7	5313
#define HS_GALLERY_TEXT8	5314
#define HS_GALLERY_TEXT9	5315
#define HS_GALLERY_TEXT10	5316
#define HS_GALLERY_TEXT11	5317
#define HS_GALLERY_TEXT12	5318
#define HS_GALLERY_TEXT13	5319
#define HS_GALLERY_TEXT14	5320
#define HS_GALLERY_TEXT15	5321
#define HS_GALLERY_TEXT16	5322
#define HS_GALLERY_TEXT17	5323
#define HS_GALLERY_TEXT18	5324
#define HS_GALLERY_TEXT19	5325
#define HS_GALLERY_TEXT20	5326

// Studio activity "room"			6000-
#define HS_BADGE            6000
#define HS_BOOKS            6001
#define HS_CERTIFICATE      6002
#define HS_PAPER            6003
#define HS_PUZZLE           6004
#define HS_SLIDESHOW		6005
#define HS_JOKEPHONE		6006

// Slide show maker "room"			6100-
#define HS_SLIDE_PLAYMOVIE	6100
#define IDC_SLIDE_IMAGE1	6101
#define IDC_SLIDE_IMAGE2	6102
#define IDC_SLIDE_IMAGE3	6103
#define IDC_SLIDE_IMAGE4	6104
#define IDC_SLIDE_IMAGE5	6105
#define IDC_SLIDE_IMAGE6	6106
#define IDC_SLIDE_IMAGE7	6107
#define IDC_SLIDE_IMAGE8	6108
#define IDC_SLIDE_IMAGE9	6109
#define IDC_SLIDE_IMAGE10	6110
#define IDC_SLIDE_IMAGE11	6111
#define IDC_SLIDE_IMAGE12	6112
#define HS_SLIDEPLAY_QUIT	6113
#define IDC_SLIDEPLAY_IMAGE	6114
// Do NOT change the order of these !
#define HS_IMAGE1_DEL		6115
#define HS_IMAGE2_DEL		6116
#define HS_IMAGE3_DEL		6117
#define HS_IMAGE4_DEL		6118
#define HS_IMAGE5_DEL		6119
#define HS_IMAGE6_DEL		6120
#define HS_IMAGE7_DEL		6121
#define HS_IMAGE8_DEL		6122
#define HS_IMAGE1_INS		6123
#define HS_IMAGE2_INS		6124
#define HS_IMAGE3_INS		6125
#define HS_IMAGE4_INS		6126
#define HS_IMAGE5_INS		6127
#define HS_IMAGE6_INS		6128
#define HS_IMAGE7_INS		6129
#define HS_IMAGE8_INS		6130
#define HS_IMAGE1_EFF		6131
#define HS_IMAGE2_EFF		6132
#define HS_IMAGE3_EFF		6133
#define HS_IMAGE4_EFF		6134
#define HS_IMAGE5_EFF		6135
#define HS_IMAGE6_EFF		6136
#define HS_IMAGE7_EFF		6137
#define HS_IMAGE8_EFF		6138
#define HS_SLIDE_NEXTPAGE	6139
#define HS_SLIDE_PREVPAGE	6140
#define HS_EFFSETUP_EDIT	6141
#define IDC_EFFSETUP_EDIT	6142
#define IDC_EFFSETUP_SOUNDLIST 6143
#define IDC_EFFSETUP_TRANLIST 6144
#define IDC_SLIDEPLAY_STOP	6145
#define IDC_SLIDEPLAY_START	6146

// Badge maker "room"				6200-
#define HS_BADGE1           6200
#define HS_BADGE2           6201
#define HS_BADGE3           6202
#define HS_BADGE4           6203
#define HS_BADGE5           6204
#define HS_BADGE6           6205
#define HS_BADGE7           6206
#define HS_BADGE8           6207
#define HS_BADGE9           6208
#define HS_BADGE10          6209
#define HS_BADGE11          6210
#define HS_BADGE12          6211

// Certificate maker "room"			6300-
#define HS_CERTIFICATE1     6300
#define HS_CERTIFICATE2     6301
#define HS_CERTIFICATE3     6302
#define HS_CERTIFICATE4     6303
#define HS_CERTIFICATE5     6304
#define HS_CERTIFICATE6     6305
#define HS_CERTIFICATE7     6306
#define HS_CERTIFICATE8     6307

// Note paper maker "room"			6400-
#define HS_PAPER1           6400
#define HS_PAPER2           6401
#define HS_PAPER3           6402
#define HS_PAPER4           6403
#define HS_PAPER5           6404
#define HS_PAPER6           6405
#define HS_PAPER7           6406
#define HS_PAPER8           6407
#define HS_PAPER9           6408
#define HS_PAPER10          6409

// Puzzler "rooms"					6500-
#define HS_PUZZLE_EASY      6500
#define HS_PUZZLE_TOUGH     6501
#define HS_PUZZLE_HARD      6502

#define HS_PUZZLE_NEWGAME   6510
#define IDC_PUZZLE_NAME     6511
#define IDC_PUZZLE_IMAGE    6512
#define IDC_PUZZLE_CHANGES  6513
#define HS_BACKPUZZLE       6514
#define HS_SHOWMEPUZZLE     6515

// Coloring book "room"				6600
#define HS_BOOK1            6600
#define HS_BOOK2            6601
#define HS_BOOK3            6602
#define HS_BOOK4            6603
#define HS_BOOK5            6604
#define HS_BOOK6            6605
#define HS_BOOK7            6606
#define HS_BOOK8            6607

// Vehicle designer "room"			6700-
#define HS_LOGO_VEHICLE     6700
#define HS_MIXUP_FRONT      6701
#define HS_MIXUP_MIDDLE     6702
#define HS_MIXUP_REAR       6703

// Fashion designer "room"			6800-
#define HS_LOGO_FASHION     6800
#define HS_MIXUP_FHEAD      6801
#define HS_MIXUP_FTOP       6802
#define HS_MIXUP_FBOTTOM    6803

// Boardwalk hidden picture "room"	6900-
#define HS_LOGO_BOARDWALK   6900
#define HS_XTRA_BOARDWALK   6901

// My Trip to Europe "room"			7000-
#define HS_LOGO_EUROPE      7000
#define HS_XTRA_EUROPE      7001

// Safari "room"					7100-
#define HS_LOGO_SAFARI      7100
#define HS_XTRA_SAFARI      7101

// Where in the World "room"		7200-
#define HS_LOGO_WORLD       7200
#define HS_XTRA_WORLD       7201
#define IDC_WORLD_ANSWER1   7202
#define IDC_WORLD_ANSWER2   7203
#define IDC_WORLD_ANSWER3   7204
#define IDC_WORLD_ANSWER4   7205

// Jokes "room"						7300-
#define HS_LOGO_JOKES       7300
#define HS_XTRA_JOKES       7301

// Mail maze "room"					7400-
#define HS_LOGO_MAILMAZE    7400
#define HS_XTRA_MAILMAZE    7401

// Adventure activity "room"		8000-
#define HS_BURGER           8000
#define HS_CAKE             8001
#define HS_CLOCK            8002
#define HS_CLOWN            8003
#define HS_DOG              8004
#define HS_JACK             8005
#define HS_LAMP             8006
#define HS_MAZE             8007
#define HS_MONKEY           8008
#define HS_MONSTER          8009
#define HS_PLACEMAT         8010
#define HS_ROCKET           8011
#define HS_SHAPES           8012
#define HS_SPACESHIP        8013
#define HS_TOASTER          8014
#define HS_VEGGIES          8015

// Placemat maker "room"			8100-
#define HS_PLACEMAT1        8100
#define HS_PLACEMAT2        8101
#define HS_PLACEMAT3        8102
#define HS_PLACEMAT4        8103
#define HS_PLACEMAT5        8104
#define HS_PLACEMAT6        8105
#define HS_PLACEMAT7        8106
#define HS_PLACEMAT8        8107

// Circus dot "room"				8200-
#define HS_LOGO_CIRCUS      8200
#define HS_XTRA_CIRCUS      8201

// Dragon's birthday "room"			8300-
#define HS_LOGO_DRAGON      8300
#define HS_XTRA_DRAGON      8301

// Jungle adventure "room"			8400-
#define HS_LOGO_JUNGLE      8400
#define HS_XTRA_JUNGLE      8401

// Monster maze craze "room"		8500-
#define HS_LOGO_MAZES       8500
#define HS_XTRA_MAZES       8501

// Monster mixup room "room"		8600-
#define HS_LOGO_MONSTER     8600
#define HS_MIXUP_HEAD       8601
#define HS_MIXUP_BODY       8602
#define HS_MIXUP_LEGS       8603

// Spaceball paint "room"			8700-
#define HS_LOGO_SPACEBALL   8700
#define HS_XTRA_SPACEBALL   8701
#define IDC_SPOTDOT_IMAGE   8702

// Sets and subsets "room"			8800-
#define HS_LOGO_SUBSETS     8800
#define HS_XTRA_SUBSETS     8801
#define IDC_SS_CLOWN		8802
#define IDC_SS_PINEAPPLE	8803
#define IDC_SS_PAN			8804
#define IDC_SS_PAROT1		8805
#define IDC_SS_CARROT		8806
#define IDC_SS_CRAB1		8807
#define IDC_SS_PIG			8808
#define IDC_SS_ELEPHANT		8809
#define IDC_SS_GFISH		8810
#define IDC_SS_SAW			8811
#define IDC_SS_APPLE		8812
#define IDC_SS_COW			8813
#define IDC_SS_TOUCAN1		8814
#define IDC_SS_FOOTBALL		8815
#define IDC_SS_GIRAFFE		8816
#define IDC_SS_OCTOPUS		8817
#define IDC_SS_BUTFLY		8818
#define IDC_SS_W1			8819
#define IDC_SS_H1			8820
#define IDC_SS_A1			8821
#define IDC_SS_T1			8822
#define IDC_SS_S1			8823
#define IDC_SS_I			8824
#define IDC_SS_N			8825
#define IDC_SS_QUEST1		8826
#define IDC_SS_W2			8827
#define IDC_SS_H2			8828
#define IDC_SS_A2			8829
#define IDC_SS_T2			8830
#define IDC_SS_S2			8831
#define IDC_SS_O			8832
#define IDC_SS_U			8833
#define IDC_SS_T3			8834
#define IDC_SS_QUEST2		8835

// Hidden Shapes "room"				8900-
#define HS_LOGO_SHAPES      8900
#define HS_XTRA_SHAPES      8901
#define HS_BLOCK_TEXT       8902
#define IDC_HT_HAT			8903
#define IDC_HT_IGLOO		8904
#define IDC_HT_DOG			8905
#define IDC_HT_BBEYES		8906
#define IDC_HT_NECKLACE		8907
#define IDC_HT_TREE			8908
#define IDC_HT_RACOON1		8909
#define IDC_HT_LBEYES		8910
#define IDC_HT_SAW			8911
#define IDC_HT_APPLE		8912
#define IDC_HT_UNICORN		8913
#define IDC_HT_RACOON2		8914
#define IDC_HT_GEYES		8915
#define HS_HT_HAT			8916
#define HS_HT_IGLOO			8917
#define HS_HT_DOG			8918
#define HS_HT_BBEYES		8919
#define HS_HT_NECKLACE		8920
#define HS_HT_TREE			8921
#define HS_HT_RACOON1		8922
#define HS_HT_LBEYES		8923
#define HS_HT_SAW			8924
#define HS_HT_APPLE			8925
#define HS_HT_UNICORN		8926
#define HS_HT_RACOON2		8927
#define HS_HT_GEYES			8928

// What's Wrong "room"				9000-
#define HS_LOGO_WRONG       9000
#define HS_XTRA_WRONG       9001
#define HS_WRONG_LBULBS		9002
#define HS_WRONG_APPLES		9003
#define HS_WRONG_PIZZA		9004
#define HS_WRONG_NUT		9005
#define HS_WRONG_BEAR		9006
#define HS_WRONG_FISH		9007
#define HS_WRONG_PINE		9008
#define HS_WRONG_PALM		9009
#define HS_WRONG_DUCK1		9010
#define HS_WRONG_DUCK2		9011
#define HS_WRONG_PIG		9012
#define HS_WRONG_BIRD		9013

/* All IDC_??? control go from 10000-20000 */

/* Generic dialog controls */
#define IDC_SAVE			10001
#define IDC_COMBOBOX		10002
#define IDC_SHARP			10003 // needed for blend, stroke
#define IDC_SMOOTH			10004
#define IDC_MERGEMODE		10005
#define IDC_CAT_SCARED		10006
#define IDC_CAT_WALK		10007

#define IDC_GALLERY_IMAGE	10008   // image control
#define IDC_HIDDEN_IMAGE 	10009   // hidden pics image object-list-control
#define IDC_HIDDEN_LEGEND	10010   // hidden pics legend image-control
#define IDC_MIXUP_IMAGE		10011   // mixup image control
#define IDC_GALLERY_EDIT	10012   // edit control
#define IDC_SHAPES_IMAGE	10013   // hidden shapes image control

// Various bitmaps
#define IDC_AMBULANCE1		10050
#define IDC_AMBULANCE2		10051

// The Crayola tools
//
// ATTN: These tool ids should map to their corresponding tool sound ids
//       by adding TS_TOOLSOUND_OFFSET defined below!
//
#define IDC_FIRST_RETOUCHTOOL IDC_PAINT
#define IDC_PAINT			10100
#define IDC_CRAYON			10101
#define IDC_MARKER			10102
#define IDC_LINES			10103
#define IDC_SHAPES			10104
#define IDC_ERASER			10105
#define IDC_STAMP			10106
#define IDC_FILL			10107
#define IDC_TOY				10108
#define IDC_EFFECTS			10109
#define IDC_TEXT			10110
#define IDC_TRANSFORMER		10111
#define IDC_GALLERY			10112

#define IDC_SOUND			10150
#define IDC_CUSTOMVIEW		10151
#define IDC_UNDO			10152
#define IDC_GOBACK			10153

// The Crayola tool sounds
//
// ATTN: These tool sound ids should map to their corresponding tool ids
//       by subtracting TS_TOOLSOUND_OFFSET defined below!
//
#define TS_TOOLSOUND_OFFSET 100
#define TS_PAINT			10200
#define TS_CRAYON			10201
#define TS_MARKER			10202
#define TS_LINES			10203
#define TS_SHAPES			10204
#define TS_ERASER			10205
#define TS_STAMP			10206
#define TS_FILL 			10207
#define TS_TOY				10208
#define TS_EFFECTS			10209
#define TS_TEXT 			10210
#define TS_TRANSFORMER		10211
#define TS_GALLERY			10212

#define TS_SOUND			10250
#define TS_CUSTOMVIEW		10251
#define TS_UNDO 			10252
#define TS_GOBACK			10253

/* The Crayola view accel's */
#define IDC_ZOOMIN			10210
#define IDC_ZOOMOUT			10211
#define IDC_VIEWALL			10212
#define IDC_VIEWLAST		10213
#define IDC_VIEWFULL		10214

/* Palette controls start at 10220 */
#define IDC_COLORS_CRAYON	10221
#define IDC_COLORS_MARKER	10222
#define IDC_COLORS_PAINT	10223
#define IDC_COLORS_TEXT		10224
#define IDC_COLORS_LINES	10225
#define IDC_COLORS_FILL		10226
#define IDC_COLORS_SHAPES	10227

#define IDC_COLOR_FIRST  	10228
#define IDC_COLOR_RAINBOW   10228
#define IDC_COLOR_BLACK     10229
#define IDC_COLOR_RED       10230
#define IDC_COLOR_ORANGE    10231
#define IDC_COLOR_YELLOW    10232
#define IDC_COLOR_GREEN     10233
#define IDC_COLOR_DKCYAN    10234
#define IDC_COLOR_BLUE      10235
#define IDC_COLOR_PURPLE    10236
#define IDC_COLOR_MAGENTA   10237
#define IDC_COLOR_TAN       10238
#define IDC_COLOR_BROWN     10239
#define IDC_COLOR_WHITE     10240
#define IDC_COLOR_GRAY      10241
#define IDC_COLOR_ROSE      10242
#define IDC_COLOR_SAND      10243
#define IDC_COLOR_LTYELLOW  10244
#define IDC_COLOR_LTGREEN   10245
#define IDC_COLOR_CYAN      10246
#define IDC_COLOR_LAVENDER  10247
#define IDC_COLOR_LTPURPLE  10248
#define IDC_COLOR_LTMAGENTA 10249
#define IDC_COLOR_LTTAN     10250
#define IDC_COLOR_LTBROWN   10251
#define IDC_COLOR_LAST  	10252

#define IDC_ACTIVECOLOR		10290

// Wallpaper open/save tools
#define IDC_TILEDWP			10300
#define IDC_CENTEREDWP		10301
#define IDC_24BITWALL		10302

/* Preferences controls start at	10500 */
#define IDC_PREF_SAVE			10500
#define IDC_PREF_SMALL			10501
#define IDC_PREF_MEDIUM			10502
#define IDC_PREF_LARGE			10503
#define IDC_PREF_UNDO			10504
#define IDC_PREF_SAVER			10505
#define IDC_PREF_ANIMATIONS		10506
#define IDC_PREF_NOTHEME		10507
#define IDC_PREF_USEWAVEMIX		10508
//#define IDC_PREF_BUBBLE		10509
//#define IDC_PREF_AUDIO		10510
//#define IDC_PREF_BOTH			10511

// The units group
#define IDC_PREF_UNITS			10513
#define IDC_PREF_UNITINCHES		10514
#define IDC_PREF_UNITMM			10515
#define IDC_PREF_UNITPICAS		10516
#define IDC_PREF_UNITCM			10517
#define IDC_PREF_UNITPIXELS		10518
#define IDC_PREF_UNITFIRST		IDC_PREF_UNITINCHES
#define IDC_PREF_UNITLAST		IDC_PREF_UNITPIXELS

#define IDC_PREF_TABLET		10551
#define IDC_PREF_TABLETUNUSED	10552
#define IDC_PREF_TABLETOPACITY	10553
#define IDC_PREF_TABLETSIZE	10554
#define IDC_PREF_TABLETBOTH	10555

/* File open control start at	10600 */
#define IDC_FNAME			10600
#define IDC_FPATH			10601
#define IDC_FLIST			10602
#define IDC_DIRS			10603
#define IDC_DRIVES			10604

#define IDC_DISKSPACE		10609
#define IDC_SAVEPATH		10610

#define IDC_CANCEL			10659

#define IDC_PATHICON_DRIVE		10671
#define IDC_PATHICON_ROOT		10672
#define IDC_PATHICON_OPEN		10673
#define IDC_PATHICON_ACTIVE		10674
#define IDC_PATHICON_CLOSED		10675
#define IDC_PATHICON_FIXED		10676
#define IDC_PATHICON_NET		10677
#define IDC_PATHICON_FLOPPY		10678
#define IDC_PATHICON_CDDRIVE	10679
#define IDC_PATHICON_RAMDRIVE	10680

/* Save as controls start at 10700 */
#define	IDC_FILETYPES		10700
#define IDC_ART				10701
#define	IDC_TIFF			10702
#define	IDC_BMP				10703

#define IDC_DATATYPES		10713
#define IDC_SAVECT			10714
#define IDC_SAVELA			10715
#define IDC_SAVESP			10716
#define IDC_SAVE4BITCOLOR	10717
#define IDC_SAVE8BITCOLOR	10718
#define IDC_SAVE24BITCOLOR	10719
#define IDC_SAVE32BITCOLOR	10720
#define	IDC_SAVECOMPRESS	10721

/* About box controls start at	10750 */
#define VERSION_ICON		10750
#define IDC_CAST			10751
#define READ_ME				10752

#define IDC_SERIALNO		10760
#define IDC_VERSION_NAME	10761
#define IDC_CASTTEXT		10762

#define IDC_ABOUTAPP		10767
#define IDC_CUSTOMER		10768
#define IDC_COMPANY			10769

/* Warning box controls start at	10800 */
#define SPA_BITMAP			10800
#define MGX_BITMAP			10801
#define TXTBLK_WARNING		10802

/* The Transformer options controls start	11000 */
#define IDC_SMARTPASTE		11004
#define IDC_PASTEINTOMASK	11005
#define IDC_ASPECTFIRST		11006

/* Brush controls start at	11500 */
#define IDC_BRUSHSHAPE		    11505
#define IDC_BRUSHCIRCLE		    11506
#define IDC_BRUSHSQUARE		    11507
#define IDC_BRUSHHORIZONTAL	    11508
#define IDC_BRUSHVERTICAL	    11509
#define IDC_BRUSHHORZHAIR	    11510
#define IDC_BRUSHVERTHAIR	    11511
#define IDC_BRUSHSLASH		    11512
#define IDC_BRUSHBACKSLASH	    11513
#define IDC_BRUSHSLASHHAIR	    11514
#define IDC_BRUSHBACKSLASHHAIR	11515
#define IDC_BRUSHSQUAREHAIR	    11516
#define IDC_BRUSHCUSTOM		    11517
#define IDC_BRUSHCUSTOMNEW	    11518
#define IDC_BRUSHSIZE		    11519
#define IDC_BRUSHICON		    11520
#define IDC_FLOODRANGE		    11521
#define IDC_FEATHER			    11522
#define IDC_MIDPOINT		    11523
#define IDC_TRANSPARENT		    11524
#define IDC_TRANSPARENT_SCROLL	11525
#define IDC_PRESSURE		    11526
#define IDC_OPACITYPRESSURE	    11527
#define IDC_BRUSHHEXCIRCLE	    11528
#define IDC_BRUSHHOLLOWCIRCLE   11529
#define IDC_BRUSHGLITTERCIRCLE  11530
#define IDC_BRUSHSWIRLCIRCLE    11531

#define IDC_BRUSHSOLID		11561
#define IDC_BRUSHSCATTER	11562

#define IDC_COLORMODEL		11611
#define IDC_FIRST_MODEL		11612
#define IDC_MODEL_RGB		11612
#define IDC_MODEL_HL		11613
#define IDC_MODEL_HSL		11614
#define IDC_MODEL_LUM		11615
#define IDC_MODEL_HUE		11616
#define IDC_MODEL_SAT		11617
#define IDC_MODEL_HS		11618
#define IDC_MODEL_SL		11619
#define IDC_LAST_MODEL		IDC_MODEL_HL

/* Process controls start at	11900 */
#define IDC_HEIGHT			11900
#define IDC_WIDTH			11901
#define IDC_RES				11902
#define IDC_SCALEX			11903
#define IDC_SCALEY			11904
#define IDC_MEMORY			11905
#define IDC_DISTORT			11906
#define IDC_SMARTSIZE		11907
#define IDC_MAINTAINSIZE	11909

/* Print and Print Setup controls start at	12200 */
#define IDC_PRINTER			12200
#define IDC_PRINTVIEW		12202

#define IDC_PRINTVIEWEXT	12205
#define IDC_PRINTVIEWPHYS	12206
#define IDC_PRINTVIEWPAGE	12207
#define IDC_PRINTVIEWIMAGE	12208

#define IDC_FIRSTPRINTITEM	12209
#define IDC_CENTERED		12212
#define IDC_SCATTER			12213
#define IDC_PRINTRESET		12214
#define IDC_PRINTSELECT		12216
#define IDC_PRINTSETUP		12217

#define IDC_PRINT_TYPE		12250
#define IDC_PRINT_GRAY		12251
#define IDC_PRINT_COLOR		12252
#define IDC_PRINT_COLORGRAY	12253
#define IDC_PRINT_BLACKSEPS	12254
#define IDC_PRINT_COLORSEPS	12255

/* Print Setup Controls start at	12300 */
#define IDC_BINARY_PS		12309

#define IDC_PRINTERTYPES	12320
#define IDC_PRINTER_IS_MONO	12321
#define IDC_PRINTER_IS_CMYK	12322
#define IDC_PRINTER_IS_RGB	12323

/* Print Halftoning controls start at	12500 */
#define IDC_CIRCLEDOT		12501
#define IDC_SQUAREDOT		12502
#define IDC_ELLIPSEDOT		12503
#define IDC_TRIANGLEDOT		12503 // Duplicate - UNUSED
#define IDC_PROPELLERDOT	12503 // Duplictae - UNUSED

/* Vignette Controls start at	13000 */
#define IDC_VIGNETTETYPE	13000
#define IDC_VIGLINEAR		13001
#define IDC_VIGRADIAL		13002
#define IDC_VIGCIRCLE		13003
#define IDC_VIGELLIPSE		13004
#define IDC_VIGSQUARE		13005
#define IDC_VIGRECTANGLE	13006
#define IDC_VIGREPEAT		13007
#define IDC_VIGTRANSITION	13008
#define IDC_VIGHARDTRANSITION	13009
#define IDC_VIGSOFTTRANSITION	13010

/* Texture Fill Controls start at	13100 */
#define IDC_TEXTUREFLIPX	13101
#define IDC_TEXTUREFLIPY	13102

/* Text Controls start at	13400 */
#define IDC_TEXT_SIZE		13400
#define IDC_TEXT_ANGLE		13401

#define IDC_STATICTEXT		13412
#define IDC_GETDLGSTRING_TEXT	13413

/* Status Box Controls start at	13600 */
#define IDC_STATUSBUBBLE    13600
#define IDC_STATUSTEXT		13601
#define IDC_STATUSBITMAP    13602

/* Extended Name ComboBox Control start at	13700 */
#define IDC_EXTNAMES		13700
#define IDC_FILENAME		13701
#define IDC_EXTMANAGE		13702
#define IDC_ADDEXT			13709
#define IDC_DELETEEXT		13710
#define IDC_RENAMEEXT		13711

/* Mask transformer controls start at	14500 */
#define IDC_TRANSFUNCTION	14500
#define IDC_COPYIMAGE		14501
#define IDC_MOVEIMAGE		14502

#define IDC_TRANSMODE		14510
#define IDC_TRANSSCALE		14511
#define IDC_TRANSSKEW		14512
#define IDC_TRANSPERSPECTIVE	14513
#define IDC_TRANSDISTORT	14514
#define IDC_TRANSMODEFIRST	IDC_TRANSSCALE
#define IDC_TRANSMODELAST	IDC_TRANSDISTORT

#define IDC_TRANSFLIPX		14520
#define IDC_TRANSFLIPY		14521
#define IDC_TRANSROTATEXY	14522
#define IDC_TRANSROTATEX	14523
#define IDC_TRANSROTATEY	14524

/* Convert To... Options ribbon controls start at	15300 */
#define IDC_MINITYPES		15340
#define IDC_MINI256			15341
#define IDC_MINI16			15342
#define IDC_MINI8			15343

#define IDC_DITHERTYPES		15360
#define IDC_DITHERPATTERN	15361
#define IDC_DITHERSCATTERED	15362
#define IDC_DITHERNONE		15363


// Room ID's
#define RM_FIRST            16000       // id of first room in list
#define RM_OPENING          16000
#define RM_WHOAREYOU        16001
#define RM_ARTROOM          16002
#define RM_BADGES           16003
#define RM_BOARDWALK        16004
#define RM_BOOKS            16005
#define RM_CERTIFICATES     16006
#define RM_CIRCUS           16007
#define RM_DRAGON           16008
#define RM_EUROPE           16009
#define RM_FASHION          16010
#define RM_GALLERY          16011
#define RM_JOKES            16012
#define RM_JUNGLE           16013
#define RM_MAILMAZE         16014
#define RM_MAZES            16015
#define RM_MONSTER          16016
#define RM_PAPER            16017
#define RM_PLACEMATS        16018
#define RM_PUZZLE           16019
#define RM_SAFARI           16020
#define RM_SHAPES           16021
#define RM_SPACEBALL        16022
#define RM_SUBSETS          16023
#define RM_VEHICLE          16024
#define RM_WORLD            16025
#define RM_PUZZLESTART      16026
#define RM_SLIDE			16027
#define RM_SLIDEPLAY		16028
#define RM_WRONG            16029
#define RM_ADDON            16030
#define RM_LAST             RM_ADDON    // id of last room in list


// Crayon Tool Options 17000-
#define IDC_CRAYON_T1		17001
#define IDC_CRAYON_T2		17002
#define IDC_CRAYON_T3		17003
#define IDC_CRAYON_T4		17004
#define IDC_CRAYON_T5		17005
#define IDC_CRAYON_T6		17006
#define IDC_CRAYON_T7		17007
#define IDC_CRAYON_T8		17008
#define IDC_CRAYON_T9		17009
#define IDC_CRAYON_T10		17010
#define IDC_CRAYON_SW1		17011
#define IDC_CRAYON_SW1a		17012
#define IDC_CRAYON_SL		17013

// Marker Tool Options 17100-
#define IDC_MARKER_T1		17101
#define IDC_MARKER_T2		17102
#define IDC_MARKER_T3		17103
#define IDC_MARKER_T4		17104
#define IDC_MARKER_T5		17105
#define IDC_MARKER_T6		17106
#define IDC_MARKER_T7		17107
#define IDC_MARKER_T8		17108
#define IDC_MARKER_T9		17109
#define IDC_MARKER_T10		17110
#define IDC_MARKER_SW1		17111
#define IDC_MARKER_SW1a		17112
#define IDC_MARKER_SL		17113

// Paint (watercolor) Tool Options 17200-
#define IDC_PAINT_T1		17201
#define IDC_PAINT_T2		17202
#define IDC_PAINT_T3		17203
#define IDC_PAINT_T4		17204
#define IDC_PAINT_T5		17205
#define IDC_PAINT_T6		17206
#define IDC_PAINT_T7		17207
#define IDC_PAINT_T8		17208
#define IDC_PAINT_T9		17209
#define IDC_PAINT_T10		17210
#define IDC_PAINT_SL1		17211
#define IDC_PAINT_SL2		17212

// Stamp (Sticker) Tool Options 17300-
#define IDC_STAMP_CATEGORY	17300
#define IDC_STAMP_T1		17301
#define IDC_STAMP_T2		17302
#define IDC_STAMP_T3		17303
#define IDC_STAMP_T4		17304
#define IDC_STAMP_T5		17305
#define IDC_STAMP_T6		17306
#define IDC_STAMP_T7		17307
#define IDC_STAMP_T8		17308
#define IDC_STAMP_T9		17309
#define IDC_STAMP_T10		17310
#define IDC_STAMP_T11		17311
#define IDC_STAMP_T12		17312
#define IDC_STAMP_T13		17313
#define IDC_STAMP_T14		17314
#define IDC_STAMP_T15		17315
#define IDC_STAMP_T16		17316
#define IDC_STAMP_T17		17317
#define IDC_STAMP_T18		17318
#define IDC_STAMP_T19		17319
#define IDC_STAMP_T20		17320
#define IDC_STAMP_SW1		17321
#define IDC_STAMP_SW1a		17322

// Eraser (explode) Tool Options 17400-
#define IDC_ERASER_T1		17401
#define IDC_ERASER_T2		17402
#define IDC_ERASER_T3		17403
#define IDC_ERASER_T4		17404
#define IDC_ERASER_T5		17405
#define IDC_ERASER_T6		17406
#define IDC_ERASER_T7		17407
#define IDC_ERASER_T8		17408
#define IDC_ERASER_T9		17409
#define IDC_ERASER_T10		17410
#define IDC_ERASER_T11		17411
#define IDC_ERASER_T12		17412
#define IDC_ERASER_T13		17413
#define IDC_ERASER_T14		17414
#define IDC_ERASER_T15		17415
#define IDC_ERASER_T16		17416
#define IDC_ERASER_T17		17417
#define IDC_ERASER_T18		17418
#define IDC_ERASER_T19		17419
#define IDC_ERASER_T20		17420
#define IDC_ERASER_SW1		17421
#define IDC_ERASER_SW1a		17422
#define IDC_ERASER_SL		17423

// Lines (Crazy Lines) Tool Options 17500-
#define IDC_LINES_CATEGORY	17500
#define IDC_LINES_T1		17501
#define IDC_LINES_T2		17502
#define IDC_LINES_T3		17503
#define IDC_LINES_T4		17504
#define IDC_LINES_T5		17505
#define IDC_LINES_T6		17506
#define IDC_LINES_T7		17507
#define IDC_LINES_T8		17508
#define IDC_LINES_T9		17509
#define IDC_LINES_T10		17510

// Text (Letters and Words) Tool Options 17600-
#define IDC_TEXT_T1			17601
#define IDC_TEXT_T2			17602
#define IDC_TEXT_T3			17603
#define IDC_TEXT_T4			17604
#define IDC_TEXT_T5			17605
#define IDC_TEXT_T6			17606
#define IDC_TEXT_T7			17607
#define IDC_TEXT_T8			17608
#define IDC_TEXT_T9			17609
#define IDC_TEXT_T10		17610
#define IDC_TEXT_SL1		17611
#define IDC_TEXT_SL2		17612

// Effects Tool Options 17700-
#define IDC_EFFECTS_CATEGORY 17700
#define IDC_EFFECTS_T1		17701
#define IDC_EFFECTS_T2		17702
#define IDC_EFFECTS_T3		17703
#define IDC_EFFECTS_T4		17704
#define IDC_EFFECTS_T5		17705
#define IDC_EFFECTS_T6		17706
#define IDC_EFFECTS_T7		17707
#define IDC_EFFECTS_T8		17708
#define IDC_EFFECTS_T9		17709
#define IDC_EFFECTS_T10		17710
#define IDC_EFFECTS_T11		17711
#define IDC_EFFECTS_T12		17712
#define IDC_EFFECTS_T13		17713
#define IDC_EFFECTS_T14		17714
#define IDC_EFFECTS_T15		17715
#define IDC_EFFECTS_T16		17716
#define IDC_EFFECTS_SW1		17717
#define IDC_EFFECTS_SW1a	17718
#define IDC_EFFECTS_SL		17719
#define IDC_EFFECTS_AMOUNT	17720

// Shape Tool Options 17800-
#define IDC_SHAPES_T1		17801
#define IDC_SHAPES_T2		17802
#define IDC_SHAPES_T3		17803
#define IDC_SHAPES_T4		17804
#define IDC_SHAPES_T5		17805
#define IDC_SHAPES_T6		17806
#define IDC_SHAPES_T7		17807
#define IDC_SHAPES_T8		17808
#define IDC_SHAPES_T9		17809
#define IDC_SHAPES_T10		17810

#define IDC_SHAPES_SW1		17811
#define IDC_SHAPES_SW1a		17812
#define IDC_SHAPES_SW2		17813
#define IDC_SHAPES_SW2a		17814

#define IDC_SHAPES_T11		17815
#define IDC_SHAPES_T12		17816
#define IDC_SHAPES_T13		17817
#define IDC_SHAPES_T14		17818
#define IDC_SHAPES_T15		17819
#define IDC_SHAPES_T16		17820
#define IDC_SHAPES_T17		17821
#define IDC_SHAPES_T18		17822
#define IDC_SHAPES_T19		17823
#define IDC_SHAPES_T20		17824

#define IDC_SHAPES_T21		17825
#define IDC_SHAPES_T22		17826
#define IDC_SHAPES_T23		17827
#define IDC_SHAPES_T24		17828
#define IDC_SHAPES_T25		17829
#define IDC_SHAPES_T26		17830
#define IDC_SHAPES_T27	    17831
#define IDC_SHAPES_T28		17832
#define IDC_SHAPES_T29		17833
#define IDC_SHAPES_T30		17834

#define IDC_SHAPES_T31		17835
#define IDC_SHAPES_T32		17836
#define IDC_SHAPES_T33		17837
#define IDC_SHAPES_T34		17838
#define IDC_SHAPES_T35		17839
#define IDC_SHAPES_T36		17840
#define IDC_SHAPES_T37		17841
#define IDC_SHAPES_T38		17842
#define IDC_SHAPES_T39		17843
#define IDC_SHAPES_T40		17844

// Fill Tool Options 17850-
#define IDC_FILL_CATEGORY	17850
#define IDC_FILL_T1			17851
#define IDC_FILL_T2			17852
#define IDC_FILL_T3			17853
#define IDC_FILL_T4			17854
#define IDC_FILL_T5			17855
#define IDC_FILL_T6			17856
#define IDC_FILL_T7			17857
#define IDC_FILL_T8			17858
#define IDC_FILL_T9			17859
#define IDC_FILL_T10		17860

// Animation Tool Options 17900-
#define IDC_TOY_CATEGORY	17900
#define IDC_TOY_T1			17901
#define IDC_TOY_T2			17902
#define IDC_TOY_T3			17903
#define IDC_TOY_T4			17904
#define IDC_TOY_T5			17905
#define IDC_TOY_T6			17906
#define IDC_TOY_T7			17907
#define IDC_TOY_T8			17908
#define IDC_TOY_T9			17909
#define IDC_TOY_T10			17910
#define IDC_TOY_SW1			17911
#define IDC_TOY_SW1a		17912

// Transformer Tool Options 18000-
#define IDC_TRANSFORMER_T1	18001
#define IDC_TRANSFORMER_T2	18002
#define IDC_TRANSFORMER_T3	18003
#define IDC_TRANSFORMER_T4	18004
#define IDC_TRANSFORMER_T5	18005
#define IDC_TRANSFORMER_T6	18006
#define IDC_TRANSFORMER_T7	18007
#define IDC_TRANSFORMER_T8	18008
#define IDC_TRANSFORMER_T9	18009
#define IDC_TRANSFORMER_T10	18010
#define IDC_TRANSFORMER_T11	18011
#define IDC_TRANSFORMER_T12	18012
#define IDC_TRANSFORMER_T13	18013
#define IDC_TRANSFORMER_T14	18014
#define IDC_TRANSFORMER_T15	18015
#define IDC_TRANSFORMER_T16	18016
#define IDC_TRANSFORMER_T17	18017
#define IDC_TRANSFORMER_T18	18018
#define IDC_TRANSFORMER_T19	18019
#define IDC_TRANSFORMER_T20	18020

// Gallery Tool Options 18100-
#define IDC_GALLERY_T1		18101
#define IDC_GALLERY_T2		18102
#define IDC_GALLERY_T3		18103
#define IDC_GALLERY_T4		18104
#define IDC_FLOPPY_TEXT		18105
#define IDC_FLOPPY1			18106
#define IDC_FLOPPY2			18107
#define IDC_CHOOSEFILE		18108

// Registration dialog
#define IDC_REG_CODE        18200
#define IDC_REG_KEY         18201

/* All IDM_??? menu items go from 20000-30000 */

/* The File menu items start at 20000 */
#define IDM_FILEMENU		0
#define IDM_FILE			20000
#define IDM_NEW				20001
#define IDM_OPEN			20002
#define IDM_SAVE			20003
#define IDM_SAVEAS			20004
#define IDM_REVERT			20005
#define IDM_PRINT			20006
#define IDM_CLOSE			20007
#define IDM_EXIT			20008
#define IDM_ALTOPEN			20009
#define IDM_NEXTPAGE		20010
#define IDM_SAVEWALLPAPER	20011
#define IDM_SAVESPECIAL		20012

#define IDM_PRINTSETUP		20021

/* The File/Recall popup menu items (enough values for max recalled images) */
#define IDM_RECALLPOPUP		2
#define IDM_RECALLIMAGE		20040
#define IDM_RECALLIMAGE0	20041
#define IDM_RECALLIMAGE1	20042
#define IDM_RECALLIMAGE2	20043
#define IDM_RECALLIMAGE3	20044
#define IDM_RECALLIMAGE4	20045
#define IDM_RECALLIMAGE5	20046
#define IDM_RECALLIMAGE6	20047
#define IDM_RECALLIMAGE7	20048
#define IDM_RECALLIMAGE8	20049
#define IDM_RECALLIMAGE9	20050

#define IDM_UNDO			20101
#define IDM_APPLY			20102
#define IDM_CUT				20103
#define IDM_COPY			20104
#define IDM_COPYTOFILE		20105
#define IDM_PASTE			20106
#define IDM_PASTEASNEW		20107

#define IDM_DELETE			20109
#define IDM_PREF			20110
#define IDM_HELP			20111
#define IDM_ABOUT			20112

/* The MDI Window id's start at 20450 */
#define IDM_WINDOWCHILD		20450

/* Accelerators start at 25000 */
#define IDM_SIZEUP			25019
#define IDM_SIZEDOWN		25020
#define IDM_SHAPEUP			25021
#define IDM_SHAPEDOWN		25022
#define IDM_ESCAPE			25025
#define IDM_DELETEITEM		25026
#define IDM_LASTTOOL		25027
#define IDM_MOVEUP			25028
#define IDM_MOVEDOWN 		25029
#define IDM_MOVELEFT 		25030
#define IDM_MOVERIGHT		25031

/* All IDS_??? menu items go from 30000-32767 */

/* String definitions start at 30000 */
#define IDS_APPNAME			30000
#define IDS_APPNAMEOLE		30001
#define IDS_PREVIEW			30002
#define IDS_UNTITLED		30003
#define IDS_POS_CLIENT		30004

#define IDS_OKTORENDERCLIPBOARD 30009
#define IDS_EDITS			30010
#define IDS_PORT_SEPARATOR	30011
#define IDS_PRINTERR		30012
#define IDS_PRINTMEM		30013
#define IDS_PRINTDISK		30014
#define IDS_PRINTUSER		30015
#define IDS_AREYOUSURE		30017
#define IDS_OKTOOPEN		30018
#define IDS_OKTOSAVEPREF	30019
#define IDS_ECUSTOMBRUSH	30022
#define IDS_OKTOREMOVEMASK	30023
#define IDS_OBJECTSNEEDCOMBINE	30024

#define IDS_EWRITEFILE		30026
#define IDS_EOPEN			30027
#define IDS_ECREATE			30028
#define IDS_EIMGVERSION		30029
#define IDS_EIMGCOMP		30030
#define IDS_EIMGPHOTO		30031
#define IDS_EIMGFILL		30032
#define IDS_EIMGORIENT		30033
#define IDS_EIMGBAD			30035
#define IDS_EBADMAP			30036
#define IDS_EBADSTYLE		30037
#define IDS_EBADMASK		30039

#define IDS_CRIPPLED		30042
#define IDS_COPYPROTECTED	30047

#define IDS_SWAPPINGUNDO   30054

#define IDS_ECLIPOPEN		30055
#define IDS_OKTOAPPLY		30057
#define IDS_EMASKREAD		30058
#define IDS_INIFILE			30059
#define IDS_NAMESFILE		30060
#define IDS_HELPFILE		30061
#define IDS_ON				30062
#define IDS_OFF				30063
#define IDS_READY			30064
#define IDS_EBADCLIP		30065

#define IDS_COLOR			30068
#define IDS_COLOR2			30069
#define IDS_COLORCOUNT		30070
#define IDS_ECOMPRESS		30072
#define IDS_HASHELP			30074
#define IDS_ACTIVECOLOR		30075
#define IDS_ALTERNATECOLOR	30076
#define IDS_EBADDEVICE		30081

#define IDS_EDISKSPACE		30107
#define IDS_ENOMASK			30108
#define IDS_ENOMASKAREA		30109

#define IDS_DELETEEXTFILE	30110
#define IDS_OVERWRITEEXTFILE	30111
#define IDS_NOUNDO			30112
#define IDS_EERASENOTHING	30113
#define IDS_EERASESIZE		30114
#define IDS_INVALIDPALETTE	30116
#define IDS_OVERWRITEIMAGE	30122
#define IDS_OKTOREVERT		30123
#define IDS_SCALEXY			30124
#define IDS_SCALE			30125
#define IDS_ROTATEXY		30126
#define IDS_ROTATE			30127
#define IDS_OKTOCLOSEALL	30134
#define IDS_RETAIL			30135
#define IDS_BADMGXIMAGE		30136
#define IDS_CONFIRM_CONVERT	30137
#define IDS_PLEASEWAIT		30138
#define IDS_BADZOOM			30141
#define IDS_EDIRECTORYCHANGE	30143
#define IDS_EWRITEPROTECT	30144
#define IDS_ENOTSPACE		30145

#define IDS_BYTES			30150
#define IDS_KILOBYTES		30151
#define IDS_MEGABYTES		30152
#define IDS_GIGABYTES		30153

#define IDS_RED				30159
#define IDS_RECT_STATUS		30166
#define IDS_WALLPAPER		30168
#define IDS_EWALLFILEFORMAT 30169
#define IDS_EIMAGEOPEN		30170
#define IDS_EPROCESSOPEN	30171
#define IDS_EFRAMEREAD		30172

// MMB - Added these new values for CMYK
#define IDS_EXTNAME_BADFILE	30177
#define IDS_EXTNAME_CREATE	30178
#define IDS_EXTNAME_COPY	30179
#define IDS_INI_NOWRITE		30180
#define IDS_INI_NOEXIST		30181

/* Menu strings */
#define IDS_UNDO			30207
#define IDS_REDO			30208
#define IDS_MANUALAPPLY		30209
#define IDS_AUTOAPPLY		30210
#define IDS_OBJECTDELETE	30223

/* Undo string definitions */
#define IDS_UNDOCHANGE		30300
#define IDS_UNDOFIRST		30301
#define IDS_UNDOTEXT		30301
#define IDS_UNDOFIRST		30301
#define IDS_UNDODELETE		30302
#define IDS_UNDOCUT			30303

#define IDS_UNDOPAINT		30310
#define IDS_UNDOERASER		30315
#define IDS_UNDOSTAMP		30316
#define IDS_UNDOMARKER		30318
#define IDS_UNDOCRAYON		30319
#define IDS_UNDOLINES		30320
#define IDS_UNDOSHAPES		30321
#define IDS_FIRST_RETOUCHUNDO IDS_UNDOPAINT

#define IDS_UNDOTRANSFORMER	30350

#define IDS_UNDOMIRRORX		30365
#define IDS_UNDOMIRRORY		30366
#define IDS_UNDOMIRRORXY	30367
#define IDS_UNDOINVERT		30368

#define IDS_UNDOMERGEMASK	30371
#define IDS_UNDOPASTE		30373
#define IDS_UNDOFEATHER		30374
#define IDS_UNDOCROP		30375
#define IDS_UNDOREDUCTION	30376
#define IDS_UNDOCONVERT		30377

#define IDS_UNDOMAP			30390
#define IDS_UNDOQUARTERTONE	30391
#define IDS_UNDOCONTBRIT	30392
#define IDS_UNDOPOSTERIZE	30393
#define IDS_UNDOHUE			30394
#define IDS_UNDOCOLORBAL	30395
#define IDS_UNDOCALIBRATEAPPLY	30396

#define IDS_UNDOSQUAREMASK	30400
#define IDS_UNDOCIRCLEMASK	30401
#define IDS_UNDOFREEMASK	30402
#define IDS_UNDOPAINTMASK	30403
#define IDS_UNDOMAGICMASK	30404
#define IDS_UNDOXFORMMASK	30405
#define IDS_UNDOPOINTEDIT	30406
#define IDS_UNDOMASKINVERT	30407
#define IDS_UNDOMASKREMOVE	30408
#define IDS_UNDOSIMILAR		30409
#define IDS_UNDOLOADMASK			30410
#define IDS_UNDOSTROKEMASK			30411
#define IDS_UNDOMAGICEXPAND			30412

#define IDS_UNDODELETEOBJECT 30420
#define IDS_UNDODUPLICATE 	30421

#define IDS_UNDOINSERT		30430
#define IDS_UNDOAPPEND		30431
#define IDS_UNDOFILL		30432
#define IDS_UNDOCOLOR		30433
#define IDS_UNDOLABEL		30434
#define IDS_UNDOMOVE		30435

#define IDS_YES				30480
#define IDS_NO				30481
#define IDS_INFOSIZE		30482
#define IDS_INFOIMAGEDISK	30486

/* Printer labels */
#define IDS_LABELPICNAME	30500
#define IDS_LABELSTYLE		30501
#define IDS_LABELPRINTER	30502
#define IDS_LABELNOINKCORRECT	30503
#define IDS_LABELRINKCORRECT	30504
#define IDS_LABELGINKCORRECT	30505
#define IDS_LABELBINKCORRECT	30506
#define IDS_LABELNOHALFTONING	30507
#define IDS_LABELCOLORFREQUENCY 30508
#define IDS_LABELCOLORANGLE		30509
#define IDS_LABELSINGLEHALFTONE 30510
#define IDS_LABELMINMAXDOT		30511
#define IDS_LABELNOBLACKGEN		30512
#define IDS_LABELBLACKGEN		30513
#define IDS_LABELUCR			30514
#define IDS_LABELNOCALIBRATION	30515
#define IDS_LABELCALIBRATION	30516
#define IDS_LABELSEPC		30517
#define IDS_LABELSEPM		30518
#define IDS_LABELSEPY		30519
#define IDS_LABELSEPK		30520

/* Unit strings */
#define IDS_INCHES			30600
#define IDS_MM				30601
#define IDS_PICAS			30602
#define IDS_CM				30603
#define IDS_PIXELS			30604

/* More Message strings starting at 30700 */
#define IDS_DELETEREMOVABLE	30701
#define IDS_CANNOTREAD		30702
#define IDS_ECOPY			30703
#define IDS_ERENAME			30704
#define IDS_EDELETE			30705

#define IDS_CONFIRMDELETE	30711
#define IDS_INVALIDFILESPEC	30712
#define IDS_NOSOURCEFILES	30713
#define IDS_BRUSHNOTFOUND	30716
#define IDS_BRUSHINVALID	30717
#define IDS_BRUSHINCOMPATIBLE	30718
#define IDS_BRUSHNOUNDO		30719
#define IDS_COLORLABEL		30722

#define IDS_NONE				30736

#define IDS_MODENORMAL			30740
#define IDS_MODEADDITIVE		30741
#define IDS_MODESUBTRACTIVE		30742
#define IDS_MODEIFLIGHTER		30743
#define IDS_MODEIFDARKER		30744
#define IDS_MODEHUESAT			30745
#define IDS_MODEHUE				30746
#define IDS_MODESAT				30747
#define IDS_MODELUM				30748
#define IDS_MODERED				30749
#define IDS_MODEGREEN			30750
#define IDS_MODEBLUE			30751
#define IDS_MODECYAN			30752
#define IDS_MODEMAGENTA			30753
#define IDS_MODEYELLOW			30754
#define IDS_MODEBLACK			30755
#define IDS_MODESCREEN			30756
#define IDS_MODEMULTIPLY		30757
#define IDS_MODEDIFFERENCE		30758
#define IDS_MODETEXTURIZE		30759
#define IDS_MODEFIRST			IDS_MODENORMAL

#define IDS_GRAYVALUE			30771
#define IDS_RGBVALUE			30772
#define IDS_HSLVALUE			30773
#define IDS_CMYKVALUE			30774
#define IDS_PERCENT				30775

#define IDS_BRUSH_READONLY		30780

/* PostScript (tm) strings start at 30800 */
#define IDS_PS_STARTBLOCK	30800
#define IDS_PS_ENDBLOCK	30801
#define IDS_PS_POP		30802
#define IDS_PS_IFELSE		30803
#define IDS_PS_DEF		30804
#define IDS_PS_END		30805
#define IDS_PS_DICTDEF		30806
#define IDS_PS_HEXDATA		30807
#define IDS_PS_BINDATA		30808
#define IDS_PS_ELLDOT1		30809
#define IDS_PS_ELLDOT2		30810
#define IDS_PS_SQUDOT		30811
#define IDS_PS_CIRDOT		30812
#define IDS_PS_TRIDOT		30813
#define IDS_PS_PROPDOT		30814
#define IDS_PS_SETSCREEN	30815
#define IDS_PS_COLOREXT	30816
#define IDS_PS_SETSPOT		30817
#define IDS_PS_SETCOLORSCREEN	30818
#define IDS_PS_BLACKSEPS	30819
#define IDS_PS_COLORSEPS	30820
#define IDS_PS_NOINVERT	30821
#define IDS_PS_CHECKINVERT	30822
#define IDS_PS_TRANSLATE	30823
#define IDS_PS_SCALE		30824
#define IDS_PS_LINE1		30825
#define IDS_PS_LINE2		30826
#define IDS_PS_LINE3		30827
#define IDS_PS_LINE4		30828
#define IDS_PS_LINE5		30829
#define IDS_PS_DOCOLORSEPDEF	30830
#define IDS_PS_NOCOLORSEPDEF	30831
#define IDS_PS_DOIMAGEDEF	30832
#define IDS_PS_DOIMAGE		30833
#define IDS_PS_COLORSEPVAL	30834
#define IDS_PS_SEPCYAN		30835
#define IDS_PS_SEPMAGENTA	30836
#define IDS_PS_SEPYELLOW	30837
#define IDS_PS_SEPBLACK	30838
#define IDS_PS_COLORIMAGE3	30839
#define IDS_PS_COLORIMAGE4	30840
#define IDS_PS_GETLINE1	30841
#define IDS_PS_GETLINE2	30842
#define IDS_PS_GETLINE3	30843
#define IDS_PS_GETLINE4	30844
#define IDS_PS_GETLINE5	30845
#define IDS_PS_DUMPLINE1	30846
#define IDS_PS_DUMPLINE2	30847
#define IDS_PS_DUMPLINE3	30848
#define IDS_PS_DUMPLINE4	30849
#define IDS_PS_DUMPLINE5	30850
#define IDS_PS_IMAGE		30851
#define IDS_PS_NOIMAGE		30852
#define IDS_PS_TRANSFORM	30853
#define IDS_PS_BEGINBINARY	30854
#define IDS_PS_ENDBINARY	30855
#define IDS_PS_MYSAVERESTORE	30856
#define IDS_PS_CRLF		30857

/* 31000 - 31099 - reserved for tool hints in ribbon */
#define IDC_RIBBONSTART		31000

/* 31100 - 31200 - reserved for tool hints over image */
#define IDC_IMAGESTART		31100

/* 31200 - 31300 - reserved for tool names over image */
#define IDC_TOOLNAMESTART	31200

/* 31300 - 31400 - reserved for TWAIN messages */
#define TWAIN_START			31300

#define PM_XFERDONE			(WM_USER + 0)

/* 31600 - 31700 - reserved for tool sounds */
#define IDC_SOUNDSTART			31600

/* 32000 - 32500 - reserved for progress messages */
/* these message id's must have the same ordering */
/* as the undo id's */
#define IDS_PROGFIRST		32001
#define IDS_PROGFLOOD		32033

#define IDS_PROGCOMBINE		32069
#define IDS_PROGPASTE		32073
#define IDS_PROGCROP		32075
#define IDS_PROGMASKINVERT	32107
#define IDS_PROGRESS1		32200
#define IDS_PROGRESS2		32201
#define IDS_PROGCOPY		32206
#define IDS_PROGPASTECLIP	32208

/* 32500 - 32600 - reserved for commonid.h */
#define IDS_EMAXIMAGEWIDTH	32500
#define IDS_EMAXIMAGEHEIGHT	32501
#define IDS_EIMAGEOVERHEAD	32502
#define IDS_EMEMALLOC		32503
#define IDS_ELINEPOOLALLOC	32504
#define IDS_EDISKCACHE		32505
#define IDS_ESCROPEN		32506
#define IDS_ESCRWRITE		32507
#define IDS_EWRITE			32508
#define IDS_EREAD			32509
#define IDS_EMALLOC			32510
#define IDS_ESCRREAD		32511
#define IDS_ETOOCOMPLEX		32512

/* 32600 - 32700 - reserved for miscellaneous sounds */
#define IDS_SELECTOR      	32600
#define IDS_AVAIL         	32601

/* 32700 - 32900 - reserved for room-related strings */
// filename format strings:
#define IDS_BADGE           32700
#define IDS_BOARDWALK       32701
#define IDS_BOOK            32702
#define IDS_CERTIFICATE     32703
#define IDS_CIRCUS          32704
#define IDS_DRAGON          32705
#define IDS_EUROPE          32706
#define IDS_FASHION         32707
#define IDS_JOKES           32708
#define IDS_JUNGLE          32709
#define IDS_MAILMAZE        32710
#define IDS_MAZES           32711
#define IDS_MONSTER         32712
#define IDS_PAPER           32713
#define IDS_PLACEMAT        32714
#define IDS_PUZZLE          32715
#define IDS_SAFARI          32716
#define IDS_SHAPES          32717
#define IDS_SPACEBALL       32718
#define IDS_SUBSETS         32719
#define IDS_VEHICLE         32720
#define IDS_WORLD           32721
#define IDS_WRONG           32722
#define IDS_MIXUP_LIB       32723
#define IDS_FASHION_LIB     32724
#define IDS_VEHICLE_LIB     32725

// gallery strings
#define IDS_GALLERY_PREFIX  32750
#define IDS_GALLERY_EXT     32751
#define IDS_GALLERY_INIFILE 32752
#define IDS_GALLERY_FILE_SECTION 32753
#define IDS_DEFAULT_EXTENSION 32754

// font strings
#define IDS_FONT_ITALIC		32755
#define IDS_FONT_NORMAL		32756

// misc. strings
#define IDS_IM_NEW			32757
#define IDS_YOUR_NAME		32758
#define IDS_USER_INIFILE	32759
#define IDS_USER_FILE_SECTION 32760
#define IDS_REG_CODE_TITLE  32761 
#define IDS_REG_CODE_MSG    32762 
#define IDS_SLIDE_FILE_SECTION 32763
#define IDS_SLIDE_FILE2_SECTION 32764
#define IDS_SLIDE_FILE2_ENTRY 32765
#define IDS_INTERMSN_TOY    32766
#define IDS_GUEST			32767
#define IDS_TOUR_GUIDE_ON   32768
#define IDS_TOUR_GUIDE_OFF  32769
#define IDS_SAVEFLOPPY		32770
#define IDS_LOADFLOPPY		32771
#define IDS_SELFLOPPY		32772
#define IDS_NOPICTURES		32773
#define IDS_SOUND_ON        32774
#define IDS_SOUND_OFF       32775
#define IDS_SOUNDDLL        32776
#define IDS_CD_MSGALERT     32777
#define IDS_CD_MSGTITLE     32778
#define IDS_CD_APPGROUP     32779
#define IDS_CD_APPICON      32780
#define IDS_CD_APPOFFER     32781
#define IDS_CD_AVIICON1     32782
#define IDS_CD_AVIICON2     32783
#define IDS_CD_AVIICON3     32784
#define IDS_CD_AVIICON4     32785
#define IDS_CD_AVIICON5     32786
#define IDS_CD_DEMOTEXT     32787
#define IDS_CD_SAFETY       32788

#endif
