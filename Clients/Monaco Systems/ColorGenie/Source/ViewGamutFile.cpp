#include "stdafx.h"
#include "Message.h"
#include "BuildProgress.h"
#include "Winmisc.h"
#include "mpfileio.h"
#include "ViewGamut.h"
#include "ViewGamutFile.h"

/////////////////////////////////////////////////////////////////////////////
bool ViewGamutFile(void) 
{
	CFileDialog FileDialog(
		/*bOpenFileDialog*/	TRUE,
		/*lpszDefExt*/		"cal",
		/*lpszFileName*/	"",
		/*dwFlags*/			OFN_HIDEREADONLY | OFN_FILEMUSTEXIST | OFN_OVERWRITEPROMPT,
		/*lpszFilter*/		"Color Data Files (*.col;*.txt)|*.col;*.txt|All Files (*.*)|*.*||",
		/*pParentWnd*/		AfxGetMainWnd()
		);

	if (FileDialog.DoModal() != IDOK)
		return false;

	CString dirname((LPCSTR)IDS_DATA_DIRNAME);
	BOOL exist = SetDefaultDir((LPCSTR)dirname);

	AfxGetMainWnd()->BeginWaitCursor();

	// Load patch data
	ProfileDocFiles profiledocfiles;
	profiledocfiles.initPrintData();
	Mpfileio mpfileio(profiledocfiles.printData);
	CString szFileName = FileDialog.GetPathName();
	McoStatus status = mpfileio.loadPatchdata((char*)LPCTSTR(szFileName), 1/*source*/);
	if (status == MCO_SUCCESS)
		status = profiledocfiles.copyOutOfPrintData();

	AfxGetMainWnd()->EndWaitCursor();

	if (status != MCO_SUCCESS)
	{
		Message("Can't load the selected file.");
		return false;
	}

	if( profiledocfiles.simupatchD->copyOutOfPatchData(&profiledocfiles.printData->srcpatch) != MCO_SUCCESS)
	{
		Message("Wrong Source data.");
		return false;
	}

	CString& szDescription = szFileName;
	int c;
	while ((c = szDescription.Find('\\')) >= 0)
		szDescription = szDescription.Mid(c+1);

	McoHandle gamut;
	CBuildProgress buildprogress((long)profiledocfiles.simupatchD, (long)&gamut, Process_GammutSurface, szDescription);
	if (buildprogress.DoModal() == IDCANCEL)
		return false;

	CViewGamut* pViewGamutDialog = new CViewGamut((long)gamut, 1/*source*/, (LPCTSTR)szDescription, AfxGetMainWnd());
	if (!pViewGamutDialog)
		return false;

	pViewGamutDialog->DoModeless(AfxGetMainWnd(), &pViewGamutDialog);
	return true;
}
