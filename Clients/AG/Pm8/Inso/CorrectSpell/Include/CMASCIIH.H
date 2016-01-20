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

#ifndef CMASCIIH_H
#define CMASCIIH_H

#include "cmglob.h"
#include "cmstrinh.h"



#define SKAND_A ((UCHAR)'Ž')
#define SKAND_a ((UCHAR)'„')
#define SKAND_O ((UCHAR)'™')
#define SKAND_o ((UCHAR)'”')
#define SWE_A ((UCHAR)'')
#define SWE_a ((UCHAR)'†')


/* prototypes */
/* init module: initializes conversion tables */
extern  void ConvertInit(void );

/* converts a string including OEM codes (characters>128) to ASCII */
extern  void OEMtoASCII(PFUCHAR s1,PFUCHAR s2);

/* converts an ASCII string back to OEM codes */
extern  void ASCIItoOEM(PFUCHAR s1,PFUCHAR s2);

#endif   /* CMASCIIH_H */

