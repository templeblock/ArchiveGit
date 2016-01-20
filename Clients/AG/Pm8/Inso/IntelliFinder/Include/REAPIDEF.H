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
 * REapidef.h - External Reference Engine Constant Definitions.             *
 *                                                                          *
 *--------------------------------------------------------------------------*

Description:
    RE external constant definitions relating to the API and to the
    implementation of the Engine from a driver application.

    This replaces the previous file EDdefines.c

Creation Date:
    19-mar-92   ECH

Change Record:
    Date        Author  Comments
    ----        ------  --------
    21-aug-92   ECH     Added API defined from redefine.h
    24-mar-93   JCC     pDBIO->mAttrib defines
    29-jul-93   KDR     Adding RE_CON_THES Reference type

*----------------------------------------------------------------------------*/

#ifndef REAPIDEF_H

#define REAPIDEF_H

/****************************************************************************
 *
 * API Function Declaration Type
 *
 ****************************************************************************
*/

#define        REAPI        RETCODE SLAPI_PASCAL


/****************************************************************************
 *
 * Primary and Secondary operations.
 *
 ****************************************************************************
*/

#define RE_NULL_OP      100     /* Null operation.                          */

#define RE_ABBREV       110     /* Retrieve abbreviated text or string.     */
#define RE_ANAGRAM      120     /* Find Anagrams.                           */
#define RE_DERIVE       130     /* Find or use run-on derivations.          */
#define RE_EXPAND       140     /* RE_DERIVE + RE_INFLECT.                  */
#define RE_FULLTEXT     145     /* Retrieve full text or string.            */
#define RE_INFLECT      150     /* Find or use inflections.                 */
#define RE_INFO         160     /* Retrieve entry information w/out text.   */
#define RE_PART_SPEECH  170     /* Retrieve entry text for a given POS.     */
#define RE_MATCH        180     /* Return words if closest alphabetic match.*/
#define RE_PRON         190     /* Pretrieve the headword pronunciation.    */
#define RE_SENTENCE     200     /* Derive a search expr. from a string.     */
#define RE_SPELL        210     /* Return spelling alternatives.            */
#define RE_SUBSTRING    220     /* Perform a substring search on entry text.*/
#define RE_TOC_LIST     230     /* List entries for selected TOC sections.  */
#define RE_VERBOSE      235     /* Retrieve expanded strings (reserved).    */
#define RE_VERIFY       240     /* Return entries for an indexed word.      */
#define RE_WILDCARD     250     /* Expand a wildcard expression.            */


#define RE_BAK_PAGE     420     /* Page backwards.                          */
#define RE_FWD_PAGE     430     /* Page forwards.                           */
#define RE_MORE         440     /* Generate more data for a single page op. */
#define RE_SET          460     /* Set/change the associated object values. */
#define RE_START        470     /* Start an object with a primary operation.*/
#define RE_STOP         480     /* Terminate object.                        */
#define RE_JUMP         490     /* Jump to a specific position              */


#define RE_ENTLST       490     /* Entry List Returns (RESERVED)            */
#define RE_ENTRY        495     /* Entry Text Returns (Character or Token)  */

#define RE_FULL_PARSE   500     /* parse all entries in the entry list      */
#define RE_SINGLE_PARSE 510     /* parse the specified entry only           */


#define REMAXFILENAME (SLFNAMEMAX + 1)
#define REMAXDATE     (sizeof("dd-mm-yyyy") + 1)
#define REPD_MAXWORD  SLWORDMAX


/* PD defines for REPDdb() primary op.
*/
#define     RE_OPEN_RO      700     /* open for read-only                   */
#define     RE_OPEN_RW      710     /* open for read/write                  */
#define     RE_CREATE       720     /* create a new pd in a read/write mode.*/
#define     RE_SAVE         730     /* full save, don't close               */
#define     RE_QUICK_SAVE   740     /* incremental fast save                */
#define     RE_CLOSE        750     /* close assumes prior save, or error   */
#define     RE_CLOSE_NOSAVE 760     /* closes and warns if outstanding edits*/

/* PD defines for REPDdb() secondary op.
*/
#define     RE_SINGLE_INST  800     /* attach to single RE instance         */
#define     RE_ALL_INST     810     /* attach to all RE instances           */

/* PD defines for REPDedit() primary op.
*/
#define     RE_ADD          900     /* add an empty entry                   */
#define     RE_DELETE       910     /* delete an entry                      */
#define     RE_EDIT         920     /* modify an entry                      */

/* PD defines for REPDedit() secondary op.
*/
#define RE_ET_ADD_GROUP     1100    /* Add a new meaning group              */
#define RE_ET_DEL_GROUP     1110    /* Delete a synonym group by name       */
#define RE_ET_ADD_SYNS      1120    /* Add a synonym set to a meaning group */
#define RE_ET_DEL_SYNS      1130    /* Delete a synonym set from a meaning  */
#define RE_ET_MOD_GROUP     1140    /* Modify attributes of a meaning group */


/****************************************************************************
 *
 * Memory Management Primary operations.
 *
 ****************************************************************************/

#define RE_APP_ST       510     /* Application I/O structure.               */
#define RE_DB_ST        520     /* Data base I/O structure.                 */
#define RE_IDXLIST_ST   530     /* Index list I/O structure.                */
#define RE_ENTLIST_ST   540     /* Entry list I/O structure.                */
#define RE_ENTRY_ST     550     /* Entry text I/O structure.                */
#define RE_TOKEN_ST     555     /* Display Parsing I/O structure.           */
#define RE_SEARCH_ST    560     /* Search list I/O structure.               */
#define RE_STRTAB_ST    565     /* Search list I/O structure.               */
#define RE_TOC_FILT_ST  570     /* TOC filter I/O structure.                */
#define RE_ENT_FILT_ST  575     /* Entry filter I/O structure.  RE_NEWTOCHA              */
#define RE_FLD_FILT_ST  580     /* Field selection filter I/O structure.    */
#define RE_ETPAR_ST     590     /* Thesaurus parser (1 & 2) I/O structure.  */

#ifdef RE_SF_PDICTIONARY  /* LRSV23 */
#define RE_PDDB_ST   600     /* PD DB I/O structure.  */
#define RE_PDEDIT_ST 610     /* PD DB Edit I/O structure.  */
#endif /* RE_SF_PDICTIONARY */ /* LRSV23 */

#define RE_ENT_FILT_SIZ 0       /* rememory takes this and sizes the entry filt itself */

/* section logic values RE_NEWTOCHA*/
#define RE_CAN_BE_TOC    0x00
#define RE_MUST_BE_TOC   0x02
#define RE_IGNORE_TOC    0x04
#define RE_CANNOT_BE_TOC 0x06
#define RE_CB_CHILD_TOC  0x08
#define RE_IG_CHILD_TOC  0x10
#define RE_IS_ROOT_TOC   0x20

#ifdef WHENREADY                        
The following are unfinished and untested.  The set range macro in particular
will not yet set the first and last byte correctly.

/* macros to init, clear, set & check entry filter values  RE_NEWTOCHA */
/* set the entire struct p to 0 (excluded) */
#define RE_0CLEAR_EFILT(p, _size) \
              Cmemset( (p), (0), ENTRY_BM_SIZE(_size) ) 
/* set the entire struct p to 1 (included) */
#define RE_1CLEAR_EFILT(p, _size) \
             Cmemset(p,1, ENTRY_BM_SIZE(_size) )
/* set a range of entries starting with s and ending with e             
 * to the value v in struct p -- s is orig 0 */ 
#define RE_SET_RANGE_EFILT(p,s,e,v) \
             Cmemset(p+SET_BM_START(s),v, SET_BM_SIZE(e,s))
/* set the entry e to the valude v in struct p */
#define RE_SET_ENTRY_EFILT(p,e,v) \
                (v?SET_NTH_BIT_RE(p,e):CLR_NTH_BIT_RE(p,e))
/* return the value of the element e in the struct p */
#define RE_IS_MEMBER_EFILT(p,e) \
                   TEST_NTH_BIT_RE(p,e))

/* macro to check TOC scope values  RE_NEWTOCHA */
#define RE_IS_MEMBER_SCOPE(p,e) \
                    RE_IS_MEMBER_EFILT(p,e)
#endif /* whenready */



/****************************************************************************
 *
 * Default Defines - Used to invode a default action or value.  All = 0
 *
 ****************************************************************************/

#define RE_DEFAULT      0       /* General purpose default.  Can be used    */
                                /* in place of any of the following.        */
#define RE_DEFAULT_SIZ  0       /* Used to designate a default size for     */

#define RE_T2TSCALE     8       /* Average Text to Token ratio.             */
                                /* memory allocation.                       */
#define RE_ANA_EXACT    0       /* Use the input word length.               */

#define RE_NATIVE_LANG  0       /* Use the lang. code for the native lang.  */

#define RE_DEF_FILTER   NULLFP  /* Null Filter, Use the database filter     */

#define RE_TASKCOMPLETE 0x7FFFFFFF  
                                /* Task Complete Signal value to abort proc */

/****************************************************************************
 *
 * Other defines.
 *
 ****************************************************************************/

#define RE_VER_LEN      9       /* Length of string in format "9.99.999"    */
                                /* The Currnt version number of the RE.     */
#define RE_VER_STR      "2.3"

#define RE_MIN_ANA_LEN  2       /* Minimum length of anagrams to generate.  */

/* Refwork types.
*/
#define RE_OTHER        0
#define RE_DICTIONARY   1
#define RE_THESAURUS    2
#define RE_DOCL         3
#define RE_QUOTATIONS   4
#define RE_ALMANAC      5
#define RE_ENCYCL       6
#define RE_GIS          7
#define RE_CON_THES     8
#define RE_BILINGUAL    9
#define RE_MONO_DICT    10
#define RE_TRANSLATION  11
#define RE_DIRECTORY    12

/* String Table Classes.
*/
#define RE_REFWORK_STR  0       /* For string tokens found in entry text.   */
#define RE_POS_STR      1       /* For part-of-speech string tokens.        */
#define RE_CONTENT_STR  2       /* For Content macro tokens (e.g, <HR>).    */
#define RE_TOC_STR      3       /* For TOC section titles.                  */
#define RE_STYLE_STR    4       /* For Style list names                     */

/* String Table Indexes.
 * Note: Not all string classes need defines for the indexes of their 
 * strings.  Some indexes are output by the database, such as part of speech,
 * and can be input directly into REstrtab().  Defines may be created for
 * these classes, if desired.
*/
/* REFWORK class
*/
#define RE_LANG_RWSTR   0       /* The lexicon is in this language          */
#define RE_2LANG_RWSTR  1       /* The target language, if bilingual        */
#define RE_TITLE_RWSTR  2       /* The title of the Reference Work.         */
#define RE_CRIGHT_RWSTR 3       /* Copyright Notice for the Reference       */

/* POS class
*/
#define RE_NOUN_POS     1   /* Noun */
#define RE_MNOUN_POS    2   /* Noun (Masculine) */
#define RE_FNOUN_POS    3   /* Noun (Feminine) */
#define RE_NNOUN_POS    4   /* Noun (Neuter) */
#define RE_PRONOUN_POS  5   /* Pronoun */
#define RE_FIL1_POS     6   /* Unused */
#define RE_FIL2_POS     7   /* Unused */
#define RE_FIL3_POS     8   /* Unused */
#define RE_FIL4_POS     9   /* Unused */
#define RE_FIL5_POS     10  /* Unused */
#define RE_VERB_POS     11  /* Verb */
#define RE_TVERB_POS    12  /* Verb (Transitive) */
#define RE_INVERB_POS   13  /* Verb (Intransitive) */
#define RE_DVERB_POS    14  /* Verb (Ditransitive) */
#define RE_RVERB_POS    15  /* Verb (Reflexive) */
#define RE_IMVERB_POS   16  /* Verb (Imperitive) */
#define RE_PVERB_POS    17  /* Verb Participle */
#define RE_O1VERB_POS   18  /* Verb (other) */
#define RE_O2VERB_POS   19  /* Verb (other) */
#define RE_O3VERB_POS   20  /* Verb (other) */
#define RE_ADJ_POS      21  /* Adjective */
#define RE_CADJ_POS     22  /* Comparative Adjective */
#define RE_SADJ_POS     23  /* Superlative Adjective */
#define RE_AADJ_POS     24  /* Attributive Adjective */
#define RE_OADJ_POS     25  /* Adjective (other) */
#define RE_FIL6_POS     26  /* Unused */
#define RE_FIL7_POS     27  /* Unused */
#define RE_FIL8_POS     28  /* Unused */
#define RE_FIL9_POS     29  /* Unused */
#define RE_FIL10_POS    30  /* Unused */
#define RE_ADV_POS      31  /* Adverb */
#define RE_CADV_POS     32  /* Comparative Adverb */
#define RE_SADV_POS     33  /* Superlative Adverb */
#define RE_ADVPHR_POS   34  /* Adverbial Phrase */
#define RE_FIL11_POS    35  /* Unused */
#define RE_FIL12_POS    36  /* Unused */
#define RE_FIL13_POS    37  /* Unused */
#define RE_FIL14_POS    38  /* Unused */
#define RE_FIL15_POS    39  /* Unused */
#define RE_FIL16_POS    40  /* Unused */
#define RE_PREP_POS     41  /* Preposition */
#define RE_PPHR_POS     42  /* Prepositional Phrase */
#define RE_INTERJ_POS   43  /* Interjection */
#define RE_CONJ_POS     44  /* Conjunction */
#define RE_NUMERAL_POS  45  /* Numeral */
#define RE_IDIOM_POS    46  /* Idiom */
#define RE_FORMULA_POS  47  /* Formula */
#define RE_MIXED_POS    48  /* Mixed */
#define RE_FUNCWORD_POS 49  /* Function Word */
#define RE_OTHER_POS    50  /* Other */

/* CONTENT class
*/
#define RE_SY_CONTSTR   1       /* <SY>, Synonyms                           */
#define RE_HY_CONTSTR   2       /* <HY>, Hyponyms                           */
#define RE_AN_CONTSTR   3       /* <AN>, Antonyms                           */
#define RE_HR_CONTSTR   4       /* <HR>, Hypernyms                          */
#define RE_ID_CONTSTR   5       /* <ID>, Phrases                            */
#define RE_CR_CONTSTR   6       /* <CR>, Cross References                   */
#define RE_RW_CONTSTR   7       /* <RW>, Related Words                      */
#define RE_SE_CONTSTR   8       /* <SE>, Related Information                */


/* Reference work models.  The RE sorts unique reference works by model
 * number (an integer) and keeps latest version numbers for each of their
 * logical files internally for compatibility checks.
 * This define is at least as great as 1 + the highest currently used model 
 * number (model #s are zero-based).
*/
#define MAX_RWMODELS_RE 129


/* Flags for selecting capitalization significance on headword lookup.
*/
#define RE_NOT_SIG_CAP  1       /* Not significant                          */
#define RE_EXT_SIG_CAP  2       /* Exactly significant                      */
#define RE_SIG_CAP      3       /* Significant                              */

/* Macro for testing validity of cap value.
*/
#define VALID_CAP(c)        ((c >= RE_NOT_SIG_CAP && c <= RE_SIG_CAP) ? 1 : 0)

/* Maximum length of a Boolean Search input expresion.
*/
#define RE_BOOLEXP_MAXLEN   128
#define RE_PQ_MAXLEN 10

/* Single-character Boolean operators in input Boolean search expressions.
*/
#define OR_OP_CHAR          '|'
#define AND_OP_CHAR         '&'
#define NOT_OP_CHAR         '!'

/* Database attribute values.  Test these against pDBIO->mAttrib using the
 * TEST_NTH_BIT_RE() macro to determine the attributes of the database.
*/
#define RE_TYPE1_DBATTR     0   /* Is a type 1 database & uses parser 1?    */
#define RE_TYPE2_DBATTR     1   /* Is a type 2 database & uses parser 2?    */
#define RE_INFLECT_DBATTR   2   /* Is an inflecting database?               */
#define RE_SEARCH_DBATTR    3   /* Database has Boolean search information? */
#define RE_TOKEN_DBATTR     4
#define RE_TOC_DBATTR       5

/* Part of speech general classes.
*/
#define VERB_POSCLASS_RE   1
#define NOUN_POSCLASS_RE   2
#define ADJ_POSCLASS_RE    3
#define ADV_POSCLASS_RE    4
#define PRON_POSCLASS_RE   5
#define PREP_POSCLASS_RE   6
#define INTJ_POSCLASS_RE   7
#define CONJ_POSCLASS_RE   8

/* Max number of POIs (Part-Of-Inflection)
*/
#define MAX_POI_RE         22

/* The number of bytes in a POI map.
*/
#define RE_POI_SIZ         4

/* POI defines.  Corresponds to bits in a POI map.  Used with the 
 * SET_NTH_BIT_RE, TEST_NTH_BIT_RE, etc. macros.
*/
#define VB_POI_RE    1
#define VBZ_POI_RE   2
#define VBD_POI_RE   3
#define VBN_POI_RE   4
#define VBG_POI_RE   5
#define PH_POI_RE    6
#define BEI_POI_RE   7
#define NO_POI_RE    8
#define NOP_POI_RE   9
#define NNP_POI_RE  10
#define NNS_POI_RE  11
#define NNSP_POI_RE 12
#define JJ_POI_RE   13
#define JJR_POI_RE  14
#define JJT_POI_RE  15
#define RB_POI_RE   16
#define RBR_POI_RE  17
#define RBT_POI_RE  18
#define PP_POI_RE   19
#define IN_POI_RE   20
#define UH_POI_RE   21
#define CC_POI_RE   22

/* bit defines for the API mAttrib[] bits in the API I/O structure.
*/

#define RE_ETPARSER_APATT           0
#define RE_ALTSPELL_APATT           1
#define RE_ANAGRAM_APATT            2
#define RE_BOOLSRCH_APATT           3
#define RE_CLITICPRO_APATT          4
#define RE_INFLECT_APATT            5
#define RE_STRINGTRANS_APATT        6
#define RE_TABLEOFCONT_APATT        7
#define RE_DISP_PARSE_APATT         8
#define RE_WILDCARD_APATT           9
#define RE_WILDPAGING_APATT         10
#define RE_CLOSEST_MATCH_APATT      11
#define RE_ENTRY_RETRV_APATT        12
#define RE_ROM_APATT                13
#define RE_SINGLE_DATABASE_APATT    14
#define RE_INTERRUPTIBLE_CODE_APATT 15


#define RE_V200_API_APATT           22
#define RE_V201_API_APATT           23

/* Set the nTH bit of a bit map, assuming the map is an array of UCHARs and
 * that map[0] holds bits 0-7, map[1] holds bits 8-15, map[2] holds bits 
 * 16-23, etc.
 * The operation "(n) >> 3" finds the byte in the map where the nTH
 * bit lies.  The operation "(n) & 0x07" determines which bit will be
 * set in that byte (& 0x07 is a modulus 8 operation).  Ex: bit 22 would be
 * in map[2] ( 22 >> 3 = 2 ) & we'd set the 7th bit in that byte
 * ( 0x01 << 21 & 0x07 = 0x01 << 6 ).
*/
#define SET_NTH_BIT_RE(mapArry,n)   ( (mapArry)[(n) >> 3] |=   0x01 << ( (n) & 0x07 )   )
#define CLR_NTH_BIT_RE(mapArry,n)   ( (mapArry)[(n) >> 3] &= ~(0x01 << ( (n) & 0x07 ))  )
#define TEST_NTH_BIT_RE(mapArry,n)  ( (mapArry)[(n) >> 3] &    0x01 << ( (n) & 0x07 )   )

/* Convert an integer part of inflection to a part of speech class.  Return
 * the part of speech class of the POI.
*/
#define POI_TO_POSCLASS_RE(poi)     (poi == VB_POI_RE  || poi == VBZ_POI_RE || \
                                     poi == VBD_POI_RE || poi == VBN_POI_RE || \
                                     poi == VBG_POI_RE || poi == PH_POI_RE  || \
                                     poi == BEI_POI_RE \
                                         ? VERB_POSCLASS_RE : \
                                     poi == NO_POI_RE  || poi == NOP_POI_RE || \
                                     poi == NNP_POI_RE || poi == NNS_POI_RE || \
                                     poi == NNSP_POI_RE \
                                         ? NOUN_POSCLASS_RE : \
                                     poi == JJ_POI_RE  || poi == JJR_POI_RE || \
                                     poi == JJT_POI_RE \
                                         ? ADJ_POSCLASS_RE : \
                                     poi == RB_POI_RE  || poi == RBR_POI_RE || \
                                     poi == RBT_POI_RE \
                                         ? ADV_POSCLASS_RE : \
                                     poi == PP_POI_RE \
                                         ? PRON_POSCLASS_RE : \
                                     poi == IN_POI_RE \
                                         ? PREP_POSCLASS_RE : \
                                     poi == UH_POI_RE \
                                         ? INTJ_POSCLASS_RE : \
                                     poi == CC_POI_RE \
                                         ? CONJ_POSCLASS_RE : 0 \
                                    )

/* Return the class of a part of speech.
*/
#define POS_TO_POSCLASS_RE(pos)     (pos >= RE_NOUN_POS && pos <= RE_NNOUN_POS \
                                         ? NOUN_POSCLASS_RE : \
                                     pos == RE_PRONOUN_POS \
                                         ? PRON_POSCLASS_RE : \
                                     pos >= RE_VERB_POS && pos <= RE_O3VERB_POS \
                                         ? VERB_POSCLASS_RE : \
                                     pos >= RE_ADJ_POS && pos <= RE_OADJ_POS \
                                         ? ADJ_POSCLASS_RE : \
                                     pos >= RE_ADV_POS && pos <= RE_ADVPHR_POS \
                                         ? ADV_POSCLASS_RE : \
                                     pos >= RE_PREP_POS && pos <= RE_PPHR_POS \
                                    )

#endif /* REAPIDEF_H */

/* End of file - REapidef.h
*/
