using System;
using System.Collections.Generic;
using System.Windows.Browser;
using System.Windows.Controls;
using System.Windows.Navigation;
using ClassLibrary;

namespace Money
{
	internal static class FrameExt
	{
		// Extension for Frame
		internal static void Navigate(this Frame frame, string contentUri)
		{
			try
			{
				int index = contentUri.IndexOf('/');
				if (index > 0)
					contentUri = contentUri.Substring(index);

				// Navigate to the page, which will make the new content visible, and trigger OnNavigated
				if (frame != null)
					frame.Navigate(new Uri(contentUri, UriKind.Relative));
			}
			catch (Exception ex)
			{
				ex.Message.Trace();
				ex.Alert();
			}
		}
	}

	public class NavigationHelper
	{
		private Frame m_Frame;
		private TabControl m_MainTabControl;
		private TabItem m_ContentTabItem;
		private bool m_IgnoreNavigate = true;
		private bool m_QueryStringProcessed = false;

		internal void Init(Frame navigationFrame, TabControl mainTabControl)
		{
			m_Frame = navigationFrame;
			m_Frame.Navigated += OnNavigated;
			m_Frame.NavigationFailed += OnNavigationFailed;
			m_MainTabControl = mainTabControl;
			m_IgnoreNavigate = false;
			m_QueryStringProcessed = false;
		}

		internal void NavigateTabControl(TabControl tabControl)
		{
			TabItem tabItem = (tabControl != null ? tabControl.SelectedItem as TabItem : null);
			if (tabItem != null)
				NavigateTabItem(tabItem);
		}

		internal void NavigateTabItem(TabItem tabItem)
		{
			if (m_IgnoreNavigate || tabItem == null || !tabItem.IsSelected)
				return;

			string contentUri = tabItem.Tag.ToString();
			m_Frame.Navigate(contentUri);
		}

		internal void RedisplayTabItem(TabItem tabItem)
		{
			if (tabItem == m_ContentTabItem)
				NavigateTabItem(tabItem);
		}

		private void OnNavigationFailed(object sender, NavigationFailedEventArgs e)
		{
			e.Handled = true;
			MessageBoxEx.ShowError("Page Error", e.Exception.Message + "\n\n" + e.Uri.ToString(), null);
		}

		private void OnNavigated(object sender, NavigationEventArgs e)
		{
			// See if there is an external querystring command
			if (!m_QueryStringProcessed)
			{
				m_QueryStringProcessed = true; // the external query string is only used once
				IDictionary<string, string> queryString = HtmlPage.Document.QueryString;
				if (queryString.ContainsKey("cmd"))
				{
					string externalCommand = queryString["cmd"].Trim();
					m_Frame.Navigate(externalCommand);
					return; //  the Navigate() above will cause another OnNavigated()
				}
				else
				if (queryString.Count > 0 && queryString.Values.ToCollection()[0] == "")
				{
					string externalCommand = queryString.Keys.ToCollection()[0].Trim();
					m_Frame.Navigate(externalCommand);
					return; //  the Navigate() above will cause another OnNavigated()
				}
			}

			// See if there is an internal querystring command (from the UriMapper)
			Page page = e.Content as Page;
			if (page != null && page.NavigationContext.QueryString.ContainsKey("cmd"))
			{
				string internalCommand = page.NavigationContext.QueryString["cmd"].Trim();
				m_Frame.Navigate(internalCommand);
				return; //  the Navigate() above will cause another OnNavigated()
			}

			// The normal command case of the form /mainCommand/subCommand
			string command = e.Uri.ToString();
			int index = command.IndexOf('?');
			if (index >= 0)
				command = command.Substring(0, index);
			UpdateUI(command);
		}

		private void UpdateUI(string command)
		{
			// Split the command into command words; the last 2 map to the main and sub commands
			string[] commands = command.Split(new char[] { '/' }, StringSplitOptions.RemoveEmptyEntries);
			int n = commands.Length;
			int iM = (n >= 2 ? n - 2 : n - 1);
			int iS = (n >= 2 ? n - 1 : -1);
			string mainCommand = (iM >= 0 ? commands[iM].Trim() : "");
			string subCommand  = (iS >= 0 ? commands[iS].Trim() : "");

			if (mainCommand == "" || mainCommand.EqualsIgnoreCase("Reset") || mainCommand.EqualsIgnoreCase("SignIn"))
			{
				if (mainCommand.EqualsIgnoreCase("Reset")) // A link from a sign in recovery email
					new ResetPasswordDialog(subCommand);
				else
				if (mainCommand.EqualsIgnoreCase("SignIn")) // A link from a sign in recovery email
					App.MainPage.OnSigninClick(null, null);
				mainCommand = "Home";
				subCommand = "Welcome";
			}

			UpdateUI(mainCommand, subCommand);
		}

		private void UpdateUI(string mainCommand, string subCommand)
		{
			// Find the tabitems by the command names, but default to the selected tabitems
			string mainContentTag = string.Format("/{0}", mainCommand);
			string subContentTag = string.Format("/{0}/{1}", mainCommand, subCommand);
			TabItem mainTabItem = (m_MainTabControl != null ? m_MainTabControl.FindByTag(mainContentTag) : null);
			if (mainTabItem == null)
				mainTabItem = (m_MainTabControl != null ? m_MainTabControl.SelectedItem as TabItem : null);
			TabControl subTabControl = (mainTabItem != null ? mainTabItem.Content as TabControl : null);
			TabItem subTabItem = (subTabControl != null ? subTabControl.FindByTag(subContentTag) : null);
			if (subTabItem == null)
				subTabItem = (subTabControl != null ? subTabControl.SelectedItem as TabItem : null);

			// Check to see if the tabs need to be selected; required in the back/next/bookmarks cases
			// Be sure to select the sub tabitem before the main tabitem to avoid flashing
			// Also disable navigation while we set the selected items to avoid recursive OnNavigated calls
			bool save = m_IgnoreNavigate;
			m_IgnoreNavigate = true;
			if (subTabControl != null && subTabControl.SelectedItem != subTabItem)
				subTabControl.SelectedItem = subTabItem;
			if (m_MainTabControl != null && m_MainTabControl.SelectedItem != mainTabItem)
				m_MainTabControl.SelectedItem = mainTabItem;
			m_IgnoreNavigate = save;

			m_ContentTabItem = subTabItem;
		}
	}
}
