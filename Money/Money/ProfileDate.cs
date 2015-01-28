using System;
using System.Globalization;
using System.Windows.Data;
using ClassLibrary;
using JulianDay = System.UInt32;

namespace Money
{
	public class ProfileDate
	{
		internal JulianDay JulianDay { get { return _JulianDay; } set { _JulianDay = value; UpdateFromProfile(); } }
		private JulianDay _JulianDay = DateTime.Now.ToJulian();

		internal ProfileCode ProfileCode { get { return _ProfileCode; } set { _ProfileCode = value; UpdateFromProfile(); } }
		private ProfileCode _ProfileCode = 0;

		public uint Value
		{
			get
			{
				return ((uint)_ProfileCode << JulianDate.Bits) + _JulianDay;
			}

			set
			{
				_JulianDay = (JulianDay)(value & JulianDate.Mask);
				_ProfileCode = (int)(value >> JulianDate.Bits);
				UpdateFromProfile();
			}
		}

		public void Init()
		{
			_JulianDay = DateTime.Now.ToJulian();
			_ProfileCode = 0;
		}

		internal ProfileDate UpdateFromProfile()
		{
			if (_ProfileCode < 0 || _ProfileCode > ProfileCode.Max)
				_ProfileCode = 0;

			if (_ProfileCode == 0)
				return this;

			Profile profile = App.Model.ProfileHolder.Profile;
			if (_ProfileCode == ProfileCode.StartOfPlan)
				_JulianDay = profile.StartProfileDate.JulianDay;
			else
			if (_ProfileCode == ProfileCode.EndOfPlan)
				_JulianDay = profile.EndProfileDate.JulianDay;
			else
			if (_ProfileCode >= ProfileCode.MeBirth && _ProfileCode <= ProfileCode.MeDeath)
			{
				if (profile.Me.BirthDate != 0)
				{
					int age = _ProfileCode - ProfileCode.MeBirth;
					_JulianDay = profile.Me.BirthDate + JulianDate.YearsToDays(
						age != ProfileCode.Death ? age : profile.Me.LifeExpectancy);
				}
			}
			else
			if (_ProfileCode >= ProfileCode.SpouseBirth && _ProfileCode <= ProfileCode.SpouseDeath)
			{
				if (profile.IncludeSpouse && profile.Spouse.BirthDate != 0)
				{
					int age = _ProfileCode - ProfileCode.SpouseBirth;
					_JulianDay = profile.Spouse.BirthDate + JulianDate.YearsToDays(
						age != ProfileCode.Death ? age : profile.Spouse.LifeExpectancy);
				}
			}
			else
			if (_ProfileCode >= ProfileCode.DependentMin && _ProfileCode <= ProfileCode.DependentMax)
			{
				int dependent = (_ProfileCode - ProfileCode.DependentMin) / ProfileCode.Range;
				Person person = (profile.IncludeDependents && dependent < profile.Dependents.Count ? profile.Dependents[(int)dependent] : null);
				if (person != null && person.BirthDate != 0)
				{
					int age = _ProfileCode - ProfileCode.DependentBirth(dependent);
					_JulianDay = person.BirthDate + JulianDate.YearsToDays(
						age != ProfileCode.Death ? age : person.LifeExpectancy);
				}
			}

			return this;
		}

		internal string GetTextFromProfile()
		{
			UpdateFromProfile();

			if (_ProfileCode == 0 && _JulianDay == 0)
				return "Unknown";

			Profile profile = App.Model.ProfileHolder.Profile;
			if (_ProfileCode == 0 || profile == null)
				return string.Format("{0:d}", _JulianDay.ToDateTime());

			if (_ProfileCode == ProfileCode.StartOfPlan)
				return "Plan start";

			if (_ProfileCode == ProfileCode.EndOfPlan)
				return "Plan end";

			int age = 0;
			string firstName = null;
			if (_ProfileCode >= ProfileCode.MeBirth && _ProfileCode <= ProfileCode.MeDeath)
			{
				age = _ProfileCode - ProfileCode.MeBirth;
				firstName = (!profile.Me.FirstName.IsEmpty() ? profile.Me.FirstName : "Me");
			}
			else
			if (_ProfileCode >= ProfileCode.SpouseBirth && _ProfileCode <= ProfileCode.SpouseDeath)
			{
				age = _ProfileCode - ProfileCode.SpouseBirth;
				firstName = (profile.IncludeSpouse && !profile.Spouse.FirstName.IsEmpty() ? profile.Spouse.FirstName : "Spouse");
			}
			else
			if (_ProfileCode >= ProfileCode.DependentMin && _ProfileCode <= ProfileCode.DependentMax)
			{
				int dependent = (_ProfileCode - ProfileCode.DependentMin) / ProfileCode.Range;
				age = _ProfileCode - ProfileCode.DependentBirth(dependent);
				Person person = (profile.IncludeDependents && dependent < profile.Dependents.Count ? profile.Dependents[(int)dependent] : null);
				firstName = (person != null && !person.FirstName.IsEmpty() ? person.FirstName : string.Format("Dep{0}", dependent + 1));
			}

			if (firstName == null)
				return string.Format("{0:d}", _JulianDay.ToDateTime());

			if (age == 0)
				return string.Format("{0} at birth", firstName);

			if (age == ProfileCode.Death)
				return string.Format("{0} at death", firstName);

			return string.Format("{0} at {1}", firstName, age);
		}
	}

	public class ProfileDateToText : IValueConverter
	{
		public object Convert(object value, Type targetType, object parameter, CultureInfo culture)
		{
			if (value == null) // Special case
				return null;
			ProfileDate date = (ProfileDate)value;
			string text = date.GetTextFromProfile();
			return text;
		}

		public object ConvertBack(object value, Type targetType, object parameter, CultureInfo culture)
		{
			if (value == null) // Special case
				return null;
			string MMDDYYYY = (string)value;
			if (MMDDYYYY.IsEmpty()) // Special case
				return 0;
			DateTime dateTime = DateTime.Parse(MMDDYYYY);
			return new ProfileDate() { JulianDay = dateTime.ToJulian() };
		}
	}
}
