//	instq.h
//	copyright Brøderbund Software 1997, 1998
//	K.A.Daniels
//	v.1.0.1.1


#ifndef	_instq_h_
#define	_instq_h_

//	dependencies

#include "proitype.h"

#include <windows.h>


//	constants

const char	KEY_TARGET_DIR[] = "$Install_ToDirectory$";
const char	KEY_UNINST_ROOT[] = "HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall";
const char	KEY_UNINST_CMD[] = "UninstallString";


//	prototypes

//	Retrieve the target installation directory, in full path form.
//	Return Vaues:
//		ERROR_SUCCESS if successful
//		refer to regdb error values on failure
long instQGetTargetInstallDir(
			const productIdentifier&	proident,
			char*	buffer,
			size_t	bufferSize
			);

//	Retrive the command line for product uninstallation, in full path form.
//	Return Vaues:
//		ERROR_SUCCESS if successful
//		refer to regdb error values on failure
long instQGetUninstallCommand(
			const productIdentifier&	proident,
			char*	buffer,
			size_t	bufferSize
			);

//	Determine whether product is installed.
//	If exeFileName is no NULL, then product exe must exist in the installation
//	target directory for the query function to succeed.
//	Return Values:
//		TRUE if the product is installed.
//		FALSE if the product is not installed.
BOOL instQQueryInstall(
			const productIdentifier&	proident,
			const char*	exeFileName = NULL
			);

//	Determine number of products installed.
//	The function uses the productIdentifier structure as a search filter.
//	The following productIdentifier fields are of interest:
//		company
//		family
//		product
//	If "company" is assigned a value, but "family" and "product" are blank,
//	then the function will report on all products belonging to the
//	indicated company.
//	If "company" and "family" are assigned values, but "product is blank,
//	then the function will report on all products belonging to the
//	indicated family within the indicated company.
//	If all three fields are assigned values, then the function will report
//	on all versions of the product indicated.
//	The function will generate an error condition if "company" is blank.
//
//	Return Values:
//		-1 if an error occurs
//		the number of installed products, otherwise
int instQGetInstalledCount(
			const productIdentifier&	proident
			);

//	Load product identifier strings for all products installed, as
//	indicated by the productIdentifier search filter (the proident
//	parameter). [Please refer to the description for
//	instQGetInstalledCount for a treatment of the productIdentifier as a
//	search filter.]
//	Each product identifier string is separated by a null character ('\0').
//	The entire group of strings is terminated by another null character.
//
//	Return Values:
//		-1 if an error occurs
//		the number of installed products, otherwise
int instQGetInstalledProidentStrings(
			const productIdentifier&	proident,
			char*	buffer,
			size_t	bufferSize
			);

#endif