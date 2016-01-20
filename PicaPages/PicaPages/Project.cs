using System;
using System.Net;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Documents;
using System.Windows.Ink;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Animation;
using System.Windows.Shapes;
using System.Runtime.Serialization;
using System.Diagnostics;
using System.Collections.Generic;
using System.Xml.Linq;
using System.ComponentModel;

namespace PicaPages
{
	[DataContract(Name="Project", Namespace="http://schemas.datacontract.org/2004/07/Asi.Inkubuzz"), DebuggerStepThrough]
	public class Project : INotifyPropertyChanged
	{
		// Fields
		private Binary ChangeStampField;
		private List<ContentElement> ContentElementsField;
		private List<ContentLayout> ContentLayoutsField;
		private int CoverBackgroundIDField;
		private short CoverTypeIDField;
		private int CreatedByField;
		private DateTime CreatedOnField;
		private DateTime? DeletedOnField;
		private int? GroupIDField;
		private ProjectActivitySummary ib_ProjectActivitySummariesField;
		private bool IsDeletedField;
		private int LastModifiedByField;
		private DateTime LastModifiedOnField;
		private List<Order> OrdersField;
		private short OutputSizeIDField;
		private int PageBackgroundIDField;
		private Guid ProjectIDField;
		private int ProjectTypeIDField;
		private List<ProjectUser> ProjectUsersField;
		private PropertyChangedEventHandler PropertyChanged;
		private List<Section> SectionsField;
		private int ThemeIDField;
		private string TitleField;
		private XElement WorkingCoverBackgroundsXMLField;
		private XElement WorkingCoverTemplatesXMLField;
		private XElement WorkingPageBackgroundsXMLField;
		private XElement WorkingPageTemplatesXMLField;

		// Events
		public event PropertyChangedEventHandler PropertyChanged;

		// Methods
		public Project();
		protected void RaisePropertyChanged(string propertyName);

		// Properties
		[DataMember(Order=8)]
		public Binary ChangeStamp { get; set; }
		[DataMember(EmitDefaultValue=false, Order=0x18)]
		public List<ContentElement> ContentElements { get; set; }
		[DataMember(EmitDefaultValue=false, Order=0x17)]
		public List<ContentLayout> ContentLayouts { get; set; }
		[DataMember(Order=6)]
		public int CoverBackgroundID { get; set; }
		[DataMember(Order=0x10)]
		public short CoverTypeID { get; set; }
		[DataMember(Order=13)]
		public int CreatedBy { get; set; }
		[DataMember(Order=3)]
		public DateTime CreatedOn { get; set; }
		[DataMember(Order=0x13)]
		public DateTime? DeletedOn { get; set; }
		[DataMember(Order=4)]
		public int? GroupID { get; set; }
		[DataMember(EmitDefaultValue=false, Order=0x19)]
		public ProjectActivitySummary ib_ProjectActivitySummaries { get; set; }
		[DataMember(Order=0x12)]
		public bool IsDeleted { get; set; }
		[DataMember(Order=14)]
		public int LastModifiedBy { get; set; }
		[DataMember(Order=15)]
		public DateTime LastModifiedOn { get; set; }
		[DataMember(EmitDefaultValue=false, Order=0x15)]
		public List<Order> Orders { get; set; }
		[DataMember(Order=9)]
		public short OutputSizeID { get; set; }
		[DataMember(Order=5)]
		public int PageBackgroundID { get; set; }
		[DataMember]
		public Guid ProjectID { get; set; }
		[DataMember]
		public int ProjectTypeID { get; set; }
		[DataMember(EmitDefaultValue=false, Order=20)]
		public List<ProjectUser> ProjectUsers { get; set; }
		[DataMember(EmitDefaultValue=false, Order=0x16)]
		public List<Section> Sections { get; set; }
		[DataMember(Order=7)]
		public int ThemeID { get; set; }
		[DataMember]
		public string Title { get; set; }
		[DataMember(Order=10)]
		public XElement WorkingCoverBackgroundsXML { get; set; }
		[DataMember(Order=0x11)]
		public XElement WorkingCoverTemplatesXML { get; set; }
		[DataMember(Order=11)]
		public XElement WorkingPageBackgroundsXML { get; set; }
		[DataMember(Order=12)]
		public XElement WorkingPageTemplatesXML { get; set; }
	}
}
