// RegCtrl.cpp: implementation of the CRegisterControl class.
//
//////////////////////////////////////////////////////////////////////

#include "STDAFX.H"
#include "RegCtrl.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CRegisterControl::CRegisterControl()
{
   m_hrRegisterErrorCode = 0;
}

CRegisterControl::~CRegisterControl()
{
}

CRegisterControl::ErrorCode CRegisterControl::Register(LPCSTR szFileName)
{
   HINSTANCE               hLibrary;
   FARPROC                 pProc = NULL;
   typedef HRESULT (STDAPICALLTYPE * RegisterProc) (void);
   RegisterProc            pRegisterProc;
   char                    szSystemDirectory[MAX_PATH];
   CString                 csLibToLoad;
   ErrorCode               errorCode;
   
   int nBytesNeeded = ::GetSystemDirectory(szSystemDirectory, sizeof(szSystemDirectory));
   ASSERT(nBytesNeeded > 0 && nBytesNeeded <= sizeof(szSystemDirectory));
   if(nBytesNeeded == 0 || nBytesNeeded > sizeof(szSystemDirectory))
   {
      ASSERT(0);
      return ecInternalError;
   }

   // If full path not supplied, build path to Windows System directory,
   // otherwise use supplied filename as is
   if(strchr(szFileName, '\\') == NULL)
   {
      csLibToLoad = szSystemDirectory;
      csLibToLoad += "\\";
      csLibToLoad += szFileName;
   }
   else
      csLibToLoad = szFileName;

   hLibrary = ::LoadLibrary((LPCSTR)csLibToLoad);
   if(hLibrary == NULL)
   {
      return ecLoadLibraryFailed;
   }

	pProc = GetProcAddress(hLibrary, "DllRegisterServer");
	if (pProc == NULL)
   {
      ::FreeLibrary(hLibrary);
      return ecRegisterProcNotFound;
   }

   pRegisterProc = (RegisterProc) pProc;
   // Call DllRegisterServer to register OCX control
   m_hrRegisterErrorCode = (*pRegisterProc)();
   if(m_hrRegisterErrorCode != S_OK)
      errorCode = ecRegisterFailed;
   else
      errorCode = ecNone;
   ::FreeLibrary(hLibrary);

   return errorCode;
}