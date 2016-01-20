#ifndef   RTF_H
#define   RTF_H
/****************************************************************************
  CBodyText -  This class is intended for the creation and manipulations of
                RTF substrings that do not require access to or modification
                of the RTF header.
                It also tracks the BEGIN ('{') and END ('}') RTF operators 
                in an effort to make them always compliment.
  
  Revisions:
            04-17-95  JRO     Added SetHidden ()
            03-22-95  JRO     Added access to CString "Find" function    
            08-14-94  CREATED - John Okerblom
****************************************************************************/
//---------------------------------------------------------------------------
enum UnderLineType
{
  BT_CONT_UNDER,                    //continue underline
  BT_DOT_UNDER,                     //dotted underline
  BT_DOUBLE_UNDER,                  //double underline
  BT_WORD_UNDER,                    //word underline
  BT_NO_UNDER,                      //turn off all underline
};

class CBodyText
{
  public:
    CBodyText ();             // Construction
    CBodyText (char *str);    // With assignment
    CBodyText (CString *str); // With assignment
//---------------------------------------------------------------------------
    void          
    Empty (void);                 // Make empty string
    int
    GetLength (void);
    char
    GetAt (int idx); 
    int
    Find (char ch);
    int
    Find (const char *str);
// RTF ----------------------------------------------------------------------
    void          
    Begin (int cnt = 1);          // Add '{'
    void          
    End (int cnt);                // Add cnt '}'s
    void          
    End (void);                   // Compliment '{'s with '}'s
    void          
    SetBold (void);               // Add '\b'
    void          
    SetItalic (void);             // Add '\i'
    void          
    SetSuper (void);              // Add '\super'
    void          
    SetSub (void);                // Add '\sub'
    void
    SetHidden (void);             // Add '\v'
    void          
    SetUnderline (enum UnderLineType = BT_CONT_UNDER);
    void          
    SetCenter (void);
    void          
    AddTab (int num = 1);
    void          
    NewLine (int num = 1);
//---------------------------------------------------------------------------
    CBodyText*   
    operator = (char *Src);        // Assignment
    CBodyText*   
    operator = (CString *Src);     // Assignment
    CBodyText*   
    operator = (CString Src);      // Assignment
    CBodyText*   
    operator = (CBodyText *Src);   // Assignment
    
    CBodyText*   
    operator += (char Src);        // Append
    CBodyText*   
    operator += (char *Src);       // Append
    CBodyText*   
    operator += (CString *Src);    // Append
    CBodyText*   
    operator += (CString Src);     // Append
    CBodyText*   
    operator += (CBodyText *Src);  // Append
    operator const char * () const
    {
      return (const char *)m_CsRtf;
    };
    CBodyText*   
    Append (unsigned char *Src);
    CBodyText*
		AddUncountedBrace (char Src);
//---------------------------------------------------------------------------
    CString *     
    Copy (CString *dest);         // Reproduce
    void          
    Move (CString *dest);         // End, reproduce and destroy
    char *        
    Copy (char *dest);            // Reproduce
    void          
    Move (char *dest);            // End, reproduce and destroy
    void
    MakeUpper (void);
//---------------------------------------------------------------------------
  public:
    CString       m_CsRtf;
  private:
    int           m_nCurlyCount;
    void          
    Init (void);
    int           
    GetCurlyCount (CString *cs);
    int           
    GetCurlyCount (const char *cp);
    void          
    SetCurlyCount (void);
};

//////////////////// Inline Functions /////////////////////
inline  CBodyText* 
CBodyText::operator = (char *Src)
{
  m_CsRtf = (const char *)Src;
  SetCurlyCount ();
  return this;
}
inline  CBodyText* 
CBodyText::operator = (CString *Src)
{
  m_CsRtf = *Src;
  SetCurlyCount ();
  return this;
}
inline  CBodyText* 
CBodyText::operator = (CString Src)
{
  m_CsRtf = Src;
  SetCurlyCount ();
  return this;
}
inline  CBodyText* 
CBodyText::operator = (CBodyText *Src)
{
  m_CsRtf = *(Src->m_CsRtf);
  SetCurlyCount ();
  return this;
}
inline  CBodyText* 
CBodyText::operator += (char Src)
{
  int	i;
  i = m_CsRtf.GetLength ();
  if ((i < 1) || (((const char *)m_CsRtf) [i - 1] != (char)'\\'))
	{
	  if (Src == '{')
	    m_nCurlyCount ++;
	  else
	  if (Src == '}')
	    m_nCurlyCount --;
	}
                  
  m_CsRtf += Src;
  return this;
}
inline  CBodyText* 
CBodyText::operator += (char *Src)
{
  m_nCurlyCount += GetCurlyCount ((const char *)Src);
  m_CsRtf += (const char *)Src;
  return this;
}
inline  CBodyText* CBodyText::operator += (CString *Src)
{
  m_nCurlyCount += GetCurlyCount (Src);
  m_CsRtf += *Src;
  return this;
}
inline  CBodyText* 
CBodyText::operator += (CString Src)
{
  m_nCurlyCount += GetCurlyCount (&Src);
  m_CsRtf += Src;
  return this;
}
inline  CBodyText* 
CBodyText::operator += (CBodyText *Src)
{
  m_nCurlyCount += GetCurlyCount (&Src->m_CsRtf);
  m_CsRtf = Src->m_CsRtf;
  return this;
}
inline  CBodyText* 
CBodyText::Append (unsigned char *Src)
{
  m_nCurlyCount += GetCurlyCount ((const char *)Src);
  m_CsRtf += (const char *)Src;
  return this;
}
//---------------------------------------------------------------------------
inline  CString * 
CBodyText::Copy (CString *dest)
{
  *dest = m_CsRtf;  
  return dest;
}
inline  void  
CBodyText::Move (CString *dest)
{
  End ();
  *dest = m_CsRtf;  
  Init ();
}
inline  char * 
CBodyText::Copy (char *dest)
{
  dest = (char *)(const char *)m_CsRtf;  
  return dest;
}
inline  void  
CBodyText::Move (char *dest)
{
  End ();
  dest = (char *)(const char *)m_CsRtf;  
  Init ();
}
inline  void  
CBodyText::Empty (void)
{
  Init ();
}               
inline int
CBodyText::GetLength (void)
{
  return m_CsRtf.GetLength ();
} 
inline char
CBodyText::GetAt (int idx)
{
  return m_CsRtf.GetAt (idx);
}
inline int
CBodyText::Find (char ch)
{
  return m_CsRtf.Find (ch);
}
inline int
CBodyText::Find (const char *str)
{
  return m_CsRtf.Find (str);
}
// Group Tracking -----------------------------------------------------------
inline  void  
CBodyText::Begin (int cnt)
{
  for (cnt; cnt > 0; cnt--)
  {
    m_CsRtf += '{'; 
    m_nCurlyCount ++;
  }  
}
inline  void  
CBodyText::End (int cnt)
{
  for (cnt; (m_nCurlyCount > 0) && (cnt > 0); cnt--)
  {
    m_CsRtf += '}'; 
    m_nCurlyCount --;
  }  
}
inline  void  
CBodyText::End (void)
{
  End (m_nCurlyCount);
}
// Basic Formatting ---------------------------------------------------------
inline  void  
CBodyText::SetBold (void)
{
  m_CsRtf += "\\b ";
}
inline  void  
CBodyText::SetItalic (void)
{
  m_CsRtf += "\\i ";
}              
inline  void  
CBodyText::SetSuper (void)
{
  m_CsRtf += "\\super ";
}
inline  void  
CBodyText::SetSub (void)
{
  m_CsRtf += "\\sub ";
}              
inline  void  
CBodyText::SetHidden (void)
{
  m_CsRtf += "\\v ";
}              
inline  void  
CBodyText::SetUnderline (enum UnderLineType flag)
{
  switch (flag)
  {             
    case  BT_CONT_UNDER:
    m_CsRtf += "\\ul ";               //continue underline
    break;
    
    case  BT_DOT_UNDER:
    m_CsRtf += "\\uld ";              //dotted underline 
    break;
    
    case  BT_DOUBLE_UNDER:
    m_CsRtf += "\\uldb ";             //double underline
    break;
    
    case   BT_WORD_UNDER:
    m_CsRtf += "\\ulw ";              //word underline
    break;
    
    case  BT_NO_UNDER:
    m_CsRtf += "\\ul0 ";              //turn off all underlines          
    break;
  }             
}
inline  void  
CBodyText::SetCenter (void)
{
  m_CsRtf += "\\qc ";
}
inline  void  
CBodyText::AddTab (int num)
{
  for (num; num > 0; num --)
    m_CsRtf += "\\tab ";
}
inline  void  
CBodyText::NewLine (int num)
{
  for (num; num > 0; num --)
    m_CsRtf += "\\par ";
}
inline  void
CBodyText::MakeUpper (void)
{
  m_CsRtf.MakeUpper ();
}
// PRIVATE ------------------------------------------------------------------
inline  void  
CBodyText::Init (void)
{
  m_CsRtf.Empty ();
  m_nCurlyCount = 0;
}                
inline  void  
CBodyText::SetCurlyCount (void)
{
  m_nCurlyCount = GetCurlyCount (&m_CsRtf);
}

#endif


