#include	"util.h"

class CMyFile : public CFile
{
public:
	void
	WriteLines(LPCSTR s)
	{
		CString Temp;
		Temp.Format("%s\r\n", s);
		WriteString(Temp);
	}
	
	void
	WriteLines(LPCSTR s1, LPCSTR s2)
	{
		CString Temp;
		Temp.Format("%s%s\r\n", s1, s2);
		WriteString(Temp);
	}

	void
	WriteLines(LPCSTR s1, LPCSTR s2, LPCSTR s3)
	{
		CString Temp;
		Temp.Format("%s%s%s\r\n", s1, s2, s3);
		WriteString(Temp);
	}
	void
	WriteLines(LPCSTR s1, LPCSTR s2, LPCSTR s3, LPCSTR s4)
	{
		CString Temp;
		Temp.Format("%s%s%s%s\r\n", s1, s2, s3, s4);
		WriteString(Temp);
	}
private:
	void
	WriteString(CString &str)
	{
		Write(str, str.GetLength());
	}
};

class CExtractGraphics
{
public:
	CExtractGraphics()
	{
		m_Started = FALSE;
		pFileIterator = NULL;
	}
	
	virtual
	~CExtractGraphics()
	{
		Kill();
	}
	
	void
	Extract(CString &Projects);
		
protected:
	void
	Kill(void)
	{
		if (pFileIterator != NULL)
		{
			pFileIterator->Close();
			delete pFileIterator;
			pFileIterator = NULL;
		}
		m_Started = FALSE;
	}
	
	BOOL
	GetFile(CString &Name, CMyFile &ErrorFile);

	void 
	Breathe (HWND BreatheDlg);

	void
	Init(CString &Projects);

	BOOL				m_Started;
	CFileIterator	*pFileIterator;
	CString			m_Search;
};
