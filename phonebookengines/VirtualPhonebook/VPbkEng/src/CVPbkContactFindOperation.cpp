/*
* Copyright (c) 2005-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  An implementation for find operation. Can be used e.g
*                in store domain implementation to handle Find -operations
*                from stores.
*
*/


#include "CVPbkContactFindOperation.h"

#include <CVPbkContactLinkArray.h>
#include <MVPbkContactLink.h>

CVPbkContactFindOperation::CVPbkContactFindOperation(
        MVPbkContactFindObserver& aObserver) :
    iObserver(aObserver)
    {
    }

inline void CVPbkContactFindOperation::ConstructL()
    {
    }

EXPORT_C CVPbkContactFindOperation* CVPbkContactFindOperation::NewLC(
        MVPbkContactFindObserver& aObserver)
    {
    CVPbkContactFindOperation* self = new(ELeave) CVPbkContactFindOperation(aObserver);
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
    }

CVPbkContactFindOperation::~CVPbkContactFindOperation()
    {
    delete iResults;
    }

void CVPbkContactFindOperation::HandleZeroSuboperations()
    {
    // If no suboperations then complete find with empty array.
    // If completing leaves then call FindFailed 
    TRAPD( res, HandleZeroSuboperationsL() );
    if ( res != KErrNone )
        {
        iObserver.FindFailed( res );
        }
    }
    
void CVPbkContactFindOperation::FindCompleteL(MVPbkContactLinkArray* aResults)
    {
    ++iCompleteCount;

    // take ownership of aResults immediately
    MVPbkContactLinkArray* passedResults = aResults;
    CleanupDeletePushL(passedResults);

    // lazy creation of result array
    if (!iResults)
        {
        iResults = CVPbkContactLinkArray::NewL();
        }

    if (aResults)
        {
        // append results to this operations array
        for (TInt i = aResults->Count() - 1; i >= 0; --i)
            {
            MVPbkContactLink* link = aResults->At(i).CloneLC();
            iResults->AppendL(link);
            CleanupStack::Pop(); // link
            }
        }
    CleanupStack::PopAndDestroy(passedResults);

    // notify clients if all suboperations have completed
    if (iCompleteCount >= SubOperationCount())
        {
        // takes ownership of results
        CVPbkContactLinkArray* results = iResults;
        iResults = NULL;
        iObserver.FindCompleteL(results);
        }
    }

void CVPbkContactFindOperation::FindFailed(TInt aError)
    {
    ++iCompleteCount;

    // notify clients if all suboperations have completed
    if (iCompleteCount >= SubOperationCount())
        {
        // if there are results give them to the client
        if (iResults)
            {
            // the observer takes ownership of results
            CVPbkContactLinkArray* results = iResults;
            iResults = NULL;
            TRAPD(err, iObserver.FindCompleteL(results));
            if (err)
                {
                // client must take ownership of results before it can leave
                iObserver.FindFailed(aError);
                }
            }
        else
            {
            // otherwise inform find failure
            iObserver.FindFailed(aError);
            }
        }
    }
    
// -----------------------------------------------------------------------------
// CVPbkContactFindOperation::HandleZeroSuboperationsL
// -----------------------------------------------------------------------------
//     
void CVPbkContactFindOperation::HandleZeroSuboperationsL()
    {
    CVPbkContactLinkArray* emptyResults = CVPbkContactLinkArray::NewL();
    // Client takes the ownership of the results
    iObserver.FindCompleteL( emptyResults );
    }
// End of File
