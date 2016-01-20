using System.Globalization;
using ClassLibrary;

namespace TrumpITReporting
{
	public class Model : NotifyPropertyChanges
	{
		public ILincDataCollection ILincDataCollection { get { return m_ILincDataCollection; } set { SetProperty(ref m_ILincDataCollection, value); } }
		private ILincDataCollection m_ILincDataCollection = new ILincDataCollection();

		public PersonDBCollection PersonCollection { get { return m_PersonCollection; } set { SetProperty(ref m_PersonCollection, value); } }
		private PersonDBCollection m_PersonCollection = new PersonDBCollection();

		public CampaignDBCollection CampaignCollection { get { return m_CampaignCollection; } set { SetProperty(ref m_CampaignCollection, value); } }
		private CampaignDBCollection m_CampaignCollection = new CampaignDBCollection();

		public MarketingLeadDBCollection MarketingLeadCollection { get { return m_MarketingLeadCollection; } set { SetProperty(ref m_MarketingLeadCollection, value); } }
		private MarketingLeadDBCollection m_MarketingLeadCollection = new MarketingLeadDBCollection();

		public AddressDBCollection AddressCollection { get { return m_AddressCollection; } set { SetProperty(ref m_AddressCollection, value); } }
		private AddressDBCollection m_AddressCollection = new AddressDBCollection();

		public string CountryCode { get { return RegionInfo.CurrentRegion.TwoLetterISORegionName; } }
		public string CountryName { get { return RegionInfo.CurrentRegion.NativeName; } }

		public Model()
		{
		}

		public override void Dispose()
		{
			base.Dispose();
			m_ILincDataCollection.Dispose();
			m_PersonCollection.Dispose();
			m_CampaignCollection.Dispose();
			m_MarketingLeadCollection.Dispose();
			m_AddressCollection.Dispose();
		}

		internal void Initialize()
		{
			m_ILincDataCollection.Initialize();
			m_PersonCollection.Initialize();
			m_CampaignCollection.Initialize();
			m_MarketingLeadCollection.Initialize();
			m_AddressCollection.Initialize();
		}
	}
}
