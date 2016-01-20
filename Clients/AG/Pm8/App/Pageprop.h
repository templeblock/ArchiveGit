#ifndef _PAGEPROP_INC_
#define _PAGEPROP_INC_
/***************************************************************************
*  FILE:        PAGEPROP.H                                                 *
*  SUMMARY:     Class for storing properties associated with a document    *
*               page.                                                      *
*  AUTHOR:      Dennis Plotzke                                             *
*  ------------------------------------------------------------------------*
// $Log: /PM8/App/Pageprop.h $
// 
// 1     3/03/99 6:08p Gbeddow
// 
// 9     2/26/99 2:40p Johno
// Added CopyOf() and Init() for copying page properties with other
// objects when a page copy is added to a document
// 
// 8     2/01/99 9:38a Johno
// Changes for paneled HTML
// 
// 7     1/20/99 5:02p Johno
// Simplified
// 
// 6     5/13/98 4:34p Johno
// CenteredHTML
// 
// 5     5/07/98 10:24a Johno
// Changed default active link color to red
// 
// 4     4/15/98 11:36a Johno
// Added missing GetBodyTextColor()

*  Initials Legend:                                                        *
*     DGP       Dennis Plotzke                                             *
*  ------------------------------------------------------------------------*
*  Revision History                                                        *
*                                                                          *
*  Date     Initials  Description of Change                                *
*  ------------------------------------------------------------------------*

*  2/13/98  DGP      Created                                               *
***************************************************************************/
// Don't use any virtual functions in this class

#include "pagedefs.h"

class CPageProperties
{
   public:
// Construction
   CPageProperties();
   ~CPageProperties();

// Operations
	void CopyOf(CPageProperties *p);
	
	DB_RECORD_NUMBER GetTextureRecNum()
	{
		return m_dbRecNumTexture; 
	}

	COLOR GetBackgroundColor()
	{ 
		return m_colorBackground; 
	}

	COLOR GetActiveLinkColor()
	{
		return m_colorActiveLink; 
	}

	COLOR GetVisitedLinkColor()
	{
		return m_colorVisitedLink; 
	}

	COLOR GetNonVisitedLinkColor()
	{
		return m_colorNonVisitedLink;
	}

	void SetTextureRecNum(DB_RECORD_NUMBER dbNewRecNum)
	{
		m_dbRecNumTexture = dbNewRecNum; 
	}
                               
	void SetActiveLinkColor(COLOR colorNew)
	{
		m_colorActiveLink = colorNew; 
	}

	void SetBackgroundColor(COLOR colorNew)
	{
		m_colorBackground = colorNew; 
	}

	void SetVisitedLinkColor(COLOR colorNew)
	{
		m_colorVisitedLink = colorNew; 
	}

	void SetNonVisitedLinkColor(COLOR colorNew)
	{
		m_colorNonVisitedLink = colorNew; 
	}

	void SetBodyTextColor(COLOR colorNew)
	{
		m_colorBodyText = colorNew; 
	}

	BOOL HasTexture()
	{
		return (GetTextureRecNum() > 0); 
	}
	// Master items visible flag. (Logic is TRUE to hide; zero default means show.)
	int GetHideMasterItems(void) const
	{ 
		return m_nHideMasterItems; 
	}

	void SetHideMasterItems(int nHideMasterItems)
	{ 
		m_nHideMasterItems = nHideMasterItems; 
	}

	COLOR
	GetBodyTextColor(void)
	{
		return m_colorBodyText;
	}

   void
   CenteredHTML(BOOL c = TRUE)
   {
      m_fCenterHTML = c;
   }

   BOOL
   IsCenteredHTML(void)
   {
      return m_fCenterHTML;
   }

	BOOL HasColor()
	{ 
		return (GetBackgroundColor() != UNDEFINED_COLOR && GetBackgroundColor() != COLOR_WHITE); 
	}

	BOOL HasBackground()
	{ 
		return (HasTexture() || HasColor()); 
	}

   protected:
// Data Members
   DB_RECORD_NUMBER  m_dbRecNumTexture;
   COLOR             m_colorBackground;
   COLOR             m_colorActiveLink;
   COLOR             m_colorVisitedLink;
   COLOR             m_colorNonVisitedLink;
   COLOR             m_colorBodyText;
	// Do we hide master items?
	// FALSE = no (default)
	// TRUE  = yes
	// -1    = always (master page)
	SHORT					m_nHideMasterItems;
   BOOL              m_fCenterHTML;

	void Init(void);
};

#endif

