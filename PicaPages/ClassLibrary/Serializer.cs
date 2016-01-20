using System.IO;
using System.Runtime.Serialization;
using System.Xml.Serialization;
using System.Text;

namespace ClassLibrary
{
	// Instantiate this class to serialize objects using either XmlSerializer or DataContractSerializer
	public class Serializer
	{
		private bool m_bXml;

		public Serializer(bool bXml)
		{
			m_bXml = bXml;
		}

		public TT Deserialize<TT>(string input)
		{
			MemoryStream stream = new MemoryStream(input.ToByteArray());
			if (m_bXml)
			{
				XmlSerializer xs = new XmlSerializer(typeof(TT));
				return (TT)xs.Deserialize(stream);
			}
			else
			{
				DataContractSerializer dc = new DataContractSerializer(typeof(TT));
				return (TT)dc.ReadObject(stream);
			}
		}

		public string Serialize<TT>(object obj)
		{
			MemoryStream stream = new MemoryStream();
			if (m_bXml)
			{
				XmlSerializer xs = new XmlSerializer(typeof(TT));
				xs.Serialize(stream, obj);
			}
			else
			{
				DataContractSerializer dc = new DataContractSerializer(typeof(TT));
				dc.WriteObject(stream, obj);
			}
			return stream.ToArray().ToUtfString();
#if false
			StringBuilder builder = new StringBuilder();
			XmlWriter writer = XmlWriter.Create(builder);
			if (m_bXml)
			{
				XmlSerializer xs = new XmlSerializer(typeof(TT));
				xs.Serialize(writer, obj);
				writer.Close();
			}
			else
			{
				DataContractSerializer dc = new DataContractSerializer(typeof(TT));
				dc.WriteObject(writer, obj);
			}
			return builder.ToString();
#endif
		}
	}
}
