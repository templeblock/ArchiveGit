using System;
using System.ComponentModel.Composition;
using System.Windows;
using ClassLibrary;

namespace CharityRace
{
	public partial class App
	{
		public static Model Model { get; set; }

		[Import]
		public ShellView Shell { get { return base.RootVisual as ShellView; } set { base.RootVisual = value; } }

		public App()
		{
			base.Startup += Application_Startup;
			base.Exit -= Application_Exit;
			base.UnhandledException += Application_UnhandledException;

			InitializeComponent();
		}

		public void Dispose()
		{
			base.Startup -= Application_Startup;
			base.UnhandledException -= Application_UnhandledException;
		}

		private void Application_Startup(object sender, StartupEventArgs e)
		{
			Model = new Model();
			Model.Load();
			CompositionInitializer.SatisfyImports(this);
		}

		private void Application_Exit(object sender, EventArgs e)
		{
			Dispose();
		}

		private void Application_UnhandledException(object sender, ApplicationUnhandledExceptionEventArgs e)
		{
			// Allow the application to continue running after an exception has been thrown but not handled
			Exception ex = e.ExceptionObject;
			ex.Alert();
			e.Handled = true;
		}
	}
}