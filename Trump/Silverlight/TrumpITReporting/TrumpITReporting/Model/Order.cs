using System;
using System.Collections;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Linq;
using System.Xml.Linq;
using ClassLibrary;

namespace TrumpITReporting
{
	public class OrderCollection : ObservableCollection<Order>, IDisposable
	{
		public virtual void Dispose()
		{
			foreach (Order item in this)
				item.Dispose();
			Clear();
		}

		internal virtual void Initialize()
		{
		}
	}

	public class Order : NotifyPropertyChanges
	{
		public Product Product { get { return _Product; } set { SetProperty(ref _Product, value); } }
		private Product _Product = new Product();

		public int Quantity { get { return _Quantity; } set { _Quantity = value; } }
		private int _Quantity;

		public override void Dispose()
		{
			base.Dispose();
		}
	}
}
