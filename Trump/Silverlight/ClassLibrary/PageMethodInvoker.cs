using System;
using System.Windows.Browser;

namespace ClassLibrary
{
	internal class PageMethodInvoker
	{
		internal delegate void PageMethodInvokerHandler(object methodName, bool success, object result, object userContext);
		internal event PageMethodInvokerHandler Completed;

		private delegate void PageMethodEventHandler(object result, object userContext, object methodName);

		internal void Invoke(string methodName, object args, object userContext)
		{
			try
			{
				if (Completed == null)
					throw new ArgumentException("PageMethodInvoker has no Completed handler");
				ScriptObject pageMethods = (ScriptObject)HtmlPage.Window.GetProperty("PageMethods");
				pageMethods.Invoke(methodName, args, new PageMethodEventHandler(Success), new PageMethodEventHandler(Failure), userContext);
			}
			catch (Exception ex)
			{
				ex.Alert();
			}
		}

		private void Success(object result, object userContext, object methodName)
		{
			if (Completed != null)
				Completed(methodName, true/*success*/, result, userContext);
		}

		private void Failure(object result, object userContext, object methodName)
		{
			if (Completed != null)
				Completed(methodName, false/*success*/, result, userContext);
		}
	}
}
