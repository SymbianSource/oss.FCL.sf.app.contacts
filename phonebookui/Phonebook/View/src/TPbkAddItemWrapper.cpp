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
*       Provides methods for BCard export property..
*
*/


// INCLUDE FILES

// This class
#include "TPbkAddItemWrapper.h"

// PbkEng classes
#include <CPbkFieldInfo.h>
#include <CPbkFieldInfoGroup.h>


/// Unnamed namespace for local defintions
namespace {
    
// LOCAL CONSTANTS AND MACROS
    
#ifdef _DEBUG
enum TPanicCode
    {
    EPanicPreCond_FieldInfoAt = 1,
    EPanicPreCond_FieldInfoAtGroup
    };
#endif

// ==================== LOCAL FUNCTIONS ====================

#ifdef _DEBUG
void Panic(TPanicCode aPanicCode)
    {
    _LIT(KPanicText, "TPbkAddItemWrapper");
    User::Panic(KPanicText, aPanicCode);
    }
#endif

} // namespace


// ==================== MEBER FUNCTIONS ====================

TPbkAddItemWrapper::TPbkAddItemWrapper(const CPbkFieldInfo& aFieldInfo)
		: iFieldInfo(&aFieldInfo), iInfoGroup(NULL)
    {
    }

TPbkAddItemWrapper::TPbkAddItemWrapper(const CPbkFieldInfoGroup& aInfoGroup)
		: iFieldInfo(NULL), iInfoGroup(&aInfoGroup)
    {
    }

TPbkAddItemWrapper::~TPbkAddItemWrapper()
    {
    }


const TDesC& TPbkAddItemWrapper::Label() const
    {
    if (iFieldInfo)
        {
        return iFieldInfo->AddItemText();
        }
    else
        {
        return iInfoGroup->Label();
        }
    }

TPbkIconId TPbkAddItemWrapper::IconId() const
    {
    if (iFieldInfo)
        {
        return iFieldInfo->IconId();
        }
    else
        {
        return iInfoGroup->IconId();        
        }    
    }

TInt TPbkAddItemWrapper::FieldInfoCount() const
    {
    if (iFieldInfo)
        {
        // only one item to return
        return 1;
        }
    else
        {
        return iInfoGroup->Count();        
        }        
    }

CPbkFieldInfo* TPbkAddItemWrapper::FieldInfoAt(const TInt aIndex) const
    {
    if (iFieldInfo)
        {
        __ASSERT_DEBUG(aIndex == 0, Panic(EPanicPreCond_FieldInfoAt));
        // only one item to return
        return MUTABLE_CAST(CPbkFieldInfo*, iFieldInfo);
        }
    else
        {
        __ASSERT_DEBUG((aIndex >= 0) && (aIndex < iInfoGroup->Count()),
            Panic(EPanicPreCond_FieldInfoAtGroup));
        if ((aIndex >= 0) && (aIndex < iInfoGroup->Count()))
            {
            return MUTABLE_CAST(CPbkFieldInfo*, &iInfoGroup->At(aIndex));
            }
        else
            {
            return NULL;
            }
        }        
    }

TPbkAddItemOrdering TPbkAddItemWrapper::AddItemOrdering() const
    {
    if (iFieldInfo)
        {
        return iFieldInfo->AddItemOrdering();
        }
    else
        {
        return iInfoGroup->AddItemOrdering();        
        }            
    }


// End of File
