using System;
using System.Net;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Documents;
using System.Windows.Ink;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Animation;
using System.Windows.Shapes;
using System.Linq;
using System.Xml.Linq;
using System.Collections.Generic;
using System.ComponentModel;
using System.IO;
using System.IO.IsolatedStorage;

namespace ClassLibrary
{
	public enum AuthenticationState
	{
		NotAuthenticated,
		Authenticated,
		Unknown
	}

	public static class UserContext
	{
		// Properties
		public static int CurrentID { get; set; }
	}

	public class GetStreamAsyncEventArgs : AsyncCompletedEventArgs
	{
		// Fields
		private Stream m_Result;

		// Methods
		internal GetStreamAsyncEventArgs(Stream result, Exception exception, bool cancelled, object userToken) : base(exception, cancelled, userToken)
		{
			this.m_Result = result;
		}

		// Properties
		public Stream Result
		{
			get
			{
				base.RaiseExceptionIfNecessary();
				return this.m_Result;
			}
		}
	}

	public abstract class Photo
	{
		// Events
		public event EventHandler<GetStreamAsyncEventArgs> CreateOriginalStreamCompleted;

		public event EventHandler<GetStreamAsyncEventArgs> CreateThumbnailStreamCompleted;

		// Methods
		protected Photo()
		{
		}

		public void CreateOriginalStreamAsync()
		{
			this.CreateOriginalStreamAsync(null);
		}

		public abstract void CreateOriginalStreamAsync(object userState);
		public void CreateThumbnailStreamAsync()
		{
			this.CreateThumbnailStreamAsync(null);
		}

		public abstract void CreateThumbnailStreamAsync(object userState);
		protected void OnOriginalStreamReady(GetStreamAsyncEventArgs args)
		{
			if (this.CreateOriginalStreamCompleted != null)
			{
				this.CreateOriginalStreamCompleted(this, args);
			}
		}

		protected void OnThumbnailStreamReady(GetStreamAsyncEventArgs args)
		{
			if (this.CreateThumbnailStreamCompleted != null)
			{
				this.CreateThumbnailStreamCompleted(this, args);
			}
		}

		// Properties
		public string Extension { get; protected set; }

		public string Title { get; protected set; }
	}

	public class PhotoSet
	{
		// Methods
		public PhotoSet()
		{
			this.Photos = new List<Photo>();
		}

		public static PhotoSet FromXML(XElement xe)
		{
			return new PhotoSet { ID = xe.Attribute("id").Value, Secret = xe.Attribute("secret").Value, Title = xe.Element("title").Value, Count = int.Parse(xe.Attribute("photos").Value), Primary = xe.Attribute("primary").Value, Server = xe.Attribute("server").Value, Farm = xe.Attribute("farm").Value };
		}

		// Properties
		public int Count { get; set; }

		public string Farm { get; set; }

		public string ID { get; set; }

		public bool IsLooseSet { get; set; }

		public bool PhotoListLoaded { get; set; }

		public List<Photo> Photos { get; private set; }

		public string Primary { get; set; }

		public string Secret { get; set; }

		public string Server { get; set; }

		public string Title { get; set; }
	}

	public class DownloadManager : FrameworkTemplate
	{
	}

	public class FlickrApiFormats
	{
		// Fields
		public const string apiKey = "daaa0fc85d0c1044bc3d397e82980f5a";
		public const string basePath = "http://api.flickr.com/services/";
		public const string getFrobFormat = "http://api.flickr.com/services/rest/?method=flickr.auth.getFrob&api_key={0}";
		public const string getPhotoSetListFormat = "http://api.flickr.com/services/rest/?method=flickr.photosets.getList&api_key={0}&auth_token={1}&user_id={2}";
		public const string getPhotoSetPhotosFormat = "http://api.flickr.com/services/rest/?method=flickr.photosets.getPhotos&api_key={0}&auth_token={1}&user_id={2}&photoset_id={3}&per_page=500&extras=original_format";
		public const string getPhotosNotInSetFormat = "http://api.flickr.com/services/rest/?method=flickr.photos.getNotInSet&api_key={0}&auth_token={1}&per_page=500&extras=original_format";
		public const string getTokenFormat = "http://api.flickr.com/services/rest/?method=flickr.auth.getToken&api_key={0}&frob={1}";
		public const string restBasePath = "http://api.flickr.com/services/rest/?method=";
		public const string sharedSecret = "7b2bc91ddf003dbe";
		public const string userAuthPathFormat = "http://api.flickr.com/services/auth/?api_key={0}&perms={1}&frob={2}";

		// Methods
		public static Uri SignedUrl(string format, params string[] formatParameters)
		{
			string str = string.Format(format, (object[]) formatParameters);
			int length = str.IndexOf('?') + 1;
			str.Substring(0, length);
			string[] strArray = (from s in str.Substring(length).Split(new char[] { '&' })
				orderby s
				select s).ToArray<string>();
			string source = "7b2bc91ddf003dbe" + string.Join("", strArray).Replace("=", "");
			string str3 = "&api_sig=" + MD5.GetMd5String(source);
			return new Uri(str + str3, UriKind.Absolute);
		}

		public static Uri Url(string format, params string[] formatParameters)
		{
			return new Uri(string.Format(format, (object[]) formatParameters), UriKind.Absolute);
		}
	}

	public class FlickrSession
	{
		// Methods
		private XElement FlickrSessionToXml()
		{
			if ((this.AuthToken == null) || (this.AuthToken.Length < 1))
			{
				return null;
			}
			return new XElement("FlickrSession", new object[] { new XElement("token", this.AuthToken), new XElement("user", new object[] { new XAttribute("nsid", this.UserID), new XAttribute("username", this.UserName), new XAttribute("fullname", this.FullName) }) });
		}

		public static FlickrSession FromIsolatedStorage()
		{
			try
			{
				string text = string.Empty;
				using (IsolatedStorageFile file = IsolatedStorageFile.GetUserStoreForApplication())
				{
					if (!file.FileExists("Flickr" + UserContext.CurrentID.ToString() + ".txt"))
					{
						return null;
					}
					using (IsolatedStorageFileStream stream = new IsolatedStorageFileStream(GetFileName(), FileMode.Open, FileAccess.Read, file))
					{
						using (StreamReader reader = new StreamReader(stream))
						{
							text = reader.ReadToEnd();
						}
					}
				}
				if (text.Length > 0)
				{
					return FromXml(XElement.Parse(text));
				}
			}
			catch (Exception)
			{
				//Logger.Error(exception);
				return null;
			}
			return null;
		}

		public static FlickrSession FromXml(XElement xe)
		{
			return new FlickrSession { AuthToken = xe.Descendants("token").First<XElement>().Value, UserID = xe.Descendants("user").First<XElement>().Attribute("nsid").Value, UserName = xe.Descendants("user").First<XElement>().Attribute("username").Value, FullName = xe.Descendants("user").First<XElement>().Attribute("fullname").Value };
		}

		private static string GetFileName()
		{
			return ("Flickr" + UserContext.CurrentID.ToString() + ".txt");
		}

		public void SaveSessionToIsolatedStorage()
		{
			XElement element = this.FlickrSessionToXml();
			if (element != null)
			{
				using (IsolatedStorageFile file = IsolatedStorageFile.GetUserStoreForApplication())
				{
					using (IsolatedStorageFileStream stream = new IsolatedStorageFileStream(GetFileName(), FileMode.Create, file))
					{
						using (StreamWriter writer = new StreamWriter(stream))
						{
							writer.Write(element.ToString());
						}
					}
				}
			}
		}

		// Properties
		public string AuthToken { get; set; }

		public string FullName { get; set; }

		public string UserID { get; set; }

		public string UserName { get; set; }
	}

	public class PhotoFlickr : Photo
	{
		// Methods
		public override void CreateOriginalStreamAsync(object userState)
		{
			WebClient client = new WebClient();
			client.OpenReadCompleted += new OpenReadCompletedEventHandler(this.originalClient_OpenReadCompleted);
			client.OpenReadAsync(this.FullUrl, userState);
		}

		public override void CreateThumbnailStreamAsync(object userState)
		{
			WebClient client = new WebClient();
			client.OpenReadCompleted += new OpenReadCompletedEventHandler(this.thumbnailClient_OpenReadCompleted);
			client.OpenReadAsync(this.ThumbnailUrl, userState);
		}

		public static PhotoFlickr FromXml(XElement xe)
		{
			return new PhotoFlickr { ID = xe.Attribute("id").Value, Secret = xe.Attribute("secret").Value, Server = xe.Attribute("server").Value, Title = xe.Attribute("title").Value, Farm = xe.Attribute("farm").Value, Extension = (xe.Attribute("original_format") != null) ? xe.Attribute("original_format").Value : "jpg" };
		}

		private void originalClient_OpenReadCompleted(object sender, OpenReadCompletedEventArgs e)
		{
			((WebClient) sender).OpenReadCompleted -= new OpenReadCompletedEventHandler(this.originalClient_OpenReadCompleted);
			GetStreamAsyncEventArgs args = new GetStreamAsyncEventArgs(e.Result, e.Error, e.Cancelled, e.UserState);
			base.OnOriginalStreamReady(args);
		}

		private void thumbnailClient_OpenReadCompleted(object sender, OpenReadCompletedEventArgs e)
		{
			((WebClient) sender).OpenReadCompleted -= new OpenReadCompletedEventHandler(this.thumbnailClient_OpenReadCompleted);
			GetStreamAsyncEventArgs args = new GetStreamAsyncEventArgs(e.Result, e.Error, e.Cancelled, e.UserState);
			base.OnThumbnailStreamReady(args);
		}

		// Properties
		private string Farm { get; set; }

		private Uri FullUrl
		{
			get
			{
				string format = "http://farm{0}.static.flickr.com/{1}/{2}_{3}_b.jpg";
				return new Uri(string.Format(format, new object[] { this.Farm, this.Server, this.ID, this.Secret }));
			}
		}

		private string ID { get; set; }

		private string Secret { get; set; }

		private string Server { get; set; }

		private Uri ThumbnailUrl
		{
			get
			{
				string format = "http://farm{0}.static.flickr.com/{1}/{2}_{3}_t.jpg";
				return new Uri(string.Format(format, new object[] { this.Farm, this.Server, this.ID, this.Secret }));
			}
		}
	}

	public class FlickrAuthenticationCycle : FrameworkTemplate
	{
		// Fields
		private FlickrSession CurrentSession;
		private string Frob = string.Empty;

		// Events
		public event EventHandler<EventArgs> FlickrAccessErrored;

		public event EventHandler<EventArgs> PhotoSetsLoaded;

		// Methods
		public FlickrAuthenticationCycle()
		{
			if (this.RestoreSessionFromIsolatedStorage())
			{
				this.CurrentAuthenticationState = AuthenticationState.Authenticated;
			}
			else
			{
				this.CurrentAuthenticationState = AuthenticationState.NotAuthenticated;
			}
		}

		public void Begin()
		{
			Uri uri = FlickrApiFormats.SignedUrl("http://api.flickr.com/services/rest/?method=flickr.auth.getFrob&api_key={0}", new string[] { "daaa0fc85d0c1044bc3d397e82980f5a" });
			this.Downloader.DownloadXmlAsync(uri, null, new XmlAvailableDelegate(this.NavigateToUserAuthPage), new DownloadFailedDelegate(this.DownloadFailed), true);
		}

		private void CheckAndFireEvent()
		{
			if (this.PhotoSets.All<PhotoSet>(ps => ps.PhotoListLoaded) && (this.PhotoSetsLoaded != null))
			{
				this.PhotoSetsLoaded(this, EventArgs.Empty);
			}
		}

		public void Continue()
		{
			if ((this.CurrentSession == null) || (this.CurrentAuthenticationState != AuthenticationState.Authenticated))
			{
				Uri uri = FlickrApiFormats.SignedUrl("http://api.flickr.com/services/rest/?method=flickr.auth.getToken&api_key={0}&frob={1}", new string[] { "daaa0fc85d0c1044bc3d397e82980f5a", this.Frob });
				this.Downloader.DownloadXmlAsync(uri, null, new XmlAvailableDelegate(this.ExtractFlickrSession), new DownloadFailedDelegate(this.DownloadFailed), true);
			}
			else
			{
				this.GetSetList();
			}
		}

		private void DownloadFailed(Uri address, Exception exception, object userState)
		{
			//Logger.Error(exception, address);
			this.FireError();
		}

		private void ExtractFlickrSession(Uri address, XDocument result, object userState)
		{
			XAttribute attribute = result.Element("rsp").Attribute("stat");
			if ((attribute == null) || (attribute.Value.ToUpper() != "OK"))
			{
				this.FireError();
			}
			else
			{
				this.CurrentSession = FlickrSession.FromXml(result.Root);
				this.CurrentSession.SaveSessionToIsolatedStorage();
				this.CurrentAuthenticationState = AuthenticationState.Authenticated;
				this.GetSetList();
			}
		}

		private void ExtractPhotos(List<Photo> photoList, XDocument doc)
		{
			photoList.AddRange((from xe in doc.Descendants("photo") select PhotoFlickr.FromXml(xe)).Cast<Photo>());
		}

		private void ExtractPhotoSetContents(Uri address, XDocument result, object userState)
		{
			XAttribute attribute = result.Element("rsp").Attribute("stat");
			if ((attribute == null) || (attribute.Value.ToUpper() != "OK"))
			{
				this.FireError();
			}
			else
			{
				this.PhotoSets = (from xe in result.Descendants("photoset") select PhotoSet.FromXML(xe)).ToList<PhotoSet>();
				foreach (PhotoSet set in this.PhotoSets)
				{
					Uri uri = FlickrApiFormats.SignedUrl("http://api.flickr.com/services/rest/?method=flickr.photosets.getPhotos&api_key={0}&auth_token={1}&user_id={2}&photoset_id={3}&per_page=500&extras=original_format", new string[] { "daaa0fc85d0c1044bc3d397e82980f5a", this.CurrentSession.AuthToken, this.CurrentSession.UserID, set.ID });
					this.Downloader.DownloadXmlAsync(uri, set, new XmlAvailableDelegate(this.ExtractPhotosFromSet), new DownloadFailedDelegate(this.DownloadFailed), true);
				}
				PhotoSet item = new PhotoSet {
					Title = "[Not in a set]",
					IsLooseSet = true
				};
				this.PhotoSets.Insert(0, item);
				Uri uri2 = FlickrApiFormats.SignedUrl("http://api.flickr.com/services/rest/?method=flickr.photos.getNotInSet&api_key={0}&auth_token={1}&per_page=500&extras=original_format", new string[] { "daaa0fc85d0c1044bc3d397e82980f5a", this.CurrentSession.AuthToken });
				this.Downloader.DownloadXmlAsync(uri2, item, new XmlAvailableDelegate(this.ExtractPhotosFromSet), new DownloadFailedDelegate(this.DownloadFailed), true);
			}
		}

		private void ExtractPhotosFromSet(Uri address, XDocument result, object userState)
		{
			PhotoSet set = (PhotoSet) userState;
			this.ExtractPhotos(set.Photos, result);
			set.PhotoListLoaded = true;
			this.CheckAndFireEvent();
		}

		private void FireError()
		{
			if (this.FlickrAccessErrored != null)
			{
				this.FlickrAccessErrored(this, EventArgs.Empty);
			}
			this.CurrentAuthenticationState = AuthenticationState.Unknown;
		}

		private void GetSetList()
		{
			Uri uri = FlickrApiFormats.SignedUrl("http://api.flickr.com/services/rest/?method=flickr.photosets.getList&api_key={0}&auth_token={1}&user_id={2}", new string[] { "daaa0fc85d0c1044bc3d397e82980f5a", this.CurrentSession.AuthToken, this.CurrentSession.UserID });
			this.Downloader.DownloadXmlAsync(uri, null, new XmlAvailableDelegate(this.ExtractPhotoSetContents), new DownloadFailedDelegate(this.DownloadFailed), true);
		}

		private void NavigateToUserAuthPage(Uri address, XDocument result, object userState)
		{
			this.Frob = result.Descendants("frob").First<XElement>().Value;
			SiteContext.NavigateTo(FlickrApiFormats.SignedUrl("http://api.flickr.com/services/auth/?api_key={0}&perms={1}&frob={2}", new string[] { "daaa0fc85d0c1044bc3d397e82980f5a", "read", this.Frob }));
		}

		public bool RestoreSessionFromIsolatedStorage()
		{
			this.CurrentSession = FlickrSession.FromIsolatedStorage();
			return (this.CurrentSession != null);
		}

		// Properties
		public AuthenticationState CurrentAuthenticationState { get; set; }

		private DownloadManager Downloader
		{
			get
			{
				return InkubookApplication.Current.Downloader;
			}
		}

		public List<PhotoSet> PhotoSets { get; private set; }
	}
}
