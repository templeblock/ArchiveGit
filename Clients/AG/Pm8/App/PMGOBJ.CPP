/*
// $Header: /PM8/App/PMGOBJ.CPP 1     3/03/99 6:09p Gbeddow $
//
// PMG object routines.
//
// $Log: /PM8/App/PMGOBJ.CPP $
// 
// 1     3/03/99 6:09p Gbeddow
// 
// 32    2/01/99 5:46p Lwilson
// Fixed bug in gradient conversion code.
// 
// 31    2/01/99 5:19p Lwilson
// Added RColor gradient conversion code to object properties
// 
// 30    2/01/99 4:34p Lwilson
// Added initial support for converting PM objects into component objects.
// 
// 29    1/29/99 12:59p Lwilson
// Moved RColor object properties fill data member and into Object
// Properties.
// 
// 28    1/27/99 1:43p Lwilson
// Added RColor support to FillForegroundColor property.
// 
// 27    12/23/98 1:51p Psasse
// Code clean up
// 
// 26    9/14/98 12:09p Jayn
// Removed system_heap. Switched to MFC in DLL.
// 
// 25    7/24/98 11:57a Rlovejoy
// Save flags when joining a group.
// 
// 24    7/18/98 6:24p Jayn
// Table fixes.
// 
// 23    7/14/98 7:05p Psasse
// Flipping grouped objects in the x and y direction simultaneously is now
// working properly
// 
// 22    6/04/98 7:52p Jayn
// Table fill down and right plus a lot of "fit to text" stuff
// 
// 21    5/15/98 12:04p Johno
// Maybe THIS will work...
// 
// 20    5/15/98 10:33a Johno
// Use member functions in CObjectProperties::MakeInvisible() and
// CObjectProperties::IsInvisible()
// 
// 19    5/14/98 4:48p Johno
// Moved :MakeInvisible() and IsInvisible() to CObjectProperties
// 
// 18    5/14/98 12:39p Johno
// Added PMGPageObject::MakeInvisible() and PMGPageObject::IsInvisible()
// 
// 17    5/13/98 4:34p Johno
// IsInvisibleHotSpot() (preliminary)
// 
// 16    4/30/98 4:47p Jayn
// Replaced FillFormat with FillFormatV1 (old) and FillFormatV2 (new).
// The new one has the gradient members.
// 
// 15    4/29/98 6:31p Rlovejoy
// Updated for new fill format.
// 
// 14    4/28/98 11:31a Johno
// aDDED nORMALIZErECT TO PMObjectShapeData::GetRect() for Netscape
// browser
// 
// 13    4/23/98 3:28p Johno
// HTML changes for rotated text (as graphic with polygon image map)
// 
// 12    4/23/98 10:49a Johno
// Added PMObjectShapeData, GetShapeData()
// for polygon image maps
// 
// 11    4/22/98 10:24a Jayn
// Changes for tables.
// 
// 10    4/13/98 5:54p Johno
// Added FreeHyperlinkRecord() in ~PMGPageObjectState()
// 
// 9     4/13/98 4:54p Johno
// Changes for undo and hyperlinks
// 
// 8     4/08/98 10:29a Johno
// PMGPageObject::assign() now creates a new hyperlink record for a
// duplicated PMGPageObject
// 
// 7     3/25/98 6:36p Johno
// HyperLink and Hyperlink now all Hyperlink for sanity
// 
// 6     3/13/98 2:54p Johno
// Added GetHyperlinkData
// 
// 5     3/11/98 5:53p Johno
// Use HyperlinkData struct
// 
// 4     3/10/98 4:08p Johno
// Added GetHyperlink, SetHyperlinkRecord, and KillHyperlinkRecord
// 
// 3     3/06/98 5:49p Johno
// Call database->delete_record in PMGPageObject::destroy (needs work)
// 
// 2     1/19/98 1:20p Fredf
// Year 2000 compliance and exorcism of CCTL3D.
// 
//    Rev 1.0   14 Aug 1997 15:23:52   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:39:46   Fred
// Initial revision.
// 
//    Rev 1.44   28 Jul 1997 14:04:56   Jay
// Fixes for draw objects overflows.
// 
//    Rev 1.43   20 Jul 1997 14:41:56   Jay
// Fixed handle glitch if bound got reversed.
// 
//    Rev 1.42   20 Jul 1997 14:37:00   Fred
// Does not allow handle wrap when cropping; handles empty clip regions
// 
//    Rev 1.41   18 Jul 1997 14:49:36   Jay
// Moved CObjectStateArray to pmgobj.
// 
//    Rev 1.40   17 Jul 1997 18:11:20   Fred
// Clips object size to MAXIMUM_OBJECT_DIMENSION (defined in pagedefs.h)
// 
//    Rev 1.39   07 Jul 1997 14:29:56   Jay
// Sets prop_dim correctly when initial bounds is 0.
// 
//    Rev 1.38   13 Jun 1997 14:06:40   Jay
// Won't continue if CombineRgn fails.
// 
//    Rev 1.37   23 May 1997 17:06:44   Fred
// New effects
// 
//    Rev 1.36   14 Apr 1997 15:42:36   Jay
// Text box editing changes
// 
//    Rev 1.35   28 Mar 1997 17:10:50   Jay
// Ripped out albums, sound manager, side bar, etc.
// 
//    Rev 1.34   26 Mar 1997 14:51:58   Jay
// Interruptable preview
// 
//    Rev 1.33   21 Mar 1997 11:10:36   Jay
// Print preview changes
// 
//    Rev 1.32   17 Mar 1997 14:48:12   Jay
// Warp shape palette and support.
// 
//    Rev 1.31   06 Mar 1997 17:28:04   Jay
// Print preview stuff
// 
//    Rev 1.30   04 Mar 1997 17:03:42   Jay
// Fixes for drawing objects, cropping, etc.
// 
//    Rev 1.29   26 Feb 1997 10:57:30   Jay
// Cropping; GetClipRgn() changes
// 
//    Rev 1.28   25 Feb 1997 17:01:20   Jay
// More crop goodies.
// 
//    Rev 1.27   25 Feb 1997 12:57:18   Jay
// Cropping and extensions
// 
//    Rev 1.26   21 Feb 1997 15:14:26   Jay
// New mode handling; crop mode support
// 
//    Rev 1.26   21 Feb 1997 15:11:30   Jay
// New mode handling; crop mode support
// 
//    Rev 1.25   19 Feb 1997 16:54:10   Jay
// Beginning of cropping
// 
//    Rev 1.24   07 Feb 1997 08:51:32   Fred
// Added GetClipRgn() object method
// 
//    Rev 1.23   29 Jan 1997 12:49:10   Jay
// Restored the ASSERTion in SetObjectProperties().
// 
//    Rev 1.22   28 Jan 1997 14:10:22   Jay
// Shape drawing, default size, mouse activate, etc.
// 
//    Rev 1.21   24 Jan 1997 12:39:10   Jay
// Initial line palette; toggle tweaks, etc.
// 
//    Rev 1.20   23 Jan 1997 11:05:32   Jay
// New popup palettes
// 
//    Rev 1.19   17 Jan 1997 17:03:12   Jay
// New select for drawing objects.
// 
//    Rev 1.18   16 Jan 1997 13:02:40   Jay
// New status stuff. More cleanup.
// 
//    Rev 1.17   08 Jan 1997 15:22:14   Fred
// More text box changes
// 
//    Rev 1.16   11 Dec 1996 15:23:58   Jay
// Passes unrotated point to PtIsOpaque.
// 
//    Rev 1.15   09 Dec 1996 11:21:44   Jay
// Scaled PtIsOpaque now works.
// 
//    Rev 1.14   09 Dec 1996 10:19:36   Jay
// More image stuff.
// 
//    Rev 1.13   28 Oct 1996 13:46:32   Jay
// Changes from 3.01 code
// 
//    Rev 1.13   25 Oct 1996 10:29:26   Jay
// Frame not on page support.
// 
//    Rev 1.12   07 Oct 1996 17:02:32   Jay
// TimeStamp
// 
//    Rev 1.11   27 Aug 1996 10:51:06   Jay
// Got rid of an erroneous ASSERT.
// 
//    Rev 1.10   29 Jul 1996 11:21:22   Jay
// ImageChanged if rotation changes.
// 
//    Rev 1.9   27 Jul 1996 10:15:06   Jay
// page fixes
// 
//    Rev 1.8   26 Jul 1996 11:38:58   Jay
// Page stuff
// 
//    Rev 1.7   25 Jul 1996 19:38:10   Jay
//  
// 
//    Rev 1.6   19 Jul 1996 18:01:02   Fred
//  
// 
//    Rev 1.5   15 Jul 1996 17:46:00   Jay
//  
// 
//    Rev 1.4   15 Jul 1996 11:34:30   Jay
//  
// 
//    Rev 1.3   12 Jul 1996 16:47:56   Jay
// Text wrap
// 
//    Rev 1.2   08 Jul 1996 08:49:06   Jay
// Bigger handles. Better pt_in_object
// 
//    Rev 1.1   28 Jun 1996 17:32:00   Jay
// OnPage() routine.
// 
//    Rev 1.0   14 Mar 1996 13:43:12   Jay
// Initial revision.
// 
//    Rev 1.1   06 Mar 1996 09:54:36   Jay
// Changes in 2.1
// 
//    Rev 2.15   16 Feb 1996 15:31:30   JAY
// Removed code which prevented object from having a layout when grouped.
// This conflicted with card panel objects (which is the only case where this
// can happen in the current code).
// 
//    Rev 2.14   10 Oct 1995 14:54:22   FRED
// Some comments
// 
//    Rev 2.13   10 Oct 1995 12:39:08   FRED
// Reworked scaling of rotated objects.
// 
// 
//    Rev 2.12   10 Oct 1995 12:37:16   FRED
// 
//    Rev 2.11   03 Sep 1995 14:43:24   JAY
// Fixed the "rotated object in group" problem.
// 
//    Rev 2.10   01 Sep 1995 18:37:26   JAY
// Some fixes... some TRACEs... some left over problems.
// 
//    Rev 2.9   14 Aug 1995 10:10:22   JAY
// Added rotation save to object (for move handle and move object).
// 
//    Rev 2.8   22 Jun 1995 16:05:34   JAY
// Now handles flip correctly.
// 
//    Rev 2.7   11 May 1995 07:56:22   JAY
// Set unrotated bound. Constrained rotate.
// 
//    Rev 2.6   10 May 1995 13:15:08   JAY
// Now handles "flat" objects correctly in calc_bounds().
// 
//    Rev 2.5   09 May 1995 08:04:56   JAY
// Now turns off more handles when rotating and moving.
// 
//    Rev 2.4   19 Apr 1995 16:05:14   JAY
// Removed some symbols.
// 
//    Rev 2.3   29 Mar 1995 08:59:08   JAY
// Lots of changes
// 
//    Rev 2.2   09 Mar 1995 08:27:26   JAY
//  
// 
//    Rev 2.1   15 Feb 1995 17:03:06   JAY
// Added states to objects.
// 
//    Rev 2.0   07 Feb 1995 15:48:02   JAY
// Initial revision.
// 
//    Rev 1.13   30 Nov 1994 16:34:20   JAY
// stdafx.h; merged database code from album
// 
//    Rev 1.12   04 Aug 1994 07:33:18   JAY
// Large model and 32-bit changes
// 
//    Rev 1.11   24 Feb 1994 13:19:12   JAY
// Toggling of objects during movement and resizing is now handled entirely
// by the object.
// 
//    Rev 1.10   17 Feb 1994 09:22:58   JAY
// Added a RedisplayContext argument to get_refresh_bound() && is_opaque();
// 
//    Rev 1.9   14 Feb 1994 11:28:20   JAY
// Made a static class near.
// 
//    Rev 1.8   10 Feb 1994 15:14:08   MACDUFF
// Toggle functions now return flag indicating whether they drew anything.
// 
//    Rev 1.7   09 Feb 1994 18:25:22   MACDUFF
// Early out check in toggle_select.
// 
//    Rev 1.6   07 Feb 1994 16:00:22   MACDUFF
// Pass clip param to xor_line and do_XorBox
// 
//    Rev 1.5   30 Jan 1994 11:58:46   JAY
// Fixed a bug in group_calc().
// 
//    Rev 1.4   28 Jan 1994 17:10:50   JAY
//  
// 
//    Rev 1.3   13 Jan 1994 17:00:08   JAY
//  
// 
//    Rev 1.2   05 Jan 1994 08:05:36   JAY
// Restructuring, etc.
// 
//    Rev 1.1   30 Dec 1993 16:10:08   JAY
//  
//
//   Rev 1.0   16 Dec 1993 16:24:50   JAY
// 
*/

#include "stdafx.h"

#include <string.h>
#include <stdlib.h>
#include <math.h>

#include "pmgdb.h"

#include "layrec.h"

#include "db386.h"

#include "utils.h"
#include "action.h"
#include "outpath.h"
#include "objimg.h"
#include "hyperrec.h"

// Framework support
#include "GradientFill.h"

#define ABS(x) (((x) >= 0 ) ? (x) : -(x))
#define DELTA(x1, x0)   (ABS((long)(x1)-(long)(x0)))

#define CTRL_KEY(key)     ((key) & MK_CONTROL)
#define SHIFT_KEY(key)   ((key) & MK_SHIFT)
#define SPECIAL_KEY(key)   ((key) & (MK_CONTROL|MK_SHIFT))

#define ROTATE_HANDLE_OFFSET  CONTROL_DOT_WIDTH*4

PPNT near RectPageObject::prop_dim;
PPNT near RectPageObject::real_dim;
ANGLE near RectPageObject::starting_angle;
PBOX near m_OriginalDrawBound;
PPNT near m_OriginalOrigin;

/////////////////////////////////////////////////////////////////////////////
// CObjectProperties

CObjectProperties::CObjectProperties(PMGDatabase* pDatabase /*=NULL*/)
{
   SetDatabase(pDatabase);
   m_lWarpField = 0;
   Reset();
}

CObjectProperties::~CObjectProperties()
{
   Reset();
}

void CObjectProperties::Reset(void)
{
   // No properties set.
   m_wPropertySet = 0;
   // So, of course, no properties undefined.
   m_wPropertyUndefined = 0;

   // Free the warp field if we have one.
   FreeWarpField();
}

void CObjectProperties::FreeWarpField(void)
{
   // Free the warp field if we have one.
   if (m_lWarpField != 0)
   {
      // If somebody set the warp field, we must have a database.
      ASSERT(m_pDatabase != NULL);
      if (m_pDatabase != NULL)
      {
         m_pDatabase->free_warp_field_record(m_lWarpField);
      }
      m_lWarpField = 0;
   }
}

void CObjectProperties::MergeWith(CObjectProperties& OtherProperties)
{
   // Fill
   if (OtherProperties.ApplyFillPattern(m_Fill.m_nPattern))
   {
      SetFillPattern(m_Fill.m_nPattern, FALSE);
   }
   if (OtherProperties.ApplyFillType(m_Fill.m_nFillType))
   {
      SetFillType(m_Fill.m_nFillType, FALSE);
   }
   if (OtherProperties.ApplyFillForegroundColor(m_Fill.m_ForegroundColor))
   {
      SetFillForegroundColor(m_Fill.m_ForegroundColor, FALSE);
   }
   if (OtherProperties.ApplyFillForegroundColor(m_crFill))
   {
      SetFillForegroundColor(m_crFill, FALSE);
   }
   if (OtherProperties.ApplyFillGradientBlendColor(m_Fill.m_GradientBlendColor))
   {
      SetFillGradientBlendColor(m_Fill.m_GradientBlendColor, FALSE);
   }

   // Outline
   if (OtherProperties.ApplyOutlineStyle(m_Outline.m_nStyle))
   {
      SetOutlineStyle(m_Outline.m_nStyle, FALSE);
   }
   if (OtherProperties.ApplyOutlineWidthType(m_Outline.m_nWidthType))
   {
      SetOutlineWidthType(m_Outline.m_nWidthType, FALSE);
   }
   if (OtherProperties.ApplyOutlineWidth(m_Outline.m_lWidth))
   {
      SetOutlineWidth(m_Outline.m_lWidth, FALSE);
   }
   if (OtherProperties.ApplyOutlinePattern(m_Outline.m_nPattern))
   {
      SetOutlinePattern(m_Outline.m_nPattern, FALSE);
   }
   if (OtherProperties.ApplyOutlineForegroundColor(m_Outline.m_ForegroundColor))
   {
      SetOutlineForegroundColor(m_Outline.m_ForegroundColor, FALSE);
   }

   // Shadow
   if (OtherProperties.ApplyShadowStyle(m_Shadow.m_nStyle))
   {
      SetShadowStyle(m_Shadow.m_nStyle, FALSE);
   }
   if (OtherProperties.ApplyShadowOffsetType(m_Shadow.m_nOffsetType))
   {
      SetShadowOffsetType(m_Shadow.m_nOffsetType, FALSE);
   }
   if (OtherProperties.ApplyShadowXOffset(m_Shadow.m_lXOffset))
   {
      SetShadowXOffset(m_Shadow.m_lXOffset, FALSE);
   }
   if (OtherProperties.ApplyShadowYOffset(m_Shadow.m_lYOffset))
   {
      SetShadowYOffset(m_Shadow.m_lYOffset, FALSE);
   }
   if (OtherProperties.ApplyShadowPattern(m_Shadow.m_nPattern))
   {
      SetShadowPattern(m_Shadow.m_nPattern, FALSE);
   }
   if (OtherProperties.ApplyShadowForegroundColor(m_Shadow.m_ForegroundColor))
   {
      SetShadowForegroundColor(m_Shadow.m_ForegroundColor, FALSE);
   }

   // Warp field
   DB_RECORD_NUMBER lWarpField;
   if (OtherProperties.ApplyWarpField(lWarpField))
   {
      SetWarpField(lWarpField, FALSE);
   }
}

#define ObjectEffectSetApply(Effect, Field, Var, Type) \
	void CObjectProperties::Set##Effect##Field(Type Var, BOOL fMerge /*=TRUE*/) \
	{ \
		if (fMerge && ((m_wPropertySet & FLAG_##Effect##Field) != 0)) \
		{ \
			if (m_##Effect.m_##Var != Var) \
			{ \
				m_wPropertyUndefined |= FLAG_##Effect##Field; \
			} \
		} \
		else \
		{ \
			m_wPropertySet |= FLAG_##Effect##Field; \
			m_wPropertyUndefined &= ~FLAG_##Effect##Field; \
			m_##Effect.m_##Var = Var; \
		} \
	} \
	BOOL CObjectProperties::Apply##Effect##Field(Type& Var) \
	{ \
		if (((m_wPropertySet & FLAG_##Effect##Field) != 0) && ((m_wPropertyUndefined & FLAG_##Effect##Field) == 0)) \
		{ \
			Var = m_##Effect.m_##Var; \
			return TRUE; \
		} \
		return FALSE; \
	}


// Pattern

	ObjectEffectSetApply(Fill, Pattern, nPattern, SHORT)
//	ObjectEffectSetApply(Fill, Type, nFillType, SHORT)
//	ObjectEffectSetApply(Fill, ForegroundColor, ForegroundColor, COLOR)
//	ObjectEffectSetApply(Fill, GradientBlendColor, GradientBlendColor, COLOR)

// Outline

	ObjectEffectSetApply(Outline, Style, nStyle, SHORT)
	ObjectEffectSetApply(Outline, WidthType, nWidthType, SHORT)
	ObjectEffectSetApply(Outline, Width, lWidth, CFixed)
	ObjectEffectSetApply(Outline, Pattern, nPattern, SHORT)
	ObjectEffectSetApply(Outline, ForegroundColor, ForegroundColor, COLOR)

// Shadow

	ObjectEffectSetApply(Shadow, Style, nStyle, SHORT)
	ObjectEffectSetApply(Shadow, OffsetType, nOffsetType, SHORT)
	ObjectEffectSetApply(Shadow, XOffset, lXOffset, CFixed)
	ObjectEffectSetApply(Shadow, YOffset, lYOffset, CFixed)
	ObjectEffectSetApply(Shadow, Pattern, nPattern, SHORT)
	ObjectEffectSetApply(Shadow, ForegroundColor, ForegroundColor, COLOR)

void CObjectProperties::SetFillForegroundColor(COLOR ForegroundColor, BOOL fMerge /*= TRUE*/)
{
	if (fMerge && ((m_wPropertySet & FLAG_FillForegroundColor) != 0)) 
	{
		if (m_Fill.m_ForegroundColor != ForegroundColor)
		{
			m_wPropertyUndefined |= FLAG_FillForegroundColor;
		}
	}
	else
	{
		m_wPropertySet |= FLAG_FillForegroundColor;
		m_wPropertyUndefined &= ~FLAG_FillForegroundColor;
		m_Fill.m_ForegroundColor = ForegroundColor;
		m_crFill = RColorFromColor( ForegroundColor );
	}
}

BOOL CObjectProperties::ApplyFillForegroundColor(COLOR& ForegroundColor)
{ 
	if (((m_wPropertySet & FLAG_FillForegroundColor) != 0) && ((m_wPropertyUndefined & FLAG_FillForegroundColor) == 0))
	{ 
		ForegroundColor = m_Fill.m_ForegroundColor;
		return TRUE;
	} 

	return FALSE;
}

void CObjectProperties::SetFillForegroundColor(const RColor& ForegroundColor, BOOL fMerge /*= TRUE*/)
{
	if (fMerge && ((m_wPropertySet & FLAG_FillForegroundColor) != 0)) 
	{
		if (m_crFill != ForegroundColor)
		{
			m_wPropertyUndefined |= FLAG_FillForegroundColor;
		}
	}
	else
	{
		m_wPropertySet |= FLAG_FillForegroundColor;
		m_wPropertyUndefined &= ~FLAG_FillForegroundColor;
		m_Fill.m_ForegroundColor = ColorFromRColor( ForegroundColor );
		m_crFill = ForegroundColor;
	}
}

BOOL CObjectProperties::ApplyFillForegroundColor(RColor& ForegroundColor)
{ 
	if (((m_wPropertySet & FLAG_FillForegroundColor) != 0) && ((m_wPropertyUndefined & FLAG_FillForegroundColor) == 0))
	{ 
		ForegroundColor = m_crFill;
		return TRUE;
	} 

	return FALSE;
}

void CObjectProperties::SetFillGradientBlendColor(COLOR GradientBlendColor, BOOL fMerge /*= TRUE*/)
{
	if (fMerge && ((m_wPropertySet & FLAG_FillGradientBlendColor) != 0)) 
	{
		if (m_Fill.m_GradientBlendColor != GradientBlendColor)
		{
			m_wPropertyUndefined |= FLAG_FillGradientBlendColor;
		}
	}
	else
	{
		m_wPropertySet |= FLAG_FillGradientBlendColor;
		m_wPropertyUndefined &= ~FLAG_FillGradientBlendColor;
		m_Fill.m_GradientBlendColor = GradientBlendColor;

		SetFillType( m_Fill.m_nFillType, fMerge );
	}
}

BOOL CObjectProperties::ApplyFillGradientBlendColor(COLOR& GradientBlendColor)
{ 
	if (((m_wPropertySet & FLAG_FillGradientBlendColor) != 0) && ((m_wPropertyUndefined & FLAG_FillGradientBlendColor) == 0))
	{ 
		GradientBlendColor = m_Fill.m_GradientBlendColor;
		return TRUE;
	} 

	return FALSE;
}

void CObjectProperties::SetFillType(SHORT nFillType, BOOL fMerge /*=TRUE*/) 
{
	if (fMerge && ((m_wPropertySet & FLAG_FillType) != 0))
	{
		if (m_Fill.m_nFillType != nFillType)
		{
			m_wPropertyUndefined |= FLAG_FillType;
		}
	}
	else
	{ 
		m_wPropertySet |= FLAG_FillType;
		m_wPropertyUndefined &= ~FLAG_FillType;
		m_Fill.m_nFillType = nFillType;

		if (nFillType == FillFormatV2::FillNone)
			m_crFill = RColor();
		else if (nFillType == FillFormatV2::FillSolid)
			m_crFill = RColorFromColor( m_Fill.m_ForegroundColor );
		else if (nFillType >= FillFormatV2::FillSweepRight && nFillType <= FillFormatV2::FillRadialCorner)
		{
			RGradientRampPoint pts[] = 
			{
				RGradientRampPoint( colorref_from_color( m_Fill.m_ForegroundColor ), 0.00, 0.50 ),
				RGradientRampPoint( colorref_from_color( m_Fill.m_GradientBlendColor ), 1.00, 0.50 ),
			};

			RGradientRamp ramp( kLinear, pts, 2 );
			RGradientFill fill;
			fill.SetRamp( &ramp );

			switch (nFillType)
			{
			case FillFormatV2::FillSweepRight:
				fill.SetGradientAngle( 0.0 );
				fill.SetType( kLinear );
				break;

			case FillFormatV2::FillSweepDown:
				fill.SetGradientAngle( -kPI / 2 );
				fill.SetType( kLinear );
				break;

			case FillFormatV2::FillRadialCenter:
				fill.SetGradientAngle( 0.0 );
				fill.SetType( kRadial );
				break;

			case FillFormatV2::FillRadialCorner:
				fill.SetGradientAngle( -kPI / 4 );
				fill.SetType( kLinear );
				break;
			}

			m_crFill = RColor( fill );
		}
	}
}

BOOL CObjectProperties::ApplyFillType(SHORT& nFillType)
{
	if (((m_wPropertySet & FLAG_FillType) != 0) && ((m_wPropertyUndefined & FLAG_FillType) == 0))
	{
		nFillType = m_Fill.m_nFillType;
		return TRUE;
	}
	return FALSE;
}

/*
// Set the warp field. We bump the reference count if we keep it.
*/

void CObjectProperties::SetWarpField(DB_RECORD_NUMBER lWarpField, BOOL fMerge /*=TRUE*/)
{
   // Is the property already set?
   if (fMerge && (m_wPropertySet & FLAG_WarpField))
   {
      // It is set. Is it the same?
      if (m_lWarpField != lWarpField)
      {
         // Not the same. Set as undefined (multiply-defined).
         m_wPropertyUndefined |= FLAG_WarpField;
      }
   }
   else
   {
      // Not set yet. Just set it.
      ASSERT(m_pDatabase != NULL);
      if (m_pDatabase != NULL)
      {
         // Get rid of any existing warp field reference.
         FreeWarpField();
         // Add a reference for this if non-zero.
         if (lWarpField != 0)
         {
            m_pDatabase->inc_warp_field_record(lWarpField);
         }

         m_wPropertySet |= FLAG_WarpField;
         m_wPropertyUndefined &= ~FLAG_WarpField;
         m_lWarpField = lWarpField;
      }
   }
}

/*
// Apply the warp field to a variable.
// Note that the reference count is NOT changed for the warp field. It is
// the responsibility of the caller to inc the reference count if the field is
// kept.
*/

BOOL CObjectProperties::ApplyWarpField(DB_RECORD_NUMBER& lWarpField)
{
   // We can apply the property if set and not undefined (multiply-defined).
   if ((m_wPropertySet & FLAG_WarpField) && !(m_wPropertyUndefined & FLAG_WarpField))
   {
      // Return the value.
      lWarpField = m_lWarpField;
      return TRUE;
   }
   // Leave as what came in.
   return FALSE;
}
// This should compliment CObjectProperties::IsInvisible()
void
CObjectProperties::MakeInvisible(void)
{
   SetFillForegroundColor(TRANSPARENT_COLOR, FALSE);
   SetFillGradientBlendColor(TRANSPARENT_COLOR, FALSE);
   SetFillType(FillFormatV2::FillNone);

   SetOutlineStyle(OutlineFormat::None, FALSE);
   SetOutlinePattern(FillFormatV1::None, FALSE);
   SetOutlineForegroundColor(TRANSPARENT_COLOR, FALSE);
   
   SetShadowStyle(ShadowFormat::None, FALSE);
   SetShadowPattern(FillFormatV1::None, FALSE);
   SetShadowForegroundColor(TRANSPARENT_COLOR, FALSE);
}

BOOL
CObjectProperties::IsInvisible(void)
{
   if (m_Fill.IsVisible() == TRUE)
         return FALSE;

   if (m_Outline.IsVisible() == TRUE)
         return FALSE;

   if (m_Shadow.IsVisible() == TRUE)
         return FALSE;

   return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CObjectStateArray

CObjectStateArray::~CObjectStateArray()
{
// Delete all states.
	DeleteAll();
}

void CObjectStateArray::DeleteAll(void)
{
	int nCount = GetSize();

	for (int nIndex = 0; nIndex < nCount; nIndex++)
	{
		delete GetAt(nIndex);
	}
	RemoveAll();
}

/////////////////////////////////////////////////////////////////////////////
// PMGPageObject methods

/*
// The constructor for a default PMG page object.
*/

PMGPageObject::PMGPageObject(DB_OBJECT_TYPE type, ObjectDatabasePtr owner)
         : PageObject(type, owner),
           next_selected(NULL),
           refresh_flags(0),
           object_image(NULL),
           container(NULL),
           parent(NULL),
           m_fOnPage(TRUE)          // Assume as such.
{
// Initialize the object here.
   memset(&record, 0, sizeof(record));
   record.primary_action = ACTION_SELECT;
   record.secondary_action = ACTION_DEFAULT_EDIT;
   record.m_lPage = ((PMGDatabase*)owner)->CurrentPage();

   // Visible rect starts out (0.0, 0.0, 1.0, 1.0)
   record.m_VisibleRect.right =
      record.m_VisibleRect.bottom = 1.0;
#if 0
   record.m_VisibleRect.left = 0.25;
   record.m_VisibleRect.top = 0.1;
   record.m_VisibleRect.right = 0.75;
   record.m_VisibleRect.bottom = 0.8;
#endif
}

/*
// The base destructor for a PMG page object.
*/

PMGPageObject::~PMGPageObject()
{
   free_image();
}

/*
// The read method for a PMG page object.
// This will read the page object header, then call the object's virtual
// method to read any other object-specific data.
*/

ERRORCODE PMGPageObject::read(StorageDevicePtr device)
{
   ERRORCODE error;

   if ((error = device->read_record(&record, sizeof(record))) == ERRORCODE_None)
   {
      if ((error = read_data(device)) == ERRORCODE_None)
      {
         // Fixup the visible rect.
         if (record.m_VisibleRect.left == 0.0
             && record.m_VisibleRect.top == 0.0
             && record.m_VisibleRect.right == 0.0
             && record.m_VisibleRect.bottom == 0.0)
         {
            // Set to (0.0, 0.0, 1.0, 1.0)
            record.m_VisibleRect.right =
               record.m_VisibleRect.bottom = 1.0;
         }

         // Fixup the page.
         if (Page() == 0)
         {
            ST_DEV_POSITION Pos;
            device->tell(&Pos);
            Page(((PMGDatabase*)database)->CurrentPage());
            device->seek(Pos, ST_DEV_SEEK_SET);
         }
         else
         {
            // This assertion fails when frames are loaded during text
            // flow spanning multiple pages when the page has not been
            // loaded yet (since the page that is being loaded is not
            // the "current page").
//          ASSERT(((PMGDatabase*)database)->CurrentPage() == 0
//                   || Page() == ((PMGDatabase*)database)->CurrentPage());
         }
      }
   }
   return error;
}

/*
// The write method for a PMG page object.
// This will write the page object header, then call the object's virtual
// method to write out any other object-specific data.
*/

ERRORCODE PMGPageObject::write(StorageDevicePtr device)
{
   ERRORCODE error;

   if ((error = device->write_record(&record, sizeof(record))) == ERRORCODE_None)
   {
      error = write_data(device);
   }
   return error;
}

/*
// The size method for a PMG page object.
// This will compute the size of the page object, then add the size of any
// object-specific data to it (by calling the size_data() virtual method)
*/

ST_MAN_SIZE PMGPageObject::size(StorageDevicePtr device)
{
   return device->size_record(sizeof(record)) + size_data(device);
}

/************************************/
/* Generally useful object methods. */
/************************************/

void PMGPageObject::xflip(BOOL fChangeAngle /*=TRUE*/)
{
   record.flags ^= OBJECT_FLAG_xflipped;

   // Reverse the visible rect.
   double d = 1.0 - record.m_VisibleRect.left;
   record.m_VisibleRect.left = 1.0 - record.m_VisibleRect.right;
   record.m_VisibleRect.right = d;

   if (fChangeAngle)
   {
      if (!(record.flags & OBJECT_FLAG_grouped))
      {
         if (record.rotc.rotation != 0.0)
         {
            record.rotc.rotation = normalize_angle(-record.rotc.rotation);

         /* Compute bound and vertices. */

            calc();
         }
      }
   }

   changed_image();
}

void PMGPageObject::yflip(BOOL fChangeAngle /*=TRUE*/)
{
   record.flags ^= OBJECT_FLAG_yflipped;

   // Reverse the visible rect.
   double d = 1.0 - record.m_VisibleRect.top;
   record.m_VisibleRect.top = 1.0 - record.m_VisibleRect.bottom;
   record.m_VisibleRect.bottom = d;

   if (fChangeAngle)
   {
      if (!(record.flags & OBJECT_FLAG_grouped))
      {
         if (record.rotc.rotation != 0.0)
         {
            record.rotc.rotation = normalize_angle(-record.rotc.rotation);

         /* Compute bound and vertices. */

            calc();
         }
      }
   }
   changed_image();
}

/*
// Set the page for this object.
*/

void PMGPageObject::Page(DB_RECORD_NUMBER lPage)
{
   record.m_lPage = lPage;
}

// Get the object's time stamp.
CTimeStamp PMGPageObject::GetTimeStamp(void)
{
   // No time stamp by default.
	CTimeStamp TimeStamp;
	TimeStamp.Invalidate();
	return TimeStamp;
}

/*
// The default primary action.
*/

PMG_OBJECT_ACTION PMGPageObject::primary_action(LPVOID far *data)
{
   return record.primary_action;
}

/*
// The default secondary action.
*/

PMG_OBJECT_ACTION PMGPageObject::secondary_action(LPVOID far *data)
{
   return record.secondary_action;
}

/*
// Destroy an object.
// If the object has a layout, decrement its reference count.
*/

VOID PMGPageObject::destroy(VOID)
{
   if (record.layout > 0)
   {
   /* Decrement the layout reference. */
      ((PMGDatabasePtr)database)->free_layout_record(record.layout);
   }

   if(record.m_HyperlinkRecord != 0)
   {
      PMGDatabasePtr pDatabase = (PMGDatabase*)database;
      pDatabase->FreeHyperlinkRecord(record.m_HyperlinkRecord);
   }
}

/*
// Set a new object layout, freeing an old one.
*/

VOID PMGPageObject::set_new_layout(DB_RECORD_NUMBER l)
{
   if (record.layout != l)
   {
      if (record.layout > 0)
      {
         get_database()->free_layout_record(record.layout);
      }
      record.layout = l;
   }
}

/*
// Assign an object to 'this'.
*/

ERRORCODE PMGPageObject::assign(PageObjectRef pobject)
{
   ERRORCODE error;
   PMGPageObject& sobject = (PMGPageObject&)pobject;

   if ((error = PageObject::assign(pobject)) == ERRORCODE_None)
   {
      DB_RECORD_NUMBER lPage = record.m_lPage;
      record = sobject.record;
      record.m_lPage = lPage;
   }

/*
// Duplicate the records.
// Extreme care must be taken to maintain the correct database at all times.
*/

   DB_RECORD_NUMBER layout;

   if ((layout = get_layout()) > 0)
   {
      ObjectLayoutPtr layrec;

      if ((layrec = (ObjectLayoutPtr)sobject.database->get_record(layout,
                           &error, RECORD_TYPE_ObjectLayout)) != NULL)
      {
         DB_RECORD_NUMBER new_layout;

      /* Duplicate the layout into our database. */

         new_layout = get_database()->new_layout_record((LPSTR)layrec->get_name(),
                     layrec->get_type(),
                     layrec->get_elements());

      /* Release the old one. */

         layrec->release();

      /* Set the new one. */

         set_layout(new_layout);
      }
   }

   record.m_HyperlinkRecord = 0;
   HyperlinkData  HData;
   sobject.get_database()->GetHyperlinkData(sobject.record.m_HyperlinkRecord, HData);
   if (HData.IsValid() == TRUE)
   {
      // Calls KillHyperlinkRecord() - pDatabase->FreeHyperlinkRecord(record.m_HyperlinkRecord)
      
      SetHyperlinkRecord(&HData);   
   }

   return error;
}

VOID PMGPageObject::get_refresh_bound(PBOX_PTR refresh_bound, RedisplayContextPtr rc)
{
   *refresh_bound = get_bound();
}

BOOL PMGPageObject::GetClipRgn(CRgn& Rgn, RedisplayContextPtr rc, int nType /*=CLIP_TYPE_Boundary*/)
{
   ASSERT(nType == CLIP_TYPE_Boundary);

   // Get the object bounds.
   PBOX Bounds = get_bound();
   rc->pbox_to_screen(&Bounds, FALSE);

   CRect crRgnBounds;
   rc->convert_pbox(&Bounds, crRgnBounds, NULL);

   return Rgn.CreateRectRgnIndirect(crRgnBounds);
}

BOOL PMGPageObject::ApplyClipRgn(RedisplayContext* rc, int nType /*=CLIP_TYPE_Boundary*/)
{
   BOOL fSuccess = FALSE;
   CRgn Rgn;
   if (!rc->is_metafile && GetClipRgn(Rgn, rc, nType))
   {
      HRGN hClip = (HRGN)(Rgn.Detach());
      ASSERT(hClip != NULL);
      ASSERT(Rgn.GetSafeHandle() == NULL);

      if (rc->clip_region != NULL)
      {
         int nResult = CombineRgn(hClip, hClip, rc->clip_region, RGN_AND);
         switch (nResult)
         {
            case ERROR:
            {
               // Some sort of problem. Restore.
               TRACE("Region error!\n");
					DeleteObject(hClip);
               return -1;
            }
            default:
            {
               break;
            }
         }
      }

      SaveDC(rc->destination_hdc);
      switch (SelectClipRgn(rc->destination_hdc, hClip))  // **** Selects a copy! ****
      {
         case NULLREGION:
         {
            // Some sort of problem. Restore.
            RestoreDC(rc->destination_hdc, -1);
            fSuccess = -1;
            break;
         }
         case ERROR:
         {
            // Some sort of problem. Restore.
            RestoreDC(rc->destination_hdc, -1);
            fSuccess = -1;
            break;
         }
         default:
         {
            fSuccess = TRUE;
            break;
         }
      }

      DeleteObject(hClip);
   }
   return fSuccess;
}

void PMGPageObject::RestoreClipRgn(RedisplayContext* rc, BOOL f)
{
   if (f == TRUE)
   {
      RestoreDC(rc->destination_hdc, -1);
   }
}

PBOX PMGPageObject::GetDrawBound(void)
{
   return ComputeDrawBound(get_bound());
}

PBOX PMGPageObject::ComputeDrawBound(PBOX Bound, CDoubleRect* pRect /*=NULL*/)
{
   if (pRect == NULL)
   {
      pRect = &record.m_VisibleRect;
   }

   // Compute the fraction of the object visible (may be > 1.0)
   double dVisibleWidth = pRect->Width();
   double dVisibleHeight = pRect->Height();

   // Compute the actual drawing width.
   double dDX = (double)(Bound.x1 - Bound.x0);
   double dDY = (double)(Bound.y1 - Bound.y0);

   dDX /= dVisibleWidth;
   dDY /= dVisibleHeight;

   // Build the draw bounds.
   PBOX DrawBound;
   DrawBound.x0 = Bound.x0 - (PCOORD)(pRect->left*dDX);
   DrawBound.y0 = Bound.y0 - (PCOORD)(pRect->top*dDY);
   DrawBound.x1 = DrawBound.x0 + (PCOORD)dDX;
   DrawBound.y1 = DrawBound.y0 + (PCOORD)dDY;

   return DrawBound;
}

BOOL PMGPageObject::is_opaque(RedisplayContextPtr rc)
{
   return FALSE;
}

/*
// Return the original dimensions of an object.
// Default is to do nothing.
*/

BOOL PMGPageObject::original_dims(PPNT_PTR p)
{
   return FALSE;
}

/*
// Return the name for this object.
// Default is to do nothing.
*/

BOOL PMGPageObject::get_name(LPSTR buffer, SHORT size, SHORT max_width)
{
   return FALSE;
}

/*
// Get an object's rotation.
*/

HCURSOR PMGPageObject::GetContentCursor(void)
{
	// Default./
	return NULL;
}

// Hyperlink specific stuff

HyperlinkRecord*
PMGPageObject::GetHyperlink(void)
{
   DB_RECORD_NUMBER  rn = GetHyperlinkRecordNumber();
   if (rn == 0)
      return NULL;

   return (HyperlinkRecord*)get_database()->get_record(rn, NULL, RECORD_TYPE_Hyperlink);
}

DB_RECORD_NUMBER
PMGPageObject::SetHyperlinkRecord(HyperlinkData* p)
{
   KillHyperlinkRecord();
   PMGDatabasePtr pd = get_database();
   record.m_HyperlinkRecord = pd->NewHyperlinkRecord(p);
   return record.m_HyperlinkRecord;
}

void
PMGPageObject::KillHyperlinkRecord(void)
{
   if (record.m_HyperlinkRecord != 0)
   {
      PMGDatabasePtr pDatabase = (PMGDatabase*)database;
      pDatabase->FreeHyperlinkRecord(record.m_HyperlinkRecord);
      record.m_HyperlinkRecord = 0;
   }
} 

void
PMGPageObject::GetHyperlinkData(HyperlinkData* p)
{
   ASSERT(p != NULL);
   if (p != NULL)
   {
      p->Init();
      HyperlinkRecord* pLink = GetHyperlink();

      if (pLink != NULL)
      {
         pLink->GetData(p);
         pLink->release();
      }
   }
}

void
PMGPageObject::MakeInvisible(void)
{
   CObjectProperties Props;

   Props.MakeInvisible();

   SetObjectProperties(Props);
}

BOOL
PMGPageObject::IsInvisible(void)
{
   CObjectProperties Props;

   GetObjectProperties(Props);

   return Props.IsInvisible();
}

BOOL
PMGPageObject::IsInvisibleHotSpot(void)
{
   if (type() != OBJECT_TYPE_Hyperlink)
      return FALSE;

   return IsInvisible();
}

void PMGPageObject::CopyAttributes( PMGPageObjectPtr pObject )
{
	record = pObject->record;
	container = pObject->container;
	refresh_flags = pObject->refresh_flags;
	m_fOnPage = pObject->m_fOnPage;

   if (record.layout > 0)
      ((PMGDatabasePtr) database)->inc_layout_record(record.layout);

	if (record.m_HyperlinkRecord)
		((PMGDatabasePtr) database)->IncrementHyperlinkRecord(record.m_HyperlinkRecord);

   CObjectProperties Props;
   pObject->GetObjectProperties(Props);
	SetObjectProperties( Props );
}

/////////////////////////////////////////////////////////////////////////////
// State management for PMGPageObject.

PageObjectState* PMGPageObject::CreateState(void)
{
   return new PMGPageObjectState;
}

void PMGPageObject::SaveState(PageObjectState* pState)
{
   INHERITED::SaveState(pState);

   PMGDatabasePtr pDatabase = (PMGDatabase*)database;

   ((PMGPageObjectState*)pState)->m_pDatabase = pDatabase;
   if (record.layout > 0)
   {
   /* Increment the layout reference. */
      pDatabase->inc_layout_record(record.layout);
   }

   pDatabase->IncrementHyperlinkRecord(record.m_HyperlinkRecord);

   ((PMGPageObjectState*)pState)->m_Record = record;
}

/* Has our image changed? */

BOOL PMGPageObject::ImageChanged(PMGPageObjectState* pState)
{
   return (((pState->m_Record.flags ^ record.flags)
                  & (OBJECT_FLAG_xflipped | OBJECT_FLAG_yflipped)) != 0)
            || pState->m_Record.rotc.rotation != record.rotc.rotation
            || pState->m_Record.m_VisibleRect != record.m_VisibleRect;
}

void PMGPageObject::RestoreState(PageObjectState* pState)
{
   PMGPageObjectState* pMyState = (PMGPageObjectState*)pState;

/* See if our image changed. */

   if (ImageChanged(pMyState))
   {
      changed_image();
   }

/* Do our parent class restore. */

   INHERITED::RestoreState(pState);

/* Handle any change in the layout. */

   if (pMyState->m_Record.layout != record.layout)
   {
      if (pMyState->m_Record.layout > 0)
      {
      /* Increment the layout reference. */
         pMyState->m_pDatabase->inc_layout_record(pMyState->m_Record.layout);
      }
      if (record.layout > 0)
      {
      /* Decrement the layout reference. */
         pMyState->m_pDatabase->free_layout_record(record.layout);
      }
   }

   if (pMyState->m_Record.m_HyperlinkRecord != record.m_HyperlinkRecord)
   {
      pMyState->m_pDatabase->IncrementHyperlinkRecord(pMyState->m_Record.m_HyperlinkRecord);
      pMyState->m_pDatabase->FreeHyperlinkRecord(record.m_HyperlinkRecord);

   }

   record = pMyState->m_Record;
}

/*
// Set whether the object is on or off the page at the moment.
*/

void PMGPageObject::OnPage(BOOL fOnPage)
{
   // O.K. We are on or off the page.
   m_fOnPage = fOnPage;
}

BOOL PMGPageObject::IsOnPage(void)
{
   return m_fOnPage;
}

/*
// Add the outline for this object to the passed array.
//
// If pClipBox is not NULL, it is assumed that an intersect test has
// already been performed with this box against the object bound.
*/

void PMGPageObject::AddOutline(COutlinePath& Path, PBOX* pClipBox /*=NULL*/)
{
   // Nothing to do at this level.
}

/*
// How does text flow around us?
*/

long PMGPageObject::GetFlowAround(void)
{
   return record.m_lFlowAround;
}

void PMGPageObject::SetFlowAround(long lFlowAround)
{
   record.m_lFlowAround = lFlowAround;
}

BOOL PMGPageObject::PtIsOpaque(PPNT p, RedisplayContextPtr rc)
{
   if (object_image == NULL || rc == NULL)
   {
      // No way to tell.
      return TRUE;
   }

   return object_image->PtIsOpaque(this, rc, p);
}

/*
// Get the properties for this object.
*/

void PMGPageObject::GetObjectProperties(CObjectProperties& Properties)
{
   // You need to define GetObjectProperties in your derived class!
   ASSERT(FALSE);
}

/*
// Set the properties for this object.
*/

BOOL PMGPageObject::SetObjectProperties(CObjectProperties& Properties)
{
   // You need to define SetObjectProperties in your derived class!
   ASSERT(FALSE);
   return FALSE;
}

/*
// We are being selected or deselected.
//
// This is mostly a notification. You should be careful if you use this to set 
// a state since the state could change without this function being called
// (e.g. if the document is saved while an object is selected).
//
// This function will be called with TRUE before the object has been toggled
// on, and it will called with FALSE before the object is toggled off.
*/

void PMGPageObject::OnSelectChange(BOOL fSelected /*=TRUE*/)
{
	// Default does nothing.
}

/////////////////////////////////////////////////////////////////////////////
// Image methods for PMGPageObjects

/*
// Begin the generation of an object image.
*/

ERRORCODE PMGPageObject::begin_image(RedisplayContextPtr rc, PBOX world)
{
   ERRORCODE error;
   CBitmapObjectImage* image;

// od("begin image...\r\n");

/* Beginning. Make sure we're clear. */

   free_image();

/* Allocate the refresh data. */

   if ((image = new CBitmapObjectImage) == NULL)
   {
      error = ERRORCODE_Memory;
   }
   else
   {
      object_image = image;

   /* Got the object. Try to begin. */

      if ((error = image->begin_image(rc, get_bound(), world)) != ERRORCODE_None)
      {
         free_image();
      }
   }
   return error;
}

/*
// End the generation of an image.
*/

ERRORCODE PMGPageObject::end_image(RedisplayContextPtr rc)
{
   ERRORCODE error;

// od("end image...\r\n");

   if (object_image == NULL)
   {
      error = ERRORCODE_NotInitialized;
   }
   else if ((error = object_image->end_image()) == ERRORCODE_None)
   {
      refresh_flags |= REFRESH_FLAG_has_image;
   }

   return error;
}

/*
// Free the object image.
*/

VOID PMGPageObject::free_image(VOID)
{
// od("free image...\r\n");

   if (object_image != NULL)
   {
      delete object_image;
      object_image = NULL;
   }
   refresh_flags &= ~REFRESH_FLAG_has_image;
}

/*
// Draw the image for this object.
*/

ERRORCODE PMGPageObject::draw_image(RedisplayContextPtr rc, PBOX extent)
{
   PBOX bound = get_bound();

   if (!IntersectBox(&bound, &extent, &bound))
   {
//    od("bound %ld, %ld, %ld, %ld not in extent %ld, %ld, %ld %ld\r\n",
//             bound, extent);
      return ERRORCODE_None;
   }

   if (object_image != NULL)
   {
      return object_image->draw_image(this, rc, bound);
   }

/* We shouldn't be here! */

// od("Not initialized!\r\n");

   free_image();
   return ERRORCODE_NotInitialized;
}

/*
// The image has changed.
*/

VOID PMGPageObject::changed_image(VOID)
{
/* Standard action - free the image. */
// od("Changed image!\r\n");

   free_image();
}

/*
// Are we currently making an image?
*/

BOOL PMGPageObject::making_image(VOID)
{
   return object_image != NULL && object_image->making_image();
}

////////////////////////////////////////////////////////////////////////////
// PMGPageObjectState

PMGPageObjectState::~PMGPageObjectState()
{
   if (m_Record.layout > 0)
   {
   /* Increment the layout reference. */
      m_pDatabase->free_layout_record(m_Record.layout);
   }

   m_pDatabase->FreeHyperlinkRecord(m_Record.m_HyperlinkRecord);
}

/*****************************************************************************/
/*                        Rectangular object methods                         */
/*****************************************************************************/

/*
// Constructor for a rectangular page object.
*/

RectPageObject::RectPageObject(DB_OBJECT_TYPE type, ObjectDatabasePtr owner)
            : PMGPageObject(type, owner)
{
   memset(&rrecord, 0, sizeof(rrecord));
}

/*
// Returns the point (in page coordinates) corresponding to the handle
// specified.
// This assume the object is rotated and that handles[] is valid.
*/

VOID RectPageObject::get_handle_point(OBJECT_HANDLE handle, PPNT_PTR pnt)
{
   static SHORT handle_defs[][2] =
   {
      { 0, 0 },         /* OBJECT_HANDLE_UL */
      { 0, 1 },         /* OBJECT_HANDLE_UPPER */
      { 1, 1 },         /* OBJECT_HANDLE_UR */
      { 3, 0 },         /* OBJECT_HANDLE_LEFT */
      { 0, 2 },         /* OBJECT_HANDLE_MOVE */
      { 1, 2 },         /* OBJECT_HANDLE_RIGHT */
      { 3, 3 },         /* OBJECT_HANDLE_LL */
      { 2, 3 },         /* OBJECT_HANDLE_LOWER */
      { 2, 2 },         /* OBJECT_HANDLE_LR */
      { 4, 4 }          /* OBJECT_HANDLE_ROTATE */
   };

   SHORT i1 = handle_defs[handle][0];
   SHORT i2 = handle_defs[handle][1];

   pnt->x = handles[i1].x;
   pnt->y = handles[i1].y;

   if (i1 != i2)     // Skip redundancy of same second point
   {
      pnt->x = (pnt->x + handles[i2].x)/2;
      pnt->y = (pnt->y + handles[i2].y)/2;
   }
}

VOID RectPageObject::move_to_anchor(OBJECT_HANDLE handle, PPNT old_pnt)
{
   PPNT new_pnt;

   get_anchor_point(handle, &new_pnt);

   PCOORD dx = old_pnt.x - new_pnt.x;
   PCOORD dy = old_pnt.y - new_pnt.y;

   if (dx != 0)
   {
      record.rotc.origin.x += dx;
      rrecord.unrotated_bound.x0 += dx;
      rrecord.unrotated_bound.x1 += dx;
   }

   if (dy != 0)
   {
      record.rotc.origin.y += dy;
      rrecord.unrotated_bound.y0 += dy;
      rrecord.unrotated_bound.y1 += dy;
   }
}

BOOL RectPageObject::toggle_selects(RedisplayContextPtr rc, FLAGS which)
{
   if (which == 0)
   {
      which = record.select_flags;
   }

   if (get_flags() & OBJECT_FLAG_grouped)
   {
      which &= ~(SELECT_FLAG_size_handles|SELECT_FLAG_move_handle|SELECT_FLAG_rotate_handle);
   }

   if ((which & SELECT_FLAG_boundary)
         && (get_refresh_flags() & (REFRESH_FLAG_cropping|REFRESH_FLAG_panning)))
   {
      which |= SELECT_FLAG_draw_boundary;
   }

   // General-use variables.
   HDC hdc = rc->destination_hdc;
   ANGLE dAngle = get_rotation();
   POINT p[4];
   PPNT v[4];

   // If we want to toggle the draw bound, do it now.
   // This flag is never set in the select_flags; it is an internal flag.
   if (which & SELECT_FLAG_draw_boundary)
   {
      // Draw the draw boundary.
      PBOX UnrotatedBound = GetUnrotatedDrawBound();
		if (UnrotatedBound.x0 <= UnrotatedBound.x1)
			UnrotatedBound.x0 -= rc->redisplay_x_pixel;
		else
			UnrotatedBound.x1 -= rc->redisplay_x_pixel;
		if (UnrotatedBound.y0 <= UnrotatedBound.y1)
			UnrotatedBound.y0 -= rc->redisplay_y_pixel;
		else
			UnrotatedBound.y1 -= rc->redisplay_y_pixel;

      PBOX RotatedBound;
      compute_rotated_bound(UnrotatedBound,
                   get_rotation(),
                   &RotatedBound,
                   get_origin(),
                   v);

      for (int i = 0; i < 4; i++)
      {
         p[i].x = rc->page_x_to_screen(v[i].x);
         p[i].y = rc->page_y_to_screen(v[i].y);
      }

//    HPEN hPen = ::CreatePen(PS_SOLID, 1, RGB(192, 192, 192));
      HPEN hPen = ::CreatePen(PS_DOT, 1, SELECT_COLOR);

      if (hPen != NULL)
      {
         HPEN hOldPen = (HPEN)SelectObject(hdc, hPen);
         int oldROP2 = SetROP2(hdc, R2_XORPEN);
         int oldBkMode = SetBkMode(hdc, TRANSPARENT);

         // Move to first point.
         ::MoveToEx(hdc, p[0].x, p[0].y, NULL);

         // Line to rest of points.
         ::LineTo(hdc, p[1].x, p[1].y);
         ::LineTo(hdc, p[2].x, p[2].y);
         ::LineTo(hdc, p[3].x, p[3].y);
         ::LineTo(hdc, p[0].x, p[0].y);

         SetROP2(hdc, oldROP2);
         SetBkMode(hdc, oldBkMode);
         SelectObject(hdc, hOldPen);
         DeleteObject(hPen);
      }
   }

   if ((which &= record.select_flags) != 0)
   {
   /* Need to toggle something. */

      SHORT i;

      // Compute screen coordinates for the four handles.

      PBOX UnrotatedBound = get_unrotated_bound();
		if (UnrotatedBound.x0 <= UnrotatedBound.x1)
			UnrotatedBound.x0 -= rc->redisplay_x_pixel;
		else
			UnrotatedBound.x1 -= rc->redisplay_x_pixel;

		if (UnrotatedBound.y0 <= UnrotatedBound.y1)
			UnrotatedBound.y0 -= rc->redisplay_y_pixel;
		else
			UnrotatedBound.y1 -= rc->redisplay_y_pixel;

      PBOX RotatedBound;
      compute_rotated_bound(UnrotatedBound,
                   get_rotation(),
                   &RotatedBound,
                   get_origin(),
                   v);

      for (i = 0; i < 4; i++)
      {
         p[i].x = rc->page_x_to_screen(v[i].x);
         p[i].y = rc->page_y_to_screen(v[i].y);
      }

   /* Draw a rotated bounding box. */

      if (which & SELECT_FLAG_size_handles)
      {
         POINT p0 = p[0];

         for (i = 1; i <= 4; i++)
         {
            ControlDot(hdc, p0.x, p0.y, dAngle, p0.x, p0.y);
            POINT p1 = p[i == 4 ? 0 : i];

            POINT m;
            m.x = midpoint(p0.x, p1.x);
            m.y = midpoint(p0.y, p1.y);
            ControlDot(hdc,
                        m.x, m.y,
                        dAngle,
                        m.x, m.y);
            p0 = p1;
         }
      }

      if (which & SELECT_FLAG_boundary)
      {
         // If we are moving/resizing the object and the position is not the
         // start position, then we want to toggle.

         if ((get_refresh_flags() & (REFRESH_FLAG_cropping|REFRESH_FLAG_panning))
             || ((get_refresh_flags() & (REFRESH_FLAG_moving|REFRESH_FLAG_sizing|REFRESH_FLAG_cropping|REFRESH_FLAG_panning))
             && (memcmp(&bound_save, &rrecord.unrotated_bound, sizeof(PBOX)) != 0
                 || angle_save != get_rotation()
                 || m_VisibleRectSave != record.m_VisibleRect)))
         {
            HPEN hPen = CreateSelectPen();

            if (hPen != NULL)
            {
               HPEN hOldPen = (HPEN)SelectObject(hdc, hPen);
               int oldROP2 = SetROP2(hdc, R2_XORPEN);
               int oldBkMode = SetBkMode(hdc, TRANSPARENT);

               ToggleBounds(rc, p);

               SetROP2(hdc, oldROP2);
               SetBkMode(hdc, oldBkMode);
               SelectObject(hdc, hOldPen);
               DeleteObject(hPen);
            }
         }
      }

      if (which & SELECT_FLAG_rotate_handle)
      {
         POINT draw_p[2];
         if (rrecord.unrotated_bound.x1 < rrecord.unrotated_bound.x0)
         {
            draw_p[0].x = midpoint(p[0].x, p[3].x);
            draw_p[0].y = midpoint(p[0].y, p[3].y);
         }
         else
         {
            draw_p[0].x = midpoint(p[1].x, p[2].x);
            draw_p[0].y = midpoint(p[1].y, p[2].y);
         }
         draw_p[1].x = draw_p[0].x + (int)handles[4].x;
         draw_p[1].y = draw_p[0].y + (int)handles[4].y;

         xor_polyline(hdc, draw_p, 2, FALSE);
         ControlDot(hdc, draw_p[1].x, draw_p[1].y, dAngle, draw_p[1].x, draw_p[1].y);
      }
   }
   return TRUE;
}

void RectPageObject::ToggleBounds(RedisplayContext* rc, POINT* p)
{
   HDC hdc = rc->destination_hdc;

   // Move to first point.
   ::MoveToEx(hdc, p[0].x, p[0].y, NULL);

   // Line to rest of points.
   ::LineTo(hdc, p[1].x, p[1].y);
   ::LineTo(hdc, p[2].x, p[2].y);
   ::LineTo(hdc, p[3].x, p[3].y);
   ::LineTo(hdc, p[0].x, p[0].y);
}

HPEN RectPageObject::CreateSelectPen(void)
{
   return ::CreatePen(PS_SOLID, 1, SELECT_COLOR);
// return ::CreatePen(PS_SOLID, 1, RGB(255, 255, 255));
}

BOOL RectPageObject::pt_in_object(PPNT p, RedisplayContextPtr rc, PMGPageObjectPtr far *sub_object)
{
   if (sub_object != NULL)
   {
      *sub_object = this;
   }

   BOOL fInBound;
   if (record.rotc.rotation != 0.0)
   {
   /*
   // We have a rotated bound.
   // Rotate the point into our rotated space. Then compare to our
   // unrotated bound.
   */
      PPNT rotp = p;
      rotate_points(&rotp, 1, record.rotc.origin, -record.rotc.rotation);
      fInBound = IsPtInBox(rotp.x, rotp.y, &rrecord.unrotated_bound);
   }
   else
   {
      fInBound = IsPtInBox(p.x, p.y, &rrecord.bound);
   }

   // We must be within the bound and on a solid part of the object.
   return fInBound && PtIsOpaque(p, rc);
}

VOID RectPageObject::turn_select_off(FLAGS which)
{
   if ((which &= (select_flags_save = record.select_flags)) != 0)
   {
      get_database()->toggle_object(this, which);
      record.select_flags ^= which;
   }
}

VOID RectPageObject::restore_select(VOID)
{
   FLAGS which = select_flags_save ^ record.select_flags;

   if (which != 0)
   {
      record.select_flags ^= which;
      get_database()->toggle_object(this, which);
   }
}

/*
// Return whether a point is over an object's move area or not.
// By default, the content area of an object *is* the move area, so the
// default return value is FALSE here.
*/

BOOL RectPageObject::pt_on_move_area(PPNT p, RedisplayContextPtr rc)
{
	return pt_in_object(p, rc, NULL);
}

PMG_OBJECT_ACTION RectPageObject::move_object(DRAG_STATE state, PPNT vector, PPNT current_xy, LPVOID far *data /*=NULL*/, BOOL fPanning /*=FALSE*/)
{
   if (fPanning)
   {
      // Panning case.
      switch (state)
      {
         case DRAG_STATE_begin:
         {
            bound_save = rrecord.unrotated_bound;
            m_OriginalDrawBound = GetUnrotatedDrawBound();
            m_VisibleRectSave = record.m_VisibleRect;

            // Turn off sizing handles.
            turn_select_off(SELECT_FLAG_size_handles | SELECT_FLAG_rotate_handle | SELECT_FLAG_subselect);
            add_refresh_flags(REFRESH_FLAG_panning);

            // Toggle the boundary just for symmetry with DRAG_STATE_end.
            // This has no real effect for the current code (since the code only
            // does the toggle when the object is different from initial conditions).

            get_database()->toggle_object(this, SELECT_FLAG_boundary);
            break;
         }
         case DRAG_STATE_continue:
         {
            get_database()->toggle_object(this, 0);

            // Add this vector onto the original drawing bound.
            // Since the vector is based on the bound (which is not moving),
            // it is always absolute.

            if (record.rotc.rotation != 0.0)
            {
               PPNT Origin;
               Origin.x = Origin.y = 0;
               rotate_points(&vector, 1, Origin, -record.rotc.rotation);
            }

            PBOX DrawBound = m_OriginalDrawBound;
            DrawBound.x0 += vector.x;
            DrawBound.y0 += vector.y;
            DrawBound.x1 += vector.x;
            DrawBound.y1 += vector.y;

            PBOX Bound = get_unrotated_bound();
            // Clip to the unrotated bound.
            if (DrawBound.x0 > Bound.x1)
            {
               DrawBound.x1 += (Bound.x1 - DrawBound.x0);
               DrawBound.x0 = Bound.x1;
            }
            if (DrawBound.y0 > Bound.y1)
            {
               DrawBound.y1 += (Bound.y1 - DrawBound.y0);
               DrawBound.y0 = Bound.y1;
            }
            if (DrawBound.x1 < Bound.x0)
            {
               DrawBound.x0 += (Bound.x0 - DrawBound.x1);
               DrawBound.x1 = Bound.x0;
            }
            if (DrawBound.y1 < Bound.y0)
            {
               DrawBound.y0 += (Bound.y0 - DrawBound.y1);
               DrawBound.y1 = Bound.y0;
            }

            // Compute the visible rect from this new draw bound.
            double dDrawWidth = double(DrawBound.x1-DrawBound.x0);
            double dDrawHeight = double(DrawBound.y1-DrawBound.y0);

            record.m_VisibleRect.left = double(Bound.x0 - DrawBound.x0)/dDrawWidth;
            record.m_VisibleRect.top = double(Bound.y0 - DrawBound.y0)/dDrawHeight;
            record.m_VisibleRect.right = double(Bound.x1 - DrawBound.x0)/dDrawWidth;
            record.m_VisibleRect.bottom = double(Bound.y1 - DrawBound.y0)/dDrawHeight;

            get_database()->toggle_object(this, 0);
            break;
         }
         case DRAG_STATE_abort:
         {
            get_database()->toggle_object(this, 0);
            record.m_VisibleRect = m_VisibleRectSave;

//          // Compute bound and vertices from this.
//          calc();

            get_database()->toggle_object(this, 0);
            // Fall through to...
         }
         case DRAG_STATE_end:
         {
            // Toggle the boundary off.
            // Most likely, the user has changed the object somehow so
            // that the boundary is now visible. We want it to end up off.
            // If the object has not been moved, this has no effect.

            get_database()->toggle_object(this, SELECT_FLAG_boundary);

            // Restore handles.
            remove_refresh_flags(REFRESH_FLAG_panning);
            restore_select();
            break;
         }
      }
   }
   else
   {
      // Non-panning case.
      switch (state)
      {
         case DRAG_STATE_begin:
         {
            bound_save = rrecord.unrotated_bound;
            angle_save = get_rotation();
            m_VisibleRectSave = record.m_VisibleRect;

            // Turn off sizing handles.

            turn_select_off(SELECT_FLAG_size_handles | SELECT_FLAG_rotate_handle | SELECT_FLAG_subselect);
            add_refresh_flags(REFRESH_FLAG_moving);
            
            // Toggle the boundary just for symmetry with DRAG_STATE_end.
            // This has no real effect for the current code (since the code only
            // does the toggle when the object is different from initial conditions).

            get_database()->toggle_object(this, SELECT_FLAG_boundary);

            break;
         }
         case DRAG_STATE_continue:
         {
            get_database()->toggle_object(this, 0);

            rrecord.unrotated_bound.x0 += vector.x;
            rrecord.unrotated_bound.y0 += vector.y;
            rrecord.unrotated_bound.x1 += vector.x;
            rrecord.unrotated_bound.y1 += vector.y;

            record.flags |= OBJECT_FLAG_needs_calc;

         /* Compute bound and vertices. */

            compute_handles();

            get_database()->toggle_object(this, 0);

            break;
         }
         case DRAG_STATE_abort:
         {
            get_database()->toggle_object(this, 0);
            rrecord.unrotated_bound = bound_save;
            set_rotation(angle_save);

         /* Compute bound and vertices from this. */

            calc();

            get_database()->toggle_object(this, 0);
         /* Fall through to... */
         }
         case DRAG_STATE_end:
         {
            // Toggle the boundary off.
            // Most likely, the user has changed the object somehow so
            // that the boundary is now visible. We want it to end up off.
            // If the object has not been moved, this has no effect.

            get_database()->toggle_object(this, SELECT_FLAG_boundary);

            // Restore handles.

            remove_refresh_flags(REFRESH_FLAG_moving);
            restore_select();
            break;
         }
      }
   }
   return ACTION_DO_NOTHING;
}

OBJECT_HANDLE RectPageObject::box_on_handle(PBOX_PTR box, RedisplayContextPtr rc)
{
   if (record.rotc.rotation == 0)
   {
   /* Compute middle locations. */

      PCOORD mid_x = (rrecord.unrotated_bound.x0 + rrecord.unrotated_bound.x1) / 2L;
      PCOORD mid_y = (rrecord.unrotated_bound.y0 + rrecord.unrotated_bound.y1) / 2L;

   /*
   // Check the size handle.
   */

      if (record.select_flags & SELECT_FLAG_size_handles)
      {
         if (IsPtInBox(rrecord.unrotated_bound.x0, rrecord.unrotated_bound.y0, box))
            return OBJECT_HANDLE_UL;

         if (IsPtInBox(rrecord.unrotated_bound.x1, rrecord.unrotated_bound.y0, box))
            return OBJECT_HANDLE_UR;

         if (IsPtInBox(rrecord.unrotated_bound.x1, rrecord.unrotated_bound.y1, box))
            return OBJECT_HANDLE_LR;

         if (IsPtInBox(rrecord.unrotated_bound.x0, rrecord.unrotated_bound.y1, box))
            return OBJECT_HANDLE_LL;

         if (IsPtInBox(mid_x, rrecord.unrotated_bound.y0, box))
            return OBJECT_HANDLE_UPPER;

         if (IsPtInBox(rrecord.unrotated_bound.x1, mid_y, box))
            return OBJECT_HANDLE_RIGHT;

         if (IsPtInBox(mid_x, rrecord.unrotated_bound.y1, box))
            return OBJECT_HANDLE_LOWER;

         if (IsPtInBox(rrecord.unrotated_bound.x0, mid_y, box))
            return OBJECT_HANDLE_LEFT;
      }

   /*
   // Check the rotate handle.
   */

      if (record.select_flags & SELECT_FLAG_rotate_handle)
      {
         SHORT sx = rc->page_x_to_screen(rrecord.unrotated_bound.x1);
         sx += ROTATE_HANDLE_OFFSET;
         PCOORD rot_x = rc->screen_x_to_page(sx);

         if (IsPtInBox(rot_x, mid_y, box))
            return OBJECT_HANDLE_ROTATE;
      }
   }
   else
   {
   /* Check rotated corner handles. */

      if (IsPtInBox(handles[0].x, handles[0].y, box))
      {
         return OBJECT_HANDLE_UL;
      }

      if (IsPtInBox(handles[1].x, handles[1].y, box))
      {
         return OBJECT_HANDLE_UR;
      }

      if (IsPtInBox(handles[2].x, handles[2].y, box))
      {
         return OBJECT_HANDLE_LR;
      }

      if (IsPtInBox(handles[3].x, handles[3].y, box))
      {
         return OBJECT_HANDLE_LL;
      }

   /*
   // Check center handles.
   */

      PPNT mid;

      mid.x = (handles[0].x + handles[1].x)/2;
      mid.y = (handles[0].y + handles[1].y)/2;
      if (IsPtInBox(mid.x, mid.y, box))
      {
         return OBJECT_HANDLE_UPPER;
      }

      PPNT right_mid;
      right_mid.x = (handles[1].x + handles[2].x)/2;
      right_mid.y = (handles[1].y + handles[2].y)/2;
      if (IsPtInBox(right_mid.x, right_mid.y, box))
      {
         return OBJECT_HANDLE_RIGHT;
      }

      mid.x = (handles[2].x + handles[3].x)/2;
      mid.y = (handles[2].y + handles[3].y)/2;
      if (IsPtInBox(mid.x, mid.y, box))
      {
         return OBJECT_HANDLE_LOWER;
      }

      mid.x = (handles[3].x + handles[0].x)/2;
      mid.y = (handles[3].y + handles[0].y)/2;
      if (IsPtInBox(mid.x, mid.y, box))
      {
         return OBJECT_HANDLE_LEFT;
      }

   /* Check rotate handle. */
      
      if (record.select_flags & SELECT_FLAG_rotate_handle)
      {
         POINT p;

         rc->ppnt_to_screen(right_mid, &p);
         p.x += (int)handles[4].x;
         p.y += (int)handles[4].y;
         rc->screen_to_ppnt(p, &mid);

         if (IsPtInBox(mid.x, mid.y, box))
         {
            return OBJECT_HANDLE_ROTATE;
         }
      }
   }

   return OBJECT_HANDLE_NONE;
}

VOID RectPageObject::calc_bounds(PCOORD_PTR the_corner_x, PCOORD_PTR the_corner_y, PCOORD_PTR opp_corner_x, PCOORD_PTR opp_corner_y, PPNT pos, FLAGS key)
{
   PCOORD new_x, new_y;
   PCOORD prop_x, prop_y;

   if (SPECIAL_KEY(key))
   {
      PCOORD dX, dY, pro_height, pro_width;

      if (SHIFT_KEY(key))
      {
      /* Use physical proportions. */
         prop_x = real_dim.x;
         prop_y = real_dim.y;
      }
      else     /* CTRL_KEY */
      {
      /* Use original proportions. */
         prop_x = prop_dim.x;
         prop_y = prop_dim.y;
      }

      if (prop_x == 0)
      {
         pos.x = *opp_corner_x;
      }
      else if (prop_y == 0)
      {
         pos.y = *opp_corner_y;
      }
      else
      {
         dX = DELTA( *opp_corner_x, pos.x );
         dY = DELTA( *opp_corner_y, pos.y );
         pro_height = scale_pcoord(dX, prop_y, prop_x);
         pro_width  = scale_pcoord(dY, prop_x, prop_y);

         if (pro_height < dY)
         {
            new_x = *opp_corner_x + (( pos.x > *opp_corner_x ) ? pro_width : -pro_width );
   #if 0
            if (((new_x < MIN_COORDINATE) && (pos.x = MIN_COORDINATE)) ||
                  ((new_x > MAX_COORDINATE) && (pos.x = MAX_COORDINATE)))
            {
               /* Recompute y. */
               dX = DELTA( *opp_corner_x, pos.x );
               pro_height = scale_pcoord(dX, prop_y, prop_x);
               new_y = *opp_corner_y + (( pos.y > *opp_corner_y ) ? pro_height : -pro_height );
            }
            else
   #endif
            {
               pos.x = new_x;
            }
         }
         else
         {
            new_y = *opp_corner_y + (( pos.y > *opp_corner_y ) ? pro_height : -pro_height );
   #if 0
            if (((new_y < MIN_COORDINATE) && (pos.y = MIN_COORDINATE)) ||
                  ((new_y > MAX_COORDINATE) && (pos.y = MAX_COORDINATE)))
            {
               /* Recompute x. */
               dY = DELTA( *opp_corner_y, pos.y );
               pro_width  = scale_pcoord(dY, prop_x, prop_y);
               new_x = *opp_corner_x + (( pos.x > *opp_corner_x ) ? pro_width : -pro_width );
            }
            else
   #endif
            {
               pos.y = new_y;
            }
         }
      }
   }

	*the_corner_x = ClipToMaximumObjectDimension(pos.x, *opp_corner_x);
   *the_corner_y = ClipToMaximumObjectDimension(pos.y, *opp_corner_y);
}

VOID RectPageObject::stretch_bounds(BOUNDS bounds, BOOL physical)
{
   PCOORD delta_x, delta_y;
   PCOORD ldelta;
   PCOORD prop_x, prop_y;

   delta_x = DELTA(rrecord.unrotated_bound.x1, rrecord.unrotated_bound.x0);
   delta_y = DELTA(rrecord.unrotated_bound.y1, rrecord.unrotated_bound.y0);

   if (physical)
   {
   /* Use physical proportions. */
      prop_x = real_dim.x;
      prop_y = real_dim.y;
   }
   else     /* CTRL_KEY */
   {
   /* Use original proportions. */
      prop_x = prop_dim.x;
      prop_y = prop_dim.y;
   }

   switch( bounds )
   {
      case X_BOUNDS:
      {
         ldelta = SafeMulDivLong(delta_y, prop_x, prop_y);
         ldelta -= delta_x;
			ldelta = ClipToMaximumObjectDimension(ldelta, -delta_x);

         rrecord.unrotated_bound.x0 -= ldelta/2L;
         rrecord.unrotated_bound.x1 += ldelta/2L;
         break;
      }

      case Y_BOUNDS:
      {
         ldelta = SafeMulDivLong(delta_x, prop_y, prop_x);
         ldelta -= delta_y;
			ldelta = ClipToMaximumObjectDimension(ldelta, -delta_y);

         rrecord.unrotated_bound.y0 -= ldelta/2L;
         rrecord.unrotated_bound.y1 += ldelta/2L;
         break;
      }
   }
}

BOOL RectPageObject::GetClipRgn(CRgn& Rgn, RedisplayContextPtr rc, int nType /*=CLIP_TYPE_Boundary*/)
{
   ASSERT(nType == CLIP_TYPE_Boundary);
   BOOL fResult = FALSE;

   // Get the unrotated bounds.
   PBOX UnrotatedBounds = get_unrotated_bound();

   if (get_rotation() == 0.0)
   {
      // The object is not rotated, just use a rectangular region.
      rc->pbox_to_screen(&UnrotatedBounds, FALSE);

      CRect crRgnBounds;
      rc->convert_pbox(&UnrotatedBounds, crRgnBounds, NULL);

      if (rc->is_print_preview)
      {
         ::LPtoDP(rc->destination_hdc, (LPPOINT)(LPRECT)crRgnBounds, 2);
      }

      fResult = Rgn.CreateRectRgnIndirect(crRgnBounds);
		ASSERT(fResult);
   }
   else
   {
      // Rotate the bounds and create a polygon region based on the rotated points.
      PBOX RotatedBounds;
      PPNT v[4];

      compute_rotated_bound(UnrotatedBounds, get_rotation(), &RotatedBounds, get_origin(), v);

      POINT p[5];
      rc->ppnt_to_screen(v[0], &(p[0]));
      rc->ppnt_to_screen(v[1], &(p[1]));
      rc->ppnt_to_screen(v[2], &(p[2]));
      rc->ppnt_to_screen(v[3], &(p[3]));
      p[4] = p[0];

      if (rc->is_print_preview)
      {
         ::LPtoDP(rc->destination_hdc, (LPPOINT)p, sizeof(p)/sizeof(p[0]));
      }

      fResult = Rgn.CreatePolygonRgn(p, sizeof(p)/sizeof(p[0]), ALTERNATE);
   }

   return fResult;
}

PBOX RectPageObject::GetDrawBound(void)
{
   PBOX DrawBound = GetUnrotatedDrawBound();

   ANGLE dAngle = get_rotation();
   if (dAngle != 0)
   {
      compute_rotated_bound(DrawBound, dAngle, &DrawBound, get_origin());
   }

   return DrawBound;
}

PBOX RectPageObject::GetUnrotatedDrawBound(void)
{
   return ComputeDrawBound(get_unrotated_bound());
}

void RectPageObject::CopyAttributes( PMGPageObjectPtr pObject )
{
	PMGPageObject::CopyAttributes( pObject );

	rrecord = ((RectPageObject *) pObject)->rrecord;
}

/*
// Convert a dy/dx set to an angle.
*/

PRIVATE ANGLE near
delta_to_angle(PCOORD dx, PCOORD dy)
{
   double angle = atan2((double)-dy, (double)dx);

   return normalize_angle(angle);
}

PMG_OBJECT_ACTION RectPageObject::move_handle(DRAG_STATE state, PPNT pos, OBJECT_HANDLE handle, FLAGS shift_status, LPVOID far *data /*=NULL*/, BOOL fCropping /*=FALSE*/)
{
   switch (state)
   {
      case DRAG_STATE_begin:
      {
      /* Save our bound for possible abort. */

			OnBeginHandleMove(pos, handle, fCropping);

         break;
      }
      case DRAG_STATE_continue:
      {
         get_database()->toggle_object(this, 0);

         if (handle == OBJECT_HANDLE_ROTATE)
         {
         /* See where we are now. */
            double dNewAngle = delta_to_angle(pos.x - record.rotc.origin.x,
                                             pos.y - record.rotc.origin.y)
                              - starting_angle;

            if (SHIFT_KEY(shift_status))
            {
            /* Force the angle to be constrained. */
               double dSnap = PI/4.0;
               dNewAngle = floor(dNewAngle/dSnap + 0.5)*dSnap;
            }
            record.rotc.rotation = normalize_angle(dNewAngle);
         }
         else
         {
            BOOL fProportional = SPECIAL_KEY(shift_status);
            BOOL fPhysical = SHIFT_KEY(shift_status);
            //PPNT org_pos = pos;
            PPNT pnt;

            if (record.rotc.rotation != 0)
            {
            /* Calculate the position of the handle which shouldn't move. */

               get_anchor_point(handle, &pnt);

            /* Rotate the cursor into the unrotated frame of reference. */

               rotate_points(&pos, 1, record.rotc.origin, -record.rotc.rotation);
            }

            switch (handle)
            {
               case OBJECT_HANDLE_UL:
               {
                  calc_bounds( &rrecord.unrotated_bound.x0, &rrecord.unrotated_bound.y0, &rrecord.unrotated_bound.x1, &rrecord.unrotated_bound.y1, pos, shift_status );
                  break;
               }
               case OBJECT_HANDLE_UR:
               {
                  calc_bounds( &rrecord.unrotated_bound.x1, &rrecord.unrotated_bound.y0, &rrecord.unrotated_bound.x0, &rrecord.unrotated_bound.y1, pos, shift_status );
                  break;
               }
               case OBJECT_HANDLE_LR:
               {
                  calc_bounds( &rrecord.unrotated_bound.x1, &rrecord.unrotated_bound.y1, &rrecord.unrotated_bound.x0, &rrecord.unrotated_bound.y0, pos, shift_status );
                  break;
               }
               case OBJECT_HANDLE_LL:
               {
                  calc_bounds( &rrecord.unrotated_bound.x0, &rrecord.unrotated_bound.y1, &rrecord.unrotated_bound.x1, &rrecord.unrotated_bound.y0, pos, shift_status );
                  break;
               }
               case OBJECT_HANDLE_UPPER:
               {
						rrecord.unrotated_bound.y0 = ClipToMaximumObjectDimension(pos.y, rrecord.unrotated_bound.y1);
                  if (fProportional)
                  {
                     stretch_bounds(X_BOUNDS, fPhysical);
                  }
                  break;
               }
               case OBJECT_HANDLE_RIGHT:
               {
                  rrecord.unrotated_bound.x1 = ClipToMaximumObjectDimension(pos.x, rrecord.unrotated_bound.x0);
                  if (fProportional)
                  {
                     stretch_bounds(Y_BOUNDS, fPhysical);
                  }
                  break;
               }
               case OBJECT_HANDLE_LOWER:
               {
                  rrecord.unrotated_bound.y1 = ClipToMaximumObjectDimension(pos.y, rrecord.unrotated_bound.y0);
                  if (fProportional)
                  {
                     stretch_bounds(X_BOUNDS, fPhysical);
                  }
                  break;
               }
               case OBJECT_HANDLE_LEFT:
               {
						rrecord.unrotated_bound.x0 = ClipToMaximumObjectDimension(pos.x, rrecord.unrotated_bound.x1);
                  if (fProportional)
                  {
                     stretch_bounds(Y_BOUNDS, fPhysical);
                  }
                  break;
               }
            }

				if (fCropping)
				{
					switch (handle)
					{
	               case OBJECT_HANDLE_UL:
	               case OBJECT_HANDLE_LL:
	               case OBJECT_HANDLE_LEFT:
						{
							if (rrecord.unrotated_bound.x1-rrecord.unrotated_bound.x0 < 1)
							{
								rrecord.unrotated_bound.x0 = rrecord.unrotated_bound.x1-1;
							}
							break;
						}

	               case OBJECT_HANDLE_UR:
	               case OBJECT_HANDLE_LR:
	               case OBJECT_HANDLE_RIGHT:
						{
							if (rrecord.unrotated_bound.x1-rrecord.unrotated_bound.x0 < 1)
							{
								rrecord.unrotated_bound.x1 = rrecord.unrotated_bound.x0+1;
							}
							break;
						}
					}

					switch (handle)
					{
	               case OBJECT_HANDLE_UL:
	               case OBJECT_HANDLE_UR:
	               case OBJECT_HANDLE_UPPER:
						{
							if (rrecord.unrotated_bound.y1-rrecord.unrotated_bound.y0 < 1)
							{
								rrecord.unrotated_bound.y0 = rrecord.unrotated_bound.y1-1;
							}
							break;
						}

	               case OBJECT_HANDLE_LL:
	               case OBJECT_HANDLE_LR:
	               case OBJECT_HANDLE_LOWER:
						{
							if (rrecord.unrotated_bound.y1-rrecord.unrotated_bound.y0 < 1)
							{
								rrecord.unrotated_bound.y1 = rrecord.unrotated_bound.y0+1;
							}
							break;
						}
					}
				}

            if (record.rotc.rotation != 0)
            {
            /*
            // Move the object to stay on the anchor point.
            */

            /* Compute handles here. */

               compute_handles();

            /* Do the move. */

               move_to_anchor(handle, pnt);
            }

            if (fCropping)
            {
               PBOX DrawBound = m_OriginalDrawBound;
               PBOX Bound = get_unrotated_bound();
               PPNT Origin = get_origin();
               PPNT Delta;

               Delta.x = m_OriginalOrigin.x - Origin.x;
               Delta.y = m_OriginalOrigin.y - Origin.y;

               PPNT UnrotDelta = m_OriginalOrigin;
               rotate_points(&UnrotDelta, 1, Origin, -get_rotation());
               UnrotDelta.x -= Origin.x;
               UnrotDelta.y -= Origin.y;

               Bound.x0 += Delta.x - UnrotDelta.x;
               Bound.y0 += Delta.y - UnrotDelta.y;
               Bound.x1 += Delta.x - UnrotDelta.x;
               Bound.y1 += Delta.y - UnrotDelta.y;

               // Compute the visible rect from this new draw bound.
               double dDrawWidth = double(DrawBound.x1-DrawBound.x0);
               double dDrawHeight = double(DrawBound.y1-DrawBound.y0);

               record.m_VisibleRect.left = double(Bound.x0 - DrawBound.x0)/dDrawWidth;
               record.m_VisibleRect.top = double(Bound.y0 - DrawBound.y0)/dDrawHeight;
               record.m_VisibleRect.right = double(Bound.x1 - DrawBound.x0)/dDrawWidth;
               record.m_VisibleRect.bottom = double(Bound.y1 - DrawBound.y0)/dDrawHeight;
//             TRACE("DrwBnd: %ld, %ld, %ld, %ld\n"
//                   "UnBnd: %ld, %ld, %ld, %ld\n"
//                   "VR: %f, %f, %f, %f\n",
//                      DrawBound, Bound, record.m_VisibleRect);
            }
         }

         record.flags |= OBJECT_FLAG_needs_calc;

      /* Compute bound and vertices. */

         compute_handles();

         get_database()->toggle_object(this, 0);

         break;
      }
      case DRAG_STATE_abort:
      {
         get_database()->toggle_object(this, 0);
			OnAbortHandleMove(handle, fCropping);
         get_database()->toggle_object(this, 0);

      /* Fall through to... */
      }
      case DRAG_STATE_end:
      {
			OnEndHandleMove(handle, fCropping);
         break;
      }
   }
   return ACTION_DO_NOTHING;
}

void RectPageObject::OnBeginHandleMove(PPNT pos, OBJECT_HANDLE handle, BOOL fCropping)
{
	bound_save = rrecord.unrotated_bound;
	angle_save = get_rotation();
	m_VisibleRectSave = record.m_VisibleRect;

	if (fCropping)
	{
		m_OriginalDrawBound = GetUnrotatedDrawBound();
		m_OriginalOrigin = get_origin();
	}

	int nOffHandle;

	if (handle == OBJECT_HANDLE_ROTATE)
	{
	/*
	// Remember our starting angle relative to the object's angle.
	*/
		starting_angle = delta_to_angle(pos.x - record.rotc.origin.x,
													pos.y - record.rotc.origin.y)
								- record.rotc.rotation;

		nOffHandle = SELECT_FLAG_size_handles;
	}
	else
	{
	/*
	// Remember our starting dimensions for proportional sizing in globals.
	// This could be handled better (no pun intended).
	*/

		prop_dim.x = DELTA(rrecord.unrotated_bound.x1, rrecord.unrotated_bound.x0);
		prop_dim.y = DELTA(rrecord.unrotated_bound.y1, rrecord.unrotated_bound.y0);
		if (prop_dim.x == 0 || prop_dim.y == 0)
		{
			prop_dim.x = prop_dim.y = 1;
		}
		real_dim.x =
			real_dim.y = -1;

		if (!original_dims(&real_dim))
		{
		/* Use proportional dimensions. */
			real_dim = prop_dim;
		}
		else
		{
			if (!fCropping)
			{
				// We need to adjust the original dims for the visible rect.
				real_dim.x = (PCOORD)(real_dim.x*record.m_VisibleRect.Width());
				real_dim.y = (PCOORD)(real_dim.y*record.m_VisibleRect.Height());
			}
		}

		// Turn off the move handle.
		nOffHandle = SELECT_FLAG_rotate_handle;
	}

	turn_select_off(SELECT_FLAG_move_handle | nOffHandle | SELECT_FLAG_subselect);
	add_refresh_flags(fCropping ? REFRESH_FLAG_cropping : REFRESH_FLAG_sizing);

	// Toggle the boundary just for symmetry with DRAG_STATE_end.
	// This has no real effect for the current code (since the code only
	// does the toggle when the object is different from initial conditions).
	get_database()->toggle_object(this, SELECT_FLAG_boundary);
}

void RectPageObject::OnEndHandleMove(OBJECT_HANDLE handle, BOOL fCropping)
{
	// Toggle the boundary off.
	// Most likely, the user has changed the object somehow so
	// that the boundary is now visible. We want it to end up off.
	// If the object has not been resized, this has no effect.
	get_database()->toggle_object(this, SELECT_FLAG_boundary);

	// Restore handles.
	remove_refresh_flags(fCropping ? REFRESH_FLAG_cropping : REFRESH_FLAG_sizing);
	restore_select();
}

void RectPageObject::OnAbortHandleMove(OBJECT_HANDLE handle, BOOL fCropping)
{
	rrecord.unrotated_bound = bound_save;
	set_rotation(angle_save);
	record.m_VisibleRect = m_VisibleRectSave;

	// Compute bound and vertices from this.
	calc();
}

/*
// Returns the coordinates of the handle which should remain anchored when
// the specified handle is used to resize an object.
*/

VOID RectPageObject::get_anchor_point(OBJECT_HANDLE handle, PPNT_PTR pnt)
{
/*
// The anchored handle is merely the handle opposite the specified one.
// With our current numbering, the calculation is easy.
*/
   get_handle_point((OBJECT_HANDLE)(8-handle), pnt);
}

PRIVATE BOOL
calc_func(PBOX_PTR r, VOIDPTR data)
{
   PBOX_PTR bound;

   bound = (PBOX_PTR)data;

/* If this is the first time, just copy the rectangle over. */

   if (bound->x0 == bound->x1)
   {
      *bound = *r;
   }
   else
   {
   /* Otherwise, include both rectangles. */
      if (bound->x0 > r->x0)
      {
         bound->x0 = r->x0;
      }
      if (bound->y0 > r->y0)
      {
         bound->y0 = r->y0;
      }
      if (bound->x1 < r->x1)
      {
         bound->x1 = r->x1;
      }
      if (bound->y1 < r->y1)
      {
         bound->y1 = r->y1;
      }
   }
   return TRUE;
}

VOID RectPageObject::calc(PBOX_PTR panel_world, FLAGS panel_flags)
{
   FLAGS flags = 0;
	BOOL fXFlip = FALSE;
	BOOL fYFlip = FALSE;
		
// if (!(record.flags & OBJECT_FLAG_grouped))
   {

      PCOORD tmp;

   // normalize it if necessary...

      if (rrecord.unrotated_bound.x0 > rrecord.unrotated_bound.x1)
      {
         tmp = rrecord.unrotated_bound.x0;
         rrecord.unrotated_bound.x0 = rrecord.unrotated_bound.x1;
         rrecord.unrotated_bound.x1 = tmp;

         // Flip it without changing the angle.
         
		 //don't actually flip until normalized in the y direction
		 fXFlip = TRUE;
      }

      if (rrecord.unrotated_bound.y0 > rrecord.unrotated_bound.y1)
      {
         tmp = rrecord.unrotated_bound.y0;
         rrecord.unrotated_bound.y0 = rrecord.unrotated_bound.y1;
         rrecord.unrotated_bound.y1 = tmp;

         // Flip it without changing the angle.
         fYFlip = TRUE;
      }

      if(fXFlip)
		xflip(FALSE);
      if(fYFlip)
		yflip(FALSE);
   /*
   // Now handle any layout.
   */

      DB_RECORD_NUMBER layout;

      if (panel_world != NULL && (layout = get_layout()) > 0)
      {
         PPNT obj_size;
         ObjectLayoutPtr layrec;
         POINT size;

      /* Get the real size. */

         if (original_dims(&obj_size))
         {
            size.x = (SHORT)obj_size.x;
            size.y = (SHORT)obj_size.y;
         }
         else
         {
            size.x = size.y = 0;
         }

         if ((layrec = (ObjectLayoutPtr)database->get_record(layout, NULL, RECORD_TYPE_ObjectLayout)) != NULL)
         {
            LayoutElementPtr element;

         /* Zero us out. */

            rrecord.unrotated_bound.x0 =
               rrecord.unrotated_bound.y0 =
               rrecord.unrotated_bound.x1 =
               rrecord.unrotated_bound.y1 = 0;

         /* Generate each image that this object owns. */

            for (element = (LayoutElementPtr)layrec->get_elements()->first_element();
                     element != NULL;
                     element = (LayoutElementPtr)element->next_element())
            {
               element->generate_rects(*panel_world, size, panel_flags, calc_func, &rrecord.unrotated_bound);
               flags |= element->record.flags;
            }

            layrec->release();
         }

      /* Assume it's unrotated. */

         rrecord.bound = rrecord.unrotated_bound;
      }
   }

/*
// Handle flow flags.
*/

   if (flags & ELEMENT_FLOW_LEFT)
   {
      add_flags(OBJECT_FLAG_flow_left);
   }
   else
   {
      remove_flags(OBJECT_FLAG_flow_left);
   }

   if (flags & ELEMENT_FLOW_RIGHT)
   {
      add_flags(OBJECT_FLAG_flow_right);
   }
   else
   {
      remove_flags(OBJECT_FLAG_flow_right);
   }

/* Finish up. */

   compute_handles();

   record.flags &= ~OBJECT_FLAG_needs_calc;
}

/*
// Join a group!
*/

BOOL RectPageObject::join_group(PBOX_PTR bound, PMGPageObjectPtr new_parent)
{
   if (bound != NULL)
   {
      rrecord.group_bound.x0 = rrecord.unrotated_bound.x0 - bound->x0;
      rrecord.group_bound.y0 = rrecord.unrotated_bound.y0 - bound->y0;
      rrecord.group_bound.x1 = rrecord.unrotated_bound.x1 - bound->x0;
      rrecord.group_bound.y1 = rrecord.unrotated_bound.y1 - bound->y0;
      rrecord.flags = record.flags;
      record.group_rotc = record.rotc;
		record.group_rotc.rotation -= new_parent->get_rotation();
      record.flags |= OBJECT_FLAG_grouped;
      parent = new_parent;
   }
   return TRUE;
}

/*
// Leave a group!
*/

VOID RectPageObject::leave_group(VOID)
{
   record.flags &= ~OBJECT_FLAG_grouped;
   parent = NULL;
}

/*
// Helper routines for rotated group_calc().
*/

BOOL adjust_vertices(PPNT* Verts, PBOX* oldrect, PBOX* newrect)
{
   PCOORD src_xmin, src_xmax, src_ymin, src_ymax, src_xspan, src_yspan;
   PCOORD dst_xmin, dst_xmax, dst_ymin, dst_ymax, dst_xspan, dst_yspan;
   double xspan_ratio, yspan_ratio;
   int i;

   src_xmin = oldrect->x0;
   src_xmax = oldrect->x1;
   src_ymin = oldrect->y0;
   src_ymax = oldrect->y1;

   if ((src_xspan = src_xmax - src_xmin)==0)
   {
      return FALSE;
   }
   if ((src_yspan = src_ymax - src_ymin)==0)
   {
      return FALSE;
   }
   dst_xmin = newrect->x0;
   dst_xmax = newrect->x1;
   dst_ymin = newrect->y0;
   dst_ymax = newrect->y1;

   dst_xspan = dst_xmax - dst_xmin;
   dst_yspan = dst_ymax - dst_ymin;
   if ((src_xspan == dst_xspan) && (src_yspan == dst_yspan))
   {
      return FALSE;
   }
   xspan_ratio = (double)dst_xspan / (double)src_xspan;
   yspan_ratio = (double)dst_yspan / (double)src_yspan;

// TRACE("verts before: (%ld, %ld), (%ld, %ld), (%ld, %ld), (%ld, %d)\n",
//          Verts[0], Verts[1], Verts[2], Verts[3]);
   for(i = 0; i < 4; i++) /* adjust vertices */
   {
      Verts[i].x = dst_xmin + (PCOORD)floor(xspan_ratio * (double)(Verts[i].x - src_xmin) + 0.5);
      Verts[i].y = dst_ymin + (PCOORD)floor(yspan_ratio * (double)(Verts[i].y - src_ymin) + 0.5);
   }
// TRACE("verts after: (%ld, %ld), (%ld, %ld), (%ld, %ld), (%ld, %d)\n",
//          Verts[0], Verts[1], Verts[2], Verts[3]);
   return TRUE;
}

static double distance_between_points(PPNT p1, PPNT p2)
{
   double dy = (double)(p1.y - p2.y);
   double dx = (double)(p1.x - p2.x);
   return sqrt(dy*dy + dx*dx);
}

void find_unrot_rect(PPNT* Verts, ANGLE& rot, PBOX* new_box)
{
   // This function attempts to take a distorted rectangle rotated at a certain
   // angle and find a reasonable rectangular representation (possibly at a
   // different rotation.)

   // This feat required working through a large number of cases and
   // contemplating two white boards of small, cryptic drawings to acheive.
   // Thus, I can't give a one liner describing how it's done. But, basically
   // each case requires you to pick between two alternative rectangles that
   // can be inscribed inside the distorted envelope. Once you've made your
   // choice, you compute the rotation and unrotated bounding box.

   // Normalize angle.

   while (rot >= PI2)   rot -= PI2;
   while (rot < 0.0)    rot += PI2;

   // Figure the quadrant of the angle. The final rotation will always
   // be in the same quadrant.

   int nQuadrant = (int)(floor(rot/(PI/2.0)));

   // When we finally get around to picking an angle, we'll have a choice between
   // two possibilities. The fPick variable is munged back and forth to indicate
   // which angle to pick.

   BOOL fPick = Verts[(nQuadrant+1)&3].x < Verts[(nQuadrant+3)&3].x;

   // Find the lengths of the diagonals of the distorted group.

   double d1 = distance_between_points(Verts[0], Verts[2]);
   double d2 = distance_between_points(Verts[1], Verts[3]);

   // Decide which points to use as anchors for the new bounding box.
   // These will always be the endpoints of the shorest diagonal.

   PPNT *p0, *p1;

   if (d1 < d2)
   {
      p0 = &Verts[0];
      p1 = &Verts[2];

      fPick = !fPick;
   }
   else
   {
      p0 = &Verts[1];
      p1 = &Verts[3];
   }

   // Choose the angle to use. The resulting value must be normalized.

   if (fPick)
   {
      rot = atan2((double)(-(Verts[2].y-Verts[1].y)), (double)(Verts[2].x-Verts[1].x))+PI/2.0;
   }
   else
   {
      rot = atan2((double)(-(Verts[1].y-Verts[0].y)), (double)(Verts[1].x-Verts[0].x));
   }

   // Normalize the angle.

   while (rot >= PI2)   rot -= PI2;
   while (rot < 0.0)    rot += PI2;

   // Rotate the anchor points into their positions for the unrotated bounds.

   PPNT origin;

   origin.x = (p0->x + p1->x) >> 1;
   origin.y = (p0->y + p1->y) >> 1;
   
   rotate_points(p0, 1, origin, -rot);
   rotate_points(p1, 1, origin, -rot);

   // Compute the new bounding box.

   new_box->x0 = p0->x;
   new_box->y0 = p0->y;
   new_box->x1 = p1->x;
   new_box->y1 = p1->y;

   // Normalize the bounding box if necessary.

   if (new_box->x0 > new_box->x1)
   {
      PCOORD t = new_box->x0;
      new_box->x0 = new_box->x1;
      new_box->x1 = t;
   }

   if (new_box->y0 > new_box->y1)
   {
      PCOORD t = new_box->y0;
      new_box->y0 = new_box->y1;
      new_box->y1 = t;
   }
}

/*
// Do a group calc.
*/

VOID RectPageObject::group_calc(PBOX_PTR current_group_bound,
                              PBOX_PTR org_group_bound,
                              FLAGS group_flags,
                              ROTATION_COMPONENT_PTR group_rotc)
{
/*
// Unpack some necessary variables.
*/

   PCOORD x0 = current_group_bound->x0;
   PCOORD y0 = current_group_bound->y0;
   PCOORD x1 = current_group_bound->x1;
   PCOORD y1 = current_group_bound->y1;
   PCOORD dx = x1 - x0;
   PCOORD dy = y1 - y0;
   PCOORD org_dx = org_group_bound->x1 - org_group_bound->x0;
   PCOORD org_dy = org_group_bound->y1 - org_group_bound->y0;

   BOOL fDone = FALSE;

   ANGLE rot = record.group_rotc.rotation;

   if (rot != 0.0)
   {
      PBOX gb = rrecord.group_bound;
   /*
   // This object was rotated when it was grouped.
   // Special handling is required to ensure that the object is scaled
   // appropriately. (It lives in a different frame of reference).
   // All of this code was brought over from Avagio (obj_grp.c).
   // Blame Mike Leber if you can't understand it.
   */
      /* We calulate the rotated vertices of the object when the parent */
      /* group is not rotated.                                          */
      gb.x0 += org_group_bound->x0;
      gb.y0 += org_group_bound->y0;
      gb.x1 += org_group_bound->x0;
      gb.y1 += org_group_bound->y0;

      PPNT Origin;

      PBOX Foo;
      PPNT Verts[4];

      /* Recalc the original origin. */
      Origin.x = (gb.x0 + gb.x1) >> 1;
      Origin.y = (gb.y0 + gb.y1) >> 1;

      compute_rotated_bound(gb, rot, &Foo, Origin, Verts);

      /* We now have vertices of the corners of the child object in the */
      /* frame of reference of the unrotated parent group.              */
      /* We now create the 'distorted' object by moving each corner     */
      /* independently within the parent group.                         */
      if (adjust_vertices(Verts, org_group_bound, current_group_bound))
      {
         /* Now we find the rectangle inside the distorted child object, & */
         /* unrotate it. This will become the new child object 'org_bound'.*/
         find_unrot_rect(Verts, rot, &rrecord.unrotated_bound);
         fDone = TRUE;
      }
   }

   if (!fDone)
   {
   /*
   // Compute the scaled, translated bound.
   //
   // The "group_bound" entry contains the object's original bound normalized
   // against the group's original bound. Now we can scale this bound and
   // translate it to its current location.
   */

      PBOX gb = rrecord.group_bound;

      SHORT xcoord_bits = bits_required_by_long(gb.x0);
      SHORT ycoord_bits = bits_required_by_long(gb.y0);
      SHORT next_bits;
      if ((next_bits = bits_required_by_long(gb.x1)) > xcoord_bits)
      {
         xcoord_bits = next_bits;
      }
      if ((next_bits = bits_required_by_long(gb.y1)) > ycoord_bits)
      {
         ycoord_bits = next_bits;
      }
      SHORT dx_bits = bits_required_by(dx);
      SHORT dy_bits = bits_required_by(dy);

      if (dx_bits + xcoord_bits > 30)
      {
         SHORT bits_to_kill = ((dx_bits + xcoord_bits) - 30 + 1)/2;
         PCOORD round_up = (1L << bits_to_kill) - 1;

         dx += round_up;
         dx >>= bits_to_kill;
         gb.x0 += round_up;
         gb.x0 >>= bits_to_kill;
         gb.x1 += round_up;
         gb.x1 >>= bits_to_kill;

         org_dx += round_up+round_up+1;
         org_dx >>= bits_to_kill*2;
         if (org_dx == 0)
         {
            org_dx = 1;
         }
      }

      if (dy_bits + ycoord_bits > 30)
      {
         SHORT bits_to_kill = ((dy_bits + ycoord_bits) - 30 + 1)/2;
         PCOORD round_up = (1L << bits_to_kill) - 1;

         dy += round_up;
         dy >>= bits_to_kill;
         gb.y0 += round_up;
         gb.y0 >>= bits_to_kill;
         gb.y1 += round_up;
         gb.y1 >>= bits_to_kill;

         org_dy += round_up+round_up+1;
         org_dy >>= bits_to_kill*2;
         if (org_dy == 0)
         {
            org_dy = 1;
         }
      }

      rrecord.unrotated_bound.x0 = scale_pcoord(gb.x0, dx, org_dx) + x0;
      rrecord.unrotated_bound.y0 = scale_pcoord(gb.y0, dy, org_dy) + y0;
      rrecord.unrotated_bound.x1 = scale_pcoord(gb.x1, dx, org_dx) + x0;
      rrecord.unrotated_bound.y1 = scale_pcoord(gb.y1, dy, org_dy) + y0;
   }

/*
// When flipped, the object occupies a position on the "other side" of the
// group. Calculate that now.
*/

   if (group_flags & OBJECT_FLAG_xflipped)
   {
      PCOORD tmp = rrecord.unrotated_bound.x0;
      rrecord.unrotated_bound.x0 = x0 + x1 - rrecord.unrotated_bound.x1;
      rrecord.unrotated_bound.x1 = x0 + x1 - tmp;
//    rot = PI2 -rot;
      rot = -rot;
   }

   if (group_flags & OBJECT_FLAG_yflipped)
   {
      PCOORD tmp = rrecord.unrotated_bound.y0;
      rrecord.unrotated_bound.y0 = y0 + y1 - rrecord.unrotated_bound.y1;
      rrecord.unrotated_bound.y1 = y0 + y1 - tmp;
      rot = -rot;
   }

/*
// Handle any rotation.
*/

	/* Calculate the vertice location if it was rotated in place. */
	record.rotc.origin.x = (rrecord.unrotated_bound.x0 + rrecord.unrotated_bound.x1) / 2;
	record.rotc.origin.y = (rrecord.unrotated_bound.y0 + rrecord.unrotated_bound.y1) / 2;
	if (group_rotc->rotation != 0.0)
	{
		/* Rotate the origin of the object through the group rotation. */
		rotate_points(&record.rotc.origin, 1, group_rotc->origin, group_rotc->rotation);
		PCOORD dx = rrecord.unrotated_bound.x1 - rrecord.unrotated_bound.x0;
		PCOORD dy = rrecord.unrotated_bound.y1 - rrecord.unrotated_bound.y0;
		rrecord.unrotated_bound.x1 =
			(rrecord.unrotated_bound.x0 = record.rotc.origin.x - dx/2) + dx;
		rrecord.unrotated_bound.y1 =
			(rrecord.unrotated_bound.y0 = record.rotc.origin.y - dy/2) + dy;

	}
	record.rotc.rotation = normalize_angle(rot + group_rotc->rotation);

/*
// And do a real calc afterwards.
*/

   calc();
}

/*
// Assign an object to 'this'.
*/

ERRORCODE RectPageObject::assign(PageObjectRef sobject)
{
   ERRORCODE error;

   if ((error = PMGPageObject::assign(sobject)) == ERRORCODE_None)
   {
      this->rrecord = ((RectPageObjectRef)sobject).rrecord;
   }
   return error;
}

/*
// read_data()
//
// This is a method invoked by the read method to handle object-specific data.
*/

ERRORCODE RectPageObject::read_data(StorageDevicePtr device)
{
   ERRORCODE error;

   if ((error = device->read_record(&rrecord, sizeof(rrecord))) == ERRORCODE_None)
   {
      compute_handles();
   }
   return error;
}

/*
// write_data()
//
// This is a method invoked by the write method to handle object-specific data.
*/

ERRORCODE RectPageObject::write_data(StorageDevicePtr device)
{
   return device->write_record(&rrecord, sizeof(rrecord));
}

/*
// size_data()
//
// This is a method invoked by the size method to handle object-specific data.
*/

ST_MAN_SIZE RectPageObject::size_data(StorageDevicePtr device)
{
   return device->size_record(sizeof(rrecord));
}

/*
// Compute the handle array from the bounds.
*/

VOID RectPageObject::compute_handles(VOID)
{
/*
// Compute the center of the unrotated bound.
*/

   record.rotc.origin.x = (rrecord.unrotated_bound.x0+rrecord.unrotated_bound.x1)/2;
   record.rotc.origin.y = (rrecord.unrotated_bound.y0+rrecord.unrotated_bound.y1)/2;

   /* Compute vertices and rotated bound. */

   compute_rotated_bound(rrecord.unrotated_bound,
                         record.rotc.rotation,
                         &rrecord.bound,
                         record.rotc.origin,
                         handles);

   /*
   // Compute the displacement of the rotate handle.
   // We can't compute an absolute page coordinate because we want the
   // rotate handle to be a fixed offset from the right handle, and changing
   // views would change that page coordinate. So we compute, in our
   // absolute display units, the distance the rotate handle is from the
   // right handle. We stuff it sneakily into the handles[] array at the
   // rotate handle position (4). Don't use this as you would use the rest!
   */

   PPNT Origin;
   Origin.x = Origin.y = 0;

   handles[4].x = ROTATE_HANDLE_OFFSET;
   handles[4].y = 0;
   rotate_points(&handles[4], 1, Origin, record.rotc.rotation);
}

VOID RectPageObject::set_bound(PBOX pb)
{
   rrecord.bound = rrecord.unrotated_bound = pb;
   record.rotc.rotation = 0;
}

PBOX RectPageObject::get_bound(VOID)
{
   return rrecord.bound;
}

void RectPageObject::set_unrotated_bound(PBOX Bound)
{
   rrecord.unrotated_bound = Bound;
   compute_handles();            // Compute bound and handles.
}
/////////////////////////////////////////////////////////////////////////////
// State management for RectPageObject.

PageObjectState* RectPageObject::CreateState(void)
{
   return new RectPageObjectState;
}

void RectPageObject::SaveState(PageObjectState* pState)
{
   INHERITED::SaveState(pState);

   ((RectPageObjectState*)pState)->m_RRecord = rrecord;
}

void RectPageObject::RestoreState(PageObjectState* pState)
{
   RectPageObjectState* pMyState = (RectPageObjectState*)pState;

   PBOX Bound;
   get_refresh_bound(&Bound);
   pMyState->m_pDatabase->do_refresh_notify(&Bound, REFRESH_ALL, NULL);

   INHERITED::RestoreState(pState);

   rrecord = pMyState->m_RRecord;

   compute_handles();         // In case we're rotated.

   get_refresh_bound(&Bound);
   pMyState->m_pDatabase->do_refresh_notify(&Bound, REFRESH_ALL, NULL);
}

/*
// Add the outline for this object to the passed array.
//
// If pClipBox is not NULL, it is assumed that an intersect test has
// already been performed with this box against the object bound.
*/

void RectPageObject::AddOutline(COutlinePath& Path, PBOX* pClipBox /*=NULL*/)
{
   PBOX Bound = get_unrotated_bound();

   CFixedRect r;
   r.Left = PageToInches(Bound.x0);
   r.Top = PageToInches(Bound.y0);
   r.Right = PageToInches(Bound.x1);
   r.Bottom = PageToInches(Bound.y1);

   COutlinePath LocalPath;

   LocalPath.MoveTo(r.Left, r.Top);
   LocalPath.LineTo(r.Right, r.Top);
   LocalPath.LineTo(r.Right, r.Bottom);
   LocalPath.LineTo(r.Left, r.Bottom);
   LocalPath.LineTo(r.Left, r.Top);
   LocalPath.Close();
   LocalPath.End();

   ANGLE dRotation = get_rotation();
   if (dRotation != 0.0)
   {
      // Compute the center of rotation.
      CFixedPoint c;
      c.x = (r.Left+r.Right)/2;
      c.y = (r.Top+r.Bottom)/2;

      // Build the rotate matrix.
      COutlinePathMatrix RotateMatrix;
      RotateMatrix.Identity();
      RotateMatrix.TranslateBy(-c.x, -c.y);
      RotateMatrix.RotateBy(MakeFixed(dRotation*RAD2DEGC));
      RotateMatrix.TranslateBy(c.x, c.y);

      // Apply the rotation.
      LocalPath.Transform(&RotateMatrix);
   }
   // Add the our path.
   Path.Append(&LocalPath);
}

void
RectPageObject::RotatePBOXToFourPoints(PBOX &Box, PPNT pp [4], double rotation)
{
   if ((&Box != NULL) && (&pp != NULL))
   {
      pp [0].x = Box.x0;
      pp [0].y = Box.y0;

      pp [1].x = Box.x1;
      pp [1].y = Box.y0;

      pp [2].x = Box.x1;
      pp [2].y = Box.y1;

      pp [3].x = Box.x0;
      pp [3].y = Box.y1;

      if (rotation != 0.0)
         rotate_points(pp, 4, get_origin(), rotation);
   }
}

void
RectPageObject::GetShapeData(PMObjectShapeData &sd)
{
   PBOX Box         = get_unrotated_bound();
   double rotation  = get_rotation(); 
   if (rotation == 0.0)
      sd.SetPoints(Box);
   else
   {   
      PPNT pp [4];
      RotatePBOXToFourPoints(Box, pp, rotation);
      sd.SetPoints(pp, 4);
   }
}
// PMObjectShapeData code
BOOL
PMObjectShapeData::IsRect(void) const
{
   if (mShape.GetSize() != 4)
      return FALSE;
   
   if (mShape[0].y != mShape[1].y)
      return FALSE;
   
   if (mShape[1].x != mShape[2].x)
      return FALSE;

   if (mShape[2].y != mShape[3].y)
      return FALSE;

   if (mShape[3].x != mShape[0].x)
      return FALSE;

   return TRUE;
}

void
PMObjectShapeData::GetPoints(CArray<CPoint, CPoint&> &Shape) const
{
   if (&Shape != NULL)
   {
      Shape.RemoveAll();
      CPoint   point;
      for (int i = 0; i < mShape.GetSize(); i ++)
      {
         point = mShape.GetAt(i);
         Shape.Add(point);
      }
   }
}

void
PMObjectShapeData::SetPoints(PBOX &pb)
{
   PPNT pp [4];
   // Put these in an order so IsRect() will work
   pp [0].x = pb.x0;
   pp [0].y = pb.y0;

   pp [1].x = pb.x1;
   pp [1].y = pb.y0;

   pp [2].x = pb.x1;
   pp [2].y = pb.y1;

   pp [3].x = pb.x0;
   pp [3].y = pb.y1;
   
   SetPoints(pp, 4);
}

void
PMObjectShapeData::SetPoints(CRect Rect)
{
   PBOX  Box;
   Box.x0 = Rect.left;
   Box.y0 = Rect.top;
   Box.x1 = Rect.right;
   Box.y1 = Rect.bottom;
   SetPoints(Box);
}

void
PMObjectShapeData::SetPoints(CArray<CPoint, CPoint&> &Shape)
{
   mShape.RemoveAll();   
   if (&Shape != NULL)
   {
      CPoint point;
      for (int i = 0; i < Shape.GetSize(); i ++)
      {
         point = Shape.GetAt(i);
         mShape.Add(point);
      }
   }
}

void
PMObjectShapeData::SetPoints(PPNT *p, int n)
{
   mShape.RemoveAll();
   CPoint   point;
   PPNT     ppnt;
   for (int i = 0; i < n; i ++)
   {
      ppnt = p[i];
      point.x = ppnt.x; point.y = ppnt.y;
      mShape.Add(point);
   }
}

BOOL
PMObjectShapeData::GetRect(CRect &Rect) const
{
   BOOL  ret = FALSE;

   if (&Rect != NULL)
   {
      Rect.SetRectEmpty();
      if (IsRect() == TRUE)
      {
         Rect.left   = mShape[0].x;
         Rect.top    = mShape[0].y;
         Rect.right  = mShape[2].x;
         Rect.bottom = mShape[2].y;
         // If this is HTML, Netscape must have this
         Rect.NormalizeRect();
         ret = TRUE;
      }
   }

   return ret;
}

void
PMObjectShapeData::CopyTo(PMObjectShapeData &sd)
{
   if (&sd != NULL)
   {
      sd.mShape.RemoveAll();
      CPoint point;
      for (int i = 0; i < mShape.GetSize(); i ++)
      {
         point = mShape.GetAt(i);
         sd.mShape.Add(point);   
      }
   }
}
/*
// The static update state used for errors.
*/

PRIVATE UpdateState near error_ustate;

/*
// The error state for a graphic.
*/

PUBLIC UpdateStatePtr 
error_update_state(UpdateStatePtr ustate, UPDATE_TYPE type, ERRORCODE error)
{
   if (ustate != NULL)
   {
      delete ustate;
   }

   error_ustate.type = type;
   error_ustate.error = error;
   return &error_ustate;
}

