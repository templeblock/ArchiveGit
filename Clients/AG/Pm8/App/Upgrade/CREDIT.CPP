/*
// $Workfile: CREDIT.CPP $
// $Revision: 1 $
// $Date: 3/03/99 6:13p $
//
// Copyright © 1996 MicroLogic Software, Inc.
// All rights reserved.
*/

/*
// Revision History:
//
// $Log: /PM8/App/UPGRADE/CREDIT.CPP $
// 
// 1     3/03/99 6:13p Gbeddow
// 
//    Rev 1.0   14 Aug 1997 15:41:38   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 10:33:10   Fred
// Initial revision.
// 
//    Rev 1.0   08 Sep 1996 09:40:24   Fred
// Initial revision.
// 
//    Rev 1.1   01 Mar 1996 13:44:36   FRED
// IL sales Tax, and type-in info on mail upgrade
// 
//    Rev 1.0   02 Feb 1996 08:58:00   JAY
// Initial revision.
// 
//    Rev 1.0   31 Jan 1996 14:12:50   FRED
// Initial revision.
// 
//    Rev 1.0   18 Jan 1996 16:40:14   FRED
// Initial revision.
*/ 

#include "stdafx.h"

#include "credit.h"
		
static void DigitsOnly(CString& csString)
{
	CString csDigits;

	TRY
	{
		csDigits.Empty();
		
		int nLength = csString.GetLength();
		
		for (int i = 0; i < nLength; i++)
		{
			if ((csString[i] >= '0') && (csString[i] <= '9'))
			{
				csDigits += csString[i];
			}
		}

		csString = csDigits;
	}
	END_TRY
}

BOOL CCreditCard::Validate(int nType, const CString& csNumber, const CString& csDate)
{
	return NumberValid(nType, csNumber) && DateValid(csDate);
}

BOOL CCreditCard::NumberValid(int nType, const CString& csNumber)
{
	BOOL fResult = FALSE;
	
	CString csDigits;
	
	TRY
	{
		// Check for special "testing" credit card number.
		if ((nType == VISA) && (csNumber.CompareNoCase("FALCON") == 0))
		{
			return TRUE;
		}
		
		// Get the digits of the card number.
		csDigits = csNumber;
		DigitsOnly(csDigits);
		
		// Validate the digits.
		if (CC2121(csDigits))
		{
			if (nType == VISA)
			{
				// 13-16 digits, starts with 4
				fResult = (csDigits.GetLength() >= 13) && (csDigits.GetLength() <= 16) && (csDigits[0] == '4');
			}
			if (nType == MASTERCARD)
			{
				// 16 digits, starts with 5
				fResult = (csDigits.GetLength() == 16) && (csDigits[0] == '5');
			}
			if (nType == AMERICANEXPRESS)
			{
				// 15 digits, starts with 3
				fResult = (csDigits.GetLength() == 15) && (csDigits[0] == '3');
			}
			if (nType == DISCOVER)
			{
				// 16 digits, starts with 6
				fResult = (csDigits.GetLength() == 16) && (csDigits[0] == '6');
			}
		}
	}
	END_TRY
	
	return fResult;
}

BOOL CCreditCard::DateValid(const CString& csDate)
{
	BOOL fResult = FALSE;
	
	CString csMonth;
	CString csYear;
	
	TRY
	{
		// Strip out everything but digits and slashes.
		csMonth.Empty();
		csYear.Empty();
		
		BOOL fSeenDivider = FALSE;
		
		int nLength = csDate.GetLength();
		
		for (int i = 0; i < nLength; i++)
		{
			if ((csDate[i] >= '0') && (csDate[i] <= '9'))
			{
				if (!fSeenDivider)
				{
					csMonth += csDate[i];
				}
				else
				{
					csYear += csDate[i];
				}
			}
			else if ((csDate[i] == '\\') || (csDate[i] == '/'))
			{
				fSeenDivider = TRUE;
			}
		}
		
		if ((csMonth.GetLength() > 0)
		 && (csMonth.GetLength() <= 2)
		 && (csYear.GetLength() > 0)
		 && (csYear.GetLength() <= 2)
		 && (atoi(csMonth) <= 12))
		{
			fResult = TRUE;
		}
	}
	END_TRY
	
	return fResult;
}

BOOL CCreditCard::CC2121(const CString& csNumber)
{
	BOOL fResult = FALSE;

	int i,j;
	int k;
	int t;
	
	CString csDigits;
	
	TRY
	{
		csDigits = csNumber;
		DigitsOnly(csDigits);
		
		if (!csDigits.IsEmpty())
		{
			// Initialize the sum.
			t = 0;
			
			// Initialize the flipping multiplier.
			j = 2;

			// Loop from last digit before the checksum digit down to the start.
			for (i = csDigits.GetLength()-2; i >= 0; i--)
			{
				k = (int)(csDigits[i]-'0');
				k = k*j;
				t = t + (k % 10) + (k / 10);
				j = 3-j;
			}

			// Confirm checksum.
			if (((t+(int)(csDigits[csDigits.GetLength()-1]-'0')) % 10) == 0)
			{
				fResult = TRUE;
			}
		}
	}
	END_TRY
	
	return fResult;
}


void CCreditCard::PrettyCreditCardNumber(int nType, CString& csNumber)
{
	CString csDigits;
	
	TRY
	{
		csDigits = csNumber;
		DigitsOnly(csDigits);
		
		if (!csDigits.IsEmpty())
		{
			csNumber.Empty();
			while (csDigits.GetLength() > 4)
			{
				if (!csNumber.IsEmpty())
				{
					csNumber += " ";
				}
				csNumber += csDigits.Left(4);
				csDigits = csDigits.Mid(4);
			}
			
			if (!csDigits.IsEmpty())
			{
				if (!csNumber.IsEmpty())
				{
					csNumber += " ";
				}
			
				csNumber += csDigits;
			}
		}
	}
	END_TRY
}
