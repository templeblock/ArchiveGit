// ProgStat.cpp : implementation file
//

#include "stdafx.h"
#include "pmw.h"
#include "ProgStat.h"
#include "util.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDownloadStatusDialog dialog

CDownloadStatusDialog::CDownloadStatusDialog()
{
	//{{AFX_DATA_INIT(CDownloadStatusDialog)
	m_strProgress = _T("");
	//}}AFX_DATA_INIT

	m_fAbort = FALSE;
	m_dwCreateTime = ::GetTickCount();

	m_ulStartTime = 0;
	m_dTimeLeftLast = 0; //xffffffff;
}

void CDownloadStatusDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDownloadStatusDialog)
	DDX_Control(pDX, IDC_PROGRESSBAR, m_ctrlProgressBar);
	DDX_Control(pDX, IDC_PROGRESS, m_staProgress);
	DDX_Text(pDX, IDC_PROGRESS, m_strProgress);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CDownloadStatusDialog, CDialog)
	//{{AFX_MSG_MAP(CDownloadStatusDialog)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDownloadStatusDialog message handlers

void CDownloadStatusDialog::OnCancel() 
{
	m_fAbort = TRUE;
}

BOOL CDownloadStatusDialog::OnInitDialog() 
{
	CDialog::OnInitDialog();

	// Set the range.
	m_ctrlProgressBar.SetRange(0, 100);
	CenterWindow();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDownloadStatusDialog::Show(void)
{
	if (!IsWindowVisible())
	{
		GetParent()->EnableWindow(FALSE);
		ShowWindow(SW_SHOW);
		CenterWindow();
		UpdateWindow();
	}
}

void CDownloadStatusDialog::Hide(void)
{
	if (IsWindowVisible())
	{
		GetParent()->EnableWindow(TRUE);
		ShowWindow(SW_HIDE);
	}
}

// Status management.
void CDownloadStatusDialog::SetStatusText(LPCSTR pszText /*=NULL*/)
{
	Util::YieldToWindows(); // Allow windows message processing

	ShowIfTime();

	if (pszText == NULL)
	{
		pszText = "";
	}

	// Set the text.
	m_staProgress.SetWindowText(pszText);

	// Make sure it is updated.
	m_staProgress.UpdateWindow();
}

// Show if it has been a while.
void CDownloadStatusDialog::ShowIfTime(void)
{
	if (!IsWindowVisible())
	{
		DWORD dwCurrentTime = GetTickCount();
		if (dwCurrentTime - m_dwCreateTime > 2*1000)
		{
			// We are going to be here a while. Show the window.
			Show();
		}
	}
}

void CDownloadStatusDialog::SetTimeProgress(LPCSTR szProgress)
{ 
	m_staProgress.SetWindowText(szProgress); 
}

void CDownloadStatusDialog::OnProgress(ULONG ulCurrent, ULONG ulProgressMax)
{
	if (m_fAbort || ulProgressMax <= 0)
	{
		return;
	}

	ShowIfTime();
	m_ctrlProgressBar.SetPos(MulDiv(ulCurrent, 100, ulProgressMax));

	if (ulCurrent > 0)
	{
		ULONG ulTimeNow = timeGetTime(); //in milliseconds
		double dRate = 0; // in Bytes/millisecond

		if ((m_ulStartTime > 0) && ((ulTimeNow - m_ulStartTime) > 0))
		{
			//note: do NOT use instantaneous rate - stick with avg. rate
			dRate = ((double)ulCurrent / (double)(ulTimeNow - m_ulStartTime));
		}
		else 
		{
			dRate = 4.0;  //make 28.8 kb/s (4 bytes / ms) the standard
		}

		double dTimeLeft = 0; // in seconds
		if (dRate > 0)
		{
			if (dTimeLeft <= m_dTimeLeftLast)
			{
				dTimeLeft = (((double)(ulProgressMax - ulCurrent) / dRate)) / 1000.0;
			}
			else
			{
				dTimeLeft = m_dTimeLeftLast;
			}
		}

		double dCopied = ulCurrent/1024;		// in KiloBytes

		CString csText;
		if (dTimeLeft == 0.0)
		{
			// Leave it blank!
		}
		else if (dTimeLeft < 60) // less than a minute
		{
			csText.Format("Estimated time left: %0.1f sec ( %0.2f KB copied )",
							  dTimeLeft,
							  dCopied);
		}
		else if (dTimeLeft < 3600) // less than an hour
		{
			csText.Format("Estimated time left: %d min %d sec ( %0.2f KB copied )",
							  (int)dTimeLeft / 60,
							  (int)dTimeLeft % 60,
							  dCopied);
		}
		else // more than an hour
		{
			csText.Format("Estimated time left: %d hr %d min %d sec ( %0.2f KB copied )",
							  (int)dTimeLeft / 3600,
							  (((int)dTimeLeft % 3600) / 60),
							  (((int)dTimeLeft % 3600) % 60),
							  dCopied);
		}

		SetTimeProgress(csText);

		m_dTimeLeftLast = dTimeLeft;
	}
}