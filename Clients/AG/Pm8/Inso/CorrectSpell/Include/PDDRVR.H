/***************************************************************************
*                   Copyright 1995 INSO Corporation                        *
*                Personal Dictionary Component Software                    *
*                                                                          *
* This work is protected by United States and international copyright laws *
* and contains certain trade secret information.  All use of this work is  *
* governed by the terms of agreements between INSO Corporation and its     *
* licensees or potential licensees.  Disclosure or disassembly of the      *
* contents of embodied programs or algorithms is prohibited.  All rights   *
* are reserved worldwide.                                                  *
****************************************************************************
 
*--------------------------------------------------------------------------*
*                                                                          *
*    PDdrvr.h - contains defines for File Compressor Utility               *
*                                                                          *
*--------------------------------------------------------------------------*

    Description:
    Creation Date:

*--------------------------------------------------------------------------*/

#ifndef PDDRVR_H
#define PDDRVR_H

/* File name delimiter */
/* Pathname delimiter to be used for opening data files */

#if (defined MSDOS || defined OS2 || defined WINDOWS || defined WIN32)
#define DELIMITER  BCKSLASH
#endif

#if (defined MAC || defined POWERMAC)
#define DELIMITER  COLON
#endif

#ifdef  UNIX
#define DELIMITER  SLASH
#endif

#ifdef  VAX
#define DELIMITER  RBRACK
#endif

#endif /* PDDRVR_H */

/* End of file - PDdrvr.h */
