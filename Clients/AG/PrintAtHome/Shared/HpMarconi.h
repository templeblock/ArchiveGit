#pragma	once
#pragma	pack(1)

//	Version	number of the Marconi iteration. For driver	use	only.

#define	HPDJ_PDMA_VERSION 0x00000001

//	Value name in PrinterDriverData	for	byte offset	to the command buffer from the end of public devmode.
//	Absence	of this	value signals that the driver does not allow access	to private devmode.
//	This value must	be non-zero.

#define	HPDJ_CMD_BUF_OFFSET	_T("CmdBufOffset")

//	Magic number. The very first DWORD in the command buffer must contain this number.
//	Else, something went wrong	and	app	should not try to access private devmode though this mechanism.

#define	HPDJ_PDMA_SIGNATURE	0xBADACC00

#define	HPDJ_PDMA_GETVERSION(dwPDMASig)	(dwPDMASig & 0x000000FF)
#define	HPDJ_PDMA_CHECKSIG(dwPDMASig)  ((dwPDMASig & 0xFFFFFF00) ==	HPDJ_PDMA_SIGNATURE)

//	Layout of the command buffer.
//	Shyam Sep/23/2003: REVISIT:	UNICODE	identifier for the below structure is causing issue	in the 
//	HP Print Previewer.	 Rolling the fix back to the original.	Kah	Kit	reported that it is	
//	displaying junk	characters when	enumerating	the	media string through marconi.  This	is 
//	because	of char	defined	for	non-unicode	targets	and	doing the multi	byte to	wide char conversions.

typedef	struct
{
	DWORD	 dwPDMASig;
	char	 cCommand;
	char	 cError;
	SHORT	 sIndex;
	DWORD	 dwIdentifier;
	DWORD	 dwValue;
	WCHAR	 stringData[MAX_PATH];
} HPDJPDMACCESS, FAR* LPHPDJPDMACCESS;

//	Commands currently supported.

#define	HPDJ_PDM_CMD_GET							 0x01
#define	HPDJ_PDM_CMD_SET							 0x02
#define	HPDJ_PDM_CMD_ENUM							 0x03

//	Error values.

#define	HPDJ_PDM_ERROR_NOERROR						 0x00
#define	HPDJ_PDM_INVALID_COMMAND					 0x01
#define	HPDJ_PDM_ERROR_UNKNOWN_ID					 0x02
#define	HPDJ_PDM_ERROR_BAD_VALUE					 0x03
#define	HPDJ_PDM_ERROR_CANNOT_SET					 0x04
#define	HPDJ_PDM_ERROR_OUTOFRANGE					 0x05
#define	HPDJ_PDM_ERROR_NOMORE_VALUES				 0x06
#define	HPDJ_PDM_ERROR_INVALID_QUERY				 0x07

//	Bool Items.	Members	accessed by	the	following identifiers take on TRUE or FALSE values.

#define	HPDJ_PDMID_BOOL_MAX_VALUE					 0x000000FF
#define	HPDJ_PDMID_PAGE_BORDERS						 0x00000001
#define	HPDJ_PDMID_FLIP_HORIZONTAL					 0x00000002
#define	HPDJ_PDMID_ORDERED_PRINTING					 0x00000003
#define	HPDJ_PDMID_CAN_DO_BANNER					 0x00000004
#define	HPDJ_PDMID_BANNER_SETTING					 0x00000005
#define	HPDJ_PDMID_FAX_PHOTOCOPY					 0x00000006
#define	HPDJ_PDMID_PRINT_PREVIEW					 0x00000007
#define	HPDJ_PDMID_CAN_DO_AUTODUPLEX				 0x00000008
#define	HPDJ_PDMID_AUTODUPLEX_SETTING				 0x00000009
#define	HPDJ_PDMID_ROTATE_180_VERTICALLY			 0x0000000A
#define	HPDJ_PDMID_PHOTOPEN							 0x0000000B
#define	HPDJ_PDMID_SEPIA							 0x0000000C
#define	HPDJ_PDMID_AUTOPICK							 0x0000000D
#define	HPDJ_PDMID_SMARTBORDERLESS					 0x0000000E
#define	HPDJ_PDMID_MINIMIZEDMARGIN					 0x0000000F
#define	HPDJ_PDMID_ECONOMODE						 0x00000010
#define	HPDJ_PDMID_GLOSSPRINTFINISH					 0x00000011
#define	HPDJ_PDMID_SCALETOFIT						 0x00000012
#define	HPDJ_PDMID_STRAIGHTPAPERPATH				 0x00000013

//	Start Karthi
#define	HPDJ_PDMID_RED_EYE_REMOVAL					 0x00000014
#define	HPDJ_PDMID_SMARTFOCUS						 0x00000017
#define	HPDJ_PDMID_LOW_MEM_MODE						 0x00000018
#define	HPDJ_PDMID_CENTER_ON_PAGE					 0x00000019
#define	HPDJ_PDMID_IMAGE_ENH_VALID					 0x0000001A
#define	HPDJ_PDMID_MARVELOUSMODE_PQ					 0x0000001B
//	End Karthi

//	Int	Items. Members accessed	by the following identifiers take integer values.

#define	HPDJ_PDMID_INT_MAX_VALUE					 0x0000FF00
#define	HPDJ_PDMID_PAGEMODE							 0x00000100
#define	HPDJ_PDMID_PAGECOUNT						 0x00000200
#define	HPDJ_PDMID_INTENSITY						 0x00000300
#define	HPDJ_PDMID_COLOR_TONE						 0x00000400
#define	HPDJ_PDMID_BRIGHTNESS						 0x00000500
#define	HPDJ_PDMID_SATURATION						 0x00000600
#define	HPDJ_PDMID_INK_LEVELS						 0x00000700
#define	HPDJ_PDMID_ZOOM_FACTOR						 0x00000D00
#define	HPDJ_PDMID_ZOOMSMART_MODE					 0x00000E00
#define	HPDJ_PDMID_ZOOMSMART_PHYSICAL_PAPER_SIZE	 0x00000F00
#define	HPDJ_PDMID_EXTRA_DRYTIME					 0x00001000
#define	HPDJ_PDMID_GRAYSCALEMODE					 0x00001100
#define	HPDJ_PDMID_MEDIAWIDTHSENSE					 0x00001200

//Start	Karthi

#define	HPDJ_PDMID_DIGITAL_PHOTO_MARVELOUS			 0x00001300
#define	HPDJ_PDMID_DIGITAL_PHOTO_BEST				 0x00001400
#define	HPDJ_PDMID_DIGITAL_PHOTO_NORMAL				 0x00001500
#define	HPDJ_PDMID_CUSTOM_WIDTH						 0x00001600
#define	HPDJ_PDMID_CUSTOM_LENGTH					 0x00001700
#define	HPDJ_PDMID_CUSTOM_UNITS						 0x00001800
#define	HPDJ_PDMID_MARGIN_MODES						 0x00001900
#define	HPDJ_PDMID_BORDERLESS_MEDIA_TYPE			 0x00001A00
#define	HPDJ_PDMID_BORDERLESS_PAPER_SIZE			 0x00001B00
#define	HPDJ_PDMID_BORDERLESS_DUPLEX_SETTINGS		 0x00001C00
#define	HPDJ_PDMID_COLOR_SPACE						 0x00001D00
#define	HPDJ_PDMID_SMOOTHNESS						 0x00001E00
#define	HPDJ_PDMID_SHARPNESS						 0x00002000
#define	HPDJ_PDMID_DIGITAL_FLASH					 0x00002100
#define	HPDJ_PDMID_CONTRAST_ENHANCEMENT				 0x00002200
#define	HPDJ_PDMID_LEFTOVERSPRAY_PERCENTAGE			 0x00002300
#define	HPDJ_PDMID_TOPOVERSPRAY_PERCENTAGE			 0x00002400
#define	HPDJ_PDMID_RIGHTOVERSPRAY_PERCENTAGE		 0x00002500
#define	HPDJ_PDMID_BOTTOMOVERSPRAY_PERCENTAGE		 0x00002600
#define	HPDJ_PDMID_LEFTOVERSPRAY					 0x00002700
#define	HPDJ_PDMID_TOPOVERSPRAY						 0x00002800
#define	HPDJ_PDMID_RIGHTOVERSPRAY					 0x00002900
#define	HPDJ_PDMID_BOTTOMOVERSPRAY					 0x00002A00
#define	HPDJ_PDMID_CMYK_CYAN						 0x00002B00
#define	HPDJ_PDMID_CMYK_MAGENTA						 0x00002C00
#define	HPDJ_PDMID_CMYK_YELLOW						 0x00002D00
#define	HPDJ_PDMID_CMYK_BLACK						 0x00002E00

//	Values for PAGEMODE.

#define	HPDJ_PDMVALUE_PAGEMODE_NORMAL				 0
#define	HPDJ_PDMVALUE_PAGEMODE_NUP					 1
#define	HPDJ_PDMVALUE_PAGEMODE_POSTER				 2

//	Start Karthi

#define	HPDJ_PDMVALUE_PAGEMODE_BOOKLET				 3

//	Values for GRAYSCALEMODE.

#define	HPDJ_PDMVALUE_GRAYSCALEMODE_NONE				0
#define	HPDJ_PDMVALUE_GRAYSCALEMODE_HIGHQUALITY			1
#define	HPDJ_PDMVALUE_GRAYSCALEMODE_BLACKONLY			2
#define	HPDJ_PDMVALUE_GRAYSCALEMODE_PHOTOCOPYFAX		3

//	Values for ZOOMSMART_MODE.

#define	HPDJ_PDMVALUE_ZOOMSMARTMODE_NORMAL				 0
#define	HPDJ_PDMVALUE_ZOOMSMARTMODE_TARGETPAPERSIZE		 1
#define	HPDJ_PDMVALUE_ZOOMSMARTMODE_FITTOPAPER			 2

//	Values for Margin Modes

#define	HPDJ_PDMVALUE_MARGINMODE_NORMAL					 0
#define	HPDJ_PDMVALUE_MARGINMODE_ZEROMARGINS			 1
#define	HPDJ_PDMVALUE_MARGINMODE_SYMMETRICMARGINS		 2

//	String Items

#define	HPDJ_PDMID_STRING_MAX_VALUE					 0xFF000000
#define	HPDJ_PDMID_MEDIA_TYPE						 0x00010000

class CHpMarconi
{
public:
	LPCTSTR m_pszDevice;
	HANDLE m_hPrinter;
	DWORD m_dwOffset;
	
public:
	CHpMarconi(LPCTSTR pszDevice)
	{
		m_pszDevice = pszDevice;
		m_hPrinter = NULL;
		m_dwOffset = 0;
		
		Init();
	}

	~CHpMarconi()
	{
		if (m_hPrinter)
			::ClosePrinter(m_hPrinter);
	}

	bool Init()
	{
		//	Get	the	current	or default devmode from	the	driver.
		//	First, get the printer handle.
		PRINTER_DEFAULTS pd;
		::ZeroMemory(&pd, sizeof(pd));
		pd.DesiredAccess = PRINTER_ALL_ACCESS;
		::OpenPrinter((LPSTR)m_pszDevice, &m_hPrinter, &pd);
		if (!m_hPrinter) // Unable to open printer, won’t be able to get devmode
			return false;

		//	Query if this driver supports Marconi interface.
		DWORD dwType = 0;
		DWORD dwSizeNeeded = 0;
		DWORD dwResult = ::GetPrinterData(m_hPrinter, (LPTSTR)HPDJ_CMD_BUF_OFFSET, &dwType, (LPBYTE)&m_dwOffset, sizeof(DWORD), &dwSizeNeeded);

		//	The	returned m_dwOffset value	must be	a non-zero positive	integer.
		if (dwResult !=	ERROR_SUCCESS || m_dwOffset <= 0)
		{
			//	This driver	does not support Marconi interface.
			::ClosePrinter(m_hPrinter);
			m_hPrinter = NULL;
			return false;
		}

		return true;
	}
	
	bool Command(LPDEVMODE pDevMode, char cCommand, DWORD dwIdentifier, DWORD& dwValue, WCHAR* strData = NULL)
	{
		if (!m_hPrinter)
			return false;

		if (!pDevMode)
			return false;

		HWND hWnd = NULL;

		//	Get	the	size in	bytes required for this	driver’s devmode structure.
		long lDMSize = ::DocumentProperties(hWnd, m_hPrinter, (LPSTR)m_pszDevice, NULL, NULL, 0);
		// Get the address of Marconi command buffer in the devmode
		LPHPDJPDMACCESS	lpCmdBuf = (LPHPDJPDMACCESS)((LPBYTE)pDevMode + pDevMode->dmSize + m_dwOffset);
		if (!lpCmdBuf)
			return false;

		// Check the validity of this structure.
		if (!(HPDJ_PDMA_CHECKSIG(lpCmdBuf->dwPDMASig)))
			return false;

		// Now we are ready to access this driver’s private devmode.
		lpCmdBuf->cCommand		= cCommand;
		lpCmdBuf->dwIdentifier	= dwIdentifier;
		lpCmdBuf->dwValue		= dwValue;
		lpCmdBuf->sIndex	    = 0;
		lpCmdBuf->stringData[0]	= NULL;
		lpCmdBuf->cError	    = HPDJ_PDM_ERROR_NOERROR;
		long lRet = ::DocumentProperties(hWnd, m_hPrinter, (LPSTR)m_pszDevice, pDevMode, pDevMode, DM_IN_BUFFER | DM_OUT_BUFFER);
//		lpCmdBuf->cCommand		= 0;
		if (lRet !=	IDOK || lpCmdBuf->cError != HPDJ_PDM_ERROR_NOERROR)
			return false;

		if (strData)
			wcscpy(strData, lpCmdBuf->stringData);
		dwValue = lpCmdBuf->dwValue;
		lpCmdBuf->sIndex++;
		return true;
	}
	
	bool Enumerate(LPDEVMODE pDevMode, DWORD& dwValue, WCHAR* strData)
	{
		if (!m_hPrinter)
			return false;

		if (!pDevMode)
			return false;

		HWND hWnd = NULL;

		//	Get	the	size in	bytes required for this	driver’s devmode structure.
		long lDMSize = ::DocumentProperties(hWnd, m_hPrinter, (LPSTR)m_pszDevice, NULL, NULL, 0);
		// Get the address of Marconi command buffer in the devmode
		LPHPDJPDMACCESS	lpCmdBuf = (LPHPDJPDMACCESS)((LPBYTE)pDevMode + pDevMode->dmSize + m_dwOffset);
		if (!lpCmdBuf)
			return false;

		// Check the validity of this structure.
		if (!(HPDJ_PDMA_CHECKSIG(lpCmdBuf->dwPDMASig)))
			return false;

		long lRet = ::DocumentProperties(hWnd, m_hPrinter, (LPSTR)m_pszDevice, pDevMode, pDevMode, DM_IN_BUFFER | DM_OUT_BUFFER);
		if (lRet !=	IDOK || lpCmdBuf->cError != HPDJ_PDM_ERROR_NOERROR)
			return false;

		if (strData)
			wcscpy(strData, lpCmdBuf->stringData);
		dwValue = lpCmdBuf->dwValue;
		lpCmdBuf->sIndex++;
		return true;
	}
};
