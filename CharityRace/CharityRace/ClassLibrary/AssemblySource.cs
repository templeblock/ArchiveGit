using System.Reflection;
using System.Windows;

// Written by: Jim McCurdy (jmccurdy@facetofacesoftware.com)

namespace ClassLibrary
{
	// Typical usage:
	//	AssemblySource.App - the current application assembly
	//	AssemblySource.This - the caller's assembly
	//	AssemblySource.Library - a library assembly
	//	AssemblySource.Type<ClassLibrary> - an assembly containing a known type
	internal class AssemblySource
	{
		// Current application assembly
		internal static Assembly App { get { return Application.Current.GetType().Assembly; } }

		// The caller's assembly
		internal static Assembly This { get { return Assembly.GetCallingAssembly(); } }

		// A library assembly
		internal static Assembly Library { get { return Assembly.GetExecutingAssembly(); } }

		// An assembly containing a known type
		internal static Assembly Type<TT>() { return typeof(TT).Assembly; }
	}
}