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
****************************************************************************/

#ifndef PDLIBHDR_H
#define PDLIBHDR_H
#include "pddefine.h"
#include "pdapi.h"
#include "pdctrl.h"
#include "pdmcp.h"

#ifdef SF_PD

SLEXTERN PFPDMCPBUFF pMCPbuff;
SLEXTERN FLAG pdmcpsta;
RETCODE PDget (PFUCHAR letter, INT2B len, PFPDIBUFF PDibuff);
RETCODE PDgetword (PFPDIBUFF PDibuff);

INT2B   PDcmp(PFUCHAR s, PFUCHAR t, UCHAR soft_hyp_char, PFUINT4B att_tab);
RETCODE PDicget (SWORD  mstr_fd, PUCHAR mstr_hdr, PFPD_WORD_IO pWordIo, FLAG init);
RETCODE PDcheckDID (UINT2B wDID);
RETCODE PDautodel(PFPDIBUFF pusr, INT2B len, UINT2B wDID);
RETCODE PDdel(PFPDIBUFF pusr, UINT2B index);
RETCODE PDinit(VOID);
RETCODE PDRDinit(PFPDIBUFF pusr);
RETCODE PDsearch(PFPDIBUFF pusr, PFUCHAR word, PFUINT2B count,
                 PFUINT2B hole, UCHAR task, PFPD_WORD_IO pWordIo, PFUCHAR extraBytes);
UCHAR   PDhypstrip(PFUCHAR word, PFSF_MAP hyphens, UCHAR soft_hyp_char);
UCHAR   PDhypins(PFUCHAR word, PFSF_MAP hyphens, UCHAR soft_hyp_char);
RETCODE PDsavsort(UINT2B wDID, FLAG save);
RETCODE PDExtSort (SL_FHANDLE xIn1Fd, UCHAR uLead1, SL_FHANDLE xIn2Fd, 
                   UCHAR uLead2, SL_FHANDLE xTmpFd, PF_SLPATH pPath, 
                   UCHAR uConvert, 
                   PFUINT4B att_tab, UCHAR lang, UCHAR uTypeBits);

RETCODE PDGets(PFUCHAR pStr, INT4B lMax, PFINT4B pLastRead, SL_FHANDLE xFd);


RETCODE PDdefragalt(UINT2B wDID);
VOID PDterm(VOID);

RETCODE PDngrams(PF_SLPATH inp_path, PFUCHAR inp_file, 
                 PF_SLPATH out_path, PFUCHAR out_file);

RETCODE PDSDinit (PFPDIBUFF pUsr, INT2B nFileMode, SL_FHANDLE xTheFile);
PFBIT_BUFF OpenOutputBuffer(UINT2B block_size);
INT2B build_tree( PFNODE nodes );
RETCODE   PDreadHead(PFUCHAR header, PFPD_HEADER_INFO theHeader);


#ifdef PD_ALTWORD_SUPPORT
RETCODE PDdelalt(INT2B alt_entry,UINT2B wDID);
RETCODE PDaltentry(PFINT2B alt_size,INT2B alt_entry,UINT2B wDID);
RETCODE PDgetalt(PFUCHAR alt_buff,UINT2B alt_size,UINT4B alt_entry,UINT2B wDID);
RETCODE PDdelalst(PFPD_DB_IO pDbIo);
RETCODE make_alt_entry (PFUCHAR alt, PFUINT4B offset, PFINT2B size, UINT2B wDID);
VOID alt_conv (PFINT2B entry,PFUCHAR bank,PFUCHAR rem);
#endif

RETCODE  PDsort (PFPDIBUFF pusr);

RETCODE PDfiletype(PFPD_DB_IO pDbIo);
UINT2B PDitoa(UINT2B num, PFUCHAR ascii);
VOID PDatoi(PFUCHAR pString, PFUINT2B pOut);
VOID PDatobyte(PFUCHAR pString, PFUCHAR pOut);

RETCODE PDgetrdraw(PFPDIBUFF pusr, PFUCHAR wordout, PFUCHAR size);


RETCODE PDgetrdwrd(PFPDIBUFF pusr, PFUCHAR wordptr, PFFLAG neg_flag,
                  PFFLAG alt_flag, PFUINT4B alt_entry, PFUCHAR len, PFUINT2B alt_entry_len, PFUCHAR extraBytes);

RETCODE PDadd (PFPDIBUFF pusr, PFUCHAR word,FLAG neg_flag,
                  FLAG alt_flag,PFUCHAR alt,INT2B alt_size, 
                  PFSF_MAP hyp_map, PFUINT4B cap_map, UCHAR cap_code,
                  UINT2B wDID, PFUCHAR extraBytes);

RETCODE PDdecod2 (PFUCHAR        letter,
                  INT2B          length,
                  INT2B          block, 
                  INT2B          nextblk,
                  PFSF_DCTRL     pDCtrl,
                  PFSF_DVCTRL    pDVCtrl);

#ifdef PD_V10_SD_READ
RETCODE PDdecodOldSD(PFUCHAR     letter,
                     INT2B       length,
                     INT2B       block, 
                     INT2B       nextblk,
                     PFSF_DCTRL  pDCtrl,
                     PFSF_DVCTRL pDVCtrl);
#endif /* PD_V10_SD_READ */

RETCODE PDdecode (PFUCHAR letter,INT2B length,
                      PFSF_DCTRL pDCtrl,PFSF_DVCTRL pDVCtrl);

#ifdef PD_CORRECTION_SUPPORT
RETCODE PDcorrec (PFUCHAR inword,INT2B iMode,PFSF_DCTRL pDCtrl,PFSF_DVCTRL pDVCtrl);
#endif

RETCODE PDcorsrt (PFUCHAR word, INT2B alt_first, INT2B alt_last, PFSF_DCTRL pDCtrl,
                  PFSF_DVCTRL pDVCtrl);

#ifdef PD_FILEIO_SUPPORT
RETCODE PDasparse(PFUCHAR ascii_word,PFUCHAR word,PFFLAG alt_flag,PFFLAG neg_flag,
          PFUCHAR alt_entry_wrd,PFUINT2B alt_entry_size, UCHAR uEBLength, PFUCHAR uExtraBytes);
RETCODE PDashead(PFUCHAR header,PFPD_DB_IO pDbIo);
RETCODE PDreadas(PFPD_DB_IO pDbIo);
RETCODE PDwriteas(PFPD_DB_IO pDbIo);
#endif

INT4B TstTimer(VOID);
INT4B DifTimer(INT4B);
int _calibrate_timer(void);


#ifdef MAC_HANDLES
RETCODE PDhndlck (PFPDIBUFF pusr, FLAG lock, UCHAR nPrimOp);
#endif

#endif /* SF_PD */

#endif /* PDLIBHDR_H */

/* End of PDlibhdr.h */
