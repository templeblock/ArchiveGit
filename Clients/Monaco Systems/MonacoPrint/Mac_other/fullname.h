#ifndef FULL_NAME_H
#define FULL_NAME_H
 
void pstrcat(StringPtr dst, StringPtr src);
void pstrinsert( unsigned char *s1, unsigned char *s2 );
void PathNameFromDirID(long dirID, short vRefNum, StringPtr fullPathName);

#endif