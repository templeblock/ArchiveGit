#include <windows.h>
#include <dos.h>
#include <time.h>
#include "types.h"
#include "id.h"
#include "routines.h"

// Static prototypes
static BOOL IsDemo( LPSTR lpProgram );
static int day( void );

/************************************************************************/
BOOL Bomb( HINSTANCE hInstance )
/************************************************************************/
{
#ifdef _MAC
	return FALSE;
#endif
FNAME sz;
BOOL bDisable, bPutDay1, bPutDay2;
int iDay, iDay1, iDay2, iDaysInstalled;
STRING szMessage;

GetModuleFileName( hInstance, sz, sizeof(sz)-1);
if ( !IsDemo( sz ) )
	return( NO );

iDay = day();
iDay1 = GetProfileInt( "windows", "DeviceID", -1 );
iDay2 = GetDefaultInt( "DeviceID", -1 );

bPutDay1 = (iDay1 < 0); // If they can't be found, we'll write them out
bPutDay2 = (iDay2 < 0);
if ( bPutDay1 && bPutDay2 ) // First time running
	iDay1 = iDay2 = iDay;
else
if ( bPutDay1 ) // Windows was reinstalled (or different version)
	iDay1 = iDay2;
else
if ( bPutDay2 ) // PP was reinstalled
	iDay2 = iDay1;

if ( bPutDay1 )
	WriteProfileString( "windows", "DeviceID", itoa(iDay1,sz,10) );
if ( bPutDay2 )
	PutDefaultInt( "DeviceID", iDay2 );

iDaysInstalled = iDay - min( iDay1, iDay2 );
if ( bDisable = (iDaysInstalled < 0 || iDaysInstalled >= 30) )
	lstrcpy( szMessage, "\tTHIS COPY HAS EXPIRED" );
else	wsprintf( szMessage, "You have %d days before this copy expires",
		30 - iDaysInstalled );

Print( "Thank you for trying out this 30 day working copy of Crayola software.\n\n%ls.\n\nTo order your full version, use the enclosed offer card or call:\n\n\tUSA, Canada\t1-800-598-1930\n\tUnited Kingdom\t(+44) 0 483 747526\n\tScandanavia\t(+45) 43 43 26 77\n\tFrance\t\t(+33) 1 69 18 19 50\n\tGermany\t\t(+49) 089 260 3830\n\tAustralia/N.Z.\t(+61) 2 415 2642\n\tOthers call USA\t1-214-234-1769\n\t\t(rotary)\t1-214-234-6018",
	(LPSTR)szMessage );

return( bDisable );
}


typedef struct {
	BYTE system[16];
	unsigned time;
	unsigned date;
	BYTE volume[14];
} PID;



/***********************************************************************/
static BOOL IsDemo( LPSTR lpProgram )
/***********************************************************************/
{
#ifndef _MAC
int         i;
int			fp;
char        buf[32];
#ifndef FileOpen
OFSTRUCT 	ofstruct;
#endif

#define	DEMO "xgm"
#define	PROTECT "lartsa"
#define SZPROTECT 6
#define SYSCHAR (BYTE far *)0xffff0000

#ifdef FileOpen
if ( ( fp = FileOpen( lpProgram, FO_READ ) ) == FILE_HANDLE_INVALID )
	return( 0 );	/* something is wierd */
if ( FileSeek( fp, -(long)sizeof(buf), SEEK_END ) < 0 )
	*buf = '\0';	/* goto the bottom bytes */
if ( FileRead( fp, buf, sizeof(buf) ) != sizeof(buf) )
	*buf = '\0';	/* something is wierd */
FileClose( fp );
#else
if ( ( fp = OpenFile( lpProgram, &ofstruct, OF_READ ) ) < 0 )
	return( 0 );	/* something is wierd */
if ( _llseek( fp, -(long)sizeof(buf), SEEK_END ) < 0 )
	*buf = '\0';	/* goto the bottom bytes */
if ( _lread( fp, buf, sizeof(buf) ) != sizeof(buf) )
	*buf = '\0';	/* something is wierd */
_lclose( fp );
#endif // FileOpen

for ( i=0; i<sizeof(buf)-1; i++ )
	{ // Zap any non-ascii characters
	if ( buf[i] < 'a' || buf[i] > 'z' )
		buf[i] = '.';
	}
buf[i] = '\0';

if ( lstrfind( buf, DEMO ) ) // Is it a crippled demo?
	return( IDS_CRIPPLED );

#endif // _MAC

return( FALSE );
}



/************************************************************************/
static int day(void)
/************************************************************************/
{
struct tm sTime;
struct tm * pTime;
time_t dwTime, dwJan1;

// get the time at Jan1
sTime.tm_sec = sTime.tm_min = sTime.tm_hour = 0; // Time is midnight
sTime.tm_mon = 0; sTime.tm_mday = 1; sTime.tm_year = 80; // Date is 1/1/80
sTime.tm_yday = sTime.tm_isdst = sTime.tm_wday = 0;
dwJan1 = mktime( &sTime );
//printf("Jan1 %ld\n", dwJan1 );

// get the current time
time( &dwTime );
//printf("Now %ld\n", dwTime );

// make it relative to Jan1_80
dwTime -= dwJan1;
//printf("Modified Now %ld\n", dwTime );

// break it down into fields
pTime = gmtime( &dwTime );
// return days since first of the year
//printf("Day %d\n", pTime->tm_yday );

return( pTime->tm_yday );
}


#ifdef UNUSED
/* If the program is crippled, try to load a demo message file */
/* If there isn't one, put up the crippled message */
//if ( Control.Crippled )
//	if ( !InstallPlayer( "DEMO.SHO" ) )
//		Message( Control.Crippled );

//time( &dwTime );	/* get the current system time */
//if ( dwTime < 0x28E8C082L /* 10/1/91 */ ||
//   dwTime > 0x2958DDA8L /* 12/25/91 */ )
//	{
//	Print( "This beta version of Picture Publisher has expired.  Please contact the Micrografx sales office for order information.");
//	return(0);
//	}
#endif
