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
 * REmacros.h - Defines for typesetting macros.                             *
 *                                                                          *
 *--------------------------------------------------------------------------*

Description:

Creation Date:
    5-31-1989   L Tang

Change Record:
    Date    Author      Comment
    21-feb-92	RAS	ED names -> RE

*---------------------------------------------------------------------------*/
                                /* Macros for typesetting:     */
#ifndef REMACROS_H

#define REMACROS_H

#define E_UPLTBD_M   "<#1>"    /* end of uppercase light bold */
#define S_UPLTBD_M   "<#2>"    /* following character(s) in */
                                /* uppercase light bold        */


#define LOE_IT_M     "<ITALE>"  /* 'e' in italic;       */

#define LOTH_IT_M    "<ITTH>"   /* italic th */

#define LOAELIG_IT_M "<ITAELIG>"/* italic ae ligature */



                                /* Macros which can be displayed in */
                                /* standard ASCII:                  */
#define ATSIGN_M     "<ATSIGN>" /* '@'              */
#define PLUS_M       "<+>"     /* '+'               */
#define EQUAL_M      "<=>"     /* '='               */
#define EMDASH_M     "<EM>"    /* "--"; substitute for emdash   */
#define ENDASH_M     "<EN>"    /* '-'; substitute for endash    */
#define GREATER_M    "<GTR>"       /* '>'               */
#define COMMA_M      "<IC>"    /* ','               */
#define COLON_M      "<IN>"    /* ':'               */
#define PERIOD_M     "<IP>"    /* '.'               */
#define LBRACE_M     "<LBRACE>" /* '{'              */
#define RBRACE_M     "<RBRACE>" /* '}'              */

                                /* REformat() does not convert the following*/
                                /* two macros to the standard ASCII for the */
                                /* convenience of OEM's conversion of the   */
                                /* output macros; however, it changes LS_M  */
                                /* to LESS_M, so OEM can only find LESS_M   */
                                /* in the output buffer for '<'             */
#define LESS_M       "<LS>"     /* '<'              */
#define LS_M         "<LESS>"   /* '<'              */

#define NUM_M        "<SP>"    /* "#"               */
#define VRULE_M      "<VRULE>"  /* '|'              */
#define MULTI_SIGN_M "<X>"     /* '*'; substitute for multiplication sign */




/* 
 * BSD should provide a table of character codes for the following macros;
 * see  ???.h 
 */

                                 /* Macros which can be displayed in */
                                 /* DEC extended character set:      */
#define BULLET_M    "<BULL>"     /* bullet          */
#define OPERATOR_M  "<OPER>"     /* a mathmatical operator; */
                                 /* bullet is a substitute  */
#define DEGREE_M    "<DEG>"     /* degree sign          */

#define PARAGRAPH_M  "<PARMRK>"  /* paragraph mark      */
#define SECTION_M    "<SECT>"    /* section mark            */


                                 /* Macros which may be displayed in DEC */
                                 /* extended character set depending upon*/
                                 /* the preceding character(s):          */
                                 /* acute accent            */
                                 /* 'A', 'E', 'I', 'O', 'U', 'a',*/
                                 /* 'e', 'i', 'o', 'u'      */
#define UPA_ACUTE_M  "A<ACUTE>"
#define UPE_ACUTE_M  "E<ACUTE>"
#define UPI_ACUTE_M  "I<ACUTE>"
#define UPO_ACUTE_M  "O<ACUTE>"
#define UPU_ACUTE_M  "U<ACUTE>"
#define LOA_ACUTE_M  "a<ACUTE>"
#define LOE_ACUTE_M  "e<ACUTE>"
#define LOI_ACUTE_M  "i<ACUTE>"
#define LOO_ACUTE_M  "o<ACUTE>"
#define LOU_ACUTE_M  "u<ACUTE>"
                                 /* grave accent            */
                                 /* 'A', 'E', 'I', 'O', 'U', 'a',*/
                                 /* 'e', 'i', 'o', 'u'      */
#define UPA_GRAVE_M  "A<GRAVE>"
#define UPE_GRAVE_M  "E<GRAVE>"
#define UPI_GRAVE_M  "I<GRAVE>"
#define UPO_GRAVE_M  "O<GRAVE>"
#define UPU_GRAVE_M  "U<GRAVE>"
#define LOA_GRAVE_M  "a<GRAVE>"
#define LOE_GRAVE_M  "e<GRAVE>"
#define LOI_GRAVE_M  "i<GRAVE>"
#define LOO_GRAVE_M  "o<GRAVE>"
#define LOU_GRAVE_M  "u<GRAVE>"
                                /* circumflex accent        */
                                 /* 'A', 'E', 'I', 'O', 'U', 'a',*/
                                 /* 'e', 'i', 'o', 'u'      */
#define UPA_CIRCM_M  "A<CIRC>"
#define UPE_CIRCM_M  "E<CIRC>"
#define UPI_CIRCM_M  "I<CIRC>"
#define UPO_CIRCM_M  "O<CIRC>"
#define UPU_CIRCM_M  "U<CIRC>"
#define LOA_CIRCM_M  "a<CIRC>"
#define LOE_CIRCM_M  "e<CIRC>"
#define LOI_CIRCM_M  "i<CIRC>"
#define LOO_CIRCM_M  "o<CIRC>"
#define LOU_CIRCM_M  "u<CIRC>"
                                  /* tilde accent           */
                                 /* 'A', 'N', 'O', 'a', 'n', 'o'    */
#define UPA_TILDE_M  "A<TILDE>"
#define UPN_TILDE_M  "N<TILDE>"
#define UPO_TILDE_M  "O<TILDE>"
#define LOA_TILDE_M  "a<TILDE>"
#define LON_TILDE_M  "n<TILDE>"
#define LOO_TILDE_M  "o<TILDE>"
                                 /* umlaut accent       */
                                 /* 'A', 'E', 'I', 'O', 'U', 'Y',*/
                                 /* 'a', 'e', 'i', 'o', 'u', 'y'    */
#define UPA_UMLAUT_M "A<UML>"
#define UPE_UMLAUT_M "E<UML>"
#define UPI_UMLAUT_M "I<UML>"
#define UPO_UMLAUT_M "O<UML>"
#define UPU_UMLAUT_M "U<UML>"
#define UPY_UMLAUT_M "Y<UML>"
#define LOA_UMLAUT_M "a<UML>"
#define LOE_UMLAUT_M "e<UML>"
#define LOI_UMLAUT_M "i<UML>"
#define LOO_UMLAUT_M "o<UML>"
#define LOU_UMLAUT_M "u<UML>"
#define LOY_UMLAUT_M "y<UML>"
                                 /* ring center over character  */
                                 /* 'A', 'a'            */
#define UPA_RING_ABV_M "A<RING>"
#define LOA_RING_ABV_M "a<RING>"
                                 /* cedilla         */
                                 /* 'C', 'c'            */
#define UPC_CEDILLA_M   "C<CEDILL>"
#define LOC_CEDILLA_M   "c<CEDILL>"
                              /* ligature;          */
                                 /* "AE", "ae", "OE", and "oe"  */
#define UPAE_LIG_M   "AE<LIG>"
#define UPOE_LIG_M   "OE<LIG>" 
#define LOAE_LIG_M   "ae<LIG>"
#define LOOE_LIG_M   "oe<LIG>"
                                 /* slash through preceding vowel*/
                                 /* 'O', 'o'            */
#define UPO_SLASH_M  "O<SLASH>"
#define LOO_SLASH_M  "o<SLASH>"


#define ACUTE_M      "<ACUTE>"
#define GRAVE_M      "<GRAVE>"
#define CIRCM_M      "<CIRC>"
#define TILDE_M      "<TILDE>"
#define UMLAUT_M     "<UML>"
#define RING_ABV_M   "<RING>"
#define CEDILLA_M    "<CEDILL>"
#define LIG_M        "<LIG>"
#define SLASH_M      "<SLASH>"

#define  DBO_MACRON_M "<OOMACRON>"
#define  DBO_MAC_M    "<OOMACR>" /* A database error */
#define  DELT_M       "<DELT>" 
#define  UCDELTA_M    "<UCDELTA>"/* A database error */
#define  EPS_M        "<EPS>"
#define  EPSILON_M    "<EPSILON>"/* A database error */
#define E_NOK_UPLTBD_M "<#++1>" /* A database error */

#endif /* REMACROS_H */
/* End of file - REmacros.h
*/
