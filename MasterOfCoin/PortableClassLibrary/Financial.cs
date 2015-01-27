using System;

namespace ClassLibrary
{
	/// <summary>
	/// Financial Functions
	/// </summary>
	public class Financial
	{ // Checkout http://api.kde.org/koffice-api/koffice-apidocs/kspread/html/financial_8cpp-source.html

		// Function to calculate the effective annual interest rate
		// rate => the nominal interest rate
		// npery => the number of compounding periods per year
		public double EFFECT(double rate, int npery)
		{
			if (npery == 0 || rate == 0)
			{
				//alert("Why do you want to test me with zeros?");
				return 0;
			}

			return Math.Pow(1 + (rate / npery), npery) - 1;
		}

		// Function to calculate the straight-line depreciation of an asset for one period
		public double SLN(double Cost, double Salvage, double Life)
		{
			if (Life <= 0.0)
				throw new ArgumentException("Invalid Argument Value", "Life");

			return (Cost - Salvage) / Life;
		}

		// Function to calculate the sum-of-years' digits depreciation of an asset for a specified period
		public double SYD(double Cost, double Salvage, double Life, int Period)
		{
			if (Life <= 0.0)
				throw new ArgumentException("Invalid Argument Value", "Life <= 0.0");

			if (Salvage < 0.0)
				throw new ArgumentException("Invalid Argument Value", "Salvage < 0.0");

			if (Period > Life)
				throw new ArgumentException("Invalid Argument Value", "Period > Life");
	
			if (Period <= 0.0)
				throw new ArgumentException("Invalid Argument Value", "Period <= 0.0");

			return ((Cost - Salvage) * (Life - Period + 1.0) * 2.0) / (Life * (Life + 1.0));
		}

		// Function to calculate the depreciation of an asset for a specified period by using the double-declining balance method
		public double DDB(double Cost, double Salvage, double Life, int Period)
		{
			double Factor = 2.0;
			if (Cost < 0.0 || Salvage < 0.0 || Life <= 0.0 || Period < 0.0 || Period > Life)
				throw new ArgumentException("Invalid Argument Value");

			if (Cost == 0.0)
				return 0.0;

			if (Life < 2.0)
				return Cost - Salvage;

			if (Life == 2.0)
			{
				if (Period > 1.0)
					return 0.0;

				return Cost - Salvage;
			}
			if (Period <= 1.0)
			{
				double Depreciation = Cost * Factor / Life;
				if (Depreciation < Cost - Salvage)
					return Depreciation;

				return Cost - Salvage;
			}

			// Algorithm
			//double TotalPriorDepreciation = 0;
			//return Math.Min(
			//	(Cost - TotalPriorDepreciation) * (Factor/Life),
			//	(Cost - Salvage - TotalPriorDepreciation)
			//);

			double temp = (Cost * (Factor / Life)) * Math.Pow(1 - (Factor / Life), Period - 1);
			double temp5 = (Cost * (1 - Math.Pow(1 - (Factor / Life), Period))) - Cost + Salvage;
			if (temp5 > 0)
				temp -= temp5;
			if (temp >= 0)
				return temp;

			return 0.0;
		}

		/// <summary>
		/// PV()
		/// Returns the present value of an investment. The present value is the total amount that a series of future payments is worth now.
		/// </summary>
		/// <param name="rate">Periodic Interest Rate.</param>
		/// <param name="numberPeriods">Number of periods.</param>
		/// <param name="payment">Periodic Payment.</param>
		/// <param name="futureValue">Future value.</param>
		/// <param name="compounding">Specifies when values are accrued, at the beginning or at the end of the period.</param>
		/// <returns></returns>
		public static double PresentValue(double rate, double numberPeriods, double payment, double futureValue, Compounding compounding)
		{
			if (rate == 0)
				return (-futureValue - (payment * numberPeriods));

			double auxiliaryRate1;
			if (compounding != Compounding.EndOfPeriod)
				auxiliaryRate1 = 1 + rate;
			else
				auxiliaryRate1 = 1;

			double auxiliaryRate2 = Math.Pow(1 + rate, numberPeriods);
			return (-(futureValue + ((payment * auxiliaryRate1) * ((auxiliaryRate2 - 1) / rate))) / auxiliaryRate2);
		}

		/// <summary>
		/// FV()
		/// Returns the future value of an investment based on periodic, constant payments and a constant interest rate.
		/// </summary>
		/// <param name="rate">Periodic Interest Rate.</param>
		/// <param name="numberPeriods">Number of periods.</param>
		/// <param name="payment">Periodic Payment.</param>
		/// <param name="presentValue">Present value or principal.</param>
		/// <param name="compounding">Specifies when values are accrued, at the beginning or at the end of the period.</param>
		/// <returns></returns>
		public static double FutureValue(double rate, double numberPeriods, double payment, double presentValue, Compounding compounding)
		{
			if (rate == 0)
				return (-presentValue - (payment * numberPeriods));

			double auxiliaryRate1;
			if (compounding != Compounding.EndOfPeriod)
				auxiliaryRate1 = 1 + rate;
			else
				auxiliaryRate1 = 1;

			double auxiliaryRate2 = Math.Pow(1 + rate, numberPeriods);
			return ((-presentValue * auxiliaryRate2) - (((payment / rate) * auxiliaryRate1) * (auxiliaryRate2 - 1)));
		}

		/// <summary>
		/// PMT() verified
		/// Calculates the payment for a loan based on constant payments and a constant interest rate.
		/// </summary>
		/// <param name="rate">Periodic Interest rate.</param>
		/// <param name="numberPeriods">Number of periods.</param>
		/// <param name="presentValue">Present value or principal.</param>
		/// <param name="futureValue">Future value.</param>
		/// <param name="compounding">Specifies when values are accrued, at the beginning or at the end of the period.</param>
		/// <returns></returns>
		public static double Payment(double rate, double numberPeriods, double presentValue, double futureValue, Compounding compounding)
		{
			if (numberPeriods == 0)
				throw new ArgumentException("Invalid Argument Value", "numberPeriods");

			if (rate == 0)
				return -(futureValue + presentValue) / numberPeriods;

			double auxiliaryRate1;
			if (compounding != Compounding.EndOfPeriod)
				auxiliaryRate1 = 1 + rate;
			else
				auxiliaryRate1 = 1;

			double auxiliaryRate2 = Math.Pow(1 + rate, numberPeriods);
			return ((-(futureValue + (presentValue * auxiliaryRate2)) / (auxiliaryRate1 * (auxiliaryRate2 - 1))) * rate);
		}

		/// <summary>
		/// PPMT()
		/// Returns the capital payment for a given period for an investment based on periodic, constant payments and a constant interest rate.
		/// </summary>
		/// <param name="rate">Periodic Interest Rate.</param>
		/// <param name="period">Period.</param>
		/// <param name="numberPeriods">Number of periods.</param>
		/// <param name="presentValue">Present value or principal.</param>
		/// <param name="futureValue">Future value.</param>
		/// <param name="compounding">Specifies when values are accrued, at the beginning or at the end of the period.</param>
		/// <returns></returns>
		public static double PrincipalPayment(double rate, int period, double numberPeriods, double presentValue, double futureValue, Compounding compounding)
		{
			if (period <= 0 || period >= numberPeriods + 1)
				throw new ArgumentException("Invalid Argument Value", "period");

			double payment = Payment(rate, numberPeriods, presentValue, futureValue, compounding);
			double interest = InterestPayment(rate, period, numberPeriods, presentValue, futureValue, compounding);
			return (payment - interest);
		}

		/// <summary>
		/// IPMT() verified
		/// Returns the interest payment for a given period for an investment based on periodic, constant payments and a constant interest rate.
		/// </summary>
		/// <param name="rate">Periodic Interest Rate.</param>
		/// <param name="period">Period.</param>
		/// <param name="numberPeriods">Number of periods.</param>
		/// <param name="presentValue">Present value or principal.</param>
		/// <param name="futureValue">Future value.</param>
		/// <param name="compounding">Specifies when values are accrued, at the beginning or at the end of the period.</param>
		/// <returns></returns>
		public static double InterestPayment(double rate, int period, double numberPeriods, double presentValue, double futureValue, Compounding compounding)
		{
			if (period <= 0 || period >= numberPeriods + 1)
				throw new ArgumentException("Invalid Argument Value", "period");

			if (compounding != Compounding.EndOfPeriod && period == 1)
				return 0;

			double payment = Payment(rate, numberPeriods, presentValue, futureValue, compounding);
			if (compounding != Compounding.EndOfPeriod)
				presentValue += payment;

			short auxiliaryPeriod;
			if (compounding != Compounding.EndOfPeriod)
				auxiliaryPeriod = 2;
			else
				auxiliaryPeriod = 1;
			double publicFutureValue = FutureValue(rate, period - auxiliaryPeriod, payment, presentValue, Compounding.EndOfPeriod);
			return (publicFutureValue * rate);
		}

		/// <summary>
		/// NPER() verified
		/// Returns the number of periods for an investment based on periodic, constant payments and a constant interest rate.
		/// </summary>
		/// <param name="rate">Periodic Interest Rate.</param>
		/// <param name="payment">Periodic Payment.</param>
		/// <param name="presentValue">Present value or principal.</param>
		/// <param name="futureValue">Future value.</param>
		/// <param name="compounding">Specifies when values are accrued, at the beginning or at the end of the period.</param>
		/// <returns></returns>
		public static double NumberOfPeriods(double rate, double payment, double presentValue, double futureValue, Compounding compounding)
		{
			if (rate <= -1)
				throw new ArgumentException("Invalid Argument Value", "rate");

			if (rate == 0)
			{
				if (payment == 0)
					throw new ArgumentException("Invalid Argument Value", "payment");

				return (-(presentValue + futureValue) / payment);
			}

			double auxiliaryRate1;
			if (compounding != Compounding.EndOfPeriod)
				auxiliaryRate1 = 1 + rate;
			else
				auxiliaryRate1 = 1;

			double auxiliaryPayment = (payment * auxiliaryRate1) / rate;
			double auxiliaryFutureValue = -futureValue + auxiliaryPayment;
			double auxiliaryPresentValue = presentValue + auxiliaryPayment;
			if (auxiliaryFutureValue < 0 && auxiliaryPresentValue < 0)
			{
				auxiliaryFutureValue = -1 * auxiliaryFutureValue;
				auxiliaryPresentValue = -1 * auxiliaryPresentValue;
			}
//j			else
//j			if (auxiliaryFutureValue <= 0 || auxiliaryPresentValue <= 0)
//j				throw new ArgumentException("Cannot Calculate Number of Periods");

			double periods = (Math.Log(auxiliaryFutureValue) - Math.Log(auxiliaryPresentValue)) / Math.Log(1 + rate);
			return periods;
		}

		/// <summary>
		/// IRR() 
		/// Returns the public rate of return for a series of cash flows
		/// </summary>
		/// <param name="cashFlow">cash flow.</param>
		/// <param name="guess">Guess.</param>
		/// <returns></returns>
		public static double publicRateOfReturn(double[] cashFlow, double guess)
		{
			if (guess == 0)
				guess = 0.033;

			short arrayUpperBound;
			try
			{
				arrayUpperBound = (short)cashFlow.GetUpperBound(0);
			}
			catch (Exception ex)
			{
				ex.GetType();
				throw new ArgumentException("Invalid Argument Value", "cashFlow");
			}
			if (guess <= -1)
				throw new ArgumentException("Invalid Argument Value", "guess");

			if (arrayUpperBound + 1 <= 1)
				throw new ArgumentException("Invalid Argument Value", "cashFlow");

			double auxiliaryValue1;
			if (cashFlow[0] > 0)
				auxiliaryValue1 = cashFlow[0];
			else
				auxiliaryValue1 = -cashFlow[0];

			short numberOfTries = 0;
			while (numberOfTries <= arrayUpperBound)
			{
				if (cashFlow[numberOfTries] > auxiliaryValue1)
					auxiliaryValue1 = cashFlow[numberOfTries];
				else
					if (-cashFlow[numberOfTries] > auxiliaryValue1)
						auxiliaryValue1 = -cashFlow[numberOfTries];

				numberOfTries++;
			}

			double auxiliaryValue3 = (auxiliaryValue1 * 1E-07) * 0.01;
			double auxiliaryValue4 = guess;
			double auxiliaryValue5 = OptionalPresentValue(cashFlow, auxiliaryValue4);
			double auxiliaryValue2;
			if (auxiliaryValue5 > 0)
				auxiliaryValue2 = auxiliaryValue4 + 1E-05;
			else
				auxiliaryValue2 = auxiliaryValue4 - 1E-05;

			if (auxiliaryValue2 <= -1)
				throw new ArgumentException("Invalid Argument Value");

			double auxiliaryValue6 = OptionalPresentValue(cashFlow, auxiliaryValue2);
			numberOfTries = 0;
			while (true)
			{
				double auxiliaryValue7;
				if (auxiliaryValue6 == auxiliaryValue5)
				{
					if (auxiliaryValue2 > auxiliaryValue4)
						auxiliaryValue4 -= 1E-05;
					else
						auxiliaryValue4 += 1E-05;

					auxiliaryValue5 = OptionalPresentValue(cashFlow, auxiliaryValue4);
					if (auxiliaryValue6 == auxiliaryValue5)
						throw new ArgumentException("Invalid Argument Value");
				}

				auxiliaryValue4 = auxiliaryValue2 - (((auxiliaryValue2 - auxiliaryValue4) * auxiliaryValue6) / (auxiliaryValue6 - auxiliaryValue5));
				if (auxiliaryValue4 <= -1)
					auxiliaryValue4 = (auxiliaryValue2 - 1) * 0.5;

				auxiliaryValue5 = OptionalPresentValue(cashFlow, auxiliaryValue4);
				if (auxiliaryValue4 > auxiliaryValue2)
					auxiliaryValue1 = auxiliaryValue4 - auxiliaryValue2;
				else
					auxiliaryValue1 = auxiliaryValue2 - auxiliaryValue4;

				if (auxiliaryValue5 > 0)
					auxiliaryValue7 = auxiliaryValue5;
				else
					auxiliaryValue7 = -auxiliaryValue5;

				if (auxiliaryValue7 < auxiliaryValue3 && auxiliaryValue1 < 1E-07)
					return auxiliaryValue4;

				auxiliaryValue1 = auxiliaryValue5;
				auxiliaryValue5 = auxiliaryValue6;
				auxiliaryValue6 = auxiliaryValue1;
				auxiliaryValue1 = auxiliaryValue4;
				auxiliaryValue4 = auxiliaryValue2;
				auxiliaryValue2 = auxiliaryValue1;
				numberOfTries++;
				if (numberOfTries > 50)
					throw new ArgumentException("Invalid Argument Value");
			}
		}

		/// <summary>
		/// present value.
		/// </summary>
		/// <param name="cashFlow">Cash flow.</param>
		/// <param name="guess">Guess.</param>
		/// <returns></returns>
		private static double OptionalPresentValue(double[] cashFlow, double guess)
		{
			short iterator = 0;
			short arrayUpperBound = (short)cashFlow.GetUpperBound(0);
			while ((iterator <= arrayUpperBound) && (cashFlow[iterator] == 0))
			{
				iterator++;
			}

			double presentValue = 0;
			double precision = 1 + guess;
			for (short i = arrayUpperBound; i >= iterator; i += -1)
			{
				presentValue /= precision;
				presentValue += cashFlow[i];
			}

			return presentValue;
		}

		/// <summary>
		/// Returns the public rate of return for a series of periodic cash flows, 
		/// considering both cost of investment and interest on reinvestment of cash
		/// </summary>
		/// <param name="cashFlow">Value array.</param>
		/// <param name="financeRate">Finance rate.</param>
		/// <param name="reInvestRate">Reinvestment rate.</param>
		/// <returns></returns>
		public static double ReinvestedpublicRateOfReturn(double[] cashFlow, double financeRate, double reInvestRate)
		{
			if (cashFlow.Rank != 1)
				throw new ArgumentException("Invalid Argument Value", "CashFlow");

			if (financeRate == -1)
				throw new ArgumentException("Invalid Argument Value", "FinanceRate");

			if (reInvestRate == -1)
				throw new ArgumentException("Invalid Argument Value", "ReinvestRate");

			if (cashFlow.GetUpperBound(0) + 1 <= 1)
				throw new ArgumentException("Invalid Argument Value", "CashFlow");

			double presentValueFinance = publicNetPresentValue(cashFlow, financeRate, -1);
			if (presentValueFinance == 0)
				throw new DivideByZeroException("Cannot Divide By Zero");

			short arrayUpperBound = (short)cashFlow.GetUpperBound(0);
			double presentValueReInvest = publicNetPresentValue(cashFlow, reInvestRate, 1);
			double publicReInvestedRate = (-presentValueReInvest * Math.Pow(1 + reInvestRate, arrayUpperBound)) / (presentValueFinance * (1 + financeRate));
			if (publicReInvestedRate < 0)
				throw new ArgumentException("Argument Invalid Value");

			return (Math.Pow(publicReInvestedRate, 1 / (arrayUpperBound - 1)) - 1);
		}

		/// <summary>
		/// NPV()
		/// Returns the net present value of an investment based on a discount rate and a series of future payments
		/// (negative values) and income (positive values),
		/// </summary>
		/// <param name="cashFlow">Cash Flow.</param>
		/// <param name="rate">Periodic Interest Rate.</param>
		/// <returns></returns>
		public static double NetPresentValue(double[] cashFlow, double rate)
		{
			if (cashFlow == null)
				throw new ArgumentException("Invalid Argument Value", "cashFlow");

			if (cashFlow.Rank != 1)
				throw new ArgumentException("Invalid Argument Value", "cashFlow");

			if (rate == -1)
				throw new ArgumentException("Invalid Argument Value", "rate");

			if (cashFlow.GetUpperBound(0) < 0)
				throw new ArgumentException("Invalid Argument Value", "cashFlow");

			return publicNetPresentValue(cashFlow, rate, 0);
		}

		/// <summary>
		/// Returns the present value.
		/// </summary>
		/// <param name="cashFlow">Cash flow.</param>
		/// <param name="rate">Periodic Interest Rate.</param>
		/// <param name="paymentDueCollectable">Payment is due or collectable.</param>
		/// <returns></returns>
		private static double publicNetPresentValue(double[] cashFlow, double rate, short paymentDueCollectable)
		{
			bool flagLessThenZero = paymentDueCollectable < 0;
			bool flagGreaterThenZero = paymentDueCollectable > 0;
			double auxiliaryValue = 1;
			double publicPresentValue = 0;
			short arrayUpperBound = (short)cashFlow.GetUpperBound(0);
			for (int arrayIterador = 0; arrayIterador <= arrayUpperBound; arrayIterador++)
			{
				double arrayValue = cashFlow[arrayIterador];
				auxiliaryValue += (auxiliaryValue * rate);
				if ((!flagLessThenZero || arrayValue <= 0) && (!flagGreaterThenZero || arrayValue >= 0))
					publicPresentValue += (arrayValue / auxiliaryValue);
			}

			return publicPresentValue;
		}


		/// <summary>
		/// RATE() verified
		/// Returns the interest rate per period of an annuity
		/// </summary>
		/// <param name="numberPeriods">Number of periods.</param>
		/// <param name="payment">Payment.</param>
		/// <param name="presentValue">Present value or principal.</param>
		/// <param name="futureValue">Future value.</param>
		/// <param name="compounding">Specifies when values are accrued, at the beginning or at the end of the period.</param>
		/// <param name="guess">Guess (optional parameter).</param>
		/// <returns></returns>
		public static double Rate(double numberPeriods, double payment, double presentValue, double futureValue, Compounding compounding, double guess)
		{
			if (numberPeriods <= 0)
				throw new ArgumentException("numberPeriods must be greater than Zero", "numberPeriods");

			if (guess == 0)
				guess = 0.1;

			double precision = guess;
			double auxiliaryRate3 = EvaluateRate(precision, numberPeriods, payment, presentValue, futureValue, compounding);
			double auxiliaryRate1;
			if (auxiliaryRate3 > 0)
				auxiliaryRate1 = precision / 2;
			else
				auxiliaryRate1 = precision * 2;

			double auxiliaryRate2 = EvaluateRate(auxiliaryRate1, numberPeriods, payment, presentValue, futureValue, compounding);
			byte numberofTries = 0;
			while (true)
			{
				if (auxiliaryRate2 == auxiliaryRate3)
				{
					if (auxiliaryRate1 > precision)
						precision -= 1E-05;
					else
						precision -= -1E-05;

					auxiliaryRate3 = EvaluateRate(precision, numberPeriods, payment, presentValue, futureValue, compounding);
					if (auxiliaryRate2 == auxiliaryRate3)
						throw new ArgumentException("Cannot Calculate Divide By Zero");
				}
				precision = auxiliaryRate1 - (((auxiliaryRate1 - precision) * auxiliaryRate2) / (auxiliaryRate2 - auxiliaryRate3));
				auxiliaryRate3 = EvaluateRate(precision, numberPeriods, payment, presentValue, futureValue, compounding);
				if (Math.Abs(auxiliaryRate3) < 1E-07)
					return precision;

				double auxiliaryRate4 = auxiliaryRate3;
				auxiliaryRate3 = auxiliaryRate2;
				auxiliaryRate2 = auxiliaryRate4;
				auxiliaryRate4 = precision;
				precision = auxiliaryRate1;
				auxiliaryRate1 = auxiliaryRate4;
				numberofTries++;
				if (numberofTries > 100)
					return precision; //j throw new ArgumentException("Cannot Calculate Rate");
			}
		}

		/// <summary>
		/// Evaluates the rate.
		/// </summary>
		/// <param name="rate">Periodic Interest Rate.</param>
		/// <param name="numberPeriods">Number of periods.</param>
		/// <param name="payment">Payment.</param>
		/// <param name="presentValue">Present value or principal.</param>
		/// <param name="futureValue">Future value.</param>
		/// <param name="compounding">Specifies when values are accrued, at the beginning or at the end of the period.</param>
		/// <returns></returns>
		private static double EvaluateRate(double rate, double numberPeriods, double payment, double presentValue, double futureValue, Compounding compounding)
		{
			if (rate == 0)
				return ((presentValue + (payment * numberPeriods)) + futureValue);

			double auxiliaryRate1;
			if (compounding != Compounding.EndOfPeriod)
				auxiliaryRate1 = 1 + rate;
			else
				auxiliaryRate1 = 1;

			double auxiliaryRate2 = Math.Pow(1 + rate, numberPeriods);
			return (((presentValue * auxiliaryRate2) + (((payment * auxiliaryRate1) * (auxiliaryRate2 - 1)) / rate)) + futureValue);
		}
	}

	public enum Compounding
	{
		BeginningOfPeriod = 0,
		EndOfPeriod = 1,
	}
}
