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
*        ICSAPI.H - Standard Function Defines for the                   *
*                   INSO Corporation Software functionality             *
*                                                                       *
*-----------------------------------------------------------------------*
*                                                                       *
        Description:
        
        This header contains all header files needed by the OEMs.

        Cautions:
        None.

*-----------------------------------------------------------------------*/
#ifndef ICSAPI_H
#define ICSAPI_H

/* Include base level library header                                        */

#include "sffun.h"

/* Include Global International CorrectSpell(TM) Module Header                */

#include "ic.h"

#include "iclngbuf.h"
#ifdef ICS_INTERNAL
#include "icctlbuf.h"
#endif
#include "icbuf.h"

#ifdef ICS_API_PD
#ifdef ICS_PDICTIONARY
#include "icpd.h"
#endif
#endif

#endif /* ICSAPI_H */
/*  End of File - ICSAPI.H */
