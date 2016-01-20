/*----------------------------------------------------------------------*
*                                                                       *
*                   Copyright 1995 INSO Corporation                     *
*                         Common Code Library                           *
*                                                                       *
* This work is protected by United States and international copyright   *
* laws and contains certain trade secret information.  All use of this  *
* work is governed by the terms of agreements between INSO Corporation  *
* and its licensees or potential licensees.  Disclosure or disassembly  *
* of the contents of embodied programs or algorithms is prohibited.     *
* All rights are reserved worldwide.                                    *
*                                                                       *
*-----------------------------------------------------------------------*

*-----------------------------------------------------------------------*
*                                                                       *
*    SLerror.h - Error defines for InfoSoft International, Inc.         *
*                                                                       *
*-----------------------------------------------------------------------*


    Description:

    This file contains #defines for error return codes.


*-----------------------------------------------------------------------*/
#ifndef SLERROR_H

#define SLERROR_H

#ifdef SL_ERR_V351  /* 'old' style of error reporting */

/*    System level specific Error return codes               */

#define ENDFILE      900    /* end of file condition         */
#define FILENOTFOUND 901    /* file requested not found      */
#define PATHNOTFOUND 902    /* path requested not found      */
#define ACCESSDENIED 903    /* requested file access denied  */
#define FILEOPEN     904    /* file is already open          */
#define BADFILENAME  905    /* invalid file name syntax      */
#define LINETOOLONG  906    /* line read too long for buffer */
#define READPASTEND  907    /* attempt to read past eof      */
#define READERR      908    /* error reading file            */
#define WRITERR      909    /* error writing file            */
#define CLOSERR      910    /* error closing file            */
#define SEEKERR      911    /* error seeking in file         */
#define OUTOFMEMORY  912    /* out of memory                 */
#define DALLOCERR    913    /* deallocation error            */
#define TOOMANYFILES 914    /* too many files open           */
#define ENTRIESFULL  915    /* too many file info structs    */
#define ERRALLOC     916    /* error allocating error msg    */
#define NOTALPHA     917    /* no alpha chars in string      */
#define FRERR        918    /* error freeing memory          */

#else /* !SL_ERR_V351 */

/***************************************************************************

Error codes are to be separated sequentially by product in groups of 1000.  
The SL product group contains error codes that are SL specific as well as 
standard error codes that can be shared by all products.

        SL      1000-1999       IPR     6000-6999
        SF      2000-2999       GCS     7000-7999
        ICS     3000-3999       IF      8000-8999
        IH      4000-4999       II      9000-9999
        PD      5000-5999       etc.    10000-10999

****************************************************************************/

/* file I/O errors */
#define SLREAD_ERR     1010
#define SLWRITE_ERR    1011
#define SLOPEN_ERR     1012
#define SLCLOSE_ERR    1013
#define SLCREATE_ERR   1014
#define SLSEEK_ERR     1015
#define SLFPOS_ERR     1016
#define SLFNAME_ERR    1017 /* file name error */
#define SLPNAME_ERR    1018 /* path name error */
#define SLTMPFNAME_ERR 1019 /* error creating a temporary file name */
#define SLRENAME_ERR   1020 
#define SLREMOVE_ERR   1021 

/* memory errors */
#define SLALLOC_ERR    1030
#define SLFREE_ERR     1031
#define SLLOCK_ERR     1032
#define SLUNLOCK_ERR   1033

/* DLL errors */
#define SLDLLLD_ERR    1040  /* DLL load error */

/* App errors */
#define SLAPPINIT_ERR  1060
#define SLAPPTERM_ERR  1061
#define SLINVAPPID_ERR 1062  /* invalid app id */

/* DB errors */
#define SLDBINIT_ERR   1080
#define SLDBTERM_ERR   1081
#define SLINVDBID_ERR  1082  /* invalid db id */

/* Proc errors */
#define SLPRIMOP_ERR    1100  /* invalid primary operator       */
#define SLSCNDOP_ERR    1101  /* invalid secondary operator     */
#define SLNULLPTR_ERR   1102
#define SLVERSION_ERR   1103  /* Database/code version mismatch */

/* language errors */
#define SLLANG_ERR     1120
#define SLDIALECT_ERR  1121
#define SLMODE_ERR     1122

/* Input data errors */
#define SLBUFF_ERR     1140  /* input buffer error  */
#define SLTEXT_ERR     1141  /* input text error    */
#define SLRDPSTEOF_ERR 1142  /* read past EOF error */



typedef struct _SL_STATUS
{   
  RETCODE      nRetCode;  /* Severity level and return code of the function  */
  UINT2B       wError;    /* Code of the warning, app error, or sys error    */
  UINT4B       dwErrInfo; /* Further product level errors                    */
  VOID SLF_PTR pExtra;    /* Void pointer to additional product information  */
  UINT2B       wLine;     /* Line # of the source file in which err occurred */
  PFUCHAR      pFile;     /* Name of the source file in which err occurred   */
} SL_STATUS, SLF_PTR PFSL_STATUS;


#ifdef SL_FULLSTAT_DBG
#define _SLSETSTATUS(pSTATUS, nRETCODE, wERROR, dwERRINFO) {  \
                                                              \
    if (!(pSTATUS)->wError || (pSTATUS)->nRetCode < nRETCODE) \
    {                                                         \
        /* either there is no previous error set    */        \
        /* OR retcode level is higher than previous */        \
                                                              \
        (pSTATUS)->nRetCode  = (nRETCODE);                    \
        (pSTATUS)->wError    = (wERROR);                      \
        (pSTATUS)->dwErrInfo = (dwERRINFO);                   \
        (pSTATUS)->wLine     = __LINE__;                      \
        (pSTATUS)->pFile     = __FILE__;                      \
    }                                                         \
    /* else preserve last status setting */                   \
                                                              \
}   /* End of Macro _SLSETSTATUS */ 

#else

#define _SLSETSTATUS(pSTATUS, nRETCODE, wERROR, dwERRINFO) {  \
                                                              \
    if (!(pSTATUS)->wError || (pSTATUS)->nRetCode < nRETCODE) \
    {                                                         \
        /* either there is no previous error set    */        \
        /* OR retcode level is higher than previous */        \
                                                              \
        (pSTATUS)->nRetCode  = (nRETCODE);                    \
        (pSTATUS)->wError    = (wERROR);                      \
    }                                                         \
    /* else preserve last status setting */                   \
                                                              \
}   /* End of Macro _SLSETSTATUS */ 
#endif                                             

#define _SLCLEARSTATUS(pSTATUS) {    \
                                     \
    (pSTATUS)->nRetCode  = 0;        \
    (pSTATUS)->wError    = 0;        \
    (pSTATUS)->dwErrInfo = 0;        \
    (pSTATUS)->wLine     = 0;        \
    (pSTATUS)->pFile     = NULLFP;   \
                                     \
} /* End of Macro _SLCLEARSTATUS */ 

#define SLSETSTATUS(pSTATUS,    \
                    nRETCODE,   \
                    wERROR,     \
                    dwERRINFO)  _SLSETSTATUS(pSTATUS, nRETCODE, wERROR, dwERRINFO)
#define SLCLEARSTATUS(pSTATUS)  _SLCLEARSTATUS(pSTATUS)

#define SL_RETCODE_STATUS(pSTATUS)       (pSTATUS)->nRetCode
#define SL_ERROR_STATUS(pSTATUS)         (pSTATUS)->wError
#define SL_ERRINFO_STATUS(pSTATUS)       (pSTATUS)->dwErrInfo

#define SL_EXTRA_STATUS(pSTATUS)         (pSTATUS)->pExtra 

#define SL_LINENUM_STATUS(pSTATUS)       (pSTATUS)->wLine
#define SL_FILENAME_STATUS(pSTATUS)      (pSTATUS)->pFile


#endif

#endif /* SLERROR_H */

/*    End of file - SLerror.h    */
