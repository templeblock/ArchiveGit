/* (c) Copyright 1992 MICROGRAFX, Inc., All Rights Reserved.  This material is
   confidential and a trade secret.  Permission to use this work for any purpose
   must be obtained in writing from:
              MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081

********************************************************************************
                                Forum Management
This file define the Forum/Attendee metaphor used for making changes 

*********************************************************************************/

#ifndef _FORUM_H_
#define _FORUM_H_

/* ********************************** Macros ******************************** */

#define DEFINE_FORUM_STATE(m,n)\
    private: uint m : n; public: uint Get##m () {return m;}\
    void Set##m (uint New##m,CAttendee * pSkip = NULL)\
        {if (m != (uint)New##m) {m = (uint)New##m; MakeRemark (pSkip);}}

#define SET_FORUM_STATE(m,r,x,v,a)\
    (r).Set##m (((r).Get##m () & ~(x)) | ((x) & (v)),a)

#define SET_FORUM_FLAG(m,r,v,a) SET_FORUM_STATE (m,r,1,v,a)

#define DEFINE_FORUM_FLAG(m) DEFINE_FORUM_STATE (m,1)

#define DEFINE_FORUM_TOPIC(m,c)\
    private: c m; public: c Get##m () {return m;}\
    void Set##m (c New##m,CAttendee * pSkip = NULL)\
        {if (m != New##m) {m = New##m; MakeRemark (pSkip);}}

/* ********************************** Types ********************************* */

class CForum;
class CAttendee;
class CSensor;
class CDiscourse;

typedef void * FORUMKEY;

/* ********************************* Classes ******************************** */

class CForum                            // forum base class
{
private:
    CPtrList Roster;                    // list of attendees
    BOOL bDiscourse;                    // TRUE if a discource is in progress
    BOOL bChanged;                      // TRUE when a remark has been made

public:
    CForum () :                         // constructor
        bDiscourse (FALSE),bChanged (FALSE)
        {}

    void Admit (CAttendee * pAttendee); // access methods
    void Dismiss (CAttendee * pAttendee);
    BOOL InSession ()
        {return bDiscourse;}
    void MakeRemark (CAttendee * pSkipAttendee = NULL);
    FORUMKEY StartDiscourse ();
    void StopDiscourse (FORUMKEY Podium,CAttendee * pSkipAttendee = NULL);
};

class CAttendee                         // forum attendee base class
{
protected:
    void MakeRemark (CForum & Forum)    // internal access methods
        {Forum.MakeRemark (this);}
    FORUMKEY StartDiscourse (CForum & Forum)
        {return Forum.StartDiscourse ();}
    void StopDiscourse (CForum & Forum,FORUMKEY Podium)
        {Forum.StopDiscourse (Podium,this);}

public:
    void ArriveAt (CForum & Forum)      // access methods
        {Forum.Admit (this);}
    void DepartFrom (CForum & Forum)
        {Forum.Dismiss (this);}
                                        // common methods
    virtual void Assimilate (CForum * pForum) = 0;
};

class CFontCombo : public CAttendee
{
public:
	CFontCombo();
	~CFontCombo();
	void Init(HWND hWnd);
	void NotifyFontChanged(int code);
	virtual void Assimilate (CForum * pForum);
	HWND m_hWnd;
};

class CZoomCombo : public CAttendee
{
public:
	CZoomCombo();
	~CZoomCombo();
	void Init(HWND hWnd);
	void NotifyChanged(int code);
	virtual void Assimilate (CForum * pForum);
	HWND m_hWnd;
};

class CStatusMonitor : public CAttendee
{
public:
	CStatusMonitor();
	~CStatusMonitor();
	void Init(HWND hWnd);
	virtual void Assimilate (CForum * pForum);
	HWND m_hWnd;
	int		y;
};


class CSensor                           // remote forum data item class
{
private:
    CForum & Forum;                     // optional reference forum

protected:                              // internal access methods
    void MakeRemark (CAttendee * pSkipAttendee = NULL)
        {Forum.MakeRemark (pSkipAttendee);}

public:
    CSensor (CForum & Ref) :            // constructor
        Forum (Ref)
        {}

    friend class CDiscourse;
};


class CDiscourse                        // forum discourse class
{
private:
    CForum & Forum;                     // reference forum
    FORUMKEY Podium;                    // forum key during discourse
    CAttendee * pAttendee;              // optional skip attendee

public:                                 // constructor
    CDiscourse (CForum & Ref,CAttendee * pSkip = NULL) :
        Forum (Ref),Podium (Forum.StartDiscourse ()),pAttendee (pSkip)
        {}
    CDiscourse (CSensor & Ref,CAttendee * pSkip = NULL) :
        Forum (Ref.Forum),Podium (Forum.StartDiscourse ()),pAttendee (pSkip)
        {}

    ~CDiscourse ()                      // destructor
        {Forum.StopDiscourse (Podium,pAttendee);}
};

/* ******************************* Public Data ****************************** */
/* ***************************** Public Functions *************************** */

#endif
