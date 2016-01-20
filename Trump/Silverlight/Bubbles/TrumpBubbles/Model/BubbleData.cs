using System.Collections.Generic;
using ClassLibrary;

namespace TrumpBubbles
{
	public class NormalBubble : NotifyPropertyChanges
	{
		public string Title { get { return _Title; } set { SetProperty(ref _Title, value); } }
		private string _Title;

		public string IconSourceUri { get { return _IconSourceUri; } set { SetProperty(ref _IconSourceUri, value); } }
		private string _IconSourceUri;

		public string TextContent { get { return _TextContent; } set { SetProperty(ref _TextContent, value); } }
		private string _TextContent;

		public object ItemContent { get { return _ItemContent; } set { SetProperty(ref _ItemContent, value); } }
		private object _ItemContent;

		public string ContentTag { get { return _ContentTag; } set { SetProperty(ref _ContentTag, value); } }
		private string _ContentTag;

		public string SourceUrl { get { return _SourceUrl; } set { SetProperty(ref _SourceUrl, value); } }
		private string _SourceUrl;

		public double HtmlHeight { get { return _HtmlHeight; } set { SetProperty(ref _HtmlHeight, value); } }
		private double _HtmlHeight;

		public static List<NormalBubble> BuildData()
		{
			List<NormalBubble> list = new List<NormalBubble>()
			{
				new NormalBubble() {
					Title = "Manage Household" ,
					IconSourceUri = "../Images/Bubble1.png" ,
					TextContent = "Add or edit your household members and companies as well as change household login information.",
					ContentTag = "x_Content1",
					SourceUrl = "./Account/ManageMembers.aspx",
					HtmlHeight = 725,
				    ItemContent = null,
				},
				new NormalBubble() {
				    Title = "Manage Settings" ,
				    IconSourceUri = "../Images/Bubble2.png" ,
				    TextContent = "Control your account settings: newsletter E-mail opt in/out, add AutoShip Discounting to your personal AutoShips and change your password. ",
				    ContentTag = "x_Content2",
				    SourceUrl = "http://www.unitedinfoxchange.com/Store/Default.aspx",
				    HtmlHeight = 550,
				    ItemContent = null,
				},
			};
			return list;
		}
	}

	public class TestAccountPageData : NotifyPropertyChanges
	{
		public List<NormalBubble> NormalBubbleData { get { return _NormalBubbleData; } set { SetProperty(ref _NormalBubbleData, value); } }
		private List<NormalBubble> _NormalBubbleData = NormalBubble.BuildData();
	}
}
