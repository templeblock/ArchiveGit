/*
// $Header: /PM8/App/STYLEHLP.CPP 1     3/03/99 6:12p Gbeddow $
//
// Routines for style helpers.
//
// $Log: /PM8/App/STYLEHLP.CPP $
// 
// 1     3/03/99 6:12p Gbeddow
// 
// 9     2/11/99 11:55a Cschende
// Added code for Drop Caps implementation
// 
// 8     12/15/98 5:46p Cschende
// added bullet colro support, also fixed teh source of a nasty bullet bug
// when settung the bullet level undefined to -1 which is 255 in a byte
// and then having the teststyle fail comparing -1 to 255
// 
// 7     12/10/98 4:11p Cschende
// Added bullet, numbered list and multilevel list support in text
// 
// 6     6/03/98 1:40p Jayn
// Removed some ASSERTs in MakeUndefined().
// 
// 5     4/30/98 4:48p Jayn
// Replaced FillFormat with FillFormatV1 (old) and FillFormatV2 (new).
// The new one has the gradient members.
// 
// 4     3/27/98 5:47p Johno
// Use -1 for CHyperlinkHelper::TestStyle
// 
// 3     3/26/98 11:55a Johno
// CHyperlinkHelper::MakeUndefined now uses -1
// 
// 2     3/24/98 5:28p Johno
// Added class CHyperlinkHelper
// 
//    Rev 1.0   14 Aug 1997 15:26:08   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:40:38   Fred
// Initial revision.
// 
//    Rev 1.14   21 Jul 1997 10:51:54   Jay
// Point size changes to 0 are now mapped to 1.
// 
//    Rev 1.13   07 Jul 1997 10:45:18   Jay
// Reduced old text style changes since they are noew removed.
// 
//    Rev 1.12   26 Jun 1997 16:14:56   Jay
// Fixed old text style changes
// 
//    Rev 1.11   29 May 1997 08:51:38   Fred
// Properly handles old effects styles
// 
//    Rev 1.10   15 May 1997 16:49:04   Fred
// New text effects
// 
//    Rev 1.9   06 Jan 1997 17:07:16   Jay
// Character spacing.
// 
//    Rev 1.8   31 Jul 1996 11:58:28   Fred
// 16-bit compatibility
// 
//    Rev 1.7   05 Jul 1996 10:47:10   Jay
//  
// 
//    Rev 1.6   27 Jun 1996 17:20:38   Jay
//  
// 
//    Rev 1.5   26 Jun 1996 09:06:52   Jay
//  
// 
//    Rev 1.4   18 Jun 1996 16:14:50   Jay
//  
// 
//    Rev 1.3   17 Jun 1996 12:01:32   Jay
//  
// 
//    Rev 1.2   14 Jun 1996 17:05:38   Jay
//  
// 
//    Rev 1.1   13 Jun 1996 15:53:04   Jay
//  
// 
//    Rev 1.0   14 Mar 1996 13:46:22   Jay
// Initial revision.
// 
//    Rev 1.0   16 Feb 1996 12:08:32   FRED
// Initial revision.
// 
//    Rev 2.0   07 Feb 1995 15:48:10   JAY
// Initial revision.
// 
//    Rev 1.2   30 Nov 1994 16:33:18   JAY
// stdafx.h; merged database code from album
// 
//    Rev 1.1   05 Jan 1994 08:06:10   JAY
// Restructuring, etc.
//
//   Rev 1.0   16 Dec 1993 16:25:08   JAY
// 
*/

#include "stdafx.h"
#include "pmgdb.h"
#include "pagedefs.h"
#include "style.h"
#include "stylehlp.h"
#include "flexmath.h"
#include "stylerec.h"

/*
// Multiply by a fraction.
*/

PRIVATE VOID
frac_scale(USHORT *whole, USHORT *fraction, STYLE_FRAC scale)
{
	FLEX flex;

	flex = UnsplitFlex(*whole, *fraction);

	flex = ScaleFlex(flex, scale.num, scale.denom);

	SplitFlex(flex, whole, fraction);
}

void StyleSizeHelper::apply_change(TextStylePtr stylep, STYLE_CHANGE_PTR scp)
{
	USHORT whole, fraction;

	whole = (USHORT)stylep->get_size();
	fraction = (USHORT)stylep->get_size_fraction();
	frac_scale(&whole, &fraction, *(STYLE_FRAC far *)&scp->new_value);
	stylep->set_size((SHORT)whole);
	stylep->set_size_fraction((SHORT)fraction);

	whole = (USHORT)stylep->get_base_size();
	fraction = (USHORT)stylep->get_base_size_fraction();
	frac_scale(&whole, &fraction, *(STYLE_FRAC far *)&scp->new_value);
	stylep->set_base_size((SHORT)whole);
	stylep->set_base_size_fraction((SHORT)fraction);
}

VOID StyleSizeHelper::build_change(STYLE_CHANGE_PTR scp, TextStylePtr new_stylep, TextStylePtr old_stylep)
{
	STYLE_FRAC	frac;

	frac.num = new_stylep->get_base_size();
	frac.denom = old_stylep->get_base_size();

	scp->new_value = *(LONG *)&frac;
}

/////////////////////////////////////////////////////////////////////////////
// Generic style helpers.

/* Invalid style change helper. */

class CStyleInvalidHelper : public CStyleHelper
{
	void ApplyChange(CTextStyle* pStyle, const CTextStyleChange* pStyleChange)
		{ ASSERT(FALSE); }
	void BuildChange(CTextStyleChange* pStyleChange, const CTextStyle* pNewStyle, const CTextStyle* pOldStyle)
		{ ASSERT(FALSE); }
	BOOL CompareStyles(const CTextStyle* pStyle1, const CTextStyle* pStyle2)
		{ return TRUE; }// Always return TRUE. Do not add to stream.
	BOOL TestStyle(const CTextStyle* pStyle)
		{ return FALSE; }
	void MakeUndefined(CTextStyle* pStyle)
		{ }
	void ApplyOldChange(CTextStyle* pStyle, const STYLE_CHANGE* pStyleChange)
		{ ASSERT(FALSE); }
};

/////////////////////////////////////////////////////////////////////////////
// Text (character) style helpers.

/* Font/Face. */

class CStyleFontHelper : public CStyleHelper
{
	void ApplyChange(CTextStyle* pStyle, const CTextStyleChange* pStyleChange)
		{ pStyle->Font(pStyleChange->m_lValue); }
	void BuildChange(CTextStyleChange* pStyleChange, const CTextStyle* pNewStyle, const CTextStyle* pOldStyle)
		{ pStyleChange->m_lValue = pNewStyle->Font(); pStyleChange->m_lOldValue = pOldStyle->Font(); }
	BOOL CompareStyles(const CTextStyle* pStyle1, const CTextStyle* pStyle2)
		{ return pStyle1->Font() == pStyle2->Font(); }
	BOOL TestStyle(const CTextStyle* pStyle)
		{ return pStyle->Font() != 0; }
	void MakeUndefined(CTextStyle* pStyle)
		{ pStyle->Font(0); }
	void ApplyOldChange(CTextStyle* pStyle, const STYLE_CHANGE* pStyleChange)
		{ pStyle->Font(pStyleChange->new_value); }
};

/* Style. */

class CStyleStyleHelper : public CStyleHelper
{
	void ApplyChange(CTextStyle* pStyle, const CTextStyleChange* pStyleChange)
		{ ASSERT(FALSE); }
	void BuildChange(CTextStyleChange* pStyleChange, const CTextStyle* pNewStyle, const CTextStyle* pOldStyle)
		{ ASSERT(FALSE); }
	BOOL CompareStyles(const CTextStyle* pStyle1, const CTextStyle* pStyle2)
		{ return TRUE; }// Always return TRUE. Do not add to stream.
	BOOL TestStyle(const CTextStyle* pStyle)
		{ return FALSE; }
	void MakeUndefined(CTextStyle* pStyle)
		{ }
	void ApplyOldChange(CTextStyle* pStyle, const STYLE_CHANGE* pStyleChange);
};

void CStyleStyleHelper::ApplyOldChange(CTextStyle* pStyle, const STYLE_CHANGE* pStyleChange)
{
	pStyle->Bold(!!(pStyleChange->new_value & FONT_STYLE_Bold));
	pStyle->Italic(!!(pStyleChange->new_value & FONT_STYLE_Italic));
}

/* Size. */

class CStyleSizeHelper : public CStyleHelper
{
	void ApplyChange(CTextStyle* pStyle, const CTextStyleChange* pStyleChange);
	void BuildChange(CTextStyleChange* pStyleChange, const CTextStyle* pNewStyle, const CTextStyle* pOldStyle);
	BOOL CompareStyles(const CTextStyle* pStyle1, const CTextStyle* pStyle2)
		{ return pStyle1->BaseSize() == pStyle2->BaseSize(); }
	BOOL TestStyle(const CTextStyle* pStyle)
		{ return pStyle->BaseSize() != UNDEFINED_CFIXED; }
	void MakeUndefined(CTextStyle* pStyle)
		{ pStyle->BaseSize(UNDEFINED_CFIXED); }
	void ApplyOldChange(CTextStyle* pStyle, const STYLE_CHANGE* pStyleChange);
};

void CStyleSizeHelper::ApplyChange(CTextStyle* pStyle, const CTextStyleChange* pStyleChange)
{
   if (pStyleChange->m_lValue == 0)
      ((CTextStyleChange*)pStyleChange)->m_lValue = MakeFixed(1);
   if (pStyleChange->m_lOldValue == 0)
      ((CTextStyleChange*)pStyleChange)->m_lOldValue = MakeFixed(1);
	pStyle->Size(MulDivFixed(pStyle->Size(), pStyleChange->m_lValue, pStyleChange->m_lOldValue));
	pStyle->BaseSize(MulDivFixed(pStyle->BaseSize(), pStyleChange->m_lValue, pStyleChange->m_lOldValue));
}

void CStyleSizeHelper::BuildChange(CTextStyleChange* pStyleChange, const CTextStyle* pNewStyle, const CTextStyle* pOldStyle)
{
	pStyleChange->m_lValue = pNewStyle->BaseSize();
	pStyleChange->m_lOldValue = pOldStyle->BaseSize();
}

void CStyleSizeHelper::ApplyOldChange(CTextStyle* pStyle, const STYLE_CHANGE* pStyleChange)
{
	STYLE_FRAC Frac = *(STYLE_FRAC far *)&pStyleChange->new_value;

	CFixed Size = pStyle->Size();
	Size = MulDivFixed(Size, MakeFixed((int)Frac.num), MakeFixed((int)Frac.denom));
	pStyle->Size(Size);

	Size = pStyle->BaseSize();
	Size = MulDivFixed(Size, MakeFixed((int)Frac.num), MakeFixed((int)Frac.denom));
	pStyle->BaseSize(Size);
}

#define Effect_Style_Helper(Effect, Field, Type, Var, UndefinedValue) \
	class CStyle##Effect##Field##Helper : public CStyleHelper \
	{ \
		void ApplyChange(CTextStyle* pStyle, const CTextStyleChange* pStyleChange) \
			{ pStyle->Effect##Field((Type)pStyleChange->m_lValue); } \
		void BuildChange(CTextStyleChange* pStyleChange, const CTextStyle* pNewStyle, const CTextStyle* pOldStyle) \
			{ pStyleChange->m_lValue = (long)pNewStyle->Effect##Field(); pStyleChange->m_lOldValue = (long)pOldStyle->Effect##Field();} \
		BOOL CompareStyles(const CTextStyle* pStyle1, const CTextStyle* pStyle2) \
			{ return pStyle1->Effect##Field() == pStyle2->Effect##Field(); } \
		BOOL TestStyle(const CTextStyle* pStyle) \
			{ return pStyle->Effect##Field() != UndefinedValue; } \
		void MakeUndefined(CTextStyle* pStyle) \
			{ pStyle->Effect##Field(UndefinedValue); } \
		void ApplyOldChange(CTextStyle* pStyle, const STYLE_CHANGE* pStyleChange) \
			{ pStyle->Effect##Field((int)pStyleChange->new_value); } \
	}; \
	static CStyle##Effect##Field##Helper Effect##Field##Helper;

	// Fill

	Effect_Style_Helper(Fill, Pattern, int, nPattern, FillFormatV1::Undefined)
	Effect_Style_Helper(Fill, ForegroundColor, COLOR, ForegroundColor, UNDEFINED_COLOR)

	// Outline.

	Effect_Style_Helper(Outline, Style, SHORT, nStyle, OutlineFormat::Undefined)
	Effect_Style_Helper(Outline, WidthType, SHORT, nWidthType, -1)
	Effect_Style_Helper(Outline, Width, CFixed, lWidth, 0x80000000L)
	Effect_Style_Helper(Outline, Pattern, SHORT, nPattern, FillFormatV1::Undefined)
	Effect_Style_Helper(Outline, ForegroundColor, COLOR, ForegroundColor, UNDEFINED_COLOR)

	// Shadow

	Effect_Style_Helper(Shadow, Style, SHORT, nStyle, ShadowFormat::Undefined)
	Effect_Style_Helper(Shadow, OffsetType, SHORT, nOffsetType, -1)
	Effect_Style_Helper(Shadow, XOffset, CFixed, lXOffset, 0x80000000L)
	Effect_Style_Helper(Shadow, YOffset, CFixed, lYOffset, 0x80000000L)
	Effect_Style_Helper(Shadow, Pattern, SHORT, nPattern, FillFormatV1::Undefined)
	Effect_Style_Helper(Shadow, ForegroundColor, COLOR, ForegroundColor, UNDEFINED_COLOR)

#undef Effect_Style_Helper

// Old effects helpers.

/* Pattern. */

class CStylePatternHelper : public CStyleHelper
{
	void ApplyChange(CTextStyle* pStyle, const CTextStyleChange* pStyleChange)
		{ pStyle->OldFill((int)pStyleChange->m_lValue); }
	void BuildChange(CTextStyleChange* pStyleChange, const CTextStyle* pNewStyle, const CTextStyle* pOldStyle)
		{ ASSERT(FALSE);}
	BOOL CompareStyles(const CTextStyle* pStyle1, const CTextStyle* pStyle2)
		{ return TRUE; }			// Always return TRUE. Do not add to stream.
	BOOL TestStyle(const CTextStyle* pStyle)
		{ return FALSE; }
	void MakeUndefined(CTextStyle* pStyle)
      { }
	void ApplyOldChange(CTextStyle* pStyle, const STYLE_CHANGE* pStyleChange)
		{ pStyle->OldFill((int)pStyleChange->new_value); }
};

/* Outline */

class CStyleOutlineHelper : public CStyleHelper
{
	void ApplyChange(CTextStyle* pStyle, const CTextStyleChange* pStyleChange)
		{ pStyle->OldOutline((int)pStyleChange->m_lValue); }
	void BuildChange(CTextStyleChange* pStyleChange, const CTextStyle* pNewStyle, const CTextStyle* pOldStyle)
		{ ASSERT(FALSE);}
	BOOL CompareStyles(const CTextStyle* pStyle1, const CTextStyle* pStyle2)
		{ return TRUE; }			// Always return TRUE. Do not add to stream.
	BOOL TestStyle(const CTextStyle* pStyle)
		{ return FALSE; }
	void MakeUndefined(CTextStyle* pStyle)
      { }
	void ApplyOldChange(CTextStyle* pStyle, const STYLE_CHANGE* pStyleChange)
		{ pStyle->OldOutline((int)pStyleChange->new_value); }
};

/* Shadow. */

class CStyleShadowHelper : public CStyleHelper
{
	void ApplyChange(CTextStyle* pStyle, const CTextStyleChange* pStyleChange)
		{ pStyle->OldShadow((int)pStyleChange->m_lValue); }
	void BuildChange(CTextStyleChange* pStyleChange, const CTextStyle* pNewStyle, const CTextStyle* pOldStyle)
		{ ASSERT(FALSE);}
	BOOL CompareStyles(const CTextStyle* pStyle1, const CTextStyle* pStyle2)
		{ return TRUE; }			// Always return TRUE. Do not add to stream.
	BOOL TestStyle(const CTextStyle* pStyle)
		{ return FALSE; }
	void MakeUndefined(CTextStyle* pStyle)
      { }
	void ApplyOldChange(CTextStyle* pStyle, const STYLE_CHANGE* pStyleChange)
		{ pStyle->OldShadow((int)pStyleChange->new_value); }
};

/* Color. */

class CStyleColorHelper : public CStyleHelper
{
	void ApplyChange(CTextStyle* pStyle, const CTextStyleChange* pStyleChange)
		{ pStyle->OldColor((COLOR)pStyleChange->m_lValue); }
	void BuildChange(CTextStyleChange* pStyleChange, const CTextStyle* pNewStyle, const CTextStyle* pOldStyle)
		{ ASSERT(FALSE);}
	BOOL CompareStyles(const CTextStyle* pStyle1, const CTextStyle* pStyle2)
		{ return TRUE; }			// Always return TRUE. Do not add to stream.
	BOOL TestStyle(const CTextStyle* pStyle)
		{ return FALSE; }
	void MakeUndefined(CTextStyle* pStyle)
		{ }
	void ApplyOldChange(CTextStyle* pStyle, const STYLE_CHANGE* pStyleChange)
		{ pStyle->OldColor(pStyleChange->new_value); }
};

/* Font bold style. */

class CStyleBoldHelper : public CStyleHelper
{
	void ApplyChange(CTextStyle* pStyle, const CTextStyleChange* pStyleChange)
		{ pStyle->Bold((int)pStyleChange->m_lValue); }
	void BuildChange(CTextStyleChange* pStyleChange, const CTextStyle* pNewStyle, const CTextStyle* pOldStyle)
		{ pStyleChange->m_lValue = (long)pNewStyle->Bold(); pStyleChange->m_lOldValue = (long)pOldStyle->Bold();}
	BOOL CompareStyles(const CTextStyle* pStyle1, const CTextStyle* pStyle2)
		{ return pStyle1->Bold() == pStyle2->Bold(); }
	BOOL TestStyle(const CTextStyle* pStyle)
		{ return pStyle->Bold() != -1; }
	void MakeUndefined(CTextStyle* pStyle)
		{ pStyle->Bold(-1); }
	void ApplyOldChange(CTextStyle* pStyle, const STYLE_CHANGE* pStyleChange)
		{ pStyle->Bold((int)pStyleChange->new_value); }
};

/* Font italic style. */

class CStyleItalicHelper : public CStyleHelper
{
	void ApplyChange(CTextStyle* pStyle, const CTextStyleChange* pStyleChange)
		{ pStyle->Italic((int)pStyleChange->m_lValue); }
	void BuildChange(CTextStyleChange* pStyleChange, const CTextStyle* pNewStyle, const CTextStyle* pOldStyle)
		{ pStyleChange->m_lValue = (long)pNewStyle->Italic(); pStyleChange->m_lOldValue = (long)pOldStyle->Italic();}
	BOOL CompareStyles(const CTextStyle* pStyle1, const CTextStyle* pStyle2)
		{ return pStyle1->Italic() == pStyle2->Italic(); }
	BOOL TestStyle(const CTextStyle* pStyle)
		{ return pStyle->Italic() != -1; }
	void MakeUndefined(CTextStyle* pStyle)
		{ pStyle->Italic(-1); }
	void ApplyOldChange(CTextStyle* pStyle, const STYLE_CHANGE* pStyleChange)
		{ pStyle->Italic((int)pStyleChange->new_value); }
};

/* Font underline style. */

class CStyleUnderlineHelper : public CStyleHelper
{
	void ApplyChange(CTextStyle* pStyle, const CTextStyleChange* pStyleChange)
		{ pStyle->Underline((UNDERLINE_STYLE)pStyleChange->m_lValue); }
	void BuildChange(CTextStyleChange* pStyleChange, const CTextStyle* pNewStyle, const CTextStyle* pOldStyle)
		{ pStyleChange->m_lValue = (long)pNewStyle->Underline(); pStyleChange->m_lOldValue = (long)pOldStyle->Underline();}
	BOOL CompareStyles(const CTextStyle* pStyle1, const CTextStyle* pStyle2)
		{ return pStyle1->Underline() == pStyle2->Underline(); }
	BOOL TestStyle(const CTextStyle* pStyle)
		{ return pStyle->Underline() != UNDERLINE_STYLE_Undefined; }
	void MakeUndefined(CTextStyle* pStyle)
		{ pStyle->Underline(UNDERLINE_STYLE_Undefined); }
	void ApplyOldChange(CTextStyle* pStyle, const STYLE_CHANGE* pStyleChange)
		{ pStyle->Underline((UNDERLINE_STYLE)pStyleChange->new_value); }
};

/* Paragraph style change helper. */

class CStyleParagraphHelper : public CStyleHelper
{
	void ApplyChange(CTextStyle* pStyle, const CTextStyleChange* pStyleChange);
	void BuildChange(CTextStyleChange* pStyleChange, const CTextStyle* pNewStyle, const CTextStyle* pOldStyle)
		{}
	BOOL CompareStyles(const CTextStyle* pStyle1, const CTextStyle* pStyle2)
		{ return TRUE; }// Always return TRUE. Do not add to stream.
	BOOL TestStyle(const CTextStyle* pStyle)
		{ return FALSE; }
	void MakeUndefined(CTextStyle* pStyle)
		{}
	void ApplyOldChange(CTextStyle* pStyle, const STYLE_CHANGE* pStyleChange)
		{ ASSERT(FALSE); }
};

void CStyleParagraphHelper::ApplyChange(CTextStyle* pStyle, const CTextStyleChange* pStyleChange)
{
	DB_RECORD_NUMBER lRecord = pStyle->ParagraphStyle();
	ASSERT(lRecord != 0);
	CTextStyleRecord* pRecord = (CTextStyleRecord*)pStyle->Database()->get_record(lRecord, NULL, RECORD_TYPE_TextStyle);
	if (pRecord != NULL)
	{
		ASSERT(pRecord->Type() == CTextStyleRecord::TEXT_STYLE_Paragraph);
		*pStyle = pRecord->Style();
		pRecord->release();
	}
}

/* Spacing. */

class CStyleSpacingHelper : public CStyleHelper
{
	void ApplyChange(CTextStyle* pStyle, const CTextStyleChange* pStyleChange)
		{ pStyle->Spacing((CFixed)pStyleChange->m_lValue); }
	void BuildChange(CTextStyleChange* pStyleChange, const CTextStyle* pNewStyle, const CTextStyle* pOldStyle)
		{ pStyleChange->m_lValue = pNewStyle->Spacing(); pStyleChange->m_lOldValue = pOldStyle->Spacing();}
	BOOL CompareStyles(const CTextStyle* pStyle1, const CTextStyle* pStyle2)
		{ return pStyle1->Spacing() == pStyle2->Spacing(); }
	BOOL TestStyle(const CTextStyle* pStyle)
		{ return pStyle->Spacing() != UNDEFINED_CFIXED; }
	void MakeUndefined(CTextStyle* pStyle)
		{ pStyle->Spacing(UNDEFINED_CFIXED); }
	void ApplyOldChange(CTextStyle* pStyle, const STYLE_CHANGE* pStyleChange)
		{ pStyle->Spacing((CFixed)pStyleChange->new_value); }
};

/* White Out... you know, that little white line around characters. */

class CStyleWhiteOutHelper : public CStyleHelper
{
	void ApplyChange(CTextStyle* pStyle, const CTextStyleChange* pStyleChange)
		{ pStyle->WhiteOut((SHORT)pStyleChange->m_lValue); }
	void BuildChange(CTextStyleChange* pStyleChange, const CTextStyle* pNewStyle, const CTextStyle* pOldStyle)
		{ pStyleChange->m_lValue = pNewStyle->WhiteOut(); pStyleChange->m_lOldValue = pOldStyle->WhiteOut();}
	BOOL CompareStyles(const CTextStyle* pStyle1, const CTextStyle* pStyle2)
		{ return pStyle1->WhiteOut() == pStyle2->WhiteOut(); }
	BOOL TestStyle(const CTextStyle* pStyle)
		{ return pStyle->WhiteOut() != -1; }
	void MakeUndefined(CTextStyle* pStyle)
		{ pStyle->WhiteOut(-1); }
	void ApplyOldChange(CTextStyle* pStyle, const STYLE_CHANGE* pStyleChange)
		{ pStyle->WhiteOut((SHORT)pStyleChange->new_value); }
};

class CHyperlinkHelper : public CStyleHelper // Just add meat!
{
   void ApplyChange(class CTextStyle * pStyle, const struct CTextStyleChange *pStyleChange)
   {
      pStyle->HyperlinkStyle((DB_RECORD_NUMBER)pStyleChange->m_lValue);
   }
   
   void BuildChange(struct CTextStyleChange * pStyleChange, const class CTextStyle * pNewStyle, const class CTextStyle * pOldStyle)
   {
      pStyleChange->m_lValue = pNewStyle->HyperlinkStyle(); pStyleChange->m_lOldValue = pOldStyle->HyperlinkStyle();
   }

   BOOL CompareStyles(const class CTextStyle * pStyle1, const class CTextStyle * pStyle2)
   {
      return pStyle1->HyperlinkStyle() == pStyle2->HyperlinkStyle();
   }

   BOOL TestStyle(const class CTextStyle *pStyle)
   {
      return pStyle->HyperlinkStyle() != -1;
   }

   void MakeUndefined(class CTextStyle *pStyle)
   {
      pStyle->HyperlinkStyle(-1);
   }

   void ApplyOldChange(class CTextStyle * pStyle, const STYLE_CHANGE *pStyleChange)
   {
      pStyle->HyperlinkStyle((DB_RECORD_NUMBER)pStyleChange->new_value);
   }
};

static CStyleFontHelper FontHelper;
static CStyleStyleHelper StyleHelper;
static CStyleInvalidHelper InvalidHelper;
static CStyleSizeHelper SizeHelper;
static CStylePatternHelper PatternHelper;
static CStyleOutlineHelper OutlineHelper;
static CStyleShadowHelper ShadowHelper;
static CStyleColorHelper ColorHelper;
static CStyleBoldHelper BoldHelper;
static CStyleItalicHelper ItalicHelper;
static CStyleUnderlineHelper UnderlineHelper;
static CStyleParagraphHelper ParagraphHelper;
static CStyleSpacingHelper SpacingHelper;
static CStyleWhiteOutHelper WhiteOutHelper;
static CHyperlinkHelper HyperlinkHelper;

/////////////////////////////////////////////////////////////////////////////
// Paragraph style helpers.

// Base class for unused functions.
class CParagraphStyleHelper : public CStyleHelper
{
	void ApplyOldChange(CTextStyle* pStyle, const STYLE_CHANGE* pStyleChange)
		{ ASSERT(FALSE); }
};

/* Alignment. */

class CStyleAlignmentHelper : public CParagraphStyleHelper
{
	void ApplyChange(CTextStyle* pStyle, const CTextStyleChange* pStyleChange)
		{ pStyle->Alignment((int)pStyleChange->m_lValue); }
	void BuildChange(CTextStyleChange* pStyleChange, const CTextStyle* pNewStyle, const CTextStyle* pOldStyle)
		{ pStyleChange->m_lValue = (long)pNewStyle->Alignment(); pStyleChange->m_lOldValue = (long)pOldStyle->Alignment();}
	BOOL CompareStyles(const CTextStyle* pStyle1, const CTextStyle* pStyle2)
		{ return pStyle1->Alignment() == pStyle2->Alignment(); }
	BOOL TestStyle(const CTextStyle* pStyle)
		{ return pStyle->Alignment() != -1; }
	void MakeUndefined(CTextStyle* pStyle)
		{ pStyle->Alignment(-1); }
};

class CStyleLeadingTypeHelper : public CParagraphStyleHelper
{
	void ApplyChange(CTextStyle* pStyle, const CTextStyleChange* pStyleChange)
		{ pStyle->LeadingType((short int)pStyleChange->m_lValue); }
	void BuildChange(CTextStyleChange* pStyleChange, const CTextStyle* pNewStyle, const CTextStyle* pOldStyle)
		{ pStyleChange->m_lValue = (long)pNewStyle->LeadingType(); pStyleChange->m_lOldValue = (long)pOldStyle->LeadingType();}
	BOOL CompareStyles(const CTextStyle* pStyle1, const CTextStyle* pStyle2)
		{ return pStyle1->LeadingType() == pStyle2->LeadingType(); }
	BOOL TestStyle(const CTextStyle* pStyle)
		{ return pStyle->LeadingType() != LEADING_undefined; }
	void MakeUndefined(CTextStyle* pStyle)
		{ pStyle->LeadingType(LEADING_undefined); }
};

class CStyleLeadingHelper : public CParagraphStyleHelper
{
	void ApplyChange(CTextStyle* pStyle, const CTextStyleChange* pStyleChange)
		{ pStyle->Leading(pStyleChange->m_lValue); }
	void BuildChange(CTextStyleChange* pStyleChange, const CTextStyle* pNewStyle, const CTextStyle* pOldStyle)
		{ pStyleChange->m_lValue = pNewStyle->Leading(); pStyleChange->m_lOldValue = pOldStyle->Leading();}
	BOOL CompareStyles(const CTextStyle* pStyle1, const CTextStyle* pStyle2)
		{ return pStyle1->Leading() == pStyle2->Leading(); }
	BOOL TestStyle(const CTextStyle* pStyle)
		{ return pStyle->Leading() != UNDEFINED_CFIXED; }
	void MakeUndefined(CTextStyle* pStyle)
		{ pStyle->Leading(UNDEFINED_CFIXED); }
};

class CStyleFirstLineIndentHelper : public CParagraphStyleHelper
{
	void ApplyChange(CTextStyle* pStyle, const CTextStyleChange* pStyleChange)
		{ pStyle->FirstLineIndent(pStyleChange->m_lValue); }
	void BuildChange(CTextStyleChange* pStyleChange, const CTextStyle* pNewStyle, const CTextStyle* pOldStyle)
		{ pStyleChange->m_lValue = pNewStyle->FirstLineIndent(); pStyleChange->m_lOldValue = pOldStyle->FirstLineIndent();}
	BOOL CompareStyles(const CTextStyle* pStyle1, const CTextStyle* pStyle2)
		{ return pStyle1->FirstLineIndent() == pStyle2->FirstLineIndent(); }
	BOOL TestStyle(const CTextStyle* pStyle)
		{ return pStyle->FirstLineIndent() != UNDEFINED_CFIXED; }
	void MakeUndefined(CTextStyle* pStyle)
		{ pStyle->FirstLineIndent(UNDEFINED_CFIXED); }
};

class CStyleLeftMarginHelper : public CParagraphStyleHelper
{
	void ApplyChange(CTextStyle* pStyle, const CTextStyleChange* pStyleChange)
		{ pStyle->LeftMargin(pStyleChange->m_lValue); }
	void BuildChange(CTextStyleChange* pStyleChange, const CTextStyle* pNewStyle, const CTextStyle* pOldStyle)
		{ pStyleChange->m_lValue = pNewStyle->LeftMargin(); pStyleChange->m_lOldValue = pOldStyle->LeftMargin();}
	BOOL CompareStyles(const CTextStyle* pStyle1, const CTextStyle* pStyle2)
		{ return pStyle1->LeftMargin() == pStyle2->LeftMargin(); }
	BOOL TestStyle(const CTextStyle* pStyle)
		{ return pStyle->LeftMargin() != UNDEFINED_CFIXED; }
	void MakeUndefined(CTextStyle* pStyle)
		{ pStyle->LeftMargin(UNDEFINED_CFIXED); }
};

class CStyleRightMarginHelper : public CParagraphStyleHelper
{
	void ApplyChange(CTextStyle* pStyle, const CTextStyleChange* pStyleChange)
		{ pStyle->RightMargin(pStyleChange->m_lValue); }
	void BuildChange(CTextStyleChange* pStyleChange, const CTextStyle* pNewStyle, const CTextStyle* pOldStyle)
		{ pStyleChange->m_lValue = pNewStyle->RightMargin(); pStyleChange->m_lOldValue = pOldStyle->RightMargin();}
	BOOL CompareStyles(const CTextStyle* pStyle1, const CTextStyle* pStyle2)
		{ return pStyle1->RightMargin() == pStyle2->RightMargin(); }
	BOOL TestStyle(const CTextStyle* pStyle)
		{ return pStyle->RightMargin() != UNDEFINED_CFIXED; }
	void MakeUndefined(CTextStyle* pStyle)
		{ pStyle->RightMargin(UNDEFINED_CFIXED); }
};

class CStyleSpaceBeforeHelper : public CParagraphStyleHelper
{
	void ApplyChange(CTextStyle* pStyle, const CTextStyleChange* pStyleChange)
		{ pStyle->SpaceBefore(pStyleChange->m_lValue); }
	void BuildChange(CTextStyleChange* pStyleChange, const CTextStyle* pNewStyle, const CTextStyle* pOldStyle)
		{ pStyleChange->m_lValue = pNewStyle->SpaceBefore(); pStyleChange->m_lOldValue = pOldStyle->SpaceBefore();}
	BOOL CompareStyles(const CTextStyle* pStyle1, const CTextStyle* pStyle2)
		{ return pStyle1->SpaceBefore() == pStyle2->SpaceBefore(); }
	BOOL TestStyle(const CTextStyle* pStyle)
		{ return pStyle->SpaceBefore() != UNDEFINED_CFIXED; }
	void MakeUndefined(CTextStyle* pStyle)
		{ pStyle->SpaceBefore(UNDEFINED_CFIXED); }
};

class CStyleSpaceAfterHelper : public CParagraphStyleHelper
{
	void ApplyChange(CTextStyle* pStyle, const CTextStyleChange* pStyleChange)
		{ pStyle->SpaceAfter(pStyleChange->m_lValue); }
	void BuildChange(CTextStyleChange* pStyleChange, const CTextStyle* pNewStyle, const CTextStyle* pOldStyle)
		{ pStyleChange->m_lValue = pNewStyle->SpaceAfter(); pStyleChange->m_lOldValue = pOldStyle->SpaceAfter();}
	BOOL CompareStyles(const CTextStyle* pStyle1, const CTextStyle* pStyle2)
		{ return pStyle1->SpaceAfter() == pStyle2->SpaceAfter(); }
	BOOL TestStyle(const CTextStyle* pStyle)
		{ return pStyle->SpaceAfter() != UNDEFINED_CFIXED; }
	void MakeUndefined(CTextStyle* pStyle)
		{ pStyle->SpaceAfter(UNDEFINED_CFIXED); }
};

// Bullets
class CStyleBulletTypeHelper : public CParagraphStyleHelper
{
	void ApplyChange(CTextStyle* pStyle, const CTextStyleChange* pStyleChange)
		{ pStyle->BulletType(pStyleChange->m_lValue); }
	void BuildChange(CTextStyleChange* pStyleChange, const CTextStyle* pNewStyle, const CTextStyle* pOldStyle)
		{ pStyleChange->m_lValue = pNewStyle->BulletType(); 
		pStyleChange->m_lOldValue = pOldStyle->BulletType(); 	}
	BOOL CompareStyles(const CTextStyle* pStyle1, const CTextStyle* pStyle2)
		{ return pStyle1->BulletType() == pStyle2->BulletType();	}
	BOOL TestStyle(const CTextStyle* pStyle)
		{ return pStyle->BulletType() != BULLET_undefined; }
	void MakeUndefined(CTextStyle* pStyle)
		{ pStyle->BulletType(BULLET_undefined); }
};

class CStyleBulletStyleHelper : public CParagraphStyleHelper
{
	void ApplyChange(CTextStyle* pStyle, const CTextStyleChange* pStyleChange)
		{ pStyle->BulletStyle(pStyleChange->m_lValue); }
	void BuildChange(CTextStyleChange* pStyleChange, const CTextStyle* pNewStyle, const CTextStyle* pOldStyle)
		{ pStyleChange->m_lValue = pNewStyle->BulletStyle(); 
		pStyleChange->m_lOldValue = pOldStyle->BulletStyle(); 	}
	BOOL CompareStyles(const CTextStyle* pStyle1, const CTextStyle* pStyle2)
		{ return pStyle1->BulletStyle() == pStyle2->BulletStyle();	}
	BOOL TestStyle(const CTextStyle* pStyle)
		{ return pStyle->BulletStyle() != BULLET_undefined; }
	void MakeUndefined(CTextStyle* pStyle)
		{ pStyle->BulletStyle(BULLET_undefined); }
};

class CStyleBulletLevelHelper : public CParagraphStyleHelper
{
	void ApplyChange(CTextStyle* pStyle, const CTextStyleChange* pStyleChange)
		{ pStyle->BulletLevel(pStyleChange->m_lValue); }
	void BuildChange(CTextStyleChange* pStyleChange, const CTextStyle* pNewStyle, const CTextStyle* pOldStyle)
		{ pStyleChange->m_lValue = pNewStyle->BulletLevel(); 
		pStyleChange->m_lOldValue = pOldStyle->BulletLevel(); 	}
	BOOL CompareStyles(const CTextStyle* pStyle1, const CTextStyle* pStyle2)
		{ return pStyle1->BulletLevel() == pStyle2->BulletLevel();	}
	BOOL TestStyle(const CTextStyle* pStyle)
		{ return pStyle->BulletLevel() != 255; }
	void MakeUndefined(CTextStyle* pStyle)
		{ pStyle->BulletLevel( 255 ); }
};

class CStyleBulletFontHelper : public CParagraphStyleHelper
{
	void ApplyChange(CTextStyle* pStyle, const CTextStyleChange* pStyleChange)
		{ pStyle->BulletFont(pStyleChange->m_lValue); }
	void BuildChange(CTextStyleChange* pStyleChange, const CTextStyle* pNewStyle, const CTextStyle* pOldStyle)
		{ pStyleChange->m_lValue = pNewStyle->BulletFont(); 
			pStyleChange->m_lOldValue = pOldStyle->BulletFont(); }
	BOOL CompareStyles(const CTextStyle* pStyle1, const CTextStyle* pStyle2)
		{ return pStyle1->BulletFont() == pStyle2->BulletFont(); }
	BOOL TestStyle(const CTextStyle* pStyle)
		{ return pStyle->BulletFont() != -1; }
	void MakeUndefined(CTextStyle* pStyle)
		{ pStyle->BulletFont(-1); }
};

class CStyleBulletColorHelper : public CParagraphStyleHelper
{
	void ApplyChange(CTextStyle* pStyle, const CTextStyleChange* pStyleChange)
		{ pStyle->BulletFont(pStyleChange->m_lValue); }
	void BuildChange(CTextStyleChange* pStyleChange, const CTextStyle* pNewStyle, const CTextStyle* pOldStyle)
		{ pStyleChange->m_lValue = pNewStyle->BulletColor(); 
			pStyleChange->m_lOldValue = pOldStyle->BulletFont(); }
	BOOL CompareStyles(const CTextStyle* pStyle1, const CTextStyle* pStyle2)
		{ return pStyle1->BulletColor() == pStyle2->BulletColor(); }
	BOOL TestStyle(const CTextStyle* pStyle)
		{ return pStyle->BulletColor() != 0xffffffff; }
	void MakeUndefined(CTextStyle* pStyle)
		{ pStyle->BulletColor(0xffffffff); }
};

class CStyleDropLinesHelper : public CParagraphStyleHelper
{
	void ApplyChange(CTextStyle* pStyle, const CTextStyleChange* pStyleChange)
		{ pStyle->DropLines(pStyleChange->m_lValue); }
	void BuildChange(CTextStyleChange* pStyleChange, const CTextStyle* pNewStyle, const CTextStyle* pOldStyle)
		{ pStyleChange->m_lValue = pNewStyle->DropLines(); 
			pStyleChange->m_lOldValue = pOldStyle->DropLines(); }
	BOOL CompareStyles(const CTextStyle* pStyle1, const CTextStyle* pStyle2)
		{ return pStyle1->DropLines() == pStyle2->DropLines(); }
	BOOL TestStyle(const CTextStyle* pStyle)
		{ return pStyle->DropLines() != -1; }
	void MakeUndefined(CTextStyle* pStyle)
		{ pStyle->DropLines(-1); }
};

class CStyleTabStopHelper : public CParagraphStyleHelper
{
	void ApplyChange(CTextStyle* pStyle, const CTextStyleChange* pStyleChange);
	void BuildChange(CTextStyleChange* pStyleChange, const CTextStyle* pNewStyle, const CTextStyle* pOldStyle)
		{}
	BOOL CompareStyles(const CTextStyle* pStyle1, const CTextStyle* pStyle2)
		{ return TRUE; }// Always return TRUE. Do not add to stream.
	BOOL TestStyle(const CTextStyle* pStyle)
		{ return FALSE; }
	void MakeUndefined(CTextStyle* pStyle)
		{}
};

void CStyleTabStopHelper::ApplyChange(CTextStyle* pStyle, const CTextStyleChange* pStyleChange)
{
	// Construct a tab stop from the style change.
	CTextTab Tab;
	Tab.FromStyleChange(pStyleChange);

	// Remove any tabs at this position from the style.
	pStyle->m_Tabs.RemoveTabAt(Tab.m_Position);
	// Add the tab in unless it is a "remove" tab.
	if (Tab.m_nType != CTextTab::TAB_remove)
	{
		// Add it to the style.
		pStyle->m_Tabs.Add(&Tab);
	}
}

class CStyleDefaultTabSizeHelper : public CParagraphStyleHelper
{
	void ApplyChange(CTextStyle* pStyle, const CTextStyleChange* pStyleChange)
		{ pStyle->m_Tabs.DefaultTabSize(pStyleChange->m_lValue); }
	void BuildChange(CTextStyleChange* pStyleChange, const CTextStyle* pNewStyle, const CTextStyle* pOldStyle)
		{ pStyleChange->m_lValue = pNewStyle->m_Tabs.DefaultTabSize(); pStyleChange->m_lOldValue = pOldStyle->m_Tabs.DefaultTabSize();}
	BOOL CompareStyles(const CTextStyle* pStyle1, const CTextStyle* pStyle2)
		{ return pStyle1->m_Tabs.DefaultTabSize() == pStyle2->m_Tabs.DefaultTabSize(); }
	BOOL TestStyle(const CTextStyle* pStyle)
		{ return pStyle->m_Tabs.DefaultTabSize() != UNDEFINED_CFIXED; }
	void MakeUndefined(CTextStyle* pStyle)
		{ pStyle->m_Tabs.DefaultTabSize(UNDEFINED_CFIXED); }
};

static CStyleAlignmentHelper AlignmentHelper;
static CStyleLeadingTypeHelper LeadingTypeHelper;
static CStyleLeadingHelper LeadingHelper;
static CStyleFirstLineIndentHelper FirstLineIndentHelper;
static CStyleLeftMarginHelper LeftMarginHelper;
static CStyleRightMarginHelper RightMarginHelper;
static CStyleSpaceBeforeHelper SpaceBeforeHelper;
static CStyleSpaceAfterHelper SpaceAfterHelper;
static CStyleTabStopHelper TabStopHelper;
static CStyleDefaultTabSizeHelper DefaultTabSizeHelper;
static CStyleBulletTypeHelper BulletTypeHelper;
static CStyleBulletStyleHelper BulletStyleHelper;
static CStyleBulletLevelHelper BulletLevelHelper;
static CStyleBulletFontHelper BulletFontHelper;
static CStyleBulletColorHelper BulletColorHelper;
static CStyleDropLinesHelper DropLinesHelper;

/////////////////////////////////////////////////////////////////////////////
// The style helper table.

CStyleHelper* near StyleHelpers[] =
{
	// Text style helpers.
	&FontHelper,				// TOKEN_Face
	&StyleHelper,				// TOKEN_Style (OBS - replaced by bold and italic)
	&SizeHelper,				// TOKEN_Size
	&PatternHelper,			// TOKEN_Pattern
	&OutlineHelper,			// TOKEN_Outline
	&InvalidHelper,			// TOKEN_Background (OBS - removed long ago)
	&ShadowHelper,				// TOKEN_Shadow
	&ColorHelper,				// TOKEN_Color (OBS - removed in 4.0 for new effects)
	&BoldHelper,				// TOKEN_Bold
	&ItalicHelper,				// TOKEN_Italic
	&UnderlineHelper,			// TOKEN_Underline
	&InvalidHelper,			// TOKEN_UnderlineColor (OBS - never used)
	&ParagraphHelper,			// TOKEN_Paragraph (internal)

	// New with 4.00

	&SpacingHelper,						// TOKEN_Spacing
	&FillPatternHelper,					// TOKEN_FillPattern,
	&FillForegroundColorHelper,		// TOKEN_FillForegroundColor,
	&OutlineStyleHelper,					// TOKEN_OutlineStyle,
	&OutlineWidthTypeHelper,			// TOKEN_OutlineWidthType,
	&OutlineWidthHelper,					// TOKEN_OutlineWidth,
	&OutlinePatternHelper,				// TOKEN_OutlinePattern,
	&OutlineForegroundColorHelper,	// TOKEN_OutlineForegroundColor,
	&ShadowStyleHelper,					// TOKEN_ShadowStyle,
	&ShadowOffsetTypeHelper,			// TOKEN_ShadowOffsetType,
	&ShadowXOffsetHelper,				// TOKEN_ShadowXOffset,
	&ShadowYOffsetHelper,				// TOKEN_ShadowYOffset,
	&ShadowPatternHelper,				// TOKEN_ShadowPattern,
	&ShadowForegroundColorHelper,		// TOKEN_ShadowForegroundColor,
	&WhiteOutHelper,						// TOKEN_WhiteOut,
   &HyperlinkHelper,						// TOKEN_Hyperlink,


	// Paragraph style helpers.
	// These must follow immediately after text style helpers.
	// See TokenToIndex().
	&AlignmentHelper,			// TOKEN_Alignment
	&LeadingTypeHelper,		// TOKEN_LeadingType
	&LeadingHelper,			// TOKEN_Leading
	&FirstLineIndentHelper,	// TOKEN_FirstLineIndent
	&LeftMarginHelper,		// TOKEN_LeftMargin
	&RightMarginHelper,		// TOKEN_RightMargin
	&SpaceBeforeHelper,		// TOKEN_SpaceBefore
	&SpaceAfterHelper,		// TOKEN_SpaceAfter
	&TabStopHelper,			// TOKEN_TabStop
	&DefaultTabSizeHelper,	// TOKEN_DefaultTabSize

	// new with 8.0
	&BulletTypeHelper,		// TOKEN_BulletType
	&BulletStyleHelper,		// TOKEN_BulletStyle
	&BulletLevelHelper,		// TOKEN_BulletLevel
	&BulletFontHelper,		// TOKEN_BulletFont
	&BulletColorHelper,		// TOKEN_BulletColor
	&DropLinesHelper			// TOKEN_DropLines
};
