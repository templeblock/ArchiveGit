/*
// $Header: /PM8/App/WARPOBJ.CPP 1     3/03/99 6:13p Gbeddow $
//
// Warp object routines.
//
// $Log: /PM8/App/WARPOBJ.CPP $
// 
// 1     3/03/99 6:13p Gbeddow
// 
// 2     12/23/98 1:51p Psasse
// Code clean up
// 
//    Rev 1.0   14 Aug 1997 15:27:04   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:41:00   Fred
// Initial revision.
// 
//    Rev 1.12   17 Apr 1997 15:36:04   Fred
// Rotated fill patterns
// 
//    Rev 1.11   11 Apr 1997 13:57:20   Fred
// Font Caching (bitmaps, rotated, warped)
// 
//    Rev 1.10   09 Apr 1997 14:51:44   Fred
// Start of new font caching (fonts and metrics)
// 
//    Rev 1.9   17 Mar 1997 14:48:20   Jay
// Warp shape palette and support.
// 
//    Rev 1.8   07 Feb 1997 08:57:20   Fred
// Passes GetCharacterFormatting() to AdvanceWidth()
// 
//    Rev 1.7   16 Jan 1997 09:02:14   Fred
// Switch to warp field data record
// 
//    Rev 1.6   07 Nov 1996 16:39:32   johno
// Moved strings to resource DLL
// 
//    Rev 1.5   05 Nov 1996 14:06:58   Jay
// Got rid of warnings, etc.
// 
//    Rev 1.4   10 Jul 1996 10:25:52   Jay
// Text states no longer allocated.
// 
//    Rev 1.3   28 Jun 1996 16:18:40   Jay
//  
// 
//    Rev 1.2   27 Jun 1996 13:28:20   Jay
//  
// 
//    Rev 1.1   26 Jun 1996 14:22:28   Jay
//  
// 
//    Rev 1.0   14 Mar 1996 13:44:28   Jay
// Initial revision.
// 
//    Rev 1.0   16 Feb 1996 12:09:40   FRED
// Initial revision.
// 
//    Rev 2.33   07 Sep 1995 15:48:40   JAY
// Now frees the old warp field during an assign().
// 
//    Rev 2.32   29 Aug 1995 13:08:04   JAY
// Now deletes the state if it tries to draw while being edited.
// 
//    Rev 2.31   28 Aug 1995 10:57:42   FRED
// Fixes for flipped inflates.
// 
//    Rev 2.30   25 Aug 1995 14:52:30   JAY
// Will not update while being edited.
// 
//    Rev 2.29   23 Aug 1995 10:20:06   FRED
// Changed text shadow offset algorithm
// 
//    Rev 2.28   19 Aug 1995 14:48:38   FRED
// Inflate now works with flipped objects
// 
//    Rev 2.27   09 Aug 1995 13:09:46   JAY
// Virtual override of MacrosInUse
// 
//    Rev 2.26   01 Aug 1995 12:59:36   JAY
// Added correct name.
// 
//    Rev 2.25   07 Jul 1995 16:10:44   JAY
// Misc changes.
// 
//    Rev 2.24   23 Jun 1995 16:36:50   JAY
// OK. So it REALLY shows the warp text box name now during printing.
// 
//    Rev 2.23   23 Jun 1995 11:40:06   JAY
// Now shows name of Warp Text in "update_names".
// Won't try to draw warp object larger than 32766 device units.
// 
//    Rev 2.22   25 May 1995 13:05:34   FRED
// Supports flipping.
// Fixed redisplay bug.
// 
// 
//    Rev 2.21   17 May 1995 14:25:18   FRED
// Warp text objec refresh bounds.
// 
// 
//    Rev 2.20   13 May 1995 11:14:50   JAY
//  
// 
//    Rev 2.19   11 May 1995 09:37:14   JAY
// Changed CreateStruct to external class.
// 
//    Rev 2.18   10 May 1995 17:42:06   JAY
// Interruptable!
// 
//    Rev 2.17   10 May 1995 12:34:16   FRED
// Shadows on warp text objects.
// 
// 
//    Rev 2.16   09 May 1995 09:48:30   JAY
// Added UsePalette flag.
// 
//    Rev 2.15   08 May 1995 12:33:00   FRED
// Fixed drawing bug.
// 
// 
//    Rev 2.14   08 May 1995 11:57:44   FRED
// New default object attributes.
// 
// 
//    Rev 2.13   08 May 1995 09:13:52   FRED
// Calls COutlinePath::Draw2() to draw borders using Inflate().
// 
// 
//    Rev 2.12   05 May 1995 17:51:40   FRED
// 
//    Rev 2.11   05 May 1995 17:08:04   FRED
// 
//    Rev 2.10   04 May 1995 17:16:16   JAY
// Fixed-point. Drawing objects.
// 
//    Rev 2.9   04 May 1995 12:29:24   FRED
// 
//    Rev 2.8   03 May 1995 10:39:52   FRED
//  
// 
//    Rev 2.7   02 May 1995 15:37:52   JAY
// Improved macro support.
// 
//    Rev 2.6   28 Apr 1995 17:38:18   FRED
//  
// 
//    Rev 2.5   28 Apr 1995 13:49:14   FRED
//  
// 
//    Rev 2.4   27 Apr 1995 13:20:24   FRED
// 
//    Rev 2.3   25 Apr 1995 16:02:28   FRED
// 
//    Rev 2.2   25 Apr 1995 10:33:32   FRED
// 
//    Rev 2.1   20 Apr 1995 16:15:58   FRED
// Warp Text Stuff
// 
//    Rev 2.0   19 Apr 1995 14:46:14   FRED
// Initial revision.
*/

#include "stdafx.h"

#include "pmw.h"
#include "pmgobj.h"
#include "utils.h"

#include "warpobj.h"
#include "warpfld.h"
#include "outpath.h"
#include "pmgdb.h"
#include "pmgfont.h"
#include "macrosrv.h"
#include "textrec.h"
#include "framerec.h"
#include "cfixed.h"

/*****************************************************************************/
/*                                WarpTextObject                                  */
/*****************************************************************************/

/*
// The creator for an Warp object.
*/
		
ERRORCODE WarpTextObject::create(DB_OBJECT_TYPE type, ObjectDatabasePtr owner, VOIDPTR creation_data, PageObjectPtr far *record)
{
	WarpTextObject* object = NULL;
	ERRORCODE error = ERRORCODE_None;

/* Create the frame object. */

	if ((object = new WarpTextObject(type, owner)) == NULL)
	{
		error = ERRORCODE_Memory;
	}
	else
	{
	/* We created the object. */
		if ((error = object->HandleCreationData(creation_data)) != ERRORCODE_None)
		{
			delete object;
			object = NULL;
		}
	}

	*record = (PageObjectPtr)object;
	return error;
}

/*
// The constructor for the Warp object.
*/

WarpTextObject::WarpTextObject(DB_OBJECT_TYPE type, ObjectDatabasePtr owner)
		: CFrameObject(type, owner)
{
	memset(&m_WRecord, 0, sizeof(m_WRecord));
	record.select_flags = SELECT_FLAG_boundary
 									| SELECT_FLAG_size_handles
									| SELECT_FLAG_move_handle
									| SELECT_FLAG_rotate_handle
									;

	m_WRecord.m_clTextFillColor = COLOR_BLACK;
	m_WRecord.m_fTextFillColorExists = TRUE;

	m_WRecord.m_nTextLineWidth = -1;

	m_WRecord.m_clTextLineColor = COLOR_BLACK;
	m_WRecord.m_fTextLineColorExists = TRUE;

	m_WRecord.m_clTextShadowColor = COLOR_BLACK;
	m_WRecord.m_fTextShadowColorExists = FALSE;

	m_WRecord.m_clBackgroundFillColor = COLOR_BLACK;
	m_WRecord.m_fBackgroundFillColorExists = FALSE;

	m_WRecord.m_nBackgroundLineWidth = -1;

	m_WRecord.m_clBackgroundLineColor = COLOR_BLACK;
	m_WRecord.m_fBackgroundLineColorExists = TRUE;

	m_WRecord.m_clBackgroundShadowColor = COLOR_BLACK;
	m_WRecord.m_fBackgroundShadowColorExists = FALSE;

	m_pWarpField = NULL;
	m_pEditingObject = NULL;
}

/*
// The destructor for the Warp object.
*/

WarpTextObject::~WarpTextObject(void)
{
	delete m_pWarpField;
	m_pWarpField = NULL;
}

/*
// Update a Warp object.
*/

UpdateStatePtr WarpTextObject::update(RedisplayContextPtr rc, PBOX_PTR extent, LPRECT clip, UpdateStatePtr ustate, REFRESH_TYPE refresh_type)
{
	if (m_pEditingObject != NULL)
	{
	/*
	// We don't draw while editing.
	// Macros may have changed within the warp text edit session.
	*/
//		ASSERT(ustate == NULL);
		if (ustate != NULL)
		{
			delete ustate;
		}
		return NULL;
	}

	ERRORCODE error;
	FontServerPtr font_server = get_database()->get_font_server();
	HDC hdc = rc->destination_hdc;
	CDC* pDC = CDC::FromHandle(hdc);
	COutlinePath Path;
	BOOL fUsePalette = (rc->info.type == RASTER_TYPE_PALETTE);
	COutlinePathMatrix ShadowMatrix;
	BOOL fBroken = FALSE;

/*
// Establish our state.
// See if we have a state to continue.
// Otherwise, create one.
*/
	CUpdateState* pState;

	if (ustate == NULL)
	{
		pState = new CUpdateState;
		pState->m_XOffset = 0;
		pState->m_fClearing = FALSE;
		// Line traversal.
		pState->m_lLineIndex = -1;
		// Word traversal.
		pState->m_lWordIndex = -1;
		// Character traversal.
		pState->m_lCharacterIndex = -1;
	}
	else
	{
		pState = (CUpdateState*)ustate;
	}

	if (rc->want_update_names())
	{
		// JRO rc->update_name(this, "Title Text Box");
      rc->update_name(this, GET_PMWAPP()->GetResourceStringPointer (IDS_TITLE_TEXT_BOX));
	}

	// Get the unrotated bounds of the object.

	PBOX ObjectBounds = get_unrotated_bound();
	rc->pbox_to_screen(&ObjectBounds, TRUE);

	if (ObjectBounds.x1 - ObjectBounds.x0 > 32766
		 || ObjectBounds.y1 - ObjectBounds.y0 > 32766)
	{
	/* This object will be too big to handle. */
		delete pState;
	   return NULL;
	}

	CRect crObject;
	if (!rc->convert_pbox(&ObjectBounds, crObject, NULL))
	{
	/* This object is clipped or too small. */
		delete pState;
	   return NULL;
	}

	get_refresh_bound(&ObjectBounds, rc);
   
	CFrameRecord* pFrame = LockFrameRecord(&error);

	if (pFrame == NULL)
	{
		// Oh, well.
		return error_update_state(pState, UPDATE_TYPE_Error, error);
	}
	// Get the frame.

	if (pFrame->TextRecord() <= 0)
	{
		pFrame->release();
		delete pState;
	   return NULL;							// Empty frame. This is OK.
	}

	PBOX object_bound;
	PBOX bound;

	object_bound = bound = pFrame->ObjectBounds();

	PCOORD startx = bound.x0;
	PCOORD starty = bound.y0;

	PBOX _extent = *extent;

	// Clip bound to extent.

	if (object_bound.x0 < _extent.x0)
	{
		object_bound.x0 = _extent.x0;
	}

	if (object_bound.x1 > _extent.x1)
	{
		object_bound.x1 = _extent.x1;
	}

	if (object_bound.x0 >= object_bound.x1)
	{
		// Totally clipped away.
		pFrame->release();
		delete pState;
	   return NULL;
	}

	CTextRecord* pText = pFrame->LockTextRecord(&error);

	if (pText == NULL)
	{
		pFrame->release();
		return error_update_state(pState, UPDATE_TYPE_Error, error);
	}

	CTextIterator Iterator;

   // Get the rectangle to toggle.
   
	CRect crToggle;
	rc->pbox_to_screen(&ObjectBounds, TRUE);
	rc->convert_pbox(&ObjectBounds, crToggle, NULL);

	if (refresh_type == REFRESH_REDRAW)
	{
		// We need to clear this!
		rc->erase_area(crToggle);
	}

	rc->toggle(FALSE, crToggle);

	TRY
	{
		// If we are restarting an interrupted update, check to see if the
		// paragraph has been modified since the interrupt.  If so, start
		// the update over from the beginning.

#if 0
		if (pState->c_index != -1 &&
			pState->nParagraphModificationCount != paragraph->GetModificationCount())
		{
			pState->l_index = 0;
			pState->w_index =
			pState->c_index = -1;
		}
#endif

		HRGN hTextClip = NULL;
		//PBOX clip_pbox = _extent;
		
	//	RECT r;
	//	rc->pbox_to_screen(&clip_pbox, TRUE);
	//	if (rc->convert_pbox(&clip_pbox, &r, NULL))
	//	{
	//		if (!rc->is_printing && (hTextClip = CreateRectRgn(r.left, r.top, r.right, r.bottom)) != NULL)
	//		if (!rc->is_printing && (hTextClip = CreateRectRgn(r.left, r.top, r.right+1, r.bottom+1)) != NULL)
	//		{
	//			SaveDC(hdc);
	//			if (rc->clip_region != NULL)
	//			{
	//				CombineRgn(hTextClip, hTextClip, rc->clip_region, RGN_AND);
	//			}
	//			SelectClipRgn(hdc, hTextClip);		// Selects a copy.
	//			DeleteObject(hTextClip);
	//		}
	//	}

		// Initialize our text iterator.
		Iterator.SetRecord(pText);

		// Build a matrix for rotating the object.

		BOOL fRotated = get_rotation() != 0.0;
		BOOL fFlipped = (get_flags() & (OBJECT_FLAG_xflipped | OBJECT_FLAG_yflipped)) != 0;
		BOOL fNegateInflate = FALSE;
		COutlinePathMatrix RotateMatrix;

		RotateMatrix.Identity();
		
		if (fFlipped)
		{
			if (get_flags() & OBJECT_FLAG_xflipped)
			{
				RotateMatrix.XFlip(MakeFixed(crObject.left), MakeFixed(crObject.right));
				fNegateInflate = !fNegateInflate;
			}
			if (get_flags() & OBJECT_FLAG_yflipped)
			{
				RotateMatrix.YFlip(MakeFixed(crObject.top), MakeFixed(crObject.bottom));
				fNegateInflate = !fNegateInflate;
			}
		}
		if (fRotated)
		{
			RotateMatrix.TranslateBy(
				-MakeFixed(crObject.left+crObject.Width()/2),
				-MakeFixed(crObject.top+crObject.Height()/2));
			RotateMatrix.RotateBy(MakeFixed(get_rotation()*RAD2DEGC));
			RotateMatrix.TranslateBy(
				MakeFixed(crObject.left+crObject.Width()/2),
				MakeFixed(crObject.top+crObject.Height()/2));
		}

		// Compute the Warp Field to use.

		CWarpField* pWarpField = GetWarpField();

		if (pWarpField != NULL)
		{
			pWarpField->Transform(crObject);
		}

		if (pState->m_lCharacterIndex == -1)
		{
			// Not continuing an interrupted state.
			// Draw the background if the user has requested.

			if ((m_WRecord.m_fBackgroundFillColorExists)
			 || (m_WRecord.m_fBackgroundLineColorExists && (m_WRecord.m_nBackgroundLineWidth >= 0)))
			{
				// Get the path of the warp field.

				if (pWarpField != NULL)
				{
					pWarpField->TransformedBounds()->Path(&Path);
				}
				else
				{
					Path.Free();
					Path.MoveTo(MakeFixed(crObject.left), MakeFixed(crObject.top));
					Path.LineTo(MakeFixed(crObject.right), MakeFixed(crObject.top));
					Path.LineTo(MakeFixed(crObject.right), MakeFixed(crObject.bottom));
					Path.LineTo(MakeFixed(crObject.left), MakeFixed(crObject.bottom));
					Path.LineTo(MakeFixed(crObject.left), MakeFixed(crObject.top));
					Path.Close();
					Path.End();
				}

				// Compute the background line size.
				
				int nWidth = m_WRecord.m_nBackgroundLineWidth;

				if (nWidth > 0)
				{
					int nXSize = MulDiv(nWidth, rc->x_resolution, 72);
					int nYSize = MulDiv(nWidth, rc->y_resolution, 72);

					if (nXSize > nYSize)
					{
						nWidth = nXSize;
					}
					else
					{
						nWidth = nYSize;
					}

					nWidth = MulDiv(nWidth, rc->GetScaleNumerator(), rc->GetScaleDenominator());
				}
				
				if (nWidth < 0)
				{
					nWidth = COutlinePath::NoLine;
				}
				else
				{
					if (fNegateInflate)
					{
						nWidth = -nWidth;
					}
				}

				// Build a matrix for making the background shadow.

				if (m_WRecord.m_fBackgroundShadowColorExists)
				{
					// Get height the shadow offset vector.

					PCOORD BackgroundShadowX, BackgroundShadowY;
					GetBackgroundShadowOffsets(&BackgroundShadowX, &BackgroundShadowY);
					
					// Build the matrix.

					ShadowMatrix.Identity();
					ShadowMatrix.TranslateBy(
						MakeFixed((int)(rc->page_x_to_screen(BackgroundShadowX)-rc->page_x_to_screen(0))),
						MakeFixed((int)(rc->page_y_to_screen(BackgroundShadowY)-rc->page_y_to_screen(0))));

					// Build the shadow path.

					COutlinePath ShadowPath;
					ShadowPath.Copy(&Path);
					ShadowPath.Transform(&ShadowMatrix);

					if (fRotated || fFlipped)
					{
						ShadowPath.Transform(&RotateMatrix);
					}

					// Draw the shadow. If there is a background, then the entire background shape
					// is shadowed. If there is only an outline, then only the outline is shadowed.
					
					if (m_WRecord.m_fBackgroundFillColorExists)
					{
						ShadowPath.Draw2(
							pDC,
							colorref_from_color(m_WRecord.m_clBackgroundShadowColor),
							m_WRecord.m_fBackgroundShadowColorExists,
							COutlinePath::NoLine,
							RGB(0,0,0),
							FALSE,
							WINDING,
							fUsePalette);
					}
					else
					{
						ShadowPath.Draw2(
							pDC,
							RGB(0,0,0),
							FALSE,
							nWidth,
							colorref_from_color(m_WRecord.m_clBackgroundShadowColor),
							m_WRecord.m_fBackgroundShadowColorExists,
							WINDING,
							fUsePalette);
					}
				}

				if (fRotated || fFlipped)
				{
					Path.Transform(&RotateMatrix);
				}
				
				Path.Draw2(
					pDC,
					colorref_from_color(m_WRecord.m_clBackgroundFillColor),
					m_WRecord.m_fBackgroundFillColorExists,
					nWidth,
					colorref_from_color(m_WRecord.m_clBackgroundLineColor),
					m_WRecord.m_fBackgroundLineColorExists,
					WINDING,
					fUsePalette);
			}
		}

		if ((m_WRecord.m_fTextFillColorExists)
		 || (m_WRecord.m_fTextLineColorExists && (m_WRecord.m_nTextLineWidth >= 0)))
		{
			if (pWarpField != NULL)
			{
				pWarpField->Transform(crObject);
			}
		
			// Run through all the lines.
			// We need to keep track of:
			// - The current line.
			// - The current word.
			// - The current character.
			// - The current character in the current word.

			if (pState->m_lLineIndex == -1)
			{
				// First line in frame.
				pState->m_lLineIndex = 0;
				// First word in frame.
				pState->m_lFirstWordInLine = 0;
				// First character in frame.
				pState->m_lFirstCharacterInLine = pFrame->FirstCharacter();
			}

			LINE_COUNT lLineCount = pFrame->NumberOfLines();

			CHARACTER cbuf[2] = { 0, 0 };
			CMacroServer* pMacroServer = ((PMGDatabase*)database)->GetMacroServer();
		
			// Build a matrix for making the text shadow.
		
			if (m_WRecord.m_fTextShadowColorExists)
			{
				// Get height and width in units of X resolution.
		
				PCOORD TextShadowX, TextShadowY;
				GetTextShadowOffsets((int)lLineCount, &TextShadowX, &TextShadowY);
						
				// Build the matrix.
		
				ShadowMatrix.Identity();
				ShadowMatrix.TranslateBy(
					MakeFixed((int)(rc->page_x_to_screen(TextShadowX)-rc->page_x_to_screen(0))),
					MakeFixed((int)(rc->page_y_to_screen(TextShadowY)-rc->page_y_to_screen(0))));
			}
		
			// Compute the size of the text border line.
			
			int nTextLineWidth = m_WRecord.m_nTextLineWidth;
		
			if (nTextLineWidth > 0)
			{
				int nXSize = MulDiv(nTextLineWidth, rc->x_resolution, 72);
				int nYSize = MulDiv(nTextLineWidth, rc->y_resolution, 72);
		
				if (nXSize > nYSize)
				{
					nTextLineWidth = nXSize;
				}
				else
				{
					nTextLineWidth = nYSize;
				}
		
				nTextLineWidth = MulDiv(nTextLineWidth, rc->GetScaleNumerator(), rc->GetScaleDenominator());
			}
			
			if (nTextLineWidth < 0)
			{
				nTextLineWidth = COutlinePath::NoLine;
			}
			else
			{
				if (fNegateInflate)
				{
					nTextLineWidth = -nTextLineWidth;
				}
			}
			
			for (; pState->m_lLineIndex < lLineCount; pState->m_lLineIndex++)
			{
				CTextLine* pLine = pFrame->GetLine(pState->m_lLineIndex);

				// Compute the extent of this line to see if it needs updating.
		
				PCOORD baseline;
		
				baseline = starty + pLine->m_Baseline;
				bound.y0 = baseline - pLine->m_Ascend;
				bound.y1 = baseline + pLine->m_Descend;
		
				// We need to update the line. Run through all the words.
		
				// Index of current word in this frame.
				if (pState->m_lWordIndex == -1)
				{
					// First word in line.
					pState->m_lWordIndex = pState->m_lFirstWordInLine;
					// First character in line.
					pState->m_lFirstCharacterInWord = pState->m_lFirstCharacterInLine;
				}
		
				WORD_INDEX lWordInLine = pState->m_lWordIndex - pState->m_lFirstWordInLine;
				ASSERT(lWordInLine >= 0);
				for (; lWordInLine < pLine->m_lWords; lWordInLine++, pState->m_lWordIndex++)
				{
					CTextWord* pWord = pFrame->GetWord(pState->m_lWordIndex);
					/* Don't bother drawing goofy words. */
		
					if (pWord->m_wType != WORD_TYPE_break
							&& pWord->m_wType != WORD_TYPE_eop)
					{
						bound.x0 = startx + pWord->m_XOffset;
			
						// This word needs updating. Calculate where we should stop.
			
						if (pState->m_lCharacterIndex == -1)
						{
							// First character in word.
							pState->m_lCharacterIndex = pState->m_lFirstCharacterInWord;
							// First offset in word.
							pState->m_XOffset = bound.x0;
						}
						CHARACTER_INDEX lCharacterInWord = pState->m_lCharacterIndex - pState->m_lFirstCharacterInWord;

						// Run through the text and update any visible characters.

			//			/* Hide toggle selects and mouse cursor if necessary */
			//			if (!rc->is_printing)
			//				rc->toggle(FALSE, &word_rect);

						while (lCharacterInWord < pWord->m_lCharacters)
						{
							CHARACTER c = *(pText->GetCharacter(pState->m_lCharacterIndex));
			
		//					TRACE("Warp: Drawing '%c'\r\n", c);

							Iterator.SetPosition(pState->m_lCharacterIndex);
			
						/*
						// We turn all characters into an array (even if only single).
						// This allows us to treat macro expansions and single characters
						// alike.
						// Note that 'cbuf' is two characters deep and cbuf[1] is 0.
						*/

							CHARACTER* cp;

							if (c >= MACRO_CHARACTER)
							{
								if (pMacroServer == NULL
									 || (cp = pMacroServer->GetMacroText(c)) == NULL)
								{
									cbuf[0] = (CHARACTER)'?';
									cp = cbuf;
								}
							}
							else
							{
								cbuf[0] = c;
								cp = cbuf;
							}

							while (*cp != 0)
							{
								CHARACTER ic = *cp++;

								// Draw the next character.

								CFontDescription FontDescription;
								Iterator.Style().GetFontDescription(&FontDescription);
								FaceEntry* pFace = typeface_server.get_face(typeface_index(FontDescription.m_nTypeface));

								extern uint16 character_map(CHARACTER ch, FaceEntry* face);
						      ic = character_map(ic, pFace);

								if (((PMGFontServer*)font_server)->CharacterOutline(rc, ic, &FontDescription, NULL, &Path))
								{
									// Transform the character outline into the object coordinates.
			
									COutlinePathMatrix Matrix;
									Matrix.Identity();
									Matrix.TranslateBy(
										MakeFixed(rc->page_x_to_screen(pState->m_XOffset)),
										MakeFixed(rc->page_y_to_screen(baseline)));
									Path.Transform(&Matrix);
			
									// Warp the object if a warp file is being applied.
			
									if (pWarpField != NULL)
									{
										Path.Warp(pWarpField);
									}
			
									if (m_WRecord.m_fTextShadowColorExists)
									{
										// Build the shadow path.
			
										COutlinePath ShadowPath;
										ShadowPath.Copy(&Path);
										ShadowPath.Transform(&ShadowMatrix);
			
										if (fRotated || fFlipped)
										{
											ShadowPath.Transform(&RotateMatrix);
										}
			
										// Draw the shadow.
			
										if (m_WRecord.m_fTextFillColorExists)
										{
											ShadowPath.Draw2(
												pDC,
												colorref_from_color(m_WRecord.m_clTextShadowColor),
												m_WRecord.m_fTextShadowColorExists,
												COutlinePath::NoLine,
												RGB(0,0,0),
												FALSE,
												WINDING,
												fUsePalette);
										}
										else
										{
											ShadowPath.Draw2(
												pDC,
												RGB(0,0,0),
												FALSE,
												nTextLineWidth,
												colorref_from_color(m_WRecord.m_clTextShadowColor),
												m_WRecord.m_fTextShadowColorExists,
												WINDING,
												fUsePalette);
										}
									}
			
									if (fRotated || fFlipped)
									{
										Path.Transform(&RotateMatrix);
									}
			
									Path.Draw2(
										pDC,
										colorref_from_color(m_WRecord.m_clTextFillColor),
										m_WRecord.m_fTextFillColorExists,
										nTextLineWidth,
										colorref_from_color(m_WRecord.m_clTextLineColor),
										m_WRecord.m_fTextLineColorExists,
										WINDING,
										fUsePalette);
								}
			
								pState->m_XOffset += font_server->GetAdvanceWidth(&Iterator.Style(), ic);
							}
							pState->m_lCharacterIndex++;
							lCharacterInWord++;
			
							if (rc->check_interrupt())
							{
								pState->type = UPDATE_TYPE_Interrupted;
								fBroken = TRUE;
								break;
							}
						}
						if (fBroken)
						{
							break;
						}
					}
					// Advance the character index.
					pState->m_lFirstCharacterInWord += pWord->m_lCharacters;

					// Advance the unaligned offset.
					pState->m_UnalignedOffset += pWord->m_Width;

					// Start at the beginning of the word.
					pState->m_lCharacterIndex = -1;
				}
				if (fBroken)
				{
					break;
				}

				// Advance to the next line.
				pState->m_lFirstWordInLine += pLine->m_lWords;
				pState->m_lFirstCharacterInLine += pLine->m_lCharacters;

				// Start at the beginning of the line.
				pState->m_lWordIndex = -1;
			}
		}

		rc->toggle(TRUE, crToggle);
		
		if (hTextClip != NULL)
		{
			RestoreDC(hdc, -1);
		}

	/* See if we need to continue. */

		if (!fBroken)
		{
			delete pState;
			pState = NULL;
		}
		ustate = pState;
	}
	CATCH(CErrorcodeException, e)
	{
		ustate = error_update_state(pState, UPDATE_TYPE_Error, e->m_error);
	}
	AND_CATCH_ALL(e)
	{
		ustate = error_update_state(pState, UPDATE_TYPE_Error, ERRORCODE_IntError);
	}
	END_CATCH_ALL

	// Release our captives.

	pText->release();
	pFrame->release();

	return ustate;
}

VOID WarpTextObject::get_refresh_bound(PBOX_PTR refresh_bound, RedisplayContextPtr rc)
{
	*refresh_bound = get_bound();
	
	PCOORD x, y;
	
	GetBackgroundShadowOffsets(&x, &y);
	
	if (get_flags() & OBJECT_FLAG_xflipped)
	{
		x = -x;
	}
	if (get_flags() & OBJECT_FLAG_yflipped)
	{
		y = -y;
	}
	
	// rotate the shadow offset vector
	
	ANGLE dRotation = get_rotation();
	
	if (dRotation != 0.0)
	{
		CFixed fxRotation = MakeFixed(dRotation*RAD2DEGC);
		CFixed fxSine = Sine(fxRotation);
		CFixed fxCosine = Cosine(fxRotation);
		CFixed fxX = MakeFixed((int)x);
		CFixed fxY = MakeFixed((int)y);
		CFixed fxNewX = MulFixed(fxX, fxCosine)+MulFixed(fxY, fxSine);
		CFixed fxNewY = MulFixed(fxY, fxCosine)-MulFixed(fxX, fxSine);
		x = (int)HIWORD(fxNewX+0x8000L);
		y = (int)HIWORD(fxNewY+0x8000L);
	}

	if (x > 0)
	{
		refresh_bound->x1 += x;
	}
	else
	{
		refresh_bound->x0 += x;
	}
	
	if (y  > 0)
	{
		refresh_bound->y1 += y;
	}
	else
	{
		refresh_bound->y0 += y;
	}

	// just in case fudging
		
	refresh_bound->x0 -= PAGE_RESOLUTION/10;
	refresh_bound->y0 -= PAGE_RESOLUTION/10;
	refresh_bound->x1 += PAGE_RESOLUTION/10;
	refresh_bound->y1 += PAGE_RESOLUTION/10;
}

VOID WarpTextObject::GetBackgroundShadowOffsets(PCOORD* pXOffset, PCOORD* pYOffset)
{
	*pXOffset = 0;
	*pYOffset = 0;
	
	if (m_WRecord.m_fBackgroundShadowColorExists)
	{
		PBOX Bounds = get_unrotated_bound();
		PCOORD x = Bounds.x1-Bounds.x0;
		PCOORD y = Bounds.y1-Bounds.y0;
		
		if (y < x)
		{
			x = y;
		}
		
		*pXOffset = x/30;
		*pYOffset = x/30;
	}
}

VOID WarpTextObject::GetTextShadowOffsets(int nLines, PCOORD* pXOffset, PCOORD* pYOffset)
{
	*pXOffset = 0;
	*pYOffset = 0;
	
	if (m_WRecord.m_fTextShadowColorExists)
	{
		PBOX Bounds = get_unrotated_bound();
		PCOORD x = Bounds.x1-Bounds.x0;
		
		nLines--;
		if (nLines <= 0)
		{
			nLines = 1;
		}
		
		PCOORD y = (Bounds.y1-Bounds.y0)/nLines;
		
		if (y < x)
		{
			x = y;
		}
		
		*pXOffset = x/45;
		*pYOffset = x/45;
	}
}

/*
// Destroy this object.
// The object is being removed from the database.
// Any and all sub-objects must be removed from the database as well.
// This should not FREE anything (in the memory sense); that's the job of the
// destructor.
*/

VOID WarpTextObject::destroy(VOID)
{
	if (m_WRecord.m_WarpField > 0)
	{
		/* Decrement the layout reference. */
		((PMGDatabasePtr)database)->free_warp_field_record(m_WRecord.m_WarpField);
	}

	INHERITED::destroy();
}

/*
// Toggle select marks for an object.
*/

BOOL WarpTextObject::toggle_selects(RedisplayContextPtr rc, FLAGS which)
{
	// Bypass the frame object which does columns and other nonsense.
	return RectPageObject::toggle_selects(rc, which);
}

void WarpTextObject::SetEditingObject(CFrameObject* pEditingObject)
{
	// Set the object for our local remembrance.
	m_pEditingObject = pEditingObject;

	// Set a pointer down in the text record.
	CFrameRecord* pFrame = LockFrameRecord();
	if (pFrame != NULL)
	{
		CTextRecord* pText = pFrame->LockTextRecord();
		if (pText != NULL)
		{
			// Get the editing text record.
			CTextRecord* pEditingText = NULL;
			if (pEditingObject != NULL)
			{
				CFrameRecord* pEditingFrame = pEditingObject->LockFrameRecord();
				if (pEditingFrame != NULL)
				{
					pEditingText = pEditingFrame->LockTextRecord();
					pEditingFrame->release();
				}
			}
			// Set the editing text record.
			pText->SetEditingText(pEditingText);
			pText->release();
		}
		pFrame->release();
	}
}

/*
// Set a new warp field, freeing an old one.
*/

VOID WarpTextObject::set_new_warp_field(DB_RECORD_NUMBER l)
{
	if (m_WRecord.m_WarpField != l)
	{
		// Get rid of the cached warp field.
		delete m_pWarpField;
		m_pWarpField = NULL;

		// Decrement the reference count for the outgoing warp field.
		if (m_WRecord.m_WarpField > 0)
		{
			get_database()->free_warp_field_record(m_WRecord.m_WarpField);
		}

		// Set the new warp field.
		m_WRecord.m_WarpField = l;

		// Add a reference for the new one.
		if (m_WRecord.m_WarpField > 0)
		{
			((PMGDatabasePtr)database)->inc_warp_field_record(m_WRecord.m_WarpField);
		}
	}
}

/*
// Return a pointer to the warp field currently referenced by
// this object.
*/

CWarpField* WarpTextObject::GetWarpField(void)
{
	if (m_pWarpField == NULL)
	{
		if (m_WRecord.m_WarpField > 0)
		{
			WarpFieldRecord* pRecord;
			ERRORCODE error;

			if ((pRecord = (WarpFieldRecord*)database->get_record(m_WRecord.m_WarpField, &error, RECORD_TYPE_WarpField)) != NULL)
			{
				m_pWarpField = new CWarpField;
				m_pWarpField->Name(pRecord->Name());
				m_pWarpField->OriginalBounds()->FromData(*(pRecord->BoundsData()));
				pRecord->release();
			}
		}
	}
	return m_pWarpField;
}

/*
// read_data()
//
// This is a method invoked by the read method to handle object-specific data.
*/

ERRORCODE WarpTextObject::read_data(StorageDevicePtr device)
{
	ERRORCODE error;

	if ((error = INHERITED::read_data(device)) == ERRORCODE_None)
	{
		error = device->read_record(&m_WRecord, sizeof(m_WRecord));
	}
	return error;
}

/*
// write_data()
//
// This is a method invoked by the write method to handle object-specific data.
*/

ERRORCODE WarpTextObject::write_data(StorageDevicePtr device)
{
	ERRORCODE error;

	if ((error = INHERITED::write_data(device)) == ERRORCODE_None)
	{
		error = device->write_record(&m_WRecord, sizeof(m_WRecord));
	}
	return error;
}

/*
// size_data()
//
// This is a method invoked by the size method to handle object-specific data.
*/

ST_MAN_SIZE WarpTextObject::size_data(StorageDevicePtr device)
{
	return INHERITED::size_data(device) + device->size_record(sizeof(m_WRecord));
}

/*
// Assign an object to this.
*/

ERRORCODE WarpTextObject::assign(PageObjectRef pobject)
{
	ERRORCODE error;
	WarpTextObject& sobject = (WarpTextObject&)pobject;

	if ((error = INHERITED::assign(pobject)) == ERRORCODE_None)
	{
	/* Free any old warp field. */
		set_new_warp_field(0);

		m_WRecord = sobject.m_WRecord;

	/*
	// Duplicate the records.
	// Extreme care must be taken to maintain the correct database at all times.
	*/

		if (sobject.m_WRecord.m_WarpField > 0)
		{
			WarpFieldRecord* pRecord;

			if ((pRecord = (WarpFieldRecord*)sobject.database->get_record(sobject.m_WRecord.m_WarpField, &error, RECORD_TYPE_WarpField)) != NULL)
			{
				// Make a create structure for the new record.

				WarpField_CreateStruct CreateStruct;

				CreateStruct.m_pszName = pRecord->Name();
				CreateStruct.m_BoundsData = *(pRecord->BoundsData());

			/* Duplicate the layout into our database. */

				m_WRecord.m_WarpField = get_database()->new_warp_field_record(&CreateStruct);

			/* Release the old one. */

				pRecord->release();
			}
		}
	}
	return error;
}

PageObjectState* WarpTextObject::CreateState(void)
{
	return new WarpTextObjectState;
}

void WarpTextObject::SaveState(PageObjectState* pState)
{
	WarpTextObjectState* pMyState = (WarpTextObjectState*)pState;

	INHERITED::SaveState(pState);

	pMyState->m_WRecord = m_WRecord;
	if (m_WRecord.m_WarpField > 0)
	{
		pMyState->m_pDatabase->inc_warp_field_record(m_WRecord.m_WarpField);
	}
}

void WarpTextObject::RestoreState(PageObjectState* pState)
{
	WarpTextObjectState* pMyState = (WarpTextObjectState*)pState;

	INHERITED::RestoreState(pState);

	DB_RECORD_NUMBER NewWarpField = pMyState->m_WRecord.m_WarpField;

/* Maintain the reference counts on the warp field records. */

	if (NewWarpField != m_WRecord.m_WarpField)
	{
		PMGDatabase* pDatabase = pMyState->m_pDatabase;

	/* In with the new. */
		if (NewWarpField > 0)
		{
			pDatabase->inc_warp_field_record(NewWarpField);
		}
	/* Out with the old. */
		if (m_WRecord.m_WarpField > 0)
		{
			pDatabase->free_warp_field_record(m_WRecord.m_WarpField);
		}
	}
	m_WRecord = pMyState->m_WRecord;
}

/////////////////////////////////////////////////////////////////////////////
// WarpTextObjectState

WarpTextObjectState::WarpTextObjectState()
{
	m_WRecord.m_WarpField = 0;
}

WarpTextObjectState::~WarpTextObjectState()
{
	if (m_WRecord.m_WarpField > 0)
	{
		m_pDatabase->free_warp_field_record(m_WRecord.m_WarpField);
	}
}
