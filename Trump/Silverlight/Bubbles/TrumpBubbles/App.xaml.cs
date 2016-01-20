using System;
using System.Windows;
using System.Windows.Browser;
using ClassLibrary;
using System.Windows.Controls;

namespace TrumpBubbles
{
	public partial class App : Application, IDisposable
	{
		internal static MainPage MainPage { get { return ApplicationEx.RootUserControl as MainPage; } }

		public App()
		{
			base.Startup += Application_Startup;
			base.Exit += Application_Exit;
			base.UnhandledException += Application_UnhandledException;
			base.CheckAndDownloadUpdateCompleted += Application_UpdateCompleted;
			InitializeComponent();

			base.CheckAndDownloadUpdateAsync();
		}

		public void Dispose()
		{
			base.Startup -= Application_Startup;
			base.Exit -= Application_Exit;
			base.UnhandledException -= Application_UnhandledException;
			base.CheckAndDownloadUpdateCompleted -= Application_UpdateCompleted;
		}

		private void Application_Startup(object sender, StartupEventArgs e)
		{
			bool ok = HtmlPage.Window.AttachEvent("onbeforeunload", Application_BeforeExit);
			ok = HtmlPage.Document.AttachEvent("onbeforeunload", Application_BeforeExit);
			MainPage mainPage = new MainPage(e.InitParams);
			base.RootVisual = mainPage;
			mainPage.Loaded += delegate(object senderMain, RoutedEventArgs eMain)
			{
			};
		}

		private void Application_UpdateCompleted(object sender, CheckAndDownloadUpdateCompletedEventArgs e)
		{
		}

		private void Application_BeforeExit(object sender, HtmlEventArgs args)
		{
			//PageMethodInvoker pageMethod = new PageMethodInvoker();
			//pageMethod.Completed += delegate(object methodName, bool success, object result, object userContext)
			//{
			//    ScriptObject e = args.EventObject;
			//    if (e != null)
			//        e.SetProperty("returnValue", userContext as string);
			//};
			//pageMethod.Invoke("ModelShutdown", new Guid().ToString(), "Context");
		}
	
		private void Application_Exit(object sender, EventArgs e)
		{
			Dispose();

		}

		private void Application_UnhandledException(object sender, ApplicationUnhandledExceptionEventArgs e)
		{
//j			if (!System.Diagnostics.Debugger.IsAttached)
			{
				// Allow the application to continue running after an exception has been thrown but not handled
				Exception ex = e.ExceptionObject;
//j				ex.Alert();
				e.Handled = true;
			}
		}
	}
}
