#ifndef MONACO_KEY_H
#define MONACO_KEY_H

#include "afxwin.h"
#include "spromeps.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


//the max # of app can be protected by a key
#define MAX_PROTECTED_APP	(7)
#define MONACO_PRINT_KEY	(0)
#define MONACO_SCAN_KEY		(1)
#define MONACO_VIEW_KEY		(2)
#define MONACO_COLOR_KEY	(3)
#define PRESSTOPROOF_KEY	(4)

typedef struct{
	long	assigned;	//set if the lock is assigned to an application
	long	program;	//set if want to program
	long	activate;	//set if want to activate
	long	found;		//set if found
	char	name[24];	//application name
	RB_WORD option;		//options
	RB_WORD dcount;		//count saved in dataword, used for demo count
	RB_WORD count;		//demo count			   used for count
	RB_WORD algo1;		//top half algo			   used for version	
	RB_WORD algo2;		//bottom half algo		   used for flag for demo
	RB_WORD apass1;		//algo passwd1	
	RB_WORD apass2;		//algo passwd2
	RB_WORD address;	//first address,			used for first address
	RB_BYTE	query[4];
	RB_BYTE	answer[4];
	RB_BYTE acode;
	char	subnames[16][24];	//application sub names

} AppLock;
		

class CMonacoKey
{
	private:
	protected:
	long				_numApps;	//number of locks
	RB_WORD				_devID;
	RB_WORD				_writePassword;
	RB_SPRO_APIPACKET   _apiPacket;
	AppLock				_appLock[MAX_PROTECTED_APP];

	void _initKey();
	void _initApp(AppLock *applock);
	void _setApp(AppLock *applock, int which);
	SP_STATUS _initPacket(RB_SPRO_APIPACKET ApiPacket);


	public:
	CMonacoKey();
	virtual ~CMonacoKey();
	BOOL getSn(unsigned short *sn);
	BOOL connectKey();
	BOOL readKey();
	BOOL readApp(AppLock *applock, int which);
	BOOL CheckApp(AppLock *applock, int which);
	BOOL DecApp(AppLock *applock);
	BOOL queryApp(AppLock *applock);
	BOOL activateKey(unsigned long p1, unsigned long p2);
	long decrementCounter();
	virtual BOOL programKey(){	return FALSE;	}
	virtual BOOL programApp(AppLock *applock, int which);
	AppLock*	getAppLock(int which){ return &_appLock[which]; }
	BOOL GeneratePassword(long sn,unsigned long* p1,unsigned long* p2);

};

#endif