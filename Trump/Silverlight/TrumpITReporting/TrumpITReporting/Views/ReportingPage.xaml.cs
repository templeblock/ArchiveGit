using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Globalization;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using ClassLibrary;
using System.Windows.Navigation;
#if TELERIK_CHARTS
using Telerik.Windows.Controls.Charting;
#endif

namespace TrumpITReporting
{
	public partial class ReportingPage : Page
	{
		public ReportingPage()
		{
			//new Telerik.Windows.Controls.VistaTheme().IsApplicationTheme = true;
			InitializeComponent();
			base.Loaded += OnLoaded;
		}

		// Executes when the user navigates to this page.
		protected override void OnNavigatedTo(NavigationEventArgs e)
		{
		}

		private void OnLoaded(object sender, RoutedEventArgs e)
		{
			base.Loaded -= OnLoaded;

			x_LayoutRoot.DataContext = CampaignSummaryData.Data;
			
			//(x_InvitationsChart.Series[0] as DataPointSeries).ItemsSource = CampaignSummaryData.Data.InvitationsData;
			//(x_InvitationsChart.Series[1] as DataPointSeries).ItemsSource = CampaignSummaryData.Data.InvitationsData;

			//SeriesMapping sm1 = new SeriesMapping();
			//sm1.SeriesDefinition = new StackedBarSeriesDefinition("Stack1");
			//sm1.LegendLabel = "Sent";
			//sm1.CollectionIndex = 0;
			//sm1.ItemMappings.Add(new ItemMapping() { DataPointMember = DataPointMember.YValue });

			//SeriesMapping sm2 = new SeriesMapping();
			//sm2.SeriesDefinition = new StackedBarSeriesDefinition("Stack1");
			//sm2.LegendLabel = "Converted";
			//sm2.CollectionIndex = 1;
			//sm2.ItemMappings.Add(new ItemMapping() { DataPointMember = DataPointMember.YValue });

			//x_InvitationsChart.SeriesMappings.Add(sm1);
			//x_InvitationsChart.SeriesMappings.Add(sm2);

			//List<double[]> itemsSource = new List<double[]> { 
			//SeriesExtensions.GetUserData(5, 0), 
			//SeriesExtensions.GetUserData(5, 1)};

			//x_InvitationsChart.ItemsSource = itemsSource;

#if TELERIK_CHARTS
			double[] minTemps = new double[] { 10, 12, 15, 18, 22, 26, 20, 22, 20, 24, 19 };
			double[] maxTemps = new double[] { 35, 30, 32, 31, 35, 37, 38, 35, 39, 41, 38 };
			DateTime startDate = new DateTime(DateTime.Today.Year, 6, 1);
			DateTime[] weeks = new DateTime[12];
			for (int i = 0; i < 12; i++)
				weeks[i] = startDate.AddDays(7 * i);
			DataSeries series1 = GetDataSeries(maxTemps, weeks);
			series1.LegendLabel = "Max. Temperatures";
			DataSeries series2 = GetDataSeries(minTemps, weeks);
			series2.LegendLabel = "Min. Temperatures";
			x_InvitationsArea.DataSeries.Add(series1);
			x_InvitationsArea.DataSeries.Add(series2);

			x_InvitationsArea.DataSeries[0].Definition.Appearance.Stroke = "#FFFF5357".ToColor().ToBrush();
			x_InvitationsArea.DataSeries[0].Definition.Appearance.PointMark.Stroke = "#FFFF5357".ToColor().ToBrush();

			x_InvitationsArea.DataSeries[1].Definition.Appearance.Stroke = "#FF79A0FB".ToColor().ToBrush();
			x_InvitationsArea.DataSeries[1].Definition.Appearance.PointMark.Stroke = "#FF79A0FB".ToColor().ToBrush();
	
			x_InvitationsArea.AxisY.AutoRange = false;
			x_InvitationsArea.AxisY.MinValue = -20;
			x_InvitationsArea.AxisY.MaxValue = 50;
			x_InvitationsArea.AxisY.Step = 10;
			x_InvitationsArea.AxisY.DefaultLabelFormat = "#VAL'C";

			x_InvitationsArea.AxisX.IsDateTime = true;
			x_InvitationsArea.AxisX.DefaultLabelFormat = "dd MMMM, yyyy";
			x_InvitationsArea.AxisX.LabelStep = 3;

			x_InvitationsArea.AnimationSettings = new AnimationSettings();
			x_InvitationsArea.AnimationSettings.ItemAnimationDuration = TimeSpan.FromSeconds(.1);
			x_InvitationsArea.AnimationSettings.ItemDelay = TimeSpan.FromSeconds(.1);
#endif
			//x_RevenueChart.DefaultSeriesDefinition = new PieSeriesDefinition();
			//x_RevenueChart.ItemsSource = SeriesExtensions.GetUserRadialData();
			//x_RevenueChart.DefaultView.ChartArea;
		}

#if TELERIK_CHARTS
		private DataSeries GetDataSeries(double[] values, DateTime[] weeks)
		{
			DataSeries series = new DataSeries();
			series.Definition = new SplineSeriesDefinition();
			for (int i = 0; i < values.Length; i++)
			{
				DataPoint point = new DataPoint();
				point.YValue = values[i];
				point.XValue = weeks[i].ToOADate();
				series.Add(point);
			}

			return series;
		}
#endif
		private void OnCampaignCategoryAllChecked(object sender, RoutedEventArgs e)
		{
			CampaignSummaryData.Data.Name = (sender as ContentControl).Content.ToString();
			CampaignSummaryData.Data.SuccessData = Success.BuildData();
			CampaignSummaryData.Data.RevenueData = Revenue.BuildData();
			CampaignSummaryData.Data.CostData = Cost.BuildData();
			CampaignSummaryData.Data.InvitationsData = Invitations.BuildData();
			CampaignSummaryData.Data.PeopleData = People.BuildData();
			CampaignSummaryData.Data.CampaignsData = Campaigns.BuildData();
			CampaignSummaryData.Data.ProspectData = Prospect.BuildData();
			CampaignSummaryData.Data.ClosingData = Closing.BuildData();
			CampaignSummaryData.Data.OffersData = Offers.BuildData();
			CampaignSummaryData.Data.EmailData = Email.BuildData();
			CampaignSummaryData.Data.SocialData = Social.BuildData();
			CampaignSummaryData.Data.PostcardData = Postcard.BuildData();
		}


		private void OnCampaignCategoryChecked(object sender, RoutedEventArgs e)
		{
			CampaignSummaryData.Data.Name = (sender as ContentControl).Content.ToString();
			CampaignSummaryData.Data.SuccessData = Success.BuildProductData();
			CampaignSummaryData.Data.RevenueData = Revenue.BuildProductData();
			CampaignSummaryData.Data.CostData = Cost.BuildProductData();
			CampaignSummaryData.Data.InvitationsData = Invitations.BuildData();
			CampaignSummaryData.Data.PeopleData = People.BuildData();
			CampaignSummaryData.Data.CampaignsData = Campaigns.BuildData();
			CampaignSummaryData.Data.ProspectData = Prospect.BuildData();
			CampaignSummaryData.Data.ClosingData = Closing.BuildData();
			CampaignSummaryData.Data.OffersData = Offers.BuildData();
			CampaignSummaryData.Data.EmailData = Email.BuildData();
			CampaignSummaryData.Data.SocialData = Social.BuildData();
			CampaignSummaryData.Data.PostcardData = Postcard.BuildData();
		}

		private void OnViewDateRangeChanged(object sender, System.Windows.Controls.SelectionChangedEventArgs e)
		{
			ComboBox comboBox = sender as ComboBox;
			ComboBoxItem item = comboBox.SelectedItem as ComboBoxItem;
			string command = (item != null ? item.Tag.ToString() : null);
			switch (command)
			{
				case "All": 
				{
					CampaignSummaryData.Data.ViewStartDate = DateTime.Parse("1/1/2000");
					CampaignSummaryData.Data.ViewEndDate = DateTime.Today;
					break;
				}
				case "MonthToDate":
				{
					CampaignSummaryData.Data.ViewStartDate = DateTime.Today - TimeSpan.FromDays((DateTime.Today.Day) - 1);
					CampaignSummaryData.Data.ViewEndDate = DateTime.Today;
					break;
				}
				case "QuarterToDate":
				{
					CampaignSummaryData.Data.ViewStartDate = DateTime.Today - TimeSpan.FromDays(((DateTime.Today.DayOfYear) % 91) - 1);
					CampaignSummaryData.Data.ViewEndDate = DateTime.Today;
					break;
				}
				case "YearToDate":
				{
					CampaignSummaryData.Data.ViewStartDate = DateTime.Today - TimeSpan.FromDays((DateTime.Today.DayOfYear) - 1);
					CampaignSummaryData.Data.ViewEndDate = DateTime.Today;
					break;
				}
				case "LastMonth":
				{
					CampaignSummaryData.Data.ViewStartDate = DateTime.Today - TimeSpan.FromDays((DateTime.Today.Day) + 30);
					CampaignSummaryData.Data.ViewEndDate = DateTime.Today - TimeSpan.FromDays((DateTime.Today.Day));
					break;
				}
				case "LastQuarter":
				{
					CampaignSummaryData.Data.ViewStartDate = DateTime.Today - TimeSpan.FromDays(((DateTime.Today.DayOfYear) % 91) + 91);
					CampaignSummaryData.Data.ViewEndDate = DateTime.Today - TimeSpan.FromDays((DateTime.Today.DayOfYear % 91));
					break;
				}
				case "LastYear":
				{
					CampaignSummaryData.Data.ViewStartDate = DateTime.Today - TimeSpan.FromDays(DateTime.Today.DayOfYear + 364 + (DateTime.IsLeapYear(DateTime.Today.Year - 1) ? 1 : 0));
					CampaignSummaryData.Data.ViewEndDate = DateTime.Today - TimeSpan.FromDays((DateTime.Today.DayOfYear));
					break;
				}
				case "Last30Days":
				{
					CampaignSummaryData.Data.ViewStartDate = DateTime.Today - TimeSpan.FromDays(30 - 1);
					CampaignSummaryData.Data.ViewEndDate = DateTime.Today;
					break;
				}
				case "Last12Months":
				{
					CampaignSummaryData.Data.ViewStartDate = DateTime.Today - TimeSpan.FromDays(365 + (DateTime.IsLeapYear(DateTime.Today.Year) ? 1 : 0));
					CampaignSummaryData.Data.ViewEndDate = DateTime.Today;
					break;
				}
				case "Custom":
				{
					break;
				}
			}
		}

		private void OnAllPeopleClick(object sender, RoutedEventArgs e)
		{
			FrameworkElement element = (sender as FrameworkElement);
			IEnumerable<CheckBox> checkboxes = (element.Parent as FrameworkElement).Parent.ChildListOfType<CheckBox>();
			foreach (CheckBox checkbox in checkboxes)
				checkbox.IsChecked = true;
		}

		private void OnNonePeopleClick(object sender, RoutedEventArgs e)
		{
			FrameworkElement element = (sender as FrameworkElement);
			IEnumerable<CheckBox> checkboxes = (element.Parent as FrameworkElement).Parent.ChildListOfType<CheckBox>();
			foreach (CheckBox checkbox in checkboxes)
				checkbox.IsChecked = false;
		}
	}

	public class CampaignSummaryData : NotifyPropertyChanges
	{
		public static CampaignSummaryData Data { get { return _Data; } }
		private static CampaignSummaryData _Data = new CampaignSummaryData();

		public string Name { get { return _Name; } set { SetProperty(ref _Name, value); } }
		private string _Name;

		public DateTime ViewStartDate { get { return _ViewStartDate; } set { SetProperty(ref _ViewStartDate, value); } }
		private DateTime _ViewStartDate;

		public DateTime ViewEndDate { get { return _ViewEndDate; } set { SetProperty(ref _ViewEndDate, value); } }
		private DateTime _ViewEndDate;

		public List<Success> SuccessData { get { return _SuccessData; } set { SetProperty(ref _SuccessData, value); } }
		private List<Success> _SuccessData = Success.BuildData();

		[DependsOn("SuccessData")]
		public double SuccessTotal { get
		{
			double total = 0;
			foreach (Success item in SuccessData)
				total += item.Value;
			return total;
		}}

		public List<Revenue> RevenueData { get { return _RevenueData; } set { SetProperty(ref _RevenueData, value); } }
		private List<Revenue> _RevenueData = Revenue.BuildData();

		[DependsOn("RevenueData")]
		public double RevenueTotal { get
		{
			double total = 0;
			foreach (Revenue item in RevenueData)
				total += item.Value;
			return total;
		}}

		public List<Cost> CostData { get { return _CostData; } set { SetProperty(ref _CostData, value); } }
		private List<Cost> _CostData = Cost.BuildData();

		[DependsOn("CostData")]
		public double CostTotal { get
		{
			double total = 0;
			foreach (Cost item in CostData)
				total += item.Value;
			return total;
		}}

		public List<Invitations> InvitationsData { get { return _InvitationsData; } set { SetProperty(ref _InvitationsData, value); } }
		private List<Invitations> _InvitationsData = Invitations.BuildData();

		public PeopleCollection PeopleData { get { return _PeopleData; } set { SetProperty(ref _PeopleData, value); } }
		private PeopleCollection _PeopleData = People.BuildData();

		[DependsOn("PeopleData")]
		public int PeopleTotal { get
		{
			int total = 0;
			foreach (People item in PeopleData)
				total += item.Count;
			return total;
		}}

		public List<Campaigns> CampaignsData { get { return _CampaignsData; } set { SetProperty(ref _CampaignsData, value); } }
		private List<Campaigns> _CampaignsData = Campaigns.BuildData();

		public List<Prospect> ProspectData { get { return _ProspectData; } set { SetProperty(ref _ProspectData, value); } }
		private List<Prospect> _ProspectData = Prospect.BuildData();

		public List<Closing> ClosingData { get { return _ClosingData; } set { SetProperty(ref _ClosingData, value); } }
		private List<Closing> _ClosingData = Closing.BuildData();

		[DependsOn("ClosingData")]
		public double ClosingTotal { get
		{
			int total = 0;
			foreach (Closing item in ClosingData)
				total += item.Count;
			return total;
		}}

		public List<Offers> OffersData { get { return _OffersData; } set { SetProperty(ref _OffersData, value); } }
		private List<Offers> _OffersData = Offers.BuildData();

		[DependsOn("OffersData")]
		public double OffersTotal { get
		{
			int total = 0;
			foreach (Offers item in OffersData)
				total += item.Count;
			return total;
		}}

		public List<Email> EmailData { get { return _EmailData; } set { SetProperty(ref _EmailData, value); } }
		private List<Email> _EmailData = Email.BuildData();

		[DependsOn("EmailData")]
		public double EmailTotal { get
		{
			double total = 0;
			foreach (Email item in EmailData)
				total += item.Value;
			return total;
		}}

		public List<Social> SocialData { get { return _SocialData; } set { SetProperty(ref _SocialData, value); } }
		private List<Social> _SocialData = Social.BuildData();

		[DependsOn("SocialData")]
		public double SocialTotal { get
		{
			double total = 0;
			foreach (Social item in SocialData)
				total += item.Value;
			return total;
		}}

		public List<Postcard> PostcardData { get { return _PostcardData; } set { SetProperty(ref _PostcardData, value); } }
		private List<Postcard> _PostcardData = Postcard.BuildData();

		[DependsOn("PostcardData")]
		public double PostcardTotal { get
		{
			double total = 0;
			foreach (Postcard item in PostcardData)
				total += item.Value;
			return total;
		}}
	}

	public class Success : NotifyPropertyChanges
	{
		public string Name { get { return _Name; } set { SetProperty(ref _Name, value); } }
		private string _Name;

		public double Value { get { return _Value; } set { SetProperty(ref _Value, value); } }
		private double _Value;

		public static List<Success> BuildData()
		{
			List<Success> list = new List<Success>()
			{
				new Success() { Name = "Business Opportunities", Value = Vary.Double(12000.00) },
				new Success() { Name = "Silhouette Solutions", Value = Vary.Double(6000.00) },
				new Success() { Name = "Ideal Health", Value = Vary.Double(1000.00) },
				new Success() { Name = "Quick Stiks", Value = Vary.Double(0.00) },
				new Success() { Name = "SnazzleSnaxx", Value = Vary.Double(0.00) },
			};
			return list;
		}

		public static List<Success> BuildProductData()
		{
			List<Success> list = new List<Success>()
			{
				new Success() { Name = "Trump Towers", Value = Vary.Double(5000.00) },
				new Success() { Name = "3D Trilogy", Value = Vary.Double(2500.00) },
				new Success() { Name = "Opus", Value = Vary.Double(2500.00) },
			};
			return list;
		}
	}

	public class Revenue : NotifyPropertyChanges
	{
		public string Name { get { return _Name; } set { SetProperty(ref _Name, value); } }
		private string _Name;

		public double Value { get { return _Value; } set { SetProperty(ref _Value, value); } }
		private double _Value;

		public static List<Revenue> BuildData()
		{
			List<Revenue> list = new List<Revenue>()
			{
				new Revenue() { Name = "Product Commissions", Value = Vary.Double(7000) },
				new Revenue() { Name = "FastStart Bonuses", Value = Vary.Double(3000) },
				new Revenue() { Name = "TrumpIT System Commissions", Value = Vary.Double(2000) },
				new Revenue() { Name = "Advertising Bonuses", Value = Vary.Double(2000) },
				new Revenue() { Name = "Retail Profits", Value = Vary.Double(4000) },
			};
			return list;
		}

		public static List<Revenue> BuildProductData()
		{
			List<Revenue> list = new List<Revenue>()
			{
				new Revenue() { Name = "Trump Towers", Value = Vary.Double(5000.00) },
				new Revenue() { Name = "3D Trilogy", Value = Vary.Double(2500.00) },
				new Revenue() { Name = "Opus", Value = Vary.Double(2500.00) },
			};
			return list;
		}
	}

	public class Cost : NotifyPropertyChanges
	{
		public string Name { get { return _Name; } set { SetProperty(ref _Name, value); } }
		private string _Name;

		public double Value { get { return _Value; } set { SetProperty(ref _Value, value); } }
		private double _Value;

		public static List<Cost> BuildData()
		{
			List<Cost> list = new List<Cost>()
			{
				new Cost() { Name = "Name List Costs", Value = Vary.Double(1000) },
				new Cost() { Name = "Postcard Costs", Value = Vary.Double(1400) },
				new Cost() { Name = "Postage Costs", Value = Vary.Double(500) },
				new Cost() { Name = "Taxes", Value = Vary.Double(100) },
			};
			return list;
		}

		public static List<Cost> BuildProductData()
		{
			List<Cost> list = new List<Cost>()
			{
				new Cost() { Name = "Trump Towers", Value = Vary.Double(5000.00) },
				new Cost() { Name = "3D Trilogy", Value = Vary.Double(2500.00) },
				new Cost() { Name = "Opus", Value = Vary.Double(2500.00) },
			};
			return list;
		}
	}

	public class Invitations : NotifyPropertyChanges
	{
		public string Name { get { return _Name; } set { SetProperty(ref _Name, value); } }
		private string _Name;

		public string ShortName { get { return _ShortName; } set { SetProperty(ref _ShortName, value); } }
		private string _ShortName;

		public int Sent { get { return _Sent; } set { SetProperty(ref _Sent, value); } }
		private int _Sent;

		public int Converted { get { return _Converted; } set { SetProperty(ref _Converted, value); } }
		private int _Converted;

		[DependsOn("Sent")]
		[DependsOn("Converted")]
		public double Rate { get { return (_Sent != 0 ? (double)_Converted / _Sent : 0); } }

		public static List<Invitations> BuildData()
		{
			List<Invitations> list = new List<Invitations>()
			{
				new Invitations() { Name = "Emails Sent", ShortName = "Email Conversions", Sent = 1000, Converted = 200, },
				new Invitations() { Name = "Postcards Sent", ShortName = "Postcard Conversions", Sent = 500, Converted = 100, },
				new Invitations() { Name = "Social Media Clicks", ShortName = "Social Media Conversions", Sent = 750, Converted = 150, },
			};
			return list;
		}
	}

	public class PeopleCollection : ObservableCollection<People>, IDisposable
	{
		public void Dispose()
		{
			foreach (People item in this)
				item.Dispose();
			Clear();
		}
	}

	public class People : NotifyPropertyChanges
	{
		public string Name { get { return _Name; } set { SetProperty(ref _Name, value); } }
		private string _Name;

		//[DependsOn("Count")]
		//[DependsOn("View")]
		//public int Value { get { return (_View ? _Count : 0); } }

		public int Value { get { return _Value; } set { SetProperty(ref _Value, value); } }
		private int _Value;

		public int Count { get { return _Count; } set { SetProperty(ref _Count, value, delegate(int count) { if (View) Value = count; else Value = 0; }); } }
		private int _Count;

		public bool View { get { return _View; } set { SetProperty(ref _View, value, delegate(bool view) { if (view) Value = Count; else Value = 0; }); } }
		private bool _View = RandomExt.Bool();

		public static PeopleCollection BuildData()
		{
			if (_Data == null)
			_Data = new PeopleCollection()
			{
				new People() { Name = "Frontline Marketer", Count = RandomExt.Int(500), View = RandomExt.Bool(), },
				new People() { Name = "Frontline Customer", Count = RandomExt.Int(500), View = RandomExt.Bool(), },
				new People() { Name = "Downline Marketer", Count = RandomExt.Int(500), View = RandomExt.Bool(), },
				new People() { Name = "Downline Customer", Count = RandomExt.Int(500), View = RandomExt.Bool(), },
				new People() { Name = "Viral Marketer", Count = RandomExt.Int(500), View = RandomExt.Bool(), },
				new People() { Name = "Viral Customer", Count = RandomExt.Int(500), View = RandomExt.Bool(), },
			};
			return _Data;
		}
		private static PeopleCollection _Data;
	}

	public class Campaigns : NotifyPropertyChanges
	{
		public string Name { get { return _Name; } set { SetProperty(ref _Name, value); } }
		private string _Name;

		public string CampaignCategory { get { return _CampaignCategory; } set { SetProperty(ref _CampaignCategory, value); } }
		private string _CampaignCategory;

		public string CampaignTypeName { get { return _CampaignTypeName; } set { SetProperty(ref _CampaignTypeName, value); } }
		private string _CampaignTypeName;

		public int CampaignType { get { return _CampaignType; } set { SetProperty(ref _CampaignType, value); } }
		private int _CampaignType;

		public string Date { get { return _Date; } set { SetProperty(ref _Date, value); } }
		private string _Date;

		public double Value { get { return _Value; } set { SetProperty(ref _Value, value); } }
		private double _Value;

		public bool IsSelected { get { return _IsSelected; } set { SetProperty(ref _IsSelected, value); } }
		private bool _IsSelected;

		public static List<Campaigns> BuildData()
		{
			List<Campaigns> list = new List<Campaigns>()
			{
				new Campaigns() { Name = "My 3D trilogy 1", CampaignTypeName = "3D Trilogy", CampaignCategory = "Business Opportunities", Date = "01/07/2009", CampaignType = 1, Value = Vary.Double(175.63) },
				new Campaigns() { Name = "My Trump Towers 1", CampaignTypeName = "Trump Towers", CampaignCategory = "Silhouette Solutions", Date = "02/07/2009", CampaignType = 2, Value = Vary.Double(102.77) },
				new Campaigns() { Name = "My Opus 1", CampaignTypeName = "Opus", CampaignCategory = "Ideal Health", Date = "03/07/2009", CampaignType = 3, Value = Vary.Double(49.33) },
				new Campaigns() { Name = "My 3D trilogy 2", CampaignTypeName = "3D Trilogy", CampaignCategory = "Quick Stiks", Date = "04/07/2009", CampaignType = 1, Value = Vary.Double(175.63) },
				new Campaigns() { Name = "My Trump Towers 2", CampaignTypeName = "Trump Towers", CampaignCategory = "SnazzleSnaxx", Date = "05/07/2009", CampaignType = 2, Value = Vary.Double(102.77) },
				new Campaigns() { Name = "My Opus 2", CampaignTypeName = "Opus", CampaignCategory = "Business Opportunities", Date = "06/07/2009", CampaignType = 3, Value = Vary.Double(49.33) },
				new Campaigns() { Name = "My 3D trilogy 3", CampaignTypeName = "3D Trilogy", CampaignCategory = "Silhouette Solutions", Date = "07/07/2009", CampaignType = 1, Value = Vary.Double(175.63) },
				new Campaigns() { Name = "My Trump Towers 3", CampaignTypeName = "Trump Towers", CampaignCategory = "Ideal Health", Date = "08/07/2009", CampaignType = 2, Value = Vary.Double(102.77) },
				new Campaigns() { Name = "My Opus 3", CampaignTypeName = "Opus", CampaignCategory = "Quick Stiks", Date = "09/07/2009", CampaignType = 3, Value = Vary.Double(49.33) },
				new Campaigns() { Name = "My 3D trilogy 4", CampaignTypeName = "3D Trilogy", CampaignCategory = "SnazzleSnaxx", Date = "10/07/2009", CampaignType = 1, Value = Vary.Double(175.63) },
				new Campaigns() { Name = "My Trump Towers 4", CampaignTypeName = "Trump Towers", CampaignCategory = "Business Opportunities", Date = "11/07/2009", CampaignType = 2, Value = Vary.Double(102.77) },
				new Campaigns() { Name = "My Opus 4", CampaignTypeName = "Opus", CampaignCategory = "Silhouette Solutions", Date = "12/07/2009", CampaignType = 3, Value = Vary.Double(49.33) },
			};
			return list;
		}
	}

	public class Prospect : NotifyPropertyChanges
	{
		public string Name { get { return _Name; } set { SetProperty(ref _Name, value); } }
		private string _Name;

		public string CampaignName { get { return _CampaignName; } set { SetProperty(ref _CampaignName, value); } }
		private string _CampaignName;

		public int Score { get { return _Score; } set { SetProperty(ref _Score, value); } }
		private int _Score;

		public int InvitationType { get { return _InvitationType; } set { SetProperty(ref _InvitationType, value); } }
		private int _InvitationType = RandomExt.Int(2);

		public int Reminders { get { return _Reminders; } set { SetProperty(ref _Reminders, value); } }
		private int _Reminders = RandomExt.Int(4);

		public bool VisitedWebsite { get { return _VisitedWebsite; } set { SetProperty(ref _VisitedWebsite, value); } }
		private bool _VisitedWebsite = RandomExt.Bool();

		public bool VipRegistered { get { return _VipRegistered; } set { SetProperty(ref _VipRegistered, value); } }
		private bool _VipRegistered = RandomExt.Bool();

		public bool VipAttended { get { return _VipAttended; } set { SetProperty(ref _VipAttended, value); } }
		private bool _VipAttended = RandomExt.Bool();

		public bool LocalEventRegistered { get { return _LocalEventRegistered; } set { SetProperty(ref _LocalEventRegistered, value); } }
		private bool _LocalEventRegistered = RandomExt.Bool();

		public bool LocalEventAttended { get { return _LocalEventAttended; } set { SetProperty(ref _LocalEventAttended, value); } }
		private bool _LocalEventAttended = RandomExt.Bool();

		public bool ProductSamples { get { return _ProductSamples; } set { SetProperty(ref _ProductSamples, value); } }
		private bool _ProductSamples = RandomExt.Bool();

		public bool ReferAFriend { get { return _ReferAFriend; } set { SetProperty(ref _ReferAFriend, value); } }
		private bool _ReferAFriend = RandomExt.Bool();

		public static List<Prospect> BuildData()
		{
			List<Prospect> list = new List<Prospect>()
			{
				new Prospect() { Score = 97, Name = "John Doe", CampaignName = "3D trilogy 1" },
				new Prospect() { Score = 95, Name = "Jane Smith", CampaignName = "Trump Towers 1" },
				new Prospect() { Score = 93, Name = "Rob Reed", CampaignName = "Opus 1" },
				new Prospect() { Score = 91, Name = "Tom Brady", CampaignName = "3D trilogy 2" },
				new Prospect() { Score = 89, Name = "Matt Casell", CampaignName = "Trump Towers 2" },
				new Prospect() { Score = 87, Name = "Tedy Bruschi", CampaignName = "Opus 2" },
				new Prospect() { Score = 85, Name = "Rodney Harrison", CampaignName = "3D trilogy 3" },
				new Prospect() { Score = 83, Name = "Wes Welker", CampaignName = "Trump Towers 3" },
				new Prospect() { Score = 81, Name = "Fred Taylor", CampaignName = "Opus 3" },
				new Prospect() { Score = 79, Name = "Randy Moss", CampaignName = "3D trilogy 4" },
				new Prospect() { Score = 77, Name = "Kevin Faulk", CampaignName = "Trump Towers 4" },
				new Prospect() { Score = 75, Name = "Julian Edelman", CampaignName = "Opus 4" },
			};
			return list;
		}
	}

	public class Closing : NotifyPropertyChanges
	{
		public string Name { get { return _Name; } set { SetProperty(ref _Name, value); } }
		private string _Name;

		public int Count { get { return _Count; } set { SetProperty(ref _Count, value); } }
		private int _Count;

		public double Percentage { get { return _Percentage; } set { SetProperty(ref _Percentage, value); } }
		private double _Percentage;

		public static List<Closing> BuildData()
		{
			List<Closing> list = new List<Closing>()
			{
				new Closing() { Name = "No Closing Strategy", Count = RandomExt.Int(100) },
				new Closing() { Name = "Trump VIP", Count = RandomExt.Int(100), },
				new Closing() { Name = "Local Event", Count = RandomExt.Int(100), },
				new Closing() { Name = "Contact Me/You", Count = RandomExt.Int(100), },
				new Closing() { Name = "Trump VIP + Local Event", Count = RandomExt.Int(100), },
				new Closing() { Name = "Trump VIP + Contact Me/You", Count = RandomExt.Int(100), },
				new Closing() { Name = "Contact Me/You + Local Event", Count = RandomExt.Int(100), },
				new Closing() { Name = "Contact Me/You + Local Event + Trump VIP", Count = RandomExt.Int(100), },
			};
			return list;
		}
	}

	public class Offers : NotifyPropertyChanges
	{
		public string Name { get { return _Name; } set { SetProperty(ref _Name, value); } }
		private string _Name;

		public int Count { get { return _Count; } set { SetProperty(ref _Count, value); } }
		private int _Count;

		public static List<Offers> BuildData()
		{
			List<Offers> list = new List<Offers>()
			{
				new Offers() { Name = "Silhouette Solutions", Count = RandomExt.Int(100), },
				new Offers() { Name = "Ideal Health", Count = RandomExt.Int(100), },
				new Offers() { Name = "Quick Stiks", Count = RandomExt.Int(100), },
				new Offers() { Name = "SnazzleSnaxx", Count = RandomExt.Int(100) },
			};
			return list;
		}
	}

	public class Email : NotifyPropertyChanges
	{
		public string Name { get { return _Name; } set { SetProperty(ref _Name, value); } }
		private string _Name;

		public double Value { get { return _Value; } set { SetProperty(ref _Value, value); } }
		private double _Value;

		public static List<Email> BuildData()
		{
			List<Email> list = new List<Email>()
			{
				new Email() { Name = "Recipients on List", Value = RandomExt.Double(100) },
				new Email() { Name = "Emails Sent", Value = RandomExt.Double(100) },
				new Email() { Name = "Email Blocked or Bounced", Value = RandomExt.Double(100) },
				new Email() { Name = "Emails Opened w/o Converting:", Value = RandomExt.Double(100) },
				new Email() { Name = "Emails Opened and Converted:", Value = RandomExt.Double(100) },
			};
			return list;
		}
	}

	public class Social : NotifyPropertyChanges
	{
		public string Name { get { return _Name; } set { SetProperty(ref _Name, value); } }
		private string _Name;

		public double Value { get { return _Value; } set { SetProperty(ref _Value, value); } }
		private double _Value;

		public static List<Social> BuildData()
		{
			List<Social> list = new List<Social>()
			{
				new Social() { Name = "Visitors to Lead Capture Page", Value = RandomExt.Double(100) },
				new Social() { Name = "Visitors Submitted Ino", Value = RandomExt.Double(100) },
				new Social() { Name = "Average Time On Site", Value = RandomExt.Double(100) },
				new Social() { Name = "Average Page Views", Value = RandomExt.Double(100) },
				new Social() { Name = "Video Views", Value = RandomExt.Double(100) },
				new Social() { Name = "Bound Rate", Value = RandomExt.Double(100) },
				new Social() { Name = "New Visits", Value = RandomExt.Double(100) },
			};
			return list;
		}
	}

	public class Postcard : NotifyPropertyChanges
	{
		public string Name { get { return _Name; } set { SetProperty(ref _Name, value); } }
		private string _Name;

		public double Value { get { return _Value; } set { SetProperty(ref _Value, value); } }
		private double _Value;

		public static List<Postcard> BuildData()
		{
			List<Postcard> list = new List<Postcard>()
			{
				new Postcard() { Name = "Campaign Commission Value (CV)", Value = RandomExt.Double(100) },
				new Postcard() { Name = "Postcard Cost", Value = RandomExt.Double(100) },
				new Postcard() { Name = "List Cost", Value = RandomExt.Double(100) },
				new Postcard() { Name = "Total Campaign Cost", Value = RandomExt.Double(100) },
				new Postcard() { Name = "List Used", Value = RandomExt.Double(100) },
			};
			return list;
		}
	}

	public class Vary
	{
		public static int Int(int value)
		{
			return Double((double)value).ToInt32();
		}

		public static double Double(double value)
		{
			return RandomExt.Double(value * .90, value * 1.10);
		}
	}

#if NOTUSED
	public class CampaignTypeToImageUri : IValueConverter
	{
		public object Convert(object value, Type targetType, object parameter, CultureInfo culture)
		{
			if (value == null) // Special case
				return null;
			if (!value.GetType().IsValueType)
				throw new Exception(string.Format("'{0}' is not a value", value));
			string iconName = "Unknown";
			int id = (int)value;
			if (id == 1)
				iconName = "Icon2";
			if (id == 2)
				iconName = "Icon2";
			if (id == 3)
				iconName = "Icon2";
			iconName += ".png";
			return string.Format(culture, (parameter as string) ?? "{0}", iconName);
		}

		public object ConvertBack(object value, Type targetType, object parameter, CultureInfo culture)
		{
			throw new NotSupportedException();
		}
	}
#endif

	public class DateRangeToVisible : IValueConverter
	{
		public object Convert(object value, Type targetType, object parameter, CultureInfo culture)
		{
			if (value == null) // Special case
				return null;
			if (!(value is ComboBoxItem))
				throw new Exception(string.Format("'{0}' is not a ComboBoxItem", value));
			ComboBoxItem item = (ComboBoxItem)value;
			return (item != null && item.Tag != null && item.Tag.ToString() == "Custom" ? Visibility.Visible : Visibility.Collapsed);
		}

		public object ConvertBack(object value, Type targetType, object parameter, CultureInfo culture)
		{
			throw new NotSupportedException();
		}
	}
}
