/*----------------------------------------------------------------------*
*                                                                       *
*                   Copyright 1995 INSO Corporation                     *
*                         Common Code Library                           *
*                                                                       *
* This work is protected by United States and international copyright   *
* laws and contains certain trade secret information.  All use of this  *
* work is governed by the terms of agreements between INSO Corporation  *
* and its licensees or potential licensees.  Disclosure or disassembly  *
* of the contents of embodied programs or algorithms is prohibited.     *
* All rights are reserved worldwide.                                    *
*                                                                       *
*-----------------------------------------------------------------------*

*-----------------------------------------------------------------------*
*                                                                       *
*   SLlang.h - Language code definitions.                               *
*                                                                       *
*-----------------------------------------------------------------------*

    Description:
        This file contains #defines for language codes.

    Cautions:
        None.

    Creation Date:
        18-jul-89   

*-------------------------------------------------------------------*/
#ifndef SLLANG_H

#define SLLANG_H


#define MAX_LANG_CODE   19          /* To limit lang->charset cnv   */

/*                              Language codes                      */

#define SLUNIVERSAL     0           /* Universal Language code      */

#define SLCZECH         1           /* Czech                        */
#define SLRUSSIAN       2           /* Russian                      */
#define SLCATALAN       3           /* Catalan                      */
#define SLHUNGARIAN     4           /* Hungarian (Magyar)           */
#define SLFRENCH        5           /* French                       */
#define SLGERMAN        6           /* German                       */
#define SLSWEDISH       7           /* Swedish                      */
#define SLSPANISH       8           /* Spanish                      */
#define SLITALIAN       9           /* Italian                      */
#define SLDANISH        10          /* Danish                       */
#define SLDUTCH         11          /* Dutch                        */
#define SLPORTUGUESE    12          /* Portuguese                   */
#define SLNORWEGIAN     13          /* Norwegian                    */
#define SLFINNISH       14          /* Finnish                      */
#define SLGREEK         15          /* Greek                        */ 
#define SLENGLISH       16          /* English                      */
#define SLAUSTRALIAN    16          /* Australian                   */
#define SLAFRIKAANS     17          /* Afrikaans                    */
#define SLPOLISH        18          /* Polish                       */
#define SLMULTI         19          /* Mixed language               */

#define SLCHINESE       30          /* Chinese                      */
#define SLJAPANESE      31          /* Japanese                     */

#ifdef COMMENT
/* To be implemented                                                */

#define SLFLEMISH       20          /* Flemish                      */
#define SLGAELIC        21          /* Gaelic                       */
#define SLHEBREW        22          /* Hebrew                       */
#define SLICELANDIC     23          /* Icelandic                    */
#define SLKOREAN        24          /* Korean (Hangul)              */
#define SLARABIC        25          /* Arabic                       */
#define SLSERBO_CROATN  26          /* Serbo-Croatian               */
#define SLSLOVAK        27          /* Slovak                       */
#define SLTURKISH       28          /* Turkish                      */
#define SLURDU          29          /* Urdu/Hindi                   */
#define SLBASQUE        32          /* Basque                       */
#define SLHAWAIIAN      33          /* Hawaiian                     */
#define SLLATIN         34          /* Latin                        */
#define SLMAORI         35          /* Maori                        */
#define SLNZEALAND      36          /* New Zealand                  */

#endif /* COMMENT */


/*                         Language bit maps                        */

/* Currently Available Languages */

#define SL_CZ_MASK    0x00000001L   /* Czech Bit Mask               */
#define SL_RU_MASK    0x00000002L   /* Russian Bit Mask             */
#define SL_CA_MASK    0x00000004L   /* Catalan Bit Mask             */
#define SL_HU_MASK    0x00000008L   /* Hungarian (Magyar) Bit Mask  */
#define SL_FR_MASK    0x00000010L   /* French Bit Mask              */
#define SL_GR_MASK    0x00000020L   /* German Bit Mask              */
#define SL_SW_MASK    0x00000040L   /* Swedish Bit Mask             */
#define SL_SP_MASK    0x00000080L   /* Spanish Bit Mask             */
#define SL_IT_MASK    0x00000100L   /* Italian Bit Mask             */
#define SL_DN_MASK    0x00000200L   /* Danish Bit Mask              */
#define SL_DU_MASK    0x00000400L   /* Dutch Bit Mask               */
#define SL_PO_MASK    0x00000800L   /* Portuguese Bit Mask          */
#define SL_NO_MASK    0x00001000L   /* Norwegian Bit Mask           */
#define SL_FN_MASK    0x00002000L   /* Finnish Bit Mask             */
#define SL_GK_MASK    0x00004000L   /* Greek Bit Mask               */
#define SL_EN_MASK    0x00008000L   /* English Bit Mask             */
#define SL_AF_MASK    0x00010000L   /* Afrikaans Bit Mask           */
#define SL_PL_MASK    0x00020000L   /* Polish Bit Mask              */
                       
#define SL_CH_MASK    0x20000000L   /* Chinese  Bit Mask            */
#define SL_JP_MASK    0x40000000L   /* Japanese Bit Mask            */

#define SL_ALL_LANGS  0xC000FFFFL   /* All currently avail. langs   */

/* Grouped Languages */

/* Languages with dialects */
#define SL_DIALECT_MASK (SL_EN_MASK | SL_GR_MASK | SL_DU_MASK | SL_NO_MASK | SL_PO_MASK)

/* Languages with post clitics */
#define SL_POSTCLT_MASK   (SL_IT_MASK | SL_SP_MASK | SL_FR_MASK | SL_PO_MASK | SL_CA_MASK)

/* Languages with post clitics classes */
#define SL_CLASSPOST_MASK (SL_FR_MASK | SL_PO_MASK | SL_CA_MASK)

/* Languages with pre clitics */
#define SL_PRECLT_MASK    (SL_IT_MASK | SL_FR_MASK | SL_CA_MASK)

/* Languages with compounds */
#define SL_CMPND_MASK     (SL_AF_MASK | SL_GR_MASK | SL_SW_MASK | SL_DU_MASK | SL_DN_MASK | SL_NO_MASK)

/* Languages with terminal double chars.  */
#define SL_DBLEND_MASK    (SL_GR_MASK | SL_SW_MASK | SL_NO_MASK)

/* Languages with Number letter Words */
#define SL_NUMLET_MASK    (SL_GR_MASK | SL_SW_MASK | SL_DN_MASK | SL_NO_MASK)

/* Languages supplied by MophoLogic */
#define SL_MORPHO_LOGIC_MASK   (SL_HU_MASK | SL_PL_MASK)

#ifdef COMMENT

/* Possible Additional Languages */
#define SL_FL_MASK    0x00040000L   /* Flemish Bit Mask             */
#define SL_GA_MASK    0x00080000L   /* Gaelic Bit Mask              */
#define SL_HE_MASK    0x00100000L   /* Hebrew Bit Mask              */
#define SL_IC_MASK    0x00200000L   /* Icelandic Bit Mask           */
#define SL_KO_MASK    0x00400000L   /* Korean (Hangul) Bit Mask     */
#define SL_AR_MASK    0x00800000L   /* Arabic Bit Mask              */
#define SL_SC_MASK    0x01000000L   /* Serbo-Croatian Bit Mask      */
#define SL_SK_MASK    0x02000000L   /* Slovak Bit Mask              */
#define SL_TK_MASK    0x04000000L   /* Turkish Bit Mask             */
#define SL_UR_MASK    0x08000000L   /* Urdu/Hindi Bit Mask          */
#define SL_BQ_MASK    0x10000000L   /* Basque Bit Mask              */

/* Additional languages require a second 4-byte bit mask.           */

#endif

/*                       Dialect and mode defines                   */


/* Default dialect/mode setting                                         */

#define SLDEFAULT       0x0080

/* Russian modes                                                        */

#define SLRUSSIE        0x0080      /* Infrequent use of the 'io' char. */
#define SLRUSSIO        0x0040      /* common use of the 'io' char.     */

 
/* Catalan modes (mutually exclusive)                                   */

#define SLPUNTVOLAT      0x0080      /* Punt volat mode                 */
#define SLPERIOD         0x0040      /* Period mode                      */


/* French and Spanish modes (mutually exclusive)                        */

#define SLACCENT        0x0080      /* Uppercase Accented               */
#define SLUNACCENT      0x0040      /* Uppercase Unaccented             */

/* French Open or Closed Ligitaures modes (mutually exclusive)          */

#define SLOPENLIG       0x0010      /* Open Ligature                    */
#define SLCLOSELIG      0x0020      /* Closed Ligature                  */


/* German dialects                                                      */

#define SLSCHARFES      0x0080      /* German Scharfes s                */
#define SLDOPPEL        0x0040      /* German Doppel s                  */


/* Dutch spelling variants                                              */

#define SLPRIMDUTCH     0x0080      /* Primary Dutch Spelling           */
#define SLSCNDDUTCH     0x0040      /* Secondary Spelling Dutch         */


/* Portuguese dialects                                                  */

#define SL_PO_EUROPEAN  0x0080      /* European Portuguese              */
#define SLIBERIAN       SL_PO_EUROPEAN /* Backward Compatible */
#define SLBRAZILIAN     0x0040      /* Brazilian Portuguese             */


/* Norwegian dialects                                                   */

#define SLBOKMAL        0x0080      /* Bokmal                           */
#define SLNYNORSK       0x0040      /* Nynorsk                          */
#define SL2BOKMAL       0x0020      /* Bokmal sideforms included        */
#define SL2NYNORSK      0x0010      /* Nynorsk sideforms included       */


/* English dialects                                                     */

#define SL_US_ENGLISH      0x0080      /* US English                    */
#define SLISE_UK_ENGLISH   0x0040      /* UK English ISE                */
#define SLIZE_UK_ENGLISH   0x0020      /* UK English IZE                */
#define SLAMERICAN      SL_US_ENGLISH    /* Backward Compatible */
#define SLISE_BRITISH   SLISE_UK_ENGLISH /* Backward Compatible */
#define SLIZE_BRITISH   SLIZE_UK_ENGLISH /* Backward Compatible */
#define SLAUSTRLN          0x0010      /* Australian English            */

#define SLFINANCIAL     0x0008      /* Banking, Finance and Insurance   */
#define SLLEGAL         0x0004      /* Legal                            */
#define SLMEDICAL       0x0002      /* Medicine                         */
#define SLSCIENCE       0x0001      /* Science                          */

#endif /* SLLANG_H */

/* End of file - SLLANG.H */
