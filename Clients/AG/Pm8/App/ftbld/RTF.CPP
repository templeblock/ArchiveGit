#include  "stdafx.h"

#include  "rtf.h"

// Construction -------------------------------------------------------------
CBodyText::CBodyText ()
{
  Init ();
}
CBodyText::CBodyText (char *str)
{
  m_CsRtf = str;
  SetCurlyCount ();
}
CBodyText::CBodyText (CString *str)
{
  m_CsRtf = *str;
  SetCurlyCount ();
}
// Operators ----------------------------------------------------------------
int           
CBodyText::GetCurlyCount (CString *cs)
{
  int 	c = 0;
  int 	l = cs->GetLength (); 
	BOOL	SawSlash = FALSE;				// "\{" is not an RTF block marker
                
  for (int i = 0; i < l; i ++)
  {
    if (((const char *)*cs) [i] == '{')
		{
      if (SawSlash == FALSE)
      	c ++;
		}
    else  
    if (((const char *)*cs) [i] == '}')
		{
			if (SawSlash == FALSE)
      	c --;
		}

		if (((const char *)*cs) [i] == '\\')
			SawSlash = TRUE;
		else
			SawSlash = FALSE;
  }
  
  return c;
}
int           
CBodyText::GetCurlyCount (const char *cp)
{
  int 	c = 0;
	BOOL	SawSlash = FALSE;
                
  for (int i = 0; cp [i] != 0; i ++)
  {
    if (cp [i] == '{')
		{
			if (SawSlash == FALSE)
      	c ++;
		}
    else  
    if (cp [i] == '}')
		{
			if (SawSlash == FALSE)
      	c --;
		}
    
    if (cp [i] == '\\')
			SawSlash = TRUE;
		else
			SawSlash = FALSE;
  }
  
  return c;
}
//---------------------------------------------------------------------------


