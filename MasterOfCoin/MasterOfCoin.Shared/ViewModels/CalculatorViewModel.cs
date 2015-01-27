using System;
using System.Windows.Input;
using ClassLibrary;
using GalaSoft.MvvmLight;
using GalaSoft.MvvmLight.Command;

namespace MasterOfCoin
{
	public class CalculatorViewModel : ViewModelBase
	{
		public ICommand ComputeCommand { get { return new RelayCommand(OnComputeCommand); } }
		public ICommand SelectScenarioCommand { get { return new RelayCommand<object>(OnSelectScenarioCommand); } }

		private Variable _rate;
		public Variable Rate { get { return _rate; } set { Set(() => Rate, ref _rate, value); } }

		private Variable _presentValue;
		public Variable PresentValue { get { return _presentValue; } set { Set(() => PresentValue, ref _presentValue, value); } }

		private Variable _futureValue;
		public Variable FutureValue { get { return _futureValue; } set { Set(() => FutureValue, ref _futureValue, value); } }

		private Variable _payment;
		public Variable Payment { get { return _payment; } set { Set(() => Payment, ref _payment, value); } }

		private Variable _periods;
		public Variable Periods { get { return _periods; } set { Set(() => Periods, ref _periods, value); } }

		private Scenario[] _scenarios;
		public Scenario[] Scenarios { get { return _scenarios; } set { Set(() => Scenarios, ref _scenarios, value); } }

		private string _computeButtonText;
		public string ComputeButtonText { get { return _computeButtonText; } set { Set(() => ComputeButtonText, ref _computeButtonText, value); } }

		private CompoundingFrequency _compoundingFrequency;
		public CompoundingFrequency CompoundingFrequency { get { return _compoundingFrequency; } set { OnCompoundingFrequencyChanged(_compoundingFrequency.ToValue(), value.ToValue()); Set(() => CompoundingFrequency, ref _compoundingFrequency, value); } }

		private PaymentFrequency _paymentFrequency;
		public PaymentFrequency PaymentFrequency { get { return _paymentFrequency; } set { OnPaymentFrequencyChanged(_paymentFrequency.ToValue(), value.ToValue()); Set(() => PaymentFrequency, ref _paymentFrequency, value); } }

		private PeriodFrequency _periodsFrequency;
		public PeriodFrequency PeriodsFrequency { get { return _periodsFrequency; } set { OnPeriodsFrequencyChanged(_periodsFrequency.ToValue(), value.ToValue()); Set(() => PeriodsFrequency, ref _periodsFrequency, value); } }

		private string _calculationTitle;
		public string CalculationTitle { get { return _calculationTitle; } set { Set(() => CalculationTitle, ref _calculationTitle, value); } }

		private double _totalPayments;
		public double TotalPayments { get { return _totalPayments; } set { Set(() => TotalPayments, ref _totalPayments, value); } }

		private double _totalInterest;
		public double TotalInterest { get { return _totalInterest; } set { Set(() => TotalInterest, ref _totalInterest, value); } }

		private Scenario _currentScenario;
		private Variable _selectedVariable;

		public CalculatorViewModel()
		{
			Scenarios = _staticScenarios;

			Rate = new Variable(OnSelectedVariableChanged);
			PresentValue = new Variable(OnSelectedVariableChanged);
			FutureValue = new Variable(OnSelectedVariableChanged);
			Payment = new Variable(OnSelectedVariableChanged);
			Periods = new Variable(OnSelectedVariableChanged);

			SelectScenario(Scenarios[0]);
		}

		private void OnSelectedVariableChanged(Variable variable)
		{
			_selectedVariable = variable;
			if (variable != null && variable.IsSelected)
				SetComputeButtonText(variable.Label);
		}

		private void SetComputeButtonText(string text)
		{
			ComputeButtonText = "Compute\n" + text.ToLower();
		}

		// Check out: http://www.visualcalc.com/products_cnstl_fc.htm
		private void OnComputeCommand()
		{
			if (_selectedVariable == null || !_selectedVariable.IsSelected)
				return;

			double daysPerCompound = IndexToDays(CompoundingFrequency.ToValue());
			double daysPerPeriod = IndexToDays(PeriodsFrequency.ToValue());
			double daysPerPayment = IndexToDays(PaymentFrequency.ToValue());

			double compoundsPerPeriod = daysPerPeriod / daysPerCompound;
			double compoundsPerPayment = daysPerPayment / daysPerCompound;
			double compoundsPerYear = 365.25 / daysPerCompound;

			const double annualInflationRate = 0;
			double annualRate = Rate.GetSafeValue();
			double periodicRate = (annualRate + annualInflationRate) / (100 * compoundsPerYear);
			double presentValue = PresentValue.GetSafeValue();
			double futureValue = FutureValue.GetSafeValue();
			double periods = Periods.GetSafeValue() * compoundsPerPeriod;
			double payment = Payment.GetSafeValue() / compoundsPerPayment;

			bool isDepleting = _currentScenario.IsDepleting;
			bool negatePayment = (isDepleting != payment < 0);
			if (negatePayment)
				payment = -payment;

			try
			{
				const Compounding compounding = Compounding.EndOfPeriod; // BeginningOfPeriod;
				if (Rate.IsSelected)
					// Negative parameters: -payment, -presentValue
					annualRate = Financial.Rate(periods, -payment, -presentValue, futureValue, compounding, 0/*guess*/);
				else if (PresentValue.IsSelected)
					// Negative parameters: -payment
					// Negative result: -presentValue
					presentValue = -Financial.PresentValue(periodicRate, periods, -payment, futureValue, compounding);
				else if (FutureValue.IsSelected)
					// Negative parameters: -payment, -presentValue
					futureValue = Financial.FutureValue(periodicRate, periods, -payment, -presentValue, compounding);
				else if (Payment.IsSelected)
				{	// Negative parameters: -futureValue
					payment = Financial.Payment(periodicRate, periods, presentValue, -futureValue, compounding);
					payment = Math.Abs(payment);
				}
				else if (Periods.IsSelected)
					// Negative parameters: -payment, -presentValue
					periods = Financial.NumberOfPeriods(periodicRate, -payment, -presentValue, futureValue, compounding);
			}
			catch (Exception ex)
			{
				ex.Alert();
			}

			if (Rate.IsSelected)
				Rate.Value = Math.Round(annualRate * (100 * compoundsPerYear), 3);
			if (PresentValue.IsSelected)
				PresentValue.Value = presentValue.Round(2);
			if (FutureValue.IsSelected)
				FutureValue.Value = futureValue.Round(2);
			if (Payment.IsSelected)
				Payment.Value = Math.Round(payment * compoundsPerPayment, 2);
			if (Periods.IsSelected)
				Periods.Value = Math.Round(periods / compoundsPerPeriod, 2);

			double totalPayments = Math.Abs(payment * periods);
			double totalInterest = Math.Abs(futureValue - presentValue) - totalPayments;
			TotalPayments = totalPayments.Round(2);
			TotalInterest = totalInterest.Round(2);

			// For general usage: add item order, multiplier, and multiple adders per field

			// Consider adding Manufacturer's Rebate
			// Subtract Manufacturer's Rebate from the loan amount (present value)

			// Consider adding Sales Tax Rate
			// Add the sales tax to the loan amount (present value)

			// Consider adding Down Payment
			// Subtract down payment from the loan amount (present value)

			// Consider adding Trade Allowance
			// Subtract trade allowance from the loan amount (present value)

			// Consider adding Amount Owed on Trade
			// Add Amount Owed on Trade to the loan amount (present value)

			// Consider adding Extra Payments
			// Adding:  $?? to your monthly mortgage payment
			// Adding:  $?? as an extra yearly mortgage payment every (Jan Feb Mar Apr May Jun Jul Aug Sep Oct Nov Dec)
		}

		private void OnSelectScenarioCommand(object parameter)
		{
			Scenario scenario = parameter as Scenario;
			if (scenario == null)
				return;
			SelectScenario(scenario);
		}

		private void SelectScenario(Scenario scenario)
		{
			// Backup variables into the current scenario
			if (_currentScenario != null)
			{
				_currentScenario.CompoundingFrequency = CompoundingFrequency;
				_currentScenario.PaymentFrequency = PaymentFrequency;
				_currentScenario.PeriodFrequency = PeriodsFrequency;

				_currentScenario.Rate.Value = Rate.Value;
				_currentScenario.PresentValue.Value = PresentValue.Value;
				_currentScenario.FutureValue.Value = FutureValue.Value;
				_currentScenario.Payment.Value = Payment.Value;
				_currentScenario.Periods.Value = Periods.Value;
			}

			// Set the current scenario
			_currentScenario = scenario;

			// Set the frequencies first
			CompoundingFrequency = scenario.CompoundingFrequency;
			PaymentFrequency = scenario.PaymentFrequency;
			PeriodsFrequency = scenario.PeriodFrequency;

			// Copy the scenario values
			Rate.CopyFrom(scenario.Rate);
			PresentValue.CopyFrom(scenario.PresentValue);
			FutureValue.CopyFrom(scenario.FutureValue);
			Payment.CopyFrom(scenario.Payment);
			Periods.CopyFrom(scenario.Periods);

			CalculationTitle = scenario.Title;
			TotalPayments = 0;
			TotalInterest = 0;
		}

		private void OnPaymentFrequencyChanged(int oldIndex, int newIndex)
		{
			if (Payment == null || Payment.Value == null)
				return;

			double factor = IndexToDays(newIndex) / IndexToDays(oldIndex);
			if (factor == 0)
				return;

			// Convert payment frequency
			var value = (double)Payment.Value * factor;
			Payment.Value = value.Round(2);
		}

		private void OnPeriodsFrequencyChanged(int oldIndex, int newIndex)
		{
			if (Periods == null || Periods.Value == null)
				return;

			double factor = IndexToDays(newIndex) / IndexToDays(oldIndex);
			if (factor == 0)
				return;

			// Convert period frequency
			var value = (double)Periods.Value / factor;
			Periods.Value = value.Round(2);
		}

		private void OnCompoundingFrequencyChanged(int oldIndex, int newIndex)
		{
			if (_selectedVariable == null || _selectedVariable.Value == null)
			{
				TotalPayments = 0;
				TotalInterest = 0;
			}
			else
				OnComputeCommand();
		}

		private double IndexToDays(int index)
		{
			if (index == 0)
				return 1; // Daily
			if (index == 1)
				return 365.25 / 12; // Monthly
			if (index == 2)
				return 365.25; // Annually
			if (index == 3)
				return 365.25 / 2; // SemiAnnually
			if (index == 4)
				return 365.25 / 4; // Quarterly
			return 0;
		}

		private static readonly Scenario[] _staticScenarios = {

			// Add a Lunch savings calculator http://www.bankrate.com/calculators/savings/bring-lunch-savings-calculator.aspx

			new Scenario("Save for a goal", "How much to save?", false, CompoundingFrequency.Monthly, PaymentFrequency.Months, PeriodFrequency.Annually)
			{
				Rate = new Variable("Annual interest rate", 4, true, true, false),
				PresentValue = new Variable("Starting balance", 1000, true, true, false),
				FutureValue = new Variable("Savings goal", 15000, true, true, false),
				Payment = new Variable("Periodic savings", null, true, true, true),
				Periods = new Variable("When goal is reached", 10, true, true, false),
			},
			new Scenario("Save for a goal", "How long to save?", false, CompoundingFrequency.Monthly, PaymentFrequency.Months, PeriodFrequency.Annually)
			{
				Rate = new Variable("Annual interest rate", 4, true, true, false),
				PresentValue = new Variable("Starting balance", 0, true, true, false),
				FutureValue = new Variable("Savings goal", 6000, true, true, false),
				Payment = new Variable("Periodic savings", 100, true, true, false),
				Periods = new Variable("When goal is reached", null, true, true, true),	
			},
			new Scenario("Save for a goal", "What is a reasonable goal?", false, CompoundingFrequency.Monthly, PaymentFrequency.Months, PeriodFrequency.Annually)
			{
				Rate = new Variable("Annual interest rate", 4, true, true, false),
				PresentValue = new Variable("Starting balance", 10000, true, true, false),
				FutureValue = new Variable("Savings goal", null, true, true, true),
				Payment = new Variable("Periodic savings", 100, true, true, false),
				Periods = new Variable("When goal is reached", 10, true, true, false),
			},
			new Scenario("Save for college", "How much to save?", false, CompoundingFrequency.Monthly, PaymentFrequency.Months, PeriodFrequency.Annually)
			{
				Rate = new Variable("Expected rate of return", 5, true, true, false),
				// Add in inflation rate
				// Subtract fed/state taxe rate from expected rate of return
				PresentValue = new Variable("College savings to date", 2000, true, true, false),
				FutureValue = new Variable("Estimated expenses", 80000, true, true, false),
				Payment = new Variable("Amount to save", null, true, true, true),
				Periods = new Variable("Time until college", 10, true, true, false),
			},
			new Scenario("Savings withdrawal", "How much will I have left?", true, CompoundingFrequency.Annually, PaymentFrequency.Years, PeriodFrequency.Annually)
			{
				Rate = new Variable("Annual interest rate", 2, true, true, false),
				PresentValue = new Variable("Starting balance", 10000, true, true, false),
				FutureValue = new Variable("Remaining funds", null, true, true, true),
				Payment = new Variable("Withdrawal amount", 50, true, true, false),
				Periods = new Variable("Time making withdrawals", 10, true, true, false),
			},
			new Scenario("Student loans", "What will my payments be?", true, CompoundingFrequency.Monthly, PaymentFrequency.Months, PeriodFrequency.Annually)
			{
				Rate = new Variable("Annual interest rate", 6.5, true, true, false),
				PresentValue = new Variable("Loan amount", 5000, true, true, false),
				FutureValue = new Variable("", 0, false, true, false),
				Payment = new Variable("Payment", null, true, true, true),
				Periods = new Variable("Length of the loan", 15, true, true, false),
			},
			new Scenario("Mortgage loans", "What will my payment be?", true, CompoundingFrequency.Monthly, PaymentFrequency.Months, PeriodFrequency.Annually)
			{
				Rate = new Variable("Annual interest rate", 5, true, true, false),
				PresentValue = new Variable("Loan amount", 100000, true, true, false),
				FutureValue = new Variable("", 0, false, true, false),
				Payment = new Variable("Payment", null, true, true, true),
				Periods = new Variable("Length of the loan", 30, true, true, false),
			},
			new Scenario("Mortgage loans", "How much can I borrow?", true, CompoundingFrequency.Monthly, PaymentFrequency.Months, PeriodFrequency.Annually)
			{
				Rate = new Variable("Annual interest rate", 5, true, true, false),
				PresentValue = new Variable("Loan amount", null, true, true, true),
				FutureValue = new Variable("", 0, false, true, false),
				Payment = new Variable("Payment", 600, true, true, false),
				Periods = new Variable("Length of the loan", 30, true, true, false),
			},
			new Scenario("Car or vehicle loans", "What will my payment be?", true, CompoundingFrequency.Monthly, PaymentFrequency.Months, PeriodFrequency.Annually)
			{
				Rate = new Variable("Annual interest rate", 7.5, true, true, false),
				PresentValue = new Variable("Loan amount", 10000, true, true, false),
				FutureValue = new Variable("", 0, false, true, false),
				Payment = new Variable("Payment", null, true, true, true),
				Periods = new Variable("Length of the loan", 4, true, true, false),
			},
			new Scenario("Car or vehicle loans", "How much can I borrow?", true, CompoundingFrequency.Monthly, PaymentFrequency.Months, PeriodFrequency.Annually)
			{
				Rate = new Variable("Annual interest rate", 7.5, true, true, false),
				PresentValue = new Variable("Loan amount", null, true, true, true),
				FutureValue = new Variable("", 0, false, true, false),
				Payment = new Variable("Payment", 300, true, true, false),
				Periods = new Variable("Length of the loan", 4, true, true, false),
			},
			new Scenario("Car or vehicle lease", "What will my payment be?", true, CompoundingFrequency.Monthly, PaymentFrequency.Months, PeriodFrequency.Annually)
			{
				Rate = new Variable("Annual interest rate", 7.5, true, true, false),
				PresentValue = new Variable("Lease amount", 18000, true, true, false),
				FutureValue = new Variable("Residual amount", 12000, true, true, false),
				Payment = new Variable("Payment", null, true, true, true),
				Periods = new Variable("Length of the lease", 3, true, true, false),
			},
			new Scenario("Credit card payoff", "How long to payoff?", true, CompoundingFrequency.Monthly, PaymentFrequency.Months, PeriodFrequency.Annually)
			{
				Rate = new Variable("Annual interest rate", 12, true, true, false),
				PresentValue = new Variable("Credit balance", 5000, true, true, false),
				FutureValue = new Variable("", 0, false, false, false),
				// Add in new charges per month
				Payment = new Variable("Payment", 100, true, true, false),
				Periods = new Variable("Time until payoff", null, true, true, true),
			},
			new Scenario("Credit card payoff", "How much to reach a payoff time?", true, CompoundingFrequency.Monthly, PaymentFrequency.Months, PeriodFrequency.Annually)
			{
				Rate = new Variable("Annual interest rate", 12, true, true, false),
				PresentValue = new Variable("Credit balance", 5000, true, true, false),
				FutureValue = new Variable("", 0, false, false, false),
				// Add in new charges per month
				Payment = new Variable("Payment", null, true, true, true),
				Periods = new Variable("Time until payoff", 5, true, true, false),
			},
			new Scenario("Investments", "When will I be a millionaire?", false, CompoundingFrequency.Monthly, PaymentFrequency.Months, PeriodFrequency.Annually)
			{
				Rate = new Variable("Expected rate of return", 7, true, true, false),
				PresentValue = new Variable("Amount currently invested", 10000, true, true, false),
				FutureValue = new Variable("Savings goal", 1000000, true, true, false),
				Payment = new Variable("Regular investment", 600, true, true, false),
				Periods = new Variable("When goal is reached", null, true, true, true),
			},
			new Scenario("Retirement savings", "How long will it last?", true, CompoundingFrequency.Annually, PaymentFrequency.Years, PeriodFrequency.Annually)
			{
				Rate = new Variable("Annual interest rate", 6, true, true, false),
				PresentValue = new Variable("Nest egg", 100000, true, true, false),
				FutureValue = new Variable("", 0, false, true, false),
				Payment = new Variable("Withdrawal amount", 8000, true, true, false),
				Periods = new Variable("When savings are depleted", null, true, true, true),
			},
			new Scenario("Retirement savings", "How much do I need?", true, CompoundingFrequency.Annually, PaymentFrequency.Years, PeriodFrequency.Annually)
			{
				Rate = new Variable("Annual interest rate", 6, true, true, false),
				PresentValue = new Variable("Nest egg", null, true, true, true),
				FutureValue = new Variable("", 0, false, true, false),
				Payment = new Variable("Withdrawal amount", 8000, true, true, false),
				Periods = new Variable("When savings are depleted", 30, true, true, false),
			},
			new Scenario("Retirement savings", "How much can I withdraw?", true, CompoundingFrequency.Annually, PaymentFrequency.Years, PeriodFrequency.Annually)
			{
				Rate = new Variable("Annual interest rate", 6, true, true, false),
				PresentValue = new Variable("Nest egg", 100000, true, true, false),
				FutureValue = new Variable("", 0, false, true, false),
				Payment = new Variable("Withdrawal amount", null, true, true, true),
				Periods = new Variable("When savings are depleted", 30, true, true, false),
			},
			new Scenario("Emergency savings", "How long to save?", false, CompoundingFrequency.Monthly, PaymentFrequency.Months, PeriodFrequency.Annually)
			{
				Rate = new Variable("Expected rate of return", 2, true, true, false),
				// Add in inflation rate
				PresentValue = new Variable("Amount already saved", 0, true, true, false),
				FutureValue = new Variable("Estimated expenses", 5000, true, true, false),
				Payment = new Variable("Amount to save", 200, true, true, false),
				Periods = new Variable("When goal is reached", null, true, true, true),
			},
			new Scenario("Certificates of deposit (CD's)", "What will it be worth?", false, CompoundingFrequency.Monthly, PaymentFrequency.Months, PeriodFrequency.Monthly)
			{
				Rate = new Variable("Interest rate", 2, true, true, false),
				PresentValue = new Variable("Initial deposit", 1000, true, true, false),
				FutureValue = new Variable("Ending balance", null, true, true, true),
				Payment = new Variable("", 0, false, true, false),
				Periods = new Variable("Time until maturity", 6, true, true, false),
			},
			new Scenario("Compound interest", "How much will my investment earn?", false, CompoundingFrequency.Monthly, PaymentFrequency.Months, PeriodFrequency.Annually)
			{
				Rate = new Variable("Interest rate", 7, true, true, false),
				PresentValue = new Variable("Investment amount", 10000, true, true, false),
				FutureValue = new Variable("Ending balance", null, true, true, true),
				Payment = new Variable("", 0, false, true, false),
				Periods = new Variable("Time invested", 10, true, true, false),
			},
			new Scenario("General", "Present value of a Future value", false, CompoundingFrequency.Annually, PaymentFrequency.Months, PeriodFrequency.Annually)
			{
				Rate = new Variable("Interest rate", 7, true, true, false),
				PresentValue = new Variable("Present value", null, true, true, true),
				FutureValue = new Variable("Future value", 10000, true, true, false),
				Payment = new Variable("", 0, false, true, false),
				Periods = new Variable("Time compounding", 10, true, true, false),
			},
			new Scenario("General", "Future value of a Present value", false, CompoundingFrequency.Annually, PaymentFrequency.Months, PeriodFrequency.Annually)
			{
				Rate = new Variable("Interest rate", 7, true, true, false),
				PresentValue = new Variable("Present value", 10000, true, true, false),
				FutureValue = new Variable("Future value", null, true, true, true),
				Payment = new Variable("", 0, false, true, false),
				Periods = new Variable("Time compounding", 10, true, true, false),
			},
		};

#if NOTUSED
		public enum CalculationType
		{
			Loan, // specifiy rate, periods, loan amount as -PV, FV = 0, compute payment
			// Where do points factor in?
			Lease, // specifiy rate, periods, lease amount as -PV, FV is residual amount, compute payment
			APR, // specifiy rate, periods, loan amount plus extra costs as -PV, FV = 0, compute payment and APR rate
			RegularInvestment, // specify rate, periods, original amount as -PV, regular investment as payment, compute FV
			YearsOfIncomeGivenAnnualAmount // specify rate, periods, original amount as -PV, regular investment as payment, compute FV then divide by annual amount
		}
#endif
	}
}
/*
http://www.math.umd.edu/~rll/cgi-bin/finance.cgi
Chapter 1. Saving for a College Education
				
	Lump Sum Account - Final Value
	Simple Annual Interest
			V = D(1+r)[^n]
			D is the amount deposited,
				r is the interest rate,
			n is the number of years,
				and V is the final amount.
				
	Daily Compounded Interest
			V = D(1 + r/365)[^365n]
			D is the amount deposited,
			r is the interest rate,
			n is the number of years,
			and V is the final amount.
				
	Lump Sum Account - Target Value
	Simple Annual Interest
			D = V/(1+r)[^n]
			V is the final target value of the account, r is the interest rate,
			n is the number of years,
				and D is the initial amount you must deposit.
				
	Daily Compounded Interest
			D = V/(1 + r/365)[^365n]
			V is the final target value of the account, r is the interest rate,
			n is the number of years, and D is the initial amount you must deposit.
				
Chapter 2. Investing for a College Education
				
	Regular Deposit Account - Final Value
	Annual Deposit, Simple Annual Interest
			V = D(1 + r)[(1 + r)[^n] - 1] / r
			D is the amount deposited annually, r is the interest rate,
			n is the number of years, and V is the final amount.
				
	Annual Deposit, Daily Compounded Interest
			V = D(1 + r/365)[^365][(1 + r/365)[^365n] - 1]/[(1 + r/365)[^365] - 1]
			D is the amount deposited annually, r is the interest rate,
			n is the number of years, and V is the final amount.
				
	Monthly Deposit, Daily Compounded Interest
			V = D(1 + r/360)[^30][(1 + r/360)[^360n] - 1]/[(1 + r/360)[^30] - 1]
			D is the amount deposited monthly, r is the interest rate,
			n is the number of years, and V is the final amount.
				
	Biweekly Deposit, Daily Compounded Interest
			V = D(1 + r/364)[^14][(1 + r/364)[^364n] - 1]/[(1 + r/364)[^14] - 1]
			D is the amount deposited biweekly, r is the interest rate,
			n is the number of years, and V is the final amount.
				
	Weekly Deposit, Daily Compounded Interest
			V = D(1 + r/364)[^7][(1 + r/364)[^364n] - 1]/[(1 + r/364)[^7] - 1]
			D is the amount deposited weekly, r is the interest rate,
			n is the number of years, and V is the final amount.
				
	Regular Deposit Account - Target Value
	Annual Deposit, Simple Annual Interest
			D = V/[(1 + r)[(1 + r)[^n] - 1] / r]
			V is the final target value of the account, r is the interest rate,
			n is the number of years, and D is the amount deposited annually that
			is required.
				
	Annual Deposit, Daily Compounded Interest
			D = V/[(1 + r/365)[^365][(1 + r/365)[^365n] - 1]/[(1 + r/365)[^365] - 1]]
			V is the final target value of the account, r is the interest rate,
			n is the number of years, and D is the required amount deposited annually.
				
Chapter 3. Taking into Consideration Taxes and Inflation
				
	Inflating Prices
			P = P[v0](1 + r)[^n]
			P[v0] is the price of an item initially, r is the annual inflation rate,
			n is the number of years, and P is the final price.
				
	After-Tax rate of Return
			r[va] = r(1 - b/100)
			r is the stated rate of return, b is the marginal income tax bracket,
			and r[va] is the actual after-tax rate of return.
				
	Effect of Taxes on a Simple Annual Interest Account
			V = D(1+r[vb])[^n]
			D is the amount deposited, r is the interest rate,
			n is the number of years, b is the marginal tax rate,
			r[vb] = r(1-b/100) is the effective yield, and
			V is the final amount.
				
Chapter 4. Tax-Deferred Accounts Can Help
				
	Taxable versus Tax-Deferred; Simplest Model
			V = D(1 + r)[(1 + r)[^n] - 1] / r, in the tax-deferred account
			V = D(1 + r[vb])[(1 + r[vb])[^n] - 1] / r[vb], in the taxable account, r[vb]=r(1-b/100)
			D is the amount deposited annually, r is the interest rate,
			n is the number of years, b is the marginal income tax bracket, and
			V is the final amount.
				
	Taxable versus Tax-Deferred; Complex Model
			V = (1-b/100)D(1 + r)[(1 + r)[^n] - 1] / r, in the tax-deferred account
			V = (1-b/100)D(1 + r[vb])[(1 + r[vb])[^n] - 1] / r[vb], in the taxable account, r[vb]=r(1-b/100)
			D is the amount available (before taxes) annually, r is the interest rate,
			n is the number of years, b is the marginal income tax bracket, and
			V is the final amount.
				
Chapter 5. Your First Salary: What is Your Job Worth
				
	Freedom Quotient
			FQ = AT/G is your after-tax income, G is your gross salary,
			FQ is your freedom quotient.
				
Chapter 6. Buying a House or Car: Mortgages and Loans
				
	Loan Payments
			P = B(r/12) / [ 1 - 1/(1+r/12)[^n] ]
			B is the amount borrowed, r is the interest rate,
			n is the number of months, and P is the monthly payment.
				
	Loan Amount
			B = P/[(r/12) / [ 1 - 1/(1+r/12)[^n]]]
			P is the monthly payment, r is the interest rate,
			n is the number of months, and B is the amount borrowed.
				
	Magic Number
			MN = (r/12) / [ 1 - 1/(1+r/12)[^n] ]
			r is the interest rate, n is the number of months, and MN is the magic number.
				
	Total Payments
			TP = nP = nB(r/12) / [ 1 - 1/(1+r/12)[^n]]
			B is the amount borrowed, r is the interest rate,
			n is the number of months, and TP is the total payment.
				
	Total Interest
			TI = TP - B, where the formula for TP is given in the form immediately above.
			B is the amount borrowed, r is the interest rate,
			n is the number of months, and TI is the total interest paid on the loan.
				
Chapter 7. Buying or Leasing Your Car
				
	Lease Payments
			P = (C - R)/n + (C + R)M
			C is the cap cost, R is the residual value,
			n is the number of months, M is the money factor, and P is the monthly payment.
				
Chapter 10. Cut up those Credit Cards
				
	Credit Card Interest
			I = Br/12
			B is the outstanding balance, r is the annual interest rate charged by your credit card company,
			and I is the interest charge for that month.
				
Chapter 12. The Stock Market and Other Investments
				
	Escalating Investment Model
			V = D(1 + r) [(1 + r)[^n] - (1 + s)[^n]] / (r - s)
			D is the initial amount invested, r is the interest ate,
			s is the rate at which the amount invested is escalated each year, n is the number of years, and V is the final value.
				
	Magic Number for an Escalating Investment Program
			MN = (1 + r) [(1 + r)[^n] - (1 + s)[^n]] / (r - s)
			r is the interest rate, s is the rate at which the amount invested is escalated each year,
			n is the number of years, and MN is the magic number.
				
Chapter 13. Retirement
				
	Retirement Account Depletion - Discounting Inflation
	How Long Will Your Money Last
			n = ln[S/(S - rE)]/ln(1+r)
			S is the annual shortfall, E is your nest egg,
			r is the interest rate, and n is the number of years till depletion of the account.
				
	How Much You Can Spend
			S = (1 + r)[^n]E/[(1 + r)[^n] - 1)/r]
			E is your nest egg, r is the interest rate,
			n is the number of years the nest egg must last, and S is the amount you can afford to spend annually.
				
	Retirement Account Depletion - Accounting for Inflation
	How Long Will Your Money Last
			n = ln[S/(S - (r - s)E)]/ln[(1+r)/(1+s)]
			S is the annual shortfall, E is your nest egg,
			r is the interest rate, s is the inflation rate, and n is the number of years till depletion of the account.
				
	How Much Can You Spend
			S = (1 + r)[^n]E/[((1 + r)[^n] - (1 + s)[^n])/(r - s)]
			E is your nest egg, r is the interest rate,
			s is the inflation rate, n is the number of years the nest egg must last,
			and S is the amount you can afford to spend annually.
*/
