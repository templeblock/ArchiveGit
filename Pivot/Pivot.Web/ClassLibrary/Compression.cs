using System;
using System.IO;
using System.Text;
#if SILVERLIGHT
	using Ionic.Zlib;
#else
	using System.IO.Compression;
#endif

namespace ClassLibrary
{
	public enum Compressor
	{
		Deflate = 0,
		GZip = 1,
	}

	public class Compression
	{
		public static string Compress(string inputString, Compressor compressor = Compressor.Deflate)
		{
			try
			{
				byte[] input = Encoding.UTF8.GetBytes(inputString);
				using (MemoryStream inmemoryStream = new MemoryStream(input))
				{
					using (MemoryStream outmemoryStream = new MemoryStream())
					{
						Stream zipStream = (compressor == Compressor.Deflate ?
							new DeflateStream(outmemoryStream, CompressionMode.Compress) as Stream : 
							new GZipStream(outmemoryStream, CompressionMode.Compress) as Stream);

						using (zipStream)
						{
							inmemoryStream.CopyTo(zipStream);
						}

						byte[] output = outmemoryStream.ToArray();
						return Convert.ToBase64String(output);
					}
				}
			}
			catch (Exception ex)
			{
				ex.GetType();
				return null;
			}
		}

		public static string Decompress(string inputString, Compressor compressor = Compressor.Deflate)
		{
			try
			{
				byte[] input = Convert.FromBase64String(inputString);
				using (MemoryStream inmemoryStream = new MemoryStream(input))
				{
					using (MemoryStream outmemoryStream = new MemoryStream())
					{
						Stream zipStream = (compressor == Compressor.Deflate ?
							new DeflateStream(inmemoryStream, CompressionMode.Decompress) as Stream :
							new GZipStream(inmemoryStream, CompressionMode.Decompress) as Stream);

						using (zipStream)
						{
							zipStream.CopyTo(outmemoryStream);
						}

						byte[] output = outmemoryStream.ToArray();
						return Encoding.UTF8.GetString(output, 0, output.Length);
					}
				}
			}
			catch (Exception ex)
			{
				ex.GetType();
				return null;
			}
		}
	}
}
