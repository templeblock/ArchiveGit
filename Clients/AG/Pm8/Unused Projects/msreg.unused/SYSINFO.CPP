// sysinfo.cpp : implementation file
//

#include "stdafx.h"
#include "msregx.h"
#include "sysinfo.h"

#include "propctl.h"

#include <mmsystem.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#ifndef WIN32
extern "C" void FAR PASCAL DOS3Call(void);
#endif

/////////////////////////////////////////////////////////////////////////////
// CCpuInfo

CCpuInfo::CCpuInfo(LPCSTR pszPath /*=NULL*/)
{
	TRY
	{
		if (pszPath == NULL)
		{
			m_csDllName.Empty();
		}
		else
		{
			m_csDllName = pszPath;
		}
#ifdef WIN32
		m_csDllName += "CPUINF32.DLL";
#else
		m_csDllName += "CPUINF16.DLL";
#endif
	}
	END_TRY

	m_hDll = NULL;
	LoadDll();
}

CCpuInfo::~CCpuInfo()
{
	UnloadDll();
}

BOOL CCpuInfo::IsValid(void)
{
	return m_hDll != NULL;
}

BOOL CCpuInfo::LoadDll(void)
{
	if (!IsValid())
	{
		UnloadDll();
		m_hDll = LoadLibrary(m_csDllName);
#ifdef WIN16
		if (m_hDll < HINSTANCE_ERROR)
		{
			m_hDll = NULL;
		}
#endif
	}

	return IsValid();
}

void CCpuInfo::UnloadDll(void)
{
	if (IsValid())
	{
		FreeLibrary(m_hDll);
	}
	m_hDll = NULL;
}

int CCpuInfo::GetCpuId(void)
{
	int nCpuId = CPU_Unknown;

	if (LoadDll())
	{
		PFN_WINCPUID pfnWinCpuId = (PFN_WINCPUID)GetProcAddress(m_hDll, "wincpuid");
		if (pfnWinCpuId != NULL)
		{
			nCpuId = (int)((*pfnWinCpuId)() & 0x7fff);
		}
	}

	return nCpuId;
}

DWORD CCpuInfo::GetCpuSpeed(void)
{
	DWORD dwCpuSpeed = 0;

	if (LoadDll())
	{
		PFN_CPUSPEED pfnCpuSpeed = (PFN_CPUSPEED)GetProcAddress(m_hDll, "cpuspeed");
		if (pfnCpuSpeed != NULL)
		{                                     
			FREQUENCY_INFO FrequencyInfo = (*pfnCpuSpeed)(0);
			dwCpuSpeed = FrequencyInfo.norm_freq;
		}
	}

	return dwCpuSpeed;
}

/////////////////////////////////////////////////////////////////////////////
// CSystemInformationDialogReadOnlyControl

WNDPROC CSystemInformationDialogReadOnlyControl::m_pfnSuperWndProc = NULL;

CSystemInformationDialogReadOnlyControl::CSystemInformationDialogReadOnlyControl()
{
}

CSystemInformationDialogReadOnlyControl::~CSystemInformationDialogReadOnlyControl()
{
}

BEGIN_MESSAGE_MAP(CSystemInformationDialogReadOnlyControl, CWnd)
	//{{AFX_MSG_MAP(CSystemInformationDialogReadOnlyControl)
	ON_WM_MOUSEACTIVATE()
	ON_WM_SETCURSOR()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSystemInformationDialogReadOnlyControl message handlers

int CSystemInformationDialogReadOnlyControl::OnMouseActivate(CWnd* pDesktopWnd, UINT uHitTest, UINT uMessage)
{
	return MA_NOACTIVATEANDEAT;
}

BOOL CSystemInformationDialogReadOnlyControl::OnSetCursor(CWnd* pWnd, UINT uHitTest, UINT uMessage)
{
	::SetCursor(AfxGetApp()->LoadStandardCursor(IDC_ARROW));
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CSystemInformationDialog dialog

CSystemInformationDialog::CSystemInformationDialog(CWnd* pParent /*=NULL*/)
	: CBaseDialog(CSystemInformationDialog::IDD, pParent),
	  m_CpuInfo(GetApp()->WorkingDirectory())
{
	//{{AFX_DATA_INIT(CSystemInformationDialog)
	m_fIncludeInformation = FALSE;
	//}}AFX_DATA_INIT
	
	m_csSystemInformation.Empty();
}


void CSystemInformationDialog::DoDataExchange(CDataExchange* pDX)
{
	CBaseDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSystemInformationDialog)
	DDX_Check(pDX, IDC_INCLUDE_INFORMATION, m_fIncludeInformation);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSystemInformationDialog, CBaseDialog)
	//{{AFX_MSG_MAP(CSystemInformationDialog)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSystemInformationDialog message handlers

BOOL CSystemInformationDialog::OnInitDialog() 
{
	// Turn on "Include Information" by default.
	m_fIncludeInformation = TRUE;

	CBaseDialog::OnInitDialog();
	
	CMlsPropertyControl::SubclassPropertyControls(this);
	
	CenterWindow();

	m_OperatingSystem.SubclassDlgItem(IDC_OPERATING_SYSTEM, this);
	m_Processor.SubclassDlgItem(IDC_PROCESSOR, this);
	m_RamSize.SubclassDlgItem(IDC_RAM_SIZE, this);
	m_HardDiskSize.SubclassDlgItem(IDC_HARD_DISK_SIZE, this);
	m_VideoResolution.SubclassDlgItem(IDC_VIDEO_RESOLUTION, this);
	m_VideoColors.SubclassDlgItem(IDC_VIDEO_COLORS, this);
	m_Cdrom.SubclassDlgItem(IDC_CDROM, this);
	m_SoundCard.SubclassDlgItem(IDC_SOUND_CARD, this);

	CString csFormat;
	CString csValue;
	CString csInfo;
	CString csItem;
	
	BOOL fSystemInformationValid = TRUE;
	m_csSystemInformation.Empty();
	
	TRY
	{
		m_csSystemInformation = "A";
	}
	CATCH_ALL(e)
	{
		fSystemInformationValid = FALSE;
	}
	END_CATCH_ALL

	// Display the operating system.
	csValue.Empty();
	TRY
	{
		int nPlatform;
		WORD wVersion;
		GetOperatingSystem(nPlatform, wVersion);
#ifdef WIN32
		if ((nPlatform == PLATFORM_Windows) && (LOBYTE(wVersion) == 4) && (HIBYTE(wVersion) == 0))
#else
		if ((nPlatform == PLATFORM_Windows) && (LOBYTE(wVersion) == 3) && (HIBYTE(wVersion) == 95))
#endif
		{
			csValue.LoadString(IDS_Windows95);
		}
		else
		{
			csFormat.LoadString(IDS_PlatformFormat);
			UINT uItem = 
				(nPlatform == PLATFORM_Windows) ? IDS_PlatformWindows :
					(nPlatform == PLATFORM_Windows_NT) ? IDS_PlatformWindowsNT : IDS_PlatformUnknown;
			csItem.LoadString(uItem);
			csValue.Format(csFormat, (LPCSTR)csItem, LOBYTE(wVersion), HIBYTE(wVersion));
		}
		
		csInfo.Format("%c%02x%02x",
				(nPlatform == PLATFORM_Unknown) ? 'U' :
					(nPlatform == PLATFORM_Windows) ? 'W' :
						(nPlatform == PLATFORM_Windows_NT) ? 'N' : 'U',
				LOBYTE(wVersion),
				HIBYTE(wVersion));
		m_csSystemInformation += csInfo;
		
	}
	CATCH_ALL(e)
	{
		fSystemInformationValid = FALSE;
	}
	END_CATCH_ALL
	m_OperatingSystem.SetWindowText(csValue);

	// Display the processor and speed.
	csValue.Empty();
	TRY
	{
		int nProcessor;
		DWORD dwSpeed;
		GetProcessor(nProcessor, dwSpeed);
		UINT uItem = 
			(nProcessor == CCpuInfo::CPU_Unknown) ? IDS_ProcessorUnknown :
				(nProcessor == CCpuInfo::CPU_8086) ? IDS_Processor8086 :
					(nProcessor == CCpuInfo::CPU_80186) ? IDS_Processor80186 :
						(nProcessor == CCpuInfo::CPU_80286) ? IDS_Processor80286 :
							(nProcessor == CCpuInfo::CPU_80386) ? IDS_Processor80386 :
								(nProcessor == CCpuInfo::CPU_80486) ? IDS_Processor80486 :
									(nProcessor == CCpuInfo::CPU_Pentium) ? IDS_ProcessorPentium :
										(nProcessor == CCpuInfo::CPU_PentiumPro) ? IDS_ProcessorPentiumPro : 0;
		if (uItem != 0)
		{
			csValue.LoadString(uItem);
		}
		else
		{
			csFormat.LoadString(IDS_ProcessorP);
			csValue.Format(csFormat, nProcessor);
		}
		if (dwSpeed > 0)
		{
			char szSpeed[16];
			ultoa(dwSpeed, szSpeed, 10);
			csValue += ' ';
			csValue += szSpeed;
			csItem.LoadString(IDS_ProcessorMHz);
			csValue += csItem;
		}
		
		csInfo.Format("%c%c%04x", '0', nProcessor+'0', (int)dwSpeed);
		m_csSystemInformation += csInfo;
	}
	CATCH_ALL(e)
	{
		fSystemInformationValid = FALSE;
	}
	END_CATCH_ALL
	m_Processor.SetWindowText(csValue);

	// Display the RAM size.
	csValue.Empty();
	TRY
	{
		int nRamSize = GetRamSize();
		csFormat.LoadString(IDS_RamSizeMb);
		csValue.Format(csFormat, nRamSize);
		
		csInfo.Format("%04x", nRamSize);
		m_csSystemInformation += csInfo;
	}
	CATCH_ALL(e)
	{
		fSystemInformationValid = FALSE;
	}
	END_CATCH_ALL
	m_RamSize.SetWindowText(csValue);

	// Display the hard disk size.
	int nHardDiskSize;
	BOOL fCdrom;
	ScanDrives(nHardDiskSize, fCdrom);
	csValue.Empty();
	TRY
	{
		csFormat.LoadString(IDS_HardDiskSizeMb);
		csValue.Format(csFormat, nHardDiskSize);
		
		csInfo.Format("%04x", nHardDiskSize);
		m_csSystemInformation += csInfo;
	}
	CATCH_ALL(e)
	{
		fSystemInformationValid = FALSE;
	}
	END_CATCH_ALL
	m_HardDiskSize.SetWindowText(csValue);

	// Display the video resolution.
	csValue.Empty();
	TRY
	{
		CSize czVideoResolution = GetVideoResolution();
		csFormat.LoadString(IDS_VideoResolution);
		csValue.Format(csFormat, czVideoResolution.cx, czVideoResolution.cy);
		
		csInfo.Format("%04x%04x", czVideoResolution.cx, czVideoResolution.cy);
		m_csSystemInformation += csInfo;
	}
	CATCH_ALL(e)
	{
		fSystemInformationValid = FALSE;
	}
	END_CATCH_ALL
	m_VideoResolution.SetWindowText(csValue);

	// Display the video color depth.
	csValue.Empty();
	TRY
	{
		int nVideoDepth = GetVideoDepth();
		if (nVideoDepth == 1)
		{
			csValue.LoadString(IDS_VideoMonochrome);
		}
		else if (nVideoDepth >= 24)
		{
			csValue.LoadString(IDS_VideoTrueColor);
		}
		else
		{
			csFormat.LoadString(IDS_VideoColors);
			csValue.Format(csFormat, 1L << nVideoDepth);
		}
		
		csInfo.Format("%04x", nVideoDepth);
		m_csSystemInformation += csInfo;
	}
	CATCH_ALL(e)
	{
		fSystemInformationValid = FALSE;
	}
	END_CATCH_ALL
	m_VideoColors.SetWindowText(csValue);

	// Display CD-ROM existance.
	csValue.Empty();
	TRY
	{
		csValue.LoadString(fCdrom ? IDS_CdromYes : IDS_CdromNo);
		
		m_csSystemInformation += fCdrom ? 'Y' : 'N';
	}
	CATCH_ALL(e)
	{
		fSystemInformationValid = FALSE;
	}
	END_CATCH_ALL
	m_Cdrom.SetWindowText(csValue);

	// Display sound card existance.
	csValue.Empty();
	TRY
	{
		BOOL fSoundCard = CheckForSoundCard();
		csValue.LoadString(fSoundCard ? IDS_SoundCardYes : IDS_SoundCardNo);
		
		m_csSystemInformation += fSoundCard ? 'Y' : 'N';
	}
	CATCH_ALL(e)
	{
		fSystemInformationValid = FALSE;
	}
	END_CATCH_ALL
	m_SoundCard.SetWindowText(csValue);
	
	if (!fSystemInformationValid)
	{
		m_csSystemInformation.Empty();
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

BOOL CSystemInformationDialog::GetOperatingSystem(int& nPlatform, WORD& wVersion)
{
	BOOL fWin3 = FALSE;
	BOOL fWin95 = FALSE;
	BOOL fWinNT = FALSE;

	DWORD dwVersion = GetVersion();
	WORD wMajorVersion = (WORD)(LOBYTE(LOWORD(dwVersion)));
	WORD wMinorVersion = (WORD)(HIBYTE(LOWORD(dwVersion)));
	
#ifdef WIN32
	BOOL fHighBit = (dwVersion & 0x80000000L) != 0;
	fWin3 = fHighBit && (wMajorVersion == 3);
	fWin95 = fHighBit && (wMajorVersion == 4);
	fWinNT = !fHighBit && ((wMajorVersion == 3) || (wMajorVersion == 4));
#else
	DWORD dwFlags = GetWinFlags();
	fWinNT = (dwFlags & 0x4000) != 0;
	fWin95 = !fWinNT && (wMajorVersion == 4);
	fWin3 = !fWinNT && !fWin95 && (wMajorVersion == 3);
#endif

	nPlatform = PLATFORM_Unknown;
	if (fWinNT)
	{
		nPlatform = PLATFORM_Windows_NT;
	}
	else
	{
		nPlatform = PLATFORM_Windows;
	}
	wVersion = MAKEWORD(wMajorVersion, wMinorVersion);

	return nPlatform != PLATFORM_Unknown;
}

void CSystemInformationDialog::GetProcessor(int& nProcessor, DWORD& dwSpeed)
{
	nProcessor = m_CpuInfo.GetCpuId();
	dwSpeed = m_CpuInfo.GetCpuSpeed();
}

int CSystemInformationDialog::GetRamSize(void)
{
#ifdef WIN32
	MEMORYSTATUS MemoryStatus;
	MemoryStatus.dwLength = sizeof(MemoryStatus);
	GlobalMemoryStatus(&MemoryStatus);

	// We want the total amount of memory installed on the motherboard,
	// but GlobalMemoryStatus() subtracts out the holes between 640K
	// and 1Mb. So, on the assumption that actual installed memory does
	// not include a fraction of a Mb (i.e., the user cannot have 13.37 Mb
	// installed), we round up to the nearest Mb.
	return (int)((MemoryStatus.dwTotalPhys+1048575)/1048576);
#else
	// The following code is from MSDN: "Determining the amount of physical memory installed"
	// It reads the physical memory size (in Kb) from the CMOS.
	WORD wBaseMemory;
	WORD wExtendedMemory;
	_asm
	{
		mov	dx,071h
		
		mov	ax,015h
		out	070h,al
		jmp	wait1
wait1:
		in		al,dx
		mov	byte ptr [wBaseMemory+0],al
		
		mov	ax,016h
		out	070h,al
		jmp	wait2
wait2:
		in		al,dx
		mov	byte ptr [wBaseMemory+1],al
		
		mov	ax,017h
		out	070h,al
		jmp	wait3
wait3:
		in		al,dx
		mov	byte ptr [wExtendedMemory+0],al
		
		mov	ax,018h
		out	070h,al
		jmp	wait4
wait4:
		in		al,dx
		mov	byte ptr [wExtendedMemory+1],al
	}
	
	// We want the total amount of memory installed on the motherboard,
	// but wBaseMemory does not include the holes between 640K and 1Mb.
	// So, on the assumption that actual installed memory does not include
	// a fraction of a Mb (i.e., the user cannot have 13.37 Mb installed),
	// we round up to the nearest Mb.
	return (wBaseMemory+wExtendedMemory+1023)/1024;
#endif
}

CSize CSystemInformationDialog::GetVideoResolution(void)
{
	CSize czResolution(0,0);

	HDC hDC = ::GetDC(NULL);
	if (hDC != NULL)
	{
		czResolution.cx = ::GetDeviceCaps(hDC, HORZRES);
		czResolution.cy = ::GetDeviceCaps(hDC, VERTRES);
		::ReleaseDC(NULL, hDC);
	}

	return czResolution;
}

int CSystemInformationDialog::GetVideoDepth(void)
{
	int nDepth = 0;

	HDC hDC = ::GetDC(NULL);
	if (hDC != NULL)
	{
		nDepth = ::GetDeviceCaps(hDC, BITSPIXEL)*::GetDeviceCaps(hDC, PLANES);
		::ReleaseDC(NULL, hDC);
	}

	return nDepth;
}

void CSystemInformationDialog::ScanDrives(int& nDiskSpace, BOOL& fCdrom)
{
	nDiskSpace = 0;
	fCdrom = FALSE;

	double dTotalFixedDiskBytes = 0;

#ifdef WIN32
	DWORD dwSize = GetLogicalDriveStrings(0, NULL);
	if (dwSize != 0)
	{
		char* pBuffer = NULL;
		TRY
		{
			pBuffer = new char[dwSize+1];
		}
		END_TRY

		if (pBuffer != NULL)
		{
			dwSize = GetLogicalDriveStrings(dwSize, pBuffer);
			
			char* pDrive = pBuffer;
			while (*pDrive != NULL)
			{
				UINT uDriveType = ::GetDriveType(pDrive);
				if (uDriveType == DRIVE_CDROM)
				{
					fCdrom = TRUE;
				}
				if (uDriveType == DRIVE_FIXED)
				{
					DWORD dwSectorsPerCluster;
					DWORD dwBytesPerSector;
					DWORD dwNumberOfFreeClusters;
					DWORD dwTotalNumberOfClusters;
					if (GetDiskFreeSpace(pDrive, &dwSectorsPerCluster, &dwBytesPerSector, &dwNumberOfFreeClusters, &dwTotalNumberOfClusters))
					{
						dTotalFixedDiskBytes += (double)dwTotalNumberOfClusters*(double)dwSectorsPerCluster*(double)dwBytesPerSector;
					}
				}

				pDrive += strlen(pDrive)+1;
			}

			delete pBuffer;
			pBuffer = NULL;
		}
	}
#else
	for (char cDriveLetter = 'A'; cDriveLetter <= 'Z'; cDriveLetter++)
	{
		UINT uDriveType = ::GetDriveType(cDriveLetter - 'A');

		if (!fCdrom && (uDriveType == DRIVE_REMOTE))
		{
			_asm
			{
				mov	ax,150bh					// MSCDEX CD-ROM drive check
				xor	bx,bx
				mov	cl,cDriveLetter
				sub	cl,'A'
				xor	ch,ch                   
				int	2fh
				cmp	bx,0ADADh				// Check MSCDEX signature
				jne	L1
				or		ax,ax						// Check drive type
				je		L1
				les	bx,fCdrom
				mov	word ptr es:[bx],TRUE
			L1:
			}
		}
		if (uDriveType == DRIVE_FIXED)
		{
			WORD wSectorsPerCluster = 0;
			WORD wBytesPerSector = 0;
			WORD wTotalNumberOfClusters = 0;

			_asm
			{
				mov	dl,cDriveLetter
				sub	dl,'A'-1
				mov	ah,036h
				call	DOS3Call
				cmp	ax,-1
				je		L2
				mov	wSectorsPerCluster,ax
				mov	wBytesPerSector,cx
				mov	wTotalNumberOfClusters,dx
			L2:
			}
			
			dTotalFixedDiskBytes += (double)wTotalNumberOfClusters*(double)wSectorsPerCluster*(double)wBytesPerSector;
		}
	}
#endif

	nDiskSpace = (int)(dTotalFixedDiskBytes/1048576.0);
}

BOOL CSystemInformationDialog::CheckForSoundCard(void)
{
	BOOL fResult = FALSE;

	HINSTANCE hInst = AfxGetResourceHandle();

	// Find the WAVE resource.
	HRSRC hResInfo = FindResource(hInst, "BLANK8", "WAVE");
	if (hResInfo != NULL)
	{
		// Load the WAVE resource.
		HGLOBAL hRes = LoadResource(hInst, hResInfo);
		if (hRes != NULL)
		{
			// Lock the WAVE resource and play it.
			LPCSTR pRes = (LPCSTR)LockResource(hRes);
			if (pRes != NULL)
			{
				fResult = sndPlaySound(pRes, SND_MEMORY | SND_SYNC | SND_NODEFAULT);
				UnlockResource(hRes);
			}
		}

		// Free the WAVE resource and return success or failure */
		FreeResource(hRes);
	}

	return fResult;
}
