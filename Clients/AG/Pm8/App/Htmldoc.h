#ifndef _HTMLDOC_INC
#define _HTMLDOC_INC
/***************************************************************************
*  FILE:        HTMLDOC.H                                                  *
*  SUMMARY:     Classes for creating HTML tags for producing a HTML Doc.   *
*  AUTHOR:      Dennis Plotzke                                             *
*  ------------------------------------------------------------------------*
*  Initials Legend:                                                        *
*     DGP       Dennis Plotzke                                             *
*  ------------------------------------------------------------------------*
*  Revision History                                                        *

   See HTMLDOC.CPP for Revision History

*                                                                          *
*  Date     Initials  Description of Change                                *
*  ------------------------------------------------------------------------*
*  2/17/98  DGP      Created                                               *
***************************************************************************/
#include "pmgobj.h"
#include "hyperrec.h"
#include "imagemap.h"

//#define  DEBUG_HTML
#ifdef   DEBUG_HTML
   #define  HTML_NOBORDER  2
   #define  IMG_NOBORDER   2
#else
   #define  HTML_NOBORDER  0
   #define  IMG_NOBORDER   0
#endif
#define LPCSTR_NEWLINE  "\r\n"
//
class CHTMLCompliantInfo
{
public:
   // These are the types of non-HTML compliancy that can occur
   // NOTE: The order here must match the order of strings in CHTMLCompliantInfo::StringID []
   enum NonComplianceTypes
   {
      nctObjectWarped                  = 0x00000001,  // 0
      nctObjectFilled                  = 0x00000002,  // 1
      nctObjectHasOutline              = 0x00000004,  // 2
      nctObjectHasShadow               = 0x00000008,  // 3
      nctObjectHasColumns              = 0x00000010,  // 4
      nctObjectIsEmpty                 = 0x00000020,  // 5
      nctParagraphNonStandardLeading   = 0x00000040,  // 6
      nctParagraphHasLeftMargin        = 0x00000080,  // 7
      nctParagraphHasRightMargin       = 0x00000100,  // 8
      nctParagraphHasLeadingSpace      = 0x00000200,  // 9
      nctParagraphHasTrailingSpace     = 0x00000400,  // 10
      nctParagraphHasIndent            = 0x00000800,  // 11
      nctParagraphHasAlignment         = 0x00001000,  // 12
	  nctParagraphHasBullets		   = 0x00002000,	// 13
	  nctParagraphHasDropCaps		   = 0x00004000,		//14

      nctCharHasNonStandardFace        = 0x00008000,  // 15
      nctCharHasNonStandardSize        = 0x00010000,  // 16
      nctCharHasNonStandardSpacing     = 0x00020000,  // 17
      nctCharHasFillPattern            = 0x00040000,  // 18
      nctCharHasOutline                = 0x00080000,  // 19
      nctCharHasShadow                 = 0x00100000,  // 20
      nctCharHasWhiteOut               = 0x00200000,  // 21

      nctObjectFlipped                 = 0x00400000,  // 22
      nctObjectRotated                 = 0x00800000,  // 23
      nctOverlappingObjects            = 0x01000000,  // 24
      nctObjectClipped                 = 0x02000000,  // 25

	  nctObjectTable				   = 0x04000000		// 26
   };

   enum { NumFlags = 27 };
   
   enum 
   { 
      KeepAsTextFlags = CHTMLCompliantInfo::nctCharHasNonStandardFace
                           | CHTMLCompliantInfo::nctParagraphHasAlignment
                           | CHTMLCompliantInfo::nctCharHasNonStandardSize
                           | nctCharHasNonStandardSpacing
   };
   // Construction
   CHTMLCompliantInfo()
   {
      Empty(); 
      ResetStrings();
      ResetObjects();
   }

   CHTMLCompliantInfo(CHTMLCompliantInfo &ci)
   {
      Empty(); 
      ResetStrings();
      ResetObjects();
      CombineFrom(ci);
   }
   // Operations
   void Empty()      
   {
      m_dwCompliance = 0;   
      m_pObjects.RemoveAll();
   }
   // Comdine one with another
   void
   CombineFrom(CHTMLCompliantInfo &ci);
	// Make me a copy of the specified object
	void
	CopyOf(CHTMLCompliantInfo &ci)
	{
		ASSERT(&ci != NULL);
		Empty();
		CombineFrom(ci);
	}
//////// Compliance functions
   BOOL HasComplianceErrors(void) const
   {
      return !(m_dwCompliance == 0);
   }
   // This method is use by the compliancy checker to inform this
   // object about non-compliant text attributes
   void 
   SetNonCompliant(NonComplianceTypes nc)
   {
      m_dwCompliance |= (DWORD)nc;
   }
   
   void
   ClearNonCompliant(NonComplianceTypes nc)
   {
      m_dwCompliance &= ~((DWORD)nc);
   }
   // Return true if we failed compliance
   // for reasons we forgive at user's request
   BOOL
   CanKeepAsText(void) const
   {
      return ((m_dwCompliance & (~KeepAsTextFlags)) == 0 ? TRUE : FALSE);
   }
   
   void
   ClearKeepAsText(void)
   {
      m_dwCompliance &= (~KeepAsTextFlags);
   }
//////// String functions
   BOOL
   GetFirstString (CString &str)
   {
      ResetStrings();
      return GetNextString(str);
   }

   BOOL
   GetNextString (CString &str);
//////// PM Object functions
   BOOL
   AddObjectPointer(PMGPageObject* po);
   
   int
   NumPMObjects(void) const
   {
      return m_pObjects.GetSize();
   }
   
   PMGPageObject*
   GetFirstObject (void)
   {
      ResetObjects();
      return GetNextObject();
   }
   
   PMGPageObject*
   GetNextObject(void)
   {
      if (m_IdxObjects < m_pObjects.GetSize())
         return m_pObjects.GetAt(m_IdxObjects++);
      
      ResetObjects();
      return NULL;
   }
   // Get the bound of all objects 
   void
   GetObjectsPBOX (PBOX &pb);
   // Get the bound of all objects, and expand it a little
   void
   GetObjectsPBOXExpanded (PBOX &pb);
   // Assume all objects on the same page
   DB_RECORD_NUMBER
   GetObjectsPage(void);

protected:
   DWORD       m_dwCompliance;
   static int  StringID [NumFlags];
   DWORD       m_IdxStrings;
   int         m_IdxObjects;
   // Pointers to object(s) that this set of flags refers to
   CArray<PMGPageObject*, PMGPageObject*> m_pObjects;

   void
   ResetStrings(void)
   {
      m_IdxStrings = 0;
   }
   
   void
   ResetObjects(void)
   {
      m_IdxObjects = 0;
   }
};
//********************************************************************
// Begin HTML Tag classes                                            *
//********************************************************************
// Default behavior for this class is to have an end tag such as </CENTER>
class CHTMLTag
{
public:
// Construction
   CHTMLTag();
   virtual ~CHTMLTag();
// Operations
   // Insert method inserts the supplied tag between this tag
   virtual void   Merge(CHTMLTag &csTag);
   virtual CString GetBody(void)
   {
      return m_csBody;
   } 
   virtual BOOL   IsBody(void)
   {
      return !m_csBody.IsEmpty();
   } 
   virtual void   AddText(LPCSTR szText);
   virtual void   AddText(char ch);
   virtual void   AddLineBreak(void)
   {
      AddText(LPCSTR_NEWLINE);
      AddText("<br />");
   }
   virtual void   Add(CHTMLTag &csTag);
   virtual void   AddRemark(LPCSTR rem)
   {
      m_csRemark = rem;
   }

   virtual void   EmptyBody(void);  // Resets body text without affecting tags
   
protected:
   // Data members
   CString              m_csBody;
   CString              m_csStartTagInsert, m_csEndTagInsert;
   CString              m_csRemark;
//   CMapStringToString   m_cmsAttrib;
   // This operator allows assigment to a CString type object
   virtual void   Append(CString &str);
   virtual void   Empty(void);
   // Return name of tag in derived class (Such as "center")
   virtual void	AppendName(CString &str) const = 0;

   virtual void   AppendStartTag(CString &str, BOOL CRLF = TRUE);
   virtual void   AppendEndTag(CString &str);
   virtual void   AppendBody(CString& Body);
   virtual void   AppendAttrib(CString &str);
   virtual void   AppendInsertedStartTag(CString &str);
   virtual void   AppendInsertedEndTag(CString &str);

   virtual void   SetText(LPCSTR szText);

   virtual void   
   AppendBaseStartTag(CString &str, BOOL CRLF = TRUE);
   
   virtual void   
   AppendBaseEndTag(CString &str, BOOL CRLF = TRUE);

   virtual int
   GetLastLineLength(CString &str) const
   {
      int   i, c;
      for (c = 0, i = str.GetLength() - 1; i >= 0; i--, c++)
      {
         if (str [i] == '\n')
            break;
      }

      return c;
   }
   
   void
   AppendColorString (CString &str, COLOR Color) const;
   virtual void   
   AddTranslatedText(LPCSTR szText, CString &str);
   virtual void   
   AddTranslatedText(char ch, CString &str);
};

class CHTMLTagText : public CHTMLTag
{
	INHERIT(CHTMLTagText, CHTMLTag)
public:
   CHTMLTagText() { Empty(); }
   CHTMLTagText(LPCSTR szText)
   {  
		INHERITED::AddText(szText); 
	}
   virtual void Add(CHTMLTagText &tagText)
   { 
		INHERITED::Add(tagText); 
	}
   virtual void Add(CHTMLTag &tagBasic)
   { 
		INHERITED::Add(tagBasic); 
	}
};
////////
class CHTMLTagLink : public CHTMLTag
{
	INHERIT(CHTMLTagLink, CHTMLTag)
public:

   void
   SetHyperlinkData(HyperlinkData &hd, DocumentRecord *pdr);

   void
   SetLocalHyperlinkData(LPCSTR URL, DocumentRecord *pdr);

	void
	SetOnClick(LPCSTR oc)
	{
		mOnClick = oc;
	}
protected:
   HyperlinkData  mHyperlinkData;
   CString        mURL, mOnClick;

   virtual void 
	AppendName(CString &str) const {str += "a"; } 

   void
   AppendBaseStartTag(CString &str, BOOL CRLF)
   {
      INHERITED::AppendBaseStartTag(str, FALSE);
   }

   virtual void
   AppendAttrib(CString &str);

   void
   AppendBaseEndTag(CString &str, BOOL CRLF)
   {
      INHERITED::AppendBaseEndTag(str, FALSE);
   }
};
////////
class CHTMLLayer : public CHTMLTag
{
	INHERIT(CHTMLLayer, CHTMLTag)
public:
   virtual void 
   SetName(LPCSTR name);
protected:
   CString	m_csName;
	
	virtual void 
	AppendName(CString &str) const {str += "layer"; } 

   virtual void
   AppendAttrib(CString &str);
};
////////
class CHTMLFont : public CHTMLTag
{
	INHERIT(CHTMLFont, CHTMLTag)
public:
   enum Styles
   {
      styleNone         = 0x00,
      styleBold         = 0x01,
      styleItalic       = 0x02,
      styleUnderline    = 0x04,
      styleSuperScript  = 0x08,
      styleSubScript    = 0x10,
      styleFixedWidth   = 0x20
   };

   struct StyleElement
   {
      enum Styles Style;
      LPCSTR      TagHeel;
   };
// Construction
   CHTMLFont();
   CHTMLFont(LPCSTR szBodyText);
// Operations
   virtual void 
   AddText(LPCSTR szText);
   
   virtual void
   AddText(char ch);
   
   virtual void 
   AddText(LPCSTR szText, DWORD Style);

   virtual void SetFace(LPCSTR szFace);
   virtual void SetText(LPCSTR szText);
   virtual void SetSize(int nSize);
   virtual void SetStyle(DWORD dwStyle);
   virtual DWORD GetStyle(void) const
   {
      return m_dwStyle; 
   }  
   virtual void SetColor(int nRed, int nGreen, int nBlue);
   void 
   SetColor(COLOR c)
   {
      mColor = c;
   }

   virtual void Empty(void);

   void
   DumpAllButHTMLStyle(void)
   {
      INHERITED::Empty();
   }

   void
   CopyHTMLStyleFrom(CHTMLFont &f)
   {
      m_dwStyle   = f.m_dwStyle;
      CopyFontInfoFrom(f);
   }

   void
   CopyFontInfoFrom(CHTMLFont &f)
   {
      m_nSize     = f.m_nSize;
      m_csFace    = f.m_csFace;
      mColor      = f.mColor;
   }

   void
   DiffFontInfoWith(CHTMLFont &f)
   {
	  m_dwStyle ^= f.m_dwStyle;
      if (m_nSize == f.m_nSize)
         m_nSize = 0;
      if (m_csFace == f.m_csFace)
         m_csFace.Empty();
      if (mColor == f.mColor)
         mColor = UNDEFINED_COLOR;
   }

   BOOL
   CompareHTMLStyle(CHTMLFont &f) const
   {
      if ((CompareFontInfo(f) == TRUE) && (m_dwStyle == f.m_dwStyle))
         return TRUE;
      return FALSE;
   }

   BOOL
   CompareFontInfo (CHTMLFont &f) const
   {
	  if (m_dwStyle != f.m_dwStyle)
         return FALSE;
      if (m_nSize != f.m_nSize)
         return FALSE;
      if (m_csFace != f.m_csFace)
         return FALSE;
      if (mColor != f.mColor)
         return FALSE;

      return TRUE;
   }

   virtual void
   Add(CHTMLTagLink &l, DWORD Style)
   {
      AppendStyleString (m_csBody, FALSE, Style);
      CHTMLTag::Add(l);
      AppendStyleString (m_csBody, TRUE, Style);
   }

protected:
   CString           m_csFace;
   DWORD             m_dwStyle;
   int               m_nSize;
   COLOR             mColor;
   
   static StyleElement  StyleTable[6];

   virtual void 
   AppendName(CString &str) const { str += "font"; };

   virtual void         
   AppendBaseStartTag(CString &str, BOOL CRLF = TRUE);

   void
   AppendBaseEndTag(CString &str, BOOL CRLF);
   
   virtual void
   AppendAttrib(CString &str);

   void
   AppendStyleString (CString &str, BOOL TheEnd, DWORD dwStyle);

   virtual void 
   AddTextLimitLine(LPCSTR szText);
};

class CHTMLTagTableData : public CHTMLTagText
{
	INHERIT(CHTMLTagTableData, CHTMLTagText)
public:
   enum TableAlignmentTypesV
   {
      VtypeNone = -1,
		VtypeTop,
      VtypeMiddle,
      VtypeBottom,
      VtypeBaseline
   };
   enum TableAlignmentTypesH
   {
      HtypeNone = -1,
		HtypeLeft,
      HtypeCenter,
      HtypeRight
   };
// Construction
   CHTMLTagTableData() { Empty(); }
   virtual void   
	SetAlignmentV(enum TableAlignmentTypesV a)
   { 
		mAlignmentV = a; 
	}

   virtual void   
	SetAlignmentH(enum TableAlignmentTypesH a)
   { 
		mAlignmentH = a; 
	}

   virtual void   
   SetColSpan(int cs)
   {
      Colspan = cs;
   }
   
   int
   ColSpan(void)
   {
      return Colspan;
   }

   virtual void   
   SetRowSpan(int rs)
   {
      Rowspan = rs;
   }

   virtual void   
   Empty(void)
   {
      Colspan = Rowspan = mHeight = mWidth = -1;
		mAlignmentV = VtypeNone;
		mAlignmentH = HtypeNone;
      INHERITED::Empty();
      mColorBack = UNDEFINED_COLOR;
   }

   virtual void         
   SetBackgroundColor(COLOR c)
   {
      mColorBack = c;
   }

   virtual void SetHeight(int h)
   { 
		mHeight = h; 
	}
   virtual void SetWidth(int w)
   { 
		mWidth = w; 
	}

protected:
   int   Colspan, Rowspan, mHeight, mWidth;
	enum TableAlignmentTypesV	mAlignmentV;
	enum TableAlignmentTypesH	mAlignmentH;
   COLOR mColorBack;

   virtual void 
	AppendName(CString &str) const {str += "td"; } 

   virtual void
   AppendAttrib(CString &str);

   void
   AppendBaseEndTag(CString &str, BOOL CRLF)
   {
      INHERITED::AppendBaseEndTag(str, FALSE);
   }
};

class CHTMLTagTableRow : public CHTMLTag
{
	INHERIT(CHTMLTagTableRow, CHTMLTag)
public:
// Construction
   CHTMLTagTableRow() { }
// Operations
   virtual void  
   Add(CHTMLTagTableData   &tagData)
   { 
      if (tagData.ColSpan() > 0)
         nNumCols += tagData.ColSpan();
      else
         nNumCols ++;
      CHTMLTag::Add(tagData); 
   }

   virtual int
   NumCols(void)
   {
      return nNumCols;
   }

   virtual void   
   Empty(void)
   {
      nNumCols = 0;
      INHERITED::Empty();
   }

protected:
   int   nNumCols;
   
	virtual void 
	AppendName(CString &str) const {str += "tr"; } 
};

class CHTMLTagTable : public CHTMLTag
{
	INHERIT(CHTMLTagTable, CHTMLTag)
public:
// Construction
   CHTMLTagTable() 
	{ 
		mBorderWidth = mCellPadding = mCellSpacing = -1;
	}
// Operations
	virtual void
   SetBorder(int b)
   {
      mBorderWidth = b;
   }
   
	virtual void   
	SetPadding(int p)
   {
		mCellPadding = p;
	}

   virtual void   
	SetSpacing(int s)
   {
		mCellSpacing = s; 
	}
// Tags which you can add
   virtual void  Add(CHTMLTagTableRow &tagRow)
   { 
		INHERITED::Add(tagRow); 
	}

protected:
   int      mBorderWidth, mCellPadding, mCellSpacing;

   virtual void 
	AppendName(CString &str) const {str += "table"; } 
   
	virtual void   
   AppendAttrib(CString &str);
};

class CHTMLTagImageMap : public CHTMLTag
{
	INHERIT(CHTMLTagImageMap, CHTMLTag)
public:
   CHTMLTagImageMap() { }
// Operations
   // Image map must have a name.  Set it with this method or in constructor
   virtual void 
   SetName(LPCSTR name);

   void
   AppendBody(CString &Body);

   virtual void
   AppendAttrib(CString &str);

   virtual void
   CopyImageMapElementArray(CImageMapElementArray &Mapr);

   void
   AppendBaseStartTag(CString &str, BOOL CRLF);

protected:
   CString                 m_csBody;
   CString                 m_csMapName;
   CImageMapElementArray   mMapArray;
   
	virtual void 
	AppendName(CString &str) const {str += "map"; } 
};

class CHTMLTagJSCall : public CHTMLTag
{
	INHERIT(CHTMLTagJSCall, CHTMLTag)
public:

   virtual void   
	AppendBody(CString& Body);
   
protected:

	virtual void 
	AppendName(CString &str) const {str += "script"; } 
};

class CHTMLTagBody : public CHTMLTag
{
	INHERIT(CHTMLTagBody, CHTMLTag)
public:
// Construction
   CHTMLTagBody() 
   { 
      mColorBack = mColorLink = mColorActive = mColorVisit = UNDEFINED_COLOR;
      CenterOutput(FALSE);
   }
// Attributes that can be set
   virtual void         
   SetBackgroundColor(COLOR c)
   {
      mColorBack = c;
   }
   
   virtual void         
   SetLinkColor(COLOR c)
   {
      mColorLink = c;
   }

   virtual void         
   SetVisitedLinkColor(COLOR c)
   {
      mColorVisit = c;
   }

   virtual void         
   SetActiveLinkColor(COLOR c)
   {
      mColorActive = c;
   }

   virtual void         
   SetBackgroundImage(LPCSTR fname)
   {
      mImageBack = fname;
   }

   void
   CenterOutput(BOOL c)
   {
      mCenterOutput = c;
   }

	void
	SetOnLoad(LPCSTR ol)
	{
		mOnLoad = ol;
	}
// Tags that you can add
	void CHTMLTagBody::Add(CHTMLTagText &tagThis)
	{ INHERITED::Add(tagThis); }
	void CHTMLTagBody::Add(CHTMLFont &tagThis)
	{ INHERITED::Add(tagThis); }
	void CHTMLTagBody::Add(CHTMLTagTable &tagThis)
	{ INHERITED::Add(tagThis); }
	void CHTMLTagBody::Add(CHTMLTagImageMap &tagThis)
	{ INHERITED::Add(tagThis); }
	void CHTMLTagBody::Add(CHTMLTagJSCall &tagThis)
	{ INHERITED::Add(tagThis); }
	void CHTMLTagBody::Add(CHTMLLayer &tagThis)
	{ INHERITED::Add(tagThis); }
protected:
   BOOL     mCenterOutput;
   COLOR    mColorBack, mColorLink, mColorVisit, mColorActive;
   CString  mImageBack;
	CString  mOnLoad;

   virtual void 
	AppendName(CString &str) const { str += "body"; };

   virtual void   
   AppendAttrib(CString &str);
   
   void
   AppendBaseStartTag(CString &str, BOOL CRLF);

   void
   AppendBaseEndTag(CString &str, BOOL CRLF);
};

class CHTMLTagImage : public CHTMLTag
{
	INHERIT(CHTMLTagImage, CHTMLTag)
public:
// Construction
   CHTMLTagImage() 
   { 
      mBorderWidth = mHeight = mWidth = -1;
   }

   virtual void SetHeight(int h)
   { 
		mHeight = h; 
	}
   virtual void SetWidth(int w)
   { 
		mWidth = w; 
	}

	virtual void
   AppendAttrib(CString &str);
   
	virtual void 
   SetUseMap(LPCSTR mapname, BOOL Local = FALSE)
   {
      mUseMap = mapname;
   }

   virtual void 
   SetSource(LPCSTR Source)
   {
      mSource = Source;
   }

   virtual void
   SetBorder(int b)
   {
      mBorderWidth = b;
   }

   virtual void
   NoBorder(void)
   {
      SetBorder(IMG_NOBORDER);
   }

protected:
   int      mBorderWidth, mHeight, mWidth;
   CString  mUseMap, mSource;

   virtual void 
	AppendName(CString &str) const {str += "img"; } 
   
	void
   AppendBaseEndTag(CString &str, BOOL CRLF){};
};

class CHTMLTagParagraph : public CHTMLTagText
{
	INHERIT(CHTMLTagParagraph, CHTMLTagText)
public:
   enum ParagraphTextAlign
   {
      typeNone =-1,
		typeLeft,
      typeRight,
      typeJustify,
      typeCenter,
      typeTextTop
   };

	CHTMLTagParagraph() 
	{ 
		mTextAlign = typeNone;
	}
   CHTMLTagParagraph(LPCSTR szText) : CHTMLTagText(szText) { }
// Operations
   virtual void SetAlignment(enum ParagraphTextAlign a)
   { 
		mTextAlign = a; 
	}

	virtual void
   AppendAttrib(CString &str);
protected:
	enum ParagraphTextAlign	mTextAlign;
   // Returns name of tag 
   virtual void 
	AppendName(CString &str) const {str += "p"; } 
};

class CHTMLTagDiv : public CHTMLTagText
{
	INHERIT(CHTMLTagDiv, CHTMLTagText)
public:
   
	CHTMLTagDiv()
	{
		Init();
	}

	virtual void 
	SetID(LPCSTR id)
	{
		mID = id;
	}

	virtual void 
	SetVisible(BOOL v)
	{
		mShown = v;
	}

	virtual void
	SetABSPosition(int l, int t)
	{
		absLeft = l;
		absTop = t;
	}

   virtual void   
   Empty(void)
   {
		Init();
      INHERITED::Empty();
   }
protected:
	BOOL		mShown;
	int		absLeft, absTop;
	CString	mID;

	virtual void 
   AppendName(CString &str) const { str += "div"; };
	
   virtual void
   AppendAttrib(CString &str);

	virtual void
	Init(void)
	{
		mShown = FALSE;
		absLeft = absTop = -1;
	}
};

class CHTMLTagTitle : public CHTMLTagText
{
	INHERIT(CHTMLTagTitle, CHTMLTagText)
public:
   CHTMLTagTitle() { }
   CHTMLTagTitle(LPCSTR szText) : CHTMLTagText(szText) { }
// Operations
   // Returns name of tag 
protected:

   virtual void 
	AppendName(CString &str) const {str += "title"; } 

   void
   AppendBaseEndTag(CString &str, BOOL CRLF)
   {
      INHERITED::AppendBaseEndTag(str, FALSE);
   }
};

class CJavaScript;

class CHTMLTagHeader : public CHTMLTag
{
	INHERIT(CHTMLTagHeader, CHTMLTag)
public:
// Construction
   CHTMLTagHeader() {};
   CHTMLTagHeader(LPCSTR t) 
   {
      CHTMLTagTitle Title(t);
      SetTitle(Title);
   };

   virtual void
   Empty(void)
   {
      mAuthor.Empty();
		mJavaScript.Empty();
      INHERITED::Empty();
   }

	virtual void   
	Add(CJavaScript &js);

   virtual        ~CHTMLTagHeader() {};
// Operations
   virtual void   SetTitle(CHTMLTagTitle & tagTitle);
   virtual void
   SetAuthor(LPCSTR a)
   {
      mAuthor = a;
   }
   // Returns name of tag 
protected:
   CString  mAuthor;
	CString	mJavaScript;

   virtual void 
   AppendName(CString &str) const {str += "head"; } 

   virtual void   
   AppendBaseStartTag(CString &str, BOOL CRLF = TRUE);
};

class CHTMLTagDocument : public CHTMLTag
{
	INHERIT(CHTMLTagDocument, CHTMLTag)
public:
// Construction - Init
   CHTMLTagDocument();
   CHTMLTagDocument(CHTMLTagHeader &tagHeader);
   virtual        ~CHTMLTagDocument();
   virtual void   Init(CHTMLTagHeader &tagHeader);
   virtual void   Add(CHTMLTagBody &tagBody)
   { CHTMLTag::Add(tagBody); }

   void
   GetStartTag(CString &str)
   {
      str.Empty();
      INHERITED::AppendStartTag(str, FALSE);
   }
   void
   GetBody(CString &str)
   {
      str.Empty();
      INHERITED::AppendBody(str);
   }
   void
   GetEndTag(CString &str)
   {
      str.Empty();
      INHERITED::AppendEndTag(str);
   }

   ERRORCODE
   Write(CFile &File);

// Operations
protected:
   virtual void AppendName(CString &str) const {str += "html"; } 
};
//********************************************************************
// Begin JavaScript classes                                          *
//********************************************************************
class CJavaScript
{
public:
	static void
	GetStartText(CString &str);
	
	static void
	GetEndText(CString &str);

	virtual void
	GetText(CString &str)
	{
	}
protected:
};

class CJavaScriptPager : public CJavaScript
{
	INHERIT(CJavaScriptPager, CJavaScript)
public:
	
	CJavaScriptPager(int w, int n)
	{
		mWidth = w;
		mNum = n;
	}

	virtual void
	GetText(CString &str);
protected:
	int mWidth, mNum;
};
#endif

