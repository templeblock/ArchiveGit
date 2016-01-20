#ifndef MONACO_PKEY_H
#define MONACO_PKEY_H

#include "monacokey.h"

class CMonacoPKey:public CMonacoKey
{
	private:
	protected:
	RB_WORD				_overwritePassword1;
	RB_WORD				_overwritePassword2;


	public:
	CMonacoPKey();
	~CMonacoPKey();

	BOOL programKey();
	BOOL programApp(AppLock *applock, int which);
	BOOL inactivateApp();
};

#endif