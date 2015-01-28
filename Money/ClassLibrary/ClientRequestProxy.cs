using System.Windows;

// Usage as follows:
//		string imageUrl = ClientRequestProxy.Translate("http://dreamnit.com/Images/Screenshot.png");
//		Uri imageUri = new Uri(imageUrl, UriKind.RelativeOrAbsolute);
//		WebClient wc = new WebClient();
//		wc.OpenReadCompleted += OnOpenReadCompleted;
//		wc.OpenReadAsync(imageUri);

namespace ClassLibrary
{
	internal static class ClientRequestProxy
	{
		internal static string Translate(string targetUrl)
		{
			const string serverProxyPage = "ClientRequestProxy.ashx";
			string finalUrl =
				Application.Current.Host.Source.Scheme + "://" +
				Application.Current.Host.Source.Host + ":" +
				Application.Current.Host.Source.Port + "/" + 
				serverProxyPage + "?url=" + targetUrl;

			return finalUrl;
		}
	}
}
