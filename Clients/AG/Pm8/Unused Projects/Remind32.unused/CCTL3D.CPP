/*
// $Workfile: CCTL3D.CPP $
// $Revision: 1 $
// $Date: 3/03/99 6:26p $
*/

/*
// Revision History:
//
// $Log: /PM8/REMIND32/CCTL3D.CPP $
// 
// 1     3/03/99 6:26p Gbeddow
// 
// 1     6/22/98 10:03a Mwilson
// 
//    Rev 1.0   25 Apr 1997 09:24:22   Fred
// Initial revision.
// 
//    Rev 1.0   20 Feb 1997 13:55:44   Fred
// Initial revision.
// 
//    Rev 1.0   14 Mar 1996 13:25:46   Jay
// Initial revision.
// 
//    Rev 1.0   16 Feb 1996 12:27:06   FRED
// Initial revision.
// 
//    Rev 2.3   26 Dec 1995 10:29:10   FRED
// Project structure changes for mac port
// 
//    Rev 2.2   29 Mar 1995 09:15:24   JAY
// Lots of changes
// 
//    Rev 2.1   08 Feb 1995 13:34:24   JAY
// Reverted. New series.
// 
//    Rev 1.4   04 Aug 1994 08:30:54   JAY
// Large model and 32-bit changes
// 
//    Rev 1.3   22 Jun 1994 10:39:10   FRED
// Uses CTL3DV2.DLL (or CTL3D32.DLL is WIN32 is defined).
// 
// 
//    Rev 1.2   22 Jun 1994 08:26:54   JAY
// Moved include of "ctl3d.h" from CCTL3D.H to CCTL3D.CPP since it wasn't
// needed in the class definition.
// 
//    Rev 1.1   22 Jun 1994 08:22:10   JAY
// Added separate Startup(). Added GetEntryPoints(). Made Shutdown() callable from
// within GetEntryPoints().
// 
//    Rev 1.0   22 Jun 1994 07:11:54   FRED
// Initial revision.
*/ 

#include "stdafx.h"
#include "util.h"
#include	"cctl3d.h"
#include	"ctl3d.h"
#include <dos.h>

#ifdef WIN32
#define	CTL3DNAME	"CTL3D32.DLL"
#else
#define	CTL3DNAME	"CTL3DV2.DLL"
#endif

CControl3D::CControl3D(void)
{
	m_fRegistered = FALSE;
	m_hInstance = NULL;
	m_hCtl3d = NULL;
	m_lpCtl3dRegister = NULL;
	m_lpCtl3dUnregister = NULL;
	m_lpCtl3dAutoSubclass = NULL;
	m_lpCtl3dColorChange = NULL;
	m_lpCtl3dSubclassCtl = NULL;
	m_lpCtl3dSubclassDlgEx = NULL;
}

CControl3D::~CControl3D(void)
{
	Shutdown();
}             
	
BOOL CControl3D::Startup(HINSTANCE hInstance)
{
	Shutdown();
	
	// if Windows 4.0, use its built-in 3D stuff

	if (IsWindows4())
	{
		return TRUE;
	}

	// Windows 3.x, use CTL3DV2.DLL
		
	m_hInstance = hInstance;
	
	// check for an existing CTL3DV2.DLL

	UINT wOldSem = SetErrorMode(SEM_NOOPENFILEERRORBOX);
			
	if ((m_hCtl3d = LoadLibrary(CTL3DNAME)) < (HINSTANCE)HINSTANCE_ERROR)
	{
		m_hCtl3d = NULL;
	}

	SetErrorMode(wOldSem);
	
	GetEntryPoints();
	
	if (IsValid())
	{
		if ((!(*m_lpCtl3dRegister)(m_hInstance))
		 || (m_fRegistered = TRUE, !(*m_lpCtl3dAutoSubclass)(m_hInstance)))
		{
			Shutdown();
		}
	}
	
	return IsValid();
}

BOOL CControl3D::Startup(HINSTANCE hInstance, const CString *pcsCtl3dDirectory)
{
	Shutdown();
	
	// if Windows 4.0, use its built-in 3D stuff

	if (IsWindows4())
	{
		return TRUE;
	}

	// Windows 3.x, use CTL3DV2.DLL
		
	m_hInstance = hInstance;
	
	// check for an existing CTL3DV2.DLL
			
	UINT wOldSem = SetErrorMode(SEM_NOOPENFILEERRORBOX);
			
	if ((m_hCtl3d = LoadLibrary(CTL3DNAME)) < (HINSTANCE)HINSTANCE_ERROR)
	{
		m_hCtl3d = NULL;
		
		// CTL3DV2.DLL does not exist, try to copy a version to the system directory
		
		if (pcsCtl3dDirectory != NULL)
		{
			CString csSourcePath;
			CString csDestinationPath;
			CString csDestinationDirectory;
			CFile cfSource;
			CFile cfDestination;
			LPSTR lpBuffer = NULL;

			TRY
			{
				UINT uResult = GetSystemDirectory(csDestinationDirectory.GetBuffer(256), 256-1);
				csDestinationDirectory.ReleaseBuffer();

				if ((uResult == 0) || (uResult >= 256))
				{
					AfxThrowUserException();
				}

				Util::ConstructPath(csDestinationPath, csDestinationDirectory, CTL3DNAME);
				Util::ConstructPath(csSourcePath, *pcsCtl3dDirectory, CTL3DNAME);
				
				/*
				// Make sure the destination directory exists.
				*/
				
				if (!Util::MakeDirectory(csDestinationDirectory))
				{
					AfxThrowUserException();
				}
				
				/*
				// Attempt to open the source file.
				*/
				
				if (!cfSource.Open(csSourcePath, CFile::modeRead|CFile::shareDenyNone))
				{
					AfxThrowUserException();
				}
				
				/*
				// Attempt to create the destination file.
				*/
				
				if (!cfDestination.Open(csDestinationPath, CFile::modeCreate|CFile::modeWrite|CFile::shareExclusive))
				{
					AfxThrowUserException();
				}
				
				/*
				// Get the creation time for the file we are installing.
				*/
				
				CFileStatus cfsSource;

				if (!cfSource.GetStatus(cfsSource))
				{
					AfxThrowUserException();
				}
				
				/*
				// Allocate a buffer for the copy.
				*/
				
				#define	BUFFER_SIZE	(10*1024)
				
				if ((lpBuffer = new far char[BUFFER_SIZE]) == NULL)
				{
					AfxThrowMemoryException();
				}
				
				UINT uNumberRead;
					
				do
				{
					if ((uNumberRead = cfSource.Read(lpBuffer, BUFFER_SIZE)) > 0)
					{
						cfDestination.Write(lpBuffer, uNumberRead);
					}
				} while (uNumberRead == BUFFER_SIZE);
				
				CFileStatus cfsDestination;

				if (!cfDestination.GetStatus(cfsDestination))
				{
					AfxThrowUserException();
				}

			/* Copy the times over. */

				cfsDestination.m_ctime = cfsSource.m_ctime;
				cfsDestination.m_mtime = cfsSource.m_mtime;
				cfsDestination.m_atime = cfsSource.m_atime;

				if (cfSource.m_hFile != (UINT)CFile::hFileNull)
				{
					cfSource.Close();
				}

				if (cfDestination.m_hFile != (UINT)CFile::hFileNull)
				{
					cfDestination.Close();
					CFile::SetStatus(csDestinationPath, cfsDestination);
				}

				// CTL3D has been copied, try to load it
				if ((m_hCtl3d = LoadLibrary(CTL3DNAME)) < (HINSTANCE)HINSTANCE_ERROR)
				{
					AfxThrowUserException();
				}
			}
			CATCH_ALL(e)
			{
				m_hCtl3d = NULL;
				
				if (cfSource.m_hFile != (UINT)CFile::hFileNull)
				{
					cfSource.Close();
				}
					
			/*
			// Be sure to close the file before deleting it to avoid causing
			// lost clusters.
			*/

				if (cfDestination.m_hFile != (UINT)CFile::hFileNull)
				{
					cfDestination.Close();
				}

				if (!csDestinationPath.IsEmpty())
				{
					_unlink(csDestinationPath);
				}
			}
			END_CATCH_ALL

			delete [] lpBuffer;
			csSourcePath.Empty();
			csDestinationPath.Empty();
			csDestinationDirectory.Empty();
		}
	}

	SetErrorMode(wOldSem);
	
	GetEntryPoints();
	
	if (IsValid())
	{
		if ((!(*m_lpCtl3dRegister)(m_hInstance))
		 || (m_fRegistered = TRUE, !(*m_lpCtl3dAutoSubclass)(m_hInstance)))
		{
			Shutdown();
		}
	}
	
	return IsValid();
}

BOOL CControl3D::Shutdown()
{
	if (IsRegistered())
	{
		(*m_lpCtl3dUnregister)(m_hInstance);
		m_fRegistered = FALSE;
	}
	
	if (m_hCtl3d >= (HINSTANCE)HINSTANCE_ERROR)
	{
		FreeLibrary(m_hCtl3d);
		m_hCtl3d = NULL;
	}
	
	m_hInstance = NULL;
	m_lpCtl3dRegister = NULL;
	m_lpCtl3dUnregister = NULL;
	m_lpCtl3dAutoSubclass = NULL;
	m_lpCtl3dColorChange = NULL;
	m_lpCtl3dSubclassCtl = NULL;
	m_lpCtl3dSubclassDlgEx = NULL;
	
	return TRUE;
}

void CControl3D::ColorChange(void)
{
	if (IsValid())
	{
		(*m_lpCtl3dColorChange)();
	}
}
	
BOOL CControl3D::SubclassCtl(HWND hWnd)
{
	if (IsValid())
	{
		return m_lpCtl3dSubclassCtl(hWnd);
	}
	return FALSE;
}

BOOL CControl3D::SubclassDlgEx(HWND hWnd, DWORD grbit)
{
	if (IsValid())
	{
		return m_lpCtl3dSubclassDlgEx(hWnd, grbit);
	}
	return FALSE;
}

BOOL CControl3D::IsWindows4(void)
{
	WORD wWinVer;
	
	wWinVer = LOWORD(GetVersion());
	wWinVer = (((WORD)(LOBYTE(wWinVer)))<<8)|((WORD)(HIBYTE(wWinVer)));
	
	return (wWinVer >= 0x35f);
}

/*
// Set up the entry points.
// This is used by both Startup methods.
*/

void CControl3D::GetEntryPoints(void)
{
	if (m_hCtl3d != NULL)
	{
		m_lpCtl3dRegister = (FN_CTL3DREGISTER)GetProcAddress(m_hCtl3d, "CTL3DREGISTER");
		m_lpCtl3dUnregister = (FN_CTL3DUNREGISTER)GetProcAddress(m_hCtl3d, "CTL3DUNREGISTER");
		m_lpCtl3dAutoSubclass = (FN_CTL3DAUTOSUBCLASS)GetProcAddress(m_hCtl3d, "CTL3DAUTOSUBCLASS");
		m_lpCtl3dColorChange = (FN_CTL3DCOLORCHANGE)GetProcAddress(m_hCtl3d, "CTL3DCOLORCHANGE");
		m_lpCtl3dSubclassCtl = (FN_CTL3DSUBCLASSCTL)GetProcAddress(m_hCtl3d, "CTL3DSUBCLASSCTL");
		m_lpCtl3dSubclassDlgEx = (FN_CTL3DSUBCLASSDLGEX)GetProcAddress(m_hCtl3d, "CTL3DSUBCLASSDLGEX");
	
		if ((m_lpCtl3dRegister == NULL)
		 || (m_lpCtl3dUnregister == NULL)
		 || (m_lpCtl3dAutoSubclass == NULL)
		 || (m_lpCtl3dColorChange == NULL)
		 || (m_lpCtl3dSubclassCtl == NULL)
		 || (m_lpCtl3dSubclassDlgEx == NULL))
		{
			Shutdown();
		}
	}
}

