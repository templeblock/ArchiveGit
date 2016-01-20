/****************************************************************************
 *                     Copyright 1995 INSO Corporation                      *
 *                    IntelliFinder(TM) Reference Engine                    *
 *                                                                          *
 * This work is protected by United States and international copyright laws *
 * and contains certain trade secret information.  All use of this work is  *
 * governed by the terms of agreements between INSO Corporation and its     *
 * licensees or potential licensees.  Disclosure or disassembly of the      *
 * contents of embodied programs or algorithms is prohibited.  All rights   *
 * are reserved worldwide.                                                  *
 ****************************************************************************/

/*--------------------------------------------------------------------------*
 *                                                                          *
 * REPDCTRL.h - Defines for the Reference Engine PD internal control structures.          *
 *                                                                          *
 *--------------------------------------------------------------------------*


    Description:

    Creation Date:
    2-jan-95   LRS

    Modification History:
*---------------------------------------------------------------------------*/

#ifndef REPDCTRL_H

#define REPDCTRL_H

#ifdef RE_SF_PDICTIONARY  /* LRSV23 */

/* the real control structure for an IF PD */
/***********************************************************************
*
*  The following structure contains all PD db file related data.  The
*  data in this structure, or the strucutures below it, pertains to
*  real pds and is unaffected by the manipulations on any virtual
*  pd.
* It is attached to the real pd file element 
* in the list maintained by the IF PD Database Manager.
***********************************************************************/

typedef struct _DM_pd_ctrl
{
    INT2B       nPDAPPID;
    INT2B       nIFDBID;                            /* the parent IF DB */
    UCHAR       sPdOpenFileName[REMAXFILENAME];     /* the (input) disk filename */
    SLPATH      xPdOpenPath;                        /* the disk file location*/
    UCHAR       sPdCloseFileName[REMAXFILENAME];    /* the (output) disk filename */
    SLPATH      xPdClosePath;                       /* the disk file location*/
    SLPATH      xTempPath;                          /* disk location for general temp files */
    UCHAR       sIndexRDTmpFileName[REMAXFILENAME]; /* temp disk filename */
    UCHAR       sTextRDTmpFileName[REMAXFILENAME];  /* temp disk filename */
    UCHAR       sIndexSDTmpFileName[REMAXFILENAME]; /* temp disk filename */
    UCHAR       sTextSDTmpFileName[REMAXFILENAME];  /* temp disk filename */

    UCHAR       sEditDate[REMAXDATE];               /*dd-mm-yyyy*/
    UINT2B      wVersionNumber;

    UCHAR       sCreatorName[REPD_MAXWORD+4];       /* field for user to self identify*/
    UCHAR       sCreationDate[REMAXDATE];           /* dd-mm-yyyy*/
    UCHAR       sPdName[REPD_MAXWORD+4];            /* field for OEM to uniquely identify each PD */
    UCHAR       sPdDisplayName[REPD_MAXWORD+4];     /* the name to display in menus in the UI*/
    UINT2B      wIfModelNum;                        /* from target IF DB (almanac, dictionary, etc.)*/
    INT4B       lIfSerialNum;                       /* from target IF DB (for future reference)*/
    INT2B       nIfLanguage ;                       /* the language of the IF DB*/ 
    UINT2B      mAvailDialect;                      /* copied from if db */    

    INT2B       nPDID;                              /* the database id of this pd */
    SL_FHANDLE  fd;                                 /* the open file handle for an existing PD*/
    UINT2B      wAccessMode;                        /* copied from user input parameters */
                /* read, write access and whether new creation              */
                /* RE_RO, RE_RW, RE_EXISTS, RE_CREATE                       */
                /* The default case of opening an existing file read-only will be 0.*/
    UINT2B      wSaveStatus;                        /* whether the data loaded was the result of REPD_STOP_QUICKSAVE */
    INT4B       lTotEntriesSDIndex;                 /* sd index subpd */
    INT4B       lTotEntriesSDText;                  /* sd text subpd */
    PFRE_STATUS pStatus;

    /* The following are reserved for future use :
    */
    /* INT2B       nRefType;      *//* The type of reference work (gen.org) */
    /* UCHAR       mAttrib[4];    *//* DB Attribute Bitmaps. type dependant */
    /* INT2B       nIdxEntMap;    *//* The entry naming type.               */
    /* INT2B       nLanguage;     *//* Language of this reference work      */
    /* UINT2B      mAvailDialect; *//* Available dialects for this ref. work*/
}
PD_CTRL_RE,
FAR *PFPD_CTRL_RE;

/***********************************************************************
 *
 * the SUBcontrol structure for RD/SD combinations in an IF PD
 *
 ***********************************************************************
*/
      /* dynamic subpd */

typedef struct RE_dynaPD
{
    INT4B   lOffsetRD;              /* offset in existing PD        */
    INT4B   lSizeRD;                /* size in existing PD          */
    UINT2B  wPrecedenceRD;

    INT2B   nRDid;                  /* NULL if no RD                */
    UINT2B  wOpenMode;              /* readonly or readwrite        */
    FLAG    bNewData;               /* data added since last save   */
    
    /* static subpd always readonly
    */
    INT4B   lOffsetSD;
    INT4B   lSizeSD;
    UINT2B  wPrecedenceSD;
    INT2B   nSDid;                  /* NULL if no SD */ 
}
DYNAPD_RE,
FAR *PFDYNAPD_RE;


/* the virtual control structure for an IF PD */
/*------------------------------------------------------------------------
 * VPD_CTRL - The following structure is the main control structure for
 *            an INSTANCE of a pd.  In it, or below it, all instance
 *            related data from the predecary structures is located.
 * It is attached to the virtual pd element 
 * in the list maintained by the IF PD Database Manager.
 *------------------------------------------------------------------------
*/

typedef struct _RE_vpd_ctrl
{
    INT2B       nPDID;              /* The database id of this pd           */
    UINT2B      wIFDBID;            /* The parent IF DB                     */
    UINT2B      wPrecedence;        /* Override order among attached PDs, from order of attachment calls*/
    UINT4B      dwNextFreeEntryNo;  /* Entry number for next addition       */
    DYNAPD_RE   eNdxSubPD;          /* The index sub-pd definition structure*/
    DYNAPD_RE   eTxtSubPD;          /* The text sub-pd definition structure */
    INT4B       lTotEntriesRDIndex; /* RD index subpd                       */
    INT4B       lTotEntriesRDText;  /* RD text subpd                        */
    UINT2B      wMaxWordLen;
    UINT2B      wMaxLineLen;
    UINT2B      wMaxEntWordLen;
    UINT4B      dwMaxRetEnt;
    UCHAR       uPercentFull;       /* The percent full information from the PD */
}
VPD_CTRL_RE,
FAR *PFVPD_CTRL_RE;

#endif /* RE_SF_PDICTIONARY */ /* LRSV23 */

#endif /* REPDCTRL_H */

/* End of file - REPDCTRL.h */
