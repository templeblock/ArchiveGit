/* this file contains the actual definitions of */
/* the IIDs and CLSIDs */

/* link this file in with the server and any clients */


/* File created by MIDL compiler version 5.01.0164 */
/* at Thu Aug 10 12:02:52 2000
 */
/* Compiler settings for AxCtp.idl:
    Oicf (OptLev=i2), W1, Zp8, env=Win32, ms_ext, c_ext
    error checks: allocation ref bounds_check enum stub_data 
*/
//@@MIDL_FILE_HEADING(  )
#ifdef __cplusplus
extern "C"{
#endif 


#ifndef __IID_DEFINED__
#define __IID_DEFINED__

typedef struct _IID
{
    unsigned long x;
    unsigned short s1;
    unsigned short s2;
    unsigned char  c[8];
} IID;

#endif // __IID_DEFINED__

#ifndef CLSID_DEFINED
#define CLSID_DEFINED
typedef IID CLSID;
#endif // CLSID_DEFINED

const IID IID_ICtp = {0x38578BFE,0x0ABB,0x11D3,{0x93,0x30,0x00,0x80,0xC6,0xF7,0x96,0xA1}};


const IID LIBID_AXCTPLib = {0x38578BF1,0x0ABB,0x11D3,{0x93,0x30,0x00,0x80,0xC6,0xF7,0x96,0xA1}};


const CLSID CLSID_Ctp = {0x38578BF0,0x0ABB,0x11D3,{0x93,0x30,0x00,0x80,0xC6,0xF7,0x96,0xA1}};


#ifdef __cplusplus
}
#endif

