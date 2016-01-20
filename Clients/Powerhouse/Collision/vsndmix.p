BOOL VSndMixInstall();
BOOL VSndMixRemove();

BOOL VSndMixGetData(LPBYTE lpWaveMemFile, long lSizeMemFile, 
	  			    LPBYTE *lpData, long *plSizeData);

BOOL VSndMixPlay(LPBYTE lpData, long lSize);
