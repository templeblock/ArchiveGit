using System;
using System.Collections;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Linq;
using System.Xml.Linq;
using ClassLibrary;

namespace TrumpITReporting
{
	public class CustomerCollection : ObservableCollection<Customer>, IDisposable
	{
		public void Dispose()
		{
			foreach (Customer item in this)
				item.Dispose();
			Clear();
		}

		internal void Initialize()
		{
		}
	}

	public class Customer : MarketingLead
	{
		public override void Dispose()
		{
			base.Dispose();
		}

		public Customer()
		{
			base.Type = PersonType.Customer;
		}

		public static Customer Create(string name)
		{
			Customer item = new Customer();
			return item;
		}
	}
}
