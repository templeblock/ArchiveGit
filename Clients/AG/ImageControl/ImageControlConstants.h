#pragma once

#define EDIT_START	-1
#define EDIT_APPLY	-2
#define EDIT_RESET	-3
#define EDIT_END	-4

//////////////////////////////////////////////////////////////////////
inline CString Str(LPCTSTR pszFormat, ...)
{
	va_list argList;
	va_start(argList, pszFormat);
	CString str;
	str.FormatV(pszFormat, argList);
	va_end(argList);
	return str;
}

//////////////////////////////////////////////////////////////////////
inline CString Str(UINT nFormatID, ...)
{
	CString strFormat;
	strFormat.LoadString(nFormatID);

	va_list argList;
	va_start(argList, nFormatID);
	CString str;
	str.FormatV(strFormat, argList);
	va_end(argList);
	return str;
}
