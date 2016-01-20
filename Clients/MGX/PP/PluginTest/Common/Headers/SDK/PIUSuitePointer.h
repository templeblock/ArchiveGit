//-------------------------------------------------------------------------------
//
//	File:
//		PIUSuitePointer.h
//
//	Copyright 1997-1998, Adobe Systems Incorporated.
//	All Rights Reserved.
//
//	Description:
//		This library contains the source and
//		routines for a set of smart suite pointers
//		that automatically Acquire and Release suites
//		in a global scope for easy plug-in use.
//
//	Use:
//		Refer to PIUSuites.cpp for a list of suite-specific
//		definitions that make use of this generic suite pointer.
//
//	Version history:
//		Version 1.0.0	7/31/1997	Created for Photoshop 5.0
//			Written by Andrew Coven.
//
//-------------------------------------------------------------------------------

#ifndef __PIUSuitePointer_h__
#define __PIUSuitePointer_h__

#include "PIUTools.h"

//-------------------------------------------------------------------------------
//	PIUSuitePointer is a smart suite pointer class.
//-------------------------------------------------------------------------------
template <class T>
class PIUSuitePointer
{
	public:
		// Constructor:
		PIUSuitePointer
			(
			SPBasicSuite* inSPBasic,
			char* id,
			short version
			);
		
		// Copy constructor:
		PIUSuitePointer
			(
			const PIUSuitePointer<T>& source
			);
		
		// Assignment operator:
		PIUSuitePointer<T>& operator=
			(
			const PIUSuitePointer<T>& source
			);
		
		// Destructor:
		~PIUSuitePointer();
		
		// Check for a valid suite loaded:
		bool IsValid();

		// Check if this suite is in an optional list:
		bool IsOptional
			(
			char** ids
			) const;
		
		// Reacquire suite with new SPBasic suite:
		SPErr ReAcquire
			(
			SPBasicSuite* inSPBasic
			);
		
		// Dereference operator:
		T& operator* ()
			{
			// PIAssert(PIUSuitePointer_ != NULL);
			return *PIUSuitePointer_;
			}
		
		// Double-dereference operator:
		T* operator-> ()
			{
			// PIAssert(PIUSuitePointer_ != NULL);
			return PIUSuitePointer_;
			}

		// Return ID:
		char* GetID (void) const
			{
			return id_;
			}
		
	private:		
		// Default constructor undefined:
		PIUSuitePointer();
		SPErr Acquire();
		void Release();
	protected:
		T* PIUSuitePointer_;
		char* id_;
		short version_;
		SPBasicSuite* sSPBasic_;
};

//-------------------------------------------------------------------------------
//	Specific template routines.
//-------------------------------------------------------------------------------
template <class T>
	SPErr PIUSuitePointer<T>::Acquire()
	{
	SPErr error = kSPNoError;
	
	if (sSPBasic_ != NULL && id_ != NULL)
		{
		error = sSPBasic_->AcquireSuite
			(
			id_,
	   		version_,
	   		(void**) &PIUSuitePointer_
	   		);
		}
	else
		{
		error = kSPBadParameterError; // Need sSPBasic and id_.
		}
	
	return error;
	};

template <class T>
	void PIUSuitePointer<T>::Release()
	{
	SPErr error = kSPNoError;
	
	if (sSPBasic_ != NULL && PIUSuitePointer_ != NULL && 
		id_ != NULL)
		{
		error = sSPBasic_->ReleaseSuite
			(
			id_,
			version_
			);
		}
	else
		{
		// Oh oh, suite missing.
		}
	PIUSuitePointer_ = NULL; // reset
	};

template <class T>
	bool PIUSuitePointer<T>::IsValid()
	{
	bool suiteValid = false;
	
	if (PIUSuitePointer_ != NULL && id_ != NULL)
		{
		suiteValid = true;
		}
	return suiteValid;
	};

template <class T>
	bool PIUSuitePointer<T>::IsOptional
	(
	char** ids
	) const
	{
	bool isOptional = false;
	
	unsigned short loop = 0;
	bool done = false;

	const char* const id = GetID();

	if (id != NULL)
		{
			while (ids[loop] != NULL && !done)
			{
			if (PIUMatch(id, ids[loop]) == kGotMatch)
				{
				// Found it in optional list.
				done = true;
				isOptional = true;
				}

			++loop;
			}
		}

	return isOptional;
	}
		

template <class T>
	PIUSuitePointer<T>::PIUSuitePointer
		(
		SPBasicSuite* inSPBasic,
		char* id,
		short version
		) :
		sSPBasic_(inSPBasic),
		id_(id),
		version_(version)
	{
	SPErr error = Acquire();

	if (error != kSPNoError)
		{ 
		// Oh oh.  Couldn't acquire suite.
		}
	};

template <class T>
	PIUSuitePointer<T>::~PIUSuitePointer()
	{
	Release();
	};

template <class T>
	PIUSuitePointer<T>::PIUSuitePointer
		(
		const PIUSuitePointer<T>& source
		)
	{
	if (source.sSPBasic_ != NULL)
		sSPBasic_ = source.sSPBasic_;
		
	if
		(
		(this->id_ != source.id_) ||
		(this->version_ != source.version_) ||
		(this->PIUSuitePointer_ != source.PIUSuitePointer_)
		)
		{ // No self copy.
		if (source.PIUSuitePointer_ != NULL)
			{
			// Only copy if not NULL.
			if (this->PIUSuitePointer_ != NULL)
				Release();

			this->PIUSuitePointer_ = source.PIUSuitePointer_;
			}

		if (source.id_ != NULL)
			this->id_ = source.id_;

		this->version_ = source.version_;
		
		/*
		T* copy_PIUSuitePointer = PIUSuitePointer_;
		char* copy_id = id_;
		short copy_version = version_;

		id_ = source.id_;
		version_ = source.version_;
		if (Acquire() == kSPNoError)
			{ // Succeeded.  Release old copy:
			T* new_PIUSuitePointer = PIUSuitePointer_;
			id_ = copy_id;
			version_ = copy_version;
			Release();
			
			// Now restore new info:
			id_ = source.id_;
			version_ = source.version_;
			PIUSuitePointer_ = new_PIUSuitePointer;
			}
		else
			{ // Poop.  Acquire failed.  Return to previous.
			PIUSuitePointer_ = copy_PIUSuitePointer;
			version = copy_version;
			if (copy_id != NULL)
				id_ = copy_id;
			}
		*/
		}
	}
	
template <class T>
	PIUSuitePointer<T>& PIUSuitePointer<T>::operator= 
		(
		const PIUSuitePointer<T>& source
		)
	{
	if (source.sSPBasic_ != NULL)
		this->sSPBasic_ = source.sSPBasic_;
		
	if
		(
		(this->id_ != source.id_) ||
		(this->version_ != source.version_) ||
		(this->PIUSuitePointer_ != source.PIUSuitePointer_)
		)
		{ // No self copy.
		if (source.PIUSuitePointer_ != NULL)
			{
			// Only copy if not NULL.
			if (this->PIUSuitePointer_ != NULL)
				Release();

			this->PIUSuitePointer_ = source.PIUSuitePointer_;
			}

		if (source.id_ != NULL)
			this->id_ = source.id_;

		this->version_ = source.version_;
		
		 /*
		T* copy_PIUSuitePointer = PIUSuitePointer_;
		char* copy_id = id_;
		short copy_version = version_;

		id_ = source.id_;
		version_ = source.version_;
		if (Acquire() == kSPNoError)
			{ // Succeeded.  Release old copy:
			T* new_PIUSuitePointer = PIUSuitePointer_;
			id_ = copy_id;
			version_ = copy_version;
			Release();
			
			// Now restore new info:
			id_ = source.id_;
			version_ = source.version_;
			PIUSuitePointer_ = new_PIUSuitePointer;
			}
		else
			{ // Poop.  Acquire failed.  Return to previous.
			PIUSuitePointer_ = copy_PIUSuitePointer;
			version_ = copy_version;
			if (copy_id != NULL)
				id_ = copy_id;
			}
		*/

		}
	
	return *this;
	
	};

template <class T>
	SPErr PIUSuitePointer<T>::ReAcquire(SPBasicSuite* inSPBasic)
	{
	Release();
	sSPBasic_ = inSPBasic;
	return Acquire();
	};

//-------------------------------------------------------------------------------

#endif // __PIUSuitePointer_h__
