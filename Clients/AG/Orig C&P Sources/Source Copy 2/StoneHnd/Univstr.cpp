#include "univstr.h"

void u2char( void* buf, const stUnivString& ustr )
{
	char* p = (char*)buf;
	for ( unsigned i = 0; i < ustr.len; i++ )
		p[i] = (char)ustr.ptr[i];
	p[i] = 0;
}

/* ==================================================================== */

void UniversalString::reverse()
{
	stUnivChar* p = (stUnivChar*)ptr;
	
	stUnivChar* ep = &p [ len ];
	while ( p < ep ) {
		stUnivChar ch = *--ep;
		*ep = *p;
		*p++ = ch;
	}
}

/* ==================================================================== */
// Universal string class
/* ==================================================================== */

int UniversalString::operator==( const stUnivString& stustr )
{
	if ( stustr.len != len )
		return 0;
	if ( stustr.ptr == ptr )
		return 1;
	else {
		for ( unsigned i = 0; i < len; i++ ) {
			if ( ptr[i] != stustr.ptr[i] )
				return 0;
		}
	}
	return 1;
}

/* ==================================================================== */

int UniversalString::operator!=( const stUnivString& stustr )
{
	if ( stustr.len != len )
		return 1;
	if ( stustr.ptr == ptr )
		return 0;
	else {
		for ( unsigned i = 0; i < len; i++ ) {
			if ( ptr[i] != stustr.ptr[i] )
				return 1;
		}
	}
	return 0;
}
