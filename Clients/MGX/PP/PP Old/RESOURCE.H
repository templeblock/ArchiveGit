#define IDOK 1
#define IDCANCEL 2

#define MF_OWNERDRAW       0x0100

/* Virtual Keys, Standard Set */
#define VK_LBUTTON	    0x01
#define VK_RBUTTON	    0x02
#define VK_CANCEL	    0x03
#define VK_MBUTTON	    0x04    /* NOT contiguous with L & RBUTTON */
#define VK_BACK 	    0x08
#define VK_TAB		    0x09
#define VK_CLEAR	    0x0C
#define VK_RETURN	    0x0D
#define VK_SHIFT	    0x10
#define VK_CONTROL	    0x11
#define VK_MENU 	    0x12
#define VK_PAUSE	    0x13
#define VK_CAPITAL	    0x14
#define VK_ESCAPE	    0x1B
#define VK_SPACE	    0x20
#define VK_PRIOR	    0x21
#define VK_NEXT 	    0x22
#define VK_END		    0x23
#define VK_HOME 	    0x24
#define VK_LEFT 	    0x25
#define VK_UP		    0x26
#define VK_RIGHT	    0x27
#define VK_DOWN 	    0x28
#define VK_SELECT	    0x29
#define VK_PRINT	    0x2A
#define VK_EXECUTE	    0x2B
#define VK_SNAPSHOT	    0x2C
/* #define VK_COPY 	    0x2C not used by keyboards. */
#define VK_INSERT	    0x2D
#define VK_DELETE	    0x2E
#define VK_HELP 	    0x2F

/* VK_A thru VK_Z are the same as their ASCII equivalents: 'A' thru 'Z' */
/* VK_0 thru VK_9 are the same as their ASCII equivalents: '0' thru '0' */

#define VK_NUMPAD0	    0x60
#define VK_NUMPAD1	    0x61
#define VK_NUMPAD2	    0x62
#define VK_NUMPAD3	    0x63
#define VK_NUMPAD4	    0x64
#define VK_NUMPAD5	    0x65
#define VK_NUMPAD6	    0x66
#define VK_NUMPAD7	    0x67
#define VK_NUMPAD8	    0x68
#define VK_NUMPAD9	    0x69
#define VK_MULTIPLY	    0x6A
#define VK_ADD		    0x6B
#define VK_SEPARATOR	    0x6C
#define VK_SUBTRACT	    0x6D
#define VK_DECIMAL	    0x6E
#define VK_DIVIDE	    0x6F
#define VK_F1		    0x70
#define VK_F2		    0x71
#define VK_F3		    0x72
#define VK_F4		    0x73
#define VK_F5		    0x74
#define VK_F6		    0x75
#define VK_F7		    0x76
#define VK_F8		    0x77
#define VK_F9		    0x78
#define VK_F10		    0x79
#define VK_F11		    0x7A
#define VK_F12		    0x7B
#define VK_F13		    0x7C
#define VK_F14		    0x7D
#define VK_F15		    0x7E
#define VK_F16		    0x7F

#define VK_NUMLOCK	    0x90

/* Window Styles */
#define WS_OVERLAPPED	    0x00000000L
#define WS_POPUP	    0x80000000L
#define WS_CHILD	    0x40000000L
#define WS_MINIMIZE	    0x20000000L
#define WS_VISIBLE	    0x10000000L
#define WS_DISABLED	    0x08000000L
#define WS_CLIPSIBLINGS     0x04000000L
#define WS_CLIPCHILDREN     0x02000000L
#define WS_MAXIMIZE	    0x01000000L
#define WS_CAPTION	    0x00C00000L     /* WS_BORDER | WS_DLGFRAME	*/
#define WS_BORDER	    0x00800000L
#define WS_DLGFRAME	    0x00400000L
#define WS_VSCROLL	    0x00200000L
#define WS_HSCROLL	    0x00100000L
#define WS_SYSMENU	    0x00080000L
#define WS_THICKFRAME	    0x00040000L
#define WS_GROUP	    0x00020000L
#define WS_TABSTOP	    0x00010000L

#define WS_MINIMIZEBOX	    0x00020000L
#define WS_MAXIMIZEBOX	    0x00010000L

#define WS_TILED	    WS_OVERLAPPED
#define WS_ICONIC	    WS_MINIMIZE
#define WS_SIZEBOX	    WS_THICKFRAME

/* Common Window Styles */
#define WS_OVERLAPPEDWINDOW (WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX)
#define WS_POPUPWINDOW	    (WS_POPUP | WS_BORDER | WS_SYSMENU)
#define WS_CHILDWINDOW	    (WS_CHILD)

#define WS_TILEDWINDOW	    (WS_OVERLAPPEDWINDOW)

/* Extended Window Styles */
#define WS_EX_DLGMODALFRAME 0x00000001L
#define WS_EX_NOPARENTNOTIFY 0x00000004L

/* Dialog Styles */
#define DS_ABSALIGN	    0x01L
#define DS_SYSMODAL	    0x02L
#define DS_LOCALEDIT	    0x20L   /* Edit items get Local storage. */
#define DS_SETFONT	    0x40L   /* User specified font for Dlg controls */
#define DS_MODALFRAME	    0x80L   /* Can be combined with WS_CAPTION  */
#define DS_NOIDLEMSG	    0x100L  /* WM_ENTERIDLE message will not be sent */

/* Edit Control Styles */
#define ES_LEFT             0x0000L
#define ES_CENTER           0x0001L
#define ES_RIGHT            0x0002L
#define ES_MULTILINE        0x0004L
#define ES_UPPERCASE        0x0008L
#define ES_LOWERCASE        0x0010L
#define ES_PASSWORD         0x0020L
#define ES_AUTOVSCROLL      0x0040L
#define ES_AUTOHSCROLL      0x0080L
#define ES_NOHIDESEL        0x0100L
#define ES_OEMCONVERT       0x0400L

/* Button Control Styles */
#define BS_PUSHBUTTON	   0x00L
#define BS_DEFPUSHBUTTON   0x01L
#define BS_CHECKBOX	   0x02L
#define BS_AUTOCHECKBOX    0x03L
#define BS_RADIOBUTTON	   0x04L
#define BS_3STATE	   0x05L
#define BS_AUTO3STATE	   0x06L
#define BS_GROUPBOX	   0x07L
#define BS_USERBUTTON	   0x08L
#define BS_AUTORADIOBUTTON 0x09L
#define BS_PUSHBOX	   0x0AL
#define BS_OWNERDRAW	   0x0BL
#define BS_LEFTTEXT	   0x20L

/* Static Control Constants */
#define SS_LEFT 	   0x00L
#define SS_CENTER	   0x01L
#define SS_RIGHT	   0x02L
#define SS_ICON 	   0x03L
#define SS_BLACKRECT	   0x04L
#define SS_GRAYRECT	   0x05L
#define SS_WHITERECT	   0x06L
#define SS_BLACKFRAME	   0x07L
#define SS_GRAYFRAME	   0x08L
#define SS_WHITEFRAME	   0x09L
#define SS_USERITEM	   0x0AL
#define SS_SIMPLE	   0x0BL
#define SS_LEFTNOWORDWRAP  0x0CL
#define SS_NOPREFIX	   0x80L    /* Don't do "&" character translation */

/* Listbox Styles */
#define LBS_NOTIFY	      0x0001L
#define LBS_SORT	      0x0002L
#define LBS_NOREDRAW	      0x0004L
#define LBS_MULTIPLESEL       0x0008L
#define LBS_OWNERDRAWFIXED    0x0010L
#define LBS_OWNERDRAWVARIABLE 0x0020L
#define LBS_HASSTRINGS        0x0040L
#define LBS_USETABSTOPS       0x0080L
#define LBS_NOINTEGRALHEIGHT  0x0100L
#define LBS_MULTICOLUMN       0x0200L
#define LBS_WANTKEYBOARDINPUT 0x0400L
#define LBS_EXTENDEDSEL	      0x0800L
#define LBS_STANDARD	      (LBS_NOTIFY | LBS_SORT | WS_VSCROLL | WS_BORDER)

/* Combo Box styles */
#define CBS_SIMPLE	      0x0001L
#define CBS_DROPDOWN	      0x0002L
#define CBS_DROPDOWNLIST      0x0003L
#define CBS_OWNERDRAWFIXED    0x0010L
#define CBS_OWNERDRAWVARIABLE 0x0020L
#define CBS_AUTOHSCROLL       0x0040L
#define CBS_OEMCONVERT        0x0080L
#define CBS_SORT              0x0100L
#define CBS_HASSTRINGS        0x0200L
#define CBS_NOINTEGRALHEIGHT  0x0400L

/* Scroll Bar Styles */
#define SBS_HORZ		    0x0000L
#define SBS_VERT		    0x0001L
#define SBS_TOPALIGN		    0x0002L
#define SBS_LEFTALIGN		    0x0002L
#define SBS_BOTTOMALIGN		    0x0004L
#define SBS_RIGHTALIGN		    0x0004L
#define SBS_SIZEBOXTOPLEFTALIGN	    0x0002L
#define SBS_SIZEBOXBOTTOMRIGHTALIGN 0x0004L
#define SBS_SIZEBOX		    0x0008L

#define TDY 10
#define EDY 11
#define BDY 14
#define CDY 10
#define RDY 10
#define SDY 10
#define CBY 9
// 4 * nGigits + 5
#define DX1 9
#define DX2 13
#define DX3 17
#define DX4 21
#define DX5 25
#define DX6 29
#define DX7 33
#define DX8 37
#define SP 3

#define WS_MODELESS (WS_VISIBLE | WS_BORDER | WS_CHILD | WS_CLIPSIBLINGS | DS_NOIDLEMSG)
#define WS_MODELESSFLOAT (WS_MODELESS | WS_CAPTION | WS_SYSMENU)
#define WS_MODELESSFLOAT_MIN (WS_MODELESSFLOAT | WS_MINIMIZEBOX)
#define WS_MODAL (WS_VISIBLE | WS_DLGFRAME | WS_POPUP | DS_NOIDLEMSG)
#define WS_MODALFLOAT (WS_MODAL | WS_CAPTION | DS_MODALFRAME)
#define WS_MENU (WS_VISIBLE | WS_BORDER | WS_POPUP | DS_NOIDLEMSG)
#define WS_NOSHADOWS	2L
#define WS_BITMAP	4L
#define WS_TOOL		8L
#define WS_TEXT		16L
#define TS_WINDOWTEXT	1L
#define TS_CENTER	2L
#define TS_PRESETSTYLE	4L
#define TS_ANSIVARFONT	8L
#define SS_DOWNBOX	16L
#define SS_SDOWNBOX	32L
#define SS_LIGHT	64L

#define TEXTBOX(name, id, x, y, dx, dy) \
	CONTROL name,id,"stext",SS_LEFT|SS_DOWNBOX|WS_GROUP|WS_DISABLED,x,y,dx,dy
#define CTEXTBOX(name, id, x, y, dx, dy) \
	CONTROL name,id,"stext",SS_CENTER|SS_DOWNBOX|WS_GROUP|WS_DISABLED,x,y,dx,dy
#define RTEXTBOX(name, id, x, y, dx, dy) \
	CONTROL name,id,"stext",SS_RIGHT|SS_DOWNBOX|WS_GROUP|WS_DISABLED,x,y,dx,dy
#define TEXTBOXL(name, id, x, y, dx, dy) \
	CONTROL name,id,"stext",SS_LEFT|SS_SDOWNBOX|SS_LIGHT|WS_GROUP|WS_DISABLED,x,y,dx,dy
#define CTEXTBOXL(name, id, x, y, dx, dy) \
	CONTROL name,id,"stext",SS_CENTER|SS_SDOWNBOX|SS_LIGHT|WS_GROUP|WS_DISABLED,x,y,dx,dy
#define RTEXTBOXL(name, id, x, y, dx, dy) \
	CONTROL name,id,"stext",SS_RIGHT|SS_SDOWNBOX|SS_LIGHT|WS_GROUP|WS_DISABLED,x,y,dx,dy
#define VBAR(id, x, y, dx, dy) \
	CONTROL "",id,"stext",SS_SIMPLE|WS_GROUP|WS_DISABLED,x,y,dx,dy
#define TEXTL(name, id, x, y, dx, dy) \
	CONTROL name,id,"stext",SS_LEFT|SS_LIGHT|WS_GROUP|WS_DISABLED,x,y,dx,dy
#define RTEXTL(name, id, x, y, dx, dy) \
	CONTROL name,id,"stext",SS_RIGHT|SS_LIGHT|WS_GROUP|WS_DISABLED,x,y,dx,dy
#define CTEXTL(name, id, x, y, dx, dy) \
	CONTROL name,id,"stext",SS_CENTER|SS_LIGHT|WS_GROUP|WS_DISABLED,x,y,dx,dy
#define SBOX(id, x, y, dx, dy) \
	CONTROL "",id,"stext",SS_USERITEM|SS_DOWNBOX|WS_GROUP|WS_DISABLED,x,y,dx,dy
#define SBOXS(id, x, y, dx, dy) \
	CONTROL "",id,"stext",SS_USERITEM|SS_SDOWNBOX|WS_GROUP|WS_DISABLED,x,y,dx,dy
#define TEXT(name, id, x, y, dx, dy) \
	LTEXT name,id,x,y,dx,dy,WS_GROUP
#define CTEXT(name, id, x, y, dx, dy) \
	CONTROL name,id,"static",SS_CENTER|WS_GROUP,x,y,dx,dy
;	CTEXT name,id,x,y,dx,dy,WS_GROUP
#define RTEXT(name, id, x, y, dx, dy) \
	CONTROL name,id,"static",SS_RIGHT|WS_GROUP,x,y,dx,dy
;	RTEXT name,id,x,y,dx,dy,WS_GROUP
#define BOX(id, x, y, dx, dy) \
	CONTROL "",id,"static",SS_BLACKFRAME|WS_GROUP,x,y,dx,dy
#define GROUPBOX(name, id, x, y, dx, dy) \
	CONTROL name,id,"button",BS_GROUPBOX|WS_GROUP,x,y,dx,dy
#define BUTTON(name, id, x, y, dx, dy) \
	PUSHBUTTON name,id,x,y,dx,dy,WS_TABSTOP|WS_GROUP
#define DUTTON(name, id, x, y, dx, dy) \
	DEFPUSHBUTTON name,id,x,y,dx,dy,WS_TABSTOP|WS_GROUP
#define CHECK(name, id, x, y, dx, dy) \
	CONTROL name,id,"MGXBTN",BS_CHECKBOX|WS_TABSTOP|WS_GROUP,x,y,dx,dy
//	CHECKBOX name,id,x,y,dx,dy,WS_TABSTOP|WS_GROUP
#define RADIO(name, id, x, y, dx, dy) \
	CONTROL name,id,"MGXBTN",BS_RADIOBUTTON|WS_TABSTOP|WS_GROUP,x,y,dx,dy
//	RADIOBUTTON name,id,x,y,dx,dy,WS_TABSTOP|WS_GROUP
#define EDIT(id, x, y, dx, dy) \
	EDITTEXT id,x,y,dx,dy,ES_LEFT|WS_BORDER|WS_TABSTOP|WS_GROUP
#define EDITX(id, x, y, dx, dy, name, tdx) \
		EDIT(id, x, y, dx, dy) \
		TEXT(name, -1, x+dx+SP, y+2, tdx, TDY)
#define SPINEDIT(id, x, y, dx, dy) \
		EDIT(id, x, y, dx, dy) \
		SPIN(id, x+dx, y, 6, dy )
#define SPINEDITX(id, x, y, dx, dy, name, tdx) \
		EDIT(id, x, y, dx, dy) \
		SPIN(id, x+dx, y, 6, dy ) \
		TEXT(name, -1, x+dx+6+SP, y+2, tdx, TDY)
#define NAME(id, x, y, dx, dy) \
	EDITTEXT id,x,y,dx,dy,ES_LEFT|WS_BORDER|WS_TABSTOP|WS_GROUP \
	|ES_AUTOHSCROLL|ES_OEMCONVERT
#define SPNAME(id, x, y, dx, dy, name, tdx) \
		NAME(id, x, y, dx, dy) \
		TEXT(name, -1, x+dx+SP, y+2, tdx, TDY)
#define LIST(id, x, y, dx, dy) \
	LISTBOX id,x,y,dx,dy,LBS_STANDARD|WS_TABSTOP|WS_GROUP
#define LISTNOSORT(id, x, y, dx, dy) \
	LISTBOX id,x,y,dx,dy,LBS_NOTIFY|WS_VSCROLL|WS_BORDER|WS_TABSTOP|WS_GROUP
#define COMBO(id, x, y, dx, dy) \
	CONTROL "",id,"combobox",CBS_DROPDOWNLIST|CBS_AUTOHSCROLL|WS_VSCROLL \
	|WS_GROUP|WS_TABSTOP,x,y,dx,1+dy
#define ICONCOMBO(id, x, y, dx, dy) \
	CONTROL "",id,"combobox",CBS_DROPDOWNLIST|CBS_AUTOHSCROLL|WS_VSCROLL \
	|WS_GROUP|WS_TABSTOP|CBS_OWNERDRAWVARIABLE|CBS_HASSTRINGS,x,y,dx,1+dy
#define COMBOSORT(id, x, y, dx, dy) \
	CONTROL "",id,"combobox",CBS_DROPDOWNLIST|CBS_AUTOHSCROLL|CBS_SORT \
	|WS_VSCROLL|WS_GROUP|WS_TABSTOP,x,y,dx,1+dy
#define COMBOEDIT(id, x, y, dx, dy) \
	CONTROL "",id,"combobox",CBS_DROPDOWN|CBS_AUTOHSCROLL|CBS_SORT \
	|WS_VSCROLL|WS_GROUP|WS_TABSTOP,x,y,dx,1+dy
#define HSCROLL(id, x, y, dx, dy) \
	SCROLLBAR id,x,y,dx,dy,SBS_HORZ|WS_GROUP
#define VSCROLL(id, x, y, dx, dy) \
	SCROLLBAR id,x,y,dx,dy,SBS_VERT|WS_GROUP
#define PATH(name, id, x, y, dx, dy) \
	CONTROL name,id,"path",WS_GROUP,x,y,dx,dy
#define TEXTBLOCK(id, x, y, dx, dy) \
	CONTROL "",id,"block",TS_PRESETSTYLE,x,y,dx,dy
#define PICON(id, x, y, dx, dy) \
	CONTROL "",id,"icon",WS_DISABLED|WS_NOSHADOWS,x,y,dx,dy
#define PBITMAP(id, x, y, dx, dy) \
	CONTROL "",id,"icon",WS_DISABLED|WS_NOSHADOWS|WS_BITMAP,x,y,dx,dy
#define PDUAL(name, id, x, y, dx, dy) \
	CONTROL name,id,"icon",WS_DISABLED|WS_TEXT,x,y,dx,dy
#define ICON(id, x, y, dx, dy) \
	CONTROL "",id,"icon",WS_NOSHADOWS,x,y,dx,dy
#define ICON3D(id, x, y, dx, dy) \
	CONTROL "",id,"icon",WS_TABSTOP,x,y,dx,dy
#define ICON3DNF(id, x, y, dx, dy) \
	CONTROL "",id,"icon", 0 ,x,y,dx,dy
#define DUAL(name, id, x, y, dx, dy) \
	CONTROL name,id,"icon",WS_TEXT,x,y,dx,dy
#define TOOL(id, x, y, dx, dy) \
	CONTROL "",id,"icon",WS_TOOL,x,y,dx,dy
#define STICK(id, x, y, dx, dy) \
	CONTROL "",id,"joystick",WS_TABSTOP,x,y,dx,dy
#define LEVER(id, x, y, dx, dy) \
	CONTROL "",id,"lever",WS_TABSTOP,x,y,dx,dy
#define PMAP(id, x, y, dx, dy) \
	CONTROL "",id,"map", 0 ,x,y,dx,dy
#define MAP(id, x, y, dx, dy) \
	CONTROL "",id,"map",WS_TABSTOP,x,y,dx,dy
#define HUEBAR(id, x, y, dx, dy) \
	CONTROL "",id,"hue",WS_TABSTOP,x,y,dx,dy
#define PICKER(id, x, y, dx, dy) \
	CONTROL "",id,"picker",WS_TABSTOP,x,y,dx,dy
#define COLORPATCH(name, id, x, y, dx, dy) \
	CONTROL name,id,"color",WS_DISABLED,x,y,dx,dy
#define COLOR(id, x, y, dx, dy) \
	CONTROL "",id,"color",WS_TABSTOP,x,y,dx,dy
#define COLOR2(id, x, y, dx, dy) \
	CONTROL "",id,"color2", 0 ,x,y,dx,dy
#define SPIN(id, x, y, dx, dy) \
	CONTROL "",id,"scroll", 0 ,x,y,dx,dy
#define CBITEM(name, id) \
	CONTROL name,id,"button",WS_GROUP,999,999,1,1
#define HISTO(name, id, x, y, dx, dy) \
	CONTROL name,id,"histo",WS_DISABLED,x,y,dx,dy
#define QUARTERS(id, x, y, dx, dy) \
	CONTROL "",id,"quarters",WS_CHILD,x,y,dx,dy
