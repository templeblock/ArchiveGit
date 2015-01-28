//j What I Spend - can be estimated from How I Live (Necessities, Discretionary)
//j What I Earn - can be estimated from How I Live
//j What about Outside influences? cost of living, inflation, equity performance
//j What about Major Life Events?

using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Collections.Specialized;
using System.ComponentModel;
using System.Globalization;
using System.Linq;
using System.Runtime.Serialization;
using System.Windows.Data;
using System.Windows.Input;
using System.Windows.Threading;
using System.Xml.Serialization;
using ClassLibrary;
using JulianDay = System.UInt32;

namespace Money
{
	public class ProfileHolder : ViewModelBase
	{
		public Profile Profile { get { return _Profile; } set { SetProperty(ref _Profile, value, System.Reflection.MethodBase.GetCurrentMethod().Name, OnProfileCreated); } }
		private Profile _Profile = new Profile();

		public event PropertyChangedEventHandler ProfileCreated;

		private DispatcherTimer _AutoSaveTimer = new DispatcherTimer();

		public ProfileHolder()
		{
		}

		public override void Dispose()
		{
			base.Dispose();
			Profile.Dispose();
			ProfileCreated = null;
			_AutoSaveTimer = new DispatcherTimer();
		}

		internal void Initialize()
		{
			// Load the user defaults
			OpenDefault();

			// AutoSave the current document every 60 seconds
			_AutoSaveTimer.Interval = new TimeSpan(60 * 1000 * TimeSpan.TicksPerMillisecond);
			_AutoSaveTimer.Tick += delegate(object sender, EventArgs e)
			{
				if (Profile != null && !AutoSave())
					MessageBoxEx.ShowError("AutoSave", string.Format("Cannot AutoSave the current profile"), null);
			};
			_AutoSaveTimer.Start();
		}

		private void OnProfileCreated(Profile profile)
		{
			if (ProfileCreated != null)
				ProfileCreated(this, null);
		}

		internal bool OpenDefault()
		{
			if (Open(ResxResources.DefaultProfileFileName))
				return true;

			//MessageBoxEx.ShowOK("Alert", string.Format("No default profile was located.  Please create one in the Family Profile tab."), null);
			return Open(null/*empty*/);
		}

		internal bool Open(object obj)
		{
			Profile profile = null;
			if (obj is FileInfoHelper || obj is string/*fileName*/)
				profile = Profile.Open(obj);
			else
				profile = new Profile();

			if (profile == null)
				return false;

			Profile = profile;
			if (Profile.StartProfileDate.JulianDay == 0) Profile.StartProfileDate = ProfileCode.MeBirth.ToProfileDate();
			if (Profile.EndProfileDate.JulianDay == 0) Profile.EndProfileDate = ProfileCode.MeDeath.ToProfileDate();
			return true;
		}

		internal bool AutoSave()
		{
			return Profile.Save(ResxResources.DefaultProfileFileName);
		}

		internal bool Save(string filename)
		{
			return Profile.Save(filename);
		}

#if NOTUSED
		internal void Close()
		{
			Profile = new Profile();
		}
#endif
	}

[DataContract]
[XmlRoot]
	public class Profile : ViewModelBase
	{
[XmlAttribute]
[DataMember] // Do not use internally; for serialization purposes only
[DependsOn("StartProfileDate")]
		public uint StartDate { get { return StartProfileDate.Value; } set { StartProfileDate.Value = value; } }
[XmlIgnore]
[IgnoreDataMember]
public ProfileDate StartProfileDate { get { return _StartProfileDate; } set { SetProperty(ref _StartProfileDate, value, System.Reflection.MethodBase.GetCurrentMethod().Name); } }
		private ProfileDate _StartProfileDate = new ProfileDate();
[XmlAttribute]
[DataMember] // Do not use internally; for serialization purposes only
[DependsOn("EndProfileDate")]
		public uint EndDate { get { return EndProfileDate.Value; } set { EndProfileDate.Value = value; } }
[XmlIgnore]
[IgnoreDataMember]
public ProfileDate EndProfileDate { get { return _EndProfileDate; } set { SetProperty(ref _EndProfileDate, value, System.Reflection.MethodBase.GetCurrentMethod().Name); } }
		private ProfileDate _EndProfileDate = new ProfileDate();
[XmlIgnore]
[IgnoreDataMember]
		public JulianDay CurrentDate { get { return _CurrentDate; } set { SetProperty(ref _CurrentDate, value, System.Reflection.MethodBase.GetCurrentMethod().Name); } }
		private JulianDay _CurrentDate = DateTime.Now.ToJulian();
[XmlAttribute]
[DataMember]
		public bool IncludeSpouse { get { return _IncludeSpouse; } set { SetProperty(ref _IncludeSpouse, value, System.Reflection.MethodBase.GetCurrentMethod().Name); } }
		private bool _IncludeSpouse;
[XmlAttribute]
[DataMember]
		public bool IncludeDependents { get { return _IncludeDependents; } set { SetProperty(ref _IncludeDependents, value, System.Reflection.MethodBase.GetCurrentMethod().Name); } }
		private bool _IncludeDependents;
[XmlElement]
[DataMember]
		public PersonEx Me { get { return _Me; } set { SetProperty(ref _Me, value, System.Reflection.MethodBase.GetCurrentMethod().Name); } }
		private PersonEx _Me = new PersonEx();
[XmlElement]
[DataMember]
		public PersonEx Spouse { get { return _Spouse; } set { SetProperty(ref _Spouse, value, System.Reflection.MethodBase.GetCurrentMethod().Name); } }
		private PersonEx _Spouse = new PersonEx();
[XmlElement]
[DataMember]
		public PersonCollection Dependents { get { return _Dependents; } set { SetProperty(ref _Dependents, value, System.Reflection.MethodBase.GetCurrentMethod().Name); } }
		private PersonCollection _Dependents = new PersonCollection();

		public event PropertyChangedEventHandler ProfileChanged;

		public Profile()
		{
			_IncludeSpouse = false;
			_IncludeDependents = false;

			_Me.ParentProfile = this;
			_Spouse.ParentProfile = this;

			_Me.PropertyChanged += OnProfilePropertyChanged;
			_Spouse.PropertyChanged += OnProfilePropertyChanged;
			_Dependents.CollectionChanged += OnDependentsCollectionChanged;
			PropertyChanged += OnProfilePropertyChanged;
		}

		public override void Dispose()
		{
			base.Dispose();
			_Me.Dispose();
			_Spouse.Dispose();
			_Dependents.Dispose();
			ProfileChanged = null;
		}

		public bool IsIncomplete
		{
			get
			{
				if (_Me.BirthDate == 0 || _Me.LifeExpectancy == 0)
					return true;
				if (_IncludeSpouse && (_Spouse.BirthDate == 0 || _Spouse.LifeExpectancy == 0))
					return true;
				if (_IncludeDependents)
				{
					foreach (Person person in _Dependents)
					{
						if (person.BirthDate == 0 || person.LifeExpectancy == 0)
							return true;
					}
				}

				return false;
			}
		}

		private void OnProfilePropertyChanged(object sender, PropertyChangedEventArgs e)
		{
			if (e.PropertyName == "Me")
			{
				_Me.PropertyChanged -= OnProfilePropertyChanged;
				_Me.PropertyChanged += OnProfilePropertyChanged;
				return;
			}
			if (e.PropertyName == "Spouse")
			{
				_Spouse.PropertyChanged -= OnProfilePropertyChanged;
				_Spouse.PropertyChanged += OnProfilePropertyChanged;
				return;
			}
			if (e.PropertyName == "Dependents")
			{
				_Dependents.CollectionChanged -= OnDependentsCollectionChanged;
				_Dependents.CollectionChanged += OnDependentsCollectionChanged;
				return;
			}
			if (e.PropertyName == "FirstName" || 
				e.PropertyName == "Gender" || 
				e.PropertyName == "RelationshipStatus" ||
				e.PropertyName == "BirthDate" || 
				e.PropertyName == "LifeExpectancy" || 
				e.PropertyName == "IncludeSpouse" || 
				e.PropertyName == "IncludeDependents" ||
				e.PropertyName == "StartDate" ||
				e.PropertyName == "EndDate" ||
				e.PropertyName == "CurrentDate")
					OnProfileChanged(e);
		}

		private void OnDependentsCollectionChanged(object sender, NotifyCollectionChangedEventArgs e)
		{
			PersonCollection dependents = sender as PersonCollection;
			foreach (Person person in dependents)
			{
				person.PropertyChanged -= OnProfilePropertyChanged;
				person.PropertyChanged += OnProfilePropertyChanged;
			}
		}

		private void OnProfileChanged(PropertyChangedEventArgs e)
		{
			StartProfileDate = _StartProfileDate.UpdateFromProfile();
			EndProfileDate = _EndProfileDate.UpdateFromProfile();

			if (ProfileChanged != null)
				ProfileChanged(null, e);
		}

		internal static Profile Open(object obj)
		{
			try
			{
				if (!IsolatedFileHelper.IsStorageAllowed())
					return null;
				if (obj is FileInfoHelper)
					return Load((obj as FileInfoHelper).ReadString());
				if (obj is string)
					return Load(IsolatedFileHelper.ReadFileString(obj as string));
				return null;
			}
			catch (Exception ex)
			{
				ex.Alert();
			}

			return null;
		}

		private static Profile Load(string profile)
		{
			try
			{
				if (profile == null)
					return null;

				Serializer serializer = new Serializer(false/*isDCS*/);
				return serializer.Deserialize<Profile>(profile);
			}
			catch (Exception ex)
			{
				ex.Alert();
			}

			return null;
		}

		internal bool Save(string fileName)
		{
			try
			{
				Serializer serializer = new Serializer(false/*isDCS*/);
				string profile = serializer.Serialize(this);
				if (profile == null)
					return false;

				IsolatedFileHelper.WriteFileString(fileName, profile);
				return true;
			}
			catch (Exception ex)
			{
				ex.Alert();
			}

			return false;
		}

		internal Person PersonNumToPerson(int personNum)
		{
			Profile profile = App.Model.ProfileHolder.Profile;
			if (personNum == 0)
				return Me;

			if (personNum == 1)
				return (profile.IncludeSpouse ? Spouse : null);

			int dependent = personNum - 2;
			if (profile.IncludeDependents && dependent < profile.Dependents.Count)
				return profile.Dependents[dependent];

			return null;
		}
	}

[DataContract]
[XmlRoot]
	public class Person : ViewModelBase
	{
[XmlAttribute]
[DataMember]
		public string FirstName { get { return _FirstName; } set { SetProperty(ref _FirstName, value, System.Reflection.MethodBase.GetCurrentMethod().Name); } }
		private string _FirstName;
[XmlAttribute]
[DataMember]
		public Gender Gender { get { return _Gender; } set { SetProperty(ref _Gender, value, System.Reflection.MethodBase.GetCurrentMethod().Name, SetNewGender); } }
		private Gender _Gender = Gender.Unknown;
[XmlAttribute]
[DataMember]
		public JulianDay BirthDate { get { return _BirthDate; } set { SetProperty(ref _BirthDate, value, System.Reflection.MethodBase.GetCurrentMethod().Name, SetNewBirthDate); } }
		private JulianDay _BirthDate;
[XmlAttribute]
[DataMember]
		// LifeExpectancy will be computed asynchronously from Gender and BirthDate, or directly set from the UI
		public double LifeExpectancy { get { return _LifeExpectancy; } set { SetProperty(ref _LifeExpectancy, value, System.Reflection.MethodBase.GetCurrentMethod().Name); } }
		private double _LifeExpectancy; // 0 = default of 100 years old
[XmlAttribute]
[DataMember]
		public RelationshipStatus RelationshipStatus { get { return _RelationshipStatus; } set { SetProperty(ref _RelationshipStatus, value, System.Reflection.MethodBase.GetCurrentMethod().Name); } }
		private RelationshipStatus _RelationshipStatus = RelationshipStatus.Unknown; // Should change over time
[XmlElement]
[DataMember]
		public IncomeTaxStatus IncomeTaxStatus { get { return _IncomeTaxStatus; } set { SetProperty(ref _IncomeTaxStatus, value, System.Reflection.MethodBase.GetCurrentMethod().Name); } }
		private IncomeTaxStatus _IncomeTaxStatus = new IncomeTaxStatus(); // Should change over time
[XmlElement]
[DataMember]
		public Location Location { get { return _Location; } set { SetProperty(ref _Location, value, System.Reflection.MethodBase.GetCurrentMethod().Name); } }
		private Location _Location = new Location(); // Should change over time
[XmlIgnore]
[IgnoreDataMember]
		public Profile ParentProfile { get { return _ParentProfile; } set { SetProperty(ref _ParentProfile, value, System.Reflection.MethodBase.GetCurrentMethod().Name); } }
		private Profile _ParentProfile = null;

		public Person()
		{
		}

		public override void Dispose()
		{
			base.Dispose();
		}

		private void SetNewGender(Gender newGender)
		{
			UpdateLifeExpectancy();
		}

		private void SetNewBirthDate(JulianDay BirthDate)
		{
			UpdateLifeExpectancy();
		}

		// Latest database is at: http://www.who.int/whosis/database/life_tables/life_tables.cfm
		internal void UpdateLifeExpectancy()
		{
			object focusControl = FocusManager.GetFocusedElement();
			if (focusControl == null)
				return;

			uint days = DateTime.Now.ToJulian() - _BirthDate;
			double age = JulianDate.DaysToYears(days);

			LifeExpectancySqlQuery query = new LifeExpectancySqlQuery(_Gender.ToString(), age.Round());
			query.Result += LifeExpectancyQueryResult;
			query.Execute();
		}

		private void LifeExpectancyQueryResult(Collection<string> collection, string error)
		{
			if (error != null)
			{
				MessageBoxEx.ShowError("Web access", error, null);
				return;
			}

			uint days = DateTime.Now.ToJulian() - _BirthDate;
			double age = JulianDate.DaysToYears(days);
			if (collection == null || collection.Count <= 0)
			{
				LifeExpectancy = Math.Min(age, 120);
				return;
			}

			// Note: we must specfiy a culture in the conversion because of the query
			string yearsString = collection.FirstOrDefault<string>();
			if (yearsString == null)
				return;
			double years = Convert.ToDouble(yearsString, CultureInfo.InvariantCulture);
			LifeExpectancy = age + years;
		}

		internal void UpdateIncomeTaxStatusByDate(JulianDay date)
		{
			// At some point, provide a mechanism to set income tax status by date
			// Update the Person's tax status

			// For now, assume that the filing status is the same as the relationship status
			_IncomeTaxStatus.FederalFilingStatus = FilingStatus.Single;
			if (_RelationshipStatus == RelationshipStatus.Married)
				_IncomeTaxStatus.FederalFilingStatus = FilingStatus.Married;
			else
			if (_RelationshipStatus == RelationshipStatus.Single ||
				_RelationshipStatus == RelationshipStatus.DivorcedOrSeparated ||
				_RelationshipStatus == RelationshipStatus.Widowed)
				_IncomeTaxStatus.FederalFilingStatus = FilingStatus.Single;

			// For now, assume that the number of exemptions is the same as the number of dependents
			bool isDependent = (_ParentProfile == null);
			if (isDependent)
				_IncomeTaxStatus.FederalExemptions = 0;
			else
			{
				int numDependents = (_ParentProfile.IncludeDependents ? _ParentProfile.Dependents.Count : 0);
				_IncomeTaxStatus.FederalExemptions = 1 + numDependents;
			}

			_IncomeTaxStatus.StateLocalFilingStatus = _IncomeTaxStatus.FederalFilingStatus;
			_IncomeTaxStatus.StateLocalExemptions = _IncomeTaxStatus.FederalExemptions;
			_IncomeTaxStatus.Blind = 0;
		}
	}

[DataContract]
[XmlRoot]
	public class PersonCollection : ObservableCollection<Person>, IDisposable
	{
		public void Dispose()
		{
			foreach (Person person in this)
				person.Dispose();
			Clear();
		}
	}

[DataContract]
[XmlRoot]
	public enum Gender
	{
		Unknown = -1,
		Female,
		Male,
	}

[DataContract]
[XmlRoot]
	public enum RelationshipStatus
	{
		Unknown = -1,
		Single,
		Married,
		DivorcedOrSeparated,
		Widowed,
	}

[DataContract]
[XmlRoot]
	public enum FilingStatus
	{
		Unknown = -1,
		Single,
		Married,
		HeadOfHousehold,
	}

[DataContract]
[XmlRoot]
	public class IncomeTaxStatus : ViewModelBase
	{
[XmlAttribute]
[DataMember]
	public FilingStatus FederalFilingStatus { get { return _FederalFilingStatus; } set { SetProperty(ref _FederalFilingStatus, value, System.Reflection.MethodBase.GetCurrentMethod().Name); } }
		private FilingStatus _FederalFilingStatus;
[XmlAttribute]
[DataMember]
		public FilingStatus StateLocalFilingStatus { get { return _StateFilingStatus; } set { SetProperty(ref _StateFilingStatus, value, System.Reflection.MethodBase.GetCurrentMethod().Name); } }
		private FilingStatus _StateFilingStatus;
[XmlAttribute]
[DataMember]
		public int FederalExemptions { get { return _FederalExemptions; } set { SetProperty(ref _FederalExemptions, value, System.Reflection.MethodBase.GetCurrentMethod().Name); } }
		private int _FederalExemptions;
[XmlAttribute]
[DataMember]
		public int StateLocalExemptions { get { return _StateExemptions; } set { SetProperty(ref _StateExemptions, value, System.Reflection.MethodBase.GetCurrentMethod().Name); } }
		private int _StateExemptions;
[XmlAttribute]
[DataMember]
		public int Blind { get { return _Blind; } set { SetProperty(ref _Blind, value, System.Reflection.MethodBase.GetCurrentMethod().Name); } }
		private int _Blind;
	}

[DataContract]
[XmlRoot]
	public class Location : ViewModelBase
	{
[XmlAttribute]
[DataMember]
	public string CountryCode { get { return _CountryCode; } set { SetProperty(ref _CountryCode, value, System.Reflection.MethodBase.GetCurrentMethod().Name); } }
		private string _CountryCode;
[XmlAttribute]
[DataMember]
		public string CountryName { get { return _CountryName; } set { SetProperty(ref _CountryName, value, System.Reflection.MethodBase.GetCurrentMethod().Name); } }
		private string _CountryName;
[XmlAttribute]
[DataMember]
		public string PostalCode { get { return _PostalCode; } set { SetProperty(ref _PostalCode, value, System.Reflection.MethodBase.GetCurrentMethod().Name, UserChangedPostalCode, ValidatePostalCode); } }
		private string _PostalCode;
[XmlAttribute]
[DataMember]
		// City, StateOrSubCountryCode, Latitude, Longitude, and Text will be computed from PostalCode
		public string City { get { return _City; } set { SetProperty(ref _City, value, System.Reflection.MethodBase.GetCurrentMethod().Name); } }
		private string _City; // Should change over time
[XmlAttribute]
[DataMember]
		public string StateOrSubCountryCode { get { return _StateOrSubCountryCode; } set { SetProperty(ref _StateOrSubCountryCode, value, System.Reflection.MethodBase.GetCurrentMethod().Name); } }
		private string _StateOrSubCountryCode; // Should change over time
[XmlAttribute]
[DataMember]
		public string Latitude { get { return _Latitude; } set { SetProperty(ref _Latitude, value, System.Reflection.MethodBase.GetCurrentMethod().Name); } }
		private string _Latitude;
[XmlAttribute]
[DataMember]
		public string Longitude { get { return _Longitude; } set { SetProperty(ref _Longitude, value, System.Reflection.MethodBase.GetCurrentMethod().Name); } }
		private string _Longitude;
[XmlAttribute]
[DataMember]
		public string Text { get { return _Text; } set { SetProperty(ref _Text, value, System.Reflection.MethodBase.GetCurrentMethod().Name); } }
		private string _Text;

		public override void Dispose()
		{
			base.Dispose();
		}

		internal void ValidatePostalCode(string postalCode)
		{
			if (postalCode.Length != 5)
				throw new ArgumentException("Zip code must be 5 digits");
//j			if (postalCode.IsEmpty())
//j				throw new ArgumentException("Zip code cannot be blank");
			if (postalCode.ToInt() == int.MaxValue)
				throw new ArgumentException("Zip code must be numeric");
		}

		internal void UserChangedPostalCode(string postalCode)
		{
			object focusControl = FocusManager.GetFocusedElement();
			if (focusControl == null)
				return;

			_City = null;
			_StateOrSubCountryCode = null;
			_Latitude = null;
			_Longitude = null;
			_CountryCode = null;
			_CountryName = null;
			_Text = null;

			Text = "Working...";

			LocationSqlQuery query = new LocationSqlQuery(_PostalCode);
			query.Result += LocationsQueryResult;
			query.Execute();
		}

		private void LocationsQueryResult(Collection<Location> collection, string error)
		{
			if (error != null)
			{
				MessageBoxEx.ShowError("Web access", error, null);
				return;
			}

			if (collection == null || collection.Count <= 0)
			{
				Text = "Zip code is invalid";
				return;
			}

			Location location = collection.FirstOrDefault<Location>();
			if (location == null)
				return;
			City = location.City;
			StateOrSubCountryCode = location.StateOrSubCountryCode;
			Latitude = location.Latitude;
			Longitude = location.Longitude;
			CountryCode = location.CountryCode;
			Text = string.Format("{0}, {1}, {2}", location.City, location.StateOrSubCountryCode, location.CountryName);
		}
	}

[DataContract]
[XmlRoot]
	public class PersonEx : Person
	{
#if NOTUSED
[XmlAttribute]
[DataMember]
		public OwnershipStatus HousingStatus { get { return _HousingStatus; } set { SetProperty(ref _HousingStatus, value); } }
		private OwnershipStatus _HousingStatus = OwnershipStatus.Unknown; // Should change over time
[XmlAttribute]
[DataMember]
		public OwnershipStatus VehicleStatus { get { return _VehicleStatus; } set { SetProperty(ref _VehicleStatus, value); } }
		private OwnershipStatus _VehicleStatus = OwnershipStatus.Unknown; // Should change over time
[XmlElement]
[DataMember]
		public ObservableCollection<WorkExperience> WorkExperience { get { return _WorkExperience; } set { SetProperty(ref _WorkExperience, value); } }
		private ObservableCollection<WorkExperience> _WorkExperience; // Grows over time - never shrinks - include current job/income?
[XmlElement]
[DataMember]
		public ObservableCollection<LifeExperience> LifeExperience { get { return _LifeExperience; } set { SetProperty(ref _LifeExperience, value); } }
		private ObservableCollection<LifeExperience> _LifeExperience; // Grows over time - never shrinks - travel, etc.
[XmlElement]
[DataMember]
		public ObservableCollection<EducationTraining> EducationTraining { get { return _EducationTraining; } set { SetProperty(ref _EducationTraining, value); } }
		private ObservableCollection<EducationTraining> _EducationTraining; // Grows over time - never shrinks - HS, college, post-grad
[XmlElement]
[DataMember]
		public ObservableCollection<HealthIssues> HealthIssues { get { return _HealthIssues; } set { SetProperty(ref _HealthIssues, value); } }
		private ObservableCollection<HealthIssues> _HealthIssues; // Should change over time, although some conditions persist
[XmlElement]
[DataMember]
		public ObservableCollection<PermDisabilities> PermDisabilities { get { return _PermDisabilities; } set { SetProperty(ref _PermDisabilities, value); } }
		private ObservableCollection<PermDisabilities> _PermDisabilities; // Grows over time - never shrinks - 
[XmlElement]
[DataMember]
		public ObservableCollection<Interests> Interests { get { return _Interests; } set { SetProperty(ref _Interests, value); } }
		private ObservableCollection<Interests> _Interests; // Should change over time - what I would do if I didn't have to work
[XmlElement]
[DataMember]
		public ObservableCollection<PersonalTraits> PersonalTraits { get { return _PersonalTraits; } set { SetProperty(ref _PersonalTraits, value); } }
		private ObservableCollection<PersonalTraits> _PersonalTraits; // Should change over time, although some conditions were persist - vegatarian, saver, spender, want kids, charitable, drinker (None, Light, Moderate, Heavy), smoker (None, Light, Moderate, Heavy), etc.
[XmlElement]
[DataMember]
		public ObservableCollection<Possessions> Possessions { get { return _Possessions; } set { SetProperty(ref _Possessions, value); } }
		private ObservableCollection<Possessions> _Possessions; // Should change over time - clothing, furniture - Can be estimated from How I Live - Need to be Stored, Maintained, Replaced
#endif
	}

#if NOTUSED
[DataContract]
[XmlRoot]
	public class WorkExperience
	{
[XmlAttribute]
[DataMember]
		public string Name;
	}

[DataContract]
[XmlRoot]
	public class LifeExperience
	{
[XmlAttribute]
[DataMember]
		public string Name;
	}

[DataContract]
[XmlRoot]
	public class EducationTraining
	{
[XmlAttribute]
[DataMember]
		public string Name;
	}

[DataContract]
[XmlRoot]
	public class HealthIssues
	{
[XmlAttribute]
[DataMember]
		public string Name;
	}

[DataContract]
[XmlRoot]
	public class PermDisabilities
	{
[XmlAttribute]
[DataMember]
		public string Name;
	}

[DataContract]
[XmlRoot]
	public class Interests
	{
[XmlAttribute]
[DataMember]
		public string Name;
	}

[DataContract]
[XmlRoot]
	public class PersonalTraits
	{
[XmlAttribute]
[DataMember]
		public string Name;
	}

[DataContract]
[XmlRoot]
	public class Possessions
	{
[XmlAttribute]
[DataMember]
		public string Name;
	}

[DataContract]
[XmlRoot]
	public enum OwnershipStatus
	{
		Unknown = -1,
		Own,
		Rent,
		Lease,
		Borrow,
	}
#endif
	public class PersonNumToIndex : IValueConverter
	{
		public object Convert(object value, Type targetType, object parameter, CultureInfo culture)
		{
			if (value == null) // Special case
				return null;
			if (!value.GetType().IsValueType)
				throw new Exception(string.Format("'{0}' is not a value", value));
			if (!targetType.IsValueType)
				throw new Exception(string.Format("'{0}' is not a value", targetType));

			int personNum = System.Convert.ToInt32(value);
			Profile profile = App.Model.ProfileHolder.Profile;
			int lastIndex = 1 + (profile.IncludeSpouse ? 1 : 0) + (profile.IncludeDependents ? profile.Dependents.Count : 0);

			if (personNum == 0)
				return 0;

			if (personNum == 1)
				return (profile.IncludeSpouse ? 1 : lastIndex);

			if (personNum >= 2 && personNum <= 15)
				return (profile.IncludeDependents ? personNum - (profile.IncludeSpouse ? 0 : 1) : lastIndex);

			return null;
		}

		public object ConvertBack(object value, Type targetType, object parameter, CultureInfo culture)
		{
			if (value == null) // Special case
				return null;
			if (!value.GetType().IsValueType)
				throw new Exception(string.Format("'{0}' is not a value", value));
			if (!targetType.IsValueType)
				throw new Exception(string.Format("'{0}' is not a value", targetType));

			// During data binding de-referencing, -1 values are passed in, 
			// and we have no choice but to return a null value
			int index = System.Convert.ToInt32(value);
			if (index < 0) // Special case
				return null;

			Profile profile = App.Model.ProfileHolder.Profile;

			if (index == 0)
				return 0;

			if (!profile.IncludeSpouse)
				index++;

			if (index == 1)
				return 1;

			if (!profile.IncludeDependents)
				index += 15;

			if (index >= 2 && index <= 15)
				return index;

			return null;
		}
	}

	public class PersonNumToList : IValueConverter
	{
		public object Convert(object value, Type targetType, object parameter, CultureInfo culture)
		{
			if (value == null) // Special case
				return null;
			if (!value.GetType().IsValueType)
				throw new Exception(string.Format("'{0}' is not a value", value));

			int personNum = System.Convert.ToInt32(value);
			bool personFound = false;

			List<string> enumList = new List<string>();
			Profile profile = App.Model.ProfileHolder.Profile;

			string firstName = (!profile.Me.FirstName.IsEmpty() ? profile.Me.FirstName : "Me");
			enumList.Add(firstName);
			if (personNum == 0)
				personFound = true;

			if (profile.IncludeSpouse)
			{
				firstName = (!profile.Spouse.FirstName.IsEmpty() ? profile.Spouse.FirstName : "Spouse");
				enumList.Add(firstName);
				if (personNum == 1)
					personFound = true;
			}

			if (profile.IncludeDependents)
			{
				int dependent = 0;
				foreach (Person person in profile.Dependents)
				{
					dependent++;
					firstName = (!person.FirstName.IsEmpty() ? person.FirstName : string.Format("Dep{0}", dependent));
					enumList.Add(firstName);
					if (personNum == 1 + dependent)
						personFound = true;
				}
			}

			// Add an extra entries at the bottom
			if (!personFound)
			{
				if (personNum == 1)
					firstName = "Spouse";
				else
					firstName = string.Format("Dep{0}", personNum - 1);

				enumList.Add(firstName);
			}

			return enumList;
		}

		public object ConvertBack(object value, Type targetType, object parameter, CultureInfo culture)
		{
			throw new NotSupportedException();
		}
	}
}
