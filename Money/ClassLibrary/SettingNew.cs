using System;
using System.Collections.Generic;
using System.Windows;

namespace ClassLibrary
{
#if NOTUSED
		settings.Initialize(new Uri("http://localhost:6655/"), "james.m.mccurdy@wellsfargo.com");
		settings.Write("Visibility", Visibility.Hidden);
		settings.Write("TestString", "AAA", false);
		settings.Write("TestInt", 123, false);
		settings.Save();
		int intValue = settings.Read<int>("TestInt");
		string stringValue = settings.Read<string>("TestString");
		Visibility visibility = settings.Read<Visibility>("Visibility");
		string[] names = { "TestString", "TestBBB", "TestInt" };
		IDictionary<string, object> dict = new Dictionary<string, object>();
		settings.WriteDictionary(dict);
#endif

	public class Settings
	{
		private SettingsDictionary _settingsDictionary = null;
		private bool _error = false;

		public void Initialize(Uri serverUri, string userId)
		{
			_settingsDictionary = new SettingsDictionary(serverUri, userId);
		}

		public TT Read<TT>(string key)
		{
			return Read<TT>(key, default(TT));
		}

		public TT Read<TT>(string key, TT defaultValue)
		{
			if (!ValidateSettingsDictionary())
				return defaultValue;

			try
			{
				string valueString;
				if (!_settingsDictionary.TryGetValue(key, out valueString))
					return defaultValue;

				object value;
				bool isSerialized = (typeof(TT) != typeof(string) && !typeof(TT).IsValueType);
				if (!isSerialized)
					value = (typeof(TT).IsEnum ? Enum.Parse(typeof(TT), valueString) : Convert.ChangeType(valueString, typeof(TT)));
				else
				{
					Serializer serializer = new Serializer(false/*isDCS*/);
					value = serializer.Deserialize(valueString, typeof(TT));
				}

				return (TT)value;
			}
			catch (Exception ex)
			{
				ex.GetType();
				return defaultValue;
			}
		}

		public void Save()
		{
			if (!ValidateSettingsDictionary())
				return;

			_settingsDictionary.Save();
		}

		public void Write(string key, object value, bool save = true)
		{
			if (!ValidateSettingsDictionary())
				return;

			bool changed = WriteSetting(key, value, _settingsDictionary);
			if (save && changed)
				_settingsDictionary.Save();
		}

		public void WriteDictionary(IDictionary<string, object> dictionary, bool save = true)
		{
			if (!ValidateSettingsDictionary())
				return;

			bool changed = false;
			ICollection<string> keys = dictionary.Keys;
			foreach (string key in keys)
			{
				object value = dictionary[key];
				changed |= WriteSetting(key, value, _settingsDictionary);
			}

			if (save && changed)
				_settingsDictionary.Save();
		}

		private bool WriteSetting(string key, object value, SettingsDictionary settingsDictionary)
		{
			bool changed = false;
			try
			{
				string valueString = null;
				if (value != null)
				{
					Type type = value.GetType();
					bool isSerialized = (type != typeof(string) && !type.IsValueType);
					if (!isSerialized)
						valueString = value.ToString();
					else
					{
						Serializer serializer = new Serializer(false/*isDCS*/);
						valueString = serializer.Serialize(value);
					}
				}

				if (settingsDictionary.ContainsKey(key))
				{
					changed = (settingsDictionary[key] != valueString);
					settingsDictionary[key] = valueString;
				}
				else
				{
					changed = true;
					settingsDictionary.Add(key, valueString);
				}
			}
			catch (Exception ex)
			{
				ex.GetType();
			}

			return changed;
		}

		private bool ValidateSettingsDictionary()
		{
			if (_settingsDictionary != null)
				return true;

			if (!_error)
				MessageBox.Show("Settings were not initialized", "Error", MessageBoxButton.OK);
			_error = true;
			return false;
		}
	}
}
