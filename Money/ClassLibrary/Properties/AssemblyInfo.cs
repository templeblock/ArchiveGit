using System.Reflection;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;

// General Information about an assembly is controlled through the following 
// set of attributes. Change these attribute values to modify the information
// associated with an assembly.
[assembly: AssemblyTitle("ClassLibrary")]
[assembly: AssemblyDescription("")]
[assembly: AssemblyConfiguration("")]
[assembly: AssemblyCompany("Face To Face Software")]
[assembly: AssemblyProduct("ClassLibrary")]
[assembly: AssemblyCopyright("Copyright © Face To Face Software 2010")]
[assembly: AssemblyTrademark("")]
[assembly: AssemblyCulture("")]

// Setting ComVisible to false makes the types in this assembly not visible 
// to COM components.  If you need to access a type in this assembly from 
// COM, set the ComVisible attribute to true on that type.
[assembly: ComVisible(false)]

// The following GUID is for the ID of the typelib if this project is exposed to COM
[assembly: Guid("a97e8fd0-a6e2-47ae-b1c9-3219f629f961")]

// Version information for an assembly consists of the following four values:
//
//      Major Version
//      Minor Version 
//      Build Number
//      Revision
//
// You can specify all the values or you can default the Revision and Build Numbers 
// by using the '*' as shown below:
[assembly: AssemblyVersion("1.0.0.0")]
[assembly: AssemblyFileVersion("1.0.0.0")]

[assembly: InternalsVisibleTo("Money")]
[assembly: InternalsVisibleTo("Pivot")]

// DeepSea Obfuscator
// Treats the assembly is if it were private, and obfuscates public members fuction, but is too aggressive
//[assembly: ObfuscateAssembly(true)]
// Do not exclude the protection features - they are required for Silverlight and WPF
//[assembly: Obfuscation(Feature = "protect-xaml /namespace:ClassLibrary", Exclude = false)]
//[assembly: Obfuscation(Feature = "protect-serialization /namespace:ClassLibrary", Exclude = false)]
//[assembly: Obfuscation(Feature = "protect-xmlserialization /namespace:ClassLibrary", Exclude = false)]
//[assembly: Obfuscation(Feature = "protect-linq /namespace:ClassLibrary", Exclude = false)]
