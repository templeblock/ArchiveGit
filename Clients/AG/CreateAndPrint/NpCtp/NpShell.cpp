// npshell.cpp - Plug-in methods called from Netscape.  Derived from WinTemp.c

#include "stdafx.h"
#include "Ctp.h"
#include "scappint.h"
#include "NpCtp.h"

//j#define XPCONNECT

CComModule _Module;

BEGIN_OBJECT_MAP(ObjectMap)
END_OBJECT_MAP()

/////////////////////////////////////////////////////////////////////////////
BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
	switch (fdwReason)
	{
		case DLL_PROCESS_ATTACH:
			_Module.Init(ObjectMap, hinstDLL);
			break;
		
		case DLL_PROCESS_DETACH:
			_Module.Term();
			break;

		case DLL_THREAD_ATTACH:
		case DLL_THREAD_DETACH:
			break;
	}

	return TRUE;
}

#ifdef NOTUSED //j
		#include "jni.h"
		#include "npapi.h"

		#ifndef XPCONNECT
			#define IMPLEMENT_MozAxPlugin
//j			#include "MozAxPlugin.h"
			#include "netscape_plugin_Plugin.h"
		#endif XPCONNECT

//j		#include "nsISupports.h"

		// Plugin types supported
		enum PluginInstanceType
		{
			itScript,
			itControl
		};

		// Data associated with a plugin instance
		struct PluginInstanceData {
			PluginInstanceType nType;
			union
			{
				int i;
//j				CActiveScriptSiteInstance *pScriptSite;
//j				CControlSiteInstance *pControlSite;
			};
		#ifdef XPCONNECT
//j			nsISupports *pScriptingPeer;
		#endif
		};
#endif NOTUSED //j


/////////////////////////////////////////////////////////////////////////////
NPError NPP_Initialize(void)
{ // called immediately after the plugin DLL is loaded
	_Module.Lock(); //x
	SCENG_Init();
	::InitCommonControls();
	return NPERR_NO_ERROR;
}

/////////////////////////////////////////////////////////////////////////////
void NPP_Shutdown(void)
{ // called immediately before the plugin DLL is unloaded.
	SCENG_Fini();

	JRIEnv* env = NPN_GetJavaEnv();
	if (env)
	{
		unuse_NpCtp(env); 
		unuse_netscape_plugin_Plugin(env);
	}

	_Module.Unlock(); //x
}


/////////////////////////////////////////////////////////////////////////////
jref NPP_GetJavaClass(void)
{
#ifndef XPCONNECT
	JRIEnv* env = NPN_GetJavaEnv(); 
	if (!env) 
		return NULL; // Java is disabled

	// This plug-in uses the Java Runtime Interface (JRI)
	struct java_lang_Class* myClass = use_NpCtp(env);
	use_netscape_plugin_Plugin(env); 
	return (jref)myClass;
#endif XPCONNECT

    return NULL;
}


/////////////////////////////////////////////////////////////////////////////
// Create a new plug-in instance.
NPError NPP_New(NPMIMEType pluginType, NPP pInstance, uint16 mode, int16 argc, char* argn[], char* argv[], NPSavedData* saved)
{	
	if (!pInstance)
		return NPERR_INVALID_INSTANCE_ERROR;

	CCtp* pCtp = new CCtp();
	if (!pCtp)
		return NPERR_GENERIC_ERROR;

	pCtp->SetNPPInstance(pInstance);
	pInstance->pdata = pCtp;

	for (int i = 0; i < argc; i++)
	{
		if (!lstrcmpi(argn[i], "Src"))
		{
			pCtp->PutSrcURL(CComBSTR(argv[i]));
		}
		else
		if (!lstrcmpi(argn[i], "Context"))
		{
			pCtp->PutContext(CComBSTR(argv[i]));
		}
	}

	return NPERR_NO_ERROR;
}


/////////////////////////////////////////////////////////////////////////////
// Destroy our plug-in instance.
NPError NPP_Destroy(NPP pInstance, NPSavedData** save)
{
	if (!pInstance)
		return NPERR_INVALID_INSTANCE_ERROR;

	CCtp* pCtp = (CCtp*)pInstance->pdata;
	if (!pCtp)
		return NPERR_GENERIC_ERROR;

	BOOL bHandled = true;
	pCtp->OnDestroy(0, 0, 0, bHandled);

	if (pCtp->m_hWnd)
		HWND hWnd = pCtp->UnsubclassWindow();
	delete pCtp;

	return NPERR_NO_ERROR;
}


/////////////////////////////////////////////////////////////////////////////
//  Associates a platform specific window handle with a plug-in instance.
//  Called multiple times while, e.g., scrolling.  Can be called for three
//  reasons:
//
//            1.  A new window has been created
//            2.  A window has been moved or resized
//            3.  A window has been destroyed
//
//  There is also the degenerate case;  that it was called spuriously, and
//  the window handle and or coords may have or have not changed, or
//  the window handle and or coords may be ZERO.  State information
//  must be maintained by the plug-in to correctly handle the degenerate
//  case.
NPError NPP_SetWindow(NPP pInstance, NPWindow* pNPWindow)
{	 
	if (!pInstance)
		return NPERR_INVALID_INSTANCE_ERROR;

	if (!pNPWindow)
		return NPERR_GENERIC_ERROR;

	CCtp* pCtp = (CCtp*)pInstance->pdata;
	if (!pCtp)
		return NPERR_GENERIC_ERROR;

	HWND hWnd = (HWND)(DWORD)pNPWindow->window;
	if (!hWnd)
		return NPERR_NO_ERROR;

	if (pCtp->m_hWnd)
		return NPERR_NO_ERROR;

	if (!pCtp->SubclassWindow(hWnd))
	{
		delete pCtp;
		pInstance->pdata = NULL;
		return NPERR_MODULE_LOAD_FAILED_ERROR;
	}

	BOOL bHandled = true;
	pCtp->OnCreate(0, 0, 0, bHandled);
	::InvalidateRect(pCtp->GetParent(), NULL, true);

	return NPERR_NO_ERROR;
}


/////////////////////////////////////////////////////////////////////////////
//  Notifies the plugin of a new data stream.
//  The data type of the stream (a MIME name) is provided.
//  The stream object indicates whether it is seekable.
//  The plugin specifies how it wants to handle the stream.
//
//  In this case, I set the streamtype to be NPAsFile.  This tells the Navigator
//  that the plugin doesn't handle streaming and can only deal with the object as
//  a complete disk file.  It will still call the write functions but it will also
//  pass the filename of the cached file in a later NPE_StreamAsFile call when it
//  is done transfering the file.
//
//  If a plugin handles the data in a streaming manner, it should set streamtype to
//  NPNormal  (e.g. *streamtype = NPNormal)...the NPE_StreamAsFile function will
//  never be called in this case
NPError NPP_NewStream(NPP pInstance, NPMIMEType type, NPStream* stream, NPBool seekable, uint16* stype)
{
	if (!pInstance)
		return NPERR_INVALID_INSTANCE_ERROR;

	if (!stream)
		return NPERR_GENERIC_ERROR;

	CCtp* pCtp = (CCtp*)pInstance->pdata;
	if (!pCtp)
		return NPERR_GENERIC_ERROR;

	CString strSrcURL = stream->url;

	if (stype)
		*stype = NP_NORMAL;

	return NPERR_NO_ERROR;
}


/////////////////////////////////////////////////////////////////////////////
// These next 2 functions are really only directly relevant 
// in a plug-in which handles the data in a streaming manner.  
// For a NPAsFile stream, they are still called but can safely 
// be ignored.
//
// In a streaming plugin, all data handling would take place here...
// Returns amount of data we can handle for the next NPP_Write
// The number of bytes that a plug-in is willing to accept in a subsequent
// NPO_Write call.
int32 NPP_WriteReady(NPP pInstance, NPStream* stream)
{
	if (!pInstance)
		return -1;

	CCtp* pCtp = (CCtp*)pInstance->pdata;
	if (!pCtp)
		return -1;

	return 0x0FFFFFFF;
}


/////////////////////////////////////////////////////////////////////////////
// Here is some data. Return -1 to abort stream.
int32 NPP_Write(NPP pInstance, NPStream* stream, int32 offset, int32 len, void* buffer)
{	
	if (!pInstance)
		return -1;

	if (!stream)
		return -1;

	CCtp* pCtp = (CCtp*)pInstance->pdata;
	if (!pCtp)
		return -1;

	if (!buffer)
		return 0;

	pCtp->GetDownload().FileData((BYTE*)buffer, len);
	return len;
}


/////////////////////////////////////////////////////////////////////////////
// Closes a stream object.  
// reason indicates why the stream was closed.  Possible reasons are
// that it was complete, because there was some error, or because 
// the user aborted it.
NPError NPP_DestroyStream(NPP pInstance, NPStream* stream, NPError reason)
{
	if (!pInstance)
		return NPERR_INVALID_INSTANCE_ERROR;

	if (!stream)
		return NPERR_GENERIC_ERROR;

	if (reason != NPRES_DONE)
		return NPERR_NO_ERROR;

	CCtp* pCtp = (CCtp*)pInstance->pdata;
	if (!pCtp)
		return NPERR_GENERIC_ERROR;

	pCtp->GetDownload().FileEnd();
	return NPERR_NO_ERROR;
}


/////////////////////////////////////////////////////////////////////////////
//  The stream is done transferring and here is a pointer to the file in the cache
//  This function is only called if the streamtype was set to NPAsFile.
void NPP_StreamAsFile(NPP pInstance, NPStream* stream, const char* fname)
{
}


/////////////////////////////////////////////////////////////////////////////
// This plug-in does not print.
void NPP_Print(NPP pInstance, NPPrint* printInfo)
{
}


/////////////////////////////////////////////////////////////////////////////
// Notifies the instance of the completion of a URL request. 
// 
// NPP_URLNotify is called when Netscape completes a NPN_GetURLNotify or
// NPN_PostURLNotify request, to inform the plug-in that the request,
// identified by url, has completed for the reason specified by reason. The most
// common reason code is NPRES_DONE, indicating simply that the request
// completed normally. Other possible reason codes are NPRES_USER_BREAK,
// indicating that the request was halted due to a user action (for example,
// clicking the "Stop" button), and NPRES_NETWORK_ERR, indicating that the
// request could not be completed (for example, because the URL could not be
// found). The complete list of reason codes is found in npapi.h. 
// 
// The parameter notifyData is the same plug-in-private value passed as an
// argument to the corresponding NPN_GetURLNotify or NPN_PostURLNotify
// call, and can be used by your plug-in to uniquely identify the request. 
void NPP_URLNotify(NPP pInstance, const char* url, NPReason reason, void* notifyData)
{
}

#ifdef XPCONNECT
//j	#include "nsIMozAxPlugin.h"
//j	#include "nsIClassInfo.h"

	// We must implement nsIClassInfo because it signals the
	// Mozilla Security Manager to allow calls from JavaScript.

	// helper class to implement all necessary nsIClassInfo method stubs
	// and to set flags used by the security system
	class nsClassInfoMozAxPlugin : public nsIClassInfo
	{
	// These flags are used by the DOM and security systems to signal that 
	// JavaScript callers are allowed to call this object's scritable methods.
	NS_IMETHOD GetFlags(PRUint32 *aFlags)
		{*aFlags = nsIClassInfo::PLUGIN_OBJECT | nsIClassInfo::DOM_OBJECT;
		return NS_OK;}
	NS_IMETHOD GetImplementationLanguage(PRUint32 *aImplementationLanguage)
		{*aImplementationLanguage = nsIProgrammingLanguage::CPLUSPLUS;
		return NS_OK;}

	// The rest of the methods can safely return error codes...
	NS_IMETHOD GetInterfaces(PRUint32 *count, nsIID * **array)
		{return NS_ERROR_NOT_IMPLEMENTED;}
	NS_IMETHOD GetHelperForLanguage(PRUint32 language, nsISupports **_retval)
		{return NS_ERROR_NOT_IMPLEMENTED;}
	NS_IMETHOD GetContractID(char * *aContractID)
		{return NS_ERROR_NOT_IMPLEMENTED;}
	NS_IMETHOD GetClassDescription(char * *aClassDescription)
		{return NS_ERROR_NOT_IMPLEMENTED;}
	NS_IMETHOD GetClassID(nsCID * *aClassID)
		{return NS_ERROR_NOT_IMPLEMENTED;}
	NS_IMETHOD GetClassIDNoAlloc(nsCID *aClassIDNoAlloc)
		{return NS_ERROR_NOT_IMPLEMENTED;}
	};

	class nsScriptablePeer : public nsIMozAxPlugin,
							public nsClassInfoMozAxPlugin
	{
		long mRef;

	protected:
		virtual ~nsScriptablePeer();

	public:
		nsScriptablePeer();

		NS_DECL_ISUPPORTS
		NS_DECL_NSIMOZAXPLUGIN
	};

	nsScriptablePeer::nsScriptablePeer()
	{
		mRef = 0;
	}

	nsScriptablePeer::~nsScriptablePeer()
	{
	}

	///////////////////////////////////////////////////////////////////////////////
	// nsISupports

	// NOTE: We're not using the helpful NS_IMPL_ISUPPORTS macro because they drag
	//       in dependencies to xpcom that a plugin like ourselves is better off
	//       without.

	/* void QueryInterface (in nsIIDRef uuid, [iid_is (uuid), retval] out nsQIResult result); */
	NS_IMETHODIMP nsScriptablePeer::QueryInterface(const nsIID & uuid, void * *result)
	{
		if (uuid.Equals(NS_GET_IID(nsISupports)))
		{
			*result = NS_STATIC_CAST(void *, this);
			AddRef();
			return NS_OK;
		}
		else if (uuid.Equals(NS_GET_IID(nsIMozAxPlugin)))
		{
			*result = NS_STATIC_CAST(void *, this);
			AddRef();
			return NS_OK;
		}
		return NS_NOINTERFACE;
	}

	/* [noscript, notxpcom] nsrefcnt AddRef (); */
	NS_IMETHODIMP_(nsrefcnt) nsScriptablePeer::AddRef()
	{
		mRef++;
		return NS_OK;
	}

	/* [noscript, notxpcom] nsrefcnt Release (); */
	NS_IMETHODIMP_(nsrefcnt) nsScriptablePeer::Release()
	{
		mRef--;
		if (mRef <= 0)
		{
			delete this;
		}
		return NS_OK;
	}

	///////////////////////////////////////////////////////////////////////////////
	// nsIMozAxPlugin

	// the following method will be callable from JavaScript
	NS_IMETHODIMP nsScriptablePeer::X()
	{
		return NS_OK;
	}

	// Happy happy fun fun - redefine some NPPVariable values that we might
	// be asked for but not defined by the PluginSDK 
	const int kVarScriptableInstance = 10; // NPPVpluginScriptableInstance
	const int kVarScriptableIID = 11; // NPPVpluginScriptableIID

	NPError	NPP_GetValue(NPP pInstance, NPPVariable variable, void* value)
	{
		if (!pInstance)
			return NPERR_INVALID_INSTANCE_ERROR;

		if (variable == kVarScriptableInstance)
		{
			CCtp* pCtp = (CCtp*)pInstance->pdata;
			// nsITestPlugin interface object should be associated with the plugin instance itself
			// see if this is the first time and we haven't created it yet
			if (!pCtp->scriptablePeer)
			{
				nsScriptablePeer *peer  = new nsScriptablePeer();
				NS_ADDREF(peer); // peer->AddRef();
				pCtp->scriptablePeer = peer;
				// don't forget to release on shutdown to trigger its destruction
			}
			if (pCtp->scriptablePeer)
			{
				// add reference for the caller requesting the object
				NS_ADDREF(pCtp->scriptablePeer);
				*(nsISupports**)value = pCtp->scriptablePeer;
			}
		}
		else
		if (variable == kVarScriptableIID)
		{
			nsIID *piid = (nsIID*)NPN_MemAlloc(sizeof(nsIID));
			*piid = NS_IMOZAXPLUGIN_IID;
			*((nsIID**) value) = piid;
			return NPERR_NO_ERROR;
		}

		return NPERR_GENERIC_ERROR;
	}

	NPError	NPP_SetValue(NPP instance, NPNVariable variable, void *value)
	{
		return NPERR_GENERIC_ERROR;
	}
#endif XPCONNECT

#ifndef XPCONNECT
//j	#include "_stubs/MozAxPlugin.c"
#endif XPCONNECT

