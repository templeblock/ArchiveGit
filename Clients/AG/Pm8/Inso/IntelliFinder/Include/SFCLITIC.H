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
*    SFclitic.h - Typedef for the clitic structures.                    *
*                                                                       *
*-----------------------------------------------------------------------*
*                                                                       *

    Creation Date:
        26-Apr-92       GJC                

    Change Date:

*-----------------------------------------------------------------------*/

#ifndef SFCLITIC_H

#define SFCLITIC_H

/* The definition for the clitic data structure.  
 * This structure serves all products performing clitic processing.  For
 * databases that have only a clitic hash table and no clitic information in
 * their wordlists, the iClitData member will never be used.
*/

typedef    struct    SF_clit
{

    /* Members for all clitic-processing databases.  JCCRE28
    */
    UINT2B  wCliticSiz;                   /* Clitic table size                */
    PFUCHAR pCliticTab;                   /* Pointer to the clitic table      */

    /* Members used for the decoding of clitic information from the wordlist
     * by SFdecode & other functions.  Any of these members except for 
     * iClitData might become used by hash-table-only DBs & will then be
     * moved above this comment.
    */
    UCHAR   byPreCapCode;                 /* Preclitic capitalization code    */
    UCHAR   byPostCapCode;                /* Postclitic capitalization code   */
    UCHAR   byOrgClitic[SFPOSTMAX + 1];   /* Original pre/postclitic          */
    UINT2B  byOrgHash;                    /* Org Pre/Postclitic hash code attr*/
    UCHAR   byPreClt [SFPREMAX  + 1];     /* Preclitic                        */
    UCHAR   byPostClt[SFPOSTMAX + 1];     /* Postclitic                       */
    INT2B   iPreClitFlag;                 /* Pre clitic flags                 */
    INT2B   iPostClitFlag;                /* Post clitic flags                */
    INT2B   iClitData;                    /* Clitic information from ICdecode */
    INT2B   pClitData[SF_MAX_ENTRIES];    /* Array of Clitic info generated
                                             by call to SFaccent in Spanish   */
    UCHAR   byStemCapCode;                /* Stem capitalization code         */
    SF_MAP  iStemMap;                     /* Stem capitalization map          */
    UINT2B  wPreHash;                     /* Preclitic hash code attributes   */
    UINT2B  wPostHash;                    /* Postclitic hash code attributes  */
    CHAR    cAccentPos;                   /* Position of accented vowel       */

}
SF_CLITIC,
FAR *PFSF_CLITIC;

#endif /* SFCLITIC_H */

/* End of file - SFclitic.h */
