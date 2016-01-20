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
 * REsubf.h - Define internal compile time options (SUB Functions).         *
 *                                                                          *
 *--------------------------------------------------------------------------*

Description:
   This header file defines internal compile-time options based upon
   #defines made by the OEM in refun.h.  This level of compile time
   flags is made to make the code more readable (less complicated
   compile-time switches in the code).

    SF as used here stands for Sub Function, unless otherwise noted.

Creation Date:
    14-may-91   ECH

Change Record:
    Date        Author  Comment
    21-feb-92   RAS     ED names -> RE
    07-jan-92   ECH     Added ED_FN_ENTRY and the side effect ED_SF_...
    09-jul-92   JCC     Introduced clitic processing defines
    14-oct-92   JCC     Accent folding
    02-feb-93   JCC     Inflection
    19-feb-93   JCC     Version control CTOs
    10-mar-93   PJM     Added RE_SF_ETPAR
*---------------------------------------------------------------------------*/

#ifndef RESUBF_H

#define RESUBF_H

/* Define this if you want batch test generation code. */
#ifdef QA
#define RE_DRV_TESTENV
#endif

/* Sub-function defines.  Based upon the OEM's choice of functions
 * (RE_FN_??????) there may be internal definitions that are made
 * here.  The purpose for this level of definition is code readability.
*/

/* Verification decoding is, for now, part of the kernel
*/
#define RE_SF_VERIFY_DECODE

/* Normal user defines that are tucked "out of the way".  These functions
 * are not currently developed or are being phased out.
*/
#undef RE_FN_FLDSEL
#undef RE_FN_GETPRT
#undef RE_FN_PRON

/* Version control CTOs.  Turn off functionality introduced in later versions
 * if OEM has defined compatibility with an earlier version.  This was
 * started with version 2.01; therefore, v2.00 being the earliest version to
 * which an OEM may revert.
*/
/* Version 2.00 didn't have inflection.  It also does not want the changes
 * made in releases after 2.01.
*/
#ifdef RE_FN_V200
#undef RE_FN_INFLECT
#define RE_SF_V201
#define RE_SF_V22
#endif

/* Version 2.01
*/
#ifdef RE_FN_V201
#define RE_SF_V201
#define RE_SF_V22
#endif

/* Version 2.2
*/
#ifdef RE_FN_V22
#define RE_SF_V22
#endif

#ifdef RE_SF_V22
#undef RE_FN_PDICTIONARY
#endif

/* Use the internal defines RE_CLITIC_PROCESS and SF_BASE_CLITIC if the
 * OEM option RE_FN_CLITIC is defined.  These are verification-related
 * defines.
*/
#ifdef RE_FN_CLITIC
#define RE_CLITIC_PROCESS
#define SF_BASE_CLITIC      /* SF here == Standard Function, not Sub Func.  */
#endif

/* Use the internal defines RE_SF_ACCENT and RE_SF_OTHER_DECODE if the
 * OEM option RE_FN_CLITIC is defined.  These are verification-related
 * defines, but require correction code due to SFaccent's modus operandi.
*/
#ifdef RE_FN_CLITIC
#define RE_SF_ACCENT
#ifndef RE_SF_OTHER_DECODE
#define RE_SF_OTHER_DECODE
#endif
#endif

/* If RE_FN_ACCENT_FOLD is defined then define SF_ACCENT which will compile 
 * the code that does the folding
 */
#ifdef RE_FN_ACCENT_FOLD
#define SF_ACCENT
#endif

/* Quadgram decoding is dependant on selectable functions
   #if defined RE_FN_ALTERN || defined RE_FN_ANAGRM || defined RE_FN_WILDCH
   #define RE_SF_OTHER_DECODE
   #endif
*/

#ifdef RE_FN_ALTERN
#ifndef RE_SF_OTHER_DECODE
#define RE_SF_OTHER_DECODE
#endif
#endif

#ifdef RE_FN_ANAGRM
#ifndef RE_SF_OTHER_DECODE
#define RE_SF_OTHER_DECODE
#endif
#ifndef SF_ANAGRAM
#define SF_ANAGRAM
#endif
#endif


#ifdef RE_FN_CLOMCH
#ifndef RE_SF_OTHER_DECODE
#define RE_SF_OTHER_DECODE
#endif
#ifndef RE_SF_CLOMCH
#define RE_SF_CLOMCH
#endif
#endif

#ifdef RE_FN_CONCISE_THES
#ifndef RE_SF_CIRCLE_INDEX
#define RE_SF_CIRCLE_INDEX
#endif
#ifndef SF_CIRCLE_INDEX
#define SF_CIRCLE_INDEX
#endif
#endif

#ifdef RE_FN_WILDCH
#ifndef RE_SF_OTHER_DECODE
#define RE_SF_OTHER_DECODE
#endif
#else /* !RE_FN_WILDCH */
#undef RE_FN_WILDPAGE
#endif

#ifdef RE_FN_WILDPAGE
#define SF_WILDPAGE
#define RE_SF_WILDPAGE
#endif

#ifdef RE_FN_ENTRY
#ifndef RE_SF_ENTRY_VERIFICATION
#define RE_SF_ENTRY_VERIFICATION
#endif
#endif

#ifdef RE_SF_CIRCLE_INDEX
#ifndef RE_SF_ENTRY_VERIFICATION
#define RE_SF_ENTRY_VERIFICATION
#endif
#endif

/* If entry retrieval is defined...
*/
#ifdef RE_FN_ENTRY

/* Make the direct links between RE_FN_.. and RE_SF_ .. defines.
*/

#ifndef RE_SF_OTHER_DECODE
#define RE_SF_OTHER_DECODE
#endif

#ifdef RE_FN_BOLSCH
#ifndef RE_SF_BOLSCH
#define RE_SF_BOLSCH
#endif
#endif 

#ifdef RE_FN_TOKEN
#define RE_SF_TOKEN
#endif

#else  /* RE_FN_ENTRY */

/* Entry retrieval is not defined.  Therefore, the following functions
 * are not available and must be undefined.
*/

#undef RE_SF_BOLSCH
#undef RE_SF_FLDSEL
#undef RE_SF_GETPRT
#undef RE_SF_PRON
#undef RE_SF_TOKEN

#endif /* RE_FN_ENTRY */


/* If either entry retrieval or Boolean searching is defined then
 * we need to return entry numbers from SFdecode.
*/

#ifdef RE_SF_ENTRY_VERIFICATION
#ifndef RE_SF_ENT_LIST
#define RE_SF_ENT_LIST
#endif
#endif
#ifdef RE_SF_BOLSCH
#ifndef RE_SF_ENT_LIST
#define RE_SF_ENT_LIST
#endif
#endif


/* Link the Reference Engine Definitions to the Standard Function
 * definitions.
*/

#ifdef RE_FN_ALTERN
#ifndef SF_CORRECTION
#define SF_CORRECTION
#endif
#endif

#ifdef RE_FN_WILDCH
#ifndef SF_WILDCARD
#define SF_WILDCARD
#endif
#endif

#ifdef RE_SF_OTHER_DECODE
#ifndef SF_XTRA_DECODE
#define SF_XTRA_DECODE
#endif
#endif

#ifdef RE_SF_CLOMCH
#ifndef SF_CLOSEST_MATCH
#define SF_CLOSEST_MATCH
#endif
#endif

#ifdef RE_FN_TOCHA
#ifndef SF_TABLE_OF_CONTENTS
#define SF_TABLE_OF_CONTENTS
#endif
#endif

#ifdef RE_SF_BOLSCH
#ifndef SF_BOOLSEARCH
#define SF_BOOLSEARCH
#endif
#endif

#ifdef RE_FN_ENTRY
#ifndef SF_ENTRY
#define SF_ENTRY
#endif
#endif

/* Both inflection and entry list retrieval use baseform processing.
*/
#ifdef RE_SF_ENT_LIST
#ifndef SF_ENT_LIST
#define SF_ENT_LIST
#endif
#ifndef SF_BASEFORM
#define SF_BASEFORM
#endif
#endif

#ifdef RE_FN_INFLECT
#ifndef RE_SF_OTHER_DECODE
#define RE_SF_OTHER_DECODE
#endif
#ifndef SF_INFLECT
#define SF_INFLECT
#endif
#ifndef SF_BASEFORM
#define SF_BASEFORM
#endif
#endif

#ifdef RE_FN_ETPAR
#ifndef RE_SF_OTHER_DECODE
#define RE_SF_OTHER_DECODE
#endif
#ifndef RE_SF_ETPAR
#define RE_SF_ETPAR
#endif
#ifndef RE_FN_STRTAB
#define RE_FN_STRTAB
#endif

#else /* RE_FN_ETPAR undefined */
#undef  RE_FN_PDICTIONARY
#endif /* RE_FN_ETPAR */

/* Single real/three virtual databases.
*/
#ifdef RE_FN_1RDB
#define RE_SF_1RDB
#endif

/* If ROM, use single real/three virtual databases.
*/
#ifdef RE_FN_ROM
#ifndef RE_SF_1RDB
#define RE_SF_1RDB
#endif
#define RE_SF_ROM
#endif


#ifndef RE_FN_V201

#ifdef  RE_FN_INTERRUPT
#ifndef RE_SF_INTERRUPT
#define RE_SF_INTERRUPT
#endif
#ifndef SF_INTERRUPT
#define SF_INTERRUPT
#endif
#endif

#ifdef RE_FN_PDICTIONARY
#ifndef RE_SF_ROM         /* Personal Dictionary not supported for ROM */
#ifndef RE_SF_PDICTIONARY
#define RE_SF_PDICTIONARY
#endif
#ifndef SF_PD
#define SF_PD
#endif
#endif
#endif

#else

#ifdef  RE_SF_INTERRUPT
#undef  RE_SF_INTERRUPT
#endif
#ifdef  SF_INTERRUPT
#undef  SF_INTERRUPT
#endif

#endif  /* RE_FN_V201 */

#endif /* RESUBF_H */

/* End of file - REsubf.h
*/
