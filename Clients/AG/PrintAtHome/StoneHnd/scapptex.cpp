/***************************************************************************
Contains:
	This module takes a handle to APPTextRun structure and manages the
	memory (specs & chars) associated with it. Locking and unlocking. It
	also maintains some internal structures that used for reading the text
	between the lock and unlock calls. Refer to the 
	APPTextRun in SCTextExch.h.
*****************************************************************************/

#include "scapptex.h"

/* ==================================================================== */
stTextImportExport::stTextImportExport()
{
	reset();
}

/* ==================================================================== */
stTextImportExport::~stTextImportExport()
{
	paras_.clear();
}

/* ==================================================================== */
void stTextImportExport::reset()
{
	pindex_ = -1;
}

/* ==================================================================== */
stPara& stTextImportExport::currentPara()
{
	return paras_[pindex_];
}

/* ==================================================================== */
// importing
void stTextImportExport::StartPara( TypeSpec& ts )
{
	if ( paras_.size() > 0 )
		paras_[pindex_].complete();
	
	stPara newPara( ts );
	paras_.push_back( newPara );
	pindex_ = (int32)paras_.size() - 1;
}

/* ==================================================================== */
void stTextImportExport::SetParaSpec( TypeSpec& ts )
{
	stPara& p = currentPara();
	p.setparaspec( ts );
}

/* ==================================================================== */
void stTextImportExport::PutString( const uchar* str, int len, TypeSpec& ts )
{
	stPara& p = currentPara();
	p.append( ts );
	p.append( str, len );
}

/* ==================================================================== */
void stTextImportExport::PutString( USTR& ustr, TypeSpec& ts )
{
	stPara& p = currentPara();
	p.append( ts );
	p.append( ustr );
}

/* ==================================================================== */
void stTextImportExport::PutChar( UCS2 ch, TypeSpec& ts )
{
	stPara& p = currentPara();
	p.append( ts );
	p.append( ch ); 
}

/* ==================================================================== */
// reading
int stTextImportExport::NextPara( TypeSpec& ts )
{
	pindex_++;
	if ( pindex_ < (int32)paras_.size() ) {
		ts = paras_[pindex_].paraspec();
		paras_[pindex_].validate();
		return pindex_;
	}
	return -1;
}

/* ==================================================================== */
int stTextImportExport::GetChar( UCS2& ch, TypeSpec& ts )
{
	stPara& p = currentPara();
	return p.get( ch, ts );
}

