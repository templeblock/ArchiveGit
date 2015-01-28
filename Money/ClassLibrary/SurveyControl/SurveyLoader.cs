using System;
using System.Collections.Generic;
using System.Windows;
using System.Windows.Controls;
using System.Xml;

namespace ClassLibrary
{
	internal class SurveyPanels
	{
		private SurveyLoader m_SurveyLoader;
		private List<StackPanel> m_GroupPanelList;
		private int m_iCurrentPanel;
		private Panel m_ParentPanel;
		private Dictionary<string, object> m_Dictionary;

		public string BackText { get; set; }
		public string NextText { get; set; }

		public SurveyPanels(Panel parentPanel, string strSurveyFile, Dictionary<string, object> dictionary)
		{
			m_ParentPanel = parentPanel;
			m_Dictionary = dictionary;
			CreatePanels(strSurveyFile);
		}

		internal void CreatePanels(string strSurveyFile)
		{
			if (m_SurveyLoader != null)
				return;

			//strSurveyFile = HttpContext.Current.Server.MapPath(strSurveyFile);
			m_SurveyLoader = new SurveyLoader(strSurveyFile);
			m_GroupPanelList = new List<StackPanel>();
			List<GroupBase> groups = m_SurveyLoader.Survey.GroupList;
			foreach (GroupBase group in groups)
			{
				StackPanel panel = new StackPanel();
				panel.Visibility = Visibility.Collapsed;
				panel.Orientation = Orientation.Vertical;
				panel.Tag = group.Name;
				m_ParentPanel.Children.Add(panel);
				m_GroupPanelList.Add(panel);

				List<QuestionBase> questions = group.QuestionList;
				foreach (QuestionBase question in questions)
				{
					StackPanel panelQuestion = new StackPanel();
					panelQuestion.Orientation = question.StatementLayoutDirection;
					panel.Children.Add(panelQuestion);
					CreateControl(question, panelQuestion);
				}
			}

			m_iCurrentPanel = 0;
		}

		private static void CreateControl(QuestionBase question, StackPanel panel)
		{
			if (question.Hidden)
				return;

			QuestionControl control;
			switch (question.Type)
			{
				case "text":
					control = new QuestionText(question, panel);
					break;
				case "password":
					control = new QuestionPassword(question, panel);
					break;
				case "date":
					control = new QuestionDate(question, panel);
					break;
				case "yesno":
					control = new QuestionYesNo(question, panel);
					break;
				case "state":
					control = new QuestionState(question, panel);
					break;
				case "chooseone":
					control = new QuestionChooseOne(question, panel);
					break;
				case "chooseany":
					control = new QuestionChooseAny(question, panel);
					break;
				case "noresponse":
					control = new QuestionControl(question, panel);
					break;
				default:
					control = new QuestionControl(question, panel);
					break;
			}
		}

		internal void ActivatePanel(int iPanelIncrement)
		{
			int iNewPanel = m_iCurrentPanel + iPanelIncrement;
			if (iNewPanel < 0 || iNewPanel >= m_GroupPanelList.Count)
				return;

			// Hide the current panel if necessary
			if (m_iCurrentPanel >= 0 && m_iCurrentPanel < m_GroupPanelList.Count && m_iCurrentPanel != iNewPanel)
				m_GroupPanelList[m_iCurrentPanel].Visibility = Visibility.Collapsed;

			m_iCurrentPanel = iNewPanel;
			m_GroupPanelList[m_iCurrentPanel].Visibility = Visibility.Visible;
			List<GroupBase> groups = m_SurveyLoader.Survey.GroupList;
			GroupBase group = groups[m_iCurrentPanel];
			if (group == null)
				return;

			BackText = group.Back;
			NextText = group.Next;
		}

		private GroupBase FindGroup(string id)
		{
			List<GroupBase> groups = m_SurveyLoader.Survey.GroupList;
			foreach (GroupBase group in groups)
			{
				if (group.Name == id)
					return group;
			}

			return null;
		}
	}

	internal class SurveyLoader
	{
		public SurveyBase Survey { get { return m_survey; } set { m_survey = value; } }
		private SurveyBase m_survey;

		public bool Loaded { get { return m_bLoaded; } }
		private readonly bool m_bLoaded;

		private readonly XmlReader m_xmlReader;
		private bool m_bSkipRead;

		public SurveyLoader(string surveyFile)
		{
#if NOTUSED
			Uri path = Helper.SitePath(surveyFile);
			HttpWebRequest request = HttpWebRequest.Create(path) as HttpWebRequest;
			request.BeginGetResponse(new AsyncCallback(responseHandler), request);
		}

		void responseHandler(IAsyncResult asyncResult)
		{
			try
			{
				HttpWebRequest request = asyncResult.AsyncState as HttpWebRequest;
				HttpWebResponse response = request.EndGetResponse(asyncResult) as HttpWebResponse;
				m_document = XmlReader.Create(response.GetResponseStream(), settings);
#else
#endif
			try
			{
				m_xmlReader = ResourceFile.GetXmlReader(surveyFile, AssemblySource.App);

				// Parse the file
				while (ReadNode())
					LoadSurvey(this);
			}
			catch (Exception ex)
			{
				//ex.DebugOutput();
				ex.Alert();
			}

			m_xmlReader.Close();
			m_bLoaded = true;
		}

		private bool LoadSurvey(SurveyLoader surveyLoader)
		{
			const string nodeName = "Survey";
			if (!MatchingElementStart(nodeName))
				return false;

			// Process any attributes
			SurveyBase survey = new SurveyBase(m_xmlReader);
			surveyLoader.Survey = survey;

			// Process any children
			if (m_xmlReader.IsEmptyElement)
				return true;

			while (ReadNode())
			{
				if (MatchingElementEnd(nodeName))
					break;

				if (LoadGroup(survey))
					continue;
			}

			return true;
		}

		private bool LoadGroup(SurveyBase survey)
		{
			const string nodeName = "Group";
			if (!MatchingElementStart(nodeName))
				return false;

			// Process any attributes
			GroupBase group = new GroupBase(m_xmlReader);
			survey.GroupList.Add(group);

			// Process any children
			if (m_xmlReader.IsEmptyElement)
				return true;

			while (ReadNode())
			{
				if (MatchingElementEnd(nodeName))
					break;

				if (LoadQuestion(group))
					continue;
			}

			return true;
		}

		private bool LoadQuestion(GroupBase group)
		{
			const string nodeName = "Question";
			if (!MatchingElementStart(nodeName))
				return false;

			// Process any attributes
			QuestionBase question = new QuestionBase(m_xmlReader);
			group.QuestionList.Add(question);

			// Process any children
			if (m_xmlReader.IsEmptyElement)
				return true;

			while (ReadNode())
			{
				if (MatchingElementEnd(nodeName))
					break;

				if (LoadStatement(question))
					continue;

				if (LoadResponses(question))
					continue;
			}

			return true;
		}

		private bool LoadStatement(QuestionBase question)
		{
			const string nodeName = "Statement";
			if (!MatchingElementStart(nodeName))
				return false;

			// Process any attributes

			// Process any children
			if (m_xmlReader.IsEmptyElement)
				return true;

			question.Statement = ReadInnerXml();
			return true;
		}

		private bool LoadResponses(QuestionBase question)
		{
			const string nodeName = "Responses";
			if (!MatchingElementStart(nodeName))
				return false;

			// Process any attributes

			// Process any children
			if (m_xmlReader.IsEmptyElement)
				return true;

			while (ReadNode())
			{
				if (MatchingElementEnd(nodeName))
					break;

				if (LoadResponse(question))
					continue;
			}

			return true;
		}

		private bool LoadResponse(QuestionBase question)
		{
			const string nodeName = "Response";
			if (!MatchingElementStart(nodeName))
				return false;

			// Process any attributes

			// Process any children
			if (m_xmlReader.IsEmptyElement)
				return true;

			question.ResponseList.Add(ReadInnerXml());
			return true;
		}

		private bool ReadNode()
		{
			if (m_bSkipRead)
			{
				m_bSkipRead = false;
				return true;
			}

			return m_xmlReader.Read();
		}

		private string ReadInnerXml()
		{
			m_bSkipRead = true;
			return m_xmlReader.ReadInnerXml();
		}

		private bool MatchingElementStart(string nodeName)
		{
			return (m_xmlReader.LocalName == nodeName && m_xmlReader.NodeType == XmlNodeType.Element);
		}

		private bool MatchingElementEnd(string nodeName)
		{
			return (m_xmlReader.LocalName == nodeName && m_xmlReader.NodeType == XmlNodeType.EndElement);
		}
	}

	internal class SurveyBase
	{
		public List<GroupBase> GroupList { get { return m_groupList; } }
		private readonly List<GroupBase> m_groupList = new List<GroupBase>();

		public SurveyBase(XmlReader node)
		{
			if (node.LocalName != "Survey")
				return;
		}
	}

	internal class GroupBase
	{
		public string Name { get { return m_Name; } }
		private readonly string m_Name;

		public string Next { get { return m_Next; } }
		private readonly string m_Next;

		public string Back { get { return m_Back; } }
		private readonly string m_Back;

		public List<QuestionBase> QuestionList { get { return m_QuestionList; } set { m_QuestionList = value; } }
		private List<QuestionBase> m_QuestionList = new List<QuestionBase>();

		public GroupBase(XmlReader node)
		{
			if (node.LocalName != "Group")
				return;

			string attribute = node.GetAttribute("name");
			m_Name = (attribute ?? "");

			attribute = node.GetAttribute("next");
			m_Next = (attribute ?? "");

			attribute = node.GetAttribute("back");
			m_Back = (attribute ?? "");
		}
	}

	internal class QuestionBase
	{
		public string Name { get { return m_Name; } }
		private readonly string m_Name;

		public string Type { get { return m_Type; } }
		private readonly string m_Type;

		public string RequiredText { get { return m_RequiredText; } }
		private readonly string m_RequiredText;

		public string ResponseDefault { get { return m_ResponseDefault; } }
		private readonly string m_ResponseDefault;

		public string Image { get { return m_sImage; } }
		private readonly string m_sImage;

		public string Statement { get { return m_Statement; } set { m_Statement = value; } }
		private string m_Statement;

		public int Cols { get { return m_nCols; } }
		private readonly int m_nCols;

		public int Rows { get { return m_nRows; } }
		private readonly int m_nRows;

		public int MaxChars { get { return m_nMaxChars; } }
		private readonly int m_nMaxChars;

		public int Indent { get { return m_nIndent; } }
		private readonly int m_nIndent;

		public bool Hidden { get { return m_bHidden; } }
		private readonly bool m_bHidden;

		public bool List { get { return m_bList; } }
		private readonly bool m_bList;

		public bool Required { get { return m_bRequired; } }
		private readonly bool m_bRequired;

		public Orientation ResponseLayoutDirection { get { return m_ResponseLayoutDirection; } }
		private readonly Orientation m_ResponseLayoutDirection;

		public Orientation StatementLayoutDirection { get { return m_QuestionLayoutDirection; } }
		private readonly Orientation m_QuestionLayoutDirection;

		public List<string> ResponseList { get { return m_ResponseList; } set { m_ResponseList = value; } }
		private List<string> m_ResponseList = new List<string>();

		public QuestionBase(XmlReader node)
		{
			if (node.LocalName != "Question")
				return;

			string attribute = node.GetAttribute("name");
			m_Name = (attribute ?? "");

			attribute = node.GetAttribute("type");
			m_Type = (attribute ?? "");

			m_nCols = 20;
			attribute = node.GetAttribute("cols");
			if (attribute != null)
				m_nCols = int.Parse(attribute);

			m_nRows = 1;
			attribute = node.GetAttribute("rows");
			if (attribute != null)
				m_nRows = int.Parse(attribute);

			m_nMaxChars = 0;
			attribute = node.GetAttribute("maxchars");
			if (attribute != null)
				m_nMaxChars = int.Parse(attribute);

			m_nIndent = 0;
			attribute = node.GetAttribute("indent");
			if (attribute != null)
				m_nIndent = int.Parse(attribute);

			m_QuestionLayoutDirection = Orientation.Horizontal;
			attribute = node.GetAttribute("questionlayout");
			if (attribute != null)
				m_QuestionLayoutDirection = (attribute == "vertical") ? Orientation.Vertical : Orientation.Horizontal;

			m_ResponseLayoutDirection = Orientation.Horizontal;
			attribute = node.GetAttribute("responselayout");
			if (attribute != null)
				m_ResponseLayoutDirection = (attribute == "vertical") ? Orientation.Vertical : Orientation.Horizontal;

			m_bHidden = false;
			attribute = node.GetAttribute("hidden");
			if (attribute != null)
				m_bHidden = attribute == "true";

			m_bList = true;
			attribute = node.GetAttribute("list");
			if (attribute != null)
				m_bList = attribute == "true";

			attribute = node.GetAttribute("requiredtext");
			m_RequiredText = (attribute ?? null);
			m_bRequired = !m_RequiredText.IsEmpty();

			attribute = node.GetAttribute("default");
			m_ResponseDefault = (attribute ?? "");

			attribute = node.GetAttribute("image");
			m_sImage = (attribute ?? "");
		}

		internal QuestionBase SetResponses(string[] responseList)
		{
			foreach (string response in responseList)
				m_ResponseList.Add(response);
			return this;
		}
	}
}