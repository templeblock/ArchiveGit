/*
// $Header: /PM8/App/STYLEHLP.H 1     3/03/99 6:12p Gbeddow $
//
// Definitions for PMG database record objects.
//
// $Log: /PM8/App/STYLEHLP.H $
// 
// 1     3/03/99 6:12p Gbeddow
// 
// 2     3/24/98 5:28p Johno
// Added class CHyperlinkHelper
// 
//    Rev 1.0   14 Aug 1997 15:26:08   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:36:06   Fred
// Initial revision.
// 
//    Rev 1.4   15 May 1997 16:49:02   Fred
// New text effects
// 
//    Rev 1.3   26 Jun 1996 09:06:52   Jay
//  
// 
//    Rev 1.2   14 Jun 1996 17:05:38   Jay
//  
// 
//    Rev 1.1   13 Jun 1996 15:53:06   Jay
//  
// 
//    Rev 1.0   14 Mar 1996 13:42:44   Jay
// Initial revision.
// 
//    Rev 1.0   16 Feb 1996 12:06:08   FRED
// Initial revision.
// 
//    Rev 2.1   15 Mar 1995 10:40:30   JAY
// Support for new text editing
// 
//    Rev 2.0   07 Feb 1995 15:49:00   JAY
// Initial revision.
// 
//    Rev 1.1   10 Feb 1994 16:46:26   JAY
// Fixed bugs in style size helpers.
// 
//    Rev 1.0   16 Dec 1993 16:26:06   JAY
//  
*/

#ifndef __STYLEHLP_H__
#define __STYLEHLP_H__

#ifdef __STYLE_H__
//
// Include old style helpers.
//

/////////////////////////////////////////////////////////////////////////////
// StyleHelper - old style helper class.

/*
// A style helper.
// There will be one of these for each token.
*/

class StyleHelper
{                                
public:
	virtual VOID apply_change(TextStylePtr stylep, STYLE_CHANGE_PTR scp) = 0;
	virtual VOID build_change(STYLE_CHANGE_PTR scp, TextStylePtr new_stylep, TextStylePtr old_stylep) = 0;
	virtual BOOL compare_styles(TextStylePtr style1, TextStylePtr style2) = 0;
	virtual BOOL test_style(TextStylePtr stylep) = 0;
	virtual VOID make_undefined(TextStylePtr stylep) = 0;
};

/* Face. */

class StyleFaceHelper : public StyleHelper
{
	VOID apply_change(TextStylePtr stylep, STYLE_CHANGE_PTR scp)
		{ stylep->set_face(scp->new_value); }
	VOID build_change(STYLE_CHANGE_PTR scp, TextStylePtr new_stylep, TextStylePtr old_stylep)
		{ scp->new_value = new_stylep->get_face(); }
	BOOL compare_styles(TextStylePtr style1, TextStylePtr style2)
		{ return style1->get_face() == style2->get_face(); }
	BOOL test_style(TextStylePtr stylep)
		{ return stylep->get_face() != 0; }
	VOID make_undefined(TextStylePtr stylep)
		{ stylep->set_face(0); }
};

/* Font style. */

class StyleFontStyleHelper : public StyleHelper
{
	VOID apply_change(TextStylePtr stylep, STYLE_CHANGE_PTR scp)
		{ stylep->set_font_style((FONT_STYLE)scp->new_value); }
	VOID build_change(STYLE_CHANGE_PTR scp, TextStylePtr new_stylep, TextStylePtr old_stylep)
		{ scp->new_value = (LONG)new_stylep->get_font_style(); }
	BOOL compare_styles(TextStylePtr style1, TextStylePtr style2)
//		{ return style1->get_font_style() == style2->get_font_style(); }
		{ return TRUE; }			// Always return TRUE. Do not add to stream.
	BOOL test_style(TextStylePtr stylep)
		{ return stylep->get_font_style() != FONT_STYLE_Undefined; }
	VOID make_undefined(TextStylePtr stylep)
		{ stylep->set_font_style(FONT_STYLE_Undefined); }
};

/* Size. */

class StyleSizeHelper : public StyleHelper
{
	VOID apply_change(TextStylePtr stylep, STYLE_CHANGE_PTR scp);
	VOID build_change(STYLE_CHANGE_PTR scp, TextStylePtr new_stylep, TextStylePtr old_stylep);
	BOOL compare_styles(TextStylePtr style1, TextStylePtr style2)
		{ return style1->get_base_size() == style2->get_base_size(); }
	BOOL test_style(TextStylePtr stylep)
		{ return stylep->get_base_size() != -1; }
	VOID make_undefined(TextStylePtr stylep)
		{ stylep->set_base_size(-1); }
};

/* Pattern. */

class StylePatternHelper : public StyleHelper
{
	VOID apply_change(TextStylePtr stylep, STYLE_CHANGE_PTR scp)
		{ stylep->set_pattern((SHORT)scp->new_value); }
	VOID build_change(STYLE_CHANGE_PTR scp, TextStylePtr new_stylep, TextStylePtr old_stylep)
		{ scp->new_value = (LONG)new_stylep->get_pattern(); }
	BOOL compare_styles(TextStylePtr style1, TextStylePtr style2)
		{ return style1->get_pattern() == style2->get_pattern(); }
	BOOL test_style(TextStylePtr stylep)
		{ return stylep->get_pattern() != -1; }
	VOID make_undefined(TextStylePtr stylep)
		{ stylep->set_pattern(-1); }
};

/* Outline. */

class StyleOutlineHelper : public StyleHelper
{
	VOID apply_change(TextStylePtr stylep, STYLE_CHANGE_PTR scp)
		{ stylep->set_outline((SHORT)scp->new_value); }
	VOID build_change(STYLE_CHANGE_PTR scp, TextStylePtr new_stylep, TextStylePtr old_stylep)
		{ scp->new_value = (LONG)new_stylep->get_outline(); }
	BOOL compare_styles(TextStylePtr style1, TextStylePtr style2)
		{ return style1->get_outline() == style2->get_outline(); }
	BOOL test_style(TextStylePtr stylep)
		{ return stylep->get_outline() != -1; }
	VOID make_undefined(TextStylePtr stylep)
		{ stylep->set_outline(-1); }
};

/* Background. */

class StyleBackgroundHelper : public StyleHelper
{
	VOID apply_change(TextStylePtr stylep, STYLE_CHANGE_PTR scp)
		{ stylep->set_background((SHORT)scp->new_value); }
	VOID build_change(STYLE_CHANGE_PTR scp, TextStylePtr new_stylep, TextStylePtr old_stylep)
		{ scp->new_value = (LONG)new_stylep->get_background(); }
	BOOL compare_styles(TextStylePtr style1, TextStylePtr style2)
		{ return style1->get_background() == style2->get_background(); }
	BOOL test_style(TextStylePtr stylep)
		{ return stylep->get_background() != -1; }
	VOID make_undefined(TextStylePtr stylep)
		{ stylep->set_background(-1); }
};

/* Shadow. */

class StyleShadowHelper : public StyleHelper
{
	VOID apply_change(TextStylePtr stylep, STYLE_CHANGE_PTR scp)
		{ stylep->set_shadow((SHORT)scp->new_value); }
	VOID build_change(STYLE_CHANGE_PTR scp, TextStylePtr new_stylep, TextStylePtr old_stylep)
		{ scp->new_value = (LONG)new_stylep->get_shadow(); }
	BOOL compare_styles(TextStylePtr style1, TextStylePtr style2)
		{ return style1->get_shadow() == style2->get_shadow(); }
	BOOL test_style(TextStylePtr stylep)
		{ return stylep->get_shadow() != -1; }
	VOID make_undefined(TextStylePtr stylep)
		{ stylep->set_shadow(-1); }
};

/* Color. */

class StyleColorHelper : public StyleHelper
{
	VOID apply_change(TextStylePtr stylep, STYLE_CHANGE_PTR scp)
		{ stylep->set_color((COLOR)scp->new_value); }
	VOID build_change(STYLE_CHANGE_PTR scp, TextStylePtr new_stylep, TextStylePtr old_stylep)
		{ scp->new_value = new_stylep->get_color(); }
	BOOL compare_styles(TextStylePtr style1, TextStylePtr style2)
//		{ return style1->get_color() == style2->get_color(); }
		{ return TRUE; }			// Always return TRUE. Do not add to stream.
	BOOL test_style(TextStylePtr stylep)
		{ return stylep->get_color() != UNDEFINED_COLOR; }
	VOID make_undefined(TextStylePtr stylep)
		{ stylep->set_color(UNDEFINED_COLOR); }
};

/* Font bold style. */

class StyleBoldHelper : public StyleHelper
{
	VOID apply_change(TextStylePtr stylep, STYLE_CHANGE_PTR scp)
		{ stylep->SetBoldStyle((int)scp->new_value); }
	VOID build_change(STYLE_CHANGE_PTR scp, TextStylePtr new_stylep, TextStylePtr old_stylep)
		{ scp->new_value = (LONG)new_stylep->GetBoldStyle(); }
	BOOL compare_styles(TextStylePtr style1, TextStylePtr style2)
		{ return style1->GetBoldStyle() == style2->GetBoldStyle(); }
	BOOL test_style(TextStylePtr stylep)
		{ return stylep->GetBoldStyle() != -1; }
	VOID make_undefined(TextStylePtr stylep)
		{ stylep->SetBoldStyle(-1); }
};

/* Font italic style. */

class StyleItalicHelper : public StyleHelper
{
	VOID apply_change(TextStylePtr stylep, STYLE_CHANGE_PTR scp)
		{ stylep->SetItalicStyle((int)scp->new_value); }
	VOID build_change(STYLE_CHANGE_PTR scp, TextStylePtr new_stylep, TextStylePtr old_stylep)
		{ scp->new_value = (LONG)new_stylep->GetItalicStyle(); }
	BOOL compare_styles(TextStylePtr style1, TextStylePtr style2)
		{ return style1->GetItalicStyle() == style2->GetItalicStyle(); }
	BOOL test_style(TextStylePtr stylep)
		{ return stylep->GetItalicStyle() != -1; }
	VOID make_undefined(TextStylePtr stylep)
		{ stylep->SetItalicStyle(-1); }
};

/* Font underline style. */

class StyleUnderlineHelper : public StyleHelper
{
	VOID apply_change(TextStylePtr stylep, STYLE_CHANGE_PTR scp)
		{ stylep->UnderlineStyle((UNDERLINE_STYLE)scp->new_value); }
	VOID build_change(STYLE_CHANGE_PTR scp, TextStylePtr new_stylep, TextStylePtr old_stylep)
		{ scp->new_value = (LONG)new_stylep->UnderlineStyle(); }
	BOOL compare_styles(TextStylePtr style1, TextStylePtr style2)
		{ return style1->UnderlineStyle() == style2->UnderlineStyle(); }
	BOOL test_style(TextStylePtr stylep)
		{ return stylep->UnderlineStyle() != UNDERLINE_STYLE_Undefined; }
	VOID make_undefined(TextStylePtr stylep)
		{ stylep->UnderlineStyle(UNDERLINE_STYLE_Undefined); }
};

/* Underline Color. */

class StyleUnderlineColorHelper : public StyleHelper
{
	VOID apply_change(TextStylePtr stylep, STYLE_CHANGE_PTR scp)
		{ stylep->UnderlineColor((COLOR)scp->new_value); }
	VOID build_change(STYLE_CHANGE_PTR scp, TextStylePtr new_stylep, TextStylePtr old_stylep)
		{ scp->new_value = new_stylep->UnderlineColor(); }
	BOOL compare_styles(TextStylePtr style1, TextStylePtr style2)
		{ return style1->UnderlineColor() == style2->UnderlineColor(); }
	BOOL test_style(TextStylePtr stylep)
		{ return stylep->UnderlineColor() != UNDEFINED_COLOR; }
	VOID make_undefined(TextStylePtr stylep)
		{ stylep->UnderlineColor(UNDEFINED_COLOR); }
};
#endif

/////////////////////////////////////////////////////////////////////////////
// CStyleHelper - style helper class.

/*
// A style helper.
// There will be one of these for each token.
*/

class CStyleHelper
{                                
public:
	virtual void ApplyChange(CTextStyle* pStyle, const CTextStyleChange* pStyleChange) = 0;
	virtual void BuildChange(CTextStyleChange* pStyleChange, const CTextStyle* pNewStyle, const CTextStyle* pOldStyle) = 0;
	virtual BOOL CompareStyles(const CTextStyle* pStyle1, const CTextStyle* pStyle2) = 0;
	virtual BOOL TestStyle(const CTextStyle* pStyle) = 0;
	virtual void MakeUndefined(CTextStyle* pStyle) = 0;
	virtual void ApplyOldChange(CTextStyle* pStyle, const STYLE_CHANGE* pStyleChange) = 0;
};

extern CStyleHelper* near StyleHelpers[];
extern CStyleHelper* near ParagraphStyleHelpers[];

#endif				// #ifndef __STYLEHLP_H__
