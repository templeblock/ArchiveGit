/* this ALWAYS GENERATED file contains the definitions for the interfaces */


/* File created by MIDL compiler version 5.01.0164 */
/* at Sat Sep 09 18:05:09 2000
 */
/* Compiler settings for AxCtp.idl:
    Oicf (OptLev=i2), W1, Zp8, env=Win32, ms_ext, c_ext
    error checks: allocation ref bounds_check enum stub_data 
*/
//@@MIDL_FILE_HEADING(  )


/* verify that the <rpcndr.h> version is high enough to compile this file*/
#ifndef __REQUIRED_RPCNDR_H_VERSION__
#define __REQUIRED_RPCNDR_H_VERSION__ 440
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

#ifndef __AxCtp_h__
#define __AxCtp_h__

#ifdef __cplusplus
extern "C"{
#endif 

/* Forward Declarations */ 

#ifndef __ICtp_FWD_DEFINED__
#define __ICtp_FWD_DEFINED__
typedef interface ICtp ICtp;
#endif 	/* __ICtp_FWD_DEFINED__ */


#ifndef __Ctp_FWD_DEFINED__
#define __Ctp_FWD_DEFINED__

#ifdef __cplusplus
typedef class Ctp Ctp;
#else
typedef struct Ctp Ctp;
#endif /* __cplusplus */

#endif 	/* __Ctp_FWD_DEFINED__ */


/* header files for imported files */
#include "oaidl.h"
#include "ocidl.h"

void __RPC_FAR * __RPC_USER MIDL_user_allocate(size_t);
void __RPC_USER MIDL_user_free( void __RPC_FAR * ); 

#ifndef __ICtp_INTERFACE_DEFINED__
#define __ICtp_INTERFACE_DEFINED__

/* interface ICtp */
/* [unique][helpstring][dual][uuid][object] */ 


EXTERN_C const IID IID_ICtp;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("38578BFE-0ABB-11D3-9330-0080C6F796A1")
    ICtp : public IDispatch
    {
    public:
        virtual /* [id][propput] */ HRESULT STDMETHODCALLTYPE put_Fonts( 
            /* [in] */ BSTR strFonts) = 0;
        
        virtual /* [id][propget] */ HRESULT STDMETHODCALLTYPE get_Fonts( 
            /* [retval][out] */ BSTR __RPC_FAR *pstrFonts) = 0;
        
        virtual /* [id][propput] */ HRESULT STDMETHODCALLTYPE put_Src( 
            /* [in] */ BSTR strSrc) = 0;
        
        virtual /* [id][propget] */ HRESULT STDMETHODCALLTYPE get_Src( 
            /* [retval][out] */ BSTR __RPC_FAR *pstrSrc) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct ICtpVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            ICtp __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            ICtp __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            ICtp __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            ICtp __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            ICtp __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            ICtp __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            ICtp __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        /* [id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Fonts )( 
            ICtp __RPC_FAR * This,
            /* [in] */ BSTR strFonts);
        
        /* [id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Fonts )( 
            ICtp __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pstrFonts);
        
        /* [id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Src )( 
            ICtp __RPC_FAR * This,
            /* [in] */ BSTR strSrc);
        
        /* [id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Src )( 
            ICtp __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pstrSrc);
        
        END_INTERFACE
    } ICtpVtbl;

    interface ICtp
    {
        CONST_VTBL struct ICtpVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define ICtp_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define ICtp_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define ICtp_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define ICtp_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define ICtp_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define ICtp_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define ICtp_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define ICtp_put_Fonts(This,strFonts)	\
    (This)->lpVtbl -> put_Fonts(This,strFonts)

#define ICtp_get_Fonts(This,pstrFonts)	\
    (This)->lpVtbl -> get_Fonts(This,pstrFonts)

#define ICtp_put_Src(This,strSrc)	\
    (This)->lpVtbl -> put_Src(This,strSrc)

#define ICtp_get_Src(This,pstrSrc)	\
    (This)->lpVtbl -> get_Src(This,pstrSrc)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [id][propput] */ HRESULT STDMETHODCALLTYPE ICtp_put_Fonts_Proxy( 
    ICtp __RPC_FAR * This,
    /* [in] */ BSTR strFonts);


void __RPC_STUB ICtp_put_Fonts_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propget] */ HRESULT STDMETHODCALLTYPE ICtp_get_Fonts_Proxy( 
    ICtp __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pstrFonts);


void __RPC_STUB ICtp_get_Fonts_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propput] */ HRESULT STDMETHODCALLTYPE ICtp_put_Src_Proxy( 
    ICtp __RPC_FAR * This,
    /* [in] */ BSTR strSrc);


void __RPC_STUB ICtp_put_Src_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propget] */ HRESULT STDMETHODCALLTYPE ICtp_get_Src_Proxy( 
    ICtp __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pstrSrc);


void __RPC_STUB ICtp_get_Src_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __ICtp_INTERFACE_DEFINED__ */



#ifndef __AXCTPLib_LIBRARY_DEFINED__
#define __AXCTPLib_LIBRARY_DEFINED__

/* library AXCTPLib */
/* [helpstring][version][uuid] */ 


EXTERN_C const IID LIBID_AXCTPLib;

EXTERN_C const CLSID CLSID_Ctp;

#ifdef __cplusplus

class DECLSPEC_UUID("38578BF0-0ABB-11D3-9330-0080C6F796A1")
Ctp;
#endif
#endif /* __AXCTPLib_LIBRARY_DEFINED__ */

/* Additional Prototypes for ALL interfaces */

unsigned long             __RPC_USER  BSTR_UserSize(     unsigned long __RPC_FAR *, unsigned long            , BSTR __RPC_FAR * ); 
unsigned char __RPC_FAR * __RPC_USER  BSTR_UserMarshal(  unsigned long __RPC_FAR *, unsigned char __RPC_FAR *, BSTR __RPC_FAR * ); 
unsigned char __RPC_FAR * __RPC_USER  BSTR_UserUnmarshal(unsigned long __RPC_FAR *, unsigned char __RPC_FAR *, BSTR __RPC_FAR * ); 
void                      __RPC_USER  BSTR_UserFree(     unsigned long __RPC_FAR *, BSTR __RPC_FAR * ); 

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif
