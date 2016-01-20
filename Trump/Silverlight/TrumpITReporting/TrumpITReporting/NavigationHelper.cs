using System;
using System.Collections.Generic;
using System.Windows;
using System.Windows.Browser;
using System.Windows.Controls;
using System.Windows.Navigation;
using ClassLibrary;

namespace TrumpITReporting
{
	public class ContentProps
	{
		// The Main attached property
		public static readonly DependencyProperty MainProperty =
			DependencyProperty.RegisterAttached("Main", typeof(string), typeof(ContentProps),
				new PropertyMetadata(null, null));

		public static string GetMain(FrameworkElement element)
		{
			return (string)element.GetValue(MainProperty);
		}

		public static void SetMain(FrameworkElement element, string value)
		{
			element.SetValue(MainProperty, value);
		}

		// The Alternate attached property
		public static readonly DependencyProperty AlternateProperty =
			DependencyProperty.RegisterAttached("Alternate", typeof(string), typeof(ContentProps),
				new PropertyMetadata(null, null));

		public static string GetAlternate(FrameworkElement element)
		{
			return (string)element.GetValue(AlternateProperty);
		}

		public static void SetAlternate(FrameworkElement element, string value)
		{
			element.SetValue(AlternateProperty, value);
		}

		// The IgnoreAlternate attached property
		public static readonly DependencyProperty IgnoreAlternateProperty =
			DependencyProperty.RegisterAttached("IgnoreAlternate", typeof(bool), typeof(ContentProps),
				new PropertyMetadata(false, OnIgnoreAlternatePropertyChanged));

		public static bool GetIgnoreAlternate(FrameworkElement element)
		{
			return (bool)element.GetValue(IgnoreAlternateProperty);
		}

		public static void SetIgnoreAlternate(FrameworkElement element, bool value)
		{
			element.SetValue(IgnoreAlternateProperty, value);
		}

		private static void OnIgnoreAlternatePropertyChanged(DependencyObject sender, DependencyPropertyChangedEventArgs e)
		{
			TabItem tabItem = sender as TabItem;
			App.MainPage.NavigationHelper.NavigateViaElement(tabItem);
		}
	}

	public class NavigationHelper
	{
		private Frame m_Frame;
		private FrameworkElement m_NavigationSelector;
		private bool m_IgnoreNavigate = true;
		private bool m_QueryStringProcessed = false;

		internal void Init(Frame navigationFrame, FrameworkElement navigationSelector)
		{
			m_Frame = navigationFrame;
//j			m_Frame.FragmentNavigation += OnFragmentNavigation;
			m_Frame.Navigated += OnNavigated;
			m_Frame.NavigationFailed += OnNavigationFailed;
			m_NavigationSelector = navigationSelector;
			m_IgnoreNavigate = false;
			m_QueryStringProcessed = false;
		}

		internal void NavigateViaElement(FrameworkElement element)
		{
			if (m_IgnoreNavigate || element == null)
				return;
			if ((element is TabItem) && !(element as TabItem).IsSelected)
				return;
			if ((element is RadioButton) && (element as RadioButton).IsChecked != true)
				return;

			string contentUri = element.Tag.ToString();
			string alternateUri = ContentProps.GetAlternate(element);
			if (alternateUri != null && !ContentProps.GetIgnoreAlternate(element))
				contentUri = alternateUri;

			Navigate(contentUri);
		}

		internal void Navigate(string contentUri)
		{
			try
			{
				int index = contentUri.IndexOf('/');
				if (index > 0)
					contentUri = contentUri.Substring(index);

				// Navigate to the page, which will make the new content visible, and trigger OnNavigated to set the tabs
				if (m_Frame != null)
					m_Frame.Navigate(new Uri(contentUri, UriKind.Relative));
			}
			catch (Exception ex)
			{
				ex.Message.Trace();
				ex.Alert();
			}
		}

//j		//private void OnFragmentNavigation(object sender, FragmentNavigationEventArgs e)
		//{
		//}

		private void OnNavigationFailed(object sender, NavigationFailedEventArgs e)
		{
			e.Handled = true;
			MessageBoxEx.ShowError("Page Error", e.Exception.Message + "\n\n" + e.Uri.ToString(), null);
		}

		private void OnNavigated(object sender, NavigationEventArgs e)
		{
			// The command is of the form /mainCommand/subCommand
			string command = null;

			// See if there is an external querystring command
			if (!m_QueryStringProcessed)
			{
				m_QueryStringProcessed = true; // the external query string is only used once
				IDictionary<string, string> queryString = HtmlPage.Document.QueryString;
				if (queryString != null && queryString.ContainsKey("cmd") && command == null)
					command = queryString["cmd"].Trim();
			}

			// See if there is an internal querystring command (from the UriMapper)
			if (command == null)
			{
				Page page = e.Content as Page;
				if (page != null && page.NavigationContext.QueryString != null && page.NavigationContext.QueryString.ContainsKey("cmd"))
					command = page.NavigationContext.QueryString["cmd"].Trim();
			}

			// The normal command case
			if (command == null)
				command = e.Uri.ToString();

			OnNavigatedCommand(command);
		}

		private void OnNavigatedCommand(string command)
		{
			// Split the command into command words; the last 2 map to the main and sub commands
			string[] commands = command.Split(new char[] { '/' }, StringSplitOptions.RemoveEmptyEntries);
			int n = commands.Length;
			int iM = (n >= 2 ? n - 2 : n - 1);
			int iS = (n >= 2 ? n - 1 : -1);
			string mainCommand = (iM >= 0 ? commands[iM].Trim() : "");
			string subCommand  = (iS >= 0 ? commands[iS].Trim() : "");

			UpdateUI(mainCommand, subCommand);

//t			Analytics.TrackEvent(Analytics.Category.AppEvents, Analytics.Action.Navigate, command, 1);
		}

		private void UpdateUI(string mainCommand, string subCommand)
		{
			if (m_NavigationSelector == null)
				return;

			// Find the element by the command name tags
			string mainContentTag = string.Format("/{0}", mainCommand);
			string subContentTag = string.Format("/{0}/{1}", mainCommand, subCommand);

			FrameworkElement mainNavSelector = m_NavigationSelector;
			FrameworkElement subNavSelector = mainNavSelector.ChildOfTag("SubNavigationContainer");

			// Disable navigation while we set the selected item to avoid recursive OnNavigated calls
			bool save = m_IgnoreNavigate;
			m_IgnoreNavigate = true;

			// See if the element needs to be selected; required in the back/next/bookmarks cases
			// Be sure to select the sub element before the main element to avoid flashing
			if (subNavSelector is TabControl)
				UpdateUITabControl(subNavSelector as TabControl, subContentTag);
			else
			if (subNavSelector is RadioButton)
				UpdateUIRadioButton(subNavSelector as RadioButton, subContentTag);

			if (mainNavSelector is TabControl)
				UpdateUITabControl(mainNavSelector as TabControl, mainContentTag);
			else
			if (mainNavSelector is RadioButton)
				UpdateUIRadioButton(mainNavSelector as RadioButton, mainContentTag);

			m_IgnoreNavigate = save;
		}

		private void UpdateUITabControl(TabControl selector, string contentTag)
		{
			FrameworkElement taggedChild = selector.ChildOfTag(contentTag);
			if (taggedChild != null && selector.SelectedItem != taggedChild)
				selector.SelectedItem = taggedChild;
		}

		private void UpdateUIRadioButton(RadioButton selector, string contentTag)
		{
			RadioButton taggedChild = selector.FindByTag(contentTag);
			if (taggedChild != null && taggedChild.IsChecked != true)
				taggedChild.IsChecked = true;
		}
	}
}
