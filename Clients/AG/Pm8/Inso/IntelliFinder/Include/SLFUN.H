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
*       SLFUN.H - OEM modifiable defines that effect the Standard       *
*           Library's FUNctionality.                                    *
*                                                                       *
*-----------------------------------------------------------------------*


    Description:
        SLFUN.H contains compile-time options that are intended to be 
        modified by OEMs.  The CTOs here affect the SL and so are platform
        related, rather than product related.  The nature of an 
        implementation determines the selection of CTOs.

    Cautions:
        Additions to this file should relate to platform issues, not product
        specific issues (prod. specific issues are resolved in SFFUN.H).

    Creation Date:
        23-dec-92   JCC
*-----------------------------------------------------------------------*/
#ifndef SLFUN_H

#define SLFUN_H

/*      Defines for the Extended Character Set(s)                       */

/* SL_HMCH1, 2, 5, and 7 are the flags to include the character sets InfoSoft
 * Latin1, InfoSoft Latin2, InfoSoft Latin5, and InfoSoft Latin7.  If these
 * #define statements are commented out or #undef'ed, the character set
 * conversion tables will not be included in the compilation.
 *
 * DO NOT comment out all 4 simultaneously, and DO NOT comment out a
 * character set flag unless you are ABSOLUTELY certain you will not need the
 * character set.
*/

#define  SL_HMCH1    
#undef   SL_HMCH2
#undef   SL_HMCH5
#undef   SL_HMCH7

/* MS-DOS and OS/2 default to Code Page 437.  DOS and OS/2 users can use Code 
 * Page 850 by defining SL_CH850:
 *
 *  #undef  SL_CH850    -- use code page 437 (default)
 *  #define SL_CH850    -- use code page 850
*/   
#undef SL_CH850

/*                            Character defines                         */

/*      Wildcard defines                                                */

#define SLWILD1         '?'     /* Single character wildcard            */
#define SLWILDS         '*'     /* Multiple character wildcard          */

/*      Default input soft hyphen character                             */

#define SL_SOFT_HYPHEN  '='     /* Default equals '='                   */

#undef SL_MAC_NOANSI_MEM        /* to use Mac Toolbox memory allocation  *
                                   calls instead of the alloc family.    *
                                   Code will be slower, but will have no *
                                   memory leaks resulting from the       *
                                   internal heap management of the       *
                                   alloc calls in MPW and Think C.       *
                                   See Apple Tech Note 510 for details.  *
                                 */


/*                         CTO's for debugging                           */

#undef SL_ASSERT_DBG    /* If defined, allow assert() macros. */
#undef SL_FULLSTAT_DBG  /* If defined, use extra SL_STATUS structure elements
                           dwErrInfo, wLine and pFile.
                        */

#ifndef SL_ASSERT_DBG
#ifndef NDEBUG
#define NDEBUG          /* Turn off ANSI assert() macros. */
#endif
#endif /* SL_ASSERT_DBG */

#endif /* SLFUN_H */

/*  End of file - slfun.h   */
