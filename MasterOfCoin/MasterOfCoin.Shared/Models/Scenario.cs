namespace MasterOfCoin
{
	public class Scenario
	{
		public string Title { get; set; }
		public string SubTitle { get; set; }
		public bool IsDepleting { get; set; }
		public CompoundingFrequency CompoundingFrequency { get; set; }
		public PaymentFrequency PaymentFrequency { get; set; }
		public PeriodFrequency PeriodFrequency { get; set; }

		public Variable Rate { get; set; }
		public Variable PresentValue { get; set; }
		public Variable FutureValue { get; set; }
		public Variable Payment { get; set; }
		public Variable Periods { get; set; }

		public Scenario(string title, string subTitle, bool isDepleting, CompoundingFrequency compoundingFrequency, PaymentFrequency paymentFrequency, PeriodFrequency periodFrequency)
		{
			Title = title;
			SubTitle = subTitle;
			IsDepleting = isDepleting;
			CompoundingFrequency = compoundingFrequency;
			PaymentFrequency = paymentFrequency;
			PeriodFrequency = periodFrequency;
		}
	}
}