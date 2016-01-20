/****************************************************************************
 *                     Copyright 1995 INSO Corporation                      *
 *                    IntelliFinder(TM) Reference Engine                    *
 *                                                                          *
 * This work is protected by United States and international copyright laws *
 * and contains certain trade secret information.  All use of this work is  *
 * governed by the terms of agreements between INSO Corporation and its     *
 * licensees or potential licensees.  Disclosure or disassembly of the      *
 * contents of embodied programs or algorithms is prohibited.  All rights   *
 * are reserved worldwide.                                                  *
 ****************************************************************************/

/*--------------------------------------------------------------------------*
 *                                                                          *
 * REstyle.h - Style definitions for the Reference Engine 2.3 databases. s. *
 *                                                                          *
 *--------------------------------------------------------------------------*/
 
/* Standard Style Definitions */ 
#define RE_HEADWORD_STYLE           0
#define RE_PARTOFSPEECH_STYLE       1
#define RE_PRONUNCIATION_STYLE      2
#define RE_ALTERNATEHEAD_STYLE      3
#define RE_SECTION_STYLE            4
#define RE_DEFINITIONNO_STYLE       5
#define RE_ETYMOLOGY_STYLE          6
#define RE_BODYTEXT_STYLE           7
#define RE_ILLUSTRATION_STYLE       8
#define RE_USAGELABEL_STYLE         9
#define RE_TITLE0_STYLE             10
#define RE_TITLE1_STYLE             11
#define RE_TITLE2_STYLE             12
#define RE_TITLE3_STYLE             13
#define RE_SOURCE_STYLE             14
#define RE_DATA_STYLE               15
#define RE_FOOTNOTE_STYLE           16
#define RE_ALTPOS_STYLE             17
#define RE_SMALLTEXT_STYLE          18
#define RE_COURIER_STYLE            19
#define RE_AUTHOR_STYLE             20
#define RE_BIBLIO_STYLE             21
#define RE_UNUSED6_STYLE            22
#define RE_UNUSED7_STYLE            23
#define RE_UNUSED8_STYLE            24
#define RE_UNUSED9_STYLE            25
#define RE_UNUSED10_STYLE           26
#define RE_UNUSED11_STYLE           27
#define RE_UNUSED12_STYLE           28
#define RE_UNUSED13_STYLE           29
#define RE_UNUSED14_STYLE           30
#define RE_UNUSED15_STYLE           31
#define RE_SPECIAL1_STYLE           32
#define RE_SPECIAL2_STYLE           33                
#define RE_SPECIAL3_STYLE           34                
#define RE_SPECIAL4_STYLE           35
#define RE_SPECIAL5_STYLE           36                
#define RE_SPECIAL6_STYLE           37                
#define RE_SPECIAL7_STYLE           38                
#define RE_SPECIAL8_STYLE           39                
#define RE_SPECIAL9_STYLE           40                
#define RE_SPECIAL10_STYLE          41                

/* Make sure DISPLAY_MAXSTYLES is equal to 1 + the maximum number used above */                
#define DISPLAY_MAXSTYLES   42
 

/*  Dictionary style definitions
 *  AHD3, 2nd College, Dell 2, Dell 3, DOCL, Websters
 */
#define DIC_HEADWORD                RE_HEADWORD_STYLE
#define DIC_PARTOFSPEECH            RE_PARTOFSPEECH_STYLE
#define DIC_ALTPOS                  RE_ALTPOS_STYLE
#define DIC_PRONUNCIATION           RE_PRONUNCIATION_STYLE
#define DIC_ALTERNATEHEAD           RE_ALTERNATEHEAD_STYLE
#define DIC_SECTION                 RE_SECTION_STYLE
#define DIC_DEFINITIONNO            RE_DEFINITIONNO_STYLE
#define DIC_ETYMOLOGY               RE_ETYMOLOGY_STYLE
#define DIC_BODYTEXT                RE_BODYTEXT_STYLE
#define DIC_ILLUSTRATION            RE_ILLUSTRATION_STYLE
#define DIC_USAGELABEL              RE_USAGELABEL_STYLE

/* Bilingual stuff          */
#define BIL_HEADWORD                RE_HEADWORD_STYLE
#define BIL_PARTOFSPEECH            RE_PARTOFSPEECH_STYLE
#define BIL_ALTPOS                  RE_ALTPOS_STYLE
#define BIL_PRONUNCIATION           RE_PRONUNCIATION_STYLE
#define BIL_ALTERNATEHEAD           RE_ALTERNATEHEAD_STYLE
#define BIL_SECTION                 RE_SECTION_STYLE
#define BIL_DEFINITIONNO            RE_DEFINITIONNO_STYLE
#define BIL_ETYMOLOGY               RE_ETYMOLOGY_STYLE
#define BIL_BODYTEXT                RE_BODYTEXT_STYLE
#define BIL_ILLUSTRATION            RE_ILLUSTRATION_STYLE
#define BIL_USAGELABEL              RE_USAGELABEL_STYLE
#define BIL_SOURCETEXT              RE_SPECIAL1_STYLE
#define BIL_TARGETTEXT              RE_SPECIAL2_STYLE

/*  Quotations style definitions
 *  Simpson's Book of Quotations
 */
#define QUO_HEAD                    RE_TITLE0_STYLE
#define QUO_SUBHEAD                 RE_TITLE1_STYLE
#define QUO_NAME                    RE_HEADWORD_STYLE
#define QUO_TITLE                   RE_TITLE3_STYLE
#define QUO_BODY                    RE_BODYTEXT_STYLE
#define QUO_SOURCE                  RE_SOURCE_STYLE
    
/*  Almanac Style definitions
 *  Information Please Almanac, Business Almanac
 *  and soon to be added Sports Almanac.
 */
#define ALM_HEAD                    RE_HEADWORD_STYLE
#define ALM_TITLE0                  RE_TITLE0_STYLE
#define ALM_TITLE1                  RE_TITLE1_STYLE
#define ALM_TITLE2                  RE_TITLE2_STYLE
#define ALM_TITLE3                  RE_TITLE3_STYLE
#define ALM_ALTERNATEHEAD           RE_ALTERNATEHEAD_STYLE
#define ALM_SOURCE                  RE_SOURCE_STYLE
#define ALM_BODY                    RE_BODYTEXT_STYLE
#define ALM_INDENT                  RE_SPECIAL3_STYLE
#define ALM_FLUSHLEFT               RE_SPECIAL4_STYLE
#define ALM_DATA                    RE_DATA_STYLE
#define ALM_FOOTNOTE                RE_FOOTNOTE_STYLE
#define ALM_SUMMARY                 RE_SPECIAL2_STYLE

/*  Encyclopedia Style definitions
 *  Columbia Encyclopedia
 */
#define ENCYCL_HEAD                 RE_HEADWORD_STYLE
#define ENCYCL_DEFINITIONNO         RE_DEFINITIONNO_STYLE
#define ENCYCL_TITLE0               RE_TITLE0_STYLE
#define ENCYCL_TITLE1               RE_TITLE1_STYLE
#define ENCYCL_TITLE2               RE_TITLE2_STYLE
#define ENCYCL_TITLE3               RE_TITLE3_STYLE
#define ENCYCL_ALTERNATEHEAD        RE_ALTERNATEHEAD_STYLE
#define ENCYCL_SOURCE               RE_SOURCE_STYLE
#define ENCYCL_COURIER              RE_COURIER_STYLE
#define ENCYCL_BODY                 RE_BODYTEXT_STYLE
#define ENCYCL_ILLUSTRATION         RE_ILLUSTRATION_STYLE
#define ENCYCL_INDENT               RE_SPECIAL3_STYLE
#define ENCYCL_FLUSHLEFT            RE_SPECIAL4_STYLE
#define ENCYCL_DATA                 RE_DATA_STYLE
#define ENCYCL_FOOTNOTE             RE_FOOTNOTE_STYLE
#define ENCYCL_SUMMARY              RE_SPECIAL2_STYLE
#define ENCYCL_SMALL                RE_SMALLTEXT_STYLE
#define ENCYCL_PRONUNCIATION        RE_PRONUNCIATION_STYLE
#define ENCYCL_LISTITEM             RE_SPECIAL5_STYLE
#define ENCYCL_AUTHOR               RE_AUTHOR_STYLE
#define ENCYCL_BIBLIO               RE_BIBLIO_STYLE

/*
 *  Directory Styles
*/                  
#define DIRECT_WD                   RE_HEADWORD_STYLE
#define DIRECT_BODY                 RE_BODYTEXT_STYLE
#define DIRECT_FAXNOTE              RE_FOOTNOTE_STYLE

#ifdef RE_SF_CIRCLE_INDEX
/*  CIET Style Definitions
 *  Concise Thesaurus
 */
#define CIET_HEAD                   RE_HEADWORD_STYLE
#define CIET_BODY                   RE_BODYTEXT_STYLE
#define CIET_PARTOFSPEECH           RE_PARTOFSPEECH_STYLE
#endif

/*  Defines for Table alignment
 */
#define ALIGN_LEFT      'L'
#define ALIGN_CENTER    'C'
#define ALIGN_RIGHT     'R'
#define ALIGN_JUSTIFY   'J'
#define ALIGN_DECIMAL   'D'

/*  Strings for Data stream
 */
#define DATA_COLCOUNT    "CO"
#define DATA_CHAINSTART  "CS"
#define DATA_CHAINEND    "CE"
#define DATA_CHAINPARENT "PAR"
#define DATA_TABLEHEAD   "TH"
#define DATA_TABLEFOOT   "TF"
#define DATA_TABLEBODY   "TB"
#define DATA_TABLEEND    "TE"
#define DATA_TABLESPAN   "SP"
#define DATA_TABLEALIGN  "AL"

/*  These values used fixed constants
 *  because not all platforms agree on
 *  the translation of '\n', '\r', and
 *  '\t'.  Therefore, we must use one
 *  concistent set of definitions.
 */
#define DATA_NEXTCELL       9    /* End of Column      */
#define DATA_NEXTROW        10   /* End of Row         */
#define DATA_LINEBREAK      10   /* End of Line        */
#define DATA_OPTBREAK       13   /* Optional Linebreak */

#define DISPLAY_MAXFONTS    8
#define DISPLAY_MAXCOLS     5

/* End of File - REstyle.h */
