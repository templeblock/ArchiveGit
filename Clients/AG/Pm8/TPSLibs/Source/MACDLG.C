/*-----------------------------------------------------------------
 * $Workfile:: MACDLG.C                                           $
 * $Revision:: 1                                                  $
 *     $Date:: 11/06/96 7:22a                                     $
 *   $Author:: Mike_houle                                         $
 *
 *-----------------------------------------------------------------
 * Desc: Macintosh Assertion Dialog
 *-----------------------------------------------------------------
 *
 * $Log: /Development/TPSLibs/Source/MACDLG.C $
 * 
 * 1     11/06/96 7:22a Mike_houle
 *
 *-----------------------------------------------------------------
 */

#if defined( TPSDEBUG )
#if defined( MAC )

#include "TPSTYPES.H"
#include "DbgDlg.h"
#include "StackCra.h"

//
//	DEFINES
#define kDlgRsrcID				128

#define ENTERkey					0x03
#define CRkey						0x0D
#define UPARROW					0x1E
#define DOWNARROW					0x1F
#define RIGHTARROW				0x1D
#define LEFTARROW					0x1C
#define HOME						0x01
#define ENDKEY						0x04
#define PAGE_UP					0x0B
#define PAGE_DOWN					0x0C

#define FRONT_WINDOW				((WindowPtr)(-1L))

#define kScrollOffset			16
#define kUp							1
#define kDown						2
#define kPage						3	
#define kTop						0
#define kBottom					0

#define kLineHorzCoord			336
#define kLineVertCoor			0
#define kLineTitleHorzCoord	284
#define kLineTitleVertCoor		0
#define kLineTitleStr			"\pLine #:"

#define kReasonHorzCoord		56
#define kReasonVertCoor			20
#define kReasonTitleHorzCoord	0
#define kReasonTitleVertCoor	20
#define kReasonTitleStr			"\pReason:"

#define kFileHorzCoord			68
#define kFileVertCoor			0
#define kFileTitleHorzCoord	0
#define kFileTitleVertCoor		0
#define kFileTitleStr			"\pFileName:"

#define kDataHorzCoord			60
#define kDataVertCoor			56
#define kDataTitleHorzCoord	0
#define kDataTitleVertCoor		56
#define kDataTitleStr			"\pMemory:"

#define kSCTitleHorzCoord		0
#define kSCTitleVertCoor		92
#define kSCTitleStr				"\pStack Crawl:"

#define kDisplayStr 				(sBYTE *)"\pThe value at 0x00000000 is : "
#define kDisplayStrLength		30

//
// Local Types...
typedef struct tagDialogData
{
	LPVOID			lpvData;
	LPSZ			lpszFilename;
	LPSZ			lpszReason;
	uLONG			ulFormat;
	uLONG			ulLineNo;
	uLONG			ulDataCount;
} DialogDataStruct, *DialogDataPtr, **DialogDataHdl;

//
// External References...
extern uLONG	g_tdb_ulFlags;
extern HSYMLIST	g_tdb_hSymList;

//
// Local Globals...
static	DialogPtr		g_pdlgAssertDlg		= nil;
static	short			g_sCurrentSymbol	= 0;
static	ControlHandle	g_hScrollControl;
static	ControlHandle	g_hDisableCntrl;
//
// Local prototypes...
static BOOLEAN				SetupDialog( DialogPtr pdlgAssert, LPSZ lpszReason, LPSZ lpszFilename,
												uLONG ulLineNo, uLONG ulCount, LPVOID lpvData, uLONG ulFormat );
static int					DoDialogItem(DialogPtr dlog, short itemHit);
static short				DlgCheckButtonsForLetter(DialogPtr dialog, unsigned char ch);
static StringPtr			CToPascal(sBYTE *str, StringPtr pstr);
static void					DisplayBytes(sBYTE FAR *pData, long count, sBYTE *msg);
static void 				DisplayWords(sBYTE FAR *pData, long count, sBYTE *msg);
static void 				DisplayLongs(sBYTE FAR *pData, long count, sBYTE *msg);
static void 				DisplayCStr(sBYTE FAR *pData, sBYTE *msg);
static void 				DisplayPStr(sBYTE FAR *pData, sBYTE *msg);
static void					NumToHexStr(long number, sBYTE *str, sWORD cnt);
static void 				NumToDecStr(long number, sBYTE *str);
static void 				_DoScroll(ControlHandle cntrl, DialogPtr dlog, short direction, short amount);
static void					DoDrawDialog(DialogPtr dlog);
static void 				_MakeDialogHNoPurge(DialogPtr dlog);
static void					DisplayTextinRect(Rect *rect, sBYTE *str);
static void					_MakeControlHNoPurge(ControlHandle hCtl);
static pascal void		_ScrollProc(ControlHandle cntrl, short part);
static void					DefaultButtonProc(WindowPtr dlogm);
static void					StackCrawlProc(WindowPtr dlog);
static pascal Boolean	DlgMessageFilter( DialogPtr dlog, EventRecord *evt, short *itemHit );


#define	ConvertNumberToString(lVal, pStr, cnt)	\
			if (g_tdb_ulFlags & kDbgHexDump)	\
				NumToHexStr(lVal, pStr, cnt);	\
			else								\
				NumToDecStr(lVal, pStr);		\

/*--------------------------------------------------------------------------
 * Function: _tdb_InitAssertDlg
 * Expects:  DEBUGPARMS FAR * lpParms
 *           uLONG            ulFlags
 * Returns:  BOOLEAN          bSuccess
 * Purpose:  
 *--------------------------------------------------------------------------
 */
TDBAPI( BOOLEAN ) _tdb_InitAssertDlg( DEBUGPARMS FAR * /* lpParms */, uLONG /* ulFlags */ )
{
	ControlHandle	hCtl;
	DialogDataHdl	DlogData;
	Rect			ctrlRect;

	DlogData = (DialogDataHdl)NewHandle(sizeof(DialogDataStruct));
	if( !DlogData ) return FALSE;

	ctrlRect.left		= 132;
	ctrlRect.top		= 119;
	ctrlRect.right		= 528;
	ctrlRect.bottom		= 311;
	g_pdlgAssertDlg		= NewDialog( nil, &ctrlRect, "\p", false, dBoxProc, (WindowPtr)-1, false, (long)DlogData, nil);
	if( !g_pdlgAssertDlg ) goto err;
	
	//	set up Ok Button
	ctrlRect.left		= 312;
	ctrlRect.top		= 168;
	ctrlRect.right		= 380;
	ctrlRect.bottom		= 188;
	hCtl				= NewControl(g_pdlgAssertDlg, &ctrlRect, "\pOK", true, 0, 0, 1, pushButProc, ID_OK );
	if( !hCtl ) goto err;

	//	set up Exit to Shell Button
	ctrlRect.left		= 144;
	ctrlRect.top		= 168;
	ctrlRect.right		= 252;
	ctrlRect.bottom		= 188;
	hCtl				= NewControl(g_pdlgAssertDlg, &ctrlRect, "\pExit to Shell", true, 0, 0, 1, pushButProc, ID_TERMINATE );
	if( !hCtl ) goto err;

	//	set up Debugger Button
	ctrlRect.left		= 12;
	ctrlRect.top		= 168;
	ctrlRect.right		= 92;
	ctrlRect.bottom		= 188;
	hCtl				= NewControl(g_pdlgAssertDlg, &ctrlRect, "\pDebugger", true, 0, 0, 1, pushButProc, ID_DEBUGGER );
	if( !hCtl ) goto err;

	//	set up Disable Asserts Button
	ctrlRect.left		= 264;
	ctrlRect.top		= 92;
	ctrlRect.right		= 388;
	ctrlRect.bottom		= 108;
	g_hDisableCntrl		= NewControl(g_pdlgAssertDlg, &ctrlRect, "\pDisable Asserts", true, 0, 0, 1, checkBoxProc, ID_DISABLEASSERTS );
	if( !g_hDisableCntrl ) goto err;

	//	set up Display As Hex Button
	ctrlRect.left		= 264;
	ctrlRect.top		= 114;
	ctrlRect.right		= 388;
	ctrlRect.bottom		= 130;
	hCtl				= NewControl(g_pdlgAssertDlg, &ctrlRect, "\pDisplay As Hex", true, 0, 0, 1, checkBoxProc, ID_DISPLAYASHEX );
	if( !g_hDisableCntrl ) goto err;

	//	set up scroll bar
	ctrlRect.left		= 240;
	ctrlRect.top		= 92;
	ctrlRect.right		= 256;
	ctrlRect.bottom		= 160;
	g_hScrollControl	= NewControl(g_pdlgAssertDlg, &ctrlRect, "\p", true, 0, 0, 1, scrollBarProc, 0L);
	if( !g_hScrollControl ) goto err;
	
	// done!
	return true;

err:
	if( DlogData )
		DisposeHandle( (Handle)DlogData );
	KillControls( g_pdlgAssertDlg );
	DisposeDialog( g_pdlgAssertDlg );
	g_pdlgAssertDlg	= nil;
	return FALSE;
}


/*--------------------------------------------------------------------------
 * Function: _tdb_DeinitAssertDlg
 * Expects:  nothing
 * Returns:  nothing
 * Purpose:  
 *--------------------------------------------------------------------------
 */
TDBAPI( VOID ) _tdb_DeinitAssertDlg( VOID )
{
	if( g_pdlgAssertDlg )
		{
		Handle	DlogData = (Handle)GetWRefCon(g_pdlgAssertDlg);

		if( DlogData != nil )
			DisposeHandle(DlogData);
		KillControls(g_pdlgAssertDlg);
		DisposeDialog(g_pdlgAssertDlg);
		g_pdlgAssertDlg = nil;
		}
}


/*--------------------------------------------------------------------------
 * Function: _tdb_DoAssertDlg
 * Expects:  char_FAR_*  lpszTest
 *           char_FAR_*  lpszReason
 *           char_FAR_*  lpszFilename
 *           uLONG       ulLineNo
 *           uLONG       ulCount
 *           void_FAR_*  lpvData
 *           uLONG       ulFormat
 * Returns:  int         iUserSelection
 * Purpose:  Setup and run the Macintosh Assertion Dialog
 * History:  10/28/94  MGD - Initial Revision
 *--------------------------------------------------------------------------
 */
TDBAPI( int ) _tdb_DoAssertDlg( LPSZ /* lpszTest */, LPSZ lpszReason, LPSZ lpszFilename, uLONG ulLineNo, uLONG ulCount, LPVOID lpvData, uLONG ulFormat )
{
	SysEnvRec	serTmp;
	GrafPtr		gpSavePort;
	BOOLEAN		bKeepGoing;
	GDHandle		gdhSaveDevice;
	GDHandle		gdhMainDevice;
	short			iUserSelection;

	/* Setup...
	 */
	ShowCursor();
	SysEnvirons( 1, &serTmp );
	if( serTmp.hasColorQD )
		{
		gdhSaveDevice	= GetGDevice();
		gdhMainDevice	= GetMainDevice();
		if( gdhSaveDevice != gdhMainDevice )
			SetGDevice( gdhMainDevice );
		}

	GetPort( &gpSavePort );
	SetPort( g_pdlgAssertDlg );
	SetupDialog( g_pdlgAssertDlg, lpszReason, lpszFilename, ulLineNo, ulCount, lpvData, ulFormat );
	SelectWindow( g_pdlgAssertDlg );
	ShowWindow( g_pdlgAssertDlg );

	/* Do the Dialog
	 */	
	do
		{
		ModalDialog( NewModalFilterProc(DlgMessageFilter), &iUserSelection );
		bKeepGoing = (BOOLEAN)DoDialogItem( g_pdlgAssertDlg, iUserSelection );
		}
	while( bKeepGoing );

	/* Cleanup...
	 */
	HideWindow( g_pdlgAssertDlg );
	SetPort( gpSavePort );
	if( serTmp.hasColorQD )
		{
		if( gdhSaveDevice != gdhMainDevice )
			SetGDevice( gdhSaveDevice );
		}

	/* return the last item hit...
	 */
	return iUserSelection;
}



//
// Local support...

/*--------------------------------------------------------------------------
 * Function: SetupDialog
 * Expects:  DialogPtr   pdlgAssert
 *           char_FAR_*  lpszReason
 *           char_FAR_*  lpszFilename
 *           uLONG       ulLineNo
 *           uLONG       ulCount
 *           void_FAR_*  lpvData
 *           uLONG       ulFormat
 * Returns:  BOOLEAN     bSuccess
 * Purpose:  
 * History:  10/28/94  MGD - Initial Revision
 *--------------------------------------------------------------------------
 */
static BOOLEAN SetupDialog( DialogPtr pdlgAssert, LPSZ lpszReason, LPSZ lpszFilename,
									uLONG ulLineNo, uLONG ulCount, LPVOID lpvData, uLONG ulFormat )
{
	DialogDataHdl	DlogData	= (DialogDataHdl)GetWRefCon(pdlgAssert);	
	DialogDataPtr	pData		= *DlogData;
	
	pData->lpszReason	= lpszReason;
	pData->lpszFilename	= lpszFilename;
	pData->lpvData		= lpvData;
	pData->ulFormat		= ulFormat;
	pData->ulDataCount	= ulCount;
	pData->ulLineNo		= ulLineNo;

	SetControlMaximum( g_hScrollControl,
		((g_tdb_hSymList != nil) ? (*g_tdb_hSymList)->wItemCnt - 1 : 0));
		
	SetControlValue( g_hDisableCntrl, 0 );

	return TRUE;
}


/*--------------------------------------------------------------------------
 * Function: DlgMessageFilter
 * Expects:  DialogPtr   dlog
 *           EventRecord *evt
 *           short       *itemHit
 * Returns:  Boolean     bAnswer
 * Purpose:  Message filter for the Assertion dialog
 *--------------------------------------------------------------------------
 */
static pascal Boolean DlgMessageFilter( DialogPtr dlog, EventRecord *evt, short *itemHit )
{
	unsigned char	ch;
	Point				where;
	short				iCtlPart;
	short				iPartCode;
	Boolean 			ans		= false;
	Boolean			doHilite	= false;
	ControlHandle	whichCtl	= nil;	//	RMS - added initialization to NIL

	switch( evt->what )
		{
		case updateEvt:
			if( (WindowPtr)(evt->message) == dlog )
				{
				DoDrawDialog(dlog);
				*itemHit = 0;
				}
			break;

		case mouseDown:
			where		= evt->where;
			GlobalToLocal(&where);
			iCtlPart	= FindControl( where, dlog, &whichCtl );
			switch( iCtlPart )
				{
				case kControlButtonPart:
					iPartCode = TrackControl( whichCtl, where, nil );
					if( iPartCode == 0 )
						*itemHit = 0;
					else
						doHilite = ans = *itemHit = GetControlReference( whichCtl );
					break;

				case kControlCheckBoxPart:
					iPartCode = TrackControl( whichCtl, where, nil );
					if( iPartCode == 0 )
						*itemHit = 0;
					else
						{
						BOOLEAN	bVal = !GetControlValue((ControlHandle)whichCtl);
						SetControlValue( whichCtl, bVal );
						ans = *itemHit = GetControlReference( whichCtl );
						if (ans == ID_DISABLEASSERTS)
							{
							tdb_ToggleDebugFlags(kDbgAssertDisabled);
							}
						else if (ans == ID_DISPLAYASHEX)
							{
							tdb_ToggleDebugFlags(kDbgHexDump);
							InvalRect(&(dlog->portRect));
							}
						}
					break;

				case kControlUpButtonPart:					
				case kControlDownButtonPart:				
				case kControlPageUpPart:					
				case kControlPageDownPart:				
					ans = TrackControl( whichCtl, where, NewControlActionProc(_ScrollProc) );
					break;

				case kControlIndicatorPart:
					break;
				}
			break;

		case keyDown:
		case autoKey:
			ch	= (unsigned char)evt->message;
			if( evt->modifiers & cmdKey )
				{
//				*itemHit = DlgCheckButtonsForLetter(dlog, ch);
//				if (*itemHit)
//					ans = doHilite = TRUE;
				}
			else
				{
				switch( ch )
					{
					case CRkey:
					case ENTERkey:
						*itemHit	= ID_OK;
						doHilite	= ans = true;
						break;
	
					case UPARROW:
						_DoScroll( g_hScrollControl, dlog, kUp, 1 );
						break;
				
					case DOWNARROW:
						_DoScroll( g_hScrollControl, dlog, kDown, 1 );
						break;
				
					case HOME:
						_DoScroll( g_hScrollControl, dlog, kUp, kTop );
						break;
				
					case ENDKEY:
						_DoScroll( g_hScrollControl, dlog, kDown, kBottom );
						break;
				
					case PAGE_UP:
						_DoScroll( g_hScrollControl, dlog, kUp, kPage );
						break;
				
					case PAGE_DOWN:
						_DoScroll( g_hScrollControl, dlog, kDown, kPage );
						break;
					}
				}
			break;
		}

	if( doHilite )
		{
		if( whichCtl )
			{
			long soon = TickCount() + 7;		/* Or whatever feels right */
			HiliteControl(whichCtl,1);
			while (TickCount() < soon) ;		/* Leave hilited for a bit */
			HiliteControl(whichCtl, 0);
			}
		}

	return ans;
}

//==================================================================================
//	_ScrollProc(ControlHandle cntrl, short part)
//
//==================================================================================
static pascal void _ScrollProc(ControlHandle cntrl, short part)
{
	Rect		rect;
	GrafPtr		savePort;
	FontInfo	finfo;
	short		page;
	
	rect.left = 88;
	rect.top = 92;
	rect.right = 256;
	rect.bottom = 160;

	GetPort(&savePort);
	SetPort(g_pdlgAssertDlg);
	
	TextFont(monaco);													//	use ROM-based font (required)
	TextSize(9);														//	make 9 point
	GetFontInfo(&finfo);												//	font must be in ROM to call this here
		
	page = (rect.bottom - rect.top) / finfo.ascent + finfo.descent + finfo.leading;		//	calc number of lines in rect - based on font
	
	switch(part)
	{
		case kControlUpButtonPart:
			_DoScroll(cntrl, g_pdlgAssertDlg, kUp, 1);
			break;
						
		case kControlDownButtonPart:				
			_DoScroll(cntrl, g_pdlgAssertDlg, kDown, 1);
			break;
	
		case kControlPageUpPart:
			_DoScroll(cntrl, g_pdlgAssertDlg, kUp, kPage);
			break;

		case kControlPageDownPart:				
			_DoScroll(cntrl, g_pdlgAssertDlg, kDown, kPage);
			break;
	}
	SetPort(savePort);
}

//==================================================================================
//	_DoScroll(DialogPtr dlog, short direction, short amount)
//
//==================================================================================
static void _DoScroll(ControlHandle cntrl, DialogPtr dlog, short direction, short amount)
{
//	short		type;
//	Handle		handle;
	Rect		rect;
	Rect		erase;
	GrafPtr		savePort;
	FontInfo	finfo;
	short		numLines;
	short		fontHeight;
	short		start = g_sCurrentSymbol;
	short		numSymbols = (*g_tdb_hSymList)->wItemCnt;

	rect.left = 88;
	rect.top = 92;
	rect.right = 256;
	rect.bottom = 160;

	GetPort(&savePort);
	SetPort(dlog);
	
	TextFont(monaco);													//	use ROM-based font (required)
	TextSize(9);														//	make 9 point
	GetFontInfo(&finfo);												//	font must be in ROM to call this here
	
	fontHeight = finfo.ascent + finfo.descent + finfo.leading;			//	height of font in pixels
	
	numLines = (rect.bottom - rect.top) / fontHeight;		//	calc number of lines in rect - based on font
	
	if (direction == kUp)
	{
		switch (amount)
		{
			case kTop:
				start = 0;
				break;

			case kPage:
				if (start - numLines >= 0)
					start -= numLines;
				else
					start = 0;
				break;

			default:
				if (start - 1 >= 0)
					--start;
				break;
		}
	}
	else
	{
		switch (amount)
		{
			case kBottom:
				start = numSymbols - numLines;
				if (start < 0)
					start = 0;
				break;

			case kPage:
				if (start + numLines < numSymbols)
					start += numLines;
				else
					start = numSymbols - 1;
				break;

			default:
				if (start + 1 < numSymbols)
					++start;
				break;
		}
	}
	SetControlValue( cntrl, start);				
	
	if (g_sCurrentSymbol != start)
	{
		erase = rect;
		InsetRect(&erase, 1, 1);
		erase.right -= 15;

		EraseRect(&erase);

		_tdb_DisplayStackCrawl(dlog, &rect, start);
	
		g_sCurrentSymbol = start;
	}
	
	SetPort(savePort);
}

//==================================================================================
//	DlgCheckButtonsForLetter(DialogPtr dlog, EventRecord *evt, short *itemHit)
//
//	comment:	this routine finds the first button in the item list of the passed
//				in dialog that has the same first letter as the letter passed in.
//				(not case sensative)
//==================================================================================
short DlgCheckButtonsForLetter(DialogPtr dialog, unsigned char ch)
{
	unsigned char	och;
	Rect			rect;
	Handle			itemHandle;
	short			i;
	short			numOfItems;
	short			itemType;
	short			itemHit = 0;
	DialogPeek		curDialogRec = (DialogPeek)dialog;


	if ((ch >= 'a') && (ch <= 'z'))			//	 Get Uppercase letter if necessary	
		och = ch - 'a' + 'A';
	else if ((ch >= 'A') && (ch <= 'Z'))
		och = ch - 'A' + 'a';

	numOfItems = **((short **)(curDialogRec->items)) + 1;
	for (i = 1; i <= numOfItems; ++i)
	{
		GetDialogItem(dialog, i, &itemType, &itemHandle, &rect);
		if (itemType == (btnCtrl + ctrlItem))
		{
			sBYTE test = (**(ControlHandle)itemHandle).contrlTitle[1];
			if ((ch == test) || (och == test))
			{
				itemHit = i;
				break;
			}
		}
	}
	return (itemHit);
}

//==================================================================================
//	DoDialogItem(DialogPtr dlog, EventRecord *evt, short *itemHit)
//
//	Return: whether or not the dialog should be closed (keepGoing).
//==================================================================================
static int DoDialogItem(DialogPtr /* dlog */, short itemHit)
{
	short	keepGoing = true;

	if (itemHit < 1 || itemHit > ID_DISABLEASSERTS )
		return(keepGoing);				/* Only legal items, please */

	switch(itemHit)
	{
		case ID_OK:
			keepGoing = false;
			break;

		case ID_TERMINATE:
			keepGoing = false;
			break;

		case ID_DEBUGGER:
			keepGoing = false;
			break;
	}
	return(keepGoing);
}


static void DoDrawDialog(DialogPtr dlog)
{
	short			size;
	void			*pdata;
	Str255			str;
	DialogDataHdl	DlogData = (DialogDataHdl)GetWRefCon(dlog);	
	DialogDataPtr	pData = *DlogData;
	FontInfo		finfo;
	GrafPtr			savePort;
	Rect			rect;
	
	GetPort(&savePort);
	SetPort(dlog);
	
	TextFont(systemFont);
	TextSize(12);
	GetFontInfo(&finfo);

	DrawControls(dlog);

	DefaultButtonProc(dlog);
	
	//	Draw Titles
	MoveTo(kSCTitleHorzCoord, kSCTitleVertCoor + (finfo.ascent + finfo.descent) - 5);
	DrawString(kSCTitleStr);

	MoveTo(kLineTitleHorzCoord, kLineTitleVertCoor + (finfo.ascent + finfo.descent) - 4);
	DrawString(kLineTitleStr);

	MoveTo(kReasonTitleHorzCoord, kReasonTitleVertCoor + (finfo.ascent + finfo.descent) - 4);
	DrawString(kReasonTitleStr);

	MoveTo(kFileTitleHorzCoord, kFileTitleVertCoor + (finfo.ascent + finfo.descent) - 4);
	DrawString(kFileTitleStr);

	MoveTo(kDataTitleHorzCoord, kDataTitleVertCoor + (finfo.ascent + finfo.descent) - 4);
	DrawString(kDataTitleStr);

	//	Draw data
	TextFont(monaco);
	TextSize(9);
	GetFontInfo(&finfo);

	StackCrawlProc(dlog);

	pData = *DlogData;
	
	MoveTo(kLineHorzCoord, kLineVertCoor + (finfo.ascent + finfo.descent));
	NumToDecStr(pData->ulLineNo, (sBYTE *)str);
	DrawString(str);

	MoveTo(kReasonHorzCoord, kReasonVertCoor + (finfo.ascent + finfo.descent));
	CToPascal((sBYTE *)pData->lpszReason, str);
	rect.top = kReasonVertCoor + (finfo.ascent + finfo.descent);
	rect.left = kReasonHorzCoord;
	rect.bottom = rect.top + 35;
	rect.right = rect.left + 320;
	DisplayTextinRect(&rect, (sBYTE *)str);

	MoveTo(kFileHorzCoord, kFileVertCoor + (finfo.ascent + finfo.descent));
	CToPascal((sBYTE *)pData->lpszFilename, str);
	DrawString(str);

	str[0] = 0;
	pdata = pData->lpvData;
	size = pData->ulDataCount;

	if (pdata != nil)
	{
		switch(pData->ulFormat)
		{
			case 'b':		// dump bytes
			case 'B':
				DisplayBytes((sBYTE FAR *)pdata, size, (sBYTE *)str);
				break;

			case 'w':		// dump words
			case 'W':
				DisplayWords((sBYTE FAR *)pdata, size, (sBYTE *)str);
				break;

			case 'l':		// dump longs
			case 'L':
				DisplayLongs((sBYTE FAR *)pdata, size, (sBYTE *)str);
				break;

			case 's':		// dump C string
			case 'S':
				DisplayCStr((sBYTE FAR *)pdata, (sBYTE *)str);
				break;

			case 'p':		// dump P string
			case 'P':
				DisplayPStr((sBYTE FAR *)pdata, (sBYTE *)str);
				break;
		}
	}

	MoveTo(kDataHorzCoord, kDataVertCoor + (finfo.ascent + finfo.descent));
	rect.top = kDataVertCoor + (finfo.ascent + finfo.descent);
	rect.left = kDataHorzCoord;
	rect.bottom = rect.top + 35;
	rect.right = rect.left + 320;
	DisplayTextinRect(&rect, (sBYTE *)str);	

	SetPort(savePort);
}

//==================================================================================
//	DisplayBytes(void FAR *pdata, long count)
//
//==================================================================================
static void DisplayBytes(sBYTE FAR *pData, long count, sBYTE *msg)
{
	Str32	temp;
	short	numberofBytes;
	short	i;
	
	NumToHexStr((long)pData, (sBYTE *)temp, 8);
	BlockMove(kDisplayStr, msg, kDisplayStrLength);
	BlockMove((sBYTE *)&temp[1], (sBYTE *)&msg[14], temp[0]);
	
	numberofBytes = count / sizeof(sBYTE);
	
	for (i = 0 ; i < numberofBytes; ++i)
	{
		ConvertNumberToString(*((sBYTE *)pData), (sBYTE *)temp, 2);
		
		if (msg[0] + temp[0] + 1 < sizeof(Str255))
		{
			BlockMove((sBYTE *)temp + 1, (sBYTE *)&msg[msg[0] + 1], temp[0]);
			msg[0] += temp[0];
			msg[msg[0] + 1] = ' ';
			msg[0]++;
			pData += sizeof(sBYTE);
		}
	}
}

//==================================================================================
//	DisplayWords(void FAR *pdata, long count)
//
//==================================================================================
static void DisplayWords(sBYTE FAR *pData, long count, sBYTE *msg)
{
	Str32	temp;
	short	numberofWords;
	short	i;
	
	NumToHexStr((long)pData, (sBYTE *)temp, 8);
	BlockMove(kDisplayStr, msg, kDisplayStrLength);
	BlockMove((sBYTE *)&temp[1], (sBYTE *)&msg[14], temp[0]);
	
	numberofWords = count;// / sizeof(short);
	
	for (i = 0 ; i < numberofWords; ++i)
	{
		ConvertNumberToString(*((short *)pData), (sBYTE *)temp, 4);
		
		if (msg[0] + temp[0] + 1 < sizeof(Str255))
		{
			BlockMove((sBYTE *)temp + 1, (sBYTE *)&msg[msg[0] + 1], temp[0]);
			msg[0] += temp[0];
			msg[msg[0] + 1] = ' ';
			msg[0]++;
			pData += sizeof(short);
		}
	}
}

//==================================================================================
//	DisplayWords(void FAR *pdata, long count)
//
//==================================================================================
static void DisplayLongs(sBYTE FAR *pData, long count, sBYTE *msg)
{
	Str32	temp;
	short	numberofLongs;
	short	i;
	
	NumToHexStr((long)pData, (sBYTE *)temp, 8);
	BlockMove(kDisplayStr, msg, kDisplayStrLength);
	BlockMove((sBYTE *)&temp[1], (sBYTE *)&msg[14], temp[0]);
	
	numberofLongs = count;// / sizeof(long);
	
	for (i = 0 ; i < numberofLongs; ++i)
	{
		ConvertNumberToString(*((long *)pData), (sBYTE *)temp, 8);
		
		if (msg[0] + temp[0] + 1 < sizeof(Str255))
		{
			BlockMove((sBYTE *)temp + 1, (sBYTE *)&msg[msg[0] + 1], temp[0]);
			msg[0] += temp[0];
			msg[msg[0] + 1] = ' ';
			msg[0]++;
			pData += sizeof(long);
		}
	}
}

//==================================================================================
//	DisplayWords(void FAR *pdata, long count)
//
//==================================================================================
static void DisplayCStr(sBYTE FAR *pData, sBYTE *msg)
{
	short	i = 0;
	Str32	temp;

	NumToHexStr((long)pData, (sBYTE *)temp, 8);
	BlockMove(kDisplayStr, msg, kDisplayStrLength);
	BlockMove((sBYTE *)&temp[1], (sBYTE *)&msg[14], temp[0]);
	
	do
	{
		++i;
	}
	while (pData[i] != '\0');
	
	if (msg[0] + i > sizeof(Str255))
		i = sizeof(Str255) - msg[0];
	BlockMove((sBYTE *)pData, (sBYTE *)&msg[msg[0] + 1], i);
	msg[0] += i;
}

//==================================================================================
//	DisplayWords(void FAR *pdata, long count)
//
//==================================================================================
static void DisplayPStr(sBYTE FAR *pData, sBYTE *msg)
{
	Str32	temp;

	NumToHexStr((long)pData, (sBYTE *)temp, 8);
	BlockMove(kDisplayStr, msg, kDisplayStrLength);
	BlockMove((sBYTE *)&temp[1], (sBYTE *)&msg[14], temp[0]);
	if (msg[0] + pData[0] > sizeof(Str255))
		pData[0] = sizeof(Str255) - msg[0];
	BlockMove((sBYTE *)&pData[1], (sBYTE *)&msg[msg[0] + 1], pData[0]);
	msg[0] += pData[0];
}

//==================================================================================
//	CToPascal(sCHAR *str, StringPtr pstr)
//
//==================================================================================
static StringPtr CToPascal(sBYTE *str, StringPtr pstr)
{
	short i = 0;

	do
	{
		pstr[i+1] = str[i];
		++i;
	}
	while (str[i] != '\0' && i < 255);
	
	pstr[0] = i;
	
	return (StringPtr)pstr;
}

//==================================================================================
//	DecStrToHexStr(sCHAR *str)
//
//==================================================================================
static void NumToHexStr(long number, sBYTE *str, sWORD cnt)
{
	short	i = 1;
	Byte	mask;

	//	adjust number to account for the bits not desired.
	if (cnt < 8)
		number <<= 8;
	if (cnt < 6)
		number <<= 8;
	if (cnt < 4)
		number <<= 8;

	//	first insert the '0x' designator into the string
	str[i++] = '0';
	str[i++] = 'x';
	cnt += 2;		//	add 2 to the count because I added the '0x' identifier

	do
	{
		mask = (number & 0xF0000000) >> 28;
		if (mask < 10)
			str[i++] = (sBYTE)(mask + '0');
		else
			str[i++] =  (sBYTE)((mask - 10) + 'A');
		number <<= 4;
	}
	while (i <= cnt);
	
	str[0] = i - 1;
}

//==================================================================================
//	NumToDecStr(sCHAR *str)
//
//==================================================================================
static void NumToDecStr(long number, sBYTE *str)
{
	uBYTE	c;
	sWORD	count;
	sWORD	i;
	sWORD	j;
	sLONG	sign;
	uLONG	result = true;
	sLONG	n = (sLONG)number;
	
	if ((sign = n) < 0)
		n = -n;
	i=1;	
	do 
	{
		str[i++] = ((uBYTE)n % 10) + '0';
	} while ((n /= 10) > 0);
	
	if (sign < 0)
		str[i++] = (uBYTE)'-';

	count = i;
	str[0] = i - 1;

	for (i = 1, j = count - 1; i < j; i++, j--)
	{
		c = str[i];
		str[i] = str[j];
		str[j] = c;
	}	
}


//==================================================================================
//	DefaultButtonProc(WindowPtr dlog, short item)
//
//==================================================================================
static void DefaultButtonProc(WindowPtr dlog)
{
	Rect	rect;
	GrafPtr	savePort;

	GetPort(&savePort);
	SetPort(dlog);
	
	rect.left = 312;
	rect.top = 168;
	rect.right = 380;
	rect.bottom = 188;

	PenSize(3,3);
	InsetRect(&rect, -4, -4);
	FrameRoundRect(&rect, 16, 16);
	PenSize(1,1);
	SetPort(savePort);
}

//==================================================================================
//	StackCrawlProc(WindowPtr dlog, short item)
//
//==================================================================================
static void StackCrawlProc(WindowPtr dlog)
{
	Rect	rect;

	rect.left = 88;
	rect.top = 92;
	rect.right = 256;
	rect.bottom = 160;
	_tdb_DisplayStackCrawl(dlog, &rect, g_sCurrentSymbol);
}

//==================================================================================
//	StackCrawlProc(WindowPtr dlog, short item)
//
//==================================================================================
static void DisplayTextinRect(Rect *rect, sBYTE *str)
{
	FontInfo finfo;
	Rect	rc;
	short	i;
	short	fontheight;
	short	width = rect->right - rect->left;
	short	length = (unsigned char)str[0];
	short	oldvert = 0;
	short	size = 1;
	short	save;

	GetFontInfo(&finfo);
	fontheight = finfo.ascent + finfo.descent + finfo.leading;

	do
	{
		i = 0;
		do
		{
			++i;
			if (size + i > length)
				break;
		}
		while(TextWidth(str, size, i) < width);

		if (size + i <= length)
		{
			save = i;
			do
			{
			--i;
			}
			while (str[size + i] != ' ' && str[size + i] != '\t' && str[size + i] != '\r' && i > 0);
		
			if (i <= 0)
				i = save - 1;
		}

		SetRect(&rc, rect->left, rect->top + oldvert - fontheight, rect->right, rect->top + oldvert);
		EraseRect(&rc);
		if (length > 0)
			DrawText(str, size, i);

		size += i;

		if (rect->top + fontheight + oldvert > (rect->bottom - fontheight))
			break;
		oldvert += fontheight;
		MoveTo(rect->left, rect->top + oldvert);
	}
	while(size < length);
}

//==================================================================================
//	EOF
//==================================================================================


#endif /* defined(MAC) */
#endif	//	TPSDEBUG

