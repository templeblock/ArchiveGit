

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


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

#ifndef ___ImageControlInterface_h__
#define ___ImageControlInterface_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

/* Forward Declarations */ 

#ifndef __IImageControl_FWD_DEFINED__
#define __IImageControl_FWD_DEFINED__
typedef interface IImageControl IImageControl;
#endif 	/* __IImageControl_FWD_DEFINED__ */


#ifndef ___IImageControlEvents_FWD_DEFINED__
#define ___IImageControlEvents_FWD_DEFINED__
typedef interface _IImageControlEvents _IImageControlEvents;
#endif 	/* ___IImageControlEvents_FWD_DEFINED__ */


#ifndef __CImageControl_FWD_DEFINED__
#define __CImageControl_FWD_DEFINED__

#ifdef __cplusplus
typedef class CImageControl CImageControl;
#else
typedef struct CImageControl CImageControl;
#endif /* __cplusplus */

#endif 	/* __CImageControl_FWD_DEFINED__ */


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

#ifndef __IImageControl_INTERFACE_DEFINED__
#define __IImageControl_INTERFACE_DEFINED__

/* interface IImageControl */
/* [unique][helpstring][dual][uuid][object] */ 


EXTERN_C const IID IID_IImageControl;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("750976F5-86CA-4D8F-9103-EA07FC9DB611")
    IImageControl : public IDispatch
    {
    public:
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE DoCommand( 
            /* [in] */ BSTR strCommand,
            /* [in] */ BSTR strValue,
            /* [retval][out] */ long *plResult) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE GetCommand( 
            /* [in] */ BSTR strCommand,
            /* [retval][out] */ long *plResult) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE SetImage( 
            /* [in] */ long *plDIB,
            /* [in] */ long *plMatrix,
            /* [retval][out] */ long *plResult) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE GetImage( 
            /* [out] */ long **pplDIB,
            /* [out] */ long **pplMatrix,
            /* [retval][out] */ long *plResult) = 0;
        
        virtual /* [id][requestedit][bindable][propget] */ HRESULT STDMETHODCALLTYPE get_HWND( 
            /* [retval][out] */ long *pHWND) = 0;
        
        virtual /* [id][requestedit][bindable][propput] */ HRESULT STDMETHODCALLTYPE put_BackColor( 
            /* [in] */ OLE_COLOR clr) = 0;
        
        virtual /* [id][requestedit][bindable][propget] */ HRESULT STDMETHODCALLTYPE get_BackColor( 
            /* [retval][out] */ OLE_COLOR *pclr) = 0;
        
        virtual /* [id][requestedit][bindable][propput] */ HRESULT STDMETHODCALLTYPE put_BackStyle( 
            /* [in] */ long style) = 0;
        
        virtual /* [id][requestedit][bindable][propget] */ HRESULT STDMETHODCALLTYPE get_BackStyle( 
            /* [retval][out] */ long *pstyle) = 0;
        
        virtual /* [id][requestedit][bindable][propput] */ HRESULT STDMETHODCALLTYPE put_BorderColor( 
            /* [in] */ OLE_COLOR clr) = 0;
        
        virtual /* [id][requestedit][bindable][propget] */ HRESULT STDMETHODCALLTYPE get_BorderColor( 
            /* [retval][out] */ OLE_COLOR *pclr) = 0;
        
        virtual /* [id][requestedit][bindable][propput] */ HRESULT STDMETHODCALLTYPE put_BorderStyle( 
            /* [in] */ long style) = 0;
        
        virtual /* [id][requestedit][bindable][propget] */ HRESULT STDMETHODCALLTYPE get_BorderStyle( 
            /* [retval][out] */ long *pstyle) = 0;
        
        virtual /* [id][requestedit][bindable][propput] */ HRESULT STDMETHODCALLTYPE put_BorderWidth( 
            /* [in] */ long width) = 0;
        
        virtual /* [id][requestedit][bindable][propget] */ HRESULT STDMETHODCALLTYPE get_BorderWidth( 
            /* [retval][out] */ long *width) = 0;
        
        virtual /* [id][requestedit][bindable][propput] */ HRESULT STDMETHODCALLTYPE put_BorderVisible( 
            /* [in] */ VARIANT_BOOL vbool) = 0;
        
        virtual /* [id][requestedit][bindable][propget] */ HRESULT STDMETHODCALLTYPE get_BorderVisible( 
            /* [retval][out] */ VARIANT_BOOL *pbool) = 0;
        
        virtual /* [id][requestedit][bindable][propput] */ HRESULT STDMETHODCALLTYPE put_Valid( 
            /* [in] */ VARIANT_BOOL vbool) = 0;
        
        virtual /* [id][requestedit][bindable][propget] */ HRESULT STDMETHODCALLTYPE get_Valid( 
            /* [retval][out] */ VARIANT_BOOL *pbool) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IImageControlVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IImageControl * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IImageControl * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IImageControl * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IImageControl * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IImageControl * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IImageControl * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IImageControl * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *DoCommand )( 
            IImageControl * This,
            /* [in] */ BSTR strCommand,
            /* [in] */ BSTR strValue,
            /* [retval][out] */ long *plResult);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *GetCommand )( 
            IImageControl * This,
            /* [in] */ BSTR strCommand,
            /* [retval][out] */ long *plResult);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *SetImage )( 
            IImageControl * This,
            /* [in] */ long *plDIB,
            /* [in] */ long *plMatrix,
            /* [retval][out] */ long *plResult);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *GetImage )( 
            IImageControl * This,
            /* [out] */ long **pplDIB,
            /* [out] */ long **pplMatrix,
            /* [retval][out] */ long *plResult);
        
        /* [id][requestedit][bindable][propget] */ HRESULT ( STDMETHODCALLTYPE *get_HWND )( 
            IImageControl * This,
            /* [retval][out] */ long *pHWND);
        
        /* [id][requestedit][bindable][propput] */ HRESULT ( STDMETHODCALLTYPE *put_BackColor )( 
            IImageControl * This,
            /* [in] */ OLE_COLOR clr);
        
        /* [id][requestedit][bindable][propget] */ HRESULT ( STDMETHODCALLTYPE *get_BackColor )( 
            IImageControl * This,
            /* [retval][out] */ OLE_COLOR *pclr);
        
        /* [id][requestedit][bindable][propput] */ HRESULT ( STDMETHODCALLTYPE *put_BackStyle )( 
            IImageControl * This,
            /* [in] */ long style);
        
        /* [id][requestedit][bindable][propget] */ HRESULT ( STDMETHODCALLTYPE *get_BackStyle )( 
            IImageControl * This,
            /* [retval][out] */ long *pstyle);
        
        /* [id][requestedit][bindable][propput] */ HRESULT ( STDMETHODCALLTYPE *put_BorderColor )( 
            IImageControl * This,
            /* [in] */ OLE_COLOR clr);
        
        /* [id][requestedit][bindable][propget] */ HRESULT ( STDMETHODCALLTYPE *get_BorderColor )( 
            IImageControl * This,
            /* [retval][out] */ OLE_COLOR *pclr);
        
        /* [id][requestedit][bindable][propput] */ HRESULT ( STDMETHODCALLTYPE *put_BorderStyle )( 
            IImageControl * This,
            /* [in] */ long style);
        
        /* [id][requestedit][bindable][propget] */ HRESULT ( STDMETHODCALLTYPE *get_BorderStyle )( 
            IImageControl * This,
            /* [retval][out] */ long *pstyle);
        
        /* [id][requestedit][bindable][propput] */ HRESULT ( STDMETHODCALLTYPE *put_BorderWidth )( 
            IImageControl * This,
            /* [in] */ long width);
        
        /* [id][requestedit][bindable][propget] */ HRESULT ( STDMETHODCALLTYPE *get_BorderWidth )( 
            IImageControl * This,
            /* [retval][out] */ long *width);
        
        /* [id][requestedit][bindable][propput] */ HRESULT ( STDMETHODCALLTYPE *put_BorderVisible )( 
            IImageControl * This,
            /* [in] */ VARIANT_BOOL vbool);
        
        /* [id][requestedit][bindable][propget] */ HRESULT ( STDMETHODCALLTYPE *get_BorderVisible )( 
            IImageControl * This,
            /* [retval][out] */ VARIANT_BOOL *pbool);
        
        /* [id][requestedit][bindable][propput] */ HRESULT ( STDMETHODCALLTYPE *put_Valid )( 
            IImageControl * This,
            /* [in] */ VARIANT_BOOL vbool);
        
        /* [id][requestedit][bindable][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Valid )( 
            IImageControl * This,
            /* [retval][out] */ VARIANT_BOOL *pbool);
        
        END_INTERFACE
    } IImageControlVtbl;

    interface IImageControl
    {
        CONST_VTBL struct IImageControlVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IImageControl_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IImageControl_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IImageControl_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IImageControl_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IImageControl_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IImageControl_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IImageControl_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IImageControl_DoCommand(This,strCommand,strValue,plResult)	\
    (This)->lpVtbl -> DoCommand(This,strCommand,strValue,plResult)

#define IImageControl_GetCommand(This,strCommand,plResult)	\
    (This)->lpVtbl -> GetCommand(This,strCommand,plResult)

#define IImageControl_SetImage(This,plDIB,plMatrix,plResult)	\
    (This)->lpVtbl -> SetImage(This,plDIB,plMatrix,plResult)

#define IImageControl_GetImage(This,pplDIB,pplMatrix,plResult)	\
    (This)->lpVtbl -> GetImage(This,pplDIB,pplMatrix,plResult)

#define IImageControl_get_HWND(This,pHWND)	\
    (This)->lpVtbl -> get_HWND(This,pHWND)

#define IImageControl_put_BackColor(This,clr)	\
    (This)->lpVtbl -> put_BackColor(This,clr)

#define IImageControl_get_BackColor(This,pclr)	\
    (This)->lpVtbl -> get_BackColor(This,pclr)

#define IImageControl_put_BackStyle(This,style)	\
    (This)->lpVtbl -> put_BackStyle(This,style)

#define IImageControl_get_BackStyle(This,pstyle)	\
    (This)->lpVtbl -> get_BackStyle(This,pstyle)

#define IImageControl_put_BorderColor(This,clr)	\
    (This)->lpVtbl -> put_BorderColor(This,clr)

#define IImageControl_get_BorderColor(This,pclr)	\
    (This)->lpVtbl -> get_BorderColor(This,pclr)

#define IImageControl_put_BorderStyle(This,style)	\
    (This)->lpVtbl -> put_BorderStyle(This,style)

#define IImageControl_get_BorderStyle(This,pstyle)	\
    (This)->lpVtbl -> get_BorderStyle(This,pstyle)

#define IImageControl_put_BorderWidth(This,width)	\
    (This)->lpVtbl -> put_BorderWidth(This,width)

#define IImageControl_get_BorderWidth(This,width)	\
    (This)->lpVtbl -> get_BorderWidth(This,width)

#define IImageControl_put_BorderVisible(This,vbool)	\
    (This)->lpVtbl -> put_BorderVisible(This,vbool)

#define IImageControl_get_BorderVisible(This,pbool)	\
    (This)->lpVtbl -> get_BorderVisible(This,pbool)

#define IImageControl_put_Valid(This,vbool)	\
    (This)->lpVtbl -> put_Valid(This,vbool)

#define IImageControl_get_Valid(This,pbool)	\
    (This)->lpVtbl -> get_Valid(This,pbool)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [id] */ HRESULT STDMETHODCALLTYPE IImageControl_DoCommand_Proxy( 
    IImageControl * This,
    /* [in] */ BSTR strCommand,
    /* [in] */ BSTR strValue,
    /* [retval][out] */ long *plResult);


void __RPC_STUB IImageControl_DoCommand_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id] */ HRESULT STDMETHODCALLTYPE IImageControl_GetCommand_Proxy( 
    IImageControl * This,
    /* [in] */ BSTR strCommand,
    /* [retval][out] */ long *plResult);


void __RPC_STUB IImageControl_GetCommand_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id] */ HRESULT STDMETHODCALLTYPE IImageControl_SetImage_Proxy( 
    IImageControl * This,
    /* [in] */ long *plDIB,
    /* [in] */ long *plMatrix,
    /* [retval][out] */ long *plResult);


void __RPC_STUB IImageControl_SetImage_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id] */ HRESULT STDMETHODCALLTYPE IImageControl_GetImage_Proxy( 
    IImageControl * This,
    /* [out] */ long **pplDIB,
    /* [out] */ long **pplMatrix,
    /* [retval][out] */ long *plResult);


void __RPC_STUB IImageControl_GetImage_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][requestedit][bindable][propget] */ HRESULT STDMETHODCALLTYPE IImageControl_get_HWND_Proxy( 
    IImageControl * This,
    /* [retval][out] */ long *pHWND);


void __RPC_STUB IImageControl_get_HWND_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][requestedit][bindable][propput] */ HRESULT STDMETHODCALLTYPE IImageControl_put_BackColor_Proxy( 
    IImageControl * This,
    /* [in] */ OLE_COLOR clr);


void __RPC_STUB IImageControl_put_BackColor_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][requestedit][bindable][propget] */ HRESULT STDMETHODCALLTYPE IImageControl_get_BackColor_Proxy( 
    IImageControl * This,
    /* [retval][out] */ OLE_COLOR *pclr);


void __RPC_STUB IImageControl_get_BackColor_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][requestedit][bindable][propput] */ HRESULT STDMETHODCALLTYPE IImageControl_put_BackStyle_Proxy( 
    IImageControl * This,
    /* [in] */ long style);


void __RPC_STUB IImageControl_put_BackStyle_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][requestedit][bindable][propget] */ HRESULT STDMETHODCALLTYPE IImageControl_get_BackStyle_Proxy( 
    IImageControl * This,
    /* [retval][out] */ long *pstyle);


void __RPC_STUB IImageControl_get_BackStyle_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][requestedit][bindable][propput] */ HRESULT STDMETHODCALLTYPE IImageControl_put_BorderColor_Proxy( 
    IImageControl * This,
    /* [in] */ OLE_COLOR clr);


void __RPC_STUB IImageControl_put_BorderColor_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][requestedit][bindable][propget] */ HRESULT STDMETHODCALLTYPE IImageControl_get_BorderColor_Proxy( 
    IImageControl * This,
    /* [retval][out] */ OLE_COLOR *pclr);


void __RPC_STUB IImageControl_get_BorderColor_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][requestedit][bindable][propput] */ HRESULT STDMETHODCALLTYPE IImageControl_put_BorderStyle_Proxy( 
    IImageControl * This,
    /* [in] */ long style);


void __RPC_STUB IImageControl_put_BorderStyle_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][requestedit][bindable][propget] */ HRESULT STDMETHODCALLTYPE IImageControl_get_BorderStyle_Proxy( 
    IImageControl * This,
    /* [retval][out] */ long *pstyle);


void __RPC_STUB IImageControl_get_BorderStyle_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][requestedit][bindable][propput] */ HRESULT STDMETHODCALLTYPE IImageControl_put_BorderWidth_Proxy( 
    IImageControl * This,
    /* [in] */ long width);


void __RPC_STUB IImageControl_put_BorderWidth_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][requestedit][bindable][propget] */ HRESULT STDMETHODCALLTYPE IImageControl_get_BorderWidth_Proxy( 
    IImageControl * This,
    /* [retval][out] */ long *width);


void __RPC_STUB IImageControl_get_BorderWidth_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][requestedit][bindable][propput] */ HRESULT STDMETHODCALLTYPE IImageControl_put_BorderVisible_Proxy( 
    IImageControl * This,
    /* [in] */ VARIANT_BOOL vbool);


void __RPC_STUB IImageControl_put_BorderVisible_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][requestedit][bindable][propget] */ HRESULT STDMETHODCALLTYPE IImageControl_get_BorderVisible_Proxy( 
    IImageControl * This,
    /* [retval][out] */ VARIANT_BOOL *pbool);


void __RPC_STUB IImageControl_get_BorderVisible_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][requestedit][bindable][propput] */ HRESULT STDMETHODCALLTYPE IImageControl_put_Valid_Proxy( 
    IImageControl * This,
    /* [in] */ VARIANT_BOOL vbool);


void __RPC_STUB IImageControl_put_Valid_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][requestedit][bindable][propget] */ HRESULT STDMETHODCALLTYPE IImageControl_get_Valid_Proxy( 
    IImageControl * This,
    /* [retval][out] */ VARIANT_BOOL *pbool);


void __RPC_STUB IImageControl_get_Valid_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IImageControl_INTERFACE_DEFINED__ */



#ifndef __ImageControlModule_LIBRARY_DEFINED__
#define __ImageControlModule_LIBRARY_DEFINED__

/* library ImageControlModule */
/* [helpstring][uuid][version] */ 


EXTERN_C const IID LIBID_ImageControlModule;

#ifndef ___IImageControlEvents_DISPINTERFACE_DEFINED__
#define ___IImageControlEvents_DISPINTERFACE_DEFINED__

/* dispinterface _IImageControlEvents */
/* [helpstring][uuid] */ 


EXTERN_C const IID DIID__IImageControlEvents;

#if defined(__cplusplus) && !defined(CINTERFACE)

    MIDL_INTERFACE("FE2FC027-8743-4F05-90F3-BBE79D22F4CE")
    _IImageControlEvents : public IDispatch
    {
    };
    
#else 	/* C style interface */

    typedef struct _IImageControlEventsVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            _IImageControlEvents * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            _IImageControlEvents * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            _IImageControlEvents * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            _IImageControlEvents * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            _IImageControlEvents * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            _IImageControlEvents * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            _IImageControlEvents * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        END_INTERFACE
    } _IImageControlEventsVtbl;

    interface _IImageControlEvents
    {
        CONST_VTBL struct _IImageControlEventsVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define _IImageControlEvents_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define _IImageControlEvents_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define _IImageControlEvents_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define _IImageControlEvents_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define _IImageControlEvents_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define _IImageControlEvents_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define _IImageControlEvents_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)

#endif /* COBJMACROS */


#endif 	/* C style interface */


#endif 	/* ___IImageControlEvents_DISPINTERFACE_DEFINED__ */


EXTERN_C const CLSID CLSID_CImageControl;

#ifdef __cplusplus

class DECLSPEC_UUID("266D1DBF-1107-4529-A48F-0FFBAE7351D1")
CImageControl;
#endif
#endif /* __ImageControlModule_LIBRARY_DEFINED__ */

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


