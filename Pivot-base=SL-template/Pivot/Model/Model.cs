using System.Globalization;
using ClassLibrary;

namespace Pivot
{
	public class Model : ViewModelBase
	{
		//public ProfileHolder ProfileHolder { get { return m_ProfileHolder; } set { SetProperty(ref m_ProfileHolder, value, System.Reflection.MethodBase.GetCurrentMethod().Name); } }
		//private ProfileHolder m_ProfileHolder = new ProfileHolder();

		//public DocumentHolder DocumentHolder { get { return m_DocumentHolder; } set { SetProperty(ref m_DocumentHolder, value, System.Reflection.MethodBase.GetCurrentMethod().Name); } }
		//private DocumentHolder m_DocumentHolder = new DocumentHolder();

		public UserServices UserServices { get { return m_UserServices; } set { SetProperty(ref m_UserServices, value, System.Reflection.MethodBase.GetCurrentMethod().Name); } }
		private UserServices m_UserServices = new UserServices();

		public Model()
		{
		}

		public override void Dispose()
		{
			base.Dispose();
			//m_ProfileHolder.Dispose();
			//m_DocumentHolder.Dispose();
			m_UserServices.Dispose();
		}

		internal void Initialize()
		{
			//m_ProfileHolder.Initialize();
			//m_DocumentHolder.Initialize();
			m_UserServices.Initialize();
		}
	}
}
