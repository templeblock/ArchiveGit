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
using System.Collections.Generic;
using System.Runtime.Serialization;
using System.Diagnostics;
using System.ComponentModel;

namespace PicaPages
{
	[DataContract(Name="ContentLayout", Namespace="http://schemas.datacontract.org/2004/07/Asi.Inkubuzz"), DebuggerStepThrough]
	public class ContentLayout : INotifyPropertyChanged
	{
		// Fields
		private int BackgroundIDField;
		private Binary ChangeStampField;
		private Guid ContentLayoutIDField;
		private List<ContentPlacement> ContentPlacementsField;
		private int CreatedByField;
		private DateTime CreatedOnField;
		private int LastModifiedByField;
		private DateTime LastModifiedOnField;
		private Guid ProjectIDField;
		private Guid SectionIDField;
		private short SortOrderField;
		private int TemplateIDField;

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
		[DataMember(Order=5)]
		public int BackgroundID
		{
			get
			{
				return this.BackgroundIDField;
			}
			set
			{
				if (!this.BackgroundIDField.Equals(value))
				{
					this.BackgroundIDField = value;
					this.RaisePropertyChanged("BackgroundID");
				}
			}
		}

		[DataMember(Order=6)]
		public Binary ChangeStamp
		{
			get
			{
				return this.ChangeStampField;
			}
			set
			{
				if (!object.ReferenceEquals(this.ChangeStampField, value))
				{
					this.ChangeStampField = value;
					this.RaisePropertyChanged("ChangeStamp");
				}
			}
		}

		[DataMember]
		public Guid ContentLayoutID
		{
			get
			{
				return this.ContentLayoutIDField;
			}
			set
			{
				if (!this.ContentLayoutIDField.Equals(value))
				{
					this.ContentLayoutIDField = value;
					this.RaisePropertyChanged("ContentLayoutID");
				}
			}
		}

		[DataMember(EmitDefaultValue=false, Order=11)]
		public List<ContentPlacement> ContentPlacements
		{
			get
			{
				return this.ContentPlacementsField;
			}
			set
			{
				if (!object.ReferenceEquals(this.ContentPlacementsField, value))
				{
					this.ContentPlacementsField = value;
					this.RaisePropertyChanged("ContentPlacements");
				}
			}
		}

		[DataMember(Order=7)]
		public int CreatedBy
		{
			get
			{
				return this.CreatedByField;
			}
			set
			{
				if (!this.CreatedByField.Equals(value))
				{
					this.CreatedByField = value;
					this.RaisePropertyChanged("CreatedBy");
				}
			}
		}

		[DataMember(Order=8)]
		public DateTime CreatedOn
		{
			get
			{
				return this.CreatedOnField;
			}
			set
			{
				if (!this.CreatedOnField.Equals(value))
				{
					this.CreatedOnField = value;
					this.RaisePropertyChanged("CreatedOn");
				}
			}
		}

		[DataMember(Order=9)]
		public int LastModifiedBy
		{
			get
			{
				return this.LastModifiedByField;
			}
			set
			{
				if (!this.LastModifiedByField.Equals(value))
				{
					this.LastModifiedByField = value;
					this.RaisePropertyChanged("LastModifiedBy");
				}
			}
		}

		[DataMember(Order=10)]
		public DateTime LastModifiedOn
		{
			get
			{
				return this.LastModifiedOnField;
			}
			set
			{
				if (!this.LastModifiedOnField.Equals(value))
				{
					this.LastModifiedOnField = value;
					this.RaisePropertyChanged("LastModifiedOn");
				}
			}
		}

		[DataMember]
		public Guid ProjectID
		{
			get
			{
				return this.ProjectIDField;
			}
			set
			{
				if (!this.ProjectIDField.Equals(value))
				{
					this.ProjectIDField = value;
					this.RaisePropertyChanged("ProjectID");
				}
			}
		}

		[DataMember]
		public Guid SectionID
		{
			get
			{
				return this.SectionIDField;
			}
			set
			{
				if (!this.SectionIDField.Equals(value))
				{
					this.SectionIDField = value;
					this.RaisePropertyChanged("SectionID");
				}
			}
		}

		[DataMember(Order=4)]
		public short SortOrder
		{
			get
			{
				return this.SortOrderField;
			}
			set
			{
				if (!this.SortOrderField.Equals(value))
				{
					this.SortOrderField = value;
					this.RaisePropertyChanged("SortOrder");
				}
			}
		}

		[DataMember]
		public int TemplateID
		{
			get
			{
				return this.TemplateIDField;
			}
			set
			{
				if (!this.TemplateIDField.Equals(value))
				{
					this.TemplateIDField = value;
					this.RaisePropertyChanged("TemplateID");
				}
			}
		}
	}
}
