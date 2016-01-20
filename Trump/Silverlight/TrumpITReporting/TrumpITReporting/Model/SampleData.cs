using System;
using System.Collections.Generic;

namespace TrumpITReporting
{
	public class SampleData
	{
		public static readonly Marketer[] Marketers = {
			Marketer.Create("Stock Purchase"),
			Marketer.Create("401k Contribution"),
		};

		public static readonly Marketer Me = new Marketer()
		{
			SignedUp = true,
			Customers = new CustomerCollection() { },
		};

	}
}
