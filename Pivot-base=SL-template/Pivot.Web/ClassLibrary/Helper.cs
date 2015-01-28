﻿using System.Diagnostics;

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

#if NOTUSED
		internal static void DisplayXmlDocument(string xmlString)
		{
			//xmlString = "<?xml version=\"1.0\" encoding=\"utf-8\" standalone=\"yes\" ?>" + xmlString;
			//xmlString = HttpUtility.HtmlEncode(xmlString);
			//xmlString = xmlString.Replace('\'', 'z');
			//xmlString = xmlString.Replace('"', 'q');
			string str = "var newWin = window.open('about:blank', 'xml'); if (newWin != null) newWin.document.write('" + xmlString + "');";
			try
			{
				HtmlPage.Window.Eval(str); 
			}
			catch (Exception ex)
			{
				ex.Assert();
			}
		}
#endif // NOTUSED
	}
}
