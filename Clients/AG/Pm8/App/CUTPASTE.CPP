// cutpaste.cpp : Glue code between the MFC objects and the Clipboard object.
//

#include "stdafx.h"
#include "pmw.h"

#include "mainfrm.h"
#include "pmwdoc.h"
#include "pmwview.h"
#include "clip.h"

/**********************************/
/* CMainFrame clipboard routines. */
/**********************************/

void CMainFrame::OnDestroyClipboard()
{
	clipboard.DestroyContents();

/* Do we need this? */

	CMDIFrameWnd::OnDestroyClipboard();
}

/*
// This is the WM_DRAWCLIPBOARD handler which tells us that the clipboard
// has had its contents changed.
*/

void CMainFrame::OnDrawClipboard()
{
/* Update what we think is in the clipboard. */

	clipboard.UpdateContents(GetSafeHwnd());

	CMDIFrameWnd::OnDrawClipboard();

	if (m_hwndNextViewer != NULL)
	{
	/* Pass it on. */
		::SendMessage(m_hwndNextViewer, WM_DRAWCLIPBOARD, 0, 0);
	}
}

void CMainFrame::OnRenderAllFormats()
{
	clipboard.RenderAllFormats();
	CMDIFrameWnd::OnRenderAllFormats();
}

void CMainFrame::OnRenderFormat(UINT nFormat)
{
	clipboard.RenderFormat(nFormat);
	CMDIFrameWnd::OnRenderFormat(nFormat);
}

/************************************/
/* CPmwView cut and paste routines. */
/************************************/

/*
// Perform the copy command.
*/

void CPmwView::OnEditCopy()
{
	CPmwDoc* pDoc = GetDocument();

	if (m_Txp.Valid())
	{
	/* Do the text copy. */
		clipboard.CopyText(this);
	}
	else
	{
		clipboard.CopyObjects(pDoc);
	}
}

/*
// Perform the cut command.
*/

void CPmwView::OnEditCut()
{
	CPmwDoc* pDoc = GetDocument();

	if (m_Txp.Valid())
	{
	/* Do the text cut. */
		clipboard.CutText(this);
		pDoc->SetModifiedFlag();
		pDoc->resize_project();
	}
	else
	{
		clipboard.CutObjects(pDoc);
	}
}

/*
// Perform the paste command.
*/

void CPmwView::OnEditPaste()
{
	CPmwDoc* pDoc = GetDocument();

	if (m_Txp.Valid())
	{
	/* Do the text copy. */
		if (clipboard.PasteText(this) == ERRORCODE_Full)
		{
			AfxMessageBox(IDS_TEXT_BOX_FULL, MB_OK | MB_ICONEXCLAMATION);
		}
		pDoc->SetModifiedFlag();
		pDoc->resize_project();
	}
	else
	{
		clipboard.PasteObjects(this);
	}
}

/*
// Update the copy command status.
*/

void CPmwView::OnUpdateEditCopy(CCmdUI* pCmdUI)
{
	BOOL can_copy = FALSE;

	if (m_Txp.Valid())
	{
	/* Editing text! */
		can_copy = m_Txp.SomethingSelected();
	}
	else
	{
		CPmwDoc* pDoc = GetDocument();
		can_copy = pDoc->selected_objects() != NULL
						&& !pDoc->IsGroupedFrame(pDoc->selected_objects());
	}
	pCmdUI->Enable(can_copy);
}

/*
// Update the cut command status.
*/

void CPmwView::OnUpdateEditCut(CCmdUI* pCmdUI)
{
	BOOL can_cut = FALSE;

	if (m_Txp.Valid())
	{
	/* Editing text! */
		can_cut = m_Txp.SomethingSelected();
	}
	else
	{
		CPmwDoc* pDoc = GetDocument();
		can_cut = pDoc->selected_objects() != NULL
						&& !pDoc->IsGroupedFrame(pDoc->selected_objects());
	}
	pCmdUI->Enable(can_cut);
}

/*
// Update the paste command status.
*/

void CPmwView::OnUpdateEditPaste(CCmdUI* pCmdUI)
{
	BOOL has_data = FALSE;

	if (m_Txp.Valid())
	{
		has_data = clipboard.CanPasteText();
	}
	else
	{
		has_data = clipboard.CanPasteObjects();
	}
	pCmdUI->Enable(has_data);
}
