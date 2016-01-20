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
*        SFdef.h - Standard Function Defines for the                    *
*                  InfoSoft Software functionality                          *
*                                                                       *
*-----------------------------------------------------------------------*
*                                                                       *
		Description:
		
		This header contains all defines for the Standard Functions 

		Cautions:
		None.

		Creation Date:
		16-Apr-92   GJC     Ver. 1.0.0

		Change Record:
		02-jun-92   ECH     Tweeked.
		12-jan-93   JCC     Part of speech defines, macros
		03-feb-93   TH/JCC  Inflection states & tasks; bitmap macros

*-----------------------------------------------------------------------*/
#ifndef SFDEF_H

#define SFDEF_H

#define SFVERSION   "2.04.005"

/* Decode states (each state corresponds to a field within a decode record).
*/
#define SFCOMMON        0
#define SFDIFFERENCE    1
#define SFEXTRA         2
#define SFFILTER        3
#define SFDONE          4
#define SFADJUST        5
#define SFGETPOS        5
#define SFGETNUMSYN     6
#define SFGETSYNONYM    7
#define SFGETBASEFORM   8
#define SFGETPOI        9
#define SFGETINFLQUAD    10
#define SFGETNUMINFLQUAD 11


/* The type of index database that is being decoded.  Determined and
 * used internally.
*/
#define RENORMAL        0       /* Headword-oriented.                  */
#define RETHESAURUS     1       /* Index-oriented.                     */

/* Types of words returned with verified words.  These apply to stand-alone
 * index lists and index lists with associated entry files.  
*/
#define SF_NONE         0       /* No word                                  */
#define SF_HEADWORD     1       /* The headword of entry                    */
#define SF_DEFNWORD     2       /* The definition of entry                  */
#define SF_RETWORD      3       /* A word encoded with input in index list  */
#define SF_RETDEF       4       /* SF_RETWORD if there else SF_HEADWORD     */

/* Flags to decode for type of database to read.
*/
#define SF_ICS_CALL         0
#define SF_STSEARCH_CALL    1
#define SF_STHEADWORD_CALL  2

#ifdef SF_CORRECTION
/* Values for the correction mode parameter.
*/
#define SF_NORM_STEP    0       /* Normal ICS 8-4 operation.            */
#define SF_SING_STEP    1       /* Same but return after each step.     */
#define SF_ALL_STEPS    2       /* Perform all 20some steps             */
#define SF_SING_ALL_STEPS 3     /* Same but return after each step      */

/* Step number to limit the number of consonant corrections.
*/
#define SF_CONS_LIMIT   14
#endif 

/* The maximum number of a baseform's irregular inflections that have the 
 * same POI.  Their word-initial quadgrams are explicitly encoded after the
 * baseform.
*/
#define SFMAXINFLQUAD   2

/* Define for the maximum quadgrams in a sector.
*/
#define SFMAX_QUADS     500

/* Define for the maximum number of string substitutions in a word.
*/
#define SFMAX_STRING    500

/* Define for a quadgram space for correction.
*/
#define SFQUADSPACE     "    "  /* Four Spaces.                         */

/* Define for a quadgram space for correction.
*/
#define SFQUADSIZE      4

/* Define for size of short anagram processing.
*/
#define SFSHORTANAGRAMS 25

/* Maximum number of baseforms that can be found for an input word.
*/
#define SFMAX_BASEFORM  4

/* Maximum number of parts of speech returned with a word.
*/
#define SFMAX_POS       6

/* Macros for string substitution.
*/
#define SFBADSTRING  pDVctrl->badstring[i]  = &pDVctrl->str_tab[i << 3];
#define SFGOODSTRING pDVctrl->goodstring[i] = &pDVctrl->str_tab[(i << 3) + 4];

/* Maximum size of a binary search key */
#define SFKEYBIS        6

#define SF_STEP_SIZE    13      /* Number of bytes in a step table step */

#define SF_SUBSTR_SIZE  4       /* Number of bytes in a substitution str*/

#define SF_MAP_SIZ      2       /* Capitalization bitmap size in UINT4Bs*/

#define SF_POI_SIZ      4       /* Number of bytes in a POI map         */

#define SF_LETTER_BUF_SZ 256    /* Maximum size of Letters array.       */

#define SFPREMAX        9       /* Max length of a preclitic.           */
#define SFPOSTMAX       12      /* Max length of a postclitic.          */
#define SFSTEMMAX       6       /* Max stems from cliticized word       */
#define SFCLTREHASH     7       /* Max clitic rehash count.             */
#define SF_MAX_ENTRIES  3       /* Max number of clitic entries         */

/* Correction pass defines.
*/
#define SF_SIMPLE_CORR  1       /* 1 - simple: transposition,           */
								/* single <-> geminate.                 */
#define SF_NORMAL_CORR  2       /* 2 - normal: all types                */
#define SF_STRING_CORR  3       /* 3 - string subs on 1st or 2nd char   */
#define SF_ACCENT_CORR  4       /* 4 - accent folding                   */
#define SF_RIGGED_CORR  5       /* 5 - exception words thru->though etc */
#define SF_PHONETIC_CORR 6      /* 6 - two phonetic errors in initial   */
								/*        quadgram, one at beginning    */
#define SF_MIDLEN_CORR  7       /* 7 - length 4, 5 or 6 words           */
#define SF_SHORT_CORR   8       /* 8 - short word correction            */
#define SF_PRE_CORR     9       /* 9 - prefix/prclitic correction       */

/* Decode task numbers
*/
#define SFINITIALIZE    1       /* Initialize Database                      */
#define SFTERMINATE     2       /* Terminate Database                       */
#define SFVERIFY        3       /* Perform VerifSFation                     */
#define SFWILD          5       /* Perform Wildcard Retrieval               */
#define SFANAGRAM       6       /* Perform Anagram Processing               */
#define SFHYPHENATE     7       /* Perform Pattern Matching Hyphenation     */
#define SFDEFINITION    8       /* Perform Synonym Definition Retrieval     */ 
#define SFSMUDGE        9       /* Perform Smudge Verification; An OCR task */ 
#define SFINFLECT      10       /* Perform Inflection                       */
#define SFGETBASE      11       /* Get baseform of an input                 */
#define SFGETIRRINF    12       /* Get baseform's irregular inflections     */
#define SFFINDCLOMCH   13       /* Find word that is closest to input       */
#define SFWORDSINSECTR 14       /* Find number of words in sector           */
#define SFGETCLOMCH    15       /* Get list of closest match words          */
#define SFBLOCKOFFSET  16       /* Get the nth record from the given block  */

/* The correct task flag must be the largest because it is incremented.
*/
#define SFCORRECT       100

/* Space insertion correction task flag.
*/
#define SFSPACEINSERT   SFCORRECT + 2


/* Decode return values
*/
#define SFRETFOUND      10      /* Word Found                           */
#define SFRETNOTFOUND   11      /* Word Not Found                       */

/* Return from SFusr                                                    */
#define SFRETALT         2      /* Alternative found in Personal Dict.  */

#define SFACCTVERIFY    0x0200  /* Accent folding verify process        */


/* Status flags returned from the lexicon (pDbBits or pXtraBits)
*/

/* All Languages */

#define SFORDINAL       0x2000  /* Word is a ordinal number             */
#define SFCARDINAL      0x1000  /* Word is a cardinal number            */
#define SFABBREVI       0x0800  /* Word is an abbreviation              */
#define SFACRONYM       0x0400  /* Word is an acronym                   */

/* English */

#define SFAART          0x0000  /* Word can be preceded by A article    */
#define SFANART         0x0001  /* Word can be preceded by AN article   */
#define SFAORANART      0x0002  /* Word can be preceded by A/AN article */

/* Compound Languages */

#define SFNONMAND       0x0200  /* non-mandatory of a mandatory         */
#define SFCMPRANK       0x0100  /* Compound Rank. Is a cmp in DB.       */
#define SFCOMPLEX       0x0080  /* complex word                         */
#define SFMANDHY        0x0040  /* mandatory hyphen                     */
#define SFSPLIT         0x0020  /* split word                           */
#define SFNUMLET        0x0010  /* number-letter word                   */
#define SFRMCC          0x0008  /* right noun compound component        */
#define SFLMCC          0x0004  /* leftmost compound component          */
#define SFARMCC         0x0002  /* right adjective compound component   */
#define SFNOTALONE      0x0001  /* cannot stand alone                   */


/* Cliticizing Languages */
#define SFPSTCLIT       0x0200  /* word is postcliticizable             */
#define SFPRECLIT       0x0100  /* word is precliticizable              */

/* Clitic Data defines (iClitData) */

#define SFEXCWORD       0x80    /* clitic exception                     */

/* Italian Preclitic Features/Attributes.
*/
#define SFMSNOUN        0x0001  /* masculine noun                       */
#define SFPLNADJ        0x0002  /* masc./fem. plur. noun & adj.         */
#define SFMNINVB        0x0004  /* masc. noun / inflected verb          */
#define SFFEMSIN        0x0008  /* feminine singular noun               */
#define SFFSNINF        0x0010  /* feminine sing. & infinitive          */
#define SFFSINVB        0x0020  /* fem. sing. & inflected verb          */
#define SFPLURLI        0x0040  /* masc. plur. noun/adj. beg. w/iC only */
#define SFPLINFA        0x0080  /* 'aeow' plural & inflected form       */
#define SFPLINFI        0x0100  /* 'i' plural & inflected form          */
#define SFINFITV        0x0200  /*  infinitive                          */
#define SFINFLVB        0x0400  /*  inflected form of verb              */

/* French Preclitic Features/Attributes.  (some used for Catalan)
*/
#define SFPRREGT        0x0001  /* preclitic regular tense              */
#define SFSUBJUN        0x0002  /* subjunctive                          */
#define SFPREINF        0x0004  /* present infinitive                   */
#define SFPASPAR        0x0008  /* past participle                      */
#define SFPRNADJ        0x0010  /* pre-clitic noun / adjective          */
#define SFADVERB        0x0020  /* adverb                               */
#define SFYANDEN        0x0040  /* words y and en                       */

/* Catalan Preclitic Features/Attributes. 
*/
#define SFNVDUP         SFINFITV /*  Noun/Verb duplicate.
									 hex value borrowed from Italian.
									 Other constants taken from Italian.
								  */

/* Catalan Postclitic Features/Attributes
*/
#define SFSTMVNU        0x0001  /* stem final vowel != 'u'              */
#define SFSTMCOU        0x0002  /* stem final consonant or 'u'          */

/* French Postclitic Features/Attributes
*/

#define SFPOREGT        0x0080  /* postclitic regular tense             */
#define SFIMPERA        0x0100  /* imperative                           */
#define SFPONADJ        0x0200  /* postclitic noun / adjective          */
#define SFRETIMP        0x0400  /* regular tense imperative             */
#define SFALLCLT        0x0800  /* stem accepts all postclitics         */
#define SFRTNADJ        0x1000  /* regular tense/noun/adjective         */

#define SFSTEMCASE      0x8000  /* serves two purposes :                */        
								/* French - final stem check            */
								/* i.e must check final char of         */
								/* stem for legitimate match            */ 
								/* with a special clitic                */ 
								/* Italian - 2 case form types          */
								/* i.e. 'Velo' & 'velo'                 */

#define SFSTEMOD        0x0001  /* Spanish stem modifying word          */
#define SFMONSYL        0x0002  /* Spanish monosyllabic word            */

#define SFCLTYPE0       0x0001  /* Portuguese postclitic type 0         */
#define SFCLTYPE1       0x0002  /* Portuguese postclitic type 1         */
#define SFCLTYPE2       0x0004  /* Portuguese postclitic type 2         */
#define SFCLTYPE3       0x0008  /* Portuguese postclitic type 3         */
#define SFCLTYPE4       0x0010  /* Portuguese postclitic type 4         */
#define SFCLTYPE5       0x0020  /* Portuguese postclitic type 5         */
#define SFCLTYPE6       0x0040  /* Portuguese postclitic type 6         */

#define SFSTEM0TYPE     0x0080  /* Portuguese stem type 0               */
#define SFSTEM1TYPE     0x0100  /* Portuguese stem type 1               */
#define SFSTEM2TYPE     0x0200  /* Portuguese stem type 2               */
#define SFSTEM3TYPE     0x0400  /* Portuguese stem type 3               */
#define SFSTEM4TYPE     0x0800  /* Portuguese stem type 4               */

/* COMMENT: this probably does not belong but is required by decode
*/
#define SFDOUBLES       0x0040  /* Uppercase Double SS                  */

/* Set the nTH bit of a bit map, assuming the map is an array of UCHARs and
 * that map[0] holds bits 0-7, map[1] holds bits 8-15, map[2] holds bits 
 * 16-23, etc.
 * The operation "(n) >> 3" finds the byte in the map where the nTH
 * bit lies.  The operation "(n) & 0x07" determines which bit will be
 * set in that byte (& 0x07 is a modulus 8 operation).  Ex: bit 22 would be
 * in map[2] ( 22 >> 3 = 2 ) & we'd set the 7th bit in that byte
 * ( 0x01 << 21 & 0x07 = 0x01 << 6 ).
*/
#define SET_NTH_BIT_SF(mapArry,n)   ( (mapArry)[(n) >> 3] |=   0x01 << ( (n) & 0x07 )   )
#define CLR_NTH_BIT_SF(mapArry,n)   ( (mapArry)[(n) >> 3] &= ~(0x01 << ( (n) & 0x07 ))  )
#define TEST_NTH_BIT_SF(mapArry,n)  ( (mapArry)[(n) >> 3] &    0x01 << ( (n) & 0x07 )   )

/* Block size for scrollable wildcarding
*/
#define SFHISTORYSIZE   10

/* Wildcarding constants
*/
#define SF_BAK_PAGE     420     /* Page backwards.                          */
#define SF_FWD_PAGE     430     /* Page forwards.                           */
#define SF_START        470     /* Start an object with a primary operation.*/
#define SF_STOP         480     /* Terminate object.                        */

/* SF Wordlist (index) Encoding Type definitions.
*/
#define SF_ICS30_ENCODE     5   /* ICS 3.0 Standard Encoding                */
#define SF_SKIP30_ENCODE    6   /* ICS 3.0 Encoding with Skip Tables        */
#define SF_ATTRIB30_ENCODE  7   /* ICS 3.0 Encoding with the attribute tbl. */

#define SF_SKIPTBL_ATTR     0   /* If this bit is set then the skip table   */
								/* is used (same as encoding ver 6).        */
#define SF_ENTRY0COMP_ATTR  1   /* If this bit is set then there is special */
								/* compression for the entry number "0".    */
								/* Reference Engine Only.                   */

/*
 * SF CTO Support 
 */

/*  SF Product Support */
#define SF_SUP_GCS      0
#define SF_SUP_ICS      1
#define SF_SUP_INFLECT  2
#define SF_SUP_IPD      3
#define SF_SUP_RE       4
#define SF_SUP_IPR      5


/*  SF Feature Support  */
#define SF_SUP_ACCENT           8
#define SF_SUP_ANAGRAM          9
#define SF_SUP_BASEFORM         10
#define SF_SUP_BOOLSEARCH       11
#define SF_SUP_CLOSEST_MATCH    12
#define SF_SUP_CORRECTION       13
#define SF_SUP_DBLVER           14
#define SF_SUP_ENTRY            15
#define SF_SUP_ENT_LIST         16
#define SF_SUP_TABLE_OF_CONTENTS    17
#define SF_SUP_WILDCARD         18
#define SF_SUP_WILDPAGE         19

/*  Language support    */
#define SF_SUP_CLITIC_PROCESS   24
#define SF_SUP_BASE_CLITIC      25
#define SF_SUP_COMPOUND_PROCESS 26

/*  Decode Features     */
#define SF_SUP_SKIPTABLE        28
#define SF_SUP_XTRA_DECODE      29

/*  SFadd1() calling options */
#define SF_UNIQUE    TRUE       
#define SF_NOFILTER  FALSE
#define SF_FILTLAST  2         

#endif /* SFDEF_H */

/* End of File - SFdef.h */
