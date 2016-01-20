using System;
using System.IO;
using System.Reflection;
using System.Windows;
using System.Windows.Documents;
using System.Windows.Markup;
using System.Windows.Media.Imaging;
using System.Windows.Resources;

namespace ClassLibrary
{
	public static class Resource
	{
		public static Assembly GetAssembly<TT>()
		{
			Assembly assembly = typeof(TT).Assembly;
			if (assembly == null)
				return Assembly.GetExecutingAssembly(); // Assembly.GetCallingAssembly();
			return assembly;
		}

		// Get an resource from the assembly DLL
		public static Stream GetFromDll<TT>(string relativeUri)
		{
			string uri = GetAssembly<TT>().Name() + "." + relativeUri.Replace("/", ".");
			return GetAssembly<TT>().GetManifestResourceStream(uri);
		}

		// Get a resource from a ZIP stream
		public static Stream GetFromZip(Stream zipStream, string relativeUri)
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

		// Get a resource from the XAP
		public static Stream GetFromXap<TT>(string relativeUri)
		{
			string componentUri = GetAssembly<TT>().Name() + ";component/" + relativeUri;
			StreamResourceInfo streamInfo = Application.GetResourceStream(new Uri(componentUri, UriKind.Relative));
			if (streamInfo == null)
				streamInfo = Application.GetResourceStream(new Uri(relativeUri, UriKind.Relative));
			if (streamInfo == null)
				return null;

			return streamInfo.Stream;
		}

		// Get a bitmap from a stream
		public static BitmapImage GetBitmapFromStream(Stream stream)
		{
			if (stream == null)
				return null;

			BitmapImage bitmap = new BitmapImage();
			bitmap.SetSource(stream);
			return bitmap;
		}

		// Get a string from a stream
		public static string GetStringFromStream(Stream stream)
		{
			if (stream == null)
				return null;

			using (StreamReader reader = new StreamReader(stream))
			{
				return reader.ReadToEnd();
			}
		}

		// Get a font from a stream
		public static FontSource GetFontFromStream(Stream stream)
		{
			if (stream == null)
				return null;

			return new FontSource(stream);
		}

		// Get a XAML object from a stream
		public static object GetXamlFromStream(Stream stream)
		{
			if (stream == null)
				return null;

			return XamlReader.Load(GetStringFromStream(stream));
		}

		// Get a bitmap from the XAP
		public static BitmapImage GetBitmapFromXap<TT>(string relativeUri)
		{
			Stream stream = GetFromXap<TT>(relativeUri);
			return GetBitmapFromStream(stream);
		}

		// Get a string from the XAP
		public static string GetStringFromXap<TT>(string relativeUri)
		{
			Stream stream = GetFromXap<TT>(relativeUri);
			return GetStringFromStream(stream);
		}

		// Get a font from the XAP
		public static FontSource GetFontFromXap<TT>(string relativeUri)
		{
			Stream stream = GetFromXap<TT>(relativeUri);
			return GetFontFromStream(stream);
		}

		// Get a XAML object from the XAP
		public static object GetXamlFromXap<TT>(string relativeUri)
		{
			Stream stream = GetFromXap<TT>(relativeUri);
			return GetXamlFromStream(stream);
		}
	}
}
