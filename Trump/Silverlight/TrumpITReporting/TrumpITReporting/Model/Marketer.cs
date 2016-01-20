using System;
using System.Collections;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Linq;
using System.Xml.Linq;
using ClassLibrary;

namespace TrumpITReporting
{
	public class MarketerCollection : ObservableCollection<Marketer>, IDisposable
	{
		public void Dispose()
		{
			foreach (Marketer item in this)
				item.Dispose();
			Clear();
		}

		internal void Initialize()
		{
		}
	}

	public class Marketer : MarketingLead
	{
		public bool SignedUp { get { return _SignedUp; } set { SetProperty(ref _SignedUp, value); } }
		private bool _SignedUp;

		public CampaignCollection Campaigns { get { return _Campaigns; } set { SetProperty(ref _Campaigns, value); } }
		private CampaignCollection _Campaigns = new CampaignCollection();

		public CustomerCollection Customers { get { return _Customers; } set { SetProperty(ref _Customers, value); } }
		private CustomerCollection _Customers = new CustomerCollection();

		public override void Dispose()
		{
			base.Dispose();
		}

		public Marketer()
		{
			base.Type = PersonType.Marketer;
		}

		public static Marketer Create(string name)
		{
			Marketer item = new Marketer();
			return item;
		}
	}
}
