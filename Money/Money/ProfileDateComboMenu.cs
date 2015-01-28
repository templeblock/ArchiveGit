using System;
using System.Collections.ObjectModel;
using System.ComponentModel;
using System.Windows;
using System.Windows.Controls;
using ClassLibrary;

namespace Money
{
	public class ProfileDateComboMenu : ComboBox
	{
		private ProfileDateControl _ProfileDateControl = new ProfileDateControl();
		private bool _RecreateProfileDateControl;

		public bool AllowPlanDates
		{
			get { return (_ProfileDateControl.x_PlanDatesPanel != null ? _ProfileDateControl.x_PlanDatesPanel.IsVisible() : false); }
			set { if (_ProfileDateControl.x_PlanDatesPanel != null) _ProfileDateControl.x_PlanDatesPanel.SetVisible(value); }
		}

		// The ProfileDate dependency property
		public static readonly DependencyProperty ProfileDateProperty =
			DependencyProperty.Register("ProfileDate", typeof(ProfileDate), typeof(ProfileDateComboMenu),
			new PropertyMetadata((d, e) => ((ProfileDateComboMenu)d).OnProfileDatePropertyChanged((ProfileDate)e.OldValue, (ProfileDate)e.NewValue)));

		public ProfileDate ProfileDate
		{
			get { return (ProfileDate)GetValue(ProfileDateProperty); }
			set { SetValue(ProfileDateProperty, value); }
		}

		private void OnProfileDatePropertyChanged(ProfileDate oldValue, ProfileDate newValue)
		{
			if (ProfileDate != null)
				base.Tag = ProfileDate.GetTextFromProfile();
		}

		public ProfileDateComboMenu()
		{
			base.Loaded += OnLoaded;
		}

		private void OnLoaded(object sender, RoutedEventArgs e)
		{
			base.Loaded -= OnLoaded;

			_ProfileDateControl.DateChanged += OnProfileDateChanged;

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
			if (e == null || (e.PropertyName != "CurrentDate" && e.PropertyName != "StartDate" && e.PropertyName != "EndDate" && e.PropertyName != "StartProfileDate" && e.PropertyName != "EndProfileDate"))
			{
				_RecreateProfileDateControl = true;
				if (ProfileDate != null)
					base.Tag = ProfileDate.GetTextFromProfile();
			}
		}

		private void OnProfileDateChanged(ProfileDate date)
		{
			ProfileDate = date;

			// Close the popup
			base.IsDropDownOpen = false;
		}

		protected override void OnDropDownOpened(EventArgs e)
		{
			ResetComboBoxItems();
			base.OnDropDownOpened(e); // This must be called first
			_ProfileDateControl.OnDropDownOpened(_RecreateProfileDateControl, ProfileDate);
			_RecreateProfileDateControl = false;
		}

		private void ResetComboBoxItems()
		{
			// This is a ComboBox with one item
			ObservableCollection<ProfileDateControl> itemsSource = new ObservableCollection<ProfileDateControl>();
			itemsSource.Add(_ProfileDateControl);
			base.ItemsSource = itemsSource;
			base.UpdateLayout();
			base.SelectedIndex = 0;
		}
	}
}
