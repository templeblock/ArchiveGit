/*
// $Workfile: CWMFCTL.CPP $
// $Revision: 1 $
// $Date: 3/03/99 6:04p $
*/

/*
// Revision History:
//
// $Log: /PM8/App/CWMFCTL.CPP $
// 
// 1     3/03/99 6:04p Gbeddow
// 
//    Rev 1.0   14 Aug 1997 15:19:50   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:37:58   Fred
// Initial revision.
// 
//    Rev 1.2   22 Oct 1996 13:53:50   johno
// Changes for resource DLL (RegisterMyClass)
// 
//    Rev 1.1   31 Jul 1996 11:57:06   Fred
// 16-bit compatibility
// 
//    Rev 1.0   14 Mar 1996 13:36:18   Jay
// Initial revision.
// 
//    Rev 1.0   16 Feb 1996 12:27:54   FRED
// Initial revision.
// 
//    Rev 1.2   05 Jul 1995 11:15:46   JAY
// Now SaveDC and RestoreDC.
// 
//    Rev 1.1   01 Jul 1995 09:27:40   FRED
// More "Make Your Own" stuff.
// 
//    Rev 1.0   30 Jun 1995 13:26:42   FRED
// Initial revision.
*/ 

#include "stdafx.h"
#include "cwmfctl.h"

#ifndef WIN32
#ifndef COLOR_BTNHILIGHT
#define	COLOR_BTNHILIGHT	COLOR_BTNHIGHLIGHT
#endif
#endif

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CWindowsMetaFileControl

WNDPROC CWindowsMetaFileControl::m_pfnSuperWndProc = NULL;

LONG CALLBACK AFX_EXPORT CWindowsMetaFileControl::FirstMsgOnlyWndProc(HWND hWnd, UINT msg, UINT wParam, LONG lParam)
{
	/*
	// Construct the object and then attach the hWnd to it.
	*/
	
	CWindowsMetaFileControl* pWindowsMetaFileControl = new CWindowsMetaFileControl;
	pWindowsMetaFileControl->Attach(hWnd);
	
	pWindowsMetaFileControl->m_fCreatedByClassName = TRUE;

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

BOOL CWindowsMetaFileControl::RegisterMyClass()
{
	WNDCLASS wcls;
	
	m_pfnSuperWndProc = ::DefWindowProc;
	
	static const char szClassName[] = "MlsWmfControl";
	
	if (::GetClassInfo(AfxGetInstanceHandle(), szClassName, &wcls))
	{
		return (wcls.lpfnWndProc == CWindowsMetaFileControl::FirstMsgOnlyWndProc);
	}
	
	// JRO
  // wcls.style = 0;
  wcls.style = CS_GLOBALCLASS;
	wcls.cbClsExtra = 0;
	wcls.cbWndExtra = 0;
	wcls.lpfnWndProc = CWindowsMetaFileControl::FirstMsgOnlyWndProc;
	wcls.hIcon = NULL;
	wcls.hCursor = ::LoadCursor(NULL, IDC_ARROW);
	wcls.hInstance = AfxGetInstanceHandle();
	wcls.hbrBackground = NULL;
	wcls.lpszMenuName = NULL;
	wcls.lpszClassName = szClassName;
	
	return (RegisterClass(&wcls) != 0);
}

void CWindowsMetaFileControl::PostNcDestroy()
{
	if (m_fCreatedByClassName)
	{
		delete this;
	}
}

CWindowsMetaFileControl::CWindowsMetaFileControl()
{
	m_fCreatedByClassName = FALSE;
	m_fUnpacked = FALSE;
	m_pMetaFile = NULL;
}

CWindowsMetaFileControl::~CWindowsMetaFileControl()
{
	delete m_pMetaFile;
}

BEGIN_MESSAGE_MAP(CWindowsMetaFileControl, CWnd)
	//{{AFX_MSG_MAP(CWindowsMetaFileControl)
	ON_WM_PAINT()
	ON_WM_SIZE()
	ON_WM_CREATE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BOOL CWindowsMetaFileControl::Unpack(void)
{
	CString csText;
	
	BOOL fResult = TRUE;
	
	TRY
	{
		if (!m_fUnpacked)
		{
			fResult = FALSE;
			
			delete m_pMetaFile;
			m_pMetaFile = NULL;
			
			GetWindowText(csText);
			
			if (CollectAlignmentFlags(csText, m_nAlignmentFlags))
			{
				if (CollectType(csText, m_nType))
				{
					switch (m_nType)
					{
						case TYPE_SHAPE:
						{
							// Collect Shape Type
							
							if (CollectShape(csText, m_nShape))
							{
								// Collect Shape Color
								
								if (CollectColor(csText, m_clShapeColor))
								{
									m_fShapeShadow = CollectColor(csText, m_clShapeShadowColor, FALSE);
									
									fResult = TRUE;
								}
							}
								
							break;
						}
						
						case TYPE_WMF:
						{
							// Collect WMF Resource Name
							
							if (CollectName(csText, m_csResourceName))
							{
								if (!m_csResourceName.IsEmpty())
								{
									m_pMetaFile = new CWindowsMetaFile;
									ASSERT(m_pMetaFile != NULL);
									
									if (m_pMetaFile != NULL)
									{
										if (m_pMetaFile->Init(AfxGetResourceHandle(), m_csResourceName, "METAFILE"))
										{
											fResult = TRUE;
										}
									}
								
								}
							}
							
							break;
						}
						
						case TYPE_BMP:
						{
							// Collect BMP Resource Name
							
							if (CollectName(csText, m_csResourceName))
							{
								fResult = TRUE;
							}
							
							break;
						}
						
						default:
						{
							ASSERT(FALSE);
							break;
						}
					}
				}
			}
		}
	}
	CATCH_ALL(e)
	{
		fResult = FALSE;
	}
	END_CATCH_ALL
	
	m_fUnpacked = fResult;
	
	return fResult;
}

BOOL CWindowsMetaFileControl::CollectWord(CString& csText, CString& csWord)
{
	BOOL fResult = FALSE;
	
	int nSeparator = csText.Find(';');
			
	if (nSeparator < 0)
	{
		csWord = csText;
		csText.Empty();
	}
	else
	{
		csWord = csText.Left(nSeparator);
		csText = csText.Mid(nSeparator+1);
	}
	
	fResult = csWord.GetLength() > 0;
	
	return fResult;
}

BOOL CWindowsMetaFileControl::CollectName(CString& csText, CString& csName)
{
	BOOL fResult = FALSE;
	
	CollectWord(csText, csName);
	
	fResult = csName.GetLength() > 0;
	
	return fResult;
}

BOOL CWindowsMetaFileControl::CollectAlignmentFlags(CString& csText, int& nAlignmentFlags, BOOL fUseDefault /*=TRUE*/, int nDefault /*=ASPECT_PRESERVE|HORIZONTAL_ALIGN_CENTER|VERTICAL_ALIGN_MIDDLE*/)
{
	BOOL fResult = FALSE;
	
	CString csFlags;
	
	CollectWord(csText, csFlags);
	csFlags.MakeUpper();
	
	if (csFlags.GetLength() == 3)
	{
		fResult = TRUE;
		
		nAlignmentFlags = 0;
		
		if (fResult)
		{
			switch (csFlags[0])
			{
				case 'S':	nAlignmentFlags |= ASPECT_STRETCH;				break;
				case 'P':	nAlignmentFlags |= ASPECT_PRESERVE;				break;
				default:		fResult = FALSE;										break;
			}
		}
	
		if (fResult)
		{
			switch (csFlags[1])
			{
				case 'L':	nAlignmentFlags |= HORIZONTAL_ALIGN_LEFT;		break;
				case 'C':	nAlignmentFlags |= HORIZONTAL_ALIGN_CENTER;	break;
				case 'R':	nAlignmentFlags |= HORIZONTAL_ALIGN_RIGHT;	break;
				default:		fResult = FALSE;										break;
			}
		}
			
		if (fResult)
		{
			switch (csFlags[2])
			{
				case 'T':	nAlignmentFlags |= VERTICAL_ALIGN_TOP;			break;
				case 'M':	nAlignmentFlags |= VERTICAL_ALIGN_MIDDLE;		break;
				case 'B':	nAlignmentFlags |= VERTICAL_ALIGN_BOTTOM;		break;
				default:		fResult = FALSE;										break;
			}
		}
	}
	
	if (!fResult)
	{
		if (fUseDefault)
		{
			nAlignmentFlags = nDefault;
			fResult = TRUE;
		}
	}
	
	return fResult;
}

BOOL CWindowsMetaFileControl::CollectShape(CString& csText, int& nShape, BOOL fUseDefault /*=TRUE*/, int nDefault /*=SHAPE_SQUARE*/)
{
	BOOL fResult = TRUE;
	
	CString csShape;
	
	CollectWord(csText, csShape);
	csShape.MakeUpper();
	
	if (csShape == "SQUARE")
	{
		nShape = SHAPE_SQUARE;
	}
	else if (csShape == "CIRCLE")
	{
		nShape = SHAPE_CIRCLE;
	}
	else if (csShape == "TRIANGLEUP")
	{
		nShape = SHAPE_TRIANGLEUP;
	}
	else if (csShape == "TRIANGLEDOWN")
	{
		nShape = SHAPE_TRIANGLEDOWN;
	}
	else if (csShape == "TRIANGLELEFT")
	{
		nShape = SHAPE_TRIANGLELEFT;
	}
	else if (csShape == "TRIANGLERIGHT")
	{
		nShape = SHAPE_TRIANGLERIGHT;
	}
	else
	{
		if (fUseDefault)
		{
			nShape = nDefault;
		}
		else
		{
			fResult = FALSE;
		}
	}
	
	return fResult;
}

BOOL CWindowsMetaFileControl::CollectColor(CString& csText, COLORREF& clColor, BOOL fUseDefault /*=TRUE*/, COLORREF clDefault /*=RGB(0,0,0)*/)
{
	BOOL fResult = FALSE;
	
	CString csColor;
	
	CollectWord(csText, csColor);
	csColor.MakeUpper();
	
	if (!csColor.IsEmpty())
	{
		fResult = TRUE;
		
		if ((csColor.GetLength() > 1) && (csColor[0] == '$'))
		{
			clColor = 0;
			
			for (int i = 1; i < csColor.GetLength(); i++)
			{
				if ((csColor[i] >= '0') && (csColor[i] <= '9'))
				{
					clColor = (clColor << 4)+(csColor[i]-'0');
				}
				else if ((csColor[i] >= 'A') && (csColor[i] <= 'F'))
				{
					clColor = (clColor << 4)+(csColor[i]-'A'+10);
				}
				else
				{
					break;
				}
			}
			
			fResult = i == csColor.GetLength();
		}
		else if (csColor == "ACTIVEBORDER")
		{
			clColor = GetSysColor(COLOR_ACTIVEBORDER);
		}
		else if (csColor == "ACTIVECAPTION")
		{
			clColor = GetSysColor(COLOR_ACTIVECAPTION);
		}
		else if (csColor == "APPWORKSPACE")
		{
			clColor = GetSysColor(COLOR_APPWORKSPACE);
		}
		else if (csColor == "BACKGROUND")
		{
			clColor = GetSysColor(COLOR_BACKGROUND);
		}
		else if (csColor == "BTNFACE")
		{
			clColor = GetSysColor(COLOR_BTNFACE);
		}
		else if (csColor == "BTNHILIGHT")
		{
			clColor = GetSysColor(COLOR_BTNHILIGHT);
		}
		else if (csColor == "BTNSHADOW")
		{
			clColor = GetSysColor(COLOR_BTNSHADOW);
		}
		else if (csColor == "BTNTEXT")
		{
			clColor = GetSysColor(COLOR_BTNTEXT);
		}
		else if (csColor == "CAPTIONTEXT")
		{
			clColor = GetSysColor(COLOR_CAPTIONTEXT);
		}
		else if (csColor == "GRAYTEXT")
		{
			clColor = GetSysColor(COLOR_GRAYTEXT);
		}
		else if (csColor == "HIGHLIGHT")
		{
			clColor = GetSysColor(COLOR_HIGHLIGHT);
		}
		else if (csColor == "HIGHLIGHTTEXT")
		{
			clColor = GetSysColor(COLOR_HIGHLIGHTTEXT);
		}
		else if (csColor == "INACTIVEBORDER")
		{
			clColor = GetSysColor(COLOR_INACTIVEBORDER);
		}
		else if (csColor == "INACTIVECAPTION")
		{
			clColor = GetSysColor(COLOR_INACTIVECAPTION);
		}
		else if (csColor == "INACTIVECAPTIONTEXT")
		{
			clColor = GetSysColor(COLOR_INACTIVECAPTIONTEXT);
		}
		else if (csColor == "MENU")
		{
			clColor = GetSysColor(COLOR_MENU);
		}
		else if (csColor == "MENUTEXT")
		{
			clColor = GetSysColor(COLOR_MENUTEXT);
		}
		else if (csColor == "SCROLLBAR")
		{
			clColor = GetSysColor(COLOR_SCROLLBAR);
		}
		else if (csColor == "WINDOW")
		{
			clColor = GetSysColor(COLOR_WINDOW);
		}
		else if (csColor == "WINDOWFRAME")
		{
			clColor = GetSysColor(COLOR_WINDOWFRAME);
		}
		else if (csColor == "WINDOWTEXT")
		{
			clColor = GetSysColor(COLOR_WINDOWTEXT);
		}
		else
		{
			TRACE("Unknown color %s\n", (LPCSTR)csColor);
			fResult = FALSE;
		}
	}
	
	if (!fResult)
	{
		if (fUseDefault)
		{
			clColor = clDefault;
			fResult = TRUE;
		}
	}
	
	return fResult;
}

BOOL CWindowsMetaFileControl::CollectType(CString& csText, int& nType, BOOL fUseDefault /*=TRUE*/, int nDefault /*=TYPE_SHAPE*/)
{
	BOOL fResult = TRUE;
	
	CString csType;
	
	CollectWord(csText, csType);
	
	if (csType == "SHAPE")
	{
		nType = TYPE_SHAPE;
	}
	else if (csType == "WMF")
	{
		nType = TYPE_WMF;
	}
	else if (csType == "BMP")
	{
		nType = TYPE_BMP;
	}
	else
	{
		if (fUseDefault)
		{
			nType = nDefault;
		}
		else
		{
			fResult = FALSE;
		}
	}
	
	return fResult;
}

/////////////////////////////////////////////////////////////////////////////
// CWindowsMetaFileControl message handlers

int CWindowsMetaFileControl::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	return CWnd::OnCreate(lpCreateStruct);
}

void CWindowsMetaFileControl::OnPaint()
{
	CPaintDC dc(this);
	
	TRY
	{
		if (Unpack())
		{
			switch (m_nType)
			{
				case TYPE_SHAPE:
				{
					// Compute the aligned destination rectangle.
					
					CRect crShape;
					
					switch (m_nShape)
					{
						case SHAPE_CIRCLE:
						case SHAPE_SQUARE:
						{
							crShape.SetRect(0,0,1000,1000);
							break;
						}
						
						case SHAPE_TRIANGLEUP:
						case SHAPE_TRIANGLEDOWN:
						{
							crShape.SetRect(0,0,1000,500);
							break;
						}
											
						case SHAPE_TRIANGLELEFT:
						case SHAPE_TRIANGLERIGHT:
						{
							crShape.SetRect(0,0,500,1000);
							break;
						}
							
						default:
						{
							ASSERT(FALSE);
							crShape.SetRect(0,0,1000,1000);
							break;
						}
					}
					
					AlignBounds(crShape);
					
					if (m_fShapeShadow)
					{
						int nXOffset = MulDiv(crShape.Width(), 1, 5);
						int nYOffset = MulDiv(crShape.Height(), 1, 5);
						
						CRect crShadow(crShape);
						crShadow.left += nXOffset;
						crShadow.top += nYOffset;
						
						DrawShape(&dc, m_nShape, &crShadow, m_clShapeShadowColor);
						
						crShape.right -= nXOffset;
						crShape.bottom -= nYOffset;
					}
					
					DrawShape(&dc, m_nShape, &crShape, m_clShapeColor);
					
					break;
				}
				
				case TYPE_WMF:
				{
					if (m_pMetaFile != NULL)
					{
						const ALDUS_WMF_HEADER* pHeader = m_pMetaFile->AldusHeader();
							
						if (pHeader != NULL)
						{
							// Compute the aligned destination rectangle.
							
							CRect crWmf(0, 0, pHeader->bbox.right-pHeader->bbox.left, pHeader->bbox.bottom-pHeader->bbox.top);
							AlignBounds(crWmf);
			
							// Set the viewport.
					
							dc.SetMapMode(MM_ANISOTROPIC);
							dc.SetViewportOrg(crWmf.left, crWmf.top);
							dc.SetViewportExt(crWmf.Width(), crWmf.Height());
							m_pMetaFile->Play(&dc, FALSE);
						}
					}
					
					break;
				}
				
				case TYPE_BMP:
				{
					break;
				}
				
				default:
				{
					ASSERT(FALSE);
					break;
				}
			}
		}
	}
	END_TRY
					
	// Do not call CWnd::OnPaint() for painting messages
}

void CWindowsMetaFileControl::AlignBounds(CRect& crBounds)
{
	CRect crClient;
	GetClientRect(crClient);
							
	if (Aspect() == ASPECT_STRETCH)
	{
		crBounds = crClient;
	}
	else
	{
		int nImageWidth = crBounds.Width();
		int nImageHeight = crBounds.Height();
									
		int nTargetWidth = crClient.Width();
		int nTargetHeight = crClient.Height();
									
		int nNumerator = nTargetWidth;
		int nDenominator = nImageWidth;
									
		if (((long)nTargetWidth*(long)nImageHeight) > ((long)nTargetHeight*(long)nImageWidth))
		{
			nNumerator = nTargetHeight;
			nDenominator = nImageHeight;
		}
										
		nImageWidth = MulDiv(nImageWidth, nNumerator, nDenominator);
		nImageHeight = MulDiv(nImageHeight, nNumerator, nDenominator);
								
		crBounds.SetRect(0, 0, nImageWidth, nImageHeight);
								
		int nXOffset = 0;
								
		if (HorizontalAlignment() != HORIZONTAL_ALIGN_LEFT)
		{
			nXOffset = nTargetWidth-nImageWidth;
									
			if (HorizontalAlignment() == HORIZONTAL_ALIGN_CENTER)
			{
				nXOffset /= 2;
			}
		}
								
		int nYOffset = 0;
								
		if (VerticalAlignment() != VERTICAL_ALIGN_TOP)
		{
			int nYOffset = nTargetHeight-nImageHeight;
									
			if (VerticalAlignment() == VERTICAL_ALIGN_MIDDLE)
			{
				nYOffset /= 2;
			}
		}
								
		crBounds.OffsetRect(nXOffset+crClient.left, nYOffset+crClient.top);
	}
}

void CWindowsMetaFileControl::DrawShape(CDC* pDC, int nShape, CRect* pcrBounds, COLORREF clColor)
{
	CBrush cbColor;
	CPen cpColor;
	
	pDC->SaveDC();

	TRY
	{
		int nWidth = pcrBounds->Width();
		int nHeight = pcrBounds->Height();

		if ((nWidth > 0) && (nHeight > 0))
		{
			if (cbColor.CreateSolidBrush(clColor))
			{
				if (cpColor.CreatePen(PS_SOLID, 0, clColor))
				{
					pDC->SelectObject(&cbColor);
					pDC->SelectObject(&cpColor);
					
					if (nShape == SHAPE_CIRCLE)
					{
						pDC->Ellipse(pcrBounds);
					}
					else if (nShape == SHAPE_SQUARE)
					{
						pDC->Rectangle(pcrBounds);
					}
					else
					{
						int x0, y0;
						int x1, y1;
						int x2, y2;
						
						switch (nShape)
						{
							case SHAPE_TRIANGLEUP:
							{
								x0 = pcrBounds->left;
								x1 = pcrBounds->right-1;
								x2 = (x0+x1)/2;
								
								y0 = pcrBounds->bottom-1;
								y1 = y0;
								y2 = pcrBounds->top;
								
								if ((nWidth%2) == 0)
								{
									x1--;
								}
								
								break;
							}
											
							case SHAPE_TRIANGLEDOWN:
							{
								x0 = pcrBounds->left;
								x1 = pcrBounds->right-1;
								x2 = (x0+x1)/2;
								
								y0 = pcrBounds->top;
								y1 = y0;
								y2 = pcrBounds->bottom-1;
								
								if ((nWidth%2) == 0)
								{
									x1--;
								}
								
								break;
							}
											
							case SHAPE_TRIANGLELEFT:
							{
								x0 = pcrBounds->right-1;
								x1 = x0;
								x2 = pcrBounds->left;
								
								y0 = pcrBounds->top;
								y1 = pcrBounds->bottom-1;
								y2 = (y0+y1)/2;
								
								if ((nHeight%2) == 0)
								{
									y1--;
								}
								
								break;
							}
											
							case SHAPE_TRIANGLERIGHT:
							{
								x0 = pcrBounds->left;
								x1 = x0;
								x2 = pcrBounds->right-1;
								
								y0 = pcrBounds->top;
								y1 = pcrBounds->bottom-1;
								y2 = (y0+y1)/2;
								
								if ((nHeight%2) == 0)
								{
									y1--;
								}
								
								break;
							}
							
							default:
							{
								ASSERT(FALSE);
								break;
							}
						}
						
						POINT Points[4];
						
						Points[0].x = x0;
						Points[0].y = y0;
						
						Points[1].x = x1;
						Points[1].y = y1;
						
						Points[2].x = x2;
						Points[2].y = y2;
						
						Points[3].x = x0;
						Points[3].y = y0;
						
						pDC->Polygon(&Points[0], 4);
					}
				}
			}
		}
	}
	END_TRY

	pDC->RestoreDC(-1);
}
						
void CWindowsMetaFileControl::OnSize(UINT nType, int cx, int cy)
{
	CWnd::OnSize(nType, cx, cy);
}
