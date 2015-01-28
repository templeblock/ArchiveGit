using System;
using System.ServiceModel.DomainServices.Server.ApplicationServices;

namespace Money.Web
{
	public partial class User : UserBase
	{
		// Properties stored in the aspnet_Profile table, configured in web.config
		//j Guid UserId - including causes problem in SqlTableProfileProvider when setting properties
		//j DateTime LastUpdatedDate - including causes problem in SqlTableProfileProvider when setting properties
		public bool OptInUpdates { get; set; }
		public int Language { get; set; }
		public string ProfileXml { get; set; }
		public string DocumentXml { get; set; }
		public bool IncludeSpouse { get; set; }
		public bool IncludeDependants { get; set; }
		public string MeFirstName { get; set; }
		public string MeGender { get; set; }
		public int MeBirthDate { get; set; }
		public double MeLifeExpectancy { get; set; }
		public string MeRelationshipStatus { get; set; }
		public string SpouseFirstName { get; set; }
		public string SpouseGender { get; set; }
		public int SpouseBirthDate { get; set; }
		public double SpouseLifeExpectancy { get; set; }
		public string SpouseRelationshipStatus { get; set; }

		// Properties stored in the aspnet_Membership table, mirrored from the MembershipUser object
		[ProfileUsage(IsExcluded=true)] public Guid MemberUserId { get; set; }
		[ProfileUsage(IsExcluded=true)] public string MemberComment { get; set; }
		[ProfileUsage(IsExcluded=true)] public string MemberEmail { get; set; }
		[ProfileUsage(IsExcluded=true)] public bool MemberIsAdmin { get; set; }
		[ProfileUsage(IsExcluded=true)] public bool MemberIsPremium { get; set; }
		//[ProfileUsage(IsExcluded=true)] public bool MemberIsApproved { get; set; }
		//[ProfileUsage(IsExcluded=true)] public bool MemberIsLockedOut { get; set; }
		//[ReadOnly(true)]
		//[ProfileUsage(IsExcluded=true)] public bool MemberIsOnline { get; set; }
		//[ProfileUsage(IsExcluded=true)] public string MemberPasswordQuestion { get; set; }
		//[ProfileUsage(IsExcluded=true)] public string MemberProviderName { get; set; }
		//[ProfileUsage(IsExcluded=true)] public DateTime MemberCreationDate { get; set; }
		//[ProfileUsage(IsExcluded=true)] public DateTime MemberLastLockoutDate { get; set; }
		//[ProfileUsage(IsExcluded=true)] public DateTime MemberLastLoginDate { get; set; }
		//[ProfileUsage(IsExcluded=true)] public DateTime MemberLastPasswordChangedDate { get; set; }
	}
}
