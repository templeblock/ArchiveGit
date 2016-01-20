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
 * REextern.h - External definitions for global variables used              *
 *              internally.                                                 *
 *                                                                          *
 *--------------------------------------------------------------------------*

Description:
    This file contains the external definitions for global variables
    used internally to the Reference Engine software.

Creation Date:
    31-may-89   L Tang

Change Record:
    Date        Author  Comment

    14-jun-90   JCC     Removed REformat structure
    02-jul-90   JCC     Added p.o.s. arrays, db version array
    07-jul-90   JCC     db names array, db names counter
    03-dec-90   JCC     ifdef Boolean search
    05-feb-90   JCC     internal wkbuf part of RE_GETINFO struct, is alloc'd
    26-mar-91   JCC     nixed the global RE_get_info_ptr
    29-oct-91   ECH     The Database Manager has replaced many globals. ECH02
    21-feb-92   RAS     ED names -> RE
    22-jun-92   JCC     Replaced db_lowest with gEncodVer & gLexVer
    14-aug-92   JCC     gLexVer renamed to gLFVer

*---------------------------------------------------------------------------*/

#ifndef REEXTERN_H

#define REEXTERN_H

SLEXTERN  UCHAR   bits[8];          /* multipurpose bit masks w/in byte */

#endif /* REEXTERN_H */

/* End of file - REextern.h
*/
