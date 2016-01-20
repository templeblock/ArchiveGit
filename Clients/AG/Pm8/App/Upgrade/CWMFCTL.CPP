/*
// $Workfile: CWMFCTL.CPP $
// $Revision: 1 $
// $Date: 3/03/99 6:13p $
//
// Copyright © 1995 MicroLogic Software, Inc.
// All rights reserved.
*/

/*
// Revision History:
//
// $Log: /PM8/App/UPGRADE/CWMFCTL.CPP $
// 
// 1     3/03/99 6:13p Gbeddow
// 
//    Rev 1.0   14 Aug 1997 15:41:40   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 10:33:12   Fred
// Initial revision.
// 
//    Rev 1.1   12 Jun 1997 16:42:58   Fred
// New class names
// 
//    Rev 1.0   08 Sep 1996 09:40:26   Fred
// Initial revision.
// 
//    Rev 1.0   02 Feb 1996 08:57:52   JAY
// Initial revision.
// 
//    Rev 1.0   22 Nov 1995 08:52:06   FRED
// Initial revision.
// 
//    Rev 1.1   26 Oct 1995 10:27:54   FRED
//  
// 
//    Rev 1.0   25 Oct 1995 10:06:18   JAY
// Initial revision.
*/ 

#include "stdafx.h"
#include "cwmfctl.h"
#include "propstr.h"

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

	static const char szClassName[] = "InstantUpgradeWmfControl";

	if (::GetClassInfo(AfxGetInstanceHandle(), szClassName, &wcls))
	{
		return (wcls.lpfnWndProc == CWindowsMetaFileControl::FirstMsgOnlyWndProc);
	}

	wcls.style = 0;
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
#ifdef USEWMF
	m_pMetaFile = NULL;
#endif
}

CWindowsMetaFileControl::~CWindowsMetaFileControl()
{
#ifdef USEWMF
	delete m_pMetaFile;
#endif
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

#ifdef USEWMF
			delete m_pMetaFile;
			m_pMetaFile = NULL;
#endif

			GetWindowText(csText);

			CMlsPropertyString Properties;

			Properties.SetProperties(csText);

			if (CollectAlignmentFlags(Properties, m_nAlignmentFlags))
			{
				if (CollectType(Properties, m_nType))
				{
					switch (m_nType)
					{
						case TYPE_SHAPE:
						{
							// Collect Shape Type

							if (CollectShape(Properties, m_nShape))
							{
								// Collect Shape Color

								if (Properties.AsColor(3, m_clShapeColor))
								{
									m_nStrokeWidth = 0;
									m_clStrokeColor = m_clShapeColor;
									m_fShapeShadow = FALSE;

								/*
								// We want to be able to allow blank (default) propeties.
								// So you could say "|black|||green|" to get a black
								// object with a green shadow.
								*/
									int nCount = Properties.Count();
									if (nCount > 4)
									{
										Properties.AsColor(4, m_clStrokeColor, m_clStrokeColor);
										Properties.AsInt(5, m_nStrokeWidth, 1);
										m_fShapeShadow = Properties.AsColor(6, m_clShapeShadowColor);
									}

									fResult = TRUE;
								}
							}

							break;
						}

						case TYPE_WMF:
						{
#ifdef USEWMF
							// Collect WMF Resource Name

							if (CollectName(Properties, m_csResourceName))
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
#else
							// Needs #define USEWMF!
							ASSERT(FALSE);
#endif

							break;
						}

						case TYPE_BMP:
						{
							// Collect BMP Resource Name

							if (CollectName(Properties, m_csResourceName))
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

BOOL CWindowsMetaFileControl::CollectName(CMlsPropertyString& Properties, CString& csName)
{
	return Properties.AsString(2, csName);
}

BOOL CWindowsMetaFileControl::CollectAlignmentFlags(CMlsPropertyString& Properties, int& nAlignmentFlags, BOOL fUseDefault /*=TRUE*/, int nDefault /*=ASPECT_PRESERVE|HORIZONTAL_ALIGN_CENTER|VERTICAL_ALIGN_MIDDLE*/)
{
	BOOL fResult = FALSE;

	CString csFlags;

	if (Properties.AsString(0, csFlags))
	{
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

BOOL CWindowsMetaFileControl::CollectShape(CMlsPropertyString& Properties, int& nShape, BOOL fUseDefault /*=TRUE*/, int nDefault /*=SHAPE_SQUARE*/)
{
	BOOL fResult = TRUE;

	CString csShape;

	Properties.AsString(2, csShape);
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

BOOL CWindowsMetaFileControl::CollectType(CMlsPropertyString& Properties, int& nType, BOOL fUseDefault /*=TRUE*/, int nDefault /*=TYPE_SHAPE*/)
{
	BOOL fResult = TRUE;

	CString csType;

	Properties.AsString(1, csType);

	csType.MakeUpper();

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

					DrawShape(&dc, m_nShape, &crShape, m_clShapeColor, m_clStrokeColor, m_nStrokeWidth);

					break;
				}

				case TYPE_WMF:
				{
#ifdef USEWMF
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
#else
				// Needs #define USEWMF!
					ASSERT(FALSE);
#endif
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

void CWindowsMetaFileControl::DrawShape(CDC* pDC,
													 int nShape,
													 CRect* pcrBounds,
													 COLORREF clColor,
													 COLORREF clStrokeColor /*=COLORREF(-1)*/,
													 int nStrokeWidth /*=0*/)
{
	CBrush cbColor;
	CPen cpColor;

	pDC->SaveDC();

	if (clStrokeColor == COLORREF(-1))
	{
		clStrokeColor = clColor;
	}

	TRY
	{
		int nWidth = pcrBounds->Width();
		int nHeight = pcrBounds->Height();

		if ((nWidth > 0) && (nHeight > 0))
		{
			if (cbColor.CreateSolidBrush(clColor))
			{
				if (cpColor.CreatePen(PS_INSIDEFRAME, nStrokeWidth, clStrokeColor))
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
