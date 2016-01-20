/*-----------------------------------------------------------------
 * $Workfile:: MACDEV.C                                           $
 * $Revision:: 1                                                  $
 *     $Date:: 11/06/96 7:22a                                     $
 *   $Author:: Mike_houle                                         $
 *
 *-----------------------------------------------------------------
 * Desc: 
 *-----------------------------------------------------------------
 *
 * $Log: /Development/TPSLibs/Source/MACDEV.C $
 * 
 * 1     11/06/96 7:22a Mike_houle
 *
 *-----------------------------------------------------------------
 */

#if	defined( TPSDEBUG )
#if defined( MAC )

#include "TPSTYPES.H"
#include "dbdevice.h"

//
// Public API
/*--------------------------------------------------------------------------
 * Function: _tdb_InitDebugDevice
 * Expects:  nothing
 * Returns:  BOOLEAN bSuccess
 * Purpose:  
 *--------------------------------------------------------------------------
 */
TDBAPI( BOOLEAN ) _tdb_InitDebugDevice( DEBUGPARMS FAR *lpDebugParms, uLONG ulFlags )
{
	return TRUE;
}


/*--------------------------------------------------------------------------
 * Function: _tdb_DeinitDebugDevice
 * Expects:  nothing
 * Returns:  nothing
 * Purpose:  
 * Globals:  g_tdb_hSymList - free'd and reset to NULL
 *--------------------------------------------------------------------------
 */
TDBAPI( VOID ) _tdb_DeinitDebugDevice( VOID )
{
}



#endif /* MAC */
#endif	//	TPSDEBUG

