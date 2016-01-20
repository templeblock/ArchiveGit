/*
// warptext.cpp : implementation file
//
// $Workfile: WARPTEXT.CPP $
// $Revision: 1 $
// $Date: 3/03/99 6:13p $
//
// Copyright © 1995 MicroLogic Software, Inc.
// All rights reserved.
*/

/*
// Revision History:
//
// $Log: /PM8/App/WARPTEXT.CPP $
// 
// 1     3/03/99 6:13p Gbeddow
// 
//    Rev 1.0   14 Aug 1997 15:27:08   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:41:02   Fred
// Initial revision.
// 
//    Rev 1.9   16 Apr 1997 12:12:32   Jay
// Fixes for text undo
// 
//    Rev 1.8   03 Apr 1997 14:31:30   Jay
// Resource cleanup
// 
//    Rev 1.7   13 Mar 1997 15:17:42   Jay
// Fixed format bar for warp text editing
// 
//    Rev 1.6   28 Oct 1996 13:54:32   Jay
// Moved changes from 3.01.
// 
//    Rev 1.5   28 Oct 1996 10:39:32   johno
// Moved strings to resource file
// 
//    Rev 1.5   25 Oct 1996 10:30:04   Jay
// Default point size is size of base style for title text box.
// 
//    Rev 1.4   10 Sep 1996 09:11:18   Jay
// Sets default point size of frame edit.
// 
//    Rev 1.3   01 Aug 1996 14:40:10   Jay
// Change face now works!
// 
//    Rev 1.2   26 Jun 1996 14:23:14   Jay
//  
// 
//    Rev 1.1   05 Jun 1996 08:22:22   Jay
//  
// 
//    Rev 1.0   14 Mar 1996 13:35:32   Jay
// Initial revision.
// 
//    Rev 1.0   16 Feb 1996 12:31:40   FRED
// Initial revision.
// 
//    Rev 1.26   18 Oct 1995 15:59:52   FRED
// Fixes for 16 bit version after 32 bit changes
// 
//    Rev 1.25   19 Sep 1995 16:41:32   JAY
// Flags the format bar as horizontal.
// 
//    Rev 1.24   06 Sep 1995 08:03:02   JAY
// Now marks macros as modified after InsertField dialog.
// 
//    Rev 1.23   02 Sep 1995 15:06:12   FRED
// More changes to make help work
// 
//    Rev 1.22   29 Aug 1995 13:07:46   JAY
// Now always refreshes the object in case it missed a refresh.
// 
//    Rev 1.21   25 Aug 1995 18:40:38   JAY
// Implemented InsertText.
// 
//    Rev 1.20   25 Aug 1995 15:00:46   JAY
// Insert fields dialog
// 
//    Rev 1.19   24 Aug 1995 14:08:56   JAY
// Fixed to text highlight.
// 
//    Rev 1.18   18 Aug 1995 07:23:26   FRED
// Miscellaneous fixes
// 
//    Rev 1.17   14 Aug 1995 17:18:56   JAY
// Cut/paste/undo from warp text dialog.
// 
//    Rev 1.16   14 Aug 1995 11:44:56   JAY
// Some slightly better stuff with font list.
// 
//    Rev 1.15   09 Aug 1995 13:12:26   JAY
// Misc changes.
// 
//    Rev 1.14   02 Aug 1995 14:56:28   JAY
//  
// 
//    Rev 1.13   02 Aug 1995 12:57:50   JAY
// New insert fill-in fields
// 
//    Rev 1.12   02 Aug 1995 08:08:18   FRED
// Fill-in fields stuff.
// 
//    Rev 1.11   12 May 1995 10:34:34   FRED
// Warp text preview
// 
//    Rev 1.10   08 May 1995 11:59:52   FRED
// Completed switch back to drop down edit box for font.
// 
// 
//    Rev 1.9   05 May 1995 17:10:04   FRED
//  
// 
//    Rev 1.8   03 May 1995 18:09:44   FRED
//  
// 
//    Rev 1.7   02 May 1995 17:09:18   JAY
// Fixed some smart fields bugs.
// 
//    Rev 1.6   02 May 1995 15:39:28   JAY
// More smart fields stuff.
// 
//    Rev 1.5   01 May 1995 15:27:56   JAY
// Restructured the macro stuff
// 
//    Rev 1.4   27 Apr 1995 13:26:54   FRED
//  
// 
//    Rev 1.3   25 Apr 1995 15:24:34   JAY
//  
// 
//    Rev 1.2   25 Apr 1995 09:58:56   JAY
// Get and put warp text.
// 
//    Rev 1.1   25 Apr 1995 09:03:14   JAY
// This is becoming real, now...
// 
//    Rev 1.0   20 Apr 1995 16:16:20   FRED
// Initial revision.
*/

#include "stdafx.h"

#include "pmw.h"
#include "pmwdoc.h"
#include "pmwview.h"
#include "pmgfont.h"
#include "textrec.h"
#include "framedit.h"
#include "warptext.h"
#include "warpobj.h"
#include "macrostd.h"
#include "macrodlg.h"
#include "typeface.h"
#include "tcommand.h"

#include "fmtbar.h"
#include "clip.h"

#include "fielddlg.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

extern Clipboard near clipboard;

/////////////////////////////////////////////////////////////////////////////
// CInsertFieldWarpHelper class
// This is a standard class used to interface a CWarpTextTextPage to the
// Insert Field dialog.

class CInsertFieldWarpHelper : public CInsertFieldHelper
{
public:
	CInsertFieldWarpHelper(CWarpTextTextPage* pPage);

	virtual void InsertMacro(int nMacro);
	virtual void InsertText(CHARACTER* pText, int nSize);
	virtual void ReflowMacros(void);
	virtual CMacroList& MacroList(void);
	virtual void GetMacrosInUse(CWordArray& cwaMacrosInUse);

protected:
	CWarpTextTextPage* m_pPage;
};

CInsertFieldWarpHelper::CInsertFieldWarpHelper(CWarpTextTextPage* pPage)
{
	m_pPage = pPage;
}

void CInsertFieldWarpHelper::InsertMacro(int nMacro)
{
	m_pPage->InsertMacro(nMacro);
}

void CInsertFieldWarpHelper::InsertText(CHARACTER* pText, int nSize)
{
	while (nSize-- > 0)
	{
		m_pPage->GetFrameEdit()->InsertCharacter(*pText++);
	}
}

void CInsertFieldWarpHelper::ReflowMacros(void)
{
	m_pPage->ReflowMacros();
}

CMacroList& CInsertFieldWarpHelper::MacroList(void)
{
/*
// We edit the doc's macro list directly.
*/
	return m_pPage->GetDocument()->GetMacroServer()->MacroList();
}

void CInsertFieldWarpHelper::GetMacrosInUse(CWordArray& cwaMacrosInUse)
{
	m_pPage->GetDocument()->GetMacrosInUse(cwaMacrosInUse);
}

/////////////////////////////////////////////////////////////////////////////
// CWarpTextTextPage property page

CWarpTextTextPage::CWarpTextTextPage(CPmwView* pView)
	: CPmwPropertyPage(CWarpTextTextPage::IDD)
{
	//{{AFX_DATA_INIT(CWarpTextTextPage)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT

	m_pObject = NULL;
	m_pView = pView;
	m_pDoc = pView->GetDocument();
	m_hAccelerators = NULL;
	m_fMacrosChanged = FALSE;
}

CWarpTextTextPage::~CWarpTextTextPage()
{
}

void CWarpTextTextPage::DoDataExchange(CDataExchange* pDX)
{
	CPmwPropertyPage::DoDataExchange(pDX);

	//{{AFX_DATA_MAP(CWarpTextTextPage)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP

	if (pDX->m_bSaveAndValidate)
	{
	}
	else
	{
	}
}

BEGIN_MESSAGE_MAP(CWarpTextTextPage, CPmwPropertyPage)
	//{{AFX_MSG_MAP(CWarpTextTextPage)
	ON_WM_SHOWWINDOW()
	ON_WM_DESTROY()
	ON_BN_CLICKED(ID_VIEW_SMART_FIELDS, OnViewFillInFields)
	ON_BN_CLICKED(ID_INSERT_FIELD, OnInsertField)
	//}}AFX_MSG_MAP
	ON_CBN_KILLFOCUS(IDC_TEXT_TYPEFACE, OnTextTypeface)
	ON_UPDATE_COMMAND_UI(IDC_TEXT_TYPEFACE, OnUpdateTextTypeface)
	ON_COMMAND(ID_TEXT_BOLD, OnTextBold)
	ON_UPDATE_COMMAND_UI(ID_TEXT_BOLD, OnUpdateTextBold)
	ON_COMMAND(ID_TEXT_ITALIC, OnTextItalic)
	ON_UPDATE_COMMAND_UI(ID_TEXT_ITALIC, OnUpdateTextItalic)
	ON_COMMAND(ID_ALIGN_LEFT, OnAlignLeft)
	ON_UPDATE_COMMAND_UI(ID_ALIGN_LEFT, OnUpdateAlignLeft)
	ON_COMMAND(ID_ALIGN_CENTER, OnAlignCenter)
	ON_UPDATE_COMMAND_UI(ID_ALIGN_CENTER, OnUpdateAlignCenter)
	ON_COMMAND(ID_ALIGN_RIGHT, OnAlignRight)
	ON_UPDATE_COMMAND_UI(ID_ALIGN_RIGHT, OnUpdateAlignRight)
	ON_COMMAND(ID_EDIT_COPY, OnEditCopy)
	ON_UPDATE_COMMAND_UI(ID_EDIT_COPY, OnUpdateEditCopy)
	ON_COMMAND(ID_EDIT_CUT, OnEditCut)
	ON_UPDATE_COMMAND_UI(ID_EDIT_CUT, OnUpdateEditCopy)
	ON_COMMAND(ID_EDIT_PASTE, OnEditPaste)
	ON_UPDATE_COMMAND_UI(ID_EDIT_PASTE, OnUpdateEditPaste)
	ON_COMMAND(ID_EDIT_UNDO, OnEditUndo)
	ON_UPDATE_COMMAND_UI(ID_EDIT_UNDO, OnUpdateEditUndo)
	ON_COMMAND(ID_EDIT_REDO, OnEditRedo)
	ON_UPDATE_COMMAND_UI(ID_EDIT_REDO, OnUpdateEditRedo)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWarpTextTextPage message handlers

BOOL CWarpTextTextPage::OnInitDialog()
{
	// Load accelerators.
	
	ASSERT(m_hAccelerators == NULL);
	
	m_hAccelerators = ::LoadAccelerators(AfxGetResourceHandle(), MAKEINTRESOURCE(IDR_FRAMEDIT));
	ASSERT(m_hAccelerators != NULL);

	/*
	// Create a macro list record we can use for our edit session.
	*/

	PMGDatabase* pDatabase = m_pDoc->get_database();

/*
// Hook us into the macro server.
*/

	CStdMacroServer* pMacroServer = m_pDoc->GetMacroServer();
	PMGFontServer* pFontServer = (PMGFontServer*)m_pDoc->get_font_server();

// Save the old list of macros. This is a real list copy.

	m_SaveMacroList = pMacroServer->GetMacroList()->MacroList();

	m_pFrameEdit = (CFrameEdit*)GetDlgItem(IDC_WARP_TEXT_DISPLAY);
	int nDefaultPointSize = pFontServer->m_nDefaultPointSize;
	CTextRecord* pText = m_pObject->LockTextRecord();
	if (pText != NULL)
	{
		CTextStyle Style(m_pDoc->get_database());

		TRY
		{
			pText->GetParagraphStyle(0, Style, FALSE);
			nDefaultPointSize = FixedInteger(Style.Size());
		}
		END_TRY
		pText->release();
	}

	m_pFrameEdit->Init(pMacroServer, nDefaultPointSize);

	CPmwPropertyPage::OnInitDialog();

	m_FormatBar.Create(this,
#ifdef WIN32
							 CBRS_ORIENT_HORZ |
#endif
							 WS_CHILD | WS_VISIBLE,
							 ID_VIEW_FORMATBAR);
	m_FormatBar.LoadBitmap(IDB_EDIT_TEXT_FORMAT);

	m_FormatBar.ShowWarpText();

	m_FormatBar.SetWindowPos(
		NULL,
		8, 25, 600, m_FormatBar.GetDefaultHeight(),
		SWP_NOZORDER|SWP_SHOWWINDOW);
	
	if (m_pObject != NULL)
	{
		m_pFrameEdit->GetTextFrom(m_pObject);
		m_pObject->SetEditingObject(m_pFrameEdit->GetEditFrame());
	}
	
	return TRUE;
}

void CWarpTextTextPage::OnDestroy()
{
	if (m_pObject != NULL)
	{
		m_pObject->SetEditingObject(NULL);
	}

	if (m_fMacrosChanged)
	{
	/*
	// If macros are recorded as changed, we need to copy them back.
	// We also need to reflow the macros in case any changes got through before.
	// Finally, we need to refresh the object because it had stopped responding to
	// updates while being edited.
	*/
		m_pDoc->GetMacroServer()->GetMacroList()->MacroList() = m_SaveMacroList;
		m_pDoc->ReflowMacros();
	}

	// We jhust always do this.
	m_pDoc->refresh_object(m_pObject);

	CPmwPropertyPage::OnDestroy();
}

void CWarpTextTextPage::OnTextTypeface()
{
	if (m_pFrameEdit != NULL)
	{
		CTxp* pTxp = m_pFrameEdit->GetTxp();

		if (pTxp != NULL)
		{
			BOOL fValid = pTxp->Valid();

			if (fValid)
			{
				CComboBox* pTypefaceList = &(m_FormatBar.m_TypefaceBox);

				CString csTypeface;

				pTypefaceList->GetWindowText(csTypeface);

			/* Find the face in the typeface list. */

				int nFace = typeface_server.find_face(csTypeface, FALSE);

				if (nFace != -1)
				{
					PMGFontServer* pFontServer = m_pFrameEdit->GetFontServer();
					DB_RECORD_NUMBER Face = pFontServer->font_face_to_record(nFace);

					if (Face > 0)
					{
					/* We need to do something. */
						CTextStyle NewStyle = pTxp->SomethingSelected()
														? pTxp->SelectStyle()
														: pTxp->Style();
						NewStyle.Font(Face);
						ChangeFont(&NewStyle, IDCmd_TextTypeface);
					}
				}
			}
		}
	}
}

void CWarpTextTextPage::OnUpdateTextTypeface(CCmdUI* pCmdUI)
{
	if (m_pFrameEdit != NULL)
	{
		CTxp* pTxp = m_pFrameEdit->GetTxp();
	
		if (pTxp != NULL)
		{
			char szBuffer[80] = "";

			CComboBox* pTypefaceList = &(m_FormatBar.m_TypefaceBox);
			BOOL fValid = pTxp->Valid();

			if (fValid)
			{
				// Only change the combo box if it doesn't have the focus.
				CWnd* pFocus = GetFocus();
				CWnd* pEdit = pTypefaceList->GetWindow(GW_CHILD);

				if (pFocus != pTypefaceList && pFocus != pEdit)
				{
					CTextStyle& Style = pTxp->SomethingSelected() ? pTxp->SelectStyle() : pTxp->Style();
					DB_RECORD_NUMBER Face = Style.Font();
					
					if (Face > 0)
					{
						// Get the face name to put in the buffer.

						PMGFontServer* pFontServer = m_pFrameEdit->GetFontServer();
		
						int nFace = pFontServer->font_record_to_face(Face);
						if (nFace != -1)
						{
							FaceEntry* pEntry = typeface_server.get_face(nFace);
							if (pEntry != NULL)
							{
								strcpy(szBuffer, pEntry->get_name());
							}
						}
					}
							
					pCmdUI->SetText(szBuffer);
				}
			}
			else
			{
				pCmdUI->SetText(szBuffer);
			}
		
			pCmdUI->Enable(fValid);
		}
	}
}

void CWarpTextTextPage::OnTextBold()
{
	if (m_pFrameEdit != NULL)
	{
		CTxp* pTxp = m_pFrameEdit->GetTxp();
	
		if (pTxp != NULL)
		{
			if (pTxp->Valid())
			{
				CTextStyle NewStyle = pTxp->SomethingSelected()
											? pTxp->SelectStyle()
											: pTxp->Style();
				int nNewBold = (NewStyle.Bold() == 0);
				NewStyle.Bold(nNewBold);
				ChangeFont(&NewStyle, IDCmd_Bold);
			}
		}
	}
}

void CWarpTextTextPage::OnUpdateTextBold(CCmdUI* pCmdUI)
{
	if (m_pFrameEdit != NULL)
	{
		CTxp* pTxp = m_pFrameEdit->GetTxp();
	
		if (pTxp != NULL)
		{
			int nStyle = 0;
			BOOL fValid = pTxp->Valid();
		
			if (fValid)
			{
				CTextStyle& Style = pTxp->SomethingSelected() ? pTxp->SelectStyle() : pTxp->Style();
				nStyle = Style.Bold();
			}
		
			pCmdUI->SetCheck(nStyle == -1 ? 2 : nStyle);
			pCmdUI->Enable(fValid);
		}
	}
}

void CWarpTextTextPage::OnTextItalic()
{
	if (m_pFrameEdit != NULL)
	{
		CTxp* pTxp = m_pFrameEdit->GetTxp();
	
		if (pTxp != NULL)
		{
			if (pTxp->Valid())
			{
				CTextStyle NewStyle = pTxp->SomethingSelected()
											? pTxp->SelectStyle()
											: pTxp->Style();
				int nNewItalic = (NewStyle.Italic() == 0);
				NewStyle.Italic(nNewItalic);
				ChangeFont(&NewStyle, IDCmd_Italic);
			}
		}
	}
}

void CWarpTextTextPage::OnUpdateTextItalic(CCmdUI* pCmdUI)
{
	if (m_pFrameEdit != NULL)
	{
		CTxp* pTxp = m_pFrameEdit->GetTxp();
	
		if (pTxp != NULL)
		{
			int nStyle = 0;
			BOOL fValid = pTxp->Valid();
		
			if (fValid)
			{
				CTextStyle& Style = pTxp->SomethingSelected() ? pTxp->SelectStyle() : pTxp->Style();
				nStyle = Style.Italic();
			}
		
			pCmdUI->SetCheck(nStyle == -1 ? 2 : nStyle);
			pCmdUI->Enable(fValid);
		}
	}
}

void CWarpTextTextPage::OnAlignLeft()
{
	RealignFrame(ALIGN_left);
}

void CWarpTextTextPage::OnUpdateAlignLeft(CCmdUI* pCmdUI)
{
	UpdateHorizontalAlignment(pCmdUI, ALIGN_left);
}

void CWarpTextTextPage::OnAlignCenter()
{
	RealignFrame(ALIGN_center);
}

void CWarpTextTextPage::OnUpdateAlignCenter(CCmdUI* pCmdUI)
{
	UpdateHorizontalAlignment(pCmdUI, ALIGN_center);
}

void CWarpTextTextPage::OnAlignRight()
{
	RealignFrame(ALIGN_right);
}

void CWarpTextTextPage::OnUpdateAlignRight(CCmdUI* pCmdUI)
{
	UpdateHorizontalAlignment(pCmdUI, ALIGN_right);
}

void CWarpTextTextPage::UpdateHorizontalAlignment(CCmdUI* pCmdUI, ALIGN_TYPE nType)
{
	if (m_pFrameEdit != NULL)
	{
		CTxp* pTxp = m_pFrameEdit->GetTxp();

		if (pTxp != NULL)
		{
			BOOL fValid = pTxp->Valid();

			if (pCmdUI->m_pSubMenu != NULL)
			{
				// Handle the menu.
				CMenu *save = pCmdUI->m_pSubMenu;
				pCmdUI->m_pSubMenu = NULL;
				pCmdUI->Enable(fValid);
				pCmdUI->m_pSubMenu = save;
			}
			else
			{
				// Handle an item only.
				pCmdUI->Enable(fValid);
				if (fValid)
				{
					pCmdUI->SetCheck(pTxp->SelectStyle().Alignment() == nType);
				}
			}
		}
	}
}

void CWarpTextTextPage::RealignFrame(ALIGN_TYPE nType)
{
#if 0
	if (m_pFrameEdit != NULL)
	{
		CTxp* pTxp = m_pFrameEdit->GetTxp();
	/* Create the undo/redo command. */

		CCmdHAlignText* pCommand = new CCmdHAlignText(nType);

		if (pCommand->Snapshot(pTxp, NULL) && pCommand->DoIt())
		{
		/* Add and execute the command. */
			m_pFrameEdit->GetCommandList()->AddCommand(pCommand);
		}
		else
		{
			delete pCommand;
		}
	}
#endif
}

void CWarpTextTextPage::ChangeFont(CTextStyle* pStyle, WORD wCmdID)
{
	CTextRecord* pText = NULL;

	TRY
	{
		if (m_pFrameEdit != NULL)
		{
			CTxp* pTxp = m_pFrameEdit->GetTxp();
		
			if (pTxp != NULL)
			{
				PMGDatabase* pDatabase = m_pFrameEdit->GetDatabase();

				if (pTxp->SomethingSelected() /*|| pTxp->LastOffsetInText() == 0*/)
				{
#if 0
					CTextCommand* pCommand = NULL;

               if (pTxp->SomethingSelected())
					{
						pCommand = new CTextCommand(IDCmd_ChangeFont);
						if (!pCommand->Before(pTxp, NULL))
						{
							delete pCommand;
							pCommand = NULL;
						}
					}
#endif

					// Change whole range.
					CHARACTER_INDEX lStart, lEnd;
					pTxp->GetRange(&lStart, &lEnd);

					pTxp->Off();

					pText = pTxp->LockText();
					ASSERT(pText != NULL);

					if (pText->ChangeRangeStyle(lStart, lEnd, pStyle) > 0)
					{
						// Finish by showing the changed style.
						pTxp->UpdateRange();
					}
					pText->release();
					pText = NULL;

				/* Finish the undo/redo command, if there is one. */

#if 0
               if (pCommand != NULL)
					{
						if (pCommand->After())
						{
							m_pFrameEdit->GetCommandList()->AddCommand(pCommand);
						}
						else
						{
							delete pCommand;
						}
					}
#endif

					pTxp->SetTransition();
					pTxp->On();
				}
				else
				{
					pTxp->Style() = *pStyle;
				}
			}
		}
	}
	END_TRY

	if (pText != NULL)
	{
		pText->release();
	}
}

void CWarpTextTextPage::GetText(void)
{
	CTextRecord* pText = NULL;
	TRY
	{
		if ((m_pObject != NULL) && (m_pFrameEdit != NULL))
		{
			// Strip off trailing new lines from text. If the users REALLY wants
			// blank lines they can use blanks.
			
			CTxp* pTxp = m_pFrameEdit->GetTxp();
		
			if (pTxp != NULL)
			{
				pText = pTxp->LockText();
				
				CHARACTER_COUNT lCharacters = pText->NumberOfCharacters();
					
				if (lCharacters > 0)
				{
					CHARACTER_COUNT lBreaks = 0;
					while (lCharacters > 0 && *(pText->GetCharacter(lCharacters-1)) == '\n')
					{
						lBreaks++;
						lCharacters--;
					}

					if (lBreaks > 0)
					{
						// Put confirm here if you want.
						pTxp->Eot(FALSE);
						pTxp->Backspace(lBreaks);
					}
					pText->release();
					pText = NULL;
				}
			}

			m_pFrameEdit->SetTextTo(m_pObject);
		}
	}
	END_TRY
	if (pText != NULL)
	{
		pText->release();
	}

/*
// We assume that the dialog is going away.
// Update the document if the macros have changed.
*/

	if (m_fMacrosChanged)
	{
	/* We need to reflow the macros. */
		m_pDoc->ReflowMacros();
		m_fMacrosChanged = FALSE;
	}
}

void CWarpTextTextPage::OnViewFillInFields()
{
	CSmartFieldsDialog Dialog(m_pView, GetParent());

	// Set up our reflow hook.
	Dialog.SetReflowHook(ReflowHook, this);

	Dialog.DoModal();

	m_pFrameEdit->SetFocus();
}

void CWarpTextTextPage::OnInsertField()
{
	CInsertFieldWarpHelper Helper(this);
	CInsertFieldDialog Dialog(&Helper);

	Dialog.DoModal();

	// Assume something changed.
	m_pDoc->GetMacroServer()->ModifiedMacros();
}

void CWarpTextTextPage::ReflowHook(LPVOID pData)
{
	CWarpTextTextPage* pThis = (CWarpTextTextPage*)pData;

	pThis->m_pFrameEdit->ReflowMacros();

// Remember we need to flow the document later.

	pThis->m_fMacrosChanged = TRUE;
}

/***************************/
/* Cut and paste routines. */
/***************************/

/*
// Perform the copy command.
*/

void CWarpTextTextPage::OnEditCopy()
{
#if 0
   if (m_pFrameEdit != NULL)
	{
	/* Do the text cut. */

		clipboard.CopyText(m_pFrameEdit->GetTxp(), NULL);
	}
#endif
}

/*
// Perform the cut command.
*/

void CWarpTextTextPage::OnEditCut()
{
#if 0
	if (m_pFrameEdit != NULL)
	{
	/* Do the text cut. */

		clipboard.CutText(m_pFrameEdit->GetTxp(), NULL, m_pFrameEdit->GetCommandList());
	}
#endif
}

/*
// Perform the paste command.
*/

void CWarpTextTextPage::OnEditPaste()
{
#if 0
	if (m_pFrameEdit != NULL)
	{
	/* Do the text copy. */
		if (clipboard.PasteText(m_pFrameEdit->GetTxp(), NULL, m_pFrameEdit->GetCommandList(), TRUE) == ERRORCODE_Full)
		{
			AfxMessageBox(IDS_TITLE_BOX_FULL, MB_OK | MB_ICONEXCLAMATION);
		}
	}
#endif
}

/*
// Update the copy command status.
*/

void CWarpTextTextPage::OnUpdateEditCopy(CCmdUI* pCmdUI)
{
	BOOL fCanCopy = FALSE;

	if (m_pFrameEdit != NULL)
	{
	/* Editing text! */
		fCanCopy = m_pFrameEdit->GetTxp()->SomethingSelected();
	}
	pCmdUI->Enable(fCanCopy);
}

/*
// Update the paste command status.
*/

void CWarpTextTextPage::OnUpdateEditPaste(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(clipboard.CanPasteText());
}

void CWarpTextTextPage::OnEditUndo()
{
	if (m_pFrameEdit != NULL)
	{
		m_pFrameEdit->GetCommandList()->Undo();
	}
}

void CWarpTextTextPage::OnUpdateEditUndo(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(m_pFrameEdit != NULL && m_pFrameEdit->GetCommandList()->CanUndo());
}

void CWarpTextTextPage::OnEditRedo()
{
	if (m_pFrameEdit != NULL)
	{
		m_pFrameEdit->GetCommandList()->Redo();
	}
}

void CWarpTextTextPage::OnUpdateEditRedo(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(m_pFrameEdit != NULL && m_pFrameEdit->GetCommandList()->CanRedo());
}

BOOL CWarpTextTextPage::PreTranslateMessage(MSG* pMsg)
{
	if (m_hAccelerators != NULL)
	{
		if (TranslateAccelerator(GetSafeHwnd(), m_hAccelerators, pMsg))
		{
			TRACE("Translated Accelerator\n");
			return TRUE;
		}
	}

	return FALSE;
}

void CWarpTextTextPage::InsertMacro(int nMacro)
{
	CHARACTER c = m_pDoc->GetMacroServer()->GetMacroCharacter(nMacro);

	m_pFrameEdit->InsertCharacter(c);
}

void CWarpTextTextPage::GetMacrosInUse(CWordArray& cwaMacrosInUse)
{
/*
// We can just ask the document for the macros in use.
// It will route the request through our edit frame (at the low warp object
// level).
// This is safe even though some macros in the edit frame may not be in the
// doc's list (since the routine doesn't really do anything with the found
// values - it just records them).
*/
	m_pDoc->GetMacrosInUse(cwaMacrosInUse);
}

void CWarpTextTextPage::ReflowMacros(void)
{
	ReflowHook((LPVOID)this);
}
