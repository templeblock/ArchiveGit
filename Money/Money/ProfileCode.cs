using ClassLibrary;
using JulianDay = System.UInt32;

namespace Money
{
	internal class ProfileCode : IntBase
	{
		// Codes use 11 bits; 4 bits for the person count (0-15); 7 bits for the age (0-127)
		internal static ProfileCode Max = 0x7FF;
		internal static ProfileCode Range = 128;
		internal static ProfileCode Death = 124;
		internal static ProfileCode StartOfPlan = 1;
		internal static ProfileCode EndOfPlan = 2;
		internal static ProfileCode MeBirth = 3;
		internal static ProfileCode MeDeath = MeBirth + Death;
		internal static ProfileCode SpouseBirth = MeBirth + Range;
		internal static ProfileCode SpouseDeath = SpouseBirth + Death;
		internal static ProfileCode DependentMin = SpouseBirth + Range;
		internal static ProfileCode DependentMax = DependentDeath(13);
		internal static ProfileCode DependentBirth(int num) { return DependentMin + (num * Range); }
		internal static ProfileCode DependentDeath(int num) { return DependentBirth(num) + Death; }

		public ProfileCode()
		{
		}

		public ProfileCode(int value)
			: base(value)
		{
			if (base.Value < 0 || base.Value > 0x7FF) base.Value = 0;
		}

		public static implicit operator ProfileCode(int profileCode)
		{
			return new ProfileCode(profileCode);
		}

		internal ProfileDate ToProfileDate()
		{
			return new ProfileDate() { ProfileCode = base.Value };
		}

		internal string ToRelativeProfileText(JulianDay date)
		{
			ProfileCode profileCode = base.Value;
			if (profileCode < 0 || profileCode > Max)
				profileCode = 0;
			JulianDay birthDate = 0;
			double lifeExpectancy = 0;
			Profile profile = App.Model.ProfileHolder.Profile;
			if (profileCode == ProfileCode.MeBirth)
			{
				birthDate = profile.Me.BirthDate;
				lifeExpectancy = profile.Me.LifeExpectancy;
			}
			else
			if (profileCode == ProfileCode.SpouseBirth)
			{
				if (profile.IncludeSpouse)
				{
					birthDate = profile.Spouse.BirthDate;
					lifeExpectancy = profile.Spouse.LifeExpectancy;
				}
			}
			else
			if (profileCode >= ProfileCode.DependentMin && profileCode <= ProfileCode.DependentMax)
			{
				int dependent = (profileCode - ProfileCode.DependentMin) / ProfileCode.Range;
				Person person = (profile.IncludeDependents && dependent < profile.Dependents.Count ? profile.Dependents[dependent] : null);
				if (person != null)
				{
					birthDate = person.BirthDate;
					lifeExpectancy = person.LifeExpectancy;
				}
			}

			if (birthDate == 0)
				return null;

			int daysAlive = (int)date - (int)birthDate;
			if (daysAlive < 0)
				return null;

//j			if (daysAlive > JulianDate.YearsToDays(lifeExpectancy))
//j				return null;

			double age = JulianDate.DaysToYears((uint)daysAlive);
			profileCode += (int)age;

			ProfileDate pjd = new ProfileDate() { ProfileCode = profileCode, JulianDay = date };
			return pjd.GetTextFromProfile();
		}
	}
}
