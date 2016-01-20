//-------------------------------------------------------------------------------
//
//	File:
//		PIUCore.h
//
//	Copyright 1997-1998, Adobe Systems Incorporated.
//	All Rights Reserved.
//
//	Description:
//		This file contains utility routines for common
//		functions related to the scripting system.
//
//	Use:
//		You can use this code as documentation for how
//		to script specific functions in Photoshop or you
//		may call the higher-level routines directly.
//		If you need specifics about what a function's
//		parameters do, refer to the Photoshop user guide
//		for that actual feature.
//
//	Error and result reporting:
//		I'm assuming you'll want to make multiple event
//		calls in a row and not have to check for errors
//		one after the other.  So I've designed these
//		routines to have you pass in your SPErr variable,
//		which, if on entry is no kSPNoError, the routine
//		will return immediately.  Otherwise the routine
//		will return kSPNoError if successful, or any other
//		error otherwise.  This way you can call groups
//		of routines and do one check at the bottom.
//
//		Also, if you're interested in the error or result reported,
//		you can call the base class's Result->GetDescriptor()
//		or Result->GetEvent() to return the exact event
//		and descriptor of the result.
//
//	Assumptions:
//		The standard suite pointers defined in PIUSuites.cpp
//		are available.
//
//	Version history:
//		1.0.0	4/3/1998	Ace		Created for Photoshop 5.0
//			Written by Andrew Coven.
//
//-------------------------------------------------------------------------------
 
#ifndef __PIUCore_h__	// Defined yet?
#define __PIUCore_h__	// Only define once.

#include "PIUNew.h"
#include "PIUSuites.h"		// Need other suites already defined.
#include "PIProperties.h"	// For RulerUnits.

//-------------------------------------------------------------------------------
//
//	class PIUResult
//
//	This is the suite that provides all the result management for the utilities.
//
//-------------------------------------------------------------------------------

class PIUResult
	{
	public:
		// Constructor:
		PIUResult ()
			{
			// Intentionally left blank.
			}

		// Destructor:
		~PIUResult ()
			{
			SetDescriptor(NULL);	// Clears descriptor.
			}

		// Makes a copy of the descriptor into the Result registry:
		void SetDescriptor(const PIActionDescriptor descriptor);
		
		// Makes a copy of the id of the event or key in the Result registry:
		void SetID(char* event);
		void SetID(const DescriptorTypeID id);

		// Returns a copy of the descriptor.  Its your
		// responsibility to Free it:
		PIActionDescriptor GetDescriptor(void) const;
			
		// Returns the string of the event or key.  May be a
		// 4-character ID as a string.  It's your responsibility
		// to Free it (via FreeBlock):
		char* GetID(void) const;
		
		// Displays an ADM message alert with the message from
		// the result descriptor, if its an error message.  Returns
		// true if it found a message in the descriptor:
		bool DisplayMessage(void);
		
		// Displays the keys and their types in the result descriptor.  Very
		// useful in determining a return descriptor's properties:
		void DisplayDescriptor(void);
			
	private:
		// None.
							
	protected:
		// None.
	};

//-------------------------------------------------------------------------------
//
//	class PIUElement
//
//	This is the base class that all abstraction layers derive from.  It
//	contains friend-like functions.
//
//-------------------------------------------------------------------------------
class PIUElement
	{
	private:
		// Since 0 is a valid ID number, we'll use -1 to indicate an
		// invalid id:
		#define kPIUInvalidID				-1
		
		// We use this class to communicate the identity of an element:
		class PIUIdentity
			{
			public:
				PIUIdentity(unsigned short initIndex = 0) :
					id_(kPIUInvalidID),
					name_(NULL),
					index_(initIndex),
					initIndex_(initIndex),
					shallow_(false),
					next_(NULL)
					{
					// Intentionally left blank.
					}
					
				~PIUIdentity();
			
				// Copy constructor:
				PIUIdentity
					(
					const PIUIdentity& identity
					);
				
				// Assignment operator:
				PIUIdentity& operator=
					(
					const PIUIdentity& identity
					);
					
				// Replace only different non-null items:
				PIUIdentity& operator+=
					(
					const PIUIdentity& identity
					);
					
				// Init identity:
				void IIdentity(void);
				
				// Make a heap-allocated identity:
				PIUIdentity* Make(void);		
				
				// Clean out an identity, freeing any allocated memory:
				void Clean(void);
				
				// Clean out all identities, freeing all in linked list:
				void CleanStack(void);
				
				// Push or pop an identity from the stack:
				void Push (void);
				SPErr Pop (void);	// Returns invalid element if stack empty.
				
				// We return a copy of the string or NULL.  It is the
				// callers responsibility to dispose it with FreeBlock():
				char* GetName(void) const;
				
				// This just returns the pointer:
				const char* const GetNamePtr(void) const;
				
				unsigned long GetID(void) const;
				unsigned short GetIndex(void) const;
				bool GetShallow(void) const;
				PIUIdentity* GetNext(void) const;
				
				void SetID(const unsigned long id);
				void SetName(const char* const name);		// We make a copy of the string.
				void SetNamePtr(const char* const name);	// We copy the pointer.
				void SetIndex(const unsigned short index);
				void SetShallow(const bool shallow);
				void SetNext(PIUIdentity* const next);

			private:
				// Items that identify this item in order of preference of reference:
				unsigned long id_;		// We'll always try to refer to it by id, if we can.
				char* name_;			// And then its name, which is more unique than its index.
				unsigned short index_;	// Index is our last resort, and it could change.
			
				const unsigned short initIndex_;	// Value we'll use to init index.
								
				// Shallow indicates we couldn't find a valid reference by propagating, so we tried only
				// this level and it did resolve so this identity indicates a non-propagated reference:
				bool shallow_;

				PIUIdentity* next_;		// Next identity on stack.
			};
			
		PIUIdentity identity_;
		
		PIUElement* parent_; // Parent of this element, NULL if none.
		
		// Result suite for utility functions:
		PIUResult result_;
		
		//-----------------------------------------------------------------------
		//	Browsing utilities.
		//-----------------------------------------------------------------------
		
		// Create an identity from the initial descriptor information that
		// we get at construction time. If this fails there's nothing we can
		// do but wait until the user supplies us with more info:
		SPErr CreateIdentity
			(
			/* IN/OUT */	PIUIdentity* identity
			);
		
		// Resolve and replace the internal identity with the new one. Returns
		// an error if it cannot resolve the identity:
		SPErr UpdateIdentity
			(
			/* IN */	PIUIdentity* identity,
			/* IN */	const bool propagate = true
			);
		
		// Resolve an identity from whatever items are available. Returns an
		// error if it cannot resolve the identity:
		SPErr ResolveIdentity
			(
			/* IN/OUT */	PIUIdentity* identity,
			/* IN */		const bool propagate = true
			);

		//-----------------------------------------------------------------------
		//	Object property utilities.
		//-----------------------------------------------------------------------

		// Hopefully you should never have to diddle with how you're going to
		// get your properties back, and the Action Abstration Layer will
		// manage the issues in the background.  The issues are substantial,
		// since many items cannot be referenced except by index or name, and
		// sometimes those things are unique and sometimes they're not.
		
		// Retrieve all the keys for this element by current element, if it
		// has a concept of 'current'.  May not always work.  Don't use this,
		// use GetAllProperties().
		PIActionDescriptor GetAllKeys (void);
		
		// Retrieve all the keys for this element given a reference:
		PIActionDescriptor GetAllKeys (const PIActionReference reference);

		// Get the parent:
		PIUElement* GetParent(void);
		
		// Rip apart a descriptor and read the useful keys from it:
		unsigned long ParseIDKey(const PIActionDescriptor descriptor);
		char* ParseNameKey(const PIActionDescriptor descriptor);
		unsigned short ParseIndexKey(const PIActionDescriptor descriptor);
		
		//-----------------------------------------------------------------------
		//	Reference utilities.
		//-----------------------------------------------------------------------
						
		// Hopefully you should never have to build a reference to something
		// because the Action Abstraction Layer has all the accessor routines
		// you'll ever need to target and do the things you want.  These are
		// used as low-level utilities to get around.  Things are still finicky
		// as of Photoshop 5, where some partial references will resolve
		// correctly and some won't.  We take the approach of building the
		// entire reference every time.

		class PIUReference
			{
			public:
				PIUReference() :
					parent_(NULL)
					{
					// Intentionally left blank.
					}
				
				~PIUReference()
					{
					// Intentionally left blank.
					}
					
				// Init this reference unit:
				void IReference(PIUElement* parent = NULL);
				
				// NOTE: Make and Free a reference are provided by sPSActionReferece.

				// These add to a reference by target, id, name, index, enum,
				// or single property:
				SPErr Add (PIActionReference reference);
				SPErr Add (PIActionReference reference, const unsigned long id);
				SPErr Add (PIActionReference reference, const char* const name);
				SPErr Add (PIActionReference reference, const unsigned short index);
				SPErr Add 
					(
					PIActionReference reference, 
					const DescriptorEnumTypeID eTypeID,
					const DescriptorEnumID eEnumID
					);
				SPErr AddProperty
					(
					PIActionReference reference,
					const DescriptorKeyID inKey = keyNull,
					const DescriptorClassID inClass = classProperty
					);
					
				// Return the actual 4-character codes for each of the
				// keys.  You'll want to override all of these in your
				// inherited class:
				DescriptorKeyID GetIDKey(void) const;
				DescriptorKeyID GetNameKey(void) const;
				DescriptorKeyID GetIndexKey(void) const;
				DescriptorKeyID	GetCountKey(void) const;
				DescriptorKeyID GetClassKey(void) const;
				
				// Actually maps the key to the element:
				DescriptorKeyID DoElementKeyMap(DescriptorKeyID inKey) const;

				// Return a pointer to the closest Result class:
				PIUResult* Result(void)
					{
					return parent_->Result();
					}
					
				// Create a descriptor and put the reference as the
				// target. It is the caller's responsibility to free
				// the reference:
				PIActionDescriptor CreateAndSetTarget(PIActionReference reference);

			private:
				PIUElement*	parent_;
				
			protected:
				// None.
			};
		
	protected:
		PIUReference reference_;
										
		// Add to a reference then propagate it up the chain:
		void PropagateReference
			(
			/* IN/OUT */	PIActionReference reference,
			/* IN/OUT */	SPErr* error		// Track errors.
			);				

	public:
		// Error return when we can't find a valid key to use as an
		// id in a descriptor:
		#define kPIUElementNotValid		'!Ele'
		
		// Error returned when we can't find the element by the
		// reference we created:
		#define kPIUElementNotFound		'!Elf'
		
		// Maximum number of keys for an element key map.  Used as
		// a safety mechanism just in case the user doesn't pass
		// a null-terminated keyMap:
		#define kElementKeyMapMax		40
		
		// Constructor:
		PIUElement
			(
			unsigned short initIndex = 0
			); 
			
		// Destructor:
		virtual ~PIUElement()
			{
			identity_.CleanStack();
			}
	
		// Return a pointer to the result utility suite:
		PIUResult* Result(void)
			{
			return &result_;
			}

		// Initialize the element.  At this point we should try to get its
		// basic descriptor and populate at least its identity.  Use this
		// as a lazy init, once things are all loaded.  Don't do this in
		// the constructor, which is called at plug-in load when things
		// aren't ready to be called:
		virtual void IElement(PIUElement* parent = NULL);
		
		//-----------------------------------------------------------------------
		//	Object keys.
		//-----------------------------------------------------------------------
		
		// This routine is responsible for returning all the keys that the
		// host returns for the different pertinent calls.  It maps the standard
		// keys and classes to what is specific for this element.  You should
		// always provide it with your derived class.
		virtual const DescriptorKeyID* ElementKeyMap (void) const;

		// Actually maps the key to the element:
		DescriptorKeyID DoElementKeyMap(DescriptorKeyID inKey) const;

		//-----------------------------------------------------------------------
		//	Object properties.
		//-----------------------------------------------------------------------
		
		// Return the ID of the element, if available (most are unique by
		// name):
		unsigned long GetID (void) const;
		
		// Returns a copy of the string pointed to by GetName, or NULL if
		// none.  It is the callers responsibility to delete with
		// FreeBlock():
		char* GetName (void) const;
		
		// Return the index of the item amongst its siblings, if
		// available:
		unsigned short GetIndex (void) const;
		
		// Returns the propagation level of the current identity:
		bool GetShallow (void) const;
		
		// Changes the propagation level of the current identity:
		void MarkShallow (void);
		void ClearShallow (void);

		// Returns the class of this element:
		DescriptorClassID GetClass (void) const;
				
		// Returns a descriptor pointing to all available
		// properties for the element.  It is the callers responsibility to
		// free the descriptor via Free(descriptor):
		PIActionDescriptor GetAllProperties (PIActionReference reference = 0);
		
		// Builds a class property reference and uses the current or
		// creates a descriptor with the actual data:
		PIActionDescriptor BuildSinglePropertyDescriptor
			(
			/* IN */	const PIActionDescriptor	descriptor,
			/* IN */	const DescriptorKeyID		inKey,
			/* OUT */	bool*						deleteWhenDone
			);

		// Retrieves a single property from the class:
		SPErr GetSingleProperty
			(
			/* IN */	const PIActionDescriptor	descriptor = 0,
			/* IN */	const DescriptorKeyID		inKey = keyNull,
			/* OUT */	DescriptorTypeID*			outType = NULL,
			/* OUT */	long*						outValue = NULL,
			/* OUT */	double*						outFloat = NULL
			);
			
		// Retrieves a single boolean property from the class:
		bool GetSingleBooleanProperty
			(
			/* IN */	const PIActionDescriptor	descriptor = 0,
			/* IN */	const DescriptorKeyID		inKey = keyNull
			);
			
		// Retrieves a single float property from the class:
		double GetSingleFloatProperty
			(
			/* IN */	const PIActionDescriptor	descriptor = 0,
			/* IN */	const DescriptorKeyID		inKey = keyNull
			);

		// Retrieves a single unit float property from the class:
		double GetSingleUnitFloatProperty
			(
			/* IN */	const PIActionDescriptor	descriptor = 0,
			/* IN */	const DescriptorKeyID		inKey = keyNull,
			/* OUT */	DescriptorUnitID*			outUnit = NULL
			);

		// Retrieves a single integer property from the class:
		long GetSingleIntegerProperty
			(
			/* IN */	const PIActionDescriptor	descriptor = 0,
			/* IN */	const DescriptorKeyID		inKey = keyNull
			);
			
		// Retrieves the value of an enumerated property from the class:
		DescriptorEnumID GetSingleEnumeratedProperty
			(
			/* IN */	const PIActionDescriptor	descriptor = 0,
			/* IN */	const DescriptorKeyID		inKey = keyNull,
			/* IN */	const DescriptorEnumTypeID	inEnumTypeID = typeNull
			);
			
		// Retrieves an object descriptor from the class:
		PIActionDescriptor GetSingleObjectProperty
			(
			/* IN */	const PIActionDescriptor	descriptor = 0,
			/* IN */	const DescriptorKeyID		inKey = keyNull,
			/* IN */	const DescriptorClassID		inClassID = classNull
			);

		// Retrieves a list from the class:
		PIActionList GetSingleListProperty
			(
			/* IN */	const PIActionDescriptor	descriptor,
			/* IN */	const DescriptorKeyID		inKey
			);
			
		// Returns a string from the class properties.  It is callers
		// responsibility to free with FreeBlock():
		char* GetSingleStringProperty
			(
			/* IN */	const PIActionDescriptor	descriptor = 0,
			/* IN */	const DescriptorKeyID		inKey = keyNull
			);
	
		// Sets a single property of the class:
		SPErr SetSingleProperty
			(
			/* IN */	const PIActionDescriptor	descriptor = 0,
			/* IN */	const DescriptorKeyID		inKey = keyNull,
			/* IN */	const DescriptorTypeID		inType = typeNull,
			/* IN */	const DescriptorClassID		inClass = classNull,	// Class or enum.
			/* IN */	const long					inValue = 0,
			/* IN */	const double				inFloat = 0.0
			);
		
		SPErr SetSingleBooleanProperty
			(
			/* IN */	const PIActionDescriptor	descriptor,
			/* IN */	const DescriptorKeyID		inKey,
			/* IN */	const bool					inBool
			);

		SPErr SetSingleIntegerProperty
			(
			/* IN */	const PIActionDescriptor	descriptor,
			/* IN */	const DescriptorKeyID		inKey,
			/* IN */	const long					inValue
			);

		SPErr SetSingleEnumeratedProperty
			(
			/* IN */	const PIActionDescriptor	descriptor,
			/* IN */	const DescriptorKeyID		inKey,
			/* IN */	const DescriptorEnumTypeID	inEnumType,
			/* IN */	const DescriptorEnumID		inEnum
			);

		SPErr SetSingleObjectProperty
			(
			/* IN */	const PIActionDescriptor	descriptor = 0,
			/* IN */	const DescriptorKeyID		inKey = keyNull,
			/* IN */	const DescriptorClassID		inClass = classNull,
			/* IN */	const PIActionDescriptor	classDescriptor = 0
			);
			
		SPErr SetSingleStringProperty
			(
			/* IN */	const PIActionDescriptor	descriptor = 0,
			/* IN */	const DescriptorKeyID		inKey = keyNull,
			/* IN */	const char* const			inString = NULL
			);
			
		//-----------------------------------------------------------------------
		//	Browsing element tree.
		//-----------------------------------------------------------------------
		
		// Return the number of elements of this type:
		virtual unsigned short GetCount (void);

		// Do generic select event of current identity:
		virtual SPErr Select (PIActionReference reference = 0);
		
		// Move to an element of the tree by ID:
		SPErr SelectByID (const unsigned long id);

		// Move to an element of the tree by name:
		SPErr SelectByName (const char* const name);

		// Move to the Nth element of the tree:
		SPErr SelectByIndex (const unsigned short index);
		
		// Do generic select event with current identity without resolving
		// parents:
		SPErr SelectShallow (void);
		
		// Move to an element of the tree by ID without resolving parents:
		SPErr SelectByIDShallow (const unsigned long id);

		// Move to an element of the tree by name without resolving parents:
		SPErr SelectByNameShallow (const char* const name);

		// Move to the Nth element of the tree without resolving parents:
		SPErr SelectByIndexShallow (const unsigned short index);
		
		// Push current selection onto stack:
		void Push (void);
			
		// Pop current selection from stack:
		SPErr Pop (void);
		
		// Pop current selection and select it:
		SPErr PopAndSelect (void);
		
		// Build a reference all the way up the chain:
		PIActionReference BuildReference (void);
		
		// Build a reference only at this point in the chain:
		PIActionReference BuildReferenceShallow (void);
		
		// Return the identity:
		PIUIdentity& GetIdentity (void);		
		
		//-----------------------------------------------------------------------
		//	Play utilities.
		//-----------------------------------------------------------------------
		
		// Play an event and set the result class with the info.  It is the
		// callers responsibility to free the descriptor handed in:
		SPErr PlayAndSetResult
			(
			/* IN */	const DescriptorEventID event,
			/* IN */	const PIActionDescriptor descriptor = 0,
			/* IN */	const PIDialogPlayOptions options = plugInDialogSilent
			);

		//-----------------------------------------------------------------------
		//	Search facilities.
		//-----------------------------------------------------------------------

		// Find the index by id:
		unsigned short FindIndexForID (const unsigned long id);
				
		// Find the index by name:
		unsigned short FindIndexForName (const char* const name);

		// Find the index by id without resolving parents:
		unsigned short FindIndexForIDShallow (const unsigned long id);
				
		// Find the index by name without resolving parents:
		unsigned short FindIndexForNameShallow (const char* const name);

		// Add a reference to this specific element in a growing reference
		// and pass it on.  This will step down to find the reference
		// form that actually works:
		virtual void AddReference
			(
			/* IN/OUT */	PIActionReference reference,
			/* IN/OUT */	SPErr* error,		// Track errors.
			/* IN */		const PIUIdentity& identity
			);
		
		// Start propagation of a reference:
		void Propagate
			(
			/* IN/OUT */	PIActionReference reference,
			/* IN/OUT */	SPErr* error		// Track errors.
			);
			
		// Coverage routines to return same keys:
		DescriptorKeyID GetIDKey(void) const
			{
			return reference_.GetIDKey();
			}
		
		DescriptorKeyID GetNameKey(void) const
			{
			return reference_.GetNameKey();
			}
			
		DescriptorKeyID GetIndexKey(void) const
			{
			return reference_.GetIndexKey();
			}
		
		DescriptorKeyID GetCountKey(void) const
			{
			return reference_.GetCountKey();
			}
		
		DescriptorKeyID GetClassKey(void) const
			{
			return reference_.GetClassKey();
			}	
	};

//-------------------------------------------------------------------------------
//
//	class PIUFilter
//
//	This is the suite that provides all the filter-related utilities.  It does
//	not need to derive from PIUElement since it has no structure to browse.
//
//-------------------------------------------------------------------------------

class PIUFilter
	{
	public:
		void GaussianBlur
			(
			/* IN/OUT */	double* radius,
			/* IN/OUT */	SPErr* error
			);

		PIUResult* Result(void)
			{
			return &result_;
			}
	
	private:
		// Result suite of functions:
		PIUResult result_;
		
	protected:
		// None.
	};
	
//-------------------------------------------------------------------------------
//
//	class PIUHistory
//
//	This is the suite that provides all the history utilities.
//
//-------------------------------------------------------------------------------

class PIUHistory : public PIUElement
	{
	public:
		PIUHistory()
			{
			// Intentionally left blank.
			}
		
		// Indicates whether this is an automatic or named history entry:
		bool IsAuto (void);
		
		// Indicates whether the current entry is the brush source:
		bool IsBrushSource (void);
		
		// Indicates whether the referenced state is the current state:
		bool IsCurrentState (void);

	private:
		// Override -- Must supply this map:
		virtual const DescriptorKeyID* ElementKeyMap (void) const;
	};

//-------------------------------------------------------------------------------
//
//	class PIUAction
//
//	This is the suite that provides all the action utilities.
//
//-------------------------------------------------------------------------------

class PIUAction : public PIUElement
	{
	public:
		// Actions can be referred to by "current" but return an error if
		// there is no current action selected (which is possible), so we
		// default to the first action:
		PIUAction
			(
			unsigned short initIndex = 1
			) :
			PIUElement(initIndex)
			{
			// Intentionally left blank.
			}

		// Actions and ActionSets return the count-1, so we override
		// and fix it:
		virtual unsigned short GetCount (void);
		unsigned short GetCountSansSet (void);

		// Add some additional selection logic:
		SPErr SelectByNameSansSet (const char* const name);
		SPErr SelectByIndexSansSet (const unsigned short index);

		// Play the current action. Reference IS freed by routine:
		SPErr Play (PIActionReference reference = 0);
		SPErr PlayShallow (void);

	private:
		// Override -- Must supply this map:
		virtual const DescriptorKeyID* ElementKeyMap (void) const;								
		// Overriding Select routine because eventSelect does not
		// work for actions (but indirect references works, so we
		// can just change the identity and not do an eventSelect):
		virtual SPErr Select (PIActionReference /* reference = 0 */)
			{
			return kSPNoError;
			}

	};

//-------------------------------------------------------------------------------
//
//	class PIUActionSet
//
//	This is the suite that provides all the action set utilities.
//
//-------------------------------------------------------------------------------

class PIUActionSet : public PIUElement
	{
	public:
		// Action sets cannot be referred to by "current" at all, so we
		// default to the first set:
		PIUActionSet
			(
			unsigned short initIndex = 1
			) :
			PIUElement(initIndex)
			{
			// Intentionally left blank.
			}
			
		// Actions and ActionSets return the count-1, so we override
		// and fix it:
		virtual unsigned short GetCount (void);

	private:
		// Override -- Must supply this map:
		virtual const DescriptorKeyID* ElementKeyMap (void) const;
		
		// Overriding Select routine because eventSelect does not
		// work for actions (but indirect references works, so we
		// can just change the identity and not do an eventSelect):
		virtual SPErr Select (PIActionReference /* reference = 0 */)
			{
			return kSPNoError;
			}
	};
		
//-------------------------------------------------------------------------------
//	PIUPathKind_t.	Used by class PIUPath.
//-------------------------------------------------------------------------------
typedef enum
	{
	kPIUInvalidPathKind = 0,
	kPIUNormalPath,
	kPIUWorkPath,
	kPIUClippingPath
	} PIUPathKind_t;
	
typedef struct
	{
	PIUPathKind_t		kind;
	DescriptorEnumID	aEnum;
	} PIUPathKindToEnum_t;

//-------------------------------------------------------------------------------
//
//	class PIUPath
//
//	This is the suite that provides all the path utilities.
//
//-------------------------------------------------------------------------------

class PIUPath : public PIUElement
	{
	public:
		PIUPath()
			{
			// Intentionally left blank.
			}
	
		// Is the current path the target?
		bool IsTarget (void);
			
		// Utilities for GetKind:
		PIUPathKind_t MapEnumToPathKind (const DescriptorEnumID aEnum);
		DescriptorEnumID MapPathKindToEnum (const PIUPathKind_t mode);

		PIUPathKind_t GetKind (void);
		
		// Clipping paths have one extra piece of information, their
		// flatness.  So if this is a clipping path, flatness will
		// be a valid value:
		double GetFlatness (void);
		SPErr SetFlatness (const double flatness = 0.0);
		
		// keyPathContents is the clipping path contents, returned
		// as a path contents list of subpaths and document points.  It
		// is complicated and there should be accessor and setter
		// functions to return it as a block of 'normal' data, but I
		// don't have time to do it for this release.

	private:
		// Override -- Must supply this map:
		virtual const DescriptorKeyID* ElementKeyMap (void) const;
	};
	
//-------------------------------------------------------------------------------
//	PIUAlphaKind_t.	Used by PIUChannel.
//-------------------------------------------------------------------------------
typedef enum
	{
	kPIUInvalidAlphaColorIndicator = 0,
	kPIUAlphaColorIsMaskedArea,
	kPIUAlphaColorIsSelectedArea,
	kPIUAlphaColorIsSpot
	} PIUAlphaColorIndicator_t;
	
typedef struct
	{
	PIUAlphaColorIndicator_t	indicator;
	DescriptorEnumID			aEnum;
	} PIUAlphaColorIndicatorToEnum_t;

//-------------------------------------------------------------------------------
//
//	class PIUChannel
//
//	This is the suite that provides all the channel utilities.
//
//-------------------------------------------------------------------------------

class PIUChannel : public PIUElement
	{
	public:
		PIUChannel()
			{
			// Intentionally left blank.
			}
			
		// Indicates whether this channel is visible in the palette:
		bool IsVisible (void);
		
		// If the current channel is an alpha channel, then these
		// routines will return alpha channel info:
		bool IsAlpha (void);
		
		unsigned short GetAlphaOpacity (void);
		
		// Utilities for GetAlphaKind:
		PIUAlphaColorIndicator_t MapEnumToAlphaColorIndicator (const DescriptorEnumID aEnum);
		DescriptorEnumID MapAlphaColorIndicatorToEnum (const PIUAlphaColorIndicator_t mode);

		PIUAlphaColorIndicator_t GetAlphaColorIndicator (void);
		
		// There is also a keyColor object sitting in the alpha channel
		// information that contains color specific information.  For
		// instance, if the color is classHSBColor, the first element will
		// be keyHue a unitFloat of unitAngle, then keySaturation and
		// keyBrightness, both float values.  classRGBColor will be
		// a keyRed, keyGreen, and keyBlue, all floats.  This is a
		// good place for a utility routine to rip apart this and
		// return it as a standard struct or object.  Perhaps later.
		
	private:
		// Override -- Must supply this map:
		virtual const DescriptorKeyID* ElementKeyMap (void) const;
	};

//-------------------------------------------------------------------------------
//	PIUBlendMode_t.	Used by class PIULayer.
//-------------------------------------------------------------------------------
typedef enum
	{
	kPIUInvalidBlendMode = 0,
	kPIUBlendNormal,
	kPIUBlendDissolve,
	kPIUBlendBehind,
	kPIUBlendClear,
	kPIUBlendMultiply,
	kPIUBlendScreen,
	kPIUBlendOverlay,
	kPIUBlendSoftLight,
	kPIUBlendHardLight,
	kPIUBlendDarken,
	kPIUBlendLighten,
	kPIUBlendDifference,
	kPIUBlendHue,
	kPIUBlendSaturation,
	kPIUBlendColor,
	kPIUBlendLuminosity,
	kPIUBlendExclusion,
	kPIUBlendColorDodge,
	kPIUBlendColorBurn
	} PIUBlendMode_t;
	
typedef struct
	{
	PIUBlendMode_t		mode;
	DescriptorEnumID	aEnum;
	} PIUBlendModeToEnum_t;
	
//-------------------------------------------------------------------------------
//
//	class PIULayer
//
//	This is the suite that provides all the channel utilities.
//
//-------------------------------------------------------------------------------

class PIULayer : public PIUElement
	{
	public:
		PIULayer()
			{
			// Intentionally left blank.
			}

		// Is this a background layer? (Which could mean either that this
		// is a flat document, or that just this layer is a background
		// layer.  You can tell this by whether there are other layers
		// or not using GetCount()):
		bool IsBackground (void);
		
		// Indicate whether layer is set to be visible or not in the palette:
		bool IsVisible (void);
		
		// Utilities for Blend Mode:
		PIUBlendMode_t MapEnumToBlendMode (const DescriptorEnumID aEnum);
		DescriptorEnumID MapBlendModeToEnum (const PIUBlendMode_t mode);
		
		// The blending mode of this layer (if not flat):
		PIUBlendMode_t GetBlendMode (void);
		SPErr SetBlendMode (const PIUBlendMode_t mode);
		
		// NOTE: There is an error in Photoshop 5.0 where you cannot
		// get the linked info of the first layer.  This is a known
		// bug.  Simply walk the rest of the layers and find the
		// other layers that it is linked to, if that one is
		// of particular interest.
		
		// Indicates how many other layers are linked to this one:
		unsigned short GetLinkCount (void);
		
		// Returns the id of layer N from the link list. Is expected to
		// be 1-based:
		long GetLinkIDByIndex (const unsigned short index = 1);
		
		// The opacity of this layer (if not flat) from 0...255:
		unsigned short GetOpacity (void);
		SPErr SetOpacity (const unsigned short opacity = 128); // 50%
		
		// If this layer can be transparent, the value of its preserve transparency:
		bool GetPreserveTransparency (void);
		SPErr SetPreserveTransparency (const bool preserve = true);
		
		// If there is a user mask, is it enabled?
		bool IsUserMaskEnabled (void);
		
		// If there is a user mask, is it linked to the layer image?
		bool IsUserMaskLinked (void);
		
		// Are layer effects set to visible?
		bool IsLayerEffectsVisible (void);
		
		// Global angle of layer effects:
		short GetLayerEffectsGlobalAngle (void);		
		
	private:
		// Override -- Must supply this map:
		virtual const DescriptorKeyID* ElementKeyMap (void) const;
	};

//-------------------------------------------------------------------------------
//	PIUImageMode_t. Used in class PIUDocument.
//-------------------------------------------------------------------------------
typedef enum
	{
	kPIUInvalidImageMode = 0,
	kPIUModeBitmap,
	kPIUModeGrayscale,
	kPIUModeIndexed,
	kPIUModeRGB,
	kPIUModeCMYK,
	kPIUModeHSL,
	kPIUModeHSB,
	kPIUModeMultichannel,
	kPIUModeLab,
	kPIUModeGrayscale16,
	kPIUModeRGB48,
	kPIUModeLab48,
	kPIUModeCMYK64
	} PIUImageMode_t;
	
typedef struct
	{
	PIUImageMode_t		mode;
	DescriptorEnumID	aEnum;
	} PIUImageModeToEnum_t;
	
//-------------------------------------------------------------------------------
//	PIUPreviewCMYK_t. Used in class PIUDocument.
//-------------------------------------------------------------------------------
typedef enum
	{
	kPIUInvalidPreviewCMYK = 0,
	kPIUPreviewOff,
	kPIUPreviewCMYK,
	kPIUPreviewCyan,
	kPIUPreviewMagenta,
	kPIUPreviewYellow,
	kPIUPreviewBlack,
	kPIUPreviewCMY
	} PIUPreviewCMYK_t;
	
typedef struct
	{
	PIUPreviewCMYK_t	preview;
	DescriptorEnumID	aEnum;
	} PIUPreviewCMYKToEnum_t;

//-------------------------------------------------------------------------------
//
//	class PIUDocument
//
//	This is the suite that provides all the document utilities.
//
//-------------------------------------------------------------------------------

class PIUDocument : public PIUElement
	{
	public:
		PIUDocument()
			{
			// Intentionally left blank.
			}
			
		// Utilities for GetImageMode:
		PIUImageMode_t MapEnumToImageMode (const DescriptorEnumID aEnum);
		DescriptorEnumID MapImageModeToEnum (const PIUImageMode_t mode);

		// Mode of the document:
		PIUImageMode_t	GetImageMode (void);
		
		// Return the horizontal and vertical pixel size:
		PIUDPoint_t GetSize (void);
		
		// Resolution of the document in pixels per inch:
		double GetResolution (void);
		
		// Return or set the horizontal and/or vertical nudge values:
		PIUDPoint_t GetNudge (void);
		SPErr SetNudge (const PIUDPoint_t inNudge);
		
		// Return or set the horizontal and/or vertical origin:
		PIUDPoint_t GetOrigin (void);
		SPErr SetOrigin (const PIUDPoint_t inOrigin);
		
		// Bit depth of the document:
		unsigned short GetDepth (void);
		
		// Is this document marked with a copyright?
		bool GetCopyright (void);
		
		// Is this document marked with a digital watermark?
		bool GetWatermark (void);
		SPErr SetWatermark (const bool inWatermark = false);
		
		// Utilities for GetPreviewCMYK/SetPreviewCMYK:
		PIUPreviewCMYK_t MapEnumToPreviewCMYK (const DescriptorEnumID aEnum);
		DescriptorEnumID MapPreviewCMYKToEnum (const PIUPreviewCMYK_t preview);

		// Are we previewing in a CMYK mode?  Which mode, if so?
		PIUPreviewCMYK_t GetPreviewCMYK (void);
		SPErr SetPreviewCMYK (const PIUPreviewCMYK_t preview = kPIUInvalidPreviewCMYK);
		
		// Is this document marked to be saved?
		bool IsDirty (void);
		SPErr SetDirty (const bool dirty = true);

		// Returns a file specification reference to the document, if
		// one exists (untitled documents have no alias records until
		// they are saved):
		SPErr GetFileReference
			(
			/* OUT */	SPPlatformFileSpecification*	fileSpec
			);
	 
		// keyFileInfo is a long text list of different keys stuffed in classFileInfo.
		// You can figure it out yourself.	

	private:
		// Override -- Must supply this map:
		virtual const DescriptorKeyID* ElementKeyMap (void) const;
	};

//-------------------------------------------------------------------------------
//	PIUEnumToRulerUnits_t. Used in class PIUApplication.
//-------------------------------------------------------------------------------
typedef struct
	{
	RulerUnits			rulerUnits;
	DescriptorEnumID	aEnum;
	} PIURulerUnitsToEnum_t;
	
//-------------------------------------------------------------------------------
//	PIUInterpolationMethod_t. Used in class PIUApplication.
//-------------------------------------------------------------------------------
typedef enum 
	{
	kPIUInvalidInterpolationMethod = 0,
	kPIUInterpolateNearestNeighbor,
	kPIUInterpolateBilinear,
	kPIUInterpolateBicubic
	} PIUInterpolationMethod_t;

typedef struct
	{
	PIUInterpolationMethod_t method;
	DescriptorEnumID		 aEnum;
	} PIUInterpolationMethodToEnum_t;
				
//-------------------------------------------------------------------------------
//	PIUEyedropperSample_t. Used in class PIUApplication.
//-------------------------------------------------------------------------------
typedef enum
	{
	kPIUInvalidEyedropperSample = 0,
	kPIUSamplePoint,
	kPIUSample3x3,
	kPIUSample5x5
	} PIUEyedropperSample_t;
	
typedef struct
	{
	PIUEyedropperSample_t	sample;
	DescriptorEnumID		aEnum;
	} PIUEyedropperSampleToEnum_t;
	
//-------------------------------------------------------------------------------
//	PIUColorPickerKind_t. Used in class PIUApplication.
//-------------------------------------------------------------------------------
typedef enum
	{
	kPIUInvalidColorPickerKind = 0,
	kPIUSystemPicker,
	kPIUPhotoshopPicker,
	kPIUPlugInPicker
	} PIUColorPickerKind_t;
	
typedef struct
	{
	PIUColorPickerKind_t	kind;
	DescriptorEnumID		aEnum;
	} PIUColorPickerKindToEnum_t;
	
//-------------------------------------------------------------------------------
//	PIUInterfaceColor_t. Used in class PIUApplication.
//-------------------------------------------------------------------------------
typedef enum
	{
	kPIUInvalidInterfaceColor = 0,
	kPIUInterfaceWhite,
	kPIUInterfaceButtonUpFill,
	kPIUInterfaceBevelShadow,
	kPIUInterfaceIconFillActive,
	kPIUInterfaceIconFillDimmed,
	kPIUInterfacePaletteFill,
	kPIUInterfaceIconFrameDimmed,
	kPIUInterfaceIconFrameActive,
	kPIUInterfaceBevelHighlight,
	kPIUInterfaceButtonDownFill,
	kPIUInterfaceIconFillSelected,
	kPIUInterfaceBorder,
	kPIUInterfaceButtonDarkShadow,
	kPIUInterfaceIconFrameSelected,
	kPIUInterfaceRed,
	kPIUInterfaceBlack,
	kPIUInterfaceToolTipBackground,
	kPIUInterfaceToolTipText
	} PIUInterfaceColor_t;
	
typedef struct
	{
	PIUInterfaceColor_t		color;
	DescriptorKeyID			aKey;
	} PIUInterfaceColorToKey_t;
	
//-------------------------------------------------------------------------------
//
//	class PIUApplication
//
//	This is the suite that provides all the application utilities.
//
//-------------------------------------------------------------------------------

class PIUApplication : public PIUElement
	{
	public:
		PIUApplication()
			{
			// Intentionally left blank.
			}
		
		// NOTE:  Utility routines missing from PIUApplication are ones to
		// set RGBSetup, CMYKSetup, GrayscaleSetup, and ProfileSetup.  These
		// are settable, but not gettable properties.  So until they are supported
		// both directions, I will leave them up to the savvy developer when
		// they want to use them.  To start, record some different values and
		// see how they appear in the Actions Palette.  That will give you a good
		// idea as to the different key names and their relevant values.  Also
		// refer to the Actions Guide.pdf.
		
		//-----------------------------------------------------------------------
		//	Preferences.
		//-----------------------------------------------------------------------			
		
		// Utilities for GetRulerUnits/SetRulerUnits:
		RulerUnits MapEnumToRulerUnits (const DescriptorEnumID aEnum);
		DescriptorEnumID MapRulerUnitsToEnum (const RulerUnits rulerUnits);

		RulerUnits GetRulerUnits (void);
		
		double GetPointsPerInch (void);	// Either 72.0 or 72.27.
		
		unsigned short GetCacheLevels (void);
		
		bool UseCacheForHistogram (void);
		
		// Utilities for GetInterpolationMethod/SetInterpolationMethod:
		DescriptorEnumID MapInterpolationToEnum
			(
			const PIUInterpolationMethod_t method
			);
		PIUInterpolationMethod_t MapEnumToInterpolation
			(
			const DescriptorEnumID aEnum
			);
			
		PIUInterpolationMethod_t GetInterpolationMethod (void);
		
		// Major grid units per inch/etc.:
		double GetMajorGridUnits (void);
		SPErr SetMajorGridUnits (const double inUnits = 0.0);
		
		// Number of units per major:
		unsigned short GetMinorGridUnits (void);
		SPErr SetMinorGridUnits (const unsigned short inUnits = 0);
		
		// Utilities for GetEyedroperSample/SetEyedropperSample:
		DescriptorEnumID MapEyedropperSampleToEnum
			(
			const PIUEyedropperSample_t sample
			);
		PIUEyedropperSample_t MapEnumToEyedropperSample
			(
			const DescriptorEnumID aEnum
			);
			
		PIUEyedropperSample_t GetEyedropperSample (void);
		SPErr SetEyedropperSample
			(
			/* IN */	const PIUEyedropperSample_t	inSample = kPIUInvalidEyedropperSample
			);
		
		//-----------------------------------------------------------------------
		//	Color Picker preferences.
		//-----------------------------------------------------------------------					

		// Utilities for GetColorPickerKind/SetColorPickerKind:
		DescriptorEnumID MapColorPickerKindToEnum
			(
			PIUColorPickerKind_t kind
			);
		PIUColorPickerKind_t MapEnumToColorPickerKind
			(
			DescriptorEnumID aEnum
			);
			
		PIUColorPickerKind_t GetColorPickerKind (void);
		SPErr SetColorPickerKind
			(
			/* IN */	const PIUColorPickerKind_t inKind = kPIUInvalidColorPickerKind
			);
		
		char* GetColorPickerID (void);
		SPErr SetColorPickerID
			(
			/* IN */	const char* const inID = NULL
			);
		
		//-----------------------------------------------------------------------
		//	History preferences.
		//-----------------------------------------------------------------------			

		unsigned short GetMaximumHistoryEntries (void);
		SPErr SetMaximumHistoryEntries
			(
			/* IN */	const unsigned short inNumEntries = 20
			);

		bool StartHistoryWithSnapshot (void);
		SPErr SetStartHistoryWithSnapshot
			(
			/* IN */	const bool inStartWithSnap = true
			);
			
		bool AllowNonLinearHistory (void);
		SPErr SetAllowNonLinearHistory
			(
			/* IN */	const bool inAllowNonLinear = false
			);

		//-----------------------------------------------------------------------
		//	Application properties.
		//-----------------------------------------------------------------------			
		
		char* GetSerialNumber (void);
		
		unsigned short GetWatchSuspension (void);
		SPErr SetWatchSuspension
			(
			/* IN */	const unsigned short inValue = 0
			);
		
		long GetMajorVersion (void);
		long GetMinorVersion (void);
		long GetFixVersion (void);				
		char* GetVersionAsString (void);
		
		// Utilities for GetInterfaceColors:
		DescriptorKeyID MapInterfaceColorToKey
			(
			PIUInterfaceColor_t color
			);
		PIUInterfaceColor_t MapKeyToInterfaceColor
			(
			DescriptorKeyID aKey
			);

		RGB16tuple* GetInterfaceColorsInDepth16 (void);
		RGBtuple* GetInterfaceColorsInDepth8 (void);
		
	private:
		// Override -- Must supply this map:
		virtual const DescriptorKeyID* ElementKeyMap (void) const;
	};

//-------------------------------------------------------------------------------
//
//	class PIUBasic
//
//	This is the main suite that provides all the Plug-in utility suites.
//
//-------------------------------------------------------------------------------

class PIUBasic
	{
	public:
		// Constructor:
		PIUBasic()
			{
			// Intentionally left blank.
			}
		
		// Destructor:
		~PIUBasic()
			{
			// Intentionally left blank.
			}
			
		// Lazy init.  Call later, after construction, after suites are
		// available:
		void IPIUBasic(void);
			
		// Return a pointer to the result utility suite:
		PIUResult* Result(void)
			{
			return &result_;
			}
			
		// Return a pointer to an element of this class: 
		PIUBasic* Make(void)
			{
			return new PIUBasic();
			}

		// We provde a double-dereference operator so that the user
		// may treat this just like the other suite pointers:
		PIUBasic* operator-> ()
			{
			// PIAssert(PIUSuitePointer_ != NULL);
			return this;
			}

		// Use this to access the application routines:
		PIUApplication* Application (void)
			{
			return &application_;
			}
			
		// Use this to access the document routines:
		PIUDocument* Document (void)
			{
			return &document_;
			}
			
		// Use this to access the layer routines:
		PIULayer* Layer (void)
			{
			return &layer_;
			}
			
		// Use this to access the channel routines:
		PIUChannel* Channel (void)
			{
			return &channel_;
			}
			
		// Use this to access the path routines:
		PIUPath* Path (void)
			{
			return &path_;
			}
			
		// Use this to access the history routines:
		PIUHistory* History (void)
			{
			return &history_;
			}

		// Use this to access the action set routines:
		PIUActionSet* ActionSet (void)
			{
			return &actionSet_;
			}
			
		// Use this to access the action routines:
		PIUAction* Action (void)
			{
			return &action_;
			}
			
		// Use this to access the filter routines:
		PIUFilter* Filter (void)
			{
			return &filter_;
			}
					
	private:
		// Result suite for utility functions:
		PIUResult result_;
		
		// Application-related suite of functions:
		PIUApplication application_;
		
		// Document-related suite of utilities:
		PIUDocument document_;
		
		// Layer-related suite of utilities:
		PIULayer layer_;
		
		// Channel-related suite of utilities:
		PIUChannel channel_;
		
		// Path-related suite of utilities:
		PIUPath path_;
		
		// History-related suite of utilities:
		PIUHistory history_;
		
		// Action sets-related suite of utilities:
		PIUActionSet actionSet_;
		
		// Action-related suite of utilities:
		PIUAction action_;
		
		// Filter-related suite of utilities:
		PIUFilter filter_;
		
	protected:
		// None.

	};

//-------------------------------------------------------------------------------
//	Globals.
//-------------------------------------------------------------------------------

// sPIU is a global pointer to the general utilities.  It's set up
// so you can use it just like the suite pointers:
extern PIUBasic sPIUBasic;

//-------------------------------------------------------------------------------

#endif // __PIUBasic_h__
