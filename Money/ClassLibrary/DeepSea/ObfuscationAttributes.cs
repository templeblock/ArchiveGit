using System;
using System.Runtime.InteropServices;
using System.Text;
using System.Reflection;

// --------------------------------------------------------
// This code is provided for .NET 1.1 projects.
// 
// The semantics of these attributes are identical
// to the corresponding .NET 2.0 attributes in the
// System.Reflection namespace.
//
// Use of this code in .NET 1.1 projects is allowed
// without restrictions.
// --------------------------------------------------------

namespace System.Reflection //j DeepSeaObfuscator
{
	/// <summary>
	/// Instructs DeepSea Obfuscator to take the specified actions for an assembly, type, or member. 
	/// </summary>
	[AttributeUsageAttribute(AttributeTargets.Assembly | AttributeTargets.Class | AttributeTargets.Struct | AttributeTargets.Enum | AttributeTargets.Method | AttributeTargets.Property | AttributeTargets.Field | AttributeTargets.Event | AttributeTargets.Interface | AttributeTargets.Parameter | AttributeTargets.Delegate, AllowMultiple = true, Inherited = false)]
	[ComVisibleAttribute(true)]
	[Obfuscation]
//j	internal sealed class ObfuscationAttribute : Attribute
	public sealed class ObfuscationAttribute : Attribute
	{
		private bool applyToMembers = true;
		private bool exclude = true;
		private string feature = "all";
		private bool stripAfterObfuscation = true;

		/// <summary>
		/// Initializes a new instance.
		/// </summary>
		public ObfuscationAttribute()
		{
		}

		/// <summary>
		/// Gets or sets a Boolean value indicating whether DeepSea Obfuscator should remove this attribute after processing. 
		/// </summary>
		public bool StripAfterObfuscation
		{
			get { return stripAfterObfuscation; }
			set { stripAfterObfuscation = value; }
		}

		/// <summary>
		/// Gets or sets a string value that is recognized by DeepSea Obfuscator, and which specifies processing options. 
		/// </summary>
		public string Feature
		{
			get { return feature; }
			set { feature = value; }
		}

		/// <summary>
		/// Gets or sets a Boolean value indicating whether DeepSea Obfuscator should exclude the type or member from obfuscation. 
		/// </summary>
		public bool Exclude
		{
			get { return exclude; }
			set { exclude = value; }
		}

		/// <summary>
		/// Gets or sets a Boolean value indicating whether the attribute of a type is to apply to the members of the type. 
		/// </summary>
		public bool ApplyToMembers
		{
			get { return applyToMembers; }
			set { applyToMembers = value; }
		}
	}

	/// <summary>
	/// Instructs DeepSea Obfuscator to use their standard obfuscation rules for the appropriate assembly type. 
	/// </summary>
	[ComVisibleAttribute(true)]
	[AttributeUsageAttribute(AttributeTargets.Assembly, AllowMultiple = false, Inherited = false)]
	public sealed class ObfuscateAssemblyAttribute : Attribute
	{
		private readonly bool assemblyIsPrivate;
		private bool stripAfterObfuscation = true;

		/// <summary>
		/// Initializes a new instance.
		/// </summary>
		/// <param name="assemblyIsPrivate">True if the assembly is private and can be fully obfuscated (including public members), false otherwise.</param>
		public ObfuscateAssemblyAttribute(bool assemblyIsPrivate)
		{
			this.assemblyIsPrivate = assemblyIsPrivate;
		}

		/// <summary>
		/// Gets a Boolean value indicating whether the assembly was marked private. 
		/// </summary>
		public bool AssemblyIsPrivate
		{
			get { return assemblyIsPrivate; }
		}

		/// <summary>
		/// Gets or sets a Boolean value indicating whether DeepSea Obfuscator should remove this attribute after processing. 
		/// </summary>
		public bool StripAfterObfuscation
		{
			get { return stripAfterObfuscation; }
			set { stripAfterObfuscation = value; }
		}
	}
}
