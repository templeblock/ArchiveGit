using System;

namespace ClassLibrary
{
	public static class TypeNameExtension
	{
		public static string FriendlyName(this Type type)
		{
			string typeName = type.ToString();
			ReplaceAny(ref typeName, "System.Void", "void");
			ReplaceAny(ref typeName, "System.Object", "object");
			ReplaceAny(ref typeName, "System.String", "string");
			ReplaceAny(ref typeName, "System.Double", "double");
			ReplaceAny(ref typeName, "System.Int32", "int");
			ReplaceAny(ref typeName, "System.Int64", "long");
			ReplaceAny(ref typeName, "System.Boolean", "bool");
			ReplaceAny(ref typeName, "System.Char", "char");
			if (ReplaceBeginning(ref typeName, "System.Nullable`1[", ""))
				typeName = typeName.Replace(']', '?');

			if (ReplaceAny(ref typeName, "`1[", "<") ||
				ReplaceAny(ref typeName, "`2[", "<"))
			{
				typeName = typeName.Replace("]", ">");
				string[] array = typeName.Split('<');
				for (int i = 0; i < array.Length; i++)
				{
					string item = array[i];
					array[i] = item.Substring(item.LastIndexOf('.') + 1);
				}
				typeName = string.Join("<", array);
			}
			else
				typeName = typeName.Substring(typeName.LastIndexOf('.') + 1);

			return typeName;
		}

		private static bool ReplaceAny(ref string text, string search, string replace)
		{
			if (text.IndexOf(search) < 0)
				return false;
			text = text.Replace(search, replace);
			return true;
		}

		private static bool ReplaceBeginning(ref string text, string search, string replace)
		{
			if (text.IndexOf(search) != 0)
				return false;
			text = replace + text.Substring(search.Length);
			return true;
		}
	}
}
