#include "think_bugs.h"
#include "fullname.h"

void pstrcat(StringPtr dst, StringPtr src)
{
	/* copy string in */
	//memmove(src + 1, dst + *dst + 1, *src);
	
	memmove(dst + *dst + 1, src + 1,  *src);
	/* adjust length byte */
	*dst += *src;
}

//insert s2 in front of s1
void pstrinsert( unsigned char *s1, unsigned char *s2 )
{
    int OriginalLen;

    OriginalLen = *s1;
    *s1 += *s2;

	memmove(s1+1+*s2, s1+1, OriginalLen);
	memmove(s1+1, s2+1, *s2);
}

void PathNameFromDirID(long dirID, short vRefNum, StringPtr fullPathName)
{
	DirInfo	block;
	Str255	mark = "\p:";
	Str255	directoryName="\p";
	OSErr	err;

	block.ioDrParID = dirID;
	block.ioNamePtr = directoryName;
	do {
			block.ioVRefNum = vRefNum;
			block.ioFDirIndex = -1;
			block.ioDrDirID = block.ioDrParID;
			err = PBGetCatInfo((CInfoPBPtr)&block, FALSE);
			pstrinsert(fullPathName, mark);
			pstrinsert(fullPathName, directoryName);
	} while (block.ioDrDirID != 2);	//note!! 2 is current volume directory
}
