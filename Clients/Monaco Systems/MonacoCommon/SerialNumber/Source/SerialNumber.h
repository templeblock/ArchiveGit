#ifndef SerialNumber_H
#define SerialNumber_H

#include "afxwin.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

class CSerialNumber
{
	public:
	CSerialNumber();
	~CSerialNumber();

	static bool IsMonacoPRINT(void);
	static bool IsMonacoSCAN(void);
	static bool IsMonacoVIEW(void);
	static bool IsPrestoProof(void);
	static bool IsMonacoProfilerLite(void);

	bool IsValid(LPCSTR lpAppNameint, int iApplicationID);
	CString GetSerialNumber(void);
	void WriteSerialNumber(CString& szSerialNumber);
	bool ValidateSerialNumber(CString& szSerialNumber);
	int CheckSumString(CString szString, int iStopValue);
	int CheckSum(int iValue, int iStopValue);

	private:
	int m_iApplicationID;
};

#endif SerialNumber_H
