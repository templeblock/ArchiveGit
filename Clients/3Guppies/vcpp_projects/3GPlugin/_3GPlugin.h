

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


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

#ifndef ___3GPlugin_h__
#define ___3GPlugin_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

/* Forward Declarations */ 

#ifndef __IBrowserHelper_FWD_DEFINED__
#define __IBrowserHelper_FWD_DEFINED__
typedef interface IBrowserHelper IBrowserHelper;
#endif 	/* __IBrowserHelper_FWD_DEFINED__ */


#ifndef __IUnusedInterface_FWD_DEFINED__
#define __IUnusedInterface_FWD_DEFINED__
typedef interface IUnusedInterface IUnusedInterface;
#endif 	/* __IUnusedInterface_FWD_DEFINED__ */


#ifndef __CBrowserHelper_FWD_DEFINED__
#define __CBrowserHelper_FWD_DEFINED__

#ifdef __cplusplus
typedef class CBrowserHelper CBrowserHelper;
#else
typedef struct CBrowserHelper CBrowserHelper;
#endif /* __cplusplus */

#endif 	/* __CBrowserHelper_FWD_DEFINED__ */


#ifndef __CShellExtension_FWD_DEFINED__
#define __CShellExtension_FWD_DEFINED__

#ifdef __cplusplus
typedef class CShellExtension CShellExtension;
#else
typedef struct CShellExtension CShellExtension;
#endif /* __cplusplus */

#endif 	/* __CShellExtension_FWD_DEFINED__ */


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

#ifndef __IBrowserHelper_INTERFACE_DEFINED__
#define __IBrowserHelper_INTERFACE_DEFINED__

/* interface IBrowserHelper */
/* [unique][helpstring][dual][uuid][object] */ 


EXTERN_C const IID IID_IBrowserHelper;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("34D909E0-1FD5-49BF-9A77-0CA6788A303F")
    IBrowserHelper : public IDispatch
    {
    public:
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE LaunchMobilizeWindow( 
            /* [in] */ VARIANT varDispWindow,
            /* [in] */ BSTR bstrURL,
            /* [retval][out] */ BSTR *plResult) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE LaunchMobilizePopup( 
            /* [in] */ VARIANT varDispWindow,
            /* [in] */ BSTR bstrURL,
            /* [in] */ BSTR bstrTargetName,
            /* [in] */ BSTR bstrFeatures,
            /* [retval][out] */ BSTR *plResult) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IBrowserHelperVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IBrowserHelper * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IBrowserHelper * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IBrowserHelper * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IBrowserHelper * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IBrowserHelper * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IBrowserHelper * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IBrowserHelper * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *LaunchMobilizeWindow )( 
            IBrowserHelper * This,
            /* [in] */ VARIANT varDispWindow,
            /* [in] */ BSTR bstrURL,
            /* [retval][out] */ BSTR *plResult);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *LaunchMobilizePopup )( 
            IBrowserHelper * This,
            /* [in] */ VARIANT varDispWindow,
            /* [in] */ BSTR bstrURL,
            /* [in] */ BSTR bstrTargetName,
            /* [in] */ BSTR bstrFeatures,
            /* [retval][out] */ BSTR *plResult);
        
        END_INTERFACE
    } IBrowserHelperVtbl;

    interface IBrowserHelper
    {
        CONST_VTBL struct IBrowserHelperVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IBrowserHelper_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IBrowserHelper_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IBrowserHelper_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IBrowserHelper_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IBrowserHelper_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IBrowserHelper_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IBrowserHelper_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IBrowserHelper_LaunchMobilizeWindow(This,varDispWindow,bstrURL,plResult)	\
    (This)->lpVtbl -> LaunchMobilizeWindow(This,varDispWindow,bstrURL,plResult)

#define IBrowserHelper_LaunchMobilizePopup(This,varDispWindow,bstrURL,bstrTargetName,bstrFeatures,plResult)	\
    (This)->lpVtbl -> LaunchMobilizePopup(This,varDispWindow,bstrURL,bstrTargetName,bstrFeatures,plResult)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [id] */ HRESULT STDMETHODCALLTYPE IBrowserHelper_LaunchMobilizeWindow_Proxy( 
    IBrowserHelper * This,
    /* [in] */ VARIANT varDispWindow,
    /* [in] */ BSTR bstrURL,
    /* [retval][out] */ BSTR *plResult);


void __RPC_STUB IBrowserHelper_LaunchMobilizeWindow_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id] */ HRESULT STDMETHODCALLTYPE IBrowserHelper_LaunchMobilizePopup_Proxy( 
    IBrowserHelper * This,
    /* [in] */ VARIANT varDispWindow,
    /* [in] */ BSTR bstrURL,
    /* [in] */ BSTR bstrTargetName,
    /* [in] */ BSTR bstrFeatures,
    /* [retval][out] */ BSTR *plResult);


void __RPC_STUB IBrowserHelper_LaunchMobilizePopup_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IBrowserHelper_INTERFACE_DEFINED__ */


#ifndef __IUnusedInterface_INTERFACE_DEFINED__
#define __IUnusedInterface_INTERFACE_DEFINED__

/* interface IUnusedInterface */
/* [unique][helpstring][uuid][object] */ 


EXTERN_C const IID IID_IUnusedInterface;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("91B336EE-A725-4A27-AC48-D1B0E17121AC")
    IUnusedInterface : public IUnknown
    {
    public:
    };
    
#else 	/* C style interface */

    typedef struct IUnusedInterfaceVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IUnusedInterface * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IUnusedInterface * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IUnusedInterface * This);
        
        END_INTERFACE
    } IUnusedInterfaceVtbl;

    interface IUnusedInterface
    {
        CONST_VTBL struct IUnusedInterfaceVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IUnusedInterface_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IUnusedInterface_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IUnusedInterface_Release(This)	\
    (This)->lpVtbl -> Release(This)


#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IUnusedInterface_INTERFACE_DEFINED__ */



#ifndef __a3GPlugin_LIBRARY_DEFINED__
#define __a3GPlugin_LIBRARY_DEFINED__

/* library a3GPlugin */
/* [helpstring][uuid][version] */ 


EXTERN_C const IID LIBID_a3GPlugin;

EXTERN_C const CLSID CLSID_CBrowserHelper;

#ifdef __cplusplus

class DECLSPEC_UUID("D29F1EC9-E53A-4A05-8A8C-5BB2E58E3E3D")
CBrowserHelper;
#endif

EXTERN_C const CLSID CLSID_CShellExtension;

#ifdef __cplusplus

class DECLSPEC_UUID("89D72376-67AA-4021-B089-AF4FC1458BAC")
CShellExtension;
#endif
#endif /* __a3GPlugin_LIBRARY_DEFINED__ */

/* Additional Prototypes for ALL interfaces */

unsigned long             __RPC_USER  BSTR_UserSize(     unsigned long *, unsigned long            , BSTR * ); 
unsigned char * __RPC_USER  BSTR_UserMarshal(  unsigned long *, unsigned char *, BSTR * ); 
unsigned char * __RPC_USER  BSTR_UserUnmarshal(unsigned long *, unsigned char *, BSTR * ); 
void                      __RPC_USER  BSTR_UserFree(     unsigned long *, BSTR * ); 

unsigned long             __RPC_USER  VARIANT_UserSize(     unsigned long *, unsigned long            , VARIANT * ); 
unsigned char * __RPC_USER  VARIANT_UserMarshal(  unsigned long *, unsigned char *, VARIANT * ); 
unsigned char * __RPC_USER  VARIANT_UserUnmarshal(unsigned long *, unsigned char *, VARIANT * ); 
void                      __RPC_USER  VARIANT_UserFree(     unsigned long *, VARIANT * ); 

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif


