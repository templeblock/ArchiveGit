/***************************************************************************/

	(c) Copyright 1993 MICROGRAFX, Inc., All Rights Reserved.
	This material is confidential and a trade secret.
	Permission to use this work for any purpose must be obtained
	in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081

/***************************************************************************/

#include "stdafx.h"
#include "cmsutil.h"

/************************************************************************/
void cmsInfoInit( LPCMSINFO pcmsInfo )
/************************************************************************/
{
pcmsInfo->src.ptInfo.frRCS = 0;
pcmsInfo->src.ptInfo.toRCS = 0;
pcmsInfo->dst.ptInfo.frRCS = 0;
pcmsInfo->dst.ptInfo.toRCS = 0;
pcmsInfo->src.DataType = FDT_NONE;
pcmsInfo->dst.DataType = FDT_NONE;
pcmsInfo->src.ColorMap = NULL;
pcmsInfo->dst.ColorMap = NULL;

}

/************************************************************************/
BOOL IsSrcPTSelected( LPPTINFO ptInfo )
/************************************************************************/
{

return( ptInfo->toRCS != 0 );
}

/************************************************************************/
BOOL IsDstPTSelected( LPPTINFO ptInfo )
/************************************************************************/
{

return( ptInfo->frRCS != 0 && ptInfo->toRCS != 0 );
}


