

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


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

#ifndef __AxCtp2_h__
#define __AxCtp2_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

/* Forward Declarations */ 

#ifndef __ICtp_FWD_DEFINED__
#define __ICtp_FWD_DEFINED__
typedef interface ICtp ICtp;
#endif 	/* __ICtp_FWD_DEFINED__ */


#ifndef ___ICtpEvents_FWD_DEFINED__
#define ___ICtpEvents_FWD_DEFINED__
typedef interface _ICtpEvents _ICtpEvents;
#endif 	/* ___ICtpEvents_FWD_DEFINED__ */


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

#ifdef __cplusplus
extern "C"{
#endif 

void * __RPC_USER MIDL_user_allocate(size_t);
void __RPC_USER MIDL_user_free( void * ); 

#ifndef __ICtp_INTERFACE_DEFINED__
#define __ICtp_INTERFACE_DEFINED__

/* interface ICtp */
/* [unique][helpstring][dual][uuid][object] */ 


EXTERN_C const IID IID_ICtp;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("4C537050-5856-4413-9C89-394611BCBE8A")
    ICtp : public IDispatch
    {
    public:
        virtual /* [id][propput] */ HRESULT STDMETHODCALLTYPE put_Src( 
            /* [in] */ BSTR strProperty) = 0;
        
        virtual /* [id][propget] */ HRESULT STDMETHODCALLTYPE get_Src( 
            /* [retval][out] */ BSTR *pstrProperty) = 0;
        
        virtual /* [id][propput] */ HRESULT STDMETHODCALLTYPE put_Context( 
            /* [in] */ BSTR strProperty) = 0;
        
        virtual /* [id][propget] */ HRESULT STDMETHODCALLTYPE get_Context( 
            /* [retval][out] */ BSTR *pstrProperty) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE PutSrc( 
            /* [in] */ BSTR strProperty) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE GetSrc( 
            /* [retval][out] */ BSTR *pstrProperty) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE PutContext( 
            /* [in] */ BSTR strProperty) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE GetContext( 
            /* [retval][out] */ BSTR *pstrProperty) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE GetVerse( 
            /* [in] */ BSTR strFilePath,
            /* [retval][out] */ BSTR *pstrProperty) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE GetAttributes( 
            /* [in] */ BSTR strFilePath,
            /* [retval][out] */ BSTR *pstrAttributes) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE ProcessImage( 
            /* [in] */ BSTR strFilePath,
            /* [in] */ int nMaxKB,
            /* [in] */ int nQuality,
            /* [retval][out] */ BSTR *pstrFilePathNew) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE DoCommand( 
            /* [in] */ BSTR strCommand,
            /* [retval][out] */ long *plResult) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE GetCommand( 
            /* [in] */ BSTR strCommand,
            /* [retval][out] */ long *plResult) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE FileDownload( 
            /* [in] */ BSTR bstrSrcUrl,
            /* [in] */ BSTR bstrInfo,
            /* [in] */ BSTR bstrOptionString) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE FYSTransfer( 
            /* [in] */ BSTR OrderId,
            /* [in] */ BSTR CorrelationId,
            /* [in] */ BSTR FYSInfo,
            /* [retval][out] */ BSTR *pstrError) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE GetUsageData( 
            /* [retval][out] */ BSTR *pstrUsageData) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE FYSClose( void) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SetAddressbookUrl( 
            /* [in] */ VARIANT varUrl) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ShowAddressbookPopup( 
            /* [in] */ VARIANT varShow) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ResizeAddressbookPopup( 
            /* [in] */ VARIANT varWidth,
            /* [in] */ VARIANT varHeight) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE CloseAddressbookPopup( void) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ProcessAddressbook( 
            /* [in] */ VARIANT varData) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SetClipArtUrl( 
            /* [in] */ VARIANT varUrl) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ShowClipArtPopup( 
            /* [in] */ VARIANT varShow) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ResizeClipArtPopup( 
            /* [in] */ VARIANT varWidth,
            /* [in] */ VARIANT varHeight) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE CloseClipArtPopup( void) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Version( 
            /* [retval][out] */ BSTR *pVal) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct ICtpVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            ICtp * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            ICtp * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            ICtp * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            ICtp * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            ICtp * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            ICtp * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            ICtp * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_Src )( 
            ICtp * This,
            /* [in] */ BSTR strProperty);
        
        /* [id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Src )( 
            ICtp * This,
            /* [retval][out] */ BSTR *pstrProperty);
        
        /* [id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_Context )( 
            ICtp * This,
            /* [in] */ BSTR strProperty);
        
        /* [id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Context )( 
            ICtp * This,
            /* [retval][out] */ BSTR *pstrProperty);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *PutSrc )( 
            ICtp * This,
            /* [in] */ BSTR strProperty);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *GetSrc )( 
            ICtp * This,
            /* [retval][out] */ BSTR *pstrProperty);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *PutContext )( 
            ICtp * This,
            /* [in] */ BSTR strProperty);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *GetContext )( 
            ICtp * This,
            /* [retval][out] */ BSTR *pstrProperty);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *GetVerse )( 
            ICtp * This,
            /* [in] */ BSTR strFilePath,
            /* [retval][out] */ BSTR *pstrProperty);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *GetAttributes )( 
            ICtp * This,
            /* [in] */ BSTR strFilePath,
            /* [retval][out] */ BSTR *pstrAttributes);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *ProcessImage )( 
            ICtp * This,
            /* [in] */ BSTR strFilePath,
            /* [in] */ int nMaxKB,
            /* [in] */ int nQuality,
            /* [retval][out] */ BSTR *pstrFilePathNew);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *DoCommand )( 
            ICtp * This,
            /* [in] */ BSTR strCommand,
            /* [retval][out] */ long *plResult);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *GetCommand )( 
            ICtp * This,
            /* [in] */ BSTR strCommand,
            /* [retval][out] */ long *plResult);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *FileDownload )( 
            ICtp * This,
            /* [in] */ BSTR bstrSrcUrl,
            /* [in] */ BSTR bstrInfo,
            /* [in] */ BSTR bstrOptionString);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *FYSTransfer )( 
            ICtp * This,
            /* [in] */ BSTR OrderId,
            /* [in] */ BSTR CorrelationId,
            /* [in] */ BSTR FYSInfo,
            /* [retval][out] */ BSTR *pstrError);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *GetUsageData )( 
            ICtp * This,
            /* [retval][out] */ BSTR *pstrUsageData);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *FYSClose )( 
            ICtp * This);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SetAddressbookUrl )( 
            ICtp * This,
            /* [in] */ VARIANT varUrl);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *ShowAddressbookPopup )( 
            ICtp * This,
            /* [in] */ VARIANT varShow);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *ResizeAddressbookPopup )( 
            ICtp * This,
            /* [in] */ VARIANT varWidth,
            /* [in] */ VARIANT varHeight);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *CloseAddressbookPopup )( 
            ICtp * This);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *ProcessAddressbook )( 
            ICtp * This,
            /* [in] */ VARIANT varData);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SetClipArtUrl )( 
            ICtp * This,
            /* [in] */ VARIANT varUrl);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *ShowClipArtPopup )( 
            ICtp * This,
            /* [in] */ VARIANT varShow);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *ResizeClipArtPopup )( 
            ICtp * This,
            /* [in] */ VARIANT varWidth,
            /* [in] */ VARIANT varHeight);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *CloseClipArtPopup )( 
            ICtp * This);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Version )( 
            ICtp * This,
            /* [retval][out] */ BSTR *pVal);
        
        END_INTERFACE
    } ICtpVtbl;

    interface ICtp
    {
        CONST_VTBL struct ICtpVtbl *lpVtbl;
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


#define ICtp_put_Src(This,strProperty)	\
    (This)->lpVtbl -> put_Src(This,strProperty)

#define ICtp_get_Src(This,pstrProperty)	\
    (This)->lpVtbl -> get_Src(This,pstrProperty)

#define ICtp_put_Context(This,strProperty)	\
    (This)->lpVtbl -> put_Context(This,strProperty)

#define ICtp_get_Context(This,pstrProperty)	\
    (This)->lpVtbl -> get_Context(This,pstrProperty)

#define ICtp_PutSrc(This,strProperty)	\
    (This)->lpVtbl -> PutSrc(This,strProperty)

#define ICtp_GetSrc(This,pstrProperty)	\
    (This)->lpVtbl -> GetSrc(This,pstrProperty)

#define ICtp_PutContext(This,strProperty)	\
    (This)->lpVtbl -> PutContext(This,strProperty)

#define ICtp_GetContext(This,pstrProperty)	\
    (This)->lpVtbl -> GetContext(This,pstrProperty)

#define ICtp_GetVerse(This,strFilePath,pstrProperty)	\
    (This)->lpVtbl -> GetVerse(This,strFilePath,pstrProperty)

#define ICtp_GetAttributes(This,strFilePath,pstrAttributes)	\
    (This)->lpVtbl -> GetAttributes(This,strFilePath,pstrAttributes)

#define ICtp_ProcessImage(This,strFilePath,nMaxKB,nQuality,pstrFilePathNew)	\
    (This)->lpVtbl -> ProcessImage(This,strFilePath,nMaxKB,nQuality,pstrFilePathNew)

#define ICtp_DoCommand(This,strCommand,plResult)	\
    (This)->lpVtbl -> DoCommand(This,strCommand,plResult)

#define ICtp_GetCommand(This,strCommand,plResult)	\
    (This)->lpVtbl -> GetCommand(This,strCommand,plResult)

#define ICtp_FileDownload(This,bstrSrcUrl,bstrInfo,bstrOptionString)	\
    (This)->lpVtbl -> FileDownload(This,bstrSrcUrl,bstrInfo,bstrOptionString)

#define ICtp_FYSTransfer(This,OrderId,CorrelationId,FYSInfo,pstrError)	\
    (This)->lpVtbl -> FYSTransfer(This,OrderId,CorrelationId,FYSInfo,pstrError)

#define ICtp_GetUsageData(This,pstrUsageData)	\
    (This)->lpVtbl -> GetUsageData(This,pstrUsageData)

#define ICtp_FYSClose(This)	\
    (This)->lpVtbl -> FYSClose(This)

#define ICtp_SetAddressbookUrl(This,varUrl)	\
    (This)->lpVtbl -> SetAddressbookUrl(This,varUrl)

#define ICtp_ShowAddressbookPopup(This,varShow)	\
    (This)->lpVtbl -> ShowAddressbookPopup(This,varShow)

#define ICtp_ResizeAddressbookPopup(This,varWidth,varHeight)	\
    (This)->lpVtbl -> ResizeAddressbookPopup(This,varWidth,varHeight)

#define ICtp_CloseAddressbookPopup(This)	\
    (This)->lpVtbl -> CloseAddressbookPopup(This)

#define ICtp_ProcessAddressbook(This,varData)	\
    (This)->lpVtbl -> ProcessAddressbook(This,varData)

#define ICtp_SetClipArtUrl(This,varUrl)	\
    (This)->lpVtbl -> SetClipArtUrl(This,varUrl)

#define ICtp_ShowClipArtPopup(This,varShow)	\
    (This)->lpVtbl -> ShowClipArtPopup(This,varShow)

#define ICtp_ResizeClipArtPopup(This,varWidth,varHeight)	\
    (This)->lpVtbl -> ResizeClipArtPopup(This,varWidth,varHeight)

#define ICtp_CloseClipArtPopup(This)	\
    (This)->lpVtbl -> CloseClipArtPopup(This)

#define ICtp_get_Version(This,pVal)	\
    (This)->lpVtbl -> get_Version(This,pVal)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [id][propput] */ HRESULT STDMETHODCALLTYPE ICtp_put_Src_Proxy( 
    ICtp * This,
    /* [in] */ BSTR strProperty);


void __RPC_STUB ICtp_put_Src_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propget] */ HRESULT STDMETHODCALLTYPE ICtp_get_Src_Proxy( 
    ICtp * This,
    /* [retval][out] */ BSTR *pstrProperty);


void __RPC_STUB ICtp_get_Src_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propput] */ HRESULT STDMETHODCALLTYPE ICtp_put_Context_Proxy( 
    ICtp * This,
    /* [in] */ BSTR strProperty);


void __RPC_STUB ICtp_put_Context_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propget] */ HRESULT STDMETHODCALLTYPE ICtp_get_Context_Proxy( 
    ICtp * This,
    /* [retval][out] */ BSTR *pstrProperty);


void __RPC_STUB ICtp_get_Context_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id] */ HRESULT STDMETHODCALLTYPE ICtp_PutSrc_Proxy( 
    ICtp * This,
    /* [in] */ BSTR strProperty);


void __RPC_STUB ICtp_PutSrc_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id] */ HRESULT STDMETHODCALLTYPE ICtp_GetSrc_Proxy( 
    ICtp * This,
    /* [retval][out] */ BSTR *pstrProperty);


void __RPC_STUB ICtp_GetSrc_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id] */ HRESULT STDMETHODCALLTYPE ICtp_PutContext_Proxy( 
    ICtp * This,
    /* [in] */ BSTR strProperty);


void __RPC_STUB ICtp_PutContext_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id] */ HRESULT STDMETHODCALLTYPE ICtp_GetContext_Proxy( 
    ICtp * This,
    /* [retval][out] */ BSTR *pstrProperty);


void __RPC_STUB ICtp_GetContext_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id] */ HRESULT STDMETHODCALLTYPE ICtp_GetVerse_Proxy( 
    ICtp * This,
    /* [in] */ BSTR strFilePath,
    /* [retval][out] */ BSTR *pstrProperty);


void __RPC_STUB ICtp_GetVerse_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id] */ HRESULT STDMETHODCALLTYPE ICtp_GetAttributes_Proxy( 
    ICtp * This,
    /* [in] */ BSTR strFilePath,
    /* [retval][out] */ BSTR *pstrAttributes);


void __RPC_STUB ICtp_GetAttributes_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id] */ HRESULT STDMETHODCALLTYPE ICtp_ProcessImage_Proxy( 
    ICtp * This,
    /* [in] */ BSTR strFilePath,
    /* [in] */ int nMaxKB,
    /* [in] */ int nQuality,
    /* [retval][out] */ BSTR *pstrFilePathNew);


void __RPC_STUB ICtp_ProcessImage_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id] */ HRESULT STDMETHODCALLTYPE ICtp_DoCommand_Proxy( 
    ICtp * This,
    /* [in] */ BSTR strCommand,
    /* [retval][out] */ long *plResult);


void __RPC_STUB ICtp_DoCommand_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id] */ HRESULT STDMETHODCALLTYPE ICtp_GetCommand_Proxy( 
    ICtp * This,
    /* [in] */ BSTR strCommand,
    /* [retval][out] */ long *plResult);


void __RPC_STUB ICtp_GetCommand_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id] */ HRESULT STDMETHODCALLTYPE ICtp_FileDownload_Proxy( 
    ICtp * This,
    /* [in] */ BSTR bstrSrcUrl,
    /* [in] */ BSTR bstrInfo,
    /* [in] */ BSTR bstrOptionString);


void __RPC_STUB ICtp_FileDownload_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id] */ HRESULT STDMETHODCALLTYPE ICtp_FYSTransfer_Proxy( 
    ICtp * This,
    /* [in] */ BSTR OrderId,
    /* [in] */ BSTR CorrelationId,
    /* [in] */ BSTR FYSInfo,
    /* [retval][out] */ BSTR *pstrError);


void __RPC_STUB ICtp_FYSTransfer_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id] */ HRESULT STDMETHODCALLTYPE ICtp_GetUsageData_Proxy( 
    ICtp * This,
    /* [retval][out] */ BSTR *pstrUsageData);


void __RPC_STUB ICtp_GetUsageData_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id] */ HRESULT STDMETHODCALLTYPE ICtp_FYSClose_Proxy( 
    ICtp * This);


void __RPC_STUB ICtp_FYSClose_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ICtp_SetAddressbookUrl_Proxy( 
    ICtp * This,
    /* [in] */ VARIANT varUrl);


void __RPC_STUB ICtp_SetAddressbookUrl_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ICtp_ShowAddressbookPopup_Proxy( 
    ICtp * This,
    /* [in] */ VARIANT varShow);


void __RPC_STUB ICtp_ShowAddressbookPopup_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ICtp_ResizeAddressbookPopup_Proxy( 
    ICtp * This,
    /* [in] */ VARIANT varWidth,
    /* [in] */ VARIANT varHeight);


void __RPC_STUB ICtp_ResizeAddressbookPopup_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ICtp_CloseAddressbookPopup_Proxy( 
    ICtp * This);


void __RPC_STUB ICtp_CloseAddressbookPopup_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ICtp_ProcessAddressbook_Proxy( 
    ICtp * This,
    /* [in] */ VARIANT varData);


void __RPC_STUB ICtp_ProcessAddressbook_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ICtp_SetClipArtUrl_Proxy( 
    ICtp * This,
    /* [in] */ VARIANT varUrl);


void __RPC_STUB ICtp_SetClipArtUrl_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ICtp_ShowClipArtPopup_Proxy( 
    ICtp * This,
    /* [in] */ VARIANT varShow);


void __RPC_STUB ICtp_ShowClipArtPopup_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ICtp_ResizeClipArtPopup_Proxy( 
    ICtp * This,
    /* [in] */ VARIANT varWidth,
    /* [in] */ VARIANT varHeight);


void __RPC_STUB ICtp_ResizeClipArtPopup_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ICtp_CloseClipArtPopup_Proxy( 
    ICtp * This);


void __RPC_STUB ICtp_CloseClipArtPopup_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE ICtp_get_Version_Proxy( 
    ICtp * This,
    /* [retval][out] */ BSTR *pVal);


void __RPC_STUB ICtp_get_Version_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __ICtp_INTERFACE_DEFINED__ */



#ifndef __AXCTP2Lib_LIBRARY_DEFINED__
#define __AXCTP2Lib_LIBRARY_DEFINED__

/* library AXCTP2Lib */
/* [helpstring][version][uuid] */ 


EXTERN_C const IID LIBID_AXCTP2Lib;

#ifndef ___ICtpEvents_DISPINTERFACE_DEFINED__
#define ___ICtpEvents_DISPINTERFACE_DEFINED__

/* dispinterface _ICtpEvents */
/* [helpstring][uuid] */ 


EXTERN_C const IID DIID__ICtpEvents;

#if defined(__cplusplus) && !defined(CINTERFACE)

    MIDL_INTERFACE("F0207328-6435-4d34-B213-6F7ECF81DD02")
    _ICtpEvents : public IDispatch
    {
    };
    
#else 	/* C style interface */

    typedef struct _ICtpEventsVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            _ICtpEvents * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            _ICtpEvents * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            _ICtpEvents * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            _ICtpEvents * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            _ICtpEvents * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            _ICtpEvents * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            _ICtpEvents * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        END_INTERFACE
    } _ICtpEventsVtbl;

    interface _ICtpEvents
    {
        CONST_VTBL struct _ICtpEventsVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define _ICtpEvents_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define _ICtpEvents_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define _ICtpEvents_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define _ICtpEvents_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define _ICtpEvents_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define _ICtpEvents_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define _ICtpEvents_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)

#endif /* COBJMACROS */


#endif 	/* C style interface */


#endif 	/* ___ICtpEvents_DISPINTERFACE_DEFINED__ */


EXTERN_C const CLSID CLSID_Ctp;

#ifdef __cplusplus

class DECLSPEC_UUID("BB383206-6DA1-4e80-B62A-3DF950FCC697")
Ctp;
#endif
#endif /* __AXCTP2Lib_LIBRARY_DEFINED__ */

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


