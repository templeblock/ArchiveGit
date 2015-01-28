using System.Windows.Browser;

namespace ClassLibrary
{
	internal class HtmlPageInfo
	{
		internal static void Set(string keywordText, string titleText, string descriptionText)
		{
			ScriptObjectCollection soc = HtmlPage.Document.GetElementsByTagName("Title");
			if (soc == null || soc.Count <= 0)
				return;

			HtmlElement titleElement = (HtmlElement)soc[0];
			//string currentTitle = (string)titleElement.GetProperty("innerHTML");
			HtmlPage.Document.SetProperty("title", titleText);

			HtmlElement head = HtmlPage.Document.GetElementsByTagName("head")[0] as HtmlElement;
			HtmlElement keywords = HtmlPage.Document.CreateElement("meta");
			keywords.SetAttribute("name=\"keywords\" content", keywordText);
			keywords.AppendChild(keywords);

			HtmlElement description = HtmlPage.Document.CreateElement("meta");
			description.SetAttribute("name=\"description\" content", descriptionText);
			head.AppendChild(description);
		}
	}
}
