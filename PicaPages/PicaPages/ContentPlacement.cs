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
using System.ComponentModel;
using System.Xml.Linq;

namespace PicaPages
{
	[DataContract(Name="ContentPlacement", Namespace="http://schemas.datacontract.org/2004/07/Asi.Inkubuzz"), DebuggerStepThrough]
	public class ContentPlacement : INotifyPropertyChanged
	{
		// Fields
		private Binary ChangeStampField;
		private Guid? ContentElementIDField;
		private Guid ContentLayoutIDField;
		private Guid ContentPlacementIDField;
		private short ContentTypeIDField;
		private XElement ContentXMLField;
		private XElement FormatXMLField;
		private string ImageTokenField;
		private string TemplateKeyField;

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
		[DataMember(Order=8)]
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

		[DataMember(Order=1)]
		public Guid? ContentElementID
		{
			get
			{
				return this.ContentElementIDField;
			}
			set
			{
				if (!this.ContentElementIDField.Equals(value))
				{
					this.ContentElementIDField = value;
					this.RaisePropertyChanged("ContentElementID");
				}
			}
		}

		[DataMember(Order=2)]
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

		[DataMember]
		public Guid ContentPlacementID
		{
			get
			{
				return this.ContentPlacementIDField;
			}
			set
			{
				if (!this.ContentPlacementIDField.Equals(value))
				{
					this.ContentPlacementIDField = value;
					this.RaisePropertyChanged("ContentPlacementID");
				}
			}
		}

		[DataMember(Order=3)]
		public short ContentTypeID
		{
			get
			{
				return this.ContentTypeIDField;
			}
			set
			{
				if (!this.ContentTypeIDField.Equals(value))
				{
					this.ContentTypeIDField = value;
					this.RaisePropertyChanged("ContentTypeID");
				}
			}
		}

		[DataMember(Order=5)]
		public XElement ContentXML
		{
			get
			{
				return this.ContentXMLField;
			}
			set
			{
				if (!object.ReferenceEquals(this.ContentXMLField, value))
				{
					this.ContentXMLField = value;
					this.RaisePropertyChanged("ContentXML");
				}
			}
		}

		[DataMember(Order=7)]
		public XElement FormatXML
		{
			get
			{
				return this.FormatXMLField;
			}
			set
			{
				if (!object.ReferenceEquals(this.FormatXMLField, value))
				{
					this.FormatXMLField = value;
					this.RaisePropertyChanged("FormatXML");
				}
			}
		}

		[DataMember(Order=4)]
		public string ImageToken
		{
			get
			{
				return this.ImageTokenField;
			}
			set
			{
				if (!object.ReferenceEquals(this.ImageTokenField, value))
				{
					this.ImageTokenField = value;
					this.RaisePropertyChanged("ImageToken");
				}
			}
		}

		[DataMember(Order=6)]
		public string TemplateKey
		{
			get
			{
				return this.TemplateKeyField;
			}
			set
			{
				if (!object.ReferenceEquals(this.TemplateKeyField, value))
				{
					this.TemplateKeyField = value;
					this.RaisePropertyChanged("TemplateKey");
				}
			}
		}
	}
}
