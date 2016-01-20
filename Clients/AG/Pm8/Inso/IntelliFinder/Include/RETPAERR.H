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
 * REtpaerr.h                                                               *
 *                                                                          *
 *--------------------------------------------------------------------------*

Description:
   This file includes error codes returned from the RE IET parsers.
   Generic parser-returned error codes can be found in REERRORS.H.

Creation Date:
   6-18-1992      LT

Change Record:
   Date      Author      Comment

*---------------------------------------------------------------------------*/

#ifndef RETPAERR_H

#define RETPAERR_H

/* Errors   */

#define  REET_LANG_ERR      -800  /* Wrong language code for the IET parsers*/


/* Warnings */

#define  REET_UNPARSED_WARN -900  /* Some or the whole input data is not    */
                                  /* parsed; the current input entry text   */
                                  /* parsing is not completed because some  */
                                  /* or the whole input data is not         */
                                  /* terminated with a newline character and*/
                                  /* not the end of entry text; the         */
                                  /* remaining unparsed data is saved in    */
                                  /* pPrevUnparsed of the synonym header    */
                                  /* structure;                             */
                                  /* Call REentry() with RE_FWD_PAGE for the*/
                                  /* next segment of entry text and call the*/
                                  /* parser again                           */

#endif /* RETPAERR_H */

/* End of file - REtpaerr.h
*/
