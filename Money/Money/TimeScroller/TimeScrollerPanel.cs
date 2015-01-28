using System.ComponentModel;
using System.Windows;

namespace Money
{
#if NOTUSED //j
	public abstract class TimeScrollerPanel : ViewPanel
	{
		public TimeScrollerPanel()
		{
			base.Loaded += OnLoaded;
			base.VisibilityChange += OnVisibilityChange;
		}

		private void OnLoaded(object sender, RoutedEventArgs e)
		{
			base.Loaded -= OnLoaded;

			// Start listening to the profile holder
			App.Model.ProfileHolder.ProfileCreated += OnProfileCreated;
			OnProfileCreated(null, null);

			// Start listening to the document holder
			App.Model.DocumentHolder.DocumentCreated += OnDocumentCreated;
			OnDocumentCreated(null, null);
		}

		private void OnVisibilityChange(bool visible)
		{
			if (base.IsVisible)
				AttachBinding();
			else
				DetachBinding();
		}

		private void OnProfileCreated(object sender, PropertyChangedEventArgs e)
		{
			// Start listening to the new profile
			App.Model.ProfileHolder.Profile.ProfileChanged += OnProfileChanged;
			OnProfileChanged(null, null);
		}

		private void OnProfileChanged(object sender, PropertyChangedEventArgs e)
		{
			if (e == null || e.PropertyName == "StartDate" || e.PropertyName == "EndDate")
			{
				App.MainPage.x_TimeScroller.StartDate = (int)App.Model.ProfileHolder.Profile.StartProfileDate.JulianDay();
				App.MainPage.x_TimeScroller.EndDate = (int)App.Model.ProfileHolder.Profile.EndProfileDate.JulianDay();
			}
		}

		private void OnDocumentCreated(object sender, PropertyChangedEventArgs e)
		{
			if (base.IsVisible)
				AttachBinding();
		}

		private void DetachBinding()
		{
			App.MainPage.x_TimeScroller.ItemsSource = null;
		}

		private void AttachBinding()
		{
			App.MainPage.x_TimeScroller.ItemsSource = App.Model.DocumentHolder.Document.Incomes;
		}
	}
#endif
}
