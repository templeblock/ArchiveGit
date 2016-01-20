//	sharecfg.h
//	copyright Brøderbund Software 1997, 1998
//	K.A. Daniels

#ifndef _sharecfg_h_
#define _sharecfg_h_

//	dependencies

#include "proitype.h"


//	constants

const short	STORE_CFGFILE	= 1;
const short	STORE_REGDB		= 2;
const short	STORE_CFGFILE83	= 3;

const short	SCOPE_USER		= 1;
const short	SCOPE_LOCAL		= 2;
const short	SCOPE_NETWORK	= 3;

const short	LEVEL_COMPANY	= 1;
const short	LEVEL_FAMILY	= 2;
const short	LEVEL_PRODUCT	= 3;
const short	LEVEL_VERSION	= 4;

const char	netScopeName[] = "SCOPENET";
const char	localScopeName[] = "SCOPELOC";
const char	currUserScopeName[] = "SCOPEUSR";
const char	iniRootStoreName[] = "BBSTORE";
const char	regRootStoreName[] = "SOFTWARE";

const char	shareDemark[] = "\\";
const char	shareSpacer[] = "_";


//	prototypes

extern "C"{

//	FUNCTION:	shareLoad
//
//	Construct a path name (file or regdb) from the product identifier structure.
//
//	The store parameter indicates whether the path is required for:
//		long file names (STORE_CFGFILE)
//		registry database (STORE_REGDB)
//		MS-DOS compliand file name (STORE_CFGFILE83)
//
//	The scope parameter indicates scope of the storage, and can be:
//		current user (SCOPE_USER)
//		local machine (SCOPE_LOCAL)
//		network (SCOPE_NETWORK)
//
//	The level parameter indicates the depth of the path contructed:
//		company name is the top level (LEVEL_COMPANY)
//		product family is the next level (LEVEL_FAMILY)
//		product name comes next (LEVEL_PRODUCT)
//		version number (major.minor) is at the lowest level of the heirarchy (LEVEL_VERSION)
//
//	Reference the other functions in this file for examples of the path that can
//	be constructed with shareLoad
//
void shareLoad(
		const productIdentifier&	proident,
		char*	buffer,
		size_t	bufferSize,
		const short	store = STORE_REGDB,
		const short	scope = SCOPE_LOCAL,
		const short	level = LEVEL_VERSION
		);

//	FUNCTION:	shareLoadDirPath
//
//	Construct a file path name (long file name).
//
//	Refer to the description of shareLoad for information on the
//	scope and level parameters.
//
//	Example path name, given the default values for the scope and level parameters:
//	BBSTORE\SCOPELOC\SOFTWARE\<company>\<family>\<product>\<version>
//
//	File name character validation is not performed by this function.
//	For example, the function will not warn that the '*' character in the name
//	is an invalid MS-DOS file name character.
//
void shareLoadDirPath(
		const productIdentifier&	proident,
		char*	buffer,
		size_t	bufferSize,
		const short	scope = SCOPE_LOCAL,
		const short	level = LEVEL_VERSION
		);


//	FUNCTION:	shareLoadRegDBPath
//
//	Construct a registry key name.
//
//	Refer to the description of shareLoad for information on the
//	scope and level parameters.
//
//	Example key name, given the default values for the scope and level parameters:
//	HKEY_LOCAL_MACHINE\SOFTWARE\<company>\<family>\<product>\<version>
//
//	Example key name, given that scope has a value of 0:
//	SOFTWARE\<company>\<family>\<product>\<version>
void shareLoadRegDBPath(
		const productIdentifier&	proident,
		char*	buffer,
		size_t	bufferSize,
		const short	scope = SCOPE_LOCAL,
		const short	level = LEVEL_VERSION
		);


//	FUNCTION:	shareConvertPathTo83
//
//	Construct a file path name (MS-DOS compliant).
//
//	The shareConvertNameTo83 is called for each subdirectory or file name.
//
//	File name character validation is not performed by this function.
//	For example, the function will not warn that the '*' character in the name
//	is an invalid MS-DOS file name character.
//
void shareConvertPathTo83(
		char*	buffer,
		size_t	bufferSize
		);


//	FUNCTION:	shareConvertNameTo83
//
//	Construct an MS-DOS compliant name, either subdirectory or file.
//
//	The name is converted using the following algorithm:
//		If the name is 8 characters or less, replace each ' ' character
//			with the shareSpacer character.
//		Done.
//
//		If the name is greater than 8 characters, then it cannot
//			begin or end with the ' ' character.
//
//		Given n separate words in the name, take the first 8/n + 8%n characters
//		from the first word, and 8/n characters from each of the other words.
//		
//		Concatenate these characters together.
//		Replace each ' ' character with the shareSpacer character.
//
//	File name character validation is not performed by this function.
//	For example, the function will not warn that the '*' character in the name
//	is an invalid MS-DOS file name character.
//		
void shareConvertNameTo83(
		char*	buffer,
		size_t	bufferSize
		);

};

#endif
