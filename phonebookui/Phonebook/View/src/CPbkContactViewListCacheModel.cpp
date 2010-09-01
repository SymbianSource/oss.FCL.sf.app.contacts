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
*       Methods for phonebook contact view listbox model.
*
*/


// INCLUDE FILES
#include "CPbkContactViewListCacheModel.h"  // This class
#include <CPbkIconArray.h>

// PbkEng.dll include files
#include <CPbkContactEngine.h>
#include <TPbkContactEntry.h>
#include <CPbkEntryCache.h>
#include <PbkEngUtils.h>
#include <MPbkViewExtension.h>

#include <PbkDebug.h>


/// Unnamed namespace for local definitions
namespace {

// LOCAL CONSTANTS AND MACROS
const TText KSeparator = '\t';

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
    _LIT(KPanicText, "CPbkContactViewListCacheModel");
    User::Panic(KPanicText, aReason);
    }
#endif

}  // namespace


// ================= MEMBER FUNCTIONS =======================

inline CPbkContactViewListCacheModel::CPbkContactViewListCacheModel
    (PbkContactViewListModelFactory::TParams& aParams) : 
    CPbkContactViewListModelCommon(aParams)
    {
    }

inline void CPbkContactViewListCacheModel::ConstructL
    (PbkContactViewListModelFactory::TParams& aParams)
    {
    CPbkContactViewListModelCommon::ConstructL();
    iEntryCache = CPbkEntryCache::NewL(*aParams.iEngine, aParams.iCacheSize);
    }

CPbkContactViewListCacheModel* CPbkContactViewListCacheModel::NewL
    (PbkContactViewListModelFactory::TParams& aParams)
    {
    __ASSERT_DEBUG(aParams.iEngine && aParams.iView && aParams.iIconArray, 
        Panic(EPanicPreCond_NewL));
        
    CPbkContactViewListCacheModel* self =
            new(ELeave) CPbkContactViewListCacheModel(aParams);
    CleanupStack::PushL(self);
    self->ConstructL(aParams);
    CleanupStack::Pop(self);
    return self;
    }

CPbkContactViewListCacheModel::~CPbkContactViewListCacheModel()
    {
    delete iEntryCache;
    }

const TPbkContactEntry* CPbkContactViewListCacheModel::GetEntryL
        (TContactItemId aContactId) const
    {
    return &iEntryCache->GetEntryL(aContactId);
    }

void CPbkContactViewListCacheModel::PurgeEntry(TContactItemId aContactId)
    {
    iEntryCache->Unload(aContactId);    
    }

void CPbkContactViewListCacheModel::SetEntryLoader
        (MPbkContactEntryLoader& aContactEntryLoader)
    {
    iEntryCache->SetEntryLoader(aContactEntryLoader);
    }

void CPbkContactViewListCacheModel::FlushCache()
    {
    iEntryCache->Flush();
    }

void CPbkContactViewListCacheModel::FormatBufferForContactL
        (const CViewContact& aViewContact) const
    {
    const TPbkContactEntry& entry = iEntryCache->GetEntryL(aViewContact.Id());
    iBuffer.Zero();
    AppendIconIndex(entry.IconId());
    iBuffer.Append(KSeparator);
    AppendName(entry.Name());
    }

//  End of File
