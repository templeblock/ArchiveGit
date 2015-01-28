using System.Diagnostics;

namespace ClassLibrary
{
	internal static class Helper
	{
		// Also an Extension for string
		internal static void Trace(string message, params object[] args)
		{
			Debug.WriteLine(message, args);
		}

		// Also an Extension for string
		internal static void Alert(string message)
		{
			Debug.WriteLine(message);
		}
	}
}
