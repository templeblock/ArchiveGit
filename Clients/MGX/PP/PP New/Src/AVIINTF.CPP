//®PL1¯®FD1¯®TP0¯®BT0¯®PL1¯®RM255¯
// (c) Copyright 1991 MICROGRAFX, Inc., All Rights Reserved.
// This material is confidential and a trade secret.
// Permission to use this work for any purpose must be obtained
// in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081
#include "pp.h"
#include "id.h"
#include "aviintf.h"

// Provides the interface to ppavi.dll, because the current avi toolkit doesn't provide
// a load on call version
HMODULE   hPPAviDll = NULL;


//	from vfw.h Shold be called first
static LONG (FAR * dll_InitVFW)(void);
LONG VFWAPI InitVFW(void)
{
	return(TRUE);
//	return((*dll_InitVFW)());
}

//	from vfw.h Shold be called first
static LONG (FAR * dll_TermVFW)(void);
LONG VFWAPI TermVFW(void)
{
	return(TRUE);
//	return((*dll_TermVFW)());
}

//	from avifile.h Shold be followed by AVIFileExit
static void (STDAPICALLTYPE FAR * dll_AVIFileInit)(void);
STDAPI_(void) AVIFileInit(void)   // Call this first!
{
	(*dll_AVIFileInit)();
}

//	from avifile.h Shold call AVIFileInit first
static void (STDAPICALLTYPE FAR * dll_AVIFileExit)(void);
STDAPI_(void) AVIFileExit(void)
{
	(*dll_AVIFileExit)();
}

// 	from avifile.h Shold be wrapped with AVIFileInit, AVIFileExit
static  HRESULT (STDAPICALLTYPE  FAR * dll_AVIFileOpen)(PAVIFILE FAR * ppfile, LPCSTR szFile, UINT uMode, LPCLSID lpHandler);
STDAPI AVIFileOpen(PAVIFILE FAR * ppfile, LPCSTR szFile, UINT uMode, LPCLSID lpHandler)
{
	return((*dll_AVIFileOpen)((PAVIFILE FAR *)ppfile, (LPCSTR)szFile, (UINT)uMode,(LPCLSID)lpHandler));
}

//	from avifile.h Shold be wrapped with AVIFileInit, AVIFileExit
static ULONG (STDAPICALLTYPE FAR * dll_AVIFileRelease)(PAVIFILE pfile);
STDAPI_(ULONG) AVIFileRelease(PAVIFILE pfile)
{
	return((*dll_AVIFileRelease)((PAVIFILE)pfile));
}

//	from avifile.h Shold be wrapped with AVIFileInit, AVIFileExit
static HRESULT (STDAPICALLTYPE FAR * dll_AVIFileGetStream)(PAVIFILE pfile, PAVISTREAM FAR * ppavi, DWORD fccType, LONG lParam);
STDAPI AVIFileGetStream(PAVIFILE pfile, PAVISTREAM FAR * ppavi, DWORD fccType, LONG lParam)
{
	return((*dll_AVIFileGetStream)((PAVIFILE) pfile, (PAVISTREAM FAR *) ppavi, (DWORD) fccType, (LONG) lParam));
}

//	from avifile.h Shold be wrapped with AVIFileInit, AVIFileExit
static HRESULT (STDAPICALLTYPE FAR * dll_AVIFileInfo )(PAVIFILE pfile, AVIFILEINFO FAR * pfi, LONG lSize);
STDAPI AVIFileInfo (PAVIFILE pfile, AVIFILEINFO FAR * pfi, LONG lSize)
{
	return((*dll_AVIFileInfo)((PAVIFILE) pfile, (AVIFILEINFO FAR *) pfi, (LONG) lSize));
}

//	from avifile.h Shold be wrapped with AVIFileInit, AVIFileExit
static HRESULT (STDAPICALLTYPE FAR * dll_AVIFileCreateStream  )(PAVIFILE pfile, PAVISTREAM FAR *ppavi, AVISTREAMINFO FAR *psi);
STDAPI AVIFileCreateStream  (PAVIFILE pfile, PAVISTREAM FAR *ppavi, AVISTREAMINFO FAR *psi)
{
	return((*dll_AVIFileCreateStream)((PAVIFILE) pfile, (PAVISTREAM FAR *) ppavi, (AVISTREAMINFO FAR *) psi));
}

//	from avifile.h Shold be wrapped with AVIFileInit, AVIFileExit
static ULONG (STDAPICALLTYPE FAR * dll_AVIStreamRelease)(PAVISTREAM pavi);
STDAPI_(ULONG) AVIStreamRelease(PAVISTREAM pavi)
{
	return((*dll_AVIStreamRelease)((PAVISTREAM) pavi));
}

//	from avifile.h Shold be wrapped with AVIFileInit, AVIFileExit
static PGETFRAME (STDAPICALLTYPE FAR * dll_AVIStreamGetFrameOpen)(PAVISTREAM pavi, LPBITMAPINFOHEADER lpbiWanted);
STDAPI_(PGETFRAME) AVIStreamGetFrameOpen(PAVISTREAM pavi, LPBITMAPINFOHEADER lpbiWanted)
{
	return((*dll_AVIStreamGetFrameOpen)((PAVISTREAM) pavi, (LPBITMAPINFOHEADER) lpbiWanted));
}

//	from avifile.h Shold be wrapped with AVIFileInit, AVIFileExit
static LPVOID (STDAPICALLTYPE FAR * dll_AVIStreamGetFrame)(PGETFRAME pg, LONG lPos);
STDAPI_(LPVOID) AVIStreamGetFrame(PGETFRAME pg, LONG lPos)
{
	return((*dll_AVIStreamGetFrame)((PGETFRAME) pg, (LONG) lPos));
}

//	from avifile.h Shold be wrapped with AVIFileInit, AVIFileExit
static HRESULT (STDAPICALLTYPE FAR * dll_AVIStreamGetFrameClose)(PGETFRAME pg);
STDAPI AVIStreamGetFrameClose(PGETFRAME pg)
{
	return((*dll_AVIStreamGetFrameClose)((PGETFRAME) pg));
}

//	from avifile.h Shold be wrapped with AVIFileInit, AVIFileExit
static HRESULT (STDAPICALLTYPE FAR * dll_AVIStreamInfo )(PAVISTREAM pavi, AVISTREAMINFO FAR * psi, LONG lSize);
STDAPI AVIStreamInfo (PAVISTREAM pavi, AVISTREAMINFO FAR * psi, LONG lSize)
{
	return((*dll_AVIStreamInfo)((PAVISTREAM) pavi, (AVISTREAMINFO FAR *) psi, (LONG) lSize));
}

//	from avifile.h Shold be wrapped with AVIFileInit, AVIFileExit
static LONG (STDAPICALLTYPE FAR * dll_AVIStreamStart )(PAVISTREAM pavi);
STDAPI_(LONG) AVIStreamStart (PAVISTREAM pavi)
{
	return((*dll_AVIStreamStart)((PAVISTREAM) pavi));
}

//	from avifile.h Shold be wrapped with AVIFileInit, AVIFileExit
static HRESULT (STDAPICALLTYPE FAR * dll_AVIStreamSetFormat )(PAVISTREAM pavi, LONG lPos,LPVOID lpFormat,LONG cbFormat);
STDAPI AVIStreamSetFormat (PAVISTREAM pavi, LONG lPos,LPVOID lpFormat,LONG cbFormat)
{
	return((*dll_AVIStreamSetFormat)((PAVISTREAM) pavi, (LONG) lPos, (LPVOID) lpFormat, (LONG) cbFormat));
}

//	from avifile.h Shold be wrapped with AVIFileInit, AVIFileExit
static HRESULT (STDAPICALLTYPE FAR * dll_AVIStreamWrite )(PAVISTREAM pavi, LONG lStart, LONG lSamples, LPVOID lpBuffer, LONG cbBuffer, DWORD dwFlags, LONG FAR *plSampWritten, LONG FAR *plBytesWritten);
STDAPI AVIStreamWrite (PAVISTREAM pavi, LONG lStart, LONG lSamples, LPVOID lpBuffer, LONG cbBuffer, DWORD dwFlags, LONG FAR *plSampWritten, LONG FAR *plBytesWritten)
{
	return((*dll_AVIStreamWrite)((PAVISTREAM) pavi, (LONG) lStart, (LONG) lSamples, (LPVOID) lpBuffer, (LONG) cbBuffer, (DWORD) dwFlags, (LONG FAR *) plSampWritten, (LONG FAR *) plBytesWritten));
}

//	from avifile.h Shold be wrapped with AVIFileInit, AVIFileExit
static LONG (STDAPICALLTYPE FAR * dll_AVIStreamLength )(PAVISTREAM pavi);
STDAPI_(LONG) AVIStreamLength (PAVISTREAM pavi)
{
	return((*dll_AVIStreamLength)((PAVISTREAM) pavi));
}


BOOL LoadAviDLL()
{
	if (hPPAviDll)
		goto ExitError;
    hPPAviDll = (HINSTANCE)AstralLoadLibrary( "avifile.dll" );
	if (!hPPAviDll)
		goto ExitError;
//	dll_InitVFW = (LONG (FAR * )(void))
//		GetProcAddress(hPPAviDll, "InitVFW");
//	if (!dll_InitVFW) goto ExitError;
//	dll_TermVFW = (LONG (FAR * )(void))
//		GetProcAddress(hPPAviDll,"TermVFW");
//	if (!dll_TermVFW) goto ExitError;
dll_AVIFileInit = (void (STDAPICALLTYPE FAR * )(void))
	GetProcAddress(hPPAviDll, "AVIFileInit");
	if (!dll_AVIFileInit) goto ExitError;
dll_AVIFileExit = (void (STDAPICALLTYPE FAR * )(void))
	GetProcAddress(hPPAviDll, "AVIFileExit");
	if (!dll_AVIFileExit) goto ExitError;
dll_AVIFileOpen = (HRESULT (STDAPICALLTYPE FAR * )(PAVIFILE FAR * ppfile, LPCSTR szFile, UINT uMode, LPCLSID lpHandler))
	GetProcAddress(hPPAviDll, "AVIFileOpen");
	if (!dll_AVIFileOpen) goto ExitError;
dll_AVIFileRelease = (ULONG (STDAPICALLTYPE FAR * )(PAVIFILE pfile))
	GetProcAddress(hPPAviDll, "AVIFileRelease");
	if (!dll_AVIFileRelease) goto ExitError;
dll_AVIFileGetStream = (HRESULT (STDAPICALLTYPE FAR * )(PAVIFILE pfile, PAVISTREAM FAR * ppavi, DWORD fccType, LONG lParam))
	GetProcAddress(hPPAviDll, "AVIFileGetStream");
	if (!dll_AVIFileGetStream) goto ExitError;
dll_AVIFileInfo  = (HRESULT (STDAPICALLTYPE FAR * )(PAVIFILE pfile, AVIFILEINFO FAR * pfi, LONG lSize))
	GetProcAddress(hPPAviDll, "AVIFileInfo");
	if (!dll_AVIFileInfo) goto ExitError;
dll_AVIFileCreateStream   = (HRESULT (STDAPICALLTYPE FAR * )(PAVIFILE pfile, PAVISTREAM FAR *ppavi, AVISTREAMINFO FAR *psi))
	GetProcAddress(hPPAviDll, "AVIFileCreateStream");
	if (!dll_AVIFileCreateStream) goto ExitError;
dll_AVIStreamRelease = (ULONG (STDAPICALLTYPE FAR * )(PAVISTREAM pavi))
	GetProcAddress(hPPAviDll, "AVIStreamRelease");
	if (!dll_AVIStreamRelease) goto ExitError;
dll_AVIStreamGetFrameOpen = (PGETFRAME (STDAPICALLTYPE FAR * )(PAVISTREAM pavi, LPBITMAPINFOHEADER lpbiWanted))
	GetProcAddress(hPPAviDll, "AVIStreamGetFrameOpen");
	if (!dll_AVIStreamGetFrameOpen) goto ExitError;
dll_AVIStreamGetFrame = (LPVOID (STDAPICALLTYPE FAR * )(PGETFRAME pg, LONG lPos))
	GetProcAddress(hPPAviDll, "AVIStreamGetFrame");
	if (!dll_AVIStreamGetFrame) goto ExitError;
dll_AVIStreamGetFrameClose = (HRESULT (STDAPICALLTYPE FAR * )(PGETFRAME pg))
	GetProcAddress(hPPAviDll, "AVIStreamGetFrameClose");
	if (!dll_AVIStreamGetFrameClose) goto ExitError;
dll_AVIStreamInfo  = (HRESULT (STDAPICALLTYPE FAR * )(PAVISTREAM pavi, AVISTREAMINFO FAR * psi, LONG lSize))
	GetProcAddress(hPPAviDll, "AVIStreamInfo");
	if (!dll_AVIStreamInfo) goto ExitError;
dll_AVIStreamStart  = (LONG (STDAPICALLTYPE FAR * )(PAVISTREAM pavi))
	GetProcAddress(hPPAviDll, "AVIStreamStart");
	if (!dll_AVIStreamStart) goto ExitError;
dll_AVIStreamSetFormat  = (HRESULT (STDAPICALLTYPE FAR * )(PAVISTREAM pavi, LONG lPos,LPVOID lpFormat,LONG cbFormat))
	GetProcAddress(hPPAviDll, "AVIStreamSetFormat");
	if (!dll_AVIStreamSetFormat) goto ExitError;
dll_AVIStreamWrite  = (HRESULT (STDAPICALLTYPE FAR * )(PAVISTREAM pavi, LONG lStart, LONG lSamples, LPVOID lpBuffer, LONG cbBuffer, DWORD dwFlags, LONG FAR *plSampWritten, LONG FAR *plBytesWritten))
	GetProcAddress(hPPAviDll, "AVIStreamWrite");
	if (!dll_AVIStreamWrite) goto ExitError;
dll_AVIStreamLength  = (LONG (STDAPICALLTYPE FAR * )(PAVISTREAM pavi))
	GetProcAddress(hPPAviDll,  "AVIStreamLength");
	if (!dll_AVIStreamLength) goto ExitError;

	if (!InitVFW())
		goto ExitError;
	AVIFileInit();
		
	return(TRUE);			

ExitError:

	if (hPPAviDll)
		FreeLibrary( hPPAviDll );
	hPPAviDll = NULL;
	Message(IDS_ENOAVI);
	return(FALSE);
}


void FreAviDLL()
{			
	if (!hPPAviDll)
		return;
	if (dll_AVIFileExit)
		AVIFileExit();
	if (dll_TermVFW)
		TermVFW();
	FreeLibrary( hPPAviDll );
	hPPAviDll = NULL;
}

