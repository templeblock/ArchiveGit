#pragma	once
#include <wininet.h>
#pragma comment(lib, "wininet.lib")
#include <atlcomtime.h>

// Usually the best way to determine if you have a connection to a particular computer is to attempt the connection.
// If the autodial feature of Windows is enabled, then attempting the connection may cause the default Internet dialup
// connectoid to be opened, and you will be prompted with your credentials to connect.
//
// To avoid having the default Internet connectoid dialed, the InternetGetConnectedState function can be used to
// determine if there is a default Internet dialup connectoid configured and whether it is currently active or not.
// If there is a default Internet dialup connectoid configured, and it is not currently active, then InternetGetConnectedState
// will return FALSE. If InternetGetConnectedState returns TRUE then you can attempt to connect to the Internet resource
// without fear of being prompted to connect to another Internet Service Provider.
//
// You cannot rely solely on the fact that InternetGetConnectedState returning TRUE means that you have a valid active
// Internet connection. It is impossible for InternetGetConnectedState to determine if the entire connection to the
// Internet is functioning without sending a request to a server. This is why you need to send a request to determine
// if you are really connected or not. You can be assured however that if InternetGetConnectedState returns TRUE, that
// attempting your connection will NOT cause you to be prompted to connect to the default Internet Service Provider.
//
// Be aware that InternetGetConnectedState only reports the status of the default Internet connectoid on IE 4.x.
// If a nondefault connectoid is connected, InternetGetConnectedState always returns FALSE (unless a LAN connection is
// used). With IE 4.x configured to use a LAN connection, InternetGetCo nectedState always returns TRUE.
//
// IE 5 behaves differently. If you are currently dialed into a different dial-up in IE 5, InternetGetConnectedState
// reports dial-up connection status as long as any connectoid is dialed or an active LAN connection exists. 
//
// There are some other ways to try to determine if you currently have a connection to a particular network resource.
// The IsDestinationReachable() function can be used to find out if there is a current connection to an address.
// However, he IsDestinationReachable() function is only concerned with whether the IP address is reachable from your
// computer. It does not work through HTTP proxies or firewalls that restrict ICMP ping packets.
//
// It is also possible to use RasEnumConnections to enhance your code so that you can tell if there is an active dialup
// connection that might have Internet access even though it is not the default Internet dialup connectoid. 

// InternetAutodial() Initiates an unattended dial-up connection. 
// InternetAutodialHangup() Disconnects a modem connection initiated by InternetAutodial. 
// InternetDial() Initiates a dial-up connection. 
// InternetHangUp() Disconnects a modem connection initiated by InternetDial. 
// InternetGetConnectedState() Retrieves the current state of the Internet connection. 
// InternetGoOnline() Prompts the user for permission to initiate a dial-up connection to the given URL. 
// InternetSetDialState() Sets the current state of the Internet connection. 

//j Example usage of this class:
//j	CInternet Internet;
//j	bool bOK;
//j	CString	str = Internet.GetConnectionName();
//j	bOK = Internet.GetIsConnectionConfigured();
//j	bOK = Internet.GetIsOnline();
//j	bOK = Internet.GetIsRasInstalled();
//j	bOK = Internet.GetUseProxy();
//j	bOK = Internet.GetUseLAN();
//j	bOK = Internet.GetUseModem();
//j	bOK = Internet.SetOnline(false);
//j	HWND hwndParentWindow = NULL;
//j	bOK = Internet.GoOnline("http://www.ag.com", hwndParentWindow);
//j	DialsFlags lDialOptions = DF_FORCE_ONLINE;
//j	bOK = Internet.Dial(hwndParentWindow, "test"/*strConnectionName*/, lDialOptions, false/*bShowOfflineButton*/);
//j	bOK = Internet.HangUp();
//j	AutoDialsFlags lAutoDialOptions = ADF_FORCE_ONLINE;
//j	bOK = Internet.Autodial(hwndParentWindow, lAutoDialOptions, true/*bFailIfSecurityCheck*/);
//j	bOK = Internet.AutodialHangup();
//j
//j	CString strCookie;
//j	bOK = Internet.SetCookie("http://www.facetofacesoftware.com", "value=1", 8, 23, 2003);
//j	bOK = Internet.SetCookie("http://www.facetofacesoftware.com", "name=jim", 8, 23, 2003);
//j	bOK = Internet.GetCookie("http://www.facetofacesoftware.com", "name", strCookie);
//j	bOK = Internet.GetCookie("http://www.facetofacesoftware.com", NULL, strCookie);

enum AutoDialsFlags
{
	ADF_FORCE_ONLINE = INTERNET_AUTODIAL_FORCE_ONLINE,			// Forces an online connection
	ADF_FORCE_UNATTENDED = INTERNET_AUTODIAL_FORCE_UNATTENDED,	// Forces an unattended Internet dial-up. If user intervention is required, the function will fail
};

enum DialsFlags
{
	DF_FORCE_ONLINE = INTERNET_AUTODIAL_FORCE_ONLINE,			// Forces an online connection
	DF_FORCE_UNATTENDED = INTERNET_AUTODIAL_FORCE_UNATTENDED,	// Forces an unattended Internet dial-up. If user intervention is required, the function will fail
	DF_DIAL_FORCE_PROMPT = INTERNET_DIAL_FORCE_PROMPT,			// Ignores the "dial automatically" setting and forces the dialing user interface to be displayed
	DF_DIAL_UNATTENDED = INTERNET_DIAL_UNATTENDED,				// Connects to the Internet through a modem, without displaying a user interface, if possible. Otherwise, the function will wait for user input
	DF_DIAL_SHOW_OFFLINE = INTERNET_DIAL_SHOW_OFFLINE,			// Shows the Work Offline button instead of the Cancel button in the dialing user interface
};

class CInternet
{
public:
	/////////////////////////////////////////////////////////////////////////////
	CInternet()
	{
		ClearConnectedState();
	}
	
	/////////////////////////////////////////////////////////////////////////////
	bool SetOnline(bool bOn)
	{
		m_bStateIsUpToDate = false;

		// Turns the local system to online mode
		INTERNET_CONNECTED_INFO	ConInfo;
		if (bOn)
		{
			ConInfo.dwConnectedState = INTERNET_STATE_CONNECTED;
			ConInfo.dwFlags = 0;
		}
		else
		{
			ConInfo.dwConnectedState = INTERNET_STATE_DISCONNECTED_BY_USER;
			ConInfo.dwFlags = ISO_FORCE_DISCONNECTED;
		}
		
		bool bOK = !!InternetSetOption(0/*hInternet*/, INTERNET_OPTION_CONNECTED_STATE, (void*)&ConInfo, sizeof(ConInfo));
		if (!bOK)
			return false;

		return true;
	}

	/////////////////////////////////////////////////////////////////////////////
	bool GoOnline(LPCSTR strURL, HWND hwndParentWindow, bool bForceLANOnlineSilently)
	{
		m_bStateIsUpToDate = false;

		// If not forcing a LAN online silently, or not using a LAN, get permission to access the URL
		if (!bForceLANOnlineSilently || !GetUseLAN())
			return !!InternetGoOnline((LPTSTR)strURL, hwndParentWindow, 0/*dwFlags*/);

		// Using a LAN, check the online status and silently go online
		if (!GetIsOnline())
			if (!SetOnline(true))
				return false;

		// Get permission to access the URL
		return !!InternetGoOnline((LPTSTR)strURL, hwndParentWindow, 0/*dwFlags*/);
	}

	/////////////////////////////////////////////////////////////////////////////
	bool HangUp()
	{
		m_bStateIsUpToDate = false;

		// Instructs the modem to disconnect from the Internet
		long lRetValue = InternetHangUp(m_dwConnectionID, 0);
		return (lRetValue == ERROR_SUCCESS);
	}

	/////////////////////////////////////////////////////////////////////////////
	bool Dial(HWND hwndParentWindow, LPCSTR strConnectionName, DialsFlags lOptions, bool bShowOfflineButton = false)
	{
		m_bStateIsUpToDate = false;

		// Initiates a connection to the Internet using	a modem
		long lFlags;
		if (bShowOfflineButton)
			lFlags = lOptions | INTERNET_DIAL_SHOW_OFFLINE;
		else
			lFlags = lOptions;

		long lRetValue = InternetDial(hwndParentWindow, (LPTSTR)strConnectionName, lFlags, &m_dwConnectionID, 0);
		if (lRetValue != ERROR_SUCCESS)
			return false;

		return true;
	}

	/////////////////////////////////////////////////////////////////////////////
	bool AutodialHangup()
	{
		m_bStateIsUpToDate = false;

		// Disconnects an automatic	dial-up	connection
		bool bOK = !!InternetAutodialHangup(0);
		return bOK;
	}

	/////////////////////////////////////////////////////////////////////////////
	bool Autodial(HWND hwndParentWindow, AutoDialsFlags	lOptions, bool bFailIfSecurityCheck = true)
	{
		m_bStateIsUpToDate = false;

		// Causes the modem	to automatically dial the default Internet connection
		long lFlags;
		if (bFailIfSecurityCheck)
			lFlags = lOptions | INTERNET_AUTODIAL_FAILIFSECURITYCHECK;

		bool bOK = !!InternetAutodial(lFlags, hwndParentWindow);
		if (!bOK)
			return false;

		return true;
	}

	/////////////////////////////////////////////////////////////////////////////
	// InternetGetCookie returns the cookies for the specified URL and all its parent URLs
	bool GetCookie(LPCSTR strURL, LPCSTR strCookieName, CString& strCookie)
	{
		// The first call to InternetGetCookie will get the required buffer size needed to download the cookie data
		LPSTR pstrData = NULL;
		DWORD dwSize = 0;
		InternetGetCookie((LPCTSTR)strURL, NULL/*strCookieName*/, (LPTSTR)pstrData, &dwSize);
		if (!dwSize)
			return false;

		// Allocate the necessary buffer.
		pstrData = new char[dwSize];
		bool bOK = false;
		if (strCookieName)
			bOK = !!InternetGetCookieEx((LPTSTR)strURL, (LPTSTR)strCookieName, (LPTSTR)pstrData, &dwSize, 0/*dwFlags*/, 0);
		else
			bOK = !!InternetGetCookie((LPTSTR)strURL, NULL/*strCookieName*/, (LPTSTR)pstrData, &dwSize);

		strCookie = pstrData;
		
		// Release the memory allocated for the buffer.
		delete [] pstrData;

		return bOK;
	}

	/////////////////////////////////////////////////////////////////////////////
	// Persistent cookies have an expiration date. These cookies are stored in the Windows\System directory
	// Session cookies are stored in memory and can be accessed only by the process that created them
	// The data for the cookie should be in the format:
	//		NAME=VALUE
	// For the expiration date, the format must be:
	//		DAY, DD-MMM-YYYY HH:MM:SS GMT
	// DAY is the three-letter abbreviation for the day of the week,
	// DD is the day of the month,
	// MMM is the three-letter abbreviation for the month,
	// YYYY is the year,
	// HH:MM:SS is the time of the day in military time.
	bool SetCookie(LPCSTR strURL, LPCSTR strNameValue, int m=0, int d=0, int y=0, int hh=0, int mm=0, int ss=0)
	{
		CString strCookie = (LPCTSTR)strNameValue;

		if (m && d && y)
		{
			COleDateTime ExpirationDate(y, m, d, hh, mm, ss);
			LPCSTR DaysOfWeek[] = {"", "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};
			LPCSTR Months[] = {"", "Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};
			CString strExpires;
			strExpires.Format(_T("; expires = %s, %02d-%s-%04d %02d:%02d:%02d GMT"),
				DaysOfWeek[ExpirationDate.GetDayOfWeek()],
				ExpirationDate.GetDay(),
				Months[ExpirationDate.GetMonth()],
				ExpirationDate.GetYear(),
				ExpirationDate.GetHour(),
				ExpirationDate.GetMinute(),
				ExpirationDate.GetSecond());
			strCookie += strExpires; // creates a persistent cookie
		}
		// else creates a session cookie

		return !!InternetSetCookie((LPTSTR)strURL, NULL/*strCookieName*/, strCookie);
	}

	/////////////////////////////////////////////////////////////////////////////
	CString	GetConnectionName()
	{
		if (!m_bStateIsUpToDate)
			GetConnectedState();

		// Name of the active connection
		return m_ConnectionName;
	}

	/////////////////////////////////////////////////////////////////////////////
	bool GetIsConnectionConfigured()
	{
		if (!m_bStateIsUpToDate)
			GetConnectedState();

		// Does the system have a valid connection to the Internet?  It may or may not be currently connected
		return m_bIsConnectionConfigured;
	}

	/////////////////////////////////////////////////////////////////////////////
	bool GetIsOnline()
	{
		if (!m_bStateIsUpToDate)
			GetConnectedState();

		// Is the system in online mode?
		return m_bIsOnline;
	}

	/////////////////////////////////////////////////////////////////////////////
	bool IsOnline(bool bForceLANOnline)
	{
		// Check the online status
		if (GetIsOnline())
			return true;

		// If not online and not using a LAN, get out
		if (!bForceLANOnline || !GetUseLAN())
			return false;

		// Using a LAN - try and go online
		if (!SetOnline(true))
			return false;

		return GetIsOnline();
	}

	/////////////////////////////////////////////////////////////////////////////
	bool GetIsRasInstalled()
	{
		if (!m_bStateIsUpToDate)
			GetConnectedState();

		// Does the system have RAS installed?
		return m_bIsRasInstalled;
	}

	/////////////////////////////////////////////////////////////////////////////
	bool GetUseProxy()
	{
		if (!m_bStateIsUpToDate)
			GetConnectedState();

		// Does the system use a proxy server to connect to the Internet?
		return m_bUseProxy;
	}

	/////////////////////////////////////////////////////////////////////////////
	bool GetUseLAN()
	{
		if (!m_bStateIsUpToDate)
			GetConnectedState();

		// Does the system use a local area network to connect to the Internet?
		return m_bUseLAN;
	}

	/////////////////////////////////////////////////////////////////////////////
	bool GetUseModem()
	{
		if (!m_bStateIsUpToDate)
			GetConnectedState();

		// Does the system use a modem to connect to the Internet?
		return m_bUseModem;
	}

private:
	/////////////////////////////////////////////////////////////////////////////
	void ClearConnectedState()
	{
		m_bStateIsUpToDate = false;
		m_bUseModem = false;
		m_bUseLAN = false;
		m_bUseProxy = false;
		m_bIsRasInstalled = false;
		m_bIsOnline = false;
		m_bIsConnectionConfigured = false;
		m_ConnectionName = "";
	}
	
	/////////////////////////////////////////////////////////////////////////////
	bool GetConnectedState()
	{
		ClearConnectedState();
		
		// Refresh the class data
		char strConnectionName[256];
		DWORD dwConnectionFlags = 0;
		bool bOK = !!InternetGetConnectedStateEx(&dwConnectionFlags, (LPTSTR)strConnectionName, sizeof(strConnectionName), 0);
		if (!bOK)
			return false;

		m_bStateIsUpToDate = true;
		m_bUseModem = !!(dwConnectionFlags & INTERNET_CONNECTION_MODEM);
		m_bUseLAN = !!(dwConnectionFlags & INTERNET_CONNECTION_LAN);
		m_bUseProxy = !!(dwConnectionFlags & INTERNET_CONNECTION_PROXY);
		m_bIsRasInstalled = !!(dwConnectionFlags & INTERNET_RAS_INSTALLED);
		m_bIsOnline = !(dwConnectionFlags & INTERNET_CONNECTION_OFFLINE);
		m_bIsConnectionConfigured = !!(dwConnectionFlags & INTERNET_CONNECTION_CONFIGURED);
		m_ConnectionName = strConnectionName;

	//j	bOK = !!InternetCheckConnection(NULL/*strURL*/, 0/*dwFlags*/, 0);
		return true;
	}

private:
	bool m_bStateIsUpToDate;
	bool m_bUseModem;
	bool m_bUseLAN;
	bool m_bUseProxy;
	bool m_bIsRasInstalled;
	bool m_bIsOnline;
	bool m_bIsConnectionConfigured;
	CString	m_ConnectionName;
	DWORD m_dwConnectionID;
};


