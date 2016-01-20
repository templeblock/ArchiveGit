//////////////////////////////////////////////////////////////////////////////////
//  FILE:       FTBLD.CPP                                                       //
//  --------------------------------------------------------------------------  //
//  REVISION HISTORY                                                            //
//                                                                              //
//  DATE        INITIALS    DISCRIPTION                                         //
//                                                                              //
//  09/29/94    YX-1        Added Table Row and Cell functions to support       //
//                          table styles                                        //
//                                                                              //
//  09/30/94    YX-2.1      Disabled the "BeginGroup" at the beginning of text  //
//                          abd the corresponding "EndGroup."
//              YX-2.2      Corrected "\\pard " to "\\pard"
//////////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include <iostream.h>      
#include <stdio.h>            
#include <windowsx.h>  

#include "ftbld.h"

#ifndef TCHAR
#include  <tchar.h>
#endif

CFormatText::CFormatText()
{
}

CFormatText::~CFormatText()
{
 if(m_RtfHead)
  delete m_RtfHead; 
 if(m_RtfColor)
  delete m_RtfColor; 
 if(m_RtfFont)
  delete m_RtfFont; 
 if(m_RtfStyle)
  delete m_RtfStyle; 
 if(m_Style)
  delete m_Style; 

  CParaghLink  *pPrev, *pTrace = m_pBeginParagh;
  while (pTrace != NULL)
  {
    pPrev = pTrace;
    pTrace = pTrace->m_pNext;
    pPrev->Clean();
    delete pPrev;
  }
}

CFormatText::CFormatText(int Type)
{
   //initialize paragraph buffers
   m_hRtfWBuffer = NULL;
   m_nParaghCount = 0; 
   m_colorCounter = 0;
   m_fontCounter = 0;
   m_flagStyle = 0;
   m_pBeginParagh = NULL;
   AddDeftParagraph();
int index = m_pCurParagh->m_CbdTx->m_CsRtf.Find("\\par");
   m_pCurParagh->m_CbdTx->m_CsRtf = m_pCurParagh->m_CbdTx->m_CsRtf.Left(index);
   m_pCurParagh->m_CbdTx->m_CsRtf += "\\pard";
   //build header
   m_RtfHead = new CString;
   * m_RtfHead = "{\\rtf1\\";
   switch(Type)
    {
     case  FT_MAC:              //mac
        * m_RtfHead += "mac";
        break;
     case  FT_PC:               //pc
        * m_RtfHead += "pc";
        break;
     case FT_PCA:               //pca                      
        * m_RtfHead += "pca";
        break;                                                     
     default:                  //ansi
        * m_RtfHead += "ansi";
    }                                
   * m_RtfHead += "\\deff0"; 

   //set default font table, color table, margin, and line spacing
   m_RtfFont = new CString;
   * m_RtfFont = "{\\fonttbl}"; 
   AddDeftFont();                      //add default font
   m_RtfColor = new CString;
   * m_RtfColor = "{\\colortbl}";   
   AddDeftColor();                     //add default color table
   m_RtfStyle = new CString;
   AddDeftStyle();                     //add default style sheet
   m_Style = new CStyle;
   AddControlWord("\\plain\\f0\\fs20");
//   SetFontSize(14);                    //add default style sheet
 } 
//////////////////////////////// public member functions ////////////////////////////////// 
BOOL
CFormatText::ParagraphChain ()
{
  if(m_pBeginParagh == NULL)              //create begin paragraph node
  {
    m_pBeginParagh = new CParaghLink;

    m_pCurParagh = m_pBeginParagh;
    m_pBeginParagh->m_pPrev = m_pBeginParagh->m_pNext = NULL;
    m_nParaghCount = 0;                  //count number of paragraphes
    return TRUE;
  }
  return FALSE;
}

BOOL  
CFormatText::AddBlankParagraph ()
{
  if (ParagraphChain () == FALSE)
  {
    m_pCurParagh->m_pNext = new CParaghLink;
    m_pCurParagh->m_pNext->m_pPrev = m_pCurParagh;
    m_pCurParagh = m_pCurParagh->m_pNext; //update current pointer and counter
    m_pCurParagh->m_pNext = NULL;
    m_nParaghCount ++;                    //count number of paragraphes

    return TRUE;
  }
  
  return FALSE;
}
//////////////////////////////////////////////////////////////////////
//  AddDeftParagraph()
//  This function creates or appends the CParaghLink linked-list and
//  initialize the paragraph with default style.
//  Return:     pointer to the new linked-list node
//////////////////////////////////////////////////////////////////////
CParaghLink * CFormatText::AddDeftParagraph()
{
 //build a new paragraph object and link it
 /*
 if(!m_pBeginParagh)                   //create begin paragraph node
 {
  m_pBeginParagh = new CParaghLink;
  m_pCurParagh = m_pBeginParagh;
  m_pBeginParagh->m_pPrev = m_pBeginParagh->m_pNext = NULL;
//  m_pBeginParagh->m_CbdTx->Begin ();     YX-2.1
  m_nParaghCount = 0;                  //count number of paragraphes
 }
*/
 ParagraphChain ();
 
 AddControlWord("\\pard");             // YX-2.2,  was "\\pard "
 return m_pCurParagh;
}
//////////////////////////////////////////////////////////////////////
//  AddNewParagraph()
//  This function creates or appends the CParaghLink linked-list and
//  initialize the paragraph buffer on the new node.
//  Return:     pointer to the new linked-list node
//////////////////////////////////////////////////////////////////////
CParaghLink * CFormatText::AddNewParagraph()
{
/*
 //build a new paragraph object and link it
 if(!m_pBeginParagh)                   //create begin paragraph node
 {
  m_pBeginParagh = new CParaghLink;
  m_pCurParagh = m_pBeginParagh;
//  m_pCurParaghHead = m_pBeginParagh;
  m_pBeginParagh->m_pPrev = m_pBeginParagh->m_pNext = NULL;
  //m_pBeginParagh->m_CbdTx->m_CsRtf = "{\\par "; //begin text group
  m_pBeginParagh->m_CbdTx->Begin ();
  m_pBeginParagh->m_CbdTx->NewLine ();
  m_nParaghCount = 0;                  //count number of paragraphes
 }
 else                                  //append paragraph linked-list
 { 
  m_pCurParagh->m_pNext = new CParaghLink;
  m_pCurParagh->m_pNext->m_pPrev = m_pCurParagh;
  m_pCurParagh = m_pCurParagh->m_pNext;//update current pointer and counter
  m_pCurParagh->m_pNext = NULL;
  //m_pCurParagh->m_CbdTx->m_CsRtf = "\\par ";
  m_pCurParagh->m_CbdTx->NewLine ();
  m_nParaghCount ++;                   //count number of paragraphes
 }
*/ 
 if (AddBlankParagraph () == FALSE)
  m_pCurParagh->m_CbdTx->Begin ();

 m_pCurParagh->m_CbdTx->NewLine ();
 
 return m_pCurParagh;
}
//////////////////////////////////////////////////////////////////////
//  EndFile()
//  This function ends the RTF file and combines the file header, 
//  color table, font table, style sheet, and all paragraphes to the
//  buffer m_hRtfWBuffer and returns a Global Handle of this buffer to caller.
//  "m_lSizeBuff" indicates size of this buffer.
// 
//  Caller must release Global Memory by calling GlobalFree().
//////////////////////////////////////////////////////////////////////
HGLOBAL CFormatText::EndFile()
{
  long         addr;// = 0;
  char huge    *pRtfWBuffer;
 
  //*m_pCurParagh->m_CbdTx += "\\par }";          //YX-2.1,  was "\\par } }"     
  *m_pCurParagh->m_CbdTx += "}";                  // JRO
  
  //combine the header, tables, and paragraphs
  //count size of the RTF file
  m_lSizeBuff = GetHeaderSize ();

  m_lSizeBuff += GetTextSize ();
  
  m_lSizeBuff += 1;
  
  //allocate memory 
  m_hRtfWBuffer = GlobalAlloc (GMEM_MOVEABLE, m_lSizeBuff);
  if(m_hRtfWBuffer == NULL)
    return NULL;
  // Get pointer to Global Object
  pRtfWBuffer = (char huge *) GlobalLock (m_hRtfWBuffer);
  // If we cannot lock it, then release memory and return error.
  if(pRtfWBuffer == NULL)
  {
    GlobalFree(m_hRtfWBuffer);
    return NULL;
  }
  
  addr = GetHeader (pRtfWBuffer);

  GetText (&pRtfWBuffer [addr]);
  
  // Unlock Global Object
  GlobalUnlock(m_hRtfWBuffer);
  return m_hRtfWBuffer;
}
  
long
CFormatText::GetHeaderSize (void)
{
  long  l;
      
  MakeMargin ();      //make margin CString

  l = m_RtfHead->GetLength ();
  l += m_RtfFont->GetLength ();
  l += m_RtfColor->GetLength ();
  l += m_RtfStyle->GetLength ();
  l += m_csMargin.GetLength ();
      
  return l;
}

long
CFormatText::GetHeader (char huge *pHeader)
{
  long  addr = 0;

  //copy header and tables to pHeader
  _fstrcpy (pHeader, m_RtfHead->GetBuffer(1));
  addr += m_RtfHead->GetLength();
  _fstrcpy (&pHeader [addr], m_RtfFont->GetBuffer(1));
  addr += m_RtfFont->GetLength();
  _fstrcpy (&pHeader [addr], m_RtfColor->GetBuffer(1));
  addr += m_RtfColor->GetLength();
  _fstrcpy (&pHeader [addr], m_RtfStyle->GetBuffer(1));
  addr += m_RtfStyle->GetLength();
  _fstrcpy (&pHeader [addr], m_csMargin.GetBuffer(1));
  addr += m_csMargin.GetLength();
  
  //free headers
  delete m_RtfHead;
  m_RtfHead = NULL;
  delete m_RtfFont;
  m_RtfFont = NULL;
  delete m_RtfColor;
  m_RtfColor = NULL;
  delete m_RtfStyle;
  m_RtfStyle = NULL;
  delete m_Style;
  m_Style = NULL;

  return addr;
}

long
CFormatText::GetTextSize (void)
{
  long          l = 0;
  CParaghLink  *pTrace;

  pTrace = m_pBeginParagh;
  while (pTrace)
  {
    l += pTrace->m_CbdTx->m_CsRtf.GetLength ();
    pTrace = pTrace->m_pNext;
  }

  return l;
}

long
CFormatText::GetText (char huge *pText)
{
  int         nLength;
  long        addr = 0;
  CParaghLink *pTrace, *pPrev;
  
  pTrace = m_pBeginParagh;
  while (pTrace)
  {
    nLength = pTrace->m_CbdTx->m_CsRtf.GetLength();
    if (pText != NULL)
      _fmemcpy (&pText [addr], pTrace->m_CbdTx->m_CsRtf.GetBuffer(1), nLength);
    addr += nLength;
    pPrev = pTrace;
    pTrace = pTrace->m_pNext;
    pPrev->Clean();
    delete pPrev;
  }
  
  if (pText != NULL)
    pText [addr] = 0;
  
  m_pBeginParagh = m_pCurParagh = NULL;

  return addr;
}
// JRO Picture stuff
void
CFormatText::PictureStart (void)
{
  AddControlWord ("\\pict");
}

void
CFormatText::PictureMeta (int type)
{
  CString Meta = "\\wmetafile";  
  Meta += IntToStr (type);
  AddControlWord (Meta);
}

void
CFormatText::PictureDib ()
{
  AddControlWord ("\\dibitmap0");
}

void
CFormatText::PictureDdb ()
{
  AddControlWord ("\\wbitmap0");
}

void
CFormatText::PictureBmp (long bpp, long pln, long wdth)
{
  CString Bmp = _T("\\wbmbitspixel");
  Bmp += IntToStr ((int)bpp);
  Bmp += _T("\\wbmplanes");
  Bmp += IntToStr ((int)pln);
  Bmp += _T("\\wbmwidthbytes");
  Bmp += IntToStr ((int)wdth);

  AddControlWord (Bmp);
}

void
CFormatText::PictureMetaBmp (int BPP)
{
  CString Bmp = "\\picbmp";

  if (BPP > 0)
  {
    Bmp += "\\picbpp";
    Bmp += IntToStr (BPP);
  }
  AddControlWord (Bmp);
}

void
CFormatText::PictureWidth (DWORD pixels)
{
  CString Width = "\\picw";
  Width += IntToStr ((int)pixels);
  AddControlWord (Width);
}

void
CFormatText::PictureHeight (DWORD pixels)
{
  CString Height = "\\pich";
  Height += IntToStr ((int)pixels);
  AddControlWord (Height);
}

void
CFormatText::PictureScaleX (int scale)
{
  CString Scale = "\\picscalex";
  Scale += IntToStr (scale);
  AddControlWord (Scale);
}

void
CFormatText::PictureScaleY (int scale)
{
  CString Scale = "\\picscaley";
  Scale += IntToStr (scale);
  AddControlWord (Scale);
}

void 
CFormatText::PictureGoalX (DWORD twips)
{
  CString Goal = "\\picwgoal";
  Goal += IntToStr ((int)twips);
  AddControlWord (Goal);
}

void 
CFormatText::PictureGoalY (DWORD twips)
{
  CString Goal = "\\pichgoal";
  Goal += IntToStr ((int)twips);
  AddControlWord (Goal);
}
//////////////////////////////////////////////////////////////////////
//  int  SetColor()
//  The function gets the color number on color table and sets to
//  current paragraph or Buff (if any).
//  Return:     number of the color on color table
//  Red:        red component of the color
//  Green:      green component of the color
//  Blue:       blue component of the color
//  Buff:       pointer to a CString buffer, default NULL
//  FB:         1 (default) for foreground color, 0 for background color
//////////////////////////////////////////////////////////////////////
int  CFormatText::SetColor(int Red, int Green, int Blue, CString * Buff,  int FB)
 {                                                                                       
  CString  SColor;                             
  int  colorNum;
  
  colorNum = AddColor(Red, Green, Blue);     
  if(colorNum >= 0)
  {          
   if(FB)                              //foreground color
    SColor = "\\cf";
   else
    SColor = "\\cb";
   SColor += IntToStr(colorNum);    
   if(Buff != NULL)
    * Buff += SColor;
   else
    AddControlWord(SColor.GetBuffer(1));
  }
  return colorNum;
 }
//////////////////////////////////////////////////////////////////////
//  void  SetFont()
//  Sets font on either the current paragraph or Buff (if any).
//  FontNum:    font number
//  Buff:       pointer to a CString buffer, default NULL
//////////////////////////////////////////////////////////////////////
int   CFormatText::SetFont(char *  Name, char *  Family, CString * Buff)
{
  CString SFont = "\\f";
  int FontNum = SetFontTable(Name, Family);

  if(FontNum >= 0)
  {
    SFont += IntToStr(FontNum);    
    if(Buff == NULL)
     AddControlWord(SFont.GetBuffer(1));
    else
     * Buff += SFont;
  }
  return FontNum;
}
//////////////////////////////////////////////////////////////////////
//  int  SetFontSize()
//  Sets font size on the current paragraph.
//  Return:     FT_OK if success
//              FTERR_PARAMETER for wrong font size
//  FontSize:   font number
//////////////////////////////////////////////////////////////////////
int  CFormatText::SetFontSize(int Size)
 {                                                                                       
  CString  SFont;

  Size *= 2;          // Convert to Half Point Size
  if(CheckFontSize(Size) != FTERR_PARAMETER)        
   {
    SFont = "\\fs";
    SFont += IntToStr(Size);
    AddControlWord(SFont.GetBuffer(1));
    return FT_OK;
   }                
  else
   return FTERR_PARAMETER;                                 
 }                                                             
/*
////////////////////////////////////////////////////////////////////// 
//  void  SetUnderline()
//  Sets or resets varies underline on the current paragraph.
//  flag:       FT_CONT_UNDER for continue underline
//              FT_DOT_UNDER for dotted underline
//              FT_DOUBLE_UNDER for double underline
//              FT_WORD_UNDER for word underline
//              FT_NO_UNDER for turnning off all underlines
////////////////////////////////////////////////////////////////////// 
void  CFormatText::SetUnderline(int  flag)
 {
  CString  SetChar;
              
  switch(flag)
   {            
    case  FT_CONT_UNDER:
      SetChar += "\\ul";               //continue underline 
      break;
    case  FT_DOT_UNDER:
      SetChar += "\\uld";              //dotted underline 
      break;
    case  FT_DOUBLE_UNDER:
      SetChar += "\\uldb";             //double underline
      break;
    case   FT_WORD_UNDER:
      SetChar += "\\ulw";              //word underline
      break;
    case  FT_NO_UNDER:
      SetChar += "\\ul0";              //turn off all underlines          
      break;
    default:     
      break;
   }             
  AddControlWord(SetChar.GetBuffer(1));
 }
*/
////////////////////////////////////////////////////////////////////// 
//  int  InsertTextInTabs()
//  Inserts text before the "Tabs" tab on the current paragraph.
//  Return:     FT_OK if success
//              FTERR_PARAMETER wrong parameter
//              FT_FAIL other error
//  Text:       text to be insert
//  Tabs:       number of the tab to insert text
////////////////////////////////////////////////////////////////////// 
int CFormatText::InsertTextInTabs(char * Text, int Tabs)
{
 CString Buff = m_pCurParagh->m_CbdTx->m_CsRtf;       //working on current paragraph
 CString SubBuff;
 CString LeftBuff, RightBuff;
 int  LengthBuff, LengthRight;
 int  Begin = 0, LocaTabs;
 int  tabs = Tabs;

 if(Tabs < 0)
   return FTERR_PARAMETER;
 if((LengthBuff = Buff.GetLength()) < 0)
   return FTERR_PARAMETER;
 RightBuff = Buff;
 LengthRight = RightBuff.GetLength();
 
 if(tabs == 0)
 {
   LocaTabs = RightBuff.Find("\\tab");//location on ParaBuff after this "tab"
   if(LocaTabs < 0)                    //no tabs found, add to beginning
     LocaTabs = 0;
   LengthRight -= LocaTabs;
   if(LengthRight < 0)
     return FT_FAIL;
   Begin += LocaTabs;
   RightBuff = RightBuff.Right(LengthRight);
 }
 else 
 while(tabs > 0)
  {
   LocaTabs = RightBuff.Find("\\tab");//location on ParaBuff after this "tab"
   if(LocaTabs < 0)                    //no tabs found, add to beginning
     break;
   LengthRight -= LocaTabs + 4;
   if(LengthBuff < 0)
     return FT_FAIL;
   Begin += LocaTabs + 4;
   RightBuff = RightBuff.Right(LengthRight);
   tabs --;
  }
 Buff = Buff.Left(Begin);  
 Buff += "{";
 Buff += Text;
 Buff += "}";
 Buff += RightBuff;
 //m_pCurParagh->m_CbdTx->m_CsRtf = Buff;
 *m_pCurParagh->m_CbdTx = Buff;
 return FT_OK;
}

int  CFormatText::FindParaBegin(CString * ParaBuff)
{
 CString ParaRight;
 char * KeyStrings[10];
 int  ii, ParaBegin, LengthBuff = ParaBuff->GetLength();

 KeyStrings[0] = "\\tq";
 KeyStrings[1] = "\\tx";
 KeyStrings[2] = "\\sl";
 KeyStrings[3] = "\\ri";
 KeyStrings[4] = "\\li";
 KeyStrings[5] = "\\qc";
 KeyStrings[6] = "\\qj";
 KeyStrings[7] = "\\qr";
 KeyStrings[8] = "\\ql";
 KeyStrings[9] = "\\par";

 for(ii=0; ii<10; ii++)
  if((ParaBegin = ParaBuff->Find(KeyStrings[ii])) >= 0)
   {
    ParaRight = ParaBuff->Right(LengthBuff - ParaBegin);
    ParaBegin += ParaRight.Find(" ") + 1;
    return ParaBegin;
   }
 return FTERR;   
}

/*
int  CFormatText::FindParaBegin(CString * ParaBuff, int Paras)
{
 CString BuffRight, BuffLeft = * ParaBuff;
 int  ii = Paras, InsertBegin, LengthLeft, LengthRight;

 while(ii > 0)
  {
   LengthLeft = BuffLeft.GetLength();
   InsertBegin = BuffLeft.ReverseFind('\\');
   if(InsertBegin < 0)
     return FTERR;
   LengthRight = LengthLeft - InsertBegin;
   if(LengthRight < 0)
     return FTERR;  
   LengthLeft = InsertBegin;
   BuffRight = BuffLeft.Right(LengthRight);
   BuffLeft = BuffLeft.Left(LengthLeft);   
   if(BuffRight.Find("\\par") >= 0)
     ii --;
  } 
 return InsertBegin;   
}
*/
////////////////////////////////////////////////////////////////////// 
//  void  ResetIndts()
//  Reset first inditation and left indication on global or a specific
//  paragraph header.
//  FirstIndt:  first inditation in inch
//  LeftIndt:   left inditation in inch
//  pParagh:    pointer to a specific paragraph header, default NULL
////////////////////////////////////////////////////////////////////// 
void  CFormatText::ResetIndts(float FirstIndt, float LeftIndt, CParaghLink * pParagh)
{
 ResetFirstIndt(FirstIndt, pParagh);
 ResetLeftIndt(LeftIndt, pParagh);
}
////////////////////////////////////////////////////////////////////// 
//  void  ResetFirstIndt()
//  Reset first inditation on global or a specific paragraph header.
//  FirstIndt:  first inditation in inch
//  pParagh:    pointer to a specific paragraph header, default NULL
////////////////////////////////////////////////////////////////////// 
void CFormatText::ResetFirstIndt(float FirstIndt, CParaghLink * pParagh)
{
 int length, left, right;
 CString Left, Right;
 CString fInd = "\\fi" + IntToStr((int)(FirstIndt * FT_RTF_TWIPS)) + " ";
 if(FirstIndt < -FT_MARG_TAB_IND || FirstIndt > FT_MARG_TAB_IND)
  return;
 if(!pParagh)
  pParagh = m_pBeginParagh;
 
 //add or replace first inditation
 length = pParagh->m_CbdTx->m_CsRtf.GetLength();
 if((left = pParagh->m_CbdTx->m_CsRtf.Find("\\fi")) >= 0)  //"\fi" not found, add new one
 {
  Left = pParagh->m_CbdTx->m_CsRtf.Left(left);   //the part just before "\fiN"
  right = length - left - 1;                     //skip the first "\"
  Right = pParagh->m_CbdTx->m_CsRtf.Right(right);//the part starts with "fiN"
  left = Right.Find(" ") + 1;                    //location just next to "\fiN "
  right -= left;
  Right = Right.Right(right);                    //the part just after "\fiN"
  *pParagh->m_CbdTx = Left + fInd + Right;
 }
 else
 {
  CString  csLeft, csRight;
  int  nIndex, nLength;
  
  nLength = pParagh->m_CbdTx->m_CsRtf.GetLength();
  if((nIndex = pParagh->m_CbdTx->m_CsRtf.Find("\\pard")) >= 0)
   {
    nIndex += 5;                                 //skip "\pard"
    csLeft = pParagh->m_CbdTx->m_CsRtf.Left(nIndex);
    csRight = pParagh->m_CbdTx->m_CsRtf.Right(nLength - nIndex);
    pParagh->m_CbdTx->m_CsRtf = csLeft + fInd + csRight;
   }
  else 
   pParagh->m_CbdTx->m_CsRtf = "\\pard" + fInd + pParagh->m_CbdTx->m_CsRtf;
 }
} 
////////////////////////////////////////////////////////////////////// 
//  void  ResetLeftIndt()
//  Reset left indication on global or a specific paragraph header.
//  LeftIndt:   left inditation in inch
//  pParagh:    pointer to a specific paragraph header, default NULL
////////////////////////////////////////////////////////////////////// 
void CFormatText::ResetLeftIndt(float LeftIndt, CParaghLink * pParagh)
{
 int length, left, right;
 CString Left, Right;

 if(LeftIndt < -FT_MARG_TAB_IND || LeftIndt > FT_MARG_TAB_IND)
  return;
 if(!pParagh) 
  pParagh = m_pBeginParagh;
 //add or replace left inditation
 CString lInd = "\\li" + IntToStr((int)(LeftIndt * FT_RTF_TWIPS)) + " ";
 length = pParagh->m_CbdTx->m_CsRtf.GetLength();
 if((left = pParagh->m_CbdTx->m_CsRtf.Find("\\li")) >= 0)  //"\li" not found, add new one
 {
  Left = pParagh->m_CbdTx->m_CsRtf.Left(left);   //the part just before "\liN"
  right = length - left - 1;                     //skip the first "\"
  Right = pParagh->m_CbdTx->m_CsRtf.Right(right);
  left = Right.Find(" ") + 1;                    //location just next to "\liN "
  right -= left;
  Right = Right.Right(right);                    //to part just after "\liN"
  *pParagh->m_CbdTx = Left + lInd + Right;
 }
 else
 {
  CString  csLeft, csRight;
  int  nIndex, nLength;
  
  nLength = pParagh->m_CbdTx->m_CsRtf.GetLength();
  if((nIndex = pParagh->m_CbdTx->m_CsRtf.Find("\\pard")) >= 0)
   {
    nIndex += 5;                                 //skip "\pard"
    csLeft = pParagh->m_CbdTx->m_CsRtf.Left(nIndex);
    csRight = pParagh->m_CbdTx->m_CsRtf.Right(nLength - nIndex);
    pParagh->m_CbdTx->m_CsRtf = csLeft + lInd + csRight;
   }
  else 
   pParagh->m_CbdTx->m_CsRtf = "\\pard" + lInd + pParagh->m_CbdTx->m_CsRtf;
 }
}
//===========================================================================
void  CFormatText::SetIndts(float FirstIndt, float LeftIndt)
{
 SetFirstIndt(FirstIndt);
 SetLeftIndt(LeftIndt);
}
  
void CFormatText::SetFirstIndt(float FirstIndt)
{
 if(FirstIndt < -FT_MARG_TAB_IND || FirstIndt > FT_MARG_TAB_IND)
  return;
 
 *m_pCurParagh->m_CbdTx += "\\fi";
 *m_pCurParagh->m_CbdTx += IntToStr((int)(FirstIndt * FT_RTF_TWIPS));
 *m_pCurParagh->m_CbdTx += " ";
} 

void CFormatText::SetLeftIndt(float LeftIndt)
{
 if(LeftIndt < -FT_MARG_TAB_IND || LeftIndt > FT_MARG_TAB_IND)
  return;

 *m_pCurParagh->m_CbdTx += "\\li";
 *m_pCurParagh->m_CbdTx += IntToStr((int)(LeftIndt * FT_RTF_TWIPS));
 *m_pCurParagh->m_CbdTx += " ";
}
////////////////////////////////////////////////////////////////////// 
//  int  ResetTabPosi()
//  Reset tab position on global or a specific paragraph header.
//  TabPosi:    tab position in inch
//  Num:        number of the tab to set position
//  pParagh:    pointer to a specific paragraph header, default NULL
////////////////////////////////////////////////////////////////////// 
int CFormatText::ResetTabPosi(float TabPosi, int seqNum, int Tbk, CParaghLink * pParagh)
{
 //CString TX = "\\tx" + IntToStr((int)(TabPosi * (float)FT_RTF_TWIPS));
 CString TX = SetTab ((int)(TabPosi * (float)FT_RTF_TWIPS), Tbk);
 CString Left, Right;
 int  length, addr;
 int  left = 0, right= 0;
 int  count = 0;
 if(TabPosi < 0 || TabPosi > 6 || seqNum < 0)
  return FTERR_PARAMETER;
 if(!pParagh)
  pParagh = m_pBeginParagh;
 Right = pParagh->m_CbdTx->m_CsRtf;
 length = Right.GetLength();

 while(count <= seqNum)
 {
  if((addr = Right.Find("\\tx")) < 0)            //no existing "\txN", add a new one
  {
   if(count < seqNum)                            //can't add more than one "\tx"
    return FT_FAIL; 
   if(count == 0)                                          
   {
    if((addr = pParagh->m_CbdTx->m_CsRtf.Find("\\pard")) >= 0)
     addr += 5;                                    //skip first "\par \pard"
    else
     addr = pParagh->m_CbdTx->m_CsRtf.Find("\\par") + 4;    //skip first "\par"
    Left = Right.Left(addr);                     //left part includeing "\par"
    Right = Right.Right(length - addr);          //part right to "\par"
   }
   else
    Left = pParagh->m_CbdTx->m_CsRtf.Left(left);
   //pParagh->m_CbdTx->m_CsRtf = Left + TX + Right;         //insert "\txN "
   *pParagh->m_CbdTx = Left + TX + Right;         //insert "\txN "
   return FT_OK;
  }
  else                                           //skip or modify current "\tx"
  {
   left += addr;                                 //location just left to "\tx"
   right = length - left - 3;                    //location just right to "\tx"    
   Right = Right.Right(right);                   //the part right to "\tx" and including number
   addr = Right.FindOneOf(" \\");                //skip string of number
   right -= addr;
   Right = Right.Right(right);                   //the right part wighout number
   
   if(count == seqNum)                           //modify current "\txN"
   {
    //Left = Right.Left(left);                     //the part left to current "\txN"
    Left = pParagh->m_CbdTx->m_CsRtf.Left(left);
    //pParagh->m_CbdTx->m_CsRtf = Left + TX + Right;        //new "\txN" replaces old one
    *pParagh->m_CbdTx = Left + TX + Right;        //new "\txN" replaces old one
    return FT_OK;
   }
   else                                          //skip current "\txN"
    left = length - right;
  }
  count ++;
 }
 return FT_OK; 
}
////////////////////////////////////////////////////////////////////// 
//  int  SetParagraph()
//  Set major properties on the default or specified paragraph.
//  Align:      alignment
//  InFirst:    first inditation
//  InLeft:     left inditation
//  InRight:    right inditation
//  LinSp:      line spacing
//  Tab:        tab size
//  Tbk:        tab type
//  pParagh:    pointer to the paragraph to be set
////////////////////////////////////////////////////////////////////// 
int  CFormatText::SetParagraph(int Align, float InFirst, float InLeft, float InRight, float LineSp, float Tab, int Tbk, CParaghLink *pParagh)
{            
  CString xAL, xIN, xLS, xTB;        

  int     errorFlag = 0;

  if(pParagh == NULL)
   pParagh = m_pBeginParagh;
  
  xAL = SetAlignment(Align);
  if(!xAL.GetLength())
    errorFlag |= FTERR_ALIGN;
  xIN = SetIndentation(InFirst, InLeft, InRight);
  if(!xIN.GetLength())
    errorFlag |= FTERR_INDENT;
  xLS = SetLineSpacing(LineSp);
  if(!xLS.GetLength())
    errorFlag |= FTERR_LINESP;
  xTB = SetTab((int)(Tab * (float)FT_RTF_TWIPS), Tbk);
  if(!xTB.GetLength())
    errorFlag |= FTERR_TABS;
  if(errorFlag)
    return errorFlag;      
  //pParagh->m_CbdTx->m_CsRtf += AL + IN + LS + TB + " ";
  *pParagh->m_CbdTx += xAL + xIN + xLS + xTB + " ";
  return errorFlag;               //must be 0
 }            
////////////////////////////////////////////////////////////////////// 
//  void  SetMargins()
//  Set Document Margins.
//  fLeft:      left margin in inch
//  fRight:     rightt margin in inch
//  fTop:       top margin in inch
//  fBottom:    bottom margin in inch
////////////////////////////////////////////////////////////////////// 
void CFormatText::SetMargins(float fLeft, float fRight, float fTop, float fBottom)
{
  m_cMargins.SetLeftMargin(fLeft);
  m_cMargins.SetRightMargin(fRight);
  m_cMargins.SetTopMargin(fTop);
  m_cMargins.SetBottomMargin(fBottom);
}

/*
int  CFormatText::SetMargins(float fLeft, float fRight, float fTop, float fBottom)
 {        
  CString  Margin;  
  CString  Left = "\\margl";
  CString  Right = "\\margr";
  CString  Top = "\\margt";
  CString  Bottom = "\\margb";    
  int  left;
  int  right;
  int  top;
  int  bottom;         
  int  addr, length;                      
  int  errorFlag = 0;
  
  if(!pParagh)                           //global margin setting
    pParagh = m_pBeginParagh;
  if((addr = pParagh->m_CbdTx->m_CsRtf.Find("\\pard")) >= 0)
    addr += 5;                                   //skip "/pard"
  else  
   if((addr = pParagh->m_CbdTx->m_CsRtf.Find("\\par")) >= 0)
     addr += 4;                                  //skip "/par"
   else  
    return FT_FAIL;                                     
  
  //  Sep 26, 94  Begin
  CString  csTempMargin = pParagh->m_CbdTx->m_CsRtf;
  CString  csTemp;
  int  addrmb, addrme;
  int  nLMargin = csTempMargin.GetLength(), nLTemp;
  if((addrmb = csTempMargin.Find("\\margl")) < 0) //find begin of margines
     addrmb = addrme = addr;
  else
   {
     addrme = csTempMargin.Find("\\margb");       //find end of margins' defination
     csTemp = csTempMargin.Right(nLMargin - addrme);
     if((nLTemp = csTemp.Find(" ")) > 0)
       addrme += nLTemp + 1;
     else
       addrme += csTemp.GetLength();  
   }   
  //  Sep 26, 94  End
  
  if(pParagh != m_pBeginParagh)          //paragraph margin setting
   {
    Left += "sxn";
    Right += "sxn";
    Top += "sxn";
    Bottom += "sxn";
   }
     
  //converting intiger numbers to strings
  if(fLeft >= 0  &&  fLeft <= FT_MARG_TAB_IND)
   {
    left = CovLRMargin(fLeft);
    Left += IntToStr(left);  
    Margin += Left;
   }                                    
  else
    errorFlag |= FTERR_LEFT_MARGIN; 
  if(fRight >= 0  &&  fRight <= FT_MARG_TAB_IND)
   { 
    right = CovLRMargin(fRight);
    Right += IntToStr(right);
    Margin += Right;
   }                                                     
  else
   errorFlag |= FTERR_RIGHT_MARGIN; 
  if(fTop >= -FT_MARG_TAB_IND  && fTop <= FT_MARG_TAB_IND)
   {      
    top = CovTBMargin(fTop);
    Top += IntToStr(top);
    Margin += Top;
   }                                                       
  else
   errorFlag |= FTERR_TOP_MARGIN; 
  if(fBottom >= -FT_MARG_TAB_IND  && fBottom <= FT_MARG_TAB_IND)
   {           
    bottom = CovTBMargin(fBottom);
    Bottom += IntToStr(bottom);
    Margin += Bottom;
   }                                                      
  else
   errorFlag |= FTERR_BOTTOM_MARGIN; 
  if(Margin.GetLength())
    Margin += " ";
                                                  
  Left = pParagh->m_CbdTx->m_CsRtf.Left(addrmb);             // "addrmb" was "addr"   Sep 26, 94
  length = pParagh->m_CbdTx->m_CsRtf.GetLength();
  Right = pParagh->m_CbdTx->m_CsRtf.Right(length - addrme);  // "addrme" was "addr"   Sep 26, 94
  pParagh->m_CbdTx->m_CsRtf = Left + Margin + Right;
  return errorFlag;  
 }
*/
//////////////////////////////// protected member functions ////////////////////////////// 
void  CFormatText::AddDeftFont()
 {
     SetFont("Times New Roman", "Roman");
 }

void  CFormatText::AddDeftStyle()
 {
  * m_RtfStyle = "{\\stylesheet{\\f0\\fs20 \\snext0 Normal;}{\\*\\cs10 \\additive Default Paragraph Font;}}";
 }

void  CFormatText::AddDeftColor()      //setting a default 16-color color-table
 {   
   int  colors;         
   char * S;
   CString  S1 = "\\red0\\green0\\blue0;\\red0\\green0\\blue255;\\red0\\green255\\blue255;\\red0\\green255\\blue0;"; 
   S1 += "\\red255\\green0\\blue255;\\red255\\green0\\blue0;\\red255\\green255\\blue0;\\red255\\green255\\blue255;";
   S1 += "\\red0\\green0\\blue128;\\red0\\green128\\blue128;\\red0\\green128\\blue0;\\red128\\green0\\blue128;";
   S1 += "\\red128\\green0\\blue0;\\red128\\green128\\blue0;\\red128\\green128\\blue128;\\red192\\green192\\blue192;";
   
   S = S1.GetBuffer(1);
   colors = AddColor(16, S); 
 }            
 
int  CFormatText::AddColor(int numColors, CString  Colors)
 {         
  CString  S = * m_RtfColor;
  int  addr, addrend;
  int  length, slength;
  int  Counter = 0;

  length = m_RtfColor->GetLength();     //get length of the *m_pRtfWString
  if((addrend = S.Find(Colors)) > 0)   //color was found, count its number
   {
    slength = length - 1;
    while((addr = S.Find(";")) < addrend && addr > 0)    
    {
     slength -= addr + 1;
     addrend -= addr + 1;
     S = S.Right(slength);          
     Counter ++;          
    }
   } 
  else                                 //color not found, add at the end of color table
   {
    addrend = length - 1;
    S = S.Left(addrend);
    * m_RtfColor = S + Colors + "}";
    m_colorCounter += numColors;
    Counter = m_colorCounter - 1;
   }
  return Counter;
 }
 
int  CFormatText::AddColor(int red, int green, int blue)
 {                 
   CString  Scolor;
 if(red>=0 && red<=255 && green>=0 && green<=255 && blue>=0 && blue<=255)
 {  
   Scolor = "\\red";            
   Scolor += IntToStr(red);
   Scolor += "\\green";
   Scolor += IntToStr(green);
   Scolor += "\\blue";    
   Scolor += IntToStr(blue);
   Scolor += ";";
   return AddColor(1, Scolor);              
 }
 else
   return  FTERR;
 }               
 ///////////////////////////////////////////////////////////////////////////
 // This function convert an intiger number to a string,
 // attatch it to the end of "String", and return "String."
 ///////////////////////////////////////////////////////////////////////////
CString  CFormatText::IntToStr(int  number)
 {                                          
   int  dec, sig;
   CString IntString;
   
   if(number < 0)
    {
     IntString = "-";
     number = -number;
    }
   if(number == 0)
    IntString += '0';
   else   
    IntString +=  _fcvt(number, 0, &dec, &sig);
   return IntString;
 }
 
void  CFormatText::AddString(int addr, CString StringIns)
 {         
  CString Sl, Sr;
  int  length = m_pCurParagh->m_CbdTx->m_CsRtf.GetLength();
  
  Sl = m_pCurParagh->m_CbdTx->m_CsRtf.Left(addr);
  Sr = m_pCurParagh->m_CbdTx->m_CsRtf.Right(length-addr);
  Sl += StringIns;
  Sl += Sr;
  //m_pCurParagh->m_CbdTx->m_CsRtf = Sl;
  *m_pCurParagh->m_CbdTx = Sl;
 }
    
int  CFormatText::SetFontTable(char *  Name, char *  Family)
 {               
  CString  S, Sfont, SBfont;
  CString CSTemp;
  char * psTemp;
  int  addr;
  int  nLength;
  int  Counter = 0;
  
  if(Name == NULL)
    return 0;                                    //return default font index
  if(Family == NULL)
    Family = "prq2";                             // Specify Variable Pitch Font
    
  //build the font string 
  if(Family != NULL)
   {
    Sfont = "\\f";
    Sfont += Family;
   } 
  Sfont += " ";
  Sfont += Name;                               
  Sfont += ";}";
  
  //search the font in font table and count its number
  if((addr = m_RtfFont->Find(Sfont.GetBuffer(1))) < 0)//add a new font if not found
  {
   Counter = m_fontCounter;
   nLength = m_RtfFont->GetLength();
   * m_RtfFont = m_RtfFont->Left(nLength - 1);      //add to last location
   * m_RtfFont += "{\\f";                           //add font number
   * m_RtfFont += IntToStr(Counter);
   * m_RtfFont += Sfont;
   * m_RtfFont += "}";                             //add "}"
   m_fontCounter ++;
   return Counter;
  }
  else                                           //count the font number
  {
   CSTemp = m_RtfFont->Left(addr);                //get part of Font table left to that Font
   psTemp = CSTemp.GetBuffer(1);
   //search and count
   Counter = 0;
  while(1)
   {
    psTemp = &psTemp[1];                         //skip the first '{'
    psTemp = _fstrchr(psTemp, '{');
    if(psTemp == NULL)
     break;
    Counter ++; 
   }
  if(Counter) 
    Counter --;
  }
  return Counter;
}           
 //converting left/right margin in inch to working value
int  CFormatText::CovLRMargin(float  fmargin)
 {                                                                                              
  return  (int)(fmargin * FT_RTF_TWIPS);
 }
 //converting top/bottom margin in inch to working value
int  CFormatText::CovTBMargin(float  fmargin)
 {                                                                                              
  return  (int)(fmargin * FT_RTF_TWIPS);
 }          
 //converting tab in inch to working value
int  CFormatText::CovTab(float  ftab)
 {                                                                                              
  return  (int)(ftab * FT_RTF_TWIPS);
 }
int  CFormatText::CheckSuPosi(int Posi)
 {
  if(Posi < 0 || Posi > 50)
    return FTERR_SU_POSI;
  return FT_OK;  
 }
  
int  CFormatText::CheckFontSize(int Size)
 {
  if(Size < 0 || Size > 100)
    return FTERR_PARAMETER;
  return FT_OK;  
 }
 
CString  CFormatText::SetAlignment(int Align)
 {
  CString SAlign;
  
  switch(Align)
   {                                  
    case FT_LEFT_ALIGN:
       SAlign = "\\ql";
       break;
    case FT_RIGHT_ALIGN: 
       SAlign = "\\qr";                //right alligned
       break;
    case FT_JUST_ALIGN:
       SAlign = "\\qj";                //justified
       break;
    case FT_CENTER_ALIGN:
       SAlign = "\\qc";                //centered
       break;
    default:
       break;
   }
   return  SAlign;
 }
 
CString  CFormatText::SetTab(int Tab, int Tbk)
{
  CString STab;    
//  int  TAB;          
  
  switch(Tbk)
   {
    case FT_TQR:
      STab += "\\tqr";
      break;
    case FT_TQC:
      STab += "\\tqc";
      break;
    case FT_TQDEC:
      STab += "\\tqdec";
      break;
    default:
      break;  
   }
   
  if(Tab >= 0 && Tab <= FT_MARG_TAB_IND * FT_RTF_TWIPS)
   {
//    TAB = (int)(Tab * FT_RTF_TWIPS);     
    STab += "\\tx";
    STab += IntToStr(Tab);
   }                                                
  return STab; 
}
 
CString  CFormatText::SetLineSpacing(float LineSp)
 {
  CString  SLineSp;
  int  LSP;                        
  
  if(LineSp >= 0 && LineSp <= 6.6)
   {
    LSP = (int)(LineSp * FT_RTF_TWIPS / 6);                       
    SLineSp += "\\sl";
    SLineSp += IntToStr(LSP);
   }
  return  SLineSp; 
 } 

CString  CFormatText::SetIndentation(float InFirst, float InLeft, float InRight)
 {
  CString SIndent;
  int Left, Right;

  if(InFirst >= -FT_MARG_TAB_IND && InFirst <= FT_MARG_TAB_IND)               
   {
    Left = (int)(InFirst * FT_RTF_TWIPS);
    SIndent += "\\fi";
    SIndent += IntToStr(Left);
   }
  if(InLeft >= -FT_MARG_TAB_IND && InLeft <= FT_MARG_TAB_IND)               
   {
    Left = (int)(InLeft * FT_RTF_TWIPS);
    SIndent += "\\li";
    SIndent += IntToStr(Left);
   }
  if(InRight >= -FT_MARG_TAB_IND && InRight <= FT_MARG_TAB_IND)
   {
    Right = (int)(InRight * FT_RTF_TWIPS);
    SIndent += "\\ri";
    SIndent += IntToStr(Right);
   }
  return SIndent;
 }
 
void  CFormatText::SetSpacings(int Type, int spacing)
 {            
  int  addr = 10;  ///////////////////////////////////
  CString  Spacing;
  
  switch(Type)
   {
    case  1:
      Spacing = "\\sb";                //spacing before
      break;
    case  2:
      Spacing = "\\sa";                //spacing after
      break;                
    default:
      Spacing = "\\sl";                //line spacing
   }
  spacing = min(132, spacing);
  spacing = max(0, spacing);
  Spacing += IntToStr(spacing);
  AddString(addr, Spacing);
 }  
 
void  CFormatText::SetParaStyle(Paragraph  Para)
  {
   CString  ParaStyle;
   ////////////////////////////  for Alignment  /////////////////////////////// 
          switch(Para.m_Alit)
           {
            case 0:
                    ParaStyle = "\\ql";          //left alignment
                    break;

            case  1:
                    ParaStyle = "\\qr";          //right alignment
                    break;

            case  2:
                    ParaStyle = "\\qj";          //justited alignment
                    break;

            case  3:
                    ParaStyle = "\\qc";          //centered alignment
                    break;
            }

 /////////////////////////////  for Indentation  //////////////////////////////////
 if(Para.m_fi >= -FT_MARG_TAB_IND && Para.m_fi <=FT_MARG_TAB_IND)
   {
    ParaStyle += "\\fi";        //left indentation
    ParaStyle += IntToStr(Para.m_fi);
  } 
 if(Para.m_li >= -FT_MARG_TAB_IND && Para.m_li <=FT_MARG_TAB_IND)
   {
    ParaStyle += "\\li";        //left indentation
    ParaStyle += IntToStr(Para.m_li);
   }    
 if(Para.m_ri >= -FT_MARG_TAB_IND && Para.m_ri <=FT_MARG_TAB_IND)
   {
    ParaStyle += "\\ri";        //left indentation
    ParaStyle += IntToStr(Para.m_ri);
   }
 /////////////////////////////////  for Line Spacing  //////////////////////////////////
 if(Para.m_Space > 0 && Para.m_Space < 7)
  {
   ParaStyle += "\\sl";
   ParaStyle += IntToStr(Para.m_Space);
  }
  if(ParaStyle.GetLength() > 0)
   ParaStyle += " ";    
  
//  * m_RtfStyle += ParaStyle;
  m_Style->m_ParaStyle = ParaStyle;
}
////////////////////////////////  CFontStyle  Class  //////////////////////////////////////
void  CFormatText::SetFontStyle(Fonts  Font)
{
 CString  FontStyle;
 char * Name = NULL;
 char * Family = NULL;
 int  fN;
 //font number
 if(Font.m_Name.GetLength() > 0)
   Name = Font.m_Name.GetBuffer(1);
 if(Font.m_Family.GetLength() > 0)
   Family = Font.m_Family.GetBuffer(1);
 if((fN = SetFontTable(Name, Family)) >= 0)
  {
   FontStyle +="\\f";
   FontStyle += CFormatText::IntToStr(fN);
   FontStyle += " ";
  }
 Font.m_Name.ReleaseBuffer();
 Font.m_Family.ReleaseBuffer(); 
 //font size 
 if(Font.m_Size > 0)
  {
   FontStyle +="\\fs";
   FontStyle += CFormatText::IntToStr(Font.m_Size * 2);  //convert point-size to half point
   FontStyle += " ";
  }
 //bold and/or italic style
 if(Font.m_bolditalic & 0x0001)
   FontStyle +="\\b";
 else
  if(Font.m_bolditalic & 0x0002)
   FontStyle +="\\b0";
 if(Font.m_bolditalic & 0x0004)
   FontStyle +="\\i";
 else
  if(Font.m_bolditalic & 0x0008)
   FontStyle +="\\i0";
 //underlines
  switch(Font.m_underline)
   {            
    case  BT_CONT_UNDER:
      FontStyle += "\\ul";             //continue underline
      break;
    case  BT_DOT_UNDER:
      FontStyle += "\\uld";            //dotted underline 
      break;
    case  BT_DOUBLE_UNDER:
      FontStyle += "\\uldb";           //double underline
      break;
    case   BT_WORD_UNDER:
      FontStyle += "\\ulw";            //word underline
      break;
    case  BT_NO_UNDER:
      FontStyle += "\\ul0";            //turn off all underlines          
      break;
    default:     
      break;
   }             
  
//  * m_RtfStyle += FontStyle;
 m_Style->m_FontStyle = FontStyle;      //set m_Style 
}  
//////////////////////////////  CColorStyle  Class  ////////////////////////////////////////
int  CFormatText::SetColorStyle(Color  Colors)
{ 
 CString  ColorStyle;
 int  red, green, blue;
 int  colorN;
 //for foreground color
 if(Colors.m_ForeColor != FT_DEFT_COLOR) //foreground color is set
  {
   red = GetRValue(Colors.m_ForeColor);
   green = GetGValue(Colors.m_ForeColor);
   blue = GetBValue(Colors.m_ForeColor);
   if((colorN = AddColor(red, green, blue)) == FTERR)
     return  FTERR; 
   ColorStyle += "\\cf";
   ColorStyle += CFormatText::IntToStr(colorN);
   ColorStyle += " ";
  }
 if(Colors.m_BackColor != FT_DEFT_COLOR) //foreground color is set
 { 
   red = GetRValue(Colors.m_BackColor);
   green = GetGValue(Colors.m_BackColor);
   blue = GetBValue(Colors.m_BackColor);
   if((colorN = AddColor(red, green, blue)) == FTERR)
     return  FTERR; 
   ColorStyle += "\\cb";
   ColorStyle += CFormatText::IntToStr(colorN);
   ColorStyle += " ";
  }
   m_Style->m_ColorStyle = ColorStyle; 
   return FT_OK;                           //success 
}
//  YX-1    Table function
void CFormatText::SetBorder(CString * Border, int BdStyle, int BdWidth, int BdColor)
{   
    switch(BdStyle)                              //Border style
      {
        case  BORDER_BOX:
            * Border += "\\box";
            break;
        case  BORDER_SINGLETH:
            * Border += "\\brdrs";
            break;
        case  BORDER_DOUBLETH:
            * Border += "\\brdrth";
            break;
        case  BORDER_SHADOW:
            * Border += "\\brdrsh";
            break;
        case  BORDER_DOUBLE:
            * Border += "\\brdrdb";
            break;
        case  BORDER_DOT:
            * Border += "\\brdrdot";
            break;
        case  BORDER_DASH:
            * Border += "\\brdrdash";
            break;
        case  BORDER_HAIR:
            * Border += "\\brdrhair";
            break;
        default:
            break;
      }
    * Border += "\\brdrw" + IntToStr(BdWidth);     //Border width
    if(BdColor > 0)
      * Border += "\\brdrcf" + IntToStr(BdColor);  //Border color
}
//  YX-1    End of Changes
// 
/* 
//////////////////////////////////////////  Style Sheet //////////////////////////////////////////////
int  CFormatText::AddStyle(char * StyleName, Fonts &SFont, Color &SColor, Paragraph &SPara,  int Update)
{
 int  bufflength;
 int  Begin, Count;
 CString  buffLeft, buffRight;
 CString  style, Style = "{";
 CString  StyleSheet = GetStyleSheet(&Begin, &Count);
  
  if(StyleName == NULL)
     return  FTERR_NO_NAME;
//////////////////////////// Call DialogBox to initialize m_Style, then ... ///////////////////////////
  if(StyleSheet.Find(StyleName) <= 0)            //the style is not existing, add it
   {
    //initialize m_Style
    m_Style.m_StyleName = StyleName;
    SetParaStyle(SPara);
    style += m_Style.m_ParaStyle;
    SetFontStyle(SFont);
    style += m_Style.m_FontStyle;
    SetColorStyle(SColor);
    style += m_Style.m_ColorStyle;
    if((bufflength = style.GetLength()) <= 0)    //no style to add
       return  FTERR_NO_STYLE;
    Style += style;
//    Style += "\\sbasedon222 ";
    Style += m_Style.m_StyleName;
    Style += ";}";                               //string to be add to stylesheet table
    StyleSheet += Style;
    GetStyleSheet(&Begin, &Count);               //get the entrys only
    return  AddToStyleTable(StyleSheet, Begin, Count);
   } 
  return  FTERR_STYLE_EXIST;
}

void  CFormatText::AddToStyleTable(CString  StyleSheet, int Begin, int Count)
{
 CString  buffLeft, buffRight;
 int  bufflength;
 bufflength = * m_RtfStyle.GetLength();
 buffLeft = * m_RtfStyle.Left(Begin);
 buffRight = * m_RtfStyle.Right(bufflength - Begin - Count);
 * m_RtfStyle = buffLeft;
 * m_RtfStyle += StyleSheet;
 * m_RtfStyle += buffRight;
}
//////////////////////////////////////////////////////////////////////
//  int  SetDeftTab()
//  Sets default tab width.
//  FontNum:    font number
//  Buff:       pointer to a CString buffer, default NULL
//////////////////////////////////////////////////////////////////////
int CFormatText::SetDeftTab(float Tabdft)
{
 int TAB;
 CString STab;
  if(Tabdft >= 0 && Tabdft <= FT_MARG_TAB_IND)
   {
    TAB = (int)(Tabdft * FT_RTF_TWIPS);     
    STab += "\\deftab";
    STab += IntToStr(TAB);
   }
  else
   return FTERR_PARAMETER;                                                 
 AddControlWord(STab.GetBuffer(1));
 return FT_OK;
}

int  CFormatText::SelectStyle(char * StyleName, int reset)
{
 int  headlength, bufflength;
 int  Begin, Count;
 int  count = 0;
 CString  buff1, buff2, buffStyle;         
 
 if(reset)
 { 
   EndStyle();                                   //ending last style
   buffStyle += "{\\plain ";                     //reset plain style
 }
 else
   buffStyle += "{";                             //beginning this style
 m_flagStyle += 1;  
 buff1 = GetStyleSheet(&Begin, &Count);
 //find the style in stylesheet table
 headlength = buff1.Find(StyleName);
 if(headlength > 0)                              //the style is found
  {
   buff2 = buff1.Left(headlength);               //now, the last style in buff1 is the style that found
   bufflength = buff2.GetLength();               //length of stylesheet buffer
   headlength = buff2.ReverseFind('{') + 1;      //beginning of this style
   buff1 = buff2.Right(bufflength - headlength); //buff1 contains that style only
   bufflength = buff1.GetLength();
   if((headlength = buff1.Find("\\sbasedon")) <= 0) //clean "garbage"
      headlength = buff1.Find("\\snext");
   if(headlength > 0)                            //found one of the strings
     buffStyle += buff1.Left(headlength);            
   else
     buffStyle +=buff1;         
   AddControlWord(buffStyle.GetBuffer(1));              //write the style to text
   buffStyle.ReleaseBuffer();
   return FT_OK;                                     //success     
  }                                                                                    
 return  FTERR_NO_STYLE;
}

CString  CFormatText::GetStyleSheet(int * Begin, int * Count)
{
 CString  buff, buffStyle;
 int bufflength = * m_RtfStyle.GetLength();    //length of the whole rtf buffer
 * Begin = * m_RtfStyle.Find("stylesheet");    //length of header before style sheet
 buff = * m_RtfStyle.Right(bufflength - * Begin); //get right part of RTF buffer starting from stylesheet
 * Count = buff.Find("}}") + 1;
 buffStyle =  buff.Left(*Count);                 //now, buff contains the whole stylesheet 
 return buffStyle;
}

void  CFormatText::EndStyle(int  endbrace)
{
 int  ii;
 if(m_flagStyle == 0)                            //check if a style is "open"
   return;
 AddControlWord(" \\par ");
 for(ii=0; ii<endbrace; ii++)
  {
   AddControlWord("}");
   m_flagStyle -= 1;
   if(m_flagStyle == 0)
     break;
  }
}

//add temp buffer to main buffer
void CFormatText::AddToBuff(CString * CFT, int Clean)
{
 m_pCurParagh->m_CbdTx += * CFT;
 if(Clean)
  CFT->Empty();
}
*/ 
//cleaning member variables of CFormatText object
/*
void CFormatText::BuffClean()
{
  m_colorCounter = 0;
  m_fontCounter = 0;
  m_flagStyle = 0;
  m_pRtfWString->Empty();
}

inline void CFormatText::SetFont(char * Name, char * Family, CString * Buff)
 {
  char * family = NULL;
  if(Family)
   family = _fstrlwr(Family);                                                            
  int fontNum = SetFont(Name, family);
  SetFont(fontNum, Buff);
 }

inline void  CFormatText::SetCurParagh(CParaghLink * pCurParagh)
{
 if(pCurParagh)
  m_pCurParagh = pCurParagh;
 else
  while(m_pCurParagh->m_pNext)
   m_pCurParagh = m_pCurParagh->m_pNext;
}
*/
/*
int  CFormatText::GetMarginEntry(int global)
 {                                                                                                      
  CString  s;
  int  addr, addrs;             
  
  addr = * m_RtfStyle.GetLength();
  addrs = * m_RtfStyle.Find("stylesheet");    
  addr -= addrs;
  if(!global || addr < 0)
    return -1;                                         
  s = * m_RtfStyle.Right(addr);   
  if((addr = s.Find("}}")) < 0)
     return addr;
  return addr + addrs + 2;      
 }
*/
