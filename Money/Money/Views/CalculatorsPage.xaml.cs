using System;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Input;
using System.Windows.Navigation;
using ClassLibrary;

namespace Money
{
	public partial class CalculatorsPage : Page
	{
		public enum CompoundFrequency
		{
			Daily = 0,
			Monthly = 1,
			Annually = 2,
			SemiAnnually = 3,
			Quarterly = 4,
		}

		internal class Variable
		{
			public string Label { get; set; }
			public double Value { get; set; }
			public bool IsVisible { get; set; }
			public bool IsTargetVisible { get; set; }
			public bool IsTarget { get; set; }

			public Variable(string label, double value, bool isVisible, bool isTargetVisible, bool isTarget)
			{
				Label = label;
				Value = value;
				IsVisible = isVisible;
				IsTargetVisible = isTargetVisible;
				IsTarget = isTarget;
			}
		}

		internal class VariableUI
		{
			public TextBlock Label { get; set; }
			public TextBox Value { get; set; }
			public Panel Panel { get; set; }
			public RadioButton Target { get; set; }

			public VariableUI(TextBlock label, TextBox value, Panel panel, RadioButton target)
			{
				Label = label;
				Value = value;
				Panel = panel;
				Target = target;
			}
		}

		public class Item
		{
			public string FullTitle { get; set; }
			public string Title { get; set; }
			public string SubTitle { get; set; }
			public bool IsDepleting { get; set; }
			public CompoundFrequency CompoundingFrequency { get; set; }
			public CompoundFrequency PaymentFrequency { get; set; }
			public CompoundFrequency PeriodFrequency { get; set; }

			internal Variable Rate { get; set; }
			internal Variable PresentValue { get; set; }
			internal Variable FutureValue { get; set; }
			internal Variable Payment { get; set; }
			internal Variable Periods { get; set; }

			public Item(string title, string subTitle, bool isDepleting, CompoundFrequency compoundingFrequency, CompoundFrequency paymentFrequency, CompoundFrequency periodFrequency)
				: base()
			{
				FullTitle = title + ": " + subTitle;
				Title = title;
				SubTitle = subTitle;
				IsDepleting = isDepleting;
				CompoundingFrequency = compoundingFrequency;
				PaymentFrequency = paymentFrequency;
				PeriodFrequency = periodFrequency;
			}
		}

		VariableUI m_Rate;
		VariableUI m_PresentValue;
		VariableUI m_FutureValue;
		VariableUI m_Payment;
		VariableUI m_Periods;

		Item m_CurrentItem;

		internal static readonly Item[] Items = {

			// Add a Lunch savings calculator http://www.bankrate.com/calculators/savings/bring-lunch-savings-calculator.aspx

			new Item("Save for a goal", "How much to save?", false, CompoundFrequency.Monthly, CompoundFrequency.Monthly, CompoundFrequency.Annually)
			{
				Rate = new Variable("Annual interest rate", 4, true, true, false),
				PresentValue = new Variable("Starting balance", 1000, true, true, false),
				FutureValue = new Variable("Savings goal", 15000, true, true, false),
				Payment = new Variable("Periodic savings", -1, true, true, true),
				Periods = new Variable("When goal is reached", 10, true, true, false),
			},
			new Item("Save for a goal", "How long to save?", false, CompoundFrequency.Monthly, CompoundFrequency.Monthly, CompoundFrequency.Annually)
			{
				Rate = new Variable("Annual interest rate", 4, true, true, false),
				PresentValue = new Variable("Starting balance", 0, true, true, false),
				FutureValue = new Variable("Savings goal", 6000, true, true, false),
				Payment = new Variable("Periodic savings", 100, true, true, false),
				Periods = new Variable("When goal is reached", -1, true, true, true),	
			},
			new Item("Save for a goal", "What is a reasonable goal?", false, CompoundFrequency.Monthly, CompoundFrequency.Monthly, CompoundFrequency.Annually)
			{
				Rate = new Variable("Annual interest rate", 4, true, true, false),
				PresentValue = new Variable("Starting balance", 10000, true, true, false),
				FutureValue = new Variable("Savings goal", -1, true, true, true),
				Payment = new Variable("Periodic savings", 100, true, true, false),
				Periods = new Variable("When goal is reached", 10, true, true, false),
			},
			new Item("Save for college", "How much to save?", false, CompoundFrequency.Monthly, CompoundFrequency.Monthly, CompoundFrequency.Annually)
			{
				Rate = new Variable("Expected rate of return", 5, true, true, false),
				// Add in inflation rate
				// Subtract fed/state taxe rate from expected rate of return
				PresentValue = new Variable("College savings to date", 2000, true, true, false),
				FutureValue = new Variable("Estimated expenses", 80000, true, true, false),
				Payment = new Variable("Amount to save", -1, true, true, true),
				Periods = new Variable("Time until college", 10, true, true, false),
			},
			new Item("Savings withdrawal", "How much will I have left?", true, CompoundFrequency.Annually, CompoundFrequency.Annually, CompoundFrequency.Annually)
			{
				Rate = new Variable("Annual interest rate", 2, true, true, false),
				PresentValue = new Variable("Starting balance", 10000, true, true, false),
				FutureValue = new Variable("Remaining funds", -1, true, true, true),
				Payment = new Variable("Withdrawal amount", 50, true, true, false),
				Periods = new Variable("Time making withdrawals", 10, true, true, false),
			},
			new Item("Student loans", "What will my payments be?", true, CompoundFrequency.Monthly, CompoundFrequency.Monthly, CompoundFrequency.Annually)
			{
				Rate = new Variable("Annual interest rate", 6.5, true, true, false),
				PresentValue = new Variable("Loan amount", 5000, true, true, false),
				FutureValue = new Variable("", 0, false, true, false),
				Payment = new Variable("Payment", -1, true, true, true),
				Periods = new Variable("Length of the loan", 15, true, true, false),
			},
			new Item("Mortgage loans", "What will my payment be?", true, CompoundFrequency.Monthly, CompoundFrequency.Monthly, CompoundFrequency.Annually)
			{
				Rate = new Variable("Annual interest rate", 5, true, true, false),
				PresentValue = new Variable("Loan amount", 100000, true, true, false),
				FutureValue = new Variable("", 0, false, true, false),
				Payment = new Variable("Payment", -1, true, true, true),
				Periods = new Variable("Length of the loan", 30, true, true, false),
			},
			new Item("Mortgage loans", "How much can I borrow?", true, CompoundFrequency.Monthly, CompoundFrequency.Monthly, CompoundFrequency.Annually)
			{
				Rate = new Variable("Annual interest rate", 5, true, true, false),
				PresentValue = new Variable("Loan amount", -1, true, true, true),
				FutureValue = new Variable("", 0, false, true, false),
				Payment = new Variable("Payment", 600, true, true, false),
				Periods = new Variable("Length of the loan", 30, true, true, false),
			},
			new Item("Car or vehicle loans", "What will my payment be?", true, CompoundFrequency.Monthly, CompoundFrequency.Monthly, CompoundFrequency.Annually)
			{
				Rate = new Variable("Annual interest rate", 7.5, true, true, false),
				PresentValue = new Variable("Loan amount", 10000, true, true, false),
				FutureValue = new Variable("", 0, false, true, false),
				Payment = new Variable("Payment", -1, true, true, true),
				Periods = new Variable("Length of the loan", 4, true, true, false),
			},
			new Item("Car or vehicle loans", "How much can I borrow?", true, CompoundFrequency.Monthly, CompoundFrequency.Monthly, CompoundFrequency.Annually)
			{
				Rate = new Variable("Annual interest rate", 7.5, true, true, false),
				PresentValue = new Variable("Loan amount", -1, true, true, true),
				FutureValue = new Variable("", 0, false, true, false),
				Payment = new Variable("Payment", 300, true, true, false),
				Periods = new Variable("Length of the loan", 4, true, true, false),
			},
			new Item("Car or vehicle lease", "What will my payment be?", true, CompoundFrequency.Monthly, CompoundFrequency.Monthly, CompoundFrequency.Annually)
			{
				Rate = new Variable("Annual interest rate", 7.5, true, true, false),
				PresentValue = new Variable("Lease amount", 18000, true, true, false),
				FutureValue = new Variable("Residual amount", 12000, true, true, false),
				Payment = new Variable("Payment", -1, true, true, true),
				Periods = new Variable("Length of the lease", 3, true, true, false),
			},
			new Item("Credit card payoff", "How long to payoff?", true, CompoundFrequency.Monthly, CompoundFrequency.Monthly, CompoundFrequency.Annually)
			{
				Rate = new Variable("Annual interest rate", 12, true, true, false),
				PresentValue = new Variable("Credit balance", 5000, true, true, false),
				FutureValue = new Variable("", 0, false, false, false),
				// Add in new charges per month
				Payment = new Variable("Payment", 100, true, true, false),
				Periods = new Variable("Time until payoff", -1, true, true, true),
			},
			new Item("Credit card payoff", "How much to reach a payoff time?", true, CompoundFrequency.Monthly, CompoundFrequency.Monthly, CompoundFrequency.Annually)
			{
				Rate = new Variable("Annual interest rate", 12, true, true, false),
				PresentValue = new Variable("Credit balance", 5000, true, true, false),
				FutureValue = new Variable("", 0, false, false, false),
				// Add in new charges per month
				Payment = new Variable("Payment", -1, true, true, true),
				Periods = new Variable("Time until payoff", 5, true, true, false),
			},
			new Item("Investments", "When will I be a millionaire?", false, CompoundFrequency.Monthly, CompoundFrequency.Monthly, CompoundFrequency.Annually)
			{
				Rate = new Variable("Expected rate of return", 7, true, true, false),
				PresentValue = new Variable("Amount currently invested", 10000, true, true, false),
				FutureValue = new Variable("Savings goal", 1000000, true, true, false),
				Payment = new Variable("Regular investment", 600, true, true, false),
				Periods = new Variable("When goal is reached", -1, true, true, true),
			},
			new Item("Retirement savings", "How long will it last?", true, CompoundFrequency.Annually, CompoundFrequency.Annually, CompoundFrequency.Annually)
			{
				Rate = new Variable("Annual interest rate", 6, true, true, false),
				PresentValue = new Variable("Nest egg", 100000, true, true, false),
				FutureValue = new Variable("", 0, false, true, false),
				Payment = new Variable("Withdrawal amount", 8000, true, true, false),
				Periods = new Variable("When savings are depleted", -1, true, true, true),
			},
			new Item("Retirement savings", "How much do I need?", true, CompoundFrequency.Annually, CompoundFrequency.Annually, CompoundFrequency.Annually)
			{
				Rate = new Variable("Annual interest rate", 6, true, true, false),
				PresentValue = new Variable("Nest egg", -1, true, true, true),
				FutureValue = new Variable("", 0, false, true, false),
				Payment = new Variable("Withdrawal amount", 8000, true, true, false),
				Periods = new Variable("When savings are depleted", 30, true, true, false),
			},
			new Item("Retirement savings", "How much can I withdraw?", true, CompoundFrequency.Annually, CompoundFrequency.Annually, CompoundFrequency.Annually)
			{
				Rate = new Variable("Annual interest rate", 6, true, true, false),
				PresentValue = new Variable("Nest egg", 100000, true, true, false),
				FutureValue = new Variable("", 0, false, true, false),
				Payment = new Variable("Withdrawal amount", -1, true, true, true),
				Periods = new Variable("When savings are depleted", 30, true, true, false),
			},
			new Item("Emergency savings", "How long to save?", false, CompoundFrequency.Monthly, CompoundFrequency.Monthly, CompoundFrequency.Annually)
			{
				Rate = new Variable("Expected rate of return", 2, true, true, false),
				// Add in inflation rate
				PresentValue = new Variable("Amount already saved", 0, true, true, false),
				FutureValue = new Variable("Estimated expenses", 5000, true, true, false),
				Payment = new Variable("Amount to save", 200, true, true, false),
				Periods = new Variable("When goal is reached", -1, true, true, true),
			},
			new Item("Certificates of deposit (CD's)", "What will it be worth?", false, CompoundFrequency.Monthly, CompoundFrequency.Monthly, CompoundFrequency.Monthly)
			{
				Rate = new Variable("Interest rate", 2, true, true, false),
				PresentValue = new Variable("Initial deposit", 1000, true, true, false),
				FutureValue = new Variable("Ending balance", -1, true, true, true),
				Payment = new Variable("", 0, false, true, false),
				Periods = new Variable("Time until maturity", 6, true, true, false),
			},
			new Item("Compound interest", "How much will my investment earn?", false, CompoundFrequency.Monthly, CompoundFrequency.Monthly, CompoundFrequency.Annually)
			{
				Rate = new Variable("Interest rate", 7, true, true, false),
				PresentValue = new Variable("Investment amount", 10000, true, true, false),
				FutureValue = new Variable("Ending balance", -1, true, true, true),
				Payment = new Variable("", 0, false, true, false),
				Periods = new Variable("Time invested", 10, true, true, false),
			},
			new Item("General", "Present value of a Future value", false, CompoundFrequency.Annually, CompoundFrequency.Monthly, CompoundFrequency.Annually)
			{
				Rate = new Variable("Interest rate", 7, true, true, false),
				PresentValue = new Variable("Present value", -1, true, true, true),
				FutureValue = new Variable("Future value", 10000, true, true, false),
				Payment = new Variable("", 0, false, true, false),
				Periods = new Variable("Time compounding", 10, true, true, false),
			},
			new Item("General", "Future value of a Present value", false, CompoundFrequency.Annually, CompoundFrequency.Monthly, CompoundFrequency.Annually)
			{
				Rate = new Variable("Interest rate", 7, true, true, false),
				PresentValue = new Variable("Present value", 10000, true, true, false),
				FutureValue = new Variable("Future value", -1, true, true, true),
				Payment = new Variable("", 0, false, true, false),
				Periods = new Variable("Time compounding", 10, true, true, false),
			},
		};

		public CalculatorsPage()
		{
			InitializeComponent();
			HtmlTextBlock content = base.Content.ChildOfType<HtmlTextBlock>();
			content.HyperlinkClick += App.MainPage.OnHyperlinkClick;
			base.Loaded += OnLoaded;
		}

		private void OnLoaded(object sender, RoutedEventArgs e)
		{
			base.Loaded -= OnLoaded;

			x_CalculateItems.ItemsSource = Items;

			m_Rate = new VariableUI(x_LabelRate, x_Rate, x_RatePanel, x_RateTarget);
			m_PresentValue = new VariableUI(x_LabelPresentValue, x_PresentValue, x_PresentValuePanel, x_PresentValueTarget);
			m_FutureValue = new VariableUI(x_LabelFutureValue, x_FutureValue, x_FutureValuePanel, x_FutureValueTarget);
			m_Payment = new VariableUI(x_LabelPayment, x_Payment, x_PaymentPanel, x_PaymentTarget);
			m_Periods = new VariableUI(x_LabelPeriods, x_Periods, x_PeriodsPanel, x_PeriodsTarget);

			m_Rate.Value.TextChanged += OnValueChanged;
			m_PresentValue.Value.TextChanged += OnValueChanged;
			m_FutureValue.Value.TextChanged += OnValueChanged;
			m_Payment.Value.TextChanged += OnValueChanged;
			m_Periods.Value.TextChanged += OnValueChanged;

			m_Rate.Target.Checked += OnTargetChecked;
			m_PresentValue.Target.Checked += OnTargetChecked;
			m_FutureValue.Target.Checked += OnTargetChecked;
			m_Payment.Target.Checked += OnTargetChecked;
			m_Periods.Target.Checked += OnTargetChecked;

			x_PaymentFrequency.SelectionChanged += OnPaymentFrequencyChanged;
			x_PeriodsFrequency.SelectionChanged += OnPeriodsFrequencyChanged;
			x_CompoundingFrequency.SelectionChanged += OnCompoundingFrequencyChanged;

			x_Compute.Click += delegate(object sender1, RoutedEventArgs e1)
			{
				Compute();
			};

			base.KeyDown += delegate(object sender2, KeyEventArgs e2)
			{
				if (e2.Key == Key.Enter)
					Compute();
			};

			SelectCalculateItem(Items[0]);
		}

		private void OnCalculateItemClick(object sender, RoutedEventArgs e)
		{
			FrameworkElement element = sender as FrameworkElement;
			Item item = element.DataContext as Item;
			SelectCalculateItem(item);
		}

		private void SelectCalculateItem(Item item)
		{
			// Backup variables into the current item
			if (m_CurrentItem != null)
			{
				m_CurrentItem.CompoundingFrequency = (CompoundFrequency)x_CompoundingFrequency.SelectedIndex;
				m_CurrentItem.PaymentFrequency = (CompoundFrequency)x_PaymentFrequency.SelectedIndex;
				m_CurrentItem.PeriodFrequency = (CompoundFrequency)x_PeriodsFrequency.SelectedIndex;
				m_CurrentItem.Rate.Value = GetTextBoxValue(m_Rate.Value);
				m_CurrentItem.PresentValue.Value = GetTextBoxValue(m_PresentValue.Value);
				m_CurrentItem.FutureValue.Value = GetTextBoxValue(m_FutureValue.Value);
				m_CurrentItem.Payment.Value = GetTextBoxValue(m_Payment.Value);
				m_CurrentItem.Periods.Value = GetTextBoxValue(m_Periods.Value);
			}

			// Make the new item the current item
			m_CurrentItem = item;

			// Set the combos first
			x_CompoundingFrequency.SelectedIndex = (int)item.CompoundingFrequency;
			x_PaymentFrequency.SelectedIndex = (int)item.PaymentFrequency;
			x_PeriodsFrequency.SelectedIndex = (int)item.PeriodFrequency;

			// Stuff any default values
			SetTextBoxValue(m_Rate.Value, item.Rate.Value, 3);
			SetTextBoxValue(m_PresentValue.Value, item.PresentValue.Value, 2);
			SetTextBoxValue(m_FutureValue.Value, item.FutureValue.Value, 2);
			SetTextBoxValue(m_Payment.Value, item.Payment.Value, 2);
			SetTextBoxValue(m_Periods.Value, item.Periods.Value, 2);

			x_CalculationTitle.Text = item.Title;

			// Set the label text
			m_Rate.Label.Text = item.Rate.Label;
			m_PresentValue.Label.Text = item.PresentValue.Label;
			m_FutureValue.Label.Text = item.FutureValue.Label;
			m_Payment.Label.Text = item.Payment.Label;
			m_Periods.Label.Text = item.Periods.Label;

			// Set the panel visibility
			m_Rate.Panel.SetVisible(item.Rate.IsVisible);
			m_PresentValue.Panel.SetVisible(item.PresentValue.IsVisible);
			m_FutureValue.Panel.SetVisible(item.FutureValue.IsVisible);
			m_Payment.Panel.SetVisible(item.Payment.IsVisible);
			m_Periods.Panel.SetVisible(item.Periods.IsVisible);

			// Set the target visibility
			m_Rate.Target.SetVisible(item.Rate.IsTargetVisible);
			m_PresentValue.Target.SetVisible(item.PresentValue.IsTargetVisible);
			m_FutureValue.Target.SetVisible(item.FutureValue.IsTargetVisible);
			m_Payment.Target.SetVisible(item.Payment.IsTargetVisible);
			m_Periods.Target.SetVisible(item.Periods.IsTargetVisible);

			// Set the target checked property
			RadioButton target = null;
			if (item.Rate.IsTarget)			target = m_Rate.Target;
			if (item.PresentValue.IsTarget)	target = m_PresentValue.Target;
			if (item.FutureValue.IsTarget)	target = m_FutureValue.Target;
			if (item.Payment.IsTarget)		target = m_Payment.Target;
			if (item.Periods.IsTarget)		target = m_Periods.Target;
			if (target != null)
				target.IsChecked = true;

			x_TotalPayments.Text = "0";
			x_TotalInterest.Text = "0";
		}

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

		private void OnValueChanged(object sender, TextChangedEventArgs e)
		{
			// Wipe out the any cached value from the Tag property
			TextBox textBox = sender as TextBox;
			if (textBox != null)
				textBox.Tag = null;
		}

		private void OnTargetChecked(object sender, RoutedEventArgs e)
		{
			SetComputeButtonText(sender as RadioButton);
		}

		private void SetComputeButtonText(RadioButton target)
		{
			string text = null;
			if (target == m_Rate.Target)			text = m_Rate.Label.Text.ToString(); else
			if (target == m_PresentValue.Target)	text = m_PresentValue.Label.Text.ToString(); else
			if (target == m_FutureValue.Target)		text = m_FutureValue.Label.Text.ToString(); else
			if (target == m_Payment.Target)			text = m_Payment.Label.Text.ToString(); else
			if (target == m_Periods.Target)			text = m_Periods.Label.Text.ToString();

			x_Compute.Content = "Compute " + text.ToLower();
		}

		private void OnPaymentFrequencyChanged(object sender, SelectionChangedEventArgs e)
		{
			if (m_Payment == null)
				return;

			double factor = GetFrequencyConversionFactor(sender, e);
			if (factor == 0)
				return;

			// Convert payment frequency
			double payment = GetTextBoxValue(m_Payment.Value);
			if (payment != -1)
				payment *= factor;
			SetTextBoxValue(m_Payment.Value, payment, 2);
		}

		private void OnPeriodsFrequencyChanged(object sender, SelectionChangedEventArgs e)
		{
			if (m_Periods == null)
				return;

			double factor = GetFrequencyConversionFactor(sender, e);
			if (factor == 0)
				return;

			// Convert period frequency
			double periods = GetTextBoxValue(m_Periods.Value);
			if (periods != -1)
				periods /= factor;
			SetTextBoxValue(m_Periods.Value, periods, 2);
		}

		private void OnCompoundingFrequencyChanged(object sender, SelectionChangedEventArgs e)
		{
			VariableUI variableUI = null;
			if ((bool)m_Rate.Target.IsChecked)			variableUI = m_Rate;
			if ((bool)m_PresentValue.Target.IsChecked)	variableUI = m_PresentValue;
			if ((bool)m_FutureValue.Target.IsChecked)	variableUI = m_FutureValue;
			if ((bool)m_Payment.Target.IsChecked)		variableUI = m_Payment;
			if ((bool)m_Periods.Target.IsChecked)		variableUI = m_Periods;

			if (variableUI == null || variableUI.Value.Text.IsEmpty())
			{
				x_TotalPayments.Text = "0";
				x_TotalInterest.Text = "0";
			}
			else
				Compute();
		}

		// Check out: http://www.visualcalc.com/products_cnstl_fc.htm
		private void Compute()
		{
			if (!(bool)m_Rate.Target.IsChecked &&
				!(bool)m_PresentValue.Target.IsChecked &&
				!(bool)m_FutureValue.Target.IsChecked &&
				!(bool)m_Payment.Target.IsChecked &&
				!(bool)m_Periods.Target.IsChecked)
			{
				MessageBoxEx.ShowError("Calculate", "You must select one of the values to calculate", null);
				return;
			}

			double daysPerCompound = IndexToDays(x_CompoundingFrequency.SelectedIndex);
			double daysPerPeriod = IndexToDays(x_PeriodsFrequency.SelectedIndex);
			double daysPerPayment = IndexToDays(x_PaymentFrequency.SelectedIndex);

			double compoundsPerPeriod = daysPerPeriod / daysPerCompound;
			double compoundsPerPayment = daysPerPayment / daysPerCompound;
			double compoundsPerYear = 365.25 / daysPerCompound;

			double annualInflationRate = 0;
			double annualRate = GetTextBoxValue(m_Rate.Value);
			double periodicRate = (annualRate + annualInflationRate) / (100 * compoundsPerYear);
			double presentValue = GetTextBoxValue(m_PresentValue.Value);
			double futureValue = GetTextBoxValue(m_FutureValue.Value);
			double periods = GetTextBoxValue(m_Periods.Value) * compoundsPerPeriod;
			double payment = GetTextBoxValue(m_Payment.Value) / compoundsPerPayment;

			bool isDepleting = m_CurrentItem.IsDepleting;
			bool negatePayment = (isDepleting != payment < 0);
			if (negatePayment)
				payment = -payment;

			try
			{
				Compounding compounding = Compounding.EndOfPeriod; // BeginningOfPeriod;
				if ((bool)m_Rate.Target.IsChecked)
					// Negative parameters: -payment, -presentValue
					annualRate = Financial.Rate(periods, -payment, -presentValue, futureValue, compounding, 0/*guess*/);
				else
				if ((bool)m_PresentValue.Target.IsChecked)
					// Negative parameters: -payment
					// Negative result: -presentValue
					presentValue = -Financial.PresentValue(periodicRate, periods, -payment, futureValue, compounding);
				else
				if ((bool)m_FutureValue.Target.IsChecked)
					// Negative parameters: -payment, -presentValue
					futureValue = Financial.FutureValue(periodicRate, periods, -payment, -presentValue, compounding);
				else
				if ((bool)m_Payment.Target.IsChecked)
				{	// Negative parameters: -futureValue
					payment = Financial.Payment(periodicRate, periods, presentValue, -futureValue, compounding);
					payment = Math.Abs(payment);
				}
				else
				if ((bool)m_Periods.Target.IsChecked)
					// Negative parameters: -payment, -presentValue
					periods = Financial.NumberOfPeriods(periodicRate, -payment, -presentValue, futureValue, compounding);
			}
			catch (Exception ex)
			{
				ex.Alert();
			}

			if ((bool)m_Rate.Target.IsChecked)
				SetTextBoxValue(m_Rate.Value, annualRate * (100 * compoundsPerYear), 3);
			if ((bool)m_PresentValue.Target.IsChecked)
				SetTextBoxValue(m_PresentValue.Value, presentValue, 2);
			if ((bool)m_FutureValue.Target.IsChecked)
				SetTextBoxValue(m_FutureValue.Value, futureValue, 2);
			if ((bool)m_Payment.Target.IsChecked)
				SetTextBoxValue(m_Payment.Value, payment * compoundsPerPayment, 2);
			if ((bool)m_Periods.Target.IsChecked)
				SetTextBoxValue(m_Periods.Value, periods / compoundsPerPeriod, 2);

			double totalPayments = Math.Abs(payment * periods);
			double totalInterest = Math.Abs(futureValue - presentValue) - totalPayments;
			x_TotalPayments.Text = totalPayments.Round(2).ToString();
			x_TotalInterest.Text = totalInterest.Round(2).ToString();

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

		private double GetFrequencyConversionFactor(object sender, SelectionChangedEventArgs e)
		{
			if (e.AddedItems.Count != 1 || e.RemovedItems.Count != 1)
				return 0;

			ComboBox comboBox = sender as ComboBox;
			if (comboBox == null)
				return 0;

			ComboBoxItem oldItem = e.RemovedItems[0] as ComboBoxItem;
			ComboBoxItem newItem = e.AddedItems[0] as ComboBoxItem;
			int oldIndex = GetItemIndex(comboBox, oldItem);
			int newIndex = GetItemIndex(comboBox, newItem);
			double oldDays = IndexToDays(oldIndex);
			double newDays = IndexToDays(newIndex);
			return newDays / oldDays;
		}

		private int GetItemIndex(ComboBox comboBox, ComboBoxItem comboBoxItem)
		{
			int index = 0;
			foreach (ComboBoxItem item in comboBox.Items)
			{
				if (item.Content.Equals(comboBoxItem.Content))
					return index;
				index++;
			}

			return -1;
		}

		private double IndexToDays(int index)
		{
			if (index == 0) return 1; // Daily
			if (index == 1) return 365.25 / 12; // Monthly
			if (index == 2) return 365.25; // Annually
			if (index == 3) return 365.25 / 2; // SemiAnnually
			if (index == 4) return 365.25 / 4; // Quarterly
			return 0;
		}

		private double GetTextBoxValue(TextBox textBox)
		{
			if (textBox.Text.IsEmpty())
				return -1;
			if (textBox.Tag is double)
				return (double)textBox.Tag;
			return textBox.Text.ToDouble();
		}

		private void SetTextBoxValue(TextBox textBox, double value)
		{
			SetTextBoxValue(textBox, value, -1);
		}

		private void SetTextBoxValue(TextBox textBox, double value, int digits)
		{
			double uiValue = (digits == -1 ? value : value.Round(digits));
			textBox.Tag = (value == -1 ? null : (object)value);
			textBox.Text = (value == -1 ? "" : uiValue.ToString());
		}
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
