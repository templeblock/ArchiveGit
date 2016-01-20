/*****************************************************************************

       Copyright (c) Micrografx, Inc., 1993 - 1994.  All rights reserved.
    Permission to use this work for any purpose must by obtained in writing
           from Micrografx, 1303 Arapaho Rd, Richardson, Texas 75081.

******************************************************************************
******************************************************************************

                                   register.c

******************************************************************************
******************************************************************************

  Description:  Register the app with Micrografx.
 Date Created:  2/8/94
       Author:  Ray

*****************************************************************************/

#include <windows.h>
#include <stdlib.h>
#include <math.h>
#include "id.h"
#include "data.h"
#include "routines.h"
#include "regkey.h"     // ref directory

extern HINSTANCE hInstAstral;

#define BASE10

#ifdef BASE36
	#define BASE 36
#else
#ifdef BASE26
	#define BASE 26
#else
	#define BASE 10
#endif
#endif

/********************************* Constants ********************************/
// The entries listed below are fictious entries used to disguise the key
// and codes stored in the WIN.INI file.
// There are two types of lock checking:
// Locked activities, and locked programs on CD
//									* Activites *	* Programs *
static LPSTR lpKeySection[]		= { "windows",		"windows"		};
static LPSTR lpKeyEntry[]		= { "DeviceLimits",	"DeviceStart"	};
static LPSTR lpCodeSection[]	= { "intl",			"intl"			};
static LPSTR lpCodeEntry[]		= { "iZoneIDs",		"iZoneTime"		};
static ITEMID idDialog[]		= { IDD_ASK_REGISTER,IDD_ASK_UNLOCK	};
static int iType;

/****************************** Local Functions ****************************/
bool AskRegistrationCode( void );
bool CheckKeyCode(LPSTR szKey, LPSTR szCode);
BOOL WINPROC EXPORT DlgRegisterProc(HWND hWnd,UINT wMsg,WPARAM wParam,LPARAM lParam);
void CreateNewLicense(PLICENSEINFO pLicenseInfo);
/****************************** Public Functions ****************************/

/****************************************************************************/
bool CheckRegistration(bool bAskforCode)
/****************************************************************************/
{
    STRING szKey;
    STRING szCode;
    int len; 
    
	iType = GetLockType();
	if ( iType != 0 && iType != 1 )
		iType = 0;
    len = GetProfileString(lpKeySection[iType], lpKeyEntry[iType], "",  szKey, sizeof(szKey));
    if (len)
    {    
        GetProfileString(lpCodeSection[iType], lpCodeEntry[iType], "",  szCode, sizeof(szCode));
        if (CheckKeyCode(szKey, szCode))
            return TRUE;
    }
        
    return bAskforCode ? AskRegistrationCode() : FALSE;
}


/****************************************************************************/
int GetLockType( void )
/****************************************************************************/
{
FNAME szFileName;

// Return 0 if no LOK file is found; this means locked activities
// Return 1 if a LOK file is found; this means a locked CD program
lstrcpy( szFileName, Control.ProgHome );
#ifdef STUDIO
	lstrcat( szFileName, "STUDIO.LOK" );
#else
	lstrcat( szFileName, "ADVENTUR.LOK" );
#endif
return( FileExists( szFileName ) );
}


/****************************************************************************/
bool CheckKeyCode(LPSTR szKey, LPSTR szCode)
/****************************************************************************/
{
    STRING szTemp;
    bool bGoodCode;
    
    MGXGenerateLicenseKey(szCode, 0, 0, 0, szTemp);
    bGoodCode = (lstrcmpi(szTemp, szKey) == 0);

	//return FALSE; // ask for code every time you enter the room.

    return bGoodCode;
}


/****************************************************************************/
bool AskRegistrationCode( void )
/****************************************************************************/
{
    HWND hWnd = GetActiveWindow();
	return AstralDlg( NO, hInstAstral, hWnd, idDialog[iType], (DLGPROC)DlgRegisterProc );
}


/****************************************************************************/
BOOL WINPROC EXPORT DlgRegisterProc(HWND hWnd,UINT wMsg,WPARAM wParam,LPARAM lParam)
/****************************************************************************/
{
    BOOL bHandled = TRUE;
    FNAME szCode;
    FNAME szKey;
    FNAME szTemp;
    BYTE bChecksum = 0;
    bool bGoodCode;
	int  len;
	LICENSEINFO AppLicenseInfo;

    switch (wMsg)
    {
        case WM_COMMAND:
#ifdef WIN32
            switch (LOWORD(wParam))
#else
            switch (wParam)
#endif
            {
        	    case IDOK:
                    szCode[0] = 0;
                    GetDlgItemText(hWnd, IDC_REG_KEY,  szKey,  sizeof(szKey));
                    GetDlgItemText(hWnd, IDC_REG_CODE, szCode, sizeof(szCode));
                    MGXGenerateLicenseKey(szCode, 0, 0, 0, szTemp);
                    bGoodCode = (lstrcmpi(szTemp, szKey) == 0);
                    if (!bGoodCode)
                    {
                        
                        LoadString (hInstAstral,IDS_REG_CODE_MSG,szKey,sizeof(szKey));
                        LoadString (hInstAstral,IDS_REG_CODE_TITLE,szCode,sizeof(szCode));
                        MessageBox(hWnd, szKey, szCode, MB_OK | MB_ICONEXCLAMATION);
                    }
                    else
                    {
                        WriteProfileString(lpCodeSection[iType], lpCodeEntry[iType], szCode);
                        WriteProfileString(lpKeySection[iType],  lpKeyEntry[iType],  szKey);
        		        AstralDlgEnd(hWnd, YES);
                    }
        		break;

        	    case IDCANCEL:
        		    AstralDlgEnd(hWnd, NO);
        		break;
                
                default:
                    bHandled = FALSE;
            } // switch (wParam)
            break;

		case WM_CTLCOLOR:
			return ( (BOOL)SetControlColors ( (HDC)wParam, hWnd,
				(HWND)LOWORD(lParam), HIWORD(lParam) ));

        case WM_INITDIALOG:
			SET_CLASS_HBRBACKGROUND(hWnd, ( HBRUSH )GetStockObject(LTGRAY_BRUSH));
			CenterPopup (hWnd);
     		len = GetProfileString(lpCodeSection[iType], lpCodeEntry[iType], "",  szCode, sizeof(szCode));
			if (!len)
			{
	        	CreateNewLicense(&AppLicenseInfo);
            	LicenseToString(&AppLicenseInfo, szCode);
				WriteProfileString(lpCodeSection[iType], lpCodeEntry[iType], szCode);
			}
            SetDlgItemText(hWnd, IDC_REG_CODE, szCode);
            break;

        case WM_DESTROY:
            break;
            
        default:
            bHandled = FALSE;
    } // switch (wMsg)

    return bHandled;
}


//==========================================================================//
// Convert a license info structure to a null terminated string.            //
//==========================================================================//
void    LicenseToString(PLICENSEINFO pLicenseInfo, LPSTR lpszLicenseString)
{
    int     i;
    DWORD   dwNum;

    for (i = 0; i < sizeof(LICENSEINFO) - 1; i++)
        ((LPBYTE)pLicenseInfo)[i] ^= pLicenseInfo->bChecksum;

#ifdef _MAC
	swapl((LPDWORD)pLicenseInfo);
#endif	
    dwNum = ((LPDWORD)pLicenseInfo)[0];
#ifdef _MAC
	swapl((LPDWORD)pLicenseInfo);
#endif
	
    for (i = MAX_LICENSE_INFO - 1; i >= 0; i--)
    {
	#ifdef BASE36
        lpszLicenseString[i] = (char) ((dwNum % BASE) + '0');
        if (lpszLicenseString[i] > '9')
            lpszLicenseString[i] += 7;
	#else
	#ifdef BASE26
        lpszLicenseString[i] = (char) ((dwNum % BASE) + 'A');
	#else
        lpszLicenseString[i] = (char) ((dwNum % BASE) + '0');
	#endif
	#endif
        dwNum /= BASE;
    }
    lpszLicenseString[MAX_LICENSE_INFO] = 0;
}



//==========================================================================//
// Convert a null terminated string to a license info structure.            //
//==========================================================================//
void    StringToLicense(LPSTR lpszLicenseString, PLICENSEINFO pLicenseInfo)
{
    int     i;
    DWORD   dwNum = 0;
    BYTE    bByte;

    for (i = 0; i < MAX_LICENSE_INFO; i++)
    {
        bByte = toupper(lpszLicenseString[i]);
		#ifdef BASE36
	        if (bByte >= 'A')
	            bByte -= 7;
	        bByte -= '0';
		#else
		#ifdef BASE26
	        bByte -= 'A';
		#else
	        bByte -= '0';
		#endif
		#endif
        dwNum = (dwNum * BASE) + bByte;
    }
    ((LPDWORD)pLicenseInfo)[0] = (DWORD) dwNum;
#ifdef _MAC
	swapl((LPDWORD)pLicenseInfo);
#endif	

    for (i = 0; i < sizeof(LICENSEINFO) - 1; i++)
        ((LPBYTE)pLicenseInfo)[i] ^= pLicenseInfo->bChecksum;
}


//==========================================================================//
// Decode the specified license information string and return a key for the //
// new license count.                                                       //
//==========================================================================//
int     MGXGenerateLicenseKey(LPSTR lpszLicenseInfo, WORD wNewLicenseCount,
                              LPWORD lpwLicenseCount, LPBYTE lpbApp,
                              LPSTR lpszKey)
{
    LICENSEINFO LicenseInfo;
    BYTE        bChecksum = 0;
    int         i;
    int         RetCode = 0;

    //----------------------------------------------------------------------//
    // Decode license info.                                                 //
    //----------------------------------------------------------------------//
    StringToLicense(lpszLicenseInfo, &LicenseInfo);

    //----------------------------------------------------------------------//
    // Verify the checksum.                                                 //
    //----------------------------------------------------------------------//
    for (i = 0; i < sizeof(LicenseInfo) - 1; i++)
        bChecksum ^= ((LPBYTE)&LicenseInfo)[i];
    if (bChecksum != LicenseInfo.bChecksum)
        RetCode = LICENSE_ERR_BADDATA;
    else
    {
        //------------------------------------------------------------------//
        // Return the requested license information.                        //
        //------------------------------------------------------------------//
        if (lpszKey)
        {
//          LicenseInfo.bLicenseCount = 255 - DEFAULT_LIC_COUNT;
//          LicenseInfo.bApp = 255 - DEF_APP_COUNT;
	        LicenseInfo.bNull = 255;
            LicenseInfo.bChecksum = 0;
            for (i = 0; i < sizeof(LicenseInfo) - 1; i++)
                LicenseInfo.bChecksum ^= ((LPBYTE)&LicenseInfo)[i];
            LicenseToString(&LicenseInfo, lpszKey);
        }
    }

    return (RetCode);
}


void CreateNewLicense(PLICENSEINFO pLicenseInfo)
{
    int i;
    if (!pLicenseInfo) return;
    
//  pLicenseInfo->bLicenseCount = DEFAULT_LIC_COUNT;
//  pLicenseInfo->bApp = DEF_APP_COUNT;
	pLicenseInfo->bNull = 0;
    pLicenseInfo->bChecksum = 0;
    pLicenseInfo->wTickCount = (WORD)GetTickCount();
#ifdef _MAC
	swapw(&pLicenseInfo->wTickCount);
#endif	
    for (i = 0; i < sizeof(LICENSEINFO) - 1; i++)
        pLicenseInfo->bChecksum ^= ((LPBYTE)pLicenseInfo)[i];
}

/****************************************************************************/

