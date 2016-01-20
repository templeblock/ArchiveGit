/****************************************************************************
 *                   Copyright  1995  INSO Corporation                      *
 *                International CorrectSpell[TM] Software                   *
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
*   ICdrvext.h - return codes and extern declarations                     *
*                                                                       *
*-----------------------------------------------------------------------*
*                                                                       *
   Description:
   This header contains all return codes and extern declarations
   for the demonstration delivery spell module.  This header must 
   be included to call any functions in this module or get access 
   to its global data.

   Cautions:
   None.

*-----------------------------------------------------------------------*/
#ifndef ICDRVEXT_H
#define ICDRVEXT_H

/* Number of language buffers to use             */

#define   ICNUM_BUFFS   21

SWORD _CDECL Cmain    ( SWORD, PCHAR * );

#ifdef ICS_PDICTIONARY
RETCODE ICdrvpd (PICBUFF, SWORD, FLAG , PUCHAR);
#endif

RETCODE ICdrvbuf ( UCHAR, PICBUFF * );
RETCODE ICdrvlan ( PICBUFF *, PUCHAR, PINT2B, PINT2B);
RETCODE ICdrvopt ( PICBUFF,PSWORD, PINT2B );

RETCODE ICdrvdia ( PICBUFF);
RETCODE ICdrvlex ( PICBUFF );
RETCODE ICdrvdsp ( PICBUFF, SWORD, INT2B );
RETCODE ICdrvglb ( VOID );
RETCODE ICdrvinp ( PUCHAR, INT2B );

/* LANGUAGE CONFIGURATION BUFFER               */
extern ICCONFG icconfig[ICNUM_BUFFS][MAX_DICTS];

/* INPUT SOFT HYPHEN CHARACTER                   */
extern unsigned char soft_hyphen;

extern INT2B  menu_num;              /* menu selection number       */

#ifdef ICS_PDICTIONARY
#define PD_NUM 3

typedef struct dictinfo
{
  PFPD_IC_IO pIcIo[PD_NUM];       /* Personal Dictionary I/O Structure */
  INT2B      pd_index;            /* Index of active PD */
} DICTINFO;

extern DICTINFO gDictinfo[ICNUM_BUFFS];
extern INT2B pd_init[ICNUM_BUFFS];     /* Total number of initialized PD for active lang */
extern FLAG pd_app_init;    /* Has the PD been initialized */
extern INT2B  wPID;         /* current Process ID */

#endif

#endif /* ICDRVEXT_H */

/*   end of file ICdrvext.h      */
