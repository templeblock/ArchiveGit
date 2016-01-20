using System;
using System.Net;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Documents;
using System.Windows.Ink;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Animation;
using System.Windows.Shapes;
using System.ComponentModel;
using System.Runtime.Serialization;
using System.Diagnostics;

namespace PicaPages
{
	[DataContract(Name="cs_User", Namespace="http://schemas.datacontract.org/2004/07/Asi.Inkubuzz"), DebuggerStepThrough]
	public class cs_User : INotifyPropertyChanged
	{
		// Fields
		private string AppUserTokenField;
		private DateTime CreateDateField;
		private string EmailField;
		private bool ForceLoginField;
		private bool IsAnonymousField;
		private bool? IsApprovedField;
		private string LastActionField;
		private DateTime LastActivityField;
		private Guid MembershipIDField;
		private short UserAccountStatusField;
		private int UserIDField;
		private string UserNameField;

		// Events
		public event PropertyChangedEventHandler PropertyChanged;

		// Methods
		protected void RaisePropertyChanged(string propertyName)
		{
			PropertyChangedEventHandler propertyChanged = this.PropertyChanged;
			if (propertyChanged != null)
			{
				propertyChanged(this, new PropertyChangedEventArgs(propertyName));
			}
		}

		// Properties
		[DataMember(Order=4)]
		public string AppUserToken
		{
			get
			{
				return this.AppUserTokenField;
			}
			set
			{
				if (!object.ReferenceEquals(this.AppUserTokenField, value))
				{
					this.AppUserTokenField = value;
					this.RaisePropertyChanged("AppUserToken");
				}
			}
		}

		[DataMember(Order=11)]
		public DateTime CreateDate
		{
			get
			{
				return this.CreateDateField;
			}
			set
			{
				if (!this.CreateDateField.Equals(value))
				{
					this.CreateDateField = value;
					this.RaisePropertyChanged("CreateDate");
				}
			}
		}

		[DataMember(Order=8)]
		public string Email
		{
			get
			{
				return this.EmailField;
			}
			set
			{
				if (!object.ReferenceEquals(this.EmailField, value))
				{
					this.EmailField = value;
					this.RaisePropertyChanged("Email");
				}
			}
		}

		[DataMember(Order=2)]
		public bool ForceLogin
		{
			get
			{
				return this.ForceLoginField;
			}
			set
			{
				if (!this.ForceLoginField.Equals(value))
				{
					this.ForceLoginField = value;
					this.RaisePropertyChanged("ForceLogin");
				}
			}
		}

		[DataMember(Order=10)]
		public bool IsAnonymous
		{
			get
			{
				return this.IsAnonymousField;
			}
			set
			{
				if (!this.IsAnonymousField.Equals(value))
				{
					this.IsAnonymousField = value;
					this.RaisePropertyChanged("IsAnonymous");
				}
			}
		}

		[DataMember(Order=9)]
		public bool? IsApproved
		{
			get
			{
				return this.IsApprovedField;
			}
			set
			{
				if (!this.IsApprovedField.Equals(value))
				{
					this.IsApprovedField = value;
					this.RaisePropertyChanged("IsApproved");
				}
			}
		}

		[DataMember(Order=6)]
		public string LastAction
		{
			get
			{
				return this.LastActionField;
			}
			set
			{
				if (!object.ReferenceEquals(this.LastActionField, value))
				{
					this.LastActionField = value;
					this.RaisePropertyChanged("LastAction");
				}
			}
		}

		[DataMember(Order=5)]
		public DateTime LastActivity
		{
			get
			{
				return this.LastActivityField;
			}
			set
			{
				if (!this.LastActivityField.Equals(value))
				{
					this.LastActivityField = value;
					this.RaisePropertyChanged("LastActivity");
				}
			}
		}

		[DataMember]
		public Guid MembershipID
		{
			get
			{
				return this.MembershipIDField;
			}
			set
			{
				if (!this.MembershipIDField.Equals(value))
				{
					this.MembershipIDField = value;
					this.RaisePropertyChanged("MembershipID");
				}
			}
		}

		[DataMember(Order=3)]
		public short UserAccountStatus
		{
			get
			{
				return this.UserAccountStatusField;
			}
			set
			{
				if (!this.UserAccountStatusField.Equals(value))
				{
					this.UserAccountStatusField = value;
					this.RaisePropertyChanged("UserAccountStatus");
				}
			}
		}

		[DataMember]
		public int UserID
		{
			get
			{
				return this.UserIDField;
			}
			set
			{
				if (!this.UserIDField.Equals(value))
				{
					this.UserIDField = value;
					this.RaisePropertyChanged("UserID");
				}
			}
		}

		[DataMember(Order=7)]
		public string UserName
		{
			get
			{
				return this.UserNameField;
			}
			set
			{
				if (!object.ReferenceEquals(this.UserNameField, value))
				{
					this.UserNameField = value;
					this.RaisePropertyChanged("UserName");
				}
			}
		}
	}
}
