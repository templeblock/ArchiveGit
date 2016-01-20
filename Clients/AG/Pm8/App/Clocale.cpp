#include "stdafx.h"
#include <locale.h>
#include <math.h>
#include "inifile.h"
#include "calinfo.h"
#include "resource.h"
#include "flatdb.h"
#include "clocale.h"

// initialize static member variables
CString CLocale::m_csLocaleName = LANGUAGE_DEFAULT;
LCID	CLocale::m_LocaleID		= LOCALE_ID_DEFAULT;

// TODO probably create a separate language.ini file, but use msreg.ini for now
CString CLocale::m_csINIFileName = "MSREG.INI";

// TODO need to get a real list for each language
//LPCSTR CensorList[] = { "HITLER", "NAZI", "BERKINAU", "ADOLF", NULL};

unsigned char MapUpper[128] = {
0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89, 0x8A, 0x8B, 0x8C, 0x8D, 0x8E, 0x8F,
0x90, 0x91, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x98, 0x99, 0x9A, 0x9B, 0x8C, 0x9D, 0x9E, 0x9F,
0xA0, 0xA1, 0xA2, 0xA3, 0xA4, 0xA5, 0xA6, 0xA7, 0xA8, 0xA9, 0xAA, 0xAB, 0xAC, 0xAD, 0xAE, 0xAF,
0xB0, 0xB1, 0xB2, 0xB3, 0xB4, 0xB5, 0xB6, 0xB7, 0xB8, 0xB9, 0xBA, 0xBB, 0xBC, 0xBD, 0xBE, 0xBF,
0x41, 0x41, 0x41, 0x41, 0x41, 0x41, 0xC6, 0x43, 0x45, 0x45, 0x45, 0x45, 0x49, 0x49, 0x49, 0x49,
0x44, 0x4E, 0x4F, 0x4F, 0x4F, 0x4F, 0x4F, 0xD7, 0x4F, 0x55, 0x55, 0x55, 0x55, 0x59, 0xDE, 0xDF,
0x41, 0x41, 0x41, 0x41, 0x41, 0x41, 0xC6, 0x43, 0x45, 0x45, 0x45, 0x45, 0x49, 0x49, 0x49, 0x49,
0x44, 0x4E, 0x4F, 0x4F, 0x4F, 0x4F, 0x4F, 0xF7, 0x4F, 0x55, 0x55, 0x55, 0x55, 0x59, 0xFE, 0x59
};

void CLocale::MakeUpper(CString *Str)
{
	LPSTR lpstr = Str->GetBuffer(Str->GetLength()*2+1);
	if (m_LocaleID != LOCALE_ID_FRENCH)
	{
		CharUpper(lpstr);
	}
	else
	{
		int i = 0;
		unsigned char ch;

		while (lpstr[i])
		{
			ch = (unsigned char) lpstr[i];
			if (ch <= 0x7F)
				lpstr[i] = toupper(lpstr[i]);
			else
				lpstr[i] = MapUpper[ch-0x80];
     		      i++;
		}	
	}
	Str->ReleaseBuffer(-1);
}

BOOL CLocale::SetLocale(LCID LocaleID)
{
	TCHAR Buf[4];
	if (GetLocaleInfo(LocaleID, LOCALE_SABBREVLANGNAME, Buf, sizeof(Buf)))
	{
		TCHAR *LocName = _tsetlocale(LC_ALL, Buf);
		if (LocName)
		{
			m_csLocaleName = LocName;
			m_LocaleID = LocaleID;
		}
	}
	return (LocaleID == m_LocaleID);
}

TCHAR CLocale::DecimalPoint()
{
// TODO should really allow multiple character decimal point markers
// although I'm not sure if any languages use them.
	TCHAR Buf[10];
	if (GetLocaleInfo(m_LocaleID, LOCALE_SDECIMAL, Buf, sizeof(Buf)))
	{
		if (strlen(Buf) == 1)
			return (Buf[0]);
	}
	// default
	return (TCHAR('.'));
}

TCHAR CLocale::MinAlphaUpper()
{
	// TODO do this properly, this won't work for Japanese
	// we should go through the characters from the beginning
	// until we reach the first upper case character, probably
	// using isupper()
	// For Japanese this function is probably no use anyway, since
	// there is no real concept of upper case characters
	return TCHAR('A');
}

TCHAR CLocale::MaxAlphaUpper()
{
	// TODO do this properly, this won't work for Japanese
	// we should go through the characters backwards from the end
	// until we reach the first upper case character, probably
	// using isupper()
	// For Japanese this function is probably no use anyway, since
	// there is no real concept of upper case characters
	return TCHAR(0xFF);
}

double CLocale::ConvertFromInches(double dMeasure)
{
	if (UseMetric())
		return dMeasure*INCHES_TO_CENTIMETRES;
	else
		return dMeasure;	
}

double CLocale::ConvertToInches(double dMeasure)
{
	if (UseMetric())
		return dMeasure/INCHES_TO_CENTIMETRES;
	else
		return dMeasure;	
}

int CLocale::ConvertPrecisionFromInches(int nPrecision)
{
	if (UseMetric())
		return nPrecision/2;
	else
		return nPrecision;	
}

BOOL CLocale::WithHelpNewsletter()
{
	if (m_LocaleID == LOCALE_ID_FRENCH)
	{
		return FALSE;
	}
	else if (m_LocaleID == LOCALE_ID_GERMAN)
	{
		return FALSE;
	}
	else
	{
		return TRUE;
	}
}

BOOL CLocale::WithHelpWebPage()
{
	if (m_LocaleID == LOCALE_ID_FRENCH)
	{
		return FALSE;
	}
	else if (m_LocaleID == LOCALE_ID_GERMAN)
	{
		return FALSE;
	}
	else return TRUE;
}

int CLocale::WithHelpPosterCategoryConvert(int nCategory)
{
	if (m_LocaleID == LOCALE_ID_FRENCH)
	{
		switch (nCategory)
		{
			case 0: return 1;
			case 1: return 3;
			default: return nCategory;
		}
	}
	else if (m_LocaleID == LOCALE_ID_GERMAN)
	{
		switch (nCategory)
		{
			case 0: return 1;
			case 1: return 3;
			default: return nCategory;
		}
	}
	else return nCategory;
}


#ifndef INSTALL
CString CLocale::HandleBrowserCaption(const CString& csINISection, const CString& csCaption, const CString& csFileName)
{
   CString csIniFile = GetGlobalPathManager()->ExpandPath(m_csINIFileName);
   CIniFile IniFile(csIniFile);
   int nShowNames = IniFile.GetInteger(csINISection, "ShowFriendlyNames",1);
   switch (nShowNames)
   {
	case 0:  // no friendly names so leave empty;
               return CString("");
      break;
      case 2:
		   return csFileName;			
      break;
      case 1:
	default:
		   return csCaption;
   }
}

#if 0
BOOL CLocale::Censor(const CString& csCaption, const CString& csFile, const CString& csKeywords)
{
// for now just use one censor list for all locales
   
   LPSTR lpszTemp;
   CString csTemp = csCaption+" "+csFile+" "+csKeywords;
   lpszTemp = csTemp.GetBuffer(csTemp.GetLength()*2);
   CharUpper(lpszTemp);
   csTemp.ReleaseBuffer(-1);
  
   for (int i = 0; CensorList[i]; i++)
   {
      if (csTemp.Find(CensorList[i]) != -1)
      {
         return TRUE;
      }
   }
   return FALSE;
}
#endif

BOOL CLocale::UseCapitalizeInCategoryBuild()
{
   CString csIniFile = GetGlobalPathManager()->ExpandPath(m_csINIFileName);
   CIniFile IniFile(csIniFile);
   int nShowNames = IniFile.GetInteger("Locale", "UseCapitalizeInCategoryBuild",1);
   return (nShowNames == 1);
}
#endif

BOOL CLocale::operator==(LCID L)
{
	return (m_LocaleID==L);
}

BOOL CLocale::operator!=(LCID L)
{
	return (m_LocaleID != L);
}
    
BOOL CLocale::UseMetric()
{
	char Buf[2];
	if (GetLocaleInfo(m_LocaleID, LOCALE_IMEASURE, Buf, sizeof(Buf)))
		return (Buf[0] == '0');
	else
		return FALSE; // default
}


WEEKDAY CLocale::FirstDayOfWeek()
{
	char Buf[2];
	WEEKDAY WhichDay = SUNDAY; // default
	if (GetLocaleInfo(m_LocaleID, LOCALE_IFIRSTDAYOFWEEK, Buf, sizeof(Buf)))
	{
		switch (Buf[0])
		{
			case '0': 
				WhichDay = MONDAY;
				break;
			case '1': 
				WhichDay = TUESDAY;
				break;
			case '2': 
				WhichDay = WEDNESDAY;
				break;
			case '3': 
				WhichDay = THURSDAY;
				break;
			case '4': 
				WhichDay = FRIDAY;
				break;
			case '5': 
				WhichDay = SATURDAY;
				break;
			case '6': 
				WhichDay = SUNDAY;
				break;
		}
	}
	return WhichDay;
}

WEEKDAY CLocale::LastDayOfWeek()
{
	WEEKDAY WhichDay = (WEEKDAY)((int)(FirstDayOfWeek()+DAYS_IN_FULL_WEEK-1) % (int)DAYS_IN_FULL_WEEK);
	return WhichDay;
}

WEEKDAY CLocale::NextDayOfWeek(WEEKDAY Day)
{
	return (WEEKDAY)( ((int)Day+1)%(int)DAYS_IN_FULL_WEEK);
}

void CLocale::WeekDayName(WEEKDAY Day, TCHAR *Name, int NameSize)
{
	ASSERT (NameSize > 0);
	LCTYPE DayType = LOCALE_SDAYNAME1;

	switch (Day)
	{
		case MONDAY: 
			DayType = LOCALE_SDAYNAME1;
			break;
		case TUESDAY: 
			DayType = LOCALE_SDAYNAME2;
			break;
		case WEDNESDAY: 
			DayType = LOCALE_SDAYNAME3;
			break;
		case THURSDAY: 
			DayType = LOCALE_SDAYNAME4;
			break;
		case FRIDAY: 
			DayType = LOCALE_SDAYNAME5;
			break;
		case SATURDAY: 
			DayType = LOCALE_SDAYNAME6;
			break;
		case SUNDAY: 
			DayType = LOCALE_SDAYNAME7;
			break;
		default:
			// error
			ASSERT(0);
	}
	GetLocaleInfo(m_LocaleID, DayType, Name, NameSize);
	Name[NameSize-1] = '\0';
}

CString CLocale::WeekDayName(WEEKDAY Day)
{
	TCHAR Buf[MAX_WEEKDAYNAME];
	WeekDayName(Day, Buf, MAX_WEEKDAYNAME);
	return (CString(Buf));
}

void CLocale::AbbrevWeekDayName(WEEKDAY Day, TCHAR *Name, int NameSize)
{
	ASSERT (NameSize > 0);
	LCTYPE DayType = LOCALE_SABBREVDAYNAME1;

	switch (Day)
	{
		case MONDAY: 
			DayType = LOCALE_SABBREVDAYNAME1;
			break;
		case TUESDAY: 
			DayType = LOCALE_SABBREVDAYNAME2;
			break;
		case WEDNESDAY: 
			DayType = LOCALE_SABBREVDAYNAME3;
			break;
		case THURSDAY: 
			DayType = LOCALE_SABBREVDAYNAME4;
			break;
		case FRIDAY: 
			DayType = LOCALE_SABBREVDAYNAME5;
			break;
		case SATURDAY: 
			DayType = LOCALE_SABBREVDAYNAME6;
			break;
		case SUNDAY: 
			DayType = LOCALE_SABBREVDAYNAME7;
			break;
		default:
			// error
			ASSERT(0);
	}
	GetLocaleInfo(m_LocaleID, DayType, Name, NameSize);
	Name[NameSize-1] = '\0';
}

CString CLocale::AbbrevWeekDayName(WEEKDAY Day)
{
	TCHAR Buf[MAX_WEEKDAYNAME];
	AbbrevWeekDayName(Day, Buf, MAX_WEEKDAYNAME);
	return (CString(Buf));
}

void CLocale::MonthName(MONTH Month, TCHAR *Name, int NameSize)
{
	ASSERT(NameSize > 0);
	LCTYPE MonthType = LOCALE_SMONTHNAME1;

	switch (Month)
	{
		case JANUARY: 
			MonthType = LOCALE_SMONTHNAME1;
			break;
		case FEBRUARY: 
			MonthType = LOCALE_SMONTHNAME2;
			break;
		case MARCH: 
			MonthType = LOCALE_SMONTHNAME3;
			break;
		case APRIL: 
			MonthType = LOCALE_SMONTHNAME4;
			break;
		case MAY: 
			MonthType = LOCALE_SMONTHNAME5;
			break;
		case JUNE: 
			MonthType = LOCALE_SMONTHNAME6;
			break;
		case JULY: 
			MonthType = LOCALE_SMONTHNAME7;
			break;
		case AUGUST: 
			MonthType = LOCALE_SMONTHNAME8;
			break;
		case SEPTEMBER: 
			MonthType = LOCALE_SMONTHNAME9;
			break;
		case OCTOBER: 
			MonthType = LOCALE_SMONTHNAME10;
			break;
		case NOVEMBER: 
			MonthType = LOCALE_SMONTHNAME11;
			break;
		case DECEMBER: 
			MonthType = LOCALE_SMONTHNAME12;
			break;
		case MONTHS_IN_YEAR:				// in case there is a 13th month
			MonthType = LOCALE_SMONTHNAME13;
			break;
		default:
			// error
			ASSERT(0);
	}

	GetLocaleInfo(m_LocaleID, MonthType, Name, NameSize);
	Name[NameSize-1] = '\0';
}

CString CLocale::MonthName(MONTH Month)
{
	TCHAR Buf[MAX_MONTHNAME];
	MonthName(Month, Buf, MAX_MONTHNAME);
	return (CString(Buf));
}


void CLocale::AbbrevMonthName(MONTH Month, TCHAR *Name, int NameSize)
{
	ASSERT(NameSize > 0);
	LCTYPE MonthType = LOCALE_SMONTHNAME1;

	switch (Month)
	{
		case JANUARY: 
			MonthType = LOCALE_SABBREVMONTHNAME1;
			break;
		case FEBRUARY: 
			MonthType = LOCALE_SABBREVMONTHNAME2;
			break;
		case MARCH: 
			MonthType = LOCALE_SABBREVMONTHNAME3;
			break;
		case APRIL: 
			MonthType = LOCALE_SABBREVMONTHNAME4;
			break;
		case MAY: 
			MonthType = LOCALE_SABBREVMONTHNAME5;
			break;
		case JUNE: 
			MonthType = LOCALE_SABBREVMONTHNAME6;
			break;
		case JULY: 
			MonthType = LOCALE_SABBREVMONTHNAME7;
			break;
		case AUGUST: 
			MonthType = LOCALE_SABBREVMONTHNAME8;
			break;
		case SEPTEMBER: 
			MonthType = LOCALE_SABBREVMONTHNAME9;
			break;
		case OCTOBER: 
			MonthType = LOCALE_SABBREVMONTHNAME10;
			break;
		case NOVEMBER: 
			MonthType = LOCALE_SABBREVMONTHNAME11;
			break;
		case DECEMBER: 
			MonthType = LOCALE_SABBREVMONTHNAME12;
			break;
		case MONTHS_IN_YEAR:				// in case there is a 13th month
			MonthType = LOCALE_SABBREVMONTHNAME13;
			break;
		default:
			// error
			ASSERT(0);
	}

	GetLocaleInfo(m_LocaleID, MonthType, Name, NameSize);
	Name[NameSize-1] = '\0';
}


CString CLocale::AbbrevMonthName(MONTH Month)
{
	TCHAR Buf[MAX_MONTHNAME];
	AbbrevMonthName(Month, Buf, MAX_MONTHNAME);
	return (CString(Buf));
}

#ifndef INSTALL
void CLocale::GetBusinessCardDimsInInches(double *PaperWidth, double *PaperHeight,
                                       double *CardWidth, double *CardHeight, 
                                       double *StepWidth, double *StepHeight,
	      				   double *VerticalMargin, double *HorizontalMargin,
						   int *nCardsAcross, int *nCardsDown)
{
// fill in default values

// TODO fill in correct metric default values
	*PaperWidth = 8.5;	
	*PaperHeight = 11.0;
	*CardWidth = 3.5; 
  	*CardHeight = 2.0;
   	*StepHeight = 3.5;
  	*StepWidth = 2.0;
  	*VerticalMargin = 0.75;
   	*HorizontalMargin = 0.5;
   	*nCardsAcross = 2;
   	*nCardsDown = 5;


   CString csMsregIni = GetGlobalPathManager()->ExpandPath(m_csINIFileName);
   CIniFile MsRegIniFile(csMsregIni);

   CString TempString;
   double TempDouble;
   int TempInt;

   if (TempString = MsRegIniFile.GetString("BusinessCards", "PaperWidth"))
   {
   	TempDouble = atof((LPCSTR) TempString);
      if (TempDouble > 1000) TempDouble = *PaperWidth;
	if (UseMetric()) TempDouble = ConvertToInches(TempDouble/10);
   	*PaperWidth = TempDouble;
   }
   
   if (TempString = MsRegIniFile.GetString("BusinessCards", "PaperHeight"))
   {
   	TempDouble = atof((LPCSTR) TempString);
      if (TempDouble > 1000) TempDouble = *PaperHeight;
	if (UseMetric()) TempDouble = ConvertToInches(TempDouble/10);
   	*PaperHeight = TempDouble;
   }

   
   if (TempString = MsRegIniFile.GetString("BusinessCards", "CardWidth"))
   {
   	TempDouble = atof((LPCSTR) TempString);
      if (TempDouble > 500) TempDouble = *CardWidth;
	if (UseMetric()) TempDouble = ConvertToInches(TempDouble/10);
   	*CardWidth = TempDouble;
   }

   if (TempString = MsRegIniFile.GetString("BusinessCards", "CardHeight"))
   {
   	TempDouble = atof((LPCSTR) TempString);
      if (TempDouble > 1000) TempDouble = *CardHeight;
	if (UseMetric()) TempDouble = ConvertToInches(TempDouble/10);
   	*CardHeight = TempDouble;
   }

   if (TempInt = MsRegIniFile.GetInteger("BusinessCards", "NumCardsAcross",*nCardsAcross))
   if (TempInt < 100) *nCardsAcross = TempInt;

   if (TempInt = MsRegIniFile.GetInteger("BusinessCards", "NumCardsDown", *nCardsDown))
   if (TempInt < 100) *nCardsDown = TempInt;


   if (TempString = MsRegIniFile.GetString("BusinessCards", "VerticalMargin"))
   {
   	TempDouble = atof((LPCSTR) TempString);
      if (TempDouble > 1000) TempDouble = *VerticalMargin;
	if (UseMetric()) TempDouble = ConvertToInches(TempDouble/10);
   	*VerticalMargin = TempDouble;
   }

   if (TempString = MsRegIniFile.GetString("BusinessCards", "HorizontalMargin"))
   {
   	TempDouble = atof((LPCSTR) TempString);
      if (TempDouble > 1000) TempDouble = *HorizontalMargin;
	if (UseMetric()) TempDouble = ConvertToInches(TempDouble/10);
   	*HorizontalMargin = TempDouble;
   }

   if (TempString = MsRegIniFile.GetString("BusinessCards", "StepWidth"))
   {
   	TempDouble = atof((LPCSTR) TempString);
      if (TempDouble > 1000) TempDouble = *StepWidth;
	if (UseMetric()) TempDouble = ConvertToInches(TempDouble/10);
   	*StepWidth = TempDouble;
   }

   if (TempString = MsRegIniFile.GetString("BusinessCards", "StepHeight"))
   {
   	TempDouble = atof((LPCSTR) TempString);
      if (TempDouble > 1000) TempDouble = *StepHeight;
	if (UseMetric()) TempDouble = ConvertToInches(TempDouble/10);
   	*StepHeight = TempDouble;
   }
}
#endif


// various functions for localization

LPSTR *ParseAddressFormat(LPSTR AddressFormat);

UINT FieldNameIDs[] = 
{
	IDS_LOC_FIRSTNAME,
	IDS_LOC_LASTNAME,
	IDS_LOC_RELATIONSHIP,
	IDS_LOC_TITLE,
	IDS_LOC_COMPANY,
	IDS_LOC_ADDRESS1,
	IDS_LOC_ADDRESS2,
	IDS_LOC_CITY,	
	IDS_LOC_STATE,
	IDS_LOC_ZIP,	
	IDS_LOC_PHONE,
	IDS_LOC_COUNTRY,
	IDS_LOC_FAX,	
	IDS_LOC_EMAIL,
	IDS_LOC_BIRTHDAY,
	IDS_LOC_ANNIVERSARY,
	IDS_LOC_USERDEF_INFO,
	IDS_LOC_USERDEF_DATE,
	0
};

UINT SenderFieldNameIDs[] = 
{
	IDS_LOC_SEN_FIRSTNAME,
	IDS_LOC_SEN_LASTNAME,
	IDS_LOC_SEN_RELATIONSHIP,
	IDS_LOC_SEN_TITLE,
	IDS_LOC_SEN_COMPANY,
	IDS_LOC_SEN_ADDRESS1,
	IDS_LOC_SEN_ADDRESS2,
	IDS_LOC_SEN_CITY,	
	IDS_LOC_SEN_STATE,
	IDS_LOC_SEN_ZIP,	
	IDS_LOC_SEN_PHONE,
	IDS_LOC_SEN_COUNTRY,
	IDS_LOC_SEN_FAX,	
	IDS_LOC_SEN_EMAIL,
	IDS_LOC_SEN_BIRTHDAY,
	IDS_LOC_SEN_ANNIVERSARY,
	IDS_LOC_SEN_USER_DEF_INFO,
	IDS_LOC_SEN_USER_DEF_DATE,
	0
};


UINT TranslateFieldsSource[] =
{
	IDS_FLDTRANS_SRC0,
	IDS_FLDTRANS_SRC1,
	IDS_FLDTRANS_SRC2,
	IDS_FLDTRANS_SRC3,
	IDS_FLDTRANS_SRC4,
	IDS_FLDTRANS_SRC5,
	IDS_FLDTRANS_SRC6,
	IDS_FLDTRANS_SRC7,
	IDS_FLDTRANS_SRC8,
	IDS_FLDTRANS_SRC9,
	IDS_FLDTRANS_SRC10,
	IDS_FLDTRANS_SRC11,
	IDS_FLDTRANS_SRC12,
	IDS_FLDTRANS_SRC13,
	IDS_FLDTRANS_SRC14,
	IDS_FLDTRANS_SRC15,
	IDS_FLDTRANS_SRC16,
	IDS_FLDTRANS_SRC17,
	IDS_FLDTRANS_SRC18,
	IDS_FLDTRANS_SRC19,
	IDS_FLDTRANS_SRC20,
	IDS_FLDTRANS_SRC21,
	IDS_FLDTRANS_SRC22,
	IDS_FLDTRANS_SRC23,
	IDS_FLDTRANS_SRC24,
	IDS_FLDTRANS_SRC25,
	IDS_FLDTRANS_SRC26,
	IDS_FLDTRANS_SRC27,
	IDS_FLDTRANS_SRC28,
	IDS_FLDTRANS_SRC29,
	IDS_FLDTRANS_SRC30,
	IDS_FLDTRANS_SRC31,
	IDS_FLDTRANS_SRC32,
	IDS_FLDTRANS_SRC33,
	IDS_FLDTRANS_SRC34,
	IDS_FLDTRANS_SRC35,
	IDS_FLDTRANS_SRC36,
	IDS_FLDTRANS_SRC37,
	IDS_FLDTRANS_SRC38,
	IDS_FLDTRANS_SRC39,
	IDS_FLDTRANS_SRC40,
	0
};

UINT TranslateFieldsDest[] =
{
	IDS_FLDTRANS_DEST0,
	IDS_FLDTRANS_DEST1,
	IDS_FLDTRANS_DEST2,
	IDS_FLDTRANS_DEST3,
	IDS_FLDTRANS_DEST4,
	IDS_FLDTRANS_DEST5,
	IDS_FLDTRANS_DEST6,
	IDS_FLDTRANS_DEST7,
	IDS_FLDTRANS_DEST8,
	IDS_FLDTRANS_DEST9,
	IDS_FLDTRANS_DEST10,
	IDS_FLDTRANS_DEST11,
	IDS_FLDTRANS_DEST12,
	IDS_FLDTRANS_DEST13,
	IDS_FLDTRANS_DEST14,
	IDS_FLDTRANS_DEST15,
	IDS_FLDTRANS_DEST16,
	IDS_FLDTRANS_DEST17,
	IDS_FLDTRANS_DEST18,
	IDS_FLDTRANS_DEST19,
	IDS_FLDTRANS_DEST20,
	IDS_FLDTRANS_DEST21,
	IDS_FLDTRANS_DEST22,
	IDS_FLDTRANS_DEST23,
	IDS_FLDTRANS_DEST24,
	IDS_FLDTRANS_DEST25,
	IDS_FLDTRANS_DEST26,
	IDS_FLDTRANS_DEST27,
	IDS_FLDTRANS_DEST28,
	IDS_FLDTRANS_DEST29,
	IDS_FLDTRANS_DEST30,
	IDS_FLDTRANS_DEST31,
	IDS_FLDTRANS_DEST32,
	IDS_FLDTRANS_DEST33,
	IDS_FLDTRANS_DEST34,
	IDS_FLDTRANS_DEST35,
	IDS_FLDTRANS_DEST36,
	IDS_FLDTRANS_DEST37,
	IDS_FLDTRANS_DEST38,
	IDS_FLDTRANS_DEST39,
	IDS_FLDTRANS_DEST40,
	0
};

CString GetSenderFieldNameFromFieldName(const CString& csFieldName)
{

	CString csTempFieldName;
	CString csTempSenderFieldName;

	int i = 0;
	do
	{
		csTempFieldName.LoadString(FieldNameIDs[i]);
		if (csTempFieldName == csFieldName)
			csTempSenderFieldName.LoadString(SenderFieldNameIDs[i]);
		else
			i++;
	} while (SenderFieldNameIDs[i] && csTempSenderFieldName.IsEmpty());
	return csTempSenderFieldName;
}


LPSTR *ParseAddressFormat(LPSTR AddressFormat)
{
	static LPSTR AddressBlock[MAX_ADDRESSBLOCK];

	int i = 0;
	char *startptr = AddressFormat;

	do
	{
		if (*startptr == '[')
		{
			*startptr = NULL;
			startptr++;
			AddressBlock[i++] = startptr;
			if (!(startptr = strchr(startptr, ']')))
				return NULL;  // something is wrong with the format string
			*startptr = NULL;
			startptr++;
		}
		else
		{
			AddressBlock[i++] = startptr;
			startptr = strchr(startptr, '[');
		}

	} while ((i < MAX_ADDRESSBLOCK-1) && (startptr) && (*startptr));

	AddressBlock[i] = NULL;
	return AddressBlock;
}


CFlatFileFieldDescription *LocalDatabaseFieldDescriptions(CFlatFileFieldDescription *C)
{
	static CFlatFileFieldDescription DatabaseFieldDescriptions[MAX_DATABASEFIELDS];
	int i = 0, j = 0;
	CString csTemp;
	while ((C[i].m_idName) && (j < MAX_DATABASEFIELDS-1))
	{
		if (csTemp.LoadString(C[i].m_idName))
		{
			DatabaseFieldDescriptions[j++] = C[i++];
		}
		else
		{
			i++;
		}
	}

	while (j < MAX_DATABASEFIELDS)
	{
		DatabaseFieldDescriptions[j].m_idName = 0;         // Field Name ID
		DatabaseFieldDescriptions[j].m_pszName = NULL;			// Field Name
		DatabaseFieldDescriptions[j].m_dwType = 0;			// Field Type
		DatabaseFieldDescriptions[j].m_dwUserData = 0;		// Field User Data
		DatabaseFieldDescriptions[j].m_dwSize = 0;			// Field Size
		j++;
	}
	return DatabaseFieldDescriptions;
}

CFlatFileIndexDescription *LocalDatabaseIndexDescriptions(CFlatFileIndexDescription *C)
{
	static CFlatFileIndexDescription DatabaseIndexDescriptions[MAX_DATABASEFIELDS];
	int i = 0, j = 0;
	CString csTemp;
	while ((C[i].m_idName) && (j < MAX_DATABASEFIELDS-1))
	{
		if (csTemp.LoadString(C[i].m_idName))
		{
			DatabaseIndexDescriptions[j++] = C[i++];
		}
		else
		{
			i++;
		}
	}

	while (j < MAX_DATABASEFIELDS)
	{
		DatabaseIndexDescriptions[j].m_idName = 0;       
		DatabaseIndexDescriptions[j].m_pszName = NULL;	
		DatabaseIndexDescriptions[j].m_dwFlags = 0;			
		DatabaseIndexDescriptions[j].m_dwUserData = 0;		
		DatabaseIndexDescriptions[j].m_idField = 0;			
		DatabaseIndexDescriptions[j].m_pszField = 0;			
		j++;
	}
	return DatabaseIndexDescriptions;
}


BOOL IsPermanentField(const CString& FieldName)
{
	int i = 0;
	CString csTemp;
	while (FieldNameIDs[i])
	{
		csTemp.LoadString(FieldNameIDs[i]);
		if (FieldName == csTemp)
			return TRUE;
		else
			i++;
	}
	i = 0;
	while (SenderFieldNameIDs[i])
	{
		csTemp.LoadString(SenderFieldNameIDs[i]);
		if (FieldName == csTemp)
			return TRUE;
		else
			i++;
	}
	return FALSE;
}

CString TranslateFields(const CString& csSrcField)
{
	int i = 0;
	CString csTempSrc, csTempDest;
	while (TranslateFieldsSource[i])
	{
		if (csTempSrc.LoadString(TranslateFieldsSource[i]))
		{
			if (csTempSrc == csSrcField)
			{
				// found field to translate
				if (csTempDest.LoadString(TranslateFieldsDest[i]))
					return csTempDest;
			}
		}
		i++;
	}
	return csSrcField;
}

long Round(double dVal)
{
	long Ret;
	Ret = (long) dVal;
	double dRound;
	dRound = dVal - floor(dVal);
	if ((Ret >= 0) && (dRound >= 0.5))
    {
		Ret++;
	}
	else if ((Ret <0) && (dRound <= 0.5))
	{
		Ret--;
	}
	return Ret;
}

#ifdef EURODEMO
void EuroDemoFeatureMessage(UINT FirstID)
{
   CString csTitle;
   CString csMessage;
   csTitle.LoadString(IDS_EURODEMO_TITLE);
   csMessage.LoadString(FirstID);
   CString csTemp1, csTemp2;
   csTemp1.LoadString(IDS_EURODEMO_STARTUP_4);
   csTemp2.LoadString(IDS_EURODEMO_STARTUP_5);
   csMessage += csTemp1;
   csMessage += csTemp2;
   ::MessageBox(NULL, csMessage, csTitle, MB_OK);
}
#endif

