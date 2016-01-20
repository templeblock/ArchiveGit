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
*       Name:                                                           *
*            IHhyphdr.h   International Hyphenator Interface header file*
*                                                                       *
*       Description:                                                    *
*                                                                       *
*-----------------------------------------------------------------------*/
#ifndef IHHYPHDR_H
#define IHHYPHDR_H

		/* limits amount of windows.h file that is used. */
#ifdef WINDOWS
#ifndef SL_INTERNAL
#define SL_INTERNAL   
#endif
#endif

/*      lower level includes                                            */
#include "sllibhdr.h"
#include "sfdef.h"
#include "sftyp.h"

/*      module level includes                                           */
#include "ih.h"
#include "ihctlbuf.h"
#include "ihbuff.h"
#include "ihhypext.h"

#endif /* IHHYPHDR_H */
/* end file ihhyphdr.h */

