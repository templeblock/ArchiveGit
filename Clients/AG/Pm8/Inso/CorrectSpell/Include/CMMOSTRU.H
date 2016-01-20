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


#define FRQMIN_SIZE        0       /* Word min. len                 */
#define FRQMAX_SIZE        16       /* Word max. len                */
#define FRQTABLE_SIZE      (FRQMAX_SIZE - FRQMIN_SIZE + 1)


/* set definitions */

# define SETSPACE 256

typedef UCHAR   setpointer;

typedef UINT2B  setword;

typedef setword FAR * setptr;

#define NAMESSIZE 16

typedef UCHAR   stringlen;

typedef UINT2B productind;


typedef productrecord     *(FAR *productptr);

