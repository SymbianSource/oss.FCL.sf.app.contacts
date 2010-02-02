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
* Description:  Virtual Phonebook Local variation manager.
*
*/


// INCLUDE FILES
#include "CVPbkLocalVariationManager.h"
#include "PhonebookPrivateCRKeys.h"
#include <centralrepository.h>

// ================= MEMBER FUNCTIONS =======================

inline void CVPbkLocalVariationManager::ConstructL()
	{
    TUid uid;
    uid.iUid = KCRUidPhonebook;
    // Construct central repository client for local variation needs
    iRepository = CRepository::NewL(uid);

    // Get flags
    iLocalVariationFlags = DoGetLocalVariationFlags();
	}

inline CVPbkLocalVariationManager::CVPbkLocalVariationManager()
	{
    }

EXPORT_C CVPbkLocalVariationManager* CVPbkLocalVariationManager::NewL()
	{
	CVPbkLocalVariationManager* self = new(ELeave) CVPbkLocalVariationManager();
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

CVPbkLocalVariationManager::~CVPbkLocalVariationManager()
	{
    delete iRepository;
    }

EXPORT_C TBool CVPbkLocalVariationManager::LocallyVariatedFeatureEnabled
        (TVPbkLocalVariantFlags aFeature)
    {
    TBool ret = EFalse;

    // Check is the feature enabled or not
    if (iLocalVariationFlags & aFeature)
        {
        ret = ETrue;
        }
    
    return ret;
    }

TInt CVPbkLocalVariationManager::DoGetLocalVariationFlags()
	{
	TInt flags = 0;
    TInt result = iRepository->Get(KPhonebookLocalVariationFlags, flags);
    if (result != KErrNone)
        {
        // If there were problems reading the flags, assume everything is off
        flags = 0;
        }
	return flags;
	}


//  End of File  
