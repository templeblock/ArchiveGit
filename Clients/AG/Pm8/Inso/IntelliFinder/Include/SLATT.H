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
*   SLatt.h - Character attributes defines and macros.                  *
*                                                                       *
*-----------------------------------------------------------------------*

    Descripton:
        This file contains character attribute maps and the 
        macros that use those maps.  The macros access variables that must
        be defined within the scope of the functions that use them.

    Cautions:
        See note in SLFUN.H about defining character sets.

    Creation Date:
        18-jul-89   Created by InfoSoft Software Engineering

    Change Date:
        29-apr-91   JCC     Comment warning on SLto*, casting on SLtolow & -up
        13-jun-91   GJC     Add SLINAPPUNCT
        23-dec-92   JMH     SLatt_tab -> att_tab (multiple char sets)
        23-feb-93   JCC     SLto...() use defines from SLASCII.H

*-----------------------------------------------------------------------*/
#ifndef SLATT_H

#define SLATT_H


/*  Attribute Table Defines                                             */

#define     SLCLCONS    0x80000000UL /* clitic consonant i.e. h          */
#define     SLUPPER     0x40000000UL /* uppercase alphabetic             */
#define     SLLOWER     0x20000000UL /* lowercase alphabetic, has upper  */
#define     SLVOWEL     0x10000000UL /* vowel                            */
#define     SLDIGIT     0x08000000UL /* digit                            */
#define     SLACUTE     0x04000000UL /* acute accented letter            */
#define     SLSPACE     0X02000000UL /* white space character            */
#define     SLPUNCT     0x01000000UL /* punctuation                      */
#define     SLTERM      0x00800000UL /* terminating punctuation          */
                                     /* ! ? . :                          */
#define     SLOPEN      0x00400000UL /* open punctuation                 */
                                     /* ( < { [ " '                      */
#define     SLCLOSE     0x00200000UL /* close punctuation                */
                                     /* ) > } ] " ' ,                    */
#define     SLCNTL      0x00100000UL /* control character                */
#define     SLMAYACC    0x00080000UL /* may take an accent               */
#define     SLUMLAUT    0x00040000UL /* umlaut vowel                     */

/* The following two attributes share the same value. This is possible   */
/* since they will never occur in the same attribute table:              */
/* SLGRAVE is Latin 1 while SLDBACUTE is Latin 2 only                    */    

#define     SLGRAVE     0x00020000UL /* grave accent                     */
#define     SLDBACUTE   0x00020000UL /* East European double acute char  */

#define     SLRIGHT     0x00010000UL /* non-alph. word part, right margin*/

#define     SLINTERNAL  0x00008000UL /* non-alph.word part, internal     */
#define     SLCIRC      0x00004000UL /* circumflex  accent               */
#define     SLPAIR      0x00002000UL /* pair attribute for [], etc.      */

/* The following two attributes share the same value as they never       */
/* occur in the same attribute table: SLITGL is Latin 1 and Italian      */
/* while SLBREVE is Latin 2.                                             */    

#define     SLITGL      0x00001000UL /* used for Italian clitics         */
#define     SLBREVE     0x00001000UL /* Latin 2 breve accent             */

/* The following two attributes share the same value as they may         */
/* never occur in the same attribute table: SLITC is Latin 1 and Italian */
/* while SLKROZHEK is Latin 2 and Czech only                             */    

#define     SLITC       0x00000800UL /* used for Italian clitics         */
#define     SLKROZHEK   0x00000800UL /* Czech krozhek accent             */

#define     SLPARDEL    0x00000400UL /* parsing delimiter                */


#define     SLGRAPH     0x00000200UL /* Graphics character               */
#define     SLCURRENCY  0x00000100UL /* Currency character               */
#define     SLROMAN     0x00000080UL /* Roman numeric character          */
            
#define     SLALPHA     0x00000040UL /* An alpha character that has no   */
                                     /* upper case (exception to SLLOWER)*/
            
#define     SLLEAD      0x00000020UL /* Leading punctuation that can be  */
                                    /* stripped off                      */
#define     SLTRAIL     0x00000010UL /* Trailing punctuation that can be */
                                    /* stripped off                      */
#define     SLNDELETE   0x00000008UL /* # character allowing n-deletion  */
#define     SLLEFT      0x00000004UL /* non-alph. word part, left margin */
#define     SLOGANEK    0x00000002UL /* Latin 2 oganek accent            */
#define     SLHACEK     0x00000001UL /* Latin 2 Hacek                    */




/* Conditionally compile character attribute tables for different InfoSoft
 * character sets.
*/

#ifdef SL_HMCH1
SLEXTERN UINT4B SLatt1_tab[];
#endif

#ifdef SL_HMCH2
SLEXTERN UINT4B SLatt2_tab[];
#endif

#ifdef SL_HMCH5
SLEXTERN UINT4B SLatt5_tab[];
#endif

/* Attribute Macros
*/
#define SLisclvow(c)    (att_tab[(c)] & (SLVOWEL | SLCLCONS) )
#define SLisupper(c)    (att_tab[(c)] & SLUPPER)
#define SLislower(c)    (att_tab[(c)] & SLLOWER)
#define SLisvowel(c)    (att_tab[(c)] & SLVOWEL)
#define SLisdigit(c)    (att_tab[(c)] & SLDIGIT)
#define SLisaccent(c)   (att_tab[(c)] & (SLACUTE | SLCIRC | SLUMLAUT | SLGRAVE ))
#define SLiscirc(c)     (att_tab[(c)] & SLCIRC)
#define SLisspace(c)    (att_tab[(c)] & SLSPACE)
#define SLispunct(c)    (att_tab[(c)] & SLPUNCT)
#define SListerm(c)     (att_tab[(c)] & SLTERM)
#define SLisopen(c)     (att_tab[(c)] & SLOPEN)
#define SLisclose(c)    (att_tab[(c)] & SLCLOSE)
#define SLiscntl(c)     (att_tab[(c)] & SLCNTL)
#define SLisalpha(c)    (att_tab[(c)] & (SLUPPER | SLLOWER | SLALPHA))
#define SLisalnum(c)    (att_tab[(c)] & (SLUPPER | SLLOWER | SLDIGIT) )
#define SLisumlaut(c)   (att_tab[(c)] & SLUMLAUT)
#define SLisstress(c)   (att_tab[(c)] & (SLACUTE | SLGRAVE) )
#define SLmayacc(c)     (att_tab[(c)] & SLMAYACC)
#define SLpardel(c)     (att_tab[(c)] & SLPARDEL)

#define SLisCrncy(c)    (att_tab[(c)] & SLCURRENCY)
#define SLisgraph(c)    (att_tab[(c)] & SLGRAPH)
#define SLisLead(c)     (att_tab[(c)] & SLLEAD)
#define SLisTrail(c)    (att_tab[(c)] & SLTRAIL)

/* Don't pass any chars w/ASCII value less than 'A' or 'a', respectively, to 
 * these macros.  Unsigned cast of c needed for chars > 127.
*/
#define SLtolow(c)   ( (UCHAR) ( ((UCHAR)c) - U_A + L_A ) )
#define SLtoup(c)    ( (UCHAR) ( ((UCHAR)c) - L_A + U_A ) )

#define SLtoupper(c) ( (SLislower(c)) ? SLtoup(c) : (c) )
#define SLtolower(c) ( (SLisupper(c)) ? SLtolow(c) : (c) )

#ifdef SL_HMCH7
/* 
    Within specific rules for accenting Greek characters, this macro returns 
    the accented form of the character that is passed in.  Character values in 
    the Latin 7 (Greek) character table have been specifically arranged to 
    work with these rules.

    If (c) is already accented, it is returned unchanged.
    (c) is unchanged if the character to the right of it in the collating 
    sequence is not accented. This is to prevent returning 'C' as the 
    accented counterpart of 'B'. 
    If (c) is not accented and the character following it in the Latin 7 
    collating sequence IS accented, we return that accented character. 
*/
#define SLGREEKACCENT(c)     (SLisstress(c) ? (c) : (SLisstress(c + 1) ? (UCHAR)(c + 1) : (c)))

/* 
    This macro returns the unaccented form of (c). 
    If (c) is accented, we return the character immediately in front of it in 
    the Latin 7 collating sequence. 
*/
#define SLGREEKUNACCENT(c)   (SLisstress(c) ? (UCHAR) (c - 1) : (UCHAR)(c))
#endif

/* Parsing delimiter defines
*/
#define SLPARHYP    0x8000      /* Internal Hyphen      */
#define SLPSLASH    0x4000      /* Internal Slash       */
#define SLLEADAPO   0x2000      /* Leading Apostrophe       */
#define SLENDAPO    0x1000      /* Ending  Apostrophe       */
#define SLLEADHYP   0x0800      /* Leading Hyphen       */
#define SLENDHYP    0x0400      /* Ending  Hyphen       */
#define SLEOWPER    0x0200      /* End Of Word Period       */
#define SLINAPPUNCT 0x0100      /* Inappropriate punctuation    */

/* Hyphenation point defines
*/
#define SLSYLLABIC   104      /* Syllabic Hyphenation points   */
#define SLPREFERRED  105      /* Preferred Hyphenation points   */
#define SLSTANDARD   112      /* Standard Hyphenation Points  */

#endif /* SLATT_H */

/* End of file - slatt.h */
