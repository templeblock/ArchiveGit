/****************************************************************************
 *                   Copyright  1995  INSO Corporation                      *
 *                International CorrectSpell[TM] Software                   *
 *                                                                          *
 * This work is protected by United States and international copyright laws *
 * and contains certain trade secret information.  All use of this work is  *
 * governed by the terms of agreements between INSO Corporation and its     *
 * licensees or potential licensees.  Disclosure or disassembly of the      *
 * contents of embodied programs or algorithms is prohibited.  All rights   *
 * are reserved worldwide.                                                  *
 ****************************************************************************
 
*-----------------------------------------------------------------------*
*                                                                       *
*    ICwin.h - Driver typedefs and defines                              *
*                                                                       *
*-----------------------------------------------------------------------*
*                                                                       *
    Description:
    This header contains defines for the International 
    CorrectSpell(TM) Software demonstration driver.

    Cautions:
    None.

*-----------------------------------------------------------------------*/
#ifndef ICWIN_H
#define ICWIN_H

/* Max size of the message buffer.         */

#define IC_MAX_MESG    128

/* Number of language buffers to use.      */

#define ICNUM_BUFFS    30

/* Number of personal dictionaries to use. */

#define IPNUM_BUFFS    6

/* File input configuration buffer         */

typedef char * ICCONFG;
#ifdef WINDOWS
#define EDITLIMIT 32767         /* Set maximum 32K bytes in the edit window */
#else
#define EDITLIMIT 5120000       /* Set maximum 5M bytes in the edit window */
#endif

#define L_IDSNAME         7     /* Length of string IDSNAME */
#define L_IDSTITLE       21     /* Length of string IDSTITLE */
#define L_IDSDIRECT      25     /* Length of string IDSSAVEDIRECT/IDSLOGDIRECT*/

#define IDSVERSION       600
#define IDSTITLE         601
#define IDSSAVEDIRECT    602    /* Direction to save file or write output file*/
#define IDSODD           603
#define IDSINAPPOST      604
#define IDSINAPPRE       605
#define IDSINVLEAD       606
#define IDSINVTRAIL      607
#define IDSCNTSTNDAL     608
#define IDSSPLCHWHCOMP   609
#define IDSSPLIT         610
#define IDSACRFND        611
#define IDSALTHYPH       612
#define IDSAPOSALT       613
#define IDSSTEMFIN       614
#define IDSACCENT        615
#define IDSCAPERR        616
#define IDSCMPOUND       617
#define IDSSFTHYPH       618
#define IDSINAPPUNCT     619
#define IDSUNKNOWN       620
#define IDSMSG1          621
#define IDSMSG2          622
#define IDSMSG3          623
#define IDSMSG4          624
#define IDSMSG5          625
#define IDSMSG6          626
#define IDSMSG7          627
#define IDSMSG8          628
#define IDSMSG9          629
#define IDSMSGMULT1      630
#define IDSMSGMULT2      631
#define IDSMSGCMPERR     632
#define IDSMSGNOTFRE     633
#define IDSMSGFINCH      634
#define IDSMSGPREPSTI    635
#define IDSMSGIPUN       636
#define IDSMSGODDCI      637
#define IDSMSGODDCA      638
#define IDSMSGFNDAC      639
#define IDSMSGAPOS       640
#define IDSMSGCAP        641
#define IDSMSGACC        642
#define IDSMSGIPTH       643
#define IDSMSGACCNYM     644
#define IDSMSG3A         645
#define IDSMSG4A         646
#define IDSMSGSPLITA     647
#define IDSMSGSPLITB     648
#define IDSSHWOLB        649
#define IDSNOTFOUND      650
#define IDSNOTFOUND2     651
#define IDSMODPOSTCLT    652
#define IDSMSGFNDACB     653
#define IDSMSGMODPSTCLT  654
#define IDSABBRERR       655
#define IDSINVSUFF       656
#define IDSMSGABBRERR    657
#define IDSMSGINVSUFF    658
#define IDSPERALT        659
#define IDSMSGPERALT     660
#define IDSMSG2PERALT    661
#define IDSNUMATT        662
#define IDSMSGNUMATT     663
#define IDSLIGMODALT     664
#define IDSMSGLIGATURE   665
#define IDSMSGPREI       666
#define IDSINVINTERPUN   669
#define IDSMODSTEM       670
#define IDSMSGSTEMCHG    671
#define IDSNONCMP        672
#define IDSMSGHE_NONCMP  673
#define IDS6_3           674
#define IDSMSGHE_6_3     675

#ifdef  ICS_PDICTIONARY
#define IDADDTO          667
#endif

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
  #defines for menu choices
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

/* menu choices */
#define IDM_LANG1      200
#define IDM_LANGLIMIT  IDM_LANG1 + ICNUM_BUFFS - 1
#define IDM_ABOUT      250
#define IDM_OPEN       251
#define IDM_NEW        252
#define IDM_SAVE       253
#define IDM_SAVEAS     254
#define IDM_EXIT       255
#define IDM_UNDO       256
#define IDM_CUT        257
#define IDM_COPY       258
#define IDM_PASTE      259
#define IDM_CLEAR      260
#define IDM_SELALL     261
#define IDM_VERIFY     262
#define IDM_CORRECT    263
#define IDM_HYPHEN     264
#define IDM_ANAGRAM    265
#define IDM_WILD       266
#define IDM_IPD        267
#define IDM_MARKED     268
#define IDM_OPTIONS    269
#define IDM_ANERROR    270
#define IDM_NOTFOUND   271
#define IDM_LOOKUP     272
#define IDM_GKPREWDERR 273

/* FOR NEW PD */
#define IDM_NEW_PD     420
#define IDM_OPEN_PD    421
#define IDM_SAVE_PD    422
#define IDM_SAVEAS_PD  423
#define IDM_MAINT_PD   424
#define IDM_CLOSE_PD   425
#define ID_PD_SV_FNAME 430
#define ID_PD_SV_PD_FORMAT  431
#define ID_PD_SV_LIST_FORMAT 432

#define DLG_PDSV_OPTION 450

#define ID_PD_LIST_ALL  433

#define DLG_PD_SELECT   451

#define ID_PD_CLOSE_FNAME 434
#define ID_PD_CL_SAVE     435
#define ID_PD_CL_SAVEAS   436
#define ID_PD_CL_DISCARD  437

#define DLG_PD_CLOSE      452

#define ID_PD_ADD_WD      438
#define ID_PD_ENTER_WD    439
#define ID_PD_NEG_WD      440
#define ID_PD_ALT_WD      441
#define ID_PD_ADD_STAY	  442
#define ID_PD_DEL_WD_FRM_LST	482

#define DLG_PD_ADD        453

#define ID_PD_OPT_FNAME       460
#define ID_PD_OPT_SOFT_HYP    461
#define ID_PD_OPT_DICT_SIZE   462
#define ID_PD_OPT_LANG_LIST   463

#define DLG_PD_OPTION     454

#define IDM_PD_SELECT     455
#define IDM_PD_ADD        456
#define IDM_PD_DELETE     457
#define IDM_PD_OPTION     458

#define ID_PD_DELWD_FNAME 470
#define ID_PD_DELETE_WD   471

#define DLG_PD_DELETE     473

#define IDM_PDWD_LIST     459

#define ID_PD_NAMES	495
#define ID_PD_LST_MORE    668


#define ID_PD_WDLS_FNAME  480
#define ID_PD_WD_LIST     481
#define ID_PD_SOFT_HYP	434
#define ID_PD_DICT_SIZE	435
#define ID_PD_LANG_LIST	436
#define DLG_PD_ADD	453
#define ID_PD_ALT_WD	441
#define ID_PD_ADD_WD	438
#define ID_PD_ENTER_WD	439
#define ID_PD_NEG_WD	440
#define DLG_PD_CLOSE	452
#define ID_PD_CLOSE_FNAME	434
#define ID_PD_CL_SAVE	435
#define ID_PD_CL_SAVEAS	436
#define ID_PD_CL_DISCARD	437
#define IDM_CLOSE_PD	425
#define DLG_PD_SELECT	451
#define ID_PD_OP_FNAME	433
#define ID_PD_LIST_ALL	433
#define DLG_PDSV_OPTION	450
#define ID_PD_SV_LIST_FORMAT	432
#define ID_PD_SV_PD_FORMAT	431
#define ID_PD_SV_FNAME	430
#define IDM_MAINT_PD	424
#define IDM_SAVEAS_PD	423
#define IDM_NEW_PD	420
#define IDM_OPEN_PD	421
#define IDM_SAVE_PD	422
#define ID_EXIT_PD_LIST	417
#define ID_CONT_PD_LIST	418
#define ID_PD_LIST	416
#define ID_DICT_SIZE	413
#define ID_LANG_CODE	414
#define ID_SOFT_HYP	415
#define ID_TEMP_FILE	412
#define ID_AUTO	407
#define ID_EMPTY	408
#define ID_FORMATTED_ASCII	409
#define ID_UNFORMATTED_ASCII	410
#define ID_COMPRESSED	411
#define ID_OPEN_FILE	404
#define ID_CLOSE_FILE	405
#define ID_BROWSE	406

#define ID_PD_WD_LIST_NUM  485
#define IDM_PD_WDLIST 486
#define ID_PD_WDLIST_FNAME 487



#define DLG_PD_WORD_LIST  482

/* IDs for the Run time Correction Options */
#define ID_STANDARD_ALTS	   490
#define ID_EXTEND_ALTS	      491
#define ID_RET_EVERY_STEP  492



#define IDM_FILE         0
#define IDM_EDIT         1
#define IDM_ICS          2
#define IDM_LANGUAGES    5
#define IDM_PD           3
#define IDM_FONT         4

/* dialog boxes */
#define DLG_ICS        300
#define DLG_ABOUT      301
#define DLG_OPTIONS1   302
#define DLG_OPTIONS2   303
#define DLG_CPYRIGHT   304
#define DLG_IPDSIZE    305
#define DLG_IPD        306
#define DLG_IPDGET     307
#define DLG_IPDSAV     308
#define DLG_IPDPRINT   309
#define DLG_MINANAGRAM 310
#define DLG_FOPENBOX   311
#define DLG_FILESAVE   312
#define DLG_CWA        313
#define DLG_HYPH       314
#define DLG_HYPHB      315
#define DLG_ERRORS     316
#define DLG_ANERR      317
#define DLG_SPLIT      318
#define DLG_NOTFOUND   319
#define DLG_CACHE      323
#define DLG_IPDALT     324
#define DLG_CORRECT    325
/* PD dbox ids */
#define DLG_PD         326
#define DLG_PDGET      327



/* resource id's */
#define ID_CURRLANG    114
#define ID_PC2DEC      115
#define ID_PARSE       116
#define ID_PATHYP      117
#define ID_SOFTHYP     118
#define ID_ACCENT      119
#define ID_UNACCENT    120
#define ID_WORD        121
#define ID_VERIFY      122
#define ID_CORRECT     123
#define ID_ANAGRAM     124
#define ID_ADD         125
#define ID_WILD        126
#define ID_INFO        127

#define ID_ACC         128
#define ID_UNACC       129

#define ID_IPDBUTTON   130
#define ID_AUTOD       131    /* autodelete                 */
#define ID_DSIZE       132    /* dictionary size            */
#define ID_DNAME       133    /* dictionary name            */
#define ID_CURRDICTS   134    /* current dictionary listbox */
#define ID_DELETE      135
#define ID_NEGFLAG     136
#define ID_DISPLAY     137
#define ID_SAVE        138
#define ID_INIT        139
#define ID_GET         140
#define ID_LANGCODE    141
#define ID_DNUM        142    /* dictionary number selected by user     */
#define ID_DTOT        143    /* total number of dictionaries available */
#define ID_PSORT       144    /* IP printing */
#define ID_PUNSORT     145
#define ID_PFOR        146
#define ID_PBAC        147
#define ID_PED         148    /* IP printing word number */
#define ID_PWORDTOT    149    /* IP static string area "(1 - 100):" */
#define ID_WLEN        150    /* max. anagram word length (static field) */
#define ID_MINANA      151    /* min. anagram word length (edit field) */
#define ID_DLT1        153    /* dialect check box items. */
#define ID_DLT2        154
#define ID_DLT3        155
#define ID_RDLT1       156    /* dialect radio button items. */
#define ID_RDLT2       157
#define ID_HYPBUTTON   158    /* HYPHENATION */
#define ID_BFI         159    /* english supplementary lexicons */
#define ID_LEG         160
#define ID_MED         161
#define ID_SCI         162
#define ID_EDITWND     163
#define ID_FNAME       164
#define ID_FLIST       165
#define ID_FPATH       166
#define ID_DIRECT      167
#define ID_MORE        168
#define ID_STOP        169
#define ID_HDWORD      170
#define ID_HDLIST      171
#define ID_NEWLANG     172
#define ID_NOLANG      173
#define ID_SPLIT       174
#define ID_MSGWND      175
#define ID_STAT1       176
#define ID_STAT2       177
#define ID_STAT3       178
#define IDC_EDIT_wDID  122 /* wDID info edit box */
#define ID_STAT4       179
#define ID_CACHECOMBO  187 /* drop down combo box for cache size */
#define ID_AUTOCOR     188 /* checkbox for auto correction */
#define ID_AUTOPSE     189 /* checkbox for auto IPD search */
#define ID_AUTOSPI     190 /* checkbox for auto space insertion */
#define ID_AUTOFPR     191 /* checkbox for auto final period removal */
#define ID_OPEN_LIG    192 /* French Open Ligature */
#define ID_CLOSE_LIG   193 /* French Close Ligature */
#define ID_UHHCOR      194 /* checkbox for auto unusual combined form corr. */
#define ID_DLT4        195 /* checkbox for Norwegian With SideForm. */

#define DLG_GKERR	   103   /* Possible error dbox for Greek. */
#define ID_GREEK_MSG	102   /* Msg Editbox for Possible Error for Greek. */
#define ID_GREEK_WD	101   /* Word Editbox for Possible Error for Greek. */


/* IDs for DLG_PD dbox's events */
#define ID_OPEN_PD            401
#define ID_ANAGRAM_PD         402
#define ID_LIST_PD            403
#define ID_CLOSE_DB	      404
/* IDs for DLG_PDGET dbox's events */
#define ID_OPEN_FILE          404
#define ID_CLOSE_FILE         405
#define ID_BROWSE             406
#define ID_AUTO               407
#define ID_EMPTY              408
#define ID_FORMATTED_ASCII    409
#define ID_UNFORMATTED_ASCII  410
#define ID_COMPRESSED         411
#define ID_TEMP_FILE          412
#define ID_DICT_SIZE          413
#define ID_SOFT_HYP           415


#define DEMOCONFIGFILE (LPSTR)"icconfig"

/* puts text in listbox */
#define INLIST(x, y) \
   SendDlgItemMessage(x, ID_INFO, LB_INSERTSTRING, (WPARAM)NEGONE, (LPARAM)(LPSTR)y)

/* puts text in split word listbox */
#define INSPLIT(x, y) \
   SendDlgItemMessage(x, ID_SPLIT, LB_INSERTSTRING, (WPARAM)NEGONE, (LPARAM)(LPSTR)y)

/* puts text in message text window */
#define INTEXT(x, y) \
   SetDlgItemText(x, ID_MSGWND, y)


#ifdef WIN32
#define GET_WM_COMMAND_HWND(wp, lp)  ((HWND)(lp))
#define GET_WM_COMMAND_ID(wp, lp)    LOWORD(wp)
#define GET_WM_COMMAND_CMD(wp, lp)   HIWORD(wp)
#else
#define GET_WM_COMMAND_HWND(wp, lp)  ((HWND)LOWORD (lp))
#define GET_WM_COMMAND_ID(wp, lp)    (wp)
#define GET_WM_COMMAND_CMD(wp, lp)   HIWORD(lp)
#endif

#endif /* ICWIN_H */

/* End of file ICwin.h */
