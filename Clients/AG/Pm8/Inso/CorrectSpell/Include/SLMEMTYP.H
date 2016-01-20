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
* SLMEMTYP.H - typedefs for standard data to be used with SL memory     *
*              management macros.                                       *
*                                                                       *
*-----------------------------------------------------------------------*


Description:
    This file contains handle and object reference type declarations 
    for some basic and common data types.  The declarations here will
    help standardize and simplify the Language Software products.

Creation Date:
    13-nov-91   JCC     from ECH's SLBASIC.H et al.

Change Record:
    15-jun-92   ECH     PUCHAR->PNUCHAR for NEAR objrcts.
    29-oct-92   RCN     Added huge buffer object

*-----------------------------------------------------------------------*/

#ifndef SLMEMTYP_H

#define SLMEMTYP_H

/* Types for handles to SL C types.
*/

SL_HDL_TYPDEF (UCHAR, SLF_HDLDEF, HUCHAR);    /* unsigned char handle     */
SL_HDL_TYPDEF (UCHAR, SLN_HDLDEF, NHUCHAR);   /* unsigned char near handle*/
SL_HDL_TYPDEF (UWORD, SLF_HDLDEF, HUWORD);    /* hdl to unsigned word     */
SL_HDL_TYPDEF (UWORD, SLN_HDLDEF, NHUWORD);   /* near hdl to unsigned word*/
SL_HDL_TYPDEF (UINT2B, SLF_HDLDEF, HUINT2B);  /* 2 byte unsgd int hdl     */
SL_HDL_TYPDEF (UINT2B, SLN_HDLDEF, NHUINT2B); /* 2 byte unsgd int near hdl*/
SL_HDL_TYPDEF (UINT4B, SLF_HDLDEF, HUINT4B);  /* 4 byte unsgd int hdl     */
SL_HDL_TYPDEF (UINT4B, SLN_HDLDEF, NHUINT4B); /* 4 byte unsgd int near hdl*/
SL_HDL_TYPDEF (CHAR, SLF_HDLDEF, HCHAR);      /* char handle              */
SL_HDL_TYPDEF (CHAR, SLN_HDLDEF, NHCHAR);     /* char near handle         */
SL_HDL_TYPDEF (SWORD, SLF_HDLDEF, HSWORD);    /* handle to word           */
SL_HDL_TYPDEF (SWORD, SLN_HDLDEF, NHSWORD);   /* near handle to word      */
SL_HDL_TYPDEF (INT2B, SLF_HDLDEF, HINT2B);    /* 2 byte int handle        */
SL_HDL_TYPDEF (INT2B, SLN_HDLDEF, NHINT2B);   /* 2 byte int near handle   */
SL_HDL_TYPDEF (INT4B, SLF_HDLDEF, HINT4B);    /* 4 byte int handle        */
SL_HDL_TYPDEF (INT4B, SLN_HDLDEF, NHINT4B);   /* 4 byte int near handle   */
SL_HDL_TYPDEF (ICHAR, SLF_HDLDEF, HICHAR);    /* unsigned 2-byte char hdl */


/* Standard objects
*/

/*----------------------------------------------------
 * BUFFER (UNSIGNED CHARACTER) OBJECTS
 *----------------------------------------------------
*/

/* Immediate far buffer object.
*/
SLF_ITYPDEF_OR (PFUCHAR, ORF_ibuff) ORF_IBUFF;

/* Immediate near buffer object.
*/
SLN_ITYPDEF_OR (PNUCHAR, ORN_ibuff) ORN_IBUFF;


/* Regular far buffer object.
*/
SLF_TYPDEF_OR (SLF_HDL_OBJ, PFUCHAR, ORF_buff) ORF_BUFF;

/* Regular near buffer object.
*/
SLN_TYPDEF_OR (SLN_HDL_OBJ, PNUCHAR, ORN_buff) ORN_BUFF;

/* Regular huge buffer object.
*/
SLH_TYPDEF_OR (SLH_HDL_OBJ, PHUCHAR, ORH_buff) ORH_BUFF;


/* Regular buffer plug.
*/
SLF_PTYPDEF_OR (SLF_HDL_OBJ, PFUCHAR, OPF_buff) OPF_BUFF;

/* Regular near buffer plug.
*/
SLN_PTYPDEF_OR (SLN_HDL_OBJ, PNUCHAR, OPN_buff) OPN_BUFF;

/* Immediate far 2-byte char buffer object.
*/
SLP_ITYPDEF_OR (PICHAR, ORP_iwbuff) ORP_IWBUFF;

/* Regular far unsigned 2-byte char buffer object.
*/
SLF_TYPDEF_OR (SLF_HDL_OBJ, PICHAR, ORF_wbuff) ORF_WBUFF;

/* Regular huge unsigned 2-byte char buffer object
*/

SLH_TYPDEF_OR (SLH_HDL_OBJ, PHICHAR, ORH_wbuff) ORH_WBUFF;


/*-----------------------------------------------------
 * CHARACTER OBJECTS
 *-----------------------------------------------------
*/

/* Signed Character object.
*/
SLF_TYPDEF_OR (SLF_HDL_OBJ, PFCHAR, ORF_char) ORF_CHAR;

/* Signed Near character object.
*/
SLN_TYPDEF_OR (SLN_HDL_OBJ, PNCHAR, ORN_char) ORN_CHAR;


/* Immediate Signed Character object.
*/
SLF_ITYPDEF_OR (PFCHAR, ORF_ichar) ORF_ICHAR;

/* Immediate Signed Near character object.
*/
SLN_ITYPDEF_OR (PNCHAR, ORN_ichar) ORN_ICHAR;


/* Signed Character plug.
*/
SLF_PTYPDEF_OR (SLF_HDL_OBJ, PFCHAR, OPF_char) OPF_CHAR;

/* Signed Near character plug.
*/
SLN_PTYPDEF_OR (SLN_HDL_OBJ, PNCHAR, OPN_char) OPN_CHAR;

/* Unsigned 2-byte character object.
*/
SLF_TYPDEF_OR (SLF_HDL_OBJ, PICHAR, ORF_wchar) ORF_WCHAR;

/* Immediate unsigned 2-byte character object.
*/
SLF_ITYPDEF_OR (PICHAR, ORF_wichar) ORF_WICHAR;

/*-----------------------------------------------------
 * UNSIGNED INTEGER OBJECTS
 *-----------------------------------------------------
*/

/* Immediate unsigned integer pointer list object. 
*/
SLF_ITYPDEF_OR (PFUINT2B, ORF_i2buint) ORF_I2BUINT;

/* Immediate near unsigned integer pointer list object. 
*/
SLN_ITYPDEF_OR (PNUINT2B, ORN_i2buint) ORN_I2BUINT;


/* Regular unsigned integer pointer list object. 
*/
SLF_TYPDEF_OR (SLF_HDL_OBJ, PFUINT2B, ORF_2buint) ORF_2BUINT;

/* Regular near unsigned integer pointer list object. 
*/
SLN_TYPDEF_OR (SLN_HDL_OBJ, PNUINT2B, ORN_2buint) ORN_2BUINT;


/*-----------------------------------------------------
 * UNSIGNED LONG INTEGER OBJECTS
 *-----------------------------------------------------
*/

/* Immediate unsigned long integer pointer list object. 
*/
SLF_ITYPDEF_OR (PFUINT4B, ORF_i4buint) ORF_I4BUINT;

/* Immediate near unsigned long integer pointer list object. 
*/
SLN_ITYPDEF_OR (PNUINT4B, ORN_i4buint) ORN_I4BUINT;


/* Regular unsigned long integer pointer list object. 
*/
SLF_TYPDEF_OR (SLF_HDL_OBJ, PFUINT4B, ORF_4buint) ORF_4BUINT;

/* Regular near unsigned long integer pointer list object. 
*/
SLN_TYPDEF_OR (SLN_HDL_OBJ, PNUINT4B, ORN_4buint) ORN_4BUINT;


/*-----------------------------------------------------
 * SIGNED INTEGER OBJECTS
 *-----------------------------------------------------
*/

/* Regular integer pointer list object. 
*/
SLF_TYPDEF_OR (SLF_HDL_OBJ, PFINT2B, ORF_2bint) ORF_2BINT;

/* Regular near integer pointer list object. 
*/
SLN_TYPDEF_OR (SLN_HDL_OBJ, PNINT2B, ORN_2bint) ORN_2BINT;


/* Immediate integer pointer list object. 
*/
SLF_ITYPDEF_OR (PFINT2B, ORF_i2bint) ORF_I2BINT;

/* Immediate near integer pointer list object. 
*/
SLN_ITYPDEF_OR (PNINT2B, ORN_i2bint) ORN_I2BINT;


/*-----------------------------------------------------
 * SIGNED LONG INTEGER OBJECTS
 *-----------------------------------------------------
*/

/* Regular long integer pointer list object. 
*/
SLF_TYPDEF_OR (SLF_HDL_OBJ, PFINT4B, ORF_4bint) ORF_4BINT;

/* Regular near long integer pointer list object. 
*/
SLN_TYPDEF_OR (SLN_HDL_OBJ, PNINT4B, ORN_4bint) ORN_4BINT;


/* Immediate long integer pointer list object. 
*/
SLF_ITYPDEF_OR (PFINT4B, ORF_i4bint) ORF_I4BINT;

/* Immediate near long integer pointer list object. 
*/
SLN_ITYPDEF_OR (PNINT4B, ORN_i4bint) ORN_I4BINT;


/* ----------------------------------------------------
 * FLAG OBJECTS
 *-----------------------------------------------------
*/

/* Flag object.
*/
SLF_TYPDEF_OR (SLF_HDL_OBJ, PFFLAG, ORF_flag) ORF_FLAG;

/* Flag Near object.
*/
SLN_TYPDEF_OR (SLN_HDL_OBJ, PNFLAG, ORN_flag) ORN_FLAG;


/* Immediate Far Flag object.
*/
SLF_ITYPDEF_OR (PFFLAG, ORF_iflag) ORF_IFLAG;

/* Immediate Near Flag object.
*/
SLN_ITYPDEF_OR (PNFLAG, ORN_iflag) ORN_IFLAG;


/* Flag object plug.
*/
SLF_PTYPDEF_OR (SLF_HDL_OBJ, PFFLAG, OPF_flag) OPF_FLAG;

/* Flag Near object plug.
*/
SLN_PTYPDEF_OR (SLN_HDL_OBJ, PNFLAG, OPN_flag) OPN_FLAG;


/*-------------------------------------------------------------
 * BUFFER POINTER OBJECTS (each element in buffer is a pointer)
 *-------------------------------------------------------------
*/

/* Buffer pointer immediate object.
*/
SLF_ITYPDEF_OR (PFUCHAR FAR *, ORF_ipbuff) ORF_IPBUFF;

/* Buffer pointer near immediate object.
*/
SLN_ITYPDEF_OR (PNUCHAR NEAR *, ORN_ipbuff) ORN_IPBUFF;


/* Buffer pointer object.
*/
SLF_TYPDEF_OR (SLF_HDL_OBJ, PFUCHAR FAR *, ORF_pbuff) ORF_PBUFF;

/* Buffer pointer near object.
*/
SLN_TYPDEF_OR (SLN_HDL_OBJ, PNUCHAR NEAR *, ORN_pbuff) ORN_PBUFF;

/* Immediate object in which buffer elements are pointers 
to unsigned 2-byte char.
*/
SLF_ITYPDEF_OR (PICHAR *, ORF_wipbuff) ORF_WIPBUFF;

/* Regular object in which buffer elements are pointers to 
unsigned 2-byte char.*/
SLF_TYPDEF_OR (SLF_HDL_OBJ, PICHAR *, ORF_wpbuff) ORF_WPBUFF;

/*-------------------------------------------------------------
 * BUFFER OBJECT OBJECTS (each element in buffer is an OR_BUFF)
 *-------------------------------------------------------------
*/

/* Immediate near object for immediate near buffer objects.
*/
SLN_ITYPDEF_OR (ORN_IBUFF *, ORN_iornibuff) ORN_IORNIBUFF;

#endif /* SLMEMTYP_H */

/* End of file - slmemtyp.h */
