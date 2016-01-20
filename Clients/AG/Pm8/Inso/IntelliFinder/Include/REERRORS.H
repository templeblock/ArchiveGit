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
 * REerrors.h - Error return codes used by the Reference Engine             * 
 *              Software.                                                   *
 *                                                                          *
 *--------------------------------------------------------------------------*

Description:
    Error numbers in RE_error if RE functions return ERRET.

Creation Date:
    26-may-89   L Tang

Change Record:
    Date        Author  Comment

    26-mar-91   JCC     added new entry retrieval error codes.
    17-may-91   JCC     added null pointer error.
    17-oct-91   ECH     added new errors for the database management. ECH02
    11-nov-91   JCC     TOCHA errors
    16-nov-91   ECH     Removed obsolete errors. ECH02
    21-feb-92   RAS     ED names -> RE
    23-mar-92   ECH     Added _ERRFILE defines for error reporting. ECH05
    25-mar-92   ECH     New errors for new API. ECH05

*---------------------------------------------------------------------------*/

#ifndef REERRORS_H

#define REERRORS_H

/************************************************************************
*
* Possible values for RE_error
*
*************************************************************************
*/

#define RE_NO_ERROR          0   /* no error                            */
#define RE_ALLOC_ERR        -100 /* object allocation error             */
#define RE_HDLLOCK_ERR      -101 /* object lock or unlock error         */
#define RE_HDLFREE_ERR      -102 /* object freeing error                */
#define RE_NULLPTR_ERR      -103 /* A null pointer encountered          */

#define RE_DBOPEN_ERR       -201 /* Cannot open database                */
#define RE_DBHEAD_ERR       -202 /* Verification error in db header     */
#define RE_DBREAD_ERR       -204 /* read database error                 */
#define RE_DBSEEK_ERR       -205 /* Random access database error        */

#define RE_INVAL_DICT_ERR   -210 /* dictionary code unrecognized        */
#define RE_DBSYNTAX_ERR     -211 /* unexpected sequence of chrs in DB   */
#define RE_UNKNFLD_ERR      -212 /* unknown field selection define      */
#define RE_DBNOBOOL_ERR     -213 /* no boolean search files init'd      */
#define RE_UNKN_FNCT        -214 /* unknown value of a mode param       */
#define RE_NOINFL_ERR       -215 /* no inflectional info in index       */

#define RE_AM_ERR           -230 /* Internal Application Manager error. */

#define RE_DM_ERR           -240 /* Internal Database Manager error.    */
#define RE_DM_NOVENTRY      -242 /* No database found for the given dbid*/
#define RE_DM_BUSY_ERR      -244 /* The virtual database has been locked*/
#define RE_DM_MULTSLEEP_ERR -246 /* The database has a neg. lock count  */
#define RE_DM_2WAKE_ERR     -248 /* The database is already awake.      */
#define RE_DM_WAKE_ERR      -250 /* The database didn't wakw up.        */
#define RE_DM_SLEEP_ERR     -252 /* The database has insomnia.          */
#define RE_DM_OPEN_ERR      -254 /* An error occured in opening the db  */
#define RE_DM_CLOSE_ERR     -256 /* An error occured in closing the db  */

#ifdef RE_SF_1RDB
#define RE_DM_INST_ERR      -258 /* tried to open too many virt. inst's */
#define RE_DM_BADID_ERR     -260 /* tried to close an invalid virt. db  */
#endif

#define RE_DM_SAVE_ERR      -270
#define RE_DM_DELETE_ERR    -271
#define RE_DM_ADD_ERR       -272


#define RE_NOWILDCHARS      -300 /* Expect wildcard characters in input */
#define RE_WILDCARD_INPUT   -301 /* Wildcard character(s) found in the  */
                                 /* input string                        */
#define RE_STR_LONG         -302 /* Input string is over the required   */
                                 /* length limit                        */
#define RE_NULL_STR         -303 /* Input is a null string              */
#define RE_NOT_IN_DB_INPUT  -304 /* Input string is not in the database */
#define RE_OUTLIST_SHORT    -305 /* Output buffer is full; Data size is */
                                 /* larger than the size of output list */
#define RE_CAP_SIG_ERR      -306 /* Capitalization significance level   */
                                 /* error                               */
#define RE_NO_RETURNS       -350 /* No returns from data retrieval      */
#define RE_TOO_MANY_RTNS    -351 /* Number of returns exceed the limit  */
#define RE_END_OF_DATA      -352 /* End of data returns                 */
#define RE_TOP_OF_DATA      -353 /* Top of data returns                 */

#define RE_RAMTABINI_ERR    -390 /* Initializing ram tables, fialed.    */
#define RE_NO_GETINI        -400 /* Get data table has not been         */
                                 /* initialized                         */
#define RE_ENTRY_NUM_ERR    -402 /* Invalid entry number                */
#define RE_ENTRY_PS_ERR     -403 /* Invalid part of speech for entry    */
#define RE_MINANA_ERR       -404 /* Min rqsted anagram length too > or <*/

#define RE_PAREN_MATCH      -410 /* parentheses don't balance in the exp*/
#define RE_PAREN_LEVEL      -411 /* paren level too deep in expression  */
#define RE_TOO_MANY_OPNDS   -412 /* too many operands in expression     */
#define RE_NO_OPNDS         -413 /* no operands in expression           */
#define RE_2WD_OPND         -414 /* a 2-word operand placed in bool exp */
#define RE_UB_OPRS_ERR      -415 /* UnBalanced OPeRatorS in bool exp    */
#define RE_DIALECT_ERR      -416 /* User requested an unavail dialect   */
#define RE_LANGUAGE_ERR     -417 /* User requested an unavail language  */

#define RE_TOCINDEX_ERR     -450 /* section index out of range for TOC  */
#define RE_DBNOTOC_ERR      -451 /* database has no TOC                 */
#define RE_TOCINI_ERR       -452 /* Error initializing TOC.             */
#define RE_TOCSCOP_ERR      -453 /* Error building TOC scope.           */
#define RE_TOCLOCKOUT_ERR   -454 /* Error building TOC scope-- lockouts.*/
#define RE_TOCADVLOG_ERR    -455 /* Error building TOC scope-- advanced logic  */

#define REAPI_STRTAB_ERR    -456 /* A failure in String Table lookup    */
#define REAPI_SEARCH_ERR    -457 /* A generic Boolean search error.     */
#define REAPI_PRON_ERR      -458 /* A generic pronunciation rtrv. error.*/
#define REAPI_ENTRY_ERR     -459 /* A generic entry retrieval error.    */
#define REAPI_NULLP_ERR     -460 /* A pointer to a structure is null.   */
#define REAPI_PRIMOP_ERR    -462 /* A bad primary operator was used.    */
#define REAPI_SECNDOP_ERR   -464 /* A bad secondary operator was used.  */
#define REAPI_WILDCH_ERR    -465 /* A generic Wildcard function error.  */
#define REAPI_VERIFY_ERR    -466 /* A generic Verification fnct error.  */
#define REAPI_TOCTOP_ERR    -467 /* Error finding the TOC topic.        */
#define REAPI_TOC_ERR       -468 /* A Generic Table of Contents op. Err.*/
#define REAPI_POS_ERR       -469 /* Error finding available p.o.s.      */
#define REAPI_TOCSET_ERR    -470 /* Error setting a new TOC filter.     */
#define REAPI_SERIAL_ERR    -471 /* Serial # in I/O doesn't match DB.   */
#define REAPI_INFLECT_ERR   -472 /* Inflection failed.                  */
#define REAPI_CLOMCH_ERR    -473 /* Headword closest match failed.      */
#define REAPI_IDXCLOMCH_ERR -474 /* IDX closest match failed.           */
#define REAPI_BUFF_OVER_ERR -476 /* Entry Buffer overflow               */

#define RE_PRS_INVALIDTABLE -480 /* Invalid Parsing Table               */
#define RE_PRS_INVALIDTOKEN -481 /* Invalid Token (Consumed)            */
#define RE_PRS_OVERFLOW     -482 /* Parsing Stack overflowed            */
#define RE_PRS_TOKENOVERFLOW -483 /* Too many Tokens                    */


#define RE_FNCT_INIT_ERR    -490 /* Function didn't initialize properly.*/
#define RE_FNCT_TERM_ERR    -492 /* Error in function termination.      */

#define RE_MAC_ERR          -500 /* A Macintosh specific error occurred.*/

#define RE_APP_OPEN_ERR     -550 /* Promlem opening an app. session.    */
#define RE_APP_CLOSE_ERR    -552 /* Problem closing an app. session.    */

#define RE_SF_GENERIC_ERR   -570 /* Generic Standard Function Error.    */
#define RE_SF_DECODE_ERR    -572 /* Error processing SFdecode.          */

#define RE_PD_ERR           -580 /* Generic PD error.          */

/* Header/RAM initialization errors
*/
#define RE_SFHDRCHK_ERR     -600 /* SF file header check failed          */
#define RE_PSHDRCHK_ERR     -610 /* Prod Spec. file header check failed  */
#define RE_LFHDRCHK_ERR     -620 /* Logical file header check failed     */
#define RE_LFTYPE_ERR       -630 /* Logical file type incorrect          */
#define RE_LFCONT_ERR       -640 /* Logical file type incorrect          */
#define RE_LFENCODVER_ERR   -650 /* Logical file encoding vers. too old. */
#define RE_NOMODLEXVER_ERR  -655 /* No model # found in lexver table     */
#define RE_LFLEXVER_ERR     -660 /* Logical file lexicon version too old */
#define RE_UNKN_LFTYPE_ERR  -670 /* Unknown logical file type            */
#define RE_ENTLF_INI_ERR    -680 /* Entry logical file ini failed        */
#define RE_ICSLF_INI_ERR    -690 /* ICS .IDX logical file ini failed     */
#define RE_EFATLF_INI_ERR   -700 /* EFAT logical file ini failed         */
#define RE_TABTABINI_ERR    -710 /* Error ini'ing table-table            */
#define RE_TABINI_ERR       -720 /* Error ini'ing table from table-table */
#define RE_STRTAB_INI_ERR   -730 /* Alloc/lock err during ini str tables */
#define RE_STSEP_FIND_ERR   -740 /* Can't find st_separator in comp tabs */
#define RE_INFL_CTO_ERR     -750 /* Inflect CTO'd out & DB is inflecting */
#define RE_LONG_LINE        -760 /* Line longer than specified in header */
#define RE_LONG_ENT         -770 /* Entry longer than specified in header */
#define RE_NO_TKNINI        -800 /* No missing RE_START call w/ RE_TOKEN */

#define RE_USERABORT        -900 /* Task aborted by user                 */

#ifdef RE_SF_PDICTIONARY  /* LRSV23 */
/* internal pdm errors 
*/
#define RE_PDMI_ALLOC_ERR            -2000
#define RE_PDMI_PDAPP_ERR            -2001
#define RE_PDMI_OPEN_ERR             -2002
#define RE_PDMI_HEADER_ERR           -2003
#define RE_PDMI_OPENSUBPD_ERR        -2004
#define RE_PDMI_TRANSFER_ERR         -2005
#define RE_PDMI_FULL_ERR             -2006
#define RE_PDMI_TMPNAM_ERR           -2007
#define RE_PDMI_SAVE_ERR             -2008
#define RE_PDMI_NOTSAVED_ERR         -2009
#define RE_PDMI_CLOSESUBPD_ERR       -2010
#define RE_PDMI_NOTRW_ERR            -2011
#define RE_PDMI_SAVESUBPD_ERR        -2012
#define RE_PDMI_PDMERGE_ERR          -2013
#define RE_PDMI_WAKE_ERR             -2014
#define RE_PDMI_SLEEP_ERR            -2015
#define RE_PDMI_CLOSE_ERR            -2016
#define RE_PDMI_INPUT_ERR            -2017
#define RE_DM_RW_ERR                 -2018
#define RE_PDMI_WRITE_ERR            -2019
#define RE_PDMI_MAXSTN_ERR           -2020
#define RE_PDMI_ADD_ERR              -2021
#define RE_PDMI_DELETE_ERR           -2022
#define RE_PDMI_EDIT_ERR             -2023
#define RE_PDMI_MODIFY_ERR           -2024
#define RE_PDMI_NOTFOUND_ERR         -2025
#define RE_PDMI_EDITSYNTAX_ERR       -2026
#define RE_PDMI_STN_ERR              -2027
#define RE_PDMI_PDID_ERR             -2028
#define RE_PDMI_ADDSYNS_ERR          -2029
#define RE_PDMI_DELSYNS_ERR          -2030
#define RE_PDMI_ADDMGNGROUP_ERR      -2031
#define RE_PDMI_DELMNGGROUP_ERR      -2032
#define RE_PDMI_MODMNGGROUP_ERR      -2033
#define RE_PDMI_INDEX_ERR            -2034
#define RE_PDMI_TYPE2GROUP_ERR       -2035
#define RE_PDMI_MODSTN_ERR           -2036
#define RE_PDMI_ACCESS_ERR           -2037
#define RE_PDMI_REVISESYN_ERR        -2038
#define RE_PDMI_REVISEMNG_ERR        -2039
#define RE_PDMI_SEARCH_ERR           -2040
#define RE_PDMI_ALTBUF_ERR           -2041
#define RE_PDMI_PDVERIFY_ERR         -2042
#define RE_PDMI_WALKSYNTAX_ERR       -2042
#define RE_PDMI_NOMEANING_ERR        -2043
#define RE_PDMI_UNKNOWNMEANING_ERR   -2044
#define RE_PDMI_HEADWORD_ERR         -2045
#define RE_PDMI_INCOMPATDB_ERR       -2046
#define RE_PDMI_DBVER_ERR            -2047
#define RE_PDMI_REVSYNDATA_ERR -2050 /* error revising the DSDS for PD edits */ 

/* re pd edittext syntax checker errors */                         
#define RE_NO_GRP_TAG_ERR   -2100 /* Type1 DBs require that GRP tag be first */
#define RE_TYPE2_ACTION_ERR -2101 /* Can't perform a type 1 action on a type2 db */
#define RE_NO_BUF_ERR       -2102 /* No edit buffer allocated, or it's empty */
#define RE_NO_FIELD_ERR     -2103 /* No field present after tag */
#define RE_TAG_ORDER_ERR    -2104 /* Tag occurs in the wrong order */
#define RE_MNG_GROUP_ERR    -2105 /* This tag not allowed in a meaning group */
#define RE_INVALID_TAG_ERR  -2106 /* This tag not allowed for this operation */
#define RE_NO_TAG_ERR       -2107 /* No tag present in the buffer */
#define RE_TAG_SIZE_ERR     -2108 /* The tag is longer than the maximum length */
#define RE_BAD_TAG_ERR      -2109 /* The tag doesn't match any known type */
#define RE_TAGBUF_ALLOC_ERR -2110 /* Allocation failure for the syntax checking buffer */

#define RE_PDM_VER_ERR      29999  

#endif /* RE_SF_PDICTIONARY */ /* LRSV23 */

/************************************************************************
*
* File Numbers returned in the RE_STATUS Structure.
*
*************************************************************************
*/

#define RENOFILE_FILENUM    0

#define READD_FILENUM       100
#define REALTERN_FILENUM    110
#define REANAGRM_FILENUM    120
#define REAPP_FILENUM       130
#define REAPPEXT_FILENUM    140
#define REAPPINI_FILENUM    150
#define REAPPMAN_FILENUM    160
#define REBISRCH_FILENUM    170
#define REBOLSCH_FILENUM    180
#define REBRUTE_FILENUM     190
#define REBRUTE8_FILENUM    200
#define REBSRCH_FILENUM     210
#define RECIRCLE_FILENUM    215
#define RECLOMCH_FILENUM    220
#define RECNVSPC_FILENUM    230
#define RECORREC_FILENUM    240
#define REDB_FILENUM        250
#define REDBEXIT_FILENUM    260
#define REDBINIT_FILENUM    270
#define REDBFILE_FILENUM    275
#define REDBMAN_FILENUM     280
#define REDECODE_FILENUM    290
#define REDECOMP_FILENUM    300
#define RETKNIZE_FILENUM    305
#define REENTLST_FILENUM    310
#define REENTRY_FILENUM     320
#define REERROR_FILENUM     330
#define REFLDALL_FILENUM    340
#define REFLDSEL_FILENUM    350
#define REGET_FILENUM       360
#define REGETDAT_FILENUM    370
#define REGETERR_FILENUM    380
#define REGETFLD_FILENUM    390
#define REGETINI_FILENUM    400
#define REGETPRT_FILENUM    410
#define REGETRWD_FILENUM    420
#define REGETSTP_FILENUM    430
#define REGLOBAL_FILENUM    440
#define REHDRCHK_FILENUM    445
#define REHRTINI_FILENUM    450
#define REHWDETN_FILENUM    460
#define REIDXLST_FILENUM    470
#define REIDXMCH_FILENUM    472
#define REINFLEC_FILENUM    475
#define REINTCHK_FILENUM    480
#define REINTGET_FILENUM    490
#define REINTRDB_FILENUM    500
#define REINTVDB_FILENUM    510
#define REINTVER_FILENUM    520
#define REISANAG_FILENUM    530
#define RELIB_FILENUM       540
#define REPARTSP_FILENUM    550
#define REPDDBFILE_FILENUM  555
#define REPERMUT_FILENUM    560
#define REPRON_FILENUM      570
#define REQBRUTE_FILENUM    580
#define REQUAD1_FILENUM     590
#define REQUAD2_FILENUM     600
#define REQUAD3_FILENUM     610
#define REQUAD6_FILENUM     620
#define REQUAD8_FILENUM     630
#define RERIGGED_FILENUM    640
#define RERLOCK_FILENUM     650
#define RERUNLOK_FILENUM    660
#define RESAMPLE_FILENUM    670
#define RESCHAUX_FILENUM    680
#define RESCHLST_FILENUM    690
#define RESEARCH_FILENUM    700
#define RESELFLD_FILENUM    710
#define RESIMILA_FILENUM    720
#define RESRTALT_FILENUM    730
#define RESTPRDB_FILENUM    740
#define RESTPVDB_FILENUM    750
#define RESTRTAB_FILENUM    755
#define RESUFCHK_FILENUM    760
#define RESUFMAT_FILENUM    770
#define RETOCABT_FILENUM    780
#define RETOCGET_FILENUM    790
#define RETOCINI_FILENUM    800
#define RETOCLST_FILENUM    810
#define RETOCSEL_FILENUM    820
#define RETOCSIZ_FILENUM    830
#define RETOCTOP_FILENUM    840
#define RETOKEN_FILENUM     842
#define RETKNGET_FILENUM    844
#define RETKNINI_FILENUM    846
#define RETKNSTP_FILENUM    848
#define REVERIFY_FILENUM    850
#define REVLOCK_FILENUM     860
#define REVUNLOK_FILENUM    870
#define REWILDCH_FILENUM    880
#define REETPAR_FILENUM     890
#define REETPAR1_FILENUM    900

#define REPDDB_FILENUM     2222
#define REPDMGR_FILENUM    2223
#define REPDEDIT_FILENUM   2224
#define REPDMGED_FILENUM   2225
#define REPDMGV_FILENUM    2226
#define REPDPARS_FILENUM   2300

#endif /* REERRORS_H */

/* End of file - REerrors.h
*/
