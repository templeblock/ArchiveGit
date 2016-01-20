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
*   SLchcnv.h - define statements for SLchcnv                           *
*                                                                       *
*-----------------------------------------------------------------------*

    Description:
       This file includes other headers that contain tables that map language 
       codes to character sets.  It also defines aliases for the conversion
       table array names.

    Cautions:
        None.

    Creation Date:
        10-dec-92   PJA

*-----------------------------------------------------------------------*/
#ifndef SLCHCNV_H
#define SLCHCNV_H


/* Check compile flags to see if character sets are included.  If a     */
/* character set for a particular language is not included, set the     */
/* pointer to null.                                                     */

/* DTN#TAB and NTD#TAB are used to create the LangToChar table. If      */
/* the character sets are compiled, they will point to the              */
/* conversion tables for the appropriate character sets                 */

/*                          MCS Latin-1 includes
*/
#ifdef SL_HMCH1
#define DTN1TAB  (PUCHAR)SLToNat1ChrSet
#define NTD1TAB  (PUCHAR)SLToDef1ChrSet
#include "slchrmcs.h"

#ifdef SLA_IBMCHAR_CS
#ifdef SL_CH850
#include "slchr850.h"   /* Include IBM PC character set (Code Page 850)   */
#else
#include "slchrdos.h"   /* Include IBM PC character set (Code Page 437)   */ 
#endif
#endif

#if defined(WINDOWS) || defined(WIN32)
#include "slchrwin.h"           /* Include Windows character defines    */
#endif

#if defined(MAC) || defined(POWERMAC)
#include "slchrmac.h"           /* Include Macinitosh character set     */
#endif

#ifdef  UNIX
#include "slchrunx.h"           /* Include UNIX character set           */
#endif

#else /* SL_HMCH1 not defined */

#define DTN1TAB  NULL
#define NTD1TAB  NULL

#endif /* SL_HMCH1 */


/*                          MCS Latin-2 includes
*/
#ifdef SL_HMCH2

#define DTN2TAB  (PUCHAR)SLToNat2ChrSet
#define NTD2TAB  (PUCHAR)SLToDef2ChrSet
#include "slch2mcs.h"

#ifdef SLA_IBMCHAR_CS
#include "slch2dos.h"           /* Include IBM PC character defines     */
#endif

#if defined(WINDOWS) || defined(WIN32)
#include "slch2win.h"           /* Include Windows character defines    */
#endif

#if defined(MAC) || defined(POWERMAC)
#include "slch2mac.h"           /* Include Macinitosh character set     */
#endif

#ifdef  UNIX
#include "slch2iso.h"           /* Include ISO Latin 2 character set    */
#endif

#ifdef  VAX
#include "slch2iso.h"           /* Include ISO Latin 2 character set    */
#endif

#else /* SL_HMCH2 not defined */

#define DTN2TAB  NULL
#define NTD2TAB  NULL

#endif /* SL_HMCH2 */


/*                          MCS Latin-5 Includes
*/
#ifdef SL_HMCH5

#define DTN5TAB  (PUCHAR)SLToNat5ChrSet
#define NTD5TAB  (PUCHAR)SLToDef5ChrSet
#include "slch5mcs.h"

#ifdef SLA_IBMCHAR_CS
#include "slch5dos.h"           /* Include IBM PC character defines     */
#endif

#if defined(WINDOWS) || defined(WIN32)
#include "slch5win.h"           /* Include Windows character defines    */
#endif

#if defined(MAC) || defined(POWERMAC)
#include "slch5mac.h"           /* Include Macinitosh character set     */
#endif

#ifdef  UNIX
#include "slch5iso.h"           /* Include ISO Latin 5 character set    */
#endif

#ifdef  VAX
#include "slch5iso.h"           /* Include ISO Latin 5 character set    */
#endif

#else /* SL_HMCH5 */

#define DTN5TAB  NULL
#define NTD5TAB  NULL

#endif /* SL_HMCH5 */


/*                      MCS Latin-7 Includes
*/     
#ifdef SL_HMCH7

#define DTN7TAB  (PUCHAR)SLToNat7ChrSet
#define NTD7TAB  (PUCHAR)SLToDef7ChrSet
#include "slch7mcs.h"

#ifdef SLA_IBMCHAR_CS
#include "slch7dos.h"           /* DOS-OS/2 CP 869 char defines */
#endif

#if defined(WINDOWS) || defined(WIN32)
#include "slch7win.h"           /* Windows 3.1 Greek char defines */          
#endif

#if defined(MAC) || defined(POWERMAC)
#include "slch7mac.h"           /* Mac Greek char defines */
#endif

#ifdef  UNIX
#include "slch7iso.h"           /* ISO8859-7 Greek char defines */
#endif

#ifdef  VAX
#include "slch7iso.h"           /* ISO8859-7 Greek char defines */
#endif

#else /* SL_HMCH7 */
#define DTN7TAB  NULL
#define NTD7TAB  NULL
#endif

#endif /* SLCHCNV_H */

/* End of file - SLCHCNV.H */
