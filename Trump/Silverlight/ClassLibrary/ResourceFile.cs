using System;
using System.IO;
using System.Reflection;
using System.Windows;
using System.Windows.Documents;
using System.Windows.Markup;
using System.Windows.Media.Imaging;
using System.Windows.Resources;
using System.Xml;

namespace ClassLibrary
{
	// Get resource files, or resource file streams, either from...
	//  o  an assembly in the XAP (Resource build action)
	//  o  the XAP itself (Content build action)
	//
	// Typical usage:
	//
	// Get content directly: bitmaps, file text, fonts, XAML, or XML
	//
	//		BitmapImage bitmap1 = ResourceFile.GetBitmap("Images/Clouds.jpg", From.App);
	//
	// Get content from a ZIP:
	//
	//		BitmapImage bitmap2 = ResourceFile.GetBitmapFromZip("Content/Zips/Files.zip", "Clouds.jpg", From.App);
	//
	// Get content streams:
	//
	//		Stream bitmapStream = ResourceFile.GetStream("Images/Clouds.jpg", From.App);
	//		Stream zipStream = ResourceFile.GetStream("Content/Zips/Files.zip", From.App);

	internal static class ResourceFile
	{
		// Get a bitmap from an assembly or the XAP
		internal static BitmapImage GetBitmap(string relativeUri, From assembly)
		{
			Stream stream = GetStream(relativeUri, assembly);
			return GetBitmapFromStream(stream);
		}

		// Get a bitmap from a zip file in an assembly or the XAP
		internal static BitmapImage GetBitmapFromZip(string relativeZipUri, string relativeUri, From assembly)
		{
			Stream stream = GetStreamFromZip(relativeZipUri, relativeUri, assembly);
			return GetBitmapFromStream(stream);
		}

		// Get a bitmap from a stream
		internal static BitmapImage GetBitmapFromStream(Stream stream)
		{
			if (stream == null)
				return null;

			BitmapImage bitmap = new BitmapImage();
			bitmap.SetSource(stream);
			return bitmap;
		}

		// Get file text from an assembly or the XAP
		internal static string GetFileText(string relativeUri, From assembly)
		{
			Stream stream = GetStream(relativeUri, assembly);
			return GetFileTextFromStream(stream);
		}

		// Get file text from a zip file in an assembly or the XAP
		internal static string GetFileTextFromZip(string relativeZipUri, string relativeUri, From assembly)
		{
			Stream stream = GetStreamFromZip(relativeZipUri, relativeUri, assembly);
			return GetFileTextFromStream(stream);
		}

		// Get file text from a stream
		internal static string GetFileTextFromStream(Stream stream)
		{
			if (stream == null)
				return null;

			using (StreamReader reader = new StreamReader(stream))
			{
				if (reader == null)
					return null;
				return reader.ReadToEnd();
			}
		}

		// Get a font from an assembly or the XAP
		internal static FontSource GetFont(string relativeUri, From assembly)
		{
			Stream stream = GetStream(relativeUri, assembly);
			return GetFontFromStream(stream);
		}

		// Get a font from a zip file in an assembly or the XAP
		internal static FontSource GetFontFromZip(string relativeZipUri, string relativeUri, From assembly)
		{
			Stream stream = GetStreamFromZip(relativeZipUri, relativeUri, assembly);
			return GetFontFromStream(stream);
		}

		// Get a font from a stream
		internal static FontSource GetFontFromStream(Stream stream)
		{
			if (stream == null)
				return null;

			return new FontSource(stream);
		}

		// Get XAML from an assembly or the XAP
		internal static UIElement GetXaml(string relativeUri, From assembly)
		{
			Stream stream = GetStream(relativeUri, assembly);
			return GetXamlFromStream(stream);
		}

		// Get XAML from a zip file in an assembly or the XAP
		internal static UIElement GetXamlFromZip(string relativeZipUri, string relativeUri, From assembly)
		{
			Stream stream = GetStreamFromZip(relativeZipUri, relativeUri, assembly);
			return GetXamlFromStream(stream);
		}

		// Get XAML from a stream
		internal static UIElement GetXamlFromStream(Stream stream)
		{
			if (stream == null)
				return null;

			return XamlReader.Load(GetFileTextFromStream(stream)) as UIElement;
		}

		// Get an XML Reader from an assembly or the XAP
		internal static XmlReader GetXmlReader(string relativeUri, From assembly)
		{
			Stream stream = GetStream(relativeUri, assembly);
			return GetXmlReaderFromStream(stream);
		}

		// Get an XML Reader from a zip file in an assembly or the XAP
		internal static XmlReader GetXmlReaderFromZip(string relativeZipUri, string relativeUri, From assembly)
		{
			Stream stream = GetStreamFromZip(relativeZipUri, relativeUri, assembly);
			return GetXmlReaderFromStream(stream);
		}

		// Get an XML Reader from a stream
		internal static XmlReader GetXmlReaderFromStream(Stream stream)
		{
			XmlReaderSettings settings = new XmlReaderSettings();
			settings.DtdProcessing = DtdProcessing.Ignore;
			settings.IgnoreWhitespace = false;
			settings.IgnoreProcessingInstructions = true;
			settings.IgnoreComments = true;
			try
			{
				return XmlReader.Create(stream, settings);
			}
			catch (Exception ex)
			{
//j				MessageBox.Show(ex.Message, "Alert", MessageBoxButton.OK);
				ex.Alert();
				return null;
			}
		}

		// Get an XML text string from an XmlReader
		internal static string GetXmlFromXmlReader(XmlReader xmlReader)
		{
			return (xmlReader != null ? xmlReader.ReadOuterXml() : null);
		}

		// Get a resource file stream info from an assembly or the XAP
		internal static StreamResourceInfo GetStreamInfo(string relativeUri, From assembly)
		{
			StreamResourceInfo streamInfo = Application.GetResourceStream(new Uri(relativeUri, UriKind.Relative));
			if (streamInfo != null) // found in the XAP
				return streamInfo;

			string componentFormat = assembly.Name + ";component/{0}";
			string assemblyUri = string.Format(componentFormat, relativeUri);
			streamInfo = Application.GetResourceStream(new Uri(assemblyUri, UriKind.Relative));
			if (streamInfo != null) // found in the assembly
				return streamInfo;

			// Last chance!
			// Resource files added to a VS project as "links" (Add / Existing Item / Add As Link)
			// must be accessed without a path
			int pathMarker = relativeUri.LastIndexOf('/');
			if (pathMarker < 0)
				return null;

			assemblyUri = string.Format(componentFormat, relativeUri.Substring(pathMarker + 1));
			streamInfo = Application.GetResourceStream(new Uri(assemblyUri, UriKind.Relative));
			if (streamInfo != null) // found in the assembly
				return streamInfo;

			return null; // not found
		}

		// Get a resource file stream from an assembly or the XAP
		internal static Stream GetStream(string relativeUri, From assembly)
		{
			StreamResourceInfo streamInfo = GetStreamInfo(relativeUri, assembly);
			if (streamInfo == null)
				return null;

			return streamInfo.Stream;
		}

		// Get a resource file stream from inside a ZIP stream
		internal static Stream GetStreamFromZip(string relativeZipUri, string relativeUri, From assembly)
		{
			Stream zipStream = GetStream(relativeZipUri, assembly);
			if (zipStream == null)
				return null;

			return GetStreamFromZipStream(zipStream, relativeUri);
		}

		// Get a resource file stream from inside a ZIP stream
		internal static Stream GetStreamFromZipStream(Stream zipStream, string relativeUri)
		{
			if (zipStream == null)
				return null;

			StreamResourceInfo zipInfo = new StreamResourceInfo(zipStream, null);
			StreamResourceInfo streamInfo = Application.GetResourceStream(zipInfo, new Uri(relativeUri, UriKind.Relative));
			if (streamInfo == null)
				streamInfo = Application.GetResourceStream(new Uri(relativeUri, UriKind.Relative));
			if (streamInfo == null)
				return null;

			return streamInfo.Stream;
		}
	}

	// Calls to access file resources are assisted by a tiny static class called "From"
	// which is used to indicate the assembly that contains resource files.
	// Typical usage:
	//	From.App - the current application assembly
	//	From.This - the caller's assembly
	//	From.Library - a library assembly
	//	From.Type<ClassLibrary> - an assembly containing a known type
	internal class From
	{
		internal Assembly Assembly { get; set; }
//j		internal string Name { get { return (Assembly != null ? Assembly.FullName.Substring(0, Assembly.FullName.IndexOf(',')) : null); } }
		internal string Name { get { return (Assembly != null ? Assembly.Name() : null); } }

		// From current application assembly
		internal static From App { get { return new From() { Assembly = Application.Current.GetType().Assembly }; } }

		// From the caller's assembly
		internal static From This { get { return new From() { Assembly = Assembly.GetCallingAssembly() }; } }

		// From a library assembly
		internal static From Library { get { return new From() { Assembly = Assembly.GetExecutingAssembly() }; } }

		// From an assembly containing a known type
		internal static From Type<TT>() { return new From() { Assembly = typeof(TT).Assembly }; }
	}
}
