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
*		Phonebook vCard property
*
*/


// INCLUDE FILES
#include "CPbkVcardProperty.h"
#include <barsread.h>


// ==================== MEMBER FUNCTIONS ====================

inline CPbkVcardProperty::CPbkVcardProperty()
    {
    }

inline void CPbkVcardProperty::ConstructL(TResourceReader& aReader)
    {
    iName = TUid::Uid(aReader.ReadInt32());
    iParameters.ConstructL(aReader);
    }

CPbkVcardProperty* CPbkVcardProperty::NewLC(TResourceReader& aReader)
    {
    CPbkVcardProperty* self = new(ELeave) CPbkVcardProperty;
    CleanupStack::PushL(self);
    self->ConstructL(aReader);
    return self;    
    }

CPbkVcardProperty::~CPbkVcardProperty()
    {
    }

TUid CPbkVcardProperty::PropertyName() const
    {
    return iName;
    }

const MPbkVcardParameters& CPbkVcardProperty::PropertyParameters() const
    {
    return iParameters;
    }


// End of File
