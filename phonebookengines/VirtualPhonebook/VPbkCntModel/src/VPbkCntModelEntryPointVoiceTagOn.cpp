/*
* Copyright (c) 2004-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Virtual Phonebook cnt model entry point
*
*/


// INCLUDES
#include <e32std.h>
#include <ecom/implementationproxy.h>
#include "ccontactstoredomain.h"
#include "cspeeddialattributemanager.h"
#include "cdefaultattributemanager.h"
#include "cvoicetagattributemanager.h"
#include "ccontactidconverter.h"

#include <vpbkpublicuid.h>
#include <vpbkprivateuid.h>

typedef VPbkCntModel::CContactStoreDomain* (*NewLPtr)(TAny*);
typedef VPbkCntModel::CSpeedDialAttributeManager* (*AttributeNewLPtr)(TAny*);
typedef VPbkCntModel::CDefaultAttributeManager* (*DefaultAttributeNewLPtr)(TAny*);
typedef VPbkCntModel::CVoiceTagAttributeManager* (*VoiceTagAttributeNewLPtr)(TAny*);
typedef VPbkCntModel::CContactIdConverter* (*ConverterNewLPtr)(TAny*);

// Define the interface UIDs
const TImplementationProxy ImplementationTable[] = 
    {
    IMPLEMENTATION_PROXY_ENTRY(KVPbkCntModelStoreDomainImplementationUID, 
        static_cast<NewLPtr>(VPbkCntModel::CContactStoreDomain::NewL)),
    IMPLEMENTATION_PROXY_ENTRY(KVPbkSpeedDialAttributeImplementationUID, 
        static_cast<AttributeNewLPtr>(VPbkCntModel::CSpeedDialAttributeManager::NewL)),
    IMPLEMENTATION_PROXY_ENTRY(KVPbkDefaultAttributeImplementationUID, 
        static_cast<DefaultAttributeNewLPtr>(VPbkCntModel::CDefaultAttributeManager::NewL)),
    IMPLEMENTATION_PROXY_ENTRY(KVPbkVoiceTagAttributeImplementationUID, 
        static_cast<VoiceTagAttributeNewLPtr>(VPbkCntModel::CVoiceTagAttributeManager::NewL)),
    IMPLEMENTATION_PROXY_ENTRY(KVPbkContactIdConverterImplementationUid, 
        static_cast<ConverterNewLPtr>(VPbkCntModel::CContactIdConverter::NewL)),
    };

EXPORT_C const TImplementationProxy* ImplementationGroupProxy(TInt& aTableCount)
    {
    aTableCount = sizeof(ImplementationTable) / sizeof(TImplementationProxy);
    return ImplementationTable;
    }

// end if file
