/*
    mmenu.h
*/



void MenuStatusFileSave(HMENU hmenu, HWND hwnd);
void MenuStatusFileSaveAs(HMENU hmenu, HWND hwnd);
void MenuStatusImageOptions(HMENU hmenu, HWND hwnd);
void MenuStatusTransform(HMENU hmenu, HWND hwnd);
void MenuStatus(int nPos, HMENU hmenu);
BOOL MenuOnCommand(WPARAM wCmd);
