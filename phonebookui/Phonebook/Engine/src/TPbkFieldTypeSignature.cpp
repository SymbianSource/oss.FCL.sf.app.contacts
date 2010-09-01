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
*		Phonebook field type signature
*
*/


// INCLUDE FILES
#include "TPbkFieldTypeSignature.h"
#include "CPbkUidMap.h"
#include "MPbkVcardParameters.h"

/// Unnamed namespace for local definitions
namespace {

// LOCAL CONSTANTS AND MACROS
enum TPanicCode
    {
    EPanicUidMapTooBig = 1
    };

//This could be too small, if cntdef.hrh file's
//mapping values increase 
//If you update this value, remember update also 
//iSignatures table.
const TInt KMaxUidMapCount = 128;
const TInt KWordLimit = 32;

// LOCAL FUNCTIONS
void Panic(TPanicCode aReason)
    {
    _LIT(KPanicText, "TPbkFieldTypeSignature");
    User::Panic(KPanicText, aReason);
    }
}


// ==================== MEMBER FUNCTIONS ====================

void TPbkFieldTypeSignature::AddType
        (const TUid& aUid, const CPbkUidMap& aUidMap)
    {
    TInt index = aUidMap.IndexOf(aUid);
    __ASSERT_ALWAYS(index < KMaxUidMapCount, Panic(EPanicUidMapTooBig));
    if (index >= 0)
        {                        
        iSignatures[index/KWordLimit] |= (TUint32(1) << index%KWordLimit);                   
        }
    }

void TPbkFieldTypeSignature::AddParameters
        (const MPbkVcardParameters& aParameters, const CPbkUidMap& aUidMap)
    {
    const TInt count = aParameters.ParameterCount();
    for (TInt i=0; i < count; ++i)
        {
        AddType(aParameters.ParameterAt(i), aUidMap);
        }
    }


// End of File
