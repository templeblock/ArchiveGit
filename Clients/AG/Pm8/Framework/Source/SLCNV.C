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
*      SLcnv.c - Global conversion table declaration and function.      *
*                                                                       *
*-----------------------------------------------------------------------*


    Description:
        This file contains character conversion functions SLchcnv( ) and 
        SLfchcnv() for character conversion between native and default 
        character sets.

        The functions use conversion tables found in the files:
        
        SLCNVTB1.C (Inso Latin-1)
        SLCNVTB2.C (Inso Latin-2/Eastern European) 
        SLCNVTB5.C (Inso Latin-5/Cyrillic)
        SLCNVTB7.C (Inso Latin-7/Greek)

        All characters, even those with no particular useful value for the
        given character set, are converted to another unique character.  
        Thus, the conversion is alway reversible, even for nonsense 
        characters.

    Cautions:
        This file doesn't use SLLIBHDR.H to automatically include all SL
        header files.

    Creation Date:  
        10-dec-92   JMH,LT

*-----------------------------------------------------------------------*/
/* Windows-specific defines in windows.h not included.
*/
#define SL_INTERNAL               

#include "sllibhdr.h"

/* Include character conversion tables selectively for the three character 
 * sets.
*/
#include "slchcnv.h"

#ifdef SL_HMCH1

#ifndef VAX
#define CONVERT                 /* Don't need conversion on the VAX yet */
#endif

#include "slcnvtb1.c"

#endif  /* End of SL_HMCH1 */


#ifdef SL_HMCH2

#define CONVERT
#include "slcnvtb2.c"

#endif  /* End of SL_HMCH2 */


#ifdef SL_HMCH5

#define CONVERT
#include "slcnvtb5.c"

#endif  /* End of SL_HMCH5 */


#ifdef SL_HMCH7

#define CONVERT
#include "slcnvtb7.c"

#endif /* END OF SL_HMCH7 */


RETCODE  SLLtoCInit (VOID);


/* Local static data declaration
*/
/* SLLangToChar is a conversion table containing pointers to the appropriate
 * character conversion tables for each language code.  Since it must be
 * defined at run-time, we set the flag LtoCNotDef to TRUE at compile time,
 * and then set it to false once we have defined the array so that we need
 * only define it once.
*/
DSTATIC PUCHAR  SLLangToChar[ MAX_LANG_CODE + 1 ][ 2 ];
DSTATIC FLAG    LtoCNotDef = TRUE;

/*----------------------------------------------------------------------*
*                                                                       *
*        SLLngToC() - Returns character conversion tables               *
*                                                                       *
*-----------------------------------------------------------------------*

    Descripton:
        This function returns a pointer to a SL character conversion 
        table given the language and conversion direction.

    Input Parameters:
        SWORD        CnvDirection ;direction of character conversion
        INT2B        lang         ;language code

*-----------------------------------------------------------------------*/


PFUCHAR SLLngToC( SWORD CnvDirection, UCHAR lang )

{
    /* If the language to character set conversion table is not initialized, 
     * do so now (first call).
    */
    if ( LtoCNotDef )
        SLLtoCInit();

#ifdef CONVERT
    return SLLangToChar[lang][CnvDirection];
#else
    return NULL;
#endif

} /* SLLngToC() */



/*----------------------------------------------------------------------*
*                                                                       *
*        SLchcnv() - Character Conversion Function                      *
*                                                                       *
*-----------------------------------------------------------------------*

    Descripton:
        This function is used to convert an input string from
        one character set to another according to the setting of
        the direction of conversion flag CnvDirection and the language
        code 'lang'

    Input Parameters:
        PUCHAR       word         ;pointer to input word
        INT2B        len          ;length of input word
        PUCHAR       outword      ;pointer to output word
        SWORD        CnvDirection ;direction of character conversion
        INT2B        lang         ;language code

    Output Parameters:
        PUCHAR       outword      ;converted word

    Routine Returns:
        ERRET        -            if lang code is invalid.
        OKRET        -            otherwise.

*-----------------------------------------------------------------------*/


RETCODE SLchcnv( PUCHAR word, INT2B len, PUCHAR outword, SWORD CnvDirection, 
                 UCHAR lang )

{

    INT2B   i;          /* Internal variable counter */
    UCHAR       tempword;   /* Storage to avoid repeated word[i] lookup. */
    PUCHAR      SLchr_set;  /* Character version table pointer */
    
    /* Loop to convert the characters of the input word.
     * NOTE: No conversion is necessary when running on VAX with the Inso 
     * Latin-1 Character Set; the application should not call this function 
     * in this case.
    */

    /* If the language to character set conversion table is not initialized, 
     * do so now (first call).
    */
    if ( LtoCNotDef )
        SLLtoCInit();

#ifdef CONVERT
    SLchr_set = SLLangToChar[lang][CnvDirection];

    if ((lang > MAX_LANG_CODE) || (SLchr_set == NULLXP))
        return (ERRET);

    for ( i = ZERO; i < len; i ++ )
    {
        /* The conversion needs to only be done for characters over 128.
        */
        tempword = word[i];
        outword[i] = (tempword < 128) ? tempword : SLchr_set[tempword - 128];
    }

    /* Null terminate the word.
    */
    outword[i] = NULL;

#else

    Cstrcpy(outword, word);

#endif

    return (OKRET);

} /* SLchcnv() */

#ifdef SLA_SEGMENT_AM       /* Only needed when FAR pointers are used */

/*----------------------------------------------------------------------*
*                                                                       *
*        SLfchcnv() - Character Conversion Function that takes far      *
*                     pointers.                                         *
*                                                                       *
*-----------------------------------------------------------------------*

    Descripton:
        This function is used to convert an input string from
        one character set to another according to the setting of
        the direction of conversion flag CnvDirection.  This function
        takes far pointers.

    Input Parameters:
        PFUCHAR      word         ;far pointer to input word
        INT2B        len          ;length of input word
        PFUCHAR      outword      ;far pointer to output word
        SWORD        CnvDirection ;direction of character conversion
        UCHAR        lang         ;language code of word 

    Output Parameters:
        PUCHAR       outword      ;converted word

    Routine Returns:
        ERRET        -            if lang code is invalid.
        OKRET        -            otherwise.

    Creation Date:  
        10-dec-92   JMH,LT

    Change Record:

*-----------------------------------------------------------------------*/

RETCODE SLfchcnv( PFUCHAR word, INT2B len, PFUCHAR outword, SWORD CnvDirection, UCHAR lang )

{

    INT2B   i;          /* Internal variable counter */
    UCHAR       tempword;   /* Storage to avoid repeated word[i] lookup. */
    PUCHAR      SLchr_set;  /* Character version table pointer */

    /* Loop to convert the characters of the input word.
     * NOTE: No conversion is necessary when running on VAX with the Inso 
     * Latin-1 Character Set; the application should not call this function 
     * in this case.
    */

    /* If the language to character set conversion table is not initialized, 
     * do so now (first call).
    */
    if ( LtoCNotDef )
        SLLtoCInit();

#ifdef CONVERT
    SLchr_set = SLLangToChar[lang][CnvDirection];

    if ((lang > MAX_LANG_CODE) || (SLchr_set == NULLXP))
        return (ERRET);

    for ( i = ZERO; i < len; i ++ )
    {
        /* The conversion needs to only be done for characters over 128.
        */
        tempword = word[i];
        outword[i] = (tempword < 128) ? tempword : SLchr_set[tempword - 128];
    }
 
    /* Null terminate the word.
    */
    outword[i] = NULL;

#else

    Cfstrcpy(outword, word);

#endif

    return (OKRET);

} /* SLfchcnv() */
#endif  /* SLA_SEGMENT_AM */

/*----------------------------------------------------------------------*
*                                                                       *
*        SLLtoCInit() - Initialize Lang to Char set conversion table    *
*                                                                       *
*-----------------------------------------------------------------------*

    Descripton:
        This function fills the table SLLangToChar with pointers to
        the character conversion tables to create a mapping from language
        codes to the conversion tables for character sets appropriate to
        those languages.

    Input Parameters:
        none

    Output Parameters:
        none

    Routine Returns:
        OKRET        -        ALWAYS

    Creation Date:  
        10-dec-92   JMH,LT

    Change Record:

*-----------------------------------------------------------------------*/

RETCODE SLLtoCInit( VOID )

{

#ifdef CONVERT

    /* Default to Native Mappings
    */
    SLLangToChar[SLUNIVERSAL  ][0] = DTN1TAB; /*  0 == Universal Language code    */
    SLLangToChar[SLCZECH      ][0] = DTN2TAB; /*  1 == Czech                      */
    SLLangToChar[SLRUSSIAN    ][0] = DTN5TAB; /*  2 == Russian                    */
    SLLangToChar[SLCATALAN    ][0] = DTN1TAB; /*  3 == Catalan                    */
    SLLangToChar[SLHUNGARIAN  ][0] = DTN2TAB; /*  4 == Hungarian (Magyar)         */
    SLLangToChar[SLFRENCH     ][0] = DTN1TAB; /*  5 == French                     */
    SLLangToChar[SLGERMAN     ][0] = DTN1TAB; /*  6 == German                     */
    SLLangToChar[SLSWEDISH    ][0] = DTN1TAB; /*  7 == Swedish                    */
    SLLangToChar[SLSPANISH    ][0] = DTN1TAB; /*  8 == Spanish                    */
    SLLangToChar[SLITALIAN    ][0] = DTN1TAB; /*  9 == Italian                    */
    SLLangToChar[SLDANISH     ][0] = DTN1TAB; /* 10 == Danish                     */
    SLLangToChar[SLDUTCH      ][0] = DTN1TAB; /* 11 == Dutch                      */
    SLLangToChar[SLPORTUGUESE ][0] = DTN1TAB; /* 12 == Portuguese                 */
    SLLangToChar[SLNORWEGIAN  ][0] = DTN1TAB; /* 13 == Norwegian                  */
    SLLangToChar[SLFINNISH    ][0] = DTN1TAB; /* 14 == Finnish                    */
    SLLangToChar[SLGREEK      ][0] = DTN7TAB; /* 15 == Greek                      */
    SLLangToChar[SLENGLISH    ][0] = DTN1TAB; /* 16 == English or                 */
                                  /* SLAUSTRALIAN --Australian                    */
    SLLangToChar[SLAFRIKAANS  ][0] = DTN1TAB; /*  17 == Afrikaans                 */
    SLLangToChar[SLPOLISH     ][0] = DTN2TAB; /*  18 == Polish                    */
    SLLangToChar[SLMULTI      ][0] = DTN1TAB; /*  19 == Mixed language            */


    /* Native To Default Mappings
    */
    SLLangToChar[SLUNIVERSAL  ][1] = NTD1TAB; /*  0 == Universal Language code    */
    SLLangToChar[SLCZECH      ][1] = NTD2TAB; /*  1 == Czech                      */
    SLLangToChar[SLRUSSIAN    ][1] = NTD5TAB; /*  2 == Russian                    */
    SLLangToChar[SLCATALAN    ][1] = NTD1TAB; /*  3 == Catalan                    */
    SLLangToChar[SLHUNGARIAN  ][1] = NTD2TAB; /*  4 == Hungarian (Magyar)         */
    SLLangToChar[SLFRENCH     ][1] = NTD1TAB; /*  5 == French                     */
    SLLangToChar[SLGERMAN     ][1] = NTD1TAB; /*  6 == German                     */
    SLLangToChar[SLSWEDISH    ][1] = NTD1TAB; /*  7 == Swedish                    */
    SLLangToChar[SLSPANISH    ][1] = NTD1TAB; /*  8 == Spanish                    */
    SLLangToChar[SLITALIAN    ][1] = NTD1TAB; /*  9 == Italian                    */
    SLLangToChar[SLDANISH     ][1] = NTD1TAB; /* 10 == Danish                     */
    SLLangToChar[SLDUTCH      ][1] = NTD1TAB; /* 11 == Dutch                      */
    SLLangToChar[SLPORTUGUESE ][1] = NTD1TAB; /* 12 == Portuguese                 */
    SLLangToChar[SLNORWEGIAN  ][1] = NTD1TAB; /* 13 == Norwegian                  */
    SLLangToChar[SLFINNISH    ][1] = NTD1TAB; /* 14 == Finnish                    */
    SLLangToChar[SLGREEK      ][1] = NTD7TAB; /* 15 == Greek                      */
    SLLangToChar[SLENGLISH    ][1] = NTD1TAB; /* 16 == English or                 */
                                  /* SLAUSTRALIAN --Australian                    */
    SLLangToChar[SLAFRIKAANS  ][1] = NTD1TAB; /*  17 == Afrikaans                 */
    SLLangToChar[SLPOLISH     ][1] = NTD2TAB; /*  18 == Polish                    */
    SLLangToChar[SLMULTI      ][1] = NTD1TAB; /*  19 == Multi                     */

    /* End of table SLLangToChar */
    
    /* Register the definition of SLLangToChar by setting LtoCNotDef to 0
    */

#endif /* End of not CONVERT section */

    LtoCNotDef = FALSE;

    return(OKRET);

} /* SLLtoCInit */

/* End of file - slcnv.c */


