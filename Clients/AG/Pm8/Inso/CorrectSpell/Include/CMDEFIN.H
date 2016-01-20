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
*/

#ifndef _CMDEFIN_DEF
#define _CMDEFIN_DEF

#define MPATHMAX 68
#define MFNAMEMAX 68

#define HDRSIZE 256
#define TMP_BUFSIZE 1024
#define ILEXSIZETAB_SIZE 6
#define ASCSET_SIZE 9
#define MAXCHARS 256
#define MAXLEXRESULTS 20
#define LEXCLASSIZE 3
#define FACTS_BUF_SIZE 2560

#define HYPMAXVAL 9300
#define HYPINPUTLENGTH 121
#define HYPSTRLEN 123
#define HYPSTRLPLUS 124


#define NONALLOWEDCHAR '?'
#define BLANC           ' '

#define STARTCONDCH '\''
#define ENDCONDCH 'Z'  

#define CONDCHARMAX (ENDCONDCH - STARTCONDCH + 1)

#define LOWCAPMASK 0x04
#define INITCAPMASK 0x08
#define ALLCAPMASK 0x10
#define MAPCAPMASK 0x20

#define CMCCHSENTINEL 0xffff


#define VERSION_STR "3.07" 

#define WINMSTRMAX 128

#endif


