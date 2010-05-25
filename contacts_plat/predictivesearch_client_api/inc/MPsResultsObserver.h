/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:  The observer interface which handles the predictive search
*                results from the server.
*
*/


#ifndef __M_PS_RESULTS_OBSERVER__ 
#define __M_PS_RESULTS_OBSERVER__

#include <CPcsDefs.h>
#include <CPsPattern.h>

// FORWARD DECLARATION
class CPsClientData;

// CLASS DECLARATION
/**
*  MPsResultsObserver
*  Mixin class.
*  Observer which handles updates from the predictive search server.
*/
class MPsResultsObserver
{
    public:

        /**
        * HandlePsResultsUpdateL.
        * Handle updates from the server.
        * 
        * @param searchResults Search results returned from the server.
        * @param searchSeqs    List of matching character sequences.
        */
        virtual void HandlePsResultsUpdate(RPointerArray<CPsClientData>& aSearchResults,
                                           RPointerArray<CPsPattern>& aSearchSeqs) = 0;

        /**
        * HandlePsErrorL.
        * Handle errors from the server.
        * 
        * @param aErrorCode Search errors returned from the server.
        */
        virtual void HandlePsError(TInt aErrorCode) = 0;

        /**
        * CachingStatus.
        * Gets called when caching is completed, or cache has been modified after
        * the initial caching. For update events, it's not guaranteed that function
        * gets called separately for each modification in case there are several same kind of
        * modifications happening in rapid sequence.
        * Observers need to implement this function accordingly
        * to take action after cache completion or update.
        * @param aStatus - caching status
        * @param aError - Any error that occurred while caching. 
        *                 KErrNone if no error, else the error code
        */
        virtual void CachingStatus(TCachingStatus& aStatus, TInt& aError) = 0;
};

#endif // __M_PS_RESULTS_OBSERVER__

// End of File
