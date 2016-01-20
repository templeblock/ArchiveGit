/*
// $Workfile: CCTL3D.H $
// $Revision: 1 $
// $Date: 3/03/99 6:22p $
*/

/*
// Revision History:
//
// $Log: /PM8/msreg/CCTL3D.H $
// 
// 1     3/03/99 6:22p Gbeddow
// 
// 1     7/09/98 2:27p Jayn
// 
//    Rev 1.0   28 Nov 1995 12:08:40   FRED
// Initial revision.
// 
//    Rev 1.0   13 Nov 1995 16:08:46   JAY
// Initial revision.
// 
//    Rev 2.2   29 Mar 1995 09:14:56   JAY
// Lots of changes
// 
//    Rev 2.1   08 Feb 1995 13:38:52   JAY
// Reverted. New series.
// 
//    Rev 1.3   22 Jun 1994 10:39:34   FRED
// No change.
// 
//    Rev 1.2   22 Jun 1994 08:26:16   JAY
// Moved include of "ctl3d.h" from CCTL3D.H to CCTL3D.CPP since it wasn't
// needed in the class definition.
// 
//    Rev 1.1   22 Jun 1994 08:22:56   JAY
// Added separate Startup(). Added GetEntryPoints(). Made Shutdown() callable from
// within GetEntryPoints().
// 
//    Rev 1.0   22 Jun 1994 07:11:58   FRED
// Initial revision.
*/ 

#ifndef _CCTL3D_H_
#define _CCTL3D_H_

class CControl3D
{
	typedef BOOL (FAR PASCAL *FN_CTL3DREGISTER)(HANDLE);
	typedef BOOL (FAR PASCAL *FN_CTL3DUNREGISTER)(HANDLE);
	typedef BOOL (FAR PASCAL *FN_CTL3DAUTOSUBCLASS)(HANDLE);
	typedef void (FAR PASCAL *FN_CTL3DCOLORCHANGE)(void);
	typedef BOOL (FAR PASCAL *FN_CTL3DSUBCLASSCTL)(HWND);
	typedef BOOL (FAR PASCAL *FN_CTL3DSUBCLASSDLGEX)(HWND, DWORD);
	
public:
	CControl3D(void);
	~CControl3D(void);
	
public:
/* Simple startup. Fail if CTL3D not available. */
	BOOL Startup(HINSTANCE hInstance);

	BOOL Shutdown(void);
	void ColorChange(void);
	
	BOOL SubclassCtl(HWND hWnd);
	BOOL SubclassDlgEx(HWND hWndDlg, DWORD grbit = 0xffff);		// CTL3D_ALL

protected:
	BOOL IsValid(void)
		{ return (m_hCtl3d != NULL); }
	BOOL IsRegistered(void)
		{ return m_fRegistered; }
	BOOL IsWindows4(void);	
	void GetEntryPoints(void);
	
private:
	BOOL m_fValid;
	BOOL m_fRegistered;
	HINSTANCE m_hInstance;
	HINSTANCE m_hCtl3d;

	FN_CTL3DREGISTER m_lpCtl3dRegister;
	FN_CTL3DUNREGISTER m_lpCtl3dUnregister;
	FN_CTL3DAUTOSUBCLASS m_lpCtl3dAutoSubclass;
	FN_CTL3DCOLORCHANGE m_lpCtl3dColorChange;
	FN_CTL3DSUBCLASSCTL m_lpCtl3dSubclassCtl;
	FN_CTL3DSUBCLASSDLGEX m_lpCtl3dSubclassDlgEx;
};

#endif

