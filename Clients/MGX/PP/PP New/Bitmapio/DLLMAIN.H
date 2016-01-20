//®FD1¯®PL1¯®TP0¯®BT0¯®RM200¯
// (c) Copyright 1991 MICROGRAFX, Inc., All Rights Reserved.
// This material is confidential and a trade secret.
// Permission to use this work for any purpose must be obtained
// in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081
#ifndef _DLLMAIN_H_
#define _DLLMAIN_H_

class CBitmapDllApp : public CWinApp
{
public:
	// ----------------------------------------------------
	// Constructors and Destructors
	// ----------------------------------------------------
    CBitmapDllApp();
    ~CBitmapDllApp();
	
    virtual BOOL ExitInstance ();
    virtual BOOL InitInstance ();

	void BlockJPGRead();
	void ReleaseJPGRead();
	void BlockJPGWrite();
	void ReleaseJPGWrite();

private: 
	HANDLE m_hLeadMutex;
};

extern CBitmapDllApp BitmapDllApp;
#endif // _DLLMAIN_H_
