// Source file: mcontrol.cpp
BOOL Meter_Register( HINSTANCE hInstance );
long Meter_Set( HWND hWnd, ITEMID id, long lValue );
long Meter_PostSet( HWND hWnd, ITEMID id, long lValue );
long WINPROC EXPORT MeterControl( HWND hWindow, unsigned message, WPARAM wParam, LPARAM lParam);

