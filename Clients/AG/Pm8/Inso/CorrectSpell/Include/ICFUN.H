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
*        ICFUN.h -                                                      *
*                 Defines the                                           *
*                 International CorrectSpell(TM) Software               *
*                 functionality.                                        *
*                                                                       *
*-----------------------------------------------------------------------*
*                                                                       *
        Description:

        This header file is where the application developer can select
        functionality and otherwise tailor International Correct Spell
        to the applications requirements.  Turning off features at compile
        time will result in a smaller and faster product.  Standard 
        modifications can be made by editing only this file.  Most of the
        changes are self-explanatory but the ICS documentation has deeper
        detail.

        This header should be included by all modules of ICS and SF


*/

#ifndef ICFUN_H
#define ICFUN_H

       

/*------------- Start of OEM modifiable code ---------------------------------*/





/**************   SUPPORTED LANGUAGES                          ***************/

/* Conditional compile for languages.  In this area define the language or 
languages that will be required by your application                          */


/*                               Afrikaans                                   */
#define ICS_AFRIKAANS         

/*---------------------------------------------------------------------------*/
/*                               Australian                                  */
#define ICS_AUSTRALIAN         

/*---------------------------------------------------------------------------*/


/*                               Catalan                                     */
#define ICS_CATALAN            
/*---------------------------------------------------------------------------*/


/*                               Czech                                       */
#ifdef SL_HMCH2
#define ICS_CZECH
#endif              
/*---------------------------------------------------------------------------*/


/*                               Danish                                      */
#define ICS_DANISH             
/*---------------------------------------------------------------------------*/


/*                               Dutch                                       */
#define ICS_DUTCH              
/*---------------------------------------------------------------------------*/


/*                               English                                     */
#define ICS_ENGLISH            
/*---------------------------------------------------------------------------*/


/*                               Finnish                                     */
#define ICS_FINNISH            

/* Allow more comprehensive Finnish morphological verification                */
#define ICS_FINNISH_MORPHO

/* Size of the Finnish morphological verifier's internal cache 
         this size must be a multiple of 32                   */

#define ICFINCACHESIZE  256

/* Size of the Finnish morphological internal db buffer (bytes) (min 64)      */
#define ICFINBUFFERSIZE  1000
/*---------------------------------------------------------------------------*/


/*                               French                                      */
#define ICS_FRENCH             
/*---------------------------------------------------------------------------*/


/*                               German                                      */
#define ICS_GERMAN             
/*---------------------------------------------------------------------------*/

/*                               Greek                                       */
#ifdef SL_HMCH7
#define ICS_GREEK
#endif              
/*---------------------------------------------------------------------------*/

/*                               Hungarian                                       */
#ifdef SL_HMCH2
#define ICS_HUNGARIAN
#endif              
/*---------------------------------------------------------------------------*/


/*                               Italian                                     */
#define ICS_ITALIAN            
/*---------------------------------------------------------------------------*/


/*                               Norwegian                                   */
#define ICS_NORWEGIAN          
/*---------------------------------------------------------------------------*/

/*                               Polish                                       */
#ifdef SL_HMCH2
#define ICS_POLISH
#endif              
/*---------------------------------------------------------------------------*/


/*                               Portuguese                                  */
#define ICS_PORTUGUESE         
/*---------------------------------------------------------------------------*/

/*                               Russian                                     */

/* The Cyrillic character set must be defined in order to use Russian */
/*    Note: Character sets including SL_HMCH5 are defined in slfun.h         */

#ifdef SL_HMCH5  
#define ICS_RUSSIAN            
#endif


/*---------------------------------------------------------------------------*/


/*                               Spanish                                     */
#define ICS_SPANISH            
/*---------------------------------------------------------------------------*/


/*                               Swedish                                     */
#define ICS_SWEDISH            
/*---------------------------------------------------------------------------*/


/* Conditional compile for Germanic language Mandatory Hyphen Word Check feature*/

#if (defined ICS_DANISH || defined ICS_DUTCH || defined ICS_GERMAN || defined ICS_NORWEGIAN || defined ICS_SWEDISH)

#define ICS_MAND_HYPHEN_CHECK

#endif
/*---------------------------------------------------------------------------*/



/**************   HYPHENATION                                  ***************/

/* Conditional compile for Pattern Matching Hyphenation */
#define ICS_HYPHENATION
/*---------------------------------------------------------------------------*/


/**************   PERSONAL DICTIONARY                          ***************/


/* Conditional compile for the Personal Dictionary Component */
#define ICS_PDICTIONARY
/*---------------------------------------------------------------------------*/





/**************   VERIFICATION PROCESSING                      ***************/

/* Conditional compile for previous word check                               */
#define ICS_PREV_WORD_CHECK

/* Conditional compile for Hard Hyphen Word Check                            */
#define ICS_HARD_HYPHEN_CHECK

/* Conditional compile for Cache Table Check                                 */
#define ICS_CACHE_CHECK

/*---------------------------------------------------------------------------*/


/**************   CORRECTION PROCESSING                        ***************/

/* Conditional compile for Correction Processing                             */
#define ICS_CORRECTION

/* Max. number of words returned during Correction Processing                */
#define ICCORMAX  20

/* Conditional compile for space insertion correction                        */
#ifdef ICS_CORRECTION
#define ICS_SPACE_INSRT_CORRECT
#define ICS_UCF_CORRECT
#endif
/*---------------------------------------------------------------------------*/


/**************   WILDCARD PROCESSING                          ***************/

/* Conditional compile for Wildcard Processing                               */
#define ICS_WILDCARD

/* Max. number of words returned during Wildcard Processing                  */
#define ICWLDMAX  20
/*---------------------------------------------------------------------------*/


/**************   ANAGRAM PROCESSING                           ***************/

/* Conditional compile for Anagram Processing                                */
#define ICS_ANAGRAM

/* Max. number of words returned during Anagram Processing                   */
#define ICANAMAX  20
/*---------------------------------------------------------------------------*/


/**************   GENERAL CONFIGURATION OPTIONS                ***************/

/* Define the backward compatible constant if the product will be required to 
run with databases that were released before ICS release version 3.03        */
#undef  ICS_BACK_COMPATIBLE 

#ifdef ICS_BACK_COMPATIBLE 
/* Conditional compile for in-RAM Verification Processing                    */
#define ICS_RAM_VERIFICATION
#endif

/* Max. number of returned candidates.  This number should be calculated
as the maximum of (ICCORMAX, ICANAMAX, ICWLDMAX).                            */

#define ICMAXALT  20

/* Max. Number of characters returned in the returned list.  This number should
be calculated as:  ICLISTSIZE = 12 * ICMAXALT                                */

#define ICLISTSIZE  (12 * ICMAXALT)

/*------------- End of OEM modifiable code ----------------------------------*/




/*****************************************************************************/
/*****************************************************************************/

#define ICS_API_PD

/* Conditional compile for the International Proofreader (TM) use of ICS.    */

#ifdef SF_IPR
#include "prfun.h"
#endif


/* Conditional compile for Germanic Language Compound Processing             */
#if (defined ICS_AFRIKAANS || defined ICS_DANISH || defined ICS_DUTCH || defined ICS_GERMAN || defined ICS_NORWEGIAN || defined ICS_SWEDISH)

#define ICS_COMPOUND_PROCESS     
#ifndef SF_COMPOUND_PROCESS
#define SF_COMPOUND_PROCESS
#endif

#endif


/* Conditional compile for Romance Language Clitic Processing                */
#if (defined ICS_CATALAN || defined ICS_FRENCH || defined ICS_ITALIAN || defined ICS_PORTUGUESE || defined ICS_SPANISH)

#define ICS_CLITIC_PROCESS
#ifndef SF_BASE_CLITIC
#define SF_BASE_CLITIC
#endif
#ifndef SF_CLITIC_PROCESS
#define SF_CLITIC_PROCESS
#endif

#endif


#ifdef ICS_PDICTIONARY

#ifndef SF_PD
#define SF_PD
#endif

/* Apply Database Customizer rules after finding the word in Personal Dict. */
#define ICS_APPLY_DC_RULES

#ifndef SF_APPLY_DC_RULES
#define SF_APPLY_DC_RULES
#endif

#endif


#ifdef ICS_WILDCARD 
#ifndef SF_WILDCARD
#define SF_WILDCARD
#endif
#endif

#ifdef ICS_ANAGRAM
#ifndef SF_ANAGRAM
#define SF_ANAGRAM
#endif
#endif

#ifdef ICS_CORRECTION
#ifndef SF_CORRECTION
#define SF_CORRECTION
#endif
#endif

#define SF_ACCENT

#ifdef ICS_FINNISH_MORPHO
#define FINNISH_MORPHO
#endif

#if defined(ICS_FINNISH_MORPHO) || defined(ICS_HUNGARIAN) || defined(ICS_POLISH)
#define ICS_ADD_LANG
#endif

#if (defined ICS_SPACE_INSRT_CORRECT || defined ICS_HARD_HYPHEN_CHECK)
#ifndef SF_DBLVER
#define SF_DBLVER
#endif
#endif

/* If correction, anagramming and wildcarding are not defined, then remove */
/* quadgram and other tables that are not needed                           */

#if (defined ICS_CORRECTION || defined ICS_CLITIC_PROCESS || defined ICS_ANAGRAM || defined ICS_WILDCARD)
#define ICS_SPECIAL_TABLES
#ifndef SF_XTRA_DECODE
#define SF_XTRA_DECODE
#endif
#endif

#endif /* ICFUN_H */

/*    End of ICfun.h                      */
