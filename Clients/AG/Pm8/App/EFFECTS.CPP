/*
// $Workfile: EFFECTS.CPP $
// $Revision: 1 $
// $Date: 3/03/99 6:05p $
// 
//  "This unpublished source code contains trade secrets which are the
//   property of Mindscape, Inc.  Unauthorized use, copying or distribution
//   is a violation of international laws and is strictly prohibited."
// 
//        Copyright © 1997 Mindscape, Inc. All rights reserved.
*/

/*
// Revision History:
//
// $Log: /PM8/App/EFFECTS.CPP $
// 
// 1     3/03/99 6:05p Gbeddow
// 
// 5     5/12/98 11:34a Rlovejoy
// Added radial fills.
// 
// 4     4/30/98 6:44p Jayn
// Further fix for the FillFormat changes.
// 
// 3     4/30/98 4:47p Jayn
// Replaced FillFormat with FillFormatV1 (old) and FillFormatV2 (new).
// The new one has the gradient members.
// 
// 2     4/29/98 5:58p Rlovejoy
// New fill format.
// 
//    Rev 1.0   14 Aug 1997 15:20:28   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:38:16   Fred
// Initial revision.
// 
//    Rev 1.3   20 Jul 1997 13:58:02   Jay
// Sets line width in hairline case (makes me feel better).
// 
//    Rev 1.2   29 May 1997 08:43:52   Fred
// New effects support
// 
//    Rev 1.1   28 May 1997 11:05:26   Fred
// Added SetColor methods.
// 
//    Rev 1.0   23 May 1997 17:05:38   Fred
// New effects
*/

#include "stdafx.h"
#include "effects.h"

/////////////////////////////////////////////////////////////////////////////
// FillFormatV1

void FillFormatV1::FromOld(int nOldStyle, COLOR OldColor)
{
	if (nOldStyle < 0)
	{
		SetType(FillNone);
	}
	else
	{
		SetColor(nOldStyle, OldColor);
	}
}

void FillFormatV1::SetType(int nType, COLOR Color /*=TRANSPARENT_COLOR*/)
{
	switch (nType)
	{
		case FillNone:
		{
			SetColor(None, TRANSPARENT_COLOR);
			break;
		}
		case FillSolid:
		{
			SetColor(Black, Color);
			break;
		}
		default:
		{
			ASSERT(FALSE);
		}
	}
}

void FillFormatV1::SetColor(SHORT nPattern, COLOR Color)
{
	m_nPattern = nPattern;
	m_ForegroundColor = Color;
}

/////////////////////////////////////////////////////////////////////////////
// FillFormatV2

void FillFormatV2::FromOld(int nOldStyle, COLOR OldColor)
{
	if (nOldStyle < 0)
	{
		SetType(FillNone);
	}
	else
	{
		SetColor(nOldStyle, OldColor);
	}
}

void FillFormatV2::SetType(int nType, COLOR Color /*=TRANSPARENT_COLOR*/, COLOR Blend /*=UNDEFINED_COLOR*/)
{
	switch (nType)
	{
		case FillNone:
		{
			SetColor(None, TRANSPARENT_COLOR);
			break;
		}
		case FillSolid:
		{
			SetColor(Black, Color);
			break;
		}
		case FillSweepRight:
		case FillSweepDown:
		{
			SetColor(Black, Color, Blend);
		}
		default:
		{
			ASSERT(FALSE);
		}
	}
	m_nFillType = nType;
}

void FillFormatV2::SetColor(SHORT nPattern, COLOR Color, COLOR Blend /*=UNDEFINED_COLOR*/)
{
	m_nPattern = nPattern;
	m_ForegroundColor = Color;
	m_GradientBlendColor = Blend;
}

const FillFormatV2& FillFormatV2::operator=(const FillFormatV1& ff)
{
	m_nPattern = ff.m_nPattern;
	m_ForegroundColor = ff.m_ForegroundColor;
	m_nFillType = (m_nPattern == None ? FillNone : FillSolid);
	return *this;
}

/////////////////////////////////////////////////////////////////////////////
// OutlineFormat

void OutlineFormat::FromOld(int nOldStyle, COLOR OldColor)
{
	switch (nOldStyle)
	{
		case 0:		// None
		{
			SetType(OutlineNone);
			break;
		}
		case 1:		// Light
		{
			SetType(OutlineHairline);
			break;
		}
		case 2:		// Medium
		{
			SetType(OutlineMedium);
			break;
		}
		case 3:		// Heavy
		{
			SetType(OutlineThick);
			break;
		}
		case 4:		// Calligraphy
		{
			SetType(OutlineCalligraphy);
			break;
		}
		case 5:		// Contour1
		{
			SetType(OutlineContour1);
			break;
		}
		case 6:		// Contour2
		{
			SetType(OutlineContour2);
			break;
		}
		default:
		{
			ASSERT(FALSE);
			SetType(OutlineNone);
		}
	}

	m_nPattern = FillFormatV1::Black;
	m_ForegroundColor = OldColor;
}

void OutlineFormat::SetType(int nType, CFixed lWidth /*=0*/)
{
	switch (nType)
	{
		case OutlineNone:
		{
			m_nStyle = None;
			break;
		}
		case OutlineHairline:
		{
			m_nStyle = Light;
			m_lWidth = lWidth;            // This is ignored, but what the heck...
			break;
		}
		case OutlineThin:
		{
			m_nStyle = Inflate;
			m_nWidthType = Relative;
			m_lWidth = MakeFixed(TEXT_OUTLINE_THIN_FRACTION);
			break;
		}
		case OutlineMedium:
		{
			m_nStyle = Inflate;
			m_nWidthType = Relative;
			m_lWidth = MakeFixed(TEXT_OUTLINE_MEDIUM_FRACTION);
			break;
		}
		case OutlineThick:
		{
			m_nStyle = Inflate;
			m_nWidthType = Relative;
			m_lWidth = MakeFixed(TEXT_OUTLINE_THICK_FRACTION);
			break;
		}
		case OutlineAbsolute:
		{
			m_nStyle = Inflate;
			m_nWidthType = Absolute;
			m_lWidth = lWidth;
			break;
		}
		case OutlineRelative:
		{
			m_nStyle = Inflate;
			m_nWidthType = Relative;
			m_lWidth = lWidth;
			break;
		}
		case OutlineCalligraphy:
		{
			m_nStyle = Calligraphy;
			m_nWidthType = Relative;
			m_lWidth = MakeFixed(0);
			break;
		}
		case OutlineContour1:
		{
			m_nStyle = Contour1;
			m_nWidthType = Relative;
			m_lWidth = MakeFixed(0);
			break;
		}
		case OutlineContour2:
		{
			m_nStyle = Contour2;
			m_nWidthType = Relative;
			m_lWidth = MakeFixed(0);
			break;
		}
		default:
		{
			ASSERT(FALSE);
		}
	}
}

void OutlineFormat::SetColor(SHORT nPattern, COLOR Color)
{
	m_nPattern = nPattern;
	m_ForegroundColor = Color;
}


/////////////////////////////////////////////////////////////////////////////
// ShadowFormat

void ShadowFormat::FromOld(int nOldStyle, COLOR OldColor)
{
	if (nOldStyle == 0)		// None
	{
		SetType(ShadowNone);
	}
	else if ((nOldStyle >= 1) && (nOldStyle <= 4))		// Old drop shadows
	{
		SetType(nOldStyle-1+ShadowDropDownRight, MakeFixed(TEXT_SHADOW_FRACTION));

		COLOR_COMPONENT cyan = CYAN_COMPONENT_OF(OldColor);
		COLOR_COMPONENT magenta = MAGENTA_COMPONENT_OF(OldColor);
		COLOR_COMPONENT yellow = YELLOW_COMPONENT_OF(OldColor);
		COLOR_COMPONENT black = BLACK_COMPONENT_OF(OldColor);

		if (black == 0)
		{
			cyan /= 2;
			magenta /= 2;
			yellow /= 2;
		}
		else
		{
			black /= 2;
		}

		OldColor = MAKE_COLOR(cyan, magenta, yellow, black);
	}
	else if ((nOldStyle >= 5) && (nOldStyle <= 8))		// Old 3D shadows
	{
		SetType(nOldStyle-5+ShadowExtrudedDownRight, MakeFixed(TEXT_SHADOW_FRACTION));
	}

	m_nPattern = FillFormatV1::Black;
	m_ForegroundColor = OldColor;
}

void ShadowFormat::SetType(int nType, CFixed lFraction /*=0*/)
{
	ASSERT((nType == ShadowNone) || (lFraction != MakeFixed(0)));

	switch (nType)
	{
		case ShadowNone:
		{
			m_nStyle = None;
			break;
		}
		case ShadowDropDownRight:
		case ShadowDropDownLeft:
		case ShadowDropUpRight:
		case ShadowDropUpLeft:
		{
			m_nStyle = Drop;
			m_nOffsetType = Relative;
			m_lXOffset = lFraction*((((nType-ShadowDropDownRight) & 1) == 0) ?  1 : -1);
			m_lYOffset = lFraction*((((nType-ShadowDropDownRight) & 2) == 0) ? -1 :  1);
			break;
		}
		case ShadowExtrudedDownRight:
		case ShadowExtrudedDownLeft:
		case ShadowExtrudedUpRight:
		case ShadowExtrudedUpLeft:
		{
			m_nStyle = Extruded;
			m_nOffsetType = Relative;
			m_lXOffset = lFraction*((((nType-ShadowExtrudedDownRight) & 1) == 0) ?  1 : -1);
			m_lYOffset = lFraction*((((nType-ShadowExtrudedDownRight) & 2) == 0) ? -1 :  1);
			break;
		}
		default:
		{
			ASSERT(FALSE);
		}
	}
}

void ShadowFormat::SetColor(SHORT nPattern, COLOR Color)
{
	m_nPattern = nPattern;
	m_ForegroundColor = Color;
}
