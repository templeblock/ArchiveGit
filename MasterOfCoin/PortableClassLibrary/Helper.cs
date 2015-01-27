using System.Diagnostics;
using Windows.ApplicationModel;

namespace ClassLibrary
{
	internal static class Helper
	{
		internal static bool IsInDesignTool { get { return DesignMode.DesignModeEnabled; } }

		// Also an Extension for string
		internal static void Trace(string message, params object[] args)
		{
			Debug.WriteLine(message, args);
		}

		// Also an Extension for string
		//internal static void Alert(string message)
		//{
		//	MessageBox.Show(message, "Alert", MessageBoxButton.OK);
		//}
	}
}
