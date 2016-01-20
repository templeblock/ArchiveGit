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
 *  REstatus.h - Status defines.                                            *
 *                                                                          *
 *--------------------------------------------------------------------------*

Description:
   This is the include file for the status returned by REgetdat() function.

Creation Date:
   9-29-1989   LT

Change Record:
   Date Author      Comment
    21-feb-92	RAS	ED names -> RE

*---------------------------------------------------------------------------*/

#ifndef RESTATUS_H

#define RESTATUS_H

#define  RE_OK             0    /* No error and no other special status */

#define  RE_NOT_OK         1    /* Error is found, check RE_error */

#define  RE_MORE_DATA      2    /* More data in the dictionary but the */
                                /* returned data has already met the   */
                                /* requested limit                     */

#define  RE_NO_MORE_DATA   3    /* No more data can be retrieved */
                                /* from the dictionary           */

#endif /* RESTATUS_H */

/* End of file - REstatus.h
*/
