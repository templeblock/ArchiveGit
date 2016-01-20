/*-----------------------------------------------------------------
 * $Workfile:: DOSSTACK.C                                         $
 * $Revision:: 1                                                  $
 *     $Date:: 11/06/96 7:22a                                     $
 *   $Author:: Mike_houle                                         $
 *
 *-----------------------------------------------------------------
 * Desc: 
 *-----------------------------------------------------------------
 *
 * $Log: /Development/TPSLibs/Source/DOSSTACK.C $
 * 
 * 1     11/06/96 7:22a Mike_houle
 *
 *-----------------------------------------------------------------
 */


#if defined(DOS)

#include "TPSTYPES.H"
#include "StackCra.h"

//
// Public API
/*--------------------------------------------------------------------------
 * Function: _tdb_InitStackCrawl
 * Expects:  nothing
 * Returns:  BOOLEAN bSuccess
 * Purpose:  Allocates the memory needed to get & display a stack crawl
 *           Must be called before the stack crawl function is used.
 *           It moves memory. - We could pass in a depth parameter to determine
 *           how much memory gets allocated. 
 * Globals:  g_tdb_hSymList -- allocated and initialized if successful
 *--------------------------------------------------------------------------
 */
TDBAPI( BOOLEAN ) _tdb_InitStackCrawl( DEBUGPARMS FAR *lpDebugParms, uLONG ulFlags )
{
	return FALSE;
}


/*--------------------------------------------------------------------------
 * Function: _tdb_DeinitStackCrawl
 * Expects:  nothing
 * Returns:  nothing
 * Purpose:  This routine disposes of the memory allocated by
 *           _tdb_InitStackCrawl.
 *           Needs to be called when your thru with the StackCrawl.
 * Globals:  g_tdb_hSymList - free'd and reset to NULL
 *--------------------------------------------------------------------------
 */
TDBAPI( VOID ) _tdb_DeinitStackCrawl( VOID )
{
}



/*--------------------------------------------------------------------------
 * Function: _tdb_DoStackCrawl
 * Expects:  DEBUGPARMS	FAR* lpParms
 *           uLONG           ulFlags
 * Returns:  BOOLEAN bSuccess
 * Purpose:  RMS - this routine works in a very mission-specific fashion,
 *           for maximum performance. Instead of analyzing the code and
 *           trying to do a full disassembly, we spin through the code
 *           stream a word at a time. Since we're operating in a context-
 *           sensitive fashion, we won't find an instruction erroneously.
 * Globals:  g_tdb_hSymList -- fills it in...
 *--------------------------------------------------------------------------
 */
TDBAPI( BOOLEAN ) _tdb_DoStackCrawl( VOID )
{
	return FALSE;
}


#endif /* DOS */
