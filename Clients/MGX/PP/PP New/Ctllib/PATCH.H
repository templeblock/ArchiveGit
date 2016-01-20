BOOL Patch_RegisterClass (HINSTANCE hInstance, UINT ClassScope);
BOOL Patch_UnRegisterClass (HINSTANCE hInstance);

VOID Patch_SetRGBColor(HWND hPatch, BYTE red, BYTE green, BYTE blue);
void Patch_UseGDI(HWND hPatch, BOOL bUseGDI);
