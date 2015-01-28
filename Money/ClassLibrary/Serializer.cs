using System;
using System.IO;
using System.Runtime.Serialization;
using System.Text;
using System.Xml;
using System.Xml.Serialization;

// The XMLSerializer and DataContractSerializer classes provides a simple means of serializing and deserializing object graphs to and from XML.
// The key differences are:
// 1. XMLSerializer has much smaller payload than DCS if you use [XmlAttribute] instead of [XmlElement]
//    DCS always store values as elements
// 2. DCS is "opt-in" rather than "opt-out"
//    With DCS you explicitly mark what you want to serialize with [DataMember]
//    With DCS you can serialize any field or property, even if they are marked protected or private
//    With DCS you can use [IgnoreDataMember] to have the serializer ignore certain properties
//    With XMLSerializer public properties are serialized, and need setters to be deserialized
//    With XmlSerializer you can use [XmlIgnore] to have the serializer ignore public properties
// 3. BE AWARE! DCS.ReadObject DOES NOT call constructors during deserialization
//    If you need to perform initialization, DCS supports the following callback hooks:
//    [OnDeserializing], [OnDeserialized], [OnSerializing], [OnSerialized]
//    (also useful for handling versioning issues) but THIS IS NOT SUPPORTED in Silverlight 2

namespace ClassLibrary
{
	// Instantiate this class to serialize objects using either XmlSerializer or DataContractSerializer
	internal class Serializer
	{
		private readonly bool _isDCS;

		internal Serializer(bool isDCS)
		{
			_isDCS = isDCS;
		}

		internal TT Deserialize<TT>(string input)
		{
			if (string.IsNullOrEmpty(input))
				return default(TT);

			MemoryStream stream = new MemoryStream(input.ToByteArray());
			return Deserialize<TT>(stream);
		}

		internal object Deserialize(string input, Type type)
		{
			if (string.IsNullOrEmpty(input))
				return null;

			MemoryStream stream = new MemoryStream(input.ToByteArray());
			return Deserialize(stream, type);
		}

		internal TT Deserialize<TT>(Stream stream)
		{
			if (stream == null)
				return default(TT);

			if (_isDCS)
			{
				DataContractSerializer dc = new DataContractSerializer(typeof(TT));
				return (TT)dc.ReadObject(stream);
			}
			else
			{
				XmlSerializer xs = new XmlSerializer(typeof(TT));
				return (TT)xs.Deserialize(stream);
			}
		}

		internal object Deserialize(Stream stream, Type type)
		{
			if (stream == null)
				return null;

			if (_isDCS)
			{
				DataContractSerializer dc = new DataContractSerializer(type);
				return dc.ReadObject(stream);
			}
			else
			{
				XmlSerializer xs = new XmlSerializer(type);
				return xs.Deserialize(stream);
			}
		}

		internal string Serialize(object obj)
		{
			if (obj == null)
				return null;

			MemoryStream stream = new MemoryStream();
			Serialize(stream, obj);

			// be aware that the Unicode Byte-Order Mark will be at the front of the string
			return stream.ToArray().ToUtfString();
		}

		internal void Serialize(Stream stream, object obj)
		{
			if (stream == null || obj == null)
				return;

			Type type = obj.GetType();
			if (_isDCS)
			{
				DataContractSerializer dc = new DataContractSerializer(type);
				dc.WriteObject(stream, obj);
			}
			else
			{
				XmlSerializer xs = new XmlSerializer(type);
				xs.Serialize(stream, obj);
			}
		}

		internal string SerializeToString<TT>(TT obj)
		{
			StringBuilder builder = new StringBuilder();
			XmlWriter xmlWriter = XmlWriter.Create(builder);
			Type type = obj.GetType();
			if (_isDCS)
			{
				DataContractSerializer dc = new DataContractSerializer(type);
				dc.WriteObject(xmlWriter, obj);
			}
			else
			{
				XmlSerializer xs = new XmlSerializer(type);
				xs.Serialize(xmlWriter, obj);
			}

			string xml = builder.ToString();
			xml = RegexHelper.ReplacePattern(xml, RegexHelper.WildcardToPattern("<?xml*>", WildcardSearch.Anywhere), string.Empty);
			xml = RegexHelper.ReplacePattern(xml, RegexHelper.WildcardToPattern(" xmlns:*\"*\"", WildcardSearch.Anywhere), string.Empty);
			xml = xml.Replace(Environment.NewLine + "  ", string.Empty);
			xml = xml.Replace(Environment.NewLine, string.Empty);
			return xml;
		}
	}
}
