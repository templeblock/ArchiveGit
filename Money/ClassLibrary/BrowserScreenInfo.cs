using System;
using System.Windows.Browser;

namespace ClassLibrary
{
	// The properties exposed off of the static class are:
	// ClientWidth, ClientHeight					Current window’s inner size  
	// ScrollLeft, ScrollTop						Window’s scrolled portion relative to the page 
	// ScreenWidth, ScreenHeight					Screen resolution 
	// AvailableScreenWidth, AvailableScreenHeight	Real estate of the screen excluding the task bar or dock 
	// ScreenPositionLeft, ScreenPositionTop		Location, in pixels, of the window’s origin on the screen 

	/// <summary> 
	/// Provides screen information about the browser 
	/// </summary> 
	/// <remarks>A simple proxy to JavaScript window and screen variables, abstracts away common web browser differences</remarks> 
	internal static class BrowserScreenInformation
	{
		/// <summary> 
		/// Provides quick access to the window.screen ScriptObject 
		/// </summary> 
		private static ScriptObject Screen
		{
			get
			{
				ScriptObject screen = (ScriptObject)HtmlPage.Window.GetProperty("screen");
				if (screen == null)
					throw new InvalidOperationException();

				return screen;
			}
		}

		/// <summary> 
		/// Gets the window object's client width 
		/// </summary> 
		internal static double ClientWidth
		{
			get
			{
				return !IsInternetExplorer ? (double)HtmlPage.Window.GetProperty("innerWidth")
					: (double)HtmlPage.Document.Body.GetProperty("clientWidth");
			}
		}

		/// <summary> 
		/// Gets the window object's client height 
		/// </summary> 
		internal static double ClientHeight
		{
			get
			{
				return !IsInternetExplorer ? (double)HtmlPage.Window.GetProperty("innerHeight")
					: (double)HtmlPage.Document.Body.GetProperty("clientHeight");
			}
		}

		/// <summary> 
		/// Gets the current horizontal scrolling offset 
		/// </summary> 
		internal static double ScrollLeft
		{
			get
			{
				return !IsInternetExplorer ? (double)HtmlPage.Window.GetProperty("pageXOffset")
					: (double)HtmlPage.Document.Body.GetProperty("scrollLeft");
			}
		}

		/// <summary> 
		/// Gets the current vertical scrolling offset 
		/// </summary> 
		internal static double ScrollTop
		{
			get
			{
				return !IsInternetExplorer ? (double)HtmlPage.Window.GetProperty("pageYOffset")
					: (double)HtmlPage.Document.Body.GetProperty("scrollHeight");
			}
		}

		/// <summary> 
		/// Gets the width of the entire display 
		/// </summary> 
		internal static double ScreenWidth
		{
			get
			{
				return (double)Screen.GetProperty("width");
			}
		}

		/// <summary> 
		/// Gets the height of the entire display 
		/// </summary> 
		internal static double ScreenHeight
		{
			get
			{
				return (double)Screen.GetProperty("height");
			}
		}

		/// <summary> 
		/// Gets the width of the available screen real estate, excluding the dock or task bar 
		/// </summary> 
		internal static double AvailableScreenWidth
		{
			get
			{
				return (double)Screen.GetProperty("availWidth");
			}
		}

		/// <summary> 
		/// Gets the height of the available screen real estate, excluding the dock or task bar 
		/// </summary> 
		internal static double AvailableScreenHeight
		{
			get
			{
				return (double)Screen.GetProperty("availHeight");
			}
		}

		/// <summary> 
		/// Gets the absolute left pixel position of the window in display coordinates 
		/// </summary> 
		internal static double ScreenPositionLeft
		{
			get
			{
				return !IsInternetExplorer ? (double)HtmlPage.Window.GetProperty("screenX")
					: (double)HtmlPage.Window.GetProperty("screenLeft");
			}
		}

		/// <summary> 
		/// Gets the absolute top pixel position of the window in display coordinates 
		/// </summary> 
		internal static double ScreenPositionTop
		{
			get
			{
				return !IsInternetExplorer ? (double)HtmlPage.Window.GetProperty("screenY")
					: (double)HtmlPage.Window.GetProperty("screenTop");
			}
		}

		internal static bool IsInternetExplorer
		{
			get
			{
				return HtmlPage.BrowserInformation.Name.Contains("Internet Explorer");
			}
		}

		internal static bool IsFirefox
		{
			get
			{
				return HtmlPage.BrowserInformation.UserAgent.Contains("Firefox");
			}
		}

		internal static bool IsChrome
		{
			get
			{
				return HtmlPage.BrowserInformation.UserAgent.Contains("Chrome");
			}
		}
	}
}
