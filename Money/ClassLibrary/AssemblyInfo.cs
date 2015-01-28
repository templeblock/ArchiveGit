using System;
using System.Reflection;

namespace ClassLibrary
{
	internal static class AssemblyInfo
	{
		internal static string Title { get { return AssemblySource.App.GetTitle(); } }
		internal static string GetTitle(this Assembly assembly)
		{
			if (assembly == null)
				assembly = AssemblySource.App;
			string title = assembly.GetAssemblyAttributeValue(typeof(AssemblyTitleAttribute), "Title");
			return (!string.IsNullOrEmpty(title) ? title : assembly.Name());
		}

		internal static string Description { get { return AssemblySource.App.GetDescription(); } }
		internal static string GetDescription(this Assembly assembly)
		{
			if (assembly == null)
				assembly = AssemblySource.App;
			return assembly.GetAssemblyAttributeValue(typeof(AssemblyDescriptionAttribute), "Description");
		}

		internal static string Product { get { return AssemblySource.App.GetProduct(); } }
		internal static string GetProduct(this Assembly assembly)
		{
			if (assembly == null)
				assembly = AssemblySource.App;
			return assembly.GetAssemblyAttributeValue(typeof(AssemblyProductAttribute), "Product");
		}

		internal static string Copyright { get { return AssemblySource.App.GetCopyright(); } }
		internal static string GetCopyright(this Assembly assembly)
		{
			if (assembly == null)
				assembly = AssemblySource.App;
			return assembly.GetAssemblyAttributeValue(typeof(AssemblyCopyrightAttribute), "Copyright");
		}

		internal static string Company { get { return AssemblySource.App.GetCompany(); } }
		internal static string GetCompany(this Assembly assembly)
		{
			if (assembly == null)
				assembly = AssemblySource.App;
			return assembly.GetAssemblyAttributeValue(typeof(AssemblyCompanyAttribute), "Company");
		}

		internal static string Version { get { return AssemblySource.App.GetVersion(); } }
		internal static string GetVersion(this Assembly assembly)
		{
			if (assembly == null)
				assembly = AssemblySource.App;
			string[] values = assembly.FullName.Split(',');
			foreach (string value in values)
			{
				string[] version = value.Split('=');
				if (version.Length == 2 && version[0].Trim() == "Version")
					return version[1].Trim();
			}

			return string.Empty;
		}

		private static string GetAssemblyAttributeValue(this Assembly assembly, Type attribute, string property)
		{
			object[] attributes = assembly.GetCustomAttributes(attribute, false);
			if (attributes.Length == 0)
				return string.Empty;

			object attributeValue = attributes[0];
			if (attributeValue == null)
				return string.Empty;

			Type attributeType = attributeValue.GetType();
			PropertyInfo propertyInfo = attributeType.GetProperty(property);
			if (propertyInfo == null)
				return string.Empty;

			object propertyValue = propertyInfo.GetValue(attributeValue, null);
			if (propertyValue == null)
				return string.Empty;

			return propertyValue.ToString();
		}
	}
}
