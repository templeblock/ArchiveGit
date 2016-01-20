// (c) Copyright 1991 MICROGRAFX, Inc., All Rights Reserved.
// This material is confidential and a trade secret.
// Permission to use this work for any purpose must be obtained
// in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081

#include "pp.h"
#include "id.h"
#include "text.h"
#include "textime.h"


extern HINSTANCE hInstAstral;
extern TEXTIME TextIME;
LOGFONT logFont;

typedef enum
{
	TEXT_ALIGN_LEFT = 0,
    TEXT_ALIGN_RIGHT,
    TEXT_ALIGN_CENTER
} TEXTALIGN;

typedef struct _textbits
   	{
   	LPFRAME lpTextBits;
   	RECT rBits;
   	} TEXTBITS, FAR *LPTEXTBITS;

class CTextInfo : public CPtrList
   	{
   	public:
      	CTextInfo();
     	~CTextInfo();
      	void EmptyIt();
      	int GetTotalSize();
      	CString *GetTotalChars();
      	void Import(CString Str);
   		void ProcessMBChar( WORD wChar, BOOL Render );
      	void OnDeleteMBChar();
      	void DeleteMBChar();
      	void DeleteLine();
      	void InsertMBChar( WORD wChar );
      	void InsertLine();
      	void TurnOffText();
      	void TextDisplayLine(HDC hInDC, BOOL fExtentsOnly, BOOL fAllowScaling);
      	void DisplayMBChar(WORD wChar, BOOL fExtentsOnly, POINT & Pos);
      	void HitTest(HDC hInDC, int x, int y);
      	BOOL SetupHDC(HDC hInDC, BOOL fAllowScaling = TRUE);
      	void ReleaseHDC(HDC hInDC);
      	int  GetExtent();
      	int  GetLineWidth(POSITION Pos = NULL);
      	int  GetMaxWidth();
      	int  GetYLoc();
      	int  GetXStartPos(POSITION Pos);
      	void UpdateYLoc();
      	void UpdateXLoc();
      	void MoveCursor(HDC hInDC, int x, int y);
      	void MoveDown();
      	void MoveUp();
      	void MoveRight();
      	void MoveLeft();
      	ABC  GetCharWidth( WORD wChar);
      	CString *GetCurLine(){return(m_pString);}
      	void Initialize();

   	public:
      	FONTID		Font;
      	int		  	yres;
      	RECT		rExtent;
      	POINT		FilePos;
      	POINT       DispPos;
      	POINT       StartPos;
      	POINT		CurPos;
      	COLORINFO	Color;
      	LFIXED		DispRate;
      	BOOL 		bCharsOn;
      	TFORM		tform;
      	HWND        m_hWnd;
      	HFONT       hFont;
      	TEXTALIGN   Justification;
      	BOOL        AntiAlias;
      	RECT        MaxRect;
      	TEXTMETRIC  tm;
      
   	private:
      	POSITION    m_CurLine;
      	int         m_CurMBChar;
      	CString    *m_pString;

      	int	      	oldMode;
      	int         oldAlign;
      	DWORD       oldColor;
      	HFONT       hOldFont;
      	HDC         m_hDC;
      	int         m_DCCount;
      	int         m_MaxWidth;
   	};

typedef struct _texttool
{
	CTextInfo	Info;
	BOOL		fNoAdd;
	HBITMAP 	hbm;
} TEXTTOOL, FAR *LPTEXTTOOL;

LOCAL HFONT TextNewFont(HDC m_hDC, LPFONTID lpFont, LFIXED DispRate, BOOL fAllowScaling, int yres);

class CTextRedrawHook : public CHookHandler
{
public:
    CTextRedrawHook() : CHookHandler() {};
    virtual void ImageHookProc(int x, int y, int dx, FRMTYPEINFO TypeInfo, LPTR lpSrc,
            LPTR lpDst, LPTR lpMsk, LPMASK lpMask) { /*no implementation*/ };
    virtual void DisplayHookProc(HWND hWnd, HDC hDC, LPRECT lpRect); 
protected:
private:
};
CTextRedrawHook TextRedrawHook;


LOCAL void TextGetTForm(HDC hDC, LPTFORM lpTForm, LPFONTID lpFont, 
   LPPOINT lpCurrentPos, BOOL Inverse = FALSE);
LOCAL int TextGetAngle(HDC hDC, LPFONTID lpFont);
LOCAL LPFRAME TextGetBitmapBits(HDC hDC, HBITMAP hBitmap, int iWidth, int iHeight, LPIMAGE lpImage);
LOCAL STATUS_CODE TextCreateObject(LPIMAGE lpImage, LPFRAME lpBits, LPRECT lpRect,
					LPTEXT_PARMS lpParms, LPRECT lpUpdateRect);
LOCAL STATUS_CODE TextFill(LPIMAGE lpImage, LPFRAME lpBits, LPRECT lpRect,
					LPCOLORINFO lpColor, int Opacity, MERGE_MODE MergeMode,
					LPRECT lpUpdateRect);
LOCAL void TextMaskProc(int y, int left, int right, LPTR lpMsk, LPTEXTBITS lpBits  );
LOCAL LPFRAME TextAntiAlias(LPFRAME lpBits, int iRasterWidth,
			int iRasterHeight, int iWidth, int iHeight);
LOCAL BOOL TextCheckRotate(BOOL fCanRotate, LPFONTDESC lpFontDesc);
LOCAL void SetBackSlashBits(LPTR lp, int w, int h, WORD wBytes);
LOCAL void SetSlashBits(LPTR lp, int w, int h, WORD wBytes);
LOCAL void SetBit(LPTR lp, int x);

LPOBJECT FindTextObject(LPIMAGE lpImage, POINT Pt);

#define ANTI_ALIAS_FACTOR 2

static TEXTTOOL		TextTool;

/************************************************************************/
/*                              CTextObject                             */
/************************************************************************/

LPOBJECT    FindTextObject(LPIMAGE lpImage, POINT Pt)
{
   	LPOBJECT    lpObject = NULL;

   	while (lpObject = ImgGetNextObject(lpImage, lpObject, YES, NO))
  	{
      	if (lpObject->lpText && PtInRect(&lpObject->rObject, Pt))
         	break;
   	}
   	return(lpObject);
}

LPOBJECT CurrentTextObject = NULL;

void StartEditing(CTextInfo *pTextInfo, CString lpText)
   {
   pTextInfo->Import(lpText);
   }

/************************************************************************/
/*                              CTextInfo                               */
/************************************************************************/

CTextInfo::CTextInfo()
   {
   Initialize();
   }

CTextInfo::~CTextInfo()
   {
   if (hFont)
      DeleteObject(hFont);
   }

void CTextInfo::Initialize()
   	{
   	m_pString  = new CString;
   	AddHead(m_pString);
   	m_CurLine  = GetHeadPosition();
   	m_CurMBChar  = 0;
   	m_hWnd     = NULL;
   	m_hDC      = NULL;
   	m_DCCount  = 0;
   	hFont      = NULL;
   	bCharsOn   = TRUE;
   	AntiAlias  = TextForum.GetAntiAlias();
   	StartPos.x = DispPos.x = CurPos.x = FilePos.x = 0;
   	StartPos.y = DispPos.y = CurPos.y = FilePos.y = 0;
   	Justification    = TEXT_ALIGN_LEFT;
   	m_MaxWidth = 0;
	MaxRect.left  = MaxRect.top    = INT_MAX;
   	MaxRect.right = MaxRect.bottom = 0;
   	}

void CTextInfo::EmptyIt()
   {
   POSITION Pos = GetHeadPosition();

   for (; Pos;)
      delete (CString *)GetNext(Pos);
   RemoveAll();
   Initialize();
   }

int CTextInfo::GetTotalSize()
   {
   int      size = 0;
   POSITION Pos = GetHeadPosition();

   for (; Pos;)
      size += ((CString *)GetNext(Pos))->GetLength();
   return(size);
   }

void CTextInfo::Import(CString Src)
   {
	WORD wChar;

    LPTSTR lpsz = Src.GetBuffer( Src.GetLength());
	while( *lpsz )
	{
		if( IsDBCSLeadByte(( BYTE )*lpsz ))
			wChar = *(( LPWORD )lpsz );
		else
			wChar = MAKEWORD( *lpsz, 0 );

		ProcessMBChar( wChar, FALSE );
		lpsz = MyCharNext( lpsz );
	}
	Src.ReleaseBuffer();

   	m_MaxWidth = 0;
}

void CTextInfo::ProcessMBChar( WORD wChar, BOOL Render)
{
   	switch (wChar)
    {
      	case VK_RETURN:
         	if (TextTool.fNoAdd)
	         	MessageBeep(0);
         	else
           	{
            	TurnOffText();
            	InsertLine();
	         	TextDisplayLine(NULL, !Render, YES);
           	}
         	break;

      	case VK_BACK:
         	if (!SetupHDC(NULL, TRUE))
            	break;

         	TurnOffText();
         	DeleteMBChar();
	      	TextDisplayLine(NULL, !Render, YES);
         	ReleaseHDC(NULL);
         	break;

      	default:
         	if (TextTool.fNoAdd)
	         	MessageBeep(0);
         	else
           	{
            	if (!SetupHDC(NULL, TRUE))
               	break;

            	TurnOffText();
            	InsertMBChar( wChar );
	      	   	TextDisplayLine(NULL, !Render, YES);
            	ReleaseHDC(NULL);
           	}
         	break;
   	}
}

void CTextInfo::OnDeleteMBChar()
{
   	int OldPos = m_CurMBChar;
   	if (SetupHDC(NULL, TRUE))
   	{
      	TurnOffText();
      	MoveRight();

      	if (OldPos != m_CurMBChar)
         	DeleteMBChar();

	   	TextDisplayLine(NULL, FALSE, YES);
      	ReleaseHDC(NULL);
   	}
}

void CTextInfo::DeleteMBChar()
{
   	m_MaxWidth = 0;

   	if( !m_CurMBChar )	// if at beginning of line
      	DeleteLine();
   	else
  	{
		TRY
		{
			ASSERT( m_CurMBChar > 0 );

      		int nLenBefore = m_pString->GetLength();
      		LPTSTR lpszString = m_pString->GetBuffer( nLenBefore );

			LPTSTR lpChar = MyCharNextN( lpszString, m_CurMBChar-1 );	// char being deleted
			int nBytesFreed = IsDBCSLeadByte(( BYTE )*lpChar ) ? MB_LEN_MAX : sizeof( BYTE ); 

      		if( m_CurMBChar < MBStrlen( lpszString ))		// if not last character in line
       		{
				// skip past deleted character, then shift remaining 
				// characters in string to the left by nBytesFreed.
				int nIndex = ( lpChar - lpszString ) + nBytesFreed; 
         		while( nIndex < nLenBefore )
           		{
            		lpszString[nIndex-nBytesFreed] = lpszString[nIndex];
					nIndex++;
           		}
       		}
      		lpszString[nLenBefore-nBytesFreed] = _T('\0');
      		m_pString->ReleaseBuffer();
		}
		CATCH( CException, e )
		{
			THROW( e );
		}
		END_CATCH

      	m_CurMBChar--;
		GetMaxWidth();
      	UpdateXLoc();
    }
}

void CTextInfo::DeleteLine()
{
   	if (m_CurLine != GetHeadPosition()) // not first line
   	{
      	POSITION Pos = m_CurLine;
      	CString	*pp	 = m_pString;

      	GetPrev(Pos);
      	CString *pPrev = (CString *)GetAt(Pos);
      	Pos = m_CurLine;
      	MoveLeft();
      	*pPrev += *pp;
      	delete (CString *)GetAt(Pos);
      	RemoveAt(Pos);
      	GetMaxWidth();
   	}
}

void CTextInfo::InsertMBChar( WORD wChar )
{
	LPTSTR lpsz, lpszStart;
	int nLenBefore, nLenAfter, nBytesMoved;

	TRY
	{
		nLenBefore = m_pString->GetLength();
		if( IsDBCSLeadByte( LOBYTE( wChar ))) 
		{
			if(( nLenBefore + 3 ) >= INT_MAX )
				return;

			*m_pString += _T("  ");
		}
		else
		{
			if(( nLenBefore + 2 ) >= INT_MAX )
				return;

			*m_pString += _T(' ');
		}
		nLenAfter = m_pString->GetLength();
   		lpsz = lpszStart = m_pString->GetBuffer( nLenAfter );


		// is character is to be inserted in middle of line?
		int nChars = MBStrlen( lpsz ) - ( nLenAfter - nLenBefore );
		if( m_CurMBChar < nChars )	
		{
			// How many bytes to shift right in the buffer?
			lpsz = MyCharNextN( lpszStart, m_CurMBChar );	
			nBytesMoved = nLenBefore - ( lpsz - lpszStart );

			// move characters following insert position up in the buffer
			lpsz = lpszStart;
			while( nBytesMoved-- )
      			lpsz[ --nLenAfter ] = lpsz[ --nLenBefore ];
		}

		// copy new character to m_CurMBChar position
		lpsz = MyCharNextN( lpszStart, m_CurMBChar++ );
		if( IsDBCSLeadByte( LOBYTE( wChar ))) 
		{
			*(( LPBYTE )lpsz )        = LOBYTE( wChar );
			*(( LPBYTE )( lpsz + 1 )) = HIBYTE( wChar );
		}
		else
			*(( LPBYTE )lpsz ) = LOBYTE( wChar );

   		m_pString->ReleaseBuffer();
	}
	CATCH( CException, e )
	{
		THROW( e );
	}
	END_CATCH

   	m_MaxWidth = 0;
   	GetMaxWidth();
	UpdateXLoc();
}

void CTextInfo::InsertLine()
{
   	if( !SetupHDC( NULL ))
    	return;

   	if( m_hWnd )
    	HideCaret( m_hWnd );

	CString  *pNew = new CString;
   	InsertAfter( m_CurLine, pNew );

	LPTSTR lpsz = m_pString->GetBuffer( m_pString->GetLength());

   	if( m_CurMBChar < MBStrlen( lpsz ))
   	{
    	*pNew = MyCharNextN( lpsz, m_CurMBChar );
		*MyCharNextN( lpsz, m_CurMBChar ) = _T('\0');
   	}
	m_pString->ReleaseBuffer();
   	m_CurMBChar  = 0;
   	m_MaxWidth = 0;
   	GetMaxWidth();

   	MoveDown();
   	if( m_hWnd )
		TextDisplayCaret( m_hWnd, m_hDC );

   	ReleaseHDC( NULL );
}

CString *CTextInfo::GetTotalChars()
{
   	POSITION Pos = GetHeadPosition();
   	CString  *pStr = new CString;

   	for (; Pos;)
   	{
      	if (Pos != GetHeadPosition())
         	*pStr += VK_RETURN;
      	*pStr += *(CString *)GetNext(Pos);
   	}
   	return(pStr);
}

void CTextInfo::TurnOffText()
{
   	RECT rInvalid;

   	if (m_hWnd && (GetFocus() == m_hWnd))
		HideCaret(m_hWnd);

	if (bCharsOn && m_hWnd)
	{
      	RECT  r = rExtent;
      	TextGetTForm(m_hDC, &tform, &Font, &DispPos);
		TransformRect(&tform, &r, &rInvalid);

#ifdef OPTIMIZE
      	RECT  rOldClip;
	   	HDC   hDC = GetDC(m_hWnd);

		r = GetDisplayPtr(m_hWnd)->DispRect;
		SelectClipRect(hDC, &r, &rOldClip);
      	FillRect(hDC, &rInvalid, (HBRUSH)GetStockObject(WHITE_BRUSH));
		SelectClipRect(hDC, &rOldClip, NULL);
      	ReleaseDC(m_hWnd, hDC);
#else
		SetDisplayHook(m_hWnd, NULL);
		InvalidateRect(m_hWnd, &rInvalid, FALSE);
		AstralUpdateWindow(m_hWnd);
		// install the redraw proc
		SetDisplayHook(m_hWnd, &TextRedrawHook);
#endif
		bCharsOn = FALSE;             // indicate no drawing done
	}
}

void CTextInfo::MoveLeft()
{
   	if (m_CurMBChar)
      	m_CurMBChar--;
   	else 
	if (m_CurLine != GetHeadPosition())
    {
      	GetPrev(m_CurLine);
      	m_pString = (CString *)GetAt(m_CurLine);

		LPTSTR lpsz = m_pString->GetBuffer( m_pString->GetLength());
       	m_CurMBChar = MBStrlen( lpsz );
		m_pString->ReleaseBuffer();

	   	CurPos.y -= (tm.tmHeight + tm.tmExternalLeading);
   	}
   	UpdateXLoc();
}

void CTextInfo::MoveRight()
{
	LPTSTR lpsz = m_pString->GetBuffer( m_pString->GetLength());
	int nChars = MBStrlen( lpsz );
	m_pString->ReleaseBuffer();

   	if( m_CurMBChar < nChars )
      	m_CurMBChar++;
   	else 
	if( m_CurLine != GetTailPosition())
    {
      	GetNext( m_CurLine );
      	m_pString = ( CString * )GetAt( m_CurLine );
      	m_CurMBChar = 0;
	   	CurPos.y += ( tm.tmHeight + tm.tmExternalLeading );
    }
   	UpdateXLoc();
}

void CTextInfo::MoveUp()
{
   	if( m_CurLine != GetHeadPosition())
    {
      	GetPrev( m_CurLine );
      	m_pString = ( CString * )GetAt( m_CurLine );

      	LPTSTR lpsz = m_pString->GetBuffer( m_pString->GetLength());
		int len = MBStrlen( lpsz );
		m_pString->ReleaseBuffer();

      	if( m_CurMBChar > len )
        	m_CurMBChar = len;

      	CurPos.y -= ( tm.tmHeight + tm.tmExternalLeading );
      	UpdateXLoc();
    }
}

void CTextInfo::MoveDown()
{
	int len;
   	if( m_CurLine != GetTailPosition())
   	{
      	GetNext( m_CurLine );
      	m_pString = ( CString * )GetAt( m_CurLine );

      	LPTSTR lpsz = m_pString->GetBuffer( m_pString->GetLength());
		len = MBStrlen( lpsz );
		m_pString->ReleaseBuffer();

      	if( m_CurMBChar > len )
         	m_CurMBChar = len;

      	CurPos.y += ( tm.tmHeight + tm.tmExternalLeading );
      	UpdateXLoc();
   	}
}

void CTextInfo::MoveCursor(HDC hInDC, int x, int y)
{
   	if (!SetupHDC(hInDC))
      	return;

   	if (m_hWnd)
      	HideCaret(m_hWnd);

   	if (x == -1)
      	MoveLeft();
   	if (x == 1)
      	MoveRight();
   	if (y == -1)
      	MoveUp();
   	if (y == 1)
      	MoveDown();

   	if (m_hWnd)
	   	TextDisplayCaret(m_hWnd, m_hDC);

   	ReleaseHDC(hInDC);
}

int  CTextInfo::GetXStartPos(POSITION Pos)
   {
   int   x;
   switch (Justification)
      {
      case TEXT_ALIGN_LEFT:
         x = DispPos.x;
         break;
      case TEXT_ALIGN_CENTER:
         x = DispPos.x + (GetMaxWidth() - GetLineWidth(Pos)) / 2;
         break;
      case TEXT_ALIGN_RIGHT:
         x = DispPos.x + (GetMaxWidth() - GetLineWidth(Pos));
         break;
      }
   return(x);
   }

void CTextInfo::UpdateYLoc()
   {
   CurPos.y  = DispPos.y + GetYLoc();
   }

void CTextInfo::UpdateXLoc()
   {
   CurPos.x = GetXStartPos(m_CurLine) + GetExtent();
   }

int  CTextInfo::GetMaxWidth()
   {
   int      w;
   POSITION Pos;
 
   if (!m_MaxWidth)
      {
      Pos = GetHeadPosition();
      for (;Pos;GetNext(Pos))
         {
         w = GetLineWidth(Pos);
         if (w > m_MaxWidth)
            m_MaxWidth = w;
         }
      }
   return(m_MaxWidth);
   }

int CTextInfo::GetLineWidth(POSITION Pos)
{
	ABC abc;
   	int w = 0;
   	CString *pStr;

   	if( !Pos )
      	pStr = m_pString;
   	else
      	pStr = ( CString * )GetAt( Pos );

	LPTSTR lp = pStr->GetBuffer( pStr->GetLength());
   	while( *lp )
   	{
      	abc = GetCharWidth(( IsDBCSLeadByte(( BYTE )*lp )) ? *(( LPWORD )lp ):*(( LPBYTE )lp ));
      	w += abc.abcA + abc.abcB + abc.abcC;
		lp = MyCharNext( lp );
   	}
	pStr->ReleaseBuffer();
	return( w );
}

int CTextInfo::GetExtent()
{
	int   i;
   	ABC   abc;
   	int   w = 0;

	LPTSTR lpsz = m_pString->GetBuffer( m_pString->GetLength());

   	for( i = 0; i < m_CurMBChar; i++ )
    {
      	abc = GetCharWidth(( IsDBCSLeadByte(( BYTE )*lpsz )) ? *(( LPWORD )lpsz ):*(( LPBYTE )lpsz ));
      	w += abc.abcA + abc.abcB + abc.abcC;
		lpsz = MyCharNext( lpsz );
   	}
	m_pString->ReleaseBuffer();
   	return( w );
}

void CTextInfo::HitTest(HDC hInDC, int x, int y)
{
	int      i;
   	POINT    Loc;
   	ABC      abc;
   	RECT     r = rExtent;
   	POSITION Pos = GetHeadPosition();

   	if( !SetupHDC( hInDC ))
      	return;

   	if( m_hWnd )
      	HideCaret( m_hWnd );

   	TextGetTForm( m_hDC, &tform, &Font, &DispPos, TRUE );
   	Loc.x = x;
   	Loc.y = y;
	Transformer( &tform, &Loc, &x, &y );

   	if (x < r.left)
      	x = r.left;
   	if (x > r.right)
      	x = r.right;
   	if (y < r.top)
      	y = r.top;
   	if (y > r.bottom)
      	y = r.bottom;

   	m_CurLine = GetTailPosition();
   	Loc = DispPos;

   	for( ; Pos; )
    {
      	if( y < Loc.y )
       	{
         	m_CurLine = Pos;
         	break;
       	}
      	GetNext( Pos );
      	if( !Pos )
         	break;
	   	Loc.y += tm.tmHeight + tm.tmExternalLeading;
   	}
   	CurPos.y   = Loc.y;
   	m_pString = ( CString * )GetAt( m_CurLine );

	LPTSTR lpsz = m_pString->GetBuffer( m_pString->GetLength());
   	m_CurMBChar = MBStrlen( lpsz );

   	for( i = 0; i < m_CurMBChar; i++ )
   	{
      	abc = GetCharWidth(( IsDBCSLeadByte(( BYTE )*lpsz )) ? *(( LPWORD )lpsz ):*(( LPBYTE )lpsz ));

      	Loc.x += abc.abcA + abc.abcB + abc.abcC;
      	if( x < Loc.x )
       	{
         	m_CurMBChar = i;
         	break;
       	}
		lpsz = MyCharNext( lpsz );
   	}
	m_pString->ReleaseBuffer();
   	UpdateXLoc();
   	if( m_hWnd )
	   	TextDisplayCaret( m_hWnd, m_hDC );
   	ReleaseHDC( hInDC );
}

BOOL CTextInfo::SetupHDC(HDC hInDC, BOOL fAllowScaling)
   {
   BOOL   ret = FALSE;
   RGBS  rgb;

   if (!m_hDC)
      {
      if (!(m_hDC = hInDC))
	      m_hDC = GetDC(m_hWnd);

      if (!m_hDC)
	      {
	      Message(IDS_EMEMALLOC);
	      return(FALSE);
	      }

      if (!DispRate || m_hWnd)
	      DispRate = (m_hWnd ? GetDisplayPtr(m_hWnd)->DispRate : FUNITY);

      if (!(hFont = TextNewFont(m_hDC, &Font, DispRate, fAllowScaling, yres)))
	      {
	      Message(IDS_EMEMALLOC);
	      if (!hInDC)
		      ReleaseDC(m_hWnd, m_hDC);
         m_hDC = NULL;
	      return(FALSE);
	      }
      else
         {
         // setup all text parameters
         rgb = Color.rgb;
         oldColor = SetTextColor(m_hDC, RGB(rgb.red, rgb.green, rgb.blue));
         oldAlign = SetTextAlign(m_hDC, TA_LEFT|TA_BASELINE|TA_NOUPDATECP);

#ifdef OPTIMIZE
         if (m_hWnd)
            oldMode  = SetBkMode(m_hDC, OPAQUE);
         else
#endif
         oldMode  = SetBkMode(m_hDC, TRANSPARENT);

         hOldFont = (HFONT)SelectObject(m_hDC, hFont);
         GetTextMetrics(m_hDC, &tm);
         }
      }
   m_DCCount++;
	return(TRUE);
   }

void CTextInfo::ReleaseHDC(HDC hInDC)
   {
   ASSERT(m_DCCount);
   m_DCCount--;
   if (!m_DCCount)
      {
      SelectObject(m_hDC, hOldFont);
      if (hFont)
         DeleteObject(hFont);
      hFont = NULL;
      SetTextAlign(m_hDC, oldAlign);
      SetBkMode(m_hDC, oldMode);
      SetTextColor(m_hDC, oldColor);
      if (!hInDC)
	      ReleaseDC(m_hWnd, m_hDC);
      m_hDC = NULL;
      }
   }

void CTextInfo::TextDisplayLine(HDC hInDC, BOOL fExtentsOnly, BOOL fAllowScaling)
{
   	BOOL        bHasFocus = FALSE;
   	CString    *Src;
   	WORD        wChar;
   	POINT       Loc;
   	POSITION    Pos;

   	if (m_hWnd && !fExtentsOnly)
   	{
	   	bHasFocus = GetFocus() == m_hWnd;
	   	if (bHasFocus)
		   	HideCaret(m_hWnd);
   	}

   	if (!SetupHDC(hInDC, fAllowScaling))
      	return;

   	bCharsOn = TRUE;
   	// initialize extent values
	rExtent.left = rExtent.right  = DispPos.x;
	rExtent.top  = rExtent.bottom = DispPos.y;

   	Pos = GetHeadPosition();
	Loc.y = DispPos.y;     // reset to top left
   	Loc.x = GetXStartPos(Pos);

   	// if device can rotate or it's a TrueType font, do rotation
   	TextGetTForm(m_hDC, &tform, &Font, &Loc);

   	for (; Pos; GetNext(Pos))
   	{
      	Src = (CString *)GetAt(Pos);
      	Loc.x  = GetXStartPos(Pos);

		LPTSTR lpsz = Src->GetBuffer( Src->GetLength());

      	while( *lpsz )
      	{
        	if( IsDBCSLeadByte(( BYTE )*lpsz ))
            	wChar = *(( LPWORD )lpsz );
        	else
            	wChar = MAKEWORD( *(( LPBYTE )lpsz ), 0 );

	      	DisplayMBChar( wChar, fExtentsOnly, Loc );
			lpsz = MyCharNext( lpsz );
      	}
      	Src->ReleaseBuffer();
	   	Loc.y += tm.tmHeight + tm.tmExternalLeading;
   	}

   	if (bHasFocus)
	   	TextDisplayCaret(m_hWnd, m_hDC);

   	if (rExtent.left < MaxRect.left)
      	MaxRect.left = rExtent.left;
   	if (rExtent.top < MaxRect.top)
      	MaxRect.top = rExtent.top;
   	if (rExtent.right > MaxRect.right)
      	MaxRect.right = rExtent.right;
   	if (rExtent.bottom > MaxRect.bottom)
      	MaxRect.bottom = rExtent.bottom;

   	ReleaseHDC(hInDC);
}

int CTextInfo::GetYLoc()
   {
   POSITION    Pos;
   int         h = 0;

   Pos = GetHeadPosition();
   for (; Pos != m_CurLine; GetNext(Pos))
	   h += tm.tmHeight + tm.tmExternalLeading;

   return(h);
   }

ABC CTextInfo::GetCharWidth( WORD wChar)
{
 	ABC	abc;
   	int	slop;
   	int	iCharWidth;

	if( !IsDBCSLeadByte( LOBYTE( wChar ))) 
	{
   		if( wChar < tm.tmFirstChar || wChar > tm.tmLastChar )
	  		wChar = tm.tmDefaultChar;
	}

   	// If is 3.1 and is a TrueType character use real extent info
   	if( !GetCharABCWidths( m_hDC, wChar, wChar, &abc ))
   	{                    
		// use bogus 3.0 extent information
	   	::GetCharWidth( m_hDC, wChar, wChar, &iCharWidth );
      	abc.abcB = iCharWidth;

	   	// italic and no overhang usually means Windows has screwed up the 
		// character extents, so add in our own compensation
	   	slop = max( tm.tmHeight / 2, tm.tmOverhang );

	   	abc.abcA  = -slop;
	   	abc.abcC  = -slop;
	   	abc.abcB += slop * 2;
   	}
   	return( abc );
}

void CTextInfo::DisplayMBChar( WORD wChar, BOOL fExtentsOnly, POINT & Pos)
{
   	int	y;
   	int	cx, cy;
   	RECT rect, rOldClip;
   	SIZE size;
   	ABC	abc;
	TCHAR szBuffer[3];

	if( IsDBCSLeadByte( LOBYTE( wChar )))
	{
		szBuffer[0] = LOBYTE( wChar );
		szBuffer[1] = HIBYTE( wChar );
		szBuffer[2] = _T('\0');
	}
	else
	{
		szBuffer[0] = LOBYTE( wChar );
		szBuffer[1] = _T('\0');
	}

   	// Display character
   	if( !fExtentsOnly )
   	{
	   	Transformer( &tform, &Pos, &cx, &cy );
	   	if( m_hWnd )
	   	{
		   	rect = GetDisplayPtr( m_hWnd )->DispRect;
		   	SelectClipRect( m_hDC, &rect, &rOldClip );
		   	TextOut( m_hDC, cx, cy, szBuffer, lstrlen( szBuffer ));
	   		SelectClipRect( m_hDC, &rOldClip, NULL );
	   	}
	   	else
		{
		   	TextOut( m_hDC, cx, cy, szBuffer, lstrlen( szBuffer ));
		}
   	}
   	// Get extents of character
   	GetTextExtentPoint( m_hDC, szBuffer, lstrlen( szBuffer ), &size );

   	abc = GetCharWidth( wChar );

   	// Advance current position by left extent (can be negative)
   	Pos.x += abc.abcA;
   	if( Pos.x < rExtent.left ) 		// check left extent
	   	rExtent.left = Pos.x;

   	// Advance current position by character width
   	Pos.x += abc.abcB;
   	if( Pos.x > rExtent.right )		// check right extent
	   	rExtent.right = Pos.x;

   	// Advance current position by right extent (can be negative)
   	Pos.x += abc.abcC;
   	if( Pos.x > rExtent.right ) 	// check right extent
	   	rExtent.right = Pos.x;

   	// check top and bottom extents
   	y = Pos.y - tm.tmAscent;
   	if( y < rExtent.top )
	   	rExtent.top = y;

   	y = Pos.y + tm.tmDescent;
   	if( size.cy > tm.tmHeight )
	   	y += ( size.cy - tm.tmHeight );

   	if( tm.tmUnderlined )
	   	y += tm.tmDescent;

   	if( y > rExtent.bottom )
	   	rExtent.bottom = y;
}

/************************************************************************/
BOOL TextBegin(HWND hWnd, int x, int y)
/************************************************************************/
{
	LPIMAGE  	lpImage;
	LPOBJECT 	lpObject = CurrentTextObject;

	TextTool.Info.EmptyIt();
	TextTool.Info.m_hWnd = hWnd;

	// create global DDE data block for IME, if used
	_fmemset( &logFont, 0, sizeof( LOGFONT ));
	SetupIME( hWnd, &logFont );		

	if (!TextTool.Info.SetupHDC(NULL, TRUE))
   		return(FALSE);

	y += TextTool.Info.tm.tmAscent;
	TextTool.Info.ReleaseHDC(NULL);

	lpImage = GetImagePtr(hWnd);
	TextTool.Info.yres = FrameResolution(ImgGetBaseEditFrame(lpImage));
	TextTool.Info.DispRate = 0;
	copy((LPTR)TextForum.GetCurrentFont(), (LPTR)&TextTool.Info.Font, sizeof(FONTID));
	TextTool.Info.Justification = (TEXTALIGN)TextForum.GetJustification();

	if (lpObject)
   	{
   		// convert to this res
   		int res = FrameResolution(ImgGetBaseEditFrame(lpImage));

   		TextTool.Info.FilePos.x = lpObject->lpText->DestPos.x;
   		TextTool.Info.FilePos.y = lpObject->lpText->DestPos.y;
   		ResConvertUL(lpObject->lpText->iBaseRes, res, &TextTool.Info.FilePos.x, &TextTool.Info.FilePos.y);
   		TextTool.Info.FilePos.x += lpObject->rObject.left;
   		TextTool.Info.FilePos.y += lpObject->rObject.top;
   
   		TextTool.Info.StartPos = TextTool.Info.FilePos;
   		TextTool.Info.DispPos  = TextTool.Info.FilePos;
   		File2Display(hWnd,(LPINT)&TextTool.Info.DispPos.x, (LPINT)&TextTool.Info.DispPos.y);
   		TextTool.Info.CurPos = TextTool.Info.DispPos;
		InvalidateRect(hWnd, &lpObject->rObject, FALSE);
		AstralUpdateWindow(hWnd);
   		lpObject->fDeleted = TRUE;
   		TextTool.Info.Import(lpObject->lpText->lpText);
   		TextTool.Info.Color = lpObject->lpText->Color;
   	}
	else
   	{
   		FRMTYPEINFO TypeInfo;
   		ImgGetTypeInfo(lpImage, &TypeInfo);

   		TextTool.Info.FilePos.x = x; TextTool.Info.FilePos.y = y;
   		TextTool.Info.DispPos = TextTool.Info.CurPos = TextTool.Info.FilePos;
   		Display2File(hWnd,(LPINT)&TextTool.Info.FilePos.x, (LPINT)&TextTool.Info.FilePos.y);
   		TextTool.Info.StartPos = TextTool.Info.FilePos;
   		GetActiveColorFromTypeInfo(TypeInfo, &TextTool.Info.Color);
   		if (TypeInfo.DataType == FDT_PALETTECOLOR && TypeInfo.ColorMap)
      		TextTool.Info.Color.rgb = TypeInfo.ColorMap->RGBData[TextTool.Info.Color.index];
   	}

	SetDisplayHook(hWnd, &TextRedrawHook);
	TextTool.Info.TextDisplayLine(NULL, NO, YES);
	return(TRUE);
}

/************************************************************************/
void TextEnd(HWND hWnd, BOOL fCancel)
/************************************************************************/
{
	int nBytes, res;
	LPIMAGE     lpImage;
	BOOL        NotEmpty;

	TerminateIME();

	TextTool.fNoAdd = NO;
	Tool.bActive = NO;
	lpImage = GetImagePtr(hWnd);
	TextTool.Info.TurnOffText();

	TextDisplayCaret(NULL, NULL);
	SetDisplayHook(hWnd, NULL);

#ifdef OPTIMIZE
   	RECT  rInvalid;
   	RECT  r = TextTool.Info.MaxRect;
	TransformRect(&TextTool.Info.tform, &r, &rInvalid);
	InvalidateRect(hWnd, &rInvalid, FALSE);
	AstralUpdateWindow(hWnd);
#endif

	NotEmpty = (TextTool.Info.GetHeadPosition() != TextTool.Info.GetTailPosition())
           	|| !TextTool.Info.GetCurLine()->IsEmpty();

	if (!fCancel && NotEmpty)
	{
		TEXT_PARMS parms;
	   	CString   *pStr = TextTool.Info.GetTotalChars();
	
		nBytes = pStr->GetLength();
		parms.lpTextBuf = (LPTSTR)Alloc(nBytes+1);
		if (parms.lpTextBuf)
		{
    	  	LPTSTR lpstr = pStr->GetBuffer(pStr->GetLength());
			copy((unsigned char *)lpstr, (LPTR)parms.lpTextBuf, nBytes);
      		pStr->ReleaseBuffer();
	      	delete pStr;

			*(parms.lpTextBuf+nBytes) = _T('\0');
			lstrcpy(parms.szFaceName, TextTool.Info.Font.FontDesc.fdFaceName);
			parms.Size           = TextTool.Info.Font.Size;
			parms.Italic         = TextTool.Info.Font.Italic;
			parms.Underline      = TextTool.Info.Font.Underline;
			parms.Strikeout      = TextTool.Info.Font.Strikeout;
			parms.Weight         = TextTool.Info.Font.Weight;
			parms.Angle          = TextTool.Info.Font.Angle;
			parms.Justification  = TextTool.Info.Justification;
			parms.AntiAlias      = TextTool.Info.AntiAlias;
    		parms.Color          = TextTool.Info.Color;
			res = FrameResolution(ImgGetBaseEditFrame(lpImage));
			parms.iBaseRes 		 = res;
			parms.StartPos       = TextTool.Info.StartPos;
			parms.DestPos        = TextTool.Info.FilePos;
			parms.Opacity        = TextForum.GetOpacity();
			parms.MergeMode      = TextForum.GetMergeMode();
      		ITEMID idCommand = CurrentTextObject? IDS_CMD_EDITTEXT : IDS_CMD_TEXT;
			ProcessCommand(lpImage->lpCmdList, idCommand, &parms);
		}
		else
			Message(IDS_EMEMALLOC);
	}
	TextTool.Info.EmptyIt();

	if (CurrentTextObject)
   	{
	   	CurrentTextObject->fDeleted = FALSE;
   		if (NotEmpty)
      	{
	   		InvalidateRect(hWnd, &CurrentTextObject->rObject, FALSE);
		   	AstralUpdateWindow(hWnd);
      	}
   		else
      	{
			TINTFILL_PARMS parms;

      		if (ImgEditInit(lpImage, ET_OBJECT, UT_DELETEOBJECTS, ImgGetBase(lpImage)))
       		{
         		CurrentTextObject->fDeleted = TRUE;
         		ImgEditedObject(lpImage, ImgGetBase(lpImage), IDS_UNDODELETE, NULL);
       		}
      		PostCommand(lpImage->lpCmdList, IDS_CMD_CUT, &parms);
      	}
	   	CurrentTextObject = NULL;
   	}
}

void OnDelMBChar()
{
TextTool.Info.OnDeleteMBChar();
}

/************************************************************************/
void TextKeystroke(HWND hWnd, WORD nVKey)
/************************************************************************/
{
TextTool.Info.ProcessMBChar(nVKey, TRUE);
}

void TextMove(HWND hWnd, int x, int y)
/************************************************************************/
{
if (TextTool.Info.SetupHDC(NULL, TRUE))
   {
   TextTool.Info.TurnOffText();

   y += TextTool.Info.tm.tmAscent;
   TextTool.Info.FilePos.x = TextTool.Info.DispPos.x = x;
   TextTool.Info.FilePos.y = TextTool.Info.DispPos.y = y;
   Display2File(hWnd,(LPINT)&TextTool.Info.FilePos.x, 
                     (LPINT)&TextTool.Info.FilePos.y);
   TextTool.Info.UpdateXLoc();
   TextTool.Info.UpdateYLoc();
   TextTool.Info.TextDisplayLine(NULL, NO, YES);
   TextTool.Info.ReleaseHDC(NULL);
   }
}
/************************************************************************/
void TextMoveCursor(HWND hWnd, int x, int y)
/************************************************************************/
{
TextTool.Info.HitTest(NULL, x, y);
}

/************************************************************************/
void TextRMove(HWND hWnd, int dx, int dy)
/************************************************************************/
{
int	x, y;
LPIMAGE lpImage;

lpImage = GetImagePtr(hWnd);
x = bound(TextTool.Info.FilePos.x + dx, 0, lpImage->npix-1);
y = bound(TextTool.Info.FilePos.y + dy, 0, lpImage->nlin-1);
if (x == TextTool.Info.FilePos.x && y == TextTool.Info.FilePos.y)
	MessageBeep(0);
else
	TextTool.Info.MoveCursor(NULL, dx, dy);
}

/************************************************************************/
void TextFontChanged(HWND hWnd)
/************************************************************************/
{
	RECT  r;
	TransformRect(&TextTool.Info.tform, &TextTool.Info.MaxRect, &r);

	copy((LPTR)TextForum.GetCurrentFont(), (LPTR)&TextTool.Info.Font, sizeof(FONTID));
	TextTool.Info.Justification = (TEXTALIGN)TextForum.GetJustification();
	TextTool.Info.AntiAlias     = TextForum.GetAntiAlias();

	if (!TextTool.Info.SetupHDC(NULL, TRUE))
	   	return;

	TextTool.Info.TurnOffText();
	TextTool.Info.UpdateXLoc();
	TextTool.Info.CurPos.y  = TextTool.Info.DispPos.y + TextTool.Info.GetYLoc();
	InvalidateRect(hWnd, &r, FALSE);
	AstralUpdateWindow(hWnd);
	TextTool.Info.TextDisplayLine(NULL, NO, YES);
	TextTool.Info.ReleaseHDC(NULL);
}

/************************************************************************/
void TextDisplayCaret(HWND hWnd, HDC hInDC)
/************************************************************************/
{
	LPIMAGE lpImage;
	LPDISPLAY lpDisplay;
	HDC	hDC;
	TEXTMETRIC tm;
	HFONT	hFont, hOldFont;
	int	cx, cy, iAngle;
	COLORINFO NewTextColor;
	LPTR	lpXorBits;
	WORD    wBytes;
	int	w, h;
	BOOL	fSlash, fBackSlash;
	CTextInfo  *lpInfo = &TextTool.Info;
	FRMTYPEINFO TypeInfo;

	DestroyCaret();

	if (TextTool.hbm)
	{
		DeleteObject(TextTool.hbm);
		TextTool.hbm = NULL;
	}
	if (!hWnd)
		return;

	if (GetFocus() != hWnd)
		return;

	if (!(hDC = hInDC))
		hDC = GetDC(hWnd);

	if (!hDC)
		return;

	lpImage = GetImagePtr(hWnd);
	lpDisplay = GetDisplayPtr(hWnd);
	hFont = TextNewFont(hDC, &lpInfo->Font, lpDisplay->DispRate, YES,lpInfo->yres);
	if (hFont)
		hOldFont = (HFONT)SelectObject(hDC, hFont);

	GetTextMetrics(hDC, &tm);

	TextGetTForm(hDC, &lpInfo->tform, &lpInfo->Font, &lpInfo->DispPos);
	Transformer(&lpInfo->tform, &lpInfo->CurPos, &cx, &cy);
	Display2File(hWnd,&cx, &cy);
	TextTool.fNoAdd = NO;
	if (cx < 0)
	{
		TextTool.fNoAdd = YES;
		cx = 0;
	}
	else if (cx >= lpImage->npix)
	{
		TextTool.fNoAdd = YES;
		cx = lpImage->npix-1;
	}
	if (cy < 0)
	{
		TextTool.fNoAdd = YES;
		cy = 0;
	}
	else if (cy >= lpImage->nlin)
	{
		TextTool.fNoAdd = YES;
		cy = lpImage->nlin-1;
	}
	File2Display(hWnd, &cx, &cy);

	iAngle = TextGetAngle(hDC, &lpInfo->Font);
	while ( iAngle < 0 ) 
		iAngle += 360;
	while ( iAngle > 360 ) 
		iAngle -= 360;

	fSlash = fBackSlash = NO;
	if (iAngle > 337 || iAngle <= 22)
	{ // Vertical
		w = 2;
		h = tm.tmHeight;
		cy = cy-tm.tmAscent+1;
	}
	else 
	if (iAngle > 22 && iAngle <= 67)
	{ // slash
		w = h = (int)sqrt(((long)tm.tmHeight*(long)tm.tmHeight)/2L);
		cy = cy-h+1;
		fSlash = YES;
	}
	else 
	if (iAngle > 67 && iAngle <= 112)
	{ // Horizontal
		w = tm.tmHeight;
		h = 2;
		cx = cx-tm.tmDescent+1;
	}
	else 
	if (iAngle > 112 && iAngle <= 157)
	{ // backslash
		w = h = (int)sqrt(((long)tm.tmHeight*(long)tm.tmHeight)/2L);
		cx = cx-tm.tmDescent+1;
		fBackSlash = YES;
	}
	else 
	if (iAngle > 157 && iAngle <= 202)
	{ // Vertical flipped
		w = 2;
		h = tm.tmHeight;
		cx = cx - 1;
		cy = cy-tm.tmDescent+1;
	}
	else 
	if (iAngle > 202 && iAngle <= 247)
	{ // slash
		w = h = (int)sqrt(((long)tm.tmHeight*(long)tm.tmHeight)/2L);
		cx = cx-tm.tmAscent+1;
		cy = cy - 1;
		fSlash = YES;
	}
	else 
	if (iAngle > 247 && iAngle <= 292)
	{ // Horizontal flipped
		w = tm.tmHeight;
		h = 2;
		cx = cx-tm.tmAscent+1;
		cy = cy - 1;
	}
	else 
	// if (iAngle > 292 && iAngle <= 337)
	{ // backslash
		w = h = (int)sqrt(((long)tm.tmHeight*(long)tm.tmHeight)/2L);
		cx = cx-tm.tmAscent+1;
		cy = cy-h+1;
		fBackSlash = YES;
	}
	wBytes = (w + 7) / 8;
	if (wBytes & 1)
		++wBytes;
	lpXorBits = Alloc((long)wBytes * h);
	if (lpXorBits)
	{
		if (fSlash)
			SetSlashBits(lpXorBits, w, h, wBytes);
		else 
		if (fBackSlash)
			SetBackSlashBits(lpXorBits, w, h, wBytes);
		else
			set(lpXorBits, wBytes*h, 255);
		TextTool.hbm = CreateBitmap(w, h, 1, 1, lpXorBits);
	   	FreeUp(lpXorBits);
	}

	CreateCaret(hWnd, TextTool.hbm, w, h);
	SetCaretPos(cx, cy);

	ImeMoveConvertWin( &TextIME, cx, cy );

	ShowCaret(hWnd);

	if (hFont)
	{
		SelectObject(hDC, hOldFont);
		DeleteObject(hFont);
	}

	if (!hInDC)
		ReleaseDC(hWnd, hDC);

	ImgGetTypeInfo(lpImage, &TypeInfo);
	GetActiveColorFromTypeInfo(TypeInfo, &NewTextColor);
	if (TypeInfo.DataType == FDT_PALETTECOLOR && TypeInfo.ColorMap)
	   	NewTextColor.rgb = TypeInfo.ColorMap->RGBData[NewTextColor.index];

	if (!ColorsEqual(&NewTextColor, &lpInfo->Color))
	{
		lpInfo->Color = NewTextColor;
		TextFontChanged(hWnd);
	}
}

/************************************************************************/
BOOL TextCanRotate(HDC hDCIn, LPFONTDESC lpFontDesc)
/************************************************************************/
{
	HDC	hDC;
	int	caps;
	BOOL    fCanRotate;

	if (!(hDC = hDCIn))
		hDC = GetDC(NULL);

	if (!hDC)
		return(FALSE);

	caps = GetDeviceCaps(hDC, TEXTCAPS);

	if (!hDCIn)
		ReleaseDC(NULL, hDC);

	fCanRotate = (caps & TC_CR_ANY) ? TRUE : FALSE;
	return(TextCheckRotate(fCanRotate, lpFontDesc));
}

/************************************************************************/
LOCAL BOOL TextCheckRotate(BOOL fCanRotate, LPFONTDESC lpFontDesc)
/************************************************************************/
{
return(fCanRotate || (lpFontDesc->FontType & TRUETYPE_FONTTYPE));
}

/************************************************************************/
BOOL TextEdit(LPIMAGE lpImage, LPTEXT_PARMS lpParms)
/************************************************************************/
{
int      res;
POINT    Pt;
LPOBJECT lpObject;

res = FrameResolution(ImgGetBaseEditFrame(lpImage));
Pt = lpParms->StartPos;
ResConvertUL(lpParms->iBaseRes, res, &Pt.x, &Pt.y);
lpObject = FindTextObject(lpImage, Pt);
if (!lpObject)
   lpObject = CurrentTextObject;
if (!lpObject)  // could not locate an object
   return(FALSE);

if (lpParms->StartPos.x != lpParms->DestPos.x || lpParms->StartPos.y != lpParms->DestPos.y)
   {
   Pt.x = lpParms->DestPos.x - lpParms->StartPos.x;
   Pt.y = lpParms->DestPos.y - lpParms->StartPos.y;
   ResConvertUL(lpParms->iBaseRes, res, &Pt.x, &Pt.y);
   OffsetRect(&lpObject->rObject, Pt.x, Pt.y);
   }
return(TextApply(lpImage, lpParms));
}

/************************************************************************/
BOOL TextApply(LPIMAGE lpImage, LPTEXT_PARMS lpParms)
/************************************************************************/
{
	HDC	      	hTextDC;
	int	      	iWidth, iHeight, iRasterWidth, iRasterHeight;
	HBITMAP     hOldBitmap, hBitmap;
	RECT        rBitmap, rApply, rRaster, rUpdate;
	LPFRAME	   	lpBits;
	int      	ScaleFactor, res;
	CTextInfo   Info;
	int         nPasses;
	int         AntiAlias;
	FRMTYPEINFO TypeInfo;
	LPOBJECT    lpObject;
	POINT       Pt;
	int         x,y;


	if (!lpImage)
	   	return(FALSE);

	// get image's resolution
	res = FrameResolution(ImgGetBaseEditFrame(lpImage));
	Pt = lpParms->DestPos;
	ResConvertUL(lpParms->iBaseRes, res, &Pt.x, &Pt.y);

	lpObject = FindTextObject(lpImage, Pt);
	if (lpObject)
   	{
   		CurrentTextObject = lpObject;
   		x = lpObject->lpText->DestPos.x;
   		y = lpObject->lpText->DestPos.y;
   		ResConvertUL(lpObject->lpText->iBaseRes, res, &x, &y);
   		x += lpObject->rObject.left;
   		y += lpObject->rObject.top;
   	}
	else
   	{
   		x = Pt.x;
   		y = Pt.y;
   	}

	Info.DispPos.x = Info.FilePos.x = x;
	Info.DispPos.y = Info.FilePos.y = y;
	nPasses = 2; // get bits and fill or create objects

	ImgGetTypeInfo(lpImage, &TypeInfo);
	AntiAlias = lpParms->AntiAlias;
	if (TypeInfo.DataType == FDT_LINEART || TypeInfo.DataType == FDT_PALETTECOLOR)
		AntiAlias = 0;

	Info.Justification = (TEXTALIGN)lpParms->Justification;
	if (AntiAlias) // do anti-aliasing
		++nPasses;

	ProgressBegin(nPasses, lpParms->Common.idDirty-IDS_UNDOFIRST+IDS_PROGFIRST);

	// create memory DC
	hTextDC = CreateCompatibleDC(NULL);
	if (!hTextDC)
	{
		Message(IDS_EMEMALLOC);
		ProgressEnd();
		return(FALSE);
	}

	TEXTMETRIC  tm;
	GetTextMetrics(hTextDC, &tm);

	if (TextForum.GetFontDesc(lpParms->szFaceName, &Info.Font.FontDesc) < 0)
	{
		Message(IDS_FONTNOTFOUND, (LPTSTR)lpParms->szFaceName);
	   	DeleteDC(hTextDC);
		ProgressEnd();
		return(FALSE);
	}
	Info.Font.Size = lpParms->Size;
	Info.Font.Italic = lpParms->Italic;
	Info.Font.Underline = lpParms->Underline;
	Info.Font.Strikeout = lpParms->Strikeout;
	Info.Font.Weight = lpParms->Weight;
	Info.Font.Angle = lpParms->Angle;
	Info.yres = res;

	Info.Import(lpParms->lpTextBuf);

	if (TypeInfo.DataType == FDT_PALETTECOLOR)
		lpParms->Color.index = FrameGetNearestIndex(TypeInfo.ColorMap, &lpParms->Color.rgb);
	Info.Color.gray = 255;
	SetColorInfo(&Info.Color, &Info.Color, CS_GRAY);
	ScaleFactor = 1;
	Info.DispRate = FGET(ScaleFactor, 1);

	// Draw text at final size
	Info.TextDisplayLine(hTextDC, YES, YES);

	// Transform (rotate) extent rectangle
	TransformRect(&Info.tform, &Info.rExtent, &rApply);

	// inflate rectangle for anti-aliasing
	if (AntiAlias)
		InflateRect(&rApply, 2*ScaleFactor, 2*ScaleFactor);

	// get width of destination bitmap
	iWidth = RectWidth(&rApply);
	iHeight = RectHeight(&rApply);

	// make sure final bitmap has some size
	if (iWidth <= 4 || iHeight <= 4)
	{
	   	DeleteDC(hTextDC);
		ProgressEnd();
		return(FALSE);
	}

	// Setup anti-alias scale factor if necessary and get extent info
	if (AntiAlias == 1) // scaling up, SmartSize Down
	{
		ScaleFactor = ANTI_ALIAS_FACTOR;
		Info.DispRate = FGET(ScaleFactor, 1);
		Info.TextDisplayLine(hTextDC, YES, NO);
	}

	// Transform (rotate) extent rectangle
	TransformRect(&Info.tform, &Info.rExtent, &rRaster);

	// inflate rectangle for anti-aliasing
	if (AntiAlias)
		InflateRect(&rRaster, 2*ScaleFactor, 2*ScaleFactor);

	// get width of destination rasterization bitmap
	iRasterWidth = RectWidth(&rRaster);
	iRasterHeight = RectHeight(&rRaster);

	if ( !(hBitmap = CreateBitmap(iRasterWidth, iRasterHeight, 1, 1, NULL)) )
	{
	   	DeleteDC(hTextDC);
		ProgressEnd();
		return(FALSE);
	}
	hOldBitmap = (HBITMAP)SelectObject(hTextDC, hBitmap);

	// Clear out the background
	rBitmap.top = rBitmap.left = 0;
	rBitmap.right = iRasterWidth;
	rBitmap.bottom = iRasterHeight;
	FillRect(hTextDC, &rBitmap, (HBRUSH)GetStockObject(BLACK_BRUSH) );

	// Draw the text
	Info.DispPos.x = Info.DispPos.x - rRaster.left;
	Info.DispPos.y = Info.DispPos.y - rRaster.top;
	Info.TextDisplayLine(hTextDC, NO, AntiAlias == 1 ? NO : YES);

	SelectObject(hTextDC, hOldBitmap);

	// Get bits from the bitmap
	lpBits = TextGetBitmapBits(hTextDC, hBitmap, iRasterWidth, iRasterHeight,lpImage);

	// Free everything and clean up
	DeleteObject(hBitmap);
	DeleteDC(hTextDC);
	if (!lpBits)
	{
		ProgressEnd();
		return(FALSE);
	}

	// Anti-Alias text if requested (i.e. - get rid of jaggies)
	// Mode 1: scale up and SmartSize down
	// Mode 2: smooth at final size
	if (AntiAlias)
	{
		if (!(lpBits = TextAntiAlias(lpBits, iRasterWidth, iRasterHeight,
					iWidth, iHeight)))
		{
			FrameClose(lpBits);
			ProgressEnd();
			return(FALSE);
		}
	}

	if (Control.MultipleObjects)
		lpParms->Common.StatusCode = TextCreateObject(lpImage, lpBits, &rApply,lpParms,&rUpdate);
	else
		lpParms->Common.StatusCode = TextFill(lpImage, lpBits, &rApply,
				  	&lpParms->Color, lpParms->Opacity, lpParms->MergeMode,
				  	&rUpdate);
	if (!AstralIsRectEmpty(&rUpdate))
		{
		lpParms->Common.UpdateType = UT_AREA;
		lpParms->Common.rUpdateArea = rUpdate;
		}
	ProgressEnd();
	return( lpParms->Common.StatusCode == SC_SUCCESS );
}

/************************************************************************/
LOCAL STATUS_CODE TextFill(LPIMAGE lpImage, LPFRAME lpBits, LPRECT lpRect,
					LPCOLORINFO lpColor, int Opacity, MERGE_MODE MergeMode,
					LPRECT lpUpdateRect)
/************************************************************************/
{
ENGINE Engine;
TEXTBITS bits;
STATUS_CODE StatusCode;

// Apply text to hi-res image objects
bits.lpTextBits = lpBits;
bits.rBits = *lpRect;
Engine.lpParam = &bits;
SetEngineColor(&Engine, lpColor, Opacity, MergeMode);
Engine.lpEditRect = lpRect;
Engine.lpMaskProc = (LPMASKPROC)TextMaskProc;
StatusCode = LineEngineSelObj(lpImage, &Engine, IDS_UNDOTEXT);
if (!AstralIsRectEmpty(&Engine.rUpdate))
	*lpUpdateRect = Engine.rUpdate;
FrameClose(lpBits);
return(StatusCode);
}

/************************************************************************/
LOCAL STATUS_CODE TextCreateObject(LPIMAGE lpImage, LPFRAME lpBits, LPRECT lpRect,
					LPTEXT_PARMS lpParms, LPRECT lpUpdateRect)
/************************************************************************/
{
LPALPHA lpAlpha;
LPTR lpData, lpBitsData, lpAlphaData;
LPFRAME lpAlphaFrame, lpDataFrame;
LPOBJECT lpTextObject;
int dx, dy, y,res;
RECT rAlpha, rObjects;
FRMTYPEINFO TypeInfo;
FRMDATATYPE MaskType = FDT_GRAYSCALE;
POINT       Pt;

ImgGetTypeInfo(lpImage, &TypeInfo);
if (TypeInfo.DataType == FDT_LINEART && Mask.OneBitMask)
   MaskType = FDT_LINEART;

ProgressBegin(2, 0);

// Create a mask for the text bits
lpAlpha = MaskCreate(lpBits, 0, 0, NO, Control.NoUndo, MaskType);
if (!lpAlpha)
	{
	FrameClose(lpBits);
	ProgressEnd();
	return(SC_MEMERROR);
	}

dx = FrameXSize(lpBits);
dy = FrameYSize(lpBits);
SetRect(&rAlpha, 0, 0, dx-1, dy-1);

// create an alpha frame the size of the minimum bounding rectangle
lpAlphaFrame = FrameOpen(FrameType(lpBits),
						RectWidth(&rAlpha),
						RectHeight(&rAlpha),
						FrameResolution(lpBits));
if (!lpAlphaFrame)
	{
	MaskClose(lpAlpha);
	ProgressEnd();
	return(SC_MEMERROR);
	}

// copy text bits into alpha frame
dx = FrameXSize(lpAlphaFrame);
dy = FrameYSize(lpAlphaFrame);

ProgressBegin(1, 0);
for (y = 0; y < dy; ++y)
	{
	AstralClockCursor(y, dy, NO);
	lpBitsData = FramePointer(lpBits, rAlpha.left, y+rAlpha.top, NO);
	lpAlphaData = FramePointer(lpAlphaFrame, 0, y, YES);
	if (lpBitsData && lpAlphaData)
		copy(lpBitsData, lpAlphaData, dx);
	}
ProgressEnd();

MaskClose(lpAlpha);
// bag text bits alpha and create one base on new alpha frame
lpAlpha = MaskCreate(lpAlphaFrame, 0, 0, NO, Control.NoUndo, MaskType);
if (!lpAlpha)
   {
   FrameClose(lpAlphaFrame);
   ProgressEnd();
   return(SC_MEMERROR);
   }

// create a data frame and fill with the active color
lpDataFrame = FrameOpen(TypeInfo,
				  FrameXSize(lpAlphaFrame),
				  FrameYSize(lpAlphaFrame),
				  FrameResolution(lpAlphaFrame));
if (!lpDataFrame)
   {
   MaskClose(lpAlpha);
   ProgressEnd();
   return(SC_MEMERROR);
   }
dx = FrameXSize(lpDataFrame);
dy = FrameYSize(lpDataFrame);
ProgressBegin(1, 0);
for (y = 0; y < dy; ++y)
   {
   AstralClockCursor(y, dy, NO);
   if (lpData = FramePointer(lpDataFrame, 0, y, YES))
	   LoadColor(lpDataFrame, lpData, dx, &lpParms->Color);
   }
ProgressEnd();

GetObjectMarqueeRect(lpImage, lpUpdateRect);
res = FrameResolution(ImgGetBaseEditFrame(lpImage));
if (!CurrentTextObject)
   {
   // create and add text object to object list
   OffsetRect(&rAlpha, lpRect->left, lpRect->top);
   lpTextObject = ObjCreateFromFrame( ST_PERMANENT, lpDataFrame, lpAlpha,
								&rAlpha, Control.NoUndo );
   if (!lpTextObject)
	   {
	   MaskClose(lpAlpha);
	   FrameClose(lpDataFrame);
	   ProgressEnd();
	   return(SC_MEMERROR);
	   }

   // set opacity and merge mode for this object
   lpTextObject->Opacity = lpParms->Opacity;
   lpTextObject->MergeMode = lpParms->MergeMode;
   Pt.x = lpTextObject->rObject.left;
   Pt.y = lpTextObject->rObject.top;
   ResConvertUL(res, lpParms->iBaseRes, &Pt.x, &Pt.y);
   Pt.x = lpParms->DestPos.x - Pt.x;
   Pt.y = lpParms->DestPos.y - Pt.y;
   CString szTextBuf = lpParms->lpTextBuf;
   lpTextObject->lpText = new CTextObject(	szTextBuf,
   											lpParms->Common.idDirty,
   											lpParms->AntiAlias,
											&lpParms->Color,
											lpParms->Justification,
											TextForum.GetCurrentFont(),
											&Pt,
											lpParms->iBaseRes);

   // select our new object
   ImgAddNewObject(lpImage, lpTextObject);
   ImgGetSelObjectRect(lpImage, &rObjects, YES);

   ImgEditInit( lpImage, ET_OBJECT, UT_DELETEOBJECTS, ImgGetBase(lpImage));
   lpTextObject->fUndoDeleted = YES;
   ImgEditedObject(lpImage, ImgGetBase(lpImage), IDS_UNDOTEXT, NULL);
   }
else
   {
   lpTextObject = CurrentTextObject;
   lpTextObject->Opacity = lpParms->Opacity;
   lpTextObject->MergeMode = lpParms->MergeMode;
   lpTextObject->fDeleted = FALSE;
   ImgEditInit(lpImage, ET_OBJECT, UT_EDITTEXT, lpTextObject);
   if (lpTextObject->lpText)
      delete lpTextObject->lpText;
   Pt.x = lpTextObject->rObject.left;
   Pt.y = lpTextObject->rObject.top;
   ResConvertUL(res, lpParms->iBaseRes, &Pt.x, &Pt.y);
   Pt.x = lpParms->DestPos.x - Pt.x;
   Pt.y = lpParms->DestPos.y - Pt.y;
   CString szTextBuf = lpParms->lpTextBuf;
   lpTextObject->lpText = new CTextObject(	szTextBuf,
   											lpParms->Common.idDirty,
   											lpParms->AntiAlias,
											&lpParms->Color,
											lpParms->Justification,
											TextForum.GetCurrentFont(),
											&Pt,
											lpParms->iBaseRes);
   lpTextObject->rObject.right  = lpTextObject->rObject.left + dx - 1;
   lpTextObject->rObject.bottom = lpTextObject->rObject.top  + dy - 1;
   rObjects = lpTextObject->rObject;
	ImgEditedObjectFrame(lpImage, lpTextObject, IDS_UNDOEDITTEXT, NULL, lpDataFrame, lpAlphaFrame);
   CurrentTextObject = NULL;
   }
AstralUnionRect(lpUpdateRect, lpUpdateRect, &rObjects);

ProgressEnd();
return(SC_SUCCESS);
}

/************************************************************************/
void CTextRedrawHook::DisplayHookProc(HWND hWnd, HDC hDC, LPRECT lpRect )
/************************************************************************/
{
    TextTool.Info.TextDisplayLine(hDC, NO, YES);
}

/************************************************************************/

/************************************************************************/
LOCAL HFONT TextNewFont(HDC hDC, LPFONTID lpFont, LFIXED DispRate,
						BOOL fAllowScaling, int yres)
/************************************************************************/
{
	int iTextHeight, iAngle, iWeight;
	HFONT hFont;
	BYTE Italic, UnderLine, StrikeOut, Quality;

	iAngle      = TextGetAngle(hDC, lpFont);
	iTextHeight = ((long)lpFont->Size * yres ) / 72L;
	iTextHeight = FMUL(iTextHeight, DispRate);
	iTextHeight = bound(iTextHeight, 0, 16384);
	iWeight     = (lpFont->Weight ? FW_BOLD : FW_NORMAL );
	Italic      = (BYTE)(lpFont->Italic ? TRUE : FALSE);
	UnderLine   = (BYTE)(lpFont->Underline?TRUE:FALSE);
	StrikeOut   = (BYTE)(lpFont->Strikeout ? TRUE : FALSE);
	Quality     = (BYTE)(fAllowScaling ? DRAFT_QUALITY : PROOF_QUALITY);

	hFont = CreateFont(
			-iTextHeight,  // Height
			0,             // Width
			-(iAngle*10),  // Escapement
			0,             // Orientation
			iWeight,
			Italic,
			UnderLine,
			StrikeOut,
			lpFont->FontDesc.fdCharSet/*ANSI_CHARSET*/,
			OUT_CHARACTER_PRECIS,
			CLIP_DEFAULT_PRECIS,
			Quality,
			lpFont->FontDesc.fdPitchAndFamily/*DEFAULT_PITCH|FF_DONTCARE*/,
			lpFont->FontDesc.fdFaceName );

	if( GetObject( hFont, sizeof( LOGFONT ), &logFont ))
		ImeCreateFont( &TextIME, &logFont );
	return(hFont);
}

/************************************************************************/
LOCAL void TextGetTForm(HDC hDC, LPTFORM lpTForm, LPFONTID lpFont, LPPOINT lpPos, BOOL Inverse)
/************************************************************************/
{
int Angle;

TInit(lpTForm);
Angle = TextGetAngle(hDC, lpFont);
if (Angle)
	{
   if (Inverse)
      Angle = -Angle;
	TMove(lpTForm, -lpPos->x, -lpPos->y);
	TRotate(lpTForm, TOFIXED(Angle), TOFIXED(Angle));
	TMove(lpTForm, lpPos->x, lpPos->y);
	}
}

/************************************************************************/
LOCAL int TextGetAngle(HDC hDC, LPFONTID lpFont)
/************************************************************************/
{
if (TextCanRotate(hDC, &lpFont->FontDesc))
	return(lpFont->Angle);
else
	return(0);
}

/************************************************************************/
LPFRAME TextGetBitmapBits(HDC hDC, HBITMAP hBitmap, int iWidth, int iHeight,
						LPIMAGE lpImage)
/************************************************************************/
{
LPTR lpDIBits, lpOut;
LPFRAME lpBits;
long lSize;
int	y;
BMI TextBitmap;
int nLines;
int Resolution = FrameResolution(ImgGetBaseEditFrame(lpImage));
FRMTYPEINFO TypeInfo;
FRMDATATYPE FrameType = FDT_GRAYSCALE;

ImgGetTypeInfo(lpImage, &TypeInfo);
if (TypeInfo.DataType == FDT_LINEART && Mask.OneBitMask)
   FrameType = FDT_LINEART;

ProgressBegin(1, 0);
if (!(lpBits = FrameOpen(FrameType, iWidth, iHeight, Resolution)))
	{
	Message(IDS_EMEMALLOC);
	ProgressEnd();
	return(NULL);
	}
lSize = (iWidth + 7) / 8;
lSize = 4 * ((lSize + 3) / 4); // DIB packing
if (!(lpDIBits = Alloc(lSize)))
	{
	FrameClose(lpBits);
	Message(IDS_EMEMALLOC);
	ProgressEnd();
	return(NULL);
	}
// Setup the DIB structure
TextBitmap.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
TextBitmap.bmiHeader.biWidth = iWidth;
TextBitmap.bmiHeader.biHeight = iHeight;
TextBitmap.bmiHeader.biPlanes = 1;
TextBitmap.bmiHeader.biBitCount = 1;
TextBitmap.bmiHeader.biCompression = BI_RGB;
TextBitmap.bmiHeader.biSizeImage = 0; //lSize;
TextBitmap.bmiHeader.biXPelsPerMeter = 0;
TextBitmap.bmiHeader.biYPelsPerMeter = 0;
TextBitmap.bmiHeader.biClrUsed = 2;
TextBitmap.bmiHeader.biClrImportant = 0;

TextBitmap.bmi.Colors[0].rgbRed   = 0;
TextBitmap.bmi.Colors[0].rgbGreen = 0;
TextBitmap.bmi.Colors[0].rgbBlue  = 0;

TextBitmap.bmi.Colors[1].rgbRed   = 255;
TextBitmap.bmi.Colors[1].rgbGreen = 255;
TextBitmap.bmi.Colors[1].rgbBlue  = 255;

for (y = 0; y < iHeight; ++y)
	{
	AstralClockCursor(y, iHeight, NO);

	lpOut = FramePointer(lpBits, 0, y, YES);
	if (!lpOut)
		continue;
	nLines = GetDIBits(hDC, hBitmap, iHeight-y-1, 1, lpDIBits,
				(LPBITMAPINFO)&TextBitmap, DIB_RGB_COLORS);
	if (!nLines)
		continue;
	la2con(lpDIBits, iWidth, lpOut, YES); 
	}
FreeUp(lpDIBits);
ProgressEnd();
return(lpBits);
}

/************************************************************************/
LOCAL void TextMaskProc(int y, int left, int right, LPTR lpMsk, LPTEXTBITS lpBits )
/************************************************************************/
{
int      iCount;
LPTR     lpTextMask;

lpTextMask = FramePointer(lpBits->lpTextBits, left-lpBits->rBits.left, y-lpBits->rBits.top, NO);
if (lpTextMask)
	{
	iCount = right - left + 1;
	ScaleDataBuf8(lpMsk, iCount, lpTextMask);
	}
}

/************************************************************************/
LPFRAME TextAntiAlias(LPFRAME lpRasterBits, int iRasterWidth, int iRasterHeight,
		int iWidth, int iHeight)
/************************************************************************/
{
#define MAXVALUES 17
BOOL ret, bCursorEnable, bMessageEnable;
int	iValues[MAXVALUES], iEdgeValue, iCenterValue;
LPFRAME lpBits;

if (iRasterWidth != iWidth && iRasterHeight != iHeight)
	{
//	bCursorEnable = AstralCursorEnable(NO);
	bMessageEnable = MessageEnable(NO);
	lpBits = DoSmartSize(lpRasterBits, iWidth,
					iHeight, FrameResolution(lpRasterBits));
//	AstralCursorEnable(bCursorEnable);
	MessageEnable(bMessageEnable);
	FrameClose(lpRasterBits);
	return(lpBits);
	}
iCenterValue = 600;
if (iCenterValue == 1000)
	return(lpRasterBits);
iEdgeValue = (1000-iCenterValue)/8;
iValues[0] = 1;
iValues[1] = iEdgeValue;
iValues[2] = iEdgeValue;
iValues[3] = iEdgeValue;
iValues[4] = iEdgeValue;
iValues[5] = iCenterValue;
iValues[6] = iEdgeValue;
iValues[7] = iEdgeValue;
iValues[8] = iEdgeValue;
iValues[9] = iEdgeValue;
iValues[10] = 0;
iValues[11] = 0;
iValues[12] = 0;
iValues[13] = 0;
iValues[14] = 255;
iValues[15] = 0;
iValues[16] = 0;

bCursorEnable = AstralCursorEnable(NO);
bMessageEnable = MessageEnable(NO);
ret = ConvolveData( lpRasterBits, iValues );
AstralCursorEnable(bCursorEnable);
MessageEnable(bMessageEnable);
if (!ret)
	{
	FrameClose(lpRasterBits);
	return(NULL);
	}
return( lpRasterBits );
}

/************************************************************************/
LOCAL void SetBit(LPTR lp, int x)
/************************************************************************/
{
BYTE	bitmask;

lp += (x/8);
bitmask = 0x80;
if (x)
	bitmask >>= (x%8);
*lp |= bitmask;
}

/************************************************************************/
LOCAL void SetBackSlashBits(LPTR lp, int w, int h, WORD wBytes)
/************************************************************************/
{
int	x, iPixels, px;

clr(lp, h*wBytes);
x = -1;

while (--h >= 0)
	{
	px = x;
	iPixels = 3;

	while (--iPixels >= 0)
		{
		if (px >= 0 && px < w)
			SetBit(lp, px);
		++px;
		}
	++x;
	lp += wBytes;
	}
}

/************************************************************************/
LOCAL void SetSlashBits(LPTR lp, int w, int h, WORD wBytes)
/************************************************************************/
{
int	x, iPixels, px;

clr(lp, h*wBytes);
x = w - 2;
while (--h >= 0)
	{
	px = x;
	iPixels = 3;

	while (--iPixels >= 0)
		{
		if (px >= 0 && px < w)
			SetBit(lp, px);
		++px;
		}
	--x;
	lp += wBytes;
	}
}
