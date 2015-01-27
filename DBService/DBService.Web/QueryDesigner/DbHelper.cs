namespace QueryDesigner
{
	internal class DbHelper
	{
		public static string BracketName(string name)
		{
			// already bracketed
			if (name.Length > 1 && name[0] == '[' && name[name.Length - 1] == ']')
				return name;

			// see if brackets are needed (never bracket expressions)
			bool needsBrackets = false; // force
			if (!IsExpression(name))
			{
				for (int i = 0; i < name.Length && !needsBrackets; i++)
				{
					char c = name[i];
					needsBrackets = i == 0
						? !char.IsLetter(c)
						: !char.IsLetterOrDigit(c) && c != '_';
				}
			}

			return (needsBrackets ? string.Format("[{0}]", name) : name);
		}

		static readonly char[] ExpressionChars = "(),*".ToCharArray();
		private static bool IsExpression(string name)
		{
			return (name.IndexOfAny(ExpressionChars) >= 0);
		}
	}
}
