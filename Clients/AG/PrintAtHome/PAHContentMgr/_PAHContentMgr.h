

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


 /* File created by MIDL compiler version 6.00.0366 */
/* at Mon Apr 03 23:06:58 2006
 */
/* Compiler settings for _PAHContentMgr.idl:
    Oicf, W1, Zp8, env=Win32 (32b run)
    protocol : dce , ms_ext, c_ext, robust
    error checks: allocation ref bounds_check enum stub_data 
    VC __declspec() decoration level: 
         __declspec(uuid()), __declspec(selectany), __declspec(novtable)
         DECLSPEC_UUID(), MIDL_INTERFACE()
*/
//@@MIDL_FILE_HEADING(  )

#pragma warning( disable: 4049 )  /* more than 64k source lines */


/* verify that the <rpcndr.h> version is high enough to compile this file*/
#ifndef __REQUIRED_RPCNDR_H_VERSION__
#define __REQUIRED_RPCNDR_H_VERSION__ 475
#endif

#include "rpc.h"
#include "rpcndr.h"

#ifndef __RPCNDR_H_VERSION__
#error this stub requires an updated version of <rpcndr.h>
#endif // __RPCNDR_H_VERSION__

#ifndef COM_NO_WINDOWS_H
#include "windows.h"
#include "ole2.h"
#endif /*COM_NO_WINDOWS_H*/

#ifndef ___PAHContentMgr_h__
#define ___PAHContentMgr_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

/* Forward Declarations */ 

#ifndef __IPack_FWD_DEFINED__
#define __IPack_FWD_DEFINED__
typedef interface IPack IPack;
#endif 	/* __IPack_FWD_DEFINED__ */


#ifndef __CPack_FWD_DEFINED__
#define __CPack_FWD_DEFINED__

#ifdef __cplusplus
typedef class CPack CPack;
#else
typedef struct CPack CPack;
#endif /* __cplusplus */

#endif 	/* __CPack_FWD_DEFINED__ */


/* header files for imported files */
#include "prsht.h"
#include "mshtml.h"
#include "mshtmhst.h"
#include "exdisp.h"
#include "objsafe.h"
#include "shldisp.h"
#include "shobjidl.h"

#ifdef __cplusplus
extern "C"{
#endif 

void * __RPC_USER MIDL_user_allocate(size_t);
void __RPC_USER MIDL_user_free( void * ); 

#ifndef __IPack_INTERFACE_DEFINED__
#define __IPack_INTERFACE_DEFINED__

/* interface IPack */
/* [unique][helpstring][dual][uuid][object] */ 


EXTERN_C const IID IID_IPack;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("300F4EEA-6570-481D-9DF1-E3F1B01ADB5E")
    IPack : public IDispatch
    {
    public:
        virtual /* [helpcontext][helpstring][id] */ HRESULT STDMETHODCALLTYPE ProcessPurchase( 
            /* [in] */ BSTR szPackUrl) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IPackVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IPack * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IPack * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IPack * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IPack * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IPack * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IPack * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IPack * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [helpcontext][helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *ProcessPurchase )( 
            IPack * This,
            /* [in] */ BSTR szPackUrl);
        
        END_INTERFACE
    } IPackVtbl;

    interface IPack
    {
        CONST_VTBL struct IPackVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IPack_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IPack_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IPack_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IPack_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IPack_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IPack_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IPack_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IPack_ProcessPurchase(This,szPackUrl)	\
    (This)->lpVtbl -> ProcessPurchase(This,szPackUrl)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpcontext][helpstring][id] */ HRESULT STDMETHODCALLTYPE IPack_ProcessPurchase_Proxy( 
    IPack * This,
    /* [in] */ BSTR szPackUrl);


void __RPC_STUB IPack_ProcessPurchase_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IPack_INTERFACE_DEFINED__ */



#ifndef __PAHContentMgr_LIBRARY_DEFINED__
#define __PAHContentMgr_LIBRARY_DEFINED__

/* library PAHContentMgr */
/* [helpstring][uuid][version] */ 


EXTERN_C const IID LIBID_PAHContentMgr;

EXTERN_C const CLSID CLSID_CPack;

#ifdef __cplusplus

class DECLSPEC_UUID("372E2874-34CF-44E8-B55D-73AFCB07AE84")
CPack;
#endif
#endif /* __PAHContentMgr_LIBRARY_DEFINED__ */

/* Additional Prototypes for ALL interfaces */

unsigned long             __RPC_USER  BSTR_UserSize(     unsigned long *, unsigned long            , BSTR * ); 
unsigned char * __RPC_USER  BSTR_UserMarshal(  unsigned long *, unsigned char *, BSTR * ); 
unsigned char * __RPC_USER  BSTR_UserUnmarshal(unsigned long *, unsigned char *, BSTR * ); 
void                      __RPC_USER  BSTR_UserFree(     unsigned long *, BSTR * ); 

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif


