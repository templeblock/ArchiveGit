using System;
using System.Diagnostics;
using System.Collections.Generic;
using System.Xml;
using System.Xml.Linq;
using System.Xml.Serialization;
using System.Runtime.Serialization;
using tMillicents = System.Int64;

namespace PicaPages
{
[DataContract(Name="Layout")]
[XmlRoot("Layout")]
	public class Layout
	{
[XmlAttribute]
[DataMember]
		public int PayloadId { set { m_sPayloadId = value; } get { return m_sPayloadId; } }
		private int m_sPayloadId;
[XmlAttribute]
[DataMember]
		public string Name { set { m_sName = value; } get { return m_sName; } }
		private string m_sName;
[XmlAttribute]
[DataMember]
		public string CategoryPath { set { m_sCategoryPath = value; } get { return m_sCategoryPath; } }
		private string m_sCategoryPath;
[XmlAttribute]
[DataMember]
		public string Icon { set { m_sIcon = value; } get { return m_sIcon; } }
		private string m_sIcon;

		/////////////////////////////////////////////////////////////////////////////
		public Layout()
		{
			m_sPayloadId = 0;
			m_sName = "";
			m_sCategoryPath = "";
			m_sIcon = "";
		}

		/////////////////////////////////////////////////////////////////////////////
		~Layout()
		{
		}
	}


[DataContract(Name="Cover")]
[XmlRoot("Cover")]
	public class Cover
	{
[XmlAttribute]
[DataMember]
		public int PayloadId { set { m_sPayloadId = value; } get { return m_sPayloadId; } }
		private int m_sPayloadId;
[XmlAttribute]
[DataMember]
		public string Name { set { m_sName = value; } get { return m_sName; } }
		private string m_sName;
[XmlAttribute]
[DataMember]
		public string CategoryPath { set { m_sCategoryPath = value; } get { return m_sCategoryPath; } }
		private string m_sCategoryPath;
[XmlAttribute]
[DataMember]
		public string Icon { set { m_sIcon = value; } get { return m_sIcon; } }
		private string m_sIcon;

		/////////////////////////////////////////////////////////////////////////////
		public Cover()
		{
			m_sPayloadId = 0;
			m_sName = "";
			m_sCategoryPath = "";
			m_sIcon = "";
		}

		/////////////////////////////////////////////////////////////////////////////
		~Cover()
		{
		}
	}

[DataContract(Name="Background")]
[XmlRoot("Background")]
	public class Background
	{
[XmlAttribute]
[DataMember]
		public int PayloadId { set { m_sPayloadId = value; } get { return m_sPayloadId; } }
		private int m_sPayloadId;
[XmlAttribute]
[DataMember]
		public string Name { set { m_sName = value; } get { return m_sName; } }
		private string m_sName;
[XmlAttribute]
[DataMember]
		public string CategoryPath { set { m_sCategoryPath = value; } get { return m_sCategoryPath; } }
		private string m_sCategoryPath;
[XmlAttribute]
[DataMember]
		public string Icon { set { m_sIcon = value; } get { return m_sIcon; } }
		private string m_sIcon;

		/////////////////////////////////////////////////////////////////////////////
		public Background()
		{
			m_sPayloadId = 0;
			m_sName = "";
			m_sCategoryPath = "";
			m_sIcon = "";
		}

		/////////////////////////////////////////////////////////////////////////////
		~Background()
		{
		}
	}

[DataContract(Name="Corner")]
[XmlRoot("Corner")]
	public class Corner
	{
[XmlAttribute]
[DataMember]
		public int PayloadId { set { m_sPayloadId = value; } get { return m_sPayloadId; } }
		private int m_sPayloadId;
[XmlAttribute]
[DataMember]
		public string Name { set { m_sName = value; } get { return m_sName; } }
		private string m_sName;
[XmlAttribute]
[DataMember]
		public string CategoryPath { set { m_sCategoryPath = value; } get { return m_sCategoryPath; } }
		private string m_sCategoryPath;
[XmlAttribute]
[DataMember]
		public string Icon { set { m_sIcon = value; } get { return m_sIcon; } }
		private string m_sIcon;

		/////////////////////////////////////////////////////////////////////////////
		public Corner()
		{
			m_sPayloadId = 0;
			m_sName = "";
			m_sCategoryPath = "";
			m_sIcon = "";
		}

		/////////////////////////////////////////////////////////////////////////////
		~Corner()
		{
		}
	}

[DataContract(Name="Photo")]
[XmlRoot("Photo")]
	public class Photo
	{
[XmlAttribute]
[DataMember]
		public int PayloadId { set { m_sPayloadId = value; } get { return m_sPayloadId; } }
		private int m_sPayloadId;
[XmlAttribute]
[DataMember]
		public string Name { set { m_sName = value; } get { return m_sName; } }
		private string m_sName;
[XmlAttribute]
[DataMember]
		public string CategoryPath { set { m_sCategoryPath = value; } get { return m_sCategoryPath; } }
		private string m_sCategoryPath;
[XmlAttribute]
[DataMember]
		public string Icon { set { m_sIcon = value; } get { return m_sIcon; } }
		private string m_sIcon;

		/////////////////////////////////////////////////////////////////////////////
		public Photo()
		{
			m_sPayloadId = 0;
			m_sName = "";
			m_sCategoryPath = "";
			m_sIcon = "";
		}

		/////////////////////////////////////////////////////////////////////////////
		~Photo()
		{
		}
	}
}
