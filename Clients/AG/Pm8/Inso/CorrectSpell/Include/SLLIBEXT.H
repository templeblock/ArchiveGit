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
*   SLlibext.h - return codes and extern declarations.                  *
*                                                                       *
*-----------------------------------------------------------------------*

    Description:
        This header contains all function prototypes for the 
        base level library.  This header should be included to 
        call any functions in this library.

    Cautions:
        None.

    Creation Date:
        18-jul-89   


*-----------------------------------------------------------------------*/
#ifndef SLLIBEXT_H

#define SLLIBEXT_H


/*              Standard Library product support functions
*/

UCHAR    SLcap    (PFUCHAR, INT2B, INT2B, PFUINT4B, PFUINT4B);
RETCODE  SLchcnv  (PUCHAR, INT2B, PUCHAR, SWORD, UCHAR);
RETCODE  SLchk    (PFUCHAR, INT2B, UCHAR, PFUINT4B, UCHAR, PFUINT4B);
RETCODE  SLcrypt  (PFUCHAR, UINT2B, PFUCHAR, UINT2B);
RETCODE  SLisint  (PFUCHAR, PFUINT4B);
FLAG     SLlisten (PFUCHAR);
PFUINT4B SLLngToA (UCHAR);
PFUCHAR  SLLngToC (SWORD, UCHAR); 
RETCODE  SLfun    (PFUCHAR, INT2B, INT2B, PFUINT2B, PFUINT2B);
RETCODE  SLmap    (PUCHAR, INT2B, INT2B, PUINT4B, PUINT4B, PUINT4B, PUINT4B);
RETCODE  SLord    (PFUCHAR, PFUCHAR, INT2B, INT2B, PFUINT4B);
RETCODE  SLpar    (PFUCHAR, PFINT2B, PFINT2B, PFUINT2B, PFUINT4B);
RETCODE  SLparcmp (PFUCHAR, PFUCHAR, PFUINT2B, FLAG, PFUINT4B);
RETCODE  SLrecap  (PFUCHAR, INT2B, UCHAR, PFUINT4B, UCHAR, PFUINT4B);
INT2B    SLtokenz (PUCHAR FAR *, PUCHAR, PFUINT2B, INT2B, PFUINT4B);
RETCODE  SLwldcmp (PFUCHAR, INT2B, PFUCHAR, INT2B);
RETCODE  SLwldfix (PFUCHAR, INT2B);
RETCODE  SLwldpro (PFUCHAR, PFINT2B, PFINT2B, INT2B);

/*              Runtime library compensation functions
*/

RETCODE SLstrcmp    (PUCHAR, PUCHAR);
RETCODE SLstrncmp   (PUCHAR, PUCHAR, SWORD);
INT2B   SLc2pasc    (PFUCHAR);
INT2B   SLpasc2c    (PFUCHAR);

#ifdef SLA_HDLPTR_MM
#ifdef SL_MAC_NOANSI_MEM   /* Use Mac Toolbox instead of ANSI lib */
PFCHAR SLrealloc    (PFCHAR *, UINT4B);
#endif
#endif

/* segmented architecture.  Note: these functions are NOT prototyped or 
 * existant in non-segmented environments.
*/
#ifdef SLA_SEGMENT_AM 

RETCODE  SLfchcnv (PFUCHAR, INT2B, PFUCHAR, SWORD, UCHAR);

#ifndef SLA_FARCHAR_LF
CHAR FAR * SLfstrcpy  (UCHAR FAR *, UCHAR FAR *);
CHAR FAR * SLfstrncpy (UCHAR FAR *, UCHAR FAR *, SWORD);
SWORD      SLfstrlen  (UCHAR FAR *);
CHAR FAR * SLfstrcat  (UCHAR FAR *, UCHAR FAR *);
VOID FAR * SLfmemset  (CHAR FAR *, SWORD, UWORD);
VOID FAR * SLfmemcpy  (CHAR FAR *, CHAR FAR *, UWORD);
#endif  /* SLA_FARCHAR_LF */

#ifndef SLA_FAROPEN_LF
SL_FHANDLE SLfaropen  (CHAR FAR *, SWORD);
SWORD      SLfarclose (SL_FHANDLE );
#endif  /* SLA_FAROPEN_LF */

VOID FAR * SLfcalloc  (UWORD, UWORD);
SWORD      SLfarwrite (SL_FHANDLE, CHAR FAR *, UWORD);
SWORD      SLfarread  (SL_FHANDLE, CHAR FAR *, UWORD);
SWORD      SLfstrncmp (UCHAR FAR *, UCHAR FAR *, SWORD);
INT4B      SLfarlseek (SL_FHANDLE, INT4B, INT2B);

#ifdef SLA_INTPASCAL_FC
SWORD FAR PASCAL   SLfstrcmp (LPSTR, LPSTR);
#else
SWORD      SLfstrcmp  (UCHAR FAR *, UCHAR FAR *);
#endif  /* SLA_INTPASCAL_FC */


#endif /* SLA_SEGMENT_AM */

RETCODE SLOpen      (PF_SLPATH, PFCHAR, PFSL_FHANDLE, INT2B);
RETCODE SLClose     (SL_FHANDLE);
RETCODE SLSeek      (SL_FHANDLE, INT4B, INT2B);
RETCODE SLFpos      (SL_FHANDLE, PFINT4B);
RETCODE SLRead      (SL_FHANDLE, INT4B, PCHAR, PFINT4B);
RETCODE SLFRead     (SL_FHANDLE, INT4B, PFCHAR, PFINT4B);
RETCODE SLHRead     (SL_FHANDLE, INT4B, PHUCHAR, PFINT4B);
RETCODE SLFWrite    (SL_FHANDLE, INT4B, PFCHAR, PFINT4B);
INT4B   SL_dos_seek (SL_FHANDLE, INT4B,INT2B);
RETCODE SL_dos_fpos (SL_FHANDLE, PFINT4B);
RETCODE SLtmpnam    (PF_SLPATH, PFCHAR); 
INT4B   SLRand      (PFINT4B);
RETCODE SLremove    (PF_SLPATH, PFCHAR); 
RETCODE SLfulfspec  (PF_SLPATH, PFCHAR, PFCHAR);
RETCODE SLpthcmp    (PF_SLPATH, PF_SLPATH);
RETCODE SLpthcpy    (PF_SLPATH, PF_SLPATH);

#ifdef SL_MEM_UDEBUG
void * SLud_calloc(size_t, size_t);
void * SLud_malloc(size_t);
void SLud_free(void *);

#ifdef SLA_SEGMENT_AM
void FAR * SLud_fcalloc(size_t, size_t);
void FAR * SLud_fmalloc(size_t);
void SLud_ffree(void FAR *);

#endif /* SLA_SEGMENT_AM */
#endif /* SL_MEM_UDEBUG */

/* Macros to allow SLcnv() and SLfarcnv() function calls even though
 * these functions have been replaced by SLchcnv() & SLfchcnv().  The macros
 * accomplish this by supplying the language code as SLUNIVERSAL -- the
 * difference between the former and latter functions is that the latter
 * take an additional parameter: language code.
*/

#define SLcnv(word,len,outword,CnvDirection)    SLchcnv(word,len,outword,CnvDirection,SLUNIVERSAL)

/* Use SLchcnv as SLfarcnv's substitute if far ptrs not used - SLfchcnv 
 * doesn't exist then.  Also map SLfchcnv to SLchcnv in that case.
*/
#ifdef SLA_SEGMENT_AM
#define SLfarcnv(word,len,outword,CnvDirection) SLfchcnv(word,len,outword,CnvDirection,SLUNIVERSAL)
#else
#define SLfarcnv(word,len,outword,CnvDirection) SLchcnv(word,len,outword,CnvDirection,SLUNIVERSAL)
#define SLfchcnv    SLchcnv
#endif  /* SLA_SEGMENT_AM */

#ifdef SLA_FILELOCK_FO
#define SLSharedUnlock SLShUnlock
VOID SLShUnlock( PF_SLPATH, PFCHAR );
#else
#define SLSharedUnlock(p,q)
#endif  /* SLA_FILELOCK_FO */


#endif /* SLLIBEXT_H */

/*  End of file - sllibext.h  */
