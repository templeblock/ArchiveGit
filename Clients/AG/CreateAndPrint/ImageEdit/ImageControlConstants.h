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

#ifdef NOTUSED
//////////////////////////////////////////////////////////////////////
inline int Split(const CString& strDelimiter, const CString& str, CStringArray& strArray)
{
	CString strTemp = str; 
	while (strTemp.Find(strDelimiter) != -1)
	{
		strArray.Add(strTemp.Mid(0, strTemp.Find(strDelimiter)));
		if ((strTemp.Mid(0, strTemp.Find(strDelimiter)).GetLength() + strDelimiter.GetLength()) == strTemp.GetLength())
			strArray.Add("");
		CString n = strTemp;
		strTemp = n.Mid(n.Find(strDelimiter) + strDelimiter.GetLength(), n.GetLength()); 
	}

	if (strTemp.GetLength() > 0)
		strArray.Add(strTemp);

	return strArray.GetSize();
}
#endif NOTUSED
