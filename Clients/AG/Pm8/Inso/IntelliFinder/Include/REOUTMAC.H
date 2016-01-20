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
 * REoutmac.h - Output data macros.                                         *
 *                                                                          *
 *--------------------------------------------------------------------------*

Description:
   Non-database macros which are in the output data.

Creation Date:
   5-31-1989   L Tang

Change Record:
   Date Author      Comment
    21-feb-92	RAS	ED names -> RE

*---------------------------------------------------------------------------*/
                                 /* Output macros for: */
#ifndef REOUTMAC_H

#define REOUTMAC_H

#define  BOLD_S_M        "< BD>" /* Beginning of bold typeset */
#define  BOLD_E_M        "</BD>" /* End of bold typeset */

#define  BO_IT_S_M       "< BI>" /* Beginning of bold italic typeset */
#define  BO_IT_E_M       "</BI>" /* End of bold italic typeset */

#define  DB_QUO_S_M      "< DQ>" /* Beginning double quotation mark */
#define  DB_QUO_E_M      "</DQ>" /* Ending double quotation mark */

#define  FRACT_S_M       "< FR>" /* Beginning of fraction */
#define  FRACT_E_M       "</FR>" /* End of fraction */

#define  ITALIC_S_M      "< IT>" /* Beginning of italic typeset */
#define  ITALIC_E_M      "</IT>" /* End of italic typeset */

#define  SI_QUO_S_M      "< SQ>" /* Beginning single quotation mark */
#define  SI_QUO_E_M      "</SQ>" /* Ending single quotation mark */

#define  SUBS_S_M        "< UB>" /* Beginning of subscript */
#define  SUBS_E_M        "</UB>" /* End of subscript */

#define  SUPERS_S_M      "< UP>" /* Beginning of superscript */
#define  SUPERS_E_M      "</UP>" /* End of superscript */

#endif /* REOUTMAC_H */

/* End of file - REoutmac.h
*/
