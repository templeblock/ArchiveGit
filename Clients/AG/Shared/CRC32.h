#pragma once

// The alogrithm for CRC32 was taken from the crc32.c source 
// file of the'zlib' general purpose compression library
// version 1.1.4.

class CCRC32
{
public:
	CCRC32(void);
	~CCRC32(void);
	ULONG crc32(ULONG crc, const BYTE *buf, UINT len);

	static inline void CalcCrc32(const BYTE byte, DWORD &dwCrc32);
	static DWORD StringCrc32(LPCTSTR szString, DWORD &dwCrc32);
	static DWORD FileCrc32Win32(LPCTSTR szFilename, DWORD &dwCrc32);
	static DWORD FileCrc32Assembly(LPCTSTR szFilename, DWORD &dwCrc32);
};
