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
*		Abstract Phonebook entry interface
*
*/


// INCLUDE FILES
#include "CPbkIdleFinder.h" // this class
#include <cntdb.h>          // CIdleFinder
#include <cntfield.h>       // CContactItemFieldDef


// ==================== MEMBER FUNCTIONS ====================

CPbkIdleFinder::CPbkIdleFinder
        (CIdleFinder* aFinder, CContactItemFieldDef* aFieldDef)
    : iFinder(aFinder), iFieldDef(aFieldDef)
    {
    }

CPbkIdleFinder::~CPbkIdleFinder()
    {
    delete iFinder;
    delete iFieldDef;
    }

EXPORT_C TBool CPbkIdleFinder::IsComplete() const
    {
    return iFinder->IsComplete();
    }

EXPORT_C CContactIdArray* CPbkIdleFinder::TakeContactIds()
    {
    return iFinder->TakeContactIds();
    }

EXPORT_C TInt CPbkIdleFinder::Error() const
    {
    return iFinder->Error();
    }

EXPORT_C CIdleFinder* CPbkIdleFinder::IdleFinder() const
    {
    return iFinder;
    }

EXPORT_C CContactItemFieldDef* CPbkIdleFinder::FieldDef() const
    {
    return iFieldDef;
    }


// End of File
