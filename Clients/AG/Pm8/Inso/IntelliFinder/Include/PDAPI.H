/***************************************************************************
*                   Copyright 1995 INSO Corporation                        *
*                Personal Dictionary Component Software                    *
*                                                                          *
* This work is protected by United States and international copyright laws *
* and contains certain trade secret information.  All use of this work is  *
* governed by the terms of agreements between INSO Corporation and its     *
* licensees or potential licensees.  Disclosure or disassembly of the      *
* contents of embodied programs or algorithms is prohibited.  All rights   *
* are reserved worldwide.                                                  *
****************************************************************************
 
*-----------------------------------------------------------------------*
*                                                                       *
*        PDapi.h - Personal Dictonary Defines                           *
*                                                                       *
*-----------------------------------------------------------------------*
*                                                                       *
        Description:
                
            This is where all of the API IO structures reside

        Cautions:
        None.

        Creation Date:
        01-Jul-93   DLB     Ver. 1.0.0

*-----------------------------------------------------------------------*/





typedef struct PD_app_io
{
    UCHAR    byProdID;         /* Applications AIM */ 
    UCHAR    byProdSubID;      /* Applications Sub AIM  */     
    UINT2B    wPID;             /* Applications PID  */     
    SL_STATUS xStatus;         /* Status Returns */          

}     PD_APP_IO,
    *PPD_APP_IO,
    FAR *PFPD_APP_IO,
    NEAR * PNPD_APP_IO;

    

typedef struct PD_db_io
{

    UCHAR       sOpenFilename[PD_MAXFILENAME];
    SLPATH      xOpenPath;
    INT2B       nFileMode;
    SL_FHANDLE  theFile; 
    UINT4B      dwOpenFileOffset;
    UINT4B      dwOpenFileLength;
    CHAR        cOtype;
    CHAR        cCtype;
    UCHAR       sCloseFilename[PD_MAXFILENAME];
    SLPATH      xClosePath;
    SLPATH      xTmpPath;
    UINT2B      wPID;
    UINT2B      wPrecedenceNumber;
    UINT2B      wDBID;
    FLAG        bLockedFlag;
    UCHAR       uPDType;
    UCHAR       uEBLength;
    UINT2B      wDID;
    UINT2B      wDictSize;
    UCHAR       uDictlang;
    UCHAR       uSoftHypChar;   
    UINT2B      wNumNegWords;                   
    SL_STATUS   xStatus;

}  PD_DB_IO,
   *PPD_DB_IO,
   FAR *PFPD_DB_IO;





typedef struct PD_rets
{

   UCHAR    sRetWord[PDWRDMAX+1];
   UINT2B   wDID;
   UCHAR    uProdID;         /* Applications AIM */ 
   UCHAR    uProdSubID;      /* Applications Sub AIM  */     
   FLAG     bNegFlag;
   UCHAR    uCapCode;
   UCHAR    uDictLang;
   UINT2B   wDBID;
   UINT2B   wPrecedenceNumber;
   SF_MAP   mCapMap;
   SF_MAP   mHypMap;
   UINT4B   wAltIdNum;
   UINT2B   wAltBuffSize;
   UCHAR    aExtraBytes[PD_EXTRA_BYTES_MAX];

}  PD_RET,
   *PPD_RET,
   NEAR *PNPD_RET,
   FAR *PFPD_RET;





typedef struct PD_edit_io
{
   UINT2B       wDID;
   UINT2B       wPID;
   PFUCHAR      sWord;
   UCHAR        uSoftHypChar;  /* Input soft hyphen character */
   FLAG         bNegWordFlag;
   UCHAR        uCapCode;
   SF_MAP       mCapMap;
   SF_MAP       mHypMap;
   PFUCHAR      pAltBuff;
   UINT2B       wAltBuffSize;
   UCHAR        aExtraBytes[PD_EXTRA_BYTES_MAX];
   UCHAR        uPercentFull;
   SL_STATUS    xStatus;

}  PD_EDIT_IO,
   *PPD_EDIT_IO,
   FAR *PFPD_EDIT_IO;


typedef struct PD_editor_io
{
   UINT2B      wDID;
   UINT2B      wPID;
   PFUCHAR     sWord;
   PFUCHAR     pWordInfo;
   UINT2B      wWordInfoSize;
   UCHAR       byPercentFull;
   SL_STATUS   xStatus;

}  PD_EDITOR_IO,
   *PPD_EDITOR_IO,
   FAR *PFPD_EDITOR_IO;

typedef struct PDWORD_INFO
{
    UINT2B      wCliticSiz;
    PFSF_CLITIC pClitic;    /* Pointer to clitic structure              */
    PFUCHAR     pStrTab;
    UINT2B      wStrSize;
    PFUCHAR     pStpTab;
    UINT2B      wStpSize;
    UINT2B      wMinAnaSize; /* Minimum Anagram Size */
}PDWORD_INFO;

typedef struct PD_word_io
{
    UINT2B      wDID;
    UINT2B      wPID;
    PFUCHAR     sWord;
    UCHAR       uSoftHypChar; /* Input soft hyphen character */
    UCHAR       uCapCode;
    SF_MAP      mCapMap;
    SF_MAP      mHypMap;
    UINT4B      mLangFilter;
    UCHAR       uProdFilter;
    UCHAR       uProdSubFilter;
    UCHAR       uNegFilter;
    UCHAR       uPDTypeFilter;
    UINT2B      wDBFilter;
    UINT2B      wPrecedenceFilter;
    UINT2B      wMaxRets;
    UINT2B      wNumRets;
    PDWORD_INFO eWordInfo;
    PFPD_RET    FAR *pReturns; /* An array of FAR PD_RETURN */
    SL_STATUS   xStatus;

}  PD_WORD_IO,
   *PPD_WORD_IO,
   FAR *PFPD_WORD_IO;

typedef struct PD_comp_io
{
    UCHAR       sInputFile1[PD_MAXFILENAME];
    SLPATH      xInputPath1;
    UINT4B      dwInputOffset1;
    UINT4B      dwInputLength1;
    UCHAR       uInputFile1Type;
    UCHAR       sInputFile2[PD_MAXFILENAME];
    SLPATH      xInputPath2;
    UINT4B      dwInputOffset2;
    UINT4B      dwInputLength2;
    UCHAR       uInputFile2Type;
    UCHAR       sOutputFile[PD_MAXFILENAME];
    SLPATH      xOutputPath;
    UCHAR       uOutputFileType;
    UCHAR       sLogFile[PD_MAXFILENAME];
    UCHAR       uLanguage;
    UCHAR       uSoftHypChar;
    UINT2B      wBlockSize;                     /* size of sectors */
    UINT2B      wPID;
    SL_STATUS   xStatus;
}  PD_COMP_IO,
   *PPD_COMP_IO,
   FAR *PFPD_COMP_IO;


typedef struct PD_alt_io
{
   UINT2B      wDID;
   UINT2B      wPID;
   UINT4B      wAltIdNum;
   PFUCHAR     pAltBuff;
   UINT2B      wAltBuffSize;
   SL_STATUS   xStatus;

}  PD_ALT_IO,
   *PPD_ALT_IO,
   FAR *PFPD_ALT_IO;

RETCODE SLAPI_PASCAL PDcomp(PFPD_COMP_IO pCompIO);

RETCODE SLAPI_PASCAL PDedit (PFPD_EDIT_IO pEditIo, UINT2B nPrimOp);
RETCODE SLAPI_PASCAL PDword (PFPD_WORD_IO pWordIo, UINT2B nPrimOp, UINT2B nScndOp);
RETCODE SLAPI_PASCAL PDalt (PFPD_ALT_IO pAltIo);
RETCODE SLAPI_PASCAL PDdb (PFPD_DB_IO pDbIo, UINT2B nPrimOp);
RETCODE SLAPI_PASCAL FAR PDapp (PFPD_APP_IO pAppIO, UINT2B wPrimOp);

RETCODE SLAPI_PASCAL PDeditor (PFPD_EDITOR_IO pmEditIo, UCHAR nPrimOp);
FLAG SLAPI_PASCAL PDchknegs(VOID);



/* End of File - PDapi.h
*/
