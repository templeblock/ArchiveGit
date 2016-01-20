#include "stdafx.h"
#include "CTPUtil.h"
#include "Logger.h"

CLogger::CLogger(CString strPath)
:m_opFile(NULL),
 m_bWriteToFile(false),
 m_bWriteToWindow(false),
 m_strPath(strPath)
{
}

CLogger::~CLogger()
{
	CloseLogFile();
}

void CLogger::SetWriteToFile()
{
	m_bWriteToFile = true;
	m_bWriteToWindow = false;
}

void CLogger::SetWriteToWindow()
{
	m_bWriteToWindow = true;
	m_bWriteToFile = false;
}

void CLogger::Log(CString Message, bool bInsertEmptyLine)
{
	CString strMessage = Message;
	strMessage += "\r\n";

	if (m_bWriteToFile)
	{
		WriteToFile(strMessage, bInsertEmptyLine);
		return;
	}

	if (m_bWriteToWindow)
	{
		WriteToWindow(strMessage, bInsertEmptyLine);
		return;
	}
}

void CLogger::WriteToFile(CString& Message, bool bInsertEmptyLine)
{
	if (!OpenLogFile())
		return;

	fwrite(Message, sizeof(char), Message.GetLength(), m_opFile);
	if (bInsertEmptyLine)
		fwrite("\r\n", sizeof(char), strlen("\r\n"), m_opFile);
	fflush(m_opFile);
}

void CLogger::WriteToWindow(CString& Message, bool bInsertEmptyLine)
{
	m_MsgArray.Add(Message);
	if (bInsertEmptyLine)
		m_MsgArray.Add("");
}

void CLogger::Draw(CDC* pDC, int cx, int cy)
{
	pDC->SetTextColor(RGB(0, 0, 0));

	int x = cx*3;
	int y = cy;

	for (int i=0; i<m_MsgArray.GetSize(); i++)
	{
		pDC->TextOut(x, y, m_MsgArray[i]);
		y = y + cy + cy/3;
	}
}

bool CLogger::OpenLogFile()
{
	if (!m_opFile)
	{
		CString strFileName = m_strPath + CString("\\ErrorFile.txt");
		m_opFile = fopen(strFileName, "wb");
	}

	return !!m_opFile;
}

void CLogger::CloseLogFile()
{
	if (m_opFile)
		fclose(m_opFile);

	m_opFile = NULL;
}

void CLogger::ClearLog()
{
	m_MsgArray.RemoveAll();
}
