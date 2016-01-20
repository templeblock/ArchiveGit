#ifndef _IPC_H_
#define _IPC_H_


// Class for Inter Process Communication using Memory Mapped Files
class CIPC
{
public:
	CIPC(LPCTSTR szName);
	virtual ~CIPC();

	bool CreateIPCMMF();
	bool OpenIPCMMF(void);
	void CloseIPCMMF(void);

	bool IsOpen(void) const {return (m_hFileMap != NULL);}

	bool ReadIPCMMF(LPBYTE pBuf, DWORD &dwBufSize);
	bool WriteIPCMMF(const LPBYTE pBuf, const DWORD dwBufSize);

	bool Lock(void);
	void Unlock(void);

protected:
	HANDLE m_hFileMap;
	HANDLE m_hMutex;
	CString m_szMMF;
	CString m_szMutex;
};

#endif
