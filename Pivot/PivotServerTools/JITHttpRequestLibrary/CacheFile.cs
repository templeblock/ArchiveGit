using System;
using System.Collections.Generic;
using System.IO;
using System.Net;
using System.Web;
using TileLibrary;

namespace JITHttpRequestLibrary
{
	internal class CacheFile
	{
		private static readonly Dictionary<string, object> _imageLocks = new Dictionary<string, object>();
		private static readonly object _sync = new object();

		public static void Save(MemoryStream memoryStream, string filePath)
		{
			CachePrep(ref filePath);
			lock (GetImageLock(filePath))
			{
				try
				{
					using (FileStream fileStream = new FileStream(filePath, FileMode.OpenOrCreate))
					{
						memoryStream.Position = 0; // Rewind the stream, ready for reading
						memoryStream.WriteTo(fileStream);
					}
				}
				catch (Exception ex)
				{
					ex.GetType();
				}

				RemoveImageLock(filePath);
			}
		}

		public static void Serve(string filePath, HttpContext context, string contentType)
		{
			CachePrep(ref filePath);
			lock (GetImageLock(filePath))
			{
				try
				{
					using (FileStream fileStream = new FileStream(filePath, FileMode.Open))
					{
						using (MemoryStream memoryStream = new MemoryStream())
						{
							memoryStream.SetLength(fileStream.Length);
							fileStream.Read(memoryStream.GetBuffer(), 0, (int)fileStream.Length);
							memoryStream.WriteTo(context.Response.OutputStream);
							context.Response.ContentType = contentType;
						}
					}
				}
				catch (Exception ex)
				{
					ex.GetType();
					//The requested resource doesn't exist. Return HTTP status code 404.
					context.Response.StatusCode = (int)HttpStatusCode.NotFound;
					context.Response.StatusDescription = "Resource not found.";
				}

				RemoveImageLock(filePath);
			}
		}

		public static bool Exists(string filePath)
		{
			try
			{
				CachePrep(ref filePath);
				return File.Exists(filePath);
			}
			catch (Exception ex)
			{
				ex.GetType();
				return false;
			}
		}

		public static void Delete(string filePath)
		{
			CachePrep(ref filePath);
			lock (GetImageLock(filePath))
			{
				try
				{
					File.Delete(filePath);
				}
				catch (Exception ex)
				{
					ex.GetType();
				}

				RemoveImageLock(filePath);
			}
		}

		public static string CachePrep(ref string filePath)
		{
			filePath = filePath.Replace(".Web", ".Web\\Cache");
			return ImageProvider.FolderPrep(filePath);
		}

		private static object GetImageLock(string fileName)
		{
			lock (_sync)
			{
				object imageLock;
				if (_imageLocks.ContainsKey(fileName))
					imageLock = _imageLocks[fileName];
				else
				{
					imageLock = new object();
					_imageLocks.Add(fileName, imageLock);
				}

				return imageLock;
			}
		}

		private static void RemoveImageLock(string fileName)
		{
			lock (_sync)
			{
				_imageLocks.Remove(fileName);
			}
		}
	}
}
