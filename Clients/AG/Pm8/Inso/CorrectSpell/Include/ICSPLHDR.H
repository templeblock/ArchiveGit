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
*    Name:                                                              *
*        ICsplhdr.h - Header for the spell module                       *
*                                                                       *
*-----------------------------------------------------------------------*

     Description:                              
    Include files for the International CorrectSpell(TM) Software.
                                      
     External Data:                             
         NONE                             
                                      
     Called by Modules:                             
         User Application                     
                                      
     Calls Modules:                             
                                          
     Returns:                             
                                          
     Cautions:    
    NONE                             
                                          
*----------------------------------------------------------------------*/
#ifndef ICSPLHDR_H
#define ICSPLHDR_H

#if defined(WINDOWS) || defined(WIN32)

/* Constant used in slwindef.h to stub out unnecessary windows.h info. */
#define SL_INTERNAL 
#endif

/* Standard Libaray Include      */

#include "sllibhdr.h"

/* Standard Function include  */
#include "sffun.h"

/* International CorrectSpell(TM) include */

#include "ic.h"

/* International CorrectSpell(TM) include */

#include "iclngbuf.h"
#include "icctlbuf.h"
#include "icbuf.h"

/* Funtion Declarations     */

#include "icsplext.h"

#endif /* ICSPLHDR_H */

/* End of file icsplhdr.h */
