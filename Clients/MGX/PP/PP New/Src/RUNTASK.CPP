//®FD1¯®PL1¯®TP0¯®BT0¯
// (c) Copyright 1991 MICROGRAFX, Inc., All Rights Reserved.
// This material is confidential and a trade secret.
// Permission to use this work for any purpose must be obtained
// in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081
#include "pp.h"
#include "ppafx.h"
#include "id.h"
#include "svrdoc.h"
#include "svrview.h"

extern CPictPubApp PictPubApp;

// Static prototypes
static BOOL FilenameMatch( LPSTR lpName1, LPSTR lpName2 );

static HWND hWndTask;
static FNAME szFileName;

/***********************************************************************/
BOOL SetupRunMenus( HMENU hMenu, int nFileItem, ITEMID idItem, int iMaxItems, ITEMID idIniSection )
/***********************************************************************/
{
    int nItems;
    ITEMID id;
    HMENU hSubMenu;
    BOOL bPresent, bThisModule;
    char AppNamesBuffer[1000];
    STRING szIniSection, szThisModule, szModule;
    LPSTR lpAppName;

    // Get the menu handle for the File menu and the Run menu
    if ( !(hSubMenu = GetSubMenu(hMenu, nFileItem)) )
	    return( FALSE ); // No Run submenu

    // Get the name of the INI section to read
    AppNamesBuffer[0] = '\0';
    if( AstralStrEx( idIniSection, szIniSection, sizeof( szIniSection )))
    { 
        // Get all of the app names from the INI file
	    GetProfileString( szIniSection, NULL, "", AppNamesBuffer,
		                  sizeof( AppNamesBuffer ));
    }

    // Get the name of this module ( we won't put it in the list )
    szThisModule[0] = '\0';

    GetModuleFileName( PictPubApp.Get_hInstAstral(), szThisModule, sizeof( szThisModule ));

    // Add the app names to the Run menu popup
    nItems = 0;
    lpAppName = AppNamesBuffer;

    for( id = idItem; id < idItem + iMaxItems; id++ )
    {
	    // Get the module name from the INI file to see it it's us
	    GetProfileString( szIniSection, lpAppName, "", szModule, sizeof( szModule ));

	    bThisModule = FilenameMatch( szModule, szThisModule );

	    // See if the menu item exists
	    bPresent = ( EnableMenuItem( hSubMenu, id, MF_ENABLED ) >= 0 );

	    if( !( *lpAppName ) || bThisModule )
	    { 
            // Get the item out of the menu
		    if( bPresent )
			    DeleteMenu( hSubMenu, id, MF_BYCOMMAND );
	    }
	    else	
        {
		    // Put the application name into the menu
		    if( bPresent )
			    ModifyMenu( hSubMenu, id, MF_BYCOMMAND | MF_STRING, id, lpAppName );
		    else	
                AppendMenu( hSubMenu, MF_STRING, id, lpAppName );
		    nItems++;
	    }
	    // Move on to the next app name
	    if( *lpAppName )
		    lpAppName += ( lstrlen( lpAppName ) + 1 );
    }

    // If no items were put into the menu, delete the File/Run menu item
    if( !nItems )
    {
        DeleteMenu( hMenu, nFileItem, MF_BYPOSITION );
        //DrawMenuBar( AfxGetMainWnd()->GetSafeHwnd());
	    return( FALSE );
    }
//    DrawMenuBar( AfxGetMainWnd()->GetSafeHwnd());
    return( TRUE );
}

/***********************************************************************/
BOOL RunTask( HWND hWnd, ITEMID idItem, ITEMID idIniSection )
/***********************************************************************/
{
    STRING szAppName, szIniSection;

    // Get the name of the application
    if( !( GetMenuString( GetMenu( hWnd ), idItem, szAppName, 
        sizeof( szAppName ), MF_BYCOMMAND )))
	    return( FALSE );

    // Get the name of the INI section to read
    if( !AstralStrEx( idIniSection, szIniSection, sizeof( szIniSection )))
	    return( FALSE );

    // Get the module file name for the application
    // Put it in a static string so the callback has access to it
    if( !( GetProfileString( szIniSection, szAppName, "", szFileName,
        sizeof( szFileName ))))
	    return( FALSE );

    // Enumerate top-level windows to find a module with the same file name
    hWndTask = NULL;
    EnumWindows( EnumTaskCallback, 0L );

    if( !hWndTask )         // If no matching task, launch it
    { 
	    if( WinExec( szFileName, SW_NORMAL ) < (UINT)HINSTANCE_ERROR )
	        if( WinExec( filename( szFileName ), SW_NORMAL ) < (UINT)HINSTANCE_ERROR )			
                return( FALSE );
    }
    return( TRUE );
}

/***********************************************************************/
BOOL CALLBACK EXPORT EnumTaskCallback( HWND hWnd, LPARAM lParam )
/***********************************************************************/
{
    HMODULE hModule;
    FNAME   szModule;
    
    if( !hWnd )
	    return( FALSE );

    HWND hMDIWindow = hWnd;

    CWnd *pWnd = CWnd::FromHandle( hMDIWindow );
    if( pWnd->IsKindOf( RUNTIME_CLASS( CServerView )))        
	    hMDIWindow = pWnd->GetParentFrame()->GetSafeHwnd();

    #ifdef WIN32
        hModule = ( HMODULE )GetClassLong( hMDIWindow, GCL_HMODULE );
    #else
        hModule = ( HMODULE )GetClassWord( hMDIWindow, GCW_HMODULE );
    #endif

    szModule[0] = '\0';
    GetModuleFileName( hModule, szModule, sizeof( szModule ));

    if( FilenameMatch( szModule, szFileName ))
    {   
        // Since the matching task exists, restore it, and bring it to the top
	    if( IsIconic( hMDIWindow ))
		    ShowWindow( hMDIWindow, SW_NORMAL );
	    else	
            BringWindowToTop( hMDIWindow );

	    hWndTask = hMDIWindow;
        return( FALSE );
    }
    return( TRUE );
}

/***********************************************************************/
static BOOL FilenameMatch( LPSTR lpName1, LPSTR lpName2 )
/***********************************************************************/
{
    FNAME szName1, szName2;
    LPSTR lp;

    lstrcpy( szName1, filename( lpName1 ));
    lp = szName1;

    while( *lp && *lp != '.' )
	    lp++;
    *lp = '\0';

    lstrcpy( szName2, filename( lpName2 ));
    lp = szName2;
    while ( *lp && *lp != '.' )
	    lp++;
    *lp = '\0';

    return( !lstrcmpi( szName2, szName1 ));
}
