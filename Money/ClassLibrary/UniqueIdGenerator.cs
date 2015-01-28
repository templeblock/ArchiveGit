using System;
using System.Security.Cryptography;
using System.Collections.Generic;
using System.Diagnostics;

namespace ClassLibrary
{
	internal class UniqueIdGenerator
	{ // http://jopinblog.wordpress.com/2009/02/04/a-shorter-friendlier-guiduuid-in-net/

		private RNGCryptoServiceProvider m_provider = new RNGCryptoServiceProvider();
		private static readonly UniqueIdGenerator m_instance = new UniqueIdGenerator();
		private static char[] m_charMap = { // 0, 1, O, and I omitted intentionally giving 32 (2^5) symbols
			'2', '3', '4', '5', '6', '7', '8', '9',
			'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H',
			'J', 'K', 'L', 'M', 'N', 'P', 'Q', 'R',
			'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z'
			};

		public static UniqueIdGenerator GetInstance()
		{
			return m_instance;
		}

		private UniqueIdGenerator()
		{
		}

		public void GetNext(byte[] bytes)
		{
			m_provider.GetBytes(bytes);
		}

		public string GetBase32UniqueId(int numDigits)
		{
			return GetBase32UniqueId(new byte[0], numDigits);
		}

		public string GetBase32UniqueId(byte[] basis, int numDigits)
		{
			int byteCount = 16;
			byte[] randBytes = new byte[byteCount - basis.Length];
			GetNext(randBytes);
			byte[] bytes = new byte[byteCount];
			Array.Copy(basis, 0, bytes, byteCount - basis.Length, basis.Length);
			Array.Copy(randBytes, 0, bytes, 0, randBytes.Length);

			ulong lo = (((ulong)BitConverter.ToUInt32(bytes, 8)) << 32) | BitConverter.ToUInt32(bytes, 12);	// BitConverter.ToUInt64(bytes, 8);
			ulong hi = (((ulong)BitConverter.ToUInt32(bytes, 0)) << 32) | BitConverter.ToUInt32(bytes, 4);	// BitConverter.ToUInt64(bytes, 0);
			ulong mask = 0x1F;

			char[] chars = new char[26];
			int charIdx = 25;

			ulong work = lo;
			for (int i = 0; i < 26; i++)
			{
				if (i == 12)
					work = ((hi & 0x01) << 4) & lo;
				else
				if (i == 13)
					work = hi >> 1;

				byte digit = (byte)(work & mask);
				chars[charIdx] = m_charMap[digit];
				charIdx--;
				work = work >> 5;
			}

			string ret = new string(chars, 26 - numDigits, numDigits);
			return ret;
		}
	}

	internal class UniqueIdGeneratorTest
	{
		public void GetInstanceTest()
		{
			var instance = UniqueIdGenerator.GetInstance();
			//Debug.Assert.AreSame(instance, UniqueIdGenerator.GetInstance());
		}

		public void GetNextTest()
		{
			var b1 = new byte[16];
			for (int i = 0; i < 16; i++ )
				b1[i] = 0;
			UniqueIdGenerator.GetInstance().GetNext(b1);
			//Debug.Assert.That(Array.Exists(b1, b => b != 0)); // This could be false every billion years or so
		}

		public void GetBase32UniqueIdTest()
		{
			var b1 = new byte[16];
			for (int i = 0; i < 16; i++)
				b1[i] = 0;
			string id = UniqueIdGenerator.GetInstance().GetBase32UniqueId(b1, 26);
			//Debug.Assert.AreEqual(26, id.Length);
			//Debug.Assert.AreEqual(new string('2', 26), id);

			b1 = new byte[] { 0xFF, 0xFF, 0xFF, 0xFF };
			id = UniqueIdGenerator.GetInstance().GetBase32UniqueId(b1, 26);
			Debug.WriteLine(id);
			//Debug.Assert.AreEqual(26, id.Length);
			//Debug.Assert.AreEqual("ZZZZZZ", id.Substring(20, 6));

			id = UniqueIdGenerator.GetInstance().GetBase32UniqueId(b1, 6);
			Debug.WriteLine(id);
			//Debug.Assert.AreEqual(6, id.Length);
			//Debug.Assert.AreEqual("ZZZZZZ", id);

			id = UniqueIdGenerator.GetInstance().GetBase32UniqueId(18);
			Debug.WriteLine(id);
			//Debug.Assert.AreEqual(18, id.Length);

			var id2 = UniqueIdGenerator.GetInstance().GetBase32UniqueId(18);
			Debug.WriteLine(id2);
			//Debug.Assert.AreEqual(18, id2.Length);
			//Debug.Assert.AreNotEqual(id, id2);
		}

		public void GetBase32UniqueIdDupTest()
		{
			var alreadySeen = new Dictionary<string, string>(1000000);
			Debug.WriteLine("Allocated");
			for (int i = 0; i < 100000000; i++)
			{
				var id = UniqueIdGenerator.GetInstance().GetBase32UniqueId(12);
				//Debug.Assert.That(!alreadySeen.ContainsKey(id));
				alreadySeen.Add(id, id);
			}
		}
	}
}
