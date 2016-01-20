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
using System.Collections.Generic;
using System.Runtime.Serialization;
using System.Diagnostics;

namespace PicaPages
{
	[DebuggerStepThrough, DataContract(Name="CoverType", Namespace="http://schemas.datacontract.org/2004/07/Asi.Inkubuzz")]
	public class CoverType : INotifyPropertyChanged
	{
		// Fields
		private short CoverTypeIDField;
		private string DescriptionField;
		private bool IsEnabledField;
		private short OutputSizeIDField;
		private List<OutputType> OutputTypesField;
		private List<Project> ProjectsField;
		private decimal TemplateHeightField;
		private decimal TemplateWidthField;
		private string TitleField;
		private int TroughWidthField;

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
		[DataMember]
		public short CoverTypeID
		{
			get
			{
				return this.CoverTypeIDField;
			}
			set
			{
				if (!this.CoverTypeIDField.Equals(value))
				{
					this.CoverTypeIDField = value;
					this.RaisePropertyChanged("CoverTypeID");
				}
			}
		}

		[DataMember(Order=3)]
		public string Description
		{
			get
			{
				return this.DescriptionField;
			}
			set
			{
				if (!object.ReferenceEquals(this.DescriptionField, value))
				{
					this.DescriptionField = value;
					this.RaisePropertyChanged("Description");
				}
			}
		}

		[DataMember(Order=4)]
		public bool IsEnabled
		{
			get
			{
				return this.IsEnabledField;
			}
			set
			{
				if (!this.IsEnabledField.Equals(value))
				{
					this.IsEnabledField = value;
					this.RaisePropertyChanged("IsEnabled");
				}
			}
		}

		[DataMember]
		public short OutputSizeID
		{
			get
			{
				return this.OutputSizeIDField;
			}
			set
			{
				if (!this.OutputSizeIDField.Equals(value))
				{
					this.OutputSizeIDField = value;
					this.RaisePropertyChanged("OutputSizeID");
				}
			}
		}

		[DataMember(EmitDefaultValue=false, Order=8)]
		public List<OutputType> OutputTypes
		{
			get
			{
				return this.OutputTypesField;
			}
			set
			{
				if (!object.ReferenceEquals(this.OutputTypesField, value))
				{
					this.OutputTypesField = value;
					this.RaisePropertyChanged("OutputTypes");
				}
			}
		}

		[DataMember(EmitDefaultValue=false, Order=9)]
		public List<Project> Projects
		{
			get
			{
				return this.ProjectsField;
			}
			set
			{
				if (!object.ReferenceEquals(this.ProjectsField, value))
				{
					this.ProjectsField = value;
					this.RaisePropertyChanged("Projects");
				}
			}
		}

		[DataMember(Order=5)]
		public decimal TemplateHeight
		{
			get
			{
				return this.TemplateHeightField;
			}
			set
			{
				if (!this.TemplateHeightField.Equals(value))
				{
					this.TemplateHeightField = value;
					this.RaisePropertyChanged("TemplateHeight");
				}
			}
		}

		[DataMember(Order=6)]
		public decimal TemplateWidth
		{
			get
			{
				return this.TemplateWidthField;
			}
			set
			{
				if (!this.TemplateWidthField.Equals(value))
				{
					this.TemplateWidthField = value;
					this.RaisePropertyChanged("TemplateWidth");
				}
			}
		}

		[DataMember]
		public string Title
		{
			get
			{
				return this.TitleField;
			}
			set
			{
				if (!object.ReferenceEquals(this.TitleField, value))
				{
					this.TitleField = value;
					this.RaisePropertyChanged("Title");
				}
			}
		}

		[DataMember(Order=7)]
		public int TroughWidth
		{
			get
			{
				return this.TroughWidthField;
			}
			set
			{
				if (!this.TroughWidthField.Equals(value))
				{
					this.TroughWidthField = value;
					this.RaisePropertyChanged("TroughWidth");
				}
			}
		}
	}
}
