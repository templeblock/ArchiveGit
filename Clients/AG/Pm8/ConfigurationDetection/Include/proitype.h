//	proitype.h
//	copyright Brøderbund Software 1997
//	K.A.Daniels


#ifndef	_proitype_h_
#define	_proitype_h_


//	dependencies

#include <windows.h>
#ifdef __BORLANDC__
#include <stdlib.h>
#endif


//	constants

const char	sectionProdIdentifier[] = "Product Identification";

const char	keyIdentCompany[] = "company";
const char	keyIdentFamily[] = "family";
const char	keyIdentProduct[] = "product";
const char	keyIdentVersion[] = "version";
const char	keyIdentProdID[] = "product identification number";

const short	PROIDENT_MAX_COMPANY_LENGTH = 256;
const short PROIDENT_MAX_FAMILY_LENGTH = 256;
const short PROIDENT_MAX_PRODUCT_LENGTH = 256;
const short PROIDENT_MAX_VERSION_LENGTH = 64;
const short	PROIDENT_MAX_IDNUMBER_LENGTH = 64;


const char	proidentDemark[] = "\\";
const char	proidentSpacer[] = "_";


//	type definitions

struct	productIdentifier
{
	char	company[PROIDENT_MAX_COMPANY_LENGTH];
	char	family[PROIDENT_MAX_FAMILY_LENGTH];
	char	product[PROIDENT_MAX_PRODUCT_LENGTH];
	char	version[PROIDENT_MAX_VERSION_LENGTH];
	char	IDNumber[PROIDENT_MAX_IDNUMBER_LENGTH];
};

#endif