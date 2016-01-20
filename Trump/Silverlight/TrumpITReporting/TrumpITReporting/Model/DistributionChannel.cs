using System;
using System.Collections;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Linq;
using System.Xml.Linq;
using ClassLibrary;

namespace TrumpITReporting
{
	public enum DeliveryType
	{
		Unknown = -1,
		DirectMail = 10,
		Email = 20,
		SocialNetwork = 30,
	}

	public class DistributionChannelCollection : ObservableCollection<DistributionChannel>, IDisposable
	{
		public void Dispose()
		{
			foreach (DistributionChannel item in this)
				item.Dispose();
			Clear();
		}

		internal void Initialize()
		{
		}
	}

	public class EmailChannel : DistributionChannel
	{
	}

	public class DirectMailChannel : DistributionChannel
	{
		public override double Cost { get { return PostcardCost + PostageCost; } set {} }

		public double PostcardCost { get { return _PostcardCost; } set { SetProperty(ref _PostcardCost, value); } }
		private double _PostcardCost;

		public double PostageCost { get { return _PostageCost; } set { SetProperty(ref _PostageCost, value); } }
		private double _PostageCost;
	}

	public class DistributionChannel : NotifyPropertyChanges
	{
		public DeliveryType DeliveryType { get { return _DeliveryType; } set { SetProperty(ref _DeliveryType, value); } }
		private DeliveryType _DeliveryType = DeliveryType.Unknown;

		public virtual double Cost { get { return 0; } set {} }

		public bool Sent { get { return _Sent; } set { SetProperty(ref _Sent, value); } }
		private bool _Sent;

		public bool Viewed { get { return _Viewed; } set { SetProperty(ref _Viewed, value); } }
		private bool _Viewed;

		public bool Opened { get { return _Opened; } set { SetProperty(ref _Opened, value); } }
		private bool _Opened;

		public override void Dispose()
		{
			base.Dispose();
		}

		public static DistributionChannel Create(string name)
		{
			DistributionChannel item = new DistributionChannel();
			return item;
		}
	}
}
