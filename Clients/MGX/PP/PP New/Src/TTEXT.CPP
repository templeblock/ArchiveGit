//®PL1¯®FD1¯®TP0¯®BT0¯
// (c) Copyright 1991 MICROGRAFX, Inc., All Rights Reserved.
// This material is confidential and a trade secret.
// Permission to use this work for any purpose must be obtained
// in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081

#include "pp.h"
#include "ppafx.h"
#include "id.h"
#include "text.h"
#include "textime.h"

extern LPOBJECT CurrentTextObject;

// LOCAL prototypes
LOCAL BOOL Text_OnInitDialog(HWND hDlg, HWND hWndFocus, LPARAM lParam);
LOCAL void Text_OnDocActivated(HWND hDlg);
LOCAL void Text_OnControlEnable(HWND hDlg, int fFlags);
LOCAL void Text_HandleEnable(HWND hDlg, int fFlags);
LOCAL void Text_OnCommand(HWND hDlg, int id, HWND hControl, UINT codeNotify);

extern void OnDelMBChar();

CFontCombo	FontCombo;
CFontCombo::CFontCombo()
{
	m_hWnd = NULL;
}

CFontCombo::~CFontCombo()
{
}

void CFontCombo::NotifyFontChanged(int code)
{
	int	 nFont;
	HWND hActiveWnd = GetActiveDoc();

	if ( code != CBN_SELCHANGE )
		return;
	nFont = SendMessage(m_hWnd, CB_GETCURSEL, 0, 0L);
	TextForum.SetFontIndex(nFont);
	ControlEnable(GetParent(m_hWnd), IDC_TEXT_ANGLE, TextCanRotate(NULL, &TextForum.GetCurrentFont()->FontDesc));
	if (Tool.bActive)
		TextFontChanged(hActiveWnd);
}

void CFontCombo::Init(HWND hWnd)
{
	if (hWnd)
	{
		ArriveAt(TextForum);
		m_hWnd = hWnd;
		TextForum.FillCombo(hWnd);
		Assimilate(&TextForum);
	}
	else
	{
		// only depart from forum if we ever joined
		if (m_hWnd)
		{
			m_hWnd = NULL;
			DepartFrom(TextForum);
		}
	}
}

void CFontCombo::Assimilate (CForum * pForum)
{
	if (pForum == &TextForum)
	{
   		ComboBox_SetCurSel(m_hWnd, TextForum.GetFontIndex());
	}
}

/***********************************************************************/
BOOL WINPROC EXPORT DlgTextProc( HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam )
/***********************************************************************/
{
switch (msg)
	{
	HANDLE_DLGRET(hDlg, WM_INITDIALOG, Text_OnInitDialog);  
	HANDLE_DLGMSG(hDlg, WM_COMMAND, Text_OnCommand);
	HANDLE_DLGMSG(hDlg, WM_DOCACTIVATED, Text_OnDocActivated);
	HANDLE_DLGMSG(hDlg, WM_CONTROLENABLE, Text_OnControlEnable);

	default:
	return Dialog_HandleDefaults(hDlg, msg, wParam, lParam, IDD_TEXT);
	}
}

/************************************************************************/
LOCAL void TextInitDefault(HWND hDlg, LPOBJECT lpObject)
{
	LPFONTID	lpFont = TextForum.GetCurrentFont();
   	FRMTYPEINFO TypeInfo;
   	BOOL fTypeEnable;

   	if (lpObject)
    {
    	CTextObject    *lpTextObj = lpObject->lpText;

		TextForum.SetAttr(&lpTextObj->FontID, lpTextObj->Justification, lpTextObj->AntiAlias);	
    }

	ImgGetTypeInfo(GetActiveImage(), &TypeInfo);
	fTypeEnable = 	TypeInfo.DataType != FDT_PALETTECOLOR &&
					TypeInfo.DataType != FDT_LINEART;

   	ComboBox_SetCurSel(GetDlgItem(hDlg, IDC_TEXT_FACES), TextForum.GetFontIndex());
   	InitDlgItemSpin(hDlg, IDC_TEXT_SIZE, lpFont->Size, NO, 1, 500);
   	SetDlgItemInt(hDlg, IDC_TEXT_SIZE, lpFont->Size, NO );
   	InitDlgItemSpin(hDlg, IDC_TEXT_ANGLE, lpFont->Angle, YES, -360, 360 );
   	ControlEnable(hDlg, IDC_TEXT_ANGLE, TextCanRotate(NULL, &lpFont->FontDesc));
   	CheckDlgButton(hDlg,  IDC_TEXT_ITALIC, lpFont->Italic );
   	CheckDlgButton(hDlg,  IDC_TEXT_UNDERLINE, lpFont->Underline );
   	CheckDlgButton(hDlg,  IDC_TEXT_BOLD, lpFont->Weight );
   	CheckDlgButton(hDlg,  IDC_TEXT_ANTIALIAS, TextForum.GetAntiAlias());
   	ControlEnable(hDlg, IDC_TEXT_ANTIALIAS, fTypeEnable);
   	CheckDlgButton(hDlg,  IDC_TEXT_LEFT, TRUE );
   	InitDlgItemSpin(hDlg, IDC_TRANSPARENT, TOPERCENT(255-TextForum.GetOpacity()),NO, 0, 99);
   	ControlEnable(hDlg, IDC_TRANSPARENT, fTypeEnable);

	int id = TextForum.GetJustification() + IDC_TEXT_LEFT;
	SendDlgItemMessage(hDlg, IDC_TEXT_LEFT  , BM_SETSTATE, id==IDC_TEXT_LEFT  ,0L);
	SendDlgItemMessage(hDlg, IDC_TEXT_CENTER, BM_SETSTATE, id==IDC_TEXT_CENTER,0L);
	SendDlgItemMessage(hDlg, IDC_TEXT_RIGHT , BM_SETSTATE, id==IDC_TEXT_RIGHT ,0L);

   	if (lpObject)
      	CurrentTextObject = lpObject;
}

/************************************************************************/
static BOOL    StartEditing = FALSE;
static HWND    hTextDlg;
/************************************************************************/
LOCAL BOOL Text_OnInitDialog(HWND hDlg, HWND hWndFocus, LPARAM lParam)
/************************************************************************/
{
	hTextDlg = hDlg;
	TextInitDefault(hDlg, NULL);
	FontCombo.Init(GetDlgItem( hDlg, IDC_TEXT_FACES));
	Text_HandleEnable(hDlg, 0);
	return(FALSE);
}

/************************************************************************/
LOCAL void Text_OnDocActivated(HWND hDlg)
/************************************************************************/
{
Text_HandleEnable(hDlg, 0);
}

/************************************************************************/
LOCAL void Text_OnControlEnable(HWND hDlg, int fFlags)
/************************************************************************/
{
Text_HandleEnable(hDlg, fFlags);
}

/************************************************************************/
LOCAL void Text_HandleEnable(HWND hDlg, int fFlags)
/************************************************************************/
{
HandleMergeCombo(hDlg, IDC_MERGEMODE, TRUE, TextForum.GetMergeMode(), 0L);
}

/************************************************************************/
LOCAL void Text_OnCommand(HWND hDlg, int id, HWND hControl, UINT codeNotify)
/************************************************************************/
{
BOOL Bool;
int i;
MERGE_MODE MergeMode;
HWND hActiveWnd;

hActiveWnd = GetActiveDoc();
switch (id)
	{
#ifndef ID_MAGIC
	case IDC_TEXT:
	if ( !SaveToolPreferences( id ) )
		break;
	break;
#endif

	case IDC_TRANSPARENT:
	if ( GetFocus() != GetDlgItem( hDlg, id ) )
		break;
	if ( codeNotify != EN_CHANGE )
		break;
	i = GetDlgItemSpin( hDlg, id, &Bool, NO );
	i= TOGRAY(i);
	SetCommonOpacity(255-i);
	break;

	case IDC_MERGEMODE:
	if ((MergeMode = HandleMergeCombo( hDlg, id, FALSE, MM_NORMAL, codeNotify )) < 0)
		break;
	SetCommonMergeMode(MergeMode);
	break;

	case IDC_TEXT_SIZE:
	if ( GetFocus() != GetDlgItem( hDlg, id ) )
		break;
	if ( codeNotify != EN_CHANGE )
		break;
   i = GetDlgItemInt( hDlg, id, &Bool, NO);
   if (!i)
      break;
	TextForum.GetCurrentFont()->Size = i;
	if (Tool.bActive)
		TextFontChanged(hActiveWnd);
	break;

   case IDC_TEXT_LEFT:
   case IDC_TEXT_RIGHT:
   case IDC_TEXT_CENTER:
	TextForum.SetJustification(id - IDC_TEXT_LEFT);
	SendDlgItemMessage(hDlg, IDC_TEXT_LEFT  , BM_SETSTATE, id==IDC_TEXT_LEFT  ,0L);
	SendDlgItemMessage(hDlg, IDC_TEXT_CENTER, BM_SETSTATE, id==IDC_TEXT_CENTER,0L);
	SendDlgItemMessage(hDlg, IDC_TEXT_RIGHT , BM_SETSTATE, id==IDC_TEXT_RIGHT ,0L);
	if (Tool.bActive)
		TextFontChanged(hActiveWnd);
	break;

	case IDC_TEXT_BOLD:
	TextForum.GetCurrentFont()->Weight = !TextForum.GetCurrentFont()->Weight;
	CheckDlgButton( hDlg, id, TextForum.GetCurrentFont()->Weight );
	if (Tool.bActive)
		TextFontChanged(hActiveWnd);
	break;

	case IDC_TEXT_ITALIC:
	TextForum.GetCurrentFont()->Italic = !TextForum.GetCurrentFont()->Italic;
	CheckDlgButton( hDlg, id, TextForum.GetCurrentFont()->Italic );
	if (Tool.bActive)
		TextFontChanged(hActiveWnd);
	break;

	case IDC_TEXT_UNDERLINE:
	TextForum.GetCurrentFont()->Underline = !TextForum.GetCurrentFont()->Underline;
	CheckDlgButton( hDlg, id, TextForum.GetCurrentFont()->Underline );
	if (Tool.bActive)
		TextFontChanged(hActiveWnd);
	break;

	case IDC_TEXT_STRIKEOUT:
	TextForum.GetCurrentFont()->Strikeout = !TextForum.GetCurrentFont()->Strikeout;
	CheckDlgButton(hDlg, id, TextForum.GetCurrentFont()->Strikeout);
	if (Tool.bActive)
		TextFontChanged(hActiveWnd);
	break;

	case IDC_TEXT_FACES:
		FontCombo.NotifyFontChanged(codeNotify);
		break;

	case IDC_TEXT_ANGLE:
	if ( GetFocus() != GetDlgItem( hDlg, id ) )
		break;
	if ( codeNotify != EN_CHANGE )
		break;
   if (GetWindowTextLength(GetDlgItem(hDlg, id)) == 0)
      TextForum.GetCurrentFont()->Angle = 0;
   else
	   TextForum.GetCurrentFont()->Angle = GetDlgItemSpin( hDlg, id, &Bool, YES );
	if (Tool.bActive)
		TextFontChanged(hActiveWnd);
	break;

	case IDC_TEXT_ANTIALIAS:
	TextForum.SetAntiAlias(!TextForum.GetAntiAlias());
	CheckDlgButton( hDlg, id, TextForum.GetAntiAlias() );
	if (Tool.bActive)
		TextFontChanged(hActiveWnd);
	break;

   default:
	break;
   }
}

extern LPOBJECT	FindTextObject(LPIMAGE lpImage, POINT Pt);
extern void 	TextMoveCursor(HWND hWnd, int x, int y);
extern STRING 	szAppName;
extern LOGFONT 	logFont;

/************************************************************************/
void TextProc( HWND hWindow, UINT msg, int x, int y, UINT32 Option )
/************************************************************************/
{
	BOOL fCancel = NO;
	WORD wKey;
	static WORD wChar;

	switch (msg)
   	{
   		case WM_SETCURSOR:
	   		if (Tool.bActive)
 		   		SetWindowCursor(ID_IBEAM);
      		else
 		   		SetWindowCursor(Tool.Prim.idCursor);
      		break;

   		case WM_CREATE:	// The first mouse down message
      		{
      		LPOBJECT lpObject = NULL;
      		LPIMAGE  lpImage;
      		POINT    pt;
      		BOOL     ret = TRUE;

      		lpImage = GetImagePtr(hWindow);
      		pt.x = x;
      		pt.y = y;
      		Display2File(hWindow,(LPINT)&pt.x, (LPINT)&pt.y);
      		lpObject = FindTextObject(lpImage, pt);
      		if (lpObject)
         		{
         		x = lpObject->rObject.left;
         		y = lpObject->rObject.top;
         		File2Display(hWindow,&x, &y);
         		}
      		if (lpObject && lpObject->lpText->Dirty)
         		{
         		LPTSTR lp;
	      		AstralStr(IDS_OKREEDITTEXT, &lp);
         		ret = ::MessageBox(PictPubApp.Get_hWndAstral(), lp, szAppName, MB_OKCANCEL | MB_ICONHAND | MB_TASKMODAL) != IDCANCEL;
         		FORWARD_WM_LBUTTONUP(hWindow, x, y, Option, PostMessage);
         		}
      
      		if (ret)
         		{
         		TextInitDefault(hTextDlg, lpObject);
	      		if (TextBegin(hWindow, x, y))
            		{
		      		StartEditing = TRUE;
		      		Tool.bActive = YES;
            		}
         		}
      		}
	   		break;

   		case WM_ACTIVATE:
   			if (!Option)
	   			FontCombo.Init(NULL);
	   		break;

   		case WM_LBUTTONDOWN:
	   		if (!StartEditing)
         		{
	      		if (SHIFT)
	         		TextMove(hWindow, x, y);
         		else
            		TextMoveCursor(hWindow, x, y);
         		}
      		else
         		StartEditing = FALSE;
	   		break;

   		case WM_LBUTTONUP:
	   		break;

   		case WM_MOUSEMOVE:	// sent when ToolActive is on
	   		break;

   		case WM_KILLFOCUS:
      		if (Tool.bActive)
			{
				TerminateIME();
	      		TextDisplayCaret(NULL, NULL);
			}
	   		break;

   		case WM_SETFOCUS:
      		if (Tool.bActive)
			{
				SetupIME( hWindow, &logFont );
	      		TextDisplayCaret(hWindow, NULL);
			}
	   		break;

   		case WM_CHAR:
			if( IsDBCSLeadByte( LOBYTE( Option )) && !wChar )	// if lead byte,
				wChar =  LOBYTE( Option );  					// save it and wait for next char
			else
			{
				if( wChar )									// if we already have the
					wChar = wChar | ( Option << 8 );		// lead byte, make DB char
				else
					wChar = Option;							// else it's a SB char

			   	TextKeystroke( hWindow, ( WORD )wChar );	// display char	and
				wChar = 0;									// reset flag
			}
	   		break;

   		case WM_KEYDOWN:
	   		wKey = (WORD)Option;
	   		switch (wKey)
		   		{
         		case VK_DELETE:
            		OnDelMBChar();
			   		break;
		   		case VK_UP:
			   		TextRMove(hWindow, 0, -1);
			   		break;
		   		case VK_DOWN:
			   		TextRMove(hWindow, 0, 1);
			   		break;
		   		case VK_LEFT:
			   		TextRMove(hWindow, -1, 0);
			   		break;
		   		case VK_RIGHT:
			   		TextRMove(hWindow, 1, 0);
			   		break;
		   		default:
			   		break;
		   		}
	   		break;

   		case WM_KEYUP:
	   		break;

   		case WM_DESTROY:	// The cancel operation message
	   		fCancel = Option == 1L;
   		case WM_LBUTTONDBLCLK:
      		if (!Tool.bActive)
         		break;
      		TextEnd(hWindow, fCancel);
	   		break;
   	}
}
