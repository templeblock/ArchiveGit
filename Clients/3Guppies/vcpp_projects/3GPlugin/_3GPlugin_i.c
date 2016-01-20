

/* this ALWAYS GENERATED file contains the IIDs and CLSIDs */

/* link this file in with the server and any clients */


 /* File created by MIDL compiler version 6.00.0366 */
/* at Tue Jul 31 19:09:05 2007
 */
/* Compiler settings for _3GPlugin.idl:
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

MIDL_DEFINE_GUID(IID, IID_IBrowserHelper,0x34D909E0,0x1FD5,0x49BF,0x9A,0x77,0x0C,0xA6,0x78,0x8A,0x30,0x3F);


MIDL_DEFINE_GUID(IID, IID_IUnusedInterface,0x91B336EE,0xA725,0x4A27,0xAC,0x48,0xD1,0xB0,0xE1,0x71,0x21,0xAC);


MIDL_DEFINE_GUID(IID, LIBID_a3GPlugin,0x81F18C34,0xF49B,0x402E,0x8E,0x55,0x31,0x3D,0x0D,0xAA,0x9F,0x21);


MIDL_DEFINE_GUID(CLSID, CLSID_CBrowserHelper,0xD29F1EC9,0xE53A,0x4A05,0x8A,0x8C,0x5B,0xB2,0xE5,0x8E,0x3E,0x3D);


MIDL_DEFINE_GUID(CLSID, CLSID_CShellExtension,0x89D72376,0x67AA,0x4021,0xB0,0x89,0xAF,0x4F,0xC1,0x45,0x8B,0xAC);

#undef MIDL_DEFINE_GUID

#ifdef __cplusplus
}
#endif



