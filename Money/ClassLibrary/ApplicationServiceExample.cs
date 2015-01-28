using System.Windows;
using System.Windows.Browser;

namespace ClassLibrary
{
	// Typical usage:
	//
	//	<Application.Services>
	//	    <c:ApplicationServiceExample x:Name="ApplicationServiceExample" />
	//	</Application.Services>
	//	<Application.ApplicationLifetimeObjects>
	//	    <c:ApplicationServiceExample x:Name="ApplicationServiceExample" />
	//	<Application.ApplicationLifetimeObjects>
	//
	//		IApplicationService appService;
	//		if (Application.Current.Services.TryGetValue("ApplicationServiceExample", out appService))
	//		{
	//			((ApplicationServiceExample)appService).Contents = "MyText";
	//			object value = ((ApplicationServiceExample)appService).Contents;
	//		}
    public class ApplicationServiceExample : IApplicationService
    {
        private ScriptObject m_ClipboardData;

        public object Contents
        {
            get
            {
                object value = null;
                if (m_ClipboardData != null)
                    value = m_ClipboardData.Invoke("getData", "Text");

				return value;
            }
            set
            {
                if (m_ClipboardData != null)
                    m_ClipboardData.Invoke("setData", "Text", value);
            }
        }

        public void StartService(ApplicationServiceContext context)
        {
            m_ClipboardData = HtmlPage.Window.GetProperty("clipboardData") as ScriptObject;
        }

        public void StopService()
        {
            m_ClipboardData = null;
        }
    }
}
