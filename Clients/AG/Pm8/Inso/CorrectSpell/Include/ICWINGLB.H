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
*    ICwinglb.h -                                                       *
*                                                                       *
*-----------------------------------------------------------------------*
*                                                                       *
    Description:

    Cautions:
    None.

*-----------------------------------------------------------------------*/
#ifndef ICWINGLB_H
#define ICWINGLB_H

typedef struct langOptions 
{
    char currlang[ICMAX + 1];
    char softhyp[2];
#ifdef ICS_HYPHENATION
    BOOL hypavail;    /* TRUE if the hyphenation DB has been init'd. */
#endif
#ifdef ICS_CORRECTION
    BOOL autocor;     /* TRUE if auto correction after verification */
#ifdef ICS_PDICTIONARY
    BOOL autopse;     /* TRUE if auto personal dictionary search */
#endif /* IPD */
#ifdef ICS_SPACE_INSRT_CORRECT
    BOOL autospi;     /* TRUE if auto space insertion */
#endif /* SPACE_INSRT */
#ifdef ICS_UCF_CORRECT
    BOOL autouhh;     /* TRUE if auto uhh correction */
#endif /* UCF_CORRECT */
    BOOL autofpr;     /* TRUE if auto final period removal */
#endif /* CORRECTION */
    BOOL initialized; /* TRUE if the language has been initialized */
} LANGOPS, FAR *PFLANGOPS;

#ifdef ICS_PDICTIONARY
typedef struct dictInfo
{
    char far *pDictName[IPNUM_BUFFS + 1];
    HGLOBAL hDictName[IPNUM_BUFFS + 1];
    int dictnum;   /* total number of dictionaries available */
    int dictindex; /* currently active dictionary */
    HGLOBAL  PDHArray[IPNUM_BUFFS + 1]; /* list of handles to pd structs */
    PFPD_IC_IO PDPArray[IPNUM_BUFFS + 1]; /* list of ptrs to pd structs */
} DICTINFO;
#endif

extern HINSTANCE ghInst; /* hInst to the desktop */

extern HWND hDeskWnd;    /* main desk window */
extern HWND hWndCombo;   /* combo box of languages */
extern HWND hWndEdit;    /* main desk edit window */

extern unsigned char currentWord[ICMAX+1]; /*remove MSC80 large model compile warning*/
extern unsigned char szRepWord[2*ICMAX+1];
extern INT2B langIndex;        /* current index into CBlang[]         */
extern HGLOBAL hLangops[ICNUM_BUFFS];
extern PFLANGOPS pLangops[ICNUM_BUFFS];
extern HGLOBAL hICbuffs[ICNUM_BUFFS];
extern PFICBUFF pICbuffs[ICNUM_BUFFS];

#ifdef ICS_PDICTIONARY
extern DICTINFO gDictinfo[ICNUM_BUFFS];
extern HGLOBAL ghUsrbuff;
extern HGLOBAL FAR *gpUsrbuff;
extern INT2B  wPID;         /* current Process ID */
#endif

extern BOOL Stop;
extern BOOL crlf;  /* does current word have a CR/LF? */

#ifdef ICS_PDICTIONARY
extern BOOL bGotFile; /* able to read file from disk. */
#endif

extern BOOL bGetMarked; /* TRUE if you only retrieve hilighted text. */

/* used to determine whether AddXtraPunct() should add lead/trail hyps. */
extern BOOL bReattachLHyp; /* TRUE if AddLeadTrail() attached a lead hyp. */
extern BOOL bReattachTHyp; /* TRUE if AddLeadTrail() attached a trail hyp. */
extern BOOL bCorrFromMenu; /* TRUE if Correction called from menu. */

extern short inputrside;

/* Font handle */
extern HFONT hFont;

#endif /* ICWINGLB_H */

/*    end of file ICwinglb.h        */
