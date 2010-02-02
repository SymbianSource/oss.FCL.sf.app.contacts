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
* Description:  Phone number matching strategy for sequential matching.
*
*/


#include "CVPbkPhoneNumberSequentialMatchStrategy.h"

#include <MVPbkContactStore.h>
#include <MVPbkContactOperation.h>

CVPbkPhoneNumberSequentialMatchStrategy::CVPbkPhoneNumberSequentialMatchStrategy() 
    {
    }

inline void CVPbkPhoneNumberSequentialMatchStrategy::ConstructL(
        const TConfig& aConfig,
        CVPbkContactManager& aContactManager,
        MVPbkContactFindObserver& aObserver)
    {
    BaseConstructL(aConfig, aContactManager, aObserver);
    }

CVPbkPhoneNumberSequentialMatchStrategy* CVPbkPhoneNumberSequentialMatchStrategy::NewL(
        const TConfig& aConfig,
        CVPbkContactManager& aContactManager, 
        MVPbkContactFindObserver& aObserver)
    {
    CVPbkPhoneNumberSequentialMatchStrategy* self = 
            new(ELeave) CVPbkPhoneNumberSequentialMatchStrategy;
    CleanupStack::PushL(self);
    self->ConstructL(aConfig, aContactManager, aObserver);
    CleanupStack::Pop(self);
    return self;
    }

CVPbkPhoneNumberSequentialMatchStrategy::~CVPbkPhoneNumberSequentialMatchStrategy()
    {
    }

MVPbkContactOperation* CVPbkPhoneNumberSequentialMatchStrategy::CreateFindOperationLC(
        const TDesC& aPhoneNumber)
    {
    MVPbkContactOperation* operation = NULL;

    // Get the next store's operation if it not null
    while ( iCurrentOperation < StoresToMatch().Count() && !operation )
        {
        operation = StoresToMatch()[iCurrentOperation]->CreateMatchPhoneNumberOperationL(
                aPhoneNumber, MaxMatchDigits(), FindObserver());
        
        if ( operation )
            {
            CleanupDeletePushL(operation);
            // Go to next operation
            }
        ++iCurrentOperation;
        }

    return operation;
    }

void CVPbkPhoneNumberSequentialMatchStrategy::InitMatchingL()
    {
    iCurrentOperation = 0;
    }
    
// End of File
