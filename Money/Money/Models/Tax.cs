using System;
using System.Runtime.Serialization;
using System.Xml.Serialization;
using ClassLibrary;
using Millicents = System.Int64;

namespace Money
{
[DataContract]
[XmlRoot]
[Flags]
	public enum TaxExemptFlags
	{
		None = 0,
		FICA = 1,
		Federal = 2,
		State = 4,
		Local = 8,
		All = 0xF,
	}

	public class TaxExempt
	{
		public bool FICA { get; set; }
		public bool Federal { get; set; }
		public bool State { get; set; }
		public bool Local { get; set; }

		public static implicit operator TaxExempt(TaxExemptFlags flags)
		{
			TaxExempt taxExempt = new TaxExempt();
			taxExempt.FICA    = (flags & TaxExemptFlags.FICA)    != 0 || (flags & TaxExemptFlags.All) == TaxExemptFlags.All;
			taxExempt.Federal = (flags & TaxExemptFlags.Federal) != 0 || (flags & TaxExemptFlags.All) == TaxExemptFlags.All;
			taxExempt.State   = (flags & TaxExemptFlags.State)   != 0 || (flags & TaxExemptFlags.All) == TaxExemptFlags.All;
			taxExempt.Local   = (flags & TaxExemptFlags.Local)   != 0 || (flags & TaxExemptFlags.All) == TaxExemptFlags.All;
			return taxExempt;
		}

		internal TaxExemptFlags ToFlags()
		{
			if (FICA && Federal && State && Local)
				return TaxExemptFlags.All;

			TaxExemptFlags flags = TaxExemptFlags.None;
			if (FICA)	 flags |= TaxExemptFlags.FICA;
			if (Federal) flags |= TaxExemptFlags.Federal;
			if (State)	 flags |= TaxExemptFlags.State;
			if (Local)	 flags |= TaxExemptFlags.Local;
			return flags;
		}
	}

	internal class Tax
	{
		internal static void AddTaxTransactions(Millicents GrossForFICA, Millicents GrossForFed, Millicents GrossForState, Millicents GrossForLocal, Income income, Person person)
		{
			Tax tax = new Tax();
			tax.AddTaxes(GrossForFICA, GrossForFed, GrossForState, GrossForLocal, income, person);
		}

		private void AddTaxes(Millicents GrossForFICA, Millicents GrossForFed, Millicents GrossForState, Millicents GrossForLocal, Income income, Person person)
		{
			person.UpdateIncomeTaxStatusByDate(0/*JulianDay*/);
			IncomeTaxStatus status = person.IncomeTaxStatus;
			TaxTable federal = TaxTable.CreateFederal(status);
			TaxTable state = TaxTable.CreateState(status, person.Location);
			TaxTable local = TaxTable.CreateLocal(status, person.Location);

			double AnnualPayPeriods = income.Frequency.ToAnnualPeriods();

			// FICA
			double GrossFICA = GrossForFICA.ToDouble();
			double SocSecWages = Math.Min(GrossFICA, Math.Max(TaxTable.MaxSocSecWages - GrossFICA, 0));
			double SocSec = Math.Round(SocSecWages * TaxTable.SocSecPercent, 2);
			double Medicare = Math.Round(GrossFICA * TaxTable.MedicarePercent, 2);

			// Federal
			double FederalGross = GrossForFed.ToDouble() * AnnualPayPeriods;
			double FederalExemptions = federal.Exemptions(status.FederalExemptions);
			double FederalTaxableGross = Math.Max(FederalGross - FederalExemptions, 0);
			double FederalTaxTotal = Math.Round(federal.Compute(FederalTaxableGross), 2);
			double FederalTax = Math.Round(FederalTaxTotal / AnnualPayPeriods, 2);

			// State
			double StateGross = GrossForState.ToDouble() * AnnualPayPeriods;
			double StateExemptions = state.Exemptions(status.StateLocalExemptions);
			double StateTaxableGross = Math.Max(StateGross - StateExemptions, 0);
			double StateTaxTotal = Math.Round(state.Compute(StateTaxableGross), 2);
			double StateTax = Math.Round(StateTaxTotal / AnnualPayPeriods, 2);

			// Local
			double LocalGross = GrossForLocal.ToDouble() * AnnualPayPeriods;
			double LocalExemptions = local.Exemptions(status.StateLocalExemptions);
			double LocalTaxableGross = Math.Max(LocalGross - LocalExemptions, 0);
			double LocalTaxTotal = Math.Round(local.Compute(LocalTaxableGross), 2);
			double LocalTax = Math.Round(LocalTaxTotal / AnnualPayPeriods, 2);

			// Other calculations
			//double NetPay = GrossFed - SocSec - Medicare - FedTax - StateTax;
			//double AvgTaxRate = (SocSec + Medicare + FedTax + StateTax) / GrossFed;
			//double CompanyPayrollTaxes = FedTax + (2*SocSec )+ (2*Medicare);
			//double CompanyLiability = GrossFed + SocSec + Medicare;

			// If the income is not exempt, add the tax transactions
			if (!income.TaxExempt.FICA && SocSec != 0)
				income.Transactions.Add(NewTaxTransaction("Tax: SS", SocSec.ToMillicents(), income.Frequency, income.TargetAccount));
			if (!income.TaxExempt.FICA && Medicare != 0)
				income.Transactions.Add(NewTaxTransaction("Tax: Medicare", Medicare.ToMillicents(), income.Frequency, income.TargetAccount));
			if (!income.TaxExempt.Federal && FederalTax != 0)
				income.Transactions.Add(NewTaxTransaction("Tax: Federal", FederalTax.ToMillicents(), income.Frequency, income.TargetAccount));
			if (!income.TaxExempt.State && StateTax != 0)
				income.Transactions.Add(NewTaxTransaction("Tax: State", StateTax.ToMillicents(), income.Frequency, income.TargetAccount));
			if (!income.TaxExempt.Local && LocalTax != 0)
				income.Transactions.Add(NewTaxTransaction("Tax: Local", LocalTax.ToMillicents(), income.Frequency, income.TargetAccount));
		}

		private Transaction NewTaxTransaction(string name, Millicents taxAmount, Frequency incomeFrequency, AccountType incomeAccount)
		{
			Transaction transaction = new Transaction();
			transaction.Name = name;
			transaction.Frequency = incomeFrequency;
			transaction.Type = TransactionType.Subtract;
			transaction.Amount = taxAmount;
			transaction.SourceAccount = AccountType.NoAccount;
			transaction.TargetAccount = incomeAccount;
			return transaction;
		}
	}
}
