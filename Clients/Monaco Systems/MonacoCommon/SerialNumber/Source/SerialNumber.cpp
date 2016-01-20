#include "SerialNumber.h"
#include "SerialNumberDlg.h"
#include "Message.h"

#define MESSAGE_PROGRAMMER_ERROR "\
Programmer's error; Rebuild your application after you copy the dialog resource 7000 \
contained in '..\\Common\\SerialNumber\\Source\\SerialNumberDlg.rc'"

/////////////////////////////////////////////////////////////////////////////
CSerialNumber::CSerialNumber()
{
	m_iApplicationID = 0;
}

/////////////////////////////////////////////////////////////////////////////
CSerialNumber::~CSerialNumber()
{
}

/////////////////////////////////////////////////////////////////////////////
/*static*/ bool CSerialNumber::IsMonacoPRINT()
{
	CSerialNumber SerialNumber;
	return SerialNumber.IsValid("MonacoPRINT", 19);
}	

/////////////////////////////////////////////////////////////////////////////
/*static*/ bool CSerialNumber::IsMonacoSCAN()
{
	CSerialNumber SerialNumber;
	return SerialNumber.IsValid("MonacoSCAN", 28);
}	

/////////////////////////////////////////////////////////////////////////////
/*static*/ bool CSerialNumber::IsMonacoVIEW()
{
	CSerialNumber SerialNumber;
	return SerialNumber.IsValid("MonacoVIEW", 37);
}	

/////////////////////////////////////////////////////////////////////////////
/*static*/ bool CSerialNumber::IsPrestoProof()
{
	CSerialNumber SerialNumber;
	return SerialNumber.IsValid("PrestoProof", 46);
}	

/////////////////////////////////////////////////////////////////////////////
/*static*/ bool CSerialNumber::IsMonacoProfilerLite()
{
	CSerialNumber SerialNumber;
	return SerialNumber.IsValid("MonacoProfilerLite", 55);
}	

/////////////////////////////////////////////////////////////////////////////
bool CSerialNumber::IsValid(LPCSTR pApplication, int iApplicationID)
{
	m_iApplicationID = iApplicationID;

	CString szSerialNumber;
	szSerialNumber = GetSerialNumber();
	if (ValidateSerialNumber(szSerialNumber))
		return true;

	// Get the serial number from the user; it must be validated in order to return IDOK
	CString szApplication(pApplication);
	CSerialNumberDlg dlg(this, szApplication);
	int nResponse = dlg.DoModal();
	if (nResponse == IDCANCEL)
		return false;
	else
	if (nResponse == -1)
		Message(MESSAGE_PROGRAMMER_ERROR);
	
	return true;
}

/////////////////////////////////////////////////////////////////////////////
CString CSerialNumber::GetSerialNumber()
{
	return AfxGetApp()->GetProfileString( "Settings", "SerialNumber", "Default" );
}

/////////////////////////////////////////////////////////////////////////////
void CSerialNumber::WriteSerialNumber(CString& szSerialNumber)
{
	AfxGetApp()->WriteProfileString( "Settings", "SerialNumber", szSerialNumber );
}

/////////////////////////////////////////////////////////////////////////////
bool CSerialNumber::ValidateSerialNumber(CString& szSerialNumber)
{
	// The pattern is as follows (processing the serial number from the RIGHT):
	// nDigits trailing digits; represents the user number
	// a single character equal to 'A' + nDigits; a check on the length of the user number
	// 2 vendor digits; represents the vendor id (select this psuedo randomly when assigning serial numbers)
	// 2 application digits; represents the app id and must match what is passed in)
	// 1 checksum digit; equal to the 1 digit checksum of app and vendor digits
	// any leading characters or digits do not matter
	// e.g. EPSON12345-61923C45

	szSerialNumber.MakeUpper();

	if (szSerialNumber == "ANDOVER")
			return true;

	// Count the trailing digits
	int iLocation = szSerialNumber.GetLength() - 1;
	int nDigits = 0;
	TCHAR c = 0;
	while (1)
	{
		if (iLocation < 0)
			return false;
		c = szSerialNumber.GetAt(iLocation--);
		if (c < '0' || c > '9')
			break; // if not a digit, break out
		nDigits++;
	}

	// the last character fetched should be 'A' + nDigits
	if (c != 'A' + nDigits)
		return false;

	// Get the 2 vendor digits, the 2 app digits, and 1 checksum digit
	int iDigit[5];
	for (int i=0; i<5; i++)
	{
		if (iLocation < 0)
			return false;
		c = szSerialNumber.GetAt(iLocation--);
		if (c < '0' || c > '9')
			return false; // if not a digit, get out
		iDigit[i] = (c - '0');
	}

	// Compare the application digit with the passed app id
	int iApplicationID = (iDigit[3] * 10) + iDigit[2];
	if (iApplicationID != m_iApplicationID)
		return false;

	// Compute the checksum of the vendor and app digits
	int iCheckSum = CheckSum(iDigit[0] + iDigit[1] + iDigit[2] + iDigit[3], 9);
	
	// Now compare the checksum digit with the computed one
	if (iDigit[4] != iCheckSum)
		return false;

	return true;
}

/////////////////////////////////////////////////////////////////////////////
int CSerialNumber::CheckSumString(CString szString, int iStopValue)
{
	// Add the ascii values in the string
	szString.MakeUpper();
	int iCheckSum = 0;
	int iLocation = szString.GetLength() - 1;
	while (iLocation >= 0)
	{
		TCHAR c = szString.GetAt(iLocation--);
		iCheckSum += c;
	}

	return CheckSum(iCheckSum, iStopValue);
}

/////////////////////////////////////////////////////////////////////////////
int CSerialNumber::CheckSum(int iValue, int iStopValue)
{
	if (iValue <= iStopValue)
		return iValue;

	int iCheckSum = 0;
	while (iValue > 0)
	{
		iCheckSum += (iValue % 10);
		iValue /= 10;
	}

	return CheckSum(iCheckSum, iStopValue);
}
