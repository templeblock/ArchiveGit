using System;
using System.IO.IsolatedStorage;

namespace ClassLibrary
{
	internal static class Settings
	{
		internal static TT Read<TT>(string name)
		{
			return Read(name, default(TT));
		}

		internal static TT Read<TT>(string name, TT defaultValue)
		{
			try
			{
				IsolatedStorageSettings settings = IsolatedStorageSettings.ApplicationSettings;
				TT value;
				if (settings == null || !settings.TryGetValue(name, out value))
					return defaultValue;
				return value;

			}
			catch (Exception ex)
			{
				ex.GetType();
			}

			return defaultValue;
		}

		internal static void Write<TT>(string name, TT value)
		{
			try
			{
				IsolatedStorageSettings settings = IsolatedStorageSettings.ApplicationSettings;
				if (settings == null)
					return;
				if (settings.Contains(name))
					settings[name] = value;
				else
					settings.Add(name, value);
				settings.Save();
			}
			catch (Exception ex)
			{
				ex.GetType();
			}
		}
	}
}
