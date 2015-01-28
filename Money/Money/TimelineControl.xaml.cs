using System;
using System.Collections.ObjectModel;
using System.ComponentModel;
using System.Windows;
using System.Windows.Controls;
using ClassLibrary;
using JulianDay = System.UInt32;

namespace Money
{
	public partial class TimelineControl : UserControl, INotifyPropertyChanged
	{
		public event Action<JulianDay> DateChanged;

		public JulianDay Date { get { return _Date; } set { _Notify.SetProperty(ref _Date, BoundDate(value), System.Reflection.MethodBase.GetCurrentMethod().Name, OnDateChange); } }
		private JulianDay _Date;

		[DependsOn("Date")]
		public DateTime DateTime { get { return Date.ToDateTime(); } }

		[DependsOn("Date")]
		public string DateTimeText { get { return Date.ToDateTime().ToString("d MMM yyyy"); } }

		private ObservableCollection<string> _PersonList = new ObservableCollection<string>();
		private ViewModelImpl<TimelineControl> _Notify = new ViewModelImpl<TimelineControl>();

		public event PropertyChangedEventHandler PropertyChanged;

		public TimelineControl()
		{
			InitializeComponent();
			base.Loaded += OnLoaded;

			_Notify.Initialize(delegate(object sender, PropertyChangedEventArgs e)
			{
				if (PropertyChanged != null)
					PropertyChanged(sender, e);
			});

			x_PositionSlider.Maximum = 0;
			x_PositionSlider.Minimum = 0;
			x_PositionSlider.Value = 0;
			x_PositionSlider.SmallChange = 30;
			x_PositionSlider.LargeChange = 365;
			x_PositionSlider.ValueChangedComplete += SliderValueChanged;
			x_PositionSlider.ValueChangedInDrag += SliderValueChanged;
		}

		private void OnLoaded(object sender, RoutedEventArgs e)
		{
			base.Loaded -= OnLoaded;

			x_PersonList.ItemsSource = _PersonList;

			// Start listening to the profile holder
			App.Model.ProfileHolder.ProfileCreated += OnProfileCreated;
			OnProfileCreated(null, null);
		}

		private void OnProfileCreated(object sender, PropertyChangedEventArgs e)
		{
			// Start listening to the new profile
			App.Model.ProfileHolder.Profile.ProfileChanged += OnProfileChanged;
			OnProfileChanged(null, null);
		}

		private void OnProfileChanged(object sender, PropertyChangedEventArgs e)
		{
			if (e == null || e.PropertyName == "CurrentDate" || e.PropertyName == "StartDate" || e.PropertyName == "EndDate")
			{
				// May also change the slider value if it is out of bounds
				JulianDay currentDate = App.Model.ProfileHolder.Profile.CurrentDate;
				JulianDay startDate = App.Model.ProfileHolder.Profile.StartProfileDate.JulianDay;
				JulianDay endDate = App.Model.ProfileHolder.Profile.EndProfileDate.JulianDay;
				if (startDate == 0 && endDate == 0)
					startDate = endDate = DateTime.Now.ToJulian();

				x_PositionSlider.Maximum = endDate;
				x_PositionSlider.Minimum = startDate;
				Date = currentDate;

				x_YearLabelStart.Text = startDate.Year().ToString();
				x_YearLabelEnd.Text = endDate.Year().ToString();
			}

			UpdateProfileAges(_Date);
		}

		private JulianDay BoundDate(JulianDay date)
		{
			if (date < x_PositionSlider.Minimum)
				date = (JulianDay)x_PositionSlider.Minimum;
			if (date > x_PositionSlider.Maximum)
				date = (JulianDay)x_PositionSlider.Maximum;
			return date;
		}

		private void OnDateChange(JulianDay date)
		{
			x_PositionSlider.Value = date;

			// Set the profile's CurrentDate since we maintain it
			App.Model.ProfileHolder.Profile.CurrentDate = date;

			UpdateProfileAges(date);
			if (DateChanged != null)
				DateChanged(date);
		}

		private void UpdateProfileAges(JulianDay date)
		{
			int index = 0;

			string profileText = ProfileCode.MeBirth.ToRelativeProfileText(_Date);
			if (profileText != null) SetPersonText(index++, profileText);

			profileText = ProfileCode.SpouseBirth.ToRelativeProfileText(_Date);
			if (profileText != null) SetPersonText(index++, profileText);

			int dependant = 0;
			while ((profileText = ProfileCode.DependentBirth(dependant++).ToRelativeProfileText(_Date)) != null)
			{
				SetPersonText(index++, profileText);
				if (index >= 5)
					break;
			}

			while (index < _PersonList.Count)
				_PersonList[index++] = null;

			if (App.Model.ProfileHolder.Profile.IsIncomplete)
				x_DateLabel.Text = "You have an incomplete profile.  Complete it in the Profile tab.";
			else
				x_DateLabel.Text = _Date.ToDateTime().ToLongDateString();
		}

		private void SetPersonText(int index, string profileText)
		{
			int count = index - _PersonList.Count + 1;
			while (--count >= 0)
				_PersonList.Add(null);
			_PersonList[index] = profileText;
		}

		private void SliderValueChanged(double value)
		{
			Date = (JulianDay)value;
		}

		private void OnYearDownClick(object sender, RoutedEventArgs e)
		{
			Date = Date.IncrementYear(-1);
		}

		private void OnMonthDownClick(object sender, RoutedEventArgs e)
		{
			Date = Date.IncrementMonth(-1);
		}

		private void OnWeekDownClick(object sender, RoutedEventArgs e)
		{
			Date = Date.IncrementDay(-7);
		}

		private void OnDayDownClick(object sender, RoutedEventArgs e)
		{
			Date = Date.IncrementDay(-1);
		}

		private void OnDayUpClick(object sender, RoutedEventArgs e)
		{
			Date = Date.IncrementDay(+1);
		}

		private void OnWeekUpClick(object sender, RoutedEventArgs e)
		{
			Date = Date.IncrementDay(+7);
		}

		private void OnMonthUpClick(object sender, RoutedEventArgs e)
		{
			Date = Date.IncrementMonth(+1);
		}

		private void OnYearUpClick(object sender, RoutedEventArgs e)
		{
			Date = Date.IncrementYear(+1);
		}

		private void OnTodayClick(object sender, RoutedEventArgs e)
		{
			Date = DateTime.Now.ToJulian();
		}
	}
}
