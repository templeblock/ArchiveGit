/******************************************************************************

   Module Name: tdbexewk.c
   Description: TPS debugging support executable symbolic information walker.
   Version:     v0.00
   Author:      Michael C. Robert
   Date:        7/13/1994
   Copyright:   (c) Turning Point Software, Inc., 1994.  All Rights Reserved.

   Revision History:

   Date     Who      What
   ----     ---      ----
   7/13/94  MCR      Initial version.

   Notes:

      In the comments describing the parameters for each function are symbols
      indicating whether the parameter is for input or output or both.  The
      symbols' meaning is as follows:

         ==>   Parameter contains input information for the function.
         <==   The parameter will receive output information from the function.
         <=>   The parameter is both of the above. 

******************************************************************************/

/* INCLUDE FILES
*/

#include <tps.h>
#include <toolhelp.h>
#include "tdbexewk.h"

/* SYMBOLIC CONSTANTS
*/

/* EWM_ = Executable Walker Metric */

#define	_sigchar(b,p)			( ((DWORD)(c)) << ((3-p)*8) )
#define	Signature(a,b,c,d)	(_sigchar(a,0)|_sigchar(b,1)|_sigchar(c,2)|_sigchar(d,3))

#defiine	EWM_VALIDNB				Signature('N','B','0','9')
//#define  EWM_VALIDNB          0x3930424E     /* Supported signature */
#define  EWM_XREADCURRENT     0x7FFFFFFF     /* Read from current position */
#define  EWM_MAXEXESIZE       0x7FFFFFFE     /* Maximum executable file size */

/* SST_ = SubSection Type */

#define  SST_MODULE           0x120
#define  SST_TYPES            0x121
#define  SST_PUBLIC           0x122
#define  SST_PUBLICSYM        0x123
#define  SST_SYMBOLS          0x124
#define  SST_ALIGNSYM         0x125 
#define  SST_SRCLNSEG         0x126
#define  SST_SRCMODULE        0x127 
#define  SST_LIBRARIES        0x128
#define  SST_GLOBALSYM        0x129
#define  SST_GLOBALPUB        0x12a
#define  SST_GLOBALTYPES      0x12b
#define  SST_MPC              0x12c
#define  SST_SEGMAP           0x12d
#define  SST_SEGNAME          0x12e
#define  SST_PRECOMP          0x12f
#define  SST_OFFSETMAP16      0x131
#define  SST_OFFSETMAP32      0x132
#define  SST_FILEINDEX        0x133
#define  SST_STATICSYM        0x134

/* TYPEDEFS
*/

typedef struct _EXESYMSTART
{
   DWORD          dwSignature;               /* NBnn signature */
   DWORD          dwStartOffsetFromEOF;      /* Start of symbolic information 
                                             ** as an offset from the end of
                                             ** the executable file
                                             */
}
   EXESYMSTART;                              /* Start of symbolic information
                                             ** structure definition
                                             */

typedef struct _EXESYMHDR
{
   DWORD          dwSignature;               /* NBnn signature */
   DWORD          dwDirOffset;               /* Subsection directory offset */
}
   EXESYMHDR;                                /* Executable symbols header
                                             ** structure definition
                                             */

typedef struct _DIRHDR
{
   WORD           wDirHdrLen;                /* Directory header length */
   WORD           wDirEntryLen;              /* Directory entry length */
   DWORD          dwDirEntries;              /* Directory entry count */
   DWORD          dwNextDirOffset;           /* Offset of the next subsection
                                             ** directory from base (currently 
                                             ** not used)
                                             */
   DWORD          dwFlags;                   /* Flags (currently not used) */
}
   DIRHDR;                                   /* Subsection directory header
                                             ** structure definition
                                             */

typedef struct _DIRENTRY
{
   WORD           wSst;                      /* Sub-section type */
   WORD           wModIdx;                   /* Module index (1 based, 
                                             ** 0xFFFF non-module specific) 
                                             */
   DWORD          dwTableOffset;             /* Table offset from base */
   DWORD          dwSubSecBytes;             /* Number of bytes in subsection */
}
   DIRENTRY;                                 /* Subsection directory entry
                                             ** structure definition
                                             */

typedef struct _SRCMODHDR
{
   WORD           wFiles;                    /* Source files in table */
   WORD           wSegments;                 /* Segments receiving code from 
                                             ** module
                                             */
}
   SRCMODHDR;                                /* Source module header structure
                                             ** definition
                                             */

typedef struct _SRCFILHDR
{
   WORD           wSegments;                 /* Segments receive code from this
                                             ** source file
                                             */
   WORD           wReserved;                 /* Reserved (must be 0) */
}
   SRCFILHDR;                                /* Source file header structure
                                             ** definition
                                             */

typedef struct _SRCLINHDR
{
   WORD           wSegment;                  /* Segment index for this list */
   WORD           wLinePairs;                /* Line segment offset pairs 
                                             ** in list
                                             */
}
   SRCLINHDR;                                /* Source line header structure
                                             ** definition
                                             */

/* MACROS
*/

/* GLOBAL VARIABLES
*/

/* FUNCTION PROTOYPES
*/

/******************************************************************************

   Function Name: tbexewkGetSourceLine

   Description:

      Retrieves the source file name and line number given a module handle,
      a segment selector, and an IP address.

   Parameters:

      hModule        ==>   Handle to the module containing the segment.

      wSegment       ==>   Handle to the segment containing the line of code.

      wIP            ==>   Instruction Pointer for the line of code.

      lpszSourceFile <==   Buffer to receive the source file string.

      wSourceFileLen ==>   Length of the source file string buffer.

      lpiLine        <==   Buffer to receive the line number.

   Return Values:

      Success: TRUE
      Failure: FALSE

   Revisions:

   Notes:

******************************************************************************/

BOOLEAN TPSAPI tdbexewkGetSourceLine( 

   HMODULE hModule, 
   WORD wSegment, 
   WORD wIP, 
   LPSZ lpszSourceFile, 
   WORD wSourceFileLen,
   int FAR *lpiSourceLine 

)

{
   /* Variable definition.
   */

   LONG           lExeLen;                   /* Executable file length */

   MODULEENTRY    me;                        /* Module entry structure */

   static SZ      szExe[ MAX_PATH + MAX_MODULE_NAME + 1 ];
                                             /* Executable file string */

   OFSTRUCT       of;                        /* Open file structure */

   int            fhExe;                     /* Executable file handle */

   /* Test the validity of the parameters.
   */

   if ( IsBadStringPtr( lpszSourceFile, wSourceFileLen ) )
   {
      DEBUGMSG( "TDBEXEWK: Invalid source file buffer specified" );
      return ( FALSE );
   }

   if ( IsBadWritePtr( lpiSourceLine ) )
   {
      DEBUGMSG( "TDBEXEWK: Invalid source line buffer specified" );
      return ( FALSE );
   }

   /* Open the executable file.
   */

   if ( !ModuleFindHandle( &me, hModule ) )
   {
      DEBUGMSG( "TDBEXEWK: Can't get module entry information" );
      return ( FALSE );
   }

   lstrcpy( szExe, me.szExePath );
   lstrcat( szExe, me.szModule );

   fhExe = OpenFile( szExe, &of, OF_READ );
   if ( fhExe == -1 )
   {
      DEBUGMSG( "TDBEXEWK: Can't open executable file" );
      return ( FALSE );
   }

   /* Find the source line.
   */

   fReturn = FindSourceLine( fhExe, 
                             wSegment, 
                             wIP, 
                             lpszSourceFile, 
                             wSourceFileLen,
                             lpiSourceLine
                           );

   /* Close the executable file.
   */

   _lclose( fhExe );

   /* Return whether or not we succeeded.
   */

   return ( fReturn );

}

/******************************************************************************

   Function Name: FindSourceLine

   Description:

      Retrieves the source file name and line number given an open executable
      file, a segment selector, and an IP address.

   Parameters:

      fhExe          ==>   Handle to the executable file.

      wSegment       ==>   Handle to the segment containing the line of code.

      wIP            ==>   Instruction Pointer for the line of code.

      lpszSourceFile <==   Buffer to receive the source file string.

      wSourceFileLen ==>   Length of the source file string buffer.

      lpiLine        <==   Buffer to receive the line number.

   Return Values:

      Success: TRUE
      Failure: FALSE

   Revisions:

   Notes:

******************************************************************************/

BOOLEAN TPSAPI FindSourceLine( 

   int fhExe,
   WORD wSegment, 
   WORD wIP, 
   LPSZ lpszSourceFile, 
   WORD wSourceFileLen,
   int FAR *lpiSourceLine 

)

{
   /* Variable definition.
   */


   LONG           lExeLen;                   /* Executable file length */

   EXESYMSTART    exesymstart;               /* Symbolic information start */

   LONG           lfaBase;                   /* File location of symbolic info in EXE */

   EXESYMHDR      exesymhdr;                 /* Symbolic header */

   LONG           lfoDir;                    /* Location of symbolic info directories */

   DIRHDR         dirhdr;                    /* Directory header structure */

   LONG           lfaDirEntries;             /* File location of directory entries */

   DWORD          dwDirEntryIdx;             /* Current directory table entry index */

   DIRENTRY       direntry;                  /* Directory entry structure */

   LONG           lfaTable;                  /* File location of symbolic info table */

   SRCMODHDR      srcmodhdr;                 /* Source module header structure */

   LONG           lfaSrcFiles;               /* File location of source files table */

   WORD           wSrcFileIdx;               /* Current source file table entry index */

   DWORD          dwSrcFilHdrOffset;         /* Source file header table offset */

   SRCFILHDR      srcfilhdr;                 /* Source file header structure */

   LONG           lfaSrcSegments;            /* File location of source file segments table */

   WORD           wSegmentIdx;               /* Current file segment table entry index */

   DWORD          dwSrcLinHdrOffset;         /* Source line header table offset */

   SRCLINHDR      srclinhdr;                 /* Source line header structure */

   LONG           lfaSrcLines;               /* File location of source lines table */

   WORD           wLineIdx;                  /* Current source line table entry index */

   DWORD          dwIP;                      /* IP offset in segment of source line */

   WORD           wNameLen;                  /* Length of source file name string */

   /* Get the length of the executable file.
   */

   lExeLen = _llseek( fhExe, 0L, SEEK_END );
   if ( lExeLen > EWM_MAXEXESIZE )
   {
      DEBUGMSG( "TDBEXEWK: Executable file exceeds 0x7FFFFFFE bytes" );
      return ( FALSE );
   }

   /* Find the executable symbolic information base.
   */

   if ( !XRead( fhExe,
                - sizeof ( EXESYMSTART ),
                &exesymstart,
                sizeof ( EXESYMSTART )
              )
      )
      return ( FALSE );

   if ( exesymstart.dwSignature != EWM_VALIDNB )
   {

      DEBUGMSG( "TDBEXEWK: Unsupported symbolic format in executable (not NB09)" );
      return ( FALSE );
   }

   lfaBase = dwExeLen - exesymstart.dwStartOffsetFromEOF;

   /* Find the subsection information directory base.
   */

   if ( !XRead( fhExe,
                lfaBase,
                &exesymhdr,
                sizeof ( EXESYMHDR )
              )
      )
      return ( FALSE );

   if ( exesymhdr.dwSignature != EWM_VALIDNB )
   {
      DEBUGMSG( "TDBEXEWK: Unsupported symbolic format in executable (not NB09)" );
      return ( FALSE );
   }

   lfoDir = exesymhdr.dwDirOffset;

   /* Loop through the directories.
   */

   while ( lfoDir != 0L )
   {
      /* Read the directory header.
      */

      if ( !XRead( fhExe,
                   lfaBase + lfoDir,
                   &dirhdr,
                   sizeof ( DIRHDR )
                 )
         )
         return ( FALSE );

      if ( dirhdr.wDirHdrLen != sizeof ( DIRHDR ) )
      {
         DEBUGMSG( "TDBEXEWK: Executable file contains unsupported symbolic directory headers" );
         return ( FALSE );
      }

      if ( dirhdr.wDirEntryLen != sizeof ( DIRENTRY ) )
      {
         DEBUGMSG( "TDBEXEWK: Executable file contains unsupported symbolic directory entries" );
         return ( FALSE );
      }

      lfaDirEntries = lfaBase + lfoDir + sizeof ( DIRHDR );

      lfoDir = dirhdr.dwNextDirOffset;

      /* Loop through the directory entries.
      */

      for ( dwDirEntryIdx = 0; dwDirEntryIdx < dirhdr.dwDirEntries; ++dwDirEntryIdx )
      {
         /* Read the next directory entry.
         */

         if ( !XRead( fhExe,
                      lfaDirEntries + dwDirEntryIdx * sizeof ( DIRENTRY ),
                      &direntry,
                      sizeof ( DIRENTRY )
                    )
            )
            return ( FALSE );

         /* Skip if not a source module entry.
         */

         if ( direntry.wSst != SST_SRCMODULE )
            continue;

         /* Get the source module structure.
         */

         lfaTable = lfaBase + direntry.dwTableOffset;

         if ( !XRead( fhExe,
                      lfaTable,
                      &srcmodhdr,
                      sizeof ( SRCMODHDR )
                    )
            )
            return ( FALSE );

         /* Loop through the source files.
         */

         lfaSrcFiles = lfoTable + sizeof ( SRCMODHDR );

         for ( wSrcFileIdx = 0; wSrcFileIdx < srcmodhdr.wFiles; ++wSrcFileIdx )
         {
            /* Read the next source file header.
            */

            if ( !XRead( fhExe,
                         lfaSrcFiles + wSrcFileIdx * sizeof ( DWORD ),
                         &dwSrcFilHdrOffset,
                         sizeof ( DWORD )
                       )
               )
               return ( FALSE );

            if ( !XRead( fhExe,
                         lfaTable + dwSrcFilHdrOffset,
                         &srcfilhdr,
                         sizeof ( SRCFILHDR )
                       )
               )
               return ( FALSE );

            /* Loop through the segments in the source file.
            */

            lfaSrcSegments = lfaSrcFiles + sizeof ( SRCFILHDR );

            for ( wSegmentIdx = 0; wSegmentIdx < srcfilhdr.wSegments; ++wSegmentIdx )
            {
               /* Read the next source line header.
               */

               if ( !XRead( fhExe,
                            lfaSrcSegments + wSegmentIdx * sizeof ( DWORD ),
                            &dwSrcLinHdrOffset,
                            sizeof ( DWORD )
                          )
                  )
                  return ( FALSE );

               if ( !XRead( fhExe,
                            lfaTable + dwSrcLinHdrOffset,
                            &srclinhdr,
                            sizeof ( SRCLINHDR )
                          )
                  )
                  return ( FALSE );

               /* Skip if the segment doesn't match.
               */

               if ( srclinhdr.wSegment != wSegment )
                  continue;

               /* Loop through the source lines in the segment.
               */

               lfaSrcLines = lfaSrcSegments + sizeof ( SRCLINHDR );

               for ( wLineIdx = 0; wLineIdx < srclinhdr.wLinePairs; ++wLineIdx )
               {
                  /* Read the next IP offset.
                  */

                  if ( !XRead( fhExe,
                               lfaSrcLines + wLineIdx * sizeof ( DWORD ),
                               &dwIP,
                               sizeof ( DWORD )
                             )
                     )
                     return ( FALSE );

                  /* Skip if the IP's don't match.
                  */

                  if ( dwIP != wIP )
                     continue;

                  /* We found the line!  So store the information requested
                  ** by the calling routine.
                  */

                  if ( !XRead( fhExe,
                               lfaSrcLines 
                                + srclinhdr.wLinePairs * sizeof ( DWORD )
                                + wLineIdx * sizeof ( WORD ),
                               lpiSourceLine,
                               sizeof ( DWORD )
                             )
                     )
                     return ( FALSE );

                  lfaSrcFileStr = srcfilhdr.wSegments * sizeof ( DWORD )
                                   + srcfilhdr.wSegments 
                                      * ( sizeof ( DWORD ) + sizeof ( DWORD ) );

                  if ( !XRead( fhExe,
                               lfaSrcFileStr,
                               &wNameLen,
                               sizeof ( WORD )
                             )
                     )
                     return ( FALSE );

                  if ( wNameLen > wSourceFileLen - 1 )
                     wNameLen = sSourceFileLen - 1;

                  if ( !XRead( fhExe,
                               lfaSrcFileStr + sizeof ( WORD ),
                               lpszSourceFile,
                               wNameLen
                             )
                     )
                     return ( FALSE );

                  lpszSourceFile[ wNameLen ] = '\0';

                  /* Return success.
                  */

                  return ( TRUE );
                  
               } /* for ( wLineIdx ... */

            } /* for ( wSegmentIdx ... */

         } /* for ( wSrcFileIdx ... */

      } /* for ( dwDirEntryIdx ... */

   } /* while ( lfoDir ... */

   /* Return failure.
   */

   return ( FALSE );
   
}

/******************************************************************************

   Function Name: XRead

   Description:

      Executable reading function.

   Parameters:

      fhExe                ==>   Executable file handle.

      lOffset              ==>   Offset into executable file.  
                                 If EWM_XREADCURRENT then continue reading
                                 from the current file position.

      lpvBuffer            <==   Pointer to the buffer to receive the data.

      dwBytesToRead        <==   Number of bytes to read.

   Return Values:

      Success: TRUE
      Failure: FALSE

   Revisions:

   Notes:

******************************************************************************/

BOOLEAN XRead(

   int fhExe,
   long lOffset,
   LPVOID lpvBuffer,
   DWORD dwBytesToRead

   )

{
   /* Variable definition.
   */

   /* Seek to the position indicated.
   */

   if ( lOffset != EWM_XREADCURRENT )
   {
      if ( _llseek( fhExe, lOffset, SEEK_SET ) == -1L )
      {
         DEBUGMSG( "TDBEXEWK: Invalid seek in executable performed" );
         return ( FALSE );
      }
   }

   /* Read the bytes indicated.
   */

   if ( _hread( fhExe, lpvBuffer, dwBytesToRead ) != dwBytesToRead )
   {
      DEBUGMSG( "TDBEXEWK: Couldn't read executable file" );
      return ( FALSE );
   }

   /* Return success.
   */

   return ( TRUE );

}   
