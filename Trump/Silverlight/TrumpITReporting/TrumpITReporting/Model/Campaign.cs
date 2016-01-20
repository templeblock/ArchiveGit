using System;
using System.Collections.ObjectModel;
using ClassLibrary;

namespace TrumpITReporting
{
	public enum CampaignCategory
	{
		Unknown = -1,
		Trilogy3D,
		TrumpTowers,
		Opus,
	}

	public class CampaignCollection : ObservableCollection<Campaign>, IDisposable
	{
		public void Dispose()
		{
			foreach (Campaign item in this)
				item.Dispose();
			Clear();
		}

		internal void Initialize()
		{
		}
	}

	public class Campaign : NotifyPropertyChanges
	{
		public string Name { get { return _Name; } set { SetProperty(ref _Name, value); } }
		private string _Name;

		public CampaignCategory Category { get { return _Category; } set { SetProperty(ref _Category, value); } }
		private CampaignCategory _Category = CampaignCategory.Unknown;

		public double Profit { get { return Revenue - Cost; } }

		public double Cost
		{
			get
			{
				double cost = 0;
				cost += EmailChannel.Cost;
				cost += DirectMailChannel.Cost;
				cost += RecipientList.ListOrder.Product.Cost;
				return 0;
			}
		}

		public double Revenue
		{
			get
			{
				double revenue = 0;
				revenue += MonthlyProductCommission;
				revenue += WeeklyFastStartBonus;
				revenue += MonthlySystemCommission;
				revenue += CumulativeAdvertisingBonus;
				revenue += ACM;
				return 0;
			}
		}

		public double NumNewMarketers
		{
			get
			{
				double count = 0;
				count += RecipientList.NewMarketers;
				return 0;
			}
		}

		public double NumNewCustomer
		{
			get
			{
				double count = 0;
				count += RecipientList.NewCustomers;
				return 0;
			}
		}

		public int NumNamesPurchased
		{
			get
			{
				double count = 0;
				count += RecipientList.ListOrder.Quantity;
				return 0;
			}
		}

		public double MonthlyProductCommission { get { return _MonthlyProductCommission; } set { SetProperty(ref _MonthlyProductCommission, value); } }
		private double _MonthlyProductCommission;

		public double WeeklyFastStartBonus { get { return _WeeklyFastStartBonus; } set { SetProperty(ref _WeeklyFastStartBonus, value); } }
		private double _WeeklyFastStartBonus;

		public double MonthlySystemCommission { get { return _MonthlySystemCommission; } set { SetProperty(ref _MonthlySystemCommission, value); } }
		private double _MonthlySystemCommission;

		public double CumulativeAdvertisingBonus { get { return _CumulativeAdvertisingBonus; } set { SetProperty(ref _CumulativeAdvertisingBonus, value); } }
		private double _CumulativeAdvertisingBonus;

		public double ACM { get { return _ACM; } set { SetProperty(ref _ACM, value); } }
		private double _ACM;

		public DistributionChannel EmailChannel { get { return _EmailChannel; } set { SetProperty(ref _EmailChannel, value); } }
		private DistributionChannel _EmailChannel = new DistributionChannel();

		public DistributionChannel DirectMailChannel { get { return _DirectMailChannel; } set { SetProperty(ref _DirectMailChannel, value); } }
		private DistributionChannel _DirectMailChannel = new DistributionChannel();

		public MarketingLeadCollection RecipientList { get { return _RecipientList; } set { SetProperty(ref _RecipientList, value); } }
		private MarketingLeadCollection _RecipientList = new MarketingLeadCollection();

		private int _CampaignOrderID;
		private Nullable<int> _ParentCampaignOrderID;
		private Nullable<int> _IsTouchPoint;
		private Nullable<int> _AccountID;
		private string _CmpNickName;
		private Nullable<int> _uStoreProductID;
		private Nullable<int> _uStoreOrderProductID;
		private Nullable<int> _EmailTemplateID;
		private Nullable<int> _pURLTemplateID;
		//private string _ProductName;
		private string _Description;
		private Nullable<DateTime> _OrderDate;
		private Nullable<DateTime> _DeploymentDate;
		private Nullable<DateTime> _CancelDate;
		private Nullable<int> _OrderStatusID;
		private Nullable<int> _DirectMailQty;
		private Nullable<int> _EmailQty;
		//private Nullable<decimal> _Cost;
		private Nullable<int> _CCApprovalCode;
		private Nullable<DateTime> _CreateDate;
		private Nullable<DateTime> _ModifiedDate;

		public int CampaignOrderID { get { return this._CampaignOrderID; } set { SetProperty(ref _CampaignOrderID, value); } }
		public Nullable<int> ParentCampaignOrderID { get { return this._ParentCampaignOrderID; } set { SetProperty(ref _ParentCampaignOrderID, value); } }
		public Nullable<int> IsTouchPoint { get { return this._IsTouchPoint; } set { SetProperty(ref _IsTouchPoint, value); } }
		public Nullable<int> AccountID { get { return this._AccountID; } set { SetProperty(ref _AccountID, value); } }
		public string CmpNickName { get { return this._CmpNickName; } set { SetProperty(ref _CmpNickName, value); } }
		public Nullable<int> uStoreProductID { get { return this._uStoreProductID; } set { SetProperty(ref _uStoreProductID, value); } }
		public Nullable<int> uStoreOrderProductID { get { return this._uStoreOrderProductID; } set { SetProperty(ref _uStoreOrderProductID, value); } }
		public Nullable<int> EmailTemplateID { get { return this._EmailTemplateID; } set { SetProperty(ref _EmailTemplateID, value); } }
		public Nullable<int> pURLTemplateID { get { return this._pURLTemplateID; } set { SetProperty(ref _pURLTemplateID, value); } }
		//public string ProductName { get { return this._ProductName; } set { SetProperty(ref _ProductName, value); } }
		public string Description { get { return this._Description; } set { SetProperty(ref _Description, value); } }
		public Nullable<DateTime> OrderDate { get { return this._OrderDate; } set { SetProperty(ref _OrderDate, value); } }
		public Nullable<DateTime> DeploymentDate { get { return this._DeploymentDate; } set { SetProperty(ref _DeploymentDate, value); } }
		public Nullable<DateTime> CancelDate { get { return this._CancelDate; } set { SetProperty(ref _CancelDate, value); } }
		public Nullable<int> OrderStatusID { get { return this._OrderStatusID; } set { SetProperty(ref _OrderStatusID, value); } }
		public Nullable<int> DirectMailQty { get { return this._DirectMailQty; } set { SetProperty(ref _DirectMailQty, value); } }
		public Nullable<int> EmailQty { get { return this._EmailQty; } set { SetProperty(ref _EmailQty, value); } }
		//public Nullable<decimal> Cost { get { return this._Cost; } set { SetProperty(ref _Cost, value); } }
		public Nullable<int> CCApprovalCode { get { return this._CCApprovalCode; } set { SetProperty(ref _CCApprovalCode, value); } }
		public Nullable<DateTime> CreateDate { get { return this._CreateDate; } set { SetProperty(ref _CreateDate, value); } }
		public Nullable<DateTime> ModifiedDate { get { return this._ModifiedDate; } set { SetProperty(ref _ModifiedDate, value); } }

		public Campaign()
		{
		}
		
		public override void Dispose()
		{
			base.Dispose();
		}
	}
}
