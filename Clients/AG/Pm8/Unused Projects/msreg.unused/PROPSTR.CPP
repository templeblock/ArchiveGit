/*
// $Workfile: PROPSTR.CPP $
// $Revision: 1 $
// $Date: 3/03/99 6:23p $
//
// Copyright © 1995 MicroLogic Software, Inc.
// All rights reserved.
*/

/*
// Revision History:
//
// $Log: /PM8/msreg/PROPSTR.CPP $
// 
// 1     3/03/99 6:23p Gbeddow
// 
// 1     7/09/98 2:27p Jayn
// 
//    Rev 1.0   22 Nov 1995 08:52:04   FRED
// Initial revision.
// 
//    Rev 1.2   26 Oct 1995 10:27:54   FRED
//  
*/ 

#include "stdafx.h"

#include "util.h"

#include "propstr.h"

///////////////////////////////////////////////////////////
//
// CMlsPropertyString
//

CMlsPropertyString::CMlsPropertyString()
{
	m_PropertyStrings.RemoveAll();
}

CMlsPropertyString::~CMlsPropertyString()
{
}

BOOL CMlsPropertyString::ExtractProperties(CString& csString)
{
	BOOL fResult = FALSE;

	TRY
	{
		m_PropertyStrings.RemoveAll();

		int nIndex = csString.Find('|');

		if (nIndex >= 0)
		{
			SetProperties(csString.Mid(nIndex+1));
			csString = csString.Left(nIndex);
			fResult = TRUE;
		}
	}
	CATCH_ALL(e)
	{
		fResult = FALSE;
	}
	END_CATCH_ALL

	return fResult;
}

BOOL CMlsPropertyString::SetProperties(const CString& csString)
{
	BOOL fResult = FALSE;
	CString csProperties;
	CString csProperty;

	TRY
	{
		m_PropertyStrings.RemoveAll();

		csProperties = csString+"|";

		while (!csProperties.IsEmpty())
		{
			int nIndex = csProperties.Find('|');

			if (nIndex < 0)
			{
				break;
			}

			csProperty = csProperties.Left(nIndex);
			Util::Trim(csProperty);
			m_PropertyStrings.Add(csProperty);
			csProperties = csProperties.Mid(nIndex+1);
		}

		if (csProperties.IsEmpty())
		{
			fResult = TRUE;
		}
	}
	CATCH_ALL(e)
	{
		fResult = FALSE;
	}
	END_CATCH_ALL

	return fResult;
}

int CMlsPropertyString::Count(void)
{
	return m_PropertyStrings.GetSize();
}

BOOL CMlsPropertyString::AsString(int nIndex, CString& csString, LPCSTR pszDefault /*=NULL*/)
{
	BOOL fResult = FALSE;

	csString = (pszDefault == NULL) ? "" : pszDefault;

	TRY
	{
		if ((nIndex >= 0) && (nIndex < Count()))
		{
			csString = m_PropertyStrings[nIndex];
			fResult = TRUE;
		}
	}
	CATCH_ALL(e)
	{
		csString = (pszDefault == NULL) ? "" : pszDefault;
		fResult = FALSE;
	}
	END_CATCH_ALL

	return fResult;
}

BOOL CMlsPropertyString::AsInt(int nIndex, int& nInteger, int nDefault /*=-1*/)
{
	BOOL fResult = FALSE;
	nInteger = nDefault;

	TRY
	{
		if ((nIndex >= 0) && (nIndex < Count()))
		{
			nInteger = atoi(m_PropertyStrings[nIndex]);
			fResult = TRUE;
		}
	}
	CATCH_ALL(e)
	{
		nInteger = nDefault;
		fResult = FALSE;
	}
	END_CATCH_ALL

	return fResult;
}

int HexValue(char c)
{
	int nResult = -1;

	if ((c >= 'a') && (c <= 'f'))
	{
		nResult = c-'a'+10;
	}
	else if ((c >= 'A') && (c <= 'F'))
	{
		nResult = c-'A'+10;
	}
	else if ((c >= '0') && (c <= '9'))
	{
		nResult = c-'0';
	}

	return nResult;
}

int HexValue(char c0, char c1)
{
	int nResult = -1;

	int n0 = HexValue(c0);

	if (n0 >= 0)
	{
		int n1 = HexValue(c0);

		if (n1 >= 0)
		{
			nResult = (n0<<4)+n1;
		}
	}

	return nResult;
}

BOOL CMlsPropertyString::AsColor(int nIndex, COLORREF& clColor, COLORREF clDefault /*=RGB(0,0,0)*/)
{
	BOOL fResult = FALSE;

	clColor = clDefault;

	CString csColor;
	int i;

	TRY
	{
		if ((nIndex >= 0) && (nIndex < Count()))
		{
			// default is black

			csColor = m_PropertyStrings[nIndex];

			if (!csColor.IsEmpty())
			{

				// check for specific color: $RRGGBB

				if (csColor[0] == '$')
				{
					if (csColor.GetLength() == 7)
					{
						int nRed   = HexValue(csColor[1], csColor[2]);
						int nGreen = HexValue(csColor[3], csColor[4]);
						int nBlue  = HexValue(csColor[5], csColor[6]);

						if ((nRed >= 0) && (nGreen >= 0) && (nBlue >= 0))
						{
							clColor = RGB(nRed, nGreen, nBlue);
							fResult = TRUE;
						}
					}
				}

				else
				{
					// check for named color

					typedef struct
					{
						LPCSTR pszName;
						COLORREF clColor;
					} NamedColor;

					static NamedColor NamedColors[] =
					{
						{ "Black",		RGB(  0,  0,  0) },
						{ "Maroon",		RGB(128,  0,  0) },
						{ "Green",		RGB(  0,128,  0) },
						{ "Olive",		RGB(128,  0,  0) },
						{ "Navy",		RGB(  0,  0,128) },
						{ "Purple",		RGB(128,  0,128) },
						{ "Teal",		RGB(  0,128,128) },
						{ "Gray",		RGB(128,128,128) },
						{ "Silver",		RGB(192,192,192) },
						{ "Red",			RGB(255,  0,  0) },
						{ "Lime",		RGB(  0,255,  0) },
						{ "Yellow",		RGB(255,255,  0) },
						{ "Blue",		RGB(  0,  0,255) },
						{ "Fuchsia",	RGB(255,  0,255) },
						{ "Aqua",		RGB(  0,255,255) },
						{ "LtGray",		RGB(192,192,192) },
						{ "DkGray",		RGB(128,128,128) },
						{ "White",		RGB(255,255,255) }
					};

					for (i = 0; i < sizeof(NamedColors)/sizeof(NamedColors[0]); i++)
					{
						if (stricmp(NamedColors[i].pszName, csColor) == 0)
						{
							clColor = NamedColors[i].clColor;
							fResult = TRUE;
							break;
						}
					}

					if (!fResult)
					{
						// check for system color
								
						typedef struct
						{
							LPCSTR pszName;
							int nColor;
						} SystemColor;

						static SystemColor SystemColors[] =
						{
							{ "ScrollBar",					COLOR_SCROLLBAR				},
							{ "Background",				COLOR_BACKGROUND				},
							{ "ActiveCaption",			COLOR_ACTIVECAPTION			},
							{ "InactiveCaption",			COLOR_INACTIVECAPTION		},
							{ "Menu",						COLOR_MENU						},
							{ "Window",						COLOR_WINDOW					},
							{ "WindowFrame",				COLOR_WINDOWFRAME				},
							{ "MenuText",					COLOR_MENUTEXT					},
							{ "WindowText",				COLOR_WINDOWTEXT				},
							{ "CaptionText",				COLOR_CAPTIONTEXT				},
							{ "ActiveBorder",				COLOR_ACTIVEBORDER			},
							{ "InactiveBorder",			COLOR_INACTIVEBORDER			},
							{ "AppWorkSpace",				COLOR_APPWORKSPACE			},
							{ "Highlight",					COLOR_HIGHLIGHT				},
							{ "HighlightText",			COLOR_HIGHLIGHTTEXT			},
							{ "BtnFace",					COLOR_BTNFACE					},
							{ "BtnShadow",					COLOR_BTNSHADOW				},
							{ "GrayText",					COLOR_GRAYTEXT					},
							{ "BtnText",					COLOR_BTNTEXT					},
							{ "InactiveCaptionText",	COLOR_INACTIVECAPTIONTEXT	},
							{ "BtnHighlight",				COLOR_BTNHIGHLIGHT			}
						};

						for (i = 0; i < sizeof(SystemColors)/sizeof(SystemColors[0]); i++)
						{
							if (stricmp(SystemColors[i].pszName, csColor) == 0)
							{
								clColor = GetSysColor(SystemColors[i].nColor);
								fResult = TRUE;
								break;
							}
						}
					}
				}
			}
		}
	}
	CATCH_ALL(e)
	{
		clColor = clDefault;
		fResult = FALSE;
	}
	END_CATCH_ALL

	return fResult;
}
