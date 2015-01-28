using System;
using System.Windows.Browser;

namespace ClassLibrary
{
	internal class PageMethodInvoker
	{
		internal delegate void PageMethodInvokerCompleted(object userContext, string methodName, bool success, object result);
		internal event PageMethodInvokerCompleted Completed;

		private delegate void PageMethodEventHandler(object result, object userContext, string methodName);

		internal void Invoke(object userContext, string methodName, params object[] args)
		{
			try
			{
				ScriptObject pageMethods = (ScriptObject)HtmlPage.Window.GetProperty("PageMethods");
				if (pageMethods == null)
					throw new ArgumentException("Web page does not support PageMethods");
				if (Completed == null)
					throw new ArgumentException("PageMethodInvoker has no Completed handler");
				object[] pageMethodArgs = { new PageMethodEventHandler(Success), new PageMethodEventHandler(Failure), null/*userContext*/};
				object[] combinedArgs = new object[args.Length + pageMethodArgs.Length];
				args.CopyTo(combinedArgs, 0);
				pageMethodArgs.CopyTo(combinedArgs, args.Length);
				pageMethods.Invoke(methodName, combinedArgs);
			}
			catch (Exception ex)
			{
				ex.Alert();
			}
		}

		private void Success(object result, object userContext, string methodName)
		{
			if (Completed != null)
				Completed(userContext, methodName, true/*success*/, result);
		}

		private void Failure(object result, object userContext, string methodName)
		{
			if (Completed != null)
				Completed(userContext, methodName, false/*success*/, result);
		}
	}
}
