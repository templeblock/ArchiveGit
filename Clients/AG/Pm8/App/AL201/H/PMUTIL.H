/*
 * PMUTIL.H
 *
 *  Header file for ArchiveLib 2.0
 *
 *  Copyright (c) 1994-1996 Greenleaf Software, Inc.
 *  All Rights Reserved
 *
 * DESCRIPTION
 *
 *  This header file contains the declaration for various
 *  utility functions used under Presentation Manager.
 *
 * CLASS DEFINITIONS:
 *
 *  None.
 *
 * REVISION HISTORY
 *
 *   February 14, 1996  2.0A : New release
 *
 */

#if !defined( _PMMUTIL_H )
#define _PMUTIL_H

#if defined( __cplusplus )
extern "C" {
#endif

AL_LINKAGE void AL_FUNCTION CenterWindow( HWND hwnd );
AL_LINKAGE void AL_FUNCTION TrimSystemMenu( HWND hwnd );
AL_LINKAGE int AL_FUNCTION SetupSlider( HWND hDlg, int slider_id, int draw_ticks );
AL_LINKAGE void AL_CFUNCTION
EditDisplay( HWND hDlg, int id, char AL_DLL_FAR *fmt, ... );

#if defined( __cplusplus )
}
#endif

#endif
