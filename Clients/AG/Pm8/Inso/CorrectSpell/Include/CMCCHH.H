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

#include "cmstruc.h"
#include "cmglob.h"


#ifndef CMCCHC

#ifdef PROTOS



boolean CacheSearch(PFUCHAR word, UCHAR wlen, PFUCHAR nonbase, UCHAR nonbaselen, pcmCS cmCtrl);
boolean InitCache(pcmCS cmCtrl);
void EndCache(pcmCS cmCtrl);
boolean AddToCache(PFUCHAR word, UCHAR wlen, UINT2B grind, pcmCS cmCtrl);

#endif
#endif
