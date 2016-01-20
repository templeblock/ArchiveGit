#include "scglobda.h"

#include <string.h> 	  
#include "scexcept.h"
#include "scmem.h"
#include "scparagr.h"
#include "sccolumn.h"
#include "sctextli.h"

scDEFINE_RTTI( scTBObj, scObject );
scDEFINE_RTTI( scColumn, scTBObj );
scDEFINE_RTTI( scTextline, scTBObj );
scDEFINE_RTTI( scContUnit, scTBObj );

scDEFINE_ABSTRACT_RTTI( scAbstractArray, scObject );
scDEFINE_RTTI( scHandleArray, scAbstractArray );
scDEFINE_RTTI( scMemArray, scAbstractArray );
scDEFINE_RTTI( scCharArray, scHandleArray );
	
BreakStruct 		gbrS;
GlobalColumnStruct	ggcS;
scStreamChangeInfo	gStreamChangeInfo;
