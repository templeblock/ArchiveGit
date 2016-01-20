

/* this ALWAYS GENERATED file contains the IIDs and CLSIDs */

/* link this file in with the server and any clients */


 /* File created by MIDL compiler version 6.00.0366 */
/* at Thu Mar 27 10:40:17 2008
 */
/* Compiler settings for AxCtp2.idl:
    Oicf, W1, Zp8, env=Win32 (32b run)
    protocol : dce , ms_ext, c_ext, robust
    error checks: allocation ref bounds_check enum stub_data 
    VC __declspec() decoration level: 
         __declspec(uuid()), __declspec(selectany), __declspec(novtable)
         DECLSPEC_UUID(), MIDL_INTERFACE()
*/
//@@MIDL_FILE_HEADING(  )

#pragma warning( disable: 4049 )  /* more than 64k source lines */


#ifdef __cplusplus
extern "C"{
#endif 


#include <rpc.h>
#include <rpcndr.h>

#ifdef _MIDL_USE_GUIDDEF_

#ifndef INITGUID
#define INITGUID
#include <guiddef.h>
#undef INITGUID
#else
#include <guiddef.h>
#endif

#define MIDL_DEFINE_GUID(type,name,l,w1,w2,b1,b2,b3,b4,b5,b6,b7,b8) \
        DEFINE_GUID(name,l,w1,w2,b1,b2,b3,b4,b5,b6,b7,b8)

#else // !_MIDL_USE_GUIDDEF_

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

#define MIDL_DEFINE_GUID(type,name,l,w1,w2,b1,b2,b3,b4,b5,b6,b7,b8) \
        const type name = {l,w1,w2,{b1,b2,b3,b4,b5,b6,b7,b8}}

#endif !_MIDL_USE_GUIDDEF_

MIDL_DEFINE_GUID(IID, IID_ICtp,0x4C537050,0x5856,0x4413,0x9C,0x89,0x39,0x46,0x11,0xBC,0xBE,0x8A);


MIDL_DEFINE_GUID(IID, LIBID_AXCTP2Lib,0x435B34F7,0x217E,0x4797,0xBE,0x46,0x2A,0xB8,0xB5,0x98,0x06,0x51);


MIDL_DEFINE_GUID(IID, DIID__ICtpEvents,0xF0207328,0x6435,0x4d34,0xB2,0x13,0x6F,0x7E,0xCF,0x81,0xDD,0x02);


MIDL_DEFINE_GUID(CLSID, CLSID_Ctp,0xBB383206,0x6DA1,0x4e80,0xB6,0x2A,0x3D,0xF9,0x50,0xFC,0xC6,0x97);

#undef MIDL_DEFINE_GUID

#ifdef __cplusplus
}
#endif



