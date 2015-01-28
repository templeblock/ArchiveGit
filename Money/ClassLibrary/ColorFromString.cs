using System;
using System.Globalization;
using System.Reflection;
using System.Windows.Media;

namespace ClassLibrary
{
	[Obfuscation(Exclude = true)]
	internal static class ColorNames
	{
		public static Color AliceBlue			{ get { return ColorFromString.ToColor("#f0f8ff"); } }
		public static Color AntiqueWhite		{ get { return ColorFromString.ToColor("#faebd7"); } }
		public static Color Aqua				{ get { return ColorFromString.ToColor("#00ffff"); } }
		public static Color Aquamarine			{ get { return ColorFromString.ToColor("#7fffd4"); } }
		public static Color Azure				{ get { return ColorFromString.ToColor("#f0ffff"); } }
		public static Color Beige				{ get { return ColorFromString.ToColor("#f5f5dc"); } }
		public static Color Bisque				{ get { return ColorFromString.ToColor("#ffe4c4"); } }
		public static Color Black				{ get { return ColorFromString.ToColor("#000000"); } }
		public static Color BlanchedAlmond		{ get { return ColorFromString.ToColor("#ffebcd"); } }
		public static Color Blue				{ get { return ColorFromString.ToColor("#0000ff"); } }
		public static Color BlueViolet			{ get { return ColorFromString.ToColor("#8a2be2"); } }
		public static Color Brown				{ get { return ColorFromString.ToColor("#a52a2a"); } }
		public static Color Burlywood			{ get { return ColorFromString.ToColor("#deb887"); } }
		public static Color CadetBlue			{ get { return ColorFromString.ToColor("#5f9ea0"); } }
		public static Color Chartreuse			{ get { return ColorFromString.ToColor("#7fff00"); } }
		public static Color Chocolate			{ get { return ColorFromString.ToColor("#d2691e"); } }
		public static Color Coral				{ get { return ColorFromString.ToColor("#ff7f50"); } }
		public static Color CornflowerBlue		{ get { return ColorFromString.ToColor("#6495ed"); } }
		public static Color Cornsilk			{ get { return ColorFromString.ToColor("#fff8dc"); } }
		public static Color Crimson				{ get { return ColorFromString.ToColor("#dc143c"); } }
		public static Color Cyan				{ get { return ColorFromString.ToColor("#00ffff"); } }
		public static Color DarkBlue			{ get { return ColorFromString.ToColor("#00008b"); } }
		public static Color DarkCyan			{ get { return ColorFromString.ToColor("#008b8b"); } }
		public static Color DarkGoldenrod		{ get { return ColorFromString.ToColor("#b8860b"); } }
		public static Color DarkGray			{ get { return ColorFromString.ToColor("#a9a9a9"); } }
		public static Color DarkGreen			{ get { return ColorFromString.ToColor("#006400"); } }
		public static Color DarkKhaki			{ get { return ColorFromString.ToColor("#bdb76b"); } }
		public static Color DarkMagenta			{ get { return ColorFromString.ToColor("#8b008b"); } }
		public static Color DarkOliveGreen		{ get { return ColorFromString.ToColor("#556b2f"); } }
		public static Color DarkOrange			{ get { return ColorFromString.ToColor("#ff8c00"); } }
		public static Color DarkOrchid			{ get { return ColorFromString.ToColor("#9932cc"); } }
		public static Color DarkRed				{ get { return ColorFromString.ToColor("#8b0000"); } }
		public static Color DarkSalmon			{ get { return ColorFromString.ToColor("#e9967a"); } }
		public static Color DarkSeaGreen		{ get { return ColorFromString.ToColor("#8fbc8f"); } }
		public static Color DarkSlateBlue		{ get { return ColorFromString.ToColor("#483d8b"); } }
		public static Color DarkSlateGray		{ get { return ColorFromString.ToColor("#2f4f4f"); } }
		public static Color DarkTurquoise		{ get { return ColorFromString.ToColor("#00ced1"); } }
		public static Color DarkViolet			{ get { return ColorFromString.ToColor("#9400d3"); } }
		public static Color DeepPink			{ get { return ColorFromString.ToColor("#ff1493"); } }
		public static Color DeepSkyBlue			{ get { return ColorFromString.ToColor("#00bfff"); } }
		public static Color DimGray				{ get { return ColorFromString.ToColor("#696969"); } }
		public static Color DodgerBlue			{ get { return ColorFromString.ToColor("#1e90ff"); } }
		public static Color FireBrick			{ get { return ColorFromString.ToColor("#b22222"); } }
		public static Color FloralWhite			{ get { return ColorFromString.ToColor("#fffaf0"); } }
		public static Color ForestGreen			{ get { return ColorFromString.ToColor("#228b22"); } }
		public static Color Fuchsia				{ get { return ColorFromString.ToColor("#ff00ff"); } }
		public static Color Gainsboro			{ get { return ColorFromString.ToColor("#dcdcdc"); } }
		public static Color GhostWhite			{ get { return ColorFromString.ToColor("#f8f8ff"); } }
		public static Color Gold				{ get { return ColorFromString.ToColor("#ffd700"); } }
		public static Color Goldenrod			{ get { return ColorFromString.ToColor("#daa520"); } }
		public static Color Gray				{ get { return ColorFromString.ToColor("#808080"); } }
		public static Color Green				{ get { return ColorFromString.ToColor("#008000"); } }
		public static Color GreenYellow			{ get { return ColorFromString.ToColor("#adff2f"); } }
		public static Color Honeydew			{ get { return ColorFromString.ToColor("#f0fff0"); } }
		public static Color HotPink				{ get { return ColorFromString.ToColor("#ff69b4"); } }
		public static Color IndianRed			{ get { return ColorFromString.ToColor("#cd5c5c"); } }
		public static Color Indigo				{ get { return ColorFromString.ToColor("#4b0082"); } }
		public static Color Ivory				{ get { return ColorFromString.ToColor("#fffff0"); } }
		public static Color Khaki				{ get { return ColorFromString.ToColor("#f0e68c"); } }
		public static Color Lavender			{ get { return ColorFromString.ToColor("#e6e6fa"); } }
		public static Color LavenderBlush		{ get { return ColorFromString.ToColor("#fff0f5"); } }
		public static Color LawnGreen			{ get { return ColorFromString.ToColor("#7cfc00"); } }
		public static Color LemonChiffon		{ get { return ColorFromString.ToColor("#fffacd"); } }
		public static Color LightBlue			{ get { return ColorFromString.ToColor("#add8e6"); } }
		public static Color LightCoral			{ get { return ColorFromString.ToColor("#f08080"); } }
		public static Color LightCyan			{ get { return ColorFromString.ToColor("#e0ffff"); } }
		public static Color LightGoldenrodYellow{ get { return ColorFromString.ToColor("#fafad2"); } }
		public static Color LightGreen			{ get { return ColorFromString.ToColor("#90ee90"); } }
		public static Color LightGrey			{ get { return ColorFromString.ToColor("#d3d3d3"); } }
		public static Color LightPink			{ get { return ColorFromString.ToColor("#ffb6c1"); } }
		public static Color LightSalmon			{ get { return ColorFromString.ToColor("#ffa07a"); } }
		public static Color LightSeaGreen		{ get { return ColorFromString.ToColor("#20b2aa"); } }
		public static Color LightSkyBlue		{ get { return ColorFromString.ToColor("#87cefa"); } }
		public static Color LightSlateGray		{ get { return ColorFromString.ToColor("#778899"); } }
		public static Color LightSteelBlue		{ get { return ColorFromString.ToColor("#b0c4de"); } }
		public static Color LightYellow			{ get { return ColorFromString.ToColor("#ffffe0"); } }
		public static Color Lime				{ get { return ColorFromString.ToColor("#00ff00"); } }
		public static Color LimeGreen			{ get { return ColorFromString.ToColor("#32cd32"); } }
		public static Color Linen				{ get { return ColorFromString.ToColor("#faf0e6"); } }
		public static Color Magenta				{ get { return ColorFromString.ToColor("#ff00ff"); } }
		public static Color Maroon				{ get { return ColorFromString.ToColor("#800000"); } }
		public static Color MediumAquaMarine	{ get { return ColorFromString.ToColor("#66cdaa"); } }
		public static Color MediumBlue			{ get { return ColorFromString.ToColor("#0000cd"); } }
		public static Color MediumOrchid		{ get { return ColorFromString.ToColor("#ba55d3"); } }
		public static Color MediumPurple		{ get { return ColorFromString.ToColor("#9370db"); } }
		public static Color MediumSeaGreen		{ get { return ColorFromString.ToColor("#3cb371"); } }
		public static Color MediumSlateBlue		{ get { return ColorFromString.ToColor("#7b68ee"); } }
		public static Color MediumSpringGreen	{ get { return ColorFromString.ToColor("#00fa9a"); } }
		public static Color MediumTurquoise		{ get { return ColorFromString.ToColor("#48d1cc"); } }
		public static Color MediumVioletRed		{ get { return ColorFromString.ToColor("#c71585"); } }
		public static Color MidnightBlue		{ get { return ColorFromString.ToColor("#191970"); } }
		public static Color MintCream			{ get { return ColorFromString.ToColor("#f5fffa"); } }
		public static Color MistyRose			{ get { return ColorFromString.ToColor("#ffe4e1"); } }
		public static Color Moccasin			{ get { return ColorFromString.ToColor("#ffe4b5"); } }
		public static Color NavajoWhite			{ get { return ColorFromString.ToColor("#ffdead"); } }
		public static Color Navy				{ get { return ColorFromString.ToColor("#000080"); } }
		public static Color OldLace				{ get { return ColorFromString.ToColor("#fdf5e6"); } }
		public static Color Olive				{ get { return ColorFromString.ToColor("#808000"); } }
		public static Color OliveDrab			{ get { return ColorFromString.ToColor("#6b8e23"); } }
		public static Color Orange				{ get { return ColorFromString.ToColor("#ffa500"); } }
		public static Color OrangeRed			{ get { return ColorFromString.ToColor("#ff4500"); } }
		public static Color Orchid				{ get { return ColorFromString.ToColor("#da70d6"); } }
		public static Color PaleGoldenrod		{ get { return ColorFromString.ToColor("#eee8aa"); } }
		public static Color PaleGreen			{ get { return ColorFromString.ToColor("#98fb98"); } }
		public static Color PaleTurquoise		{ get { return ColorFromString.ToColor("#afeeee"); } }
		public static Color PaleVioletRed		{ get { return ColorFromString.ToColor("#db7093"); } }
		public static Color PapayaWhip			{ get { return ColorFromString.ToColor("#ffefd5"); } }
		public static Color PeachPuff			{ get { return ColorFromString.ToColor("#ffdab9"); } }
		public static Color Peru				{ get { return ColorFromString.ToColor("#cd853f"); } }
		public static Color Pink				{ get { return ColorFromString.ToColor("#ffc0cb"); } }
		public static Color Plum				{ get { return ColorFromString.ToColor("#dda0dd"); } }
		public static Color PowderBlue			{ get { return ColorFromString.ToColor("#b0e0e6"); } }
		public static Color Purple				{ get { return ColorFromString.ToColor("#800080"); } }
		public static Color Red					{ get { return ColorFromString.ToColor("#ff0000"); } }
		public static Color RosyBrown			{ get { return ColorFromString.ToColor("#bc8f8f"); } }
		public static Color RoyalBlue			{ get { return ColorFromString.ToColor("#4169e1"); } }
		public static Color SaddleBrown			{ get { return ColorFromString.ToColor("#8b4513"); } }
		public static Color Salmon				{ get { return ColorFromString.ToColor("#fa8072"); } }
		public static Color SandyBrown			{ get { return ColorFromString.ToColor("#f4a460"); } }
		public static Color SeaGreen			{ get { return ColorFromString.ToColor("#2e8b57"); } }
		public static Color SeaShell			{ get { return ColorFromString.ToColor("#fff5ee"); } }
		public static Color Sienna				{ get { return ColorFromString.ToColor("#a0522d"); } }
		public static Color Silver				{ get { return ColorFromString.ToColor("#c0c0c0"); } }
		public static Color SkyBlue				{ get { return ColorFromString.ToColor("#87ceeb"); } }
		public static Color SlateBlue			{ get { return ColorFromString.ToColor("#6a5acd"); } }
		public static Color SlateGray			{ get { return ColorFromString.ToColor("#708090"); } }
		public static Color Snow				{ get { return ColorFromString.ToColor("#fffafa"); } }
		public static Color SpringGreen			{ get { return ColorFromString.ToColor("#00ff7f"); } }
		public static Color SteelBlue			{ get { return ColorFromString.ToColor("#4682b4"); } }
		public static Color Tan					{ get { return ColorFromString.ToColor("#d2b48c"); } }
		public static Color Teal				{ get { return ColorFromString.ToColor("#008080"); } }
		public static Color Thistle				{ get { return ColorFromString.ToColor("#d8bfd8"); } }
		public static Color Tomato				{ get { return ColorFromString.ToColor("#ff6347"); } }
		public static Color Turquoise			{ get { return ColorFromString.ToColor("#40e0d0"); } }
		public static Color Violet				{ get { return ColorFromString.ToColor("#ee82ee"); } }
		public static Color Wheat				{ get { return ColorFromString.ToColor("#f5deb3"); } }
		public static Color White				{ get { return ColorFromString.ToColor("#ffffff"); } }
		public static Color WhiteSmoke			{ get { return ColorFromString.ToColor("#f5f5f5"); } }
		public static Color Yellow				{ get { return ColorFromString.ToColor("#ffff00"); } }
		public static Color YellowGreen			{ get { return ColorFromString.ToColor("#9acd32"); } }
	}

	internal static class ColorFromString
	{
		// Extension for string
		internal static Color ToColor(this string value)
		{
			if (value == null)
				return Colors.Red;

			if (value[0] != '#')
			{
				// Named Colors
				string valueLower = value.ToLower();
				PropertyInfo[] properties = typeof(ColorNames).GetProperties(BindingFlags.Static | BindingFlags.Public);
				foreach (PropertyInfo property in properties)
				{
					if (property.Name.ToLower() == valueLower)
						return (Color)property.GetValue(null, null);
				}
			}

			// #ARGB and #RGB Hex Colors
			if (value[0] == '#')
				value = value.Remove(0, 1);

			int length = value.Length;
			if ((length == 6 || length == 8) && IsHexColor(value))
			{
				if (length == 8)
					return Color.FromArgb(
						byte.Parse(value.Substring(0, 2), NumberStyles.HexNumber), 
						byte.Parse(value.Substring(2, 2), NumberStyles.HexNumber), 
						byte.Parse(value.Substring(4, 2), NumberStyles.HexNumber), 
						byte.Parse(value.Substring(6, 2), NumberStyles.HexNumber));

				if (length == 6)
					return Color.FromArgb(0xff, 
						byte.Parse(value.Substring(0, 2), NumberStyles.HexNumber), 
						byte.Parse(value.Substring(2, 2), NumberStyles.HexNumber), 
						byte.Parse(value.Substring(4, 2), NumberStyles.HexNumber));
			}

			// A,R,G,B and R,G,B Decimal Colors
			string[] argb = value.Split(new char[] { ',', ' ' }, StringSplitOptions.RemoveEmptyEntries);
			if (argb != null)
			{
				if (argb.Length == 4)
					return Color.FromArgb(byte.Parse(argb[0]), byte.Parse(argb[1]), byte.Parse(argb[2]), byte.Parse(argb[3]));

				if (argb.Length == 3)
					return Color.FromArgb(0xff, byte.Parse(argb[0]), byte.Parse(argb[1]), byte.Parse(argb[2]));
			}

			return Colors.Red;
		}

		private static bool IsHexColor(string value)
		{
			if (value == null)
				return false;

			foreach (char c in value.ToCharArray())
				if (!Uri.IsHexDigit(c))
					return false;

			return true;
		}
	}
}


































































		














	




	
	
	
	


















































