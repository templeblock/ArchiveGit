//®PL1¯®FD1¯®TP0¯®BT0¯
// (c) Copyright 1991 MICROGRAFX, Inc., All Rights Reserved.
// This material is confidential and a trade secret.
// Permission to use this work for any purpose must be obtained
// in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081
#include "pp.h"
#include "ppafx.h"
#include "id.h"

// Static prototypes
static  char far *GetNextParm(char far *lpString);

#define KEY_LENGTH	512   /* Allows for at least 20 device keynames */
#define PRINTERS	"PrinterPorts"

LOCAL STRING NullPort;
LOCAL STRING Separator;

/***********************************************************************/
short InitPrinterListBox( HWND hDlg, LPSTR lpDevice )
/***********************************************************************/
  /* This function initializes the list of devices in the GetDevice dialog box.
     The list box in the dialog box contains a list of all devices installed in
     the system.  The user will select from among these various strings to
     select the current printer.  The number of printers listed in the list
     box is returned as a result.  The currently selected device (lpDevice)
     should have the form "NAME,DRIVER,PORT", and will be highlighted initially
     in the list box. */
{
short nDevice;
char KeyBuffer[KEY_LENGTH];
STRING szDevice, Buffer, szString;
LPSTR lp, lpName, lpDriver, lpPort, lpNext;
LPSTR lpActiveName, lpActiveDriver, lpActivePort;
int CurrDevice; /* index of currently selected device */

if ( !(*lpDevice) )
	GetDefaultPrinter( lpDevice );
// Format of the device string is NAME,DRIVER,PORT etc.

lstrcpy( szDevice, lpDevice ); // Copy the device string so we won't destroy it

if ( AstralStr( IDS_PORT_SEPARATOR, &lp ) )
	lstrcpy( Separator, lp );

lpActiveName = szDevice; // get the name
if ( lpActiveDriver = GetNextParm( lpActiveName ) ) // get the driver
	{
	*lpActiveDriver++ = '\0';
	if ( lpActivePort = GetNextParm( lpActiveDriver ) ) // get the port
		*lpActivePort++ = '\0';
	}

nDevice = 0;
CurrDevice = -1;
GetProfileString( PRINTERS, NULL, "\0\0", KeyBuffer, KEY_LENGTH );
lpName = KeyBuffer;
while ( *lpName )
	{
	GetProfileString( PRINTERS, lpName, "\0", Buffer, MAX_STR_LEN );
	// Format of the buffer is DRIVER,PORT,15,45,PORT,15,45,PORT,15,45 etc.
	lpDriver = Buffer;
	lpPort = GetNextParm( lpDriver ); // skip over driver to get the port
	while ( lpPort )
		{
		*lpPort++ = '\0';
		if ( lpNext = GetNextParm( lpPort ) )
			*lpNext++ = '\0';
		lstrcpy( szString, lpName );
		lstrcat( szString, Separator );
		lstrcat( szString, lpPort );
		if ( hDlg )
			SendDlgItemMessage( hDlg, IDC_PRINTSELECT,
				LB_INSERTSTRING, -1, (long)(LPSTR)szString );
		if ( lpActiveName && !lstrcmp(lpActiveName,lpName) &&
		     lpActivePort && !lstrcmp(lpActivePort,lpPort) )
			CurrDevice = nDevice;
		nDevice++;
		if ( lpNext = GetNextParm( lpNext ) ) // skip the 15
			lpNext++;
		lpPort = GetNextParm( lpNext ); // skip the 45
		}
	lpName += (lstrlen(lpName) + 1);
	}

if ( !nDevice && hDlg )
	{
	GetProfileString( "windows", "NullPort", "None", NullPort, MAX_STR_LEN);
	SendDlgItemMessage( hDlg, IDC_PRINTSELECT, LB_INSERTSTRING, -1,
		(long)(LPSTR)NullPort );
	nDevice++;
	}

if ( CurrDevice < 0 ) // the name wasn't in the list, so pass back the default
	GetDefaultPrinter( lpDevice );

if ( hDlg )
	{
	if ( CurrDevice < 0 ) // the name wasn't in the list...
		SendDlgItemMessage( hDlg, IDC_PRINTSELECT, LB_SELECTSTRING,
			-1, (long)lpDevice );
	else	SendDlgItemMessage( hDlg, IDC_PRINTSELECT, LB_SETCURSEL,
			CurrDevice, 0L );
	}

return( nDevice );
}


/***********************************************************************/
void GetPrinterListBox( HWND hDlg, LPSTR lpDevice )
/***********************************************************************/
  /* This function retrieves the current selection from the dialog box.  If
     no string is selected in the list box, an empty string is the result.
     Otherwise, the device string buffer is returned as a result.  The
     device string buffer will contain a copy of the selected string. */
{
STRING szEntry, szDriver;
int idItem;
LPSTR lpName, lpPort, lpNext;
short SepLength;
char End;
BOOL bMatch;

idItem = SendDlgItemMessage( hDlg, IDC_PRINTSELECT, LB_GETCURSEL, 0, 0L );
if ( idItem < 0 )
	{
	*lpDevice = '\0';
	return;
	}

SendDlgItemMessage( hDlg, IDC_PRINTSELECT, LB_GETTEXT, idItem,
	(long)(LPSTR)szEntry );
if ( !lstrcmp( szEntry, NullPort ) )
	{
	*lpDevice = '\0';
	return;
	}

lpName = szEntry;
lpPort = lpName;
SepLength = lstrlen( Separator );
while ( *lpPort )
	{
	End = lpPort[SepLength];
	lpPort[SepLength] = '\0';
	bMatch = !lstrcmp( lpPort, Separator );
	lpPort[SepLength] = End;
	if ( bMatch )
		break;
	lpPort++;
	}

if ( !*lpPort )
	{
	*lpDevice = '\0';
	return;
	}
*lpPort = '\0';
lpPort += SepLength;

GetProfileString( PRINTERS, lpName, "\0", szDriver, MAX_STR_LEN );
// Format of the device is DRIVER,PORT,15,45,PORT,15,45,PORT,15,45 etc.
if ( !(lpNext = GetNextParm( szDriver )) )
	{
	*lpDevice = '\0';
	return;
	}
*lpNext = '\0';

lstrcpy( lpDevice, lpName );
lstrcat( lpDevice, "," );
lstrcat( lpDevice, szDriver );
lstrcat( lpDevice, "," );
lstrcat( lpDevice, lpPort );
}


/***********************************************************************/
void GetDefaultPrinter( LPSTR lpDevice )
/***********************************************************************/
{ // The currently selected device will have the form "NAME,DRIVER,PORT"
GetProfileString( "windows", "device", "\0", lpDevice, MAX_STR_LEN );
}


/***********************************************************************/
void SetDefaultPrinter( LPSTR lpDevice )
/***********************************************************************/
{ // The currently selected device will have the form "NAME,DRIVER,PORT"
LPSTR lpDriver, lpPort, lpName, lpNext;
LPSTR lpActiveDriver, lpActivePort, lpActiveName;
STRING szDevice, szString, Buffer;
char KeyBuffer[KEY_LENGTH];

if ( !lpDevice || !*lpDevice )
	return;

lstrcpy( szDevice, lpDevice ); // Copy the device string so we won't destroy it
lpDevice = szDevice;

WriteProfileString( "windows", "device", lpDevice );
SendMessage( (HWND)-1, WM_WININICHANGE, 0, (LPARAM)(LPSTR)"windows" );

lpActiveName = lpDevice;
if ( !(lpActiveDriver = GetNextParm( lpActiveName )) )
	return;
*lpActiveDriver++ = '\0';
if ( !(lpActivePort = GetNextParm( lpActiveDriver )) )
	return;
*lpActivePort++ = '\0';

GetProfileString( PRINTERS, NULL, "\0\0", KeyBuffer, KEY_LENGTH );
lpName = KeyBuffer;
while ( *lpName )
	{
	GetProfileString( PRINTERS, lpName, "\0", Buffer, MAX_STR_LEN );
	// Format of the buffer is DRIVER,PORT,15,45,PORT,15,45,PORT,15,45 etc.
	lpDriver = Buffer;
	if ( lpNext = GetNextParm( lpDriver ) )
		*lpNext++ = '\0';
	lstrcpy( szString, lpDriver );
	while ( lpPort = lpNext )
		{
		if ( lpNext = GetNextParm( lpPort ) )
			*lpNext++ = '\0';
		lstrcat( szString, "," );
		if ( !lstrcmp( lpPort, lpActivePort ) &&
		     ( lstrcmp( lpName, lpActiveName ) ||
		       lstrcmp( lpDriver, lpActiveDriver ) ) )
			lstrcat( szString, NullPort );
		else	lstrcat( szString, lpPort );
		if ( lpNext = GetNextParm( lpNext ) ) // skip the 15
			lpNext++;
		if ( lpNext = GetNextParm( lpNext ) ) // skip the 45
			lpNext++;
		}
	WriteProfileString( "devices", lpName, szString );
	lpName += (lstrlen(lpName) + 1);
	}

SendMessage( (HWND)-1, WM_WININICHANGE, 0, (long)(LPSTR)"devices" );
}


/***********************************************************************/
short SetupPrinter( HWND hWindow, LPSTR lpDevice )
/***********************************************************************/
  /* This function causes the named device's set-up dialog box to be displayed
     to the user.  The parameter 'lpDevice' contains a device
     specification in the form used in the user profile for specifying the
     current device for windows (NAME,DRIVER,PORT).  This
     function returns TRUE if the device is set correctly, FALSE if the user
     cancels selection of device attributes, and -1 if the device dialog box
     cannot be displayed. */
{
HMODULE hDriver;
short Result;
LPSTR lpName, lpDriver, lpPort, lpNext;
STRING szDevice, szDriver;
int (FAR PASCAL *lpDeviceModes)(HWND, HMODULE, LPSTR, LPSTR);

if ( !lpDevice || !*lpDevice )
	return( -1 );

lstrcpy( szDevice, lpDevice ); // Copy the device string so we won't destroy it
lpDevice = szDevice;

// Format of the device string is NAME,DRIVER,PORT
lpName = lpDevice;
if ( !(lpDriver = GetNextParm( lpName )) )
	return( -1 );
*lpDriver++ = '\0';
if ( !(lpPort = GetNextParm( lpDriver )) )
	return( -1 );
*lpPort++ = '\0';
if ( lpNext = GetNextParm( lpPort ) )
	*lpNext = '\0';

lstrcpy( szDriver, lpDriver );
lstrcat( szDriver, ".DRV" );

if (Result = ((hDriver = (HMODULE)AstralLoadLibrary( szDriver )) != NULL))
	{ // replace 13L with "DEVICEMODE"
	if ( lpDeviceModes = (int (FAR PASCAL *)(HWND, HMODULE, LPSTR, LPSTR))GetProcAddress( hDriver, (LPSTR)13L ) )
		{
	    Result = (*lpDeviceModes)( PictPubApp.Get_hWndAstral(), hDriver, (LPSTR)lpName, (LPSTR)lpPort );
		}
	FreeLibrary( hDriver );
	}

return( Result );
}


/***********************************************************************/
void ValidatePrinter( LPSTR lpDevice )
/***********************************************************************/
{
InitPrinterListBox( NULL, lpDevice );
}


/***********************************************************************/
void DeviceDescription( LPSTR lpDevice, LPSTR lpDescription )
/***********************************************************************/
//   The passed in device (lpDevice) should have the form "NAME,DRIVER,PORT"
{
LPSTR lpName, lpDriver, lpPort, lpSeparator;
STRING szDevice;

ValidatePrinter( lpDevice );
*lpDescription = '\0';
if ( !AstralStr( IDS_PORT_SEPARATOR, &lpSeparator ) )
	return;
lstrcpy( szDevice, lpDevice );
lpName = szDevice;
if ( !(lpDriver = GetNextParm( lpName )) ) // skip over name to get the driver
	return;
*lpDriver++ = '\0';
if ( !(lpPort = GetNextParm( lpDriver )) ) // skip over driver to get the port
	return;
*lpPort++ = '\0';
lstrcpy( lpDescription, lpName );
lstrcat( lpDescription, lpSeparator );
lstrcat( lpDescription, lpPort );
}


/***********************************************************************/
LOCAL LPSTR GetNextParm( LPSTR lpString )
/***********************************************************************/
/*   This function searches the given string for the location of the next
     printer parameter.  It returns a pointer to the parameter.
     If there are no more parameters, NULL is returned. */
{
if ( !lpString )
	return( NULL );
while ( *lpString )
	{
	if ( *lpString == ',' )
		return( lpString );
	lpString++;
	}
return( NULL );
}


