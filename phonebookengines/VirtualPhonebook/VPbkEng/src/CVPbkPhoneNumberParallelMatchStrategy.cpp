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
* Description:  Phone number matching strategy for parallel matching.
*
*/


#include "CVPbkPhoneNumberParallelMatchStrategy.h"

#include <MVPbkContactStore.h>
#include <CVPbkContactFindOperation.h>
#include <cntdb.h>

CVPbkPhoneNumberParallelMatchStrategy::CVPbkPhoneNumberParallelMatchStrategy()
    {
    }

inline void CVPbkPhoneNumberParallelMatchStrategy::ConstructL(
        const TConfig& aConfig,
        CVPbkContactManager& aContactManager, 
        MVPbkContactFindObserver& aObserver)
    {
    BaseConstructL(aConfig, aContactManager, aObserver);
    }

CVPbkPhoneNumberParallelMatchStrategy* CVPbkPhoneNumberParallelMatchStrategy::NewL(
        const TConfig& aConfig,
        CVPbkContactManager& aContactManager, 
        MVPbkContactFindObserver& aObserver)
    {
    CVPbkPhoneNumberParallelMatchStrategy* self = 
        new(ELeave) CVPbkPhoneNumberParallelMatchStrategy;
    CleanupStack::PushL(self);
    self->ConstructL(aConfig, aContactManager, aObserver);
    CleanupStack::Pop(self);
    return self;
    }

CVPbkPhoneNumberParallelMatchStrategy::~CVPbkPhoneNumberParallelMatchStrategy()
    {
    }

MVPbkContactOperation* CVPbkPhoneNumberParallelMatchStrategy::CreateFindOperationLC(
        const TDesC& aPhoneNumber)
    {
    CVPbkContactFindOperation* operation = NULL;
    
    if (!iMatchingStarted)
        {
        operation = CVPbkContactFindOperation::NewLC(FindObserver());
        const TInt storeCount = StoresToMatch().Count();
        TInt maxDigits = MaxMatchDigits();
        
        for (TInt i = 0; i < storeCount; ++i)
            {
            MVPbkContactOperation* subOperation = NULL;
            
            if ( maxDigits == KBestMatchingPhoneNumbers &&
                    IsSimStore( *( StoresToMatch()[i] ) ) )
                {
                // KBestMatchingPhoneNumbers enables best matching strategy 
                // on store level only for phone memory stores, for sim store
                // MaxDigits parameter should be set to 7 or greater
                const TInt KMaxDigitsForSimStore = 7;
                subOperation = StoresToMatch()[i]->CreateMatchPhoneNumberOperationL(
                        aPhoneNumber, KMaxDigitsForSimStore, *operation);
                }
            else
                {
                subOperation = StoresToMatch()[i]->CreateMatchPhoneNumberOperationL(
                        aPhoneNumber, maxDigits, *operation);
                }

            if (subOperation)
                {
                CleanupDeletePushL(subOperation);
                operation->AddSubOperationL(subOperation);
                CleanupStack::Pop(); // subOperation
                }
            }

        if (operation->SubOperationCount() == 0)
            {
            CleanupStack::PopAndDestroy(); // operation
            operation = NULL;
            }
        iMatchingStarted = ETrue;
        }

    return operation;
    }

void CVPbkPhoneNumberParallelMatchStrategy::InitMatchingL()
    {
    iMatchingStarted = EFalse;
    }
    
// End of File
