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
 * RElineid.h - Line indentifications.                                      *
 *                                                                          *
 *--------------------------------------------------------------------------*

Description:


Creation Date:
    5-31-1989   L Tang

Change Record:
    Date    Author      Comment
    21-feb-92	RAS	ED names -> RE

*---------------------------------------------------------------------------*/

#ifndef RELINEID_H

#define RELINEID_H

/*
 * Refer to Format Section in Engineering Design Specifications
 */
                                /* Typesetting code for format */
#define  BOLD            1
#define  ITALIC          2
#define  BOLD_ITALIC     3

                                 /* Line identifiers:           */
#define BOLD_LI         "BD"    /* bold; following word(s) in bold  */
#define ITALIC_LI       "IT"    /* italic; following word(s) in italic  */
#define ROMAN_LI        "RM"    /* roman; following word(s) in roman    */


#define ENTRY_E_LI      "EE"    /* end of entry             */

#define PRO_S_LI        "PR"    /* start of pronunciation       */
#define PRO_E_LI        "EP"    /* end of pronunciation         */
#define DEF_S_LI        "DF"    /* start of definition          */
#define DEFNO_S_LI      "NM"    /* start of a numbered definition */
#define DEF_E_LI        "ED"    /* end of definition            */

#define ETY_S_LI        "EY"    /* start of etymology */
#define ETY_E_LI        "EN"    /* end of etymology */

#define SYM_S_LI        "SY"    /* synonym paragraph */
#define SYM_E_LI        "ES"    /* end of synonym paragraph     */

#define USNOTE_S_LI     "UN"    /* usage note */

#define USNOTE_E_LI     "EU"    /* end of usage note            */ 

#define VARIANT_LI      "AV"    /* variant text */

#define IDIOM_LI        "ID"    /* idiom */

#define PHRASALVERB_LI  "PV"    /* phrasal verb; emdash followed by     */

#define SEEUSAGE_LI     "SU"    /* see-usage comment */

#define RUNON_LI        "RO"    /* run-on   */

#define PRIMARYHW_LI    "WD"    /* primary headword */
#define VARIANTHW_LI    "AL"    /* primary variant headword */
#define VARHW_LI        "LA"    /* primary variant headword */
#define SECONDARYHW_LI  "MA"    /* secondary variant headword */

#define DEFVARHW_LI     "RF"    /* definition-specific form variant */

#define INFLECTION_LI   "IF"    /* verb inflection */
#define INFLEMMA_LI     "IL"    /* variant verb lemma */
#define INFVARIANT_LI   "IV"    /* variant verb inflection */

#define GRAMMARCMT_LI   "GM"    /* grammar comment */

#define FIELD_LI        "FT"    /* field displine label */

#define GEOGRAPHIC_LI   "UG"    /* geographic label */
#define USAGE_LI        "US"    /* usage label */

#define CITATION_LI     "CT"    /* citation/example */

#define LETTER_LI       "LT"    /* start of each of 26 letters      */
#define REVERSION_LI    "FL"    /* entry classfier; used for data   */
                                 /* management only         */

#define POS_LI          "PS"    /* part of speech */

#define SYMBOL_LI       "SB"    /* "Symbol " */

#define PLURALNOUN_LI   "PL"    /* plural noun */
#define PLNOUN_2LI      "PF"    /* plural noun */
#define VARPLURALNOUN__LI "PA"  /* variant plural noun */
#define SINGULARNOUN_LI "SI"    /* singular noun */

#endif /* RELINEID_H */
/* End of file - RElineid.h
*/
