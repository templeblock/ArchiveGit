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
*    SFtyp.h - Typedefs for the Standard functyion library.             *
*                                                                       *
*-----------------------------------------------------------------------*
*                                                                       *

    Creation Date:
        22-jun-92       ECH

    Change Date:

*-----------------------------------------------------------------------*/

#ifndef SFTYP_H

#define SFTYP_H

/* Type definitions for SF capitalization bitmaps.  SF_MAP_SIZ is the
 * number of long integers (32bits) that are used in a cap map.
*/
typedef UINT4B       SF_MAP[SF_MAP_SIZ];
typedef SF_MAP FAR  *PFSF_MAP;
typedef SF_MAP NEAR *PNSF_MAP;
typedef SF_MAP      *PXSF_MAP;

typedef SLF_HDL_OBJ HFSF_MAP;

SLF_TYPDEF_OR (HFSF_MAP, PFSF_MAP, ORF_map_sf) ORFSF_MAP;
SLF_ITYPDEF_OR (PFSF_MAP, ORF_imap_sf) ORFSF_IMAP;

/* Parts Of Inflection (POI) maps.  This map has 1 bit for each POI (feature
 * tag) possible for a given database.  Bit 0 isn't used because a POI of 0
 * is a flag value.  This map follows the SF bitmap bit/byte ordering (see
 * comment at SET_NTH_BIT_SF macro).
*/
typedef UCHAR SF_POI[SF_POI_SIZ];
typedef SF_POI FAR  *PFSF_POI;
typedef SF_POI NEAR *PNSF_POI;
typedef SF_POI      *PXSF_POI;

/*	Type definitions for version number and CTO attributes.
 */
typedef	struct
{	CHAR	wVersion,
			wMajor,
			wMinor;
}	SF_VER, *PSF_VER, FAR *PFSF_VER, NEAR *PNSF_VER;

typedef CHAR SF_ATTRIB[4];
typedef	CHAR *PSF_ATTRIB, FAR *PFSF_ATTRIB, NEAR *PNSF_ATTRIB;
typedef FLAG (FAR PASCAL *SF_ABORTPROC)(UINT4B, UINT4B);

#endif /* SFTYP_H */

/* End of file - SFtyp.h */
