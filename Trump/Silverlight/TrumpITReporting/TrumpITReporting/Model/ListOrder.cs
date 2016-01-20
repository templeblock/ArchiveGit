using System;
using System.Collections;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Linq;
using System.Xml.Linq;
using ClassLibrary;

namespace TrumpITReporting
{
	public enum ListOrderType
	{
		Personal,
		Purchased,
	}

	public enum ListOrderName
	{
		Divorcees,
		Teachers,
		Contractors,
		MortgageBrokers
	}

	public class ListOrderCollection : OrderCollection
	{
		public override void Dispose()
		{
			foreach (ListOrder item in this)
				item.Dispose();
			Clear();
		}

		internal override void Initialize()
		{
		}
	}

	public class ListOrder : Order
	{
		public override void Dispose()
		{
			base.Dispose();
		}

		public static ListOrder CreateListOrder(string name)
		{
			ListOrder item = new ListOrder();
			return item;
		}
	}
}
