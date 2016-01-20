using System;
using System.Collections;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Linq;
using System.Xml.Linq;
using ClassLibrary;

namespace TrumpITReporting
{
	public enum ProductType
	{
		Opportunities,
		WeightLoss,
		SilhouetteSolutions,
		IdealHealth,
		QuickStiks,
		SnazzleSnaxx,
	}

	public class ProductCollection : ObservableCollection<Product>, IDisposable
	{
		public void Dispose()
		{
			foreach (Product item in this)
				item.Dispose();
			Clear();
		}

		internal void Initialize()
		{
		}
	}

	public class Product : NotifyPropertyChanges
	{
		public string Name { get { return _Name; } set { SetProperty(ref _Name, value); } }
		private string _Name;

		public double Cost { get { return _Cost; } set { SetProperty(ref _Cost, value); } }
		private double _Cost;

		public override void Dispose()
		{
			base.Dispose();
		}

		public static Product Create(string name)
		{
			Product item = new Product();
			return item;
		}
	}
}
