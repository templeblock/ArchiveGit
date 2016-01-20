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
 * REPDDB.h - Defines for the Reference Engine PD support.          *
 *                                                                          *
 *--------------------------------------------------------------------------*


    Description:

    Creation Date:
    2-jan-95   LRS

    Modification History:
*---------------------------------------------------------------------------*/

#ifndef REPDDB_H

#define REPDDB_H
#ifdef RE_SF_PDICTIONARY  

#define RE_MAX_FILENAME SLFNAMEMAX+1
#define RE_MAX_FILEPATH SLPATHMAX
#define RE_MAX_PATHDELIM 1
#define RE_MAX_FULLPATH (RE_MAX_FILENAME + RE_MAX_PATHDELIM + RE_MAX_FILEPATH)

#define RE_PATH_DELIMITER '\\'

#define RE_BUFFSIZE 1024

#define MAXINT2B 32767

#define RE_MAX_PD_HDRSIZ 1024

#define REPD_EXISTS 1
#define REPD_CREATE 2

#define REPD_RW 1
#define REPD_RO 2

#define REPD_FULLSAVE  1
#define REPD_QUICKSAVE 2    

/* Maximum number of STNs
*/   
#define REPD_MAX_STN (1024L * 1024L) 

#define REPD_MAX_ALTBUF PDALTBUFSIZE


/* Extra byte manipulation
*/
#define REPD_POS_OFFSET (0)
#define REPD_STN_OFFSET (1)

#define REPD_SET_XTRA_POS(_xb, _pos) \
    _xb[REPD_POS_OFFSET] = _pos;

#define REPD_GET_XTRA_POS(_xb)  _xb[REPD_POS_OFFSET]

#define REPD_SET_XTRA_STN(_xb, _stn) \
    SFHHEAD4_MAKE( _xb, REPD_STN_OFFSET, _stn);

#define REPD_GET_XTRA_STN(_xb) SFHHEAD4( _xb, REPD_STN_OFFSET)

#define RE_INDEXSUBPD 1
#define RE_TEXTSUBPD  2

#define RE_SD_SUBPD 1
#define RE_RD_SUBPD  2

#define REPD_FILL_OEMIO      1

#define REPD_FILL_HDR       1
#define REPD_FILL_CTRL      2 

#define REPD_HDR_IN         1
#define REPD_HDR_OUT        2

#define REPD_HEADER_ID_PATT "I#3^dF7!"
#define REPD_HEADER_ID_PATT_SIZ sizeof(REPD_HEADER_ID_PATT)

/* the header structure for an IF PD */
/***********************************************************************
*
The PD Header
The following is the internal header structure for IF PDs.  It's fields are written to the beginning of the PD 
disk file, followed by the subpd components.
*
***********************************************************************/

typedef struct _REPDHDR
{
    /* The string to identify this file.
    */
    UCHAR   sFilePattern[REPD_HEADER_ID_PATT_SIZ];
    /*Creation Fields -- Set by OEM at PD creation time, then autofilled in IO STRUCT.
    */
    UCHAR   sCreatorName[REPD_MAXWORD+4];   /* field for user to self identify*/
    UCHAR   sCreationDate[REMAXDATE];       /* dd-mm-yyyy            */
    UCHAR   sPdName[REPD_MAXWORD+4];        /* field for OEM to uniquely identify each PD */
    UCHAR   sPdDisplayName[REPD_MAXWORD+4]; /* the name to display in menus in the UI*/
    UINT2B  wIfModelNum;                    /* from target IF DB (almanac, dictionary, etc.)*/
    INT4B   lIfSerialNum;                   /* from target IF DB (for future reference)*/
    INT2B   nIfLanguage;                    /* the language of the IF DB    */
    UINT2B  wSaveStatus;                    /* whether the data loaded was the result of REPD_QUICKSAVE */
    UINT4B  dwOffsetRDIndex;                /* offset to subpd              */
    UINT4B  dwSizeRDIndex;                  /* size of subpd                */
    INT4B   lTotEntriesRDIndex;             /* rd index subpd               */
    UINT4B  dwOffsetRDText;                 /* offset to subpd              */
    UINT4B  dwSizeRDText;                   /* size of subpd                */
    INT4B   lTotEntriesRDText;              /* rd text subpd                */
    UINT4B  dwOffsetSDIndex;                /* offset to subpd              */
    UINT4B  dwSizeSDIndex;                  /* size of subpd                */
    INT4B   lTotEntriesSDIndex;             /* sd index subpd               */
    UINT4B  dwOffsetSDText;                 /* offset to subpd              */
    UINT4B  dwSizeSDText;                   /* size of subpd                */
    INT4B   lTotEntriesSDText;              /* sd text subpd                */  
    UINT2B  wMaxWordLen;
    UINT2B  wMaxLineLen;
    UINT2B  wMaxEntWordLen;
    UINT4B  dwMaxRetEnt;  /* the longest entry text */

    /* field to note the date of last edit READ-WRITE only
    */
    UCHAR sEditDate[REMAXDATE]; /*dd-mm-yyyy*/

    /* field to keep track of edits, READ-WRITE only.
    */
    UINT2B wVersionNumber;
}
REPDHDR,
FAR *PFREPDHDR;

/* the following move file header fields to a congruent structure */
#define RE_HDRSTR_TO_STRUCT(_var) \
    Cfstrncpy(_var, pBuf, sizeof( _var )); pBuf += sizeof( _var );
#define RE_HDR2B_TO_STRUCT(_var) \
    _var = SFHHEAD2( pBuf, 0 ); pBuf += sizeof( _var );
#define RE_HDR4B_TO_STRUCT(_var) \
    _var = SFHHEAD4( pBuf, 0 ); pBuf += sizeof( _var );

/* the following move the struct to file header fields */
#define RE_STRUCT_TO_HDRSTR(_var) \
    Cfstrncpy(pBuf, _var, sizeof( _var )); pBuf += sizeof( _var );
#define RE_STRUCT_TO_HDR2B(_var) \
    SFHHEAD2_MAKE( pBuf, 0, _var); pBuf += sizeof( _var );
#define RE_STRUCT_TO_HDR4B(_var) \
    SFHHEAD4_MAKE( pBuf, 0, _var); pBuf += sizeof( _var );

/* the foll control conversion between IF ST numbers and local PD ST numbers */
#define REPD_CONVERT_IFTOPD       1
#define REPD_CONVERT_PDTOIF       2

/* convert a pd stn plus pdid to an if stn or vice versa */
#define REPD_GETGLOBALSTN(_pdid, _localSTN) \
                            ((((INT4B) _pdid) * REPD_MAX_STN) + _localSTN)
#define REPD_GETGLOBALSTARTRANGE(_pdid) \
                            (_pdid * REPD_MAX_STN)
#define REPD_GETLOCALPDID(_globalSTN) \
                            (_globalSTN / REPD_MAX_STN)
#define REPD_GETLOCALSTN(_globalSTN) \
                            (_globalSTN % REPD_MAX_STN)
#define REPD_ISIFSTN(_globalSTN) \
                            (_globalSTN < REPD_MAX_STN)
#define REPD_PDTOSTORE(_localSTN) \
                            (_localSTN + REPD_MAX_STN)
#define REPD_PDFROMSTORE(_localSTN) \
                            (_localSTN - REPD_MAX_STN)
                            
#define REPD_GRP        "<GRP>"
#define REPD_REP        "<REP>"
#define REPD_POS        "<POS>"
#define REPD_G2     "<G2>"
#define REPD_GT     "<GT>"
#define REPD_GTN        "<GTN>"
#define REPD_GI     "<GI>"
#define REPD_SYN        "<SYN>"
#define REPD_S2     "<S2>"
#define REPD_STN        "<STN>"
#define REPD_CR     "<CR>" 

/* the prefix of the following must be constant */
#define REPD_HEADER_SYM  "<H/"
#define REPD_ADD_GROUP_SYM  "<H/AG> "
#define REPD_DEL_GROUP_SYM  "<H/DG> "
#define REPD_ADD_SYNS_SYM   "<H/AS> "
#define REPD_DEL_SYNS_SYM   "<H/DS> "
#define REPD_MOD_GROUP_SYM  "<H/MG> "
/* keep the following same size */
#define REPD_LONGEST_GROUP_SYM REPD_MOD_GROUP_SYM 

/*
#define REPD_PROC_MNGS      1
#define REPD_PROC_SYNS      2
*/

/* MODIFY MEANINGS
*/
#define REPD_ADD_MNG        1
#define REPD_DEL_MNG        2
#define REPD_ADD_G2         3
#define REPD_ADD_GT         4
#define REPD_ADD_GTN        5
#define REPD_ADD_GI         6
#define REPD_ADD_REP        7

/* MODIFY SYNONYMS
*/
#define REPD_ADD_S2         8
#define REPD_ADD_STN        9
#define REPD_ADD_CR         10

#define REPD_HWD_DELIMITER "<HWD>"
#define REPD_POS_DELIMITER "<HPOS>"

/* Caution: the following must not overlap RE_ET_MOD_GROUP
*/
#define RE_ET_HEADWORD      1111
#define RE_ET_POS           1112
#define RE_ET_SKIP          1113

/* Header function prototypes
*/
FLAG FAR PASCAL    REPDMhdr               (PFREPDHDR, SL_FHANDLE, FLAG);
FLAG FAR PASCAL    TransferPDioStruct     (PFREPD_DB_IO, PFPD_CTRL_RE, PFVPD_CTRL_RE, INT2B);
FLAG FAR PASCAL    TransferPDctrlStruct   (PFREPD_DB_IO, PFREPDHDR, PFPD_CTRL_RE, PFVPD_CTRL_RE,
                                           INT2B);

#endif /* RE_SF_PDICTIONARY */

#endif /* REPDDB_H */

/* End of file - REPDDB.h */
