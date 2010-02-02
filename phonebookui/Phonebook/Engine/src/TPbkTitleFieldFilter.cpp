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
*		Generic contact title field filter
*
*/


// INCLUDE FILES
#include "TPbkTitleFieldFilter.h" // this class

namespace {

// LOCAL CONSTANTS AND MACROS

#ifdef _DEBUG
enum TPanicCode
    {
    EPanicPreCond_PbkFieldTime = 1
    };
#endif

// ==================== LOCAL FUNCTIONS ====================

#ifdef _DEBUG
void Panic(TInt aReason)
    {
    _LIT(KPanicText, "TPbkTitleFieldFilter");
    User::Panic(KPanicText, aReason);
    }
#endif


// MODULE DATA STRUCTURES


}  // namespace



// ==================== MEMBER FUNCTIONS ====================

TPbkTitleFieldData::TPbkTitleFieldData() :
    iFieldId(EPbkFieldIdNone)
    {
    }

void TPbkTitleFieldData::Set
        (TPbkFieldId aFieldId, 
        const TDesC& aText)
    {
    iFieldId = aFieldId;
    iFieldText.Set(aText);
    }

TStorageType TPbkTitleFieldData::PbkFieldType() const
    {
    return KStorageTypeText;
    }

TPbkFieldId TPbkTitleFieldData::PbkFieldId() const
    {
    return iFieldId;
    }

TPtrC TPbkTitleFieldData::PbkFieldText() const
    {
    return iFieldText;
    }

TTime TPbkTitleFieldData::PbkFieldTime() const
    {
    // This function should never be called
    __ASSERT_DEBUG(EFalse, Panic(EPanicPreCond_PbkFieldTime));
    return Time::NullTTime();
    }


// End of File
