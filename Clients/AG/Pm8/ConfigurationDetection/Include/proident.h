//	proident.h
//	copyright Brøderbund Software 1997
//	K.A.Daniels


#ifndef	_proident_h_
#define	_proident_h_


//	dependencies

#include "proitype.h"

#include <windows.h>
#include <stdlib.h>


//	prototypes

//	Initialize the productIdentifier data structure.
//	It is not necessary to call this function prior to using
//	other functions in this API.
void proidInit(
		productIdentifier&	proident,
		const char*	company = NULL,
		const char*	family = NULL,
		const char*	product = NULL,
		const char*	version = NULL,
		const char*	IDNumber = NULL
		);

//	Load the product identification information from the specified
//	configuration file.
//	Return Value:
//		Total number of bytes copied from configuration file to
//		product identification structure.
DWORD proidLoad(
		const char*	configFile,
		productIdentifier&	proident
		);

//	Load the product identification information from the specified
//	configuration file, and convert information into string format.
void proidLoadString(
		const char*	configFile,
		char*	buffer,
		size_t	bufferSize
		);

//	Convert product identification information from data structure
//	to string format.
void proidConvertToString(
		const productIdentifier&	proident,
		char*	buffer,
		size_t	bufferSize
		);

//	Convert product identification information from string format
//	to data structure.
void proidConvertFromString(
		const char*	piString,
		productIdentifier&	proident
		);


//	for backwards compatibility

const char	keyIdentRedirect[] = "file redirect";
#define InitProdIdent(pi,co,fa,pr,ve,id)	proidInit(pi,co,fa,pr,ve,id)
DWORD GetProdIdentFromCfg(
		const char*	configFile,
		productIdentifier&	proident
		);

#endif