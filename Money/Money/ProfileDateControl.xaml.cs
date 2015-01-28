using System;
using System.Collections.ObjectModel;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Input;
using ClassLibrary;
using JulianDay = System.UInt32;

namespace Money
{
	public partial class ProfileDateControl : UserControl
	{
		private ProfileCode _ListItemProfileCode = 0;
		private ProfileDate _ProfileDate;

		public event Action<ProfileDate> DateChanged;

		public ProfileDateControl()
		{
			InitializeComponent();
			base.Loaded += OnLoaded;
		}

		private void OnLoaded(object sender, RoutedEventArgs e)
		{
			base.Loaded -= OnLoaded;

			base.MouseLeftButtonDown += IgnoreMouseEvents;
			base.MouseLeftButtonUp += IgnoreMouseEvents;
		}

		private void IgnoreMouseEvents(object sender, MouseButtonEventArgs e)
		{
			// Gobble up mouse up/down events, so the owning combobox does not close unexpectedly
			e.Handled = true;
		}

		public void OnDropDownOpened(bool recreate, ProfileDate profileDate)
		{
			_ProfileDate = profileDate;

			// If necessary, recreate the listbox names, and bind them to the listbox
			if (x_AgeRanges.Items.Count <= 0 || recreate)
			{
				x_AgeRanges.ItemsSource = CreateAgeRangeItems();
				x_AgeRanges.UpdateLayout();
				if (x_AgeRanges.Items.Count <= 0)
					return;
			}

			// Make the ListBox selection
			int index = 0;
			int age = -1;
			Control focusControl = null;
			Profile profile = App.Model.ProfileHolder.Profile;
			ProfileCode profileCode = 0;
			if (_ProfileDate != null)
				profileCode = _ProfileDate.ProfileCode;
			if (profileCode == 0)
				focusControl = x_Calendar;
			else
			{
				if (profileCode == ProfileCode.StartOfPlan || profileCode == ProfileCode.EndOfPlan)
				{
					index = 0;
					focusControl = (profileCode == ProfileCode.StartOfPlan ? x_PlanStartDate : x_PlanEndDate);
				}
				else
				if (profileCode >= ProfileCode.MeBirth && profileCode <= ProfileCode.MeDeath)
				{
					index = 1;
					age = profileCode - ProfileCode.MeBirth;
				}
				else
				if (profile != null && profile.IncludeSpouse && profileCode >= ProfileCode.SpouseBirth && profileCode <= ProfileCode.SpouseDeath)
				{
					index = 2;
					age = profileCode - ProfileCode.SpouseBirth;
				}
				else
				if (profile != null && profile.IncludeDependents && profileCode >= ProfileCode.DependentMin && profileCode <= ProfileCode.DependentMax)
				{
					int dependent = (profileCode - ProfileCode.DependentMin) / ProfileCode.Range;
					index = 2 + (profile.IncludeSpouse ? 1 : 0) + dependent;
					age = profileCode - ProfileCode.DependentBirth(dependent);
				}
			}

			x_AgeRanges.SelectedIndex = Math.Min(index, x_AgeRanges.Items.Count - 1);
			if (age == 0 || age == ProfileCode.Death)
			{
				focusControl = (age == 0 ? x_BirthDate : x_DeathDate);
				age = -1;
			}

			if (focusControl != null)
				focusControl.FocusEx();
			else
			if (age >= 0)
				x_AgeControl.Age = age;
		}

		private ObservableCollection<ListBoxItem> CreateAgeRangeItems()
		{
			ObservableCollection<ListBoxItem> items = new ObservableCollection<ListBoxItem>();
			Profile profile = App.Model.ProfileHolder.Profile;
			if (profile == null)
				return items;

			ListBoxItem item = null;
			items.Clear();

			item = new ListBoxItem() { Content = "Specific", Tag = null };
			items.Add(item);

			string firstName = (!profile.Me.FirstName.IsEmpty() ? profile.Me.FirstName : "Me");
			item = new ListBoxItem() { Content = firstName, Tag = ProfileCode.MeBirth };
			items.Add(item);

			if (profile.IncludeSpouse)
			{
				firstName = (!profile.Spouse.FirstName.IsEmpty() ? profile.Spouse.FirstName : "Spouse");
				item = new ListBoxItem() { Content = firstName, Tag = ProfileCode.SpouseBirth };
				items.Add(item);
			}

			if (profile.IncludeDependents)
			{
				int dependent = 0;
				foreach (Person person in profile.Dependents)
				{
					firstName = (!person.FirstName.IsEmpty() ? person.FirstName : string.Format("Dep{0}", dependent + 1));
					item = new ListBoxItem() { Content = firstName, Tag = ProfileCode.DependentBirth(dependent) };
					items.Add(item);
					dependent++;
				}
			}

			return items;
		}

		private void OnListSelectionChanged(object sender, SelectionChangedEventArgs e)
		{
			ListBox listBox = sender as ListBox;
			bool showAgeControls = (listBox.SelectedIndex > 0);
			x_PlanControls.SetVisible(!showAgeControls);
			x_AgeControls.SetVisible(showAgeControls);

			ListBoxItem listBoxItem = listBox.SelectedItem as ListBoxItem;
			if (listBoxItem != null && (listBoxItem.Tag is ProfileCode))
				_ListItemProfileCode = listBoxItem.Tag as ProfileCode;
			else
				_ListItemProfileCode = 0;

			if (!showAgeControls && _ProfileDate != null)
			{
				x_Calendar.DisplayDate = _ProfileDate.JulianDay.ToDateTime();
				x_Calendar.SelectedDate = x_Calendar.DisplayDate;
			}
		}

		private void OnStartPlanClick(object sender, RoutedEventArgs e)
		{
			SetProfileDate(ProfileCode.StartOfPlan.ToProfileDate());
		}

		private void OnEndPlanClick(object sender, RoutedEventArgs e)
		{
			SetProfileDate(ProfileCode.EndOfPlan.ToProfileDate());
		}

		private void OnBirthClick(object sender, RoutedEventArgs e)
		{
			if (_ListItemProfileCode == 0)
				return;

			SetProfileDate(_ListItemProfileCode.ToProfileDate());
		}

		private void OnDeathClick(object sender, RoutedEventArgs e)
		{
			if (_ListItemProfileCode == 0)
				return;

			ProfileCode profileCode = _ListItemProfileCode + ProfileCode.Death;
			SetProfileDate(profileCode.ToProfileDate());
		}

		private void OnAgeClick(object sender, RoutedEventArgs e)
		{
			if (_ListItemProfileCode == 0)
				return;

			AgeControl ageControl = sender as AgeControl;
			ProfileCode profileCode = _ListItemProfileCode + ageControl.Age;
			SetProfileDate(profileCode.ToProfileDate());
		}

		private void OnCalendarDateChanged(object sender, SelectionChangedEventArgs e)
		{
			Calendar calendar = sender as Calendar;
			if (calendar.SelectedDate == null)
				return;

			// If this is not the result of a user click, get out
			if (!calendar.HasFocus())
				return;

			JulianDay date = ((DateTime)calendar.SelectedDate).ToJulian();
			SetProfileDate(new ProfileDate() { JulianDay = date });
		}

		private void SetProfileDate(ProfileDate profileDate)
		{
			_ProfileDate = profileDate;
			if (DateChanged != null)
				DateChanged(_ProfileDate);
		}
	}
}
