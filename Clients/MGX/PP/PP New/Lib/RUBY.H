/* @doc
 ****************************************************************************

         (C) Copyright 1994 by Micrografx, Inc.  All rights reserved.
    Permission to use this work for any purpose must by obtained in writing
            from Micrografx, 1303 Arapaho, Richardson, TX. 75081.

 ****************************************************************************

                                  RUBY.H

 @module RUBY.H - Main header for the Filter Mini-App |
                  This header is where the structure and function definitions
                  for using the Filter Mini-App are maintained.
                  NOTE! Any modifications to the interfaces for Ruby should
                  also be reflected in DLLRUBY.CPP until the DLL version of
                  the Filter Mini-App is phased out.

 ****************************************************************************

 
*/

#ifndef _RUBY_H_
#define _RUBY_H_

#ifdef __cplusplus
extern "C" {
#endif

// @enum RUBYPROC | Defined as WINAPI __export for Win16 and WINAPI for Win32
#ifndef WIN32
#define RUBYPROC        WINAPI __export
#else
#define RUBYPROC        WINAPI
#endif


/******************************** Constants *********************************/

// @enum RUBY_MAX_INTERFACES | Maximum number of interfaces that can be
// specified when passing interface lists to Ruby

#define RUBY_MAX_INTERFACES     10  


// @enum RUBY_MAX_ERROR_STRING | Maximum length of the error strings without
// the trailing NULL

#define RUBY_MAX_ERROR_STRING   128


// current major version number
#define RUBY_VER_MAJOR  1

// current minor version number
#define RUBY_VER_MINOR  1

// current internal build version
#define RUBY_VER_BUILD  1


/********************************** Types ***********************************/

// @enum RUBYRETURN | Return codes for the Filter Mini-App functions
typedef enum
{
    RUBY_SUCCESS = 1,        // @emem (1) Translation completed succesfully
    RUBY_FAILED,             // @emem (2) Translation failed for an
                             // unspecified reason
    RUBY_UNSUPPORTED_FORMAT, // @emem (3) The specifice file format is
                             // unsupported
    RUBY_BAD_INTERFACE_TYPE, // @emem (4) The RUBYINTERFACETYPE parameter is
                             // invalid
    RUBY_BAD_PARAMETER,      // @emem (5) A parameter passed to the RUBY was
                             // invalid
    RUBY_USER_CANCEL,        // @emem (6) The user cancelled the operation
    RUBY_NOT_PRESENT         // @emem (7) This error will be returned if
                             // a critical piece of Ruby is missing
} RUBYRETURN, FAR * PRUBYRETURN;



// @enum RUBYINTERFACETYPE | Interface definitions
typedef enum
{
    RUBY_DT_MARKER = 0,           // @emem (0) Marker used to indicate the end
                                  // of a list of interfaces
    RUBY_DT_WMF_FILE = 0x0001,    // @emem (x1) interface data is a file in
                                  // WMF format
    RUBY_DT_DRW_FILE = 0x0002,    // @emem (x2) interface data is a file in
                                  // DRW format
    RUBY_DT_TIFF_FILE = 0x0004,   // @emem (x4) interface data is a file in
                                  // TIFF format
    RUBY_DT_BMP_FILE = 0x0008,    // @emem (x8) interface data is a file in
                                  // BMP format

    RUBY_DT_FRAME_MEMORY = 0x0010 // @emem (x10) interface data is a memory
                                  // stream in FRAME format

} RUBYINTERFACETYPE, FAR *PFMINTERFACETYPE;


// @enum RUBYSTATUSCODE | Valid status codes that will be sent to a client
//                        status procedure
typedef enum
{
    RUBY_STATUS_PERCENT = 1,      // @emem Normal status notification.
    RUBY_STATUS_NOTIFY  = 2,      // @emem Benign notification message, can
                                  // be logged, but generally should not be
                                  // displayed.
    RUBY_STATUS_WARNING = 3,      // @emem Warning that can be displayed
                                  // to the user.
    RUBY_STATUS_ERROR   = 4,      // @emem Error severe enough to halt the
                                  // translation.
    RUBY_STATUS_UNKNOWN = 5       // @emem Unknown error. Should be treated
                                  // as fatal.
} RUBYSTATUSCODE;


typedef BOOL (RUBYPROC * CLIENTSTATUSPROC)( BYTE, RUBYSTATUSCODE, LPCSTR );



// @struct RUBYIMPORTFILE | Used when the client is importing a file
typedef struct
{
    UINT              uiSize;           // @field size of this structure.
    LPCSTR            pszTempFile;      // @field Full path and name of the
                                        // temporary file that is to be used
                                        // for data transfer.
    RUBYINTERFACETYPE Interface;        // @field Indicates the what interface
                                        // to use when returning data to the
                                        // client.
    LPCSTR            pszSrcFile;       // @field Full path and name of the
                                        // source file.
    LPCSTR            pszSrcFormat;     // @field Common extension of the
                                        // source file. Used to determine the
                                        // file type.
    CLIENTSTATUSPROC  pStatusProc;      // @field Pointer to the client supplied
                                        // status callback.  NULL if RUBY should
                                        // handle status and error processing.
    HWND              hwndParent;       // @field Window to use as the parent
                                        // for any error or status dialogs
    BOOL              bConfigure;       // @field BOOLEAN to allow for 
                                        // translation configuration.
    RUBYRETURN        RetCode;          // @field RUBY return code.
} RUBYIMPORTFILE, FAR *PRUBYIMPORTFILE;



// @struct RUBYEXPORTFILE | Used when the client is exporting a file
typedef struct
{
    UINT              uiSize;           // @field size of this structure.
    LPCSTR            pszTempFile;      // @field Full path and name of the
                                        // temporary file that is to be used
                                        // for data transfer.
    RUBYINTERFACETYPE Interface;        // @field Indicates the what interface
                                        // to use when accepting data from the
                                        // client.
    LPCSTR            pszDstFile;       // @field Full path and name of the
                                        // destination file.
    LPCSTR            pszDstFormat;     // @field Common extension of the
                                        // destination file. Used to determine
                                        // the file type.
    CLIENTSTATUSPROC  pStatusProc;      // @field Pointer to the client supplied
                                        // status callback. NULL if RUBY should
                                        // handle status and error processing.
    HWND              hwndParent;       // @field Window to use as the parent
                                        // for any error or status dialogs
    BOOL              bConfigure;       // @field BOOLEAN to allow for 
                                        // translation configuration.
    RUBYRETURN        RetCode;          // @field RUBY return code.
} RUBYEXPORTFILE, FAR *PRUBYEXPORTFILE;


// @struct RUBYIMPORT | Used when the client is importing a file
typedef struct
{
    UINT              uiSize;           // @field size of this structure.
    LPCSTR            pszTempFile;      // @field Full path and name of the
                                        // temporary file that is to be used
                                        // for data transfer.
    RUBYINTERFACETYPE Interface;        // @field Indicates the what interface
                                        // to use when returning data to the
                                        // client.
    CLIENTSTATUSPROC  pStatusProc;      // @field Pointer to the client supplied
                                        // status callback.  NULL if RUBY should
                                        // handle status and error processing.
    HWND              hwndParent;       // @field Window to use as the parent
                                        // for any error or status dialogs
    LPCSTR            pszWindowTitle;   // @field Title that should appear in
                                        // the file selection dialog.
    BOOL              bConfigure;       // @field BOOLEAN to allow for 
                                        // translation configuration.
    LPCSTR            pszDefaultFilter; // @field String indicating what filter
                                        // or filter type should be used as the
                                        // initial filter type in the file
                                        // selection dialog.
    RUBYRETURN        RetCode;          // @field RUBY return code.
} RUBYIMPORT, FAR *PRUBYIMPORT;



// @struct RUBYEXPORT | Used when the client is exporting a file
typedef struct
{
    UINT              uiSize;           // @field size of this structure.
    LPCSTR            pszTempFile;      // @field Full path and name of the
                                        // temporary file that is to be used
                                        // for data transfer.
    RUBYINTERFACETYPE Interface;        // @field Indicates the what interface
                                        // to use when accepting data from the
                                        // client.
    CLIENTSTATUSPROC  pStatusProc;      // @field Pointer to the client supplied
                                        // status callback.  NULL if RUBY should
                                        // handle status and error processing.
    HWND              hwndParent;       // @field Window to use as the parent
                                        // for any error or status dialogs
    LPCSTR            pszWindowTitle;   // @field Title that should appear in
                                        // the file selection dialog.
    BOOL              bConfigure;       // @field BOOLEAN to allow for 
                                        // translation configuration.
    LPCSTR            pszDefaultFilter; // @field String indicating what filter
                                        // or filter type should be used as the
                                        // initial filter type in the file
                                        // selection dialog.
    RUBYRETURN        RetCode;          // @field RUBY return code.
} RUBYEXPORT, FAR *PRUBYEXPORT;



// @struct RUBYQUERYFORMATS | Used when the client is querying ruby to get a
// list of supported file types
typedef struct
{
    UINT               uiSize;          // @field Size of this structure.
    DWORD              Interfaces;      // @field bitfield containing the interfaces
                                        // that are supported by the client.
    LPCSTR             pszFormatString; // @field format string
                                        // that should be passed to the common
                                        // dialog. Filled in by Ruby.
    RUBYRETURN         RetCode;         // @field RUBY return code.
} RUBYQUERYFORMATS, FAR *PRUBYQUERYFORMATS;


// @struct RUBYCHOOSEFILE | Temp
typedef struct
{
    UINT              uiSize;           // @field Size of this structure.
    DWORD             Interfaces;       // @field bitfield containing the interfaces
                                        // that are supported by the client.
    HWND              hwndParent;       // @field Window to use as the parent
                                        // for the file selection dialog.
    LPCSTR            pszWindowTitle;   // @field Title that should appear in
                                        // the file selection dialog.
    LPCSTR            pszDefaultFilter; // @field String indicating what filter
                                        // or filter type should be used as the
                                        // initial filter type in the file
                                        // selection dialog.
    LPCSTR            pszFileData;      // @field Buffer that will be filled in
                                        // by Ruby.  This buffer will contain
                                        // the file that the user selected and
                                        // the common extension.
    RUBYRETURN        RetCode;          // @field RUBY return code.
} RUBYCHOOSEFILE, FAR *PRUBYCHOOSEFILE;


// @struct RUBYGETINTERFACE | Temp
typedef struct
{
    UINT               uiSize;          // @field Size of this structure.
    UINT               Preferences;     // @field Preferences, currently unused.
    LPCSTR             pszFileFormat;   // @field Common extension of the file the client
                                        // wants Ruby to process.
    RUBYINTERFACETYPE  Interfaces[ RUBY_MAX_INTERFACES ];  // @field Pointer to an array of interfaces
                                        // that can be used to import a file.
    int                nNumInterfaces;  // @field Number of interfaces contained
                                        // in the Interfaces array.
    RUBYRETURN         RetCode;         // @field RUBY return code.
} RUBYGETINTERFACE, FAR *PRUBYGETINTERFACE;


/********************************* Functions ********************************/


BOOL RUBYPROC rubyConfigureExport( LPCSTR pszFormat );

typedef BOOL (RUBYPROC *FPRUBYCONFIGUREEXPORT) ( LPCSTR );

BOOL RUBYPROC rubyConfigureImport( PFMINTERFACETYPE pritInterface );

typedef BOOL (RUBYPROC *FPRUBYCONFIGUREIMPORT) ( PFMINTERFACETYPE );

BOOL RUBYPROC rubyExportFile( PRUBYEXPORTFILE pExportData );

typedef BOOL (RUBYPROC *FPRUBYEXPORTFILE) ( PRUBYEXPORTFILE );


BOOL RUBYPROC rubyExport( PRUBYEXPORT pExportData );

typedef BOOL (RUBYPROC *FPRUBYEXPORT) ( PRUBYEXPORT );


BOOL RUBYPROC rubyImportFile( PRUBYIMPORTFILE pImportData );

typedef BOOL (RUBYPROC *FPRUBYIMPORTFILE) ( PRUBYIMPORTFILE );


BOOL RUBYPROC rubyImport( PRUBYIMPORT pImportData );

typedef BOOL (RUBYPROC *FPRUBYIMPORT) ( PRUBYIMPORT );


BOOL RUBYPROC rubyShowVersion( HWND hwndParent );

typedef BOOL (RUBYPROC *FPRUBYSHOWVERSION) ( HWND );


BOOL RUBYPROC rubyShowError( HWND hwndParent, LPSTR lpszErrorString);

typedef BOOL (RUBYPROC *FPRUBYSHOWERROR) ( HWND, LPSTR );


DWORD RUBYPROC rubyGetVersion( void );

typedef DWORD (RUBYPROC *FPRUBYGETVERSION) ( void );


BOOL RUBYPROC rubyGetErrorString( LPSTR pszDst, WORD Length,
                                  RUBYRETURN rrError );

typedef BOOL (RUBYPROC *FPRUBYGETERRORSTRING) ( LPSTR pszDst,
                                                WORD Length,
                                                RUBYRETURN rrError );

BOOL RUBYPROC rubyQueryImportFormats( PRUBYQUERYFORMATS pQueryData );

typedef BOOL (RUBYPROC *FPRUBYQUERYIMPORTFORMATS) ( PRUBYQUERYFORMATS pQueryData );


BOOL RUBYPROC rubyQueryExportFormats( PRUBYQUERYFORMATS pQueryData );

typedef BOOL (RUBYPROC *FPRUBYQUERYEXPORTFORMATS) ( PRUBYQUERYFORMATS pQueryData );


BOOL RUBYPROC rubyChooseExportFile( PRUBYCHOOSEFILE pChooseData );

typedef BOOL (RUBYPROC *FPRUBYCHOOSEEXPORTFILE) ( PRUBYCHOOSEFILE pChooseData );


BOOL RUBYPROC rubyChooseImportFile( PRUBYCHOOSEFILE pChooseData );

typedef BOOL (RUBYPROC *FPRUBYCHOOSEIMPORTFILE) ( PRUBYCHOOSEFILE pChooseData );


BOOL RUBYPROC rubyGetImportInterface( PRUBYGETINTERFACE pInterfaceData );

typedef BOOL (RUBYPROC *FPRUBYGETIMPORTINTERFACE) ( PRUBYGETINTERFACE pInterfaceData );


BOOL RUBYPROC rubyGetExportInterface( PRUBYGETINTERFACE pInterfaceData );

typedef BOOL (RUBYPROC *FPRUBYGETEXPORTINTERFACE) ( PRUBYGETINTERFACE pInterfaceData );


BOOL RUBYPROC rubyFreeBuffer( LPCSTR pszString );

typedef BOOL (RUBYPROC *FPRUBYFREEBUFFER) ( LPCSTR pszString );


#ifdef __cplusplus
}
#endif


#endif  //_RUBY_H_

