using System.Reflection;

namespace Money
{
	public class DeepSeaTest
	{
		public void TestPublic(DeepSeaTest arg)
		{
			TestPublic(null);
		}

		[Obfuscation(Exclude = false)]
		public void TestPublicExcludeFalse(DeepSeaTest arg)
		{
			TestPublicExcludeFalse(null);
		}

		[Obfuscation(Exclude = true)]
		public void TestPublicExcludeTrue(DeepSeaTest arg)
		{
			TestPublicExcludeTrue(null);
		}

		internal void TestInternal(DeepSeaTest arg)
		{
			TestInternal(null);
		}

		[Obfuscation(Exclude = false)]
		internal void TestInternalExcludeFalse(DeepSeaTest arg)
		{
			TestInternalExcludeFalse(null);
		}

		[Obfuscation(Exclude = true)]
		internal void TestInternalExcludeTrue(DeepSeaTest arg)
		{
			TestInternalExcludeTrue(null);
		}
	}
}
