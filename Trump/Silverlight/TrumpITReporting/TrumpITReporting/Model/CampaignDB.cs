using System;
using System.Collections;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Linq;
using System.Xml.Linq;
using ClassLibrary;

namespace TrumpITReporting
{
	public class CampaignDBCollection : ObservableCollection<CampaignDB>, IDisposable
	{
		public void Dispose()
		{
			foreach (CampaignDB item in this)
				item.Dispose();
			Clear();
		}

		internal void Initialize()
		{
		}

		internal void GetCampaignFromAccountID(string accountID)
		{
			CampaignDBSqlQuery query = new CampaignDBSqlQuery(accountID);
			query.Result += delegate(Collection<CampaignDB> collection)
			{
				base.Clear();
				if (collection == null)
					return;

				foreach (CampaignDB item in collection)
					base.Add(item);
			};
			query.Execute("TrumpIT");
		}
	}

	internal class CampaignDBSqlQuery : DBServiceBase<CampaignDB>
	{
		private string _accountID;

		internal CampaignDBSqlQuery(string accountID)
		{
			_accountID = accountID;
		}

		internal override string SqlQuery()
		{
			return string.Format(@"
				SELECT 
					c.CampaignOrderID, c.ParentCampaignOrderID, c.IsTouchPoint, c.AccountID, 
					c.CmpNickName, c.uStoreProductID, c.uStoreOrderProductID, c.EmailTemplateID, 
					c.pURLTemplateID, c.ProductName, c.Description, c.OrderDate, 
					c.DeploymentDate, c.CancelDate, c.OrderStatusID, c.DirectMailQty, 
					c.EmailQty, c.Cost, c.CCApprovalCode, c.CreateDate, c.ModifiedDate
				FROM CampaignOrder AS c
				WHERE (AccountID = '{0}')", _accountID);
				//Salutation.Name AS 'SalutationName', 
				//FiscalIDTy.Descr AS 'FiscalType', NameTy.Descr AS 'NameType'
				//LEFT OUTER JOIN Salutation ON c.SalutationID = Salutation.SalutationID
				//LEFT OUTER JOIN NameTy ON c.NameTy = NameTy.NameTy
				//LEFT OUTER JOIN FiscalIDTy ON c.FiscalIdTy = FiscalIDTy.FiscalIDTy
				//LEFT OUTER JOIN Ethnicity ON c.EthnicityCode = Ethnicity.EthnicityCode
		}

		internal override IEnumerable LinqQuery(XDocument xmlDoc)
		{
			IEnumerable<CampaignDB> query = from item in xmlDoc.Root.Descendants()
			//select Extract(item);
			select new CampaignDB
			{
				CampaignOrderID = item.AttributeValue("CampaignOrderID").ToInt(),
				ParentCampaignOrderID = item.AttributeValue("ParentCampaignOrderID").ToInt(),
				IsTouchPoint = item.AttributeValue("IsTouchPoint").ToInt(),
				AccountID = item.AttributeValue("AccountID").ToInt(),
				CmpNickName = item.AttributeValue("CmpNickName"),
				uStoreProductID = item.AttributeValue("uStoreProductID").ToInt(),
				uStoreOrderProductID = item.AttributeValue("uStoreOrderProductID").ToInt(),
				EmailTemplateID = item.AttributeValue("EmailTemplateID").ToInt(),
				pURLTemplateID = item.AttributeValue("pURLTemplateID").ToInt(),
				ProductName = item.AttributeValue("ProductName"),
				Description = item.AttributeValue("Description"),
				OrderDate = item.AttributeValue("OrderDate").ToDateTime(),
				DeploymentDate = item.AttributeValue("DeploymentDate").ToDateTime(),
				CancelDate = item.AttributeValue("CancelDate").ToDateTime(),
				OrderStatusID = item.AttributeValue("OrderStatusID").ToInt(),
				DirectMailQty = item.AttributeValue("DirectMailQty").ToInt(),
				EmailQty = item.AttributeValue("EmailQty").ToInt(),
				Cost = item.AttributeValue("Cost").ToDecimal(),
				CCApprovalCode = item.AttributeValue("CCApprovalCode").ToInt(),
				CreateDate = item.AttributeValue("CreateDate").ToDateTime(),
				ModifiedDate = item.AttributeValue("ModifiedDate").ToDateTime(),
			};

			return query;
		}
	}

	public class CampaignDB : NotifyPropertyChanges
	{
		private int _CampaignOrderID;
		private Nullable<int> _ParentCampaignOrderID;
		private Nullable<int> _IsTouchPoint;
		private Nullable<int> _AccountID;
		private string _CmpNickName;
		private Nullable<int> _uStoreProductID;
		private Nullable<int> _uStoreOrderProductID;
		private Nullable<int> _EmailTemplateID;
		private Nullable<int> _pURLTemplateID;
		private string _ProductName;
		private string _Description;
		private Nullable<DateTime> _OrderDate;
		private Nullable<DateTime> _DeploymentDate;
		private Nullable<DateTime> _CancelDate;
		private Nullable<int> _OrderStatusID;
		private Nullable<int> _DirectMailQty;
		private Nullable<int> _EmailQty;
		private Nullable<decimal> _Cost;
		private Nullable<int> _CCApprovalCode;
		private Nullable<DateTime> _CreateDate;
		private Nullable<DateTime> _ModifiedDate;

		public CampaignDB()
		{
		}
		
		public override void Dispose()
		{
			base.Dispose();
		}

		public int CampaignOrderID { get { return this._CampaignOrderID; } set { SetProperty(ref _CampaignOrderID, value); } }
		
		public Nullable<int> ParentCampaignOrderID { get { return this._ParentCampaignOrderID; } set { SetProperty(ref _ParentCampaignOrderID, value); } }
		
		public Nullable<int> IsTouchPoint { get { return this._IsTouchPoint; } set { SetProperty(ref _IsTouchPoint, value); } }
		
		public Nullable<int> AccountID { get { return this._AccountID; } set { SetProperty(ref _AccountID, value); } }
		
		public string CmpNickName { get { return this._CmpNickName; } set { SetProperty(ref _CmpNickName, value); } }
		
		public Nullable<int> uStoreProductID { get { return this._uStoreProductID; } set { SetProperty(ref _uStoreProductID, value); } }
		
		public Nullable<int> uStoreOrderProductID { get { return this._uStoreOrderProductID; } set { SetProperty(ref _uStoreOrderProductID, value); } }
		
		public Nullable<int> EmailTemplateID { get { return this._EmailTemplateID; } set { SetProperty(ref _EmailTemplateID, value); } }
		
		public Nullable<int> pURLTemplateID { get { return this._pURLTemplateID; } set { SetProperty(ref _pURLTemplateID, value); } }
		
		public string ProductName { get { return this._ProductName; } set { SetProperty(ref _ProductName, value); } }
		
		public string Description { get { return this._Description; } set { SetProperty(ref _Description, value); } }
		
		public Nullable<DateTime> OrderDate { get { return this._OrderDate; } set { SetProperty(ref _OrderDate, value); } }
		
		public Nullable<DateTime> DeploymentDate { get { return this._DeploymentDate; } set { SetProperty(ref _DeploymentDate, value); } }
		
		public Nullable<DateTime> CancelDate { get { return this._CancelDate; } set { SetProperty(ref _CancelDate, value); } }
		
		public Nullable<int> OrderStatusID { get { return this._OrderStatusID; } set { SetProperty(ref _OrderStatusID, value); } }
		
		public Nullable<int> DirectMailQty { get { return this._DirectMailQty; } set { SetProperty(ref _DirectMailQty, value); } }
		
		public Nullable<int> EmailQty { get { return this._EmailQty; } set { SetProperty(ref _EmailQty, value); } }
		
		public Nullable<decimal> Cost { get { return this._Cost; } set { SetProperty(ref _Cost, value); } }
		
		public Nullable<int> CCApprovalCode { get { return this._CCApprovalCode; } set { SetProperty(ref _CCApprovalCode, value); } }
		
		public Nullable<DateTime> CreateDate { get { return this._CreateDate; } set { SetProperty(ref _CreateDate, value); } }

		public Nullable<DateTime> ModifiedDate { get { return this._ModifiedDate; } set { SetProperty(ref _ModifiedDate, value); } }
	}
}
