//////////////////////////////////////////////////////////////////////////////
//                                                                          //
//  ISGDI9.H                                                                //
//  Standard header file for file import DLL's.                             //
//                                                                          //
//////////////////////////////////////////////////////////////////////////////
#ifndef ISGDI9_H
#define ISGDI9_H


//////////////////////////////////////////////////////////////////////////////
//  Macros                                                                  //
//////////////////////////////////////////////////////////////////////////////

#define IME_STATUS          0x0000  /* STATUS  MESSAGE */
#define IME_NOTIFY          0x0001  /* NOTIFY  MESSAGE */
#define IME_WARNING         0x0002  /* WARNING MESSAGE */
#define IME_ERROR           0x0003  /* ERROR   MESSAGE */

#define IME_OK              0x0000  /* NO ERROR                       */
#define IME_ABORT           0x0001  /* ABORT   IMPORT/EXPORT          */
#define IME_UPDATE_PERCENT -0x0000  /* the normal code for chk_esc()  */

#define IME_INIT_IMPORT    -0x0001  /* prescan & scan % restraints    */
#define IME_INIT_EXPORT    -0x0002  /* import & export % restraints   */
#define IME_PRESCAN        -0x0003  /* we are in import prescan mode  */
#define IME_IMPORT         -0x0004  /* we are in import scan mode     */

#define IME_GENERIC_NOTIFY  0x0042  /* Imcompatible CGI.DLL version   */
#define IME_GENERIC_WARNING 0x0043  /* Imcompatible CGI.DLL version   */
#define IME_GENERIC_ERROR   0x0044  /* Imcompatible CGI.DLL version   */
#define IME_GENERIC_UNKNOWN_CODE  0x0045  /* Imcompatible CGI.DLL version   */


//////////////////////////////////////////////////////////////////////////////
//  Type definitions                                                        //
//////////////////////////////////////////////////////////////////////////////
#define DLLPTR  FAR

typedef char    CHAR;
typedef INT ( FAR PASCAL *GETSET_STATUS )( WORD, INT, INT, INT, INT, INT,
                                            CHAR FAR * FAR * );
typedef INT ( FAR PASCAL *CREATESETUP )
                    ( HANDLE, CHAR FAR *, CHAR FAR *, CHAR FAR * );
typedef INT ( FAR PASCAL *DESTROYSETUP )(HANDLE);
typedef INT ( FAR PASCAL *IMPORTPROC )( LPSTR, LPSTR, LPSTR, LPSTR, LPSTR );
typedef INT ( FAR PASCAL *DD_ENTRY )( INT16, CHAR DLLPTR *, UINT16 );


//////////////////////////////////////////////////////////////////////////////
//  Structure definitions                                                   //
//////////////////////////////////////////////////////////////////////////////
typedef struct
{
  WORD            Version;
  WORD            hDialog;
  GETSET_STATUS   GetSet_Status;
  DD_ENTRY        dd_entry;
  WORD            hDC;
  WORD            hIC;
  WORD            hMF;
  INT             Left;
  INT             Top;
  INT             Right;
  INT             Bottom;
  INT             Inch;
  HANDLE          FileHandle;
  ULONG           FilePos;
} EXPORT_OPTIONS, 
  FAR * LPEXPORT_OPTIONS;

typedef struct
{
  WORD             Version;
  HANDLE           hInputData;
  HANDLE           hOutputData;
  CHAR   DLLPTR   *lpszDriverPath;
  CHAR   DLLPTR   *lpszWorkPath;
  CHAR   DLLPTR   *lpszFontPath;
  LPEXPORT_OPTIONS lpExport;
  HANDLE           FileHandle;
  ULONG            FilePos;
} IMPORT_OPTIONS, FAR * LPIMPORT_OPTIONS;

//////////////////////////////////////////////////////////////////////////////
//  Function Prototypes                                                     //
//////////////////////////////////////////////////////////////////////////////
HANDLE FAR PASCAL CreateSetup( HANDLE, CHAR DLLPTR *, CHAR DLLPTR *, CHAR DLLPTR * );
INT    FAR PASCAL ImportFile(CHAR  DLLPTR    *lpszDevice,
                             CHAR  DLLPTR    *lpszDriver,
                             CHAR  DLLPTR    *lpszConnection,
                             CHAR  DLLPTR    *lpszFileName,
                             LPIMPORT_OPTIONS lpOptions);
INT    FAR PASCAL DestroySetup(HANDLE);

#endif  // ISGDI9_H
