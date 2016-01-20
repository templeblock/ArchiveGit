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
 * RE.h - Main include file for the Reference Engine                        *
 *                                                                          *
 *--------------------------------------------------------------------------*

Description:
    This is the main include file for the Electronic Dictionary.  This
    file includes all files that are needed by the OEM.

Creation Date:
    17-jul-89   JGH

Change Record:
    Date        Author  Comment
    ---------   ------  ------- 
    08-nov-91   JCC     TOCHA support
    08-jan-92   ECH     RE_FN_ENTRY CTO.  ECH04
    21-feb-92   RAS         ED names -> RE
    02-apr-92   ECH     New API.  ECH05
    02-jun-92   ECH     New Common Decode (3rd. Generation).  ECH06

*---------------------------------------------------------------------------*/

#ifndef RE_H

#define RE_H

/* Define to activate the debug version of the standard library memory 
 * management macros. 
*/
#undef SL_MEM_DEBUG

#include "sllibhdr.h"
#include "sffun.h"

/* If the PD is defined then includ ehte PD library header.
*/
#ifdef RE_SF_PDICTIONARY
#include "pdlibhdr.h"
#endif

/* Specific includes for the Reference Engine.
*/
#include "reerrors.h"
#include "restatus.h"
#include "reoutmac.h"
#include "reapidef.h"
#include "reapityp.h"
#include "retpatyp.h"
#include "reapi.h"

#endif /* RE_H */

/* End of file - RE.h
*/
