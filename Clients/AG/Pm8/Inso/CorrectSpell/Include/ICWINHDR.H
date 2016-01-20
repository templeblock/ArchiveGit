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
*    ICwinhdr.h - Needed to compile the Window application and demo     *
*                                                                       *
*-----------------------------------------------------------------------*
*                                                                       *

    Description:
    This header file contains the includes for The Windows version
    of International CorrectSpell(TM).

    Returns:
    Nothing.

    Cautions:
    None.

*-----------------------------------------------------------------------*/
#ifndef ICWINHDR_H
#define ICWINHDR_H

#if defined(WINDOWS) || defined(WIN32)

/* Ignore a few groups of references in windows.h                */

#define NOATOM           /* Atom Manager routines                */
#define NOKANJI          /* Kanji support stuff.                 */
#define NOMETAFILE       /* typedef METAFILEPICT                 */
#define NORASTEROPS      /* binary and tertiary raster ops       */
#define NOSOUND          /* Sound driver routines                */

#define NOFONT           /* typedef FONT and associated routines */
#define NOPEN            /* typedef HPEN and associated routines */

#define NOGDICAPMASKS    /* No cap masks                         */
#define NOWH             /* No Windows hook                      */
#define NOPROFILER       /* No profile routines                  */
#define NOHELP
#define NOMDI
#define NOLANGUAGE
#define NOPROFILER
#define NOLOGERROR
#define NOSCALABLEFONT

#endif

#define ICS_INTERNAL

/* Include specific SL files for Windows driver w/out defining SL_INTERNAL. */

#include "slfun.h"
#include "sl.h"
#include "slcalls.h"
#include "slascii.h"
#include "slchrmcs.h"
#include "slatt.h"
#include "slmacros.h"
#include "sllang.h"
#include "sllibext.h"
#include "slmemman.h"
#include "slmemtyp.h"
#include "slerror.h"

/* Include slch2mcs.h if Latin-2 (Eastern European) characters are needed */

#ifdef SL_HMCH2
#include "slch2mcs.h"
#endif

/* Include slch2mcs.h if Latin-5 (Cyrillic) characters are needed */

#ifdef SL_HMCH5
#include "slch5mcs.h"
#endif

/* Include slch7mcs.h if Latin-7 (Greek) characters are needed */

#ifdef SL_HMCH7
#include "slch7mcs.h"
#endif

#include "icsapi.h"

#include "icwin.h"

/* Include the extern file for the modules that are called                */

#include "icsext.h"

#include "icwinglb.h"
#include "icwinext.h"

#endif /* ICWINHDR_H */

/*  end of file - icwinhdr.h  */
