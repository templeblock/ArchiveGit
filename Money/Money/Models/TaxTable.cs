using System;

namespace Money
{
	internal class TaxTable
	{
		// State Tax Data: http://www.taxfoundation.org/taxdata/show/228.html
		// Overkill Withholding calculator: http://www.irs.gov/individuals/page/0,,id=14806,00.html
		// Paycheck calculator: http://www.paycheckcity.com/NetPayCalc/netpaycalculator.asp
		// Tax Tables: http://www.themoneyalert.com/Tax-Tables.html
		// if claimed as a dependant, this changes
		// Does 65 or older matter?
		// Blind matters?
		// Child care credit?
		// Student?
		// # of jobs in the family (count pension or military pay as a job)
		// had a former job?

		private delegate double CalculateExemptions(int numPersons);
		private CalculateExemptions _CalculateExemptions;
		private double[,] _Data;
		private IncomeTaxStatus _Status;

		// FICA Constants
		internal const double SocSecPercent = 0.062;
		internal const double MedicarePercent = 0.0145;
		internal const double MaxSocSecWages = 106800; // 2009

		internal static TaxTable CreateFederal(IncomeTaxStatus status)
		{
			TaxTable taxTable = new TaxTable();
			taxTable._Status = status;
			taxTable._CalculateExemptions = taxTable.Fed_Exemptions;
			taxTable._Data = (status.FederalFilingStatus == FilingStatus.Married ? TaxTable.Fed_Married : TaxTable.Fed_Single);
			return taxTable;
		}

		internal static TaxTable CreateState(IncomeTaxStatus status, Location location)
		{
			TaxTable taxTable = new TaxTable();
			taxTable._Status = status;
			taxTable._CalculateExemptions = taxTable.MA_Exemptions; //j Should be based on Location
			taxTable._Data = (status.StateLocalFilingStatus == FilingStatus.Married ? TaxTable.MA_Married : TaxTable.MA_Single);
			return taxTable;
		}

		internal static TaxTable CreateLocal(IncomeTaxStatus status, Location location)
		{
			TaxTable taxTable = new TaxTable();
			taxTable._Status = status;
			taxTable._CalculateExemptions = null; //j Should point to a local tax function
			taxTable._Data = null; //j Should point to a local tax table
			return taxTable;
		}

		internal double Compute(double income)
		{
			if (_Data == null)
				return 0;

			double tax = 0;
			double currentBaseIncome = 0;
			double currentRate = 0;
			int lastrow = _Data.GetLength(0);
			for (int row = 0; row <= lastrow; row++)
			{
				if (income <= currentBaseIncome)
					break;

				double baseIncome = (row != lastrow ? _Data[row, 0] : double.MaxValue);
				double rate = (row != lastrow ? _Data[row, 1] : 1.0);
				double excess = Math.Min(income, baseIncome) - currentBaseIncome;
				if (currentRate > 0 && excess > 0)
					tax += (currentRate * excess);
				currentBaseIncome = baseIncome;
				currentRate = rate;
			}

			return tax;
		}

		public double Exemptions(int numPersons)
		{
			if (_CalculateExemptions == null)
				return 0;

			return _CalculateExemptions(numPersons);
		}

		private double Fed_Exemptions(int numPersons)
		{
			// Federal constants
			const double FederalExemptionPerPerson = 3650; // 2009
			// Federal exemptions
			return FederalExemptionPerPerson * numPersons;
		}

		private double MA_Exemptions(int numPersons)
		{
			// Massachusetts constants
			const double StateStandardExemption = 3400; // 2009
			const double StateExemptionPerPerson = 1000; // 2009
			const double StateBlindExemptionPerPerson = 2200; // 2009
			const double StateHohExemption = 2400; // 2009
			// Massachusetts exemptions
			double StateExemption = (StateStandardExemption + (StateExemptionPerPerson * numPersons));
			double BlindnessExemption = (StateBlindExemptionPerPerson * _Status.Blind);
			double HeadeOfHouseholdExemption = (_Status.StateLocalFilingStatus == FilingStatus.HeadOfHousehold ? StateHohExemption : 0);
			double ExemptUpTo2kofSSandMedicare = 0;
			return (StateExemption + BlindnessExemption + HeadeOfHouseholdExemption + ExemptUpTo2kofSSandMedicare);
		}

		private static double[,] Fed_Single =
		{
			{   7180, .100 },
			{  10400, .150 },
			{  36200, .250 },
			{  66530, .280 },
			{ 173600, .330 },
			{ 375000, .350 },
		};

		private static double[,] Fed_Married =
		{
		    {  15750, .100 },
		    {  24450, .150 },
		    {  75650, .250 },
		    { 118130, .280 },
		    { 216600, .330 },
		    { 380700, .350 },
		};

		private static double[,] MA_Single =
		{
			{   7999, .000 }, // Special case: no taxes if below 8000
			{      0, .053 }, // Now everyone pays
		};

		private static double[,] MA_Married = MA_Single;
	}
}
