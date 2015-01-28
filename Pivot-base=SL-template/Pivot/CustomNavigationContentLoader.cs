using System;
using System.Windows.Navigation;

namespace Pivot
{
	public class CustomNavigationContentLoader : INavigationContentLoader
	{
		private static PageResourceContentLoader _defaultPageContentLoader = new PageResourceContentLoader();

		public IAsyncResult BeginLoad(Uri targetUri, Uri currentUri, AsyncCallback userCallback, object asyncState)
		{
			return _defaultPageContentLoader.BeginLoad(targetUri, currentUri, userCallback, asyncState);
		}

		public bool CanLoad(Uri targetUri, Uri currentUri)
		{
			return _defaultPageContentLoader.CanLoad(targetUri, currentUri);
		}

		public void CancelLoad(IAsyncResult asyncResult)
		{
			_defaultPageContentLoader.CancelLoad(asyncResult);
		}

		public LoadResult EndLoad(IAsyncResult asyncResult)
		{
			return _defaultPageContentLoader.EndLoad(asyncResult);
		}
	}
}
