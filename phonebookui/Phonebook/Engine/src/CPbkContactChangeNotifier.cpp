/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: 
*        Class for registering CPbkContactEngine observers
*
*/


// INCLUDE FILES
#include "CPbkContactChangeNotifier.h"
#include "CPbkContactEngine.h"


/// Unnamed namespace for local definitions
namespace {

#ifdef _DEBUG
// LOCAL CONSTANTS AND MACROS
enum TPanicCode
    {
    EPanicPreCond_NewL = 1
    };

// LOCAL FUNCTIONS
static void Panic(TPanicCode aReason)
    {
    _LIT(KPanicText, "CPbkContactChangeNotifier");
    User::Panic(KPanicText, aReason);
    }
#endif // _DEBUG

} // namespace

// ==================== MEMBER FUNCTIONS ====================

inline CPbkContactChangeNotifier::CPbkContactChangeNotifier
        (MPbkContactDbObserver* aObserver)
    : iEngine(NULL), iObserver(aObserver)
    {
    }

inline void CPbkContactChangeNotifier::ConstructL(CPbkContactEngine& aEngine)
    {
    aEngine.AddObserverL(iObserver);
    // AddObserverL was succesful, set iEngine member
    iEngine = &aEngine;
    }

EXPORT_C CPbkContactChangeNotifier* CPbkContactChangeNotifier::NewL
        (CPbkContactEngine& aEngine, MPbkContactDbObserver* aObserver)
    {
    __ASSERT_DEBUG(aObserver, Panic(EPanicPreCond_NewL));

    CPbkContactChangeNotifier* self = 
        new(ELeave) CPbkContactChangeNotifier(aObserver);
    CleanupStack::PushL(self);
    self->ConstructL(aEngine);
    CleanupStack::Pop(self); 
    return self;
    }

CPbkContactChangeNotifier::~CPbkContactChangeNotifier()
    {
    if (iEngine)
        {
        iEngine->RemoveObserver(iObserver);
        }
    }


// End of File
