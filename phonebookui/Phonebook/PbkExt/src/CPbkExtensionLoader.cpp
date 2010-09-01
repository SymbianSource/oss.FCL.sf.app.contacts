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
*       Implementation for extension dll loader.
*
*/


// INCLUDE FILES
#include "CPbkExtensionLoader.h"
#include <ExtensionUID.h>
#include <MPbkExtensionFactory.h>
#include <CPbkExtensionFactory.h>

#include <PbkDebug.h>
#include "PbkProfiling.h"

// ================= MEMBER FUNCTIONS =======================

inline CPbkExtensionLoader::CPbkExtensionLoader()
    {
    }

CPbkExtensionLoader* CPbkExtensionLoader::NewL(TUid aUid)
    {
    CPbkExtensionLoader* self = new (ELeave) CPbkExtensionLoader;
    CleanupStack::PushL(self);
    self->LoadExtensionDllL(aUid);
    CleanupStack::Pop(self);
    return self;
    }

CPbkExtensionLoader::~CPbkExtensionLoader()
    {
    PBK_DEBUG_PRINT(PBK_DEBUG_STRING("Unloaded extension %x"), iFactory);

    delete iFactory;
    }

MPbkExtensionFactory* CPbkExtensionLoader::ExtensionFactory()
    {    
    return iFactory;
    }

void CPbkExtensionLoader::LoadExtensionDllL(TUid aUid)
    {
    PBK_DEBUG_PRINT(PBK_DEBUG_STRING("Start loading and initializing ECom DLL impl_uid=%x"), aUid);
    __PBK_PROFILE_START(PbkProfiling::EEComUiExtensionLoadAndInit);

    iFactory = CPbkExtensionFactory::NewL(aUid);

    __PBK_PROFILE_END(PbkProfiling::EEComUiExtensionLoadAndInit);
    __PBK_PROFILE_DISPLAY(PbkProfiling::EEComUiExtensionLoadAndInit);
    PBK_DEBUG_PRINT(PBK_DEBUG_STRING("Done loading and initializing ECom DLL impl_uid=%x"), aUid);
    }

// End of File
