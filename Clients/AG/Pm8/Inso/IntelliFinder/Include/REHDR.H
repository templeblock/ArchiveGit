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
 * REhdr.h - Main internal include file for the Reference Engine.           *
 *                                                                          *
 *--------------------------------------------------------------------------*

Description:
   This is the main internal include file for the Reference Engine.
   This file includes all header files that are needed by the internal
   RE functions.  The "include tree" is shown below.

   REhdr.h
    |
    |___ RE.h
    |     |
    |     |___SLlibhdr.h
    |     |    |
    |     |    |___SL*.h
    |     |
    |     |___SFfun.h
    |     |    |
    |     |    |___REfun.h
    |     |    |    |
    |     |    |    |_REsubf.h
    |     |    |
    |     |    |___XXfun.h (If built with another INSO product XX.) 
    |     |    
    |     |___PDlibhdr.h
    |     |    
    |     |___REerrors.h
    |     |___REstatus.h
    |     |___REoutmac.h
    |     |___REapidef.h
    |     |___REapityp.h
    |     |___REapi.h
    |
    |___REdefine.h
    |___REtoc.h
    |___REfields.h
    |___RElineid.h
    |___REmaccvt.h
    |___REmacros.h
    |___REdbhead.h
    |___REdecomp.h
    |___REextern.h
    |___REctrl.h
    |___REpdctrl.h
    |___REpdmhdr.h
    |___REdmhdr.h
    |___REamhdr.h
    |___REintfun.h
    |___REtypdef.h

Creation Date:
    17-jul-89   JGH

Change Record:
    Date        Author  Comment

    06-sep-90   JCC     Added field selection
    05-feb-91   JCC     merged eddattyp.h w/edrtvdat.h
    29-oct-91   ECH     Added the database manager.  ECH02
    13-nov-91   JCC     removed edspell.h, eddbbufs.h
    21-jan-92   JCC     Added SL_INTERNAL define (to switch on SLWINDEF.H)
    21-feb-92	RAS	    ED names -> RE
    27-apr-92   ECH     New API.  ECH05
    02-jun-92   ECH     New Common Decode (3rd Generation).  ECH06

*---------------------------------------------------------------------------*/

#ifndef REHDR_H

#define REHDR_H


/* Alert lower-level include files that an internal file is being compiled.
*/
#define SL_INTERNAL

/* Include all header files needed by the internal RE software.
*/

#include "re.h"             /* includes external headers & SL               */
#include "redefine.h"
#include "retoc.h" 
#include "refields.h"
#include "relineid.h"
#include "remaccvt.h"
#include "remacros.h"
#include "redbhead.h"
#include "redecomp.h"
#include "reextern.h"
#include "rectrl.h"
#ifdef RE_SF_PDICTIONARY
#include "repdctrl.h"
#include "repdmhdr.h"
#endif
#include "redmhdr.h"
#include "reamhdr.h"
#include "reintfun.h"
#include "retypdef.h"

#endif /* REHDR_H */
/* End of file - REhdr.h
*/
