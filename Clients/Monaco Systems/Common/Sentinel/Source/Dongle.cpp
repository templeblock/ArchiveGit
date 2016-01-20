#include "Dongle.h"
#include "DongleUpgradeDlg.h"
#include "Message.h"

#define MESSAGE_NO_DONGLE "\
This application requires a Monaco Systems dongle.  \
If you have one, please connect it to the parallel port of your computer, \
and run this application again.  If you do not have one, \
please call and order one from Monaco Systems at 978-749-9944."

#define MESSAGE_CANNOT_READ "\
This application requires a Monaco Systems dongle.  \
The dongle connected to the parallel port of your computer \
does not appear to be programmed by Monaco Systems.  \
Please call and order a new one from Monaco Systems at 978-749-9944."

#define MESSAGE_APP_INACTIVE "\
According to the Monaco Systems dongle connected to the parallel port of your computer, \
this application has expired or is inactive.\r\n\r\n\
Press YES if you wish to activate it.\r\n\
Press NO if you wish to exit the application."

#define MESSAGE_COUNT_DECREMENTED "\
According to the Monaco Systems dongle connected to the parallel port of your computer, \
this application will expire after:\r\n\r\n\
\t%d more use(s).\r\n\r\n\
Press YES to upgrade the application to the full version upgrade now.\r\n\
Press NO to continue running the application."

#define MESSAGE_DAYS_LEFT "\
According to the Monaco Systems dongle connected to the parallel port of your computer, \
this application will expire after:\r\n\r\n\
\t%d more day(s).\r\n\r\n\
Press YES to upgrade the application to the full version upgrade now.\r\n\
Press NO to continue running the application."

#define MESSAGE_PROGRAMMER_ERROR "\
Programmer's error; Rebuild your application after you copy the dialog resource 8000 \
contained in '..\\Common\\Sentinel\\Source\\DongleUpgradeDlg.rc'"

/////////////////////////////////////////////////////////////////////////////
CDongle::CDongle()
{
	m_devID = 0x4779;
	m_writePassword = 0x18E2;
	m_overwritePassword1 = 0xE4EA;
	m_overwritePassword2 = 0x370A;

	for (int i=0; i<MAX_PROTECTED_APP; i++)
	{
		bool bAssigned = (AppNames[i][0] != '\0');
		AppLock* applock = &m_appLock[i];
		applock->number = i;
		applock->assigned = bAssigned;
		applock->active = false;
		applock->versions = 0x0000;
		applock->features = 0x0000;
		applock->date  = 0;
		applock->count = 0;
	}
}

/////////////////////////////////////////////////////////////////////////////
CDongle::~CDongle()
{
}

/////////////////////////////////////////////////////////////////////////////
/*static*/ bool CDongle::IsMonacoPRINT(LPWORD pVersions, LPWORD pFeatures)
{
	CDongle Dongle;
	return Dongle.IsApplication(APP_MonacoPRINT, pVersions, pFeatures);
}	

/////////////////////////////////////////////////////////////////////////////
/*static*/ bool CDongle::IsMonacoSCAN(LPWORD pVersions, LPWORD pFeatures)
{
	CDongle Dongle;
	return Dongle.IsApplication(APP_MonacoSCAN, pVersions, pFeatures);
}	

/////////////////////////////////////////////////////////////////////////////
/*static*/ bool CDongle::IsMonacoVIEW(LPWORD pVersions, LPWORD pFeatures)
{
	CDongle Dongle;
	return Dongle.IsApplication(APP_MonacoVIEW, pVersions, pFeatures);
}	

/////////////////////////////////////////////////////////////////////////////
/*static*/ bool CDongle::IsPrestoProof(LPWORD pVersions, LPWORD pFeatures)
{
	CDongle Dongle;
	return Dongle.IsApplication(APP_PrestoProof, pVersions, pFeatures);
}	

/////////////////////////////////////////////////////////////////////////////
bool CDongle::IsApplication(int which, LPWORD pVersions, LPWORD pFeatures)
{
	WORD Versions;
	if (!pVersions)
		pVersions = &Versions;
	*pVersions = 0;

	WORD Features;
	if (!pFeatures)
		pFeatures = &Features;
	*pFeatures = 0;

	// See if there is a dongle connected
	WORD wSerialNumber;
	if (!GetSerialNumber(&wSerialNumber))
	{
		Message(MESSAGE_NO_DONGLE);
		return false;
	}

	AppLock* applock = GetAppLock(which);
	if (!applock || !applock->assigned)
	{ // This would only occur if the app were not properly defined in the header's tables
		return false;
	}

	// See if the dongle is properly programmed
	if (!ReadApp(applock))
	{
		Message(MESSAGE_CANNOT_READ);
		return false;
	}

	// Pass back some info
	*pVersions = applock->versions;
	*pFeatures = applock->features;

	// If the app is active and not being counted, it is ready to run
	if (applock->active && applock->count >= 100)
		return true;

	// If the dongle has a date count, update it now before checking anything else
	UpdateDateAndCount(applock);

	// Track what we should do if the user bails on upgrading
	bool bOKtoRun = false;

	// If the app is active and IS being counted, it is OK to run,
	// But fix the count (if necessary), tell the user about it, and give them an opportunity to upgrade
	if (applock->active && applock->count > 0 && applock->count < 100)
	{
		if (applock->date)
		{
			if (MessageConfirm(MB_YESNO, MESSAGE_DAYS_LEFT, applock->count) == IDNO)
				return true;

			// the user wants to upgrade, but failure to upgrade still allows the app to run
			bOKtoRun = true;
		}
		else
		{
			applock->count--;
			SetDateAndCount(applock);
			if (MessageConfirm(MB_YESNO, MESSAGE_COUNT_DECREMENTED, applock->count) == IDNO)
				return true;

			// the user wants to upgrade, but failure to upgrade still allows the app to run
			bOKtoRun = true;
		}
	}
	else
	// the app is deactivated or has expired
	//if (!applock->active || !applock->count)
	{
		if (MessageConfirm(MB_YESNO, MESSAGE_APP_INACTIVE) == IDNO)
			return false;

		// the user wants to upgrade, but failure to upgrade will NOT allow the app to run
		bOKtoRun = false;
	}

	// Attempt to upgrade!!!

	CDongleUpgradeDlg dlg(wSerialNumber, which, *pVersions, *pFeatures);
	int nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// Create a Dongle object
		CDongle* pDongle = new CDongle();
		if (!pDongle)
			return bOKtoRun;

		// Check the attached dongle and get the serial number
		unsigned short ssn;
		BOOL hasKey = pDongle->GetSerialNumber(&ssn);
		if (!hasKey)
			return bOKtoRun;

		AppLock* applock = pDongle->GetAppLock(which);
		if (!applock)
			return bOKtoRun;

//		BeginWaitCursor();
		BOOL bOk = pDongle->ReadApp(applock);
		applock->active = true;
		applock->count = 9999; // Unlimited
		applock->date = 0;
		int iVersion = 1;
		applock->versions &= ~(1 << (iVersion-1));

		bOk = pDongle->ProgramApp(applock);
		if (!bOk)
			; //AfxMessageBox(IDS_PROGRAM_ERR);
//		EndWaitCursor();

		// The upgrade is complete
		return true;
	}
	else
	if (nResponse == -1)
		Message(MESSAGE_PROGRAMMER_ERROR);

	return bOKtoRun;
}

/////////////////////////////////////////////////////////////////////////////
void CDongle::UpdateDateAndCount(AppLock* applock)
{
	if (!applock || !applock->active || !applock->date)
		return;

	if (applock->count <= 0 || applock->count >= 100)
		return;

	if (applock->date == 1) // First time user!
		applock->date = GetToday();

	// The root date is 'January 1, 1998 Midnight'
	CTime RootDay( 1998/*y*/, 1/*m*/, 1/*d*/, 0/*hrs*/, 0/*min*/, 0/*sec*/ );
	CTimeSpan StartDays( applock->date, 0/*hrs*/, 0/*min*/, 0/*sec*/);
	CTimeSpan EndDays( applock->date + applock->count, 0/*hrs*/, 0/*min*/, 0/*sec*/);
	CTime Today = CTime::GetCurrentTime();

	if (RootDay + StartDays > Today)
	{ // Someone's been messing with the data; make it all inactive
		applock->active = false;
		applock->count = 0;
		applock->date = 0;
	}
	else
 	if (RootDay + EndDays < Today)
	{ // Time is up; make it all inactive
		applock->active = false;
		applock->count = 0;
		applock->date = 0;
	}
	else
	{ // Update the values
		CTimeSpan Span = (RootDay + EndDays) - Today;
		long lDaysLeft = Span.GetDays() + 1;
		if (lDaysLeft < 0) lDaysLeft = 0;
		if (lDaysLeft > 65535) lDaysLeft = 65535;

		applock->count = (WORD)lDaysLeft;
		applock->date = GetToday();
	}

	SetDateAndCount(applock);
}

/////////////////////////////////////////////////////////////////////////////
WORD CDongle::GetToday()
{
	// The root date is 'January 1, 1998 Midnight'
	CTime RootDay( 1998/*y*/, 1/*m*/, 1/*d*/, 0/*hrs*/, 0/*min*/, 0/*sec*/ );
	CTimeSpan TimeSpan = CTime::GetCurrentTime() - RootDay;

	long lDays = TimeSpan.GetDays();
	if (lDays < 0) lDays = 0;
	if (lDays > 65535) lDays = 65535;
	return (WORD)lDays;
}

/////////////////////////////////////////////////////////////////////////////
SP_STATUS CDongle::InitPacket(RB_SPRO_APIPACKET ApiPacket)
{
	SP_STATUS status;

	status = RNBOsproFormatPacket(ApiPacket, sizeof(RB_SPRO_APIPACKET));
	if (status != SP_SUCCESS) return status;

	status = RNBOsproInitialize(ApiPacket);
	if (status != SP_SUCCESS) return status;

	status = RNBOsproFindFirstUnit(ApiPacket, m_devID);
	if (status != SP_SUCCESS) return status;

	return status;
}

/////////////////////////////////////////////////////////////////////////////
AppLock* CDongle::GetAppLock(int which)
{
	return &m_appLock[which];
}

/////////////////////////////////////////////////////////////////////////////
bool CDongle::GetSerialNumber(WORD* pwSerialNumber)
{
	if (!pwSerialNumber)
		return false;

	*pwSerialNumber = 0;

	SP_STATUS status;
	RB_WORD	data;

	RB_SPRO_APIPACKET apiPacket;
	status = InitPacket(apiPacket);
	if (status != SP_SUCCESS) return false;

	status = RNBOsproRead(apiPacket, 0, &data);
	if (status != SP_SUCCESS) return false;
	
	*pwSerialNumber = data;
	return true;
}

/////////////////////////////////////////////////////////////////////////////
DWORD CDongle::GetAlgorithm(AppLock* applock)
{
	if (!applock)
		return 0;

	int which = applock->number;

	if (applock->active)
		return AppAlgorithmDescriptors[which];
	else
		return AppAlgorithmDescriptors[which] & ~0x00008000;
}

/////////////////////////////////////////////////////////////////////////////
DWORD CDongle::GetPassword(AppLock* applock)
{
	if (!applock)
		return false;

	int which = applock->number;

	return AppAlgorithmPasswords[which];
}

/////////////////////////////////////////////////////////////////////////////
// Dongle SDK helper function
SP_STATUS SP_API RNBOsproQuery32(SP_IN  RBP_SPRO_APIPACKET packet,
                                 SP_IN  RB_WORD            address,
                                 SP_IN  RB_DWORD           Query32,
                                 SP_OUT RBP_DWORD          pResponse32)
{
	char Query[4];
	Query[0] = HIBYTE(HIWORD(Query32));
	Query[1] = LOBYTE(HIWORD(Query32));
	Query[2] = HIBYTE(LOWORD(Query32));
	Query[3] = LOBYTE(LOWORD(Query32));
	char Response[4];
	RB_DWORD Dummy;
	SP_STATUS status = RNBOsproQuery(packet, address, Query, Response, &Dummy, sizeof(DWORD));
	*pResponse32 = MAKELONG(MAKEWORD(Response[3], Response[2]), MAKEWORD(Response[1], Response[0]));
	return status;
}

/////////////////////////////////////////////////////////////////////////////
bool CDongle::ReadApp(AppLock* applock)
{
	if (!applock)
		return false;

	applock->active   = false;
	applock->versions = 0x0000;
	applock->features = 0x0000;
	applock->date	  = 0;
	applock->count	  = 0;
	
	RB_SPRO_APIPACKET apiPacket;
	SP_STATUS status = InitPacket(apiPacket);
	if (status != SP_SUCCESS) return false;

	int which = applock->number;

	//try a query/response pair to verify the algorithm and password
	RB_DWORD response = 0;
	status = RNBOsproQuery32(apiPacket, AppAddress[which]+OFFSET_ALGORITHM1, AppAlgorithmQueries[which], &response);
	if (status != SP_SUCCESS) return false;

	//set the active flag if the algorithm response was correct
	applock->active = (response == AppAlgorithmResponses[which]);
		
	//read versions
	status = RNBOsproRead(apiPacket, AppAddress[which]+OFFSET_VERSIONS, &applock->versions);
	if (status != SP_SUCCESS) return false;

	//read features
	status = RNBOsproRead(apiPacket, AppAddress[which]+OFFSET_FEATURES, &applock->features);
	if (status != SP_SUCCESS) return false;

	//read date
	status = RNBOsproRead(apiPacket, AppAddress[which]+OFFSET_DATE, &applock->date);
	if (status != SP_SUCCESS) return false;

	//read count
	status = RNBOsproRead(apiPacket, AppAddress[which]+OFFSET_COUNT, &applock->count);
	if (status != SP_SUCCESS) return false;

	return true;
}

/////////////////////////////////////////////////////////////////////////////
bool CDongle::ProgramApp(AppLock* applock)
{
	if (!applock)
		return false;

	// When you fully program the app, be sure the date is either 0 or 1
	if (applock->date)
		applock->date = 1;

	RB_SPRO_APIPACKET apiPacket;
	SP_STATUS status = InitPacket(apiPacket);
	if (status != SP_SUCCESS) return false;

	int which = applock->number;

	//program algorithm
	DWORD algorithm = GetAlgorithm(applock);
	WORD algorithm1 = HIWORD(algorithm);
	WORD algorithm2 = LOWORD(algorithm);

	//program algorithm1	
	status = RNBOsproOverwrite(apiPacket, m_writePassword, m_overwritePassword1, m_overwritePassword2,
								AppAddress[which]+OFFSET_ALGORITHM1, algorithm1, 3);
	if (status != SP_SUCCESS) return false;

	//program algorithm2	
	status = RNBOsproOverwrite(apiPacket, m_writePassword, m_overwritePassword1, m_overwritePassword2,
								AppAddress[which]+OFFSET_ALGORITHM2, algorithm2, 3);
	if (status != SP_SUCCESS) return false;

	//program apassword
	DWORD apassword = GetPassword(applock);
	WORD apassword1 = HIWORD(apassword);
	WORD apassword2 = LOWORD(apassword);

	//program apassword1	
	status = RNBOsproOverwrite(apiPacket, m_writePassword, m_overwritePassword1, m_overwritePassword2,
								AppAddress[which]+OFFSET_PASSWORD1, apassword1, 3);
	if (status != SP_SUCCESS) return false;

	//program apassword2	
	status = RNBOsproOverwrite(apiPacket, m_writePassword, m_overwritePassword1, m_overwritePassword2,
								AppAddress[which]+OFFSET_PASSWORD2, apassword2, 3);
	if (status != SP_SUCCESS) return false;

	//program versions
	status = RNBOsproOverwrite(apiPacket, m_writePassword, m_overwritePassword1, m_overwritePassword2,
								AppAddress[which]+OFFSET_VERSIONS, applock->versions, 0);
	if (status != SP_SUCCESS) return false;

	//program features
	status = RNBOsproOverwrite(apiPacket, m_writePassword, m_overwritePassword1, m_overwritePassword2,
								AppAddress[which]+OFFSET_FEATURES, applock->features, 0);
	if (status != SP_SUCCESS) return false;

	//program date
	status = RNBOsproOverwrite(apiPacket, m_writePassword, m_overwritePassword1, m_overwritePassword2,
								AppAddress[which]+OFFSET_DATE, applock->date, 0);
	if (status != SP_SUCCESS) return false;

	//program count	
	status = RNBOsproOverwrite(apiPacket, m_writePassword, m_overwritePassword1, m_overwritePassword2,
								AppAddress[which]+OFFSET_COUNT, applock->count, 0);
	if (status != SP_SUCCESS) return false;

	return true;
}

/////////////////////////////////////////////////////////////////////////////
bool CDongle::SetDateAndCount(AppLock* applock)
{
	if (!applock)
		return false;

	RB_SPRO_APIPACKET apiPacket;
	SP_STATUS status = InitPacket(apiPacket);
	if (status != SP_SUCCESS) return false;

	int which = applock->number;

	// Program a new date
	status = RNBOsproWrite(apiPacket, m_writePassword, AppAddress[which]+OFFSET_DATE, applock->date, 0);
	if (status != SP_SUCCESS) return false;

	// Program a new count	
	status = RNBOsproWrite(apiPacket, m_writePassword, AppAddress[which]+OFFSET_COUNT, applock->count, 0);
	if (status != SP_SUCCESS) return false;

	return true;
}
