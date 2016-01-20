#ifndef UNIVSTR_H_
#define UNIVSTR_H_

#include <string.h>

// a string class that can hold ascii, jis or ISO10646
typedef unsigned short	  stUnivChar;
struct stUnivString {
  const stUnivChar *ptr;
  unsigned long len;
};

class UniversalString : public stUnivString {
public:
	UniversalString()
			{
				ptr = 0;
				len = 0;
			}
	UniversalString( const char* str )
			{
				stUnivChar* p = new stUnivChar[ strlen( str ) ];
				ptr = (const stUnivChar*)p;
				len = strlen( str );
				const char* ch = str;
				for ( int i = 0; *ch; )
					p[i++] = *ch++;

			}
	UniversalString( const stUnivString& stustr )
			{
				stUnivChar* p = new stUnivChar[ stustr.len ];
				ptr = (const stUnivChar*)p;
				len = stustr.len;
				for ( unsigned i = 0; i < stustr.len; i++ )
					p[i] = stustr.ptr[i];
			}
	~UniversalString()
			{
				delete [] (stUnivChar*)ptr;
			}
	UniversalString& operator=( const stUnivString& stustr )
			{
				delete [] (stUnivChar*)ptr;
				stUnivChar* p;
				if ( stustr.len )
					p = new stUnivChar[ stustr.len ];
				else
					p = 0;
				ptr = (const stUnivChar*)p;
				len = stustr.len;
				for ( unsigned i = 0; i < stustr.len; i++ )
					p[i] = stustr.ptr[i];
				return *this;
			}
	UniversalString& operator=( const UniversalString& stustr )
			{
				delete [] (stUnivChar*)ptr;
				stUnivChar* p;
				if ( stustr.len )
					p = new stUnivChar[ stustr.len ];
				else
					p = 0;
				ptr = (const stUnivChar*)p;
				len = stustr.len;
				for ( unsigned i = 0; i < stustr.len; i++ )
					p[i] = stustr.ptr[i];
				return *this;
			}

	UniversalString& operator=( const char* str )
			{
				delete [] (stUnivChar*)ptr, ptr = 0;
				len = 0;
				stUnivChar* p = new stUnivChar[ strlen( str ) ];
				ptr = (const stUnivChar*)p;
				len = strlen( str );
				const char* ch = str;
				for ( int i = 0; *ch; )
					p[i++] = *ch++;
				return *this;
			}

	void	reverse();

	int operator==( const stUnivString& );
	int operator!=( const stUnivString& );
};

void u2char( void* buf, const stUnivString& ustr );

#endif


