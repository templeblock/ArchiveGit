//memory wrapper
#ifndef MCOMEM_H
#define MCOMEM_H

#include "mcotypes.h"

McoHandle McoNewHandle(int32);
void 	McoDeleteHandle(McoHandle);
void	*McoLockHandle(McoHandle);
void	*McoLockHandleHi(McoHandle h);
void	McoUnlockHandle(McoHandle);

void 	*McoMalloc(int32 size);
void 	McoFree(void *thePtr);

#endif