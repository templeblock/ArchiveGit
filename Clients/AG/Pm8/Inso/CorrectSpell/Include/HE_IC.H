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
*    HE_ic.h - definitions and type declaration for HE (Hungarian       *
*    Speller by MorfoLogic) and IC (Inso CorrectSpell) interface        *   
*                                                                       *
*-----------------------------------------------------------------------*
*                                                                       *
*-----------------------------------------------------------------------*/

#if (defined ICS_HUNGARIAN) || (defined ICS_POLISH)

#ifndef HUIC_H
#define HUIC_H

#include "he_defs.h"
#include "he_api.h"

#define   HE_FULL -1L        /* constant for full correction       */
#define   HE_FIRST 1L        /* constant for first correction step */
#define   HE_DONE  0L        /* no more correction steps           */
#define   HE_HALF  0x00000100  /* correction half time             */

typedef   SPELL_ARG        * PSPELL_ARG;             
typedef   SPELL_ARG  FAR   * PFSPELL_ARG;             
typedef   INTL  FAR        * PFINTL;

/* Definition for Hungarian Buffer */
typedef    struct    _HUBUFF
{
   SLF_HDL_OBJ        hspellenv;                /* Pointer to datafiles */
   SLF_HDL_OBJ        hcp;                      /* Pointer to code page object */
   PFSPELL_ARG        pfSpellArg;               /* Pointer to Hungarian spell buffer */ 
#ifdef ICS_CORRECTION
   SLF_HDL_OBJ        hsgenv;                   /* Ptr to suggest dict file handle */
   INT4B              byStep;                   /* Current correction step */
#endif
#ifdef ICS_HYPHENATION
   SLF_HDL_OBJ        hhypenv;                  /* Ptr to suggest dict file handle */
#endif
#if defined (WINDOWS) || defined (WIN32) || defined(OS2)
   HMODULE    hInstHunDll;
   HMODULE    hInstPDDll;                      /* Instance handle of ICS for callback functions */
#endif
}
HUBUFF,
FAR * PFHUBUFF;

#endif
#endif
