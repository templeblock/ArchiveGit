using System.Windows;
using System.Windows.Controls;
using System.Windows.Navigation;
using ClassLibrary;

namespace Money
{
	public partial class ProfileInfoPage : Page
	{
		public ProfileInfoPage()
		{
			InitializeComponent();
			HtmlTextBlock content = base.Content.ChildOfType<HtmlTextBlock>();
			content.HyperlinkClick += App.MainPage.OnHyperlinkClick;
			base.Loaded += OnLoaded;
		}

		private void OnLoaded(object sender, RoutedEventArgs e)
		{
			base.Loaded -= OnLoaded;
		}

		private void OnUpdateLifeExpectancyClick(object sender, RoutedEventArgs e)
		{
			if (!(sender is FrameworkElement))
				return;
			Person person = (sender as FrameworkElement).Tag as Person;
			if (person != null)
				person.UpdateLifeExpectancy();
		}

		private void OnAddDependentClick(object sender, RoutedEventArgs e)
		{
			App.Model.ProfileHolder.Profile.Dependents.Add(new Person());
		}

		private void OnRemoveDependentClick(object sender, RoutedEventArgs e)
		{
			int count = App.Model.ProfileHolder.Profile.Dependents.Count;
			if (count > 0)
				App.Model.ProfileHolder.Profile.Dependents.RemoveAt(count - 1);
		}
	}
}
