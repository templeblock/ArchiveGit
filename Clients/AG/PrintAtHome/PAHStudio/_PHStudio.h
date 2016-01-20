

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


 /* File created by MIDL compiler version 6.00.0366 */
/* at Mon Apr 03 23:05:32 2006
 */
/* Compiler settings for _PHStudio.idl:
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

#ifndef ___PHStudio_h__
#define ___PHStudio_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

/* Forward Declarations */ 

#ifndef __IClientAccess_FWD_DEFINED__
#define __IClientAccess_FWD_DEFINED__
typedef interface IClientAccess IClientAccess;
#endif 	/* __IClientAccess_FWD_DEFINED__ */


#ifndef __IPAHStudio_FWD_DEFINED__
#define __IPAHStudio_FWD_DEFINED__
typedef interface IPAHStudio IPAHStudio;
#endif 	/* __IPAHStudio_FWD_DEFINED__ */


#ifndef __PAHStudio_FWD_DEFINED__
#define __PAHStudio_FWD_DEFINED__

#ifdef __cplusplus
typedef class PAHStudio PAHStudio;
#else
typedef struct PAHStudio PAHStudio;
#endif /* __cplusplus */

#endif 	/* __PAHStudio_FWD_DEFINED__ */


#ifndef __CClientAccess_FWD_DEFINED__
#define __CClientAccess_FWD_DEFINED__

#ifdef __cplusplus
typedef class CClientAccess CClientAccess;
#else
typedef struct CClientAccess CClientAccess;
#endif /* __cplusplus */

#endif 	/* __CClientAccess_FWD_DEFINED__ */


/* header files for imported files */
#include "docobj.h"

#ifdef __cplusplus
extern "C"{
#endif 

void * __RPC_USER MIDL_user_allocate(size_t);
void __RPC_USER MIDL_user_free( void * ); 

#ifndef __IClientAccess_INTERFACE_DEFINED__
#define __IClientAccess_INTERFACE_DEFINED__

/* interface IClientAccess */
/* [unique][helpstring][dual][uuid][object] */ 


EXTERN_C const IID IID_IClientAccess;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("D869010E-FB38-48F5-BF06-DB757BBE30C2")
    IClientAccess : public IDispatch
    {
    public:
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SetClientObject( 
            /* [in] */ IUnknown *punkClient) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IClientAccessVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IClientAccess * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IClientAccess * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IClientAccess * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IClientAccess * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IClientAccess * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IClientAccess * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IClientAccess * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SetClientObject )( 
            IClientAccess * This,
            /* [in] */ IUnknown *punkClient);
        
        END_INTERFACE
    } IClientAccessVtbl;

    interface IClientAccess
    {
        CONST_VTBL struct IClientAccessVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IClientAccess_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IClientAccess_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IClientAccess_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IClientAccess_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IClientAccess_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IClientAccess_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IClientAccess_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IClientAccess_SetClientObject(This,punkClient)	\
    (This)->lpVtbl -> SetClientObject(This,punkClient)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IClientAccess_SetClientObject_Proxy( 
    IClientAccess * This,
    /* [in] */ IUnknown *punkClient);


void __RPC_STUB IClientAccess_SetClientObject_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IClientAccess_INTERFACE_DEFINED__ */



#ifndef __PAHStudioLib_LIBRARY_DEFINED__
#define __PAHStudioLib_LIBRARY_DEFINED__

/* library PAHStudioLib */
/* [uuid][version] */ 


EXTERN_C const IID LIBID_PAHStudioLib;

#ifndef __IPAHStudio_DISPINTERFACE_DEFINED__
#define __IPAHStudio_DISPINTERFACE_DEFINED__

/* dispinterface IPAHStudio */
/* [uuid] */ 


EXTERN_C const IID DIID_IPAHStudio;

#if defined(__cplusplus) && !defined(CINTERFACE)

    MIDL_INTERFACE("AFBB908F-6EDF-43DF-B640-9C127350088F")
    IPAHStudio : public IDispatch
    {
    };
    
#else 	/* C style interface */

    typedef struct IPAHStudioVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IPAHStudio * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IPAHStudio * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IPAHStudio * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IPAHStudio * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IPAHStudio * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IPAHStudio * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IPAHStudio * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        END_INTERFACE
    } IPAHStudioVtbl;

    interface IPAHStudio
    {
        CONST_VTBL struct IPAHStudioVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IPAHStudio_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IPAHStudio_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IPAHStudio_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IPAHStudio_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IPAHStudio_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IPAHStudio_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IPAHStudio_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)

#endif /* COBJMACROS */


#endif 	/* C style interface */


#endif 	/* __IPAHStudio_DISPINTERFACE_DEFINED__ */


EXTERN_C const CLSID CLSID_PAHStudio;

#ifdef __cplusplus

class DECLSPEC_UUID("98036F45-00EC-4EC0-A5F2-1FBAC24C15D9")
PAHStudio;
#endif

EXTERN_C const CLSID CLSID_CClientAccess;

#ifdef __cplusplus

class DECLSPEC_UUID("26B3E5A6-B47E-481A-9A1D-0D98603152E5")
CClientAccess;
#endif
#endif /* __PAHStudioLib_LIBRARY_DEFINED__ */

/* Additional Prototypes for ALL interfaces */

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif


