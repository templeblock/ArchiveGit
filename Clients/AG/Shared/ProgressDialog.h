#pragma once

#include <shlobj.h>
#include <shlguid.h>

class CProgressDialog
{
public:
	/////////////////////////////////////////////////////////////////////////////
	CProgressDialog()
	{
		m_bStarted = false;
		m_dwDlgFlags = PROGDLG_NORMAL;
		m_dwLastMaxProgress = 0;
		m_u64LastMaxProgress = 0;
		m_pProgressDialog = NULL;
	}

	/////////////////////////////////////////////////////////////////////////////
	~CProgressDialog()
	{
		Destroy();
	}

	/////////////////////////////////////////////////////////////////////////////
	bool Create()
	{
		Destroy();

		HRESULT hr = CoCreateInstance(CLSID_ProgressDialog, NULL, CLSCTX_INPROC_SERVER, IID_IProgressDialog, (void**)&m_pProgressDialog);
		if (FAILED(hr))
			return false;

		HINSTANCE hInstance = ::LoadLibrary("shell32.dll");
		if (hInstance)
		{
			UINT idResource = 160; // 161, 168, 170
			SetAnimation(hInstance, idResource);
		}

		AllowMinimize(true);
		ShowTimeLeft(true);
		ShowProgressBar(true);
		return true;
	}

	/////////////////////////////////////////////////////////////////////////////
	void Destroy()
	{
		EndDialog();

		m_pProgressDialog = NULL;
	}

	/////////////////////////////////////////////////////////////////////////////
	// Closes the progress dialog.
	void EndDialog()
	{
		if (!m_bStarted)
			return;

		m_bStarted = false;

		if (m_pProgressDialog == NULL)
			return;

		HRESULT hr = m_pProgressDialog->StopProgressDialog();
	}

	/////////////////////////////////////////////////////////////////////////////
	// Shows the dialog as a modal window.
	bool ShowModal(HWND hWndParent)
	{
		HRESULT hr = m_pProgressDialog->StartProgressDialog(hWndParent, NULL, m_dwDlgFlags | PROGDLG_MODAL, NULL);
		m_bStarted = SUCCEEDED(hr);
		return m_bStarted;
	}

	/////////////////////////////////////////////////////////////////////////////
	// Shows the dialog as a modeless window.
	bool ShowModeless(HWND hWndParent)
	{
		HRESULT hr = m_pProgressDialog->StartProgressDialog(hWndParent, NULL, m_dwDlgFlags, NULL);
		m_bStarted = SUCCEEDED(hr);
		return m_bStarted;
	}

	/////////////////////////////////////////////////////////////////////////////
	// Sets the text that appears in the dialog caption bar.
	void SetTitle(LPCTSTR szTitle)
	{
		if (!szTitle)
			return;

		USES_CONVERSION;
		HRESULT hr = m_pProgressDialog->SetTitle(T2COLE(szTitle));
	}

	/////////////////////////////////////////////////////////////////////////////
	// Specifies the module that contains an AVI resource, and the resource ID.
	void SetAnimation(HINSTANCE hinst, UINT uRsrcID)
	{
		if (m_pProgressDialog == NULL)
			return;

		HRESULT hr = m_pProgressDialog->SetAnimation(hinst, uRsrcID);
	}

	/////////////////////////////////////////////////////////////////////////////
	// Specifies text that the dialog will display on line 3 if the user clicks the Cancel button.
	void SetCancelMessage(LPCTSTR szMessage)
	{
		if (m_pProgressDialog == NULL)
			return;

		if (!szMessage)
			return;

		USES_CONVERSION;
		HRESULT hr = m_pProgressDialog->SetCancelMsg(T2COLE(szMessage), NULL);
	}

	/////////////////////////////////////////////////////////////////////////////
	// Sets whether the dialog will estimate the time remaining before the maximum progress value is reached.
	// Passing true makes line 3 unavailable to SetLineText().
	void ShowTimeLeft(bool bShow)
	{
		m_dwDlgFlags &= ~(PROGDLG_NOTIME | PROGDLG_AUTOTIME);

		if (bShow)
			m_dwDlgFlags |= PROGDLG_AUTOTIME;
		else
			m_dwDlgFlags |= PROGDLG_NOTIME;
	}

	/////////////////////////////////////////////////////////////////////////////
	// Sets whether the dialog will have a minimize button.
	// NOTE: This is currently broken- the dialog always seems to have a minimize button.
	void AllowMinimize(bool bAllow)
	{
		if (bAllow)
			m_dwDlgFlags &= ~PROGDLG_NOMINIMIZE;
		else
			m_dwDlgFlags |= PROGDLG_NOMINIMIZE;
	}

	/////////////////////////////////////////////////////////////////////////////
	// Sets whether the dialog will have a progress bar.
	void ShowProgressBar(bool bShow)
	{
		if (bShow)
			m_dwDlgFlags &= ~PROGDLG_NOPROGRESSBAR;
		else
			m_dwDlgFlags |= PROGDLG_NOPROGRESSBAR;
	}

	/////////////////////////////////////////////////////////////////////////////
	// Sets one of the three lines of text in the dialog.
	// Parameters:
	//   dwLine: Must be 1, 2, or 3.  The first two lines are above the progress
	//			 bar, and the third line is below the progress bar.  Line 3 is
	//			 unavailable if the dialog is showing the estimated time remaining.
	//   szText: Text to display.
	//   bCompactPath: Pass true if szText is a file name or path.  The dialog will
	//			 shorten the path if necessary to make it fit in the dialog
	void SetLineText(DWORD dwLine, LPCTSTR szText, bool bCompactPath = false)
	{
		if (m_pProgressDialog == NULL)
			return;

		if (!szText)
			return;

		if (dwLine < 1 || dwLine > 3)
			dwLine = 1;

		USES_CONVERSION;
		HRESULT hr = m_pProgressDialog->SetLine(dwLine, T2COLE(szText), bCompactPath, NULL);
	}

	/////////////////////////////////////////////////////////////////////////////
	// Sets the dialog's progress bar.
	// You must call UpdateProgress(DWORD dwProgress, DWORD dwMax) at least once to set the max value.
	void UpdateProgress(DWORD dwProgress, DWORD dwMax)
	{
		if (m_pProgressDialog == NULL)
			return;

		if (!dwMax)
			return;

		m_dwLastMaxProgress = dwMax;
		m_u64LastMaxProgress = 0;

		HRESULT hr = m_pProgressDialog->SetProgress(dwProgress, dwMax);
	}

	/////////////////////////////////////////////////////////////////////////////
	// Sets the dialog's progress bar.
	// You must call UpdateProgress(dwProgress, dwMax) at least once to set the max value.
	void UpdateProgress(DWORD dwProgress)
	{
		if (m_pProgressDialog == NULL)
			return;

		if (!m_dwLastMaxProgress)
			return;

		HRESULT hr = m_pProgressDialog->SetProgress(dwProgress, m_dwLastMaxProgress);
	}

	/////////////////////////////////////////////////////////////////////////////
	// Sets the dialog's progress bar.
	// You must call UpdateProgress(DWORD dwProgress, DWORD dwMax) at least once to set the max value.
	void UpdateProgress64(ULONGLONG u64Progress, ULONGLONG u64ProgressMax)
	{
		if (m_pProgressDialog == NULL)
			return;

		if (!u64ProgressMax)
			return;

		m_dwLastMaxProgress = 0;
		m_u64LastMaxProgress = u64ProgressMax;

		HRESULT hr = m_pProgressDialog->SetProgress64(u64Progress, u64ProgressMax);
	}

	/////////////////////////////////////////////////////////////////////////////
	// Sets the dialog's progress bar.
	// You must call UpdateProgress(DWORD dwProgress, DWORD dwMax) at least once to set the max value.
	void UpdateProgress64(ULONGLONG u64Progress)
	{
		if (m_pProgressDialog == NULL)
			return;

		if (!m_u64LastMaxProgress)
			return;

		HRESULT hr = m_pProgressDialog->SetProgress64(u64Progress, m_u64LastMaxProgress);
	}

	/////////////////////////////////////////////////////////////////////////////
	// Returns true if the user has clicked the Cancel button.
	// You should call this periodically during your processing.
	bool HasUserCanceled()
	{
		if (m_pProgressDialog == NULL)
			return true;

		return !!m_pProgressDialog->HasUserCancelled();
	}

	/////////////////////////////////////////////////////////////////////////////
	// Restarts the dialog's timer that is used to estimate the time remaining.
	// You normally don't have to call this function, since the dialog
	// will gague the time by the time between calls to 
	// IProgressDialog::StartProgressDialog() and IProgressDialog::SetProgress().
	// If you have a long period of inactivity between DoModal/DoModeless and
	// your first UpdateProgress(), then call this function right before the first
	// call to UpdateProgress().
	void ResetTimer()
	{
		if (m_pProgressDialog == NULL)
			return;

		HRESULT hr = m_pProgressDialog->Timer(PDTIMER_RESET, NULL);
	}

	/////////////////////////////////////////////////////////////////////////////
	bool IsCreated() const
	{
		return (m_pProgressDialog != NULL);
	}

protected:
	CComPtr<IProgressDialog> m_pProgressDialog;
	bool m_bStarted;
	DWORD m_dwDlgFlags;
	DWORD m_dwLastMaxProgress;
	ULONGLONG m_u64LastMaxProgress;
};
