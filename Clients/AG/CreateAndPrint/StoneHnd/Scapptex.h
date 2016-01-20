/***************************************************************************
Contains:	The class for passing content plus typo state back between client and toolbox.
****************************************************************************/

#pragma once

#include "sctypes.h"
#include "scparagr.h"
#include <vector>
using namespace std;

class stTextImportExport
{
public:
	stTextImportExport();
	~stTextImportExport();

	// writing
	void	StartPara( TypeSpec& );
	void	SetParaSpec( TypeSpec& );
	void	PutString( const uchar*, int, TypeSpec& );
	void	PutString( USTR&, TypeSpec& );
	void	PutChar( UCS2, TypeSpec& ); 
	
	// reading
	int 	NextPara( TypeSpec& );
	int 	GetChar( UCS2&, TypeSpec& );		
	void	reset();

protected:
	stPara& currentPara();
	
	int32	pindex_;
	vector<stPara>	paras_;
};
