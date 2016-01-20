//////////////////////////////////////////////////////////////////////////////////
//  FILE:       FTBLD.H                                                         //
//  --------------------------------------------------------------------------  //
//  REVISION HISTORY                                                            //
//                                                                              //
//  DATE        INITIALS    DISCRIPTION                                         //
//                                                                              //
//  12-20-95    JRO         Added picture stuff                                                                            //
//  04-17-95    JRO         Added SetHidden ()                                  //
//  09/29/94    YX-1.1      Added new definations to support Table style        //
//              YX-1.2      Added Table Row and Cell functions descriptions     //
//              YX-1.3      Added Table inline functions                        //
//                                                                              //
//////////////////////////////////////////////////////////////////////////////////

#ifndef  FTBLD_H
#define FTBLD_H

#ifdef  WIN32
  #ifndef huge
    #define huge
  #endif  
#endif

#include "rtf.h"

#ifndef _INC_WINDOWS    /* #defined if windows.h has been included */
#ifndef	_WINDOWS_				/* JRO #defined if windows.h 95 has been included */
typedef DWORD COLORREF;
#define RGB(r,g,b)          ((COLORREF)(((BYTE)(r)|((WORD)(g)<<8))|(((DWORD)(BYTE)(b))<<16)))

#define GetRValue(rgb)      ((BYTE)(rgb))
#define GetGValue(rgb)      ((BYTE)(((WORD)(rgb)) >> 8))
#define GetBValue(rgb)      ((BYTE)((rgb)>>16))
#endif
#endif

#define  FTERR                 -1
#define  FTERR_PARAMETER       -2
#define  FT_OK                  1
#define  FT_FAIL                0
#define  FT_MARG_TAB_IND       22   
#define  FT_RTF_TWIPS        1440           
#define  FT_DEFT_SIZE          24      //default font size
#define  FT_DEFT_POSI           6      //default superscript and subscript position
#define  FT_DEFT_MARGLR         1.25
#define  FT_DEFT_MARGTB         1
#define  FT_DEFT_LINESP         0
#define  FT_DEFT_TAB            0.5
#define  FT_DEFT_INDENT         0
#define  FT_DEFT_COLOR          0

typedef  enum
{             
  FT_ANSI,                             //ansi file
  FT_MAC,                              //mac file
  FT_PC,                               //pc file         
  FT_PCA,                              //pca file
//  FT_CONT_UNDER,                    //continue underline
//  FT_DOT_UNDER,                     //dotted underline
//  FT_DOUBLE_UNDER,                  //double underline
//  FT_WORD_UNDER,                    //word underline
//     FT_NO_UNDER,                      //turn off all underline
  FT_LEFT_ALIGN,                    //left alligned
  FT_RIGHT_ALIGN,                   //right alligned
  FT_JUST_ALIGN,                    //justed alligned
  FT_CENTER_ALIGN,                  //centere alligned
  FT_TQR,                           //flush-right tab
  FT_TQC,                           //centered tab
  FT_TQDEC,                         //decimal tab
//Error messages
  FTERR_HEADER,
  FTERR_SU_POSI,
  FTERR_NO_STYLE,
  FTERR_STYLE_EXIST,
  FTERR_ADD_STYLE,
  FTERR_NO_NAME,
  FTERR_EMPTY_PARA,
  FTERR_LESS_TABS
} FT_NORMAL_LIST;


//  YX-1.1      Definations to support Table style
typedef  enum
{
  TB_BORDER_TOP,                   //Table row border top
  TB_BORDER_LEFT,                  //Table row border left
  TB_BORDER_BOTTOM,                //Table row border bottom
  TB_BORDER_RIGHT,                 //Table row border right
  BORDER_BOX,                      //Border around the paragraph
  BORDER_SINGLETH,                 //Single-thickness border
  BORDER_DOUBLETH,                 //Double-thickness border
  BORDER_SHADOW,                   //Shadowed border
  BORDER_DOUBLE,                   //Double border
  BORDER_DOT,                      //Dotted border
  BORDER_DASH,                     //Dashed border
  BORDER_HAIR                      //Hailline Border
} FT_TABLE_BORDER;
//  YX-1.1      End of changes

/*
//Error messages
typedef  enum
{             
  ERR0,
  ERR1,
  FTERR_HEADER,
  FTERR_SU_POSI,
  FTERR_NO_STYLE,
  FTERR_STYLE_EXIST,
  FTERR_ADD_STYLE,
  FTERR_NO_NAME,
  FTERR_EMPTY_PARA,
  FTERR_LESS_TABS
} FT_ERROR_LIST;
*/

#define  FTERR_LEFT_MARGIN      0x0001
#define  FTERR_RIGHT_MARGIN     0x0002
#define  FTERR_TOP_MARGIN       0x0004
#define  FTERR_BOTTOM_MARGIN    0x0008

#define  FTERR_ALIGN            0x0001
#define  FTERR_INDENT           0x0002
#define  FTERR_LINESP           0x0010
#define  FTERR_TABS             0x0020


//////////////////////////  Class CMargins  ///////////////////////
class  CMargins
{
  public:
  CMargins()
  {
    m_nLeftMargin = 1 * FT_RTF_TWIPS;     //default left margin 1"
    m_nRightMargin = (int)(FT_DEFT_MARGLR * (float)FT_RTF_TWIPS);
    m_nTopMargin = FT_DEFT_MARGTB * FT_RTF_TWIPS;
    m_nBottomMargin = FT_DEFT_MARGTB * FT_RTF_TWIPS;
  };
        
  void  SetLeftMargin (float fMargin);
  void  SetRightMargin (float fMargin);
  void  SetTopMargin (float fMargin);
  void  SetBottomMargin (float fMargin);

  public:
  int  m_nLeftMargin;
  int  m_nRightMargin;
  int  m_nTopMargin;
  int  m_nBottomMargin;
};
    

/////////////////////////  Class CParagraph  ///////////////////////
class  Paragraph
{
  public:
  Paragraph ()
  {
    m_Alit = 0;
    m_fi = m_li = m_ri = 0;
    m_Space = 1;
  }
         
  public:
  int     m_Alit;                   //aligment 
  int     m_fi;                     //first line identation
  int     m_li;                     //left identation
  int     m_ri;                     //right identation
  int     m_Space;                  //line space
  CString m_Para;               //temp buffer for new paragraph
        
  CMargins m_cMargins;           //class containing margins
};
    
class  Fonts
{
  public:
  Fonts ()
  {
    m_Size = 12;
    m_bolditalic = 0;
    m_underline = 0;
  }
         
  public:
  CString m_Family;             //font family
  CString m_Name;               //font name
  int     m_Size;                 //font size
  int     m_bolditalic;           //a flag: bit0 and bit1 turn on/off bold, bit2, bit3 turn on/off italic 
  int     m_underline;            //different underline style
};      
    
class  Color
{
  public:
  Color ()
  {
    m_ForeColor = m_BackColor = 0;     
  }
         
  public:
  COLORREF    m_ForeColor;       //foreground color
  COLORREF    m_BackColor;       //background color
};
    
class  CStyle
{
  public:
  CString  m_StyleName;          //style name
  CString  m_ParaStyle;          //paragraph 
  CString  m_FontStyle;
  CString  m_ColorStyle;
};

////////////////////////////////////////////////////////////
//  Class that link paragraphs
////////////////////////////////////////////////////////////
class  CParaghLink
{
  //public function
  public:
  CParaghLink ()
  {
    m_CbdTx = new CBodyText;
  }
        
  inline void Clean ()
  {
    m_pPrev = m_pNext = NULL;
    //m_CbdTx->Empty ();
    delete m_CbdTx;
  }
        
  //public member variables
  public:
  CParaghLink * m_pPrev;
  CParaghLink * m_pNext;
  CBodyText   * m_CbdTx;
//  CString  m_CsRtf;
  };
    
class   CFormatText
{
  public:
  CFormatText ();
  CFormatText (int Type);  //Type:  1: for mac; '2: for pc; 3: for pca; 0 or default  for ansi (default)     
  virtual ~CFormatText ();
  
  public:
  CParaghLink  * AddDeftParagraph ();
  CParaghLink  * AddNewParagraph ();
  CParaghLink  * AddNewParagraph (int Align)
  {
    CParaghLink *p;
    CString     s;   

    p = AddNewParagraph ();
    s = SetAlignment (Align);
    AddControlWord (s);
    return p;
  }
     
  void         
  AddTab (int tabs=1);
  void         
  AddText (char * Text);
  void         
  AddText (CBodyText * BdText);
  void         
  BeginGroup ();
  void         
  EndGroup (int cnt);
  void         
  EndGroup (void);
  void         
  BeginTempParagraph (CParaghLink * pTempParagh);
  void         
  EndTempParagraph ();
  int          
  InsertTextInTabs (char * Text, int Tabs=1);
  int          
  InsertTextInTabs (CBodyText * BdText, int Tabs=1);
  HGLOBAL      
  EndFile ();
// JRO 12-04-95    
  long
  GetHeaderSize (void);
  long
  GetTextSize (void);
  long
  GetHeader (char huge *Header);
  long         
  GetText (char huge *Text);
// JRO Picture stuff
  void
  PictureStart (void);
  void
  PictureMeta (int type);
  void
  PictureDib ();
  void
  PictureDdb ();
  void
  PictureBmp (long bpp, long pln, long wdth);
  void
  PictureMetaBmp (int BPP = 0);
  void
  PictureWidth (DWORD pixels);
  void
  PictureHeight (DWORD pixels);
  void
  PictureScaleX (int scale = 99);
  void
  PictureScaleY (int scale = 99);
  void 
  PictureGoalX (DWORD twips);
  void 
  PictureGoalY (DWORD twips);

  void         
  ResetIndts (float FirstIndt, float LeftIndt, CParaghLink * pParagh=NULL);
  void         
  ResetFirstIndt (float FirstIndt, CParaghLink * pParagh=NULL);
  void         
  ResetLeftIndt (float LeftIndt, CParaghLink * pParagh=NULL);

  void         
  SetIndts (float FirstIndt, float LeftIndt);
  void 
  SetFirstIndt (float FirstIndt);
  void 
  SetLeftIndt (float LeftIndt);

  //int  ResetTabPosi (float TabPosi, int seqNum=0, CParaghLink * pParagh=NULL);
  int  
  ResetTabPosi (float TabPosi, int seqNum=0, int Tbk=-1, CParaghLink * pParagh=NULL);
  void 
  SetBold ();
  int  
  SetColor (int Red, int Green, int Blue, CString * Buff=NULL, int FB=1);
  int  
  SetFont (char * Name, char * Family=NULL, CString * Buff=NULL);
  int  
  SetFontSize (int Size=FT_DEFT_SIZE);
  void 
  SetItalic ();
  int  
  SetParagraph (int Align=FT_LEFT_ALIGN, float inFirst = FT_DEFT_INDENT, float 
                inLeft=FT_DEFT_INDENT, float inRight=FT_DEFT_INDENT, float lineSp=FT_DEFT_LINESP,
  float tab=FT_DEFT_TAB, int tbk=FT_TQR, CParaghLink * pParagh=NULL);
  void 
  SetPlain ();
  void 
  SetMargins (float mLeft, float mRight=FT_DEFT_MARGLR, float mTop=FT_DEFT_MARGTB, float mBottom=FT_DEFT_MARGTB);
  //     int  SetMargins (float left, float right, float top, float bottom);
  void 
  SetSub ();
  void 
  SetHidden (void);
  void 
  SetSuper ();
  void 
  SetUnderline (enum UnderLineType Type);
  void 
  NewLine (int i = 1);
  void 
  SetTabSizeType (int TabSize, int Tbk=-1);
  void 
  SetTabSizeType (float TabSize, int Tbk=-1);
  
//  int  AddStyle (char * StyleName, Fonts &SFont, Color &SColor, Paragraph &SPara, int Update = 0);
//  int  SelectStyle (char * StyleName, int reset = 1);
//  int  AddToStyleTable (CString  Style, int Begin, int Count);     
//  void EndStyle (int  endbrace = 1000);           //default value that ending all open styles
     
  protected:
  void 
  AddControlWord (char * ControlWord);
  void
  AddControlWord (CString &str)
  {
    AddControlWord ((char *)(const char *)str);  
  }
  int  
  SetFontTable (char * Name, char * Family);
  int  
  AddColor (int Red, int Green, int Blue);
  int  
  AddColor (int numColors, CString  Colors);
  int  
  CovLRMargin (float  margin);  //converting left/right margin in inch to working value
  int  
  CheckFontSize (int Size);
  int  
  CheckSuPosi (int Size);
  int  
  CovTBMargin (float  margin);  //converting top/bottom margin in inch to working value
  int  
  CovTab (float tab);
  int  
  GetMarginEntry (int  global);
  void 
  AddDeftColor ();             
  void 
  AddDeftFont ();        
  void 
  AddDeftStyle ();        
  void 
  AddString (int Addr, CString StringIns);//function that insert "StringIns" to "Start"
  void 
  SetSpacings (int Type, int size);
  CString  
  IntToStr (int number);    //function that convert intiger to string
  CString  
  SetAlignment (int Align);
  CString  
  SetIndentation (float InFirst, float InLeft, float InRight);
  CString  
  SetLineSpacing (float LineSp);
  CString  
  SetTab (int Tab, int Tbk);  
  void 
  MakeMargin ();

  CString  
  GetStyleSheet (int * Begin, int * Count);
  void 
  SetParaStyle (Paragraph  Para);
  void 
  SetFontStyle (Fonts  Font);
  int  
  SetColorStyle (Color  Colors);
  int  
  FindParaBegin (CString * ParaBuff);
//  int  FindParaBegin (CString * ParaBuff, int Paras);
     
      
  public:
  HGLOBAL     m_hRtfWBuffer;    //RTF buffer  
  long        m_lSizeBuff;      //size of the RTF buffer
  int         m_nParaghCount;   //number of paragraphes
  CStyle      *m_Style;         //style object
  CString     *m_RtfHead;       //buffer of header
  CString     *m_RtfColor;      //buffer color table
  CString     *m_RtfFont;       //buffer of font table
  CString     *m_RtfStyle;      //buffer of style sheet
  CParaghLink *m_pBeginParagh;  //pointer to begin pargraph node 
  CParaghLink *m_pCurParagh;    //pointer to current pargraph node 
//  CParaghLink * m_pCurParaghHead;  //pointer to current pargraph header node 
  CMargins  m_cMargins;
  CString   m_csMargin;
      
  private:
  int  m_colorCounter;             //number of colors used
  int  m_fontCounter;              //number of fonts used
  int  m_flagStyle;
//  YX-1.2      Table function descriptions
  public:
  void 
  SetTblRowDeft (void);
  void 
  EndRow (void);
  void 
  EndCell (void);
  void 
  ParaghBeTbl (void);
  void 
  RowjLeft ();
  void 
  RowjRight ();
  void 
  RowjCenter ();
  void 
  SetCellSpace (int Space);
  void 
  SetTblLeftEdge (int Edge);
  void 
  SetTblHeight (int Height);
  void 
  SetClRight (int Boundary);
  void 
  SetClShade (int Percentage);
  void 
  SetTblBdTop (int BdStyle, int BdWidth, int BdColor=0);
  void 
  SetTblBdLeft (int BdStyle, int BdWidth, int BdColor=0);
  void 
  SetTblBdBottom (int BdStyle, int BdWidth, int BdColor=0);
  void 
  SetTblBdRight (int BdStyle, int BdWidth, int BdColor=0);
  void 
  SetClBdTop (int BdStyle, int BdWidth, int BdColor=0);
  void 
  SetClBdLeft (int BdStyle, int BdWidth, int BdColor=0);
  void 
  SetClBdBottom (int BdStyle, int BdWidth, int BdColor=0);
  void 
  SetClBdRight (int BdStyle, int BdWidth, int BdColor=0);
  void 
  SetAlignLeft ();
  void 
  SetAlignRight ();
  void 
  SetAlignCenter ();
  void 
  SetAlignJustify ();
      
  BOOL  
  AddBlankParagraph ();
  
  protected:
  void 
  CFormatText::SetBorder (CString * Border, int BdStyle, int BdWidth, int BdColor);
//  YX-1.2      End of Changes  
  BOOL
  ParagraphChain (void);
};               
 
inline void  CFormatText::AddTab (int tabs)
{
  m_pCurParagh->m_CbdTx->AddTab (tabs);
/*  while (tabs > 0)
  {
    AddControlWord ("\\tab");
    tabs --;
    } */
}

inline void  CFormatText::AddText (char * Text)
{         
  //m_pCurParagh->m_CbdTx->m_CsRtf += Text;
  *m_pCurParagh->m_CbdTx += Text;
}                                      

inline void  CFormatText::AddText (CBodyText * BdText)
{         
  //m_pCurParagh->m_CbdTx->m_CsRtf += BdText->m_CsRtf;
  *m_pCurParagh->m_CbdTx += BdText->m_CsRtf;
}                                      


inline void  CFormatText::AddControlWord (char * ControlWord)
{         
  //m_pCurParagh->m_CbdTx->m_CsRtf += ControlWord;
  //m_pCurParagh->m_CbdTx->m_CsRtf += " ";        
  *m_pCurParagh->m_CbdTx += ControlWord;   
  *m_pCurParagh->m_CbdTx += " ";
}                                      

inline void  CFormatText::BeginGroup ()  
{
  //AddControlWord ("{");
  ParagraphChain ();
  m_pCurParagh->m_CbdTx->Begin ();
}                                    

inline void  CFormatText::EndGroup (int cnt)  
{
  //AddControlWord ("}");
  ParagraphChain ();
  m_pCurParagh->m_CbdTx->End (cnt);
}                                    

inline void  CFormatText::EndGroup (void)  
{
  //AddControlWord ("}");
  m_pCurParagh->m_CbdTx->End ();
}                                    

inline void  CFormatText::BeginTempParagraph (CParaghLink * pTempParagh)
{
  m_pCurParagh = pTempParagh;
}

inline void  CFormatText::EndTempParagraph ()
{
  m_pCurParagh = m_pBeginParagh;
  while (m_pCurParagh->m_pNext != NULL)
  m_pCurParagh = m_pCurParagh->m_pNext;
}

////////////////////////////////////////////////////////////////////// 
//  int  InsertTextInTabs ()
//  Inserts CBodyText text before the "Tabs" tab on the current paragraph.
//  Return:     FT_OK if success
//              FTERR_PARAMETER wrong parameter
//              FT_FAIL other error
//  BdText:     text to be inserted
//  Tabs:       number of the tab to insert text
////////////////////////////////////////////////////////////////////// 
inline int CFormatText::InsertTextInTabs (CBodyText * BdText, int Tabs)
{
  char * Text = BdText->m_CsRtf.GetBuffer (1);
  return InsertTextInTabs (Text, Tabs);
}

////////////////////////////////////////////////////////////////////// 
//  void SetBold ()
//  Sets Bold in the current paragraph.
////////////////////////////////////////////////////////////////////// 
inline void  CFormatText::SetBold ()
{                    
  m_pCurParagh->m_CbdTx->SetBold ();
}
////////////////////////////////////////////////////////////////////// 
//  void SetItalic ()
//  Sets Italic in the current paragraph.
////////////////////////////////////////////////////////////////////// 
inline void  CFormatText::SetItalic ()
{                    
  m_pCurParagh->m_CbdTx->SetItalic ();
} 
////////////////////////////////////////////////////////////////////// 
//  void  SetSub ()
//  Sets subscript on the current paragraph.
////////////////////////////////////////////////////////////////////// 
inline void  CFormatText::SetSub ()
{
  m_pCurParagh->m_CbdTx->SetSub ();
}
////////////////////////////////////////////////////////////////////// 
//  void  SetHidden ()
//  Sets text to hidden.
////////////////////////////////////////////////////////////////////// 
inline void  CFormatText::SetHidden ()
{
  m_pCurParagh->m_CbdTx->SetHidden ();
}
////////////////////////////////////////////////////////////////////// 
//  void  SetSuper ()
//  Sets superscript on the current paragraph.
////////////////////////////////////////////////////////////////////// 
inline void  CFormatText::SetSuper ()
{
  m_pCurParagh->m_CbdTx->SetSuper ();
}               
////////////////////////////////////////////////////////////////////// 
//  void  SetPlain ()
//  Sets superscript on the current paragraph.
////////////////////////////////////////////////////////////////////// 
inline void  CFormatText::SetPlain ()
{
  AddControlWord ("\\plain");
}
////////////////////////////////////////////////////////////////////// 
//  void  SetTabSizeType ()
//  Sets both tab size and type on the current paragraph.
////////////////////////////////////////////////////////////////////// 
inline void  CFormatText::SetTabSizeType (int TabSize, int Tbk)
{
  CString Tab = SetTab (TabSize, Tbk);
  AddControlWord (Tab.GetBuffer (1));
}
////////////////////////////////////////////////////////////////////// 
//  void  SetTabSizeType ()
//  Sets both tab size and type on the current paragraph.
////////////////////////////////////////////////////////////////////// 
inline void  CFormatText::SetTabSizeType (float TabSize, int Tbk)
{
  CString Tab = SetTab ((int)(TabSize * (float)FT_RTF_TWIPS), Tbk);
  AddControlWord (Tab.GetBuffer (1));
}
////////////////////////////////////////////////////////////////////// 
//  void  SetUnderline ()
//  Sets or resets varies underline on the current paragraph.
//  flag:       FT_CONT_UNDER for continue underline
//              FT_DOT_UNDER for dotted underline
//              FT_DOUBLE_UNDER for double underline
//              FT_WORD_UNDER for word underline
//              FT_NO_UNDER for turnning off all underlines
////////////////////////////////////////////////////////////////////// 
inline void  CFormatText::SetUnderline (enum UnderLineType  flag)
{
  m_pCurParagh->m_CbdTx->SetUnderline (flag);
}

inline void CFormatText::NewLine (int i)
{
  m_pCurParagh->m_CbdTx->NewLine (i);
}
////////////////////////////////////////////////////////////////////// 
//  void  MakeDocuMargins ()
//  Write document margins to the RTF file.
////////////////////////////////////////////////////////////////////// 
inline  void  CFormatText::MakeMargin ()
{
  //Set margins to a CString
  m_csMargin = "\\margl" + IntToStr (m_cMargins.m_nLeftMargin);
  m_csMargin += "\\margr" + IntToStr (m_cMargins.m_nRightMargin);
  m_csMargin += "\\margt" + IntToStr (m_cMargins.m_nTopMargin);
  m_csMargin += "\\margb" + IntToStr (m_cMargins.m_nBottomMargin);
}
////////////////// Inline Functions of CMargins  /////////////////////
inline  void  CMargins::SetLeftMargin (float fMargin)
{
  m_nLeftMargin = (int)(fMargin * (float)FT_RTF_TWIPS);
}

inline  void  CMargins::SetRightMargin (float fMargin)
{
  m_nRightMargin = (int)(fMargin * (float)FT_RTF_TWIPS);
}

inline  void  CMargins::SetTopMargin (float fMargin)
{
  m_nTopMargin = (int)(fMargin * (float)FT_RTF_TWIPS);
}

inline  void  CMargins::SetBottomMargin (float fMargin)
{
  m_nBottomMargin = (int)(fMargin * (float)FT_RTF_TWIPS);
}
//  YX-1.3      Inline Table functions
///////////////////////////  General Table Functions  //////////////////////////
// Sets table row default
inline  void CFormatText::SetTblRowDeft (void)
{
  AddControlWord ("\\trowd");
}
// End of table row
inline  void CFormatText::EndRow (void)
{
  AddControlWord ("\\row");
}
// End of table cell
inline  void CFormatText::EndCell (void)
{
  AddControlWord ("\\cell");
}
//////////////////////  Row Adjustment Functions  ///////////////////
// The following paragraph is part of a table
inline  void CFormatText::ParaghBeTbl (void)
{
  AddControlWord ("\\intbl");
}
// Row Adjust Left
inline  void CFormatText::RowjLeft ()
{
  AddControlWord ("\\trql");
}
// Row Adjust Right
inline  void CFormatText::RowjRight ()
{
  AddControlWord ("\\trqr");
}

// Row Adjust Center
inline  void CFormatText::RowjCenter ()
{
  AddControlWord ("\\trqc");
}
// Sets half cell space of a table row in Twips
inline  void CFormatText::SetCellSpace (int Space)
{
  CString  CellSpace = "\\trgaph" + IntToStr (Space);
  AddControlWord (CellSpace.GetBuffer (1));
}
// Sets leftmost edge of the table with respect to the left edge of its column
inline  void CFormatText::SetTblLeftEdge (int Edge)
{
  CString  LeftEdge = "\\trleft" + IntToStr (Edge);
  AddControlWord (LeftEdge.GetBuffer (1));
}
// Sets height of a table row
inline  void CFormatText::SetTblHeight (int Height)
{
  CString  TblHeight = "\\trrh" + IntToStr (Height);
  AddControlWord (TblHeight.GetBuffer (1));
}
// Sets the right boundary of a table cell
inline  void CFormatText::SetClRight (int Boundary)
{
  CString  RightBnd = "\\cellx" + IntToStr (Boundary);
  AddControlWord (RightBnd.GetBuffer (1));
}

//////////////////////////  Cell Shadding Functions  ///////////////////////////
// Sets shading of a table cell in percentage
inline  void CFormatText::SetClShade (int Percentage)
{
  CString  CellShading = "\\clshdng" + IntToStr (Percentage);
  AddControlWord (CellShading.GetBuffer (1));
}
//////////////////////////  Table Border Functions  ////////////////////////////
//  Sets table row or cell border Style, Thickness, and Color
//  BdStyle:    Border styles defined in FT_BORDER_STYLE
//  BdWidth:    Border width in Twips
//  BdColor:    Border color, an index of the Color Table
//////////////////////////////////////////////////////////////////////////////// 
inline  void CFormatText::SetTblBdTop (int BdStyle, int BdWidth, int BdColor)
{   
  CString  Border = "\\trbrdrt";
  SetBorder (&Border, BdStyle, BdWidth, BdColor);
  AddControlWord (Border.GetBuffer (1));
}    
inline  void CFormatText::SetTblBdLeft (int BdStyle, int BdWidth, int BdColor)
{   
  CString  Border = "\\trbrdrl";
  SetBorder (&Border, BdStyle, BdWidth, BdColor);
  AddControlWord (Border.GetBuffer (1));
}    

inline  void CFormatText::SetTblBdBottom (int BdStyle, int BdWidth, int BdColor)
{   
  CString  Border = "\\trbrdrb";
  SetBorder (&Border, BdStyle, BdWidth, BdColor);
  AddControlWord (Border.GetBuffer (1));
}    

inline  void CFormatText::SetTblBdRight (int BdStyle, int BdWidth, int BdColor)
{   
  CString  Border = "\\trbrdrr";
  SetBorder (&Border, BdStyle, BdWidth, BdColor);
  AddControlWord (Border.GetBuffer (1));
}    
      
inline  void CFormatText::SetClBdTop (int BdStyle, int BdWidth, int BdColor)
{   
  CString  Border = "\\clbrdrt";
  SetBorder (&Border, BdStyle, BdWidth, BdColor);
  AddControlWord (Border.GetBuffer (1));
}    

inline  void CFormatText::SetClBdLeft (int BdStyle, int BdWidth, int BdColor)
{   
  CString  Border = "\\clbrdrl";
  SetBorder (&Border, BdStyle, BdWidth, BdColor);
  AddControlWord (Border.GetBuffer (1));
}    

inline  void CFormatText::SetClBdBottom (int BdStyle, int BdWidth, int BdColor)
{   
  CString  Border = "\\clbrdrb";
  SetBorder (&Border, BdStyle, BdWidth, BdColor);
  AddControlWord (Border.GetBuffer (1));
}    

inline  void CFormatText::SetClBdRight (int BdStyle, int BdWidth, int BdColor)
{   
  CString  Border = "\\clbrdrr";
  SetBorder (&Border, BdStyle, BdWidth, BdColor);
  AddControlWord (Border.GetBuffer (1));
}    

inline  void CFormatText::SetAlignLeft ()
{
  AddControlWord ("\\ql");
}

inline  void CFormatText::SetAlignRight ()
{
  AddControlWord ("\\qr");
}

inline  void CFormatText::SetAlignCenter ()
{
  AddControlWord ("\\qc");
}

inline  void CFormatText::SetAlignJustify ()
{
  AddControlWord ("\\qj");
}
//  YX-1.3      End of Changes
#endif 
