// (c) Copyright    1991 MICROGRAFX, Inc., All Rights Reserved.
// This material is confidential and a trade secret.
// Permission to use this work for any purpose must be obtained
// in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX
#ifndef _ID_H_
#define _ID_H_
#include "commonid.h"
#include "bsrid.h"

//************************************************************************
// THIS FILE CONTAIN ID'S FOR ALL WINDOWS, MENUS, DIALOGS, AND CONTROLS
// NOTHING IN THIS FILE SHOULD BE MODIFIED FOR LOCALIZATION
//************************************************************************

// for strings
#define PROGRESS_ID(idundo) \
	((idundo)-IDS_UNDOFIRST+IDS_PROGFIRST)
#define TASK_ID(idundo) \
	((idundo)-IDS_UNDOFIRST+IDS_TASKFIRST)

#define ID_APPL             _T("Picture Publisher")
#define ID_APPCLASS         _T("PP5")
#define ID_EFFECTS          _T("Picture Publisher Effects")
#define ID_SETTINGS			_T("Settings")
#define IDR_PPMAIN 			100
#define IDC_HERALD          102

#define ID_PWACCEL          103
#define IDC_PPFILEDOC       104    // PP5 and PP4 file icon (ie. Explorer)
#define IDC_OTHERFILEDOC    105    // Other supported files icon (ie. Explorer and TIFF,GIF etc.)
#define IDC_PP16X16ICON     106    // color 16x16 small Application icon
#define IDC_DOC16X16ICON    107    // color 16x16 small document icon

/* Print cancel controls */
#define ID_NAME         	110
#define ID_PRINTER          111
#define ID_PORT         	112
#define ID_FILENAME        	113
#define ID_OUTPUTTYPE		114

/* Cursors */
#define ID_ARROW        	32512
#define ID_IBEAM	      	32513
#define ID_SIZE         	32646
#define ID_NULL             500

#define ID_SELECTOR     505

#define ID_CLOCK01      506
#define ID_CLOCK02      507
#define ID_CLOCK03      508
#define ID_CLOCK04      509
#define ID_CLOCK05      510
#define ID_CLOCK06      511
#define ID_CLOCK07      512
#define ID_CLOCK08      513
#define ID_CLOCK09      514
#define ID_CLOCK10      515
#define ID_CLOCK11      516
#define ID_CLOCK12      517

#define ID_FREE             518
#define ID_CUSTOMVIEW       519
#define ID_MAGICWAND        520
#define ID_GRABBER          521
#define ID_PROBE            522
#define ID_VIGNETTE         523
#define ID_TEXT             524
#define ID_FLOOD            525
#define ID_PENCIL           526
#define ID_SHIELD           527
#define ID_BALL1            528
#define ID_BALL2            529
#define ID_BALL3            530
#define ID_BALL4            531
#define ID_APPHELP          532
#define ID_ROTATE           533
#define ID_SKEWHORZ         534
#define ID_SKEWVERT         535
#define ID_NOHELP           536
#define ID_DRAGMOVE         537
#define ID_DRAGCOPY         538
#define ID_DRAGNOT          539
#define ID_TOMSAWYER1       540
#define ID_TOMSAWYER2       541
#define ID_TOMSAWYER3       542
#define ID_TOMSAWYER4       543
#define ID_TRANSFORMER      544
#define ID_POINTER          545
#define ID_CROP             546
// the left right cursors must stay paired
#define ID_ADDLEFT          547
#define ID_ADDRIGHT         548
#define ID_MOVELEFT         549
#define ID_MOVERIGHT        550
#define ID_DELETELEFT       551
#define ID_DELETERIGHT      552

#define ID_FREEALT          553
#define ID_SELECTORALT      ID_SIZE
#define LAST_CURSOR     	ID_FREEALT
#define LAST_ICON           31999

//  1960-1990 id's for full names for file types starting at    1990
// must be in same order as IDN_'s
#define IDI_FIRSTFILETYPE   IDI_PP    //  1960-1990 reserved
#define IDI_PPCOMPOUND      1960
#define IDI_PP              1961
#define IDI_TIFF            1962
#define IDI_TARGA           1963
#define IDI_GIF             1964
#define IDI_BMP             1965
#define IDI_DIB             1966
#define IDI_PCX             1967
#define IDI_EPS             1968
#define IDI_DCS             1969
#define IDI_JPEG            1970
#define IDI_PHOTOCD         1971
#define IDI_SCITEX          1972
#define IDI_AVI             1973
#define IDI_THUMBNAIL       1974
#define IDI_PP4             1975
#define IDI_LASTFILETYPE    IDI_PP4

// The image file types
#define IDN_FIRSTFILETYPE   1980
#define IDN_PPCOMPOUND      1980 // PP6 >=
#define IDN_PP              1981 // PP5
#define IDN_TIFF            1982
#define IDN_TARGA           1983
#define IDN_GIF             1984
#define IDN_BMP             1985
#define IDN_DIB             1986
#define IDN_PCX             1987
#define IDN_EPS             1988
#define IDN_DCS             1989
#define IDN_JPEG            1990
#define IDN_PHOTOCD         1991
#define IDN_SCITEX          1992
#define IDN_AVI             1993
#define IDN_THUMBNAIL       1994
#define IDN_PP4             1995
#define IDN_LASTFILETYPE    IDN_PP4
#define IDN_NOFILETYPE      (IDN_LASTFILETYPE+1)

// The extended name id's
#define IDN_FIRSTEXTTYPE    IDN_MAP
#define IDN_MAP             2000
#define IDN_STYLE           2001
#define IDN_HALFTONE        2002
#define IDN_CALMAP          2003
#define IDN_SEPARATE        2004
#define IDN_BGMAP           2005
#define IDN_SPECIAL         2006
#define IDN_SCANNER         2007
#define IDN_GRABBER         2008
#define IDN_SHAPE           2009
							
#define IDN_TEXTURE         2010
#define IDN_CLIPBOARD       2011
#define IDN_MASK            2012
#define IDN_CUSTOM          2013
#define IDN_SCANMAP         2014
#define IDN_PALETTE         2015
#define IDN_CLRMGR          2016

#define IDN_PAINTSTYLE      2020
#define IDN_SPRAYSTYLE      2021
#define IDN_CLONESTYLE      2022
#define IDN_TEXTURESTYLE    2023
#define IDN_SMEARSTYLE      2024
#define IDN_ERASESTYLE      2025
#define IDN_FIRST_RETOUCHSTYLE IDN_PAINTSTYLE

#define IDN_SHARPSTYLE      2030
#define IDN_SMOOTHSTYLE     2031
#define IDN_LIGHTENSTYLE    2032
#define IDN_DARKENSTYLE     2033
#define IDN_FIRST_FILTERSTYLE IDN_SHARPSTYLE

#define IDN_PENCILSTYLE     2050
#define IDN_DRAWSHAPESTYLE  2051
#define IDN_DRAWFREESTYLE   2052
#define IDN_FIRST_DRAWSTYLE IDN_PENCILSTYLE

#define IDN_STROKESTYLE     2055
#define IDN_FIRST_STROKESTYLE IDN_STROKESTYLE

#define IDN_EXPORT          2060
#define IDN_MACRO           2061
#define IDN_IMPORTFILTERS   2062
#define IDN_PCDLUT		    2063
#define IDN_LASTEXTTYPE     IDN_PCDLUT

/* Leave an equal number of slots below for "add" strings */
#define MAX_FILETYPES       100 // 2100-2199 reserved

#define IDI_FIRSTIMPORTFILETYPE IDI_AI
#define IDI_AI                  2500
#define IDI_CDR                 2501
#define IDI_CGM                 2502
#define IDI_DRW                 2503
#define IDI_WMF                 2504
#define IDI_LASTIMPORTFILETYPE  IDI_WMF


#define IDN_FIRSTIMPORTFILETYPE IDN_AI
#define IDN_AI                  2530
#define IDN_CDR                 2531
#define IDN_CGM                 2532
#define IDN_DRW                 2533
#define IDN_WMF                 2534
#define IDN_LASTIMPORTFILETYPE  IDN_WMF
#define IDN_NOIMPORTFILETYPE    (IDN_LASTIMPORTFILETYPE+1)

/* Full screen view window id */
#define IDD_VIEWFULL        3000
#define IDD_ZOOMBOX         3001
#define IDD_PRINTPREVIEW    3002
#define IDD_SUMMDLG         3003

/* Main toolbox window id */
#define IDD_TOOLS           3100
#define IDD_VIEWING         3101

/* Opening herald window id */
#define IDD_HERALD          3200
#define IDD_COMBO           3201

/* The mask toolbox option window id's */
#define IDD_PAINTON         3299
#define IDD_FREE            3300
#define IDD_SQUARE          3301
//#define IDD_CIRCLE            3302
#define IDD_MAGICWAND       3303
#define IDD_SHIELD          3304
#define IDD_TRANSFORMER     3305
#define IDD_POINTER         3306
#define IDD_CROP            3307

/* The retouch toolbox option window id's */
#define IDD_PAINT           3400
#define IDD_SPRAY           3401
#define IDD_CLONE           3402
#define IDD_TEXTURE         3403
#define IDD_SMEAR           3404
#define IDD_ERASER          3405

/* The filter toolbox option window id's */
#define IDD_SHARP           3451
#define IDD_SMOOTH          3452
#define IDD_LIGHTEN         3453
#define IDD_DARKEN          3454
#define IDD_STITCH          3455

/* The fill toolbox option window id's */
#define IDD_VIGNETTE        3500
#define IDD_FLOOD           3501
#define IDD_TEXTUREFILL     3502
#define IDD_TINTFILL        3503

/* The draw toolbox option window id's */
#define IDD_PENCIL              3600
#define IDD_DRAWSQUARE          3601
#define IDD_DRAWFREE            3602

/* The other toolbox option window id's */
#define IDD_CUSTOMVIEW      3800
#define IDD_TEXT            3802
#define IDD_PROBE           3803
#define IDD_OBJECTTRANSFORM 3804
#define IDD_SELECTOR        3805
#define IDD_MASKTRANSFORM   3806
#define IDD_SELECTORTRANSFORM   3807

/* Dialog box definitions */
#define IDD_HISTOGRAM 			3894
#define IDD_PASTEFROM_OPTIONS 	3895
#define IDD_EXT_INFO			3896
#define IDD_HUE_SHIFT       	3897
#define IDD_QUARTER_OPTIONS 	3898
#define IDD_MAP_OPTIONS     	3899
#define IDD_COLORPICKER     	3900
#define IDD_STATUS          	3901
#define IDD_PALETTE         	3902
#define IDD_NEW             	3903
#define IDD_OPEN            	3904
#define IDD_SAVEAS          	3905
#define IDD_ABORT           	3906
#define IDD_GRAB            	3907
#define IDD_GRABSETUP       	3908
#define IDD_SCAN            	3909
#define IDD_SCANSETUP       	3910
#define IDD_CALSCANEDIT     	3911
#define IDD_PREF            	3912
#define IDD_AVI             	3913
#define IDD_FASTBITS        	3914
#define IDD_MAP             	3915
#define IDD_MASKSTROKE      	3916
#define IDD_POSTERIZE       	3917
#define IDD_HUE             	3918
#define IDD_QUARTERTONE     	3919
#define IDD_MASKLOAD        	3920
#define IDD_MASKSAVE        	3921
#define IDD_SIZE            	3922
#define IDD_ROTATE          	3923
#define IDD_ABOUT           	3924
#define IDD_WARNING         	3925
#define IDD_AVIFRAME        	3926
#define IDD_COPYTOFILE      	3927
#define IDD_PASTEFROMFILE   	3928
#define IDD_STATUSBOX       	3929
#define IDD_THUMBOPEN       	3930

#define IDD_MAPSAVE         	3931
#define IDD_MAPLOAD         	3932
#define IDD_CALIBRATESCAN   	3933
#define IDD_CALIBRATEPRINT  	3934
#define IDD_CALIBRATEAPPLY  	3935
#define IDD_THUMBSAVEAS     	3936
#define IDD_CALIBRATEPRINTEDIT 	3937

#define IDD_SHIELDFLOAT     3338
#define IDD_STITCHCONFIRM   3339
#define IDD_PRINT           3940
#define IDD_PRINTSETUP      3941
#define IDD_PRINTSTYLE      3942
#define	IDD_THRESHOLD		3943
#define IDD_GRAYBALANCEEDIT 3944

#define IDD_TEXTURIZE       3945
#define IDD_CONVERTTO       3946

#define IDD_PASTEFROMFILE_THUMB   3947

#define IDD_INFOFLOAT       3948
#define IDD_GETDLGSTRING    3949
#define IDD_ADDEXTNAME      3950
#define IDD_MACROPLAY       3951
#define IDD_MACRORECORD     3952

#define IDD_CONTBRIT_STICK  3953
#define IDD_CONTBRIT_VISUAL 3954
#define IDD_COLORBAL_STICK  3955
#define IDD_COLORBAL_VISUAL 3956
#define IDD_LOWRES_OPEN     3957
#define IDD_OBJFLOAT        3958
#define IDD_FEATHEROBJECT   3959
#define IDD_EXPAND          3960
#define IDD_MACROREPEAT     3961
#define IDD_MACROSAVE       3962
#define IDD_LOWRESQUERY     3963

/* The filter extended name open dialog id's */
#define IDD_SPECIALFXFILT   3964
#define IDD_EXPORT          3965

#define	IDD_SHAPELOAD		3966 
#define	IDD_SHAPESAVE		3967 

#define IDD_FEATHER         3968
#define IDD_MASK_SMOOTH		3969

#define IDD_APPLYORUNDO     3970
#define IDD_CUSTOMBRUSH     3971
#define IDD_IMAGEINFO       3972
#define IDD_GAMMA           3973
#define IDD_PALETTELOAD     3974
#define IDD_PALETTESAVE     3975
#define IDD_PALETTENAME     3976
#define IDD_PALETTENEW      3977
#define IDD_PALETTEMERGE    3978
#define IDD_PALETTEFILL     3979
#define IDD_PALETTELABEL    3980
#define IDD_PALETTEFIND     3981
#define IDD_EPSOPTIONS      3982
#define IDD_LOCATIONS       3983
#define IDD_TIFFOPTIONS     3984
#define IDD_PPFFOPTIONS     3985
#define IDD_AVIOPTIONS      3986
#define IDD_FILECOPY        3987
#define IDD_FILERENAME      3988
#define IDD_FILEDELETE      3989
#define IDD_GLOBALFLOAT     3990
#define IDD_PALETTEOPTIONS  3991
#define IDD_JPEGOPTIONS     3992
#define IDD_PCDOPTIONS      3993
#define IDD_PCDFILEINFO     3994
#define IDD_OPENWALL        3995
#define IDD_SAVEWALL        3996
#define IDD_DCSOPTIONS      3997
#define IDD_OKDEL_PASTEFROM	3998
#define IDD_PP4OPTIONS		3999

/* Color Management Dialog */
#define IDD_CLRMGRSETUP     4000
#define IDD_CLRMGRAPPLY     4001
#define IDD_CLRMGRDISPSETUP 4002

/* New size dialogs. Mikey and Ted told me to put them in a hole somewhere.*/
#define IDD_CONTBRIT_VISUAL86   4010
#define IDD_CONTBRIT_VISUAL107  4011
#define IDD_COLORBAL_VISUAL86   4012
#define IDD_COLORBAL_VISUAL107  4013
#define IDD_TGAOPTIONS   		4014


#define IDD_HALFTONE        4100
#define IDD_CALIBRATE       4101
#define IDD_SEPARATE        4102
#define IDD_BLACKGENERATE   4103
#define IDD_EXTRAS			4104
#define IDD_CMSSEP			4105
#define IDD_PPSEP			4106

#define IDD_CREATEBRUSH         4110
#define IDD_CREATETEXTUREBRUSH  4111
#define IDD_CREATEDRAWBRUSH     4112
#define IDD_CREATEBRUSHSCRATCH  4113
#define IDD_CREATEPENCILBRUSH   4114

#define IDD_TASKMANAGER     4120

// Object Alignment Dialog

#define IDD_ALIGNOBJECT     4121

// Palette Color Picker Dialog
#define IDD_PALETTEPICKER       4122
// Palette Editor Dialog
#define IDD_PALETTEEDITOR       4123

// Command List Undo dialog
#define IDD_CMDLIST_UNDO    4124
#define IDD_CMDLIST_REDO    4125
#define IDD_CMDLIST_EDIT    4126

// Position Objects Dialog
#define IDD_POSOBJECT       4127

#define IDD_MACRO_EDIT      4128
#define IDD_MACRO_NAME      4129
#define IDD_MACRO_STATUS    4130
#define IDD_MACRO_LOAD		4131
#define IDD_MACRO_BATCH		4132
#define IDD_CMDLIST_STATUS	4133
#define IDD_BUILD_STATUS	4134
#define IDD_BATCH_OPTIONS	4135

// Color Manager Dialogs
#define IDD_CMS_SRCDST		4136
#define IDD_CMS_SRCDSTINFO 	4137
#define IDD_CMS_DST			4138
#define IDD_CMS_DSTINFO		4139			
#define IDD_CMS_SRC			4140
#define IDD_CMS_SRCINFO		4141			

#define IDD_OPEN_SS	     	4500  // single select open dialog browser
#define IDD_THUMBOPEN_SS    4501  // single select thumb dialog browser
#define IDD_IMPORT_SS       4502  // single select open importdialog browser
#define IDD_THUMBIMPORT_SS  4503  // single select thumb importdialog browser
#define IDD_IMPORT          4504
#define IDD_IMPORTSETUP     4505
#define IDD_THUMBIMPORT     4506
#define IDD_PREFUNITS		4507
#define IDD_PREFPLUGIN		4508
#define IDD_PREFSCRATCHPAD 	4509
#define IDD_PREFOBJECTS		4510
#define IDD_PREFUNDO		4511
#define IDD_PREFMEMORY		4512
#define IDD_PREFMISC		4513

#define IDD_MONITORSETUP	4514
#define IDD_SCREENBALANCE	4515
#define IDD_MONITORCMS		4516

#define IDD_SCANCAL			4517
#define IDD_CMSCAL			4518
#define IDD_PRINTVISUAL		4519
#define IDD_PRINTNUMERIC	4520

#define IDD_CALIBRATESCANVISUAL   	4521
#define IDD_CALIBRATEPRINTVISUAL  	4522

#define IDD_SCANMAP_LOAD	4523

#define IDD_MAPNUMERIC		 	4524
#define IDD_MAPVISUAL		 	4525
#define IDD_MAPVISUALNOPROBE 	4526

#define IDD_CALMAP_LOAD		 	4527
#define IDD_PCDRESOLUTION	 	4528

#define IDD_PHOTOCD		    4529
#define IDD_PHOTOCDCMS		4530
#define IDD_PACKAGEPP5		4531
#define IDD_YESNORETRY		4532
#define IDD_SUMMARYTABDLG	4533

// nag box dlg IDs
#define	IDD_TRIALDAYSLEFT	4600
#define	IDD_TRIALEXPIRED	4601

// Image Browser uses 5000-7000 in bsrid.h

/* All IDC_??? control go from 9900-20000 */
/* Generic controls */
#define IDC_SAVE            	9900
#define IDC_LOAD            	9901
#define IDC_RESET           	9902
#define IDC_RESETALL        	9903
#define IDC_PREVIEW         	9904
#define IDC_TOOLICON        	9905
#define IDC_STYLES          	9906
#define IDC_COMBOBOX        	9907
#define IDC_CLOSEBOX        	9908
#define IDC_UNLOCK          	9909
#define IDC_LOCK            	9910
#define IDC_PANE0           	9950
#define IDC_PANE1           	9951
#define IDC_PANE2           	9952
#define IDC_PANE3           	9953
#define IDC_PANE4           	9954
#define IDC_PANE5           	9955
#define IDC_PANE6           	9956
#define IDC_PANE7           	9957
#define IDC_PANE8           	9958
#define IDC_PANE9           	9959
#define IDC_CLOSE				9960
#define IDC_PERCENT				9961
#define ID_SECTION0           	9970
#define ID_SECTION1           	9971
#define ID_SECTION2           	9972
#define ID_SECTION3           	9973
#define ID_SECTION4           	9974
#define ID_SECTION5           	9975
#define ID_SECTION6           	9976
#define ID_SECTION7           	9977
#define ID_SECTION8           	9978
#define ID_SECTION9           	9979
#define IDC_TABSWITCHER			9980
#define IDC_TABBORDER			9981
#define IDC_TABTEXT0			9982
#define IDC_TABTEXT1			9983
#define IDC_TABTEXT2			9984
#define IDC_TABTEXT3			9985
#define IDC_TABTEXT4			9986
#define IDC_TABTEXT5			9987
#define IDC_TABTEXT6			9988
#define IDC_TABTEXT7			9989
#define IDC_TABTEXT8			9990
#define IDC_TABTEXT9			9991
#define IDC_TABTEXT10			9992
#define IDC_TABTEXT11			9993
#define IDC_YESNORETRY_TEXT		9994

/* Tools bar controls start at  10000 */
#define IDC_SELECTOR        10000
#define IDC_MASK            10001
#define IDC_RETOUCH     	10002
#define IDC_FILTER          10003
#define IDC_FILL            10004
#define IDC_DRAW            10005
#define IDC_ZOOM		    10006
#define IDC_TEXT            10007
#define IDC_PROBE           10008
#define IDC_FIRSTTOOL       IDC_SELECTOR
#define IDC_LASTTOOL        IDC_PROBE
#define IDC_GRABBER         10009

#define IDC_ACTIVECOLOR     10013

// The selection tools
#define IDC_WANDRANGE       10015
#define IDC_WANDFADE       	10016

/* The mask toolbox icons */
#define IDC_PAINTON         10019
#define IDC_FREE            10020
#define IDC_SQUARE          10021
#define IDC_CIRCLE          10022
#define IDC_MAGICWAND       10023
#define IDC_SHIELD          10024
#define IDC_TRANSFORMER 	10025
#define IDC_POINTER     	10026
#define IDC_CROP            10027
#define IDC_OBJECTTRANSFORM 10028


/* The retouch toolbox icons */
#define IDC_PAINT           	10030
#define IDC_SPRAY           	10031
#define IDC_CLONE           	10032
#define IDC_TEXTURE         	10033
#define IDC_SMEAR           	10034
#define IDC_ERASER          	10035
#define IDC_FIRST_RETOUCHTOOL   IDC_PAINT
#define IDC_ALTERASER			10036

/* The filter toolbox icons */
#define IDC_SHARP           	10040
#define IDC_SMOOTH          	10041
#define IDC_LIGHTEN         	10042
#define IDC_DARKEN          	10043
#define IDC_STITCH          	10044
#define IDC_FIRST_FILTERTOOL    IDC_SHARP

/* The fill toolbox icons */
#define IDC_VIGNETTE        10050
#define IDC_FLOOD           10051
#define IDC_TEXTUREFILL     10052
#define IDC_TINTFILL        10053

/* The draw toolbox icons */
#define IDC_PENCIL      	10060
#define IDC_DRAWSQUARE      10061
#define IDC_DRAWFREE        10062
#define IDC_FIRST_DRAWTOOL  IDC_PENCIL

#define IDC_DRAWCIRCLEFILLED    10064
#define IDC_DRAWSQUAREFILLED    10065
#define IDC_DRAWFREEFILLED  	10066

/* The View controls */
#define IDC_CUSTOMFUNCTION  10070
#define IDC_ZOOMIN          10071
#define IDC_ZOOMOUT         10072
#define IDC_ZOOMWINDOW      10073
#define IDC_VIEW1TO1        10074
#define IDC_VIEWALL         10075
#define IDC_VIEWLAST        10076
#define IDC_VIEWFULL        10077
#define IDC_VIEWIMAGE0      10078
#define IDC_VIEWIMAGE1      10079
#define IDC_VIEWIMAGE2      10080
#define IDC_VIEWIMAGE3      10081
#define IDC_VIEWIMAGE4      10082
#define IDC_VIEWIMAGE5      10083
#define IDC_VIEWIMAGE6      10084
#define IDC_VIEWIMAGE7      10085
#define IDC_VIEWIMAGE8      10086
#define IDC_VIEWIMAGE9      10087
#define IDC_VIEWIMAGEUP     10088
#define IDC_VIEWIMAGEDOWN   10089
#define IDC_VIEWREADOUT     10090
#define IDC_VIEWUP          10091
#define IDC_VIEWDOWN        10092
#define IDC_QUICKZOOM       10093
#define IDC_CUSTOMVIEW		10094
#define IDC_ZOOM_COMBO		10095

#define IDC_FIRSTICON       IDC_PAINTON
#define IDC_LASTICON        IDC_DRAWFREEFILLED

/* The Palette Dialog Menu id's start at 20800 */
/* Palette manage menu items at 10100 */
#define IDC_PALETTEMENU     10100
#define IDC_PALFILEMENU     10101
#define IDC_PALEDITMENU     10102
#define IDC_PALNEW          10103
#define IDC_PALOPEN         10104
#define IDC_PALSAVE         10105
#define IDC_PALSAVEAS       10106
#define IDC_PALMERGE        10107
#define IDC_PALRESET        10108
#define IDC_PALUNDO         10109
#define IDC_PALINSERT       10110
#define IDC_PALAPPEND       10111
#define IDC_PALDELETEITEM   10112
#define IDC_PALLABEL        10113
#define IDC_PALFIND         10114
#define IDC_PALFILL         10115
#define IDC_PALOPTIONS      10116
#define IDC_PALSETACTIVE    10117
#define IDC_PALAUTOSET      10118
#define IDC_PALMOVE         10119
#define IDC_PALUNDOFILL     10120

// Remove Holes
#define IDC_RH_AMOUNT		10160
#define IDC_RH_DOTS			10161

// histogram controls 
#define IDC_HISTO_VALUE		10170
#define IDC_HISTO_TOTAL		10171
#define IDC_HISTO_POS		10172
#define IDC_HISTO_GRAD		10173
#define IDC_HISTO_HI		10174
#define IDC_HISTO_MID		10175
#define IDC_HISTO_SHAD		10176

/* Palette Dialog controls start at 10200 */
#define IDC_PALETTE         10200
#define IDC_COLORLABEL      10202

/* Palette Size Dialog controls start at    10220 */
#define IDC_PALHORZ         10220
#define IDC_PALVERT         10221
#define IDC_PALROWS         10222
#define IDC_PALCOLUMNS      10223

/* Palette Fill Dialog controls start at    10240 */
#define IDC_FILLRGB         	10240
#define IDC_FILLHSL         	10241
#define IDC_FILLMAX         	10242
#define IDC_PALETTEFILL     	10243
#define IDC_PALETTEFILL_COLOR1  10244
#define IDC_PALETTEFILL_COLOR2  10245

/* Palette Find Dialog controls start at    10260 */
#define IDC_COLORNAME       10260
#define IDC_COLORSFOUND     10261
#define IDC_PREVIOUS        10262
#define IDC_NEXT            10263
#define IDC_FINDPATCH       10264
#define IDC_FINDENABLE      10265
#define IDC_FINDCOLOR       10266

/* Palette Name Dialog controls start at    10280 */
#define IDC_PALNAME         10280

/* Palette Color Label Dialog controls start at 10300 */
#define IDC_LABELNAME       10300

/* New Palette Dialog controls start at 10310 */
#define IDC_PALIMAGEFILL    10310

/* Color picker controls start at   10390 */
#define IDC_PICK_2PATCH1    10390
#define IDC_PICK_2PATCH2    10391
#define IDC_1COLOR_WND      10392
#define IDC_2COLOR_WND      10393

#define IDC_PICKER          10400
#define IDC_PICK_PATCH1     10401
#define IDC_PICK_PATCH2     10402
#define IDC_PICK_LABEL      10403
#define IDC_PICK_GRADIENT   10404
#define IDC_PICK_LEVEL      10405

#define IDC_PICK_H          10406
#define IDC_PICK_S          10407
#define IDC_PICK_L          10408
#define IDC_PICK_R          10409
#define IDC_PICK_G          10410
#define IDC_PICK_B          10411
#define IDC_PICK_C          10412
#define IDC_PICK_M          10413
#define IDC_PICK_Y          10414
#define IDC_PICK_K          10415

#define IDC_PICKER_IS       10416

#define IDC_PICKER_IS_H     10417
#define IDC_PICKER_IS_S     10418
#define IDC_PICKER_IS_L     10419
#define IDC_PICKER_IS_R     10420
#define IDC_PICKER_IS_G     10421
#define IDC_PICKER_IS_B     10422
#define IDC_PICKER_IS_C     10423
#define IDC_PICKER_IS_M     10424
#define IDC_PICKER_IS_Y     10425
#define IDC_PICKER_IS_K     10426

#define IDS_PICKER_IS_H     10427
#define IDS_PICKER_IS_S     10428
#define IDS_PICKER_IS_L     10429
#define IDS_PICKER_IS_R     10430
#define IDS_PICKER_IS_G     10431
#define IDS_PICKER_IS_B     10432
#define IDS_PICKER_IS_C     10433
#define IDS_PICKER_IS_M     10434
#define IDS_PICKER_IS_Y     10435
#define IDS_PICKER_IS_K     10436

#define IDC_ACTIVE_SELECT   	10437
#define IDC_ALTERNATE_SELECT    10438

#define IDC_PERCENT0        10440
#define IDC_PERCENT1        10441
#define IDC_PERCENT2        10442
#define IDC_PERCENT3        10443
#define IDC_PERCENT4        10444
#define IDC_PERCENT5        10445
#define IDC_PERCENT6        10446
#define IDC_PERCENT7        10447
#define IDC_PERCENT8        10448
#define IDC_PERCENT9        10449

/* Stroke Mask start at 10450 */
#define IDC_STROKEMASK      10450
#define IDC_STROKETOOLS     10451
#define IDC_PAINTTOOL       10452
#define IDC_SPRAYTOOL       10453
#define IDC_TEXTURETOOL     10454
#define IDC_SMEARTOOL       10455
#define IDC_SHARPENTOOL     10456
#define IDC_SMOOTHTOOL      10457
#define IDC_LIGHTENTOOL     10458
#define IDC_DARKENTOOL      10459
#define IDC_STROKEFIRST     IDC_PAINTTOOL
#define IDC_STROKELAST      IDC_DARKENTOOL

#define IDC_STROKEAMOUNT    10460
#define IDC_STROKEAMOUNTLBL 10461
#define IDC_TRANSPARENCYLBL 10462
#define IDC_PRESSURELBL     10463

/* Chroma Mask start at 10465 */
#define IDC_CHROMA_PROBE		10465
#define IDC_CHROMA_COLOR		10466
#define	IDC_CHROMA_RANGE		10467
#define	IDC_CHROMA_FADE			10468
#define IDD_CHROMA_MASK			10469

/* Feather Mask start at    10470 */
#define IDC_FEATHER_AMOUNT  10470
#define IDC_FEATHER_EDGE    10471
#define IDC_FEATHER_HARD    10472
#define IDC_FEATHER_NORMAL  10473
#define IDC_FEATHER_SOFT    10474
#define IDC_FEATHER_INOUT   10475
#define IDC_FEATHER_OUT     10476
#define IDC_FEATHER_ALL     10477
#define IDC_FEATHER_IN      10478

/* Info Dialog start at 10480 */
#define IDC_VALUE_R         10480
#define IDC_VALUE_G         10481
#define IDC_VALUE_B         10482
#define IDC_VALUE_C         10483
#define IDC_VALUE_M         10484
#define IDC_VALUE_Y         10485
#define IDC_VALUE_K         10486
#define IDC_VALUE_GRAY      10487
#define IDC_INFO_XPOS       10488
#define IDC_INFO_YPOS       10489
#define IDC_INFO_WIDTH      10490
#define IDC_INFO_HEIGHT     10491
#define IDC_INFO_RGB        10492
#define IDC_INFO_CMYK       10493
#define IDC_INFO_GRAY       10494
#define IDC_INFO_SIZE       10495

/* Preferences controls start at 10500 */
#define IDC_PREF_SAVE           10500
#define IDC_PREF_LOCATIONS      10501

// The units group
#define IDC_PREF_DISPLAYPIXELS  10510
#define IDC_PREF_USERULERS      10511
#define IDC_PREF_SCREENWIDTH    10512
#define IDC_PREF_UNITS          10513
#define IDC_PREF_UNITINCHES     10514
#define IDC_PREF_UNITMM         10515
#define IDC_PREF_UNITPICAS      10516
#define IDC_PREF_UNITCM         10517
#define IDC_PREF_UNITPIXELS     10518
#define IDC_PREF_UNITFIRST      IDC_PREF_UNITINCHES
#define IDC_PREF_UNITLAST       IDC_PREF_UNITPIXELS

// The memory group
#define IDC_PREF_POUCH          10520
#define IDC_PREF_RAMDISK        10521
#define IDC_PREF_MEMMIN         10522
#define IDC_PREF_MEMFACTOR      10523
#define IDC_PREF_LINEART        10524

// The scratchpad group
#define IDC_PREF_SCRATCHX       10530
#define IDC_PREF_SCRATCHY       10531
#define IDC_PREF_SCRATCHTYPE    10532
#define IDC_PREF_SCRATCHGRAY    10533
#define IDC_PREF_SCRATCHCOLOR   10534

// (Plugin Group)
#define IDC_PREF_PLUGINPATH1    10535
#define IDC_PREF_PLUGINPATH2    10536

// The object group
#define IDC_PREF_OBJECTS        10537
#define IDC_PREF_OBJECTMASKS    10538

// (Plugin Group)
#define IDC_PREF_USEPLUGINS 10539

// The undo group
#define IDC_PREF_UNDOMODE       10540
#define IDC_PREF_IMAGEUNDO      10541
#define IDC_PREF_OBJECTUNDO     10542
#define IDC_PREF_UNDOTYPE       10543
#define IDC_PREF_NOUNDO         10544
#define IDC_PREF_AUTOAPPLY      10545
#define IDC_PREF_MANUALAPPLY    10546
#define IDC_PREF_MASKUNDO       10547
#define IDC_PREF_DISABLECMDLIST 10548

// The miscellaneous group
#define IDC_PREF_TOOLSETTINGS   10550
#define IDC_PREF_TABLET     	10551
#define IDC_PREF_TABLETUNUSED   10552
#define IDC_PREF_TABLETOPACITY  10553
#define IDC_PREF_TABLETSIZE 	10554
#define IDC_PREF_TABLETBOTH 	10555
#define IDC_PREF_FILETYPE		10556

#define IDC_PREF_STARTMACRO 10557
#define IDC_PREF_STARTMACRONONE 10558
#define IDC_PREF_DISABLEDITHER 	10559

#define IDC_PREF_MASKTINT   10560
#define IDC_PREF_MASKTINT1  10561
#define IDC_PREF_MASKTINT2  10562
#define IDC_PREF_MASKTINT3  10563
#define IDC_PREF_MASKTINT4  10564
#define IDC_PREF_MASKTINT5  10565
#define IDC_PREF_MASKTINT6  10566

#define IDC_PREF_PASTEATREALSIZE 	10567
#define IDC_PREF_USEGRAYSCALEICONS 	10568
#define IDC_PREF_DISABLEAUTOSCROLL 	10569

// Make sure to leave romm for more groups...
#define IDC_PREF_GROUP      	10570
#define IDC_PREF_GROUPMEMORY    10571
#define IDC_PREF_GROUPOBJECT    10572
#define IDC_PREF_GROUPSCRATCH   10573
#define IDC_PREF_GROUPPLUGIN    10574
#define IDC_PREF_GROUPUNITS 	10575
#define IDC_PREF_GROUPUNDO  	10576
#define IDC_PREF_GROUPMISC  	10577
#define IDC_PREF_GROUPLAST  	10578

// Pane swapping id's
#define IDC_PREF_PANEMEMORY     10580
#define IDC_PREF_PANEOBJECT     10581
#define IDC_PREF_PANESCRATCH    10582
#define IDC_PREF_PANEPLUGIN     10583
#define IDC_PREF_PANEUNITS      10584
#define IDC_PREF_PANEUNDO       10585
#define IDC_PREF_PANEMISC       10586

/* File open control start at   10600 */
#define IDC_FNAME           10600
#define IDC_FPATH           10601
#define IDC_FLIST           10602
#define IDC_DIRS            10603
#define IDC_DRIVES          10604

#define IDC_DISKSPACE       10609
#define IDC_SAVEPATH        10610

#define IDC_VIEWTHUMBNAILS  10616
#define IDC_TNROWS          10619
#define IDC_FILEMANAGE      10650
#define IDC_FILEINFO        10651
#define IDC_FILECOPY        10652
#define IDC_FILEDELETE      10653
#define IDC_FILERENAME      10654
#define IDC_THUMBNAILMANAGE 10655
#define IDC_CREATETHUMBNAILS    10656
#define IDC_DELETETHUMBNAILS    10657
#define IDC_DELETEALLTHUMBNAILS 10658
#define IDC_CANCEL          10659
#define IDC_AUTOTHUMBNAILS  10660

/* Feather object start at    10670 */
#define IDC_FEATHEROBJ_AMOUNT  10670
#define IDC_FEATHEROBJ_EDGE    10671
#define IDC_FEATHEROBJ_HARD    10672
#define IDC_FEATHEROBJ_NORMAL  10673
#define IDC_FEATHEROBJ_SOFT    10674
#define IDC_FEATHEROBJ_INOUT   10675
#define IDC_FEATHEROBJ_OUT     10676
#define IDC_FEATHEROBJ_ALL     10677
#define IDC_FEATHEROBJ_IN      10678

// Fastbit controls start at    10680
#define IDC_FAST_LOCK       10680
#define IDC_FAST_PREVIEW    10681
#define IDC_FAST_GRID       10682

/* About box controls start at  10700 */
#define VERSION_ICON        10700
#define IDC_CAST            10701
#define READ_ME             10702
#define IDC_DESIGNER        10703
#define IDC_CHARISMA        10704
#define IDC_DRAWPLUS        10705
#define IDC_PICTPUB         10706
#define IDC_CLIPART         10707
#define IDC_TECHSUPT        10708
#define IDC_MGX             10709
#define IDC_SERIALNO        10710
#define IDC_VERSION_NAME    10711
#define IDC_CASTTEXT        10712
#define IDC_ORGCHART        10713
#define IDC_ABCFLOWCHART    10714
#define IDC_PHOTOMAGIC      10715
#define IDC_PHOTOCREDITS    10716
#define IDC_ABOUTAPP        10717
#define IDC_CUSTOMER        10720
#define IDC_COMPANY     	10721
#define IDC_SNAPGRAFX		10722

/* Warning box controls start at    10800 */
#define SPA_BITMAP          10800
#define MGX_BITMAP          10801
#define TXTBLK_WARNING      10802

/* Nag box controls start at    10850 */
#define TXTBLK_TRIALDAYSLEFT    10850
#define TXTBLK_TRIALEXPIRED     10851
#define IDC_TRIALDAYSLEFT	    10852

/* New controls start at    10900 */
#define IDC_NEWTYPE         10900
#define IDC_NEWLINEART      10901
#define IDC_NEWGRAY         10902
#define IDC_NEWCOLOR        10903
#define IDC_NEWCMYKCOLOR    10904

/* The Edit/Paste options controls start    11000 */
#define IDC_PASTEANDBLEND   11000
#define IDC_BLENDLIGHT      11001
#define IDC_BLENDMEDIUM     11002
#define IDC_BLENDHEAVY      11003
#define IDC_SMARTPASTE      11004
#define IDC_PASTEINTOMASK   11005

/* Hue and saturation controls start at 11100 */
#define IDC_SLIDEHUE1       	11100
#define IDC_SLIDEHUE2       	11101
#define IDC_SLIDEHUE3       	11102
#define IDC_SLIDEHUE4       	11103
#define IDC_SLIDEHUE5       	11104
#define IDC_SLIDEHUE6       	11105
#define IDC_SLIDEHUE7       	11106
#define IDC_SLIDEHUE8       	11107
#define IDC_SLIDEHUE9       	11108
#define IDC_SLIDEHUE10      	11109
#define IDC_SLIDEHUE11      	11110
#define IDC_SLIDEHUE12      	11111
#define IDC_HUE1            	11112
#define IDC_HUE2            	11113
#define IDC_HUE3            	11114
#define IDC_HUE4            	11115
#define IDC_HUE5            	11116
#define IDC_HUE6            	11117
#define IDC_HUE7            	11118
#define IDC_HUE8            	11119
#define IDC_HUE9            	11120
#define IDC_HUE10           	11121
#define IDC_HUE11           	11122
#define IDC_HUE12           	11123
#define IDC_INHUE1          	11124
#define IDC_INHUE2          	11125
#define IDC_INHUE3          	11126
#define IDC_INHUE4          	11127
#define IDC_INHUE5          	11128
#define IDC_INHUE6          	11129
#define IDC_INHUE7          	11130
#define IDC_INHUE8          	11131
#define IDC_INHUE9          	11132
#define IDC_INHUE10         	11133
#define IDC_INHUE11         	11134
#define IDC_INHUE12         	11135
#define IDC_SATURATION      	11136
#define IDC_SATURATION_SCROLL   11137   
#define IDC_LIGHTNESS       	11138
#define IDC_LIGHTNESS_SCROLL    11139
#define IDC_HUE_PROBE			11140
#define IDC_HUE_COLORIZE		11141
#define IDC_HUE_VALUE			11142

// id's for map dialogs 11150-11349
/* Color Map controls start at  11150 */
#define IDC_BITMAP_BRIGHT1  11192
#define IDC_BITMAP_BRIGHT2  11193
#define IDC_MAP_HBAR        11194   
#define IDC_MAP_VBAR        11195   
#define IDC_AUTO 		    11196   
#define IDC_MAP_NUM         11197
#define IDC_MAP_GRID        11198
#define IDC_MAP_PERCENT     11199
#define IDC_MAPTOOL         11200
#define IDC_MAPTOOL11       11201
#define IDC_MAPTOOL5        11202
#define IDC_MAPTOOL3        11203
#define IDC_MAPTOOLV        11204

#define IDC_MAP             11205

#define IDC_MAP_PROBE       11206
#define IDC_MAP_OPTIONS     11207
#define IDC_MAP_USE_CURVES  11208 
#define IDC_MAP_CURVES      11209 

#define IDC_MAPIN           11210
#define IDC_MAP0IN          11211
#define IDC_MAP1IN          11212
#define IDC_MAP2IN          11213
#define IDC_MAP3IN          11214
#define IDC_MAP4IN          11215
#define IDC_MAP5IN          11216
#define IDC_MAP6IN          11217
#define IDC_MAP7IN          11218
#define IDC_MAP8IN          11219
#define IDC_MAP9IN          11220
#define IDC_MAP10IN         11221

#define IDC_MAPOUT          11222
#define IDC_MAP0OUT         11223
#define IDC_MAP1OUT         11224
#define IDC_MAP2OUT         11225
#define IDC_MAP3OUT         11226
#define IDC_MAP4OUT         11227
#define IDC_MAP5OUT         11228
#define IDC_MAP6OUT         11229
#define IDC_MAP7OUT         11230
#define IDC_MAP8OUT         11231
#define IDC_MAP9OUT         11232
#define IDC_MAP10OUT        11233

#define IDC_MAP_SHIFT_UP    11234
#define IDC_MAP_SHIFT_DOWN  11235
#define IDC_MAP_SHIFT_LEFT  11236
#define IDC_MAP_SHIFT_RIGHT 11237

#define IDC_MAP_GAMMA       11238
#define IDC_MAP_GAMMA_TEXT  11239



// Color balancer
#define IDC_CORRECT_R       11240
#define IDC_CORRECT_G       11241
#define IDC_CORRECT_B       11242
#define IDC_CORRECT_C       11243
#define IDC_CORRECT_M       11244
#define IDC_CORRECT_Y       11245
#define IDC_CORRECT_K       11246
#define IDC_CORRECT_W       11247
#define IDC_MAINTAIN        11252

// Map Control EXTRA's
#define IDC_MAPTOOL1_PANE   11253
#define IDC_MAPTOOL2_PANE   11254
#define IDC_MAP_LINES		11255
#define IDC_MAP_BEZIER		11256

#define IDC_CURVE_SMOOTH	11260
#define IDC_SMOOTH_FIRST	IDC_SMOOTH_EXSOFT
#define IDC_SMOOTH_EXSOFT	11261
#define IDC_SMOOTH_SOFT		11262
#define IDC_SMOOTH_NORMAL	11263
#define IDC_SMOOTH_HARD		11264
#define IDC_SMOOTH_EXHARD	11265
#define IDC_SMOOTH_LAST		IDC_SMOOTH_EXHARD

/* Various mapping controls start at    11300 */
#define IDC_MAPNEGATE       	11300
#define IDC_MAPLEVELS       	11301
#define IDC_MAPLEVELS_SCROLL    11302
#define IDC_MAPTHRESH       	11303
#define IDC_MAPTHRESH_SCROLL    11304

#define IDC_MAPSAVE         11307
#define IDC_MAPLOAD         11308
#define IDC_CONTBRIT        11309

#define IDC_CONTRAST        11310
#define IDC_BRIGHTNESS      11311
#define IDC_CALALLSAME		11312
#define IDC_CALGRAY			11314
#define IDC_CALRGB			11315

// id for visual calibration    11350-11499
/* Visual Calibration Controls start at 11350*/
#define IDC_VISUAL_BEFORE   11350
#define IDC_VISUAL_00       11351
#define IDC_VISUAL_01       11352
#define IDC_VISUAL_02       11353
#define IDC_VISUAL_10       11354
#define IDC_VISUAL_11       11355
#define IDC_VISUAL_12       11356
#define IDC_VISUAL_20       11357
#define IDC_VISUAL_21       11358
#define IDC_VISUAL_22       11359
#define IDC_VISUAL_PREVIEW  11360
#define IDC_VISUAL_DUMMY1   11361
#define IDC_VISUAL_DUMMY2   11362
#define IDC_VISUAL_DUMMY3   11363
#define IDC_VISUAL_DUMMY4   11364
#define IDC_VISUAL_DUMMY5   11365
#define IDC_VISUAL_DUMMY6   11366
#define IDC_VISUAL_DUMMY7   11367
#define IDC_VISUAL_DUMMY8   11368
#define IDC_VISUAL_DUMMY9   11369
#define IDC_VISUAL_DUMMY10  11370
#define IDC_VISUAL_DUMMY11  11371
#define IDC_VISUAL_DUMMY12  11372
#define IDC_VISUAL_DUMMY13  11373

#define IDC_VSTEP           11398
#define IDC_VSTEP_TEXT      11399

/* Channel selection combo start at 11400 */
#define IDC_CHANNELS        11400
#define IDC_DO_ALL          11401
#define IDC_DO_RED          11402
#define IDC_DO_GREEN        11403
#define IDC_DO_BLUE         11404
#define IDC_DO_CYAN         11405
#define IDC_DO_MAGENTA      11406
#define IDC_DO_YELLOW       11407
#define IDC_DO_BLACK        11408

#define IDC_SCAN_CHANNELS   11410
#define IDC_SCAN_RED        11411
#define IDC_SCAN_GREEN      11412
#define IDC_SCAN_BLUE       11413
#define IDC_SCAN_GRAY		11414

/* Band combo for color balance start at    11420*/
#define IDC_BAND            11420
#define IDC_BAND_ALL        11421
#define IDC_BAND_HI         11422
#define IDC_BAND_MID        11423
#define IDC_BAND_LO         11424

/* Brush controls start at  11500 */
#define IDC_PRESSLIGHT      	11500
#define IDC_PRESSMEDIUM     	11501
#define IDC_PRESSHEAVY      	11502
#define IDC_BRUSHSHAPE      	11505
#define IDC_BRUSHCIRCLE     	11506
#define IDC_BRUSHSQUARE     	11507
#define IDC_BRUSHHORIZONTAL 	11508
#define IDC_BRUSHVERTICAL   	11509
#define IDC_BRUSHSLASH      	11510
#define IDC_BRUSHBACKSLASH  	11511
#define IDC_BRUSHCUSTOM     	11512
#define IDC_BRUSHCUSTOMNEW  	11513
#define IDC_FIRST_BRUSHSHAPE    IDC_BRUSHCIRCLE
#define IDC_BRUSHSIZE       	11514
#define IDC_BRUSHICON       	11515
#define IDC_FLOODRANGE      	11516
#define IDC_FEATHER         	11517
#define IDC_MIDPOINT        	11518
#define IDC_TRANSPARENT     	11519
#define IDC_TRANSPARENT_SCROLL  11520
#define IDC_PRESSURE        	11521
#define IDC_PRESSURE_SCROLL 	11522
#define IDC_OPACITYPRESSURE 	11523
#define IDC_FLOODFADE      		11524

#define IDC_CLONESOURCE     11530
#define IDC_MIRRORX         11531
#define IDC_MIRRORY         11532

#define IDC_SPACING         11540
#define IDC_FADESIZE        11541
#define IDC_FADEOPACITY     11542
#define IDC_MINTRANSPARENCY 11543
#define IDC_OVERLAP         11544
#define IDC_SMOOTHNESS      11545
#define IDC_STYLEPRESSURE   11546
#define IDC_BRUSHPREVIEW    11547
#define IDC_PREVIEWRESET    11548

#define IDC_BRUSHSTYLE      11560
#define IDC_BRUSHSOLID      11561
#define IDC_BRUSHSCATTER    11562

/* Mask controls start at   11600 */
#define IDC_AUTOSEARCHDEPTH 11601
#define IDC_AUTOMINEDGE     11602
#define IDC_MASKMETHOD      11603

#define IDC_COLORMODEL      	11611
#define IDC_FIRST_MODEL     	11612
#define IDC_MODEL_RGB       	11612
#define IDC_MODEL_HL        	11613
#define IDC_MODEL_HSL       	11614
#define IDC_MODEL_LUM       	11615
#define IDC_MODEL_HUE       	11616
#define IDC_MODEL_SAT       	11617
#define IDC_MODEL_HS        	11618
#define IDC_MODEL_SL        	11619
#define IDC_LAST_MODEL      	IDC_MODEL_HL
#define IDC_PATHCONTINUITY  	11621
#define IDC_PATHCURVE_TOLERANCE	11622
#define IDC_GROW            	11623
#define IDC_GROW_SPIN       	11624
#define IDC_SIMILAR         	11625
#define IDC_POINTTYPE       	11626
#define IDC_ADDBEZIER       	11627
#define IDC_PATHTOLERANCE   	11628

#define IDC_POINT_WND           11630
#define IDC_AUTOMASK_WND        11631
#define IDC_FREEHAND_WND        11632
#define IDC_FIRSTFREE_WND       IDC_POINT_WND
#define IDC_LASTFREE_WND        IDC_FREEHAND_WND

#define IDC_PATHTYPE			11650
#define IDC_PATH_LINES			11651
#define IDC_PATH_CURVES			11652

// TWAIN Additions - Not used for actual file saves - MMB
#define IDC_SAVETWAINDIB    11722
#define IDC_SAVETWAINFILE   11723

/* Draw controls start at   11800 */
//#define IDC_CIRSTYLE      11800
#define IDC_SQUSTYLE        11800
#define IDC_SQUEDGE         11801
#define IDC_SQUFILL         11802
#define IDC_SQUEDGEANDFILL  11803
#define IDC_CIREDGE         11804
#define IDC_CIRFILL         11805
#define IDC_CIREDGEANDFILL  11806
#define IDC_FRESTYLE        11808
#define IDC_FREEDGE         11809
#define IDC_FREFILL         11810
#define IDC_FREEDGEANDFILL  11811

/* Process controls start at    11900 */
#define IDC_HEIGHT          11900
#define IDC_WIDTH           11901
#define IDC_RES             11902
#define IDC_SCALEX          11903
#define IDC_SCALEY          11904
#define IDC_MEMORY          11905
#define IDC_DISTORT         11906
#define IDC_SMARTSIZE       11907
#define IDC_SIGMA           11908
#define IDC_MAINTAINSIZE    11909

/* Rotate any controls start at 12000 */
#define IDC_ANGLE           12000
#define IDC_CWISE           12001
#define IDC_CCWISE          12002

/* Unit Labels start at 12100 */
#define IDC_HEIGHTLABEL     12100
#define IDC_WIDTHLABEL      12101
#define IDC_TOPLABEL        12102
#define IDC_LEFTLABEL       12103
#define IDC_BOTTOMLABEL     12104
#define IDC_RIGHTLABEL      12105
#define IDC_FIRSTLABEL      IDC_HEIGHTLABEL
#define IDC_LASTLABEL       IDC_RIGHTLABEL

/* Print and Print Setup controls start at  12200 */
#define IDC_PRINTER         	12200
#define IDC_PRINTSTYLE      	12201
#define IDC_PRINTVIEW       	12202
#define IDC_PRINTVIEWVISUAL 	12203
#define IDC_PRINTVIEWNUMERIC    12204

#define IDC_PRINTVIEWEXT    12205
#define IDC_PRINTVIEWPHYS   12206
#define IDC_PRINTVIEWPAGE   12207
#define IDC_PRINTVIEWIMAGE  12208

#define IDC_FIRSTPRINTITEM  12209
#define IDC_OFFSETX         12210
#define IDC_OFFSETY         12211
#define IDC_CENTERED        12212
#define IDC_SCATTER         12213
#define IDC_PRINTRESET      12214
#define IDC_PRINTTESTPAGE   12215
#define IDC_PRINTSELECT     12216
#define IDC_PRINTSETUP      12217

#define IDC_PRINT_TYPE      12250
#define IDC_PRINT_GRAY      12251
#define IDC_PRINT_COLOR     12252
#define IDC_PRINT_COLORGRAY 12253
#define IDC_PRINT_BLACKSEPS 12254
#define IDC_PRINT_COLORSEPS 12255

#define IDC_DO_CPATCH       12256
#define IDC_DO_MPATCH       12257
#define IDC_DO_YPATCH       12258
#define IDC_DO_KPATCH       12259

/* Print Setup Controls start at    12300 */
#define IDC_SETUPPRINTER    12301
#define IDC_SETUPSTYLE      12302
#define IDC_TRIMMARKS       12303
#define IDC_REGMARKS        12304
#define IDC_LABELS          12305
#define IDC_STEPSCALE       12306
#define IDC_NEGATIVE        12307
#define IDC_EMULSIONDOWN    12308
#define IDC_BINARY_PS       12309
#define IDC_PRINTER_SCREENING 12310
#define IDC_USEPRINTSCALING 12311

#define IDC_PRINTERTYPES    12320
#define IDC_PRINTER_IS_MONO 12321
#define IDC_PRINTER_IS_CMYK 12322
#define IDC_PRINTER_IS_RGB  12323

/* Print Style controls start at 12400 */
#define IDC_HALFTONENAMES   12401
#define IDC_HALFTONEFILE    12402
#define IDC_HALFTONEMGR     12403

#define IDC_CALIBRATENAMES  12405
#define IDC_CALIBRATEFILE   12406
#define IDC_CALIBRATEMGR    12407

#define IDC_SEPARATENAMES   12409
#define IDC_SEPARATEFILE    12410
#define IDC_SEPARATEMGR     12411

#define IDC_BALNAMES		12413
#define IDC_BALFILE    		12414
#define IDC_BALMGR     		12415

#define IDC_BLACKNAMES		12417
#define IDC_BLACKFILE  		12418
#define IDC_BLACKMGR   		12419

/* Print Halftoning controls start at   12500 */
#define IDC_DOTSHAPES       	12500
#define IDC_CIRCLEDOT       	12501
#define IDC_SQUAREDOT       	12502
#define IDC_ELLIPSEDOT      	12503
#define IDC_TRIANGLEDOT     	12503 // Duplicate - UNUSED
#define IDC_PROPELLERDOT    	12503 // Duplictae - UNUSED
#define IDC_SCRULING0       	12504
#define IDC_SCRULING1       	12505
#define IDC_SCRULING2       	12506
#define IDC_SCRULING3       	12507
#define IDC_ANGLE0          	12508
#define IDC_ANGLE1          	12509
#define IDC_ANGLE2          	12510
#define IDC_ANGLE3          	12511
#define IDC_HILIGHT         	12512
#define IDC_SHADOW          	12513
#define IDC_USEPRINTERHALFTONE 	12514
#define IDC_CBAL				12515
#define IDC_MBAL				12516
#define IDC_YBAL				12517
#define IDC_KBAL				12518
#define IDC_DOTGAIN				12519
#define IDC_CALONCONVERT		12520
#define IDC_NOCALONCONVERT		12521

/* Print Ink correction controls start at   12600 */
#define IDC_CORRECT_RED     12601
#define IDC_CORRECT_GREEN   12602
#define IDC_CORRECT_BLUE    12603
#define IDC_M_IN_R          12604
#define IDC_Y_IN_R          12605
#define IDC_Y_IN_G          12606
#define IDC_C_IN_G          12607
#define IDC_C_IN_B          12608
#define IDC_M_IN_B          12609
#define IDC_UCR             12610
#define IDC_BLACKLIMIT		12611
#define IDC_TOTALINK		12612
#define IDC_GRAYRAMP		12613
#define IDC_SATBOOST		12614

/* New Print Style dialog controls start at 12620 */
#define IDC_HALFTONESTYLE		12620
#define IDC_CALIBRATIONSTYLE	12621
#define IDC_SEPARATIONSTYLE		12622
#define IDC_EXTRASSTYLE			12623
#define IDC_FIRSTEDITSTYLE		IDC_HALFTONESTYLE
#define IDC_LASTEDITSTYLE		IDC_EXTRASSTYLE

#define IDC_BLACKSTYLE			12624
#define IDC_INKSTYLE			12625
#define IDC_FIRSTSEPSTYLE		IDC_BLACKSTYLE
#define IDC_LASTSEPSTYLE		IDC_INKSTYLE

#define IDC_PPSEPARATIONS		12640
#define IDC_CMSSEPARATIONS		12641
#define IDC_FIRSTSEPTYPE		IDC_PPSEPARATIONS
#define IDC_LASTSEPTYPE			IDC_CMSSEPARATIONS

#define IDC_PRINTUSECMS			12642

/* Map Apply Calibration controls start 12650 */
#define IDC_CALSCANNAMES    	12650
#define IDC_CALSCANFILE     	12651
#define IDC_CALSCANMGR      	12652
#define IDC_CALPRINTNAMES   	12653
#define IDC_CALPRINTFILE    	12654
#define IDC_CALPRINTMGR     	12655
#define IDC_CALVISUALNAMES    	12656
#define IDC_CALVISUALFILE     	12657
#define IDC_CALVISUALMGR      	12658

/* Print Calibration controls start 12700 */
#define IDC_CAL_CHANNELS	12699
#define IDC_CAL_MASTER 		12700
#define IDC_CAL_CYAN        12701
#define IDC_CAL_MAGENTA     12702
#define IDC_CAL_YELLOW      12703
#define IDC_CAL_BLACK       12704

#define IDC_CALMETHOD		12705
#define IDC_CALMEASURE		12706
#define IDC_CALVISUAL		12707

#define IDC_CALVIEW_CHANNELS	12710
#define IDC_CALVIEW_CYAN        12711
#define IDC_CALVIEW_MAGENTA     12712
#define IDC_CALVIEW_YELLOW      12713
#define IDC_CALVIEW_BLACK       12714

#define IDC_SCANMAPTYPE		12720
#define IDC_SCANMAPGRAY		12721
#define IDC_SCANMAPCOLOR 	12722

/* Color Shield Controls start at   12900 */
#define IDC_SHIELD_IFHIDDEN 12900
#define IDC_SHIELD_PROBE    12901

#define IDC_MASKCOLOR1      12921
#define IDC_MASKCOLOR2      12922
#define IDC_MASKCOLOR3      12923
#define IDC_MASKCOLOR4      12924
#define IDC_MASKCOLOR5      12925
#define IDC_MASKCOLOR6      12926
#define IDC_MASKCOLOR7      12927
#define IDC_MASKCOLOR8      12928

#define IDC_MASKRANGE1      12931
#define IDC_MASKRANGE2      12932
#define IDC_MASKRANGE3      12933
#define IDC_MASKRANGE4      12934
#define IDC_MASKRANGE5      12935
#define IDC_MASKRANGE6      12936
#define IDC_MASKRANGE7      12937
#define IDC_MASKRANGE8      12938

#define IDC_MASKACTIVE1     12941
#define IDC_MASKACTIVE2     12942
#define IDC_MASKACTIVE3     12943
#define IDC_MASKACTIVE4     12944
#define IDC_MASKACTIVE5     12945
#define IDC_MASKACTIVE6     12946
#define IDC_MASKACTIVE7     12947
#define IDC_MASKACTIVE8     12948

#define IDC_SHIELDSELECT    12950
#define IDC_MASKINCLUDE     12951
#define IDC_MASKEXCLUDE     12952
#define IDC_CHROMASELECT    12953

/* Vignette Controls start at   13000 */
#define IDC_VIGNETTETYPE    	13000
#define IDC_VIGLINEAR       	13001
#define IDC_VIGRADIAL       	13002
#define IDC_VIGCIRCLE       	13003
#define IDC_VIGELLIPSE      	13004
#define IDC_VIGSQUARE       	13005
#define IDC_VIGRECTANGLE    	13006
#define IDC_VIGFIRST        	IDC_VIGLINEAR
#define IDC_VIGREPEAT       	13007
#define IDC_VIGTRANSITION   	13008
#define IDC_VIGHARDTRANSITION   13009
#define IDC_VIGSOFTTRANSITION   13010
#define IDC_VIGCOLORMODEL   	13020
#define IDC_VIGRGB          	13021
#define IDC_VIGHSL          	13022

/* Texture Fill Controls start at   13100 */
#define IDC_TEXTURENAME     13100
#define IDC_TEXTUREFLIPX    13101
#define IDC_TEXTUREFLIPY    13102

/* Scan Setup Controls start at 13150 */
#define IDC_GRABFILE        13151
#define IDC_GRABDATE        13152
#define IDC_SCANUSECMS 		13153

/* Scan Controls start at   13200 */
#define IDC_COLORPREVIEW    13202
#define IDC_SCANFRAME       13203
#define IDC_PRESCAN     	13204
#define IDC_LOCKASPECT      13205

#define IDC_VELOCITY1       13208
#define IDC_VELOCITY2       13209
#define IDC_VELOCITY3       13210
#define IDC_VELOCITY4       13211
#define IDC_VELOCITY5       13212
#define IDC_SCANMAP         13213
#define IDC_SCANMIRROR      13214
#define IDC_SCANINVERT      13215
#define IDC_DMA             13216
#define IDC_SCANCHECK       13217
#define IDC_OPTIONS         13218
#define IDC_SCANTYPE        13220
#define IDC_SCAN1BIT        13221
#define IDC_SCAN8BIT        13222
#define IDC_SCANCOLOR       13223

/* Effects Controls start at    13300 */
#define IDC_FXPREVIEW       13300
#define IDC_FXRESET         13301
#define IDC_FXAPPLY         13302
#define IDC_FXBOX           13303
#define IDC_FXINOUT         13304
#define IDC_FXIMAGE_PREVIEW 13305
#define IDC_FXIMAGE_FULLVIEW    13306

/* Text Controls start at   13400 */
#define IDC_TEXT_STRING     	13400
#define IDC_TEXT_FACES      	13401
#define IDC_TEXT_SIZE       	13402
#define IDC_TEXT_ITALIC     	13403
#define IDC_TEXT_UNDERLINE  	13404
#define IDC_TEXT_STRIKEOUT  	13405
#define IDC_TEXT_LIGHT      	13406
#define IDC_TEXT_NORMAL     	13407
#define IDC_TEXT_BOLD       	13408
#define IDC_TEXT_WEIGHT     	13409
#define IDC_TEXT_ANGLE      	13410
#define IDC_TEXT_ANTIALIAS  	13411
#define IDC_STATICTEXT      	13412
#define IDC_GETDLGSTRING_TEXT   13413
#define IDC_TEXT_LEFT			13414
#define IDC_TEXT_RIGHT			13415
#define IDC_TEXT_CENTER			13416

/* Mask Constraint Controls start at    13500 */
#define IDC_ASPECTRATIO     13500
#define IDC_UNCONSTRAIN     13501
#define IDC_CONSTRAIN       13502
#define IDC_FIXEDSIZE       13503
#define IDC_ASPECTFIRST     IDC_UNCONSTRAIN
#define IDC_ASPECTLAST      IDC_FIXEDSIZE

#define IDC_HCONSTRAIN      13510
#define IDC_WCONSTRAIN      13511
#define IDC_MASKSHAPE       13512
#define IDC_SQUAREMASK      13513
#define IDC_CIRCLEMASK      13514
#define IDC_CONSTRAINSIZE_WND   13520
#define IDC_CONSTRAINUNIT_WND   13521

/* Status Box Controls start at 13600 */
#define IDC_STATUSTEXT      13600
#define IDC_STATUSINFO      13601
#define IDC_STATUSSHIELD    13602
#define IDC_STATUSMASK      13603
#define IDC_STATUSPALETTE   13604
#define IDC_STATUSGLOBAL    13605
#define IDC_MASKEDITALPHA   13606
#define IDC_VIEWSLIME       13607

/* Extended Name ComboBox Control start at  13700 */
#define IDC_EXTNAMES        13700
#define IDC_FILENAME        13701
#define IDC_EXTMANAGE       13702
#define IDC_CLIPEXTMANAGE   13703
#define IDC_CREATEEXTMANAGE 13704
#define IDC_EDITEXTMANAGE   13705
#define IDC_BRUSHEXTMANAGE  13706

#define IDC_CREATEEXT       	13707
#define IDC_EDITEXT         	13708
#define IDC_ADDEXT          	13709
#define IDC_DELETEEXT       	13710
#define IDC_RENAMEEXT       	13711
#define IDC_PASTEEXTMANAGE		13712
#define IDC_PASTEEXTMANAGE_MULT	13713
#define IDC_PASTE_DELETEEXT		13714
#define IDC_EXTDUMMY			13720

/* Quartertone controls start at    13800 */
#define IDC_HILIGHTS_SACRIFICED 13800
#define IDC_SHADOWS_SACRIFICED  13801
#define IDC_HISTOGRAM       	13803
#define IDC_QUART1          	13804
#define IDC_QUART2          	13805
#define IDC_QUART3          	13806
#define IDC_QUART1_PROBE    	13807
#define IDC_QUART2_PROBE    	13808
#define IDC_QUART3_PROBE    	13809
#define IDC_QUART1_VALUE    	13810
#define IDC_QUART2_VALUE    	13811
#define IDC_QUART3_VALUE    	13812
#define IDC_MINHIGHLIGHT    	13813
#define IDC_MAXSHADOW    		13814
#define IDC_QUART_DEFAULTS 		13815
#define IDC_USE_MID_PER 		13816
#define IDC_TAGS				13817
#define IDC_HISTOTAG1			13818
#define IDC_HISTOTAG2			13819

/* Apply or undo message controls start at  13900 */
#define IDC_APPLY           13900
#define IDC_UNDO            13901

/* Custom brush prompt controls start at    14000 */
#define IDC_EDIT_BRUSH      14000

/* Image Information controls start at  14200 */
#define IDC_INFOFILENAME    	14200
#define IDC_INFOFILETYPE    	14201
#define IDC_INFODATATYPE    	14202
#define IDC_INFOMODIFIED    	14203
#define IDC_INFOIMAGEMEMORY 	14204
#define IDC_INFOEDITPERCENT 	14205
#define IDC_INFOUNDOPERCENT 	14206
#define IDC_INFOMEMORY      	14207
#define IDC_INFOWIDTH       	14208
#define IDC_INFOHEIGHT      	14209
#define IDC_INFORESOLUTION  	14210
#define IDC_INFOIMAGEDISK   	14211
#define IDC_INFONUMOBJECTS  	14212
#define IDC_INFOOBJECTSMEMORY 	14213
#define IDC_INFOMASKEDITPERCENT 14214
#define IDC_INFOMASKUNDOPERCENT 14215

/* Scan Calibration controls start at   14300 */
#define IDC_MEASURE         14300
#define IDC_RMEASURE1       14301
#define IDC_RMEASURE2       14302
#define IDC_RMEASURE3       14303
#define IDC_RMEASURE4       14304
#define IDC_RMEASURE5       14305
#define IDC_RMEASURE6       14306
#define IDC_RMEASURE7       14307
#define IDC_RMEASURE8       14308
#define IDC_RMEASURE9       14309
#define IDC_RMEASURE10      14310
#define IDC_RMEASURE11      14311
#define IDC_GMEASURE1       14321
#define IDC_GMEASURE2       14322
#define IDC_GMEASURE3       14323
#define IDC_GMEASURE4       14324
#define IDC_GMEASURE5       14325
#define IDC_GMEASURE6       14326
#define IDC_GMEASURE7       14327
#define IDC_GMEASURE8       14328
#define IDC_GMEASURE9       14329
#define IDC_GMEASURE10      14330
#define IDC_GMEASURE11      14331
#define IDC_BMEASURE1       14341
#define IDC_BMEASURE2       14342
#define IDC_BMEASURE3       14343
#define IDC_BMEASURE4       14344
#define IDC_BMEASURE5       14345
#define IDC_BMEASURE6       14346
#define IDC_BMEASURE7       14347
#define IDC_BMEASURE8       14348
#define IDC_BMEASURE9       14349
#define IDC_BMEASURE10      14350
#define IDC_BMEASURE11      14351
#define IDC_XMEASURE1       14361
#define IDC_XMEASURE2       14362
#define IDC_XMEASURE3       14363
#define IDC_XMEASURE4       14364
#define IDC_XMEASURE5       14365
#define IDC_XMEASURE6       14366
#define IDC_XMEASURE7       14367
#define IDC_XMEASURE8       14368
#define IDC_XMEASURE9       14369
#define IDC_XMEASURE10      14370
#define IDC_XMEASURE11      14371

/* Copy To... dialog controls start at  14390 */
#define IDC_COPYTOCLIP      14390
#define IDC_COPYTOIMAGE     14391
#define IDC_COPYTOTEXTURE   14392
#define IDC_COPYTOBRUSH     14393
#define IDC_COPYTONAME      14394

/* Summary Dialog dialog controls start at 14400  see IDD_SUMMDLG */
#define IDC_SUMM_AUTHOR     14400
#define IDC_SUMM_KEYWORDS   14401
#define IDC_SUMM_COMMENTS   14402
#define IDC_SUMM_TITLE      14403
#define IDC_SUMM_SUBJECT    14404
#define IDC_SUMM_CREATED    14405
#define IDC_SUMM_LASTAUTHOR 14406
#define IDC_SUMM_LASTSAVED  14407
#define IDC_SUMM_LASTPRINT  14408

/* Mask transformer controls start at   14500 */
#define IDC_TRANSFUNCTION   14500
#define IDC_COPYMASK        14501
#define IDC_MOVEMASK        14502
#define IDC_COPYIMAGE       14503
#define IDC_MOVEIMAGE       14504
#define IDC_TRANSFUNCTIONFIRST  IDC_COPYMASK
#define IDC_TRANSFUNCTIONLAST   IDC_MOVEIMAGE

#define IDC_TRANSMODE       	14510
#define IDC_TRANSSCALE      	14511
#define IDC_TRANSSKEW       	14512
#define IDC_TRANSPERSPECTIVE    14513
#define IDC_TRANSDISTORT    	14514
#define IDC_TRANSMODEFIRST  	IDC_TRANSSCALE
#define IDC_TRANSMODELAST   	IDC_TRANSDISTORT

#define IDC_TRANSFLIPX      14520
#define IDC_TRANSFLIPY      14521
#define IDC_TRANSROTATEXY   14522
#define IDC_TRANSROTATEX    14523
#define IDC_TRANSROTATEY    14524
#define IDC_TRANSBLEND      14525
#define IDC_NOBLEND_WND     14526
#define IDC_BLEND_WND       14527


/* Probe Controls start at  14700 */
#define IDC_PROBEMETHOD     14700
#define IDC_PROBEPOINT      14701
#define IDC_PROBERECT       14702

/* Monitor Gamma Controls start at  14800 */
#define IDC_RGAMMA          14800
#define IDC_RGAMMA_SCROLL   14801
#define IDC_RGAMMA_EDIT     14802
#define IDC_GGAMMA          14803
#define IDC_GGAMMA_SCROLL   14804
#define IDC_GGAMMA_EDIT     14805
#define IDC_BGAMMA          14806
#define IDC_BGAMMA_SCROLL   14807
#define IDC_BGAMMA_EDIT     14808
#define IDC_GAMMA           14809
#define IDC_GAMMA_DISABLE	14810

/* Split Dialog Controls start at   14830 */
#define IDC_SPLITRGB        14830
#define IDC_SPLITHSL        14831

/* Scanner/Grabber Info dialog box controls start at    14840 */
#define IDC_SCANNAME        14843
#define IDC_SCANFILE        14844
#define IDC_SCANDATE        14845

/* EPS Options dialog box controls start at 14860 */
#define IDC_PREVIEWTYPE		14860
#define IDC_PREVIEWNONE     14861
#define IDC_PREVIEW1        14862
#define IDC_PREVIEW8        14863
#define IDC_PREVIEW24		14864
#define IDC_DODCS           14865
#define IDC_EPSCOMPOSITE    14866
#define IDC_STYLELABEL      14867
#define IDC_STYLENAME       14868

#define IDC_PROMPTFOROPTIONS    14869
#define IDC_TIFFCOMPRESS    14870
#define IDC_TIFFDIFF        14871
#define IDC_PPFFCOMPRESS    14872
#define IDC_PPFFSAVEMASK    14873
#define IDC_PPFFSAVEPROXY   14874
#define IDC_PPFFSAVECMDLIST 14875
#define IDC_EXPORTMASK		14876
#define IDC_PPFFSAVELINK	14877
#define IDC_TIFFSAVEMASK	14878

// Avi Options
#define IDC_AVI_ADD_END     14880
#define IDC_AVI_INSERT      14881
#define IDC_AVI_REPLACE     14882
#define IDC_AVI_POSI        14883
#define IDC_AVI_POSR        14884
#define IDC_AVI_CLIP        14885
#define IDC_AVI_SIZE        14886

/* File Location dialog box controls start at   14900 */
#define IDC_LOCATE_TEXTURES 		14900
#define IDC_LOCATE_DEVICES  		14901
#define IDC_LOCATE_CLIPBITS 		14902
#define IDC_LOCATE_FILTERS  		14903
#define IDC_LOCATE_MAPS     		14904
#define IDC_LOCATE_PRNSTYLES    	14905
#define IDC_LOCATE_BRUSHES  		14906
#define IDC_LOCATE_PALETTES 		14907
#define IDC_LOCATE_MACROS   		14908
#define IDC_LOCATE_IMPORTFILTERS  	14909

/* Stitch Options ribbon controls start at  15000 */
#define IDC_STITCHPOINT1    15000
#define IDC_STITCHPOINT2    15001
#define IDC_STITCHIMAGE1    15002
#define IDC_STITCHIMAGE2    15003
#define IDC_STITCHPOINT1C   15004
#define IDC_STITCHPOINT2C   15005
#define IDC_STITCHPOINT1S   15006
#define IDC_STITCHPOINT2S   15007
#define IDC_STITCHALIGN     15008
#define IDC_DOSTITCH        15009

/* Stitch Dialog options controls start at  15100 */
#define IDC_AUTOBRIGHTNESS  15100
#define IDC_STITCHBRIGHTNESS    15101
#define IDC_STITCHBRIGHTNESS_SCROLL 15102
#define IDC_STITCHBLEND     15103
#define IDC_SMARTSTITCH     15104
#define IDC_STITCHTRANS1    15105
#define IDC_STITCHTRANS2    15106
#define IDC_STITCHPRESSURE  15107
#define IDC_STITCHPRESSURE_SCROLL   15108

/* Convert To... Options ribbon controls start at   15300 */
#define IDC_MINITYPES       15340
#define IDC_MINI256         15341
#define IDC_MINI16          15342
#define IDC_MINI8           15343
#define IDC_MINICUSTOM		15344

#define IDC_PALETTECOLORS	15350

#define IDC_DITHERTYPES     15360
#define IDC_DITHERNONE      15361
#define IDC_DITHERPATTERN   15362
#define IDC_DITHERSCATTERED 15363

#define IDC_PALETTETYPES        15370
#define IDC_PALETTEOPTIMIZED    15371
#define IDC_PALETTESTANDARD     15372
#define IDC_PALETTECUSTOM		15373

/* File Manager Copy... and Rename... controls start at 15500 */
#define IDC_FILEDIR         15500
#define IDC_FILEFROM        15501
#define IDC_FILETO          15502
#define IDC_FROMSTATUS      15503
#define IDC_TOSTATUS        15504
#define IDC_FROMLABEL       15505
#define IDC_TOLABEL         15506

/* Advanced Editing Controls start at 15600 */
#define IDC_GLOBALMODE      15600
#define IDC_MERGEMODE       15600

#define IDC_SURFACENORMAL   15650
#define IDC_GLOBAL_IFHIDDEN 15651

/* Palette Add Controls start at 15700 */
#define IDC_PALENTRIES      15700
#define IDC_CPOPALETTE      15701

/* JPEG Save Options Controls start at 15800 */
#define IDC_SUBSAMPLING_COMBO	 15800
#define IDC_COMPRESSION_SLIDER	 15801
#define IDC_COMPRESSION_EDIT	 15802

/*These will need to go away when the dialog is re-done to support hinting*/
#define IDC_JPEGAMOUNT      15830
#define IDC_JPEGLESS        15831
#define IDC_JPEGNORMAL      15832 
#define IDC_JPEGMORE        15833
#define IDC_JPEGCUSTOM      15834
#define IDC_JPEGSUBSAMPLING 15840
#define IDC_JPEGLR          15841
#define IDC_JPEGMR          15842
#define IDC_JPEGHR          15843
#define IDC_JPEGYQUANT      15850
#define IDC_JPEGCQUANT      15851

/* Photo CD Open Options Controls start at 15900 */
#define IDC_PCDFORMAT       15900
#define IDC_PCDSINGLE       15901
#define IDC_PCDRGB          15902
#define IDC_PCDRESOLUTION   15910
#define IDC_PCD_BASE_OVER_16 15911
#define IDC_PCD_BASE_OVER_4 15912
#define IDC_PCD_BASE        15913
#define IDC_PCD_4BASE       15914
#define IDC_PCD_16BASE      15915
#define IDC_PCD_64BASE      15916
#define IDC_PCD_IMAGESIZE   15920
#define IDC_PCD_CROPSIZE    15921
#define IDC_PCD_DOPREVIEW   15922
#define IDC_PCD_PREVIEW		15923
#define IDC_PCD_BRIGHTBUTTON 15924
#define IDC_PCD_REDBUTTON	15925
#define IDC_PCD_GREENBUTTON 15926
#define IDC_PCD_BLUEBUTTON 	15927
#define IDC_PCD_SATBUTTON  	15928
#define IDC_PCD_OUTOFGAMUT	15929
#define IDC_PCD_SUBTRACTSBA	15930
#define IDC_PCD_INFO		15931
#define IDC_PCD_MEDIAID			15932
#define IDC_PCD_MEDIATYPE		15933
#define IDC_PCD_SCANNERVENDOR 	15934
#define IDC_PCD_SCANNERMODEL  	15935
#define IDC_PCD_EQUIPMENT		15936
#define IDC_PCD_COPYRIGHT		15937
#define IDC_PCD_COLORNEGATIVE	15938
#define IDC_PCD_COLORREVERSAL	15939
#define IDC_PCD_COLORHARDCOPY	15940
#define IDC_PCD_THERMALHARDCOPY	15941
#define IDC_PCD_BWNEGATIVE		15942
#define IDC_PCD_BWREVERSAL		15943
#define IDC_PCD_BWHARDCOPY		15944
#define IDC_PCD_INTERNEGATIVE	15945
#define IDC_PCD_SYNTHETICIMAGE	15946

// AVI controls start at 15950
#define IDC_AVI_PREVIEW     15950
#define IDC_AVI_HOME        15951
#define IDC_AVI_END         15952
#define IDC_AVI_STEP        15953
#define IDC_AVI_RSTEP       15954
#define IDC_AVI_PLAY        15955
#define IDC_AVI_RPLAY       15956
#define IDC_AVI_STOP        15957
#define IDC_AVI_CONFIGURE   15958
#define IDC_AVI_FRAMES      15959
#define IDC_AVI_FRAME       15960
#define IDC_AVI_LASTFRAME   15961

/* Photo CD File Info Controls start at 16000 */
#define IDC_INFOSCANTIME    16000
#define IDC_INFOLASTMODIFIED    16001
#define IDC_INFOMEDIATYPE   16002
#define IDC_INFOSCANNERVENDOR   16003
#define IDC_INFOSCANNERTYPE 16004
#define IDC_INFOCOPYRIGHT   16005

/* Selector Tool Controls start at 16020 */
#define IDC_TOTOP               16020
#define IDC_TOBOTTOM            16021
#define IDC_OBJECTGROUP         16023
#define IDC_OBJECTDELETE        16024
#define IDC_MASKTRANSFORM       16025
#define IDC_SELECTORTRANSFORM   16026
#define IDC_OBJECTLOCK			16027

/* Color Management Controls */
#define IDC_CLRMGRINP   16030
#define IDC_CLRMGROUT   16031
#define IDC_CLRMGRMON   16032
#define IDC_CLRMGRSIM   16033

#define IDC_CLRMGRPREVIEW   16040
#define IDC_CLRMGRAPPLY 16041

// Color Manager Icons
#define IDC_CLRMGRINPICON   16045
#define IDC_CLRMGRMONICON   16046
#define IDC_CLRMGROUTICON   16047
#define IDC_CLRMGRSIMICON   16048

// Calibration Mode controls
#define IDC_CLRMGRINPMODE 16060
#define IDC_CLRMGRMONMODE 16061
#define IDC_CLRMGROUTMODE 16062
#define IDC_CLRMGRSIMMODE 16063

/* Display setup for color manager */
#define IDC_CLRMGRDISPENABLER 16070
#define IDC_CLRMGRDISPSETUP 16071

/* Open Low res dialog id's */
#define IDC_OPENRES         16180

// Macro dialogs
#define IDC_MACRO_COUNT     16185

/* Paint On Mask ribbon id's */
#define IDC_PAINTONDEST     16190
#define IDC_PAINTONMASK     16191
#define IDC_PAINTONOBJECT   16192

/* Object Manager dialog id's */
#define IDC_VIEWOBJUP       16200
#define IDC_VIEWOBJDOWN     16201
#define IDC_VIEWOBJ0        16202
#define IDC_VIEWOBJ1        16203
#define IDC_VIEWOBJ2        16204
#define IDC_VIEWOBJ3        16205
#define IDC_VIEWOBJ4        16206
#define IDC_VIEWOBJ5        16207
#define IDC_VIEWOBJ6        16208
#define IDC_VIEWOBJ7        16209
#define IDC_VIEWOBJ8        16210
#define IDC_VIEWOBJ9        16211
#define IDC_VIEWOBJFIRST    IDC_VIEWOBJ0
#define IDC_VIEWOBJLAST     IDC_VIEWOBJ5

/* Expand dialog id's */
#define IDC_EXPANDTOP       16230
#define IDC_EXPANDBOTTOM    16231
#define IDC_EXPANDLEFT      16232
#define IDC_EXPANDRIGHT     16233
#define IDC_FILLINCOLOR		16234

/* Load mask dialog id's */
#define IDC_DELETEMASK      16240

/* Load path dialog id's */
#define IDC_SHAPE_SCALE		16250

/* Icons for Contrast Brightness dialog */
#define IDC_BRITICON  16501
#define IDC_BRITICON2 16502
#define IDC_BRITICON3 16503
#define IDC_BRITICON4 16504
#define IDC_BRITICON5 16505
#define IDC_BRITICON6 16506

#define IDC_CONTICON  16500
#define IDC_CONTICON2 16507
#define IDC_CONTICON3 16508
#define IDC_CONTICON4 16509
#define IDC_CONTICON5 16510
#define IDC_CONTICON6 16511

/* Task Manager dialog id's */
#define IDC_TASKLIST    16520
#define IDC_TASKSTOP    16521
#define IDC_TASKSUSPEND 16522
#define IDC_TASKRESUME  16523

/* Object Align Dialog ID's */
#define IDC_ALIGN_COMBO     16530
#define IDC_ALIGN_RESET     16531
#define IDC_ALIGN_IMAGE     16532
#define IDC_ALIGN_OBJECT    16533
#define IDC_ALIGN_MASK      16534
#define IDC_ALIGN_TRIANGLE  16535
#define IDC_ALIGN_CIRCLE    16536
#define IDC_ALIGN_SQUARE    16537
#define IDC_ALIGN_GRID  16538
#define IDC_ALIGN_LEFT  16539
#define IDC_ALIGN_RIGHT     16540
#define IDC_ALIGN_UP        16541
#define IDC_ALIGN_DOWN  16542
#define IDC_ALIGN_CENTER    16543
#define IDC_ALIGN_MID       16544    
#define IDC_ALIGN_HSPACE    16545    
#define IDC_ALIGN_VSPACE    16546    

/* Palette editor dialog id's */
#define IDC_PALETTEPICKER       16550
#define IDC_PALETTELOAD     16551
#define IDC_PALETTESAVE     16552
#define IDC_PALETTEREPLACE      16553
#define IDC_PALETTEREMAP        16554
#define IDC_PALETTECOLORVALUE	16555

/* Command List Undo dialog id's */
#define IDC_CMDLIST_UNDO        16560
#define IDC_NUMTOUNDO           16561
#define IDC_SAVEUNDOLIST        16562
#define IDC_CMDLIST_FULLNAMES   16563

/* Command List Redo dialog id's */
#define IDC_CMDLIST_REDO        16565
#define IDC_NUMTOREDO           16566
#define IDC_SAVEREDOLIST        16567

/* Command List Editor dialog id's */
#define IDC_CMDLIST_EDIT        16570
#define IDC_CMDLIST_DISABLE 16571
#define IDC_CMDLIST_DELETE      16572
#define IDC_CMDLIST_INSERT      16573
#define IDC_CMDLIST_SELECTED    16574
#define IDC_CMDLIST_POSITION    16575
#define IDC_CMDLIST_SAVE        16576
#define IDC_CMDLIST_DISABLETEXT 16577
#define IDC_CMDLIST_ENABLETEXT  16578

/* Object Position dialog id's */
#define IDC_POS_X               16590
#define IDC_POS_Y               16591

/* Macro Status dialog id's */
#define IDC_TASK_NAME		   16595
#define IDC_MACRO_PERCENT      16596
#define IDC_MACRO_PAUSE    	   16597
#define IDC_MACRO_RESUME       16598
#define IDC_MACRO_STOP         16599
#define IDC_TASK_PERCENT	   16600
#define IDC_MACRO_FILENAME		16601

/* Macro Editor dialog id's */
#define IDC_MACRO_LOAD			16605
#define IDC_MACRO_PLAY			16606

/* Batch Macro dialog id's */
#define IDC_BATCH_QUEUE			16610
#define IDC_BATCH_MACROLIST		16611
#define IDC_BATCH_DELETE		16612
#define IDC_BATCH_ADDFILES		16613
#define IDC_BATCH_ADDMACROS		16614
#define IDC_BATCH_PLAY			16615
#define IDC_BATCH_OPTIONS		16616

/* Batch Options dialog id's */
#define IDC_CLOSEWHENDONE		16630
#define IDC_BATCHSAVE			16631
#define IDC_NOSAVE				16632
#define IDC_SAVETOORIGINAL		16633
#define IDC_SAVETODIR			16634
#define IDC_SAVETOALBUM			16635
#define IDC_BATCHSAVEFIRST		IDC_NOSAVE
#define IDC_BATCHSAVELAST		IDC_SAVETOALBUM
#define IDC_ALBUMLIST			16636
#define IDC_DIRNAME				16637
#define IDC_BATCHCHANGETYPE		16638
#define IDC_BATCHFILETYPE		16639

/* Color Manager ID's */
#define IDC_CMS				16650
#define IDC_CMS_SRC			16651
#define IDC_CMS_DST			16652
#define IDC_CMS_ENABLED		16653
#define IDC_CMS_SRCLIST		16654
#define IDC_CMS_DSTLIST		16655
#define IDC_CMS_SRCINFO		16656
#define IDC_CMS_DSTINFO		16657
#define IDC_CMS_SRCNAME		16658
#define IDC_CMS_DSTNAME		16659
#define IDC_CMS_SAVE		16660
#define IDC_PREVIEW_DST		16661

// import dlg IDs
#define IDC_ANTIALIAS           17000
#define IDC_EDITRES             17001
#define IDC_IMAGE_TYPE          17002
#define IDC_IMAGE_FIRST         IDC_IMAGE_LINEART
#define IDC_IMAGE_LINEART       17003
#define IDC_IMAGE_GRAYSCALE16   17004
#define IDC_IMAGE_COLOR16       17005
#define IDC_IMAGE_GRAYSCALE256  17006
#define IDC_IMAGE_COLOR256      17007
#define IDC_IMAGE_24BIT         17008
#define IDC_IMAGE_LAST          IDC_IMAGE_24BIT


// Package PP5 dialog id's
#define IDC_PACKAGEPATH			17010

// Thumbnail checkbox in Common Dialog
#define IDC_THUMBCHECK        17100

/* ----------------------------------------------------------------------*/
/* OLE 2.0 ID Range 17500 - 17999 */
#define ID_OLE2_BASEID_MIN  17500 
#define ID_OLE2_BASEID_MAX  (ID_OLE2_BASEID_MIN  + 499)
/* ----------------------------------------------------------------------*/

/* ----------------------------------------------------------------------*/
/* CUSTOM TOOL ID Range 18000 - 18999 */
#define ID_CUSTOMTOOL_MIN  18000 
#define ID_CUSTOMTOOL_MAX  (ID_CUSTOMTOOL_MIN  + 999)
/* ----------------------------------------------------------------------*/

/* All IDM_??? menu items go from 19000-30000 */

/* ----------------------------------------------------------------------*/
// 	19000-19199 are menu (or accelerator) items which should be 
//	passed on the the ribbons for processing
//  DONT MOVE ANYTHING OUT OF HERE(19000 to 19199) vvvvvv
#define IDM_FIRST_RIBBONMENU	19000
#define IDC_MASKMODE_REVERSE    19001
#define IDC_MASKMODE_ADD        19002
#define IDC_MASKMODE_SUBTRACT   19003
//  DONT MOVE ANYTHING OUT OF HERE (19000 to 19199)^^^^^^^
#define IDC_FREEHAND        	19004
#define IDC_AUTOMASK        	19005
#define IDC_POINTMODE       	19006
#define IDC_MASKMETHODFIRST 	IDC_FREEHAND
#define IDC_MASKMETHODLAST  	IDC_POINTMODE
#define IDC_IS_BEZIER       	19007
#define IDC_IS_POINT        	19008
//  DONT MOVE ANYTHING OUT OF HERE (19000 to 19199)^^^^^^^
	// Mask point editor controls
#define IDC_POINTERFUNCTION 	19009
#define IDC_FIRST_POINTERFN 	19010
#define IDC_MOVEPOINTS      	19012
#define IDC_ADDPOINTS       	19013
#define IDC_DELPOINTS       	19014
#define IDC_CLIPPOINTS      	19015
#define IDC_LAST_POINTERFN  	19016
//  DONT MOVE ANYTHING OUT OF HERE (19000 to 19199)^^^^^^^
	// virtual pointer function
#define IDC_MOVEADD				19017
#define	IDC_DRAW_BEZIER			19018				
#define	IDC_DRAW_LINE			19019

#define IDM_LAST_RIBBONMENU		19199
//  DONT MOVE ANYTHING OUT OF HERE (19000 to 19199)^^^^^^^

/* ----------------------------------------------------------------------*/

/* The File menu items start at 20000 */
#define IDM_FILEMENU        0
#define IDM_FILE            20000
#define IDM_NEW             20001
#define IDM_OPEN            20002
#define IDM_SAVE            20003
#define IDM_SAVEAS          20004
#define IDM_REVERT          20005
#define IDM_PRINT           20006
#define IDM_SCAN            20007
#define IDM_GRAB            20008
#define IDM_CLOSE           20009
#define IDM_EXIT            20010
#define IDM_ALTOPEN         20011
#define IDM_ACQUIRE         20012
#define IDM_IMPORT          20013
#define IDM_PRINTPREVIEW    20014
#define IDM_SENDMAIL        20015

/*
// The File/Wallpaper popup menu items
#define IDM_WALLPOPUP       -1
#define IDM_WALLPAPER       20014
#define IDM_OPENWALLPAPER   20015
#define IDM_SAVEWALLPAPER   20016
*/

// The File/Setup popup menu items 
#define IDM_SETUP           20020
#define IDM_PRINTSETUP      20021
#define IDM_ACQUIRESETUP    20022
#define IDM_SCANSETUP       20023
#define IDM_MONITORMAP      20024
#define IDM_CALIBRATION     20025
#define IDM_CALIBRATESCAN   20026
#define IDM_CALIBRATEPRINT  20027

/* The File/Macro popup menu items */
#define IDM_MACRO           20029
#define IDM_MACROPLAY       20030
#define IDM_MACROPLAYBATCH  20031
#define IDM_MACRORECORD     20032
#define IDM_MACROSTOP       20033
#define IDM_MACRO_EDIT      20034

#define IDM_EXPORTFILE		20039

/* The File/Recall popup menu items (enough values for max recalled images) */
#define IDM_RECALLPOPUP     20
#define IDM_RECALLIMAGE     20040
#define IDM_RECALLIMAGE0    20041
#define IDM_RECALLIMAGE1    20042
#define IDM_RECALLIMAGE2    20043
#define IDM_RECALLIMAGE3    20044
#define IDM_RECALLIMAGE4    20045
#define IDM_RECALLIMAGE5    20046
#define IDM_RECALLIMAGE6    20047
#define IDM_RECALLIMAGE7    20048
#define IDM_RECALLIMAGE8    20049
#define IDM_RECALLIMAGE9    20050

// offset used by File/ Run, Import, Export when LE version created.
#define IDM_OEMOFFSET       4

/* The File/Run popup menu items (enough values for max runable apps) */
#define IDM_RUNPOPUP        -1
#define IDM_RUN             20060
#define IDM_RUN0            20061
#define IDM_RUN1            20062
#define IDM_RUN2            20063
#define IDM_RUN3            20064
#define IDM_RUN4            20065
#define IDM_RUN5            20066
#define IDM_RUN6            20067
#define IDM_RUN7            20068
#define IDM_RUN8            20069
#define IDM_RUN9            20070

/* The File/Export popup menu items */
#define IDM_EXPORTPOPUP     11
#define IDM_EXPORT          20079
#define IDM_EXPORT0         20080

/* The File/Import popup menu items */
#define IDM_IMPORTPOPUP     10
#define IDM_IMPORT0         20090

/* The Edit menu items start at 20100 */
#define IDM_EDITMENU        1
#define IDM_EDIT            20100
#define IDM_UNDO            20101
#define IDM_APPLY           20102
#define IDM_CUT             20103
#define IDM_COPY            20104
#define IDM_COPYTOFILE      20105
#define IDM_PASTE           20106
#define IDM_PASTEASNEW      20107
#define IDM_PASTEFROMFILE   20108
#define IDM_DELETE          20109
#define IDM_PREF            20110
#define IDM_PASTESPECIAL    20111

#define IDM_CMDLIST_POPUP   2
#define IDM_CMDLIST     	20120
#define IDM_CMDLIST_UNDO    20121
#define IDM_CMDLIST_REDO    20122
#define IDM_CMDLIST_EDIT    20123

/* The Mapping menu items start at 20200 */
#define IDM_MAPMENU         5
#define IDM_MAPPING         20200
#define IDM_MAP             20201
#define IDM_POSTERIZE       20202
#define IDM_HUE             20203
#define IDM_QUARTERTONE     20204
#define IDM_CLRMGRAPPLY     20205
#define IDM_PALETTEEDIT     20206
#define IDM_HUE_SHIFT       20207
#define IDM_HISTOGRAM    	20208
#define	IDM_THRESHOLD		20209

/* The Mapping/ContratsBrightness popup menu items start at 20210 */
#define IDM_CONTBRITPOPUP   1
#define IDM_CONTBRIT        20210
#define IDM_CONTBRIT_STICK  20211
#define IDM_CONTBRIT_VISUAL 20212

/* The Mapping/Balance popup menu items start at 20215 */
#define IDM_COLORBALPOPUP   2
#define IDM_COLORBAL        20215
#define IDM_COLORBAL_STICK  20216
#define IDM_COLORBAL_VISUAL 20217

/* The Mapping/Calibrate popup menu items start at 20220 */
#define IDM_CALIBRATEAPPLY  20220

/* The Mask menu items start at 20300*/
#define IDM_MASKMENU        4
#define IDM_MASK            20300
#define IDM_MASKUNDO        20301
#define IDM_MASKREMOVE      20302
#define IDM_MASKLOAD        20303
#define IDM_MASKSAVE        20304
#define IDM_CROP            20305
#define IDM_FEATHER         20306
#define IDM_MASKSTROKE      20307
#define IDM_MASKINVERT      20308
#define IDM_HIDEMARQUEE     20309
#define IDM_REMOVE_HOLES	 20310
#define IDM_CHROMA_MASK		 20311
#define IDM_MASK_SMOOTH	    20312
#define IDM_MASKFROMOBJECT  20313
#define IDM_OBJECTFROMMASK  20314


/* The Effects menu items start at 20400 */
#define IDM_IMAGEMENU       6
#define IDM_EFFECTS         20400
#define IDM_SIZE            20401
#define IDM_INVERT          20402
#define IDM_SPECIALFX       20403

/* Object Combine Menu		*/
#define IDM_COMBINEPOPUP	4
#define IDM_OBJCOMBINE		20404
#define IDM_OBJ_COMBOBJS	20405
#define IDM_COMBINE         20406
#define IDM_COMBINE_SEL 	20407


#define IDM_TEXTURIZE       20409
#define IDM_EXPAND          20410

/* The Effects/Rotate popup menu items start at 20420 */
#define IDM_ROTATEPOPUP     2
#define IDM_ROTATE          20420
#define IDM_ROTATE90        20421
#define IDM_ROTATE180       20422
#define IDM_ROTATE270       20423
#define IDM_ROTATEANY       20424

/* The Effects/Channels popup menu items start at 20430 */
#define IDM_CHANNELSPOPUP   4
#define IDM_CHANNELS        20430
#define IDM_SPLITRGB        20431
#define IDM_SPLITHSL        20432
#define IDM_SPLITCMYK       20433
#define IDM_CHANNELCOMBINE  20434

/* The Effects/Mirror popup menu items start at 20440 */
#define IDM_MIRRORPOPUP     3
#define IDM_MIRROR          20440
#define IDM_MIRRORX         20441
#define IDM_MIRRORY         20442
#define IDM_MIRRORXY        20443

/* The Effects/Convert popup menu items start at 20450 */
#define IDM_CONVERTPOPUP    5
#define IDM_CONVERT     20449
#define IDM_CONVLA          20450
#define IDM_CONVSP          20451
#define IDM_CONVCT          20452
#define IDM_CONVPALETTE     20453
#define IDM_CONVRGBCOLOR    20454
#define IDM_CONVCMYKCOLOR   20455
#define IDM_COLORMANAGED    20456

/* The Windows menu items start at 20500 */
#define IDM_WINDOWMENU      8
#define IDM_WINDOW          20500

#define IDM_CLOSEIT         20503
#define IDM_CLOSEALL        20504

/* Option Menu Hints  */
#define IDM_OPTIONS   20505
#define IDM_SAVEPOSITIONS   20506

/* The Window/Show/Hide popup menu items start at 20520 */
#define IDM_SHOWHIDEPOPUP   -1
#define IDM_SHOWHIDE        20520
#define IDM_SHOWINFO        20521
#define IDM_SHOWZOOMBOX     20522
#define IDM_SCRATCHPAD      20523
#define IDM_TOOLSHOW        20524
#define IDM_STATUSSHOW      20525
#define IDM_RULERSHOW       20526
#define IDM_TASKMANSHOW     20527

/*   Bubble Hint Id's   */

#define IDM_BUBBLEHINTS     20528    
#define IDM_BHINTSSHOW      20529    
#define IDM_SHINTSSHOW      20530
#define IDM_WHINTSSHOW      20531


/* The Object menu items start at 20560 */
#define IDM_OBJECTMENU          7
#define IDM_OBJECT              20560
#define IDM_OBJ_ALIGN           20561
#define IDM_OBJ_SELECTALL       20562
#define IDM_OBJ_FEATHER         20563
#define IDM_OBJ_DELETE          20564
#define IDM_OBJ_HIDEMARQUEE     20565
#define IDM_OBJ_MANAGER         20566
#define IDM_OBJ_APPLYMASK       20567
#define IDM_OBJ_SELECTINVERT    20568
#define IDM_OBJ_POSITION        20569

/*    Object Arrange Menu     */
#define	IDM_ARRANGEPOPUP		2
#define	IDM_ARRANGE				20570
#define IDM_OBJ_GROUP           20571
#define IDM_OBJ_UNGROUP         20572   
#define IDM_OBJ_LOCK            20573
#define IDM_OBJ_UNLOCK          20574   

/* The Object/Order popup menu items start at 20580 */
#define IDM_OBJ_ORDERPOPUP      3
#define IDM_ORDER               20580
#define IDM_OBJ_DOWNLEVEL       20581
#define IDM_OBJ_UPLEVEL         20582
#define IDM_OBJ_MOVETOBOTTOM    20583
#define IDM_OBJ_MOVETOTOP       20584

/* The MDI Window id's start at 20600 */
#define IDM_WINDOWCHILD     20600

/* Help menu items start at 20700 */
#define IDM_HELPMENU        9
#define IDM_HELP            20700
#define IDM_HELP_CONTEXT    20701
#define IDM_HELP_INDEX      20702
#define IDM_HELP_USING      20703
#define IDM_HELP_README     20704
#define IDM_HELP_HELPONHELP 20705
#define IDM_ABOUT           20706
#define IDM_TUTORIAL        20707
#define IDM_HELP_CONTEXTMODE    20708

/* MDI child window system menu id's start at 20900 */
#define IDM_CPODISPLAY      20900
#define IDM_FASTBITS        20901
#define IDM_AVI_PREVIOUS    20902
#define IDM_AVI_NEXT        20903

/* Accelerators start at 25000 */
#define IDM_FUNC0           25000
#define IDM_FUNC1           25001
#define IDM_FUNC2           25002
#define IDM_FUNC3           25003
#define IDM_FUNC4           25004
#define IDM_FUNC5           25005
#define IDM_FUNC6           25006
#define IDM_FUNC7           25007
#define IDM_FUNC8           25008
#define IDM_FUNC9           25009
#define IDM_FUNC10          25010
#define IDM_FUNC11          25011
#define IDM_FUNC12          25012
#define IDM_SCROLLUP        25013
#define IDM_SCROLLDOWN      25014
#define IDM_SCROLLLEFT      25015
#define IDM_SCROLLRIGHT     25016
#define IDM_PRESSUREUP      25017
#define IDM_PRESSUREDOWN    25018
#define IDM_SIZEUP          25019
#define IDM_SIZEDOWN        25020
#define IDM_SHAPEUP         25021
#define IDM_SHAPEDOWN       25022
#define IDM_MEASURE         25023
#define IDM_BACKUP          25024
#define IDM_ESCAPE          25025
#define IDM_DELETEITEM      25026
#define IDM_LASTTOOL        25027
#define IDM_BRITUP          IDM_SIZEUP
#define IDM_BRITDN          IDM_SIZEDOWN
#define IDM_CONTUP          IDM_SHAPEUP
#define IDM_CONTDN          IDM_SHAPEDOWN
#define IDM_MOVEUP          25028
#define IDM_MOVEDOWN        25029
#define IDM_MOVELEFT        25030
#define IDM_MOVERIGHT       25031

/* All IDS_??? menu items go from 30000-32767 */

/* String definitions start at 30000 */
#define IDS_APPNAME         30000
#define IDS_APPNAMEOLE      30001
#define IDS_PREVIEW         30002
#define IDS_UNTITLED        30003
#define IDS_JUNIOR          30004
#define IDS_RUNTASKLIST     30005
//#define IDS_EXPORTTASKLIST  30006
#define IDS_EALREADYRUNNING	30006  
#define IDS_MACRORECORD     30007
#define IDS_MACROPLAY       30008
#define IDS_OKTORENDERCLIPBOARD 30009
#define IDS_EDITS           30010
#define IDS_PORT_SEPARATOR  30011
#define IDS_PRINTERR        30012
#define IDS_PRINTMEM        30013
#define IDS_PRINTDISK       30014
#define IDS_PRINTUSER       30015
#define IDS_WELCOME         30016
#define IDS_AREYOUSURE      30017
#define IDS_OKTOOPEN        30018
#define IDS_OKTOSAVEPREF    30019
#define IDS_INSIDE          30020
#define IDS_OUTSIDE         30021
#define IDS_ECUSTOMBRUSH    30022
#define IDS_OKTOREMOVEMASK  30023
#define IDS_OBJECTSNEEDCOMBINE  30024
#define IDS_OBJECTSONLYONE  30025
#define IDS_EWRITEFILE      30026
#define IDS_EOPEN           30027
#define IDS_ECREATE         30028
#define IDS_EIMGVERSION     30029
#define IDS_EIMGCOMP        30030
#define IDS_EIMGPHOTO       30031
#define IDS_EIMGFILL        30032
#define IDS_EIMGORIENT      30033
#define IDS_EMAPASC         30034
#define IDS_EIMGBAD         30035
#define IDS_EBADMAP         30036
#define IDS_EBADSTYLE       30037
#define IDS_EBADCAL         30038
#define IDS_EBADMASK        30039
#define IDS_EBADCLRMGR      30040
#define IDS_NOTINMASKEDIT   30041

#define IDS_CRIPPLED        30042
#define IDS_EWILLOW         30043
#define IDS_NOIMAGE         30044
#define IDS_INMEMORY        30045
#define IDS_EVIDEOREAD      30046
#define IDS_COPYPROTECTED   30047
#define IDS_ECALASC         30048
#define IDS_ETUTORIAL       30049
#define IDS_OKTOMEASUREGRAY 30050
#define IDS_OKTOMEASURERGB  30051
#define IDS_OKTOMEASUREBLACK    30052
#define IDS_OKTOMEASURECMYK 30053

#define IDS_SWAPPINGUNDO   30054

#define IDS_ECLIPOPEN       30055
#define IDS_ECLIPWRITE      30056
#define IDS_OKTOAPPLY       30057
#define IDS_EMASKREAD       30058
#define IDS_INIFILE         30059
#define IDS_NAMESFILE       30060
#define IDS_HELPFILE        30061
#define IDS_ON              30062
#define IDS_OFF             30063
#define IDS_READY           30064
#define IDS_EBADCLIP        30065

#define IDS_NO_CMYK_IN_GALLERY 30066

#define IDS_ETOOWIDE        30067
#define IDS_COLOR           30068
#define IDS_COLOR2          30069
#define IDS_COLORCOUNT      30070
#define IDS_EAVIREAD        30071
#define IDS_ECOMPRESS       30072
#define IDS_FXOKTOEXIT      30073
#define IDS_HASHELP         30074
#define IDS_ACTIVECOLOR     30075
#define IDS_ALTERNATECOLOR  30076
#define IDS_NOGRB           30079
#define IDS_GRBABORT        30080
#define IDS_EBADDEVICE      30081
#define IDS_EBADDRIVER      30082
#define IDS_ESCANOPEN       30083
#define IDS_ESCANMEM        30084
#define IDS_ESCANWRITE      30085
#define IDS_ESCANINIT       30086
#define IDS_ESCANENV        30087
#define IDS_ESCANGETATT     30088
#define IDS_ESCANSETATT     30089
#define IDS_ESCANCLOSE      30090
#define IDS_ESCANSTART      30091
#define IDS_ESCANIMAGE      30092
#define IDS_ESCANCROP       30093
#define IDS_SCANCHECK       30094
#define IDS_RESETFAIL       30095
#define IDS_TSTRDYFAIL      30096
#define IDS_INQFAIL         30097
#define IDS_REWFAIL         30098
#define IDS_NOSEQ           30099
#define IDS_SPCFAIL         30100
#define IDS_SPCFAIL1        30101
#define IDS_MDSELFAIL       30102
#define IDS_WFMFAIL         30103
#define IDS_WRTFAIL         30104
#define IDS_FFFAIL          30105

#define IDS_FXNOINTERSECT  30106

#define IDS_EDISKSPACE      30107
#define IDS_ENOMASK         30108
#define IDS_ENOMASKAREA     30109

#define IDS_DELETEEXTFILE   30110
#define IDS_OVERWRITEEXTFILE    30111
#define IDS_NOUNDO          30112
#define IDS_EERASENOTHING   30113
#define IDS_EERASESIZE      30114
#define IDS_NEWBRUSH        30115
#define IDS_INVALIDPALETTE  30116
#define IDS_PALETTEEXISTS   30117
#define IDS_BADSCANMASK     30118
#define IDS_BADPRINTMASK    30119
#define IDS_NEEDSCANMASK    30120
#define IDS_NEEDPRINTMASK   30121
#define IDS_OVERWRITEIMAGE  30122
#define IDS_OKTOREVERT      30123
#define IDS_SCALEXY         30124
#define IDS_SCALE           30125
#define IDS_ROTATEXY        30126
#define IDS_ROTATE          30127
#define IDS_COPYTOCLIP      30128
#define IDS_COPYTOIMAGE     30129
#define IDS_COPYTOTEXTURE   30130
#define IDS_COPYTOBRUSH     30131
#define IDS_CHANNELS        30132
#define IDS_CHANNELSSIZE    30133
#define IDS_OKTOCLOSEALL    30134
//#define IDS_RETAIL          30135
#define IDS_BADMGXIMAGE     30136
#define IDS_CONFIRM_CONVERT 30137
#define IDS_PLEASEWAIT      30138
#define IDS_ESCANDRIVER     30139
#define IDS_EGRABDRIVER     30140
#define IDS_BADZOOM         30141
#define IDS_EDOSALLOC       30142
#define IDS_EDIRECTORYCHANGE    30143
#define IDS_EWRITEPROTECT   30144
#define IDS_STITCHDEPTH     30145
#define IDS_STITCHTRANSFORM 30146
#define IDS_STITCHANCHOR    30147
#define IDS_EBADINSTALL     30148
#define IDS_EBADTYPE        30149

#define IDS_BYTES           30150
#define IDS_KILOBYTES       30151
#define IDS_MEGABYTES       30152
#define IDS_GIGABYTES       30153

#define IDS_DELETEEXTFILE_MULTI	30154

#define IDS_ETEMPFILE       30155
#define IDS_EVOLUME         30156
#define IDS_EFILE_VOLUME    30157
#define IDS_WARN_READONLY   30158

#define IDS_RED             30159
#define IDS_GREEN           30160
#define IDS_BLUE            30161
#define IDS_CYAN            30162
#define IDS_MAGENTA         30163
#define IDS_YELLOW          30164
#define IDS_BLACK           30165
#define IDS_RECT_STATUS     30166
#define IDS_SCRATCHPAD      30167
#define IDS_WALLPAPER       30168
#define IDS_EWALLFILEFORMAT 30169
#define IDS_EIMAGEOPEN      30170
#define IDS_EPROCESSOPEN    30171
#define IDS_EFRAMEREAD      30172
#define IDS_APPLYTOHIRES    30173
#define IDS_CONVERTLOWRES   30174
#define IDS_EMASKINTERSECT  30175
#define IDS_ENOBRUSH        30176

// MMB - Added these new values for CMYK
#define IDS_EXTNAME_BADFILE 30177
#define IDS_EXTNAME_CREATE  30178
#define IDS_EXTNAME_COPY    30179
#define IDS_INI_NOWRITE     30180
#define IDS_INI_NOEXIST     30181

#define IDS_TOOMANYCOLORS   30182
#define IDS_COLORSWATCH     30183
#define IDS_NOTWITHLINEART	30184
#define IDS_CMDLISTFAILURE	30185

#define IDS_ESAMEFILENAME 	30186
#define IDS_ENOLOWRESDATA	30187
#define IDS_UNKNOWN			30188

#define IDS_HUE             30190
#define IDS_SAT             30191
#define IDS_LUM             30192
#define IDS_MACROCHANGED	30193
#define IDS_BATCHOPEN_TITLE		30194
#define IDS_BATCHOPEN_BUTTON	30195
#define IDS_SAVEHALFTONECHANGES	30196
#define IDS_SAVECALIBRATIONCHANGES	30197
#define IDS_PARTIALCHANGE	30198
#define IDS_IMPORTTASKLIST  30199

/* Menu strings */
#define IDS_RGBSPLIT            30200
#define IDS_COMBINE             30201
#define IDS_CPODISPLAY          30202
#define IDS_SAVE                30203
#define IDS_UPDATESAVE          30204
#define IDS_EXIT                30205
#define IDS_UPDATEEXIT          30206
#define IDS_UNDO                30207
#define IDS_REDO                30208
#define IDS_MANUALAPPLY         30209
#define IDS_AUTOAPPLY           30210
#define IDS_SHOWZOOMBOX         30211
#define IDS_HIDEZOOMBOX         30212
#define IDS_CREATE_SCRATCHPAD   30213
#define IDS_SHOW_TOOLS          30215
#define IDS_HIDE_TOOLS          30216
#define IDS_SHOW_STATUS         30217
#define IDS_HIDE_STATUS         30218
#define IDS_SHOW_RULERS         30219
#define IDS_HIDE_RULERS         30220
#define IDS_CMYKSPLIT           30221
#define IDS_NOTWITHMASK         30222
#define IDS_OBJECTDELETE        30223
#define IDS_SHOW_MARQUEE        30224
#define IDS_HIDE_MARQUEE        30225
#define IDS_GROUP               20226
#define IDS_UNGROUP             20227
#define IDS_SHOW_INFO           30228
#define IDS_HIDE_INFO           30229
#define IDS_SHOW_OBJ            30230
#define IDS_HIDE_OBJ            30231
#define IDS_FASTBITS            30232
#define IDS_NONUNDOABLE         30233
#define IDS_AVI_PREVIOUS        30234
#define IDS_AVI_NEXT            30235
#define IDS_SHOW_TASKMAN        30240
#define IDS_HIDE_TASKMAN        30241
#define IDS_DISABLE             30242
#define IDS_ENABLE              30243
#define IDS_SHOW_BHINT          30244
#define IDS_HIDE_BHINT          30245
#define IDS_SHOW_SHINT          30246
#define IDS_HIDE_SHINT          30247
#define IDS_SHOW_WHINT          30248
#define IDS_HIDE_WHINT          30249
#define IDS_SHOW_MASK		    30250
#define IDS_HIDE_MASK		    30251
#define IDS_NOTMEASUREMENTMAP	30252
#define IDS_LOOKFORFILE		    30253
#define IDS_LOOKFORPCDFILE 	    30254
#define IDS_SHOW_PALETTE        30255
#define IDS_HIDE_PALETTE        30256
#define IDS_PRINTCYAN 			30257
#define IDS_PRINTMAGENTA		30258
#define IDS_PRINTYELLOW			30259
#define IDS_PRINTBLACK			30260
#define IDS_PRINTCOLOR			30261
#define IDS_PRINTGRAY			30262		
#define IDS_CMDDISABLED			30263
#define IDS_ECOPYFILE			30264

/* Undo string definitions 30301 - 30500 */
#define IDS_UNDOFIRST       30301
#define IDS_UNDOTEXT        30301
#define IDS_UNDOFIRST       30301
#define IDS_UNDODELETE      30302
#define IDS_UNDOCUT         30303
#define IDS_UNDOEDITTEXT    30304

#define IDS_UNDOPAINT       30310
#define IDS_UNDOSPRAY       30311
#define IDS_UNDOCLONE       30312
#define IDS_UNDOTEXTURE     30313
#define IDS_UNDOSMEAR       30314
#define IDS_UNDOERASER      30315
#define IDS_FIRST_RETOUCHUNDO IDS_UNDOPAINT

#define IDS_UNDOSHARP       30320
#define IDS_UNDOSMOOTH      30321
#define IDS_UNDOLIGHTEN     30322
#define IDS_UNDODARKEN      30323
#define IDS_FIRST_FILTERUNDO IDS_UNDOSHARP

#define IDS_UNDOVIGNETTE    30330
#define IDS_UNDOTEXTUREFILL 30331
#define IDS_UNDOTINTFILL    30332
#define IDS_UNDOFLOOD       30333

#define IDS_UNDOPENCIL      30340
#define IDS_UNDODRAWSHAPE   30341
#define IDS_UNDODRAWFREE    30342

#define IDS_UNDOTRANSFORMER 30350

#define IDS_UNDOSIZE        30360
#define IDS_UNDOROTATE90    30361
#define IDS_UNDOROTATE180   30362
#define IDS_UNDOROTATE270   30363
#define IDS_UNDOROTATEANY   30364
#define IDS_UNDOMIRRORX     30365
#define IDS_UNDOMIRRORY     30366
#define IDS_UNDOMIRRORXY    30367
#define IDS_UNDOINVERT      30368
#define IDS_UNDOCOMBINE     30369
#define IDS_UNDOEXPAND      30370

#define IDS_UNDOMERGEMASK   30371
#define IDS_UNDOSPECIALFILTER   30372
#define IDS_UNDOPASTE       30373
#define IDS_UNDOFEATHER     30374
#define IDS_UNDOCROP        30375
#define IDS_UNDOREDUCTION   30376
#define IDS_UNDOCONVERT     30377
#define IDS_UNDOREMOVE_HOLES 30378
#define IDS_UNDOMASK_SMOOTH 30379

#define IDS_UNDOMAP        			30390
#define IDS_UNDOQUARTERTONE			30391
#define IDS_UNDOCONTBRIT   			30392
#define IDS_UNDOPOSTERIZE  			30393
#define IDS_UNDOHUE        			30394
#define IDS_UNDOCOLORBAL    		30395
#define IDS_UNDOCALIBRATEAPPLY  	30396
#define IDS_UNDOPALETTEEDIT 		30397
#define	IDS_UNDOHUESHIFT			30398
#define IDS_UNDOTHRESHOLD  			30399

#define IDS_UNDOSQUAREMASK  30400
#define IDS_UNDOCIRCLEMASK  30401
#define IDS_UNDOFREEMASK    30402
#define IDS_UNDOPAINTMASK   30403
#define IDS_UNDOMAGICMASK   30404
#define IDS_UNDOXFORMMASK   30405
#define IDS_UNDOMASKINVERT  30407
#define IDS_UNDOMASKREMOVE  30408
#define IDS_UNDOLOADMASK            30410
#define IDS_UNDOSTROKEMASK          30411
#define IDS_UNDOMAGICEXPAND         30412
#define IDS_UNDOMASKFROMOBJECT      30413
#define IDS_UNDOOBJECTFROMMASK      30414

#define IDS_UNDODELETEOBJECT 		30420
#define IDS_UNDODUPLICATE   		30421
#define IDS_UNDOFEATHEROBJECT		30422

#define IDS_UNDOINSERT      30430
#define IDS_UNDOAPPEND      30431
#define IDS_UNDOFILL        30432
#define IDS_UNDOCOLOR       30433
#define IDS_UNDOLABEL       30434
#define IDS_UNDOMOVE        30435
#define IDS_UNDOEFFECT      30436
#define IDS_UNDOOBJALIGN    30437
#define IDS_UNDOSELCOMBINE  30438
#define IDS_UNDOOBJPOS      30439
#define IDS_UNDOOBJLOCK     30440
#define IDS_UNDOOBJCOMBOBJS 30441
#define IDS_UNDOOBJUNLOCK   30442
#define IDS_UNDOGROUPOBJ    30443
#define IDS_UNDOUNGROUPOBJ  30444
#define IDS_UNDOPOINTEDIT  	30445
#define IDS_UNDOSIMILAR		30446
#define IDS_UNDO1	       	30447
#define IDS_UNDO2   	    30448
#define IDS_UNDOLOADIMAGE   	30449
#define IDS_UNDOSAVEIMAGE   	30450
#define IDS_UNDOLOWRESOPEN  	30451
#define IDS_UNDOSETUPDISPLAY    30452
#define IDS_UNDOCMSOPENING	    30453
#define IDS_UNDOCMSINFO	    	30454
#define IDS_UNDOIMPORTIMAGE 	30455
#define IDS_UNDOPRINTING		30456
#define IDS_UNDOPASTECLIP   	30457
#define IDS_UNDOCOPYTOFILE  	30458
#define IDS_UNDOCOPY        	30459
#define IDS_UNDOSTITCH 		    30460
#define IDS_UNDOPASTEFROMFILE 	30461
#define IDS_UNDOSPLITCHANNEL 	30462
#define IDS_UNDOCOMBINECHANNEL 	30463
#define IDS_UNDOEXPORTFILE		30464
#define IDS_UNDOSAVEMASK		30465
#define IDS_UNDOFASTBITSOPEN  	30466
#define IDS_UNDORENDER			30467
#define IDS_UNDOREBUILD			30468

/* Printer labels */
#define IDS_LABELPICNAME    30500
#define IDS_LABELSTYLE      30501
#define IDS_LABELPRINTER    30502
#define IDS_LABELNOINKCORRECT   30503
#define IDS_LABELRINKCORRECT    30504
#define IDS_LABELGINKCORRECT    30505
#define IDS_LABELBINKCORRECT    30506
#define IDS_LABELNOHALFTONING   30507
#define IDS_LABELCOLORFREQUENCY 30508
#define IDS_LABELCOLORANGLE     30509
#define IDS_LABELSINGLEHALFTONE 30510
#define IDS_LABELMINMAXDOT      30511
#define IDS_LABELNOBLACKGEN     30512
#define IDS_LABELBLACKGEN       30513
#define IDS_LABELUCR            30514
#define IDS_LABELNOCALIBRATION  30515
#define IDS_LABELCALIBRATION    30516
#define IDS_LABELSEPC       30517
#define IDS_LABELSEPM       30518
#define IDS_LABELSEPY       30519
#define IDS_LABELSEPK       30520

#define IDS_YES             30530
#define IDS_NO                  30531
#define IDS_INFOSIZE            30532
#define IDS_INFOIMAGEMEMORY 30533
#define IDS_INFOEDITPERCENT 30534
#define IDS_INFOUNDOPERCENT 30535
#define IDS_INFOIMAGEDISK       30536
#define IDS_INFOMEMORY          30537
#define IDS_INFOMASKEDITPERCENT 30538
#define IDS_INFOMASKUNDOPERCENT 30539

#define IDS_LOGONFAIL			30540 // mail logon failure

#define IDS_CMSNOTENABLED		30550
#define IDS_DIALOGINUSE     30551

/* Unit strings */
#define IDS_INCHES          30600
#define IDS_MM              30601
#define IDS_PICAS           30602
#define IDS_CM              30603
#define IDS_PIXELS          30604

/* More Message strings starting at 30700 */
#define IDS_DELETETHUMBNAILS    30700
#define IDS_DELETEREMOVABLE 30701
#define IDS_CANNOTREAD      30702
#define IDS_ECOPY           30703
#define IDS_ERENAME         30704
#define IDS_EDELETE         30705
#define IDS_DELETELISTEDTHUMBNAILS  30706
#define IDS_ECREATETHUMBNAIL    30707
#define IDS_EREADTHUMBNAIL  30708
#define IDS_EWRITETHUMBNAIL 30709
#define IDS_CREATEPCDTHUMBNAILS 30710
#define IDS_CONFIRMDELETE   30711
#define IDS_INVALIDFILESPEC 30712
#define IDS_NOSOURCEFILES   30713
#define IDS_ASKFORDISK      30714
#define IDS_STUBHEADER      30715
#define IDS_BRUSHNOTFOUND   30716
#define IDS_BRUSHINVALID    30717
#define IDS_BRUSHINCOMPATIBLE   30718
#define IDS_BRUSHNOUNDO     30719
#define IDS_BRUSHCLONETYPE  30720
#define IDS_COLORLABEL      30722
#define IDS_SUPPLEMENTAL    30723
#define IDS_ASKFORMANIFEST  30724
#define IDS_MAIN            30725
#define IDS_FILEOPEN_TITLE      30726
#define IDS_FILEOPEN_BUTTON     30727
#define IDS_FILESAVE_TITLE      30728
#define IDS_FILESAVE_BUTTON     30729
#define IDS_FILEIMPORT_TITLE    30730
#define IDS_FILEIMPORT_BUTTON   30731


// MMB - Added for CMYK Color
#define IDS_FILEADD_TITLE       30732
#define IDS_FILEADD_BUTTON      30733

#define IDS_NONE                30736

#define IDS_MODENORMAL          30740
#define IDS_MODEADDITIVE        30741
#define IDS_MODESUBTRACTIVE     30742
#define IDS_MODEIFLIGHTER       30743
#define IDS_MODEIFDARKER        30744
#define IDS_MODEHUESAT          30745
#define IDS_MODEHUE             30746
#define IDS_MODESAT             30747
#define IDS_MODELUM             30748
#define IDS_MODERED             30749
#define IDS_MODEGREEN           30750
#define IDS_MODEBLUE            30751
#define IDS_MODECYAN            30752
#define IDS_MODEMAGENTA         30753
#define IDS_MODEYELLOW          30754
#define IDS_MODEBLACK           30755
#define IDS_MODESCREEN          30756
#define IDS_MODEMULTIPLY        30757
#define IDS_MODEDIFFERENCE      30758
#define IDS_MODETEXTURIZE       30759
#define IDS_MODEINVERT			30760
#define IDS_MODEFIRST           IDS_MODENORMAL
#define IDS_MODELAST            IDS_MODEINVERT

#define IDS_NOTIMPLEMENTED      30770
#define IDS_GRAYVALUE           30771
#define IDS_RGBVALUE            30772
#define IDS_HSLVALUE            30773
#define IDS_CMYKVALUE           30774
#define IDS_PERCENT             30775
#define IDS_LORES_NI            30776
#define IDS_FASTBITS_NI         30777
#define IDS_CAPTIONQZOOM		30778

#define IDS_BRUSH_READONLY      30780
#define IDS_BRUSH_NODELETE      30781
#define IDS_FINDFILE_TITLE		30782
#define IDS_FINDFILE_BUTTON		30783
#define IDS_ENOAVI				30784
#define IDS_TONE_BADMAP			30785
#define IDS_ESAMPLES			30786

/* PostScript (tm) strings start at 30800 */
#define IDS_PS_STARTBLOCK   30800
#define IDS_PS_ENDBLOCK 30801
#define IDS_PS_POP      30802
#define IDS_PS_IFELSE       30803
#define IDS_PS_DEF      30804
#define IDS_PS_END      30805
#define IDS_PS_DICTDEF      30806
#define IDS_PS_HEXDATA      30807
#define IDS_PS_BINDATA      30808
#define IDS_PS_ELLDOT1      30809
#define IDS_PS_ELLDOT2      30810
#define IDS_PS_SQUDOT       30811
#define IDS_PS_CIRDOT       30812
#define IDS_PS_TRIDOT       30813
#define IDS_PS_PROPDOT      30814
#define IDS_PS_SETSCREEN    30815
#define IDS_PS_COLOREXT 30816
#define IDS_PS_SETSPOT      30817
#define IDS_PS_SETCOLORSCREEN   30818
#define IDS_PS_BLACKSEPS    30819
#define IDS_PS_COLORSEPS    30820
#define IDS_PS_NOINVERT 30821
#define IDS_PS_CHECKINVERT  30822
#define IDS_PS_TRANSLATE    30823
#define IDS_PS_SCALE        30824
#define IDS_PS_LINE1        30825
#define IDS_PS_LINE2        30826
#define IDS_PS_LINE3        30827
#define IDS_PS_LINE4        30828
#define IDS_PS_LINE5        30829
#define IDS_PS_DOCOLORSEPDEF    30830
#define IDS_PS_NOCOLORSEPDEF    30831
#define IDS_PS_DOIMAGEDEF   30832
#define IDS_PS_DOIMAGE      30833
#define IDS_PS_COLORSEPVAL  30834
#define IDS_PS_SEPCYAN      30835
#define IDS_PS_SEPMAGENTA   30836
#define IDS_PS_SEPYELLOW    30837
#define IDS_PS_SEPBLACK 30838
#define IDS_PS_COLORIMAGE3  30839
#define IDS_PS_COLORIMAGE4  30840
#define IDS_PS_GETLINE1 30841
#define IDS_PS_GETLINE2 30842
#define IDS_PS_GETLINE3 30843
#define IDS_PS_GETLINE4 30844
#define IDS_PS_GETLINE5 30845
#define IDS_PS_DUMPLINE1    30846
#define IDS_PS_DUMPLINE2    30847
#define IDS_PS_DUMPLINE3    30848
#define IDS_PS_DUMPLINE4    30849
#define IDS_PS_DUMPLINE5    30850
#define IDS_PS_IMAGE        30851
#define IDS_PS_NOIMAGE      30852
#define IDS_PS_TRANSFORM    30853
#define IDS_PS_BEGINBINARY  30854
#define IDS_PS_ENDBINARY    30855
#define IDS_PS_MYSAVERESTORE    30856
#define IDS_PS_CRLF     30857

#define IDS_EPS_PSADOBE 30860
#define IDS_EPS_BOUNDINGBOX 30861
#define IDS_EPS_CREATOR 30862
#define IDS_EPS_TITLE       30863
#define IDS_EPS_CREATIONDATE    30864
#define IDS_EPS_SUPPRESSDOT 30865
#define IDS_EPS_PROCESSCOLORS   30866
#define IDS_EPS_CYANPLATE   30867
#define IDS_EPS_MAGENTAPLATE    30868
#define IDS_EPS_YELLOWPLATE 30869
#define IDS_EPS_BLACKPLATE  30870
#define IDS_EPS_ENDCOMMENTS 30871
#define IDS_EPS_DICTDEF 30872
#define IDS_EPS_DOIMAGE 30873
#define IDS_EPS_BIGDEF1 30874
#define IDS_EPS_BIGDEF2 30875
#define IDS_EPS_BIGDEF3 30876
#define IDS_EPS_BIGDEF4 30877
#define IDS_EPS_BIGDEF5 30878
#define IDS_EPS_PLANE0      30879
#define IDS_EPS_PLANE1      30880
#define IDS_EPS_PLANE2      30881
#define IDS_EPS_PLANE3      30882
#define IDS_EPS_PLANE4      30883
#define IDS_EPS_PLANE5      30884
#define IDS_EPS_PLANE6      30885
#define IDS_EPS_PLANE7      30886
#define IDS_EPS_PLANE8      30887
#define IDS_EPS_PLANE9      30888
#define IDS_EPS_GRESTORE    30889
#define IDS_EPS_TRANSFORM   30890

#define IDS_NOCLRMGR            30891
#define IDS_CLRMGROPENFAILED    30892
#define IDS_CLRMGRGETINFOFAILED 30893


// ai header strings 30900 to 30920
#define IDS_AI_HEADER_BEGIN		30900
#define IDS_AI_HEADER			30901
#define IDS_AI_HEADER_END		30902
#define IDS_AI_SETUP			30903
#define	IDS_AI_GROUP_BEGIN		30904
#define	IDS_AI_GROUP_END		30905
#define	IDS_AI_END				30906
#define IDS_EPS_NEWPATH			30907
#define IDS_EPS_MOVETO			30908
#define IDS_EPS_LINETO			30909
#define IDS_EPS_CLIP			30910
#define IDS_EPS_SCALE			30911
#define IDS_EPS_CURVETO			30912

/* 31000 - 31099 - reserved for tool hints in ribbon */
#define IDC_RIBBONSTART     31000

/* 31100 - 31200 - reserved for tool hints over image */
#define IDC_IMAGESTART      31100

/* 31200 - 31300 - reserved for tool names over image */
#define IDC_TOOLNAMESTART   31200

/* 31300 - 31400 - reserved for TWAIN messages */
#define TWAIN_START         31300
#define IDS_DSMNAME         (TWAIN_START + 0)
#define IDS_WINDOWTITLE     (TWAIN_START + 1)

// Resource based standard App error messages
#define IDS_NODSM           (TWAIN_START + 2)
#define IDS_NODS            (TWAIN_START + 3)
#define IDS_LOWMEMORY       (TWAIN_START + 4)
#define IDS_MAXCONNECTIONS  (TWAIN_START + 5)
#define IDS_BUMMER          (TWAIN_START + 6)
#define IDS_BADCAP          (TWAIN_START + 7)
#define IDS_BADPROTOCOL     (TWAIN_START + 8)
#define IDS_BADVALUE        (TWAIN_START + 9)
#define IDS_OPERATIONERROR  (TWAIN_START + 10)
#define IDS_SEQERROR        (TWAIN_START + 11)
#define IDS_SUCCESS         (TWAIN_START + 12)
#define IDS_NODETAIL        (TWAIN_START + 13)
#define IDS_DEFAULT         (TWAIN_START + 14)
#define IDS_MEMXFERFAILED   (TWAIN_START + 15)
#define IDS_INVALIDPIXTYPE  (TWAIN_START + 16)
#define IDS_UNSUPPORTEDPIXTYPE  (TWAIN_START + 17)
#define IDS_TWAINERROR      (TWAIN_START + 18)
#define IDS_TWAINFAILURE    (TWAIN_START + 19)
#define IDS_CAPGETFAILED    (TWAIN_START + 20)
#define IDS_CAPSETFAILED    (TWAIN_START + 21)

#define PM_XFERDONE         (WM_USER + 0)

/* 31400 - 31500 - reserved for brush style strings */
/* Strings for Brush Styles */
#define IDS_BS_DEFAULT          31401
#define IDS_BS_SOURCECOLOR      31402
#define IDS_BS_ACTIVECOLOR      31403
#define IDS_BS_ALTERNATECOLOR   31404
#define IDS_BS_DESTDATA         31405
#define IDS_BS_CURRENTIMAGE     31406
#define IDS_BS_CURRENTUNDO      31407
#define IDS_BS_SOURCEDATA       31408
#define IDS_BS_CURRENTDEST      31409
#define IDS_BS_FLIPMODE         31410
#define IDS_BS_FLIPNONE         31411
#define IDS_BS_MIRRORHORZ       31412
#define IDS_BS_MIRRORVERT       31413
#define IDS_BS_MIRRORBOTH       31414
#define IDS_BS_TILE             31415
#define IDS_BS_TILEHORZ         31416
#define IDS_BS_TILEVERT         31417
#define IDS_BS_TILEBOTH         31418
#define IDS_BS_SOURCEFUNC       31419
#define IDS_BS_SOLIDCOLOR       31420
#define IDS_BS_COPY             31421
#define IDS_BS_SHARPEN          31422
#define IDS_BS_SMOOTH           31423
#define IDS_BS_LIGHTEN          31424
#define IDS_BS_DARKEN           31425
#define IDS_BS_SMEAR            31426
#define IDS_BS_STROKEMETHOD     31427
#define IDS_BS_UNCONNECTED      31428
#define IDS_BS_STYLE            31429
#define IDS_BS_SCATTERED        31430
#define IDS_BS_SIZE             31431
#define IDS_BS_SHAPE            31432
#define IDS_BS_CIRCLE           31433
#define IDS_BS_SQUARE           31434
#define IDS_BS_VERTICAL         31435
#define IDS_BS_HORIZONTAL       31436
#define IDS_BS_SLASH            31437
#define IDS_BS_BACKSLASH        31438
#define IDS_BS_PRESSURE         31439
#define IDS_BS_WEIGHT           31440
#define IDS_BS_EDGETYPE         31441
#define IDS_BS_AIRBRUSH         31442
#define IDS_BS_FEATHER          31443
#define IDS_BS_FADE             31444
#define IDS_BS_TRANSPARENCY     31445
#define IDS_BS_OPACITYSCALE     31446
#define IDS_BS_TILT             31447
#define IDS_BS_TIMER            31448
#define IDS_BS_MAXFLOWRATE      31449
#define IDS_BS_SENSITIVITY      31450
#define IDS_BS_MAXFLOWCHANGE    31451
#define IDS_BS_SURFACE          31452
#define IDS_BS_SURFACEFLIP      31453
#define IDS_BS_MERGEMODE        31454
#define IDS_BS_ADDITIVE         31455
#define IDS_BS_SUBTRACTIVE      31456
#define IDS_BS_IFLIGHTER        31457
#define IDS_BS_IFDARKER         31458
#define IDS_BS_HUESAT           31459
#define IDS_BS_HUE              31460
#define IDS_BS_SAT              31461
#define IDS_BS_LUM              31462
#define IDS_BS_RED              31463
#define IDS_BS_GREEN            31464
#define IDS_BS_BLUE             31465
#define IDS_BS_CYAN             31466
#define IDS_BS_MAGENTA          31467
#define IDS_BS_YELLOW           31468
#define IDS_BS_BLACK            31469
#define IDS_BS_SCREEN           31470
#define IDS_BS_MULTIPLY     31471
#define IDS_BS_DIFFERENCE       31472
#define IDS_BS_TEXTURIZE        31473
#define IDS_BS_STYLEPRESSURE    31474
#define IDS_BS_CONNECTED        31475
#define IDS_BS_SOLID            31476
#define IDS_BS_PAINT            31477
#define IDS_BS_SECTION          31478
#define IDS_BS_SPACING          31479
#define IDS_BS_SMOOTHNESS       31480
#define IDS_BS_OVERLAP          31481
#define IDS_BS_ON               31482
#define IDS_BS_PROTECT          31483
#define IDS_BS_READONLY     31484
#define IDS_BS_NODELETE     31485

// plugin error message
#define IDS_UNSUPPORTEDPLUGINTYPE   31600
#define IDS_NOTABLET                31601

// ImageMark import error strings
#define IDS_INVALIDIMPORTFILE_ERROR     31700
#define IDS_IMPORTLIBLOAD_ERROR         31701
#define IDS_EXPORTLIBLOAD_ERROR         31702
#define IDS_IMPORTFILE_ERROR            31703
#define IDS_INVALIDIMPORTFILE_TYPE      31704
#define IDS_INVALIDIMPORTFILE_VERSION   31705

#define IDS_CMDLISTABORT		31706
#define IDS_FONTNOTFOUND		31707
#define IDS_BADLOWRES			31708
#define IDS_BADCOLORMAPSIZE		31709
#define IDS_BADINTERLEAVE		31710
#define IDS_NOCLONESOURCE		31711
#define IDS_BADBATCHMACRO		31712
#define IDS_NOIMAGETOWORKON		31713
#define IDS_COMMANDERROR		31714
#define IDS_INVALIDOPCODE		31715
#define IDS_BADSIGNATURE		31716
#define IDS_NODATA				31717
#define IDS_PCDERROR			31718
#define IDS_COMPRESSFASTBITS	31719
#define IDS_CMDOUTOFRANGE		31720
#define IDS_BADCMDTYPE			31721
#define IDS_ERESMISSING         31722
#define IDS_NOTPALETTECOLOR		31723
#define IDS_SAVEPCDASPP5		31724
#define IDS_CLIPBOARDEMPTY		31725
#define IDS_ENOTPALETTEEFFECT	31726

// pcx error messages
#define IDS_BADVERSION          31727
#define IDS_BADENCODING         31728
#define IDS_BADBITSPERPIXEL     31729
#define IDS_BADRESERVEDBYTE     31730
#define IDS_BADBYTESPERLINE     31731
#define IDS_BADNUMCOLORIMAGES   31732
#define IDS_BADMANUFACTURER     31733
#define IDS_BUSYTHREAD          31734
#define IDS_BADTHREADSETUP      31735
#define IDS_WAITRESULT          31736
#define IDS_PSONLY				31737
#define IDS_SCREENWARNING		31738
#define IDS_MUSTSAVEMASK		31739
#define IDS_CANTSAVELINK		31740
#define IDS_ETMPCREATE          31741
#define IDS_DESTROYCMDLIST      31742


#define IDS_ENOTLINEARTEFFECT	31790
#define IDS_EEFFECTDEPTHCHANGE	31791
#define IDS_EEFFECTDEPTHCHANGE2	31792
/* 32001 - 32200 - reserved for progress messages */
// THESE MIRROR IDS_UNDO's
#define IDS_PROGFIRST           32001
/* 32001 - 32200 - reserved for progress messages */

// THESE MIRROR IDS_UNDO's
///* 32201 - 32400 - reserved for task status messages */
#define IDS_TASKFIRST           32201
//* 32201 - 32400 - reserved for task status messages */

#define IDS_EBADPT			 	32401
#define IDS_CMS_NOSRCDEST		32402
#define IDS_CMS_NODESTNOMON	    32403
#define IDS_CMS_EFILERGBPTCMYK  32404
#define IDS_CMS_EFILECMYKPTRGB  32405
#define IDS_CMS_EFILEPALETTECLR 32406
#define IDS_CMS_EBADCOMBO	 	32407
#define IDS_EBADFILETYPE	 	32408
#define IDS_EBADCMSPTJPG	 	32409
#define IDS_EGETTINGPT		 	32410
#define IDS_FILEPTMISMATCH	 	32411
#define IDS_CMS_OPENING	 	    32412
#define IDS_CMS_CONNECTING	 	32413
#define IDS_CMS_ENOPT  	 	    32414
#define IDS_CMS_EPTNOMON  	 	32415
#define IDS_CMS_EFILEPTMM 	 	32416
#define IDS_CMS_BUILDING	 	32417

/* 32500 - 32600 - reserved for commonid.h */
#define IDS_EMAXIMAGEWIDTH  32500
#define IDS_EMAXIMAGEHEIGHT 32501
#define IDS_EIMAGEOVERHEAD  32502
#define IDS_EMEMALLOC       32503
#define IDS_ELINEPOOLALLOC  32504
#define IDS_EDISKCACHE      32505
#define IDS_ESCROPEN        32506
#define IDS_ESCRWRITE       32507
#define IDS_EWRITE          32508
#define IDS_EREAD           32509
#define IDS_EMALLOC         32510
#define IDS_ESCRREAD        32511
#define IDS_ETOOCOMPLEX     32512
#define IDS_ELOWRESOURCES   32513   
#define	IDS_EWRONGDATAOPEN	32514
#define IDS_ENO_OEMSUPPORT  32515
#define IDS_TRIALDAYSLEFT   32516

/* 32520 - 32550 - reserved for property sheet caption strings */
#define IDS_MONITORSETUP	32520
#define IDS_EDITPRINTSTYLE	32521
#define IDS_PREFERENCES		32522
#define IDS_SUMMARYTABDLG   32523

// Strings for Common Dialog 32551 - 32570
#define	IDS_CDG_OPENNORMAL	32551
#define	IDS_CDG_OPENFASTBITS	32552
#define	IDS_CDG_OPENLOWRES	32553

#define	IDS_CDG_OPENFIRST		IDS_CDG_OPENNORMAL
#define	IDS_CDG_OPENLAST		IDS_CDG_OPENLOWRES

#define IDS_CMD_FIRST       IDS_CMD_ROTATE90
#define IDS_CMD_ROTATE90    32600
#define IDS_CMD_ROTATE180   32601
#define IDS_CMD_ROTATE270   32602
#define IDS_CMD_ROTATEANY   32603
#define IDS_CMD_LOADFILE    32604
#define IDS_CMD_SIZE        32605
#define IDS_CMD_INVERT      32606
#define IDS_CMD_EXPAND      32607
#define IDS_CMD_MIRRORX     32608
#define IDS_CMD_MIRRORY     32609
#define IDS_CMD_MIRRORXY    32610
#define IDS_CMD_CONVERT     32611
#define IDS_CMD_GRADIENT    32612
#define IDS_CMD_TINTFILL    32613
#define IDS_CMD_DELETE      32614
#define IDS_CMD_CUT         32615
#define IDS_CMD_TEXTUREFILL 32616
#define IDS_CMD_FLOOD       32617
#define IDS_CMD_CONTBRIT    32618
#define IDS_CMD_BALANCE     32619
#define IDS_CMD_MAP         32620
#define IDS_CMD_LOWRESLOAD      32621
#define IDS_CMD_SAVEFILE        32622
#define IDS_CMD_SAVEANDCLOSE    32623
#define IDS_CMD_TONEBALANCE     32624
#define IDS_CMD_POSTERIZE	    32625
#define IDS_CMD_HUESAT          32626
#define IDS_CMD_APPLYCAL        32627
#define IDS_CMD_SHAPEMASK       32628
#define IDS_CMD_FREEMASK        32629
#define IDS_CMD_MAGICMASK       32630
#define IDS_CMD_TRANSFORMMASK    32631
#define IDS_CMD_TRANSFORMOBJECTS   32632
#define IDS_CMD_SELECTOBJECTS 32633
#define IDS_CMD_MOVEOBJECTS   32634
#define IDS_CMD_SETOPACITY   32635
#define IDS_CMD_ORDERUP        32636
#define IDS_CMD_GROUPOBJECTS   32637
#define IDS_CMD_DUPLICATEOBJECTS   32638
#define IDS_CMD_MERGEMASK      32639
#define IDS_CMD_PASTEFROM      32640
#define IDS_CMD_PASTE      32641
#define IDS_CMD_LOADMASK       32642
#define IDS_CMD_EFFECT          32643
#define IDS_CMD_REMOVEMASK      32644
#define IDS_CMD_INVERTMASK      32645
#define IDS_CMD_FEATHERMASK     32646
#define IDS_CMD_FEATHEROBJECT   32647
#define IDS_CMD_TEXT            32648
#define IDS_CMD_ALPHAEDIT       32649
#define IDS_CMD_COMBINE     32650
#define IDS_CMD_ACTIVATEWINDOW  32651
#define IDS_CMD_NEWIMAGE        32652
#define IDS_CMD_EDITUNDO        32653
#define IDS_CMD_MASKUNDO        32654
#define IDS_CMD_EDITAPPLY       32655
#define IDS_CMD_COPYTOFILE      32656
#define IDS_CMD_TRANSFORMIMAGE 32657
#define IDS_CMD_COPYTOIMAGE 32658
#define IDS_CMD_PAINT           32659
#define IDS_CMD_SPRAY           32660
#define IDS_CMD_CLONE           32661
#define IDS_CMD_TEXTURE     32662
#define IDS_CMD_SMEAR           32663
#define IDS_CMD_ERASER          32664
#define IDS_CMD_SHARP           32665
#define IDS_CMD_SMOOTH          32666
#define IDS_CMD_LIGHTEN     32667
#define IDS_CMD_DARKEN          32668
#define IDS_CMD_PAINTON          32669
#define IDS_CMD_UNDOERASE        32670
#define IDS_CMD_DRAWSHAPE        32671
#define IDS_CMD_DRAWFREE         32672
#define IDS_CMD_PENCIL           32673
#define IDS_CMD_PALETTEEDIT      32674
#define IDS_CMD_OBJECTALIGN 	   32675
#define IDS_CMD_SETMERGEMODE     32676
#define IDS_CMD_ORDERDOWN        32677
#define IDS_CMD_ORDERTOTOP     	32678
#define IDS_CMD_ORDERTOBOTTOM    32679
#define IDS_CMD_COMBINESEL       32680
#define IDS_CMD_OBJECTPOS        32681
#define IDS_CMD_OBJLOCK     	   32682
#define IDS_CMD_OBJCOMBOBJS 	   32683
#define IDS_CMD_PSFILTER		   32684
#define IDS_CMD_CROP			      32685
#define IDS_CMD_CROPMASK		   32686
#define IDS_CMD_CLOSE			   32687
#define IDS_CMD_SAVETOALBUM		32688
#define IDS_CMD_REMOVE_HOLES	   32689
#define IDS_CMD_CREATECLIPOBJECT 32690
#define IDS_CMD_CHROMA           32691
#define IDS_CMD_MASK_SMOOTH		32692
#define IDS_CMD_MASKFROMOBJECT   32693
#define IDS_CMD_OBJECTFROMMASK   32694
#define IDS_CMD_OBJUNLOCK     	32695
#define IDS_CMD_UNGROUPOBJECTS   32696
#define IDS_CMD_POINTEDIT 		   32697
#define IDS_CMD_SIMILAR			   32698
#define IDS_CMD_FASTBITS         32699
#define IDS_CMD_DELETEOBJECTS	   32700
#define IDS_CMD_STROKEMASK		   32701
#define IDS_CMD_SELECTOBJRELATIVE 32702
#define IDS_CMD_OBJECTLAYERING   32703
#define IDS_CMD_COPYTOCLIPBOARD	32704
#define IDS_CMD_SAVEMASK		32705
#define IDS_CMD_THRESHOLD		32706
#define IDS_CMD_HUESHIFT         32707
#define IDS_CMD_EDITTEXT         32708
#define IDS_CMD_DISCARDUNDO		32709
#define IDS_CMD_LAST            IDS_CMD_DISCARDUNDO

/*--------------------------------*/
/* OLE 2.0 ID Range 17000 - 17500 */
/* See Base Identifier defines at */
/* top of id.h.                   */
/*--------------------------------*/
#define IDS_E_OLEINITFAILED      17000
#define IDR_PPSVRTYPE_PP         17001   // PP Doc Type
#define IDR_PPSVRTYPE_SRVR_EMB   17002   // Embedded Resource OLE to use when activated NON-Insitu
#define IDR_PPSVRTYPE_SRVR_IP    17003   // OLE In-place activation resources
#define IDD_INPLACE_BLANKTOOLBOX 17050 	 // Ribbon PLaceholder for Insitu
#define IDD_INPLACE_BLANKTOOLBOXFLOAT 17051	// Floating Ribbon PLaceholder for Insitu
#define IDD_INSERTOLE			 17055   // Insert Ole Object Dialog Box
#define IDC_INSERTEXIST			 17056	 // Insert Ole Object Dialog controls
#define IDC_INSERTNEW			 17057	 // Insert Ole Object Dialog controls
#define IDC_INSERTIMPORT		 17058	 // Insert Ole Object Dialog controls
#define IDS_E_CLOSEDOCWITHMODALUP 17059 // Error Closing an OLEEdit Session with a Modal dlg up
#define IDS_MGXIMAGE_FORMAT      17060   
#define IDS_MGXIMAGE5_FORMAT     17061   
#define IDC_SWATCHICON			17100

/* CUSTOM TOOLBAR ID Range 18000 - 18999 */
#define IDD_CUSTOM_TOOLBOX     18000
#define IDD_EDITCUSTOOL        18001
#define IDM_CUSTOMTOOLBOX      ID_CUSTOMTOOL_MIN+2
#define IDM_CUSTBOXLOAD        ID_CUSTOMTOOL_MIN+3
#define IDM_CUSTBOXSAVE        ID_CUSTOMTOOL_MIN+4
#define IDM_CUSTBOXEDIT        ID_CUSTOMTOOL_MIN+5
#define IDD_TOOLBOX_LOAD       18006
#define IDD_RENAMECUSTOOL      18007


#define IDC_TOOLLIST           ID_CUSTOMTOOL_MIN+100
#define IDC_TOOLBOX            ID_CUSTOMTOOL_MIN+101
#define IDC_ADD                ID_CUSTOMTOOL_MIN+102
#define IDC_DEL                ID_CUSTOMTOOL_MIN+103
#define IDC_CATEGORY           ID_CUSTOMTOOL_MIN+104
#define IDC_TOOLNAME           ID_CUSTOMTOOL_MIN+105
#define IDC_TOOLBOXWIDTH       ID_CUSTOMTOOL_MIN+106
#define IDC_INSERT             ID_CUSTOMTOOL_MIN+107
#define IDC_COLORPATCH			 ID_CUSTOMTOOL_MIN+108
#define IDC_ICONPREVIEW			 ID_CUSTOMTOOL_MIN+109
#define IDC_TOOLBOXTOOLS	    ID_CUSTOMTOOL_MIN+110
#define IDC_ICONPREVIEWTEXT    ID_CUSTOMTOOL_MIN+111

#define IDS_CUSTCATLIST        ID_CUSTOMTOOL_MIN+200
#define IDS_CUSTFILECMD        ID_CUSTOMTOOL_MIN+300
#define IDS_CUSTEDITCMD        ID_CUSTOMTOOL_MIN+350
#define IDS_CUSTMASKCMD        ID_CUSTOMTOOL_MIN+380
#define IDS_CUSTMAPCMD         ID_CUSTOMTOOL_MIN+410
#define IDS_CUSTOBJCMD         ID_CUSTOMTOOL_MIN+440
#define IDS_CUSTIMAGECMD       ID_CUSTOMTOOL_MIN+480
#define IDS_CUSTOPTIONCMD      ID_CUSTOMTOOL_MIN+530
#define IDS_CUSTWNDCMD         ID_CUSTOMTOOL_MIN+560
#define IDS_CUSTHELPCMD        ID_CUSTOMTOOL_MIN+580
#define IDS_CUSTTOOLCMD        ID_CUSTOMTOOL_MIN+600

#define IDS_EDITCUSTTOOL       ID_CUSTOMTOOL_MIN+700
#define IDS_LOADCUSTTOOL       ID_CUSTOMTOOL_MIN+701

#define IDM_LOADSHAPE          ID_CUSTOMTOOL_MIN+750
#define IDM_SAVESHAPE          ID_CUSTOMTOOL_MIN+751
// IDM_EDITSHAPE is not used
#define IDM_EDITSHAPE          ID_CUSTOMTOOL_MIN+752
#define IDM_MAKELINE           ID_CUSTOMTOOL_MIN+753
#define IDM_MAKECURVE          ID_CUSTOMTOOL_MIN+754
#define IDM_ERASER             ID_CUSTOMTOOL_MIN+755
#define IDM_PROBE              ID_CUSTOMTOOL_MIN+756
#define IDM_CUSTOMVIEW         ID_CUSTOMTOOL_MIN+757
#define IDM_CUSTBOXCREATE      ID_CUSTOMTOOL_MIN+758

#define IDC_CHROMA             ID_CUSTOMTOOL_MIN+760
#define IDC_CHROMA_NORMAL      ID_CUSTOMTOOL_MIN+761
#define IDC_CHROMA_HSL         ID_CUSTOMTOOL_MIN+762
#define IDC_CHROMA_LUMIN       ID_CUSTOMTOOL_MIN+763
#define IDS_CHROMA_APPLY       ID_CUSTOMTOOL_MIN+764
#define IDS_CUSTNAMEDUP        ID_CUSTOMTOOL_MIN+765
#define IDC_SWITCHACTIVE_COLOR ID_CUSTOMTOOL_MIN+766
#define IDC_REVERT_COLORSWATCH ID_CUSTOMTOOL_MIN+767
#define IDS_RENAMECUST         ID_CUSTOMTOOL_MIN+768
#define IDS_OKREEDITTEXT       ID_CUSTOMTOOL_MIN+769
#define IDS_CUSTNAMEINVALID    ID_CUSTOMTOOL_MIN+770

#define IDR_MAINFRAME                   26000
#define IDD_TOOLBARS                    26001
#define IDD_OCCustom                    26002
#define IDR_OCTOOLMENU                  26003
#define IDC_COMBO1                      26004
#define IDC_TBARLIST                    26005
#define IDC_COLORBUTTON                 26006
#define IDC_LARGEBUTTON                 26007
#define IDC_TBARNEW                     26008
#define IDC_TBARCUST                    26009
#define IDC_TBARHELP                    26010
#define IDC_OCCat                       26011
#define IDC_OCBUTTONS                   26012
#define IDC_INSTRUCTION                 26013
#define IDM_TOOLBARS                    26014
#define IDM_OCCustom                    26015
#define IDC_TBAROPTIONS                 26016
#define IDC_TBARDELRESET                26017
#define IDS_TBARMAIN					26018
#define IDS_TBARRIBBON					26019
#define IDS_TBARSTATUS					26020
#define IDC_TBARTOOLTIPS				26021
#define IDS_TBARRESET					26022
#define IDS_TBARDELETE					26023
#define IDS_CANTREPLACEMAIN				26024
#define IDS_CANTREPLACESTATUS			26025
#define IDS_CANTREPLACERIBBON			26026
#define IDS_CATFILEMENU					26027
#define IDS_CATEDITMENU					26028
#define IDS_CATVIEWMENU					26029
#define IDS_CATTOOLSMENU				26030
#define IDS_CATMASKMENU					26031
#define IDS_CATMAPMENU					26032
#define IDS_CATIMAGEMENU				26033
#define IDS_CATOBJECTMENU				26034
#define IDS_CATWINDOWMENU				26035
#define IDS_CATHELPMENU					26036
#define IDS_CATSELECTORTOOL				26037
#define IDS_CATMASKTOOLS				26038
#define IDS_CATRETOUCHTOOLS				26039
#define IDS_CATFILTERTOOLS				26040
#define IDS_CATFILLTOOLS				26041
#define IDS_CATDRAWTOOLS				26042
#define IDS_CATZOOMTOOLS				26043
#define IDS_CATTEXTTOOL					26044
#define IDS_CATPROPETOOLS				26045
#define IDS_CATMACROTOOLS				26046
#define IDS_CATCONTROLS					26047
#define IDR_FILEMENU					26048
#define IDR_EDITMENU					26049
#define IDR_VIEWMENU					26050
#define IDR_TOOLMENU					26051
#define IDR_MASKMENU					26052
#define IDR_MAPMENU						26053
#define IDR_IMAGEMENU					26054
#define IDR_OBJMENU						26055
#define IDR_WINDOWMENU					26056
#define IDR_HELPMENU					26057
#define IDR_SELECTOR					26058
#define IDR_MASKTOOL					26059
#define IDR_PAINTTOOL					26060
#define IDR_FILTERTOOL					26061
#define IDR_FILLTOOL					26062
#define IDR_DRAWTOOL					26063
#define IDR_ZOOMTOOL					26064
#define IDR_TEXTTOOL					26065
#define IDR_PROPETOOL					26066
#define IDC_OCMacro						26067
#define IDC_OCMACRO_STEXT				26068
#define IDS_OCZOOM						26069
#define IDS_OCSTATUS					26070

// 26100 - ... reserved for exposing localized strings
#define IDS_SLASH 						26100
#define IDS_NONAME						26101
#define IDS_FONTFACENAME				26102

#endif // _ID_H_

