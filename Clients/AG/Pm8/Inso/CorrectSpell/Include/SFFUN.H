/*----------------------------------------------------------------------*
*                                                                       *
*                   Copyright 1995 INSO Corporation                     *
*                         Common Code Library                           *
*                                                                       *
* This work is protected by United States and international copyright   *
* laws and contains certain trade secret information.  All use of this  *
* work is governed by the terms of agreements between INSO Corporation  *
* and its licensees or potential licensees.  Disclosure or disassembly  *
* of the contents of embodied programs or algorithms is prohibited.     *
* All rights are reserved worldwide.                                    *
*                                                                       *
*-----------------------------------------------------------------------*
 
*-----------------------------------------------------------------------*
*                                                                       *
*        SFfun.h - Standard Function Defines                            *
*                                                                       *
*-----------------------------------------------------------------------*
*                                                                       *
        Description:
        
        This header contains all function declarations for the Standard
        Functions. It also includes all other needed SF header files. 

        This header should be included in each products main header file
        immediately after SLlibhdr.h

        SF_BASE_CLITIC and SF_CLITIC_PROCESS: 
        For any product doing clitic processing, one of these must be defined.  
        Products with clitic information encoded in their wordlists must 
        define SF_BASE_CLITIC and SF_CLITIC_PROCESS.  Products containing 
        only a clitic hash table must define SF_BASE_CLITIC and need not
        define SF_CLITIC_PROCESS.

        Cautions:
        None.

        Creation Date:
        16-Apr-92   GJC     Ver. 1.0.0

        Change Record:
        02-jun-92   ECH     Moved defines to SFdef.h, Moved SL to SFhdr.h.
        02-feb-94   RLM     Fixed for IntelliScope

*-----------------------------------------------------------------------*/
#ifndef SFFUN_H

#define SFFUN_H



/*-------------- Start of OEM modifiable defines -----------------------*/

/* OEM should define all products here.  Alternatively, these can
   be defined on the command line. */

/* #define   SF_II   */    /* IntelliScope                  */
/* #define   SF_ICS  */    /* International CorrectSpell    */
/* #define   SF_RE   */    /* Reference Engine              */
/* #undef    SF_GCS  */    /* Grammar Correction System     */

/*-------------- End of OEM modifiable defines  ------------------------*/


#ifdef SF_II
#include "iifun.h"
#include "iifil.h"
#endif

#if defined(SF_ICS) || defined(SF_IPR)
#include "icfun.h"
#endif

#ifdef SF_RE
#include "refun.h"
#endif

#ifdef SF_GCS
#include "gcfun.h"
#endif

#ifdef SF_PD
#include "pdfun.h"
#endif

#ifdef SF_IN
#include "infun.h"
#endif

/* Standard function included files.
*/
#include "sfheader.h"
#include "sfdef.h"
#include "sftyp.h"
#include "sfclitic.h"
#include "sfcoldef.h"
#include "sfctrl.h"
#include "sfexthdr.h"

#endif /* SFFUN_H */

/* End of File - SFfun.h */
