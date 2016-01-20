/*****************************************************************************
 
       (C) Copyright 1994 by Micrografx, Inc. All rights reserved.
    Permission to use this work for any purpose must by obtained in writing
            from Micrografx, 1303 Arapaho, Richardson, TX. 75081.
 
 ****************************************************************************
 ****************************************************************************

                                    BOMB.C                 

                           Self-Destruct Time Bomb

    The Free Trial Demo version of Picture Publisher is a fully functional build of
    PP with a few annoyances and a 30 day self destructing time bomb.
    The time bomb allows the user to run the program for 30 days since the
    first running of PP or 90 times which ever comes first.

    The limits apply to the machine not the user. This works by installing 
    locks to the machine the first time the program is run. These locks
    are used in conjunction with the executable to store time and count 
    info as well as to disable the program after that the limits have 
    expired.

    There is a hole in this scheme - if all the lock are removed, the
    executable thinks it is the first time. 

    The Locking Mechanism is a combination of several hidden variables
    within the user's system that all have to be evaluated for PP
    to operate.


    1)  PP.INI: keep date setting.

        [Preferences]              
        Ruler Data=xxx           
        Units Index=yyy       

        where xxx = the total number of seconds since 1/1/94 + 757411200
                  =((days since 1/1/94 ) * 24 * 60 * 60 ) + 757411200

        ex: 5/1/94 =((31+28+31+30 ) * 24 * 60 * 60 ) + 757411200
                   = 767779200

        Basically, the value of this .ini setting is the 'time_t' long integer used
        to construct a CTime MFC class.  If this entry does not exist, then the
        value defaults to 0.

        where - yyy is the number of times the program has been run encoded
                by a lookup table taht makes it apear to be random. This value
                can also be set to special value that indicates an intruder 
                alert.


    2)  WIN.INI: ini marked indicating that we have been on this system.

        [Colors]                    // real section.
        Status Color=192 192 192    // dummy setting.

        if an intruder is detected by a invalid setting this color 
        value is changed to 191 191 191.


    3)  TXWIN.DLL   // file date set to match windows win.com date and time
                    // so that it does not look like a new file.
                    // Also archive bit cleared.

        Dummy dll that is just a binary file that contains the
        structure described below.

        struct MasterLock
        {
            time_t DateStarted;         // date compared with init date.
            WORD   TrialCount           // count compared to ini count.
            BOOL   bIntruderDetected;   // set if any locks are missing 
                                        // do not match criteria.
        };

 ****************************************************************************
 ****************************************************************************/

#include "pp.h"
#include "ppafx.h"
#include "bomb.h"
#include <stdlib.h>
#include <io.h>
#include <time.h>
#include <sys\types.h>
#include <sys\stat.h>
#include <sys\utime.h>
#include <dos.h>

/********************************* Constants ********************************/

#define TIME_LIMIT                  30      // 30 days
#define TRIAL_LIMIT                 90      // 90 trials 

#define INTRUDER_COUNT              ( TRIAL_LIMIT + 1 )
#define INTRUDER_BIT                0x7000
                             
#define DUMMY_DLL                   _T("PXWIN.DLL")
#define DUMMY_COLOR_NAME            _T("PStatusColor")
#define PICTPUB_PROFILE             _T("PP.INI")
#define PICTPUB_SECTION             _T("Picture Publisher")

#define DUMMY_DLL_SIZE              4563                
#define WININI_COLOR_SECTION        _T("Colors")
#define DUMMY_COLOR_GOOD_VALUE      _T("192 192 192")
#define DUMMY_COLOR_BAD_VALUE       _T("191 191 191")
#define DATE_KEYNAME                _T("RulerData")
#define COUNT_KEYNAME               _T("UnitsIndex")

//#define SLIDESHOW                   _T("demo.exe")
//#define SLIDESHOWSIZE                0

/*********************************** Macros *********************************/
/*********************************** Types **********************************/

struct SMasterLock
{
    time_t TimeStarted;         // date compared with ini date and cur date.
    WORD   TrialCount;          // count of how many times prog executed.
    BOOL   bIntruderDetected;   // set if any locks are missing or
                                // do not match criteria.
};

/******************************** Local Data ********************************/

static BOOL bInitialized = FALSE;   // make sure that no one jumps around us.

/******************************** Public Data *******************************/
/****************************** Local Functions *****************************/

HFILE OpenDummyFile( UINT Mode );
void  WriteDummyFile( SMasterLock *pMaster );
BOOL  NoLocksInstalled();
void  InstallLocks();
void  SetIntruderLocks();
WORD  DecodeCount( WORD wCodedCount );
WORD  EncodeCount( WORD wCount );
void  UpdateCountLock( WORD wCount );
WORD  CheckCountLock( WORD wCount, WORD wMasterCount );
WORD  CheckTimeLock( time_t tTime, time_t tMasterTime );
BOOL  OpenLocks( WORD &wDaysLeft, WORD &wTrialsLeft, BOOL &bIntruder );


HFILE OpenDummyFile( UINT Mode )
{
    OFSTRUCT of;
    FNAME szDllPathname;
    
    // Build up full pathname of dummy dll that contains the master lock 
    // information. This is located in the windows directory.

    GetWindowsDirectory( szDllPathname, _MAX_PATH );
    lstrcat( szDllPathname, _T("\\")DUMMY_DLL );
    
    return OpenFile( szDllPathname, &of, Mode );
}


void WriteDummyFile( SMasterLock *pMaster )
{
    HFILE hFile = OpenDummyFile( OF_EXIST );

    if( hFile == HFILE_ERROR )
        hFile = OpenDummyFile( OF_CREATE | OF_WRITE );
    else
        hFile = OpenDummyFile( OF_WRITE );
    
    if( hFile != HFILE_ERROR )
    {
        _lwrite( hFile, (LPCSTR)pMaster, sizeof( SMasterLock ));
        _chsize( hFile, DUMMY_DLL_SIZE ); // makes it look bigger.
        _lclose( hFile );
    
        // Set the date to match windows. 

        struct _stat FileStats;
        struct _utimbuf FileDate;
        FNAME szWinPathname;

        GetWindowsDirectory( szWinPathname, _MAX_PATH );
        lstrcat( szWinPathname, _T("\\win.com") );
        if( _stat( szWinPathname, &FileStats ) != -1 ) // if we cant get stats blow it off.
        {
            GetWindowsDirectory( szWinPathname, _MAX_PATH );
            lstrcat( szWinPathname, _T("\\")DUMMY_DLL );
            FileDate.actime = FileStats.st_atime;
            FileDate.modtime = FileStats.st_mtime;
            _utime( szWinPathname, &FileDate );
        }
    }
}


BOOL NoLocksInstalled()
/*****************************************************************************
    Returns true if the system has no locks installed. This indicates that 
    this is the first time that the free trial demo is being run. Or some
    smart hacker has figured out how to remove all the locks.
*****************************************************************************/
{
    BOOL bNoneInstalled = TRUE;
    FNAME szTemp;

    // Check existance of all locks.
    if( OpenDummyFile( OF_EXIST ) != HFILE_ERROR                            ||
        GetProfileString( WININI_COLOR_SECTION, 
                          DUMMY_COLOR_NAME, _T(""), szTemp, sizeof( szTemp ))   ||
        GetDefaultString( DATE_KEYNAME, _T(""), szTemp, sizeof( szTemp ))        ||
        GetDefaultString( COUNT_KEYNAME, _T(""), szTemp, sizeof( szTemp )))
    {
        bNoneInstalled = FALSE;
    }
    return bNoneInstalled;
}


void InstallLocks()
{
    SMasterLock Master;
    TCHAR szTemp[20];
    
    Master.bIntruderDetected = FALSE;   // No intruders yet.
    Master.TrialCount = 1;              // include this one.
    time( &Master.TimeStarted );        // get today's date.

    WriteDummyFile( &Master );
    
    WriteProfileString( WININI_COLOR_SECTION, DUMMY_COLOR_NAME, 
                        DUMMY_COLOR_GOOD_VALUE );

    wsprintf( szTemp, "%lu", Master.TimeStarted );
    PutDefaultString( DATE_KEYNAME, szTemp );

    wsprintf( szTemp, "%u", EncodeCount( Master.TrialCount ));
    PutDefaultString( COUNT_KEYNAME, szTemp );
}    


void SetIntruderLocks()
{
    SMasterLock Master;
    TCHAR szTemp[10];
    
    Master.bIntruderDetected = TRUE;
    WriteDummyFile( &Master );

    WriteProfileString( WININI_COLOR_SECTION, DUMMY_COLOR_NAME, 
                        DUMMY_COLOR_BAD_VALUE );

    wsprintf( szTemp, "%u", EncodeCount( INTRUDER_COUNT ));
    PutDefaultString( COUNT_KEYNAME, szTemp );
}


WORD DecodeCount( WORD wCodedCount ) // value from 0 to MAX_RAND;
/*****************************************************************************
    Decodes the count value stored in the ini file. It uses the rand function 
    with a seed of 100. The encoded count is simply the nth element of the 
    rand series seeded at 100, where n is the count. So this function runs 
    thru the rand series comparing the code and incrementing the count until
    a match was found. If no match was found the return will be TRIAL_LIMIT+1.
    This will and indicator of an intruder. Assured 0-91 rand returns all
    unique values.
*****************************************************************************/
{
    WORD wCount;

    srand( 100 );         

    for( wCount = 1; wCount <= TRIAL_LIMIT; wCount++ )
    {
        if( wCodedCount == ( WORD )rand())
            break;
    }
    return wCount;
}


WORD EncodeCount( WORD wCount ) // value from 0 to 90;
/*****************************************************************************
    Encodes the count value stored in the ini file. It uses the rand function 
    with a seed of 100. The encoded count is simply the nth element of the 
    rand series seeded at 100, where n is the count. Assured 0-91 rand returns
    all unique values.
*****************************************************************************/
{
    WORD wCodedCount = 0;

    srand( 100 );

    while( wCount-- )
        wCodedCount = ( WORD )rand();

    return wCodedCount;
}


void UpdateCountLock( WORD wCount )
{
    HFILE hFile = OpenDummyFile( OF_READWRITE );
    TCHAR szCount[10];

    if( hFile != HFILE_ERROR )
    {
        SMasterLock Master;

        _lread( hFile, &Master, sizeof( SMasterLock ));
        _lclose( hFile );
        Master.TrialCount = wCount;
        WriteDummyFile( &Master );
    }

    wsprintf( szCount, "%u", EncodeCount( wCount ));
    PutDefaultString( COUNT_KEYNAME, szCount );
}


WORD CheckCountLock( WORD wCount, WORD wMasterCount )
/*****************************************************************************
    Checks the trial count with the master count and with the TRIAL_LIMIT.
    Returns true if all withing limits. Otherwise it returns false. For 
    unsuccessfull checks the return has the Intruder bit set if the counts
    where found to be tampered with.
*****************************************************************************/
{
    WORD wTrialsLeft = 0;
    
    wCount = DecodeCount( wCount );

    if( wCount <= TRIAL_LIMIT && wCount == wMasterCount )
        wTrialsLeft = TRIAL_LIMIT - wCount;
    else
        wTrialsLeft |= INTRUDER_BIT;

    return wTrialsLeft;
}


WORD CheckTimeLock( time_t tTime,           // time found in ini file.
                    time_t tMasterTime )    // time found in master lock.
/*****************************************************************************
    Valid time check:
    
    1 ) ini and master timestamps are the same.
    2 ) today's date exceeds the timestamp plus TIME_LIMIT # of days;
    3 ) the time stamp date stamp exceeds today's date( it's been tampered w/ );

    For unsuccessfull checks the return has the Intruder bit set if the 
    dates where found to be tampered with.
*****************************************************************************/
{
    WORD wDaysLeft = 0;
    CTime tTimeStamp ( tTime );                 // ini time stamp.
    CTime tTimeStamp2( tMasterTime );           // master lock time stamp.
    CTime tToday = CTime::GetCurrentTime();     // get current.

    if(( tTimeStamp == tTimeStamp2 ) &&( tTimeStamp < tToday ))
    { 
        if( tToday < tTimeStamp + CTimeSpan( TIME_LIMIT, 0, 0, 0 ))
        {
            CTimeSpan tsSpan = tToday - tTimeStamp;

            wDaysLeft =( WORD )TIME_LIMIT -( WORD )tsSpan.GetDays();
        }
    }
    else
        wDaysLeft |= INTRUDER_BIT; // its been tampered with.

    return wDaysLeft;
}


BOOL OpenLocks( WORD &wDaysLeft,        // days left on trial.
                WORD &wTrialsLeft,      // how many trials left.
                BOOL &bIntruder )       // intruder detected.
/*****************************************************************************
    This checks and validates all the lock mechanisms and detects if 
    and intruder has been modifying the locks. It returns true if all
    is ok and the locks are opened. If successfull the first two params
    will be filled in with info about remaining days and trials. If 
    unsuccessfull the bIntruder param will indicate if the trial count
    or date is over or that an intruder caused the failure.
*****************************************************************************/
{
    FNAME szTemp;

    wDaysLeft = 0;    // initialize to zero in case of errors.
    wTrialsLeft = 0;  // initialize to zero in case of errors.  
    bIntruder = TRUE; // assume we have been violated until we open all locks.

    // Open dummy dll and read master lock out of dummy.
    // If files do not exists assume hacker is at fault.

    if( OpenDummyFile( OF_EXIST ) != HFILE_ERROR ) 
    {
        HFILE hFile = OpenDummyFile( OF_READ );

        if( hFile != HFILE_ERROR )
        {
            SMasterLock Master;

            if( _lread( hFile, &Master, sizeof( SMasterLock )) == 
                sizeof( SMasterLock ))
            {
                //  Has hacker been detected previously.

                if( !Master.bIntruderDetected )
                {
                    if( GetProfileString( WININI_COLOR_SECTION, 
                        DUMMY_COLOR_NAME, _T(""), szTemp, sizeof( szTemp )))
                    {
                        // if it is here, make sure that a hacker 
                        // has not mucked with the dummy value,  or, 
                        // see if we have set it to another setting to 
                        // indicate an intruder alert form a previous try.

                        if( lstrcmpi( szTemp, DUMMY_COLOR_GOOD_VALUE ) == 0 )
                        {
                            // Check the date.
                             
                            if( GetDefaultString( DATE_KEYNAME, _T(""), szTemp, sizeof( szTemp )))
                            {
                                wDaysLeft = CheckTimeLock( 
                                   ( time_t )atol( szTemp ), 
                                    Master.TimeStarted );

                                if( wDaysLeft & INTRUDER_BIT )
                                    wDaysLeft &~ INTRUDER_BIT;
                                else
                                    bIntruder = FALSE;
                               
                                // If still ok check the count.

                                if( !bIntruder && wDaysLeft )
                                {
									UINT Value;
                                    WORD wCodedCount;
								    FNAME szTemp;
    
								    GetDefaultString( COUNT_KEYNAME, _T(""), szTemp, sizeof( szTemp ));
									sscanf(szTemp, "%u", &Value);
									wCodedCount = (WORD)Value;

                                    if( wCodedCount )
                                    {
                                        wTrialsLeft = CheckCountLock( 
                                            wCodedCount, Master.TrialCount );

                                        if( wTrialsLeft & INTRUDER_BIT )
                                        {
                                            wTrialsLeft &~ INTRUDER_BIT;
                                            bIntruder = TRUE;
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
            _lclose( hFile ); // close dummy dll with master lock.
        }
    }
    return( !bIntruder && wDaysLeft && wTrialsLeft );
}


/****************************** Public Functions ****************************/

void CleanLocks()
/*****************************************************************************
    Delete all locks. That is, we delete the dummy dll and remove 
    any bogus INI entries from the PP and windows profile files.
*****************************************************************************/
{
    if( OpenDummyFile( OF_EXIST ) != HFILE_ERROR )
        OpenDummyFile( OF_DELETE );

    WriteProfileString( WININI_COLOR_SECTION, DUMMY_COLOR_NAME, NULL );
    PutDefaultString( DATE_KEYNAME, NULL );
    PutDefaultString( COUNT_KEYNAME, NULL );
}


BOOL FreeTrialStartup( WORD &wDaysLeft,     // if all locks valid, set to number of days left.
                       WORD &wTrialsLeft,   // if all locks valid, set to number of trials left.
                       BOOL &bIntruder,     // set to TRUE if intruder detected.
                       BOOL &bFirstTime )   // set to TRUE if first time on system.
/*****************************************************************************
    This function is called by the program startup routine to initialize 
    or check the lock mechanisms. It will return TRUE if the
    program should continue to run. If FALSE the bIntruder flag is set 
    to indicate that the locks have been modified or missing.
*****************************************************************************/
{
    BOOL bResult = FALSE;
    wDaysLeft = 0;
    wTrialsLeft = 0;
    bIntruder = FALSE;
    bFirstTime = FALSE;

    if( NoLocksInstalled())     // clean system - lets set it up.
    {
        wDaysLeft = TIME_LIMIT;
        wTrialsLeft = TRIAL_LIMIT;
        bFirstTime = TRUE;
        bResult = TRUE;
        InstallLocks();
    }
    else // lets see if we will allow the program to run.
    {
        bResult = OpenLocks( wDaysLeft, wTrialsLeft, bIntruder );

        if( bResult )
            UpdateCountLock( TRIAL_LIMIT - wTrialsLeft + 1 );  
        else 
            UpdateCountLock( TRIAL_LIMIT );  // time and trials up.
    }

    if( bIntruder )                                 
        SetIntruderLocks(); // teach him from hacking with us!
    else
        bInitialized = TRUE;

    return bResult;
}


BOOL FreeTrialFailSafe()
/*****************************************************************************
    Call this sometime after calling FreeTrialStartup to asuure that some 
    one has not jumped over startup call.
*****************************************************************************/
{   
    if( !bInitialized )
        SetIntruderLocks();     // teach him to hack us!

    return bInitialized;
}


#ifdef UNUSED
BOOL RunSlideShow()
{
    BOOL bResult = FALSE;
    CString strModulePathname;
    TCHAR *pPath = strModulePathname.GetBuffer( _MAX_PATH );

    if( pPath )
    {
        int Pos;

        // Build up full pathname of slideshow. This is located in the 
        // same dir as the execuatble.

        GetModuleFileName( AfxGetInstanceHandle(), pPath, _MAX_PATH );
        strModulePathname.ReleaseBuffer();
        Pos = strModulePathname.ReverseFind( _T('\\') );
        
        if( Pos != -1 )
        {                
            CString strSlideShow;
            OFSTRUCT of;
            
            strSlideShow = strModulePathname.Left( Pos+1 );
            strSlideShow += SLIDESHOW;

            if( OpenFile( strSlideShow, &of, OF_EXIST ) != HFILE_ERROR )
            {
                WinExec( strSlideShow, SW_SHOW ); // spawn slideshow.
                bResult = TRUE;
            }
            else
                MessageBeep( 0 );
        }
    }   
    else
        strModulePathname.ReleaseBuffer();

    return bResult;
}
#endif // UNUSED

/****************************************************************************/

