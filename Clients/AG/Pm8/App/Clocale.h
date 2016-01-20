
#ifndef __CLOCALE_H__
#define __CLOCALE_H__

#include "flatdb.h"
#include "calinfo.h"

// Locale Identifiers
 
#define LOCALE_ID_NEUTRAL		0x0000 // Language Neutral 
#define LOCALE_ID_USENGLISH		0x0409 // US English
#define LOCALE_ID_UKENGLISH		0x0809 // UK English
#define LOCALE_ID_GERMAN		0x0407 // German
#define LOCALE_ID_FRENCH		0x040C // French
#define LOCALE_ID_ITALIAN		0x0410 // Italian
#define LOCALE_ID_DEFAULT		LOCALE_ID_USENGLISH

// Language Names
#define LANGUAGE_USENGLISH		"ENU"
#define LANGUAGE_UKENGLISH		"ENG"
#define LANGUAGE_GERMAN			"DEU"
#define LANGUAGE_FRENCH			"FRA"
#define LANGUAGE_DEFAULT		LANGUAGE_USENGLISH

#define MAX_ADDRESSBLOCK		30  // maximum number of strings in an address block
#define MAX_DATABASEFIELDS		25
#define MAX_WEEKDAYNAME			80  // max possible length of weekday names
#define MAX_MONTHNAME			80  // max possible length of month names

#define INCHES_TO_CENTIMETRES 2.54

// access to current locale information
static class CLocale
{
public:
// locale
	static BOOL SetLocale(LCID LocaleID);
	static LCID GetLocaleID() {return m_LocaleID;};

// strings
	TCHAR MinAlphaUpper();
	TCHAR MaxAlphaUpper();
	void MakeUpper(CString *Str);

// numbers
	TCHAR DecimalPoint();

// measurements
	static BOOL UseMetric();
	double ConvertFromInches(double dMeasure);
	// converts from inches to centimetres if the locale uses the metric system
	// otherwise just returns the original value

	double ConvertToInches(double dMeasure);
	// converts from centimetres to incjes if the locale uses the metric system
	// otherwise just returns the original value

	int ConvertPrecisionFromInches(int nPrecision);
	// If the current locale uses the metric system
	// converts the inches precision to a suitable equivalent for centimetres

// calendars
	WEEKDAY FirstDayOfWeek();
	WEEKDAY LastDayOfWeek();
	WEEKDAY NextDayOfWeek(WEEKDAY Day);

	void WeekDayName(WEEKDAY Day, TCHAR *Name, int NameSize);
	CString WeekDayName(WEEKDAY Day);

	void AbbrevWeekDayName(WEEKDAY Day, TCHAR *Name, int NameSize);
	CString AbbrevWeekDayName(WEEKDAY Day);

	void MonthName(MONTH Month, TCHAR *Name, int NameSize);
	CString MonthName(MONTH Month);

	void AbbrevMonthName(MONTH Month, TCHAR *Name, int NameSize);
	CString AbbrevMonthName(MONTH Month);

// with help stuff - this needs to be revised, the with help is currently a mess
	BOOL WithHelpNewsletter(void);
	BOOL WithHelpWebPage(void);
	int WithHelpPosterCategoryConvert(int nCategory);


// business cards

	void GetBusinessCardDimsInInches(double *PaperWidth, double *PaperHeight,
                                       double *CardWidth, double *CardHeight, 
                                       double *StepWidth, double *StepHeight,
	      				   double *VerticalMargin, double *HorizontalMargin,
						   int *nCardsAcross, int *nCardsDown);
	// fills in business card dimensions for the current locale in inches

//    browser options
	CString HandleBrowserCaption(const CString& csINISection, const CString& csCaption, const CString& csFileName); 
      BOOL UseCapitalizeInCategoryBuild();
      BOOL Censor(const CString& csCaption, const CString& csFile, const CString& csKeywords);

// operators
	BOOL operator==(LCID);
	BOOL operator!=(LCID);
private:
	static CString m_csLocaleName;
	static LCID m_LocaleID;
	static CString m_csINIFileName;
} CurrentLocale;

// various functions for localization

CString GetSenderFieldNameFromFieldName(const CString& csFieldName);
// This function is used to get the full sender field name
// from the equivalent field name
// e.g. "First Name" returns "Sender First Name" (in English)
// This info is obtained from the resource file, because there is
// no simple rule, for dynamically constructing a Sender field name
// from a field name, which will work for all languages.


LPSTR *ParseAddressFormat(LPSTR AddressFormat);
// Given a sender address format string or a receiver address
// format string (which should be obtained from the resource file)
// returns a pointer to an array of strings constituting an address
// block. This allows the address block format to be specified in the
// resource file rather than hard-coded.

CFlatFileFieldDescription *LocalDatabaseFieldDescriptions(CFlatFileFieldDescription *C);
CFlatFileIndexDescription *LocalDatabaseIndexDescriptions(CFlatFileIndexDescription *C);

CString TranslateFields(const CString& csSrcField);
BOOL IsPermanentField(const CString& FieldName);

long Round(double dVal);

#ifdef EURODEMO
void EuroDemoFeatureMessage(UINT FirstID);
#endif


#endif
