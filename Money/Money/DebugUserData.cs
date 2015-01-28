using System.Collections.Generic;

namespace Money
{
#if NOTUSED //j
	internal class DummyProfileData
	{
		internal static void Load(Profile profile)
		{
			profile.Me.IsValid = true;
			profile.Me.FirstName = "Fred";
			profile.Me.Gender = Gender.Male;
			profile.Me.BirthDate = JulianDate.Create(11, 7, 1955);
			profile.Me.LifeExpectancy = 100.0;
			profile.Me.RelationshipStatus = RelationshipStatus.Married;
			profile.Me.Location.PostalCode = "01810";
			profile.Me.Location.StateOrSubCountryCode = "MA";
			profile.Me.Location.CountryCode = "US";
			profile.Me.Location.CountryName = "United States";
			profile.Me.HousingStatus = OwnershipStatus.Own;
			profile.Me.VehicleStatus = OwnershipStatus.Own; // How many?
			//profile.Me.WorkExperience
			//profile.Me.LifeExperience
			//profile.Me.EducationTraining
			//profile.Me.HealthIssues
			//profile.Me.PermDisabilities
			//profile.Me.Interests
			//profile.Me.PersonalTraits
			//profile.Me.Possessions

			profile.IncludeSpouse = true;
			profile.Spouse.IsValid = true;
			profile.Spouse.FirstName = "Wilma";
			profile.Spouse.Gender = Gender.Female;
			profile.Spouse.BirthDate = JulianDate.Create(7, 3, 1957);
			profile.Spouse.LifeExpectancy = 100.0;
			profile.Spouse.RelationshipStatus = RelationshipStatus.Married;
			profile.Spouse.Location.PostalCode = "01810";
			profile.Spouse.Location.StateOrSubCountryCode = "MA";
			profile.Spouse.Location.CountryCode = "US";
			profile.Spouse.Location.CountryName = "United States";
			profile.Spouse.HousingStatus = OwnershipStatus.Own;
			profile.Spouse.VehicleStatus = OwnershipStatus.Own; // How many?
			//profile.Spouse.WorkExperience
			//profile.Spouse.LifeExperience
			//profile.Spouse.EducationTraining
			//profile.Spouse.HealthIssues
			//profile.Spouse.PermDisabilities
			//profile.Spouse.Interests
			//profile.Spouse.PersonalTraits
			//profile.Spouse.Possessions

			profile.IncludeDependents = true;
			profile.Dependents.Add(new Person() { IsValid = true, FirstName = "Jess", BirthDate = JulianDate.Create(10, 9, 1983), Gender = Gender.Female, });
			profile.Dependents[0].LifeExpectancy = 100.0;
			profile.Dependents[0].Location.PostalCode = "02127";
			profile.Dependents[0].Location.StateOrSubCountryCode = "MA";
			profile.Dependents[0].Location.CountryCode = "US";
			profile.Dependents[0].Location.CountryName = "United States";

			profile.Dependents.Add(new Person() { IsValid = true, FirstName = "Judy", BirthDate = JulianDate.Create(5, 22, 1986), Gender = Gender.Female, });
			profile.Dependents[1].LifeExpectancy = 100.0;
			profile.Dependents[1].Location.PostalCode = "01810";
			profile.Dependents[1].Location.StateOrSubCountryCode = "MA";
			profile.Dependents[1].Location.CountryCode = "US";
			profile.Dependents[1].Location.CountryName = "United States";

			profile.Dependents.Add(new Person() { IsValid = true, FirstName = "Jill", BirthDate = JulianDate.Create(12, 6, 1991), Gender = Gender.Female, });
			profile.Dependents[2].LifeExpectancy = 100.0;
			profile.Dependents[2].Location.PostalCode = "01810";
			profile.Dependents[2].Location.StateOrSubCountryCode = "MA";
			profile.Dependents[2].Location.CountryCode = "US";
			profile.Dependents[2].Location.CountryName = "United States";

			//profile.Siblingss
			//profile.Parents

		}
	}
#endif

	internal class DebugDocumentData
	{
		internal static void Load(Document document)
		{
			// Load the income packages
			foreach (Income income in m_IncomePackages)
			{
				Income newIncome = new Income(income);
				document.Incomes.Add(newIncome);
			}

			// Load the spending packages
			foreach (Package package in m_SpendingPackages)
			{
				Package newPackage = new Package(package);
				document.Packages.Add(newPackage);
			}
		}

		private static readonly Deduction[] m_Deductions = {
			Deduction.DebugCreate("Stock Purchase",		TaxExemptFlags.None, DeductionType.Transfer,	100.00,		0, AccountType.Investments),
			Deduction.DebugCreate("401k Contribution",	TaxExemptFlags.All, DeductionType.Transfer,		100.00,		0, AccountType.Retirement1),
		};

		private static Income[] m_IncomePackages = {
			Income.DebugCreate("Fred Has A Job",		Frequency.Weekly, IncomeType.Add, 1886.40, AccountType.NoAccount, AccountType.Checking, TaxExemptFlags.None, 0/*receivedBy*/, ProfileCode.MeBirth+40, ProfileCode.MeBirth+60, m_Deductions),
			Income.DebugCreate("Wilma Has A Job",		Frequency.Weekly, IncomeType.Add, 1886.40, AccountType.NoAccount, AccountType.Checking, TaxExemptFlags.None, 1/*receivedBy*/, ProfileCode.SpouseBirth+40, ProfileCode.SpouseBirth+60, m_Deductions),
		};

		private static readonly Transaction[] m_FredOwnsACar = {
			Transaction.Create("CarInsurance",			Frequency.SemiAnnually, TransactionType.Subtract, 500.00, AccountType.NoAccount, AccountType.Checking),
			Transaction.Create("Gas",					Frequency.Weekly, TransactionType.Subtract, 40.00		, AccountType.NoAccount, AccountType.Checking),
			Transaction.Create("CarMaintenance",		Frequency.Annually, TransactionType.Subtract, 500.00	, AccountType.NoAccount, AccountType.Checking),
			Transaction.Create("CarRegistration",		Frequency.Annually, TransactionType.Subtract, 50.00		, AccountType.NoAccount, AccountType.Checking),
			Transaction.Create("CarExciseTax",			Frequency.Annually, TransactionType.Subtract, 100.00	, AccountType.NoAccount, AccountType.Checking),
		};

		private static readonly Transaction[] m_FredBuysACar = {
			Transaction.Create("CarSalesTax",			Frequency.OneTime, TransactionType.Subtract, 1500.00	, AccountType.NoAccount, AccountType.Checking),
			Transaction.Create("CarLoan",				Frequency.Monthly, TransactionType.Subtract, 400.00		, AccountType.NoAccount, AccountType.Checking, 48, 0),
		};

		private static List<Transaction> FredOwnsACar()
		{
			List<Transaction> list = new List<Transaction>();
			list.AddRange(m_FredBuysACar);
			list.AddRange(m_FredOwnsACar);
			return list;
		}

		private static readonly Transaction[] m_JessGetsMarried = {
			Transaction.Create("Jess's Wedding",		Frequency.OneTime, TransactionType.Subtract, 20000.00, AccountType.NoAccount, AccountType.Checking),
		};

		private static readonly Transaction[] m_JudyGetsMarried = {
			Transaction.Create("Judy's Wedding",		Frequency.OneTime, TransactionType.Subtract, 20000.00, AccountType.NoAccount, AccountType.Checking),
		};

		private static readonly Transaction[] m_JillGetsMarried = {
			Transaction.Create("Jill's Wedding",		Frequency.OneTime, TransactionType.Subtract, 20000.00, AccountType.NoAccount, AccountType.Checking),
		};

		private static readonly Transaction[] m_JessGoesToCollege = {
			Transaction.Create("Jess's College",		Frequency.Annually, TransactionType.Subtract, 15000.00, AccountType.NoAccount, AccountType.Checking),
		};

		private static readonly Transaction[] m_JudyGoesToCollege = {
			Transaction.Create("Judy's College",		Frequency.Annually, TransactionType.Subtract, 38000.00, AccountType.NoAccount, AccountType.Checking),
		};

		private static readonly Transaction[] m_JillGoesToCollege = {
			Transaction.Create("Jill's College",		Frequency.Annually, TransactionType.Subtract, 38000.00, AccountType.NoAccount, AccountType.Checking),
		};

//j		private static Transaction m_CreditLineInterest =
//j			Transaction.DebugCreate(Frequency.Monthly,		"CreditLineInterest", TransactionType.AddPct, 18.000 / 12, AccountType.NoAccount, AccountType.Credit);

		private static Package[] m_SpendingPackages = {
			Package.DebugCreate("Fred Owns A Car",		ProfileCode.MeBirth+40, ProfileCode.MeBirth+50, FredOwnsACar()),
			Package.DebugCreate("Fred Owns A Car",		ProfileCode.MeBirth+50, ProfileCode.MeBirth+60, FredOwnsACar()),
			Package.DebugCreate("Fred Owns A Car",		ProfileCode.MeBirth+70, ProfileCode.MeBirth+80, FredOwnsACar()),

			Package.DebugCreate("Miscellaneous",		ProfileCode.MeBirth+40, ProfileCode.MeBirth+95, new[] { Transaction.Create("Miscellaneous",		Frequency.Monthly,	 TransactionType.Subtract, 2000.00,	 AccountType.NoAccount,		 AccountType.Checking  )}),
			Package.DebugCreate("Cable",				ProfileCode.MeBirth+40, ProfileCode.MeBirth+95, new[] { Transaction.Create("Cable",				Frequency.Monthly,	 TransactionType.Subtract, 30.00,	 AccountType.NoAccount,		 AccountType.Checking  )}),
			Package.DebugCreate("Telephone",			ProfileCode.MeBirth+40, ProfileCode.MeBirth+95, new[] { Transaction.Create("Telephone",			Frequency.Monthly,	 TransactionType.Subtract, 90.00,	 AccountType.NoAccount,		 AccountType.Checking  )}),
			Package.DebugCreate("Primary Mortgage",		ProfileCode.MeBirth+40, ProfileCode.MeBirth+70, new[] { Transaction.Create("MA Mortgage",		Frequency.Monthly,	 TransactionType.Subtract, 2800.00,	 AccountType.NoAccount,		 AccountType.Checking  )}),
			Package.DebugCreate("Secondary Mortgage",	ProfileCode.MeBirth+40, ProfileCode.MeBirth+55, new[] { Transaction.Create("NH Mortgage",		Frequency.Monthly,	 TransactionType.Subtract, 1400.00,	 AccountType.NoAccount,		 AccountType.Checking  )}),
			Package.DebugCreate("Food",					ProfileCode.MeBirth+40, ProfileCode.MeBirth+95, new[] { Transaction.Create("Food",				Frequency.Weekly,	 TransactionType.Subtract, 200.00,	 AccountType.NoAccount,		 AccountType.Checking  )}),
			Package.DebugCreate("Clothing",				ProfileCode.MeBirth+40, ProfileCode.MeBirth+95, new[] { Transaction.Create("Clothing",			Frequency.Quarterly, TransactionType.Subtract, 300.00,	 AccountType.NoAccount,		 AccountType.Checking  )}),
			Package.DebugCreate("Entertainment",		ProfileCode.MeBirth+40, ProfileCode.MeBirth+95, new[] { Transaction.Create("Entertainment",		Frequency.Weekends,	 TransactionType.Subtract, 100.00,	 AccountType.NoAccount,		 AccountType.Checking  )}),
			Package.DebugCreate("Investment",			ProfileCode.MeBirth+40, ProfileCode.MeBirth+41, new[] { Transaction.Create("Investment",			Frequency.OneTime,	 TransactionType.Add, 200000.00,	 AccountType.NoAccount,		 AccountType.Investments  )}),
			Package.DebugCreate("Investment Interest",	ProfileCode.MeBirth+40, ProfileCode.MeBirth+95, new[] { Transaction.Create("Investment Interest",Frequency.Monthly,	 TransactionType.AddPct, 8.000/12,	 AccountType.NoAccount,		 AccountType.Investments)}),
			Package.DebugCreate("Retirement Interest",	ProfileCode.MeBirth+40, ProfileCode.MeBirth+95, new[] { Transaction.Create("Retirement Interest",Frequency.Monthly,	 TransactionType.AddPct, 6.000/12,	 AccountType.NoAccount,		 AccountType.Retirement1 )}),

			Package.DebugCreate("Jess Gets Married",	ProfileCode.MeBirth+53, ProfileCode.MeBirth+54, m_JessGetsMarried),
			Package.DebugCreate("Judy Gets Married",	ProfileCode.MeBirth+56, ProfileCode.MeBirth+56, m_JudyGetsMarried),
			Package.DebugCreate("Jill Gets Married",	ProfileCode.MeBirth+62, ProfileCode.MeBirth+63, m_JillGetsMarried),

			Package.DebugCreate("Jess Goes To College",	ProfileCode.DependentBirth(0)+18, ProfileCode.DependentBirth(0)+22, m_JessGoesToCollege),
			Package.DebugCreate("Judy Goes To College",	ProfileCode.MeBirth+49, ProfileCode.MeBirth+53, m_JudyGoesToCollege),
			Package.DebugCreate("Jill Goes To College",	ProfileCode.MeBirth+55, ProfileCode.MeBirth+59, m_JillGoesToCollege),
		};
	}
}
