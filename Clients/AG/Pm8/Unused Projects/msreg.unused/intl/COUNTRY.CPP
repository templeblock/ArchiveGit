/*
// $Workfile: COUNTRY.CPP $
// $Revision: 1 $
// $Date: 3/03/99 6:22p $
//
// Copyright © 1996 MicroLogic Software, Inc.
// All rights reserved.
*/

/*
// Revision History:
//
// $Log: /PM8/msreg/intl/COUNTRY.CPP $
// 
// 1     3/03/99 6:22p Gbeddow
// 
// 1     7/14/98 4:07p Jayn
// 
//    Rev 1.4   06 Jun 1997 16:53:46   Fred
// Removed phone registration for North America
// 
//    Rev 1.3   07 Oct 1996 14:03:24   FRED
// Added GetTaxInformation()
// 
//    Rev 1.2   04 Oct 1996 15:06:54   FRED
// Added GetWebSite()
// 
//    Rev 1.1   04 Oct 1996 10:03:50   FRED
// Added GetCountryName(); Country Id's to data
// 
//    Rev 1.0   03 Oct 1996 10:31:04   FRED
// Initial revision.
*/

#include "stdafx.h"
#include "intl.h"
#include "country.h"

static CCountryTableEntry CountryTable[] =
{
	{ ID_Afghanistan,                  "93","00"    },
	{ ID_Albania,                      "355","00"   },
	{ ID_Algeria,                      "213","00"   },
	{ ID_AmericanSamoa,                "684","00"   },
	{ ID_Andorra,                      "376","00"   },
	{ ID_Angola,                       "244","01"   },
	{ ID_Anguilla,                     "1","1"      },
	{ ID_Antigua,                      "1","011"    },
	{ ID_Argentina,                    "54","00"    },
	{ ID_Armenia,                      "374","810"  },
	{ ID_Aruba,                        "297","00"   },
	{ ID_AscensionIsland,              "247","01"   },
	{ ID_Australia,                    "61","0011"  },
	{ ID_AustralianAntarcticTerritory, "672","00"   },
	{ ID_Austria,                      "43","00"    },
	{ ID_Azerbaijan,                   "994","810"  },
	{ ID_Bahamas,                      "1","0011"   },
	{ ID_Bahrain,                      "973","0"    },
	{ ID_Bangladesh,                   "880","00"   },
	{ ID_Barbados,                     "1","011"    },
	{ ID_Barbuda,                      "1","011"    },
	{ ID_Belarus,                      "375","810"  },
	{ ID_Belgium,                      "32","00"    },
	{ ID_Belize,                       "501","00"   },
	{ ID_Benin,                        "229","00"   },
	{ ID_Bermuda,                      "1","011"    },
	{ ID_Bhutan,                       "975","00"   },
	{ ID_Bolivia,                      "591","00"   },
	{ ID_BosniaAndHerzegovina,         "387","00"   },
	{ ID_Botswana,                     "267","00"   },
	{ ID_Brazil,                       "55","00"    },
	{ ID_BritishVirginIslands,         "1","011"    },
	{ ID_Brunei,                       "673","00"   },
	{ ID_Bulgaria,                     "359","00"   },
	{ ID_BurkinaFaso,                  "226","00"   },
	{ ID_Burundi,                      "257","90"   },
	{ ID_Cambodia,                     "855","00"   },
	{ ID_Cameroon,                     "237","00"   },
	{ ID_Canada,                       "1","011"    },
	{ ID_CapeVerdeIslands,             "238","0"    },
	{ ID_CaymanIslands,                "1","011"    },
	{ ID_CentralAfricanRepublic,       "236","19"   },
	{ ID_Chad,                         "235","15"   },
	{ ID_Chile,                        "56","00"    },
	{ ID_China,                        "86","00"    },
	{ ID_ChristmasIsland,              "672","00"   },
	{ ID_CocosKeelingIslands,          "61","00"    },
	{ ID_Colombia,                     "57","90"    },
	{ ID_Comoros,                      "269","10"   },
	{ ID_Congo,                        "242","00"   },
	{ ID_CookIslands,                  "682","00"   },
	{ ID_CostaRica,                    "506","00"   },
	{ ID_Croatia,                      "385","00"   },
	{ ID_Cuba,                         "53","00"    },
	{ ID_Cyprus,                       "357","00"   },
	{ ID_CzechRepublic,                "42","00"    },
	{ ID_Denmark,                      "45","00"    },
	{ ID_DiegoGarcia,                  "246","00"   },
	{ ID_Djibouti,                     "253","00"   },
	{ ID_Dominica,                     "1","011"    },
	{ ID_DominicanRepublic,            "1","011"    },
	{ ID_Ecuador,                      "593","00"   },
	{ ID_Egypt,                        "20","00"    },
	{ ID_ElSalvador,                   "503","0"    },
	{ ID_EquatorialGuinea,             "240","00"   },
	{ ID_Eritrea,                      "291","00"   },
	{ ID_Estonia,                      "372","810"  },
	{ ID_Ethiopia,                     "251","00"   },
	{ ID_FYROM,                        "389","00"   },
	{ ID_FaeroeIslands,                "298","009"  },
	{ ID_FalklandIslands,              "500","0"    },
	{ ID_FijiIslands,                  "679","05"   },
	{ ID_Finland,                      "358","990"  },
	{ ID_France,                       "33","19"    },
	{ ID_FrenchAntilles,               "590","19"   },
	{ ID_FrenchGuiana,                 "594","19"   },
	{ ID_FrenchPolynesia,              "689","00"   },
	{ ID_Gabon,                        "241","00"   },
	{ ID_Gambia,                       "220","00"   },
	{ ID_Georgia,                      "995","810"  },
	{ ID_Germany,                      "49","00"    },
	{ ID_Ghana,                        "233","00"   },
	{ ID_Gibraltar,                    "350","00"   },
	{ ID_Greece,                       "30","00"    },
	{ ID_Greenland,                    "299","009"  },
	{ ID_Grenada,                      "1","011"    },
	{ ID_Guadeloupe,                   "590","19"   },
	{ ID_Guam,                         "671","011"  },
	{ ID_GuantanamoBay,                "53","00"    },
	{ ID_Guatemala,                    "502","00"   },
	{ ID_Guinea,                       "224","00"   },
	{ ID_GuineaBissau,                 "245","099"  },
	{ ID_Guyana,                       "592","19"   },
	{ ID_Haiti,                        "509","00"   },
	{ ID_Honduras,                     "504","00"   },
	{ ID_HongKong,                     "852","001"  },
	{ ID_Hungary,                      "36","00"    },
	{ ID_Iceland,                      "354","00"   },
	{ ID_India,                        "91","00"    },
	{ ID_Indonesia,                    "62","001"   },
	{ ID_Iran,                         "98","00"    },
	{ ID_Iraq,                         "964","00"   },
	{ ID_Ireland,                      "353","00"   },
	{ ID_Israel,                       "972","00"   },
	{ ID_Italy,                        "39","00"    },
	{ ID_IvoryCoast,                   "225","00"   },
	{ ID_Jamaica,                      "1","00"     },
	{ ID_Japan,                        "81","001"   },
	{ ID_Jordan,                       "962","00"   },
	{ ID_Kazakhstan,                   "7","810"    },
	{ ID_Kenya,                        "254","00"   },
	{ ID_KiribatiRepublic,             "686","09"   },
	{ ID_KoreaNorth,                   "850","00"   },
	{ ID_KoreaSouth,                   "82","001"   },
	{ ID_Kuwait,                       "965","00"   },
	{ ID_Kyrgyzstan,                   "7","810"    },
	{ ID_Laos,                         "856","14"   },
	{ ID_Latvia,                       "371","00"   },
	{ ID_Lebanon,                      "961","00"   },
	{ ID_Lesotho,                      "266","00"   },
	{ ID_Liberia,                      "231","00"   },
	{ ID_Libya,                        "218","00"   },
	{ ID_Liechtenstein,                "41","00"    },
	{ ID_Lithuania,                    "370","810"  },
	{ ID_Luxembourg,                   "352","00"   },
	{ ID_Macao,                        "853","00"   },
	{ ID_Madagascar,                   "261","16"   },
	{ ID_Malawi,                       "265","101"  },
	{ ID_Malaysia,                     "60","007"   },
	{ ID_Maldives,                     "960","00"   },
	{ ID_Mali,                         "223","00"   },
	{ ID_Malta,                        "356","00"   },
	{ ID_MarshallIslands,              "692","00"   },
	{ ID_Martinique,                   "596","19"   },
	{ ID_Mauritania,                   "222","00"   },
	{ ID_Mauritius,                    "230","00"   },
	{ ID_MayotteIsland,                "269","10"   },
	{ ID_Mexico,                       "52","98"    },
	{ ID_Micronesia,                   "691","00"   },
	{ ID_Moldova,                      "373","810"  },
	{ ID_Monaco,                       "33","19"    },
	{ ID_Mongolia,                     "976","00"   },
	{ ID_Montserrat,                   "1","011"    },
	{ ID_Morocco,                      "212","00"   },
	{ ID_Mozambique,                   "258","00"   },
	{ ID_Myanmar,                      "95","0"     },
	{ ID_Namibia,                      "264","09"   },
	{ ID_Nauru,                        "674","115"  },
	{ ID_Nepal,                        "977","00"   },
	{ ID_Netherlands,                  "31","00"    },
	{ ID_NetherlandsAntilles,          "599","00"   },
	{ ID_Nevis,                        "1","011"    },
	{ ID_NewCaledonia,                 "687","00"   },
	{ ID_NewZealand,                   "64","00"    },
	{ ID_Nicaragua,                    "505","00"   },
	{ ID_Niger,                        "227","00"   },
	{ ID_Nigeria,                      "234","009"  },
	{ ID_Niue,                         "683","00"   },
	{ ID_NorfolkIsland,                "672","0101" },
	{ ID_Norway,                       "47","095"   },
	{ ID_Oman,                         "968","00"   },
	{ ID_Pakistan,                     "92","00"    },
	{ ID_Palau,                        "680","00"   },
	{ ID_Panama,                       "507","0"    },
	{ ID_PapuaNewGuinea,               "675","05"   },
	{ ID_Paraguay,                     "595","00"   },
	{ ID_Peru,                         "51","00"    },
	{ ID_Philippines,                  "63","00"    },
	{ ID_Poland,                       "48","00"    },
	{ ID_Portugal,                     "351","00"   },
	{ ID_PuertoRico,                   "1","011"    },
	{ ID_Qatar,                        "974","0"    },
	{ ID_ReunionIsland,                "262","19"   },
	{ ID_Romania,                      "40","00"    },
	{ ID_RotaIsland,                   "670","011"  },
	{ ID_Russia,                       "7","810"    },
	{ ID_Rwanda,                       "250","00"   },
	{ ID_SaintLucia,                   "1","011"    },
	{ ID_SaipanIsland,                 "670","011"  },
	{ ID_SanMarino,                    "378","00"   },
	{ ID_SaoTomeAndPrincipe,           "239","00"   },
	{ ID_SaudiArabia,                  "966","00"   },
	{ ID_SenegalRepublic,              "221","00"   },
	{ ID_SeychelleIslands,             "248","00"   },
	{ ID_SierraLeone,                  "232","00"   },
	{ ID_Singapore,                    "65","005"   },
	{ ID_SlovakRepublic,               "42","00"    },
	{ ID_Slovenia,                     "386","00"   },
	{ ID_SolomonIslands,               "677","00"   },
	{ ID_Somalia,                      "252","19"   },
	{ ID_SouthAfrica,                  "27","09"    },
	{ ID_Spain,                        "34","07"    },
	{ ID_SriLanka,                     "94","00"    },
	{ ID_StHelena,                     "290","01"   },
	{ ID_StKitts,                      "1","011"    },
	{ ID_StPierreAndMiquelon,          "508","19"   },
	{ ID_StVincentAndTheGrenadines,    "1","0"      },
	{ ID_Sudan,                        "249","00"   },
	{ ID_Suriname,                     "597","00"   },
	{ ID_Swaziland,                    "268","00"   },
	{ ID_Sweden,                       "46","009"   },
	{ ID_Switzerland,                  "41","00"    },
	{ ID_Syria,                        "963","00"   },
	{ ID_TaiwanRepublicOfChina,        "886","002"  },
	{ ID_Tajikistan,                   "7","810"    },
	{ ID_Tanzania,                     "255","0900" },
	{ ID_Thailand,                     "66","001"   },
	{ ID_TinianIsland,                 "670","011"  },
	{ ID_Togo,                         "228","00"   },
	{ ID_Tokelau,                      "690","00"   },
	{ ID_Tonga,                        "676","00"   },
	{ ID_TrinidadAndTobago,            "1","01"     },
	{ ID_Tunisia,                      "216","00"   },
	{ ID_Turkey,                       "90","00"    },
	{ ID_Turkmenistan,                 "7","810"    },
	{ ID_TurksAndCaicosIslands,        "1","0"      },
	{ ID_Tuvalu,                       "688","00"   },
	{ ID_Uganda,                       "256","00"   },
	{ ID_Ukraine,                      "380","810"  },
	{ ID_UnitedArabEmirates,           "971","00"   },
	{ ID_UnitedKingdom,                "44","00"    },
	{ ID_UnitedStatesVirginIslands,    "1","011"    },
	{ ID_UnitedStatesOfAmerica,        "1","011"    },
	{ ID_Uruguay,                      "598","00"   },
	{ ID_Uzbekistan,                   "7","810"    },
	{ ID_Vanuatu,                      "678","00"   },
	{ ID_VaticanCity,                  "39","00"    },
	{ ID_Venezuela,                    "58","00"    },
	{ ID_Vietnam,                      "84","00"    },
	{ ID_WallisAndFutunaIslands,       "681","19"   },
	{ ID_WesternSamoa,                 "685","0"    },
	{ ID_Yemen,                        "967","00"   },
	{ ID_Yugoslavia,                   "381","99"   },
	{ ID_Zaire,                        "243","00"   },
	{ ID_Zambia,                       "260","00"   },
	{ ID_Zimbabwe,                     "263","110"  },
};

// Utility string functions.

LPSTR NewString(const CString& csString)
{
	LPSTR pszString = NULL;
	TRY
	{
		if (!csString.IsEmpty())
		{
			pszString = new char[csString.GetLength()+1];
			if (pszString != NULL)
			{
				strcpy(pszString, csString);
			}
		}
	}
	END_TRY
	return pszString;
}

void FreeString(LPSTR pszString)
{
	delete pszString;
}

void CopyString(LPSTR pszDestination, LPCSTR pszSource, int nMaximumLength)
{
	if (nMaximumLength > 0)
	{
		strncpy(pszDestination, pszSource, nMaximumLength);
		pszDestination[nMaximumLength-1] = '\0';
	}
}

/////////////////////////////////////////////////////////////////////////////
// CNameList

CNameList::CNameList(LPCSTR pszNames /*=NULL*/, char cSeparator /*='|'*/)
{
	SetNames(pszNames, cSeparator);
}

CNameList::~CNameList()
{
}

void CNameList::SetNames(LPCSTR pszNames, char cSeparator /*='|'*/)
{
	CString csNames;
	CString csName;
	TRY
	{
		RemoveAll();
		if (pszNames != NULL)
		{
			csNames = pszNames;
			while (GetNextName(csName, csNames, cSeparator))
			{
				Add(csName);
			}
		}
	}
	END_TRY
}

BOOL CNameList::GetNextName(CString& csName, CString& csNames, char cSeparator)
{
	BOOL fResult = FALSE;
	TRY
	{
		csName.Empty();
		while (csName.IsEmpty())
		{
			int nIndex = csNames.Find(cSeparator);
			if (nIndex == -1)
			{
				csName = csNames;
				csNames.Empty();
			}
			if (nIndex > 0)
			{
				csName = csNames.Left(nIndex);
				csNames = csNames.Mid(nIndex+1);
			}

			int nLength = csName.GetLength();
			int nStart = 0;
			int nEnd = nLength-1;
			
			for (; nStart < nLength; nStart++)
			{
				if ((csName[nStart] != ' ') && (csName[nStart] != '\t'))
				{
					break;
				}
			}
			
			for (; nEnd >= 0; nEnd--)
			{
				if ((csName[nEnd] != ' ') && (csName[nEnd] != '\t'))
				{
					break;
				}
			}
	
			if (nStart > nEnd)
			{
				csName.Empty();
			}
			else
			{
				csName = csName.Mid(nStart, nEnd-nStart+1);
			}
	
			if (csNames.IsEmpty())
			{
				break;
			}
		}
		fResult = !csName.IsEmpty();
	}
	CATCH_ALL(e)
	{
		TRY
		{
			csName.Empty();
		}
		END_TRY
		fResult = FALSE;
	}
	END_CATCH_ALL

	return fResult;
}

/////////////////////////////////////////////////////////////////////////////
// CCountryList

CCountryList::CCountryList(LPCSTR pszCountries /*=NULL*/, char cSeparator /*='|'*/) :
	CNameList(pszCountries, cSeparator)
{
	m_fAllCountries = FALSE; // used by FindCountry()!
	m_fAllCountries = FindCountry("0");
}
 
CCountryList::~CCountryList()
{
}

void CCountryList::SetNames(LPCSTR pszNames, char cSeparator /*='|'*/)
{
	m_fAllCountries = FALSE; // used by FindCountry()!
	CNameList::SetNames(pszNames, cSeparator);
	m_fAllCountries = FindCountry("0");
}

BOOL CCountryList::FindCountry(WORD wId)
{
	BOOL fResult = FALSE;

	CString csId;
	TRY
	{
		csId.Format("%d", wId);
		fResult = FindCountry(csId);
	}
	END_TRY

	return fResult;
}

BOOL CCountryList::FindCountry(LPCSTR pszId)
{
	BOOL fResult = FALSE;

	if (m_fAllCountries)
	{
		fResult = TRUE;
	}
	else
	{
		TRY
		{
			for (int nIndex = 0; nIndex < GetSize(); nIndex++)
			{
				if (stricmp(GetAt(nIndex), pszId) == 0)
				{
					fResult = TRUE;
					break;
				}
			}
		}
		END_TRY
	}

	return fResult;
};

/////////////////////////////////////////////////////////////////////////////
// CInternationalOfficeData

void CInternationalOfficeData::PhoneNumber::Clear(void)
{
	m_pszNumber = NULL;
	m_fFree = FALSE;
}

void CInternationalOfficeData::PhoneNumber::Free(void)
{
	FreeString(m_pszNumber);
	m_pszNumber = NULL;
}

void CInternationalOfficeData::PhoneNumber::Load(LPCSTR pszFullOfficeName, CIniFile& IniFile, LPCSTR pszPrefix)
{
	CString csPrefix;

	csPrefix = pszPrefix;
	csPrefix += "Number";
	m_pszNumber = NewString(IniFile.GetString(pszFullOfficeName, csPrefix, ""));

	csPrefix = pszPrefix;
	csPrefix += "Free";
	m_fFree = IniFile.GetInteger(pszFullOfficeName, csPrefix, FALSE);
}

void CInternationalOfficeData::CountryRegistration::Clear(void)
{
	m_pszCountries = NULL;
	m_ModemNumber.Clear();
	m_PhoneNumber.Clear();
	m_FaxNumber.Clear();
	m_HelpNumber.Clear();
	m_pszMailAddress = NULL;
}

void CInternationalOfficeData::CountryRegistration::Free(void)
{
	FreeString(m_pszCountries);
	m_pszCountries = NULL;
	m_ModemNumber.Free();
	m_PhoneNumber.Free();
	m_FaxNumber.Free();
	m_HelpNumber.Free();
	FreeString(m_pszMailAddress);
	m_pszMailAddress = NULL;
}

void CInternationalOfficeData::CountryRegistration::Load(LPCSTR pszFullOfficeName, CIniFile& IniFile, LPCSTR pszPrefix)
{
	CString csPrefix;

	csPrefix = pszPrefix;
	csPrefix += "Countries";
	m_pszCountries = NewString(IniFile.GetString(pszFullOfficeName, csPrefix, ""));

	csPrefix = pszPrefix;
	csPrefix += "Modem";
	m_ModemNumber.Load(pszFullOfficeName, IniFile, csPrefix);

	csPrefix = pszPrefix;
	csPrefix += "Phone";
	m_PhoneNumber.Load(pszFullOfficeName, IniFile, csPrefix);

	csPrefix = pszPrefix;
	csPrefix += "Fax";
	m_FaxNumber.Load(pszFullOfficeName, IniFile, csPrefix);

	csPrefix = pszPrefix;
	csPrefix += "Help";
	m_HelpNumber.Load(pszFullOfficeName, IniFile, csPrefix);

	csPrefix = pszPrefix;
	csPrefix += "MailAddress";
	m_pszMailAddress = NewString(IniFile.GetString(pszFullOfficeName, csPrefix, ""));
}

void CInternationalOfficeData::Registration::Clear(void)
{
	m_Local.Clear();
	m_Foreign.Clear();
}

void CInternationalOfficeData::Registration::Free(void)
{
	m_Local.Free();
	m_Foreign.Free();
}

void CInternationalOfficeData::Registration::Load(LPCSTR pszFullOfficeName, CIniFile& IniFile, LPCSTR pszPrefix)
{
	CString csPrefix;

	csPrefix = pszPrefix;
	csPrefix += "Local";
	m_Local.Load(pszFullOfficeName, IniFile, csPrefix);

	csPrefix = pszPrefix;
	csPrefix += "Foreign";
	m_Foreign.Load(pszFullOfficeName, IniFile, csPrefix);
}

void CInternationalOfficeData::CountryInstantSales::Clear(void)
{
	m_pszCountries = NULL;
	m_ModemNumber.Clear();
	m_PhoneNumber.Clear();
	m_FaxNumber.Clear();
	m_HelpNumber.Clear();
	m_pszMailAddress = NULL;
}

void CInternationalOfficeData::CountryInstantSales::Free(void)
{
	FreeString(m_pszCountries);
	m_pszCountries = NULL;
	m_ModemNumber.Free();
	m_PhoneNumber.Free();
	m_FaxNumber.Free();
	m_HelpNumber.Free();
	FreeString(m_pszMailAddress);
	m_pszMailAddress = NULL;
}

void CInternationalOfficeData::CountryInstantSales::Load(LPCSTR pszFullOfficeName, CIniFile& IniFile, LPCSTR pszPrefix)
{
	CString csPrefix;

	csPrefix = pszPrefix;
	csPrefix += "Countries";
	m_pszCountries = NewString(IniFile.GetString(pszFullOfficeName, csPrefix, ""));

	csPrefix = pszPrefix;
	csPrefix += "Modem";
	m_ModemNumber.Load(pszFullOfficeName, IniFile, csPrefix);

	csPrefix = pszPrefix;
	csPrefix += "Phone";
	m_PhoneNumber.Load(pszFullOfficeName, IniFile, csPrefix);

	csPrefix = pszPrefix;
	csPrefix += "Fax";
	m_FaxNumber.Load(pszFullOfficeName, IniFile, csPrefix);

	csPrefix = pszPrefix;
	csPrefix += "Help";
	m_HelpNumber.Load(pszFullOfficeName, IniFile, csPrefix);

	csPrefix = pszPrefix;
	csPrefix += "MailAddress";
	m_pszMailAddress = NewString(IniFile.GetString(pszFullOfficeName, csPrefix, ""));
}

void CInternationalOfficeData::InstantSales::Clear(void)
{
	m_Local.Clear();
	m_Foreign.Clear();
}

void CInternationalOfficeData::InstantSales::Free(void)
{
	m_Local.Free();
	m_Foreign.Free();
}

void CInternationalOfficeData::InstantSales::Load(LPCSTR pszFullOfficeName, CIniFile& IniFile, LPCSTR pszPrefix)
{
	CString csPrefix;

	csPrefix = pszPrefix;
	csPrefix += "Local";
	m_Local.Load(pszFullOfficeName, IniFile, csPrefix);

	csPrefix = pszPrefix;
	csPrefix += "Foreign";
	m_Foreign.Load(pszFullOfficeName, IniFile, csPrefix);
}

void CInternationalOfficeData::Clear(void)
{
	m_pszOffice = NULL;
	m_pszCountry = NULL;
	m_pszTax = NULL;
	m_Registration.Clear();
	m_InstantSales.Clear();
}

void CInternationalOfficeData::Free(void)
{
	FreeString(m_pszOffice);
	m_pszOffice = NULL;
	FreeString(m_pszCountry);
	m_pszCountry = NULL;
	FreeString(m_pszTax);
	m_pszTax = NULL;
	m_Registration.Free();
	m_InstantSales.Free();
}

BOOL CInternationalOfficeData::Load(LPCSTR pszOfficeName, CIniFile& IniFile)
{
	BOOL fResult = FALSE;

	// Clear out any existing values. Note that Clear() nulls out everything
	// without freeing any exitsing data. So, if there are existing strings
	// allocated, they will become leaks. We cannot define constructor and
	// destructor functions since we want to initialize an instance of this
	// class using static data. The lesson is to be careful and call Free()
	// when you're done with the data returned by Load().

	Clear();

	CString csFullOfficeName;
	TRY
	{
		// Build the full section name for the given office.
		csFullOfficeName = "Office";
		csFullOfficeName += pszOfficeName;

		// Load the values.
		m_pszOffice = NewString(IniFile.GetString(csFullOfficeName, "Office", ""));
		m_pszCountry = NewString(IniFile.GetString(csFullOfficeName, "Country", ""));
		m_pszTax = NewString(IniFile.GetString(csFullOfficeName, "Tax", ""));
		m_Registration.Load(csFullOfficeName, IniFile, "Registration");
		m_InstantSales.Load(csFullOfficeName, IniFile, "InstantSales");

		// No errors.
		fResult = TRUE;
	}
	END_TRY

	if (!fResult)
	{
		// Some sort of error happened. Free any partial results.
		Free();
	}

	return fResult;
}

/////////////////////////////////////////////////////////////////////////////
// CInternationalInformation

CInternationalOfficeData CInternationalInformation::DefaultOfficeData =
{
	"Emeryville",						// office name
	"1222",								// office country: United States of America (land of the free)
	"CA|8.25",							// office tax list
	{										// registration information
		{										// local registration information
			"1222|1038",						// local countries: United States of America|Canada
			{	"800-338-4321", TRUE  },	// registration modem

			// 6/6/97 (FF) Removed phone registration. This change coincides with the
			// introduction of the CR5 registration code prefix and the registration
			// code which matches the last four digits of the parent part number reversed.

			{	"",             FALSE },	// registration phone
//			{	"800-859-3387", TRUE	 },	// registration phone

			{	"800-933-0571", TRUE	 },	// registration fax
			{	"800-888-9078", TRUE	 },	// registration help
			"Mindscape|@Department@|1351 Ocean Avenue|Emeryville, CA 94608-1128",
		},
		{										// foreign registration information
			"0",									// foreign countries: all
			{	"510-652-9841", FALSE },	// registration modem
			{	"",             FALSE },	// registration phone
			{	"510-652-5040", FALSE },	// registration fax
			{	"",             FALSE },	// registration help
			"Mindscape|@Department@|1351 Ocean Avenue|Emeryville, CA 94608-1128|U.S.A.",
		},
	},
	{										// instant sales information
		{										// local instant sales information
			"1222|1038",						// local countries: United States of America|Canada
			{	"800-338-4321", TRUE	 },	// instant sales modem
			{	"800-888-0436", TRUE	 },	// instant sales phone
			{	"800-933-0571", TRUE	 },	// instant sales fax
			{	"800-888-9078", TRUE	 },	// instant sales help
			"Mindscape|@Department@|1351 Ocean Avenue|Emeryville, CA 94608-1128",
		},
		{										// foreign instant sales information
			"0",									// foreign countries: all
			{	"510-652-9841", FALSE },	// instant sales modem
			{	"",             FALSE },	// instant sales phone
			{	"510-652-5040", FALSE },	// instant sales fax
			{	"",             FALSE },	// instant sales help
			"Mindscape|@Department@|1351 Ocean Avenue|Emeryville, CA 94608-1128|U.S.A.",
		},
	},
};

void CInternationalInformation::PhoneNumber::Clear(void)
{
	m_csOffice.Empty();
	m_wCountry = 0;
	m_fUnitedStatesLocal = FALSE;
	m_fLocal = FALSE;
	m_fFree = FALSE;
	m_csNumber.Empty();
	m_csPrefix.Empty();
};

void CInternationalInformation::MailAddress::Clear(void)
{
	m_csOffice.Empty();
	m_wCountry = 0;
	m_fUnitedStatesLocal = FALSE;
	m_fLocal = FALSE;
	m_csAddress.Empty();
};

void CInternationalInformation::Registration::Clear(void)
{
	m_Modem.Clear();
	m_Phone.Clear();
	m_Fax.Clear();
	m_Help.Clear();
	m_Mail.Clear();
}

void CInternationalInformation::InstantSales::Clear(void)
{
	m_Modem.Clear();
	m_Phone.Clear();
	m_Fax.Clear();
	m_Help.Clear();
	m_Mail.Clear();
}

CInternationalInformation::CInternationalInformation()
{
	m_wCountry = 0;
	m_fUnitedStatesLocal = FALSE;
	m_Registration.Clear();
	m_InstantSales.Clear();
	m_csWebSite.Empty();
	m_csTaxInformation.Empty();
}

CInternationalInformation::~CInternationalInformation()
{
}

BOOL CInternationalInformation::GetInternationalData(WORD wCountryId, LPCSTR pszIniFile, CInternationalData* pInternationalData)
{
	BOOL fSuccess = FALSE;

	CInternationalInformation* pInformation = NULL;
	CIniFile IniFile;
	LPSTR pszOffices = NULL;

	CInternationalOfficeData OfficeData;
	OfficeData.Clear();

	TRY
	{
		// Look up the country in the country table.
		CCountryTableEntry* pCountry = CCountryTableEntry::GetCountryEntryById(wCountryId);

		ASSERT((pCountry != NULL) && (pszIniFile != NULL) && (pInternationalData != NULL));
		if ((pCountry != NULL) && (pszIniFile != NULL) && (pInternationalData != NULL))
		{
			// Initialize the .INI file.
			IniFile.Name(pszIniFile);

			// Allocate the object to hold the international information.
			pInformation = new CInternationalInformation;	// constructor clears data
			ASSERT(pInformation != NULL);
			if (pInformation != NULL)
			{
				// We have a place to put the data. We now have to run across all
				// the offices and fill in the best deals for the different entries.
				// The offices in the .INI file are checked in the order in which they
				// are listed. Finally, the default office data is checked.

				// Record the ID of the country.
				pInformation->m_wCountry = pCountry->GetId();

				// Record whether the source country is local to the United States.
				pInformation->m_fUnitedStatesLocal = pCountry->IsUnitedStatesLocal();

				// Get the list of office names.
				pszOffices = IniFile.GetSectionKeys("Offices");
				if (pszOffices != NULL)
				{
					LPCSTR pszOffice = pszOffices;

					// Loop through each office.
					while (*pszOffice != '\0')
					{
						// Load the section as office data.
						OfficeData.Load(pszOffice, IniFile);

						// Update the information with the new office data,
						pInformation->Update(pCountry, &OfficeData);

						// Free the allocated office data.
						OfficeData.Free();

						// Move on to the next office.
						pszOffice += strlen(pszOffice)+1;
					}
				}

				// Update the information with the default data in case a
				// better office was not found.
				pInformation->Update(pCountry, GetDefaultOfficeData());

				// Set the web site.
				GetWebSite(wCountryId, pszIniFile, pInformation->m_csWebSite);

				// Set the tax information.
				GetTaxInformation(pszIniFile, pInformation->m_csTaxInformation);

				// Copy the information to the InternationalData block.
				pInformation->Copy(pInternationalData);

				// We did the best we could.
				fSuccess = TRUE;
			}
		}
	}
	CATCH_ALL(e)
	{
		fSuccess = FALSE;
	}
	END_CATCH_ALL

	// Clean up.

	delete pszOffices;
	pszOffices = NULL;

	OfficeData.Free();

	delete pInformation;
	pInformation = NULL;

	return fSuccess;
}

void CInternationalInformation::Update(CCountryTableEntry* pCountry, CInternationalOfficeData* pOfficeData)
{
	// Update the current information structure with data from an office.
	ASSERT(pCountry != NULL);
	ASSERT(pOfficeData != NULL);

	// Get the country table entry for the office.
	CCountryTableEntry* pOfficeCountry = pCountry->GetCountryEntryById(pOfficeData->m_pszCountry);
	if (pOfficeCountry != NULL)
	{
		// Update with registration information.
		m_Registration.Update(pCountry, pOfficeCountry, pOfficeData, &(pOfficeData->m_Registration));

		// Update with instant sales information.
		m_InstantSales.Update(pCountry, pOfficeCountry, pOfficeData, &(pOfficeData->m_InstantSales));
	}
}

void CInternationalInformation::Registration::Update(CCountryTableEntry* pCountry, CCountryTableEntry* pOfficeCountry, CInternationalOfficeData* pOfficeData, CInternationalOfficeData::Registration* pData)
{
	// Update with local registration information.
	Update(pCountry, pOfficeCountry, pOfficeData, &(pData->m_Local), TRUE);

	// Update with foreign registration information.
	Update(pCountry, pOfficeCountry, pOfficeData, &(pData->m_Foreign), FALSE);
}

void CInternationalInformation::Registration::Update(CCountryTableEntry* pCountry, CCountryTableEntry* pOfficeCountry, CInternationalOfficeData* pOfficeData, CInternationalOfficeData::CountryRegistration* pData, BOOL fLocal)
{
	// Make sure we're in the country list.
	CCountryList Countries(pData->m_pszCountries);
	if (Countries.FindCountry(pCountry->GetId()))
	{
		// Update registration information fields.
		m_Modem.Update(pCountry, pOfficeCountry, pOfficeData, &(pData->m_ModemNumber), fLocal);
		m_Phone.Update(pCountry, pOfficeCountry, pOfficeData, &(pData->m_PhoneNumber), fLocal);
		m_Fax.Update(pCountry, pOfficeCountry, pOfficeData, &(pData->m_FaxNumber), fLocal);
		m_Help.Update(pCountry, pOfficeCountry, pOfficeData, &(pData->m_HelpNumber), fLocal);
		m_Mail.Update(pCountry, pOfficeCountry, pOfficeData, pData->m_pszMailAddress, fLocal);
	}
}

void CInternationalInformation::InstantSales::Update(CCountryTableEntry* pCountry, CCountryTableEntry* pOfficeCountry, CInternationalOfficeData* pOfficeData, CInternationalOfficeData::InstantSales* pData)
{
	// Update with local instant sales information.
	Update(pCountry, pOfficeCountry, pOfficeData, &(pData->m_Local), TRUE);

	// Update with foreign instant sales information.
	Update(pCountry, pOfficeCountry, pOfficeData, &(pData->m_Foreign), FALSE);
}

void CInternationalInformation::InstantSales::Update(CCountryTableEntry* pCountry, CCountryTableEntry* pOfficeCountry, CInternationalOfficeData* pOfficeData, CInternationalOfficeData::CountryInstantSales* pData, BOOL fLocal)
{
	// Make sure we're in the country list.
	CCountryList Countries(pData->m_pszCountries);
	if (Countries.FindCountry(pCountry->GetId()))
	{
		// Update instant sales information fields.
		m_Modem.Update(pCountry, pOfficeCountry, pOfficeData, &(pData->m_ModemNumber), fLocal);
		m_Phone.Update(pCountry, pOfficeCountry, pOfficeData, &(pData->m_PhoneNumber), fLocal);
		m_Fax.Update(pCountry, pOfficeCountry, pOfficeData, &(pData->m_FaxNumber), fLocal);
		m_Help.Update(pCountry, pOfficeCountry, pOfficeData, &(pData->m_HelpNumber), fLocal);
		m_Mail.Update(pCountry, pOfficeCountry, pOfficeData, pData->m_pszMailAddress, fLocal);
	}
}

void CInternationalInformation::PhoneNumber::Update(CCountryTableEntry* pCountry, CCountryTableEntry* pOfficeCountry, CInternationalOfficeData* pOfficeData, CInternationalOfficeData::PhoneNumber* pData, BOOL fLocal)
{
	// Update a phone number information field.
	if (pData->Exists())
	{
		// We have a phone number. Use it if we're currently empty or if
		// the current value is foreign and the new value is local.
		if (!Exists() || (!m_fLocal && fLocal))
		{
			m_csOffice = pOfficeData->m_pszOffice;
			m_wCountry = pOfficeCountry->GetId();
			m_fUnitedStatesLocal = pOfficeCountry->IsUnitedStatesLocal();
			m_fLocal = fLocal;
			m_fFree = pData->IsFree();
			m_csNumber = pData->GetNumber();

			// Figure out the dialing prefix.
			if (fLocal)
			{
				m_csPrefix.Empty();
			}
			else
			{
				m_csPrefix = pCountry->m_pszDialingPrefix;
				m_csPrefix += " ";
				m_csPrefix += pOfficeCountry->m_pszCountryCode;
				m_csPrefix += " ";
			}
		}
	}
}

void CInternationalInformation::MailAddress::Update(CCountryTableEntry* pCountry, CCountryTableEntry* pOfficeCountry, CInternationalOfficeData* pOfficeData, LPCSTR pData, BOOL fLocal)
{
	// Update a mailing address information field.
	if (pData != NULL)
	{
		// We have a mailing address. Use it if we're currently empty or if
		// the current value is foreign and the new value is local.
		if (!Exists() || (!m_fLocal && fLocal))
		{
			m_csOffice = pOfficeData->m_pszOffice;
			m_wCountry = pOfficeCountry->GetId();
			m_fUnitedStatesLocal = pOfficeCountry->IsUnitedStatesLocal();
			m_fLocal = fLocal;
			m_csAddress = pData;
		}
	}
}

void CInternationalInformation::Copy(CInternationalData* pData)
{
	ASSERT(pData != NULL);
	pData->m_wCountry = m_wCountry;
	pData->m_fUnitedStatesLocal = m_fUnitedStatesLocal;
	m_Registration.Copy(&(pData->m_Registration));
	m_InstantSales.Copy(&(pData->m_InstantSales));
	CopyString(pData->m_szWebSite, m_csWebSite, sizeof(pData->m_szWebSite));
	CopyString(pData->m_szTaxInformation, m_csTaxInformation, sizeof(pData->m_szTaxInformation));
}

void CInternationalInformation::Registration::Copy(CInternationalData::Registration* pData)
{
	ASSERT(pData != NULL);
	m_Modem.Copy(&(pData->m_Modem));
	m_Phone.Copy(&(pData->m_Phone));
	m_Fax.Copy(&(pData->m_Fax));
	m_Help.Copy(&(pData->m_Help));
	m_Mail.Copy(&(pData->m_Mail));
}

void CInternationalInformation::InstantSales::Copy(CInternationalData::InstantSales* pData)
{
	ASSERT(pData != NULL);
	m_Modem.Copy(&(pData->m_Modem));
	m_Phone.Copy(&(pData->m_Phone));
	m_Fax.Copy(&(pData->m_Fax));
	m_Help.Copy(&(pData->m_Help));
	m_Mail.Copy(&(pData->m_Mail));
}

void CInternationalInformation::PhoneNumber::Copy(CInternationalData::PhoneNumber* pData)
{
	ASSERT(pData != NULL);
	CopyString(pData->m_szOffice, m_csOffice, sizeof(pData->m_szOffice));
	pData->m_wCountry = m_wCountry;
	pData->m_fUnitedStatesLocal = m_fUnitedStatesLocal;
	pData->m_fLocal = m_fLocal;
	pData->m_fFree = m_fFree;
	CopyString(pData->m_szNumber, m_csNumber, sizeof(pData->m_szNumber));
	CopyString(pData->m_szPrefix, m_csPrefix, sizeof(pData->m_szPrefix));
}

void CInternationalInformation::MailAddress::Copy(CInternationalData::MailAddress* pData)
{
	ASSERT(pData != NULL);
	CopyString(pData->m_szOffice, m_csOffice, sizeof(pData->m_szOffice));
	pData->m_wCountry = m_wCountry;
	pData->m_fUnitedStatesLocal = m_fUnitedStatesLocal;
	pData->m_fLocal = m_fLocal;
	CopyString(pData->m_szAddress, m_csAddress, sizeof(pData->m_szAddress));
}

// Figure out the web site to which to refer the user. This information is
// extracted from the [WebSites] section of MSREG.INI.
BOOL CInternationalInformation::GetWebSite(WORD wCountryId, LPCSTR pszIniFile, CString& csWebSite)
{
	LPSTR pszWebSites = NULL;
	CCountryList Countries;
	CIniFile IniFile;

	TRY
	{
		csWebSite.Empty();

		IniFile.Name(pszIniFile);

		pszWebSites = IniFile.GetSectionKeys("WebSites");
		if (pszWebSites != NULL)
		{
			LPCSTR pszWebSite = pszWebSites;
			while (*pszWebSite != NULL)
			{
				Countries.SetNames(IniFile.GetString("WebSites", pszWebSite, ""));
				if (Countries.FindCountry(wCountryId))
				{
					// Found a matching country, use its preferred web site.
					csWebSite = pszWebSite;
					break;
				}

				pszWebSite += strlen(pszWebSite)+1;
			}
		}
	}
	END_TRY

	delete pszWebSites;
	pszWebSites = NULL;

	if (csWebSite.IsEmpty())
	{
		TRY
		{
			csWebSite.LoadString(IDS_DefaultWebSite);
		}
		END_TRY
	}

	return !csWebSite.IsEmpty();
}

// Get the tax information.
BOOL CInternationalInformation::GetTaxInformation(LPCSTR pszIniFile, CString& csTaxInformation)
{
	BOOL fSuccess = FALSE;

	csTaxInformation.Empty();

	CIniFile IniFile;

	TRY
	{
		// Initialize the .INI file.
		IniFile.Name(pszIniFile);

		// Get the tax information.
		csTaxInformation = IniFile.GetString("SalesInformation", "TaxRates", "");

		fSuccess = TRUE;
	}
	CATCH_ALL(e)
	{
		csTaxInformation.Empty();
		fSuccess = FALSE;
	}
	END_CATCH_ALL

	return fSuccess;
}

/////////////////////////////////////////////////////////////////////////////
// CCountryTableEntry
// An entry in the country table.

int CCountryTableEntry::GetCountryTableEntryCount(void)
{
	return sizeof(CountryTable)/sizeof(CountryTable[0]);
}

CCountryTableEntry* CCountryTableEntry::GetCountryEntryByIndex(int nIndex)
{
	ASSERT((nIndex >= 0) && (nIndex < GetCountryTableEntryCount()));
	if ((nIndex >= 0) && (nIndex < GetCountryTableEntryCount()))
	{
		return &(CountryTable[nIndex]);
	}
	return NULL;
}

CCountryTableEntry* CCountryTableEntry::GetCountryEntryById(WORD wId)
{
	int nCount = GetCountryTableEntryCount();
	for (int nIndex = 0; nIndex < nCount; nIndex++)
	{
		CCountryTableEntry* pEntry = GetCountryEntryByIndex(nIndex);
		if (pEntry != NULL)
		{
			if (pEntry->GetId() == wId)
			{
				return pEntry;
			}
		}
	}
	return NULL;
}

CCountryTableEntry* CCountryTableEntry::GetCountryEntryById(LPCSTR pszId)
{
	CCountryTableEntry* pEntry = NULL;

	if (pszId != NULL)
	{
		WORD wId = atoi(pszId);
		if (wId != 0)
		{
			pEntry = GetCountryEntryById(atoi(pszId));
		}
	}

	return pEntry;
}

BOOL CCountryTableEntry::IsUnitedStatesLocal(void)
{
	return (GetId() == ID_UnitedStatesOfAmerica) || (GetId() == ID_Canada);
}

#if 0
/////////////////////////////////////////////////////////////////////////////
// CCountryList Combo Box

CCountryList::CCountryList()
{
	m_fReturnGoesToNextControl = FALSE;
}

CCountryList::~CCountryList()
{
}

BEGIN_MESSAGE_MAP(CCountryList, CComboBox)
	//{{AFX_MSG_MAP(CRegistrationEdit)
	ON_WM_CHAR()
	ON_WM_KEYDOWN()
	ON_WM_GETDLGCODE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CCountryList::Fill(LPCSTR pszInitialCountry /*=NULL*/)
{
	SetRedraw(FALSE);
	ResetContent();
	for (int nEntry = 0; nEntry < sizeof(CountryTable)/sizeof(CountryTable[0]); nEntry++)
	{
		CCountryTableEntry* pEntry = &(CountryTable[nEntry]);
		int nIndex = AddString(pEntry->GetName());
		if (nIndex != CB_ERR)
		{
			SetItemData(nIndex, (DWORD)pEntry);
		}
	}
	if (pszInitialCountry != NULL)
	{
		SelectByName(pszInitialCountry);
	}
	SetRedraw(TRUE);
	Invalidate();
}

CCountryTableEntry* CCountryList::GetCountryTableEntry(int nIndex /*=-1*/)
{
	CCountryTableEntry* pEntry = NULL;

	if (nIndex == -1)
	{
		nIndex = GetCurSel();
	}
	if (nIndex != CB_ERR)
	{
		pEntry = (CCountryTableEntry*)GetItemData(nIndex);
		if (pEntry == (CCountryTableEntry*)CB_ERR)
		{
			pEntry = NULL;
		}
	}

	return pEntry;
}

BOOL CCountryList::SelectByName(LPCSTR pszName)
{
	BOOL fSelected = FALSE;

	int nIndex = FindStringExact(-1, pszName);
	if (nIndex != CB_ERR)
	{
		if (SetCurSel(nIndex) != CB_ERR)
		{
			fSelected = TRUE;
		}
	}

	return fSelected;
}

/////////////////////////////////////////////////////////////////////////////
// CCountryList message handlers

void CCountryList::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if ((nChar != VK_RETURN) || (!m_fReturnGoesToNextControl))
	{
		CComboBox::OnKeyDown(nChar, nRepCnt, nFlags);
	}
}

void CCountryList::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if ((nChar == VK_RETURN) && (m_fReturnGoesToNextControl))
	{
		((CDialog*)GetParent())->NextDlgCtrl();
	}
	else
	{
		CComboBox::OnKeyDown(nChar, nRepCnt, nFlags);
	}
}

UINT CCountryList::OnGetDlgCode()
{
	UINT uResult = CComboBox::OnGetDlgCode();

	const MSG* pGetDlgCodeMsg = GetCurrentMessage();

	if (m_fReturnGoesToNextControl)
	{
		if (pGetDlgCodeMsg->lParam != 0)
		{
			MSG* pMsg = (LPMSG)(pGetDlgCodeMsg->lParam);

			if ((pMsg->message == WM_KEYDOWN) || (pMsg->message == WM_CHAR))
			{
				 if (pMsg->wParam == VK_RETURN)
				{
					uResult |= DLGC_WANTMESSAGE;
				}
			}
		}
	}

	return uResult;
}
#endif
