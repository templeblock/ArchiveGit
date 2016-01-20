// npshell.cpp - Plug-in methods called from Netscape.  Derived from WinTemp.c

#include "stdafx.h"
#include "Plugin.h"

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

/////////////////////////////////////////////////////////////////////////////
NPError NPP_Initialize(void)
{ // called immediately after the plugin DLL is loaded
	return NPERR_NO_ERROR;
}

/////////////////////////////////////////////////////////////////////////////
void NPP_Shutdown(void)
{ // called immediately before the plugin DLL is unloaded.
}

/////////////////////////////////////////////////////////////////////////////
jref NPP_GetJavaClass(void)
{
	return NULL;
}

/////////////////////////////////////////////////////////////////////////////
// Create a new plug-in instance.
NPError NPP_New(NPMIMEType pluginType, NPP pInstance, uint16 mode, int16 argc, char* argn[], char* argv[], NPSavedData* saved)
{	
	if (!pInstance)
		return NPERR_INVALID_INSTANCE_ERROR;

	CPlugin* pPlugin = new CPlugin();
	if (!pPlugin)
		return NPERR_GENERIC_ERROR;

	pPlugin->SetNPPInstance(pInstance);
	pInstance->pdata = pPlugin;

	for (int i = 0; i < argc; i++)
	{
		if (!lstrcmpi(argn[i], "srcurl"))
		{
			pPlugin->PutSrc(CComBSTR(argv[i]));
		}
		else
		if (!lstrcmpi(argn[i], "url"))
		{
			pPlugin->PutURL(CComBSTR(argv[i]));
		}
		else
		if (!lstrcmpi(argn[i], "context"))
		{
			pPlugin->PutContext(CComBSTR(argv[i]));
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

	CPlugin* pPlugin = (CPlugin*)pInstance->pdata;
	if (!pPlugin)
		return NPERR_GENERIC_ERROR;

	BOOL bHandled = true;
	pPlugin->OnDestroy(0, 0, 0, bHandled);

	if (pPlugin->m_hWnd)
		HWND hWnd = pPlugin->UnsubclassWindow();
	delete pPlugin;

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

	CPlugin* pPlugin = (CPlugin*)pInstance->pdata;
	if (!pPlugin)
		return NPERR_GENERIC_ERROR;

	HWND hWnd = (HWND)(DWORD)pNPWindow->window;
	if (!hWnd)
		return NPERR_NO_ERROR;

	if (pPlugin->m_hWnd)
		return NPERR_NO_ERROR;

	if (!pPlugin->SubclassWindow(hWnd))
	{
		delete pPlugin;
		pInstance->pdata = NULL;
		return NPERR_MODULE_LOAD_FAILED_ERROR;
	}

	BOOL bHandled = true;
	pPlugin->OnCreate(0, 0, 0, bHandled);
	::InvalidateRect(pPlugin->GetParent(), NULL, true);

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

	CPlugin* pPlugin = (CPlugin*)pInstance->pdata;
	if (!pPlugin)
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

	CPlugin* pPlugin = (CPlugin*)pInstance->pdata;
	if (!pPlugin)
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

	CPlugin* pPlugin = (CPlugin*)pInstance->pdata;
	if (!pPlugin)
		return -1;

	if (!buffer)
		return 0;

	pPlugin->GetDownload().FileData((BYTE*)buffer, len);
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

	CPlugin* pPlugin = (CPlugin*)pInstance->pdata;
	if (!pPlugin)
		return NPERR_GENERIC_ERROR;

	pPlugin->GetDownload().FileEnd();
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
