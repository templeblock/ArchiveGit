/*=================================================================
Contains:	exception code
=================================================================*/

#ifndef _H_EXCEPT
#define _H_EXCEPT

#include "sctypes.h"

#define raise(err)				throw( err )
#define raise_if(exp, err)		{if (exp) throw(err);}

#endif _H_EXCEPT
