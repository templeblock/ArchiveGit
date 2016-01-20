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
 * REtypdef.h - Defines for interface functions.                            *
 *                                                                          *
 *--------------------------------------------------------------------------*

Description:
   Typedefs defined for Reference Engine  interface functions.

Creation Date:
   5-26-1989    L Tang

Change Record:
    Date        Author  Comment

    17-may-91   JCC     added wildcard expression initialization template
    27-may-91   ECH     Made all structure pointers FAR for OEM Interface.
    09-jun-91   JCC     Any pointers in OEM structs make FAR JCC02
    08-nov-91   ECH     Added the filespec structure. ECH02.
    08-nov-91   JCC     TOCHA support
    21-feb-92	RAS	    ED names -> RE
    17-apr-92   ECH     Internal RE structures.  ECH05.

*---------------------------------------------------------------------------*/

#ifndef RETYPDEF_H

#define RETYPDEF_H

/* Boolean expressions, Internal structure..
*/
typedef struct RE_bool_ini
{
    PFCHAR      str;        /* a boolean expression string              */
    UCHAR       whole;      /* whole word or substring search           */
    UCHAR       cap;        /* Capitalization option                    */
}
RE_BOOL_INI,
FAR *PFRE_BOOL_INI;


/* Input wildcard expressions.
*/
typedef struct RE_wildexp_input
{
    PFUCHAR     str;        /* a wildcard expression string             */
    INT2B       listsize;   /* # bytes in outbuf for wildcard matches   */
                            /* max amt. of matches in each batch        */
    INT2B       rtnsPerBatch;
}
   RE_WILDEXP_INI,
   FAR *PFRE_WILDEXP_INI;


#endif /* RETYPDEF_H */

/* End of file - REtypdef.h
*/
