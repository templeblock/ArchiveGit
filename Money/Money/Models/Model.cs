using System.Globalization;
using ClassLibrary;

namespace Money
{
	public class Model : ViewModelBase
	{
		public ProfileHolder ProfileHolder { get { return _ProfileHolder; } set { SetProperty(ref _ProfileHolder, value, System.Reflection.MethodBase.GetCurrentMethod().Name); } }
		private ProfileHolder _ProfileHolder = new ProfileHolder();

		public DocumentHolder DocumentHolder { get { return _DocumentHolder; } set { SetProperty(ref _DocumentHolder, value, System.Reflection.MethodBase.GetCurrentMethod().Name); } }
		private DocumentHolder _DocumentHolder = new DocumentHolder();

		public UserServices UserServices { get { return _UserServices; } set { SetProperty(ref _UserServices, value, System.Reflection.MethodBase.GetCurrentMethod().Name); } }
		private UserServices _UserServices = new UserServices();

		public string CountryCode { get { return RegionInfo.CurrentRegion.TwoLetterISORegionName; } }
		public string CountryName { get { return RegionInfo.CurrentRegion.NativeName; } }

		public Model()
		{
		}

		public override void Dispose()
		{
			base.Dispose();
			_ProfileHolder.Dispose();
			_DocumentHolder.Dispose();
			_UserServices.Dispose();
		}

		internal void Initialize()
		{
			_ProfileHolder.Initialize();
			_DocumentHolder.Initialize();
			_UserServices.Initialize();
		}
	}
}
