//
// npshell.cpp - Plug-in methods called from Netscape.
//

#include "stdafx.h"
#include <string.h>
#include "NpApi.h"
#include "Ctp.h"

#include "scappint.h"

CComModule _Module;

BEGIN_OBJECT_MAP(ObjectMap)
END_OBJECT_MAP()


BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID /*lpvReserved*/)
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


//
// NPP_Initialize
//
NPError NPP_Initialize(void)
{
	SCENG_Init();
	return NPERR_NO_ERROR;
}

//
// NPP_Shutdown
//
void NPP_Shutdown(void)
{
	CAGDC::Free();
	SCENG_Fini();
}


//
// NPP_New - Create a new plug-in instance.
//
NPError NP_LOADDS NPP_New(NPMIMEType /*pluginType*/, NPP pInstance, uint16 /*mode*/,
							int16 argc, char *argn[], char *argv[],
							NPSavedData * /*saved*/)
{	
	if (pInstance == NULL)
		return NPERR_INVALID_INSTANCE_ERROR;

	CCtp *pCtp = new CCtp();
	pCtp->SetNPPInstance(pInstance);
	pInstance->pdata = pCtp;

	for (int i = 0; i < argc; i++)
	{
		if (lstrcmpi(argn[i], "Fonts") == 0)
		{
			pCtp->SetFontURL(argv[i]);
			break;
		}
	}

	return NPERR_NO_ERROR;
}


//
// NPP_Destroy - Destroy our plug-in instance.
//
NPError NP_LOADDS NPP_Destroy(NPP pInstance, NPSavedData ** /*save*/)
{
	if (pInstance == NULL)
		return NPERR_INVALID_INSTANCE_ERROR;

	CCtp *pCtp = (CCtp *)pInstance->pdata;
	if (pCtp)
	{
		HWND hWnd = pCtp->UnsubclassWindow();
		BOOL bTemp;
		pCtp->OnDestroy(0, 0, 0, bTemp);
		delete pCtp;

		HWND hParent = ::GetParent(hWnd);
		LONG lStyle = ::GetWindowLong(hParent, GWL_STYLE);
		lStyle |= WS_CLIPCHILDREN;
		::SetWindowLong(hParent, GWL_STYLE, lStyle);
	}

	return NPERR_NO_ERROR;
}


//
// NPP_SetWindow - A window was created, resized, or destroyed.
//
NPError NP_LOADDS NPP_SetWindow(NPP pInstance, NPWindow *pNPWindow)
{	 
	if (pInstance == NULL)
		return NPERR_INVALID_INSTANCE_ERROR;
	CCtp *pCtp = (CCtp *)pInstance->pdata;

	if (pNPWindow == NULL || pCtp == NULL)
		return NPERR_GENERIC_ERROR;
	HWND hWnd = (HWND)(DWORD)pNPWindow->window;

	if (hWnd == NULL)
		return NPERR_NO_ERROR;

	if (hWnd != NULL && pCtp->m_hWnd == NULL)
	{
		HWND hParent = ::GetParent(hWnd);
		LONG lStyle = ::GetWindowLong(hParent, GWL_STYLE);
		lStyle &= ~WS_CLIPCHILDREN;
		::SetWindowLong(hParent, GWL_STYLE, lStyle);

		if (!pCtp->SubclassWindow(hWnd))
		{
			delete pCtp;
			pInstance->pdata = NULL;
			return NPERR_MODULE_LOAD_FAILED_ERROR;
		}
		BOOL bTemp;
		pCtp->OnCreate(0, 0, 0, bTemp);
		::InvalidateRect(pCtp->GetParent(), NULL, TRUE);
	}

	return NPERR_NO_ERROR;
}


//
// NPP_NewStream - A new stream was created.
//
NPError NP_LOADDS NPP_NewStream(NPP pInstance, NPMIMEType type, NPStream *stream,
								NPBool /*seekable*/, uint16 *stype)
{
	if (pInstance == NULL)
		return NPERR_INVALID_INSTANCE_ERROR;

	CCtp *pCtp = (CCtp *)pInstance->pdata;

	if (lstrcmpi(&stream->url[lstrlen(stream->url) - 3], "ctp") == 0)
		pCtp->FileStart();
	else
	if (lstrcmpi(&stream->url[lstrlen(stream->url) - 3], "ttz") == 0)
		pCtp->FontStart(&stream->url[lstrlen(stream->url) - 12]);

	*stype = NP_NORMAL;

	return NPERR_NO_ERROR;
}


//
// NPP_WriteReady - Returns amount of data we can handle for the next NPP_Write
//							   
int32 NP_LOADDS NPP_WriteReady(NPP pInstance, NPStream * /*stream*/)
{
	if (pInstance == NULL)
		return NPERR_INVALID_INSTANCE_ERROR;

	return 0x0FFFFFFF;
}


//
// NPP_Write - Here is some data. Return -1 to abort stream.
// 
int32 NP_LOADDS NPP_Write(NPP pInstance, NPStream *stream, int32 /*offset*/,
							int32 len, void *buffer)
{	
	if (pInstance == NULL)
		return NPERR_INVALID_INSTANCE_ERROR;

	CCtp *pCtp = (CCtp *)pInstance->pdata;
	if (lstrcmpi(&stream->url[lstrlen(stream->url) - 3], "ctp") == 0)
		pCtp->FileData((BYTE *)buffer, len);
	else
	if (lstrcmpi(&stream->url[lstrlen(stream->url) - 3], "ttz") == 0)
	{
		pCtp->FontData(&stream->url[lstrlen(stream->url) - 12], (BYTE *)buffer, len);
	}
	
	return len;
}


//
// NPP_DestroyStream - Stream is done, but audio may still be playing.
// 
NPError NP_LOADDS NPP_DestroyStream(NPP pInstance, NPStream *stream,
										NPError reason)
{
	if (pInstance == NULL)
		return NPERR_INVALID_INSTANCE_ERROR;

	if (reason == NPRES_DONE)
	{
		CCtp *pCtp = (CCtp *)pInstance->pdata;
		if (lstrcmpi(&stream->url[lstrlen(stream->url) - 3], "ctp") == 0)
			pCtp->FileEnd();
		else
		if (lstrcmpi(&stream->url[lstrlen(stream->url) - 3], "ttz") == 0)
			pCtp->FontEnd(&stream->url[lstrlen(stream->url) - 12]);
	}

	return NPERR_NO_ERROR;
}


//
// NPP_StreamAsFile - For file based plug-ins, not streaming.
// 
void NP_LOADDS NPP_StreamAsFile(NPP /*pInstance*/, NPStream * /*stream*/,
								const char * /*fname*/)
{
}


jref NP_LOADDS NPP_GetJavaClass(void)
{
	return NULL;
}


void NP_LOADDS NPP_Print(NPP /*pInstance*/, NPPrint * /*printInfo*/)
{
}


void NP_LOADDS NPP_URLNotify(NPP /* pInstance */, const char * /* url */,
  NPReason /* reason */, void * /* notifyData */)
{
}

