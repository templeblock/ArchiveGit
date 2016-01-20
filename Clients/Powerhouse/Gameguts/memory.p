// Source file: memory.cpp
LPTR Alloc( long lCount );
LPTR AllocX( long lCount, WORD wFlags );
LPTR AllocExtend( LPTR lpMemory, long lCount );
void FreeUp( LPVOID lpMemory );
DWORD AvailableMemory( void );
BOOL AllocLines( LPPTR lpList, int nLines, int iWidth, int iDepth );

