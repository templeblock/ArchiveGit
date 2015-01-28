﻿using System.Windows;
using System.Windows.Controls;
using System.Windows.Navigation;
using ClassLibrary;

namespace Money
{
	public partial class ProfileAccountsPage : Page
	{
		public ProfileAccountsPage()
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
	}
}
