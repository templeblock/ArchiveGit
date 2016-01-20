/*-----------------------------------------------------------------
 * $Workfile:: DOSDLG.C                                           $
 * $Revision:: 1                                                  $
 *     $Date:: 11/06/96 7:22a                                     $
 *   $Author:: Mike_houle                                         $
 *
 *-----------------------------------------------------------------
 * Desc: 
 *-----------------------------------------------------------------
 *
 * $Log: /Development/TPSLibs/Source/DOSDLG.C $
 * 
 * 1     11/06/96 7:22a Mike_houle
 *
 *-----------------------------------------------------------------
 */


#if defined(DOS)

#include	"tpstypes.h"
#include	"dbgdlg.h"
#include	"stackcra.h"


uLONG			_DebugFlags;

/*--------------------------------------------------------------------------
 * Function: _tdb_InitAssertDlg
 * Expects:  DEBUGPARMS FAR * lpParms
 *           uLONG            ulFlags
 * Returns:  BOOLEAN          bSuccess
 * Purpose:  
 *--------------------------------------------------------------------------
 */
TDBAPI( BOOLEAN ) _tdb_InitAssertDlg( DEBUGPARMS FAR * lpParms, uLONG ulFlags )
{
	
	return FALSE;	// no error
}


/*--------------------------------------------------------------------------
 * Function: _tdb_DoAssertDlg
 * Expects:  LPSZ  lpszTest
 *           LPSZ  lpszReason
 *           LPSZ  lpszFilename
 *           uLONG       ulLineNo
 *           uLONG       ulCount
 *           LPVOID  lpvData
 *           uLONG       ulFormat
 * Returns:  int         iUserSelection
 * Purpose:  Setup and run the Macintosh Assertion Dialog
 * History:  10/28/94  MGD - Initial Revision
 *--------------------------------------------------------------------------
 */
TDBAPI( int ) _tdb_DoAssertDlg( LPSZ lpszTest, LPSZ lpszReason, LPSZ lpszFilename, uLONG ulLineNo, uLONG ulCount, LPVOID lpvData, uLONG ulFormat )
{
	return ID_TERMINATE;
}


//
//	Shut down debug library.
//
TDBAPI( VOID ) tdb_DeinitAssertDlg( VOID )
{
}

#endif /* DOS */
