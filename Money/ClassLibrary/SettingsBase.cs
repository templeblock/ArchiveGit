using System;
using System.Collections.Generic;
using System.Reflection;
using System.Windows;
using System.Windows.Browser;
using System.Windows.Media;

namespace ClassLibrary
{
	public abstract class SettingsBase
	{
		internal void LoadFromInitParams(IDictionary<string, string> initParams)
		{
			// Load the initParams into the public fields of the derived class
			BindingFlags flags = BindingFlags.Instance | BindingFlags.FlattenHierarchy | BindingFlags.Public | BindingFlags.NonPublic;
			FieldInfo[] fields = this.GetType().GetFields(flags);

			// Check the initParams for unknown keys
			string unknownParams = null;
			foreach (string key in initParams.Keys)
			{
				if (key.StartsWith("//"))
					continue;
				if (ParamKeyIsKnown(key, fields))
					continue;

				// Track any unknown parameters
				unknownParams += string.Format("\t{0} = {1}\n", key, initParams[key]);
			}

			// Load the initParams; each parameter key must match the name of a public fields (case is ignored)
			string invalidParams = null;
			char[] invalidValueChars = { '\r', '\n', '\t' };
			foreach (FieldInfo field in fields)
			{
				string stringValue = initParams.GetValueIgnoreCase(field.Name, null);
				if (stringValue == null)
					continue;
				if (stringValue == "null")
					stringValue = "";
				else
				if (stringValue.IndexOfAny(invalidValueChars) >= 0)
				{
					invalidParams += string.Format("\t{0} = {1}\n", field.Name, stringValue);
					continue;
				}

				try
				{
					object convertedValue = null;
					if (field.FieldType.IsEnum)
					{
						stringValue = StandardizeDelimeters(stringValue);
						string typePrefix = field.FieldType.FriendlyName() + ".";
						int indexPrefix = (stringValue.IndexOf(typePrefix) == 0 ? typePrefix.Length : 0);
						convertedValue = Enum.Parse(field.FieldType, stringValue.Substring(indexPrefix), true/*ignoreCase*/);
					}
					else
					if (field.FieldType.Equals(typeof(Uri)))
						convertedValue = UriFactory(stringValue);
					else
					if (field.FieldType.Equals(typeof(Color)))
					{
						stringValue = StandardizeDelimeters(stringValue);
						convertedValue = stringValue.ToColor();
					}
					else
					if (field.FieldType.Equals(typeof(Size)))
					{
						string[] strings = Split(stringValue);
						if (strings.Length == 0)
							convertedValue = new Size();
						else
						if (strings.Length == 2)
							convertedValue = new Size(double.Parse(strings[0]), double.Parse(strings[1]));
					}
					else
					if (field.FieldType.Equals(typeof(Point)))
					{
						string[] strings = Split(stringValue);
						if (strings.Length == 0)
							convertedValue = new Point();
						else
						if (strings.Length == 2)
							convertedValue = new Point(double.Parse(strings[0]), double.Parse(strings[1]));
					}
					else
					if (field.FieldType.Equals(typeof(Rect)))
					{
						string[] strings = Split(stringValue);
						if (strings.Length == 0)
							convertedValue = new Rect();
						else
						if (strings.Length == 4)
							convertedValue = new Rect(double.Parse(strings[0]), double.Parse(strings[1]),
													  double.Parse(strings[2]), double.Parse(strings[3]));
					}
					else
					if (field.FieldType.Equals(typeof(Thickness)))
					{
						string[] strings = Split(stringValue);
						if (strings.Length == 0)
							convertedValue = new Thickness();
						else
						if (strings.Length == 1)
							convertedValue = new Thickness(double.Parse(strings[0]));
						else
						if (strings.Length == 4)
							convertedValue = new Thickness(double.Parse(strings[0]), double.Parse(strings[1]),
														   double.Parse(strings[2]), double.Parse(strings[3]));
					}
					else
						convertedValue = Convert.ChangeType(stringValue, field.FieldType, null);

					field.SetValue(this, convertedValue);
				}
				catch
				{
					// Track any invaild parameters
					invalidParams += string.Format("\t{0} = {1}\n", field.Name, stringValue);
				}
			}

			// Display any unknown or invalid parameters
			if (invalidParams != null || unknownParams != null)
			{
				string message = null;
				if (unknownParams != null)
					message += string.Format("Unknown parameters:\n\n{0}\n", unknownParams);

				if (invalidParams != null)
					message += string.Format("Invalid parameters:\n\n{0}\n", invalidParams);

				message += string.Format("Note - the following parameters can be passed:\n\n");
				foreach (FieldInfo field in fields)
					message += string.Format("\t{0} = typeof({1})\n", field.Name, field.FieldType.FriendlyName());

				message.Alert();
			}

			InitParamsLoaded();
		}

		protected virtual void InitParamsLoaded()
		{
		}

		private static Uri UriFactory(string uriString)
		{
			if (string.IsNullOrEmpty(uriString))
				return null;

			// If this is an absoluteUri...
			if (uriString.IndexOf(Uri.SchemeDelimiter) >= 0)
				return new Uri(uriString);

			// Convert the relativeUri to an absoluteUri, as in...
			// absoluteUri = http://www.domain.com/appfolder/relativeUri
			return new Uri(HtmlPage.Document.DocumentUri, "./" + uriString);
		}

		private static bool ParamKeyIsKnown(string key, FieldInfo[] fields)
		{
			foreach (FieldInfo field in fields)
			{
				if (key.EqualsIgnoreCase(field.Name))
					return true;
			}

			return false;
		}

		private string[] Split(string value)
		{
			value = StandardizeDelimeters(value);
			return value.Split(new char[] { ',' });
		}

		private string StandardizeDelimeters(string value)
		{
			if (value.Contains("-")) value = value.Replace('-', ','); else
			if (value.Contains("|")) value = value.Replace('|', ','); else
			if (value.Contains(";")) value = value.Replace(';', ',');
			return value;
		}
	}
}
