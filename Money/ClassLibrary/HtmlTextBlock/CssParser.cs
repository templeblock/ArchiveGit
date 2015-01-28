using System.Collections.Generic;

namespace ClassLibrary
{
	internal class CssStyles : Dictionary<string, string>
	{
		internal static CssStyles Create(string style)
		{
			CssStyles styles = new CssStyles();
			styles.AddFromText(style);
			return styles;
		}

		public void Dispose()
		{
			Clear();
		}

		internal void AddFromText(string text)
		{
			if (text == null)
				return;

			string[] blocks = text.Split(';');
			if (blocks.Length <= 0)
				return;

			foreach (string block in blocks)
			{
				string[] pair = block.Split(':');
				if (pair.Length == 2)
				{
					string style = pair[0].Trim().ToLower();
					string value = pair[1].Trim();
					if (base.ContainsKey(style))
						base[style] = value;
					else
						base.Add(style, value);
				}
			}
		}

		internal string Get(string style)
		{
			style = style.ToLower();
			if (!this.ContainsKey(style))
				return string.Empty;

			string value = this[style];
			if (value == null)
				return string.Empty;

			return value;
		}
	}

	internal class CssSelectors : Dictionary<string, string>
	{
		internal static CssSelectors Create(string selector)
		{
			CssSelectors selectors = new CssSelectors();
			selectors.AddFromText(selector);
			return selectors;
		}

		public void Dispose()
		{
			Clear();
		}

		internal void AddFromText(string text)
		{
			if (text == null)
				return;

			text = RegexHelper.ReplaceCStyleComments(text);
			string[] blocks = text.Split('}');
			if (blocks.Length <= 0)
				return;

			foreach (string block in blocks)
			{
				string[] pair = block.Split('{');
				if (pair.Length == 2)
				{
					string selectorText = pair[0].ToLower();
					string styleText = pair[1];

					selectorText = RegexHelper.ReplacePattern(selectorText, @"(\[[^\[\]]*?\])", ""); // Ignore attribute sets in selectors
					string[] selectors = selectorText.Split(',');
					foreach (string selector in selectors)
					{
						string key = selector.Trim();
						if (base.ContainsKey(key))
							base[key] += ";" + styleText;
						else
							base.Add(key, styleText);
					}
				}
			}
		}

		internal CssStyles GetStyles(string selector)
		{
			selector = selector.ToLower();
			return CssStyles.Create(base.ContainsKey(selector) ? base[selector] : null);
		}
	}

	internal class CssParser
	{
		internal static double ParseFontSize(string text, double size)
		{
			int i = 0;
			// units == px
			i = text.IndexOf("px");
			if (i >= 0) return (text.Substring(0, i).ToDouble());
			// units == pt; 12pt = 16px
			i = text.IndexOf("pt");
			if (i >= 0) return (text.Substring(0, i).ToDouble()) * 1.33333;
			// units == em;  1em == 100%
			i = text.IndexOf("em");
			if (i >= 0) return (text.Substring(0, i).ToDouble()) * size;
			// units == %
			i = text.IndexOf("%");
			if (i >= 0) return (text.Substring(0, i).ToDouble() / 100) * size;
			// No units == px
			double value = text.ToDouble();
			if (value > 0) return value;

			if (text == "xx-large") return 32;
			if (text == "x-large")	return 24;
			if (text == "large")	return 18;
			if (text == "medium")	return 16;
			if (text == "small")	return 14;
			if (text == "x-small")	return 10;
			if (text == "xx-small")	return 8;
			if (text == "7")		return 48;
			if (text == "6")		return 32;
			if (text == "5")		return 24;
			if (text == "4")		return 18;
			if (text == "3")		return 16;
			if (text == "2")		return 12;
			if (text == "1")		return 10;
			if (text == "0")		return 8;
			if (text == "h1")		return 32;
			if (text == "h2")		return 24;
			if (text == "h3")		return 18;
			if (text == "h4")		return 16;
			if (text == "h5")		return 14;
			if (text == "h6")		return 10;

			return size;
		}
	}
}
