/*
// $Workfile: INTLDLL.H $
// $Revision: 1 $
// $Date: 3/03/99 6:07p $
//
// Copyright © 1996 MicroLogic Software, Inc.
// All rights reserved.
*/

/*
// Revision History:
//
// $Log: /PM8/App/LICDLL/INTLDLL.H $
// 
// 1     3/03/99 6:07p Gbeddow
// 
//    Rev 1.0   14 Aug 1997 15:36:12   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 10:20:22   Fred
// Initial revision.
// 
//    Rev 1.0   10 Jun 1997 14:10:42   Fred
// Initial revision.
// 
//    Rev 1.3   07 Oct 1996 14:03:24   FRED
// Added GetTaxInformation()
// 
//    Rev 1.2   04 Oct 1996 15:06:52   FRED
// Added GetWebSite()
// 
//    Rev 1.1   04 Oct 1996 10:05:42   FRED
// Added GetCountryName(); Country Id's to data
// 
//    Rev 1.0   03 Oct 1996 10:31:00   FRED
// Initial revision.
*/

class CInternationalInformationDll;
class CInternationalData;

#ifndef __INTLDLL_H__
#define __INTLDLL_H__

/////////////////////////////////////////////////////////////////////////////
// CInternationalInformationDll
// This class which wraps INTLxx.DLL.

class CInternationalInformationDll
{
public:
	CInternationalInformationDll(LPCSTR pszDirectory);
	virtual ~CInternationalInformationDll();

public:
	BOOL GetInternationalData(WORD wCountryId, LPCSTR pszIniFile, CInternationalData* pData);
	BOOL FillCountryComboBox(HWND hComboBox, WORD wSelectCountryId = 0);
	BOOL GetCountryName(WORD wCountryId, CString& csCountryName);
	BOOL GetWebSite(WORD wCountryId, LPCSTR pszIniFile, CString& csWebSite);
	BOOL GetTaxInformation(LPCSTR pszIniFile, CString& csTaxInformation);

protected:
	BOOL Open(void);
	void Close(void);

protected:
	CString m_csDirectory;
	HINSTANCE m_hDll;
};

/////////////////////////////////////////////////////////////////////////////
// CInternationalData
// This class is used to pass international information back to the client.

class CInternationalData
{
public:
	struct PhoneNumber
	{
		char m_szOffice[64];
		WORD m_wCountry;
		BOOL m_fUnitedStatesLocal;
		BOOL m_fLocal;
		BOOL m_fFree;
		char m_szNumber[64];
		char m_szPrefix[64];
		BOOL Exists(void)
			{ return m_szNumber[0] != '\0'; }
	};

	struct MailAddress
	{
		char m_szOffice[64];
		WORD m_wCountry;
		BOOL m_fUnitedStatesLocal;
		BOOL m_fLocal;
		char m_szAddress[256];
		BOOL Exists(void)
			{ return m_szAddress[0] != '\0'; }
	};

	struct Registration
	{
		PhoneNumber m_Modem;
		PhoneNumber m_Phone;
		PhoneNumber m_Fax;
		PhoneNumber m_Help;
		MailAddress m_Mail;
	};

	struct InstantSales
	{
		PhoneNumber m_Modem;
		PhoneNumber m_Phone;
		PhoneNumber m_Fax;
		PhoneNumber m_Help;
		MailAddress m_Mail;
	};

	WORD m_wCountry;
	BOOL m_fUnitedStatesLocal;
	Registration m_Registration;
	InstantSales m_InstantSales;
	char m_szWebSite[256];
	char m_szTaxInformation[256];
	BOOL Exists(void)
		{ return m_wCountry != 0; }
	BOOL WebSiteExists(void)
		{ return m_szWebSite[0] != '\0'; }
	BOOL TaxInformationExists(void)
		{ return m_szTaxInformation[0] != '\0'; }
};

// These values enumerate all the known countries. They must
// match the values listed in RESOURCE.H

enum
{
	COUNTRY_Afghanistan                  =   1000,
	COUNTRY_Albania                      =   1001,
	COUNTRY_Algeria                      =   1002,
	COUNTRY_AmericanSamoa                =   1003,
	COUNTRY_Andorra                      =   1004,
	COUNTRY_Angola                       =   1005,
	COUNTRY_Anguilla                     =   1006,
	COUNTRY_Antigua                      =   1007,
	COUNTRY_Argentina                    =   1008,
	COUNTRY_Armenia                      =   1009,
	COUNTRY_Aruba                        =   1010,
	COUNTRY_AscensionIsland              =   1011,
	COUNTRY_Australia                    =   1012,
	COUNTRY_AustralianAntarcticTerritory =   1013,
	COUNTRY_Austria                      =   1014,
	COUNTRY_Azerbaijan                   =   1015,
	COUNTRY_Bahamas                      =   1016,
	COUNTRY_Bahrain                      =   1017,
	COUNTRY_Bangladesh                   =   1018,
	COUNTRY_Barbados                     =   1019,
	COUNTRY_Barbuda                      =   1020,
	COUNTRY_Belarus                      =   1021,
	COUNTRY_Belgium                      =   1022,
	COUNTRY_Belize                       =   1023,
	COUNTRY_Benin                        =   1024,
	COUNTRY_Bermuda                      =   1025,
	COUNTRY_Bhutan                       =   1026,
	COUNTRY_Bolivia                      =   1027,
	COUNTRY_BosniaAndHerzegovina         =   1028,
	COUNTRY_Botswana                     =   1029,
	COUNTRY_Brazil                       =   1030,
	COUNTRY_BritishVirginIslands         =   1031,
	COUNTRY_Brunei                       =   1032,
	COUNTRY_Bulgaria                     =   1033,
	COUNTRY_BurkinaFaso                  =   1034,
	COUNTRY_Burundi                      =   1035,
	COUNTRY_Cambodia                     =   1036,
	COUNTRY_Cameroon                     =   1037,
	COUNTRY_Canada                       =   1038,
	COUNTRY_CapeVerdeIslands             =   1039,
	COUNTRY_CaymanIslands                =   1040,
	COUNTRY_CentralAfricanRepublic       =   1041,
	COUNTRY_Chad                         =   1042,
	COUNTRY_Chile                        =   1043,
	COUNTRY_China                        =   1044,
	COUNTRY_ChristmasIsland              =   1045,
	COUNTRY_CocosKeelingIslands          =   1046,
	COUNTRY_Colombia                     =   1047,
	COUNTRY_Comoros                      =   1048,
	COUNTRY_Congo                        =   1049,
	COUNTRY_CookIslands                  =   1050,
	COUNTRY_CostaRica                    =   1051,
	COUNTRY_Croatia                      =   1052,
	COUNTRY_Cuba                         =   1053,
	COUNTRY_Cyprus                       =   1054,
	COUNTRY_CzechRepublic                =   1055,
	COUNTRY_Denmark                      =   1056,
	COUNTRY_DiegoGarcia                  =   1057,
	COUNTRY_Djibouti                     =   1058,
	COUNTRY_Dominica                     =   1059,
	COUNTRY_DominicanRepublic            =   1060,
	COUNTRY_Ecuador                      =   1061,
	COUNTRY_Egypt                        =   1062,
	COUNTRY_ElSalvador                   =   1063,
	COUNTRY_EquatorialGuinea             =   1064,
	COUNTRY_Eritrea                      =   1065,
	COUNTRY_Estonia                      =   1066,
	COUNTRY_Ethiopia                     =   1067,
	COUNTRY_FYROM                        =   1068,
	COUNTRY_FaeroeIslands                =   1069,
	COUNTRY_FalklandIslands              =   1070,
	COUNTRY_FijiIslands                  =   1071,
	COUNTRY_Finland                      =   1072,
	COUNTRY_France                       =   1073,
	COUNTRY_FrenchAntilles               =   1074,
	COUNTRY_FrenchGuiana                 =   1075,
	COUNTRY_FrenchPolynesia              =   1076,
	COUNTRY_Gabon                        =   1077,
	COUNTRY_Gambia                       =   1078,
	COUNTRY_Georgia                      =   1079,
	COUNTRY_Germany                      =   1080,
	COUNTRY_Ghana                        =   1081,
	COUNTRY_Gibraltar                    =   1082,
	COUNTRY_Greece                       =   1083,
	COUNTRY_Greenland                    =   1084,
	COUNTRY_Grenada                      =   1085,
	COUNTRY_Guadeloupe                   =   1086,
	COUNTRY_Guam                         =   1087,
	COUNTRY_GuantanamoBay                =   1088,
	COUNTRY_Guatemala                    =   1089,
	COUNTRY_Guinea                       =   1090,
	COUNTRY_GuineaBissau                 =   1091,
	COUNTRY_Guyana                       =   1092,
	COUNTRY_Haiti                        =   1093,
	COUNTRY_Honduras                     =   1094,
	COUNTRY_HongKong                     =   1095,
	COUNTRY_Hungary                      =   1096,
	COUNTRY_Iceland                      =   1097,
	COUNTRY_India                        =   1098,
	COUNTRY_Indonesia                    =   1099,
	COUNTRY_Iran                         =   1100,
	COUNTRY_Iraq                         =   1101,
	COUNTRY_Ireland                      =   1102,
	COUNTRY_Israel                       =   1103,
	COUNTRY_Italy                        =   1104,
	COUNTRY_IvoryCoast                   =   1105,
	COUNTRY_Jamaica                      =   1106,
	COUNTRY_Japan                        =   1107,
	COUNTRY_Jordan                       =   1108,
	COUNTRY_Kazakhstan                   =   1109,
	COUNTRY_Kenya                        =   1110,
	COUNTRY_KiribatiRepublic             =   1111,
	COUNTRY_KoreaNorth                   =   1112,
	COUNTRY_KoreaSouth                   =   1113,
	COUNTRY_Kuwait                       =   1114,
	COUNTRY_Kyrgyzstan                   =   1115,
	COUNTRY_Laos                         =   1116,
	COUNTRY_Latvia                       =   1117,
	COUNTRY_Lebanon                      =   1118,
	COUNTRY_Lesotho                      =   1119,
	COUNTRY_Liberia                      =   1120,
	COUNTRY_Libya                        =   1121,
	COUNTRY_Liechtenstein                =   1122,
	COUNTRY_Lithuania                    =   1123,
	COUNTRY_Luxembourg                   =   1124,
	COUNTRY_Macao                        =   1125,
	COUNTRY_Madagascar                   =   1126,
	COUNTRY_Malawi                       =   1127,
	COUNTRY_Malaysia                     =   1128,
	COUNTRY_Maldives                     =   1129,
	COUNTRY_Mali                         =   1130,
	COUNTRY_Malta                        =   1131,
	COUNTRY_MarshallIslands              =   1132,
	COUNTRY_Martinique                   =   1133,
	COUNTRY_Mauritania                   =   1134,
	COUNTRY_Mauritius                    =   1135,
	COUNTRY_MayotteIsland                =   1136,
	COUNTRY_Mexico                       =   1137,
	COUNTRY_Micronesia                   =   1138,
	COUNTRY_Moldova                      =   1139,
	COUNTRY_Monaco                       =   1140,
	COUNTRY_Mongolia                     =   1141,
	COUNTRY_Montserrat                   =   1142,
	COUNTRY_Morocco                      =   1143,
	COUNTRY_Mozambique                   =   1144,
	COUNTRY_Myanmar                      =   1145,
	COUNTRY_Namibia                      =   1146,
	COUNTRY_Nauru                        =   1147,
	COUNTRY_Nepal                        =   1148,
	COUNTRY_Netherlands                  =   1149,
	COUNTRY_NetherlandsAntilles          =   1150,
	COUNTRY_Nevis                        =   1151,
	COUNTRY_NewCaledonia                 =   1152,
	COUNTRY_NewZealand                   =   1153,
	COUNTRY_Nicaragua                    =   1154,
	COUNTRY_Niger                        =   1155,
	COUNTRY_Nigeria                      =   1156,
	COUNTRY_Niue                         =   1157,
	COUNTRY_NorfolkIsland                =   1158,
	COUNTRY_Norway                       =   1159,
	COUNTRY_Oman                         =   1160,
	COUNTRY_Pakistan                     =   1161,
	COUNTRY_Palau                        =   1162,
	COUNTRY_Panama                       =   1163,
	COUNTRY_PapuaNewGuinea               =   1164,
	COUNTRY_Paraguay                     =   1165,
	COUNTRY_Peru                         =   1166,
	COUNTRY_Philippines                  =   1167,
	COUNTRY_Poland                       =   1168,
	COUNTRY_Portugal                     =   1169,
	COUNTRY_PuertoRico                   =   1170,
	COUNTRY_Qatar                        =   1171,
	COUNTRY_ReunionIsland                =   1172,
	COUNTRY_Romania                      =   1173,
	COUNTRY_RotaIsland                   =   1174,
	COUNTRY_Russia                       =   1175,
	COUNTRY_Rwanda                       =   1176,
	COUNTRY_SaintLucia                   =   1177,
	COUNTRY_SaipanIsland                 =   1178,
	COUNTRY_SanMarino                    =   1179,
	COUNTRY_SaoTomeAndPrincipe           =   1180,
	COUNTRY_SaudiArabia                  =   1181,
	COUNTRY_SenegalRepublic              =   1182,
	COUNTRY_SeychelleIslands             =   1183,
	COUNTRY_SierraLeone                  =   1184,
	COUNTRY_Singapore                    =   1185,
	COUNTRY_SlovakRepublic               =   1186,
	COUNTRY_Slovenia                     =   1187,
	COUNTRY_SolomonIslands               =   1188,
	COUNTRY_Somalia                      =   1189,
	COUNTRY_SouthAfrica                  =   1190,
	COUNTRY_Spain                        =   1191,
	COUNTRY_SriLanka                     =   1192,
	COUNTRY_StHelena                     =   1193,
	COUNTRY_StKitts                      =   1194,
	COUNTRY_StPierreAndMiquelon          =   1195,
	COUNTRY_StVincentAndTheGrenadines    =   1196,
	COUNTRY_Sudan                        =   1197,
	COUNTRY_Suriname                     =   1198,
	COUNTRY_Swaziland                    =   1199,
	COUNTRY_Sweden                       =   1200,
	COUNTRY_Switzerland                  =   1201,
	COUNTRY_Syria                        =   1202,
	COUNTRY_TaiwanRepublicOfChina        =   1203,
	COUNTRY_Tajikistan                   =   1204,
	COUNTRY_Tanzania                     =   1205,
	COUNTRY_Thailand                     =   1206,
	COUNTRY_TinianIsland                 =   1207,
	COUNTRY_Togo                         =   1208,
	COUNTRY_Tokelau                      =   1209,
	COUNTRY_Tonga                        =   1210,
	COUNTRY_TrinidadAndTobago            =   1211,
	COUNTRY_Tunisia                      =   1212,
	COUNTRY_Turkey                       =   1213,
	COUNTRY_Turkmenistan                 =   1214,
	COUNTRY_TurksAndCaicosIslands        =   1215,
	COUNTRY_Tuvalu                       =   1216,
	COUNTRY_Uganda                       =   1217,
	COUNTRY_Ukraine                      =   1218,
	COUNTRY_UnitedArabEmirates           =   1219,
	COUNTRY_UnitedKingdom                =   1220,
	COUNTRY_UnitedStatesVirginIslands    =   1221,
	COUNTRY_UnitedStatesOfAmerica        =   1222,
	COUNTRY_Uruguay                      =   1223,
	COUNTRY_Uzbekistan                   =   1224,
	COUNTRY_Vanuatu                      =   1225,
	COUNTRY_VaticanCity                  =   1226,
	COUNTRY_Venezuela                    =   1227,
	COUNTRY_Vietnam                      =   1228,
	COUNTRY_WallisAndFutunaIslands       =   1229,
	COUNTRY_WesternSamoa                 =   1230,
	COUNTRY_Yemen                        =   1231,
	COUNTRY_Yugoslavia                   =   1232,
	COUNTRY_Zaire                        =   1233,
	COUNTRY_Zambia                       =   1234,
	COUNTRY_Zimbabwe                     =   1235
};

#endif
