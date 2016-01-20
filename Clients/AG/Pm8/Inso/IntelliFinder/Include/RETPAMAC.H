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
 * REtpamac.h                                                               *
 *                                                                          *
 *--------------------------------------------------------------------------*

Description:
   This is a list of IET macro type numbers and macro strings.

Creation Date:
   6-26-1992     LT

Change Record:
   Date      Author      Comment

*---------------------------------------------------------------------------*/

#ifndef RETPAMAC_H

#define RETPAMAC_H

/* IET macro types                                                         */

#define  ET_DUMMY_MAC       0 

#define  ET_UNKNOWN_MAC    -1 /* Input string is not a valid IET macro     */

#define  ET_B_MAC          -2 /* </BB>, <BB>                               */
#define  ET_BB_MAC         -3 /* <BB>                                      */
#define  ET_SL_BB_MAC      -4 /* </BB>                                     */

#define  ET_H_MAC          -5 /* </HY>, <HR>, <HY>                         */
#define  ET_HR_MAC         -6 /* <HR>                                      */
#define  ET_HY_MAC         -7 /* <HY>                                      */
#define  ET_SL_HY_MAC      -8 /* </HY>                                     */

#define  ET_I_MAC          -9 /* </ID>, <ID>, <IL>                         */
#define  ET_ID_MAC         -10/* <ID>                                      */
#define  ET_SL_ID_MAC      -11/* </ID>                                     */
#define  ET_IL_MAC         -12/* <IL>                                      */

#define  ET_S_MAC          -13/* </SY>, <SE>, <SY>                         */
#define  ET_SE_MAC         -14/* <SE>                                      */
#define  ET_SY_MAC         -15/* <SY>                                      */
#define  ET_SL_SY_MAC      -16/* </SY>                                     */

#define  ET_A_MAC          -17/* <AL>, <AN>                                */
#define  ET_AL_MAC         -18/* <AL>                                      */
#define  ET_AN_MAC         -19/* <AN>                                      */

#define  ET_C_MAC          -20/* <CR>, <CT>, <CX>                          */
#define  ET_CR_MAC         -21/* <CR>                                      */
#define  ET_CT_MAC         -22/* <CT>                                      */
#define  ET_CX_MAC         -23/* <CX>                                      */

#define  ET_D_MAC          -24/* <D1>, <D2>, <DF>, <DS>                    */
#define  ET_D1_MAC         -25/* <D1>                                      */
#define  ET_D2_MAC         -26/* <D2>                                      */
#define  ET_DF_MAC         -27/* <DF>,                                     */
#define  ET_DS_MAC         -28/* <DS>                                      */

/* <FT>        field type label (e.g., biology, law, etc.)                       */
#define  ET_FT_MAC         -29/* <FT>                                      */

#define  ET_G_MAC          -30/* <GL>, <GM>                                */
#define  ET_GL_MAC         -31/* <GL>                                      */
#define  ET_GM_MAC         -32/* <GM>                                      */

/* <PS>        opens the part of speech field which contains an integer          */
#define  ET_PS_MAC         -33/* <PS>                                      */

/* <RW>        opens a field for related words--words that are similar in meaning
         to headword but not synonyms and not necessarily of the same part of
         speech (RWs may have embedded <IL> entry numbers if the headword
         identified points to more than one entry number in the index)     */
#define  ET_RW_MAC         -34/* <RW>                                      */

#define  ET_U_MAC          -35/* <UG>, <UN>, <UP>, <US>                    */
#define  ET_UG_MAC         -36/* <UG>                                      */
#define  ET_UN_MAC         -37/* <UN>                                      */
#define  ET_UP_MAC         -38/* <UP>                                      */
#define  ET_US_MAC         -39/* <US>                                      */

/* <WD>        identifies the headword                                           */
#define  ET_WD_MAC         -40/* <WD>                                      */

#define  ET_E_MAC          -41/* <E0>, </E0>                               */
#define  ET_E0_MAC         -42/* <E0>                                      */
#define  ET_SL_E0_MAC      -43/* </E0>                                     */

/* <TRADE>        the preceding string is a trademark; this represents the trademark
            sign                                                           */
#define  ET_TR_MAC         -44/* <TRADE>                                   */

/* <{sz}>        English ise-ize macro; this represents either 's' or 'z' depending
            on the dialect code                                            */
#define  ET_DI_MAC         -45/* <{sz}>                                    */





/* IET macro strings                                                       */

#define  ET_BB_END   "</BB>" /* </BB>        closes a field for the bibliographical
                                       reference of a citation             */
                              

                              
                              
#define  ET_BB_BEG   "<BB>"  /* <BB>        opens a field for the bibliographical        
                                       reference of a citation             */
                              
#define  ET_HY_BEG   "<HY>"  /* <HY>        opens a list of hyponyms (oak, maple,
                                       spruce are hyponyms of tree)        */
                              
#define  ET_HR_BEG   "<HR>"  /* <HR>        opens the hypernym field (tree is the
                                       hypernym of oak) (HRs may have embedded
                                       <IL> entry numbers if the headword
                                       identified points to more than one
                                       entry number in the index) (currently
                                       Dutch only)                         */

#define  ET_HY_END   "</HY>" /* </HY>        closes a list of hyponyms           */
                              
#define  ET_ID_END   "</ID>" /* </ID>        closes a list of phrasal synonyms   */
                              
#define  ET_ID_BEG   "<ID>"  /* <ID>        opens a list of phrasal synonyms    */
                              
#define  ET_IL_BEG   "<IL>"  /* <IL>        opens the field for an embedded entry
                                       number in a <CR>, <RW>, <HR>, or <SE>
                                       field                               */
                              
#define  ET_SY_END   "</SY>" /* </SY>        closes a list of synonyms           */

#define  ET_SE_BEG   "<SE>"  /* <SE>        identifies a synonym that is itself a
                                       main entry with additional information
                                       (usually a usage note or a citation--
                                       currently Spanish only) (SEs may have
                                       embedded <IL> entry numbers if the
                                       headword identified points to more
                                       than one entry number in the index) */
                              
#define  ET_SY_BEG   "<SY>"  /* <SY>        opens a list of synonyms            */
                              
#define  ET_AL_BEG   "<AL>"  /* <AL>        variant spelling of the preceding     
                                       headword                            */
                              
#define  ET_AN_BEG   "<AN>"  /* <AN>        antonym(s) of preceding synonyms or of
                                       headword                            */
                              
#define  ET_CR_BEG   "<CR>"  /* <CR>        cross reference to another entry in
                                       thesaurus; material in the other entry
                                       is directly relevant to the current
                                       entry (CRs may have embedded <IL>
                                       entry numbers if the headword
                                       identified points to more than one
                                       entry number in the index)          */
                              
#define  ET_CT_BEG   "<CT>"  /* <CT>        opens a citation (currently Spanish
                                       only)                               */
                              
#define  ET_CX_BEG   "<CX>"  /* <CX>        opens a field for a phrase
                                       distinguishing one homograph from
                                       another (currently Dutch only)      */
                              
#define  ET_D1_BEG   "<D1>"  /* <D1>        delimits discrete elements within a
                                       single field                        */
                              
                             
#define  ET_DF_BEG   "<DF>"  /* <DF>        definition of following list of
                                       synonyms, hyponyms, etc. when not
                                       within a list of synonyms, hyponyms,
                                       etc.;
                                       definition of preceding element when
                                       within a list of synonyms, hyponyms,
                                       etc.                                */

#define  ET_DS_BEG   "<DS>"  /* <DS>        identifies the definition that takes
                                       the place of a headword in the English
                                       thesaurus                           */
                              
#define  ET_GL_BEG   "<GL>"  /* <GL>        phrasal expression including an
                                       inflection of the headword          */
                              
#define  ET_GM_BEG   "<GM>"  /* <GM>        grammatical usage label (e.g.,     
                                       feminine only, usually plural)      */
                              
#define  ET_UG_BEG   "<UG>"  /* <UG>        geographical usage label (e.g., Irish,
                                       Canadian)                           */
                              
#define  ET_UN_BEG   "<UN>"  /* <UN>        usage note that explains subtle
                                       differences in meaning or usage
                                       within a list of synonyms. (currently
                                       Spanish only)                       */
                                 
#define  ET_UP_BEG   "<UP>"  /* <UP>        homograph number. Homographs are two
                                       words that are spelled the same way.
                                       In our headword-oriented thesaurusus,
                                       if a word has two different parts of
                                       speech, each one is a separate entry.
                                       Consequently we often have more than
                                       one entry with identically-spelled
                                       headwords.  These are distinguished
                                       by homograph numbers.               */
                              
#define  ET_US_BEG   "<US>"  /* <US>        usage label (e.g., colloquial,      
                                       vulgar, archaic)                    */


                              
#define  ET_E0_BEG   "<E0>"   /* <E0>           represents an opening single
                                          quotation mark; indicating the
                                          beginning of italic text         */
                              
#define  ET_E0_END   "</E0>"  /* </E0>    represents a closing single 
                                          quoatation mark; indicating the 
                                          end of italic text               */

#define  ET_D2_CHR   "<D2>"   /* <D2>     represents a comma within a      
                                          terminal node                    */

#define  ET_TR_CHR   "<TRADE>"/* <TRADE>  the preceding string is a          
                                          trademark; this represents the     
                                          trademark sign                   */

#define  ET_DI_CHR   "<{sz}>" /* <{sz}>   English ise-ize macro; this        
                                          represents either 's' or 'z'       
                                          depending on the dialect code    */


#endif /* RETPAMAC_H */

/* End of file - REtpamac.h
*/
