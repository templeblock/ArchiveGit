/*
 * AL.H
 *
 *  Header file for ArchiveLib 2.0
 *
 *  Copyright (c) 1994-1996 Greenleaf Software, Inc.
 *  All Rights Reserved
 *
 * DESCRIPTION
 *
 *  This is the master file that you can use to include
 *  all of the ArchiveLib headers.  If you are using precompiled
 *  headers with your compiler, this might be the file you want
 *  included first in all your source files.
 *
 * REVISION HISTORY
 *
 *  May 26, 1994  1.0A  : First release
 *
 *  February 14, 1996  2.0A : New release
 */

#ifndef _AL_H
#define _AL_H

#include "arclib.h"

/*
 * Derived classes
 */

#include "filestor.h"
#include "fileattr.h"
#include "memstore.h"
#include "copyengn.h"
#include "glengn.h"
#include "glarc.h"
#include "pkengn.h"
#include "pkarc.h"

/*
 * Demo classes and functions
 */

#if defined( AL_WINDOWS )
#  include "winmon.h"
#  include "algauge.h"
#elif defined( AL_OS2_PM )
#  include "pmmon.h"
#endif

#  include "bargraph.h"
#  include "spinner.h"

/*
 * Additional classes 
 */
#include "wildcard.h"

#endif /* #ifdef _AL_H */
