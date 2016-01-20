#pragma once

bool RemoveFilesFromFolder(LPCTSTR szFolder, LPCTSTR szType);
bool IsFilePresent(LPCTSTR szFilePath);
bool GetInstallPath(CString &szPath);
BOOL RestoreWindowProc(HWND hwnd, LONG pfProc);
WNDPROC SetWindowProc(HWND hwnd, LONG pfProc);
