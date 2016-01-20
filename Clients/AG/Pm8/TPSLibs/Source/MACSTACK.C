/*-----------------------------------------------------------------
 * $Workfile:: MACSTACK.C                                         $
 * $Revision:: 1                                                  $
 *     $Date:: 11/06/96 7:22a                                     $
 *   $Author:: Mike_houle                                         $
 *
 *-----------------------------------------------------------------
 * Desc: 
 *-----------------------------------------------------------------
 *
 * $Log: /Development/TPSLibs/Source/MACSTACK.C $
 * 
 * 1     11/06/96 7:22a Mike_houle
 *
 *-----------------------------------------------------------------
 */

#if defined( TPSDEBUG )
#if defined( MAC )

#include "TPSTYPES.H"
#include "StackCra.h"

//
//	DEFINES
#ifndef THINK_C
	extern long MyGetA6(void) ONEWORDINLINE(0x200E);
	#define	_GetStackPtr(sp)	sp=(StackPtr)MyGetA6()
#else
	#define	_GetStackPtr(sp)	asm{ move.l a6,sp }
#endif

#define kOffsetToSymbols	(sizeof(short) + sizeof(short) + sizeof(long))
#define kMaxNameLength		(sizeof(Str255))
#define kMoreStringSize		(5)
#define kMaxDepth				(40)
#define kScrollOffset		(16)
#define kMoreString			"\pMore"
#define kNoNameString		"\pUndefined"
#define kMainRoutineName	"\pmain"	//	REVIEW RMS - will this work for C++?
#define kUpArrowId			128
#define kDownArrowId			129

//
//	useful instruction bit patterns
#define kLinkInstruction	0x4E56
#define kUnlinkInstruction	0x4E5E
#define kJMPA0Instruction	0x4ED0
#define kRTSInstruction		0x4E75
#define kRTDInstruction		0x4E74

//
// Local types
typedef struct tagStackPtr
{
	long	nextEntry;
	long	returnAddr;
} StackStruct, *StackPtr;


//
//		External Variables
extern HSYMLIST	g_tdb_hSymList;


//
// Local Prototypes
static void getMacsBugSymbol( unsigned char *symStart, unsigned char *symbol );

//
// Public API
/*--------------------------------------------------------------------------
 * Function: _tdb_InitStackCrawl
 * Expects:  nothing
 * Returns:  BOOLEAN bSuccess
 * Purpose:  Allocates the memory needed to get & display a stack crawl
 *           Must be called before the stack crawl function is used.
 *           It moves memory. - We could pass in a depth parameter to determine
 *           how much memory gets allocated. 
 * Globals:  g_tdb_hSymList -- allocated and initialized if successful
 *--------------------------------------------------------------------------
 */
TDBAPI( BOOLEAN ) _tdb_InitStackCrawl( DEBUGPARMS FAR * /* lpDebugParms */, uLONG /* ulFlags */ )
{
#ifndef powerc
	//
	// Allocate the symbol list...
	g_tdb_hSymList = (HSYMLIST)NewHandle( sizeof(SYMLIST) + (kMaxNameLength * kMaxDepth) );
	if( !g_tdb_hSymList )
		return FALSE;
	//
	// Initialize the symbol list...
	(*g_tdb_hSymList)->wItemCnt		= 0;		// no symbols in list yet.
	(*g_tdb_hSymList)->byteCount	= sizeof(SYMLIST) - sizeof((*g_tdb_hSymList)->symbols);
#endif
	//
	// Done...
	return TRUE;
}


/*--------------------------------------------------------------------------
 * Function: _tdb_DeinitStackCrawl
 * Expects:  nothing
 * Returns:  nothing
 * Purpose:  This routine disposes of the memory allocated by
 *           _tdb_InitMacStackCrawl.
 *           Needs to be called when your thru with the StackCrawl.
 * Globals:  g_tdb_hSymList - free'd and reset to NULL
 *--------------------------------------------------------------------------
 */
TDBAPI( VOID ) _tdb_DeinitStackCrawl( VOID )
{
#ifndef powerc
	if( g_tdb_hSymList )
		{
		DisposeHandle( (Handle)g_tdb_hSymList );
		g_tdb_hSymList = (HSYMLIST)NULL;
		}
#endif
}



/*--------------------------------------------------------------------------
 * Function: _tdb_DisplayMacStackCrawl
 * Expects:  WindowPtr   window         -- window to draw into
 *           Rect*       prcDisplay     -- rectangle to draw within
 *           short       sTop           -- index of symbol at top of rect
 * Returns:  nothing
 * Purpose:  This routine displays the list of symbols currently on the stack.
 * Globals:  g_tdb_hSymList -- the actual stack crawl must have been setup
 *                         via _tdb_DoStackCrawl...
 *--------------------------------------------------------------------------
 */
TDBAPI( VOID ) _tdb_DisplayStackCrawl( WindowPtr window, Rect* prcDisplay, short sTop )
{
	GrafPtr	savePort;

#ifndef powerc
	Rect		rcText;

	LPSZ		templist;
	short		numLines;
	short		numItems;
	short		limit;
	short		i;
#endif

	FontInfo	finfo;
	short		fontHeight;

	GetPort(&savePort);
	SetPort(window);
	
	TextFont(monaco);													//	use ROM-based font (required)
	TextSize(9);														//	make 9 point
	GetFontInfo(&finfo);												//	font must be in ROM to call this here
	
	fontHeight = finfo.ascent + finfo.descent + finfo.leading;			//	height of font in pixels
	
#ifndef powerc
	numLines = (prcDisplay->bottom - prcDisplay->top) / fontHeight;		//	calc number of lines in rect - based on font
	numItems = (short)(*g_tdb_hSymList)->wItemCnt;
	templist = (LPSZ)((*g_tdb_hSymList)->symbols);

	//
	// Skip to the first entry to display...
	for( i = 0; i<sTop; ++i )
		templist += (templist[0] + 1);

	//
	// Determine number of symbols to draw...
	limit = sTop + numLines;
	if( numItems < limit )
		limit = numItems;

	//
	// get local copy of display rect so we can move it around...
	rcText = *prcDisplay;

	//
	// Draw the symbols in the rectangle...
	for(i = sTop; i < limit; ++i)
		{
		Str255			symbol;
		short			strWidth;//	= StringWidth( templist );
		unsigned char	ucOrigLen	= templist[0];

		BlockMoveData(templist, symbol, *templist + 1);
		strWidth = StringWidth(symbol);
		//
		// Clip the string so it fits in the rectangle...
		while( (rcText.left + strWidth + 2) > (rcText.right - kScrollOffset) )
			{
			//
			//	the string would over-write the scroll bar, so shrink it...
			--symbol[0];
			strWidth = StringWidth( symbol );
			}
		//
		// Move to the baseline...
		MoveTo( rcText.left + 2, rcText.top + finfo.ascent );
		//
		// Draw the symbol name...
		DrawString( (StringPtr)templist );
		//
		// Restore the original length...
		templist[0] = ucOrigLen;
		//
		// Move down to the next line...
		rcText.top += fontHeight;
		//
		// Advance to next symbol...
		templist += (templist[0]+1);
		}
#else
	MoveTo(prcDisplay->left + 2, prcDisplay->top + finfo.ascent);
	DrawString("\pnot available on PowerPC.");
#endif

	//
	// Frame the list...
	FrameRect( prcDisplay );
	//
	// Reset to the old port...
	SetPort(savePort);
	//
	// Done...
}

/*--------------------------------------------------------------------------
 * Function: _tdb_DoStackCrawl
 * Expects:  nothing
 * Returns:  BOOLEAN bSuccess
 * Purpose:  RMS - this routine works in a very mission-specific fashion,
 *           for maximum performance. Instead of analyzing the code and
 *           trying to do a full disassembly, we spin through the code
 *           stream a word at a time. Since we're operating in a context-
 *           sensitive fashion, we won't find an instruction erroneously.
 * Globals:  g_tdb_hSymList -- fills it in...
 *--------------------------------------------------------------------------
 */
TDBAPI( BOOLEAN ) _tdb_DoStackCrawl( VOID )
{
#ifndef powerc
	StackPtr		theStack;			//	pointer to stack frame
	unsigned short *codePtr;			//	pointer to code
	BOOLEAN			found		= false;	//	was the symbol found
	BOOLEAN			endCrawl	= false;	//	did we run pass the number of symbols we could store
	Str255			name;					//	symbol name

	unsigned int	uMaxSize;
	unsigned int	uCurSize;
	uBYTE				*templist;

	ShowCursor();

	//
	//	clean up from last stack crawl
	if ((*g_tdb_hSymList)->wItemCnt)
		(*g_tdb_hSymList)->wItemCnt = 0;

	//
	// watch the end of the handle (leave room for the 'More' string)
	uMaxSize = (kMaxNameLength * kMaxDepth) - kMoreStringSize;
	uCurSize	= 0;
	templist	= (uBYTE*)(*g_tdb_hSymList)->symbols;

	//
	//	what about the pc - should we sTop from there ???? 
	_GetStackPtr( theStack );

	//
	// Do the crawl...
	do
		{
		found = false;
		//
		// Get the return address from the stack
		codePtr = (unsigned short *)theStack->returnAddr;

		//
		// loop through the instructions until we find the unlink A6 instruction...
		do
			{
			unsigned short opcode = *(codePtr++);
			if (opcode == kUnlinkInstruction)
				{
				//
				// we found the unlink instruction
				// Get the next instruction...
				opcode = *(codePtr++);
				//
				// Check to see if the instruction following the UNLK A6 is an
				// RTS, RTD #N, or JMP (a0).  When these sequences are present
				// and Macsbug names are turned on, you know you're at the end
				// of a function...
				if( (opcode == kRTSInstruction)		||
					 (opcode == kRTDInstruction)		||
					 (opcode == kJMPA0Instruction) )
					{
					//
					//	symbols are present, and we've found the name...
					found = true;
					//
					// Get the name...
					getMacsBugSymbol( (unsigned char *)codePtr, (unsigned char *)&name );
					//
					// Insert it into the list of symbols...
					if( g_tdb_hSymList )
						{
						//
						// if we found an empty name, use the NoName string...
						if( !name[0] )
							BlockMoveData( kNoNameString, name, 10 );
						//
						// make sure we don't overflow the buffer...
						if( (uCurSize+name[0]+1) > uMaxSize )
							{
							//
							// the name would overflow the buffer, so substitute the 
							// 'More' string and bail...
							BlockMoveData(kMoreString, name, kMoreStringSize);
							endCrawl = true;
							}
						//
						// store the name in the symbol list...
						BlockMoveData( name, templist, name[0]+1 );
						//
						// advance to next position in list...
						templist += (templist[0]+1);
						//
						// count the name!
						++(*g_tdb_hSymList)->wItemCnt;
						//
						// don't trace back beyond 'main()'
						if( IUEqualString(name, kMainRoutineName) == 0 )
							endCrawl = TRUE;
						}// if g_tdb_hSymList was allocated
					}// if kRTSInstruction, kRTDInstruction or kJMPA0Instruction follow the kUnlinkInstruction
				else if (opcode == kLinkInstruction)
					{
					//
					// a link instruction follows the unlink instruction, so there's no name
					// for this function.  Exit the loop...
					found = true;
					}// if kLinkInstruction follows kUnlinkInstruction
				}// if kUnlinkInstruction
			else if (opcode == kLinkInstruction)
				{
				//	if we find a link instruction before finding an unlink instruction,
				// so there is no name for this function.  Exit the loop...
				found = true;
				}// if kLinkInstruction
			}// do
		while( !found );

		//
		// Advance to the next stack frame...
		theStack = (StackPtr)theStack->nextEntry;
		}
	while( theStack != nil && !endCrawl );
#endif

	return true;
}


/*--------------------------------------------------------------------------
 * Function: getMacsBugSymbol
 * Expects:  unsigned char *symStart -- => code containg MacsBug symbol
 *           unsigned char *symbol   -- => buffer to place routine name in.
 * Returns:  nothing
 * Purpose:  Taken from DisAsmLookup.h - (header didn't have a function to do this)
 *           but it explains what the format is for MacsBugs symbols.
 *
 *	Check that the bytes pointed to by symStart represents a valid MacsBug symbol.
 *	The symbol must be fully contained in the bytes sToping at symStart, up to,
 *	but not including, the byte pointed to by the limit parameter.
 *	
 *	If a valid symbol is NOT found, then NULL is returned as the function's result.
 *	However, if a valid symbol is found, it is copied to symbol (if it is not NULL)
 *	as a null terminated Pascal string, and return a pointer to where we think the
 *	FOLLOWING module begins. In the "old style" cases (see below) this will always
 *	be 8 or 16 bytes after the input symStart.  For new style Apple Pascal and C
 *	cases this will depend on the symbol length, existence of a pad byte, and size
 *	of the constant (literal) area.  In all cases, trailing blanks are removed from
 *	the symbol.
 *	
 *	A valid MacsBug symbol consists of the characters '_', '%', spaces, digits, and
 *	upper/lower case letters in a format determined by the first two bytes of the
 *	symbol as follows:
 *	
 *	1st byte	|	2nd byte	|	Byte	|
 *		Range	|	Range		|	Length	| Comments
 *	==============================================================================
 *	 $20 - $7F	|	$20 - $7F	|    	8   | "Old style" MacsBug symbol format
 *	 $A0 - $7F	|	$20 - $7F	|		8   | "Old style" MacsBug symbol format
 *	------------------------------------------------------------------------------
 *	 $20 - $7F 	|	$80 - $FF 	|   	16	| "Old style" MacApp symbol ab==>b.a
 *	 $A0 - $7F 	|	$80 - $FF 	|   	16	| "Old style" MacApp symbol ab==>b.a
 *	------------------------------------------------------------------------------
 *	 $80       	|	$01 - $FF 	|   	 n	| n = 2nd byte       (Apple Compiler symbol)
 *	 $81 - $9F 	|	$00 - $FF 	|   	 m	| m = 1st byte & $7F (Apple Compiler symbol)
 *	==============================================================================
 *	
 *	The formats are determined by whether bit 7 is set in the first and second
 *	bytes.  This bit will removed when we find it or'ed into the first and/or
 *	second valid symbol characters.
 *	
 *	The first two formats in the above table are the basic "old style" (pre-
 *	existing) MacsBug formats. The first byte may or may not have bit 7 set the
 *	second byte is a valid symbol character.  The first byte (with bit 7 removed)
 *	and the next 7 bytes are assumed to comprise the symbol.
 *	
 *	The second pair of formats are also "old style" formats, but used for MacApp
 *	symbols.  Bit 7 set in the second character indicates these formats. The symbol
 *	is assumed to be 16 bytes with the second 8 bytes preceding the first 8 bytes
 *	in the generated symbol.  For example, 12345678abcdefgh represents the symbol
 *	abcdefgh.12345678.
 *	
 *	The last pair of formats are reserved by Apple and generated by the MPW Pascal
 *	and C compilers.  In these cases the value of the first byte is always between
 *	$80 and $9F, or with bit 7 removed, between $00 and $1F.  For $00, the second
 *	byte is the length of the symbol with that many bytes following the second
 *	byte (thus a max length of 255). Values $01 to $1F represent the length itself.
 *	A pad byte may follow these variable length cases if the symbol does not end
 *	on a word boundary.  Following the symbol and the possible pad byte is a word
 *	containing the size of the constants (literals) generated by the compiler.
 *--------------------------------------------------------------------------
 */
static void getMacsBugSymbol( unsigned char *symStart, unsigned char *symbol )
{
	BOOLEAN bFirstHi	= (symStart[0]&0x80)? true : false;
	BOOLEAN	bSecondHi	= (symStart[1]&0x80)? true : false;
	
	symStart[0] &= 0x7f;
	symStart[1] &= 0x7f;
	
	if( !(*symStart) )
		{
		//
		// the first byte is 0 (Apple Compiler symbol) so the
		// length of the symbol(n) is in the second byte...
		BlockMoveData( symStart+1, symbol, symStart[1]+1 );
		}
	else if( *symStart <= 31 )
		{
		//
		//	the first byte is less than 31 (Apple Compiler symbol) so
		// the length of the symbol(m) = 1st byte & $7F 
		BlockMoveData( symStart, symbol, symStart[0]+1 );
		}
	else if( bSecondHi )
		{
		//
		//	the second byte had the 7th bit on so it is an "Old style" MacApp
		// symbol where ab==>b.a
		// copy the last 8 bytes of the name(from the code) to the first 8 of
		// the symbol. then insert a period.
		BlockMoveData( &symStart[8], symbol+1, 8 );
		symbol[9] = '.';
		//
		// now copy the first 8 bytes of the name(from the code) to the last 8
		// bytes of the symbol...
		BlockMoveData( symStart, symbol+10 , 8 );
		//
		// set the length to the default 17 bytes...
		symbol[0] = 17;
		}
	else
		{
		//
		// it must be the old MacsBugs symbol.
		// copy the next 8 bytes from the code to the symbol...
		BlockMoveData( symStart, symbol+1, 8 );
		//
		// set the length to the default 8
		symbol[0] = 8;
		}
	if( bFirstHi ) symStart[0] |= 0x80;
	if( bSecondHi ) symStart[1] |= 0x80;
	return;
}


#endif /* MAC */
#endif // TPSDEBUG

