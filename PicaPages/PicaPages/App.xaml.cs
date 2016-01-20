using System;
using System.Windows;
using ClassLibrary;
using System.Windows.Controls;

namespace PicaPages
{
	public partial class App : Application
	{
		static public Page PageProp { get { return ApplicationEx.RootUserControl as Page; } }

		public App()
		{
			base.Startup += Application_Startup;
			base.Exit += Application_Exit;
			base.UnhandledException += Application_UnhandledException;
			InitializeComponent();
		}

		private void Application_Startup(object sender, StartupEventArgs e)
		{
			base.RootVisual = new Page();
		}

		private void Application_Exit(object sender, EventArgs e)
		{
		}

		private void Application_UnhandledException(object sender, ApplicationUnhandledExceptionEventArgs e)
		{
			Exception ex = e.ExceptionObject;
			ex.Assert();
			e.Handled = true;
		}
	}
}
