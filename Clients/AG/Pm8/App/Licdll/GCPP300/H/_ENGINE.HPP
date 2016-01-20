//*
//* _ENGINE.HPP
//*
//* DESCRIPTION
//*         class GFFileIO
//*         class GFFileBaseEngine
//*
//*
//* The Greenleaf Comm++ Library
//*
//* Copyright (C) 1989-95 Greenleaf Software Inc.  All Rights Reserved.
//*
//* MODIFICATIONS
//*   May 15, 1995        3.00A   Moved file engine from filexfer.hpp
//*                               to this file.
//*   August 18, 1995     3.00B   Added skip_traversal to GFFileIO class
//*                               to enable using the file engine without
//*                               traversing subdirectories.  Also added
//*                               NO_TRAVERSE to t_state.
//*   August 18, 1995     3.00B   Added GCPP_ACCESS macro so access()
//*                               could be mapped to the correct function
//*                               name for different compilers.
//*   August 18, 1995     3.00B   Marked all functions virtual in derived
//*                               file engine classes.
//*

#ifndef _ENGINE_HPP
#define _ENGINE_HPP

#include <stdio.h>
#include "cppcomp.h"
#include "compplib.hpp"
#if !defined( GCPP_OS2 )
  #include <dos.h>
#endif

//*
//* File Engine definitions
//*

//*
//* The file engine class will enable the users to easily send multiple
//* files.  For protocols like Ymodem or Zmodem the user can
//* give the wildcards *.*, or *.CPP, etc, to send all matching files.
//*
#if defined( GCPP_OS2 )
  #define INCL_NOPM
  #include <os2.h>
  #define FILE_STRUCT               FILEFINDBUF3
  #define FILE_NAME                 achName
  #define FIND_FIRST                DosFindFirst
  #define FIND_NEXT                 DosFindNext
  #define FIND_DIRECTORY            0x0010

#elif ( defined( GCPP_SYMANTEC )  ||\
      defined( GCPP_MICROSOFT ) ||\
      defined( GCPP_WATCOM ) ) &&\
      !defined( GCPP_WIN32 )

typedef struct _find_t             FILE_STRUCT;
#define FILE_NAME                  name
#define FIND_FIRST( ws, fs, att )  _dos_findfirst( ws, att, fs )
#define FIND_NEXT                  _dos_findnext
#define FIND_DIRECTORY             _A_SUBDIR

#elif defined( GCPP_BORLAND ) && !defined( GCPP_WIN32 )

#include <dir.h>
typedef struct ffblk               FILE_STRUCT;
#define FILE_NAME                  ff_name
#define FIND_FIRST( ws, fs, att )  findfirst( ws, fs, att )
#define FIND_NEXT                  findnext
#define FIND_DIRECTORY             FA_DIREC

#elif defined( GCPP_WIN32 )

#include <windows.h>

typedef WIN32_FIND_DATA            FILE_STRUCT;
#define FILE_NAME                  cFileName
#define FIND_FIRST( ws, fs )       FindFirstFile( ws, fs )
#define FIND_NEXT                  FindNextFile
#define FIND_DIRECTORY             _A_SUBDIR

#endif

#if defined( GCPP_WIN32 ) && defined( GCPP_MICROSOFT )
  #define GCPP_STAT   _stat
#else
  #define GCPP_STAT   stat
#endif

#if defined( GCPP_MICROSOFT )
  #define GCPP_ACCESS _access
#else
  #define GCPP_ACCESS access
#endif

enum t_state { SCANNING, OPEN_SEARCH, SEARCHING, OPEN_TRAVERSE,
               SEARCH_TRAVERSE, DO_TRAVERSE, DONE_TRAVERSING, NO_TRAVERSE };

typedef struct t_search_file_data
{
  FILE_STRUCT file_block[ 10 ];
#if defined( GCPP_WIN32 )
  HANDLE hFile[ 10 ];
#endif
  char *current_input_name;
  char current_wild_path[ 256 ];
  char current_wild_name[ 256 ];
  t_state current_state;
  int current_level;
  int recurse_flag;
} GCPP_FILE_DATA;


class GCPP_CLASS_TYPE GFFileIO
{
  public:
    GCPP_PROTO GFFileIO( void );
    virtual GCPP_PROTO ~GFFileIO( void );

    virtual int GCPP_PROTO Initialize( char GCPP_DLL_FAR *input_name = NULL,
                            int recurse_flag = 0 ) = 0;
    virtual int GCPP_PROTO Open( char GCPP_DLL_FAR *filename,
                            char GCPP_DLL_FAR *mode ) = 0;
    virtual int GCPP_PROTO Close( void ) = 0;

    virtual int GCPP_PROTO SetCurrentPosition( long pos, int origin ) = 0;
    virtual long GCPP_PROTO GetCurrentPosition( void ) = 0;
    virtual int GCPP_PROTO SetTimeDate( void ) = 0;
    virtual int GCPP_PROTO GetTimeDate( void ) = 0;
    virtual long GCPP_PROTO GetLength( void ) = 0;
    virtual int GCPP_PROTO GetNextName( void ) = 0;

    virtual int GCPP_PROTO PutBuffer( unsigned char *buf, int buf_length ) = 0;
    virtual int GCPP_PROTO GetBuffer( unsigned char *buf, int buf_length ) = 0;
    virtual int GCPP_PROTO PutChar( char c ) = 0;
    virtual int GCPP_PROTO GetChar( char GCPP_DLL_FAR *c ) = 0;
    virtual int GCPP_PROTO Flush( void ) = 0;
    virtual void GCPP_PROTO Reset( void );
    virtual char GCPP_DLL_FAR * GCPP_PROTO GetInputName( void );
    virtual void GCPP_PROTO ProcessFileTime( long ftime );
    virtual void GCPP_PROTO FromJulian( long jdn );
    virtual long GCPP_PROTO CreateUnixTime( void );
    virtual long GCPP_PROTO ToJulian( void );
    virtual void GCPP_PROTO SetTimeDateValues( int yy,
                              int mm,
                              int dd,
                              int hmhour,
                              int hmmin,
                              int hmsec );
    virtual void GCPP_PROTO GetTimeDateValues( int GCPP_DLL_FAR &yy,
                              int GCPP_DLL_FAR &mm,
                              int GCPP_DLL_FAR &dd,
                              int GCPP_DLL_FAR &hmhour,
                              int GCPP_DLL_FAR &hmmin,
                              int GCPP_DLL_FAR &hmsec );
    int year;
    int month;
    int day;
    int hours;
    int minutes;
    int seconds;
    long length;
    int recursive_find;
    int skip_traverse;
    char *input_name_list;
    char input_name[ 514 ];
};


#if !defined( GCPP_WINDOWS ) && !defined( GCPP_WIN32 ) && !defined( GCPP_OS2 )

class GCPP_CLASS_TYPE GFFileBaseEngine : public GFFileIO
{
  public:
    GCPP_PROTO GFFileBaseEngine( void );
    virtual GCPP_PROTO ~GFFileBaseEngine( void );

    virtual int GCPP_PROTO Initialize( char GCPP_DLL_FAR *input_name = NULL,
                            int recurse_flag = 0 );
    virtual int GCPP_PROTO Open( char GCPP_DLL_FAR *filename,
                            char GCPP_DLL_FAR *mode );
    virtual int GCPP_PROTO Close( void );

    virtual int GCPP_PROTO SetCurrentPosition( long pos, int origin );
    virtual long GCPP_PROTO GetCurrentPosition( void );
    virtual int GCPP_PROTO SetTimeDate( void );
    virtual int GCPP_PROTO GetTimeDate( void );
    virtual long GCPP_PROTO GetLength( void );
    virtual int GCPP_PROTO GetNextName( void );

    virtual int GCPP_PROTO PutBuffer( unsigned char GCPP_DLL_FAR *buf,
                            int buf_length );
    virtual int GCPP_PROTO GetBuffer( unsigned char GCPP_DLL_FAR *buf,
                            int buf_length );
    virtual int GCPP_PROTO PutChar( char c );
    virtual int GCPP_PROTO GetChar( char GCPP_DLL_FAR *c );
    virtual int GCPP_PROTO Flush( void );

  protected:
    //*
    //* These functions are called by GetNextName()
    //*
    virtual int GCPP_PROTO FindFirst( char GCPP_DLL_FAR *name,
                              FILE_STRUCT GCPP_DLL_FAR *ff_block,
                              int attr ) = 0;
    virtual int GCPP_PROTO FindNext( void ) = 0;
    virtual void GCPP_PROTO GetNextWildName( void );

    FILE GCPP_DLL_FAR *file_pointer;
    GCPP_FILE_DATA GCPP_DLL_FAR *local_data;
    char output_name[ 128 ];
};

class GCPP_CLASS_TYPE GFDOSFileEngine : public GFFileBaseEngine
{
  public:
    GCPP_PROTO GFDOSFileEngine( void );
    GCPP_PROTO ~GFDOSFileEngine( void );
    virtual int GCPP_PROTO SetTimeDate( void );
    virtual int GCPP_PROTO GetTimeDate( void );
    virtual int GCPP_PROTO FindFirst( char GCPP_DLL_FAR *name,
                              FILE_STRUCT GCPP_DLL_FAR *ff_block,
                              int attr );
    virtual int GCPP_PROTO FindNext( void );
};

#define GFDefaultFileEngine   GFDOSFileEngine

#elif defined( GCPP_OS2 )

class GCPP_CLASS_TYPE GFOS2FileEngine : public GFFileIO
{
  public:
    GCPP_PROTO GFOS2FileEngine( void );
    GCPP_PROTO ~GFOS2FileEngine( void );

    virtual int GCPP_PROTO Initialize( char GCPP_DLL_FAR *input_name = NULL,
                            int recurse_flag = 0 );
    virtual int GCPP_PROTO Open( char GCPP_DLL_FAR *filename,
                            char GCPP_DLL_FAR *mode );
    virtual int GCPP_PROTO Close( void );

    virtual int GCPP_PROTO SetCurrentPosition( long pos, int origin );
    virtual long GCPP_PROTO GetCurrentPosition( void );
    virtual int GCPP_PROTO SetTimeDate( void );
    virtual int GCPP_PROTO GetTimeDate( void );
    virtual long GCPP_PROTO GetLength( void );
    virtual int GCPP_PROTO GetNextName( void );

    virtual int GCPP_PROTO PutBuffer( unsigned char *buf, int buf_length );
    virtual int GCPP_PROTO GetBuffer( unsigned char *buf, int buf_length );
    virtual int GCPP_PROTO PutChar( char c );
    virtual int GCPP_PROTO GetChar( char GCPP_DLL_FAR *c );
    virtual int GCPP_PROTO Flush( void );

  protected:
    virtual int GCPP_PROTO FindFirst( char GCPP_DLL_FAR *name,
                              FILE_STRUCT GCPP_DLL_FAR *ff_block,
                              int attr );
    virtual int GCPP_PROTO FindNext( void );
    virtual void GCPP_PROTO GetNextWildName( void );
    HFILE file_handle;
    int hdir;
    GCPP_FILE_DATA GCPP_DLL_FAR *local_data;
};

#define GFDefaultFileEngine   GFOS2FileEngine

#elif defined( GCPP_WIN32 )

class GCPP_CLASS_TYPE GFWin32FileEngine : public GFFileIO
{
  public:
    GCPP_PROTO GFWin32FileEngine( void );
    GCPP_PROTO ~GFWin32FileEngine( void );

    virtual int GCPP_PROTO Initialize( char GCPP_DLL_FAR *input_name = NULL,
                            int recurse_flag = 0 );
    virtual int GCPP_PROTO Open( char GCPP_DLL_FAR *filename,
                            char GCPP_DLL_FAR *mode );
    virtual int GCPP_PROTO Close( void );

    virtual int GCPP_PROTO SetCurrentPosition( long pos, int origin );
    virtual long GCPP_PROTO GetCurrentPosition( void );
    virtual int GCPP_PROTO SetTimeDate( void );
    virtual int GCPP_PROTO GetTimeDate( void );
    virtual long GCPP_PROTO GetLength( void );
    virtual int GCPP_PROTO GetNextName( void );

    virtual int GCPP_PROTO PutBuffer( unsigned char *buf, int buf_length );
    virtual int GCPP_PROTO GetBuffer( unsigned char *buf, int buf_length );
    virtual int GCPP_PROTO PutChar( char c );
    virtual int GCPP_PROTO GetChar( char GCPP_DLL_FAR *c );
    virtual int GCPP_PROTO Flush( void );

  protected:
    virtual int GCPP_PROTO FindFirst( char GCPP_DLL_FAR *name,
                              FILE_STRUCT GCPP_DLL_FAR *ff_block,
                              int attr );
    virtual int GCPP_PROTO FindNext( void );
    virtual void GCPP_PROTO GetNextWildName( void );
    HANDLE file_handle;
    GCPP_FILE_DATA GCPP_DLL_FAR *local_data;
};

#define GFDefaultFileEngine   GFWin32FileEngine

#else

class GCPP_CLASS_TYPE GFWinFileEngine : public GFFileIO
{
  public:
    GCPP_PROTO GFWinFileEngine( void );
    GCPP_PROTO ~GFWinFileEngine( void );

    virtual int GCPP_PROTO Initialize( char GCPP_DLL_FAR *input_name = NULL,
                            int recurse_flag = 0 );
    virtual int GCPP_PROTO Open( char GCPP_DLL_FAR *filename,
                            char GCPP_DLL_FAR *mode );
    virtual int GCPP_PROTO Close( void );

    virtual int GCPP_PROTO SetCurrentPosition( long pos, int origin );
    virtual long GCPP_PROTO GetCurrentPosition( void );
    virtual int GCPP_PROTO SetTimeDate( void );
    virtual int GCPP_PROTO GetTimeDate( void );
    virtual long GCPP_PROTO GetLength( void );
    virtual int GCPP_PROTO GetNextName( void );

    virtual int GCPP_PROTO PutBuffer( unsigned char *buf, int buf_length );
    virtual int GCPP_PROTO GetBuffer( unsigned char *buf, int buf_length );
    virtual int GCPP_PROTO PutChar( char c );
    virtual int GCPP_PROTO GetChar( char GCPP_DLL_FAR *c );
    virtual int GCPP_PROTO Flush( void );

  protected:
    virtual int GCPP_PROTO FindFirst( char GCPP_DLL_FAR *name,
                              FILE_STRUCT GCPP_DLL_FAR *ff_block,
                              int attr );
    virtual int GCPP_PROTO FindNext( void );
    virtual void GCPP_PROTO GetNextWildName( void );
    HFILE file_handle;
    GCPP_FILE_DATA GCPP_DLL_FAR *local_data;
};

#define GFDefaultFileEngine   GFWinFileEngine

#endif


#endif
