using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
using System.IO.IsolatedStorage;
using System.Net;
using System.Xml;
using System.Xml.Linq;

namespace ClassLibrary
{
	public sealed class SettingsDictionary : Dictionary<string, string>
	{
		private const string _contentPathFormat = "AppData/Users/{0}/{1}";
		private const string _xmlFileName = "Settings.xml";

		private Uri _serverUri;
		private string _userId;
		private string _contentPath;
		private IsolatedStorageFile _storage = IsolatedStorageFile.GetUserStoreForAssembly();
		private WebClient _download = new WebClient() { UseDefaultCredentials = true };
		private WebClient _upload = new WebClient() { UseDefaultCredentials = true };

		public SettingsDictionary(Uri serverUri, string userId)
		{
			// The userId cannot end with a '.' because of a server side MapPath error
			_userId = userId.ToLower();
			if (_userId.LastIndexOf('.') == _userId.Length - 1)
				_userId = _userId.Substring(0, _userId.Length - 1);

			_serverUri = serverUri;
			_contentPath = string.Format(_contentPathFormat, _userId, _xmlFileName);
			Debug.WriteLine(string.Format("SettingsDictionary: Initializing for user '{0}', content path '{1}'", _userId, _contentPath));

			_download.OpenReadCompleted += OnDownloadCompleted;
			_upload.OpenWriteCompleted += OnUploadCompleted;

			LoadDictionaryFromFile();
			DownloadSettingsFile();
		}

		~SettingsDictionary()
		{
			// settings are automatically saved if the application close normally
			// Too late...
			//Save();
		}

		private void DownloadSettingsFile()
		{
			Uri uriResult = new Uri(_serverUri, _contentPath);
			Debug.WriteLine(string.Format("SettingsDictionary: Downloading \"{0}\"", uriResult.ToString()));
			Uri uri = uriResult;
			_download.OpenReadAsync(uri);
		}

		private void UploadSettingsFile()
		{
			Uri uriResult = new Uri(_serverUri, _contentPath);
			Debug.WriteLine(string.Format("SettingsDictionary: Uploading \"{0}\"", uriResult.ToString()));
			Uri uri = new Uri(_serverUri, string.Format("UploadHandler.ashx?ContentPath=~/{0}", _contentPath));
			_upload.OpenWriteAsync(uri);
		}

		private void OnDownloadCompleted(object sender, OpenReadCompletedEventArgs e)
		{
			try
			{
				if (e.Error != null)
				{
					Debug.WriteLine(string.Format("SettingsDictionary: Download error: {0}", e.Error.Message));
					return;
				}

				using (Stream readerStream = e.Result)
				{
					using (IsolatedStorageFileStream writerStream = _storage.CreateFile(_xmlFileName))
					{
						readerStream.CopyTo(writerStream);
					}
				}
			}
			catch (Exception ex)
			{
				ex.GetType();
			}

			LoadDictionaryFromFile();
		}

		private void OnUploadCompleted(object sender, OpenWriteCompletedEventArgs e)
		{
			try
			{
				if (e.Error != null)
				{
					Debug.WriteLine(string.Format("SettingsDictionary: Upload error: {0}", e.Error.Message));
					return;
				}

				using (IsolatedStorageFileStream readerStream = _storage.OpenFile(_xmlFileName, FileMode.Open))
				{
					using (Stream writerStream = e.Result)
					{
						readerStream.CopyTo(writerStream);
					}
				}
			}
			catch (Exception ex)
			{
				ex.GetType();
			}
		}

		public void Save()
		{
			SaveDictionaryToFile();
			UploadSettingsFile();
		}

		private void SaveDictionaryToFile()
		{
			try
			{
				XDocument xml = SaveDictionaryToXml();
				using (IsolatedStorageFileStream writerStream = _storage.CreateFile(_xmlFileName))
				{
					SaveXmlToStream(writerStream, xml);
				}
			}
			catch (Exception ex)
			{
				ex.GetType();
			}
		}

		private void SaveXmlToStream(Stream writerStream, XDocument document)
		{
			try
			{
				using (XmlWriter writer = XmlWriter.Create(writerStream))
				{
					document.WriteTo(writer);
				}
			}
			catch (Exception ex)
			{
				ex.GetType();
			}
		}

		private XDocument SaveDictionaryToXml()
		{
			XDocument xml = new XDocument();
			XElement rootElement = new XElement("Settings");
			xml.Add(rootElement);
			foreach (string key in base.Keys)
			{
				string value;
				if (!base.TryGetValue(key, out value))
					value = null;

				XElement element = new XElement("Setting");
				rootElement.Add(element);
				element.SetAttributeValue("Key", key);
				element.SetAttributeValue("Value", value);
			}

			return xml;
		}

		private void LoadDictionaryFromFile()
		{
			try
			{
				if (_storage == null || !_storage.FileExists(_xmlFileName))
					return;

				using (IsolatedStorageFileStream readerStream = _storage.OpenFile(_xmlFileName, FileMode.Open))
				{
					LoadDictionaryFromStream(readerStream);
				}
			}
			catch (Exception ex)
			{
				ex.GetType();
			}
		}

		private void LoadDictionaryFromStream(Stream readerStream)
		{
			try
			{
				XDocument xml = XDocument.Load(readerStream);
				LoadDictionaryFromXml(xml);
			}
			catch (Exception ex)
			{
				ex.GetType();
			}
		}

		private void LoadDictionaryFromXml(XDocument xml)
		{
			try
			{
				XElement rootElement = xml.Root;
				XNode node = rootElement.FirstNode;
				while (node != null)
				{
					if (node.NodeType == XmlNodeType.Element)
					{
						try
						{
							XElement element = node as XElement;
							string key = element.AttributeValue("Key");
							string valueString = element.AttributeValue("Value");
							if (base.ContainsKey(key))
								base[key] = valueString;
							else
								base.Add(key, valueString);
						}
						catch (Exception ex)
						{
							ex.GetType();
						}
					}

					node = node.NextNode;
				}
			}
			catch (Exception ex)
			{
				ex.GetType();
			}
		}
	}
}
