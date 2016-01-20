/*
// $Workfile: CPROGBAR.CPP $
// $Revision: 1 $
// $Date: 3/03/99 6:06p $
*/

/*
// Revision History:
//
// $Log: /PM8/App/LICDLL/CPROGBAR.CPP $
// 
// 1     3/03/99 6:06p Gbeddow
// 
//    Rev 1.0   14 Aug 1997 15:36:00   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 10:19:42   Fred
// Initial revision.
// 
//    Rev 1.0   20 May 1997 10:41:32   johno
// Initial revision.
//
//	  Rev 2.0   08 Apr 1997  Aaron Evans (aaron@situs.com)
// Extracted files/functions from \catalog and \upgrade
// from PrintMaster 3.0 to create a generic unlock DLL. 
//
*/
/*
// Revision History:
//
// $Log: /PM8/App/LICDLL/CPROGBAR.CPP $
// 
// 1     3/03/99 6:06p Gbeddow
// 
//    Rev 1.0   08 Sep 1996 09:40:24   Fred
// Initial revision.
// 
//    Rev 1.0   02 Feb 1996 08:57:54   JAY
// Initial revision.
// 
//    Rev 1.0   22 Nov 1995 08:52:10   FRED
// Initial revision.
// 
//    Rev 1.0   30 Oct 1995 13:36:42   JAY
// Initial revision.
// 
//    Rev 2.2   13 Oct 1995 14:20:00   FRED
// 32 bit changes
// 
//    Rev 2.1   08 Feb 1995 13:35:58   JAY
// Reverted. New series.
// 
//    Rev 1.2   29 Jun 1994 16:14:02   FRED
// Changed rounding so 100% is reach only at the end.
// 
// 
//    Rev 1.1   16 Jun 1994 18:15:42   FRED
// Some improvements including Hide/Show
// methods that work.
// 
// 
//    Rev 1.0   16 Jun 1994 09:03:20   FRED
// Initial revision.
*/ 

#include "stdafx.h"
#include "cprogbar.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CProgressBar

WNDPROC CProgressBar::m_pfnSuperWndProc = NULL;

#ifdef WIN32
LONG CALLBACK CProgressBar::FirstMsgOnlyWndProc(HWND hWnd, UINT msg, WPARAM wParam, LONG lParam)
#else
LONG CALLBACK __export CProgressBar::FirstMsgOnlyWndProc(HWND hWnd, UINT msg, UINT wParam, LONG lParam)
#endif
{
	/*
	// Construct the object and then attach the hWnd to it.
	*/
	
	CProgressBar* pProgressBar = new CProgressBar;
	pProgressBar->Attach(hWnd) ;
	
	pProgressBar->m_fCreatedByClassName = TRUE ;
	
	/*
	// Subclass the window by changing the window procedure to AfxWndProc
	// and pass the current message to it.
	*/
	
	::SetWindowLong(hWnd, GWL_WNDPROC, (DWORD)AfxWndProc);

#ifdef STRICT
	return ::CallWindowProc((WNDPROC)AfxWndProc, hWnd, msg, wParam, lParam);
#else
	return ::CallWindowProc((FARPROC)AfxWndProc, hWnd, msg, wParam, lParam);
#endif
}

BOOL CProgressBar::RegisterMyClass()
{
	WNDCLASS wcls;
	
	m_pfnSuperWndProc = ::DefWindowProc;
	
	static const char szClassName[] = "MlsProgressBar";
	
	if (::GetClassInfo(AfxGetInstanceHandle(), szClassName, &wcls))
	{
		return (wcls.lpfnWndProc == CProgressBar::FirstMsgOnlyWndProc);
	}
	
	wcls.style = 0;
	wcls.cbClsExtra = 0;
	wcls.cbWndExtra = 0;
	wcls.lpfnWndProc = CProgressBar::FirstMsgOnlyWndProc;
	wcls.hIcon = NULL;
	wcls.hCursor = ::LoadCursor(NULL, IDC_ARROW);
	wcls.hInstance = AfxGetInstanceHandle();
	wcls.hbrBackground = NULL;
	wcls.lpszMenuName = NULL;
	wcls.lpszClassName = szClassName;
	
	return (RegisterClass(&wcls) != 0);
}

void CProgressBar::PostNcDestroy()
{
	if (m_fCreatedByClassName)
	{
		delete this;
	}
}

CProgressBar::CProgressBar()
{
}

CProgressBar::~CProgressBar()
{
}

BEGIN_MESSAGE_MAP(CProgressBar, CWnd)
	//{{AFX_MSG_MAP(CProgressBar)
	ON_WM_PAINT()
	ON_WM_CREATE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CProgressBar message handlers


void CProgressBar::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	
	CBrush cbBlack;
	cbBlack.CreateStockObject(BLACK_BRUSH);
			
	CBrush cbWhite;
	cbWhite.CreateStockObject(WHITE_BRUSH);
			
	CRect crInside;
	UINT uBarLength;
			
	// draw the border
	crInside = dc.m_ps.rcPaint;
	GetClientRect(&crInside);
	dc.FrameRect(&crInside, &cbBlack);
			
	// move the rectangle inside the border
	crInside.left++;
	crInside.top++;
	crInside.right--;
	crInside.bottom--;
			
	// compute the length of the current progress bar
	uBarLength = (UINT)GetPosition((DWORD)(crInside.Width()));
							
	CDC cdcMemory;
			
	if (cdcMemory.CreateCompatibleDC(&dc))
	{
		// create a monochrome bitmap with a white background and the percentage text in black
		CRect crBar(0, 0, crInside.Width(), crInside.Height());
				
		CBitmap cbmText;
								
		if (cbmText.CreateBitmap(crBar.right, crBar.bottom, 1, 1, NULL))
		{
			CBitmap *pcbmOld;
									
			if ((pcbmOld = cdcMemory.SelectObject(&cbmText)) != NULL)
			{
				BOOL fIsMonochrome = ((dc.GetDeviceCaps(PLANES)*dc.GetDeviceCaps(BITSPIXEL)) == 1);
										
				UINT uPercent;
				char szPercent[256];
										
				cdcMemory.FillRect(&crBar, &cbWhite);
				cdcMemory.SetTextAlign(TA_CENTER | TA_TOP | TA_NOUPDATECP);
				cdcMemory.SetTextColor(RGB(0,0,0));
				cdcMemory.SetBkMode(TRANSPARENT);
			
				uPercent = (UINT)GetPosition(100L);
			
				wsprintf(szPercent, m_szFormat, uPercent);
				cdcMemory.TextOut(
					crBar.right/2,
					1,
					szPercent,
					lstrlen(szPercent));
						
				/*
				// Draw the monochrome memory bitmap into the destination DC.
				// This is done in two BitBlt() calls. The first paints the
				// "done" protion, the second paints the "left to do" portion.
				*/
						
				COLORREF crOldBackground = dc.SetBkColor(m_crColor);
				COLORREF crOldText = dc.SetTextColor(crOldBackground);
										
				if (uBarLength > 0)
				{
					dc.BitBlt(
						crInside.left,
						crInside.top,
						uBarLength,
						crBar.bottom,
						&cdcMemory,
						0,
						0,
						fIsMonochrome ? NOTSRCCOPY : SRCCOPY);
				}
											
				dc.SetTextColor(m_crColor);
				dc.SetBkColor(crOldBackground);
										
				if (crBar.right > (int)uBarLength)
				{
					dc.BitBlt(
						crInside.left+uBarLength,
						crInside.top,
						crBar.right-uBarLength,
						crBar.bottom,
						&cdcMemory,
						uBarLength,
						0,
						SRCCOPY);
				}
											
				dc.SetTextColor(crOldText);
											
				cdcMemory.SelectObject(pcbmOld);
			}
									
			cbmText.DeleteObject();
		}
								
		cdcMemory.DeleteDC();
	}
			
	cbBlack.Detach();
	cbWhite.Detach();
	
	// Do not call CWnd::OnPaint() for painting messages
}

/*
// ParseText
//
// Purpose:
//  Parses window text for a valid format, range and initial position.
//  This function is used when creating the control or setting the
//  window text to set the initial range and position but is also
//  used to validate text entered in the Style dialog in the Dialog
//  Editor interface if the PBS_TEXTHASRANGE style is selected.
//
//  The range and position must all be valid for any change to
//  occur in lpdwMin, lpdwMax, and lpdwPos.
//
// Parameters:
//  lpszText        LPSTR pointer to the window text to parse out
//                  the range and the position.
//  lpszFormat      LPSTR pointer to format string buffer
//  nFormatSize     int size of format string buffer
//  lpdwMin         LPDWORD pointer to location to store minimum.
//  lpdwMax         LPDWORD pointer to location to store maximum.
//  lpdwPos         LPDWORD pointer to location to store position.
//
// Return Value:
//  BOOL            TRUE if the function successfully initializes
//                  the range and position. FALSE if any part of
//                  the text is not a valid number between comma
//                  delimeters.
*/

BOOL CProgressBar::ParseText(LPSTR lpszText, LPSTR lpszFormat, int nFormatSize, LPDWORD lpdwMin, LPDWORD lpdwMax, LPDWORD lpdwPos)
{
	DWORD dwMin;
	DWORD dwMax;
	DWORD dwPos;

	if (lpszText == NULL)
	{
		return FALSE;
	}
	
	/*
	// Parse off the format string.
	*/
	
	if (!StringUpToChar(&lpszText, lpszFormat, nFormatSize-1, ';'))
	{
		return FALSE;
	}

	/*
	// Parse off the bottom of the range.  Note that we depend
	// on NumberUpToChar to modify lpszText to point to the character
	// after the delimeter which is why we pass &lpszText.
	*/
	
	dwMin = NumberUpToChar(&lpszText, ',');

	// check for valid value AND that there's text remaining
	if ((dwMin == (DWORD)-1) || (*lpszText == '\0'))
	{
		return FALSE;
	}

	// parse off the top of the range.
	dwMax = NumberUpToChar(&lpszText, ',');

	// check for valid value AND that there's text remaining
	if ((dwMax == (DWORD)-1) || (*lpszText == '\0'))
	{
		return FALSE;
	}

	// parse off the position and validate it
	dwPos = NumberUpToChar(&lpszText, ',');
	
	if (dwPos == (DWORD)-1)
	{
		return FALSE;
	}

	// store the parsed values in the return locations
	*lpdwMin = dwMin;
	*lpdwMax = dwMax;
	*lpdwPos = dwPos;

	return TRUE;
}

/*
// NumberUpToChar
//
// Purpose:
//  Scans a string for digits, converting the series of digits to
//  an integer value as the digits are scanned.  Scanning stops
//  at chDelimeter or the end of the string.
//
//  If the scan sees a non-digit character, -1 is returned to indicate
//  error.  If the scan sees a null-terminator before any text, we
//  return 0.
//
// Parameters:
//  lplpszText      LPSTR * pointer to pointer to the string to scan.
//                  On return, the pointer will point to the character
//                  after the delimeter OR the NULL terminator.
//
//                  We want a pointer to the pointer so we can modify
//                  that pointer for the calling function since we are
//                  using the return value for the parsed value
//
//  cDelimiter      char delimeter at which the scanning stops.
//
// Return Value:
//  DWORD           -1 if the string contains non-digits excluding
//                  the comma.  Otherwise the converted value is
//                  returned and the pointer to the address after
//                  the comma is stored at lplpszText.
*/

DWORD CProgressBar::NumberUpToChar(LPSTR FAR *lplpszText, char cDelimeter)
{
	DWORD dwResult = 0;
	LPSTR lpszText = *lplpszText;

	// fail if there's no more string to parse
	if (*lpszText == '\0')
	{
		return ((DWORD)-1);
	}

	// scan string, stopping at 0 or cDelimeter
	char ch;
	
	while ((ch = *lpszText) != '\0')
	{
		if (ch == cDelimeter)
		{
			lpszText++;	// skip delimeter
			break;
		}
	
		// check for digits, returning -1 on a non-digit
		ch -= '0';
		
		if ((ch < 0) || (ch > 9))
		{
			return ((DWORD)-1);
		}
		
		// Add up the value as we scan
		dwResult = (dwResult*10L) + (DWORD)ch;
		
		/*
		// We increment lpszText here so if we hit a null-terminator
		// lpszText is always valid.  If we incremented in the while
		// statement then lpszText might be past the null-terminator
		// and possibly invalid.
		*/
		
		lpszText++;
	}

	/*
	// Store the new pointer and the value.  Note that the *lpszText++
	// already incremented lpszText past the delimeter AND the zero.
	*/
	
	*lplpszText = lpszText;
	
	return dwResult;
}

/*
// StringUpToChar
//
// Purpose:
//  Extracts a string for another until a delimeter or end of string is reached.
//
//  If the scanned string is empty, FALSE is returned.
//
// Parameters:
//  lplpszText      LPSTR * pointer to pointer to the string to scan.
//                  On return, the pointer will point to the character
//                  after the delimeter OR the NULL terminator.
//
//                  We want a pointer to the pointer so we can modify
//                  that pointer for the calling function since we are
//                  using the return value for the parsed value
//
//  lpszBuffer      Pointer to buffer into which the characters are placed.
//
//  nMaximum        Maximum number of characters to collect.
//
//  cDelimiter      char delimeter at which the scanning stops.
//
// Return Value:
//  BOOL            FALSE if string is empty, TRUE otherwise.
//                  The pointer to the address after the
//                  delimeter is stored at lplpszText.
*/

BOOL CProgressBar::StringUpToChar(LPSTR FAR *lplpszText, LPSTR lpszBuffer, int nMaximum, char cDelimeter)
{
	int nCount = 0;
	
	// scan string, stopping at 0 or cDelimeter
	LPSTR lpszText = *lplpszText;
	char ch;
	
	while ((ch = *lpszText) != '\0')
	{
		if (ch == cDelimeter)
		{
			lpszText++;	// skip delimeter
			break;
		}
		
		// copy the character if it will fit
		if (nCount < nMaximum)
		{
			*(lpszBuffer++) = *lpszText;
			nCount++;
		}
		
		/*
		// We increment lpszText here so if we hit a null-terminator
		// lpszText is always valid.  If we incremented in the while
		// statement then lpszText might be past the null-terminator
		// and possibly invalid.
		*/
		
		lpszText++;
	}
   
   // terminate the output buffer
	*lpszBuffer = '\0';
	
	/*
	// Store the new pointer and the value.  Note that the *lpszText++
	// already incremented lpszText past the delimeter AND the zero.
	*/
	
	*lplpszText = lpszText;
	
	return nCount > 0;
}

int CProgressBar::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
	{
		return -1;
	}
	
	// copy style DWORD
	m_dwStyle = lpCreateStruct->style;
	
	// by default, use blue as our color
	m_crColor = RGB(0,0,255);
	
	// either parse the text or use defaults for initial range and position
	DWORD dwMin, dwMax, dwPos;
	BOOL fTextRange = FALSE;
	
	if ((PBS_TEXTHASRANGE & m_dwStyle) != 0)
	{
		fTextRange = ParseText((LPSTR)lpCreateStruct->lpszName, (LPSTR)m_szFormat, sizeof(m_szFormat), &dwMin, &dwMax, &dwPos);
	
		/*
		// Verify that the position is in the given range.
		// If the position is outside the range, force it to zero.
		*/

		if (fTextRange)
		{		
			if ((dwPos < dwMin) || (dwPos > dwMax))
			{
				dwPos = 0;
			}
		}
	}
	
	/*
	// Use defaults if we never had PBS_TEXTHASRANGE or
	// ParseText failed.
	*/
	
	if (!fTextRange)
	{
		lstrcpyn(m_szFormat, "%d%%", sizeof(m_szFormat)-1);
	}
	
	m_dwMin = (fTextRange ? dwMin : DEFAULT_MIN);
	m_dwMax = (fTextRange ? dwMax : DEFAULT_MAX);
	m_dwPos = (fTextRange ? dwPos : DEFAULT_POS);

	m_fIsHidden = FALSE;
	Hide();	
	
	return 0;
}

DWORD CProgressBar::GetPosition(DWORD dwNumber)
{
	if (m_dwMin >= m_dwMax)
	{
		return 0L;
	}
	
	if (m_dwPos <= m_dwMin)
	{
		return 0L;
	}
	
	if (m_dwPos >= m_dwMax)
	{
		return dwNumber;
	}
	
	return (DWORD)((((double)dwNumber)*(((double)(m_dwPos-m_dwMin))/((double)(m_dwMax-m_dwMin)))));
}

void CProgressBar::SetPercentage(UINT uPercentage)
{
	if (uPercentage == 0)
	{
		m_dwPos = m_dwMin;
	}
	
	if (uPercentage >= 100)
	{
		m_dwPos = m_dwMax;
	}
	
	m_dwPos = m_dwMin+(DWORD)(((((double)uPercentage)/100.0)*((double)(m_dwMax-m_dwMin))));
}
