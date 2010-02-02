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
*
*/


// INCLUDE FILES
#include "CPbkLocalVariationManager.h"
#include "PbkUID.h"

#include <PbkGlobalSettingFactory.h>
#include <MPbkGlobalSetting.h>


// ================= MEMBER FUNCTIONS =======================

CPbkLocalVariationManager* CPbkLocalVariationManager::NewL()
	{
	CPbkLocalVariationManager* self = new(ELeave) CPbkLocalVariationManager();
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

void CPbkLocalVariationManager::ConstructL()
	{
    // Construct shared data client for local variation needs
    iLocalVariationSetting = PbkGlobalSettingFactory::CreatePersistentSettingL();
    iLocalVariationSetting->ConnectL(MPbkGlobalSetting::ELocalVariationCategory);

    // Get flags
    iLocalVariationFlags = GetLocalVariationFlags();
	}

inline CPbkLocalVariationManager::CPbkLocalVariationManager()
	{
    }

CPbkLocalVariationManager::~CPbkLocalVariationManager()
	{
    if (iLocalVariationSetting)
        {
        iLocalVariationSetting->Close();
        delete iLocalVariationSetting;
        }
    }

TInt CPbkLocalVariationManager::GetLocalVariationFlags()
	{
	TInt flags = 0;
    TInt result = iLocalVariationSetting->Get
        (MPbkGlobalSetting::ELocalVariationFlags, flags);
    if (result != KErrNone)
        {
        // If there were problems reading the flags, assume everything is off
        flags = 0;
        }
	return flags;
	}

TBool CPbkLocalVariationManager::LocallyVariatedFeatureEnabled
        (TPbkLocalVariantFlags aFeature)
    {
    TBool ret = EFalse;

    // Check is the feature enabled or not
    if (iLocalVariationFlags & aFeature)
        {
        ret = ETrue;
        }
    
    return ret;
    }

//  End of File  
