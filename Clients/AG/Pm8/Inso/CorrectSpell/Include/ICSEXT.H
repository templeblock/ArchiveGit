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
*    ICSEXT.H - return codes and extern declarations                    *
*                                                                       *
*-----------------------------------------------------------------------*
*                                                                       *
    Description:
    This header contains the return code and extern declaration
    for the ICspl module.  

    Cautions:
    None.

*-----------------------------------------------------------------------*/
#ifndef ICSEXT_H
#define ICSEXT_H

RETCODE SLAPI_PASCAL ICspl (PFUCHAR, PFICBUFF);

#ifdef ICS_API_PD
#ifndef ICS_PDICTIONARY
RETCODE SLAPI_PASCAL ICpd();
#else
RETCODE SLAPI_PASCAL ICpd(PFICBUFF pficbuff, PFPD_IC_IO pfIcIo);
#endif
#endif

#endif /* ICSEXT_H */
/* End of file ICSEXT.H */
