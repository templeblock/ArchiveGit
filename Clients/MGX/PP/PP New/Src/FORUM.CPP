/* (c) Copyright 1992 MICROGRAFX, Inc., All Rights Reserved.  This material is
   confidential and a trade secret.  Permission to use this work for any purpose
   must be obtained in writing from:
              MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081

********************************************************************************
********************************************************************************

                                Forum Management

********************************************************************************
********************************************************************************

*/
#include "pp.h"
#include "forum.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif
#define new DEBUG_NEW

/* ******************************** Constants ******************************* */
/* ********************************** Macros ******************************** */
/* ********************************** Types ********************************* */
/* ******************************* Local Data ******************************* */
/* ******************************* Public Data ****************************** */
/* ***************************** Local Functions **************************** */
/* ***************************** Public Functions *************************** */

void CForum::Admit (CAttendee * pAttendee)
{
	ASSERT (Roster.Find (pAttendee) == NULL);
    Roster.AddHead (pAttendee);
}


void CForum::Dismiss (CAttendee * pAttendee)
{
    ASSERT (Roster.Find (pAttendee));
    Roster.RemoveAt (Roster.Find (pAttendee));

}


void CForum::MakeRemark (CAttendee * pSkipAttendee)
{
    bChanged = TRUE;
    if (!InSession ())
        StopDiscourse (StartDiscourse (),pSkipAttendee);
}


FORUMKEY CForum::StartDiscourse ()
{
    return bDiscourse ? NULL : (bDiscourse = TRUE, &bDiscourse);
}


void CForum::StopDiscourse (FORUMKEY Podium, CAttendee * pSkipAttendee)
{
    if (Podium)
    {
        ASSERT (bDiscourse);
        ASSERT (Podium == &bDiscourse);
        if (bChanged)
        {
            bChanged = FALSE;
            POSITION Pos = Roster.GetHeadPosition ();
            while (Pos)
            {
                CAttendee * pAttendee = (CAttendee *)Roster.GetNext (Pos);
                if (pAttendee && pAttendee != pSkipAttendee)
                    pAttendee->Assimilate (this);
            }
        }
        bDiscourse = FALSE;
    }
}
