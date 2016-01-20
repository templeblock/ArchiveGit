#ifndef _LAUNCHINTERFACES_H_
#define _LAUNCHINTERFACES_H_

struct SBDocDescription;

typedef enum
{
	kAppError = -1,
	kAppLauncher,
	kAppPrintShop,
	kAppPressWriter,
	kAppWebSiteDesigner

} EAppType;

typedef enum
{
	kResultOK = 1,
	kResultCancel = 2,
	kResultOpen = 3,
	kResultOtherApp = 4

} EResult;

typedef enum
{
	kMethodScratch,
	kMethodCustomize,

} EProjMethod;

typedef union tagCustomData
{
	struct
	{
		char  lpszQSLName[_MAX_PATH];
		short fSampleGraphics;
		short fSampleText;
	};

} QSLData;

// {8DC39CD0-4D90-11d2-BD04-006008473DD0}
DEFINE_GUID( IID_IPSLaunchData, 
0x8DC39CD0, 0x4D90, 0x11d2, 0xBD, 0x04, 0x00, 0x60, 0x08, 0x47, 0x3d, 0xd0);

interface IPSLaunchData : IUnknown
{
	STDMETHOD( Initialize )( HANDLE hFile ) = 0;

	STDMETHOD_( short, GetProjectMethod)( void ) = 0;
	STDMETHOD_( short, GetProjectType)( void ) = 0;
	STDMETHOD_( short, GetProjectFormat)( void ) = 0;
	STDMETHOD_( short, GetProjectPaperSize)( void ) = 0;
	STDMETHOD_( void, GetQSLData)( QSLData* m_pData ) = 0;
	STDMETHOD_( void, GetDocDescription)( SBDocDescription* m_pData ) = 0;
};

// {942BA380-6AC1-11d2-9B93-00A0C99F6B3C}
DEFINE_GUID(IID_IPressWriterData, 
0x942ba380, 0x6ac1, 0x11d2, 0x9b, 0x93, 0x0, 0xa0, 0xc9, 0x9f, 0x6b, 0x3c);

// {8DC39CD2-4D90-11d2-BD04-006008473DD0}
DEFINE_GUID( IID_IPSLaunchCtrl, 
0x8DC39CD2, 0x4D90, 0x11d2, 0xBD, 0x04, 0x00, 0x60, 0x08, 0x47, 0x3d, 0xd0);

interface IPSLaunchCtrl : IUnknown
{
	STDMETHOD( DoFileNew )(HWND hwndParent, EAppType eAppType, EResult* pResult) = 0;
	STDMETHOD( DoFileOpen )(HWND hwndParent, EAppType eAppType, EResult* pResult) = 0;
	STDMETHOD( GetData )(IPSLaunchData** ppObj) = 0;
};

// {8DC39CD1-4D90-11d2-BD04-006008473DD0}
DEFINE_GUID( CLSID_PSLaunchData, 
0x8DC39CD1, 0x4D90, 0x11d2, 0xBD, 0x04, 0x00, 0x60, 0x08, 0x47, 0x3d, 0xd0);

// {8DC39CD3-4D90-11d2-BD04-006008473DD0}
DEFINE_GUID( CLSID_PSLaunchCtrl, 
0x8DC39CD3, 0x4D90, 0x11d2, 0xBD, 0x04, 0x00, 0x60, 0x08, 0x47, 0x3d, 0xd0);

#endif // _LAUNCHINTERFACES_H_
