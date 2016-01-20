//////////////////////////////////////////////////////////////////////////
//                                                                      //
//  PPIMPORT.CPP                                                        //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#include "pp.h"
#include "ppafx.h"
#include "id.h"

extern CPictPubApp PictPubApp;

extern "C"
{
#include "isgdi9.h"
}
#include "ppimport.h"

LOCAL void GetImportFilter( int, LPSTR, LPSTR );
LOCAL int Calc_PercentRestraints( int, int );

//////////////////////////////////////////////////////////////////////////
BOOL SetupImportMenu( HMENU hFileMenu, ITEMID idItem )
//////////////////////////////////////////////////////////////////////////
{
    OFSTRUCT ofstruct;
    FNAME szDriver;

    GetModuleFileName( PictPubApp.Get_hInstAstral(), szDriver, sizeof( szDriver ));
    *strrchr( szDriver, '\\' ) = '\0';        
    lstrcat( szDriver, "\\" );
    lstrcat( szDriver, FILTER_DRIVER );

    // if filter driver is not installed, delete filter import menu. 
    if( OpenFile( szDriver, &ofstruct, OF_EXIST ) == HFILE_ERROR ) 
    {
        DeleteMenu( hFileMenu, idItem, MF_BYCOMMAND );
        return FALSE;
    }
    return TRUE;
}

//////////////////////////////////////////////////////////////////////////
int AstralImageImport( LPSTR lpInFile, LPSTR lpProfile, int nFileType )
//////////////////////////////////////////////////////////////////////////
{
    OFSTRUCT ofstruct;

    HINSTANCE hDLL              = NULL;
    HINSTANCE hSrcLibrary       = NULL;
    HINSTANCE hDstLibrary       = NULL;
    HINSTANCE hImportSetupData  = NULL;
    HINSTANCE hExportSetupData  = NULL;

    CREATESETUP  lpfnCreateSetup    = NULL;
    DESTROYSETUP lpfnDestroySetup   = NULL;
    IMPORTPROC   lpfnImportProc     = NULL;

    IMPORT_OPTIONS   Options;
    EXPORT_OPTIONS   Export;

    int error = IMPORT_NOERROR;

    FNAME szFilterPath;
    FNAME szOutFile; 
    FNAME szTempFile;
    FNAME szINIPath;
    char szFilter[13]; 
    char szINI[13]; 

    ProgressBegin( 1, PROGRESS_ID( IDS_UNDOIMPORTIMAGE ));

    szFilterPath[0] = szOutFile[0] = szTempFile[0] = szINIPath[0] = 0;
    szFilter[0] = szINI[0] = 0; 

    if( !nFileType || 
         nFileType < IDN_FIRSTIMPORTFILETYPE || 
         nFileType > IDN_LASTIMPORTFILETYPE )
    {
        error = IMPORT_INVALIDFILE_ERROR;
        goto ImportAbort;
    }

    // load ISGDI9.DLL explicitly
    GetModuleFileName( PictPubApp.Get_hInstAstral(), szFilterPath, sizeof( szFilterPath ));
    *strrchr( szFilterPath, '\\' ) = '\0';        
    lstrcat( szFilterPath, "\\" );
    lstrcat( szFilterPath, FILTER_DRIVER );

    hDLL = LoadLibrary( szFilterPath );

#ifdef WIN32
    if( hDLL == NULL )
#else
    if( hDLL < HINSTANCE_ERROR )
#endif
    {
        error = IMPORT_DLLLOAD_ERROR;
        goto ImportAbort;
    }

    // get filter and profile names
    GetImportFilter( nFileType, szFilter, szINI );

    // get location of filters
    GetExtNamePath( szFilterPath, IDN_IMPORTFILTERS );

    // set import filter path
    lstrcat( szFilterPath, szFilter );  

    if( OpenFile( szFilterPath, &ofstruct, OF_EXIST ) == HFILE_ERROR ) 
    {
        error = IMPORT_SRCLIBLOAD_ERROR;
        goto ImportAbort;
    }

    // set import filter profile path
    if( Control.bNetworkInstall )       // use pouch path
    {
        GetIniPath( szINIPath );
    }
    else                               // use filters directory
    {
        GetExtNamePath( szINIPath, IDN_IMPORTFILTERS );
    }
    lstrcat( szINIPath, szINI );  

#ifdef WIN32
    if(( hSrcLibrary = LoadLibrary( szFilterPath )) != NULL )
#else
    if(( hSrcLibrary = LoadLibrary( szFilterPath )) >= HINSTANCE_ERROR )
#endif
    {
        lpfnCreateSetup     = 
                ( CREATESETUP )GetProcAddress( hSrcLibrary, "CreateSetup" );
        hImportSetupData    = 
                ( HINSTANCE )( *lpfnCreateSetup )( hSrcLibrary, szINIPath, NULL, NULL );
    }
    else
    {
        error = IMPORT_SRCLIBLOAD_ERROR;
        goto ImportAbort;
    }

    // set export filter path 
    *strrchr( szFilterPath, '\\' ) = '\0';        
    lstrcat( szFilterPath, "\\" );
    lstrcat( szFilterPath, EXPORT_FILTER );

    if( OpenFile( szFilterPath, &ofstruct, OF_EXIST ) == HFILE_ERROR ) 
    {
        error = IMPORT_DSTLIBLOAD_ERROR;
        goto ImportAbort;
    }

#ifdef WIN32
    if(( hDstLibrary = LoadLibrary( szFilterPath )) != NULL )
#else
    if(( hDstLibrary = LoadLibrary( szFilterPath )) >= HINSTANCE_ERROR )
#endif
    {
        lpfnCreateSetup     = 
                ( CREATESETUP )GetProcAddress( hDstLibrary, "CreateSetup" );
        hExportSetupData    = 
                ( HINSTANCE )( *lpfnCreateSetup )( hDstLibrary, lpProfile, NULL, NULL );
    }
	else
    {
        error = IMPORT_DSTLIBLOAD_ERROR;
        goto ImportAbort;
    }

    memset( &Export, 0, sizeof( EXPORT_OPTIONS ));

    Export.GetSet_Status   = 
        ( GETSET_STATUS )MakeProcInstance(( FARPROC )GetSet_DialogStatus, PictPubApp.Get_hInstAstral());

    if( Export.GetSet_Status == NULL )
    {
        error = IMPORT_IMPORTFILE_ERROR;
        goto ImportAbort;
    }

    *strrchr( szFilterPath, '\\' ) = '\0';        
    *strrchr( szINIPath, '\\' ) = '\0';        

    memset( &Options, 0, sizeof( IMPORT_OPTIONS ));

    Options.hInputData     = hImportSetupData;      // Handle to import data
    Options.hOutputData    = hExportSetupData;      // Handle to export data
    Options.lpszDriverPath = szFilterPath;          // Driver DLL directory
    Options.lpszFontPath   = szFilterPath;          // Font file directory
    Options.lpExport       = &Export;

    // set temp file directory
    if( Control.bNetworkInstall )
        Options.lpszWorkPath   = szINIPath;         
    else
        Options.lpszWorkPath   = szFilterPath;         

    lstrcpy( szOutFile, Options.lpszWorkPath );     
    lstrcat( szOutFile, "\\" ); 

    // create temp file name
    lstrcpy( szTempFile, "PP" );
    GetTempFilename( szTempFile, ".TIF" );
    lstrcat( szOutFile, szTempFile );    

    lpfnImportProc = ( IMPORTPROC )GetProcAddress( hSrcLibrary, "ImportFile" );

    if( !( error = (( *lpfnImportProc )( NULL, EXPORT_FILTER, szOutFile, lpInFile, ( LPSTR )&Options ))))    
        lstrcpy( lpInFile, szOutFile );
    else
        FileDelete( szOutFile );        // get rid of temp file in case of error

    if( Export.GetSet_Status != NULL )
        FreeProcInstance(( FARPROC )Export.GetSet_Status );
    
ImportAbort:

#ifdef WIN32
    if( hDstLibrary != NULL )
#else
    if( hDstLibrary >= HINSTANCE_ERROR )
#endif
    {
        lpfnDestroySetup = ( DESTROYSETUP )GetProcAddress( hDstLibrary, "DestroySetup" );
        ( *lpfnDestroySetup )( hExportSetupData );

        FreeLibrary( hDstLibrary );
    }

#ifdef WIN32
    if( hSrcLibrary != NULL )
#else
    if( hSrcLibrary >= HINSTANCE_ERROR )
#endif
    {
        lpfnDestroySetup = ( DESTROYSETUP )GetProcAddress( hSrcLibrary, "DestroySetup" );
        ( *lpfnDestroySetup )( hImportSetupData );

        FreeLibrary( hSrcLibrary );
    }

#ifdef WIN32
    if( hDLL != NULL )
#else
    if( hDLL >= HINSTANCE_ERROR )
#endif
    {
        FreeLibrary( hDLL );
    }

    ProgressEnd();

    switch( error )
    {
        case IMPORT_NOERROR:
            break;

        case IMPORT_INVALIDFILE_ERROR:              // type not supported
            Message( IDS_INVALIDIMPORTFILE_ERROR );
            break;

        case IMPORT_SRCLIBLOAD_ERROR:               // error loading import filter
            Message( IDS_IMPORTLIBLOAD_ERROR );
            break;

        case IMPORT_DSTLIBLOAD_ERROR:               // error loading export filter
            Message( IDS_EXPORTLIBLOAD_ERROR );
            break;

        case IME_INVALID_FILE_TYPE:                 // incorrect file format
            Message( IDS_INVALIDIMPORTFILE_TYPE );
            break;

        case IME_INVALID_FILE_VERSION:              // unsupported file version
            Message( IDS_INVALIDIMPORTFILE_VERSION );
            break;

        case IME_MEMORY:                            // insufficient memory
            Message( IDS_EMEMALLOC );
            break;

        case IMPORT_DLLLOAD_ERROR:
        case IMPORT_IMPORTFILE_ERROR:               // non-specific error from app
        default:                                    // non-specific error from filter
            Message( IDS_IMPORTFILE_ERROR );    
            break;
    }
    return error;
}

//////////////////////////////////////////////////////////////////////////
LOCAL void GetImportFilter( int nFileType, LPSTR szFilter, LPSTR szINI )
//////////////////////////////////////////////////////////////////////////
{
    switch( nFileType )
    {
        case IDN_AI:
            lstrcpy( szFilter, "imps_9.flt" );
            lstrcpy( szINI, "imps_9.ini" );
            break;

        case IDN_CDR:
            lstrcpy( szFilter, "imcdr9.flt" );
            lstrcpy( szINI, "imcdr9.ini" );
            break;

        case IDN_CGM:
            lstrcpy( szFilter, "imcgm9.flt" );
            lstrcpy( szINI, "imcgm9.ini" );
            break;

        case IDN_DRW:
            lstrcpy( szFilter, "imdrw9.flt" );
            lstrcpy( szINI, "imdrw9.ini" );
            break;

        case IDN_WMF:
            lstrcpy( szFilter, "imwmf9.flt" );
            lstrcpy( szINI, "imwmf9.ini" );
            break;

        default:
            szFilter = szINI = NULL;
            break;
    }
}

static int isPrescan                = FALSE;
static int PrescanPercentRestraint  = 0;
static int ScanPercentRestraint     = 100;
static int ImportPercentRestraint   = 100;
static int ExportPercentRestraint   = 0;
static int WeightedPercent          = 0;

//////////////////////////////////////////////////////////////////////////
int FAR PASCAL EXPORT GetSet_DialogStatus( HWND hDialog, int Flag, int Code, 
                                           int Import, int Export, int n, 
                                           LPSTR FAR *Strings )
//////////////////////////////////////////////////////////////////////////
//   This is a call back procedure for getting or setting dialog        //
//   status.  What it does depends on the Flag.  The flags are:         //
//   1 ) IME_STATUS:   Only checking if user wants to cancel.  No       //
//                     message is displayed, so we don't pause the      //
//                     processing.( CALLED from chk_esc( ).)            //
//                                                                      //
//   2 ) IME_NOTIFY:   Only imforming the user( e.g. a process has      //
//                     started ).  No checking for cancel takes place.  //
//                     We don't pause the information because the user  //
//                     should have time to see it before it scrolls     //
//                     off the screen.( CALLED from message().          //
//                                                                      //
//   3 ) IME_WARNING:  Warning the user that a non-fatal error has      //
//                     taken place.  The user is allowed to continue    //
//                     or to abort the process.  Thus we check for      //
//                     cancel, and we pause to allow them to respond.   //
//                     ( CALLED from ask_yeno( ). )                     //
//                                                                      //
//   3 ) IME_ERROR:    Telling the user that a fatal error has          //
//                     occured.  We pause to allow the user to see      //
//                     the error message.  However, we will abort, so   //
//                     we return IME_ABORT.( CALLED from error(). )     //
//                                                                      //
//////////////////////////////////////////////////////////////////////////
{
    int Escape = IME_OK;
    int PercentComplete = 0;

    switch( Flag )
    {
        case IME_STATUS:    // No message strings for simple STATUSes 
            switch( Code )
            {
                case IME_UPDATE_PERCENT :
                    PercentComplete = Calc_PercentRestraints( Import, Export );
                    if( AstralClockCursor( PercentComplete, 100, YES ))
                        Escape = IME_ABORT;
                    else
                        Escape = IME_OK;
                    return( Escape );

                case IME_INIT_IMPORT :
                    isPrescan               = FALSE;
                    ImportPercentRestraint  = 100;
                    ExportPercentRestraint  = 0;

                    PrescanPercentRestraint = Import;
                    ScanPercentRestraint    = Export;
                    isPrescan = FALSE;
                    return( IME_OK ); 

                case IME_INIT_EXPORT :
                    ImportPercentRestraint  = Import;
                    ExportPercentRestraint  = Export;
                    return( IME_OK ); 

                case IME_PRESCAN :
                    isPrescan = TRUE;
                    return( IME_OK ); 

                case IME_IMPORT :
                    isPrescan = FALSE;
                    return( IME_OK ); 
            }

        case IME_NOTIFY:
            return( IME_OK );   // don't checkstatus -- will check next 
                                // time that a chk_esc() is called.

        case IME_WARNING:
            return( IME_OK );

        case IME_ERROR: 
            return( IME_ABORT );
    }   
    return( IME_OK );
}

//////////////////////////////////////////////////////////////////////////
LOCAL int Calc_PercentRestraints( int ImportPercent, int ExportPercent )
//  Fills a bar by percentage of PercentAmount  
//////////////////////////////////////////////////////////////////////////
{
    if( ImportPercent > 0 )         // we are importing 
    {
        if( isPrescan )
        {
            WeightedPercent =( int )( ImportPercent *
                                    ( ImportPercentRestraint * 0.01 ) *
                                    ( PrescanPercentRestraint * 0.01 ));
        }
        else
        {
            WeightedPercent =( int )(( ImportPercent *
                                     ( ImportPercentRestraint * 0.01 ) *
                                     ( ScanPercentRestraint * 0.01 ))
                                        /* plus where we left off */
                                     + ( PrescanPercentRestraint  *
                                         ImportPercentRestraint * 0.01 ));
        }
    }
    else if( ExportPercent > 0 )    // we are exporting
    {
        WeightedPercent = ( int )(( ExportPercent *
                                  ( ExportPercentRestraint * 0.01 ))
                                  + ImportPercentRestraint );
    }
    return( WeightedPercent );
}
