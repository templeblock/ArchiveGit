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
// $Log: /PM8/msreg/COUNTRY.CPP $
// 
// 1     3/03/99 6:22p Gbeddow
// 
// 1     7/09/98 2:27p Jayn
// 
//    Rev 1.3   01 Oct 1996 10:38:12   FRED
//  
// 
//    Rev 1.2   10 Jul 1996 13:02:12   FRED
// Fixes to international changes
// 
//    Rev 1.1   10 Jul 1996 09:34:22   FRED
// Real phone numbers.
// 
//    Rev 1.0   28 Jun 1996 10:22:36   FRED
// Initial revision.
*/

#include "stdafx.h"
#include "msregx.h"
#include "util.h"
#include "country.h"

// THE definitive country table
static CCountryTableEntry CountryTable[] =
{
	{"Afghanistan","93","00"                                                       },
	{"Albania","355","00"                                                          },
	{"Algeria","213","00"                                                          },
	{"American Samoa","684","00"                                                   },
	{"Andorra","376","00"                                                          },
	{"Angola","244","01"                                                           },
	{"Anguilla","1","1"                                                            },
	{"Antigua","1","011"                                                           },
	{"Argentina","54","00"                                                         },
	{"Armenia","374","810"                                                         },
	{"Aruba","297","00"                                                            },
	{"Ascension Island","247","01"                                                 },
	{"Australia","61","0011"                                                       },
	{"Australian Antarctic Territory","672","00"                                   },
	{"Austria","43","00"                                                           },
	{"Azerbaijan","994","810"                                                      },
	{"Bahamas","1","0011"                                                          },
	{"Bahrain","973","0"                                                           },
	{"Bangladesh","880","00"                                                       },
	{"Barbados","1","011"                                                          },
	{"Barbuda","1","011"                                                           },
	{"Belarus","375","810"                                                         },
	{"Belgium","32","00"                                                           },
	{"Belize","501","00"                                                           },
	{"Benin","229","00"                                                            },
	{"Bermuda","1","011"                                                           },
	{"Bhutan","975","00"                                                           },
	{"Bolivia","591","00"                                                          },
	{"Bosnia and Herzegovina","387","00"                                           },
	{"Botswana","267","00"                                                         },
	{"Brazil","55","00"                                                            },
	{"British Virgin Islands","1","011"                                            },
	{"Brunei","673","00"                                                           },
	{"Bulgaria","359","00"                                                         },
	{"Burkina Faso","226","00"                                                     },
	{"Burundi","257","90"                                                          },
	{"Cambodia","855","00"                                                         },
	{"Cameroon","237","00"                                                         },
	{"Canada","1","011"                                                            },
	{"Cape Verde Islands","238","0"                                                },
	{"Cayman Islands","1","011"                                                    },
	{"Central African Republic","236","19"                                         },
	{"Chad","235","15"                                                             },
	{"Chile","56","00"                                                             },
	{"China","86","00"                                                             },
	{"Christmas Island","672","00"                                                 },
	{"Cocos-Keeling Islands","61","00"                                             },
	{"Colombia","57","90"                                                          },
	{"Comoros","269","10"                                                          },
	{"Congo","242","00"                                                            },
	{"Cook Islands","682","00"                                                     },
	{"Costa Rica","506","00"                                                       },
	{"Croatia","385","00"                                                          },
	{"Cuba","53","00"                                                              },
	{"Cyprus","357","00"                                                           },
	{"Czech Republic","42","00"                                                    },
	{"Denmark","45","00"                                                           },
	{"Diego Garcia","246","00"                                                     },
	{"Djibouti","253","00"                                                         },
	{"Dominica","1","011"                                                          },
	{"Dominican Republic","1","011"                                                },
	{"Ecuador","593","00"                                                          },
	{"Egypt","20","00"                                                             },
	{"El Salvador","503","0"                                                       },
	{"Equatorial Guinea","240","00"                                                },
	{"Eritrea","291","00"                                                          },
	{"Estonia","372","810"                                                         },
	{"Ethiopia","251","00"                                                         },
	{"F.Y.R.O.M. (Former Yugoslav Republic of Macedonia)","389","00"               },
	{"Faeroe Islands","298","009"                                                  },
	{"Falkland Islands","500","0"                                                  },
	{"Fiji Islands","679","05"                                                     },
	{"Finland","358","990"                                                         },
	{"France","33","19"                                                            },
	{"French Antilles","590","19"                                                  },
	{"French Guiana","594","19"                                                    },
	{"French Polynesia","689","00"                                                 },
	{"Gabon","241","00"                                                            },
	{"Gambia","220","00"                                                           },
	{"Georgia","995","810"                                                         },
	{"Germany","49","00"                                                           },
	{"Ghana","233","00"                                                            },
	{"Gibraltar","350","00"                                                        },
	{"Greece","30","00"                                                            },
	{"Greenland","299","009"                                                       },
	{"Grenada","1","011"                                                           },
	{"Guadeloupe","590","19"                                                       },
	{"Guam","671","011"                                                            },
	{"Guantanamo Bay","53","00"                                                    },
	{"Guatemala","502","00"                                                        },
	{"Guinea","224","00"                                                           },
	{"Guinea-Bissau","245","099"                                                   },
	{"Guyana","592","19"                                                           },
	{"Haiti","509","00"                                                            },
	{"Honduras","504","00"                                                         },
	{"Hong Kong","852","001"                                                       },
	{"Hungary","36","00"                                                           },
	{"Iceland","354","00"                                                          },
	{"India","91","00"                                                             },
	{"Indonesia","62","001"                                                        },
	{"Iran","98","00"                                                              },
	{"Iraq","964","00"                                                             },
	{"Ireland","353","00"                                                          },
	{"Israel","972","00"                                                           },
	{"Italy","39","00"                                                             },
	{"Ivory Coast","225","00"                                                      },
	{"Jamaica","1","00"                                                            },
	{"Japan","81","001"                                                            },
	{"Jordan","962","00"                                                           },
	{"Kazakhstan","7","810"                                                        },
	{"Kenya","254","00"                                                            },
	{"Kiribati Republic","686","09"                                                },
	{"Korea (North)","850","00"                                                    },
	{"Korea (South)","82","001"                                                    },
	{"Kuwait","965","00"                                                           },
	{"Kyrgyzstan","7","810"                                                        },
	{"Laos","856","14"                                                             },
	{"Latvia","371","00"                                                           },
	{"Lebanon","961","00"                                                          },
	{"Lesotho","266","00"                                                          },
	{"Liberia","231","00"                                                          },
	{"Libya","218","00"                                                            },
	{"Liechtenstein","41","00"                                                     },
	{"Lithuania","370","810"                                                       },
	{"Luxembourg","352","00"                                                       },
	{"Macao","853","00"                                                            },
	{"Madagascar","261","16"                                                       },
	{"Malawi","265","101"                                                          },
	{"Malaysia","60","007"                                                         },
	{"Maldives","960","00"                                                         },
	{"Mali","223","00"                                                             },
	{"Malta","356","00"                                                            },
	{"Marshall Islands","692","00"                                                 },
	{"Martinique","596","19"                                                       },
	{"Mauritania","222","00"                                                       },
	{"Mauritius","230","00"                                                        },
	{"Mayotte Island","269","10"                                                   },
	{"Mexico","52","98"                                                            },
	{"Micronesia","691","00"                                                       },
	{"Moldova","373","810"                                                         },
	{"Monaco","33","19"                                                            },
	{"Mongolia","976","00"                                                         },
	{"Montserrat","1","011"                                                        },
	{"Morocco","212","00"                                                          },
	{"Mozambique","258","00"                                                       },
	{"Myanmar","95","0"                                                            },
	{"Namibia","264","09"                                                          },
	{"Nauru","674","115"                                                           },
	{"Nepal","977","00"                                                            },
	{"Netherlands","31","00"                                                       },
	{"Netherlands Antilles","599","00"                                             },
	{"Nevis","1","011"                                                             },
	{"New Caledonia","687","00"                                                    },
	{"New Zealand","64","00"                                                       },
	{"Nicaragua","505","00"                                                        },
	{"Niger","227","00"                                                            },
	{"Nigeria","234","009"                                                         },
	{"Niue","683","00"                                                             },
	{"Norfolk Island","672","0101"                                                 },
	{"Norway","47","095"                                                           },
	{"Oman","968","00"                                                             },
	{"Pakistan","92","00"                                                          },
	{"Palau","680","00"                                                            },
	{"Panama","507","0"                                                            },
	{"Papua New Guinea","675","05"                                                 },
	{"Paraguay","595","00"                                                         },
	{"Peru","51","00"                                                              },
	{"Philippines","63","00"                                                       },
	{"Poland","48","00"                                                            },
	{"Portugal","351","00"                                                         },
	{"Puerto Rico","1","011"                                                       },
	{"Qatar","974","0"                                                             },
	{"Reunion Island","262","19"                                                   },
	{"Romania","40","00"                                                           },
	{"Rota Island","670","011"                                                     },
	{"Russia","7","810"                                                            },
	{"Rwanda","250","00"                                                           },
	{"Saint Lucia","1","011"                                                       },
	{"Saipan Island","670","011"                                                   },
	{"San Marino","378","00"                                                       },
	{"Sao Tome and Principe","239","00"                                            },
	{"Saudi Arabia","966","00"                                                     },
	{"Senegal Republic","221","00"                                                 },
	{"Seychelle Islands","248","00"                                                },
	{"Sierra Leone","232","00"                                                     },
	{"Singapore","65","005"                                                        },
	{"Slovak Republic","42","00"                                                   },
	{"Slovenia","386","00"                                                         },
	{"Solomon Islands","677","00"                                                  },
	{"Somalia","252","19"                                                          },
	{"South Africa","27","09"                                                      },
	{"Spain","34","07"                                                             },
	{"Sri Lanka","94","00"                                                         },
	{"St. Helena","290","01"                                                       },
	{"St. Kitts","1","011"                                                         },
	{"St. Pierre and Miquelon","508","19"                                          },
	{"St. Vincent and the Grenadines","1","0"                                      },
	{"Sudan","249","00"                                                            },
	{"Suriname","597","00"                                                         },
	{"Swaziland","268","00"                                                        },
	{"Sweden","46","009"                                                           },
	{"Switzerland","41","00"                                                       },
	{"Syria","963","00"                                                            },
	{"Taiwan, Republic of China","886","002"                                       },
	{"Tajikistan","7","810"                                                        },
	{"Tanzania","255","0900"                                                       },
	{"Thailand","66","001"                                                         },
	{"Tinian Island","670","011"                                                   },
	{"Togo","228","00"                                                             },
	{"Tokelau","690","00"                                                          },
	{"Tonga","676","00"                                                            },
	{"Trinidad and Tobago","1","01"                                                },
	{"Tunisia","216","00"                                                          },
	{"Turkey","90","00"                                                            },
	{"Turkmenistan","7","810"                                                      },
	{"Turks and Caicos Islands","1","0"                                            },
	{"Tuvalu","688","00"                                                           },
	{"Uganda","256","00"                                                           },
	{"Ukraine","380","810"                                                         },
	{"United Arab Emirates","971","00"                                             },
	{"United Kingdom","44","00"                                                    },
	{"United States Virgin Islands","1","011"                                      },
	{"United States of America","1","011"                                          },
	{"Uruguay","598","00"                                                          },
	{"Uzbekistan","7","810"                                                        },
	{"Vanuatu","678","00"                                                          },
	{"Vatican City","39","00"                                                      },
	{"Venezuela","58","00"                                                         },
	{"Vietnam","84","00"                                                           },
	{"Wallis and Futuna Islands","681","19"                                        },
	{"Western Samoa","685","0"                                                     },
	{"Yemen","967","00"                                                            },
	{"Yugoslavia","381","99"                                                       },
	{"Zaire","243","00"                                                            },
	{"Zambia","260","00"                                                           },
	{"Zimbabwe","263","110"                                                        }
};

/////////////////////////////////////////////////////////////////////////////
// CStringList
// Utility string list class.

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
			Util::Trim(csName);
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
// CCountryNameList
// List of countries.

CCountryNameList::CCountryNameList(LPCSTR pszCountries /*=NULL*/, char cSeparator /*='|'*/) :
	CNameList(pszCountries, cSeparator)
{
	m_fAllCountries = FALSE; // used by FindCountry()!
	m_fAllCountries = FindCountry("All");
}
 
CCountryNameList::~CCountryNameList()
{
}

void CCountryNameList::SetNames(LPCSTR pszNames, char cSeparator /*='|'*/)
{
	m_fAllCountries = FALSE; // used by FindCountry()!
	CNameList::SetNames(pszNames, cSeparator);
	m_fAllCountries = FindCountry("All");
}

BOOL CCountryNameList::FindCountry(LPCSTR pszCountry)
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
				if (stricmp(GetAt(nIndex), pszCountry) == 0)
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
// COfficeCountryNameList
// List of countries.

BOOL CRegistrationOfficeData::Load(LPCSTR pszOfficeName, CIniFile& IniFile)
{
	BOOL fResult = FALSE;

	m_pszCountry = NULL;
	m_pszLocalCountries = NULL;
	m_pszLocalMailAddress = NULL;
	m_pszLocalModemNumber = NULL;
	m_pszLocalPhoneNumber = NULL;
	m_pszLocalFaxNumber = NULL;
	m_pszLocalHelpNumber = NULL;
	m_pszForeignCountries = NULL;
	m_pszForeignMailAddress = NULL;
	m_pszForeignModemNumber = NULL;
	m_pszForeignPhoneNumber = NULL;
	m_pszForeignFaxNumber = NULL;
	m_pszForeignHelpNumber = NULL;

	CString csFullOfficeName;
	TRY
	{
		csFullOfficeName = "RegistrationOffice";
		csFullOfficeName += pszOfficeName;
		m_pszCountry = NewString(IniFile.GetString(csFullOfficeName, "Country", ""));
		m_pszLocalCountries = NewString(IniFile.GetString(csFullOfficeName, "LocalCountries", ""));
		m_pszLocalMailAddress = NewString(IniFile.GetString(csFullOfficeName, "LocalMailAddress", ""));
		m_pszLocalModemNumber = NewString(IniFile.GetString(csFullOfficeName, "LocalModemNumber", ""));
		m_fLocalModemFree = IniFile.GetInteger(csFullOfficeName, "LocalModemFree", FALSE);
		m_pszLocalPhoneNumber = NewString(IniFile.GetString(csFullOfficeName, "LocalPhoneNumber", ""));
		m_fLocalPhoneFree = IniFile.GetInteger(csFullOfficeName, "LocalPhoneFree", FALSE);
		m_pszLocalFaxNumber = NewString(IniFile.GetString(csFullOfficeName, "LocalFaxNumber", ""));
		m_fLocalFaxFree = IniFile.GetInteger(csFullOfficeName, "LocalFaxFree", FALSE);
		m_pszLocalHelpNumber = NewString(IniFile.GetString(csFullOfficeName, "LocalHelpNumber", ""));
		m_fLocalHelpFree = IniFile.GetInteger(csFullOfficeName, "LocalHelpFree", FALSE);
		m_pszForeignCountries = NewString(IniFile.GetString(csFullOfficeName, "ForeignCountries", ""));
		m_pszForeignMailAddress = NewString(IniFile.GetString(csFullOfficeName, "ForeignMailAddress", ""));
		m_pszForeignModemNumber = NewString(IniFile.GetString(csFullOfficeName, "ForeignModemNumber", ""));
		m_fForeignModemFree = IniFile.GetInteger(csFullOfficeName, "ForeignModemFree", FALSE);
		m_pszForeignPhoneNumber = NewString(IniFile.GetString(csFullOfficeName, "ForeignPhoneNumber", ""));
		m_fForeignPhoneFree = IniFile.GetInteger(csFullOfficeName, "ForeignPhoneFree", FALSE);
		m_pszForeignFaxNumber = NewString(IniFile.GetString(csFullOfficeName, "ForeignFaxNumber", ""));
		m_fForeignFaxFree = IniFile.GetInteger(csFullOfficeName, "ForeignFaxFree", FALSE);
		m_pszForeignHelpNumber = NewString(IniFile.GetString(csFullOfficeName, "ForeignHelpNumber", ""));
		m_fForeignHelpFree = IniFile.GetInteger(csFullOfficeName, "ForeignHelpFree", FALSE);
		fResult = TRUE;
	}
	END_TRY

	if (!fResult)
	{
		Free();
	}

	return fResult;
}

void CRegistrationOfficeData::Free(void)
{
	FreeString(m_pszCountry);
	FreeString(m_pszLocalCountries);
	FreeString(m_pszLocalMailAddress);
	FreeString(m_pszLocalModemNumber);
	FreeString(m_pszLocalPhoneNumber);
	FreeString(m_pszLocalFaxNumber);
	FreeString(m_pszLocalHelpNumber);
	FreeString(m_pszForeignCountries);
	FreeString(m_pszForeignMailAddress);
	FreeString(m_pszForeignModemNumber);
	FreeString(m_pszForeignPhoneNumber);
	FreeString(m_pszForeignFaxNumber);
	FreeString(m_pszForeignHelpNumber);
}

LPSTR CRegistrationOfficeData::NewString(const CString& csString)
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

void CRegistrationOfficeData::FreeString(LPSTR pszString)
{
	delete pszString;
}

/////////////////////////////////////////////////////////////////////////////
// CCountryRegistrationParameters
// Registration parameters for a given country.

CRegistrationOfficeData CCountryRegistrationParameters::DefaultOfficeData =
{
	// Office Country
	"United States of America",

	// Local Information
	"United States of America|Canada",
	"Mindscape|Registration Dept. @RegistrationCode@|1351 Ocean Avenue|Emeryville, CA 94608-1128",
	"800-338-4321", TRUE,			// modem
	"800-859-3387", TRUE,			// phone
	"800-933-0571", TRUE,			// fax
	"800-888-9078", TRUE,			// help

	// Foreign Information
	"All",
	"Mindscape|Registration Dept. @RegistrationCode@|1351 Ocean Avenue|Emeryville, CA 94608-1128|U.S.A.",
	"510-652-9841", FALSE,			// modem
	"",             FALSE,			// phone
	"510-652-5040", FALSE,			// fax
	"",             FALSE,			// help
};


CCountryRegistrationParameters::CCountryRegistrationParameters()
{
	m_pCountry = NULL;
	m_fIsLocal = FALSE;
	m_fModemFree = FALSE;
	m_fPhoneFree = FALSE;
	m_fFaxFree = FALSE;
	m_fHelpFree = FALSE;
}

CCountryRegistrationParameters::~CCountryRegistrationParameters()
{
}

CCountryRegistrationParameters* CCountryRegistrationParameters::Create(CCountryTableEntry* pCountry, CRegistrationOfficeData* pData /*=NULL*/)
{
	CCountryRegistrationParameters* pParameters = NULL;

	if (pData == NULL)
	{
		pData = &DefaultOfficeData;
	}

	ASSERT(pCountry != NULL);
	ASSERT(pData != NULL);

	if ((pCountry != NULL) && (pData != NULL))
	{
		TRY
		{
			// Check if the country is local, foreign, or neither.
			CCountryNameList LocalCountries(pData->m_pszLocalCountries);
			CCountryNameList ForeignCountries(pData->m_pszForeignCountries);

			BOOL fLocal = FALSE;
			BOOL fForeign = FALSE;

			if ((fLocal = LocalCountries.FindCountry(pCountry->GetName())) == FALSE)
			{
				// No local, try foreign.
				fForeign = ForeignCountries.FindCountry(pCountry->GetName());
			}

			// Must be local or foreign.
			if (fLocal || fForeign)
			{
				// Allocate a place to put the parameters.
				pParameters = new CCountryRegistrationParameters;
				if (pParameters != NULL)
				{
					pParameters->m_pCountry = pCountry;
					pParameters->m_pOfficeCountry = CCountryTableEntry::GetCountryEntryByName(pData->m_pszCountry);
					if (pParameters->m_pOfficeCountry == NULL)
					{
						AfxThrowUserException();
					}
					pParameters->m_fIsLocal = fLocal;

					if (pParameters->m_fIsLocal)
					{
						// Local Country.
						pParameters->m_MailAddress.SetNames(pData->m_pszLocalMailAddress);
						pParameters->m_csModemNumber = pData->m_pszLocalModemNumber;
						pParameters->m_csPhoneNumber = pData->m_pszLocalPhoneNumber;
						pParameters->m_csFaxNumber = pData->m_pszLocalFaxNumber;
						pParameters->m_csHelpNumber = pData->m_pszLocalHelpNumber;
						pParameters->m_fModemFree = pData->m_fLocalModemFree;
						pParameters->m_fPhoneFree = pData->m_fLocalPhoneFree;
						pParameters->m_fFaxFree = pData->m_fLocalFaxFree;
						pParameters->m_fHelpFree = pData->m_fLocalHelpFree;
					}
					else
					{
						// Foreign Country.
						pParameters->m_MailAddress.SetNames(pData->m_pszForeignMailAddress);
						pParameters->m_csModemNumber = pData->m_pszForeignModemNumber;
						pParameters->m_csPhoneNumber = pData->m_pszForeignPhoneNumber;
						pParameters->m_csFaxNumber = pData->m_pszForeignFaxNumber;
						pParameters->m_csHelpNumber = pData->m_pszForeignHelpNumber;
						pParameters->m_fModemFree = pData->m_fForeignModemFree;
						pParameters->m_fPhoneFree = pData->m_fForeignPhoneFree;
						pParameters->m_fFaxFree = pData->m_fForeignFaxFree;
						pParameters->m_fHelpFree = pData->m_fForeignHelpFree;

						// Figure out the dialing prefix.
						pParameters->m_csDialingPrefix = pParameters->m_pCountry->m_pszDialingPrefix;
						pParameters->m_csDialingPrefix += " ";
						pParameters->m_csDialingPrefix += pParameters->m_pOfficeCountry->m_pszCountryCode;
						pParameters->m_csDialingPrefix += " ";
					}

					GetWebSite(pCountry->GetName(), pParameters->m_csWebSite);
				}
			}
		}
		CATCH_ALL(e)
		{
			delete pParameters;
			pParameters = NULL;
		}
		END_CATCH_ALL
	}

	return pParameters;
}

CCountryRegistrationParameters* CCountryRegistrationParameters::Create(CCountryTableEntry* pCountry, CIniFile& IniFile)
{
	CCountryRegistrationParameters* pParameters = NULL;

	ASSERT(pCountry != NULL);
	if (pCountry != NULL)
	{
		// Try to find an office which is local to the given country.
		LPSTR pszOffices = NULL;
		CString csOffice;
		CCountryNameList Countries;
		TRY
		{
			pszOffices = IniFile.GetSectionKeys("RegistrationOffices");
			if (pszOffices != NULL)
			{
				// Look for an office which is local to the country.
				LPSTR pszOffice = pszOffices;
				while (*pszOffice != '\0')
				{
					csOffice = "RegistrationOffice";
					csOffice += pszOffice;
					Countries.SetNames(IniFile.GetString(csOffice, "LocalCountries", ""));
					if (Countries.FindCountry(pCountry->GetName()))
					{
						// Found a local office.
						TRACE("Found local office |%s| for country |%s|\n", pszOffice, pCountry->GetName());
						break;
					}

					// Try the next office.
					pszOffice += strlen(pszOffice)+1;
				}

				// If we did not find a local office, try to find
				// a foreign office.
				if (*pszOffice == '\0')
				{
					pszOffice = pszOffices;
					while (*pszOffice != '\0')
					{
						csOffice = "RegistrationOffice";
						csOffice += pszOffice;
						Countries.SetNames(IniFile.GetString(csOffice, "ForeignCountries", ""));
						if (Countries.FindCountry(pCountry->GetName()))
						{
							// Found a foreign office.
							TRACE("Found foreign office |%s| for country |%s|\n", pszOffice, pCountry->GetName());
							break;
						}

						// Try the next office.
						pszOffice += strlen(pszOffice)+1;
					}
				}

				// Check if we found any office.
				if (*pszOffice == '\0')
				{
					TRACE("Could not find office for country |%s|\n", pCountry->GetName());
				}
				else
				{
					// Build registration data for the office
					// that was found.
					CRegistrationOfficeData OfficeData;
					TRY
					{
						if (OfficeData.Load(pszOffice, IniFile))
						{
							// Attempt to create the registration
							// parameters from the country and the
							// office data that was found.
							pParameters = Create(pCountry, &OfficeData);

							// Free the data allocated by Load().
							OfficeData.Free();
						}
					}
					END_TRY
				}
			}
		}
		CATCH_ALL(e)
		{
			delete pParameters;
			pParameters = NULL;
		}
		END_CATCH_ALL

		delete pszOffices;
	}

	return pParameters;
}

// Figure out the web site to which to refer the user. This information is
// extracted from the [WebSite] section of MSREG.INI.
void CCountryRegistrationParameters::GetWebSite(LPCSTR pszCountryName, CString& csWebSite)
{
	LPSTR pszWebSites = NULL;
	CCountryNameList Countries;

	TRY
	{
		csWebSite.Empty();

		CIniFile& IniFile = ((CMsregDllApp*)AfxGetApp())->IniFile();

		pszWebSites = IniFile.GetSectionKeys("WebSite");
		if (pszWebSites != NULL)
		{
			LPCSTR pszWebSite = pszWebSites;
			while (*pszWebSite != NULL)
			{
				Countries.SetNames(IniFile.GetString("WebSite", pszWebSite, ""));
				if (Countries.FindCountry(pszCountryName))
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

	TRACE("WebSite = |%s|\n", csWebSite);
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
}

/////////////////////////////////////////////////////////////////////////////
// CCountryTableEntry
// An entry in the country table.

CCountryTableEntry* CCountryTableEntry::GetCountryEntryByName(LPCSTR pszName)
{
	CCountryTableEntry* pCountry = NULL;

	for (int nEntry = 0; nEntry < sizeof(CountryTable)/sizeof(CountryTable[0]); nEntry++)
	{
		CCountryTableEntry* pEntry = &(CountryTable[nEntry]);
		if (stricmp(pEntry->GetName(), pszName) == 0)
		{
			pCountry = pEntry;
			break;
		}
	}

	return pCountry;
}

BOOL CCountryTableEntry::IsUnitedStatesLocal(void)
{
	return (stricmp(GetName(), "United States of America") == 0)
	    || (stricmp(GetName(), "Canada") == 0);
}

CCountryRegistrationParameters* CCountryTableEntry::CreateRegistrationParameters(CIniFile& IniFile)
{
	CCountryRegistrationParameters* pParameters = NULL;
	
	// Attempt to get the parameters from the IniFile.
	if ((pParameters = CCountryRegistrationParameters::Create(this, IniFile)) == NULL)
	{
		// Use default office.
		pParameters = CCountryRegistrationParameters::Create(this);
	}

	ASSERT(pParameters != NULL);

	return pParameters;
}

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
