#if !defined(_INITGDIPLUS_H_)
#define _INITGDIPLUS_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <gdiplus.h>

class InitGDIPlus {
private:
	HANDLE							m_hMap;
	bool							m_bInited, m_bInitCtorDtor;
	ULONG_PTR						m_gdiplusToken;
	Gdiplus::GdiplusStartupInput	m_gdiplusStartupInput;
	long							m_initcount;

public:
	InitGDIPlus(bool bInitCtorDtor = false) : m_bInitCtorDtor(bInitCtorDtor), 
				m_bInited(false), m_hMap(NULL), m_gdiplusToken(NULL), 
				m_gdiplusStartupInput(NULL), m_initcount(0)  
	{
		if (m_bInitCtorDtor) {
			Initialize();
		}
	}

	virtual ~InitGDIPlus()  {
		if (m_bInitCtorDtor) {
			Deinitialize();
		}
	}

	void Initialize() {
		if (!m_bInited) {
			char buffer[1024];
			sprintf(buffer, "GDIPlusInitID=%x", GetCurrentProcessId());
			m_hMap = CreateFileMapping((HANDLE) INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE | SEC_COMMIT, 0, sizeof(long), buffer);
			if (m_hMap != NULL) {
				if (GetLastError() == ERROR_ALREADY_EXISTS) { 
					CloseHandle(m_hMap); 
				} else {
					m_bInited = true;
					Gdiplus::GdiplusStartup(&m_gdiplusToken, &m_gdiplusStartupInput, NULL);
					ATLTRACE("Inited GDIPlus\n");
				}
			}
		}
		m_initcount++;
	}

	void Deinitialize()
	{
		m_initcount--;
		if (m_bInited && m_initcount == 0) {
			ATLTRACE("GDIPlus shutdown\n");
			Gdiplus::GdiplusShutdown(m_gdiplusToken);
			CloseHandle(m_hMap);
			m_bInited = false;
		}
	}
};

#endif