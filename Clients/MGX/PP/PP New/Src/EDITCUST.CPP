//
// (c) Copyright 1991 MICROGRAFX, Inc., All Rights Reserved.
// This material is confidential and a trade secret.
// Permission to use this work for any purpose must be obtained
// in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081
#include "pp.h"
#include "ppafx.h"
#include "id.h"
#include "editcust.h"
#include "custools.h"

#ifdef _DEBUG
#undef THIS_FILE
static TCHAR BASED_CODE THIS_FILE[] = __FILE__;
#endif

#define CATEGORY_MAX  11
static UINT BASED_CODE   CategoryMap[CATEGORY_MAX] = {0,22,34,47,60,79,102,117,122,128,173};
static int BASED_CODE  MainTools[9] = {128,129,137,144,149,154,158,166,167};
CString NewToolBoxName = _T("");
BOOL    CreatingCustom = FALSE;

#define CUST_CMD_MAX    173

CUSTCMD BASED_CODE CustCmdMap[CUST_CMD_MAX] = {     // file menu commands
      {IDM_NEW,           0,IDS_CUSTFILECMD+0 },
      {IDM_OPEN,          0,IDS_CUSTFILECMD+2 },
      {IDM_IMPORT,        0,IDS_CUSTFILECMD+4 },
      {IDM_SCAN,          0,IDS_CUSTFILECMD+6 },
      {IDM_ACQUIRE,       0,IDS_CUSTFILECMD+8 },
      {IDM_SAVE,          0,IDS_CUSTFILECMD+10},
      {IDM_SAVEAS,        0,IDS_CUSTFILECMD+12},
      {IDM_REVERT,        0,IDS_CUSTFILECMD+14},
      {IDM_CLOSE,         0,IDS_CUSTFILECMD+16},
      {IDM_PRINT,         0,IDS_CUSTFILECMD+18},
      {IDM_PRINTPREVIEW,  0,IDS_CUSTFILECMD+20},
      {IDM_PRINTSETUP,    0,IDS_CUSTFILECMD+22},
      {IDM_SCANSETUP,     0,IDS_CUSTFILECMD+24},
      {IDM_MONITORMAP,    0,IDS_CUSTFILECMD+26},
      {IDM_CALIBRATESCAN, 0,IDS_CUSTFILECMD+28},
      {IDM_CALIBRATEPRINT,0,IDS_CUSTFILECMD+30},
      {IDM_MACROPLAY,     0,IDS_CUSTFILECMD+32},
      {IDM_MACROPLAYBATCH,0,IDS_CUSTFILECMD+34},
      {IDM_MACRORECORD,   0,IDS_CUSTFILECMD+36},
      {IDM_MACRO_EDIT,    0,IDS_CUSTFILECMD+38},
      {IDM_MACROSTOP,     0,IDS_CUSTFILECMD+40},
      {IDM_EXIT,          0,IDS_CUSTFILECMD+42},

      // 22                                     // edit menu commands
      {IDM_UNDO,         0,IDS_CUSTEDITCMD+0 },
      {IDM_APPLY,        0,IDS_CUSTEDITCMD+2 },
      {IDM_CMDLIST_UNDO, 0,IDS_CUSTEDITCMD+4 },
      {IDM_CMDLIST_REDO, 0,IDS_CUSTEDITCMD+6 },
      {IDM_CMDLIST_EDIT, 0,IDS_CUSTEDITCMD+8 },
      {IDM_CUT,          0,IDS_CUSTEDITCMD+10},
      {IDM_COPY,         0,IDS_CUSTEDITCMD+12},
      {IDM_COPYTOFILE,   0,IDS_CUSTEDITCMD+14},
      {IDM_PASTE,        0,IDS_CUSTEDITCMD+16},
      {IDM_PASTEASNEW,   0,IDS_CUSTEDITCMD+18},
      {IDM_DELETE,       0,IDS_CUSTEDITCMD+20},
      {IDM_PASTEFROMFILE,0,IDS_CUSTEDITCMD+22},

      // 34                                      // Mask menu commands
      {IDM_MASKUNDO,       0,IDS_CUSTMASKCMD+0 },
      {IDM_MASKREMOVE,     0,IDS_CUSTMASKCMD+2 },
      {IDM_MASKLOAD,       0,IDS_CUSTMASKCMD+4 },
      {IDM_MASKSAVE,       0,IDS_CUSTMASKCMD+6 },
      {IDM_CHROMA_MASK,    0,IDS_CUSTMASKCMD+8 },
      {IDM_MASKFROMOBJECT, 0,IDS_CUSTMASKCMD+10}, 
      {IDM_MASKINVERT,     0,IDS_CUSTMASKCMD+12},
      {IDM_FEATHER,        0,IDS_CUSTMASKCMD+14},
      {IDM_REMOVE_HOLES,   0,IDS_CUSTMASKCMD+16},
      {IDM_MASK_SMOOTH,    0,IDS_CUSTMASKCMD+18},
      {IDM_CROP,           0,IDS_CUSTMASKCMD+20}, 
      {IDM_MASKSTROKE,     0,IDS_CUSTMASKCMD+22},
      {IDM_HIDEMARQUEE,    0,IDS_CUSTMASKCMD+24},

      // 47                                      // Map menu commands
      {IDM_MAP,            0,IDS_CUSTMAPCMD+0 },
      {IDM_CONTBRIT_STICK, 0,IDS_CUSTMAPCMD+2 },
      {IDM_CONTBRIT_VISUAL,0,IDS_CUSTMAPCMD+4 },
      {IDM_COLORBAL_STICK, 0,IDS_CUSTMAPCMD+6 },
      {IDM_COLORBAL_VISUAL,0,IDS_CUSTMAPCMD+8 },
      {IDM_QUARTERTONE,    0,IDS_CUSTMAPCMD+10},
      {IDM_POSTERIZE,      0,IDS_CUSTMAPCMD+12},
      {IDM_THRESHOLD,      0,IDS_CUSTMAPCMD+14},
      {IDM_HUE_SHIFT,      0,IDS_CUSTMAPCMD+16},
      {IDM_HUE,            0,IDS_CUSTMAPCMD+18},
      {IDM_HISTOGRAM,      0,IDS_CUSTMAPCMD+20},
      {IDM_CALIBRATEAPPLY, 0,IDS_CUSTMAPCMD+22},
      {IDM_PALETTEEDIT,    0,IDS_CUSTMAPCMD+24},

      // 60                                      // Object menu commands
      {IDM_OBJ_GROUP,       0,IDS_CUSTOBJCMD+0 },
      {IDM_OBJ_UNGROUP,     0,IDS_CUSTOBJCMD+2 },
      {IDM_OBJ_LOCK,        0,IDS_CUSTOBJCMD+4 },
      {IDM_OBJ_UNLOCK,      0,IDS_CUSTOBJCMD+6 },
      {IDM_OBJ_ALIGN,       0,IDS_CUSTOBJCMD+8 },
      {IDM_OBJ_POSITION,    0,IDS_CUSTOBJCMD+10},
      {IDM_OBJ_DOWNLEVEL,   0,IDS_CUSTOBJCMD+12},
      {IDM_OBJ_UPLEVEL,     0,IDS_CUSTOBJCMD+14},
      {IDM_OBJ_MOVETOBOTTOM,0,IDS_CUSTOBJCMD+16},
      {IDM_OBJ_MOVETOTOP,   0,IDS_CUSTOBJCMD+18},
      {IDM_OBJ_COMBOBJS,    0,IDS_CUSTOBJCMD+20},
      {IDM_COMBINE,         0,IDS_CUSTOBJCMD+22},
      {IDM_COMBINE_SEL,     0,IDS_CUSTOBJCMD+24},
      {IDM_OBJ_FEATHER,     0,IDS_CUSTOBJCMD+26},
      {IDM_OBJ_APPLYMASK,   0,IDS_CUSTOBJCMD+28},
      {IDM_OBJECTFROMMASK,  0,IDS_CUSTOBJCMD+30},
      {IDM_OBJ_DELETE,      0,IDS_CUSTOBJCMD+32},
      {IDM_OBJ_SELECTALL,   0,IDS_CUSTOBJCMD+34},
      {IDM_OBJ_HIDEMARQUEE, 0,IDS_CUSTOBJCMD+36}, 

      // 79                                      // Image menu commands
      {IDM_SIZE,           0,IDS_CUSTIMAGECMD+0 },
      {IDM_EXPAND,         0,IDS_CUSTIMAGECMD+2 },
      {IDM_ROTATE90,       0,IDS_CUSTIMAGECMD+4 },
      {IDM_ROTATE270,      0,IDS_CUSTIMAGECMD+6 },
      {IDM_ROTATE180,      0,IDS_CUSTIMAGECMD+8 },
      {IDM_ROTATEANY,      0,IDS_CUSTIMAGECMD+10},
      {IDM_MIRRORX,        0,IDS_CUSTIMAGECMD+12},
      {IDM_MIRRORY,        0,IDS_CUSTIMAGECMD+14},
      {IDM_MIRRORXY,       0,IDS_CUSTIMAGECMD+16},
      {IDM_SPLITRGB,       0,IDS_CUSTIMAGECMD+18},
      {IDM_SPLITHSL,       0,IDS_CUSTIMAGECMD+20},
      {IDM_SPLITCMYK,      0,IDS_CUSTIMAGECMD+22},
      {IDM_CHANNELCOMBINE, 0,IDS_CUSTIMAGECMD+24},
      {IDM_CONVLA,         0,IDS_CUSTIMAGECMD+26},
      {IDM_CONVSP,         0,IDS_CUSTIMAGECMD+28},
      {IDM_CONVCT,         0,IDS_CUSTIMAGECMD+30}, 
      {IDM_CONVPALETTE,    0,IDS_CUSTIMAGECMD+32},
      {IDM_CONVRGBCOLOR,   0,IDS_CUSTIMAGECMD+34},
      {IDM_CONVCMYKCOLOR,  0,IDS_CUSTIMAGECMD+36},
      {IDM_COLORMANAGED,   0,IDS_CUSTIMAGECMD+44},
      {IDM_INVERT,         0,IDS_CUSTIMAGECMD+38},
      {IDC_STITCH,         0,IDS_CUSTIMAGECMD+40},
      {IDM_SPECIALFX,      0,IDS_CUSTIMAGECMD+42},

      // 102                                            // option menu commands      
      {IDM_SAVEPOSITIONS,        0,IDS_CUSTOPTIONCMD+0 },
      {IDM_SCRATCHPAD,           0,IDS_CUSTOPTIONCMD+2 },
      {IDM_CUSTBOXLOAD,          0,IDS_CUSTOPTIONCMD+4 },
      {IDM_CUSTBOXCREATE,        0,IDS_CUSTOPTIONCMD+6 },
      {IDM_BHINTSSHOW,           0,IDS_CUSTOPTIONCMD+8 },
      {IDM_SHINTSSHOW,           0,IDS_CUSTOPTIONCMD+10},
      {IDM_TOOLSHOW,             0,IDS_CUSTOPTIONCMD+12},
      {IDC_STATUSPALETTE,        0,IDS_CUSTOPTIONCMD+14},
      {IDM_SHOWINFO,             0,IDS_CUSTOPTIONCMD+16},
      {IDM_SHOWZOOMBOX,          0,IDS_CUSTOPTIONCMD+18},
      {IDM_OBJ_MANAGER,          0,IDS_CUSTOPTIONCMD+20},
      {IDM_STATUSSHOW,           0,IDS_CUSTOPTIONCMD+22},
      {IDM_RULERSHOW,            0,IDS_CUSTOPTIONCMD+24},
      {IDC_STATUSINFO,           0,IDS_CUSTOPTIONCMD+26},
      {IDM_PREF,                 0,IDS_CUSTOPTIONCMD+28},

      //117                                      // window menu commands
      {ID_WINDOW_CASCADE,  0,IDS_CUSTWNDCMD+0 },
      {ID_WINDOW_TILE_HORZ,0,IDS_CUSTWNDCMD+2 },
      {ID_WINDOW_ARRANGE,  0,IDS_CUSTWNDCMD+4 },
      {ID_WINDOW_NEW,      0,IDS_CUSTWNDCMD+6 },
      {IDM_CLOSEALL,       0,IDS_CUSTWNDCMD+10},

      // 122                                     // help menu commands
      {IDM_HELP_CONTEXT,   0,IDS_CUSTHELPCMD+0 },
      {IDM_HELP_INDEX,     0,IDS_CUSTHELPCMD+2 },
      {IDM_HELP_USING,     0,IDS_CUSTHELPCMD+4 },
      {IDM_HELP_README,    0,IDS_CUSTHELPCMD+6 },
      {IDM_HELP_HELPONHELP,0,IDS_CUSTHELPCMD+8 },
      {IDM_ABOUT,          0,IDS_CUSTHELPCMD+10},

      // 128                                  	 // tools
      {IDC_SELECTOR,  1,IDS_CUSTTOOLCMD+24},

      // 129 masking tools
      {IDC_MASK,        1,IDS_CUSTTOOLCMD+25},
      {IDC_SQUARE,      1,IDS_CUSTTOOLCMD},
      {IDC_FREE,        1,IDS_CUSTTOOLCMD+1}, 
      {IDC_PAINTON,     1,IDS_CUSTTOOLCMD+2},
      {IDC_MAGICWAND,   1,IDS_CUSTTOOLCMD+3}, 
      {IDC_TRANSFORMER, 1,IDS_CUSTTOOLCMD+4},
      {IDC_POINTER,     1,IDS_CUSTTOOLCMD+5},
      {IDC_CROP,        1,IDS_CUSTTOOLCMD+6},

      // 137 retouch tools
      {IDC_RETOUCH,  1,IDS_CUSTTOOLCMD+26},
      {IDC_PAINT,    1,IDS_CUSTTOOLCMD+7},
      {IDC_SPRAY,    1,IDS_CUSTTOOLCMD+8},
      {IDC_CLONE,    1,IDS_CUSTTOOLCMD+9},
      {IDC_TEXTURE,  1,IDS_CUSTTOOLCMD+10},
      {IDC_SMEAR,    1,IDS_CUSTTOOLCMD+11},
      {IDC_ERASER,   1,IDS_CUSTTOOLCMD+12},

      // 144 filter tools
      {IDC_FILTER,   1,IDS_CUSTTOOLCMD+27},
      {IDC_SHARP,    1,IDS_CUSTTOOLCMD+13},
      {IDC_SMOOTH,   1,IDS_CUSTTOOLCMD+14},
      {IDC_LIGHTEN,  1,IDS_CUSTTOOLCMD+15},
      {IDC_DARKEN,   1,IDS_CUSTTOOLCMD+16},

      // 149 fill tools
      {IDC_FILL,        1,IDS_CUSTTOOLCMD+28},
      {IDC_VIGNETTE,    1,IDS_CUSTTOOLCMD+17},
      {IDC_TEXTUREFILL, 1,IDS_CUSTTOOLCMD+18},
      {IDC_TINTFILL,    1,IDS_CUSTTOOLCMD+19},
      {IDC_FLOOD,       1,IDS_CUSTTOOLCMD+20},

      // 154 draw tools
      {IDC_DRAW,        1,IDS_CUSTTOOLCMD+29},
      {IDC_DRAWSQUARE,  1,IDS_CUSTTOOLCMD+21},
      {IDC_DRAWFREE,    1,IDS_CUSTTOOLCMD+22},
      {IDC_PENCIL,      1,IDS_CUSTTOOLCMD+23},

      // 158
      {IDC_CUSTOMVIEW,  1,IDS_CUSTTOOLCMD+30},
      {IDC_ZOOMIN,      0,IDS_CUSTTOOLCMD+33},
      {IDC_ZOOMOUT,     0,IDS_CUSTTOOLCMD+34},
      {IDC_VIEWLAST,    1,IDS_CUSTTOOLCMD+35},
      {IDC_VIEW1TO1,    1,IDS_CUSTTOOLCMD+36},
      {IDC_VIEWALL,     1,IDS_CUSTTOOLCMD+37},
      {IDC_VIEWFULL,    1,IDS_CUSTTOOLCMD+38},
      {IDC_QUICKZOOM,   1,IDS_CUSTTOOLCMD+44},

      // 166
      {IDC_TEXT,      1,IDS_CUSTTOOLCMD+31}, 
      {IDC_PROBE,     1,IDS_CUSTTOOLCMD+32},

      // 168      
      {IDC_STATUSINFO,     1,IDS_CUSTTOOLCMD+43},
      {IDC_MASKEDITALPHA,  1,IDS_CUSTTOOLCMD+39},
      {IDC_VIEWSLIME,      1,IDS_CUSTTOOLCMD+40},
      {IDC_STATUSPALETTE,  1,IDS_CUSTTOOLCMD+41},
      {IDC_STATUSSHIELD,   1,IDS_CUSTTOOLCMD+42}
      };

BOOL CCustToolBox::LoadMain()
{
    CCustTool     *lpct;
    LPTSTR         lp;
    CString        SName;
    CUSTCMD       *lpCmd;

    m_Patch = TRUE;
    m_Width  = 1;

    // skip selector tool, if LE version
    for( int i = ( Control.Retail ? 0:1 ); i < 9; i++ ) 
    {
        lpCmd = &CustCmdMap[MainTools[i]];

        AstralStr(lpCmd->LongName, &lp);

		if( IsDBCSLeadByte(( BYTE )*lp ))
		{
			if( *(( LPWORD )lp ) == _T(' '))
			{
				if( IsDBCSLeadByte(( BYTE )*MyCharNextN( lp, 2 )))
					( LPWORD )SName = ( LPWORD )MyCharNextN( lp, 2 );
				else
					SName = MyCharNextN( lp, 2 );
			}
			else
				( LPWORD )SName = ( LPWORD )lp;
		}
		else
		{
			if( *lp == _T(' '))
			{
				if( IsDBCSLeadByte(( BYTE )*MyCharNextN( lp, 2 )))
					( LPWORD )SName = ( LPWORD )MyCharNextN( lp, 2 );
				else
					SName = MyCharNextN( lp, 2 );
			}
			else
				SName = lp;
		}

        //SName = lp[0] == _T(' ') ? &lp[2] : lp;

        lpct = new CCustTool(CUSTOOL_ICON, lpCmd->Cmd, SName);
        Add(lpct);
    }
    return(TRUE);
}


/***************************************************************************/
/*                                  CToolList                              */
/***************************************************************************/

CToolList::CToolList()
{
}

BOOL CToolList::FillIt(HWND hDlg, int ID)
{
    int      i,s,e;
    FNAME    szFileName;

    ResetContent();
    if (ID != -1)
    {
        if ((ID+1) == CATEGORY_MAX)  // Macro Category
        {
            szFileName[0] = _T('\0');
            InitExtName(hDlg, IDC_TOOLLIST, NULL, Names.Macro, IDN_MACRO);
        }
        else
        {
            CUSTCMD     *rec;
            LPTSTR        lp;
            s = CategoryMap[ID++];
            e = CategoryMap[ID];
   
            for (i=s; i < e; i++)
            {
                rec = &CustCmdMap[i];
                if (AstralStr(CustCmdMap[i].LongName, &lp))
                    AddString(lp);
                else
                    break;
            }
        }
        SetCurSel(0);
    }
    return(TRUE);
   }

/***************************************************************************/
/*                                  CCategoryList                          */
/***************************************************************************/

CCategoryList::CCategoryList()
   {
   }

BOOL CCategoryList::FillIt()
   {
   LPTSTR    lp;

   ResetContent();
   for (int i= IDS_CUSTCATLIST; i < (IDS_CUSTCATLIST+CATEGORY_MAX); i++)
      {
      if (AstralStr(i, &lp))
         AddString(lp);
      else
         break;
      }
   return(TRUE);
   }

/***************************************************************************/
/*                            CEditCustTool dialog                         */
/***************************************************************************/

CEditCustTool::CEditCustTool(CCustToolBox *ptb, CWnd* pParent /*=NULL*/)
	: CDialog(CEditCustTool::IDD, pParent)
{
	//{{AFX_DATA_INIT(CEditCustTool)
	m_CatVal  = 9;
	m_ListVal = -1;
	m_NameVal = _T("");
   m_pToolBox = new CCustToolBox(*ptb);
   m_WidthVal = m_pToolBox->GetWidth();
   m_ColorPatch = m_pToolBox->IsPatchUp();
   m_Original = ptb;
   m_LastIcon = IDC_ICONPREVIEW;
   m_LastSelectedIcon = FALSE;
	//}}AFX_DATA_INIT
}

CEditCustTool::~CEditCustTool()
   {
   delete m_pToolBox;
   }

BOOL CEditCustTool::FillToolBox()
   {
   POSITION        Pos;
   CString         Name;
   LPTSTR          lp;
   CCustTool      *pctb;
   CObList        *List = m_pToolBox->GetList();
   CWnd           *pListBox;

   GetDlgItem(IDC_TOOLBOX)->SendMessage(LB_RESETCONTENT, (WPARAM)0, (LPARAM)0);
   GetDlgItem(IDC_TOOLBOXTOOLS)->SendMessage(LB_RESETCONTENT, (WPARAM)0, (LPARAM)0);
   for (Pos = List->GetHeadPosition(); Pos;)
      {
      pctb = (CCustTool *)List->GetNext(Pos);
      Name = pctb->GetShortName();
      lp   = Name.GetBuffer(256);
      if (pctb->GetType() == CUSTOOL_ICON)
         pListBox = GetDlgItem(IDC_TOOLBOXTOOLS);
      else
         pListBox = GetDlgItem(IDC_TOOLBOX);
      pListBox->SendMessage(LB_ADDSTRING, 0, (LPARAM)lp);
      Name.ReleaseBuffer();
      }
   return(TRUE);
   }


void CEditCustTool::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CEditCustTool)
	DDX_Control(pDX, IDC_TOOLNAME, m_ToolName);
	DDX_Control(pDX, IDC_TOOLBOXWIDTH, m_Width);
	DDX_Control(pDX, IDC_TOOLLIST, m_Tools);
	DDX_Control(pDX, IDC_DEL, m_Delete);
	DDX_Control(pDX, IDC_CATEGORY, m_Category);
	DDX_Control(pDX, IDC_ADD, m_Add);
	DDX_CBIndex(pDX, IDC_CATEGORY, m_CatVal);
	DDX_LBIndex(pDX, IDC_TOOLLIST, m_ListVal);
	DDX_Text(pDX, IDC_TOOLNAME, m_NameVal);
	DDX_Text(pDX, IDC_TOOLBOXWIDTH, m_WidthVal);
	DDV_MaxChars(pDX, m_NameVal, 256);
   DDV_MinMaxInt(pDX, m_WidthVal, 1, 99);
	//}}AFX_DATA_MAP
}

#define ON_LBN_MOVEITEM(id, memberFxn) \
	ON_CONTROL(LBN_MOVEITEM, id, memberFxn)

#define ON_EN_CHANGE(id, memberFxn) \
	ON_CONTROL(EN_CHANGE, id, memberFxn)

BEGIN_MESSAGE_MAP(CEditCustTool, CDialog)
	//{{AFX_MSG_MAP(CEditCustTool)
	ON_BN_CLICKED(IDC_INSERT, OnInsert)
	ON_BN_CLICKED(IDC_ADD, OnAdd)
   ON_BN_CLICKED(IDC_COLORPATCH, OnColorPatch)
	ON_CBN_SELCHANGE(IDC_CATEGORY, OnSelchangeCategory)
	ON_BN_CLICKED(IDC_DEL, OnDel)
	ON_LBN_MOVEITEM(IDC_TOOLBOX,  OnMoveItem)
	ON_LBN_MOVEITEM(IDC_TOOLBOXTOOLS,  OnMoveItemTools)
	ON_LBN_SELCHANGE(IDC_TOOLBOX, OnSelchangeToolbox)
	ON_LBN_SELCHANGE(IDC_TOOLBOXTOOLS, OnSelchangeToolboxTools)
	ON_EN_CHANGE(IDC_TOOLBOXWIDTH,  OnSpin)
	ON_LBN_SELCHANGE(IDC_TOOLLIST, OnSelchangeToollist)
	ON_EN_KILLFOCUS(IDC_TOOLNAME, OnKillfocusToolname)
   ON_WM_SETCURSOR()
   ON_WM_CTLCOLOR()
   ON_WM_DRAWITEM()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


#define OD_SELECTED (ODS_SELECTED | ODS_CHECKED)
void CEditCustTool::OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDraw)
   {
   RECT     ClientRect;
   CWnd    *pCtrl;
   HDC      hDC;
   BOOL     bFocus, bHilighted;
   STRING   szString;
   int      iOldMode;
   COLORREF lOldColor;
   HBRUSH   hBrush;
   LPTSTR   lpString;
   DWORD    dwStyle;
   HFONT	hOldFont;
   int		nItems;

   if ( lpDraw->itemID == -1 )
	   return;

   // Check to see if its focus draw only
   if ( lpDraw->itemAction == ODA_FOCUS )
	   {
	   DrawFocusRect( lpDraw->hDC, &lpDraw->rcItem );
	   return;
	   }

   hDC = lpDraw->hDC;
   ClientRect = lpDraw->rcItem;
   pCtrl = GetDlgItem(lpDraw->CtlID);

            
   dwStyle = GetWindowLong(pCtrl->GetSafeHwnd(), GWL_STYLE);
   pCtrl->SendMessage(LB_GETTEXT, (WPARAM)lpDraw->itemID, (LPARAM)szString);
   nItems = pCtrl->SendMessage(LB_GETCOUNT, (WPARAM)0, (LPARAM)0);
   bHilighted = ( lpDraw->itemState & OD_SELECTED );
   bFocus = ( lpDraw->itemState & ODS_FOCUS );

   // Draw the box interior
   hBrush = CreateSolidBrush( 
	   GetSysColor( bHilighted ? COLOR_HIGHLIGHT : COLOR_WINDOW ) );
   FillRect( hDC, &ClientRect, hBrush );
   DeleteObject( hBrush );

   lpString = SkipSpaces( szString );

   if (lstrlen(lpString))
	   {
	   // Draw the text
	   iOldMode = SetBkMode( hDC, TRANSPARENT );
		
		lOldColor = SetTextColor( hDC,
			GetSysColor( bHilighted ? COLOR_HIGHLIGHTTEXT  : COLOR_WINDOWTEXT  ) );

	   if (dwStyle & CBS_THINFONT)
		   hOldFont = (HFONT)SelectObject(hDC, GetStockObject(Window.iLabelFont));
	   else
		   hOldFont = NULL;
	   DrawText( hDC, lpString, -1, &ClientRect,
		   DT_NOPREFIX | DT_SINGLELINE | DT_VCENTER | DT_LEFT);
	   if (hOldFont)
		   SelectObject(hDC, hOldFont);
	   SetBkMode( hDC, iOldMode );
	   SetTextColor( hDC, lOldColor );
	   }

   if ( bFocus )
	   DrawFocusRect( lpDraw->hDC, &lpDraw->rcItem );
   }

void CEditCustTool::OnSpin()
   {
   BOOL  Bool;

	m_WidthVal = GetDlgItemSpin(GetSafeHwnd(), IDC_TOOLBOXWIDTH, &Bool, NO);
	if (m_WidthVal > 99)
		m_WidthVal = 99;
	if (m_WidthVal < 1)
		m_WidthVal = 1;
   }

CCustTool *CEditCustTool::CreateNewTool()
{
	int         catstart = CategoryMap[m_CatVal];
   	CObList    *List = m_pToolBox->GetList();
   	CCustTool  *lpct = NULL;
   	CUSTCMD     cmd;
   
   	if (m_CatVal != -1 && m_ListVal != -1)
    {
      	LPTSTR 	lp;
      	CString SName;
      
      	if ((m_CatVal+1) == CATEGORY_MAX)   // Macro Category
        {
        	m_Tools.GetText(m_ListVal, SName);
         	lpct = new CCustTool(SName, SName);
       	}
      	else
        {
         	int offset = ((m_CatVal+2) != CATEGORY_MAX); // not Tools category
         	cmd = CustCmdMap[m_ListVal + catstart];

         	AstralStr(cmd.LongName+offset, &lp);

			if( IsDBCSLeadByte(( BYTE )*lp ))
			{
				if( *(( LPWORD )lp ) == _T(' '))
				{
					if( IsDBCSLeadByte(( BYTE )*MyCharNextN( lp, 2 )))
						( LPWORD )SName = ( LPWORD )MyCharNextN( lp, 2 );
					else
						SName = MyCharNextN( lp, 2 );
				}
				else
					( LPWORD )SName = ( LPWORD )lp;
			}
			else
			{
				if( *lp == _T(' '))
				{
					if( IsDBCSLeadByte(( BYTE )*MyCharNextN( lp, 2 )))
						( LPWORD )SName = ( LPWORD )MyCharNextN( lp, 2 );
					else
						SName = MyCharNextN( lp, 2 );
				}
				else
					SName = lp;
			}
		 
         	//SName = lp[0] == _T(' ')? &lp[2] : lp;

         	if (cmd.Kind)                  		// tools 
            	lpct = new CCustTool(CUSTOOL_ICON, cmd.Cmd, SName);
         	else                                // menu command
            	lpct = new CCustTool(CUSTOOL_CMD, cmd.Cmd, SName);
    	}
    }
   	return(lpct);
}


void CEditCustTool::OnAdd()
   {
   CCustTool  *lpct = CreateNewTool();
   
   if (lpct)
      {
	   m_pToolBox->Add(lpct);
      FillToolBox();
	   }
   }

void CEditCustTool::OnInsert()
   {
   CCustTool  *lpNew = CreateNewTool();
   if (lpNew)
      {
      CWnd       *pListBox;
      POSITION    Pos = NULL;
      int         count;

      if (lpNew->GetType() == CUSTOOL_ICON)
         pListBox = GetDlgItem(IDC_TOOLBOXTOOLS);
      else
         pListBox  = GetDlgItem(IDC_TOOLBOX);
      count = pListBox->SendMessage(LB_GETSELCOUNT, (WPARAM)0, (LPARAM)0);

      if (count == 1)    // if one is selected
         {
         CCustTool  *lpAfter;
         CObList    *List = m_pToolBox->GetList();
         INT         sel;

         pListBox->SendMessage(LB_GETSELITEMS, (WPARAM)1, (LPARAM)&sel);
         Pos  = List->FindIndex(sel);
         lpAfter = (CCustTool *)List->GetAt(Pos);
         if (!((lpNew->GetType() == CUSTOOL_ICON && lpAfter->GetType() == CUSTOOL_ICON) ||
             (lpNew->GetType() != CUSTOOL_ICON && lpAfter->GetType() != CUSTOOL_ICON)))
            Pos = NULL;    // not same type add at end
         }
	   m_pToolBox->Add(lpNew, Pos);
      FillToolBox();
      }
   }

void CEditCustTool::OnDel()
   {
   CWnd    *pListBox;
   int      i, count,sel;
   LPINT    lpSelItems;

   if (m_LastSelectedIcon)
      pListBox = GetDlgItem(IDC_TOOLBOXTOOLS);
   else
      pListBox  = GetDlgItem(IDC_TOOLBOX);
   count = pListBox->SendMessage(LB_GETSELCOUNT, (WPARAM)0, (LPARAM)0);
   if (count)
      {
	   lpSelItems = (LPINT)Alloc(sizeof(int)*count);
	   if (!lpSelItems)
		   {
		   Message(IDS_EMEMALLOC);
		   return;
		   }
      pListBox->SendMessage(LB_GETSELITEMS, (WPARAM)count, (LPARAM)lpSelItems);
	   for (i = count - 1; i >= 0 ; i--)
		   {
		   sel = lpSelItems[i];
         m_pToolBox->Delete(sel, m_LastSelectedIcon);
		   }
	   FreeUp(lpSelItems);

      FillToolBox();
      m_ToolName.SetWindowText(_T(""));
      }
   }

void CEditCustTool::OnSelchangeCategory()
   {
   CWnd  *pText = GetDlgItem(IDC_ICONPREVIEWTEXT);
	CDataExchange dx(this, TRUE);

	DDX_CBIndex(&dx, IDC_CATEGORY, m_CatVal);
	m_Tools.FillIt(GetSafeHwnd(), m_CatVal);

   BOOL  IsATool = ((m_CatVal+2) == CATEGORY_MAX);
   if (!IsATool)
      {
      HWND     HIcon = GetDlgItem(m_LastIcon)->GetSafeHwnd();

      if (HIcon)
         {
         m_LastIcon = IDC_ICONPREVIEW;
         SetWindowLong(HIcon, GWL_ID, m_LastIcon);
         ::InvalidateRect(HIcon, NULL, TRUE);
         }
      pText->ShowWindow(SW_HIDE);
      }
   else
      pText->ShowWindow(SW_SHOW);
   OnSelchangeToollist();
   }

void CEditCustTool::OnMoveItemTools()
   {

   SortList(TRUE);
   }

void CEditCustTool::OnMoveItem()
   {
   SortList(FALSE);
   }

void CEditCustTool::SortList(BOOL Tools)
   {
   CObList    *List = m_pToolBox->GetList();
   CCustTool  *NewTool;
   CCustTool  *OldTool;
   int         nOld, nNew;
   POSITION    NewPos, OldPos;
   CWnd        *pListBox;

   if (Tools)
      pListBox = GetDlgItem(IDC_TOOLBOXTOOLS);
   else
      pListBox  = GetDlgItem(IDC_TOOLBOX);
	ListBox_GetMoveItem(pListBox->GetSafeHwnd(), &nOld, &nNew);
   if (Tools)
      {
      nOld += m_pToolBox->GetNumNonIconTools();
      nNew += m_pToolBox->GetNumNonIconTools();
      }
	if (nOld != nNew)
		{
      int   Count = List->GetCount();

      OldPos  = List->FindIndex(nOld);
      OldTool = (CCustTool *)List->GetAt(OldPos);
      if (nNew != Count)
         NewPos  = List->FindIndex(nNew);
      else
         NewPos  = List->GetTailPosition();
      NewTool = (CCustTool *)List->GetAt(NewPos);
      if ((NewTool->GetType() == CUSTOOL_ICON && OldTool->GetType() == CUSTOOL_ICON) ||
          (NewTool->GetType() != CUSTOOL_ICON && OldTool->GetType() != CUSTOOL_ICON))
         {
         List->RemoveAt(OldPos);
         if (nNew != Count)
            List->InsertBefore(NewPos, OldTool);
         else
            List->AddTail(OldTool);
         }
      else
         FillToolBox();
      }
   }

void CEditCustTool::OnSelchangeToolboxTools()
   {
   int      i;
   CWnd     *pListBox = GetDlgItem(IDC_TOOLBOXTOOLS);
   
   m_LastSelectedIcon = TRUE;
   UpdateEditName(pListBox);
   pListBox = GetDlgItem(IDC_TOOLBOX);
   i = pListBox->SendMessage(LB_GETSELCOUNT, (WPARAM)0, (LPARAM)0);
   if (i > 0)
	   ((CListBox *)pListBox)->SelItemRange(FALSE, 0, i - 1);
   }

void CEditCustTool::OnSelchangeToolbox()
   {
   int      i;
   CWnd    *pListBox = GetDlgItem(IDC_TOOLBOX);

   m_LastSelectedIcon = FALSE;
   UpdateEditName(pListBox);
   pListBox = GetDlgItem(IDC_TOOLBOXTOOLS);
   i = pListBox->SendMessage(LB_GETSELCOUNT, (WPARAM)0, (LPARAM)0);
   if (i > 0)
      ((CListBox *)pListBox)->SelItemRange(FALSE, 0, i - 1);
   }

void CEditCustTool::UpdateEditName(CWnd *pListBox)
   {
	CDataExchange  dx(this, TRUE);
   CString        Tool;
   LPTSTR          lp;
   int             i;

   lp = Tool.GetBuffer(256);
   i = pListBox->SendMessage(LB_GETSELCOUNT, (WPARAM)0, (LPARAM)0);
   if (i == 1)
      {
      i = pListBox->SendMessage(LB_GETCARETINDEX, (WPARAM)0, (LPARAM)0);
      pListBox->SendMessage(LB_GETTEXT, (WPARAM)i, (LPARAM)lp);
      }
   else
      lp[0] = 0;   
   m_ToolName.SetWindowText(lp);
   Tool.ReleaseBuffer();
   }

void CEditCustTool::OnSelchangeToollist()
   {
   BOOL  IsATool = ((m_CatVal+2) == CATEGORY_MAX);
	CDataExchange dx(this, TRUE);

	DDX_LBIndex(&dx, IDC_TOOLLIST, m_ListVal);
   if (IsATool)
      {
      int         catstart = CategoryMap[m_CatVal];
      CUSTCMD     cmd = CustCmdMap[m_ListVal + catstart];
      HWND        HIcon = GetDlgItem(m_LastIcon)->GetSafeHwnd();

      if (HIcon)
         {
         m_LastIcon = cmd.Cmd;
         SetWindowLong(HIcon, GWL_ID, m_LastIcon);
         ::InvalidateRect(HIcon, NULL, TRUE);
         }
      }
   }

void CEditCustTool::OnKillfocusToolname()
{
    CDataExchange   dx(this, TRUE);
    CCustTool      *pctb;
    CObList        *List = m_pToolBox->GetList();
    POSITION        Pos;
    int             i = 0;
    CWnd           *pListBox;

    if (m_LastSelectedIcon)
        pListBox = GetDlgItem(IDC_TOOLBOXTOOLS);
    else
        pListBox = GetDlgItem(IDC_TOOLBOX);

    DDX_Text(&dx, IDC_TOOLNAME, m_NameVal);

    i = pListBox->SendMessage(LB_GETSELCOUNT, (WPARAM)0, (LPARAM)0);
    if (!i || i > 1)
    {
        m_ToolName.SetWindowText(_T(""));
        return;
    }
    i = pListBox->SendMessage(LB_GETCARETINDEX, (WPARAM)0, (LPARAM)0);
   
    if (m_LastSelectedIcon)
        i += m_pToolBox->GetNumNonIconTools();

    Pos  = List->FindIndex(i);
    pctb = (CCustTool *)List->GetAt(Pos);

    FixExtName( &m_NameVal );   // limit length of extended name

    pctb->SetShortName(m_NameVal);
    FillToolBox();
}

void CEditCustTool::OnCancel()
{
    // TODO: Add extra cleanup here
	
	EndDialog(IDCANCEL);
}

extern BOOL Dialog_OnSetCursor(HWND hDlg, HWND hWndCursor, UINT codeHitTest,UINT msg);
extern HBRUSH Dialog_OnCtlColor(HWND hDlg, HDC hDC, HWND hControl, int wType);

BOOL CEditCustTool::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
   {
   return Dialog_OnSetCursor(GetSafeHwnd(), pWnd->GetSafeHwnd(), nHitTest, message);
   }

HBRUSH CEditCustTool::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
   {
   return Dialog_OnCtlColor(GetSafeHwnd(), pDC->GetSafeHdc(), pWnd->GetSafeHwnd(), nCtlColor);
   }

void CEditCustTool::OnColorPatch()
   {
	m_ColorPatch = !m_ColorPatch;
	CheckDlgButton(IDC_COLORPATCH, m_ColorPatch);
   }

void CEditCustTool::OnOK()
   {
	CDataExchange dx(this, TRUE);

   GetDlgItem(IDOK)->SetFocus();
	DDX_Text(&dx, IDC_TOOLBOXWIDTH, m_WidthVal);
   if (m_WidthVal <= 0 || m_WidthVal > 99)
      m_WidthVal = 2;
   
   m_pToolBox->SetPatchUp(m_ColorPatch);
   m_pToolBox->SetWidth(m_WidthVal);
   m_pToolBox->Save();
   m_Original->ReLoad();
	EndDialog(IDOK);
   }

BOOL CEditCustTool::OnInitDialog()
{
    CDialog::OnInitDialog();
    CString  Caption;
    CString  Name;
    LPTSTR    lp;

    Name = m_pToolBox->GetName();
    lp   = Caption.GetBuffer(256);
    GetWindowText(lp, 256);
    Caption.ReleaseBuffer();
    Caption += _T(" - ");
    Caption += Name;
    lp   = Caption.GetBuffer(256);
    SetWindowText(lp);
    Caption.ReleaseBuffer();
   
    GetDlgItem(m_LastIcon)->EnableWindow(FALSE);
    CenterPopup(GetSafeHwnd());
    m_Category.FillIt();
    m_Category.SetCurSel(m_CatVal);
    FillToolBox();
    m_ListVal = 0;
    OnSelchangeCategory();
    OnSelchangeToollist();

    CEdit *eptr = ( CEdit *)GetDlgItem( IDC_TOOLNAME );
    if( eptr )
        eptr->LimitText( MAX_EXTNAME_LEN-1 );

    CheckDlgButton(IDC_COLORPATCH, m_ColorPatch);

    InitDlgItemSpin(GetSafeHwnd(), IDC_TOOLBOXWIDTH, m_WidthVal, NO, 1, 99 );
    HWND hListBox = GetDlgItem(IDC_TOOLBOX)->GetSafeHwnd();
    ListBox_SetMoveCursor(hListBox,
					    LoadCursor(PictPubApp.Get_hInstAstral(), MAKEINTRESOURCE(ID_DRAGMOVE)));
    hListBox = GetDlgItem(IDC_TOOLBOXTOOLS)->GetSafeHwnd();
    ListBox_SetMoveCursor(hListBox,
					    LoadCursor(PictPubApp.Get_hInstAstral(), MAKEINTRESOURCE(ID_DRAGMOVE)));

    return TRUE;  // return TRUE  unless you set the focus to a control
}


/**************************************************************************/
/*                         Rename Dialog                                  */
/**************************************************************************/
BEGIN_MESSAGE_MAP(CRenameCustTool, CDialog)
	//{{AFX_MSG_MAP(CEditCustTool)
   ON_WM_SETCURSOR()
   ON_WM_CTLCOLOR()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

CRenameCustTool::CRenameCustTool(CString *Name)
	: CDialog(CRenameCustTool::IDD, NULL)
   {
   m_pNameVal = Name;
   }

CRenameCustTool::~CRenameCustTool()
   {
   }

BOOL CRenameCustTool::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
   {
   return Dialog_OnSetCursor(GetSafeHwnd(), pWnd->GetSafeHwnd(), nHitTest, message);
   }

HBRUSH CRenameCustTool::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
   {
   return Dialog_OnCtlColor(GetSafeHwnd(), pDC->GetSafeHdc(), pWnd->GetSafeHwnd(), nCtlColor);
   }

extern STRING szAppName;
void CRenameCustTool::OnOK()
   {
   CCustToolList     *CBList    = PictPubApp.GetCustomBoxes();
	CDataExchange     dx(this, TRUE);
   POSITION          Pos;

	DDX_Text(&dx, IDC_TOOLNAME, *m_pNameVal);
	DDV_MaxChars(&dx, *m_pNameVal, 256);
   Pos = CBList->FindToolBox(*m_pNameVal);
   if (!Pos && m_pNameVal->GetLength())
	   CDialog::OnOK();
   else
      {
      LPTSTR    lp;
      BOOL     ret;

      if (Pos)
         {
	      AstralStr(IDS_CUSTNAMEDUP, &lp);
         ret = ::MessageBox(PictPubApp.Get_hWndAstral(), lp, szAppName, 
               MB_ICONQUESTION | MB_OKCANCEL | MB_TASKMODAL) == IDOK;
         if (ret)
            {
	         CDialog::OnOK();
            CBList->Delete(Pos);
            }
         }
      else
         {
	      AstralStr(IDS_CUSTNAMEINVALID, &lp);
         ::MessageBox(PictPubApp.Get_hWndAstral(), lp, szAppName, MB_OK |
                      MB_TASKMODAL);
         }
      }
   }

void CRenameCustTool::OnCancel()
   {
	// TODO: Add extra cleanup here
	
	CDialog::OnCancel();
   }

BOOL CRenameCustTool::OnInitDialog()
{
    CString  Caption;

    CenterPopup(GetSafeHwnd());

    CEdit *eptr = ( CEdit *)GetDlgItem( IDC_TOOLNAME );
    if( eptr )
        eptr->LimitText( MAX_EXTNAME_LEN-1 );

    if (CreatingCustom)
    {
        LPTSTR lp = Caption.GetBuffer(256);

        AstralStr(IDS_RENAMECUST, &lp);
        SetWindowText(lp);
        Caption.ReleaseBuffer();
    }
	CDialog::OnInitDialog();
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CRenameCustTool::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CEditCustTool)
	DDX_Control(pDX, IDC_TOOLNAME, m_ToolName);
	DDX_Text(pDX, IDC_TOOLNAME, *m_pNameVal);
	DDV_MaxChars(pDX, *m_pNameVal, 256);
	//}}AFX_DATA_MAP
}

/***************************************************************************
/*                         Load Dialog                               */
/**************************************************************************/

LOCAL  BOOL SaveToolBox_OnInitDialog(HWND hDlg, HWND hWndFocus, LPARAM lParam);
LOCAL  void SaveToolBox_OnCommand(HWND hDlg, int id, HWND hControl, UINT codeNotify);
static ComboSelection = -1;

BOOL WINPROC EXPORT DlgSaveToolBoxProc( HWND hDlg, UINT msg, WPARAM wParam,
                                 LPARAM lParam)
   {
   switch (msg)
	   {
	   HANDLE_DLGRET(hDlg, WM_INITDIALOG, SaveToolBox_OnInitDialog);  
	   HANDLE_DLGMSG(hDlg, WM_COMMAND,    SaveToolBox_OnCommand);

	   default:
	   return Dialog_HandleDefaults(hDlg, msg, wParam, lParam, IDD_TOOLBOX_LOAD);
	   }
   }

LOCAL void InitCombo(HWND hDlg)
   {
   CCustToolList     *CBList    = PictPubApp.GetCustomBoxes();
   CComboBox         *pCombo  = (CComboBox *)CWnd::FromHandle(GetDlgItem(hDlg,IDC_EXTNAMES));
   CCustToolBox      *pctb;
   LPTSTR              lp;
   POSITION           Pos;
   CString            Name;
   HWND               hOk;
   BOOL               Found = FALSE;

   pCombo->ResetContent();
   for (Pos = CBList->GetHeadPosition(); Pos != NULL;)
      {
      pctb = (CCustToolBox *)CBList->GetNext(Pos);
      if (pctb->m_IsMain)
         continue;
      Found = TRUE;
      Name = pctb->GetName();
      lp   = Name.GetBuffer(256);
      pCombo->AddString(lp);
      Name.ReleaseBuffer();
      }
   hOk = GetDlgItem(hDlg,IDOK);
   if (Found)
      EnableWindow(hOk, TRUE);
   else
      EnableWindow(hOk, FALSE);
   
   pCombo->SetCurSel(ComboSelection);
   }

LOCAL BOOL SaveToolBox_OnInitDialog(HWND hDlg, HWND hWndFocus, LPARAM lParam)
   {
   CCustToolList     *CBList = PictPubApp.GetCustomBoxes();

   CenterPopup(hDlg);
   ComboSelection = CBList->GetHeadPosition()? 0 : -1;
   InitCombo(hDlg);

   return(TRUE);
   }

extern CCustToolBox *CurCustomToolBox;
LOCAL void SaveToolBox_OnCommand(HWND hDlg, int id, HWND hControl, UINT codeNotify)
   {
   CCustToolList     *CBList = PictPubApp.GetCustomBoxes();

   switch (id)
	   {
	   case IDC_EXTNAMES:
         {
         CComboBox *pCombo  = (CComboBox *)CWnd::FromHandle(GetDlgItem(hDlg,IDC_EXTNAMES));
         ComboSelection = pCombo->GetCurSel();
         }
	      break;
	   case IDC_EXTMANAGE:
         {
         HMENU hMenu, hPopup;
         RECT Rect;
         STRING szMenuChange;
         CComboBox *pCombo  = (CComboBox *)CWnd::FromHandle(GetDlgItem(hDlg,IDC_EXTNAMES));

         if ( !(hMenu = LoadMenu( PictPubApp.Get_hInstAstral(), MAKEINTRESOURCE(id))))
	         break;
         hPopup = GetSubMenu(hMenu, 0);
         if (ComboSelection != -1)
            {
		      DeleteMenu(hPopup, 2, MF_BYPOSITION);
            pCombo->GetLBText(ComboSelection, szMenuChange);
		      ModifyMenu( hPopup, IDC_FILENAME, MF_BYCOMMAND | MF_STRING,
			      IDC_FILENAME, szMenuChange );
            }
	      else
		      {
		      DeleteMenu(hPopup, 0, MF_BYPOSITION);
		      DeleteMenu(hPopup, 0, MF_BYPOSITION);
		      DeleteMenu(hPopup, 0, MF_BYPOSITION);
		      EnableMenuItem( hPopup, IDC_DELETEEXT, MF_BYCOMMAND|MF_GRAYED );
		      EnableMenuItem( hPopup, IDC_RENAMEEXT, MF_BYCOMMAND|MF_GRAYED );
		      }

         GetWindowRect( hControl, &Rect );

         TrackPopupMenu( hPopup, 0, Rect.right, Rect.top, 0, hDlg, NULL );
         DestroyMenu( hMenu );
         }
	      break;
	   case IDC_ADDEXT:
         {
		   FORWARD_WM_COMMAND(PictPubApp.Get_hWndAstral(),
							IDM_CUSTBOXCREATE, NULL, 0, SendMessage);
         InitCombo(hDlg);
         }
	      break;
	   case IDC_DELETEEXT:
         {
         CCustToolBox  *pctb;
         POSITION       Pos;

         Pos  = CBList->FindIndex(ComboSelection);
         pctb = (CCustToolBox *)CBList->GetAt(Pos);
         if (pctb != CurCustomToolBox)
            {
            CBList->Delete(ComboSelection);
            ComboSelection = CBList->GetHeadPosition()? 0 : -1;
            InitCombo(hDlg);
            }
         else
            {
            // MessageBox can not delete an Open ToolBox
            }
         }
	      break;
	   case IDC_RENAMEEXT:
         {
         CCustToolBox  *pctb;
         POSITION       Pos;

         Pos  = CBList->FindIndex(ComboSelection);
         pctb = (CCustToolBox *)CBList->GetAt(Pos);
         NewToolBoxName = pctb->GetName();
         CreatingCustom = FALSE;
         CRenameCustTool(&NewToolBoxName).DoModal();
         CBList->ReName(ComboSelection, NewToolBoxName);
         InitCombo(hDlg);
         }
	      break;
	   case IDOK:
         if (ComboSelection != -1)
            {
            CCustToolBox  *pctb;
            POSITION       Pos  = CBList->FindIndex(ComboSelection);

            pctb = (CCustToolBox *)CBList->GetNext(Pos);
            if (!pctb->GetSafeHwnd())
               pctb->Create();
            else
	            pctb->SetWindowPos(NULL, 0, 0, 0, 0, SWP_SHOWWINDOW | 
                           SWP_NOSIZE | SWP_NOMOVE | SWP_DRAWFRAME);
            }
         AstralDlgEnd(hDlg, TRUE);
	      break;
	   case IDCANCEL:
	      AstralDlgEnd(hDlg, FALSE);
	      break;
      default:
	      break;
      }
   }
