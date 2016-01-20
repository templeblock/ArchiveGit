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

namespace PicaPages
{
	[DataContract(Name="Binary", Namespace="http://schemas.datacontract.org/2004/07/System.Data.Linq"), DebuggerStepThrough]
	public class Binary : INotifyPropertyChanged
	{
		// Fields
		private byte[] BytesField;

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
		public byte[] Bytes
		{
			get
			{
				return this.BytesField;
			}
			set
			{
				if (!object.ReferenceEquals(this.BytesField, value))
				{
					this.BytesField = value;
					this.RaisePropertyChanged("Bytes");
				}
			}
		}
	}
}
