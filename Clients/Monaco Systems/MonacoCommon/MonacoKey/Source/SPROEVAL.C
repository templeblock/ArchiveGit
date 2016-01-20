/***************************************************************************
* SPROEVAL.C
*
* (C) Copyright 1989-1993 Rainbow Technologies, Inc. All rights reserved.
*
* Description : SentinelSuperPro Evaluation Program for WIN16/WIN32s/WIN32.
*
* Purpose     : To demostrate the SentinelSuperPro API for C languages
*               running under Windows.
****************************************************************************/
#include <windows.h>               /* for all Window apps                  */
#include <stdio.h>                 /* for sscanf()                         */
#include <string.h>                /* for strcpy()                         */
#include <ctype.h>                 /* for isxdigit() isascii()             */
#include "spromeps.h"              /* SuperPro API function prototypes     */
#include "sproeval.h"              /* resource defines                     */

#if (defined(WIN32) || defined(_WIN32) || defined(_WIN32_))
#undef _WIN32_
#define _WIN32_
#endif

#if     defined(_MSC_VER)          /* Microsoft C/C++ compiler             */
#ifdef _WIN32_
#define EXPORT
#else                              /* for WIN16                            */
#define EXPORT _export             /* for exported callback routines       */
#endif
#elif  defined(__BORLANDC__)       /* Borland C/C++ Compiler               */
#define EXPORT _export             /* for exported callback routines       */
#endif

#if    defined(__cplusplus)
#define EXPORT_PROC    extern "C"
#else
#define EXPORT_PROC    extern
#endif
#define LOCAL_PROC     static

#define START_EVAL      0x1234     /* Message to start eval program        */
#define PPROC           FARPROC    /* for MakeProcInstance()               */

#ifdef _WIN32_
#define MESSAGE         UINT
#define GET_LOWORD(param) LOWORD(param)
#define GET_HIWORD(param)  HIWORD(param)
#else                              /* Windows 16 bit                       */
#define MESSAGE         WORD
#define WPARAM          WORD
#define GET_LOWORD(param)  param
#define GET_HIWORD(param)   param
#endif

#define STR_BUF_SIZE     255

#ifdef _WIN32_
#define xFreeProcInstance( param )
#else
#define xFreeProcInstance( param )   FreeProcInstance( param )
#endif

RB_SPRO_APIPACKET      ApiPacket;  /* SuperPro packet                      */
RB_SPRO_LIB_PARAMS     LibParams;  /* Library Configuration Parameters     */

RB_WORD gDevID        = 0;         /* Developer ID                         */
RB_WORD gWritePW      = 0;         /* Write password                       */
RB_WORD gOverwritePW1 = 0;         /* Overwrite password 1                 */
RB_WORD gOverwritePW2 = 0;         /* Overwrite password 2                 */
RB_WORD gActivatePW1  = 0;
RB_WORD gActivatePW2  = 0;
RB_WORD gActivateCell = 0;
RB_WORD gDecCell      = 0;
RB_WORD gOwValue      = 0;
RB_WORD gOwCell       = 0;
RB_BYTE gOwAccessCode = (RB_BYTE)-1;
RB_WORD gQCell        = (RB_WORD)-1;    
RB_WORD gQrySize      = 0;  
RB_BYTE gQData[ SPRO_MAX_QUERY_SIZE + 1] = "";    
RB_BYTE gQResult[ SPRO_MAX_QUERY_SIZE + 1] = "";  
RB_DWORD gQResult32   = 0;
RB_WORD gRCell        = (RB_WORD)-1;
RB_WORD gRData        = (RB_WORD)-1;
RB_WORD gWCell        = (RB_WORD)-1;
RB_WORD gWData        = (RB_WORD)-1;
RB_BYTE gWAccessCode  = (RB_BYTE)-1;
RB_WORD gXRCell       = (RB_WORD)-1;
RB_WORD gXRData       = (RB_WORD)-1;
RB_BYTE gXRAccessCode = (RB_BYTE)-1;
HANDLE  ghInst;                    /* Current instance handle              */
HANDLE  ghHourGlassCursor;         /* Handle to the hour glass cursor      */
HWND    ghWndMain;                 /* Main Window Handle                   */
HWND    ghMainDlg = NULL;          /* Main Dialog window                   */
HWND    ghViewDlg = NULL;          /* View Dialog window                   */
HWND    ghCurrentDlg = NULL;       /* The current modeless dialog          */
char    gAppName[] = "SUPERPRO";   /* Application name                     */
char    gString[STR_BUF_SIZE];     /* generic string                       */
char    gMsg[STR_BUF_SIZE];        /* Another generic string               */
char    gStatus[STR_BUF_SIZE];     /* Status string                        */
char    gResult[STR_BUF_SIZE];     /* Result string                        */
char    gFullStatus[STR_BUF_SIZE]; /* Extended Status string               */
char    gViewText[STR_BUF_SIZE];   /* String for View dialog box           */
WORD    xCommand = IDC_SET_CFG_RB;
WORD    xFunc    = IDC_DRVR_TYPE_FUNC_RB;
/* function prototypes */
LOCAL_PROC
void FormatErrStr( LPSTR         theString,
                   int           idsIdx,
                   SP_STATUS     spStatus,
                   RB_WORD       spFullStatus );
LOCAL_PROC
void UpdateQueryDlg( HWND hWndDlg,
                     RB_WORD qCell,
                     RBP_BYTE qData,
                     RBP_BYTE qResult,
                     RB_DWORD qResult32,
                     RB_WORD  qrySize );
LOCAL_PROC
void UpdateViewLB( HWND hWndDlg );
LOCAL_PROC
void ToggleButton( HWND hWnd, WORD theButton );
LOCAL_PROC
BOOL InitApplication( HANDLE hInstance );
LOCAL_PROC
BOOL InitInstance( HANDLE hInstance, int nCmdShow );
LOCAL_PROC
void CenterWindow( HWND hWnd, int  top );
LOCAL_PROC
unsigned int ConvertBufferToHex( PSTR buffer, unsigned int bufLen );
LOCAL_PROC
WORD ValidateQryStr( LPSTR qData, WORD len );
LOCAL_PROC
void ErrorMsg( HWND hWnd, LPSTR msgText );
LOCAL_PROC
WORD GetDlgItemWord( HWND hWnd, WORD cntrlID, WORD *theData );
LOCAL_PROC
BOOL GetCellAddress( HWND hWnd, WORD cntrlID, WORD *theCell );
LOCAL_PROC
BOOL GetWritePassword( HWND hWnd, WORD cntrlID, WORD *thePW );
LOCAL_PROC
BOOL GetActivate1Password( HWND hWnd, WORD cntrlID, WORD *thePW );
LOCAL_PROC
BOOL GetActivate2Password( HWND hWnd, WORD cntrlID, WORD *thePW );
LOCAL_PROC
BOOL GetOverwrite2Password( HWND hWnd, WORD cntrlID, WORD *thePW );
LOCAL_PROC
BOOL GetOverwrite1Password( HWND hWnd, WORD cntrlID, WORD *thePW );
LOCAL_PROC
BOOL GetDataValue( HWND hWnd, WORD cntrlID, WORD *theData );
LOCAL_PROC
BOOL GetAccessCode( HWND hWnd, WORD cntrlID, BYTE *theAC );
LOCAL_PROC
void ResetPW( void );
LOCAL_PROC
BOOL ProcessPhyAddr( HWND hWndDlg, WORD * portAddr );
LOCAL_PROC
BOOL ProcessLogPort( HWND hWndDlg, WORD * logPortNum );
LOCAL_PROC
BOOL ProcessSysPort( HWND hWndDlg, WORD * sysPortNum );
LOCAL_PROC
BOOL ProcessDevRetryCount( HWND hWndDlg, WORD * drc);
LOCAL_PROC
BOOL ProcessContRetryCount( HWND hWndDlg, WORD * prc);
LOCAL_PROC
BOOL ProcessContRetryInterval( HWND hWndDlg, DWORD * pri);

EXPORT_PROC
BOOL WINAPI VERSIONMsgProc( HWND    hWndDlg,
                               MESSAGE Message,
                               WPARAM  wParam,
                               LPARAM  lParam );
EXPORT_PROC
BOOL WINAPI ACTIVEMsgProc( HWND    hWndDlg,
                               MESSAGE Message,
                               WPARAM  wParam,
                               LPARAM  lParam );
EXPORT_PROC
BOOL WINAPI DECREMsgProc( HWND    hWndDlg,
                              MESSAGE Message,
                              WPARAM  wParam,
                              LPARAM  lParam );
EXPORT_PROC
BOOL WINAPI FINDMsgProc( HWND    hWndDlg,
                             MESSAGE Message,
                             WPARAM  wParam,
                             LPARAM  lParam );
EXPORT_PROC
BOOL WINAPI OWRITEMsgProc( HWND    hWndDlg,
                               MESSAGE Message,
                               WPARAM  wParam,
                               LPARAM  lParam );
EXPORT_PROC
BOOL WINAPI QUERYMsgProc( HWND    hWndDlg,
                              MESSAGE Message,
                              WPARAM  wParam,
                              LPARAM  lParam );
EXPORT_PROC
BOOL WINAPI READMsgProc( HWND    hWndDlg,
                             MESSAGE Message,
                             WPARAM  wParam,
                             LPARAM  lParam );
EXPORT_PROC
BOOL WINAPI WRITEMsgProc( HWND    hWndDlg,
                              MESSAGE Message,
                              WPARAM  wParam,
                              LPARAM  lParam );
EXPORT_PROC
BOOL WINAPI XREADMsgProc( HWND    hWndDlg,
                              MESSAGE Message,
                              WPARAM  wParam,
                              LPARAM  lParam );
EXPORT_PROC
BOOL WINAPI VIEWMsgProc( HWND    hWndDlg,
                             MESSAGE Message,
                             WPARAM  wParam,
                             LPARAM  lParam );
EXPORT_PROC
BOOL WINAPI INFOMsgProc( HWND    hWndDlg,
                             MESSAGE Message,
                             WPARAM  wParam,
                             LPARAM  lParam );
EXPORT_PROC
BOOL WINAPI MAINMsgProc( HWND    hWndDlg,
                             MESSAGE Message,
                             WPARAM  wParam,
                             LPARAM  lParam );
EXPORT_PROC
LONG WINAPI WndProc( HWND    hWnd,
                         MESSAGE Message,
                         WPARAM  wParam,
                         LPARAM  lParam );
int PASCAL WinMain( HANDLE hInstance,
                    HANDLE hPrevInstance,
                    LPSTR  lpszCmdLine,
                    int    nCmdShow );

/***************************************************************************
* Function : DoCfgLibParams
*
* Purpose  : To handle the library configuration.
*
* Inputs   : apiPacket - pointer to the API packet.
*            libParams - pointer to the library configuration parameters.
*
* Outputs  : none.
*
* Returns  : BOOL - TRUE if successful, else FALSE.
****************************************************************************/
BOOL DoCfgLibParams( RBP_SPRO_APIPACKET      apiPacket,
                     RBP_SPRO_LIB_PARAMS     libParams )
{
     SP_STATUS spStatus;
     WORD      fullStatus;

     spStatus = RNBOsproCfgLibParams( apiPacket, libParams );
     if ( spStatus != SP_SUCCESS )
     {
          fullStatus = RNBOsproGetFullStatus( apiPacket );
          FormatErrStr( (LPSTR)gMsg, IDS_CFG_LIB, spStatus, fullStatus );
          ErrorMsg( NULL, (LPSTR)gMsg );
          return( FALSE );
     }
     return( TRUE );
} /* end DoCfgLibParams */

/***************************************************************************
* Procedure: SetSelectText
*
* Purpose  : To handle the selecting text within an edit control.
*
* Inputs   : hWndDlg   - handle to window/dialog.
*            controlID - is the ID of the control.
*            startPos  - is the start position to select.
*            endPos    - is the end position to select
*
* Outputs  : none.
*
* Returns  : none.
****************************************************************************/
void SetSelectText( HWND hWndDlg,
                    WORD controlID,
                    WORD startPos,
                    WORD endPos )
{
     #ifdef _WIN32_
     SendDlgItemMessage( hWndDlg,
                         controlID,
                         EM_SETSEL,
                         (WPARAM)0,
                         (LPARAM)MAKELONG(startPos,endPos) );
     #else
     SendDlgItemMessage( hWndDlg,
                         controlID,
                         EM_SETSEL,
                         (WPARAM)startPos,
                         (LPARAM)endPos );
     #endif     
} /* end SetSelectText */

/***************************************************************************
* Function : SetMachTypeDlg
*
* Purpose  : To handle the Set Machine Type dialog box.
*
* Inputs   : hWndDlg - handle to a dialog.
*            Message - is the message to process.
*            wParam  - is a WPARAM size parameter, if any.
*            lParam  - is a LPARAM size parameter, if any.
*
* Outputs  : none.
*
* Returns  : BOOL    - TRUE  if we process the message
*                      FALSE if not.
****************************************************************************/
#ifdef __BORLANDC__
#pragma argsused
#endif
EXPORT_PROC
BOOL WINAPI SetMachTypeDlg( HWND    hWndDlg,
                            MESSAGE Message,
                            WPARAM  wParam,
                            LPARAM  lParam )
{
   static WORD machType = IDC_MACH_TYPE_AUTO_RB;

   /* to avoid warnings */
   #ifndef __BORLANDC__
   lParam = lParam;
   #endif

   switch( Message )
   {
       case WM_INITDIALOG:
           CenterWindow(hWndDlg, 0);
           CheckRadioButton( hWndDlg,
                             IDC_MACH_TYPE_IBM_RB,
                             IDC_MACH_TYPE_AUTO_RB,
                             machType );
           break;

       case WM_CLOSE:
           PostMessage(hWndDlg, (MESSAGE)WM_COMMAND, IDOK, 0L);
           break;

       case WM_COMMAND: 
           switch( GET_LOWORD(wParam) )
           {

               case IDC_MACH_TYPE_IBM_RB:
                     LibParams.params.machineType = RB_IBM_MACHINE;
                     goto check_mach_rb;

               case IDC_MACH_TYPE_NEC_RB:
                     LibParams.params.machineType = RB_NEC_MACHINE;
                     goto check_mach_rb;

               case IDC_MACH_TYPE_FMR_RB:
                     LibParams.params.machineType = RB_FMR_MACHINE;
                     goto check_mach_rb;

               case IDC_MACH_TYPE_AUTO_RB:
                     LibParams.params.machineType = RB_AUTODETECT_MACHINE;
                     goto check_mach_rb;

                     check_mach_rb:;
                     machType = GET_LOWORD(wParam);
                     CheckRadioButton( hWndDlg,
                                       IDC_MACH_TYPE_IBM_RB,
                                       IDC_MACH_TYPE_AUTO_RB,
                                       machType );
                     break;

               case IDOK:
                   ToggleButton( hWndDlg, IDOK );
                   EndDialog(    hWndDlg, TRUE );
                   DoCfgLibParams( ApiPacket, &LibParams );
                   break;

               case IDCANCEL:
                  PostMessage(hWndDlg, (MESSAGE)WM_CLOSE, 0, 0L );
                  break;

               default :
                   return( FALSE );

           }
           break;

       default:
           return( FALSE );
   }
   return( TRUE );
} /* end SetMachTypeDlg */

/***************************************************************************
* Function : GetMachTypeDlg
*
* Purpose  : To handle the Get Machine Type dialog box.
*
* Inputs   : hWndDlg - handle to a dialog.
*            Message - is the message to process.
*            wParam  - is a WPARAM size parameter, if any.
*            lParam  - is a LPARAM size parameter, if any.
*
* Outputs  : none.
*
* Returns  : BOOL    - TRUE  if we process the message
*                      FALSE if not.
****************************************************************************/
#ifdef __BORLANDC__
#pragma argsused
#endif
EXPORT_PROC
BOOL WINAPI GetMachTypeDlg( HWND    hWndDlg,
                            MESSAGE Message,
                            WPARAM  wParam,
                            LPARAM  lParam )
{
   /* to avoid warnings */
   #ifndef __BORLANDC__
   lParam = lParam;
   #endif

   switch( Message )
   {
       case WM_INITDIALOG:
           CenterWindow(hWndDlg, 0);
           if ( DoCfgLibParams( ApiPacket, &LibParams ) )
           {
               switch( LibParams.params.machineType )
               {
                    case RB_FMR_MACHINE:
                         CheckRadioButton( hWndDlg,
                                           IDC_MACH_TYPE_IBM_RB,
                                           IDC_MACH_TYPE_FMR_RB,
                                           IDC_MACH_TYPE_FMR_RB );
                         break;
                         
                    case RB_NEC_MACHINE:
                         CheckRadioButton( hWndDlg,
                                           IDC_MACH_TYPE_IBM_RB,
                                           IDC_MACH_TYPE_FMR_RB,
                                           IDC_MACH_TYPE_NEC_RB );
                         break;
                    
                    case RB_IBM_MACHINE:
                         CheckRadioButton( hWndDlg,
                                           IDC_MACH_TYPE_IBM_RB,
                                           IDC_MACH_TYPE_FMR_RB,
                                           IDC_MACH_TYPE_IBM_RB );
                         break;
               }
          }
          else
               EndDialog( hWndDlg, TRUE );
          break;

       case WM_CLOSE:
           PostMessage(hWndDlg, (MESSAGE)WM_COMMAND, IDOK, 0L);
           break;

       case WM_COMMAND: 
           switch( GET_LOWORD(wParam) )
           {
               case IDOK:
                   ToggleButton( hWndDlg, IDOK );
                   EndDialog(    hWndDlg, TRUE );
                   break;

               default :
                   return( FALSE );

           }
           break;

       default:
           return( FALSE );
   }
   return( TRUE );
} /* end GetMachTypeDlg */

/***************************************************************************
* Procedure : ProcessPhyAddr
*
* Purpose   : To handle the update of the Physical Port Address field.
*
* Inputs    : hWndDlg - handle to a dialog.
*
* Outputs   : none.
*
* Returns   : BOOL - TRUE if successful, else FALSE.
****************************************************************************/
LOCAL_PROC
BOOL ProcessPhyAddr( HWND hWndDlg, WORD * portAddr )
{
   char  buffer[20];
   char  hexbuf[20];
   buffer[0] = 0;
   if ( GetDlgItemText( hWndDlg,
                        IDC_PHY_PORT_ADDR,
                        (LPSTR)buffer,
                        (sizeof(buffer)-1) ) )
   {
      if (strlen(buffer) > (sizeof(short)*2))
      {
         MessageBox( NULL,
                     (LPSTR)"Physical Port Address Out of range!",
                     (LPSTR)"Physical Port Address",
                     MB_OK | MB_TASKMODAL | MB_ICONEXCLAMATION );
         SetFocus( GetDlgItem( hWndDlg, IDC_PHY_PORT_ADDR ) );
         strcpy (buffer, "0");
         SetDlgItemText( hWndDlg, IDC_PHY_PORT_ADDR, buffer );
         return (FALSE);
      }

      wsprintf(hexbuf, "0x%s", buffer);
      if ( !sscanf( hexbuf, "%x", portAddr ) )
      {
         bad_phy_addr:;
         MessageBox( NULL,
                     (LPSTR)"Invalid Physical Port Address!",
                     (LPSTR)"Physical Port Address",
                     MB_OK | MB_TASKMODAL | MB_ICONEXCLAMATION );
         SetFocus( GetDlgItem( hWndDlg, IDC_PHY_PORT_ADDR ) );
         strcpy (buffer, "0");
         SetDlgItemText( hWndDlg, IDC_PHY_PORT_ADDR, buffer );
         return(FALSE);
      }
      else
         return(TRUE);
   }
   else
   {
        wsprintf( buffer, "%x", *portAddr );
        SetDlgItemText( hWndDlg, IDC_PHY_PORT_ADDR, buffer );
        goto bad_phy_addr;
   }
}

/***************************************************************************
* Procedure : ProcessLogPort
*
* Purpose   : To handle the update of the Logical Port Number field.
*
* Inputs    : hWndDlg - handle to a dialog.
*
* Outputs   : none.
*
* Returns   : BOOL - TRUE if successful, else FALSE.
****************************************************************************/
LOCAL_PROC
BOOL ProcessLogPort( HWND hWndDlg, WORD * logPortNum )
{
   UINT  temp;
   BOOL  err;
   temp = GetDlgItemInt( hWndDlg, IDC_LOG_PORT_NUM, &err, FALSE );
   if ( err == FALSE )
   {
      MessageBox( NULL,
                  (LPSTR)"Invalid Logical Port Number value!",
                  (LPSTR)"Logical Port Number",
                  MB_OK | MB_TASKMODAL | MB_ICONEXCLAMATION );
      SetFocus( GetDlgItem( hWndDlg, IDC_LOG_PORT_NUM ) );
      SetDlgItemInt( hWndDlg, IDC_LOG_PORT_NUM, *logPortNum, FALSE );
      return(FALSE); 
   }
   else
   {
      if ( (int)temp < (int)RB_FIRST_LOG_PORT ||
           (int)temp > (int)RB_LAST_LOG_PORT  )
      {
         MessageBox( NULL,
                     (LPSTR)"Logical Port Number Out of range!",
                     (LPSTR)"Logical Port Number",
                     MB_OK | MB_TASKMODAL | MB_ICONEXCLAMATION );
         SetFocus( GetDlgItem( hWndDlg, IDC_LOG_PORT_NUM ) );
         SetDlgItemInt( hWndDlg, IDC_LOG_PORT_NUM, *logPortNum, FALSE );
         return (FALSE);
      }
      else
      {
         *logPortNum = (WORD) temp;
         return(TRUE);
      }
   }
}

/***************************************************************************
* Procedure : ProcessSysPort
*
* Purpose   : To handle the update of the System Port Number field.
*
* Inputs    : hWndDlg - handle to a dialog.
*
* Outputs   : none.
*
* Returns   : BOOL - TRUE if successful, else FALSE.
****************************************************************************/
LOCAL_PROC
BOOL ProcessSysPort( HWND hWndDlg, WORD * sysPortNum )
{
   UINT  temp;
   BOOL  err;
   temp = GetDlgItemInt( hWndDlg, IDC_SYS_PORT_NUM, &err, TRUE );
   if ( err == FALSE )
   {
      MessageBox( NULL,
                  (LPSTR)"Invalid System Port Number value!",
                  (LPSTR)"System Port Number",
                  MB_OK | MB_TASKMODAL | MB_ICONEXCLAMATION );
      SetFocus( GetDlgItem( hWndDlg, IDC_SYS_PORT_NUM ) );
      SetDlgItemInt( hWndDlg, IDC_SYS_PORT_NUM, *sysPortNum, TRUE );
      return(FALSE); 
   }
   else
   {
      if (temp > 0xffff)
      {
         MessageBox( NULL,
                     (LPSTR)"System Port Number Out of range!",
                     (LPSTR)"System Port Number",
                     MB_OK | MB_TASKMODAL | MB_ICONEXCLAMATION );
         SetFocus( GetDlgItem( hWndDlg, IDC_SYS_PORT_NUM ) );
         SetDlgItemInt( hWndDlg, IDC_SYS_PORT_NUM, *sysPortNum, TRUE );
         return (FALSE);
      }
      else
      {
         *sysPortNum = (WORD) temp;
         return(TRUE);
      }
   }
}

/***************************************************************************
* Procedure : ProcessDevRetryCount
*
* Purpose   : To handle the update of the Device Retry Count field.
*
* Inputs    : hWndDlg - handle to a dialog.
*
* Outputs   : none.
*
* Returns   : BOOL - TRUE if successful, else FALSE.
****************************************************************************/
LOCAL_PROC
BOOL ProcessDevRetryCount( HWND hWndDlg, WORD * drc )
{
   UINT  temp;
   BOOL  err;
   temp = GetDlgItemInt( hWndDlg, IDC_DEV_RC, &err, TRUE );
   if ( err == FALSE )
   {
      MessageBox( NULL,
                  (LPSTR)"Invalid device retry count value!",
                  (LPSTR)"Device Retry Count",
                  MB_OK | MB_TASKMODAL | MB_ICONEXCLAMATION );
      SetFocus( GetDlgItem( hWndDlg, IDC_DEV_RC ) );
      SetDlgItemInt( hWndDlg, IDC_DEV_RC, *drc, TRUE );
      return(FALSE); 
   }
   else
   {
      if (temp > 0xffff)
      {
         MessageBox( NULL,
                     (LPSTR)"Device Retry Count Out of range!",
                     (LPSTR)"Device Retry Count",
                     MB_OK | MB_TASKMODAL | MB_ICONEXCLAMATION );
         SetFocus( GetDlgItem( hWndDlg, IDC_DEV_RC ) );
         SetDlgItemInt( hWndDlg, IDC_DEV_RC, *drc, TRUE );
         return (FALSE);
      }
      else
      {
         *drc = (WORD) temp;
         return(TRUE);
      }
   }
}

/***************************************************************************
* Procedure : ProcessContRetryCount
*
* Purpose   : To handle the update of the Contention Retry Count field.
*
* Inputs    : hWndDlg - handle to a dialog.
*
* Outputs   : none.
*
* Returns   : BOOL - TRUE if successful, else FALSE.
****************************************************************************/
LOCAL_PROC
BOOL ProcessContRetryCount( HWND hWndDlg, WORD * prc )
{
   UINT  temp;
   BOOL  err;
   temp = GetDlgItemInt( hWndDlg, IDC_PORT_CONT_RC, &err, TRUE );
   if ( err == FALSE )
   {
      MessageBox( NULL,
                  (LPSTR)"Invalid port contention retry count value!",
                  (LPSTR)"Port Contention Retry Count",
                  MB_OK | MB_TASKMODAL | MB_ICONEXCLAMATION );
      SetFocus( GetDlgItem( hWndDlg, IDC_PORT_CONT_RC ) );
      SetDlgItemInt( hWndDlg, IDC_PORT_CONT_RC, *prc, TRUE );
      return(FALSE); 
   }
   else
   {
      if (temp > 0xffff)
      {
         MessageBox( NULL,
                     (LPSTR)"Port Contention Retry Count Out of range!",
                     (LPSTR)"Port Contention Retry Count",
                     MB_OK | MB_TASKMODAL | MB_ICONEXCLAMATION );
         SetFocus( GetDlgItem( hWndDlg, IDC_PORT_CONT_RC ) );
         SetDlgItemInt( hWndDlg, IDC_PORT_CONT_RC, *prc, TRUE );
         return (FALSE);
      }
      else
      {
         *prc = (WORD) temp;
         return(TRUE);
      }
   }
}

/***************************************************************************
* Procedure : ProcessContRetryInterval
*
* Purpose   : To handle the update of the Contention Retry Interval field.
*
* Inputs    : hWndDlg - handle to a dialog.
*
* Outputs   : none.
*
* Returns   : BOOL - TRUE if successful, else FALSE.
****************************************************************************/
LOCAL_PROC
BOOL ProcessContRetryInterval( HWND hWndDlg, DWORD * pri )
{
   UINT  temp;
   BOOL  err;
   temp = GetDlgItemInt( hWndDlg, IDC_PORT_CONT_RI, &err, TRUE );
   if ( err == FALSE )
   {
      MessageBox( NULL,
                  (LPSTR)"Invalid port contention retry interval value!",
                  (LPSTR)"Port Contention Retry Interval",
                  MB_OK | MB_TASKMODAL | MB_ICONEXCLAMATION );
      SetFocus( GetDlgItem( hWndDlg, IDC_PORT_CONT_RI ) );
      SetDlgItemInt( hWndDlg, IDC_PORT_CONT_RI, *pri, TRUE );
      return(FALSE); 
   }
   else
   {
      if (temp > 0xffff)
      {
         MessageBox( NULL,
                     (LPSTR)"Port Contention Retry Interval Out of range!",
                     (LPSTR)"Port Contention Retry Interval",
                     MB_OK | MB_TASKMODAL | MB_ICONEXCLAMATION );
         SetFocus( GetDlgItem( hWndDlg, IDC_PORT_CONT_RI ) );
         SetDlgItemInt( hWndDlg, IDC_PORT_CONT_RI, *pri, TRUE );
         return (FALSE);
      }
      else
      {
         *pri = (DWORD) temp;
         return(TRUE);
      }
   }
}

/***************************************************************************
* Function : SetPortParamsDlg
*
* Purpose  : To handle the Set Port Parameters dialog box.
*
* Inputs   : hWndDlg - handle to a dialog.
*            Message - is the message to process.
*            wParam  - is a WPARAM size parameter, if any.
*            lParam  - is a LPARAM size parameter, if any.
*
* Outputs  : none.
*
* Returns  : BOOL    - TRUE  if we process the message
*                      FALSE if not.
****************************************************************************/
#ifdef __BORLANDC__
#pragma argsused
#endif
EXPORT_PROC
BOOL WINAPI SetPortParamsDlg( HWND    hWndDlg,
                              MESSAGE Message,
                              WPARAM  wParam,
                              LPARAM  lParam )
{
   static WORD portType    = IDC_PORT_TYPE_AUTO_RB;
   static WORD portAddr    = 0;
   static WORD flgValidatePort= 1;
   static WORD logPortNum  = RB_FIRST_LOG_PORT;
   static WORD sysPortNum  = 0;
   static WORD  drc        = RB_DEV_RETRY_CNT_DEF;
   static WORD  prc        = RB_PORT_CONT_RETRY_CNT_DEF;
   static DWORD pri        = RB_PORT_CONT_RETRY_INT_DEF;
   static UINT  cmSysInt   = 0;
   static UINT  cmMaskInt  = 0;
   static UINT  cmPollHW   = 0;
   static UINT  cmRBW      = 0;
   static UINT  cmDrvrDef  = 1;
          char  buffer[255];

   /* to avoid warnings */
   #ifndef __BORLANDC__
   lParam = lParam;
   #endif

   switch( Message )
   {
       case WM_INITDIALOG:
           CenterWindow(hWndDlg, 0);
           CheckRadioButton( hWndDlg,
                             IDC_PORT_TYPE_IBM_RB,
                             IDC_PORT_TYPE_AUTO_RB,
                             portType );
           CheckDlgButton( hWndDlg, IDC_VALIDATE_PORT_CB, flgValidatePort );
           CheckDlgButton( hWndDlg, IDC_PORT_CONT_MET_MASK_INTS_CB, cmMaskInt );
           CheckDlgButton( hWndDlg, IDC_PORT_CONT_MET_SYS_INTS_CB, cmSysInt );
           CheckDlgButton( hWndDlg, IDC_PORT_CONT_MET_HW_CB, cmPollHW );
           CheckDlgButton( hWndDlg, IDC_PORT_CONT_MET_AUTO_CB, cmDrvrDef );
           CheckDlgButton( hWndDlg, IDC_PORT_CONT_MET_RBW_CB, cmRBW );
           SetDlgItemInt( hWndDlg, IDC_DEV_RC, drc, TRUE );
           SetDlgItemInt( hWndDlg, IDC_PORT_CONT_RC, prc, TRUE );
           SetDlgItemInt( hWndDlg, IDC_PORT_CONT_RI, pri, TRUE );
           SetDlgItemInt( hWndDlg, IDC_LOG_PORT_NUM, logPortNum, FALSE );
           SetDlgItemInt( hWndDlg, IDC_SYS_PORT_NUM, sysPortNum, FALSE );
           wsprintf( buffer, "%x", portAddr );
           SetDlgItemText( hWndDlg, IDC_PHY_PORT_ADDR, buffer );
           if ( cmDrvrDef )
           {
                EnableWindow( GetDlgItem( hWndDlg, IDC_PORT_CONT_MET_RBW_CB ), FALSE );
                EnableWindow( GetDlgItem( hWndDlg, IDC_PORT_CONT_MET_HW_CB ), FALSE );
                EnableWindow( GetDlgItem( hWndDlg, IDC_PORT_CONT_MET_SYS_INTS_CB ), FALSE );
                EnableWindow( GetDlgItem( hWndDlg, IDC_PORT_CONT_MET_MASK_INTS_CB ), FALSE );
           }
           else
           {
                EnableWindow( GetDlgItem( hWndDlg, IDC_PORT_CONT_MET_RBW_CB ), TRUE );
                EnableWindow( GetDlgItem( hWndDlg, IDC_PORT_CONT_MET_HW_CB ), TRUE );
                EnableWindow( GetDlgItem( hWndDlg, IDC_PORT_CONT_MET_SYS_INTS_CB ), TRUE );
                EnableWindow( GetDlgItem( hWndDlg, IDC_PORT_CONT_MET_MASK_INTS_CB ), TRUE );
           }
           break;

       case WM_CLOSE:
           EndDialog( hWndDlg, TRUE );
           break;

       case WM_COMMAND: 
           switch( GET_LOWORD(wParam) )
           {

               case IDC_VALIDATE_PORT_CB:
                    flgValidatePort = (WORD)IsDlgButtonChecked( hWndDlg, IDC_VALIDATE_PORT_CB );
                    CheckDlgButton( hWndDlg, IDC_VALIDATE_PORT_CB, flgValidatePort );
                    break;

               case IDC_PORT_CONT_MET_RBW_CB:
                    cmRBW = IsDlgButtonChecked( hWndDlg, IDC_PORT_CONT_MET_RBW_CB );
                    CheckDlgButton( hWndDlg, IDC_PORT_CONT_MET_RBW_CB, cmRBW );
                    break;

               case IDC_PORT_CONT_MET_HW_CB:
                    cmPollHW = IsDlgButtonChecked( hWndDlg, IDC_PORT_CONT_MET_HW_CB );
                    CheckDlgButton( hWndDlg, IDC_PORT_CONT_MET_HW_CB, cmPollHW );
                    break;

               case IDC_PORT_CONT_MET_MASK_INTS_CB:
                    cmMaskInt = IsDlgButtonChecked( hWndDlg, IDC_PORT_CONT_MET_MASK_INTS_CB );
                    CheckDlgButton( hWndDlg, IDC_PORT_CONT_MET_MASK_INTS_CB, cmMaskInt );
                    break;

               case IDC_PORT_CONT_MET_SYS_INTS_CB:
                    cmSysInt = IsDlgButtonChecked( hWndDlg, IDC_PORT_CONT_MET_SYS_INTS_CB );
                    CheckDlgButton( hWndDlg, IDC_PORT_CONT_MET_SYS_INTS_CB, cmSysInt );
                    break;

               case IDC_PORT_CONT_MET_AUTO_CB:
                    cmDrvrDef = IsDlgButtonChecked( hWndDlg, IDC_PORT_CONT_MET_AUTO_CB );
                    CheckDlgButton( hWndDlg, IDC_PORT_CONT_MET_AUTO_CB, cmDrvrDef );
                    if ( cmDrvrDef )
                    {
                         EnableWindow( GetDlgItem( hWndDlg, IDC_PORT_CONT_MET_RBW_CB ), FALSE );
                         EnableWindow( GetDlgItem( hWndDlg, IDC_PORT_CONT_MET_HW_CB ), FALSE );
                         EnableWindow( GetDlgItem( hWndDlg, IDC_PORT_CONT_MET_SYS_INTS_CB ), FALSE );
                         EnableWindow( GetDlgItem( hWndDlg, IDC_PORT_CONT_MET_MASK_INTS_CB ), FALSE );
                    }
                    else
                    {
                         EnableWindow( GetDlgItem( hWndDlg, IDC_PORT_CONT_MET_RBW_CB ), TRUE );
                         EnableWindow( GetDlgItem( hWndDlg, IDC_PORT_CONT_MET_HW_CB ), TRUE );
                         EnableWindow( GetDlgItem( hWndDlg, IDC_PORT_CONT_MET_SYS_INTS_CB ), TRUE );
                         EnableWindow( GetDlgItem( hWndDlg, IDC_PORT_CONT_MET_MASK_INTS_CB ), TRUE );
                    }
                    break;

               case IDC_PORT_TYPE_IBM_DMA_RB:
                    portType = IDC_PORT_TYPE_IBM_DMA_RB;
                    goto check_rb;

               case IDC_PORT_TYPE_NEC_RB:
                    portType = IDC_PORT_TYPE_NEC_RB;
                    goto check_rb;

               case IDC_PORT_TYPE_FMR_RB:
                    portType = IDC_PORT_TYPE_FMR_RB;
                    goto check_rb;

               case IDC_PORT_TYPE_AUTO_RB:
                    portType = IDC_PORT_TYPE_AUTO_RB;
                    goto check_rb;

               case IDC_PORT_TYPE_IBM_RB:
                    portType = IDC_PORT_TYPE_IBM_RB;
                    goto check_rb;

                    check_rb:;
                    CheckRadioButton( hWndDlg,
                                      IDC_PORT_TYPE_IBM_RB,
                                      IDC_PORT_TYPE_AUTO_RB,
                                      portType );
                    break;

               case IDOK:
                    EndDialog( hWndDlg, TRUE );
                    break;

               case IDC_SET_PORT_PARMS:
                   if (ProcessLogPort(hWndDlg,&logPortNum) == FALSE) break;
                   if (ProcessPhyAddr(hWndDlg,&portAddr) == FALSE) break;
                   if (ProcessSysPort(hWndDlg,&sysPortNum) == FALSE) break;
                   if (ProcessDevRetryCount(hWndDlg,&drc) == FALSE) break;
                   if (ProcessContRetryCount(hWndDlg,&prc) == FALSE) break;
                   if (ProcessContRetryInterval(hWndDlg,&pri) == FALSE) break;

                   ToggleButton( hWndDlg, IDC_SET_PORT_PARMS );
                   flgValidatePort = (WORD)IsDlgButtonChecked( hWndDlg, IDC_VALIDATE_PORT_CB );
                   cmRBW = IsDlgButtonChecked( hWndDlg, IDC_PORT_CONT_MET_RBW_CB );
                   cmPollHW = IsDlgButtonChecked( hWndDlg, IDC_PORT_CONT_MET_HW_CB );
                   cmMaskInt = IsDlgButtonChecked( hWndDlg, IDC_PORT_CONT_MET_MASK_INTS_CB );
                   cmSysInt = IsDlgButtonChecked( hWndDlg, IDC_PORT_CONT_MET_SYS_INTS_CB );
                   cmDrvrDef = IsDlgButtonChecked( hWndDlg, IDC_PORT_CONT_MET_AUTO_CB );

                   LibParams.params.portParams.phyAddr    = portAddr;
                   LibParams.params.portParams.logPortNum = logPortNum;
                   LibParams.params.portParams.sysPortNum = sysPortNum;
                   LibParams.params.portParams.deviceRetryCnt = drc;
                   LibParams.params.portParams.contentionRetryCnt = prc;
                   LibParams.params.portParams.contentionRetryInterval = pri;
                   LibParams.params.portParams.contentionMethod = 0;
                   LibParams.params.portParams.flags1           = 0;

                   if ( cmDrvrDef )
                   {
                        LibParams.params.portParams.contentionMethod =
                        RB_CONT_METH_DRVR_DEFINED;
                   }
                   else
                   {

                         if ( cmRBW )
                         {
                              LibParams.params.portParams.contentionMethod |=
                              RB_CONT_METH_RBW;
                         }
                         if ( cmMaskInt )
                         {
                              LibParams.params.portParams.contentionMethod |=
                              RB_CONT_METH_MASK_INT;
                         }
                         if ( cmSysInt )
                         {
                              LibParams.params.portParams.contentionMethod |=
                              RB_CONT_METH_SYS_INT;
                         }
                         if ( cmPollHW )
                         {
                              LibParams.params.portParams.contentionMethod |=
                              RB_CONT_METH_POLL_HW;
                         }

                         if ( flgValidatePort )
                         {
                              LibParams.params.portParams.flags1 |= RB_VALIDATE_PORT;
                         }
                   }

                   if ( portType == IDC_PORT_TYPE_AUTO_RB )
                   {
                         LibParams.params.portParams.portType = RB_AUTODETECT_PORT_TYPE;
                   }
                   else if ( portType == IDC_PORT_TYPE_IBM_RB )
                   {
                         LibParams.params.portParams.portType = RB_PS2_PORT_TYPE;
                   }
                   else if ( portType == IDC_PORT_TYPE_IBM_DMA_RB )
                   {
                         LibParams.params.portParams.portType = RB_PS2_DMA_PORT_TYPE;
                   }
                   else if ( portType == IDC_PORT_TYPE_NEC_RB )
                   {
                         LibParams.params.portParams.portType = RB_NEC_PORT_TYPE;
                   }
                   else if ( portType == IDC_PORT_TYPE_FMR_RB )
                   {
                         LibParams.params.portParams.portType = RB_FMR_PORT_TYPE;
                   }
                   DoCfgLibParams( ApiPacket, &LibParams );
                   break;

               case IDCANCEL:
                  EndDialog( hWndDlg, TRUE );
                  break;

               default :
                   return( FALSE );

           }
           break;

       default:
           return( FALSE );
   }
   return( TRUE );
} /* end SetPortParamsDlg */

/***************************************************************************
* Function : GetPortParamsDlg
*
* Purpose  : To handle the Get Port Parameters dialog box.
*
* Inputs   : hWndDlg - handle to a dialog.
*            Message - is the message to process.
*            wParam  - is a WPARAM size parameter, if any.
*            lParam  - is a LPARAM size parameter, if any.
*
* Outputs  : none.
*
* Returns  : BOOL    - TRUE  if we process the message
*                      FALSE if not.
****************************************************************************/
#ifdef __BORLANDC__
#pragma argsused
#endif
EXPORT_PROC
BOOL WINAPI GetPortParamsDlg( HWND    hWndDlg,
                              MESSAGE Message,
                              WPARAM  wParam,
                              LPARAM  lParam )
{
   #define contMethod (LibParams.params.portParams.contentionMethod)
   #define sysPortNum (LibParams.params.portParams.sysPortNum)
   #define portType   (LibParams.params.portParams.portType)
   #define drc        (LibParams.params.portParams.deviceRetryCnt)
   #define prc        (LibParams.params.portParams.contentionRetryCnt)
   #define pri        (LibParams.params.portParams.contentionRetryInterval)
   #define portAddr   (LibParams.params.portParams.phyAddr)
   #define sysPort    (LibParams.params.portParams.sysPortNum)
   #define logPort    (LibParams.params.portParams.logPortNum)
   #define flags      (LibParams.params.portParams.flags1)
   #define mapAddr    (LibParams.params.portParams.mappedAddr)

   static WORD logPortNum  = RB_FIRST_LOG_PORT;
          char  buffer[255];
          WORD  temp;
          BOOL  err;

   /* to avoid warnings */
   #ifndef __BORLANDC__
   lParam = lParam;
   #endif

   switch( Message )
   {
       case WM_INITDIALOG:
           CenterWindow(hWndDlg, 0);

           update_dlg:;
           logPort = logPortNum;

           if ( DoCfgLibParams( ApiPacket, &LibParams ) )
           {
               if ( portType == RB_NEC_PORT_TYPE )
               {
                   CheckRadioButton( hWndDlg,
                                     IDC_PORT_TYPE_IBM_RB,
                                     IDC_PORT_TYPE_FMR_RB,
                                     IDC_PORT_TYPE_NEC_RB );
               }
               else if ( portType == RB_FMR_PORT_TYPE )
               {
                   CheckRadioButton( hWndDlg,
                                     IDC_PORT_TYPE_IBM_RB,
                                     IDC_PORT_TYPE_FMR_RB,
                                     IDC_PORT_TYPE_FMR_RB );
               }
               else if ( portType == RB_PS2_PORT_TYPE )
               {
                   CheckRadioButton( hWndDlg,
                                     IDC_PORT_TYPE_IBM_RB,
                                     IDC_PORT_TYPE_FMR_RB,
                                     IDC_PORT_TYPE_IBM_RB );
               }
               else if ( portType == RB_PS2_DMA_PORT_TYPE )
               {
                   CheckRadioButton( hWndDlg,
                                     IDC_PORT_TYPE_IBM_RB,
                                     IDC_PORT_TYPE_FMR_RB,
                                     IDC_PORT_TYPE_IBM_DMA_RB );
               }
               
               CheckDlgButton( hWndDlg,
                               IDC_VALIDATE_PORT_CB,
                               (flags & RB_VALIDATE_PORT) );
               CheckDlgButton( hWndDlg,
                               IDC_USER_DEF_PORT_CB,
                               (flags & RB_USER_DEFINED_PORT) );
               CheckDlgButton( hWndDlg,
                               IDC_CONT_HNDLR_INST_CB,
                               (flags & RB_CONT_HNDLR_INSTALLED) );
               CheckDlgButton( hWndDlg,
                               IDC_PORT_CONT_MET_SYS_CB,
                               (contMethod & RB_CONT_METH_SYS_ALLOC )  );
               CheckDlgButton( hWndDlg,
                               IDC_PORT_CONT_MET_MASK_INTS_CB,
                               (contMethod & RB_CONT_METH_MASK_INT )  );
               CheckDlgButton( hWndDlg,
                               IDC_PORT_CONT_MET_SYS_INTS_CB,
                               (contMethod & RB_CONT_METH_SYS_INT )  );
               CheckDlgButton( hWndDlg,
                               IDC_PORT_CONT_MET_WCS_CB,
                               (contMethod & RB_CONT_METH_WIN_CS )  );
               CheckDlgButton( hWndDlg,
                               IDC_PORT_CONT_MET_HW_CB,
                               (contMethod & RB_CONT_METH_POLL_HW)  );
               CheckDlgButton( hWndDlg,
                               IDC_PORT_CONT_MET_RBW_CB,
                               (contMethod & RB_CONT_METH_RBW )  );
               CheckDlgButton( hWndDlg,
                               IDC_PORT_CONT_MET_NT_RIRQL_CB,
                               (contMethod & RB_CONT_METH_NT_RIRQL )  );

               SetDlgItemInt( hWndDlg, IDC_SYS_PORT_NUM, sysPortNum, TRUE );
               SetDlgItemInt( hWndDlg, IDC_DEV_RC, drc, TRUE );
               SetDlgItemInt( hWndDlg, IDC_PORT_CONT_RC, prc, TRUE );
               SetDlgItemInt( hWndDlg, IDC_PORT_CONT_RI, pri, TRUE );
               SetDlgItemInt( hWndDlg, IDC_LOG_PORT_NUM, logPortNum, FALSE );
               wsprintf( buffer, "0x%04X", portAddr );
               SetDlgItemText( hWndDlg, IDC_PHY_PORT_ADDR, (LPSTR)buffer );
               wsprintf( buffer, "0x%04X", mapAddr );
               SetDlgItemText( hWndDlg, IDC_MAPPED_ADDR, (LPSTR)buffer );
           }    
           break;

       case WM_SHOWWINDOW:
            SetFocus( GetDlgItem( hWndDlg, IDC_LOG_PORT_NUM ) );
            SetSelectText( hWndDlg, IDC_LOG_PORT_NUM, 0, (WORD)-1 );
            break;

       case WM_CLOSE:
           EndDialog( hWndDlg, TRUE );
           break;

       case WM_COMMAND: 
           switch( GET_LOWORD(wParam) )
           {

               case IDOK:
                    ToggleButton( hWndDlg, IDOK );
                    EndDialog( hWndDlg, TRUE );
                    break;

               case IDC_LOG_PORT_NUM:
                    temp = (WORD)GetDlgItemInt( hWndDlg,
                                          IDC_LOG_PORT_NUM,
                                          &err,
                                          FALSE );
                    if ( (int)temp < (int)RB_FIRST_LOG_PORT ||
                         (int)temp > (int)RB_LAST_LOG_PORT  ||
                         err == FALSE )
                    {
                         MessageBox( NULL,
                                     (LPSTR)"Invalid Logical Port Number!",
                                     (LPSTR)"Logical Port Number",
                                     MB_OK | MB_TASKMODAL | MB_ICONEXCLAMATION );
                         SetDlgItemInt( hWndDlg,
                                        IDC_LOG_PORT_NUM,
                                        logPortNum,
                                        FALSE );
                         SetSelectText( hWndDlg, IDC_LOG_PORT_NUM, 0, (WORD)-1 );
                                             
                    }
                    else
                    {
                         logPortNum = temp;
                    }
                    break;

               case IDC_GET_PORT_PARMS:
                    temp = (WORD)GetDlgItemInt( hWndDlg,
                                          IDC_LOG_PORT_NUM,
                                          &err,
                                          FALSE );
                    if ( (int)temp < (int)RB_FIRST_LOG_PORT ||
                         (int)temp > (int)RB_LAST_LOG_PORT  ||
                         err == FALSE )
                    {
                         MessageBox( NULL,
                                     (LPSTR)"Invalid Logical Port Number!",
                                     (LPSTR)"Logical Port Number",
                                     MB_OK | MB_TASKMODAL | MB_ICONEXCLAMATION );
                         SetDlgItemInt( hWndDlg,
                                        IDC_LOG_PORT_NUM,
                                        logPortNum,
                                        FALSE );
                         SetSelectText( hWndDlg, IDC_LOG_PORT_NUM, 0, (WORD)-1 );
                                             
                    }
                    else
                    {
                         logPortNum = temp;
                         goto update_dlg;
                    }
                    break;

               case IDCANCEL:
                    EndDialog( hWndDlg, TRUE );
                    break;

               default :
                   return( FALSE );

           }
           break;

       default:
           return( FALSE );
   }
   return( TRUE );

   #undef contMethod
   #undef sysPortNum
   #undef portType  
   #undef drc       
   #undef prc       
   #undef pri       
   #undef portAddr   
   #undef sysPort   
   #undef logPort   
   #undef flags
   #undef mapAddr
} /* end GetPortParamsDlg */

/***************************************************************************
* Function : SetDelayDlg
*
* Purpose  : To handle the Set Timing dialog box.
*
* Inputs   : hWndDlg - handle to a dialog.
*            Message - is the message to process.
*            wParam  - is a WPARAM size parameter, if any.
*            lParam  - is a LPARAM size parameter, if any.
*
* Outputs  : none.
*
* Returns  : BOOL    - TRUE  if we process the message
*                      FALSE if not.
****************************************************************************/
#ifdef __BORLANDC__
#pragma argsused
#endif
EXPORT_PROC
BOOL WINAPI SetDelayDlg( HWND    hWndDlg,
                         MESSAGE Message,
                         WPARAM  wParam,
                         LPARAM  lParam )
{
   static WORD Delay = RB_USE_AUTOTIMING;
   char buffer[255];

   /* to avoid warnings */
   #ifndef __BORLANDC__
   lParam = lParam;
   #endif

   switch( Message )
   {
       case WM_INITDIALOG:
           CenterWindow(hWndDlg, 0);
           wsprintf( buffer, "%d", Delay );
           SetDlgItemText( hWndDlg,
                           IDC_SET_DELAY_VALUE,
                           (LPSTR)buffer );
           break;

       case WM_CLOSE:
           PostMessage(hWndDlg, (MESSAGE)WM_COMMAND, IDOK, 0L);
           break;

       case WM_COMMAND: 
           switch( GET_LOWORD(wParam) )
           {
               case IDC_SET_DELAY_VALUE:
                   Delay = (WORD)GetDlgItemInt( hWndDlg,
                                                IDC_SET_DELAY_VALUE,
                                                NULL,
                                                FALSE );
                    break;

               case IDOK:
                   ToggleButton( hWndDlg, IDOK );
                   LibParams.params.delay = Delay;
                   EndDialog(    hWndDlg, TRUE );
                   DoCfgLibParams( ApiPacket, &LibParams );
                   break;

               case IDCANCEL:
                  PostMessage(hWndDlg, (MESSAGE)WM_CLOSE, 0, 0L );
                  break;

               default :
                   return( FALSE );

           }
           break;

       default:
           return( FALSE );
   }
   return( TRUE );
} /* end SetDelayDlg */

/***************************************************************************
* Function : GetDelayDlg
*
* Purpose  : To handle the Get Timing dialog box.
*
* Inputs   : hWndDlg - handle to a dialog.
*            Message - is the message to process.
*            wParam  - is a WPARAM size parameter, if any.
*            lParam  - is a LPARAM size parameter, if any.
*
* Outputs  : none.
*
* Returns  : BOOL    - TRUE  if we process the message
*                      FALSE if not.
****************************************************************************/
#ifdef __BORLANDC__
#pragma argsused
#endif
EXPORT_PROC
BOOL WINAPI GetDelayDlg( HWND    hWndDlg,
                         MESSAGE Message,
                         WPARAM  wParam,
                         LPARAM  lParam )
{
   char buffer[255];

   /* to avoid warnings */
   #ifndef __BORLANDC__
   lParam = lParam;
   #endif

   switch( Message )
   {
       case WM_INITDIALOG:
            CenterWindow(hWndDlg, 0);
            DoCfgLibParams( ApiPacket, &LibParams );
            wsprintf( buffer, "%d", LibParams.params.delay );
            SetDlgItemText( hWndDlg,
                            IDC_GET_DELAY_VALUE,
                            (LPCSTR)buffer );
            break;

       case WM_CLOSE:
           PostMessage(hWndDlg, (MESSAGE)WM_COMMAND, IDOK, 0L);
           break;

       case WM_COMMAND: 
           switch( GET_LOWORD(wParam) )
           {
               case IDOK:
                   ToggleButton( hWndDlg, IDOK );
                   EndDialog(    hWndDlg, TRUE );
                   break;

               default :
                   return( FALSE );

           }
           break;

       default:
           return( FALSE );
   }
   return( TRUE );
} /* end GetDelayDlg */

/***************************************************************************
* Function : SetDrvrTypeDlg
*
* Purpose  : To handle the Set Driver Type dialog box.
*
* Inputs   : hWndDlg - handle to a dialog.
*            Message - is the message to process.
*            wParam  - is a WPARAM size parameter, if any.
*            lParam  - is a LPARAM size parameter, if any.
*
* Outputs  : none.
*
* Returns  : BOOL    - TRUE  if we process the message
*                      FALSE if not.
****************************************************************************/
#ifdef __BORLANDC__
#pragma argsused
#endif
EXPORT_PROC
BOOL WINAPI SetDrvrTypeDlg( HWND    hWndDlg,
                         MESSAGE Message,
                         WPARAM  wParam,
                         LPARAM  lParam )
{
   static WORD DrvrType = IDC_DRVR_TYPE_AUTO_RB;
 
   /* to avoid warnings */
   #ifndef __BORLANDC__
   lParam = lParam;
   #endif

   switch( Message )
   {
       case WM_INITDIALOG:
           CenterWindow(hWndDlg, 0);
           CheckRadioButton( hWndDlg,
                             IDC_DRVR_TYPE_LOCAL_RB,
                             IDC_DRVR_TYPE_AUTO_RB,
                             DrvrType );
           break;

       case WM_CLOSE:
           PostMessage(hWndDlg, (MESSAGE)WM_COMMAND, IDOK, 0L);
           break;

       case WM_COMMAND: 
           switch( GET_LOWORD(wParam) )
           {
               case IDOK:
                    ToggleButton( hWndDlg, IDOK );
                    EndDialog(    hWndDlg, TRUE );
                    DoCfgLibParams( ApiPacket, &LibParams );
                    break;

               case IDC_DRVR_TYPE_LOCAL_RB:
                    LibParams.params.routerFlags = RB_ROUTER_USE_LOCAL_DRVR;
                    DrvrType = IDC_DRVR_TYPE_LOCAL_RB;
                    goto check_drvrtype_rb;

               case IDC_DRVR_TYPE_SYS_RB:
                    LibParams.params.routerFlags = RB_ROUTER_USE_SYS_DRVR;
                    DrvrType = IDC_DRVR_TYPE_SYS_RB;
                    goto check_drvrtype_rb;

               case IDC_DRVR_TYPE_AUTO_RB:
                    LibParams.params.routerFlags = RB_ROUTER_AUTODETECT_DRVR;
                    DrvrType = IDC_DRVR_TYPE_AUTO_RB;
                    goto check_drvrtype_rb;

                    check_drvrtype_rb:;
                    CheckRadioButton( hWndDlg,
                                      IDC_DRVR_TYPE_LOCAL_RB,
                                      IDC_DRVR_TYPE_AUTO_RB,
                                      DrvrType );
                    break;

               case IDCANCEL:
                    PostMessage(hWndDlg, (MESSAGE)WM_CLOSE, 0, 0L );
                    break;

               default :
                   return( FALSE );

           }
           break;

       default:
           return( FALSE );
   }
   return( TRUE );
} /* end SetDrvrTypeDlg */

/***************************************************************************
* Function : GetDrvrTypeDlg
*
* Purpose  : To handle the Get Driver Type dialog box.
*
* Inputs   : hWndDlg - handle to a dialog.
*            Message - is the message to process.
*            wParam  - is a WPARAM size parameter, if any.
*            lParam  - is a LPARAM size parameter, if any.
*
* Outputs  : none.
*
* Returns  : BOOL    - TRUE  if we process the message
*                      FALSE if not.
****************************************************************************/
#ifdef __BORLANDC__
#pragma argsused
#endif
EXPORT_PROC
BOOL WINAPI GetDrvrTypeDlg( HWND    hWndDlg,
                            MESSAGE Message,
                            WPARAM  wParam,
                            LPARAM  lParam )
{
   /* to avoid warnings */
   #ifndef __BORLANDC__
   lParam = lParam;
   #endif

   switch( Message )
   {
       case WM_INITDIALOG:
            CenterWindow(hWndDlg, 0);
            if ( DoCfgLibParams( ApiPacket, &LibParams ) )
            {
               switch( LibParams.params.routerFlags & RB_MAX_ROUTER_FLAGS )
               {
                  case RB_ROUTER_USE_LOCAL_DRVR:
                    CheckRadioButton( hWndDlg,
                                      IDC_DRVR_TYPE_LOCAL_RB,
                                      IDC_DRVR_TYPE_SYS_RB,
                                      IDC_DRVR_TYPE_LOCAL_RB );
                    break;
               
                  case RB_ROUTER_USE_SYS_DRVR:
                    CheckRadioButton( hWndDlg,
                                      IDC_DRVR_TYPE_LOCAL_RB,
                                      IDC_DRVR_TYPE_SYS_RB,
                                      IDC_DRVR_TYPE_SYS_RB );
                    break;
               }
            }
            else
                 EndDialog( hWndDlg, TRUE );
            break;

       case WM_CLOSE:
            EndDialog( hWndDlg, TRUE );
            break;

       case WM_COMMAND: 
           switch( GET_LOWORD(wParam) )
           {
               case IDOK:
                   ToggleButton( hWndDlg, IDOK );
                   EndDialog(    hWndDlg, TRUE );
                   break;

               default :
                   return( FALSE );

           }
           break;

       default:
           return( FALSE );
   }
   return( TRUE );
} /* end GetDrvrTypeDlg */


/***************************************************************************
* Function : SetMaskIntsDlg
*
* Purpose  : To handle the Set Mask Ints dialog box.
*
* Inputs   : hWndDlg - handle to a dialog.
*            Message - is the message to process.
*            wParam  - is a WPARAM size parameter, if any.
*            lParam  - is a LPARAM size parameter, if any.
*
* Outputs  : none.
*
* Returns  : BOOL    - TRUE  if we process the message
*                      FALSE if not.
****************************************************************************/
#ifdef __BORLANDC__
#pragma argsused
#endif
EXPORT_PROC
BOOL WINAPI SetMaskIntsDlg( HWND    hWndDlg,
                            MESSAGE Message,
                            WPARAM  wParam,
                            LPARAM  lParam )
{
   static WORD MaskInts = RB_IRQ_MASK_DEF;

   /* to avoid warnings */
   #ifndef __BORLANDC__
   lParam = lParam;
   #endif

   switch( Message )
   {
       case WM_INITDIALOG:
           CenterWindow(hWndDlg, 0);
                  CheckDlgButton( hWndDlg,
                                              IDC_MASK_INT_LPT1_CB,
                                              (MaskInts & RB_IRQ_MASK_LPT1 ? 1 : 0) );
                  CheckDlgButton( hWndDlg,
                                              IDC_MASK_INT_LPT2_CB,
                                              (MaskInts & RB_IRQ_MASK_LPT2 ? 1 : 0) );
                  CheckDlgButton( hWndDlg,
                                              IDC_MASK_INT_TIMER_CB,
                                              (MaskInts & RB_IRQ_MASK_TIMER ? 1 : 0) );
           break;

       case WM_CLOSE:
           PostMessage(hWndDlg, (MESSAGE)WM_COMMAND, IDOK, 0L);
           break;

       case WM_COMMAND: 
           switch( GET_LOWORD(wParam) )
           {
               case IDOK:
                    ToggleButton( hWndDlg, IDOK );
                    EndDialog(    hWndDlg, TRUE );
                                LibParams.params.maskInterrupts = MaskInts;
                    DoCfgLibParams( ApiPacket, &LibParams );
                    break;

               case IDC_MASK_INT_TIMER_CB:
                    if ( IsDlgButtonChecked( hWndDlg, IDC_MASK_INT_TIMER_CB ) )
                             MaskInts |= RB_IRQ_MASK_TIMER;
                    else
                             MaskInts &= ~RB_IRQ_MASK_TIMER;
                    goto check_maskints_cb;

               case IDC_MASK_INT_LPT1_CB:
                    if ( IsDlgButtonChecked( hWndDlg, IDC_MASK_INT_LPT1_CB ) )
                             MaskInts |= RB_IRQ_MASK_LPT1;
                    else
                             MaskInts &= ~RB_IRQ_MASK_LPT1;
                    goto check_maskints_cb;

               case IDC_MASK_INT_LPT2_CB:
                    if ( IsDlgButtonChecked( hWndDlg, IDC_MASK_INT_LPT2_CB ) )
                             MaskInts |= RB_IRQ_MASK_LPT2;
                    else
                             MaskInts &= ~RB_IRQ_MASK_LPT2;
                    goto check_maskints_cb;

                    check_maskints_cb:;
                    CheckDlgButton( hWndDlg,
                                  IDC_MASK_INT_LPT1_CB,
                                  (MaskInts & RB_IRQ_MASK_LPT1 ? 1 : 0) );
                    CheckDlgButton( hWndDlg,
                                  IDC_MASK_INT_LPT2_CB,
                                  (MaskInts & RB_IRQ_MASK_LPT2 ? 1 : 0) );
                    CheckDlgButton( hWndDlg,
                                  IDC_MASK_INT_TIMER_CB,
                                  (MaskInts & RB_IRQ_MASK_TIMER ? 1 : 0) );
                    break;

               case IDCANCEL:
                    PostMessage(hWndDlg, (MESSAGE)WM_CLOSE, 0, 0L );
                    break;

               default :
                   return( FALSE );

           }
           break;

       default:
           return( FALSE );
   }
   return( TRUE );
} /* end SetMaskIntsDlg */

/***************************************************************************
* Function : GetMaskIntsDlg
*
* Purpose  : To handle the Get Driver Type dialog box.
*
* Inputs   : hWndDlg - handle to a dialog.
*            Message - is the message to process.
*            wParam  - is a WPARAM size parameter, if any.
*            lParam  - is a LPARAM size parameter, if any.
*
* Outputs  : none.
*
* Returns  : BOOL    - TRUE  if we process the message
*                      FALSE if not.
****************************************************************************/
#ifdef __BORLANDC__
#pragma argsused
#endif
EXPORT_PROC
BOOL WINAPI GetMaskIntsDlg( HWND    hWndDlg,
                            MESSAGE Message,
                            WPARAM  wParam,
                            LPARAM  lParam )
{
   /* to avoid warnings */
   #ifndef __BORLANDC__
   lParam = lParam;
   #endif

   switch( Message )
   {
       case WM_INITDIALOG:
            CenterWindow(hWndDlg, 0);
            DoCfgLibParams( ApiPacket, &LibParams );
            CheckDlgButton( hWndDlg,
                            IDC_MASK_INT_LPT1_CB,
                            (LibParams.params.maskInterrupts & RB_IRQ_MASK_LPT1 ? 1 : 0) );
            CheckDlgButton( hWndDlg,
                            IDC_MASK_INT_LPT2_CB,
                            (LibParams.params.maskInterrupts & RB_IRQ_MASK_LPT2 ? 1 : 0) );
            CheckDlgButton( hWndDlg,
                            IDC_MASK_INT_TIMER_CB,
                            (LibParams.params.maskInterrupts & RB_IRQ_MASK_TIMER ? 1 : 0) );
            break;

       case WM_CLOSE:
           PostMessage(hWndDlg, (MESSAGE)WM_COMMAND, IDOK, 0L);
           break;

       case WM_COMMAND: 
           switch( GET_LOWORD(wParam) )
           {
               case IDOK:
                   ToggleButton( hWndDlg, IDOK );
                   EndDialog(    hWndDlg, TRUE );
                   break;

               default :
                   return( FALSE );

           }
           break;

       default:
           return( FALSE );
   }
   return( TRUE );
} /* end GetMaskIntsDlg */

/***************************************************************************
* Function : SetOsParamsDlg
*
* Purpose  : To handle the Set Operating System Parameters dialog box.
*
* Inputs   : hWndDlg - handle to a dialog.
*            Message - is the message to process.
*            wParam  - is a WPARAM size parameter, if any.
*            lParam  - is a LPARAM size parameter, if any.
*
* Outputs  : none.
*
* Returns  : BOOL    - TRUE  if we process the message
*                      FALSE if not.
****************************************************************************/
#ifdef __BORLANDC__
#pragma argsused
#endif
EXPORT_PROC
BOOL WINAPI SetOsParamsDlg( HWND    hWndDlg,
                            MESSAGE Message,
                            WPARAM  wParam,
                            LPARAM  lParam )
{
   static WORD OsParams = IDC_OS_TYPE_AUTO_RB;

   /* to avoid warnings */
   #ifndef __BORLANDC__
   lParam = lParam;
   #endif

   switch( Message )
   {
       case WM_INITDIALOG:
           LibParams.params.osParams.osVer = 0;
           CenterWindow(hWndDlg, 0);
           CheckRadioButton( hWndDlg,
                             IDC_OS_TYPE_AUTO_RB,
                             IDC_OS_TYPE_OS2_RB,
                             OsParams );
           break;

       case WM_CLOSE:
           PostMessage(hWndDlg, (MESSAGE)WM_COMMAND, IDCANCEL, 0L);
           break;

       case WM_COMMAND: 
           switch( GET_LOWORD(wParam) )
           {

               case IDOK:
                    ToggleButton( hWndDlg, IDOK );
                    switch( OsParams )
                    {
                         case IDC_OS_TYPE_AUTO_RB:
                              LibParams.params.osParams.osType = RB_AUTODETECT_OS_TYPE;
                              break;

                         case IDC_OS_TYPE_WIN32s_RB:
                              LibParams.params.osParams.osType = RB_OS_WIN32s;
                              break;

                         case IDC_OS_TYPE_WIN95_RB:
                              LibParams.params.osParams.osType = RB_OS_WIN95;
                              break;

                         case IDC_OS_TYPE_OS2_RB:
                              LibParams.params.osParams.osType = RB_OS_OS2;
                              break;

                         case IDC_OS_TYPE_WINNT_RB:
                              LibParams.params.osParams.osType = RB_OS_WINNT;
                              break;
                    }
                    DoCfgLibParams( ApiPacket, &LibParams );
                    EndDialog( hWndDlg, TRUE );
                    break;

               case IDC_OS_TYPE_AUTO_RB:
                  OsParams = IDC_OS_TYPE_AUTO_RB;
                  LibParams.params.osParams.osType = RB_AUTODETECT_OS_TYPE;
                  goto check_ostype_rb;

               case IDC_OS_TYPE_WIN32s_RB:
                  OsParams = IDC_OS_TYPE_WIN32s_RB;
                  LibParams.params.osParams.osType = RB_OS_WIN32s;
                  LibParams.params.osParams.osVer  = 0;
                  goto check_ostype_rb;

               case IDC_OS_TYPE_WIN95_RB:
                  OsParams = IDC_OS_TYPE_WIN95_RB;
                  LibParams.params.osParams.osType = RB_OS_WIN95;
                  LibParams.params.osParams.osVer  = 0;
                  goto check_ostype_rb;

               case IDC_OS_TYPE_WINNT_RB:
                  OsParams = IDC_OS_TYPE_WINNT_RB;
                  LibParams.params.osParams.osType = RB_OS_WINNT;
                  LibParams.params.osParams.osVer  = 0;
                  goto check_ostype_rb;

               case IDC_OS_TYPE_OS2_RB:
                  OsParams = IDC_OS_TYPE_OS2_RB;
                  LibParams.params.osParams.osType = RB_OS_OS2;
                  LibParams.params.osParams.osVer  = 0;
                  goto check_ostype_rb;

                  check_ostype_rb:;
                  CheckRadioButton( hWndDlg,
                                  IDC_OS_TYPE_AUTO_RB,
                                  IDC_OS_TYPE_OS2_RB,
                                  OsParams );
                  break;

               case IDCANCEL:
                  EndDialog( hWndDlg, TRUE );
                  break;

               default :
                   return( FALSE );

           }
           break;

       default:
           return( FALSE );
   }
   return( TRUE );
} /* end SetOsParamsDlg */

/***************************************************************************
* Function : GetOsParamsDlg
*
* Purpose  : To handle the Get Operating System dialog box.
*
* Inputs   : hWndDlg - handle to a dialog.
*            Message - is the message to process.
*            wParam  - is a WPARAM size parameter, if any.
*            lParam  - is a LPARAM size parameter, if any.
*
* Outputs  : none.
*
* Returns  : BOOL    - TRUE  if we process the message
*                      FALSE if not.
****************************************************************************/
#ifdef __BORLANDC__
#pragma argsused
#endif
EXPORT_PROC
BOOL WINAPI GetOsParamsDlg( HWND    hWndDlg,
                            MESSAGE Message,
                            WPARAM  wParam,
                            LPARAM  lParam )
{
   char buffer[255];

   /* to avoid warnings */
   #ifndef __BORLANDC__
   lParam = lParam;
   #endif

   switch( Message )
   {
       case WM_INITDIALOG:
            CenterWindow(hWndDlg, 0);
            if ( DoCfgLibParams( ApiPacket, &LibParams ) )
            {
               switch( LibParams.params.osParams.osType )
               {
                  case RB_OS_WIN3x:
                     CheckRadioButton( hWndDlg,
                                       IDC_OS_TYPE_WIN32s_RB,
                                       IDC_OS_TYPE_WIN3x_RB,
                                       IDC_OS_TYPE_WIN3x_RB );
                     break;

                  case RB_OS_WIN32s:
                     CheckRadioButton( hWndDlg,
                                       IDC_OS_TYPE_WIN32s_RB,
                                       IDC_OS_TYPE_WIN3x_RB,
                                       IDC_OS_TYPE_WIN32s_RB );
                     break;
               
                  case RB_OS_WIN95:
                     CheckRadioButton( hWndDlg,
                                       IDC_OS_TYPE_WIN32s_RB,
                                       IDC_OS_TYPE_WIN3x_RB,
                                       IDC_OS_TYPE_WIN95_RB );
                     break;
               
                  case RB_OS_WINNT:
                     CheckRadioButton( hWndDlg,
                                       IDC_OS_TYPE_WIN32s_RB,
                                       IDC_OS_TYPE_WIN3x_RB,
                                       IDC_OS_TYPE_WINNT_RB );
                     break;
               
                  case RB_OS_OS2:
                     CheckRadioButton( hWndDlg,
                                       IDC_OS_TYPE_WIN32s_RB,
                                       IDC_OS_TYPE_WIN3x_RB,
                                       IDC_OS_TYPE_OS2_RB );
                     break;

               }
               wsprintf( buffer, "0x%04X", LibParams.params.osParams.osVer );
               SetDlgItemText( hWndDlg,
                               IDC_OS_VER,
                               (LPSTR)buffer );
            }
            else
                 EndDialog( hWndDlg, TRUE );
            break;

       case WM_CLOSE:
           EndDialog( hWndDlg, TRUE );
           break;

       case WM_COMMAND: 
           switch( GET_LOWORD(wParam) )
           {
               case IDOK:
                   ToggleButton( hWndDlg, IDOK );
                   EndDialog(    hWndDlg, TRUE );
                   break;

               default :
                   return( FALSE );

           }
           break;

       default:
           return( FALSE );
   }
   return( TRUE );
} /* end GetOsParamsDlg */

/***************************************************************************
* Function : CfgLibDlg
*
* Purpose  : To handle the Libraray Configuration dialog box.
*
* Inputs   : hWndDlg - handle to a dialog.
*            Message - is the message to process.
*            wParam  - is a WPARAM size parameter, if any.
*            lParam  - is a LPARAM size parameter, if any.
*
* Outputs  : none.
*
* Returns  : BOOL    - TRUE  if we process the message
*                      FALSE if not.
****************************************************************************/
#ifdef __BORLANDC__
#pragma argsused
#endif
EXPORT_PROC
BOOL WINAPI CfgLibDlg( HWND    hWndDlg,
                       MESSAGE Message,
                       WPARAM  wParam,
                       LPARAM  lParam )
{
   static PPROC     proc          = NULL;
   static LPSTR     dialog        = NULL;
   static HWND      hGetSetButton = NULL;
   static HWND      hInitButton   = NULL;
          SP_STATUS spStatus;
          RB_WORD   fullStatus;

  /* to avoid warnings */
  #ifndef __BORLANDC__
  lParam = lParam;
  #endif

   switch( Message )
   {
       case WM_INITDIALOG:
           hGetSetButton = GetDlgItem( hWndDlg, IDC_SET_GET );
           hInitButton   = GetDlgItem( hWndDlg, IDOK );
           proc   = NULL;
           dialog = NULL;
           CenterWindow(hWndDlg, 0);
           CheckRadioButton( hWndDlg,
                             IDC_GET_CFG_RB,
                             IDC_SET_CFG_RB,
                             xCommand );
           CheckRadioButton( hWndDlg,
                             IDC_MACH_TYPE_FUNC_RB,
                             IDC_MASK_INTS_FUNC_RB,
                             xFunc );

           if ( xCommand == IDC_GET_CFG_RB )
           {
                SetWindowText( hGetSetButton, (LPSTR)"Get" );
                SetWindowText( hInitButton, (LPSTR)"OK" );
           }
           else
           {
                SetWindowText( hGetSetButton, (LPSTR)"Set" );
                SetWindowText( hInitButton, (LPSTR)"Initialize" );
           }
           break;

       case WM_SHOWWINDOW:
           SetFocus( hGetSetButton );
           SendDlgItemMessage( hWndDlg, IDOK,  BM_SETSTYLE, BS_PUSHBUTTON, (LPARAM)TRUE );
           SendMessage( hWndDlg, DM_SETDEFID, IDC_SET_GET, (LPARAM)0L );
           SendDlgItemMessage( hWndDlg, IDC_SET_GET, BM_SETSTYLE, BS_DEFPUSHBUTTON, (LPARAM)TRUE );
           break;

       case WM_CLOSE:
           EndDialog( hWndDlg, TRUE );
           break;

       case WM_COMMAND:
           switch( GET_LOWORD(wParam) )
           {

               case IDC_GET_CFG_RB:
                    xCommand = IDC_GET_CFG_RB;
                    SetFocus( hGetSetButton );
                    SendDlgItemMessage( hWndDlg, IDOK,  BM_SETSTYLE, BS_PUSHBUTTON, (LPARAM)TRUE );
                    SendMessage( hWndDlg, DM_SETDEFID, IDC_SET_GET, (LPARAM)0L );
                    SendDlgItemMessage( hWndDlg, IDC_SET_GET, BM_SETSTYLE, BS_DEFPUSHBUTTON, (LPARAM)TRUE );
                    goto check_rb;

               case IDC_SET_CFG_RB:
                    xCommand = IDC_SET_CFG_RB;
                    SetFocus( hGetSetButton );
                    SendDlgItemMessage( hWndDlg, IDOK,  BM_SETSTYLE, BS_PUSHBUTTON, (LPARAM)TRUE );
                    SendMessage( hWndDlg, DM_SETDEFID, IDC_SET_GET, (LPARAM)0L );
                    SendDlgItemMessage( hWndDlg, IDC_SET_GET, BM_SETSTYLE, BS_DEFPUSHBUTTON, (LPARAM)TRUE );
                    goto check_rb;

               case IDC_MACH_TYPE_FUNC_RB:
                    SendDlgItemMessage( hWndDlg, IDOK,  BM_SETSTYLE, BS_PUSHBUTTON, (LPARAM)TRUE );
                    SendMessage( hWndDlg, DM_SETDEFID, IDC_SET_GET, (LPARAM)0L );
                    SendDlgItemMessage( hWndDlg, IDC_SET_GET, BM_SETSTYLE, BS_DEFPUSHBUTTON, (LPARAM)TRUE );
                    SetFocus( hGetSetButton );
                    xFunc           = IDC_MACH_TYPE_FUNC_RB;
                    goto check_rb;

               case IDC_DELAY_FUNC_RB:
                    SendDlgItemMessage( hWndDlg, IDOK,  BM_SETSTYLE, BS_PUSHBUTTON, (LPARAM)TRUE );
                    SendMessage( hWndDlg, DM_SETDEFID, IDC_SET_GET, (LPARAM)0L );
                    SendDlgItemMessage( hWndDlg, IDC_SET_GET, BM_SETSTYLE, BS_DEFPUSHBUTTON, (LPARAM)TRUE );
                    SetFocus( hGetSetButton );
                    xFunc           = IDC_DELAY_FUNC_RB;
                    goto check_rb;

               case IDC_OS_PARMS_FUNC_RB:
                    SendDlgItemMessage( hWndDlg, IDOK,  BM_SETSTYLE, BS_PUSHBUTTON, (LPARAM)TRUE );
                    SendMessage( hWndDlg, DM_SETDEFID, IDC_SET_GET, (LPARAM)0L );
                    SendDlgItemMessage( hWndDlg, IDC_SET_GET, BM_SETSTYLE, BS_DEFPUSHBUTTON, (LPARAM)TRUE );
                    SetFocus( hGetSetButton );
                    xFunc           = IDC_OS_PARMS_FUNC_RB;
                    goto check_rb;

               case IDC_PORT_PARAMS_FUNC_RB:
                    SendDlgItemMessage( hWndDlg, IDOK,  BM_SETSTYLE, BS_PUSHBUTTON, (LPARAM)TRUE );
                    SendMessage( hWndDlg, DM_SETDEFID, IDC_SET_GET, (LPARAM)0L );
                    SendDlgItemMessage( hWndDlg, IDC_SET_GET, BM_SETSTYLE, BS_DEFPUSHBUTTON, (LPARAM)TRUE );
                    SetFocus( hGetSetButton );
                    xFunc           = IDC_PORT_PARAMS_FUNC_RB;
                    goto check_rb;

               case IDC_DRVR_TYPE_FUNC_RB:
                    SendDlgItemMessage( hWndDlg, IDOK,  BM_SETSTYLE, BS_PUSHBUTTON, (LPARAM)TRUE );
                    SendMessage( hWndDlg, DM_SETDEFID, IDC_SET_GET, (LPARAM)0L );
                    SendDlgItemMessage( hWndDlg, IDC_SET_GET, BM_SETSTYLE, BS_DEFPUSHBUTTON, (LPARAM)TRUE );
                    SetFocus( hGetSetButton );
                    xFunc           = IDC_DRVR_TYPE_FUNC_RB;
                    goto check_rb;

               case IDC_MASK_INTS_FUNC_RB:
                    SendDlgItemMessage( hWndDlg, IDOK,  BM_SETSTYLE, BS_PUSHBUTTON, (LPARAM)TRUE );
                    SendMessage( hWndDlg, DM_SETDEFID, IDC_SET_GET, (LPARAM)0L );
                    SendDlgItemMessage( hWndDlg, IDC_SET_GET, BM_SETSTYLE, BS_DEFPUSHBUTTON, (LPARAM)TRUE );
                    SetFocus( hGetSetButton );
                    xFunc           = IDC_MASK_INTS_FUNC_RB;
                    goto check_rb;

                    check_rb:;
                    if ( xCommand == IDC_GET_CFG_RB )
                    {
                         SetWindowText( hGetSetButton, (LPSTR)"Get" );
                         SetWindowText( hInitButton, (LPSTR)"OK" );
                    }
                    else
                    {
                         SetWindowText( hGetSetButton, (LPSTR)"Set" );
                         SetWindowText( hInitButton, (LPSTR)"Initialize" );
                    }
                    CheckRadioButton( hWndDlg,
                                      IDC_MACH_TYPE_FUNC_RB,
                                      IDC_MASK_INTS_FUNC_RB,
                                      xFunc );
                    CheckRadioButton( hWndDlg,
                                      IDC_GET_CFG_RB,
                                      IDC_SET_CFG_RB,
                                      xCommand );
                    break;

               case IDC_SET_GET:
                   ToggleButton( hWndDlg, IDC_SET_GET );
                   SendDlgItemMessage( hWndDlg, IDC_SET_GET,  BM_SETSTYLE, BS_PUSHBUTTON, (LPARAM)TRUE );
                   SendMessage( hWndDlg, DM_SETDEFID, IDOK, (LPARAM)0L );
                   SendDlgItemMessage( hWndDlg, IDOK, BM_SETSTYLE, BS_DEFPUSHBUTTON, (LPARAM)TRUE );
                   SetFocus( hInitButton );
                   if ( xCommand == IDC_SET_CFG_RB )
                   {
                         LibParams.cmd = RB_SET_LIB_PARAMS_CMD;
                   }
                   else
                   {
                         LibParams.cmd = RB_GET_LIB_PARAMS_CMD;
                   }
                   if ( xFunc == IDC_MASK_INTS_FUNC_RB )
                   {
                         LibParams.func = RB_MASK_INTS_FUNC;
                         proc = MakeProcInstance(
                         (PPROC)(xCommand == IDC_SET_CFG_RB ?
                                 SetMaskIntsDlg : GetMaskIntsDlg),
                                 ghInst );
                         dialog = (xCommand == IDC_SET_CFG_RB ?
                                   SET_MASK_INTS_DLG : GET_MASK_INTS_DLG);
                   }
                   else if ( xFunc == IDC_DRVR_TYPE_FUNC_RB )
                   {
                         LibParams.func = RB_ROUTER_FLAGS_FUNC;
                         proc = MakeProcInstance(
                         (PPROC)(xCommand == IDC_SET_CFG_RB ?
                                 SetDrvrTypeDlg : GetDrvrTypeDlg),
                                 ghInst );
                         dialog = (xCommand == IDC_SET_CFG_RB ?
                                   SET_DRVR_TYPE_DLG : GET_DRVR_TYPE_DLG);
                   }
                   else if ( xFunc == IDC_PORT_PARAMS_FUNC_RB )
                   {
                         LibParams.func = RB_PORT_PARAMS_FUNC;
                         proc = MakeProcInstance(
                         (PPROC)(xCommand == IDC_SET_CFG_RB ?
                                 SetPortParamsDlg : GetPortParamsDlg),
                                 ghInst );
                         dialog = (xCommand == IDC_SET_CFG_RB ?
                                   SET_PORT_PARMS_DLG : GET_PORT_PARMS_DLG);
                   }
                   else if ( xFunc == IDC_OS_PARMS_FUNC_RB )
                   {
                         LibParams.func = RB_OS_PARAMS_FUNC;
                         proc = MakeProcInstance(
                         (PPROC)(xCommand == IDC_SET_CFG_RB ?
                                 SetOsParamsDlg : GetOsParamsDlg),
                                 ghInst );
                         dialog = (xCommand == IDC_SET_CFG_RB ?
                                   SET_OS_PARMS_DLG : GET_OS_PARMS_DLG);

                   }
                   else if ( xFunc == IDC_DELAY_FUNC_RB )
                   {
                         LibParams.func = RB_DELAY_FUNC;
                         proc = MakeProcInstance(
                         (PPROC)(xCommand == IDC_SET_CFG_RB ?
                                 SetDelayDlg : GetDelayDlg),
                                 ghInst );
                         dialog = (xCommand == IDC_SET_CFG_RB ?
                                   SET_DELAY_DLG : GET_DELAY_DLG);
                   }
                   else if ( xFunc == IDC_MACH_TYPE_FUNC_RB )
                   {
                         LibParams.func = RB_MACHINE_TYPE_FUNC;
                         proc = MakeProcInstance(
                         (PPROC)(xCommand == IDC_SET_CFG_RB ?
                                 SetMachTypeDlg : GetMachTypeDlg),
                                 ghInst );
                         dialog = (xCommand == IDC_SET_CFG_RB ?
                                   SET_MACH_TYPE_DLG : GET_MACH_TYPE_DLG);
                   }
                   if ( proc && dialog )
                   {
                         DialogBox( ghInst, dialog, hWndDlg, proc );
                         xFreeProcInstance( proc );
                   }
                   break;

               case IDOK:
                   ToggleButton( hWndDlg, IDOK );
                   if ( xCommand == IDC_SET_CFG_RB )
                   {
                         spStatus = RNBOsproInitialize( ApiPacket );
                         
                         if ( spStatus != SP_SUCCESS )
                         {
                               init_err:;
                               fullStatus = RNBOsproGetFullStatus( ApiPacket );
                         
                               FormatErrStr( (LPSTR)gMsg,
                                             IDS_ERR_INIT_FAILED,
                                             spStatus,
                                             fullStatus );
                         
                               ErrorMsg( NULL, (LPSTR)gMsg );
                          }
                          else
                          {
                               LoadString( ghInst,
                                           IDS_INIT,
                                           (LPSTR)gString,
                                           sizeof(gString) );
                         
                               LoadString( ghInst,
                                           IDS_INIT_SUCCESS,
                                           (LPSTR)gResult,
                                           sizeof(gResult) );
                         
                               MessageBox( NULL,
                                          (LPSTR)gResult,
                                          (LPSTR)gString,
                                          MB_OK | MB_TASKMODAL );
                         }
                   }
                   EndDialog( hWndDlg, TRUE );
                   break;

               case IDCANCEL:
                  if ( xCommand == IDC_SET_CFG_RB )
                  {
                         RNBOsproFormatPacket( ApiPacket, sizeof(ApiPacket) );
                         spStatus = RNBOsproInitialize( ApiPacket );
                         if ( spStatus != SP_SUCCESS )
                         {
                              goto init_err;
                         }
                  }  
                  EndDialog( hWndDlg, TRUE );
                  break;

               default :
                   return( FALSE );

           }
           break;

       default:
           return( FALSE );
   }
   return( TRUE );
} /* end CfgLibDlg */


/****************************************************************************
* Procedure : ToggleButton
*
* Purpose   : To toggle a buttons state and set's it's focus.
*
* Inputs    : none.
*
* Outputs   : none.
*
* Returns   : none.
****************************************************************************/
void ToggleButton( HWND hWnd, WORD theButton )
{
   register DWORD x = 100000;
   SetFocus( GetDlgItem( hWnd, theButton ) );
   SendDlgItemMessage( hWnd, theButton, (MESSAGE)BM_SETSTATE, (WPARAM)TRUE, (LPARAM)NULL );
   while( x-- ); /* delay to get depressed button effect */
   SendDlgItemMessage( hWnd, theButton, (MESSAGE)BM_SETSTATE, (WPARAM)FALSE, (LPARAM)NULL );
} /* end ToggleButton */

/****************************************************************************
* Procedure : ResetPW
*
* Purpose   : To reset all passwords.
*
* Inputs    : none.
*
* Outputs   : none.
*
* Returns   : none.
****************************************************************************/
LOCAL_PROC
void ResetPW( void )
{
   gOverwritePW1 = gOverwritePW2 = gWritePW = 0;
} /* end ResetPW */

/****************************************************************************
* Function : InitInstance
*
* Purpose  : To initialize the applicaton's instance and globals.
*
* Inputs   : hInstance - is a handle to the current instance.
*            nCmdShow  - is the attributes of displaying the window.
*
* Outputs  : none.
*
* Returns  : BOOL - TRUE if successful, else FALSE.
****************************************************************************/
#ifdef __BORLANDC__
#pragma argsused
#endif
LOCAL_PROC
BOOL InitInstance( HANDLE hInstance, int nCmdShow )
{
   /* to avoid warnings */
   #ifndef __BORLANDC__
   nCmdShow = nCmdShow;
   #endif

   ghHourGlassCursor = LoadCursor( NULL, IDC_WAIT );
   if ( !ghHourGlassCursor ) return( FALSE );

   /* create application's Main window  */
   ghWndMain = CreateWindow( gAppName,         /* Window class name        */
                             "SUPERPRO EVAL",  /* Window's title           */
                              WS_CLIPCHILDREN |/* don't draw in child windows areas */
                              WS_MAXIMIZE |
                              WS_OVERLAPPED,
                              CW_USEDEFAULT, 0,/* Use default X, Y         */
                              CW_USEDEFAULT, 0,/* Use default X, Y         */
                              NULL,            /* Parent window's handle   */
                              NULL,            /* Default to Class Menu    */
                              hInstance,       /* Instance of window       */
                              NULL );          /* Create struct for WM_CREATE */

   if ( !ghWndMain ) return( FALSE );
   LoadString( hInstance, IDS_SP_STATUS, (LPSTR)gStatus, sizeof(gStatus) );
   LoadString( hInstance, IDS_SP_FULL_STATUS, (LPSTR)gFullStatus, sizeof(gFullStatus) );
   return( TRUE );
} /* end InitInstance */

/****************************************************************************
* Function : InitApplication.
*
* Purpose  : Initializes the window class.
*
* Inputs   : hInstance - is a handle to the current instance.
*
* Outputs  : none.
*
* Returns  : BOOL - TRUE if successful, else FALSE.
****************************************************************************/
LOCAL_PROC
BOOL InitApplication( HANDLE hInstance )
{
   HANDLE      hMem;
   PWNDCLASS   pWndClass;
   BOOL        bSuccess;

   hMem = LocalAlloc( LMEM_FIXED | LMEM_ZEROINIT, sizeof(WNDCLASS) );
   if ( !hMem ) return( FALSE );
   pWndClass = (PWNDCLASS)LocalLock( hMem );

   if ( !pWndClass )
   {
       LocalUnlock( hMem );
       LocalFree( hMem );
       return( FALSE );
   }

   /* load WNDCLASS with window's characteristics */
   pWndClass->style       = CS_BYTEALIGNWINDOW;
#if defined(__BORLANDC__)
   ((PPROC)pWndClass->lpfnWndProc) = (PPROC)WndProc;
#else
   pWndClass->lpfnWndProc = (WNDPROC)WndProc;
#endif

   /* Extra storage for Class and Window objects */
   pWndClass->cbClsExtra = 0;
   pWndClass->cbWndExtra = 0;
   pWndClass->hInstance  = hInstance;
   pWndClass->hIcon      = LoadIcon(ghInst, "RNBO");
   pWndClass->hCursor    = LoadCursor(NULL, IDC_ARROW);

   /* Create brush for erasing background */
   pWndClass->hbrBackground = (HBRUSH)(COLOR_WINDOW+1);
   pWndClass->lpszMenuName  = (LPSTR)NULL;          /* Menu Name is App Name */
   pWndClass->lpszClassName = gAppName;             /* Class Name is App Name*/

   /* register the window class */
   bSuccess = RegisterClass( pWndClass );

   LocalUnlock( hMem );
   LocalFree( hMem );
   return( bSuccess );
} /* end InitApplication */

/****************************************************************************
* Procedure : CenterWindow
*
* Purpose   : Centers a window based on the client area of its parent.
*
* Inputs    : hWnd - is a handle to the window to center.
*             top  - amount to adjust the window position up or down.
*
* Outputs   : none.
*
* Returns   : none.
****************************************************************************/
LOCAL_PROC
void CenterWindow( HWND hWnd,
                   int  top )
{
   POINT pt;
   RECT  swp;
   RECT  rParent;
   int   iwidth;
   int   iheight;

   /* get the rectangles for the parent and the child */
   GetWindowRect( hWnd, &swp );
   GetClientRect( ghWndMain, &rParent );

   /* calculate the height and width for MoveWindow */
   iwidth = swp.right - swp.left;
   iheight = swp.bottom - swp.top;

   /* find the center point and convert to screen coordinates */
   pt.x = (rParent.right - rParent.left) / 2;
   pt.y = (rParent.bottom - rParent.top) / 2;
   ClientToScreen(ghWndMain, &pt);

   /* calculate the new x, y starting point    */
   pt.x = pt.x - (iwidth / 2);
   pt.y = pt.y - (iheight / 2);

   /* top will adjust the window position, up or down */
   if ( top ) pt.y = pt.y + top;

   /* move the window */
   MoveWindow( hWnd, pt.x, pt.y, iwidth, iheight, FALSE );
} /* end CenterWindow */

/***************************************************************************
* Function : ConvertBufferToHex
*
* Purpose  : Converts a character buffer to hexidecimal values.
*
* Inputs   : buffer - is a pointer to a char buffer that contains the data
*                     to validate.
*            buflen - is the length of the buffer (must be even # of bytes).
*
* Outputs  : None.
*
* Returns  : The number of bytes converted.
****************************************************************************/
LOCAL_PROC
unsigned int ConvertBufferToHex( PSTR buffer, unsigned int bufLen )
{
   register unsigned int i,j;
   char                  hexBuffer[SPRO_MAX_QUERY_SIZE];

   /** Check for even length buffer **/
   if ( !(bufLen & 0xFFFE) || (bufLen/2) > sizeof(hexBuffer) )  return( 0 );

   for ( i = j = 0; i <= bufLen; j++)
   {
        sscanf( &buffer[i], "%2hx", (unsigned short int *)&(hexBuffer[ j ]) ) ;
        i = i + 2 ;
   }
   for( i=0; i < j; i++ ) buffer[ i ] = hexBuffer[ i ];
   i--;
   return( i );
} /* end ConvertBufferToHex */

/****************************************************************************
* Funciton : ValidateQryStr
*
* Purpose  : To verify a query string is valid.
*
* Inputs   : qData - is a pointer to the query data.
*            len   - is the buffer length in bytes.
*
* Outpus   : NONE
*
* Returns  : WORD  - 0 if data is valid.
*                    1 if length is < 8 hex digits.
*                    2 if length is not an even number of hex digits.
*                    3 if data is not all hexidecimal.
****************************************************************************/
LOCAL_PROC
WORD ValidateQryStr( LPSTR qData, WORD len )
{
   BOOL  hex;
   LPSTR cp;

   /** Make sure length is at least 8 char **/
   if ( len < 8 ) return( 1 );

   /** Make sure even length string **/
   if ( (len & 0x0001) ) return( 2 );

   /**  check if hex string.  **/
   for ( cp=qData, hex=TRUE; (hex && len); hex = isxdigit(*cp++), len-- );
   if ( !hex ) return( 3 );
   return( 0 );

} /* end ValidateQryStr */

/***************************************************************************
* Procedure : ErrorMsg
*
* Purpose   : To display a MessageBox for errors.
*
* Inputs    : hWnd    - the handle to the parent window that generated the
*                       error.
*             msgText - is a pointer to the error message to display.
*
* Outputs   : none.
*
* Returns   : none.
****************************************************************************/
LOCAL_PROC
void ErrorMsg( HWND hWnd, LPSTR msgText )
{
   MessageBeep( 0 );
   MessageBox( hWnd, msgText, NULL, MB_ICONEXCLAMATION | MB_TASKMODAL );
} /* end ErrorMsg */

/***************************************************************************
* Function : GetDlgItemWord
*
* Purpose  : To retrieve a data word from a control within a dialog.
*
* Inputs   : hWnd    - handle to a window that contains the control.
*            cntrlID - the control ID to retrieve data from.
*            theData - is a pointer to a word.
*
* Outputs  : theData - has the WORD value of the control if successful,
*                      else undefined.
*
* Returns  : WORD    - 0 if successful.
*                      1 if no data exists within the control.
*                      2 if bad data format.
****************************************************************************/
LOCAL_PROC
WORD GetDlgItemWord( HWND hWnd, WORD cntrlID, WORD *theData )
{
   char dataString[ (sizeof(WORD)*2)+1 ];

   GetDlgItemText( hWnd, cntrlID, (LPSTR)dataString, sizeof(dataString) );

   if ( !lstrlen( dataString ) ) return( 1 );
   if ( !sscanf( dataString, "%4hx", theData ) ) return( 2 );
   return( 0 );
} /* end GetDlgItemWord */

/***************************************************************************
* Function : GetCellAddress
*
* Purpose  : To retrieve a cell address from a dialog control.
*
* Inputs   : hWnd    - handle to a window that contains the control.
*            cntrlID - the control ID to retrieve data from.
*            theCell - is a pointer to a word.
*
* Outputs  : theCell - has the cell address if successful, else undefined.
*
* Returns  : BOOL    - TRUE  if successful.
*                      FALSE if not.
****************************************************************************/
LOCAL_PROC
BOOL GetCellAddress( HWND hWnd, WORD cntrlID, WORD *theCell )
{
   BOOL status = FALSE;

   switch( GetDlgItemWord( hWnd, cntrlID, theCell ) )
   {
       case 0:
           status = TRUE;
           break;

       case 1:
           LoadString( ghInst,
                       IDS_ERR_NO_ADDR,
                       (LPSTR)gString,
                       sizeof(gString) );
           ErrorMsg( NULL, gString );
           status = FALSE;
           break;

       case 2:
           LoadString( ghInst,
                       IDS_ERR_INVALID_INPUT,
                       (LPSTR)gString,
                       sizeof(gString) );
           ErrorMsg( NULL, gString );
           status = FALSE;
           break;
   }
   return( status );
} /* end GetCellAddress */

/***************************************************************************
* Function : GetWritePassword
*
* Purpose  : To retrieve a password from a dialog control.
*
* Inputs   : hWnd    - handle to a window that contains the control.
*            cntrlID - the control ID to retrieve data from.
*            thePW   - is a pointer to a word.
*
* Outputs  : thePW   - has the password if successful, else undefined.
*
* Returns  : BOOL    - TRUE  if successful.
*                      FALSE if not.
****************************************************************************/
LOCAL_PROC
BOOL GetWritePassword( HWND hWnd, WORD cntrlID, WORD *thePW )
{
   BOOL status = FALSE;
    
   switch( GetDlgItemWord( hWnd, cntrlID, thePW ) )
   {
       case 0:
           status = TRUE;
           break;

       case 1:
           LoadString( ghInst,
                       IDS_ERR_NO_WRITE_PW,
                       (LPSTR)gString,
                       sizeof(gString) );
           ErrorMsg( NULL, gString );
           status = FALSE;
           break;

       case 2:
           LoadString( ghInst,
                       IDS_ERR_INVALID_INPUT,
                       (LPSTR)gString,
                       sizeof(gString) );
           ErrorMsg( NULL, gString );
           status = FALSE;
           break;
   }
   return( status );
} /* end GetWritePassword */

/***************************************************************************
* Function : GetActivate1Password
*
* Purpose  : To retrieve a password from a dialog control.
*
* Inputs   : hWnd    - handle to a window that contains the control.
*            cntrlID - the control ID to retrieve data from.
*            thePW   - is a pointer to a word.
*
* Outputs  : thePW   - has the password if successful, else undefined.
*
* Returns  : BOOL    - TRUE  if successful.
*                      FALSE if not.
****************************************************************************/
LOCAL_PROC
BOOL GetActivate1Password( HWND hWnd, WORD cntrlID, WORD *thePW )
{
   BOOL status = FALSE;
    
   switch( GetDlgItemWord( hWnd, cntrlID, thePW ) )
   {
       case 0:
           status = TRUE;
           break;

       case 1:
           LoadString( ghInst,
                       IDS_ERR_NO_ACTIVATE_PW1,
                       (LPSTR)gString,
                       sizeof(gString) );
           ErrorMsg( NULL, gString );
           status = FALSE;
           break;

       case 2:
           LoadString( ghInst,
                       IDS_ERR_INVALID_INPUT,
                       (LPSTR)gString,
                       sizeof(gString) );
           ErrorMsg( NULL, gString );
           status = FALSE;
           break;
   }
   return( status );
} /* end GetActivate1Password */

/***************************************************************************
* Function : GetActivate2Password
*
* Purpose  : To retrieve a password from a dialog control.
*
* Inputs   : hWnd    - handle to a window that contains the control.
*            cntrlID - the control ID to retrieve data from.
*            thePW   - is a pointer to a word.
*
* Outputs  : thePW   - has the password if successful, else undefined.
*
* Returns  : BOOL    - TRUE  if successful.
*                      FALSE if not.
****************************************************************************/
LOCAL_PROC
BOOL GetActivate2Password( HWND hWnd, WORD cntrlID, WORD *thePW )
{
   BOOL status = FALSE;
    
   switch( GetDlgItemWord( hWnd, cntrlID, thePW ) )
   {
       case 0:
           status = TRUE;
           break;

       case 1:
           LoadString( ghInst,
                       IDS_ERR_NO_ACTIVATE_PW2,
                       (LPSTR)gString,
                       sizeof(gString) );
           ErrorMsg( NULL, gString );
           status = FALSE;
           break;

       case 2:
           LoadString( ghInst,
                       IDS_ERR_INVALID_INPUT,
                       (LPSTR)gString,
                       sizeof(gString) );
           ErrorMsg( NULL, gString );
           status = FALSE;
           break;
   }
   return( status );
} /* end GetActivate2Password */

/***************************************************************************
* Function : GetOverwrite2Password
*
* Purpose  : To retrieve a password from a dialog control.
*
* Inputs   : hWnd    - handle to a window that contains the control.
*            cntrlID - the control ID to retrieve data from.
*            thePW   - is a pointer to a word.
*
* Outputs  : thePW   - has the password if successful, else undefined.
*
* Returns  : BOOL    - TRUE  if successful.
*                      FALSE if not.
****************************************************************************/
LOCAL_PROC
BOOL GetOverwrite2Password( HWND hWnd, WORD cntrlID, WORD *thePW )
{
   BOOL status = FALSE;
    
   switch( GetDlgItemWord( hWnd, cntrlID, thePW ) )
   {
       case 0:
           status = TRUE;
           break;

       case 1:
           LoadString( ghInst,
                       IDS_ERR_NO_OVERWRITE_PW2,
                       (LPSTR)gString,
                       sizeof(gString) );
           ErrorMsg( NULL, gString );
           status = FALSE;
           break;

       case 2:
           LoadString( ghInst,
                       IDS_ERR_INVALID_INPUT,
                       (LPSTR)gString,
                       sizeof(gString) );
           ErrorMsg( NULL, gString );
           status = FALSE;
           break;
   }
   return( status );
} /* end GetOverwrite2Password */

/***************************************************************************
* Function : GetOverwrite1Password
*
* Purpose  : To retrieve a password from a dialog control.
*
* Inputs   : hWnd    - handle to a window that contains the control.
*            cntrlID - the control ID to retrieve data from.
*            thePW   - is a pointer to a word.
*
* Outputs  : thePW   - has the password if successful, else undefined.
*
* Returns  : BOOL    - TRUE  if successful.
*                      FALSE if not.
****************************************************************************/
LOCAL_PROC
BOOL GetOverwrite1Password( HWND hWnd, WORD cntrlID, WORD *thePW )
{
   BOOL status = FALSE;
    
   switch( GetDlgItemWord( hWnd, cntrlID, thePW ) )
   {
       case 0:
           status = TRUE;
           break;

       case 1:
           LoadString( ghInst,
                       IDS_ERR_NO_OVERWRITE_PW1,
                       (LPSTR)gString,
                       sizeof(gString) );
           ErrorMsg( NULL, gString );
           status = FALSE;
           break;

       case 2:
           LoadString( ghInst,
                       IDS_ERR_INVALID_INPUT,
                       (LPSTR)gString,
                       sizeof(gString) );
           ErrorMsg( NULL, gString );
           status = FALSE;
           break;
   }
   return( status );
} /* end GetOverwrite1Password */

/***************************************************************************
* Function : GetDataValue
*
* Purpose  : To retrieve a data value from a dialog control.
*
* Inputs   : hWnd    - handle to a window that contains the control.
*            cntrlID - the control ID to retrieve data from.
*            theData - is a pointer to a word.
*
* Outputs  : theData - has the data if successful, else undefined.
*
* Returns  : BOOL    - TRUE  if successful.
*                      FALSE if not.
****************************************************************************/
LOCAL_PROC
BOOL GetDataValue( HWND hWnd, WORD cntrlID, WORD *theData )
{
   BOOL status = FALSE;

   switch( GetDlgItemWord( hWnd, cntrlID, theData ) )
   {
       case 0:
           status = TRUE;
           break;

       case 1:
           LoadString( ghInst,
                       IDS_ERR_NO_VALUE,
                       (LPSTR)gString,
                       sizeof(gString) );
           ErrorMsg( NULL, gString );
           status = FALSE;
           break;

       case 2:
           LoadString( ghInst,
                       IDS_ERR_INVALID_INPUT,
                       (LPSTR)gString,
                       sizeof(gString) );
           ErrorMsg( NULL, gString );
           status = FALSE;
           break;
   }
   return( status );
} /* end GetDataValue */

/***************************************************************************
* Function : GetAccessCode
*
* Purpose  : To retrieve a access code from a dialog control.
*
* Inputs   : hWnd    - handle to a window that contains the control.
*            cntrlID - the control ID to retrieve data from.
*            theAC   - is a pointer to a byte.
*
* Outputs  : theAC   - has the access code if successful, else undefined.
*
* Returns  : BOOL    - TRUE  if successful.
*                      FALSE if not.
****************************************************************************/
LOCAL_PROC
BOOL GetAccessCode( HWND hWnd, WORD cntrlID, BYTE *theAC )
{
   WORD dummy;
   BOOL status = FALSE;

   switch( GetDlgItemWord( hWnd, cntrlID, &dummy ) )
   {
       case 0:
           *theAC = (BYTE)dummy;
           status = TRUE;
           break;

       case 1:
           LoadString( ghInst,
                       IDS_ERR_NO_ACCESS_CODE,
                       (LPSTR)gString,
                       sizeof(gString) );
           ErrorMsg( NULL, gString );
           status = FALSE;
           break;

       case 2:
           LoadString( ghInst,
                       IDS_ERR_INVALID_INPUT,
                       (LPSTR)gString,
                       sizeof(gString) );
           ErrorMsg( NULL, gString );
           status = FALSE;
           break;
   }
   return( status );
} /* end GetAccessCode */

/***************************************************************************
* Procedure : ForamtErrStr
*
* Purpose   : To format a ErrorMsg string.
*
* Inputs    : theString   - is a pointer to the string to contain the message.
*             idsIdx      - is the index into the string table of the message.
*             spStatus    - is the SP_STATUS code to display.
*             spFullStatus- is the RB_WORD code to display.
*
* Outputs   : theString   - is updated.
*
* Returns   : none.
****************************************************************************/
LOCAL_PROC
void FormatErrStr( LPSTR     theString,
                   int       idsIdx,
                   SP_STATUS spStatus,
                   RB_WORD   spFullStatus )
{

   char str[STR_BUF_SIZE];
   char tmp[STR_BUF_SIZE];
   int  statusIdx;

   switch( spStatus )
   {
       case SP_SUCCESS:
           statusIdx = IDS_SP_SUCCESS;
           break;

       case SP_MEM_ALIGNMENT_ERROR:
           statusIdx = IDS_SP_MEM_ALIGNMENT_ERROR;
           break;

       case SP_INVALID_FUNCTION_CODE:
           statusIdx = IDS_SP_INVALID_FUNCTION_CODE;
           break;

       case SP_ACCESS_DENIED:
           statusIdx = IDS_SP_ACCESS_DENIED;
           break;

       case SP_INVALID_MEMORY_ADDRESS:
           statusIdx = IDS_SP_INVALID_MEMORY_ADDRESS;
           break;

       case SP_INVALID_ACCESS_CODE:
           statusIdx = IDS_SP_INVALID_ACCESS_CODE;
           break;

       case SP_WRITE_NOT_READY:
           statusIdx = IDS_SP_WRITE_NOT_READY;
           break;

       case SP_ALREADY_ZERO:
           statusIdx = IDS_SP_ALREADY_ZERO;
           break;

       case SP_MEM_ACCESS_ERROR:
           statusIdx = IDS_SP_MEM_ACCESS_ERROR;
           break;

       case SP_INVALID_PACKET:
           statusIdx = IDS_SP_INVALID_PACKET;
           break;

       case SP_VERSION_NOT_SUPPORTED:
           statusIdx = IDS_SP_VERSION_NOT_SUPPORTED;
           break;

       case SP_INVALID_PARAMETER:
           statusIdx = IDS_SP_INVALID_PARAMETER;
           break;

       case SP_PACKET_TOO_SMALL:
           statusIdx = IDS_SP_PACKET_TOO_SMALL;
           break;

       case SP_UNIT_NOT_FOUND:
           statusIdx = IDS_SP_UNIT_NOT_FOUND;
           break;

       case SP_PORT_IS_BUSY:
           statusIdx = IDS_SP_PORT_IS_BUSY;
           break;

       case SP_NO_PORT_FOUND:
           statusIdx = IDS_SP_NO_PORT_FOUND;
           break;

       case SP_QUERY_TOO_LONG:
           statusIdx = IDS_SP_QUERY_TOO_LONG;
           break;

       case SP_OS_NOT_SUPPORTED:
           statusIdx = IDS_SP_OS_NOT_SUPPORTED;
           break;

       case SP_DRIVER_OPEN_ERROR:
           statusIdx = IDS_SP_DRIVER_OPEN_ERROR;
           break;

       case SP_DRIVER_NOT_INSTALLED:
           statusIdx = IDS_SP_DRIVER_NOT_INSTALLED;
           break;

       case SP_IO_COMMUNICATIONS_ERROR:
           statusIdx = IDS_SP_IO_COMMUNICATIONS_ERROR;
           break;

       case SP_INVALID_COMMAND:
            statusIdx = IDS_SP_INVALID_COMMAND;
            break;

       case SP_PORT_ALLOCATION_FAILURE:
            statusIdx = IDS_SP_PORT_ALLOCATION_FAILURE;
            break;

       case SP_PORT_RELEASE_FAILURE:
            statusIdx = IDS_SP_PORT_RELEASE_FAILURE;
            break;

       case SP_ACQUIRE_PORT_TIMEOUT:
            statusIdx = IDS_SP_ACQUIRE_PORT_TIMEOUT;
            break;

       case SP_SIGNAL_NOT_SUPPORTED:
            statusIdx = IDS_SP_SIGNAL_NOT_SUPPORTED;
            break;

       case SP_UNKNOWN_MACHINE:
            statusIdx = IDS_SP_UNKNOWN_MACHINE;
            break;

       case SP_SYS_API_ERROR:
            statusIdx = IDS_SP_SYS_API_ERROR;
            break;

       case SP_UNIT_IS_BUSY:
            statusIdx = IDS_SP_UNIT_IS_BUSY;
            break;

       case SP_INVALID_PORT_TYPE:
            statusIdx = IDS_SP_INVALID_PORT_TYPE;
            break;

       case SP_INVALID_MACH_TYPE:
            statusIdx = IDS_SP_INVALID_MACH_TYPE;
            break;

       case SP_INVALID_IRQ_MASK:
            statusIdx = IDS_SP_INVALID_IRQ_MASK;
            break;

       case SP_INVALID_CONT_METHOD:
            statusIdx = IDS_SP_INVALID_CONT_METHOD;
            break;

       case SP_INVALID_PORT_FLAGS:
            statusIdx = IDS_SP_INVALID_PORT_FLAGS;
            break;

       case SP_INVALID_LOG_PORT_CFG:
            statusIdx = IDS_SP_INVALID_LOG_PORT_CFG;
            break;

       case SP_INVALID_OS_TYPE:
            statusIdx = IDS_SP_INVALID_OS_TYPE;
            break;

       case SP_INVALID_LOG_PORT_NUM:
            statusIdx = IDS_SP_INVALID_LOG_PORT_NUM;
            break;

       case SP_INVALID_ROUTER_FLGS:
            statusIdx = IDS_SP_INVALID_ROUTER_FLGS;
            break;

       case SP_INIT_NOT_CALLED:
            statusIdx = IDS_SP_INIT_NOT_CALLED;
            break;

       case SP_DRVR_TYPE_NOT_SUPPORTED:
            statusIdx = IDS_SP_DRIVER_TYPE_NOT_SUPPORTED;
            break;

       case SP_FAIL_ON_DRIVER_COMM:
            statusIdx = IDS_SP_FAIL_ON_DRIVER_COMM;
            break;

       default:
           statusIdx = IDS_SP_INTERNAL_ERROR;
           break;
   }

   LoadString( ghInst, idsIdx, (LPSTR)str, sizeof(str) );
   wsprintf( theString, "%s\n", str );
   LoadString( ghInst, statusIdx, (LPSTR)str, sizeof(str) );
   wsprintf( tmp, "%s %d (%s)\n", gStatus, spStatus, str );
   lstrcat( theString, tmp );
   wsprintf( str, "%s 0x%04X", gFullStatus, spFullStatus );
   lstrcat( theString, str );
} /* end FormatErrStr */

/***************************************************************************
* Function : VERSIONMsgProc
*
* Purpose  : To handle the Version dialog box.
*
* Inputs   : hWndDlg - handle to a dialog.
*            Message - is the message to process.
*            wParam  - is a WPARAM size parameter, if any.
*            lParam  - is a LPARAM size parameter, if any.
*
* Outputs  : none.
*
* Returns  : BOOL    - TRUE  if we process the message
*                      FALSE if not.
****************************************************************************/
#ifdef __BORLANDC__
#pragma argsused
#endif
EXPORT_PROC
BOOL WINAPI VERSIONMsgProc( HWND    hWndDlg,
                                MESSAGE Message,
                                WPARAM  wParam,
                                LPARAM  lParam )
{
   RB_BYTE    majVer;
   RB_BYTE    minVer;
   RB_BYTE    rev;
   RB_BYTE    osDrvrType;
   SP_STATUS  status;
   RB_WORD    fullStatus;

   /* to avoid warnings */
   #ifndef __BORLANDC__
   lParam = lParam;
   #endif

   switch( Message )
   {
       case WM_INITDIALOG:
           CenterWindow( hWndDlg, 0 );
           /* initialize working variables                                 */

  /************************************************************************/
  /* RNBOsproGetVersion: Retrieves version information.                   */
  /************************************************************************/

           status = RNBOsproGetVersion(ApiPacket,
                                       &majVer,
                                       &minVer,
                                       &rev,
                                       &osDrvrType );

           if ( status != SP_SUCCESS )
           {
               fullStatus = RNBOsproGetFullStatus( ApiPacket );

               FormatErrStr( (LPSTR)gMsg,
                             IDS_ERR_VERSION_FAILED,
                             status,
                             fullStatus );
               ErrorMsg( NULL, (LPSTR)gMsg );
           }
           else
           {
               wsprintf( gString, "%2d", majVer );
               SetDlgItemText( hWndDlg, SP_VER_MAJOR, (LPSTR)gString );
               wsprintf( gString, "%2d", minVer );
               SetDlgItemText( hWndDlg, SP_VER_MINOR, (LPSTR)gString );
               wsprintf( gString, "%2d", rev );
               SetDlgItemText( hWndDlg, SP_VER_REV, (LPSTR)gString );

               switch( osDrvrType )
               {
                   case RB_WIN3x_LOCAL_DRVR:
                       LoadString( ghInst,
                                   IDS_WIN3x_LOCAL_DRVR,
                                   (LPSTR)gResult,
                                   sizeof(gResult) );
                       break;

                   case RB_WIN3x_SYS_DRVR:
                       LoadString( ghInst,
                                   IDS_WIN3x_SYS_DRVR,
                                   (LPSTR)gResult,
                                   sizeof(gResult) );
                       break;

                   case RB_WINNT_SYS_DRVR:
                       LoadString( ghInst,
                                   IDS_WINNT_SYS_DRVR,
                                   (LPSTR)gResult,
                                   sizeof(gResult) );
                       break;

                   case RB_WIN32s_LOCAL_DRVR:
                       LoadString( ghInst,
                                   IDS_WIN32s_LOCAL_DRVR,
                                   (LPSTR)gResult,
                                   sizeof(gResult) );
                       break;

                   case RB_OS2_SYS_DRVR:
                       LoadString( ghInst,
                                   IDS_OS2_SYS_DRVR,
                                   (LPSTR)gResult,
                                   sizeof(gResult) );
                       break;

                   case RB_WIN95_SYS_DRVR:
                       LoadString( ghInst,
                                   IDS_WIN95_SYS_DRVR,
                                   (LPSTR)gResult,
                                   sizeof(gResult) );
                       break;

                   case RB_DOSRM_LOCAL_DRVR:
                       LoadString( ghInst,
                                   IDS_DOSRM_LOCAL_DRVR,
                                   (LPSTR)gResult,
                                   sizeof(gResult) );
                       break;

                   default:
                       LoadString( ghInst,
                                   IDS_DRVR_UNDEFINED,
                                   (LPSTR)gResult,
                                   sizeof(gResult) );
                       break;
               }
               wsprintf( gString, "%2d ", osDrvrType );
               lstrcat( gString, gResult );
               SetDlgItemText( hWndDlg, SP_VER_OSTYPE, (LPSTR)gString );
           }
           break;

       case WM_CLOSE:
           /* Closing the Dialog behaves the same as Cancel               */
           PostMessage(hWndDlg, (MESSAGE)WM_COMMAND, IDCANCEL, 0L);
           break;

       case WM_COMMAND:
           switch( GET_LOWORD(wParam) )
           {
               case IDOK:  /* Activate button      */
                   PostMessage(hWndDlg, (MESSAGE)WM_COMMAND, IDCANCEL, 0L);
                   break;

               case IDCANCEL:
                   EndDialog( hWndDlg, FALSE );
                   break;

               default:
                   return( FALSE );
           }
           break;

       default:
           return( FALSE );
   }
   return( TRUE );
} /* End of VERSIONMsgProc */

/***************************************************************************
* Function : ACTIVEMsgProc
*
* Purpose  : To handle the Activate Password dialog box.
*
* Inputs   : hWndDlg - handle to a dialog.
*            Message - is the message to process.
*            wParam  - is a WPARAM size parameter, if any.
*            lParam  - is a LPARAM size parameter, if any.
*
* Outputs  : none.
*
* Returns  : BOOL    - TRUE  if we process the message
*                      FALSE if not.
****************************************************************************/
#ifdef __BORLANDC__
#pragma argsused
#endif
EXPORT_PROC
BOOL WINAPI ACTIVEMsgProc( HWND    hWndDlg,
                           MESSAGE Message,
                           WPARAM  wParam,
                           LPARAM  lParam )
{
   static RB_WORD   activatePW1;
   static RB_WORD   activatePW2;
   static RB_WORD   activateCell;
          RB_WORD   reg;
          SP_STATUS status;
          RB_WORD   fullStatus;
          HCURSOR   hSaveCursor;

   /* to avoid warnings */
   #ifndef __BORLANDC__
   lParam = lParam;
   #endif

   switch( Message )
   {
       case WM_INITDIALOG:
           activatePW1 = gActivatePW1;
           activatePW2 = gActivatePW2;
           activateCell= gActivateCell;
           CenterWindow( hWndDlg, 0 );
           /* initialize working variables                                 */

           /* Initialize the Cell Address Combo Box                        */
           for ( reg = 0; reg <= 0x003f; reg++ )
           {
               /* convert to ascii, base 16    */
               wsprintf( gString, "%02X", reg );
               SendDlgItemMessage( hWndDlg,
                                   ACT_ID,
                                   (MESSAGE)CB_ADDSTRING,
                                   (WPARAM)0,
                                   (LPARAM)(LPSTR)gString );
           }

           if ( activateCell )
           {
               wsprintf( gString, "%X", activateCell );
               SetDlgItemText( hWndDlg, ACT_ID, (LPSTR)gString );
           }

           if ( activatePW1 )
           {
               wsprintf( gString, "%X", activatePW1 );
               SetDlgItemText( hWndDlg, ACT_AP1, (LPSTR)gString );
           }

           if ( activatePW2 )
           {
               wsprintf( gString, "%X", activatePW2 );
               SetDlgItemText( hWndDlg, ACT_AP2, (LPSTR)gString );
           }

           if ( gWritePW )
           {
               wsprintf( gString, "%X", gWritePW );
               SetDlgItemText( hWndDlg, ACT_WP, (LPSTR)gString );
           }
           break;

       case WM_CLOSE:
           /* Closing the Dialog behaves the same as Cancel               */
           PostMessage(hWndDlg, (MESSAGE)WM_COMMAND, IDCANCEL, 0L);
           break;

       case WM_COMMAND:
           switch( GET_LOWORD(wParam) )
           {
               case IDOK:  /* Activate button      */
                   ToggleButton( hWndDlg, IDOK );
                   if ( !GetCellAddress( hWndDlg, ACT_ID, &activateCell ) )
                       return( FALSE );

                   if ( !GetWritePassword( hWndDlg, ACT_WP, &gWritePW ) )
                       return( FALSE );

                   if ( !GetActivate1Password( hWndDlg, ACT_AP1, &activatePW1 ) )
                       return( FALSE );

                   if ( !GetActivate2Password( hWndDlg, ACT_AP2, &activatePW2 ) )
                       return( FALSE );

                   hSaveCursor = SetCursor( ghHourGlassCursor );

  /************************************************************************/
  /* RNBOsproActivate: Activates an inactive algorithm.                   */
  /************************************************************************/

                   status = RNBOsproActivate( ApiPacket,
                                               gWritePW,
                                               activatePW1,
                                               activatePW2,
                                               activateCell );

                   SetCursor( hSaveCursor );

                   if ( status != SP_SUCCESS )
                   {
                       fullStatus = RNBOsproGetFullStatus( ApiPacket );

                       FormatErrStr( (LPSTR)gMsg,
                                     IDS_ERR_ACTIVATE_FAILED,
                                     status,
                                     fullStatus );
                       ErrorMsg( NULL, (LPSTR)gMsg );
                   }
                   else
                   {
                       gActivatePW1 = activatePW1;
                       gActivatePW2 = activatePW2;
                       gActivateCell= activateCell;
                       LoadString( ghInst,
                                   IDS_ACTIVATE_SUCCESS,
                                   (LPSTR)gString,
                                   sizeof(gString) );
                       LoadString( ghInst,
                                   IDS_ACTIVATE_ALGO,
                                   (LPSTR)gMsg,
                                   sizeof(gMsg) );
                       MessageBox( NULL,
                                   (LPSTR)gString,
                                   (LPSTR)gMsg,
                                   MB_ICONINFORMATION | MB_TASKMODAL );
                   }
                   EndDialog( hWndDlg, TRUE );

               case IDCANCEL:
                   ToggleButton( hWndDlg, IDCANCEL );
                   EndDialog( hWndDlg, FALSE );
                   break;

               default:
                   return( FALSE );
           }
           break;

       default:
           return( FALSE );
   }
   return( TRUE );
} /* End of ACTIVEMsgProc */

/***************************************************************************
* Function : DECREMsgProc
*
* Purpose  : To handle the Decrement dialog box.
*
* Inputs   : hWndDlg - handle to a dialog.
*            Message - is the message to process.
*            wParam  - is a WPARAM size parameter, if any.
*            lParam  - is a LPARAM size parameter, if any.
*
* Outputs  : none.
*
* Returns  : BOOL    - TRUE  if we process the message
*                      FALSE if not.
****************************************************************************/
#ifdef __BORLANDC__
#pragma argsused
#endif
EXPORT_PROC
BOOL WINAPI DECREMsgProc( HWND    hWndDlg,
                          MESSAGE Message,
                          WPARAM  wParam,
                          LPARAM  lParam )
{
   static RB_WORD   decCell;
          SP_STATUS status;
          RB_WORD   fullStatus;
          RB_WORD   reg;
          HCURSOR   hSaveCursor;

   /* to avoid warnings */
   #ifndef __BORLANDC__
   lParam = lParam;
   #endif

   switch( Message )
   {
       case WM_INITDIALOG:
           decCell = gDecCell;

           CenterWindow( hWndDlg, 0 );
           /* initialize working variables                                 */

           /* Initialize the DEC_ID control                                */
           for ( reg = 0; reg <= 0x003f; reg++ )
           {
               /* convert to ascii, base 16    */
               wsprintf( gString, "%02X", reg );
               SendDlgItemMessage( hWndDlg,
                                   DEC_ID,
                                   (MESSAGE)CB_ADDSTRING,
                                   (WPARAM)NULL,
                                   (LPARAM)(LPSTR)gString );
           }

           /* Set the item in DEC_ID if previously defined */
           if ( decCell )
           {
               wsprintf( gString, "%X", decCell );
               SetDlgItemText( hWndDlg, DEC_ID, (LPSTR)gString );
           }

           /* Initialize the Write Password control if previously defined */
           if ( gWritePW )
           {
               wsprintf( gString, "%X", gWritePW );
               SetDlgItemText( hWndDlg, DEC_WP, (LPSTR)gString );
           }
           break;

       case WM_CLOSE:
           /* Closing the Dialog behaves the same as Cancel               */
           PostMessage(hWndDlg, (MESSAGE)WM_COMMAND, IDCANCEL, 0L);
           break;

       case WM_COMMAND:
           switch( GET_LOWORD(wParam) )
           {
               case IDOK:  /* Decrement button */
                   ToggleButton( hWndDlg, IDOK );
                   if ( !GetCellAddress( hWndDlg, DEC_ID, &decCell ) )
                       return( FALSE );

                   if ( !GetWritePassword( hWndDlg, DEC_WP, &gWritePW ) )
                       return( FALSE );

                   hSaveCursor = SetCursor( ghHourGlassCursor );

  /************************************************************************/
  /* RNBOsproDecrement: Decrement a counter word                          */
  /************************************************************************/

                   status = RNBOsproDecrement( ApiPacket,
                                               gWritePW,
                                               decCell );

                   SetCursor( hSaveCursor );
                   if ( status != SP_SUCCESS )
                   {
                       fullStatus = RNBOsproGetFullStatus( ApiPacket );

                       FormatErrStr( (LPSTR)gMsg,
                                     IDS_ERR_DEC_FAILED,
                                     status,
                                     fullStatus );

                       ErrorMsg( NULL, (LPSTR)gMsg );
                   }
                   else
                   {
                       gDecCell = decCell;
                       LoadString( ghInst,
                                   IDS_DEC_SUCCESS,
                                   (LPSTR)gString,
                                   sizeof(gString) );
                       LoadString( ghInst,
                                   IDS_DECREMENT,
                                   (LPSTR)gMsg,
                                   sizeof(gMsg) );
                       MessageBox( NULL,
                                   (LPSTR)gString,
                                   (LPSTR)gMsg,
                                   MB_ICONINFORMATION | MB_TASKMODAL );            
                   }
                   return( FALSE );

               case IDCANCEL:
                   ToggleButton( hWndDlg, IDCANCEL );
                   EndDialog( hWndDlg, FALSE );
                   break;

               default:
                   return( FALSE );
           }
           break;

       default:
           return( FALSE );
   }
   return( TRUE );
} /* end DECREMsgProc */

/***************************************************************************
* Function : FINDMsgProc
*
* Purpose  : To handle the Find First dialog box.
*
* Inputs   : hWndDlg - handle to a dialog.
*            Message - is the message to process.
*            wParam  - is a WPARAM size parameter, if any.
*            lParam  - is a LPARAM size parameter, if any.
*
* Outputs  : none.
*
* Returns  : BOOL    - TRUE  if we process the message
*                      FALSE if not.
****************************************************************************/
#ifdef __BORLANDC__
#pragma argsused
#endif
EXPORT_PROC
BOOL WINAPI FINDMsgProc( HWND    hWndDlg,
                         MESSAGE Message,
                         WPARAM  wParam,
                         LPARAM  lParam )
{
   SP_STATUS status;
   RB_WORD   fullStatus;
   RB_WORD   devID;
   HCURSOR   hSaveCursor;

   /* to avoid warnings */
   #ifndef __BORLANDC__
   lParam = lParam;
   #endif

   switch( Message )
   {
       case WM_INITDIALOG:
           CenterWindow( hWndDlg, 0 );
           /* initialize working variables                                */
           if ( gDevID )
           {
               wsprintf( gString, "%4X", gDevID );
               SetDlgItemText( hWndDlg, FF_ID, (LPSTR)gString );
           }
           break;

       case WM_CLOSE:
           /* Closing the Dialog behaves the same as Cancel               */
           PostMessage( hWndDlg, (MESSAGE)WM_COMMAND, IDCANCEL, 0L );
           break;

       case WM_COMMAND:
           switch( GET_LOWORD(wParam) )
           {
               case IDOK:
                   ToggleButton( hWndDlg, IDOK );
                   switch( GetDlgItemWord( hWndDlg, FF_ID, &devID ) )
                   {
                       case 1:
                           LoadString( ghInst,
                                       IDS_ERR_NO_DEV_ID,
                                       (LPSTR)gString,
                                       sizeof(gString) );
                           ErrorMsg( NULL, (LPSTR)gString );
                           return( FALSE );

                       case 2:
                           LoadString( ghInst,
                                       IDS_ERR_INVALID_INPUT,
                                       (LPSTR)gString,
                                       sizeof(gString) );
                           ErrorMsg( NULL, gString );
                           return( FALSE );
                   }

                   if ( gDevID )
                       if ( gDevID != devID ) ResetPW();

                   gDevID = devID;

                   hSaveCursor = SetCursor( ghHourGlassCursor );

  /************************************************************************/
  /* RNBOsproFindFirstUnit: Searches for the first occurance of a         */
  /*                        SentinelSuperPro by the developer ID          */
  /************************************************************************/

                   status = RNBOsproFindFirstUnit( ApiPacket,
                                                   gDevID );

                   SetCursor( hSaveCursor );

                   if ( status != SP_SUCCESS )
                   {
                       fullStatus = RNBOsproGetFullStatus( ApiPacket );
                       FormatErrStr( (LPSTR)gMsg,
                                     IDS_ERR_FF_FAILED,
                                     status,
                                     fullStatus );
                       ErrorMsg( NULL, (LPSTR)gMsg );
                       return( FALSE );
                   }
                   else
                   {
                      LoadString( ghInst,
                                  IDS_FF_SUCCESS,
                                  (LPSTR)gString,
                                  sizeof(gString) );
                      LoadString( ghInst,
                                  IDS_FINDFIRST,
                                  (LPSTR)gMsg,
                                  sizeof(gMsg) );
                       MessageBox( NULL,
                                   (LPSTR)gString,
                                   (LPSTR)gMsg,
                                   MB_ICONINFORMATION | MB_TASKMODAL );
                       EndDialog( hWndDlg, TRUE );
                   }
                   break;

               case IDCANCEL:
                   ToggleButton( hWndDlg, IDCANCEL );
                   EndDialog( hWndDlg, FALSE );
                   break;

               default:
                   return( FALSE );
           }
           break;

       default:
           return( FALSE );
   }
   return( TRUE );
} /* end FINDMsgProc */

/***************************************************************************
* Function : OWRITEMsgProc
*
* Purpose  : To handle the Overwrite dialog box.
*
* Inputs   : hWndDlg - handle to a dialog.
*            Message - is the message to process.
*            wParam  - is a WPARAM size parameter, if any.
*            lParam  - is a LPARAM size parameter, if any.
*
* Outputs  : none.
*
* Returns  : BOOL    - TRUE  if we process the message
*                      FALSE if not.
****************************************************************************/
#ifdef __BORLANDC__
#pragma argsused
#endif
EXPORT_PROC
BOOL WINAPI OWRITEMsgProc( HWND    hWndDlg,
                           MESSAGE Message,
                           WPARAM  wParam,
                           LPARAM  lParam )
{
   static RB_WORD   owCell;
   static RB_WORD   owValue;
   static RB_BYTE   owAccessCode;
          SP_STATUS status;
          RB_WORD   fullStatus;
          RB_WORD   i;
          HCURSOR   hSaveCursor;

   /* to avoid warnings */
   #ifndef __BORLANDC__
   lParam = lParam;
   #endif

   switch( Message )
   {
       case WM_INITDIALOG:
           owCell       = gOwCell;
           owValue      = gOwValue;
           owAccessCode = gOwAccessCode;
           CenterWindow(hWndDlg, 0);

           if ( gOverwritePW1 )
           {
               wsprintf( gString, "%X", gOverwritePW1 );
               SetDlgItemText( hWndDlg, OW_PW_1, (LPSTR)gString );
           }

           if ( gOverwritePW2 )
           {
               wsprintf( gString, "%X", gOverwritePW2 );
               SetDlgItemText( hWndDlg, OW_PW_2, (LPSTR)gString );
           }

           if ( gWritePW )
           {
               wsprintf( gString, "%X", gWritePW );
               SetDlgItemText( hWndDlg, OW_PW_3, (LPSTR)gString );
           }

           /* Initialize the OW_ID control  */
           for ( i = 0; i <= 0x003f; i++ )
           {
               /* convert to ascii, base 16    */
               wsprintf( gString, "%02X", i );
               SendDlgItemMessage( hWndDlg,
                                   OW_ID,
                                   (MESSAGE)CB_ADDSTRING,
                                   (WPARAM)NULL,
                                   (LPARAM)(LPSTR)gString );
           }

           if ( owCell )
           {
               wsprintf( gString, "%X", owCell );
               SetDlgItemText( hWndDlg, OW_ID, (LPSTR)gString );
           }

           if ( owValue )
           {
               wsprintf( gString, "%X", owValue );
               SetDlgItemText( hWndDlg, OW_VAL, (LPSTR)gString );
           }

           if ( owAccessCode != (RB_BYTE)-1 )
           {
               wsprintf( gString, "%1d", owAccessCode );
               SetDlgItemText( hWndDlg, OW_AC, (LPSTR)gString );
           }

           /* Initialize the OW_AC control                                */
           for ( i = 0; i <= 3; i++ )
           {
               wsprintf( gString, "%d", i );
               SendDlgItemMessage( hWndDlg,
                                   OW_AC,
                                   (MESSAGE)CB_ADDSTRING,
                                   (WPARAM)NULL,
                                   (LPARAM)(LPSTR)gString );
           }
           break;

       case WM_CLOSE:
           PostMessage(hWndDlg, (MESSAGE)WM_COMMAND, IDCANCEL, 0L);
           break;

       case WM_COMMAND:
           switch( GET_LOWORD(wParam) )
           {
               case IDOK:
                   ToggleButton( hWndDlg, IDOK );
                   if ( !GetCellAddress( hWndDlg, OW_ID, &owCell ) )
                       return( FALSE );

                   if ( !GetAccessCode( hWndDlg, OW_AC, &owAccessCode ) )
                       return( FALSE );

                   if ( !GetDataValue( hWndDlg, OW_VAL, &owValue ) )
                      return( FALSE );

                   if ( !GetOverwrite1Password( hWndDlg, OW_PW_1, &gOverwritePW1 ) )
                       return( FALSE );

      
                   if ( !GetOverwrite2Password( hWndDlg, OW_PW_2, &gOverwritePW2 ) )
                       return( FALSE );

                   if ( !GetWritePassword( hWndDlg, OW_PW_3, &gWritePW ) )
                       return( FALSE );

                   hSaveCursor = SetCursor( ghHourGlassCursor );

  /************************************************************************/
  /* RNBOsproOverwrite: Overwrite a cell who's access code is greater than*/
  /*                    0 (read/write data).                              */
  /************************************************************************/

                   status = RNBOsproOverwrite( ApiPacket,
                                               gWritePW,
                                               gOverwritePW1,
                                               gOverwritePW2,
                                               owCell,
                                               owValue,
                                               owAccessCode );

                   SetCursor( hSaveCursor );

                   if ( status != SP_SUCCESS )
                   {
                       fullStatus = RNBOsproGetFullStatus( ApiPacket );
                       FormatErrStr( (LPSTR)gMsg,
                                     IDS_ERR_OVERWRITE_FAILED,
                                     status,
                                     fullStatus );
                       ErrorMsg( NULL, (LPSTR)gMsg );
                       return( FALSE );
                   }
                   else
                   {
                       gOwValue      = owValue;
                       gOwCell       = owCell;
                       gOwAccessCode = owAccessCode;

                       LoadString( ghInst,
                                   IDS_OVERWRITE_SUCCESS,
                                   (LPSTR)gString,
                                   sizeof(gString) );
                       LoadString( ghInst,
                                   IDS_OVERWRITE,
                                   (LPSTR)gMsg,
                                   sizeof(gMsg) );
                       MessageBox( hWndDlg,
                                   (LPSTR)gString,
                                   (LPSTR)gMsg,
                                   MB_ICONINFORMATION | MB_TASKMODAL );
                       EndDialog( hWndDlg, TRUE );
                   }
                   break;

               case IDCANCEL:
                   ToggleButton( hWndDlg, IDCANCEL );
                   EndDialog( hWndDlg, FALSE );
                   break;

               default:
                   return( FALSE );
           }
           break;

       default:
           return( FALSE );
   }
   return( TRUE );
} /* end OWRITEMsgProc */

/***************************************************************************
* Procedure : UpdateQueryDlg
*
* Purpose   : To update the controls within the Query Dialog box.
*
* Inputs    : hWndDlg   - handle to a dialog.
*             qCell     - is the query cell address.
*             qData     - is the query data if any.
*             qResult   - is the query result if any.
*             qResult32 - is the 32 bit query result if any.
*
* Outputs  : none.
*
* Returns  : none.
****************************************************************************/
LOCAL_PROC
void UpdateQueryDlg( HWND     hWndDlg,
                     RB_WORD  qCell,
                     RBP_BYTE qData,
                     RBP_BYTE qResult,
                     RB_DWORD qResult32,
                     RB_WORD  qrySize )
{
   register RB_WORD i;
            RB_WORD k;
            PSTR    cp;

   if ( qCell == (RB_WORD)-1 ) return;

   wsprintf( gString, "%02X", qCell );
   SetDlgItemText( hWndDlg, QRY_ID, (LPSTR)gString );

   for ( i = 0, cp = gString; i < qrySize; i++, cp += 2 )
   {
       k = qData[i];
       wsprintf( cp, "%02X", k );
   }
   gString[(qrySize<<1)+1] = '\0';
   SetDlgItemText( hWndDlg, QRY_QS, (LPSTR)gString );

   for ( i = 0, cp = gString; i < qrySize; i++, cp += 2 )
       wsprintf( cp, "%02X", qResult[i] );
   gString[(qrySize<<2)+1] = '\0';
   SetDlgItemText( hWndDlg, QRY_RSLT, (LPSTR)gString );

   wsprintf( gString, "%08X", qResult32 );
   SetDlgItemText( hWndDlg, QRY_RSLT_32, (LPSTR)gString );
} /* end UpdateQueryDlg */

/***************************************************************************
* Function : QUERYMsgProc
*
* Purpose  : To handle the Query dialog box.
*
* Inputs   : hWndDlg - handle to a dialog.
*            Message - is the message to process.
*            wParam  - is a WPARAM size parameter, if any.
*            lParam  - is a LPARAM size parameter, if any.
*
* Outputs  : none.
*
* Returns  : BOOL    - TRUE  if we process the message
*                      FALSE if not.
****************************************************************************/
#ifdef __BORLANDC__
#pragma argsused
#endif
EXPORT_PROC
BOOL WINAPI QUERYMsgProc( HWND hWndDlg,
                          MESSAGE Message,
                          WPARAM  wParam,
                          LPARAM  lParam )
{
  register  RB_WORD   i;
   static   RB_WORD   qCell;
   static   RB_WORD   qrySize;
   static   RB_BYTE   qData[ SPRO_MAX_QUERY_SIZE   + 1 ];
   static   RB_BYTE   qResult[ SPRO_MAX_QUERY_SIZE + 1 ];
   static   RB_DWORD  qResult32;
            HCURSOR   hSaveCursor;
            SP_STATUS status;
            RB_WORD   fullStatus;

   /* to avoid warnings */
   #ifndef __BORLANDC__
   lParam = lParam;
   #endif

   switch( Message )
   {
       case WM_INITDIALOG:
           qCell     = gQCell;
           qrySize   = gQrySize;
           memcpy( qData, gQData, qrySize );
           memcpy( qResult, gQResult, qrySize );
           qResult32 = gQResult32;

           CenterWindow( hWndDlg, 0 );
           /* Initialize the QRY_ID control */
           for ( i = 0; i <= 0x003f; i++ )
           {
               /* convert to ascii, base 16    */
               wsprintf( gString, "%02X", i );
               SendDlgItemMessage( hWndDlg,
                                   QRY_ID,
                                   (MESSAGE)CB_ADDSTRING,
                                   (WPARAM)NULL,
                                   (LPARAM)(LPSTR)gString );
           }
           UpdateQueryDlg( hWndDlg,
                           qCell,
                           qData,
                           qResult,
                           qResult32,
                           qrySize );
           break;

       case WM_CLOSE:
           PostMessage(hWndDlg, (MESSAGE)WM_COMMAND, IDCANCEL, 0L);
           break;

       case WM_COMMAND:
           switch( GET_LOWORD(wParam) )
           {
               case IDOK:
                   ToggleButton( hWndDlg, IDOK );
                   if ( !GetCellAddress( hWndDlg, QRY_ID, &qCell ) )
                       return( FALSE );

                   qrySize = (WORD)GetDlgItemText( hWndDlg,
                                             QRY_QS,
                                             (LPSTR)qData,
                                             SPRO_MAX_QUERY_SIZE );
                   switch( ValidateQryStr( (LPSTR)qData, qrySize ) )
                   {
                       case 0:
                           qrySize = (WORD)ConvertBufferToHex( (PSTR)qData, qrySize );
                           break;

                       case 1:
                           LoadString( ghInst,
                                       IDS_ERR_QUERY1,
                                       (LPSTR)gString,
                                       sizeof(gString) );
                           ErrorMsg( NULL, (LPSTR)gString );
                           qrySize = 0;
                           return( FALSE );

                       case 2:
                           LoadString( ghInst,
                                       IDS_ERR_QUERY2,
                                       (LPSTR)gString,
                                       sizeof(gString) );
                           ErrorMsg( NULL, (LPSTR)gString );
                           qrySize = 0;
                           return( FALSE );

                       case 3:
                           LoadString( ghInst,
                                       IDS_ERR_QUERY3,
                                       (LPSTR)gString,
                                       sizeof(gString) );
                           ErrorMsg( NULL, (LPSTR)gString );
                           qrySize = 0;
                           return( FALSE );
                   }

                   hSaveCursor = SetCursor( ghHourGlassCursor );

  /************************************************************************/
  /* RNBOsproQuery: Queries a active algorithm.                           */
  /************************************************************************/

                   status = RNBOsproQuery( ApiPacket,
                                           qCell,
                                           (RBP_VOID)qData,
                                           (RBP_VOID)qResult,
                                           &qResult32,
                                           qrySize );

                   SetCursor( hSaveCursor );

                   if ( status != SP_SUCCESS )
                   {
                       fullStatus = RNBOsproGetFullStatus( ApiPacket );
                       FormatErrStr( (LPSTR)gMsg,
                                     IDS_ERR_QUERY_FAILED,
                                     status,
                                     fullStatus );
                       ErrorMsg( NULL, (LPSTR)gMsg );
                   }
                   else
                   {

                        gQCell     = qCell;
                        gQrySize   = qrySize;
                        memcpy( gQData, qData, qrySize );
                        memcpy( gQResult, qResult, qrySize );
                        gQResult32 = qResult32;

                        UpdateQueryDlg( hWndDlg,
                                        qCell,
                                        qData,
                                        qResult,
                                        qResult32,
                                        qrySize );
                   }
                   return( FALSE );

               case IDCANCEL:
                   ToggleButton( hWndDlg, IDCANCEL );
                   EndDialog( hWndDlg, FALSE );
                   break;

               default:
                   return( FALSE );
           }
           break;

       default:
           return( FALSE );
   }
   return( TRUE );
} /* end QUERYMsgProc */

/***************************************************************************
* Function : READMsgProc
*
* Purpose  : To handle the Read dialog box.
*
* Inputs   : hWndDlg - handle to a dialog.
*            Message - is the message to process.
*            wParam  - is a WPARAM size parameter, if any.
*            lParam  - is a LPARAM size parameter, if any.
*
* Outputs  : none.
*
* Returns  : BOOL    - TRUE  if we process the message
*                      FALSE if not.
****************************************************************************/
#ifdef __BORLANDC__
#pragma argsused
#endif
EXPORT_PROC
BOOL WINAPI READMsgProc( HWND    hWndDlg,
                         MESSAGE Message,
                         WPARAM  wParam,
                         LPARAM  lParam )
{
   static RB_WORD   rCell;
   static RB_WORD   rData;
          SP_STATUS status;
          RB_WORD   fullStatus;
          RB_WORD   i;
          HCURSOR   hSaveCursor;

   /* to avoid warnings */
   #ifndef __BORLANDC__
   lParam = lParam;
   #endif

   switch( Message )
   {
       case WM_INITDIALOG:
           rCell = gRCell;
           rData = gRData;
           CenterWindow(hWndDlg, 0);
           /* Initialize the RD_ID control */
           for ( i = 0; i <= 0x003f; i++ )
           {
               /* convert to ascii, base 16    */
               wsprintf( gString, "%02X", i );
               SendDlgItemMessage( hWndDlg,
                                   RD_ID,
                                   (MESSAGE)CB_ADDSTRING,
                                   (WPARAM)NULL,
                                   (LPARAM)(LPSTR)gString );
           }

           if ( rCell != (RB_WORD)-1 )
           {
               wsprintf( gString, "%X", rCell );
               SetDlgItemText( hWndDlg, RD_ID, (LPSTR)gString );
           }

           if ( rData != (RB_WORD)-1 )
           {
               wsprintf( gString, "%X", rData );
               SetDlgItemText( hWndDlg, RD_VAL, (LPSTR)gString );
           }

           break;

       case WM_CLOSE:
           PostMessage(hWndDlg, (MESSAGE)WM_COMMAND, IDCANCEL, 0L);
           break;

       case WM_COMMAND:
           switch( GET_LOWORD(wParam) )
           {
               case IDOK:
                   ToggleButton( hWndDlg, IDOK );
                   if ( !GetCellAddress( hWndDlg, RD_ID, &rCell ) )
                       return( FALSE );

                   hSaveCursor = SetCursor( ghHourGlassCursor );

  /************************************************************************/
  /* RNBOsproRead: Returns the data content of a cell                     */
  /************************************************************************/

                   status = RNBOsproRead( ApiPacket,
                                           rCell,
                                           &rData );

                   SetCursor( hSaveCursor );

                   if ( status != SP_SUCCESS )
                   {
                       fullStatus = RNBOsproGetFullStatus( ApiPacket );
                       FormatErrStr( (LPSTR)gMsg,
                                     IDS_ERR_READ_FAILED,
                                     status,
                                     fullStatus );
                       ErrorMsg( NULL, (LPSTR)gMsg );
                       return( FALSE );
                   }
                   else
                   {
                       gRCell = rCell;
                       gRData = rData;
                       /* Update RD_VAL field */
                       wsprintf( gString, "%X", rData );
                       SetDlgItemText( hWndDlg,
                                       RD_VAL,
                                       (LPCSTR)gString );
                   }
                   return( FALSE );

               case IDCANCEL:
                   ToggleButton( hWndDlg, IDCANCEL );
                   EndDialog(hWndDlg, FALSE);
                   break;

               default:
                   return( FALSE );
           }
           break;

       default:
           return( FALSE );
   }
   return( TRUE );
} /* end READMsgProc */

/***************************************************************************
* Function : WRITEMsgProc
*
* Purpose  : To handle the Write dialog box.
*
* Inputs   : hWndDlg - handle to a dialog.
*            Message - is the message to process.
*            wParam  - is a WPARAM size parameter, if any.
*            lParam  - is a LPARAM size parameter, if any.
*
* Outputs  : none.
*
* Returns  : BOOL    - TRUE  if we process the message
*                      FALSE if not.
****************************************************************************/
#ifdef __BORLANDC__
#pragma argsused
#endif
EXPORT_PROC
BOOL WINAPI WRITEMsgProc( HWND    hWndDlg,
                          MESSAGE Message,
                          WPARAM  wParam,
                          LPARAM  lParam )
{
   static RB_WORD   wCell;      
   static RB_WORD   wData;      
   static RB_BYTE   wAccessCode;
          SP_STATUS status;
          RB_WORD   fullStatus;
          RB_WORD   i;
          HCURSOR   hSaveCursor;

   /* to avoid warnings */
   #ifndef __BORLANDC__
   lParam = lParam;
   #endif

   switch( Message )
   {
       case WM_INITDIALOG:
           wCell = gWCell;
           wData = gWData;
           wAccessCode = gWAccessCode;
           CenterWindow(hWndDlg, 0);
           /* Initialize the WR_ID control */
           for ( i = 0; i <= 0x003f; i++ )
           {
               /* convert to ascii, base 16    */
               wsprintf( gString, "%02X", i );
               SendDlgItemMessage( hWndDlg,
                                   WR_ID,
                                   (MESSAGE)CB_ADDSTRING,
                                   (WPARAM)NULL,
                                   (LPARAM)(LPSTR)gString );
           }

           /* Initialize the WR_AC control */
           for ( i = 0; i <= 3 ; i++ )
           {
               /* convert to ascii, base 16    */
               wsprintf( gString, "%d", i );
               SendDlgItemMessage( hWndDlg,
                                   WR_AC,
                                   (MESSAGE)CB_ADDSTRING,
                                   (WPARAM)NULL,
                                   (LPARAM)(LPSTR)gString );
           }

           if ( wAccessCode != (RB_BYTE)-1 )
           {
               wsprintf( gString, "%1d", wAccessCode );
               SetDlgItemText( hWndDlg, WR_AC, (LPSTR)gString );
           }

           if ( wCell != (RB_WORD)-1 )
           {
               wsprintf( gString, "%X", wCell );
               SetDlgItemText( hWndDlg, WR_ID, (LPSTR)gString );
           }

           if ( wData != (RB_WORD)-1 )
           {
               wsprintf( gString, "%X", wData );
               SetDlgItemText( hWndDlg, WR_VAL, (LPSTR)gString );
           }

           if ( gWritePW )
           {
               wsprintf( gString, "%X", gWritePW );
               SetDlgItemText( hWndDlg, WR_PW, (LPSTR)gString );
           }
           break;

       case WM_CLOSE:
           PostMessage(hWndDlg, (MESSAGE)WM_COMMAND, IDCANCEL, 0L);
           break;

       case WM_COMMAND:
           switch( GET_LOWORD(wParam) )
           {
               case IDOK:
                   ToggleButton( hWndDlg, IDOK );
                   if ( !GetCellAddress( hWndDlg, WR_ID, &wCell ) )
                       return( FALSE );

                   if ( !GetAccessCode( hWndDlg, WR_AC, &wAccessCode ) )
                       return( FALSE );

                   if ( !GetWritePassword( hWndDlg, WR_PW, &gWritePW ) )
                       return( FALSE );

                   if ( !GetDataValue( hWndDlg, WR_VAL, &wData ) )
                       return( FALSE );

                   hSaveCursor = SetCursor( ghHourGlassCursor );

  /************************************************************************/
  /* RNBOsproWrite: Writes data and sets the access code to a cell who's  */
  /*                current access code is 0 (read/write data).           */
  /************************************************************************/

                   status = RNBOsproWrite( ApiPacket,
                                            gWritePW,
                                            wCell,
                                            wData,
                                            wAccessCode );

                    SetCursor( hSaveCursor );
                    if ( status != SP_SUCCESS )
                    {
                       fullStatus = RNBOsproGetFullStatus( ApiPacket );
                       FormatErrStr( (LPSTR)gMsg,
                                     IDS_ERR_WRITE_FAILED,
                                     status,
                                     fullStatus );
                       ErrorMsg( NULL, (LPSTR)gMsg );
                        return( FALSE );
                    }
                    else
                    {
                        gWCell       = wCell;
                        gWAccessCode = wAccessCode;
                        gWData       = wData;
                        LoadString( ghInst,
                                    IDS_WRITE_SUCCESS,
                                    (LPSTR)gString,
                                    sizeof(gString) );
                        LoadString( ghInst,
                                    IDS_WRITE,
                                    (LPSTR)gMsg,
                                    sizeof(gMsg) );
                        MessageBox( hWndDlg,
                                    (LPSTR)gString,
                                    (LPSTR)gMsg,
                                    MB_ICONINFORMATION | MB_TASKMODAL );
                    }
                    EndDialog( hWndDlg, TRUE );
                    break;

               case IDCANCEL:
                   ToggleButton( hWndDlg, IDCANCEL );
                   EndDialog(hWndDlg, FALSE);
                   break;

               default:
                   return( FALSE );
           }
           break;

       default:
           return( FALSE );
   }
   return( TRUE );
} /* end WRITEMsgProc */

/***************************************************************************
* Function : XREADMsgProc
*
* Purpose  : To handle the Extended Read dialog box.
*
* Inputs   : hWndDlg - handle to a dialog.
*            Message - is the message to process.
*            wParam  - is a WPARAM size parameter, if any.
*            lParam  - is a LPARAM size parameter, if any.
*
* Outputs  : none.
*
* Returns  : BOOL    - TRUE  if we process the message
*                      FALSE if not.
****************************************************************************/
#ifdef __BORLANDC__
#pragma argsused
#endif
EXPORT_PROC
BOOL WINAPI XREADMsgProc( HWND    hWndDlg,
                          MESSAGE Message,
                          WPARAM  wParam,
                          LPARAM  lParam )
{
   static RB_WORD   rCell;
   static RB_WORD   rData;
   static RB_BYTE   rAccessCode;
          SP_STATUS status;
          RB_WORD   fullStatus;
          RB_WORD   i;
          HCURSOR   hSaveCursor;

   /* to avoid warnings */
   #ifndef __BORLANDC__
   lParam = lParam;
   #endif

   switch( Message )
   {
       case WM_INITDIALOG:
           rCell = gXRCell;
           rData = gXRData;
           rAccessCode = gXRAccessCode;

           CenterWindow(hWndDlg, 0);
           /* Initialize the XR_ID control */
           for ( i = 0; i <= 0x003f; i++ )
           {
               /* convert to ascii, base 16    */
               wsprintf( gString, "%02X", i );
               SendDlgItemMessage( hWndDlg,
                                   XR_ID,
                                   (MESSAGE)CB_ADDSTRING,
                                   (WPARAM)NULL,
                                   (LPARAM)(LPSTR)gString );
           }

           if ( rCell != (RB_WORD)-1 )
           {   
               wsprintf( gString,"%X", rCell );
               SetDlgItemText( hWndDlg, XR_ID, (LPSTR)gString );
           }

           if ( rData != (RB_WORD)-1 )
           {
               wsprintf( gString, "%X", rData );
               SetDlgItemText( hWndDlg, XR_VAL, (LPSTR)gString );
           }

           if ( rAccessCode != (RB_BYTE)-1 )
           {
               wsprintf( gString, "%d", rAccessCode );
               SetDlgItemText( hWndDlg, XR_AC, (LPSTR)gString );
           }
           break;

       case WM_CLOSE:
           PostMessage(hWndDlg, (MESSAGE)WM_COMMAND, IDCANCEL, 0L);
           break;

       case WM_COMMAND:
           switch( GET_LOWORD(wParam) )
           {
               case IDOK:
                   ToggleButton( hWndDlg, IDOK );
                   if ( !GetCellAddress( hWndDlg, XR_ID, &rCell ) )
                       return( FALSE );

                   hSaveCursor = SetCursor( ghHourGlassCursor );

  /************************************************************************/
  /* RNBOsproExtendedRead: Returns the cell data and access code.         */
  /************************************************************************/

                 status = RNBOsproExtendedRead( ApiPacket,
                                                   rCell,
                                                   &rData,
                                                   &rAccessCode );

                   SetCursor( hSaveCursor );
                   if ( status != SP_SUCCESS )
                   {
                       fullStatus = RNBOsproGetFullStatus( ApiPacket );
                       FormatErrStr( (LPSTR)gMsg,
                                     IDS_ERR_XREAD_FAILED,
                                     status,
                                     fullStatus );
                       ErrorMsg( NULL, (LPSTR)gMsg );
                   }
                   else
                   {
                       gXRCell       = rCell;
                       gXRData       = rData;
                       gXRAccessCode = rAccessCode;

                       /* Update XR_VAL field */
                       wsprintf( gString, "%X", rData );
                       SetDlgItemText( hWndDlg,
                                       XR_VAL,
                                       (LPCSTR)gString );

                       /* Update XR_AC field */
                       wsprintf( gString, "%d", rAccessCode );
                       SetDlgItemText( hWndDlg,
                                       XR_AC,
                                       (LPCSTR)gString );
                   }
                   break;

               case IDCANCEL:
                   ToggleButton( hWndDlg, IDCANCEL );
                   EndDialog( hWndDlg, TRUE );
                   break;

               default:
                   return( FALSE );

           }
           break;

       default:
           return( FALSE );
   }
   return( TRUE );
} /* end XREADMsgProc */

/***************************************************************************
* Function : INFOMsgProc
*
* Purpose  : To handle the Information dialog box.
*
* Inputs   : hWndDlg - handle to a dialog.
*            Message - is the message to process.
*            wParam  - is a WPARAM size parameter, if any.
*            lParam  - is a LPARAM size parameter, if any.
*
* Outputs  : none.
*
* Returns  : BOOL    - TRUE  if we process the message
*                      FALSE if not.
****************************************************************************/
#ifdef __BORLANDC__
#pragma argsused
#endif
EXPORT_PROC
BOOL WINAPI INFOMsgProc( HWND    hWndDlg,
                         MESSAGE Message,
                         WPARAM  wParam,
                         LPARAM  lParam )
{
   /* to avoid warnings */
   #ifndef __BORLANDC__
   lParam = lParam;
   #endif

   switch( Message )
   {
       case WM_INITDIALOG:
           CenterWindow(hWndDlg, 0);
           break;

       case WM_CLOSE:
           PostMessage(hWndDlg, (MESSAGE)WM_COMMAND, IDOK, 0L);
           break;

       case WM_COMMAND:
           switch( GET_LOWORD(wParam) )
           {
               case IDOK:
                   ToggleButton( hWndDlg, IDOK );
                   EndDialog( hWndDlg, TRUE );
                   break;

               default :
                   return( FALSE );

           }
           break;

       default:
           return( FALSE );
   }
   return( TRUE );
} /* end INFOMsgProc */

/***************************************************************************
* Procedure : UpdateViewLB
*
* Purpose   : To handle the update of the view list box.
*
* Inputs    : hWndDlg - handle to a dialog.
*
* Outputs   : none.
*
* Returns   : none.
****************************************************************************/
LOCAL_PROC
void UpdateViewLB( HWND hWndDlg )
{
   register RB_WORD   a;
   register RB_WORD   b;
            RB_WORD   rData = 0;
            RB_BYTE   rAccessCode = 0;
            SP_STATUS status;

   SendDlgItemMessage( hWndDlg,
                       VIEW_TEXT,
                       (MESSAGE)LB_RESETCONTENT,
                       (WPARAM)NULL,
                       (LPARAM)NULL );
   wsprintf( gViewText, " Adr  Data  AC    Adr  Data  AC    Adr  Data  AC    Adr  Data  AC ");
   SendDlgItemMessage( hWndDlg,
                       VIEW_TEXT,
                       (MESSAGE)LB_ADDSTRING,
                       (WPARAM)NULL,
                       (LPARAM)(LPCSTR)gViewText );
   wsprintf( gViewText, " ---  ----  --    ---  ----  --    ---  ----  --    ---  ----  -- " );
   SendDlgItemMessage( hWndDlg,
                       VIEW_TEXT,
                       (MESSAGE)LB_ADDSTRING,
                       (WPARAM)NULL,
                       (LPARAM)(LPCSTR)gViewText );
   for ( a = 0; a < 16; a++ )
   {
       gViewText[0] = '\0';
       for ( b = 0; b < 4; b++ )
       {
           wsprintf( gString, " %02X:  ", a+16*b );
           lstrcat( gViewText, gString );

           status = RNBOsproExtendedRead( ApiPacket,
                                          (RB_WORD)(a+16*b),
                                          &rData,
                                          &rAccessCode );

           if ( status != SP_SUCCESS )
           {
               lstrcpy( gString, "????  " );
               if ( status == SP_ACCESS_DENIED )
                   lstrcat( gString, " 3   " );
                else
                   lstrcat( gString, " ?   " );
               lstrcat( gViewText, gString );
           }
           else
           {
               wsprintf( gString, "%04X  ", rData );
               lstrcat( gViewText, gString );
               wsprintf( gString, "%2d   ",  rAccessCode );
               lstrcat( gViewText, gString );
           }
       }
       SendDlgItemMessage( hWndDlg,
                           VIEW_TEXT,
                           (MESSAGE)LB_ADDSTRING,
                           (WPARAM)NULL,
                           (LPARAM)(LPCSTR)gViewText );
   }
} /* end UpdateViewLB */

/***************************************************************************
* Function : VIEWMsgProc
*
* Purpose  : To handle the View dialog box.
*
* Inputs   : hWndDlg - handle to a dialog.
*            Message - is the message to process.
*            wParam  - is a WPARAM size parameter, if any.
*            lParam  - is a LPARAM size parameter, if any.
*
* Outputs  : none.
*
* Returns  : BOOL    - TRUE  if we process the message
*                      FALSE if not.
****************************************************************************/
#ifdef __BORLANDC__
#pragma argsused
#endif
EXPORT_PROC
BOOL WINAPI VIEWMsgProc( HWND    hWndDlg,
                         MESSAGE Message,
                         WPARAM  wParam,
                         LPARAM  lParam )
{
   HCURSOR hSaveCursor;

   /* to avoid warnings */
   #ifndef __BORLANDC__
   lParam = lParam;
   #endif

   switch( Message )
   {
       case WM_ACTIVATE:
           ghCurrentDlg = ( GET_LOWORD(wParam) ? hWndDlg : NULL );
           break;

       case WM_INITDIALOG:
           CenterWindow(hWndDlg, 0);
           hSaveCursor = SetCursor( ghHourGlassCursor );
           UpdateViewLB( hWndDlg );
           SetCursor( hSaveCursor );
           break;

       case WM_CLOSE:
           PostMessage( hWndDlg,
                       (MESSAGE)WM_COMMAND,
                       (WPARAM)IDCANCEL,
                       (LPARAM)NULL );
           break;

       case WM_COMMAND:
           switch( GET_LOWORD(wParam) )
           {
               case IDOK:
                   ToggleButton( hWndDlg, IDOK );
                   hSaveCursor = SetCursor( ghHourGlassCursor );
                   UpdateViewLB( hWndDlg );
                   SetCursor( hSaveCursor );
                   break;

               case IDCANCEL:
                   ToggleButton( hWndDlg, IDCANCEL );
                   DestroyWindow( hWndDlg );
                   ghViewDlg = NULL;
                   break;
           }
           break;

       default:
           return( FALSE );
   }
   return( TRUE );
} /* end VIEWMsgProc */

/***************************************************************************
* Function : MAINMsgProc
*
* Purpose  : To handle the main dialog box.
*
* Inputs   : hWndDlg - handle to a dialog.
*            Message - is the message to process.
*            wParam  - is a WPARAM size parameter, if any.
*            lParam  - is a LPARAM size parameter, if any.
*
* Outputs  : none.
*
* Returns  : BOOL    - TRUE  if we process the message
*                      FALSE if not.
****************************************************************************/
#ifdef __BORLANDC__
#pragma argsused
#endif
EXPORT_PROC
BOOL WINAPI MAINMsgProc( HWND    hWndDlg,
                         MESSAGE Message,
                         WPARAM  wParam,
                         LPARAM  lParam )
{
   PPROC     lpfnVIEWMsgProc = NULL;
   PPROC     lpfnMsgProc;
   SP_STATUS status;
   RB_WORD   fullStatus;

   /* to avoid warnings */
   #ifndef __BORLANDC__
   lParam = lParam;
   #endif

   switch( Message )
   {
       case WM_ACTIVATE:
           ghCurrentDlg = ( GET_LOWORD(wParam) ? hWndDlg : NULL );
           break;

       case WM_INITDIALOG:
           CenterWindow(hWndDlg, 0);
           break;

       case WM_CLOSE:
           PostMessage( hWndDlg, (MESSAGE)WM_COMMAND, IDCANCEL, (LPARAM)NULL );
           break;

       case WM_COMMAND:
           switch( GET_LOWORD(wParam) )
           {
               case SP_API_FF: /* "Findfirst" */
                   ToggleButton( hWndDlg, SP_API_FF );

                   lpfnMsgProc = MakeProcInstance( (PPROC)FINDMsgProc,
                                                   ghInst );
                   DialogBox( ghInst,
                              FIND_DLG,
                              hWndDlg,
                              lpfnMsgProc );
                   xFreeProcInstance( lpfnMsgProc );
                   break;

               case SP_API_FN: /* "Findnext" */
                   ToggleButton( hWndDlg, SP_API_FN );

  /************************************************************************/
  /* RNBOsproFindNextUnit: Finds the next occurance of a SentinelSuperPro */
  /*                       described in the RB_SPRO_APIPACKET.            */
  /************************************************************************/

                   status = RNBOsproFindNextUnit( ApiPacket );

                   if ( status != SP_SUCCESS )
                   {
                       fullStatus = RNBOsproGetFullStatus( ApiPacket );
                       FormatErrStr( (LPSTR)gMsg,
                                     IDS_ERR_FINDNEXT_FAILED,
                                     status,
                                     fullStatus );
                       ErrorMsg( hWndDlg, (LPSTR)gMsg );
                   }
                   else
                   {
                       LoadString( ghInst,
                                   IDS_FF_SUCCESS,
                                   (LPSTR)gString,
                                   sizeof(gString) );
                       LoadString( ghInst,
                                   IDS_FINDNEXT,
                                   (LPSTR)gMsg,
                                   sizeof(gMsg) );
                       MessageBox( hWndDlg,
                                   (LPSTR)gString,
                                   (LPSTR)gMsg,
                                   MB_ICONINFORMATION | MB_TASKMODAL );
                   }
                   break;

               case SP_API_RD: /* "Read" */
                   ToggleButton( hWndDlg, SP_API_RD );
                   lpfnMsgProc = MakeProcInstance( (PPROC)READMsgProc,
                                                   ghInst );
                   DialogBox( ghInst,
                              READ_DLG,
                              hWndDlg,
                              lpfnMsgProc );
                   xFreeProcInstance( lpfnMsgProc );
                   break;

               case SP_API_XRD: /* "Extended Read" */
                   ToggleButton( hWndDlg, SP_API_XRD );
                   lpfnMsgProc = MakeProcInstance( (PPROC)XREADMsgProc,
                                                   ghInst );
                   DialogBox( ghInst,
                              XREAD_DLG,
                              hWndDlg,
                              lpfnMsgProc );
                   xFreeProcInstance( lpfnMsgProc );
                   break;

               case SP_API_WT: /* "Write" */
                   ToggleButton( hWndDlg, SP_API_WT );
                   lpfnMsgProc = MakeProcInstance( (PPROC)WRITEMsgProc,
                                                   ghInst );
                   DialogBox( ghInst,
                              WRITE_DLG,
                              hWndDlg,
                              lpfnMsgProc );
                   xFreeProcInstance( lpfnMsgProc );
                   break;

               case SP_API_OWT: /* "Overwrite" */
                   ToggleButton( hWndDlg, SP_API_OWT );
                   lpfnMsgProc = MakeProcInstance( (PPROC)OWRITEMsgProc,
                                                   ghInst );
                   DialogBox( ghInst,
                              OWRITE_DLG,
                              hWndDlg,
                              lpfnMsgProc );
                   xFreeProcInstance( lpfnMsgProc );
                   break;

               case SP_API_DEC: /* "Decrement" */
                   ToggleButton( hWndDlg, SP_API_DEC );
                   lpfnMsgProc = MakeProcInstance( (PPROC)DECREMsgProc,
                                                   ghInst );
                   DialogBox( ghInst,
                              DEC_DLG,
                              hWndDlg,
                              lpfnMsgProc );
                   xFreeProcInstance( lpfnMsgProc );
                   break;

               case SP_API_QRY: /* "Query" */
                   ToggleButton( hWndDlg, SP_API_QRY );
                   lpfnMsgProc = MakeProcInstance( (PPROC)QUERYMsgProc,
                                                   ghInst );
                   DialogBox( ghInst,
                              QUERY_DLG,
                              hWndDlg,
                              lpfnMsgProc );
                   xFreeProcInstance( lpfnMsgProc );
                   break;

               case SP_API_ACT: /* "Activate" */
                   ToggleButton( hWndDlg, SP_API_ACT );
                   lpfnMsgProc = MakeProcInstance( (PPROC)ACTIVEMsgProc,
                                                   ghInst );
                   DialogBox( ghInst,
                              ACTIVATE_DLG,
                              hWndDlg,
                              lpfnMsgProc );
                   xFreeProcInstance( lpfnMsgProc );
                   break;

               case SP_NFO: /* "Info" */
                   ToggleButton( hWndDlg, SP_NFO );
                   lpfnMsgProc = MakeProcInstance( (PPROC)INFOMsgProc,
                                                   ghInst );
                   DialogBox( ghInst,
                              INFO_DLG,
                              hWndDlg,
                              lpfnMsgProc );
                   xFreeProcInstance( lpfnMsgProc );
                   break;

               case SP_VW: /* "View" */
                   ToggleButton( hWndDlg, SP_VW );
                   if ( !ghViewDlg )
                   {
                       lpfnVIEWMsgProc = MakeProcInstance( (PPROC)VIEWMsgProc,
                                                           ghInst );
                       if ( lpfnVIEWMsgProc )
                       {
                           ghViewDlg = CreateDialog( ghInst,
                                                     VIEW_DLG,
                                                     ghWndMain,
                                                     lpfnVIEWMsgProc );
                           if ( ghViewDlg )
                               ShowWindow( ghViewDlg, SW_SHOW );
                       }
                   }
                   else
                       SetFocus( ghViewDlg );
                   break;

               case SP_API_FMT:

  /************************************************************************/
  /* RNBOsproFormatPacket: Initialize the RB_SPRO_APIPACKET.              */
  /************************************************************************/

                    status = RNBOsproFormatPacket( ApiPacket,
                                                   sizeof( ApiPacket ));

                    if ( status != SP_SUCCESS )
                    {
                         fullStatus = RNBOsproGetFullStatus( ApiPacket );

                         FormatErrStr( (LPSTR)gMsg,
                                       IDS_ERR_FMT_FAILED,
                                       status,
                                       fullStatus );

                         ErrorMsg( NULL, (LPSTR)gMsg );
                    }
                    else
                    {
                         LoadString( ghInst,
                                     IDS_FMT,
                                     (LPSTR)gString,
                                     sizeof(gString) );

                         LoadString( ghInst,
                                     IDS_FMT_SUCCESS,
                                     (LPSTR)gResult,
                                     sizeof(gResult) );

                         MessageBox( hWndDlg,
                                    (LPSTR)gResult,
                                    (LPSTR)gString,
                                    MB_OK | MB_TASKMODAL );
                    }
                    break;

               case SP_API_INIT:

  /************************************************************************/
  /* RNBOsproInitialize: Initialize the RB_SPRO_APIPACKET and driver.     */
  /************************************************************************/

                    status = RNBOsproInitialize( ApiPacket );

                    if ( status != SP_SUCCESS )
                    {
                         fullStatus = RNBOsproGetFullStatus( ApiPacket );

                         FormatErrStr( (LPSTR)gMsg,
                                       IDS_ERR_INIT_FAILED,
                                       status,
                                       fullStatus );

                         ErrorMsg( NULL, (LPSTR)gMsg );
                    }
                    else
                    {
                         gDevID        = 0;
                         gWritePW      = 0;
                         gOverwritePW1 = 0;
                         gOverwritePW2 = 0;
                         gActivatePW1  = 0;
                         gActivatePW2  = 0;
                         gActivateCell = 0;
                         gDecCell      = 0;
                         gOwValue      = 0;           
                         gOwCell       = 0;           
                         gOwAccessCode = (RB_BYTE)-1; 
                         gQCell        = (RB_WORD)-1;             
                         gQrySize      = 0;                       
                         gQData[0]     = '\0';
                         gQResult[0]   = '\0';
                         gQResult32    = 0;                       
                         gRCell        = (RB_WORD)-1; 
                         gRData        = (RB_WORD)-1; 
                         gWCell        = (RB_WORD)-1; 
                         gWData        = (RB_WORD)-1; 
                         gWAccessCode  = (RB_BYTE)-1; 
                         gXRCell       = (RB_WORD)-1; 
                         gXRData       = (RB_WORD)-1; 
                         gXRAccessCode = (RB_BYTE)-1; 

                         LoadString( ghInst,
                                     IDS_INIT,
                                     (LPSTR)gString,
                                     sizeof(gString) );

                         LoadString( ghInst,
                                     IDS_INIT_SUCCESS,
                                     (LPSTR)gResult,
                                     sizeof(gResult) );

                         MessageBox( hWndDlg,
                                    (LPSTR)gResult,
                                    (LPSTR)gString,
                                    MB_OK | MB_TASKMODAL );
                    }
                    break;

               case SP_API_GFS:
                   ToggleButton( hWndDlg, SP_API_GFS );

  /************************************************************************/
  /* RNBOsproGetFullStatus: Retrieves full status information.            */
  /************************************************************************/

                   fullStatus = RNBOsproGetFullStatus( ApiPacket );

                   LoadString( ghInst,
                               IDS_GET_FULL_STATUS,
                               (LPSTR)gString,
                               sizeof(gString) );

                   LoadString( ghInst,
                               IDS_RESULT,
                               (LPSTR)gResult,
                               sizeof(gString) );

                   wsprintf( gMsg, "0x%04X", fullStatus );
                   lstrcat( gResult, gMsg );
                   MessageBox( hWndDlg,
                               (LPSTR)gResult,
                               (LPSTR)gString,
                               MB_OK | MB_TASKMODAL );
                   break;

               case SP_API_GV: /* "GetVersion" */
                   ToggleButton( hWndDlg, SP_API_GV );

                   lpfnMsgProc = MakeProcInstance( (PPROC)VERSIONMsgProc,
                                                   ghInst );
                   DialogBox( ghInst,
                              VERSION_DLG,
                              hWndDlg,
                              lpfnMsgProc );
                   xFreeProcInstance( lpfnMsgProc );
                   break;

               case IDOK:
                   ToggleButton( hWndDlg, IDOK );
                   PostMessage( hWndDlg,
                                (MESSAGE)WM_COMMAND,
                                (WPARAM)IDCANCEL,
                                (LPARAM)NULL );
                   break;

               case SP_API_CFG_LIB: /* Configure Lib */
                   ToggleButton( hWndDlg, SP_API_CFG_LIB );
                   lpfnMsgProc = MakeProcInstance( (PPROC)CfgLibDlg, ghInst );
                   if ( lpfnMsgProc )
                   {
                         DialogBox( ghInst, CFG_LIB_PARMS_DLG, hWndDlg, lpfnMsgProc );
                         xFreeProcInstance( lpfnMsgProc );
                   }
                    break;

               case SP_HLP: /* "Help" */
                   ToggleButton( hWndDlg, SP_HLP );
                   LoadString( ghInst,
                               IDS_MORE_INFO,
                               (LPSTR)gString,
                               sizeof(gString) );
                   LoadString( ghInst,
                               IDS_HELP,
                               (LPSTR)gMsg,
                               sizeof(gMsg) );
                   MessageBox( hWndDlg,
                               (LPSTR)gString,
                               (LPSTR)gMsg,
                               MB_ICONINFORMATION | MB_TASKMODAL );
                   break;

               case IDCANCEL:
                   ToggleButton( hWndDlg, IDCANCEL );
                   if ( ghViewDlg )
                       PostMessage( ghViewDlg,
                                    (MESSAGE)WM_CLOSE,
                                    (WPARAM)0,
                                    (LPARAM)0 );
                   DestroyWindow( hWndDlg );
                   ghCurrentDlg = NULL;
                   ghMainDlg    = NULL;
                   if ( lpfnVIEWMsgProc )
                   {
                       xFreeProcInstance( lpfnVIEWMsgProc );
                   }
                   PostMessage( ghWndMain,
                                (MESSAGE)WM_CLOSE,
                                (WPARAM)0,
                                (LPARAM)0 );
                   break;
           }
           break;

       default:
           return( FALSE );
   }
   return( TRUE );
} /* end MAINMsgProc  */

/***************************************************************************
* Function : WinProc
*
* Purpose  :
*
* Inputs   : hWnd    - handle to the current window.
*            Message - is the message to process.
*            wParam  - is a WPARAM size parameter, if any.
*            lParam  - is a LPARAM size parameter, if any.
*
* Outputs  : none.
*
* Returns  : LONG    - ????
****************************************************************************/
#ifdef __BORLANDC__
#pragma argsused
#endif
EXPORT_PROC
LONG WINAPI WndProc( HWND    hWnd,
                     MESSAGE Message,
                     WPARAM  wParam,
                     LPARAM  lParam )
{
   PPROC lpfnMAINMsgProc;

   /* to avoid warnings */
   #ifndef __BORLANDC__
   lParam = lParam;
   #endif

   switch ( Message )
   {
       case WM_COMMAND:
           switch( GET_LOWORD(wParam) )
           {
               case START_EVAL:
                   lpfnMAINMsgProc = MakeProcInstance( (PPROC)MAINMsgProc,
                                                       ghInst );
                   if ( lpfnMAINMsgProc )
                   {
                       ghMainDlg = CreateDialog( ghInst,
                                                 MAIN_DLG,
                                                 hWnd,
                                                 lpfnMAINMsgProc );
                       if ( ghMainDlg )
                           ShowWindow( ghMainDlg, SW_SHOW );
                   }
                   break;

               default:
                   return( DefWindowProc(hWnd, Message, wParam, lParam) );
           }
           break;

       case WM_CREATE:
           PostMessage( hWnd,
                        (MESSAGE)WM_COMMAND,
                        (WPARAM)START_EVAL,
                        (LPARAM)NULL );
           break;

       case WM_CLOSE:
           if ( ghMainDlg )
               PostMessage( ghMainDlg,
                            (MESSAGE)WM_CLOSE,
                            (WPARAM)0,
                            (LPARAM)0 );
           xFreeProcInstance( lpfnMAINMsgProc );
           DestroyWindow( hWnd );
           if ( hWnd == ghWndMain ) PostQuitMessage(0); /* Quit */
           break;

       default:
           return( DefWindowProc(hWnd, Message, wParam, lParam) );
   }
   return( 0L );
} /* end of WndProc */

/***************************************************************************
* Function : WinMain
*
* Purpose  : Main Windows entry point.
*
* Inputs   : hInstance     - a handle of the current instance.
*            hPrevInstance - a handle of the previouse instance.
*            lpszCmdLine   - pointer to command line parameters.
*            nCmdShow      - flag indicating if window should be visible.
*
* Outputs  : none.
*
* Returns  :
****************************************************************************/
#ifdef __BORLANDC__
#pragma argsused
#endif
EXPORT_PROC
int PASCAL WinMain( HANDLE hInstance,
                    HANDLE hPrevInstance,
                    LPSTR  lpszCmdLine,
                    int    nCmdShow )
{
   MSG       msg;
   SP_STATUS status;
   RB_WORD   fullStatus;

   /* to avoid warnings */
   #ifndef __BORLANDC__
   lpszCmdLine = lpszCmdLine;
   nCmdShow    = nCmdShow;
   #endif

   strcpy( gAppName, "SPRO" );
   ghInst = hInstance;
   if( !hPrevInstance )
       if ( !InitApplication( hInstance ) ) return( FALSE );

   if ( !InitInstance( hInstance, SW_HIDE ) ) return( FALSE );

  /************************************************************************/
  /* RNBOsproFormatPacket: Initialize the RB_SPRO_APIPACKET and driver.   */
  /* RNBOsproInitialize:                                                  */
  /************************************************************************/

   status = RNBOsproFormatPacket( ApiPacket,
                                  sizeof( ApiPacket ));

   if ( status != SP_SUCCESS )
   {
        fullStatus = RNBOsproGetFullStatus( ApiPacket );

        FormatErrStr( (LPSTR)gMsg,
                      IDS_ERR_FMT_FAILED,
                      status,
                      fullStatus );

        ErrorMsg( NULL, (LPSTR)gMsg );
   }

   status = RNBOsproInitialize( ApiPacket );

   if ( status != SP_SUCCESS )         /* Initialization failed!           */
   {
       fullStatus = RNBOsproGetFullStatus( ApiPacket );

       FormatErrStr( (LPSTR)gMsg,
                     IDS_ERR_INIT_FAILED,
                     status,
                     fullStatus );

       ErrorMsg( NULL, (LPSTR)gMsg );
       return( FALSE );
   }

   while( GetMessage( &msg, NULL, 0, 0 ) ) /* Until WM_QUIT message        */
   {
       if ( ghCurrentDlg == 0 || !IsDialogMessage( ghCurrentDlg, &msg ) )
       {
           TranslateMessage( &msg );
           DispatchMessage( &msg );
       }
   }
   return( msg.wParam );
} /* end WinMain */
/* end of file */
