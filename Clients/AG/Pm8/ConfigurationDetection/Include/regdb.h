//	regdb.h
//	copyright Brøderbund Software 1997
//	K.A. Daniels

#ifndef _regdb_h_
#define _regdb_h_


//	dependencies

#include <windows.h>


//	constants

const char	regdbDemark[] = "\\";


//	prototypes

//	Convert a string value to registry database key
//	plus subkey.
void regdbConvertStringToKey(
			char*	keyString,
			HKEY&	regDBRootKey
			);

long regdbGetString(
			const HKEY	regDBKey,
			const char*	regDBSubKey,
			const char*	regDBKeyValName,
			char*	buffer,
			size_t	bufferSize
			);

long regdbWriteString(
			const HKEY	regDBKey,
			const char*	regDBSubKey,
			const char*	regDBKeyValName,
			const char*	buffer
			);

#endif
