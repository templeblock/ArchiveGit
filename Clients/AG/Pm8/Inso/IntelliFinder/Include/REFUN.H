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
 * REfun.h - Function Declaration file.                                     *
 *                                                                          *
 *--------------------------------------------------------------------------*

Description:
    This file defines which functions are compiled into the Reference
    Engine.  There is a define for each available function below.  To
    include it in the RE place a #define in front of it.  To exclude
    the function place an #undef in front of it.

    Some of these defines relate to the RE itself, some are related to the
    RE in combination with other products.  The latter appear first in this
    file and are commented out, rather than #undef'd, which makes them
    turn-onnable via the command line.

Creation Date:
    02-jun-92   ECH

Change Record:
    Date        Author  Comment
    09-jul-92   JCC     Introduced clitic processing option
    16-sep-92   JCC     ET parser define
    11-may-93   PJM     added ROM code define
*---------------------------------------------------------------------------*/

#ifndef REFUN_H

#define REFUN_H


/* Undefine if you don't want entry retrieval or related functions.
 * NOTE:  Undefining RE_FN_ENTRY will disable the following
 *        functions (regardless of the choices made below).
 *        RE_FN_BASEFORM
 *        RE_FN_BOLSCH
 *        RE_FN_FLDSEL
 *        RE_FN_GETPRT
 *        RE_FN_PRON
*/
#define RE_FN_ENTRY


/* Undefine if you don't want the token stream parsing functionality.
*/
#define RE_FN_TOKEN


/* Undefine if you don't want the parser for the International Electronic
 * Thesauruses.
*/
#define RE_FN_ETPAR


/* Undefine if you are not using a concise thesaurus database.  If you are
 * using ONLY concise thesarurses, you can undefine RE_FN_ENTRY to gain
 * further savings.
*/
#define RE_FN_CONCISE_THES


/* Undefine if you don't want spelling alternative retrieval.
*/
#define RE_FN_ALTERN


/* Undefine if you don't want anagram generation.
*/
#define RE_FN_ANAGRM


/* Undefine if you don't want wildcard retrieval.
 * NOTE:  Un-defining RE_FN_WILDCH will disable RE_FN_WILDPAGE.
*/
#define RE_FN_WILDCH


/* Undefine if you don't want pageable (forward & backward scrolling) 
 * wildcarding.
*/
#define RE_FN_WILDPAGE


/* Undefine if you don't want closest match retrieval.
 */
#define RE_FN_CLOMCH


/* Undefine if you don't want Boolean search data or code.
*/
#define RE_FN_BOLSCH


/* Undefine if you don't want table of contents functionality.
*/
#define RE_FN_TOCHA


/* Define if you want to be able to interrupt processing during
 * potentially long operations.
 */
#define RE_FN_INTERRUPT

/* Undefine if you do NOT want to use accent folding
 */
#define RE_FN_ACCENT_FOLD

/* Undefine if you don't want string translation table data or code.
*/
#define RE_FN_STRTAB


/* Undefine if you do not want clitic processing code or accent folding
 * code (for verification in romance languages).
*/
#define RE_FN_CLITIC


/* Undefine if you do not want the code to inflect input words.
*/
#define RE_FN_INFLECT

/* Undefine if you do not want to support the personal dictionary.
*/
//#define RE_FN_PDICTIONARY

/* Define if you want ROM-friendly code.
*/
#undef RE_FN_ROM


/* Define if you're only using a single database and you do not want
 * the overhead associated with managing multiple databases.
*/
#undef RE_FN_1RDB


/* Define if you want the RE API to revert to that of RE V2.00.
 * This will change the RE API I/O structures to match the structures
 * as they were defined for Release 2.00. 
 * NOTE:  This will disable the inflection capability.
*/
#undef RE_FN_V200


/* Define if you want the RE API to revert to that of RE V2.01.
 * This will change the RE API I/O structures to match the structures
 * as they were defined for Release 2.01. 
*/
#undef RE_FN_V201

/* Define if you want the RE API to revert to that of RE V2.2.
 * This will change the RE API I/O structures to match the structures
 * as they were defined for Release 2.2. 
*/
#undef RE_FN_V22

/* Create lower-level defines that are dependent upon the above.
*/
#include "resubf.h"

#endif /* REFUN_H */

/* End of file - REfun.h
*/
