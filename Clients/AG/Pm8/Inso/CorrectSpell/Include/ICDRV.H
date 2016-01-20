/****************************************************************************
 *                   Copyright  1995  INSO Corporation                      *
 *                International CorrectSpell[TM] Software                   *
 *                                                                          *
 * This work is protected by United States and international copyright laws *
 * and contains certain trade secret information.  All use of this work is  *
 * governed by the terms of agreements between INSO Corporation and its     *
 * licensees or potential licensees.  Disclosure or disassembly of the      *
 * contents of embodied programs or algorithms is prohibited.  All rights   *
 * are reserved worldwide.                                                  *
 ****************************************************************************

*-----------------------------------------------------------------------*
*                                                                       *
*   ICdrv.h - Sample implementation typedefs and defines                *
*                                                                       *
*-----------------------------------------------------------------------*
*                                                                       *
   Description:
   This header contains defines for the International 
   CorrectSpell(TM) Software sample implementation.

   Cautions:
   None.

*-----------------------------------------------------------------------*/
#ifndef ICDRV_H
#define ICDRV_H

/* Max size of the message buffer               */

#define IC_MAX_MESG   128

/* File name delimiter */

/* Pathname delimiter to be used for opening data files. 
*/
#if (defined MSDOS || defined OS2 || defined WINDOWS)
#define DELIMITER  BCKSLASH
#endif

#if  (defined MAC	 || defined POWERMAC)
#define DELIMITER  COLON
#endif

#ifdef  UNIX
#define DELIMITER  SLASH
#endif

#ifdef  VAX
#define DELIMITER  RBRACK
#endif

/* Max number of dictionaries for one language */

#define   MAX_DICTS   7

/* File input configuration buffer               */

typedef char * ICCONFG;

/* Exit Sample Implementation                    */
#define ICEXIT         100

#endif /* ICDRV_H */

/* End of file ICdrv.h */
