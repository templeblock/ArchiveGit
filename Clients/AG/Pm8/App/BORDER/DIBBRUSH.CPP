/*
// $Workfile: DIBBRUSH.CPP $
// $Revision: 1 $
// $Date: 3/03/99 6:03p $
*/

/*
// Revision History:
//
// $Log: /PM8/App/BORDER/DIBBRUSH.CPP $
// 
// 1     3/03/99 6:03p Gbeddow
// 
// 1     4/22/98 11:44a Fredf
*/ 

#include "stdafx.h"
#include "dibbrush.h"

BYTE const aDividedBy51[256] =
{
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
  2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
  2, 2, 2, 2, 2, 2, 2, 2, 2, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
  3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
  3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
  4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
  4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 5, 
};

BYTE const aModulo51[256] =
{
  0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19,
  20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37,
  38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 0, 1, 2, 3, 4, 5, 6,
  7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25,
  26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43,
  44, 45, 46, 47, 48, 49, 50, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12,
  13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30,
  31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48,
  49, 50, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17,
  18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35,
  36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 0, 1, 2, 3,
  4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22,
  23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40,
  41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 0, 
};

BYTE const near aTimes6[6] =
{
  0, 6, 12, 18, 24, 30
};

BYTE const aTimes36[6] =
{
  0, 36, 72, 108, 144, 180
};

typedef BYTE DITHER_MATRIX[8][8];

DITHER_MATRIX palette_dither_matrix =
{
	01, 32,  8, 40,  2, 34, 10, 42,
	48, 16, 56, 24, 50, 18, 58, 26,
	12, 44,  4, 36, 14, 46,  6, 38,
	60, 28, 52, 20, 62, 30, 54, 22,
	 3, 35, 11, 43,  1, 33,  9, 41,
	51, 19, 59, 27, 49, 17, 57, 25,
	15, 47,  7, 39, 13, 45,  5, 37,
	63, 31, 55, 23, 61, 29, 53, 21
};

/***************************************************************************
  aWinGHalftoneTranslation

  Translates a 2.6 bit-per-pixel halftoned representation into the
  slightly rearranged WinG Halftone Palette.
*/

BYTE const near aWinGHalftoneTranslation[216] =
{
  0,
  29,
  30,
  31,
  32,
  249,
  33,
  34,
  35,
  36,
  37,
  38,
  39,
  40,
  41,
  42,
  43,
  44,
  45,
  46,
  47,
  48,
  49,
  50,
  51,
  52,
  53,
  54,
  55,
  56,
  250,
  250,
  57,
  58,
  59,
  251,
  60,
  61,
  62,
  63,
  64,
  65,
  66,
  67,
  68,
  69,
  70,
  71,
  72,
  73,
  74,
  75,
  76,
  77,
  78,
  79,
  80,
  81,
  82,
  83,
  84,
  85,
  86,
  87,
  88,
  89,
  250,
  90,
  91,
  92,
  93,
  94,
  95,
  96,
  97,
  98,
  99,
  100,
  101,
  102,
  103,
  104,
  105,
  106,
  107,
  108,
  109,
  110,
  111,
  227,
  112,
  113,
  114,
  115,
  116,
  117,
  118,
  119,
  151,
  120,
  121,
  122,
  123,
  124,
  228,
  125,
  126,
  229,
  133,
  162,
  135,
  131,
  132,
  137,
  166,
  134,
  140,
  130,
  136,
  143,
  138,
  139,
  174,
  141,
  142,
  177,
  129,
  144,
  145,
  146,
  147,
  148,
  149,
  150,
  157,
  152,
  153,
  154,
  155,
  156,
  192,
  158,
  159,
  160,
  161,
  196,
  163,
  164,
  165,
  127,
  199,
  167,
  168,
  169,
  170,
  171,
  172,
  173,
  207,
  175,
  176,
  210,
  178,
  179,
  180,
  181,
  182,
  183,
  184,
  185,
  186,
  187,
  188,
  189,
  190,
  191,
  224,
  193,
  194,
  195,
  252,
  252,
  197,
  198,
  128,
  253,
  252,
  200,
  201,
  202,
  203,
  204,
  205,
  206,
  230,
  208,
  209,
  231,
  211,
  212,
  213,
  214,
  215,
  216,
  217,
  218,
  219,
  220,
  221,
  222,
  254,
  223,
  232,
  225,
  226,
  255,
};

void Create8x8Dither(BYTE bRed, BYTE bGreen, BYTE bBlue, LPBYTE pData)
{
	BYTE bBaseRed = aDividedBy51[bRed];
	BYTE bBaseGreen = aDividedBy51[bGreen];
	BYTE bBaseBlue = aDividedBy51[bBlue];

	BYTE bBaseIndex = aTimes36[bBaseBlue]
							+ aTimes6[bBaseGreen]
							+ bBaseRed;

	BYTE bDitherRed = aModulo51[bRed];
	BYTE bDitherGreen = aModulo51[bGreen];
	BYTE bDitherBlue = aModulo51[bBlue];

	// Start at the top; do all 64 values.

	BYTE near * pDitherByte = palette_dither_matrix[0];

	for (int i = 0; i < 8*8; i++)
	{
		BYTE bDitherValue = *pDitherByte++;
		BYTE bIndex = bBaseIndex;

		if (bDitherRed >= bDitherValue)
		{
			bIndex++;				// Increment red component.
		}
		if (bDitherGreen >= bDitherValue)
		{
			bIndex += 6;			// Increment green component.
		}
		if (bDitherBlue >= bDitherValue)
		{
			bIndex += 36;			// Increment blue component.
		}
		*pData++ = aWinGHalftoneTranslation[bIndex];
	}
}

/////////////////////////////////////////////////////////////////////////////
// CDibPatternBrush

CDibPatternBrush::DIBPATTERN CDibPatternBrush::DibPattern =
{
	{
		sizeof(BITMAPINFOHEADER),
		8L,
		8L,
		1,
		8,
		BI_RGB,
		8*8,
		0L,
		0L,
		256L,
		256L
	},
	{
		0, 0
	}
};

CDibPatternBrush::CDibPatternBrush()
{
	m_hData = NULL;
	m_pData = NULL;
	m_rgbColor = RGB(0,0,0);
	m_fUsePalette = TRUE;
}

CDibPatternBrush::~CDibPatternBrush()
{
	if (m_hData != NULL)
	{
		if (m_pData != NULL)
		{
			::GlobalUnlock(m_hData);
			m_pData = NULL;
		}
		
		::GlobalFree(m_hData);
		m_hData = NULL;
	}
}

void CDibPatternBrush::UsePalette(BOOL fUsePalette /*=TRUE*/)
{
	if (m_fUsePalette != fUsePalette)
	{
		m_fUsePalette = fUsePalette;
		// Invalidate object.
		if (m_DibBrush.GetSafeHandle() != NULL)
		{
			m_DibBrush.DeleteObject();
		}
	}
}

BOOL CDibPatternBrush::Color(COLORREF rgbColor)
{
	BOOL fResult = FALSE;
	
	// Allocate the memory if necessary.
	
	if (m_hData == NULL)
	{
		m_hData = ::GlobalAlloc(GMEM_MOVEABLE, sizeof(DIBPATTERN));
			
		// Lock the memory if it exists.
		
		if (m_hData != NULL)
		{
			m_pData = (LPBYTE)::GlobalLock(m_hData);
				
			if (m_pData == NULL)
			{
				GlobalFree(m_hData);
				m_hData = NULL;
			}
			else
			{
				// Initialize the color array if not initialized yet.
			
				if (DibPattern.bmiColors[1] == 0)
				{
					SHORT *q;
					SHORT i;
			
					for (i = 0, q = DibPattern.bmiColors; i < 256; )
					{
						*q++ = i++;
					}
				}
		
				// Copy the base information over.
		
				memcpy(m_pData, &DibPattern, sizeof(DIBPATTERN));
		
				m_rgbColor = RGB(0,0,0);
			}
		}
	}
	
	if (m_pData != NULL)
	{
		// Prepare the dib pattern if it's not correct.

		if (rgbColor != m_rgbColor)
		{
			// Delete any old brush we had since it's now obsolete.

			m_DibBrush.DeleteObject();

			// Create the dither for the new color.

			Create8x8Dither(GetRValue(rgbColor), GetGValue(rgbColor), GetBValue(rgbColor), ((DIBPATTERN*)m_pData)->bmiData);

			m_rgbColor = rgbColor;
		}

		// Create the brush if we need to.

		if (m_DibBrush.GetSafeHandle() == NULL)
		{
			if (m_fUsePalette)
			{
				m_DibBrush.CreateDIBPatternBrush(m_hData, DIB_PAL_COLORS);
			}
			else
			{
				m_DibBrush.CreateSolidBrush(m_rgbColor);
			}
		}
		
		fResult = m_DibBrush.GetSafeHandle() != NULL;
	}
	
	return fResult;
}
