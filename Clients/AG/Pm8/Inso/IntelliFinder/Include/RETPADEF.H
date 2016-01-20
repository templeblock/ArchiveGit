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
 * REtpadef.h                                                               *
 *                                                                          *
 *--------------------------------------------------------------------------*

Description:


Creation Date:
   6-26-1992     LT

Change Record:
   Date      Author      Comment

*---------------------------------------------------------------------------*/

#ifndef RETPADEF_H

#define RETPADEF_H

#define ET_STOP_ALL        -1    /* Free all pSynData structure in the  */
                                 /* IET parser I/O structure            */


#define ET_POS_MAX         50    /* Number of parts of speech           */

#define ET_CONMAC_MAX      8     /* Number of "content" macros          */


#define ET_MACLEN_MAX      7     /* Maximum length of IET macros        */

#define ET_ENUM_STR_MAXLEN 10    /* Maximum length of IET entry number  */
                                 /* string, e.g., "129", "3640", etc.   */

#define ASTERISK_STR       "*"

#define TM_STRLEN          3     /* Length of the following string      */
#define TM_STR             " TM" /* " TM" substitues "<TRADE>" for the  */
                                 /* time being; this will be changed    */
                                 /* when the master character table     */
                                 /* can have the trademark character    */

#define LPAREN_STR         "("

#define RPAREN_STR         ")"

#define ET_1TEST_DEFMEANING   1  /* First test for finding the default  */
                                 /* meaning                             */
#define ET_2TEST_DEFMEANING   2  /* Second test for finding the default */
#define ET_3TEST_DEFMEANING   3  /* Third test for finding the default  */

#endif /* RETPADEF_H */

/* End of file - REtpadef.h
*/
