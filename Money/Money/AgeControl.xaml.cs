using System.Collections.Generic;
using System.Windows;
using System.Windows.Controls;
using ClassLibrary;

namespace Money
{
	public partial class AgeControl : UserControl
	{
		public event RoutedEventHandler Click;

		// The Age dependency property
		public static readonly DependencyProperty AgeProperty =
			DependencyProperty.Register("Age", typeof(int), typeof(AgeControl),
			null);

		public int Age
		{
			get { return (int)GetValue(AgeProperty); }
			set { SetValue(AgeProperty, value); SelectProperTab(); }
		}

		public AgeControl()
		{
			InitializeComponent();
			base.Loaded += OnLoaded;
		}

		private void OnLoaded(object sender, RoutedEventArgs e)
		{
			base.Loaded -= OnLoaded;
			SelectProperTab();
		}

		private void SelectProperTab()
		{
			int age = Age;
			IEnumerable<TabItem> tabs = this.ChildListOfType<TabItem>();
			foreach (TabItem tab in tabs)
			{
				if (!DoesTabContainAge(tab, age))
					continue;

				(tab.Parent as TabControl).SelectedItem = tab;
				DependencyObject content = tab.Content as DependencyObject;
				if (content != null)
				{
					IEnumerable<Button> buttons = content.ChildListOfType<Button>();
					foreach (Button button in buttons)
					{
						if (age == GetButtonAge(button))
						{
							button.FocusEx();
							break;
						}
					}
				}
				break;
			}
		}

		private bool DoesTabContainAge(TabItem tab, int age)
		{
			if (tab == null)
				return false;
			string content = tab.Tag as string;
			if (content == null)
				return false;
			if (content == null)
				return false;

			return (age < content.ToInt());
		}

		private int GetButtonAge(Button button)
		{
			if (button == null)
				return -1;
			string content = button.Content as string;
			if (content == null)
				return -1;

			return content.ToInt();
		}

		private void OnAgeClick(object sender, RoutedEventArgs e)
		{
			Button button = sender as Button;
			int age = GetButtonAge(button);
			if (age < 0)
				return;

			Age = age;
			if (Click != null)
				Click(this, e);
		}
	}
}
