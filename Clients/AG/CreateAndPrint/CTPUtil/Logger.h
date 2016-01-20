#pragma once

class CLogger
{
public:
	CLogger(CString strPath);
	~CLogger();

	void SetWriteToFile();
	void SetWriteToWindow();

	void Log(CString Message, bool bInsertEmptyLine = false);
	void Draw(CDC* pDC, int cx, int cy);
	void ClearLog();

protected:

	void WriteToFile(CString& Message, bool bInsertEmptyLine);
	void WriteToWindow(CString& Message, bool bInsertEmptyLine);

	bool OpenLogFile();
	void CloseLogFile();

protected:
	bool m_bWriteToFile;
	bool m_bWriteToWindow;

	CString m_strPath;
	CStringArray m_MsgArray;

	FILE* m_opFile;
};

