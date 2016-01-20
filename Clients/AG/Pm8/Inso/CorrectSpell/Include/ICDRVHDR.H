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
*        ICdrvhdr.h - Needed to compile the sample implementation       *
*                                                                       *
*-----------------------------------------------------------------------*
*                                                                       *

        Description:
        This header file contains the includes for The International 
        CorrectSpell(TM) Software sample implementation.

        Returns:
        Nothing.

        Cautions:
        None.

*-----------------------------------------------------------------------*/
#ifndef ICDRVHDR_H
#define ICDRVHDR_H


/* Include the local demo header                                        */
#ifndef ICS_INTERNAL
#define ICS_INTERNAL
#endif

/* Include standard library header                                      */

#include "sllibhdr.h"

#include "icsapi.h"
/* Include the extern file for the modules that are called                */

#include "icsext.h"

#include "icdrv.h"

/* Include the extern file for the modules that are called                */
#include "icdrvext.h"

#endif /* ICDRVHDR_H */

/*  end of file - icdrvhdr.h  */
