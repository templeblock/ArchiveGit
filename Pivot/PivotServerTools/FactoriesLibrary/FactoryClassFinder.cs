using System;
using System.Collections.Generic;
using System.IO;
using System.Reflection;

namespace FactoriesLibrary
{
	// Methods to locate assemblies and classes that implementat CollectionFactoryBase
	internal static class FactoryClassFinder
	{
		// Return new instances of all classes deriving from CollectionFactoryBase
		// from all assemblies in the given folder.
		public static IEnumerable<CollectionFactoryBase> Find(string folderPath)
		{
			List<CollectionFactoryBase> factories = new List<CollectionFactoryBase>();
			foreach (Type t in EnumerateTypesInAssemblies(folderPath))
			{
				if (t.IsSubclassOf(typeof(CollectionFactoryBase)))
				{
					CollectionFactoryBase factory = (CollectionFactoryBase)Activator.CreateInstance(t);
					if (string.IsNullOrEmpty(factory.FactoryName))
						factory.FactoryName = t.Name;

					factories.Add(factory);
				}
			}

			return factories;
		}

		private static IEnumerable<Assembly> EnumerateAssemblies(string folderPath)
		{
			string[] dllFilePaths = Directory.GetFiles(folderPath, "*.dll");
			foreach (string filePath in dllFilePaths)
			{
				Assembly assembly = Assembly.LoadFrom(filePath);
				yield return assembly;
			}
		}

		private static IEnumerable<Type> EnumerateTypesInAssemblies(string folderPath)
		{
			Assembly thisAssembly = Assembly.GetExecutingAssembly();
			foreach (Assembly assembly in EnumerateAssemblies(folderPath))
			{
				if (assembly != thisAssembly) //Ignore types in this PivotServerTools assembly.
				{
					Type[] types = assembly.GetTypes();
					foreach (Type t in types)
						yield return t;
				}
			}
		}
	}
}
