/*	$Header: /PM8/App/AnimationPreviewDlg.cpp 1     3/03/99 6:03p Gbeddow $
//
//	Implementation of the CPAnimationPreviewDlg class.
//
//	This class provides a wrapper for the GIF animation preview dialog.
//	
//	(c)Copyright 1999 The Learning Company, Inc., all rights reserved
//
// $Log: /PM8/App/AnimationPreviewDlg.cpp $
// 
// 1     3/03/99 6:03p Gbeddow
// 
// 1     1/26/99 3:24p Rgrenfell
// New dialog class for use in animation previews.
*/

#include "stdafx.h"
#include "resource.h"
#include "AnimationPreviewDlg.h"
#include "GifAnimator.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

enum
{
	AP_ANIMATION_COMPLETE = 0,
	AP_ANIMATION_CONTINUES
};

/////////////////////////////////////////////////////////////////////////////
// CPAnimationPreviewDlg dialog

/* Class constructor.
*/
CPAnimationPreviewDlg::CPAnimationPreviewDlg(
	CPGIFAnimator *pGIFAnimator,	// The GIF animator to be used for the preview.
	CWnd* pParent /*=NULL*/)		// The parent window if specified.
	: CDialog(CPAnimationPreviewDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CPAnimationPreviewDlg)
	//}}AFX_DATA_INIT

	m_pGIFAnimator = pGIFAnimator;
	m_bUpdating = FALSE;
	m_nTimer = 0;
}


void CPAnimationPreviewDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPAnimationPreviewDlg)
	DDX_Control(pDX, IDC_PREVIEW_FRAME, m_ctlPreviewFrame);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPAnimationPreviewDlg, CDialog)
	//{{AFX_MSG_MAP(CPAnimationPreviewDlg)
	ON_WM_TIMER()
	ON_WM_DRAWITEM()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPAnimationPreviewDlg message handlers

/* Handle the cancel button and clean up the timer.
*/
void CPAnimationPreviewDlg::OnCancel()
{
	if (m_nTimer != 0)
	{
		KillTimer( m_nTimer );
		m_nTimer = 0;
	}

	CDialog::OnCancel();
}

/* Handle the timer events so that the animation renders.
*/
void CPAnimationPreviewDlg::OnTimer(UINT nIDEvent)
{
	if (IsWindowVisible())
	{
		if (!m_bUpdating && m_pGIFAnimator->NeedsUpdate())
		{
			m_ctlPreviewFrame.RedrawWindow( NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW );
		}
	}
	else
	{
		CDialog::OnTimer( nIDEvent );
	}
}


/* Initialize the dialog.  We need to start a timer here so that we have a
	regularly triggered event which we can use to control the animation.
*/
BOOL CPAnimationPreviewDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	
	// Determine the image size so we can center it in our viewing area.
	CSize sizeImage = m_pGIFAnimator->GetGIFScreenSize();
	m_ctlPreviewFrame.GetWindowRect( &m_rcViewingArea );

	// Fix our rectangle to be in client coordinates within the dialog.
	ScreenToClient(&m_rcViewingArea);

	// Buffer the viewing area space.  We want a couple pixel border.
	m_rcViewingArea.DeflateRect( 1, 1 );
	int nWidth, nHeight;
	if ((m_rcViewingArea.Width() < sizeImage.cx) || (m_rcViewingArea.Height() < sizeImage.cy))
	{
		// Ensure we have legal x and y values (otherwise we can get division by zero errors below.
		if (sizeImage.cx == 0)
		{
			sizeImage.cx = 1;
		}
		if (sizeImage.cy == 0)
		{
			sizeImage.cy = 1;
		}

		// Scale to fit in the box
		nHeight = m_rcViewingArea.Height();
		if ((nWidth = ((nHeight * sizeImage.cx) / sizeImage.cy)) > m_rcViewingArea.Width())
		{
			nWidth = m_rcViewingArea.Width();
			nHeight = (nWidth * sizeImage.cy) / sizeImage.cx;
		}
	}
	else
	{
		nWidth = sizeImage.cx;
		nHeight = sizeImage.cy;
	}

	// Adjust the rect based on the width and heights determined above.
	m_rcViewingArea.DeflateRect(	(m_rcViewingArea.Width() - nWidth) / 2,
											(m_rcViewingArea.Height() - nHeight) / 2 );

	m_ctlPreviewFrame.SetWindowPos( NULL, m_rcViewingArea.left, m_rcViewingArea.top,
												m_rcViewingArea.Width(), m_rcViewingArea.Height(),
												SWP_NOZORDER | SWP_NOREDRAW );

	// Get the new viewing area coordinates in client space.
	m_ctlPreviewFrame.GetClientRect( &m_rcViewingArea );

	// Start the timer to play the animation.
	m_nTimer = SetTimer(GIF_PREVIEW_TIMER, 10, NULL);	// GIF animations have delays defined in 100ths of seconds.

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

/* Render the frame for the preview region.
*/
void CPAnimationPreviewDlg::OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	// Only wory about the preview frame for localized rendering.
	if (nIDCtl == IDC_PREVIEW_FRAME)
	{
		// Update the animation.
		m_bUpdating = TRUE;
		if ( Update( lpDrawItemStruct ) == AP_ANIMATION_COMPLETE )
		{
			// There was a loop count and the animation is over, so stop the timer.
			KillTimer( m_nTimer );
			m_nTimer = 0;
		}
		m_bUpdating = FALSE;
	}
	else
	{
		CDialog::OnDrawItem(nIDCtl, lpDrawItemStruct);
	}
}


/* This method renders the appropriate frame of the attached animated GIF if
	present.  If sufficient time has not yet gone by to warrent drawing the next
	frame of the animation, this will re-render the current frame.

	Returns : AP_ANIMATION_COMPLETE if the final frame has been rendered and the
				 loop count has expired.
				 AP_ANIMATION_CONTINUES if there are further frames to be rendered.
*/
UINT CPAnimationPreviewDlg::Update(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	// If we are totally clipped, skip it.
	RECT r;
	if (GetClipBox(lpDrawItemStruct->hDC, &r) == NULLREGION)
	{
		return AP_ANIMATION_CONTINUES;	// We can continue, we just don't need an update.
	}

	// Create a CDC from the draw item structure that the GIF animator can use
	// to render the current frame for us.
	CDC dcView;
	dcView.Attach( lpDrawItemStruct->hDC );

	// Have the GIF animator render the current frame.
	UINT uContinuationCode;
	if (m_pGIFAnimator->Update( &dcView, m_rcViewingArea ))
	{
		uContinuationCode = AP_ANIMATION_CONTINUES;
	}
	else
	{
		uContinuationCode = AP_ANIMATION_COMPLETE;
	}

	// Release the Device.
	dcView.Detach();

	return uContinuationCode;
}
