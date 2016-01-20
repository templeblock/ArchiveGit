// Source file: bcontrol.cpp
BOOL Bitmap_Register( HINSTANCE hInstance );
long WINPROC EXPORT BitmapControl( HWND hWindow, unsigned message, WPARAM wParam, LPARAM lParam);
void Bitmap_OnInit(HWND hWindow, HMCI hMCIfile, BOOL fPlay);
void Bitmap_Play(HWND hWindow);
void Bitmap_Show(HWND hWindow, BOOL bShow);
//static BOOL ComputeDIBControlSize( HWND hControl, LPINT lpDX, LPINT lpDY );
//static LPFRAME DibResource2??( HINSTANCE hInst, ITEMID idResource );

