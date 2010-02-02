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
*		Phonebook vCard parameters
*
*/


// INCLUDE FILES
#include "CPbkVcardParameters.h"
#include <barsread.h>


// ==================== MEMBER FUNCTIONS ====================

CPbkVcardParameters* CPbkVcardParameters::NewL(TResourceReader& aReader)
    {
    CPbkVcardParameters* self = new(ELeave) CPbkVcardParameters;
    CleanupStack::PushL(self);
    self->ConstructL(aReader);
    CleanupStack::Pop(self);
    return self;    
    }

CPbkVcardParameters::CPbkVcardParameters()
    {
    }

void CPbkVcardParameters::ConstructL(TResourceReader& aReader)
    {
	TInt count=aReader.ReadInt16();
	while (count-- > 0)
		{
        User::LeaveIfError(iParameters.Append(TUid::Uid(aReader.ReadInt32())));
        }
    }

CPbkVcardParameters::~CPbkVcardParameters()
    {
    iParameters.Close();
    }

TInt CPbkVcardParameters::ParameterCount() const
    {
    return iParameters.Count();    
    }

TUid CPbkVcardParameters::ParameterAt(TInt aIndex) const
    {
    return iParameters[aIndex];     
    }


// End of File
