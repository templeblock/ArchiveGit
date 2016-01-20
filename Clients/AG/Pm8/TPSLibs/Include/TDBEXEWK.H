/******************************************************************************

   Header Name: tdbexewk.h
   Description: TPS debugging support executable symbolic information walker header.
   Version:     v0.00
   Author:      Michael C. Robert
   Date:        7/18/1994
   Copyright:   (c) Turning Point Software, Inc., 1994.  All Rights Reserved.

   Revision History:

   Date     Who      What
   ----     ---      ----
   7/18/94  MCR      Initial version.

******************************************************************************/

#ifndef  _tdbexewk_h

#define  _tdbexewk_h

/* INCLUDE FILES
*/

/* SYMBOLIC CONSTANTS
*/

/* TYPEDEFS
*/

/* MACROS
*/

/* EXTERNAL REFERENCES
*/

#ifndef  _no_globals

#endif   /* _no_globals */

/* FUNCTION PROTOTYPES
*/

BOOLEAN TPSAPI tdbexewkGetSourceLine( HMODULE hModule, WORD wSegment, WORD wIP, LPSZ lpszSourceFile, WORD wSourceFileLen, int FAR *lpiSourceLine );

#endif   /* _tdbexewk_h */
