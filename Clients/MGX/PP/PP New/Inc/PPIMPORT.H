//////////////////////////////////////////////////////////////////////////////
//                                                                          //
//  PPIMPORT.H                                                              //
//                                                                          //
//////////////////////////////////////////////////////////////////////////////
#ifndef PPIMPORT_H
#define PPIMPORT_H

//////////////////////////////////////////////////////////////////////////////
//  Macros                                                                  //
//////////////////////////////////////////////////////////////////////////////
#define FILTER_DRIVER       _T("ISGDI9.DLL")
#define EXPORT_FILTER       _T("EBTIF9.FLT")
#define DEFAULT_PROFILE     _T("EBTIF9.INI")

#define IMPORT_NOERROR              0
#define IMPORT_INVALIDFILE_ERROR    1
#define IMPORT_SRCLIBLOAD_ERROR     2
#define IMPORT_DSTLIBLOAD_ERROR     3
#define IMPORT_IMPORTFILE_ERROR     4
#define IMPORT_DLLLOAD_ERROR        5

// these error codes are returned by the filters
#define IME_INVALID_FILE_TYPE       -23
#define IME_INVALID_FILE_VERSION    -24
#define IME_MEMORY                  0x1b  

//////////////////////////////////////////////////////////////////////////////
//  prototypes                                                              //
//////////////////////////////////////////////////////////////////////////////
BOOL SetupImportMenu( HMENU, ITEMID );
int  AstralImageImport( LPTSTR, LPTSTR, int );

int far PASCAL EXPORT GetSet_DialogStatus( HWND, int, int, int, int, int, 
                                           TCHAR far * far * /*LPTSTR far**/ );

#endif  // PPIMPORT_H

