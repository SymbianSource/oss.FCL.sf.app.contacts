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
*       Methods for Phonebook contact view listbox model.
*
*/


// INCLUDE FILES
#include "CPbkContactViewListFixedIconModel.h"  // This class
#include <CPbkIconArray.h>

// PbkEng.dll include files
#include <CPbkContactEngine.h>
#include <TPbkContactEntry.h>
#include <PbkEngUtils.h>
#include <MPbkContactNameFormat.h>
#include <CPbkViewContactNameFormatter.h>

#include <PbkDebug.h>


/// Unnamed namespace for local definitions
namespace {

// LOCAL CONSTANTS AND MACROS

#ifdef _DEBUG
enum TPanicCode
    {
    EPanicPreCond_NewL = 1
    };
#endif


// ==================== LOCAL FUNCTIONS ====================

#ifdef _DEBUG
void Panic(TPanicCode aReason)
    {
    _LIT(KPanicText, "CPbkContactViewListFixedIconModel");
    User::Panic(KPanicText, aReason);
    }
#endif

}  // namespace


// ================= MEMBER FUNCTIONS =======================

inline CPbkContactViewListFixedIconModel::CPbkContactViewListFixedIconModel
    (PbkContactViewListModelFactory::TParams& aParams) : 
    CPbkContactViewListModelCommon(aParams),
    iNameFormatter(aParams.iEngine->ContactNameFormat())
    {
    // CBase::operator new(TLeave) resets other member data
    }

inline void CPbkContactViewListFixedIconModel::ConstructL()
    {    
    PBK_DEBUG_PRINT
        (PBK_DEBUG_STRING("CPbkContactViewListFixedIconModel"));
    __PBK_PROFILE_START(1);
    CPbkContactViewListModelCommon::ConstructL();
    iViewContactNameFormatter = 
        CPbkViewContactNameFormatter::NewL(iNameFormatter);
    __PBK_PROFILE_END(1);
    __PBK_PROFILE_DISPLAY(1);
    }

CPbkContactViewListFixedIconModel* CPbkContactViewListFixedIconModel::NewL
        (PbkContactViewListModelFactory::TParams& aParams)
    {
    __ASSERT_DEBUG(aParams.iEngine && aParams.iView && aParams.iIconArray, 
        Panic(EPanicPreCond_NewL));
    __ASSERT_DEBUG(aParams.iDefaultId != EPbkNullIconId || 
                   aParams.iEmptyId != EPbkNullIconId,
                   Panic(EPanicPreCond_NewL));
        
    CPbkContactViewListFixedIconModel* self =
        new(ELeave) CPbkContactViewListFixedIconModel(aParams);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

CPbkContactViewListFixedIconModel::~CPbkContactViewListFixedIconModel()
    {
    delete iViewContactNameFormatter;
    }

const TText KSeparator = '\t';

void CPbkContactViewListFixedIconModel::FormatBufferForContactL
        (const CViewContact& aViewContact) const
    {
    // Format only name using aViewContact
    iBuffer.Zero();
    AppendIconIndex(EPbkNullIconId);
    iBuffer.Append(KSeparator);
    TPtrC name(iViewContactNameFormatter->ContactTitleL
        (aViewContact, ViewSortOrder()));
    if (name.Length() == 0)
        {
        name.Set(iNameFormatter.UnnamedText());
        }
    AppendName(name);
    }

//  End of File  
