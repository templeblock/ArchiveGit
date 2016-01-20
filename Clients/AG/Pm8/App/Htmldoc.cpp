/*
// $Workfile: Htmldoc.cpp $
// $Revision: 1 $
// $Date: 3/03/99 6:06p $
// 
//  "This unpublished source code contains trade secrets which are the
//   property of Mindscape, Inc.  Unauthorized use, copying or distribution
//   is a violation of international laws and is strictly prohibited."
// 
//        Copyright © 1997 Mindscape, Inc. All rights reserved.
//
// Revision History:
//
// $Log: /PM8/App/Htmldoc.cpp $
// 
// 1     3/03/99 6:06p Gbeddow
// 
// 47    2/03/99 5:14p Johno
// Added recentering on resize for IE when viewing Paneled HTML.
// 
// 46    1/21/99 6:13p Johno
// Changes for links, hotspots, and text links!
// 
// 45    1/21/99 9:12a Johno
// Make them the same!
// 
// 44    1/19/99 4:09p Cschendel
// added non-complience flags for drop caps and bullets
// 
// 43    1/13/99 6:01p Johno
// Moved some HTML stuff from HTMLCONV to PMWDOC
// 
// 42    1/13/99 10:47a Johno
// Changes for HTML projects
// 
// 41    1/07/99 6:12p Johno
// Preliminary "paneled HTML" for some projects
// Changes to eliminate some h file dependecies
// 
// 40    9/04/98 3:52p Johno
// Simplified: eliminated htmlattr.*
// 
// 39    6/15/98 5:50p Johno
// Changes for new CHTMLCompliantInfo::nctObjectTable flag
// 
// 38    6/05/98 2:40p Johno
// META tags
// 
// 37    6/05/98 12:23p Johno
// Trashed CHTMLTagMeta
// 
// 36    6/04/98 5:16p Johno
// Got rid of unused TagHeading, added background color option for table
// cells (for debug), other changes
// 
// 35    6/02/98 7:04p Johno
// Simplifications / changes / debugs to find HTML positioning bug
// 
// 34    6/02/98 12:00p Johno
// Tweaks
// 
// 33    5/31/98 5:27p Johno
// Functional HTML warning rectangle
// 
// 32    5/30/98 6:27p Johno
// Changes for HTML warning rect
// 
// 31    5/30/98 4:26p Johno
// CHTMLCompliantInfo now tracks PrintMaster object pointers
// 
// 30    5/29/98 5:58p Johno
// Added to KeepAsTextFlags
// 
// 29    5/28/98 4:58p Johno
// Changes for HTML compliance groups
// 
// 28    5/26/98 6:54p Johno
// New "clipped" HTML flag
// 
// 27    5/22/98 3:45p Johno
// Changes for design checker
// 
// 26    5/20/98 6:14p Johno
// Changes for design checker
// 
// 25    5/20/98 12:13p Johno
// Changes for design checker
// 
// 24    5/13/98 4:33p Johno
// "centered output" option
// 
// 23    5/12/98 4:16p Johno
// Code reduction
// 
// 22    5/11/98 6:27p Johno
// Changes to better handle HTML styles (bold, itallic, etc.)
// 
// 21    5/11/98 12:06p Johno
// Added CHTMLTagDocument::Write()
// 
// 20    5/08/98 6:14p Johno
// Big code reduction
// 
// 19    5/07/98 5:57p Johno
// Changes for text - to - graphic override
// All HTML <BR>s preceded by ASCII CRLF, for beauty
// 
// 18    5/06/98 4:39p Johno
// Simplifications
// 
// 17    4/29/98 7:21p Johno
// Moved CHTMLCompliantInfo here
// 
// 16    4/29/98 10:04a Johno
// CHTMLFont::AddText() tries to limit text lines to about 200 charaters
// (ASCII limits lines - no effect on the HTML displayed).
// 
// 15    4/28/98 11:30a Johno
// html FORMATTING CHANGES / SIMPLIFICATIONS
// 
// 14    4/24/98 6:24p Johno
// Simple is beautiful...
// 
// 13    4/23/98 10:51a Johno
// Changes for polygon image maps
// 
// 12    4/08/98 4:26p Johno
// Use AppendBaseStartTag() instead of GetBaseStartTag() 
// Use AppendAttrib() instead of GetAttrib()
// Got rid of CHTMLTagLineBreak
// Skip carriage returns in HTML
// 
// 11    4/03/98 4:33p Johno
// Simplifications / changes for paging links
// 
// 10    4/01/98 12:10p Johno
// Changes to HTML text output
// 
// 9     3/27/98 5:38p Johno
// Removed GetFullURL; Added GetHyperlinkData
// 
// 8     3/25/98 6:34p Johno
// Moved AddTranslatedText in hierarchy
// 
// 7     3/24/98 5:31p Johno
// Moved HTML character translation to CHTMLTagFont
// 
// 6     3/20/98 2:02p Johno
// Simplified code, new ImageMap object
// 
// 5     3/17/98 3:46p Johno
// preliminary implementation of hyperlinks (single graphic objects only)
// 
*/
#include "stdafx.h"
#include "htmldoc.h"

#include "resource.h"
#include "project.h"    // For docrec.h
#include "docrec.h"     // For IndexOfPage()
#include "htmlconv.h"   // For CHTMLDocConverter::GetHTMLName
#include "pmwcfg.h"     // For GetConfiguration()
#include "pmw.h"        // For GET_PMWAPP()
#include "utils.h"

// NOTE: The order here must match the order of flags in CHTMLCompliantInfo::NonComplianceTypes
int CHTMLCompliantInfo::StringID [NumFlags] = 
{
   IDS_WEB_WARP,           // 0
   IDS_WEB_FILLED,         // 1
   IDS_WEB_OUTLINED,       // 2
   IDS_WEB_SHADOW,         // 3
   IDS_WEB_COLUMS,         // 4
   IDS_WEB_EMPTY,          // 5
   IDS_WEB_LEADING,        // 6
   IDS_WEB_MARGIN_LEFT,    // 7
   IDS_WEB_MARGIN_RIGHT,   // 8
   IDS_WEB_LEADING_SPACE,  // 9
   IDS_WEB_TRAILING_SPACE, // 10
   IDS_WEB_INDENT,         // 11
   IDS_WEB_ALIGNMENT,      // 12
   IDS_WEB_BULLET,			// 13
   IDS_WEB_DROPCAP,			// 14
   IDS_WEB_FONT,           // 15
   IDS_WEB_TEXT_SIZE,      // 16
   IDS_WEB_TEXT_SPACING,   // 17
   IDS_WEB_TEXT_FILLED,    // 18
   IDS_WEB_TEXT_OUTLINED,  // 19
   IDS_WEB_TEXT_SHADOW,    // 20
   IDS_WEB_TEXT_WHITE,     // 21
   IDS_WEB_TEXT_FLIPPED,   // 22
   IDS_WEB_TEXT_ROTATED,   // 23
   IDS_WEB_OVERLAP,        // 24
   IDS_WEB_CLIPPED,        // 25
	IDS_WEB_TABLE_OBJECT	// 26

};

BOOL
CHTMLCompliantInfo::GetNextString (CString &str)
{
   ASSERT(&str != NULL);
   str.Empty();
   for (m_IdxStrings; m_IdxStrings < NumFlags; m_IdxStrings ++)
   {
      DWORD Mask = 1 << m_IdxStrings;
      if (m_dwCompliance & Mask)
      {
         ASSERT (m_IdxStrings < NumFlags);
         str.LoadString(StringID[m_IdxStrings]);
         m_IdxStrings ++;
         return TRUE;
      }
   }
   
   ResetStrings();
   return FALSE;
}

void
CHTMLCompliantInfo::CombineFrom(CHTMLCompliantInfo &ci)
{
   m_dwCompliance |= ci.m_dwCompliance;

   PMGPageObject  *po;
   for (po = ci.GetFirstObject(); po != NULL; po = ci.GetNextObject()) 
      AddObjectPointer(po);   
}

BOOL
CHTMLCompliantInfo::AddObjectPointer(PMGPageObject* po)
{
   PMGPageObject  *lpo;
   for (lpo = GetFirstObject(); lpo != NULL; lpo = GetNextObject()) 
      if (lpo == po)
         return FALSE;
   
   m_pObjects.Add(po);
   return TRUE;
}

void
CHTMLCompliantInfo::GetObjectsPBOX (PBOX &pb)
{
   PBOX  bound;
   pb.Empty();
   
   PMGPageObject  *lpo;
   for (lpo = GetFirstObject(); lpo != NULL; lpo = GetNextObject()) 
   {
      bound = lpo->get_bound();
      if (pb.IsEmpty())
         pb = bound;
      else
         UnionBox(&pb, &pb, &bound);
   }
}

void
CHTMLCompliantInfo::GetObjectsPBOXExpanded (PBOX &pb)
{
   GetObjectsPBOX(pb);
   
   if (!pb.IsEmpty())
   {
      PCOORD   pc = scale_pcoord(8, PAGE_RESOLUTION, 96);

      if (pb.x0 >= pc) pb.x0 -= pc;
      if (pb.y0 >= pc) pb.y0 -= pc;
      if (pb.x1 + pc > 0) pb.x1 += pc;
      if (pb.y1 + pc > 0) pb.y1 += pc;
   }
}

DB_RECORD_NUMBER
CHTMLCompliantInfo::GetObjectsPage(void)
{
   DB_RECORD_NUMBER  ret = -1;
   // Assume all objects on the same page
   PMGPageObject  *lpo = GetFirstObject();

   if (lpo != NULL)
      ret = lpo->Page();

   return ret;
}
//////// CHTMLTag methods
CHTMLTag::CHTMLTag()
{
}

CHTMLTag::~CHTMLTag()
{
}

void CHTMLTag::Add(CHTMLTag &tagHTML)
{
   tagHTML.Append(m_csBody);
}

void CHTMLTag::AddText(LPCSTR szText)
{
   m_csBody += szText;
}

void CHTMLTag::AddText(char ch)
{
   m_csBody += ch;
}

void CHTMLTag::AddTranslatedText(char c, CString &str)
{
   if(c == '\t')
   {
      // Replace tab with five spaces
      str += "     ";
   }
   else if(c == '\n')
   {
      str += LPCSTR_NEWLINE;
      str += "<br />";
   }
   else if(c == '\r')
   {
      return;
   }
   else if(c == '<')             // Process tag characters to HTML alias
   {
      str += "&lt;";
   }
   else if(c == '>')
   {
      str += "&gt;";
   }
   else if(c == '&')
   {
      str += "&amp;";
   }
   else
   {
      str += c;
   }
}

void CHTMLTag::AddTranslatedText(LPCSTR szText, CString &str)
{
   for (unsigned long l = 0; szText[l] != 0; l++)
      CHTMLTag::AddTranslatedText(szText[l], str);
}

void CHTMLTag::SetText(LPCSTR szText)
{
   m_csBody = szText;
}
// This method allows merging CHTMLTag derived objects together to form
// a new object that combines the start and end tags and body text.
// Note that no rules are enforced with which tags are allowed to 
// be merged together.  
// A derived class could provide this behavior to enforce rules.

// This method can be useful for creating text formatting objects that may
// have complex formatting characteristics
void CHTMLTag::Merge(CHTMLTag &csTag)
{
   csTag.AppendStartTag(m_csStartTagInsert);
   CString Temp = m_csEndTagInsert;
   csTag.AppendEndTag(m_csEndTagInsert);
   m_csEndTagInsert += Temp;
   csTag.AppendBody(m_csBody);
}

void
CHTMLTag::AppendInsertedStartTag(CString &str)
{
   str += m_csStartTagInsert;
}

void
CHTMLTag::AppendInsertedEndTag(CString &str)
{
   str += m_csEndTagInsert;
}
   
void
CHTMLTag::Append(CString &str)
{
   AppendStartTag(str);
   AppendBody(str);
   AppendEndTag(str);
}
   
void
CHTMLTag::AppendStartTag(CString &str, BOOL CRLF)
{
   AppendBaseStartTag(str, CRLF);
   AppendInsertedStartTag(str);
}

void
CHTMLTag::AppendEndTag(CString &str)
{
   AppendInsertedEndTag(str);
   AppendBaseEndTag(str);
}

void            
CHTMLTag::AppendBaseStartTag(CString &str, BOOL CRLF /*TRUE*/)
{
   if (CRLF == TRUE)
      str += LPCSTR_NEWLINE;

   if (m_csRemark.IsEmpty() == FALSE)
   {
      str += "<!-- ";
      str += m_csRemark;
      str += " -->";
      if (CRLF == TRUE)
         str += LPCSTR_NEWLINE;
   }

   str += "<";
   AppendName(str);
   AppendAttrib(str);
   str += ">";
}

void
CHTMLTag::AppendBaseEndTag(CString &str, BOOL CRLF)
{
   if (CRLF == TRUE)
      str += LPCSTR_NEWLINE;
   str += "<";

   str += "/";
   AppendName(str);
   str += ">";
}

void
CHTMLTag::AppendAttrib(CString &str)
{
/*
   POSITION             position;
   CString              csKey, csValue, csAttrib;

   for(position = m_cmsAttrib.GetStartPosition();position != NULL;)
   {
      str += " ";
      m_cmsAttrib.GetNextAssoc(position, csKey, csValue);
      str += csValue;
   }
*/
}

void
CHTMLTag::AppendBody(CString &str)
{
   str += m_csBody;
}

void CHTMLTag::Empty()
{
   m_csStartTagInsert.Empty();
   m_csEndTagInsert.Empty();
   //m_cmsAttrib.RemoveAll();
   m_csRemark.Empty();
   EmptyBody();
}

void CHTMLTag::EmptyBody()
{
   m_csBody.Empty();
}

void
CHTMLTag::AppendColorString (CString &str, COLOR Color) const
{
   RGBCOLOR rgb = RGBCOLOR_FROM_COLOR(Color);

   int Red   = RED_COMPONENT_OF(rgb);
   int Green = GREEN_COMPONENT_OF(rgb);
   int Blue  = BLUE_COMPONENT_OF(rgb);;
   
   CString tmp;
   tmp.Format("#%02X%02X%02X", Red, Green, Blue);
   str += tmp;
}
////////
void
CHTMLTagBody::AppendBaseStartTag(CString &str, BOOL CRLF)
{
   INHERITED::AppendBaseStartTag(str, TRUE);
   if (mCenterOutput)
   {
      str += LPCSTR_NEWLINE;
      str += "<center>";
   }
}

void   
CHTMLTagBody::AppendAttrib(CString &str)
{
   if (mColorBack != UNDEFINED_COLOR)
   {
      str += " bgcolor=\"";
      AppendColorString (str, mColorBack);
      str += "\"";
   }

   if (mColorLink != UNDEFINED_COLOR)
   {
      str += " link=\"";
      AppendColorString (str, mColorLink);
      str += "\"";
   }

   if (mColorVisit != UNDEFINED_COLOR)
   {
      str += " vlink=\"";
      AppendColorString (str, mColorVisit);
      str += "\"";
   }

   if (mColorActive != UNDEFINED_COLOR)
   {
      str += " alink=\"";
      AppendColorString (str, mColorActive);
      str += "\"";
   }

   if (mImageBack.IsEmpty() == FALSE)
   {
      str += " background=\"";
      str += mImageBack;
      str += "\"";
   }

	if (mOnLoad.IsEmpty() == FALSE)
	{
      str += " onLoad=\"";
      str += mOnLoad;
      str += "\"";
	}
}

void
CHTMLTagBody::AppendBaseEndTag(CString &str, BOOL CRLF)
{
   if (mCenterOutput)
   {
      str += LPCSTR_NEWLINE;
      str += "</center>";
   }
   INHERITED::AppendBaseEndTag(str, CRLF);
}
////////
void CHTMLTagHeader::SetTitle(CHTMLTagTitle & tagTitle)
{
   INHERITED::Add(tagTitle);
}

void   
CHTMLTagHeader::AppendBaseStartTag(CString &str, BOOL CRLF)
{
   INHERITED::AppendBaseStartTag(str, CRLF);

   if(!mAuthor.IsEmpty())
   {
      str += LPCSTR_NEWLINE;
      str += "<meta name=\"author\" content=\"";
      str += mAuthor;
      str += "\">";
   }
                    
   str += LPCSTR_NEWLINE;
   CString  Temp = "<meta name=\"generator\" content=\"@@T v%s\">";
   GetConfiguration()->ReplaceText(Temp);
   CString  Temp2;
   Temp2.Format(Temp, GET_PMWAPP()->GetVersion());
   str += Temp2;

	if (mJavaScript.IsEmpty() == FALSE)
	{
		CJavaScript::GetStartText(str);
		str += mJavaScript;
		CJavaScript::GetEndText(str);	
	}
}

void   
CHTMLTagHeader::Add(CJavaScript &js)
{
	js.GetText(mJavaScript);
}
// CHTMLFont
CHTMLFont::CHTMLFont()
{
   Empty();
}

CHTMLFont::CHTMLFont(LPCSTR szBodyText) 
{
   Empty();
   INHERITED::AddText(szBodyText);
}

void 
CHTMLFont::AddText(char c)
{
   INHERITED::AddTranslatedText(c, m_csBody);
}

void 
CHTMLFont::AddText(LPCSTR szText, DWORD Style)
{
/*
   CString  Temp;

   DWORD StyleChange = (m_dwStyle ^ Style);           // XOR to get changed bits
   DWORD StyleNew    = (StyleChange & Style);         // Mask to get newly on bits
   DWORD StyleOff    = (StyleChange & ~(Style));      // Mask to get newly off bits 
   
   AppendStyleString (Temp, TRUE, StyleOff);
   AppendStyleString (Temp, FALSE, StyleNew);
   CHTMLTag::AddTranslatedText(szText, Temp);
//   AppendStyleString (Temp, TRUE, StyleNew);
   CHTMLTag::AddText(Temp);

   m_dwStyle = Style;
*/
   if (szText[0] != 0)
   {
      CString  Temp;
      AppendStyleString (Temp, FALSE, Style);
      INHERITED::AddTranslatedText(szText, Temp);
      AppendStyleString (Temp, TRUE, Style);
      AddTextLimitLine(Temp);
   }
}

void 
CHTMLFont::AddText(LPCSTR szText)
{
   CString  Temp;
   INHERITED::AddTranslatedText(szText, Temp);
   AddTextLimitLine(Temp);
}
// Adds text, limits the ASCII line length
void 
CHTMLFont::AddTextLimitLine(LPCSTR szText)
{
   int l = GetLastLineLength(m_csBody);
   CString  Temp = szText;
   for (int i = 0; i < Temp.GetLength(); i ++)
   {
      if ((Temp [i] == ' ') && (i + l > 200))
      {
         CString Temp2 = Temp.Left(i);
         Temp2 += LPCSTR_NEWLINE;
         INHERITED::AddText(Temp2);
         Temp = Temp.Right(Temp.GetLength() - i);
         l = 0;//GetLastLineLength(m_csBody);
         i = -1;
         continue;
      }
   }

   INHERITED::AddText(Temp);
}

void CHTMLFont::Empty()
{
   INHERITED::Empty();
   m_dwStyle   = 0;
   m_nSize     = 0;
   m_csFace.Empty();
   mColor = UNDEFINED_COLOR;
}

void CHTMLFont::SetFace(LPCSTR szFace)
{
   m_csFace = szFace;
}

void CHTMLFont::SetText(LPCSTR szText)
{
   EmptyBody();
   INHERITED::AddText(szText);
}

void CHTMLFont::SetSize(int nSize)
{
   m_nSize = nSize;
}

void CHTMLFont::SetStyle(DWORD dwStyle)
{
   m_dwStyle = dwStyle;
}

void 
CHTMLFont::SetColor(int nRed, int nGreen, int nBlue)
{
   mColor = COLOR_FROM_RGB(nRed, nGreen, nBlue);
}
// I want no attributes from the base class
void   
CHTMLFont::AppendAttrib(CString &str)
{
   if (mColor != UNDEFINED_COLOR)
   {
      str += " color=\"";
      AppendColorString(str, mColor);
      str += "\"";
   }

   if(!m_csFace.IsEmpty())
   {
      str += " face=\"";
      str += m_csFace;
      str += "\"";
   }
   
   if(m_nSize > 0)
   {
      CString  Temp;
      int      nHTMLSize;

      if(m_nSize <= 8)
         nHTMLSize = 1;
      else if(m_nSize <= 10)
         nHTMLSize = 2;
      else if(m_nSize <= 12)
         nHTMLSize = 3;
      else if(m_nSize <= 14)
         nHTMLSize = 4;
      else if(m_nSize <= 18)
         nHTMLSize = 5;
      else if(m_nSize <= 24)
         nHTMLSize = 6;
      else
         nHTMLSize = 7;
      
      Temp.Format(" size=\"%d\"", nHTMLSize);      
      str += Temp;
   }
}

void
CHTMLFont::AppendBaseStartTag(CString &str, BOOL CRLF)
{
   INHERITED::AppendBaseStartTag(str, CRLF);
   AppendStyleString (str, FALSE, m_dwStyle);
}

void
CHTMLFont::AppendBaseEndTag(CString &str, BOOL CRLF)
{
   AppendStyleString (str, TRUE, m_dwStyle);
   INHERITED::AppendBaseEndTag(str, FALSE);
}

CHTMLFont::StyleElement CHTMLFont::StyleTable[6] = 
{
   styleBold,        "b>",
   styleItalic,      "i>",
   styleUnderline,   "u>",
   styleSuperScript, "sup>",
   styleSubScript,   "sub>",
   styleFixedWidth,  "tt>"
};

void
CHTMLFont::AppendStyleString (CString &str, BOOL TheEnd, DWORD dwStyle)
{
   if(dwStyle)
   {
      int      Start, End, Adder;
      LPCSTR   TagFront;
      if (TheEnd == TRUE)
      {
         Start = 5; End = -1; Adder = -1;
         TagFront = "</";
      }
      else
      {
         Start = 0; End = 6; Adder = 1;
         TagFront = "<";
      }

      for (int i = Start; i != End; i += Adder)
      {
         if (dwStyle & StyleTable[i].Style)
         {
            str += TagFront;
            str += StyleTable[i].TagHeel;
         }
      }
   }
}
// CHTMLDoc methods
CHTMLTagDocument::CHTMLTagDocument()
{
}

CHTMLTagDocument::CHTMLTagDocument(CHTMLTagHeader &tagHeader)
{
   Init(tagHeader);
}

CHTMLTagDocument::~CHTMLTagDocument()
{
}

ERRORCODE
CHTMLTagDocument::Write(CFile &File)
{
   CString Temp;

   GetStartTag(Temp);
   File.Write((LPCSTR)Temp, Temp.GetLength());
   GetBody(Temp);
   File.Write((LPCSTR)Temp, Temp.GetLength());
   GetEndTag(Temp);
   File.Write((LPCSTR)Temp, Temp.GetLength());

   return ERRORCODE_None;
}

void CHTMLTagDocument::Init(CHTMLTagHeader &tagHeader)
{
   Empty();
   INHERITED::Add(tagHeader);
}
////////////////////////////////////////////////////////////////////////////////
// I want no attributes from the base class
void
CHTMLTagLink::AppendAttrib(CString &str)
{
   str += mURL;
	if (mOnClick.IsEmpty() == FALSE)
	{
		str += " OnClick=\"";
		str += mOnClick;
		str += "\"";
	}
}

void
CHTMLTagLink::SetHyperlinkData(HyperlinkData &hd, DocumentRecord *pdr)
{
   mHyperlinkData.Copy(hd);
   mURL = "";
   pdr->AppendFullURL(hd, mURL);         
}

void
CHTMLTagLink::SetLocalHyperlinkData(LPCSTR URL, DocumentRecord *pdr)
{
   mHyperlinkData.Init();
	mHyperlinkData.FilePathOrURL = URL;
	mHyperlinkData.StringType = TYPE_URL;
   pdr->AppendFullURL(mHyperlinkData, mURL);         
}
////////////////////////////////////////////////////////////////////////////////
void
CHTMLLayer::AppendAttrib(CString &str)
{
   INHERITED::AppendAttrib(str);
	if (m_csName.IsEmpty() == FALSE)
	{
		str += " name=\"";
		str += m_csName;
		str += "\"";
	}
}

void 
CHTMLLayer::SetName(LPCSTR name)
{
   m_csName = name;   
}
////////////////////////////////////////////////////////////////////////////////
void
CHTMLTagImage::AppendAttrib(CString &str)
{
   INHERITED::AppendAttrib(str);
   if (mSource.IsEmpty() == FALSE)
   {
      str += " src=\"";
      str += mSource;
      str += "\"";
   }

   if (mBorderWidth >= 0)
   {
      CString  Temp;
      Temp.Format(" border=\"%d\"", mBorderWidth);
      str += Temp;
   }

   if (mWidth >= 0)
   {
      CString  Temp;
      Temp.Format(" width=\"%d\"", mWidth);
      str += Temp;
   }

   if (mHeight >= 0)
   {
      CString  Temp;
      Temp.Format(" height=\"%d\"", mHeight);
      str += Temp;
   }

   if (mUseMap.IsEmpty() == FALSE)
   {
      str += " usemap=\"#";
      str += mUseMap;
      str += "\"";
   }
}
////////////////////////////////////////////////////////////////////////////////
void
CHTMLTagParagraph::AppendAttrib(CString &str)
{
	INHERITED::AppendAttrib(str);
	
	if (mTextAlign != typeNone)
	{
		str += " align=\"";
		switch (mTextAlign)
		{
			case typeLeft: 
			str += "left";
			break;
			
			case typeRight: 
			str += "right";
			break;
			
			case typeJustify: 
			str += "justify";
			break;
			
			case typeCenter: 
			str += "center";
			break;
			
			case typeTextTop: 
			str += "texttop";
			break;
		}
		str += "\"";
	}
}
////////////////////////////////////////////////////////////////////////////////
void
CHTMLTagDiv::AppendAttrib(CString &str)
{
	INHERITED::AppendAttrib(str);

	CString	temp;

	if (mID.IsEmpty() == FALSE)
	{
		str += " id=\"";
		str += mID;
		str += "\"";
	}

	str += " style=\"";

	if ((absTop >= 0) || (absLeft >= 0))
	{
		str += "position:absolute;";
		if (absLeft >= 0)
		{
			temp.Format("left:%d;", absLeft);
			str += temp;
		}
		if (absTop >= 0)
		{
			temp.Format("top:%d;", absTop);
			str += temp;
		}
	}

	str += "visibility:";
	str += mShown == TRUE ? "visible;" : "hidden;";
	str += "\"";
}
////////////////////////////////////////////////////////////////////////////////
void
CHTMLTagImageMap::AppendBaseStartTag(CString &str, BOOL CRLF)
{
   INHERITED::AppendBaseStartTag(str, TRUE);
}

void 
CHTMLTagImageMap::SetName(LPCSTR name)
{
   m_csMapName = name;   
}

void
CHTMLTagImageMap::AppendBody(CString &str)
{
   for(int i = mMapArray.GetSize() - 1; i >= 0; i --)
   {
      CImageMapElement* p = mMapArray.GetAt(i);
      
      str += "\r\n<area";
      if (p->IsURL() == TRUE)
      {
         if (p->IsRect())
         {
            str += " shape=\"rect\" coords=\"";
         
            CRect rect;
            p->GetRectPosition(rect);
            CString coords;
            coords.Format("%d,%d,%d,%d", rect.left, rect.top, rect.right, rect.bottom);
            str += coords;
            str += "\"";
         }
         else
         if (p->IsPoly())
         {
            str += " shape=\"polygon\" coords=\"";

            CArray<CPoint, CPoint&> Points;
            p->GetPoints(Points);
            CPoint   point;
            CString  Temp;
            for(int j = 0; j < Points.GetSize(); j ++)
            {
               point = Points.GetAt(j);
               Temp.Format("%d,%d", point.x, point.y);
               if (j > 0)
                  str += ',';
               str += Temp;
            }

            str += "\"";
         }
         else
         {
            // Unknown Shape type
            ASSERT(0);
         }
         
         HyperlinkData  hd = p->GetHyperlinkData();
         p->AppendFullURL(str);         
      }
      else
      {
         ASSERT(FALSE);
         str += " shape=\"default\" nohref";   
      }
      str += ">";
   }
}
// I want no attributes from the base class
void
CHTMLTagImageMap::AppendAttrib(CString &str)
{
   if (m_csMapName.IsEmpty() == FALSE)
   {
      str += " name=\"";
      str += m_csMapName;
      str += "\"";
   }
}

void
CHTMLTagImageMap::CopyImageMapElementArray(CImageMapElementArray &Mapr)
{
   mMapArray.Copy(Mapr);
}
////////
void   
CHTMLTagJSCall::AppendBody(CString &str)
{
	str += "\r\n<!--\r\n";
	INHERITED::AppendBody(str);
	str += "\r\n// -->";
}
////////
void
CHTMLTagTable::AppendAttrib(CString &str)
{
   INHERITED::AppendAttrib(str);

	if (mBorderWidth >= 0)
   {
      CString  Temp;
      Temp.Format(" border=\"%d\"", mBorderWidth);
      str += Temp;
   }

	if (mCellPadding >= 0)
   {
      CString  Temp;
      Temp.Format(" cellpadding=\"%d\"", mCellPadding);
      str += Temp;
   }

	if (mCellSpacing >= 0)
   {
      CString  Temp;
      Temp.Format(" cellspacing=\"%d\"", mCellSpacing);
      str += Temp;
   }
}
////////
void
CHTMLTagTableData::AppendAttrib(CString &str)
{
   INHERITED::AppendAttrib(str);
   
   if (mColorBack != UNDEFINED_COLOR)
   {
      str += " bgcolor=\"";
      AppendColorString (str, mColorBack);
      str += "\"";
   }

   if (mWidth >= 0)
   {
      CString  Temp;
      Temp.Format(" width=\"%d\"", mWidth);
      str += Temp;
   }

   if (mHeight >= 0)
   {
      CString  Temp;
      Temp.Format(" height=\"%d\"", mHeight);
      str += Temp;
   }

   if (Colspan >= 0)
   {
      CString  Temp;
      Temp.Format(" colspan=\"%d\"", Colspan);
      str += Temp;
   }

   if (Rowspan >= 0)
   {
      CString  Temp;
      Temp.Format(" rowspan=\"%d\"", Rowspan);
      str += Temp;
   }

	if (mAlignmentH != VtypeNone)
	{
		str += " align=\"";
		switch (mAlignmentH)
		{
			case HtypeLeft:
			str += "left";
			break;
			
			case HtypeCenter:
			str += "center";
			break;
			
			case HtypeRight:
			str += "right";
			break;
		}
		str += "\"";
	} 

	if (mAlignmentV != VtypeNone)
	{
		str += " valign=\"";
		switch (mAlignmentV)
		{
			case VtypeTop:
			str += "top";
			break;
			
			case VtypeMiddle:
			str += "middle";
			break;
			
			case VtypeBottom:
			str += "bottom";
			break;
			
			case VtypeBaseline:
			str += "baseline";
			break;
		}
		str += "\"";
	} 
}
//********************************************************************
// Begin JavaScript classes                                          *
//********************************************************************
void
CJavaScript::GetStartText(CString &str)
{
	str += "\r\n<script>\r\n<!--\r\n";
}

void
CJavaScript::GetEndText(CString &str)
{
	str += "//-->\r\n</script>";
}
////
void
CJavaScriptPager::GetText(CString &str)
{
	int		i;
	CString	s;

	str += 
	"var Panel = 0\r\n"\
	"var ns4 = (document.layers)? true:false\r\n"\
	"var ie4 = (document.all)? true:false\r\n"\
	"var cardHalf = ";
	
	s.Format("%d\r\n", mWidth / 2);
	str += s;
 	
	str +=
	"// Fix for NS bug\r\n"\
	"function reload()\r\n"\
	"{\r\n"\
	" document.location = document.location\r\n"\
	"}\r\n"\
	"if (ns4)\r\n"\
	" window.onResize = reload\r\n"\
	"if (ie4)\r\n"\
	" window.onresize = recenter\r\n";
	
	str +=
	"var MyDivs = new Array()\r\n"\
	"function recenter()\r\n"\
	"{\r\n"\
	"  LayerHide(MyDivs[Panel])\r\n"\
	"  LayerCenter(MyDivs[Panel])\r\n"\
	"  LayerShow(MyDivs[Panel])\r\n"\
	"}\r\n"\
	"function LayerCenter(obj)\r\n"\
	"{\r\n"\
	" var y = 10\r\n"\
	" var x = 10\r\n"\
	" if(window.innerWidth)\r\n"\
	"  x = (window.innerWidth / 2) - cardHalf\r\n"\
	" if (document.body)\r\n"\
	"  if (document.body.clientWidth)\r\n"\
	"   x = (document.body.clientWidth / 2) - cardHalf\r\n"\
	" obj.xpos = x\r\n"\
	" obj.ypos = y\r\n"\
	" obj.left = obj.xpos\r\n"\
	" obj.top = obj.ypos\r\n"\
	"}\r\n"\
	"function LayerShow(obj)\r\n"\
	"{\r\n"\
	" if (ns4) obj.visibility = \"show\"\r\n"\
	" else if (ie4) obj.visibility = \"visible\"\r\n"\
	"}\r\n"\
	"function LayerHide(obj)\r\n"\
	"{\r\n"\
	" if (ns4) obj.visibility = \"hide\"\r\n"\
	" else if (ie4) obj.visibility = \"hidden\"\r\n"\
	"}\r\n"\
	"function NextPage()\r\n"\
	"{\r\n"\
    " if((Panel >= 0)&&(Panel < MyDivs.length - 1))\r\n"\
	" {\r\n"\
	"  LayerHide(MyDivs[Panel])\r\n"\
	"  Panel++;\r\n"\
	"  LayerCenter(MyDivs[Panel])\r\n"\
	"  LayerShow(MyDivs[Panel])\r\n"\
	" }\r\n"\
	"}\r\n"\
	"function PrevPage()\r\n"\
	"{\r\n"\
    " if((Panel > 0)&&(Panel < MyDivs.length))\r\n"\
	" {\r\n"\
	"  LayerHide(MyDivs[Panel])\r\n"\
	"  Panel--;\r\n"\
	"  LayerShow(MyDivs[Panel])\r\n"\
	" }\r\n"\
	"}\r\n"\
	"function Init()\r\n"\
	"{\r\n"\
	" if (ie4)\r\n"\
	" {\r\n";
	
	for (i = 0; i < mNum; i++)
	{
		s.Format("  MyDivs[%d] = div%d.style\r\n", i, i);
		str += s;
	}

	str +=
	" }\r\n"\
	" if(ns4)\r\n"\
	" {\r\n";

	for (i = 0; i < mNum; i++)
	{
		s.Format("  MyDivs[%d] = document.div%d\r\n", i, i);
		str += s;
	}
	
	str +=
	" }\r\n"\
	" if(MyDivs.length > 0)\r\n"\
	" {\r\n"\
	"  LayerCenter(MyDivs[0])\r\n"\
	"  LayerShow(MyDivs[0])\r\n"\
	" }\r\n"\
	"}\r\n";
}
////

