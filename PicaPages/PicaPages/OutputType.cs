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
using System.ComponentModel;
using System.Runtime.Serialization;
using System.Diagnostics;

namespace PicaPages
{
	[DataContract(Name="OutputType", Namespace="http://schemas.datacontract.org/2004/07/Asi.Inkubuzz"), DebuggerStepThrough]
	public class OutputType : INotifyPropertyChanged
	{
		// Fields
		private List<AvailableShippingMethod> AvailableShippingMethodsField;
		private short CoverTypeIDField;
		private string DescriptionField;
		private List<Order> OrdersField;
		private List<OutputPrice> OutputPricesField;
		private short OutputSizeIDField;
		private OutputTypeCoverDimension OutputTypeCoverDimensionField;
		private List<OutputTypeCoverSpineDimension> OutputTypeCoverSpineDimensionsField;
		private short OutputTypeIDField;
		private OutputTypePageDimension OutputTypePageDimensionField;
		private string PublisherCodeField;
		private short PublishingVendorIDField;
		private List<ShippingPrice> ShippingPricesField;

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
		[DataMember(EmitDefaultValue=false, Order=8)]
		public List<AvailableShippingMethod> AvailableShippingMethods
		{
			get
			{
				return this.AvailableShippingMethodsField;
			}
			set
			{
				if (!object.ReferenceEquals(this.AvailableShippingMethodsField, value))
				{
					this.AvailableShippingMethodsField = value;
					this.RaisePropertyChanged("AvailableShippingMethods");
				}
			}
		}

		[DataMember(Order=5)]
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

		[DataMember(EmitDefaultValue=false, Order=11)]
		public List<Order> Orders
		{
			get
			{
				return this.OrdersField;
			}
			set
			{
				if (!object.ReferenceEquals(this.OrdersField, value))
				{
					this.OrdersField = value;
					this.RaisePropertyChanged("Orders");
				}
			}
		}

		[DataMember(EmitDefaultValue=false, Order=10)]
		public List<OutputPrice> OutputPrices
		{
			get
			{
				return this.OutputPricesField;
			}
			set
			{
				if (!object.ReferenceEquals(this.OutputPricesField, value))
				{
					this.OutputPricesField = value;
					this.RaisePropertyChanged("OutputPrices");
				}
			}
		}

		[DataMember(Order=4)]
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

		[DataMember(EmitDefaultValue=false, Order=12)]
		public OutputTypeCoverDimension OutputTypeCoverDimension
		{
			get
			{
				return this.OutputTypeCoverDimensionField;
			}
			set
			{
				if (!object.ReferenceEquals(this.OutputTypeCoverDimensionField, value))
				{
					this.OutputTypeCoverDimensionField = value;
					this.RaisePropertyChanged("OutputTypeCoverDimension");
				}
			}
		}

		[DataMember(EmitDefaultValue=false, Order=6)]
		public List<OutputTypeCoverSpineDimension> OutputTypeCoverSpineDimensions
		{
			get
			{
				return this.OutputTypeCoverSpineDimensionsField;
			}
			set
			{
				if (!object.ReferenceEquals(this.OutputTypeCoverSpineDimensionsField, value))
				{
					this.OutputTypeCoverSpineDimensionsField = value;
					this.RaisePropertyChanged("OutputTypeCoverSpineDimensions");
				}
			}
		}

		[DataMember]
		public short OutputTypeID
		{
			get
			{
				return this.OutputTypeIDField;
			}
			set
			{
				if (!this.OutputTypeIDField.Equals(value))
				{
					this.OutputTypeIDField = value;
					this.RaisePropertyChanged("OutputTypeID");
				}
			}
		}

		[DataMember(EmitDefaultValue=false, Order=7)]
		public OutputTypePageDimension OutputTypePageDimension
		{
			get
			{
				return this.OutputTypePageDimensionField;
			}
			set
			{
				if (!object.ReferenceEquals(this.OutputTypePageDimensionField, value))
				{
					this.OutputTypePageDimensionField = value;
					this.RaisePropertyChanged("OutputTypePageDimension");
				}
			}
		}

		[DataMember(Order=2)]
		public string PublisherCode
		{
			get
			{
				return this.PublisherCodeField;
			}
			set
			{
				if (!object.ReferenceEquals(this.PublisherCodeField, value))
				{
					this.PublisherCodeField = value;
					this.RaisePropertyChanged("PublisherCode");
				}
			}
		}

		[DataMember]
		public short PublishingVendorID
		{
			get
			{
				return this.PublishingVendorIDField;
			}
			set
			{
				if (!this.PublishingVendorIDField.Equals(value))
				{
					this.PublishingVendorIDField = value;
					this.RaisePropertyChanged("PublishingVendorID");
				}
			}
		}

		[DataMember(EmitDefaultValue=false, Order=9)]
		public List<ShippingPrice> ShippingPrices
		{
			get
			{
				return this.ShippingPricesField;
			}
			set
			{
				if (!object.ReferenceEquals(this.ShippingPricesField, value))
				{
					this.ShippingPricesField = value;
					this.RaisePropertyChanged("ShippingPrices");
				}
			}
		}
	}
}
