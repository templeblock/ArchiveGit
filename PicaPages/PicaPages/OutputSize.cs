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
using System.Runtime.Serialization;
using System.Diagnostics;
using System.Collections.Generic;
using System.ComponentModel;

namespace PicaPages
{
	[DebuggerStepThrough, DataContract(Name="OutputSize", Namespace="http://schemas.datacontract.org/2004/07/Asi.Inkubuzz")]
	public class OutputSize : INotifyPropertyChanged
	{
		// Fields
		private List<Background> BackgroundsField;
		private List<ContentLayoutTemplate> ContentLayoutTemplatesField;
		private List<CoverType> CoverTypesField;
		private short OutputSizeIDField;
		private List<OutputType> OutputTypesField;
		private List<Project> ProjectsField;
		private List<ProjectTheme> ProjectThemesField;
		private decimal TemplateHeightField;
		private decimal TemplateWidthField;
		private string TitleField;

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
		[DataMember(EmitDefaultValue=false, Order=4)]
		public List<Background> Backgrounds
		{
			get
			{
				return this.BackgroundsField;
			}
			set
			{
				if (!object.ReferenceEquals(this.BackgroundsField, value))
				{
					this.BackgroundsField = value;
					this.RaisePropertyChanged("Backgrounds");
				}
			}
		}

		[DataMember(EmitDefaultValue=false, Order=5)]
		public List<ContentLayoutTemplate> ContentLayoutTemplates
		{
			get
			{
				return this.ContentLayoutTemplatesField;
			}
			set
			{
				if (!object.ReferenceEquals(this.ContentLayoutTemplatesField, value))
				{
					this.ContentLayoutTemplatesField = value;
					this.RaisePropertyChanged("ContentLayoutTemplates");
				}
			}
		}

		[DataMember(EmitDefaultValue=false, Order=9)]
		public List<CoverType> CoverTypes
		{
			get
			{
				return this.CoverTypesField;
			}
			set
			{
				if (!object.ReferenceEquals(this.CoverTypesField, value))
				{
					this.CoverTypesField = value;
					this.RaisePropertyChanged("CoverTypes");
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

		[DataMember(EmitDefaultValue=false, Order=6)]
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

		[DataMember(EmitDefaultValue=false, Order=7)]
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

		[DataMember(EmitDefaultValue=false, Order=8)]
		public List<ProjectTheme> ProjectThemes
		{
			get
			{
				return this.ProjectThemesField;
			}
			set
			{
				if (!object.ReferenceEquals(this.ProjectThemesField, value))
				{
					this.ProjectThemesField = value;
					this.RaisePropertyChanged("ProjectThemes");
				}
			}
		}

		[DataMember(Order=2)]
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

		[DataMember(Order=3)]
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
	}
}
