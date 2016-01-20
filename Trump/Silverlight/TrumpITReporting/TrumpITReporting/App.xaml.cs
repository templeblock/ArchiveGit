using System;
using System.Windows;
using System.Windows.Browser;
using ClassLibrary;

namespace TrumpITReporting
{
	public partial class App : Application, IDisposable
	{
		internal static Model Model { get; set; }

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
			Model.Dispose();
			Model = null;

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
			Model = new Model();
			mainPage.Loaded += delegate(object senderMain, RoutedEventArgs eMain)
			{
				Model.Initialize();
			};
		}

		private void Application_UpdateCompleted(object sender, CheckAndDownloadUpdateCompletedEventArgs e)
		{
		}

		private void Application_BeforeExit(object sender, HtmlEventArgs args)
		{
			// Upload the user's documents back to the database
//			if (!model.DocumentHolder.Document.Modified)
//				return null;

//j			MessageBoxResult messageBoxResult = MessageBox.Show(
//j				"Save changes you have made?", "Save changes?", MessageBoxButton.OKCancel);
//j			if (messageBoxResult != MessageBoxResult.OK)
//j			{
//j				//return "If you close this window you will lose any unsaved work.";
//j				return null;
//j			}

//j			WaitingWindow waitingWindow = new WaitingWindow();
//j			waitingWindow.NoticeText = "Saving work on server. Please wait...";
//j			waitingWindow.Show();

			PageMethodInvoker pageMethod = new PageMethodInvoker();
			pageMethod.Completed += delegate(object methodName, bool success, object result, object userContext)
			{
				ScriptObject e = args.EventObject;
				if (e != null)
					e.SetProperty("returnValue", userContext as string);
			};
			pageMethod.Invoke("ModelShutdown", new Guid().ToString(), "Context");
		}
	
		private void Application_Exit(object sender, EventArgs e)
		{
			Dispose();

//j			HtmlPage.Window.Navigate(new Uri("http://trumpnetwork.com", UriKind.Absolute));
		}

		private void Application_UnhandledException(object sender, ApplicationUnhandledExceptionEventArgs e)
		{
//j			if (!System.Diagnostics.Debugger.IsAttached)
			{
				// Allow the application to continue running after an exception has been thrown but not handled
				Exception ex = e.ExceptionObject;
#if false
				// Report the exception using a ChildWindow control
				ChildWindow dialog = new ErrorWindow(e.ExceptionObject);
				//dialog.Title = "Error";
				//dialog.Closed += ExceptionDialogClosed;
				dialog.Show();
#else
				ex.Alert();
#endif
				e.Handled = true;
			}
		}
	}
}
