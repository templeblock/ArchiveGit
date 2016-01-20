

/* this ALWAYS GENERATED file contains the IIDs and CLSIDs */

/* link this file in with the server and any clients */


 /* File created by MIDL compiler version 6.00.0366 */
/* at Thu Mar 27 11:01:45 2008
 */
/* Compiler settings for _ImageControlInterface.idl:
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

MIDL_DEFINE_GUID(IID, IID_IImageControl,0x750976F5,0x86CA,0x4D8F,0x91,0x03,0xEA,0x07,0xFC,0x9D,0xB6,0x11);


MIDL_DEFINE_GUID(IID, LIBID_ImageControlModule,0x093C0E16,0xC4B2,0x40CF,0xB2,0x9D,0x6B,0x4A,0x66,0x55,0x76,0x04);


MIDL_DEFINE_GUID(IID, DIID__IImageControlEvents,0xFE2FC027,0x8743,0x4F05,0x90,0xF3,0xBB,0xE7,0x9D,0x22,0xF4,0xCE);


MIDL_DEFINE_GUID(CLSID, CLSID_CImageControl,0x266D1DBF,0x1107,0x4529,0xA4,0x8F,0x0F,0xFB,0xAE,0x73,0x51,0xD1);

#undef MIDL_DEFINE_GUID

#ifdef __cplusplus
}
#endif



