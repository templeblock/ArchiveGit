#ifndef _SINGLEINSTANCE_H_
#define _SINGLEINSTANCE_H_

class CSingleInstance  
{
public:
	CSingleInstance();
	virtual ~CSingleInstance();

	bool Create(LPCTSTR szMutexName);

protected:
	HANDLE m_hMutex;
};

#endif
