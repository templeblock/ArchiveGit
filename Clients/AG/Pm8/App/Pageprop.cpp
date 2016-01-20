/***************************************************************************
*  FILE:        PAGEPROP.CPP                                               *
*  SUMMARY:     Class for storing properties associated with a document    *
*               page.                                                      *
*  AUTHOR:      Dennis Plotzke                                             *
*  ------------------------------------------------------------------------*
// $Log: /PM8/App/Pageprop.cpp $
// 
// 1     3/03/99 6:08p Gbeddow
// 
// 8     2/26/99 2:38p Johno
// Added CopyOf() and Init() for copying page properties with other
// objects when a page copy is added to a document
// 
// 7     2/01/99 9:38a Johno
// Changes for paneled HTML
// 
// 6     5/13/98 4:34p Johno
// CenteredHTML
// 
// 5     5/07/98 10:24a Johno
// Changed default active link color to red
// 
// 4     4/15/98 11:36a Johno
// Added missing CPageProperties::GetBodyTextColor()

*  Initials Legend:                                                        *
*     DGP       Dennis Plotzke                                             *
*  ------------------------------------------------------------------------*
*  Revision History                                                        *
*                                                                          *
*  Date     Initials  Description of Change                                *
*  ------------------------------------------------------------------------*
*  2/13/98  DGP      Created                                               *
***************************************************************************/

#include "stdafx.h"
#include "pageprop.h"
#include "utils.h"      // For converting COLOR values

CPageProperties::CPageProperties()
{
	Init();
}

CPageProperties::~CPageProperties()
{
}

void
CPageProperties::Init()
{
   SetTextureRecNum(0);
   SetBackgroundColor(COLOR_WHITE);
   SetActiveLinkColor(color_from_colorref(RGB(255, 0, 0)));
   SetVisitedLinkColor(color_from_colorref(RGB(85, 26, 139)));    // Purple
   SetNonVisitedLinkColor(color_from_colorref(RGB(0, 0, 238)));
   SetBodyTextColor(color_from_colorref(RGB(0, 0, 0)));
	SetHideMasterItems(FALSE);
   CenteredHTML(FALSE);
}

void 
CPageProperties::CopyOf(CPageProperties *p)
{
	Init();
	if (p != NULL)
	{
		SetTextureRecNum(p->GetTextureRecNum());
		SetBackgroundColor(p->GetBackgroundColor());
		SetActiveLinkColor(p->GetActiveLinkColor());
		SetVisitedLinkColor(p->GetVisitedLinkColor());
		SetNonVisitedLinkColor(p->GetNonVisitedLinkColor());
		SetBodyTextColor(p->GetBodyTextColor());
		SetHideMasterItems(p->GetHideMasterItems());
		CenteredHTML(p->IsCenteredHTML());
	}
}
