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
*       Scanner that finds extensions.
*
*/


// INCLUDE FILES
#include "CPbkExtensionScanner.h"
#include <ecom/ecom.h>
#include "CPbkExtGlobals.h"
#include <ExtensionUID.h>
#include "CPbkExtensionLoader.h"
#include "PbkUID.h"

#include <PbkDebug.h>        // Phonebook debugging support
#include "PbkProfiling.h"


// Unnamed namespace for local definitions
namespace {

void CleanupResetAndDestroy(TAny* aObj)
    {
    if (aObj)
        {
        static_cast<RImplInfoPtrArray*>(aObj)->ResetAndDestroy();
        }
    }

} // namespace

// ==================== MEMBER FUNCTIONS ====================                 
CPbkExtensionScanner::CPbkExtensionScanner(CPbkExtensionArray& aExtensions) :
    iExtensions(aExtensions)
    {
	}	

CPbkExtensionScanner::~CPbkExtensionScanner()
    {
    }


CPbkExtensionScanner*
CPbkExtensionScanner::NewLC(CPbkExtensionArray& aExtensions)
    {
    CPbkExtensionScanner* self = 
        new (ELeave) CPbkExtensionScanner(aExtensions);
    CleanupStack::PushL( self );
    return self;
    }

void CPbkExtensionScanner::ScanL()
    {
    __PBK_PROFILE_START(PbkProfiling::EEComUiExtensionScanPrepare);
    RImplInfoPtrArray implementations;
    TEComResolverParams resolverParams;

    resolverParams.SetWildcardMatch(ETrue);
    _LIT8(KPbkExtension, KPbkUiExtensionDefaultDataString);
    resolverParams.SetDataType(KPbkExtension);

    __PBK_PROFILE_END(PbkProfiling::EEComUiExtensionScanPrepare);
    PBK_DEBUG_PRINT(PBK_DEBUG_STRING("Start ECom extension scan"));
    __PBK_PROFILE_START(PbkProfiling::EEComUiExtensionScan);

    // Use ROM only resolver
    REComSession::ListImplementationsL
        (TUid::Uid(KPbkUiExtensionInterfaceDefinitionUID), 
         resolverParams,         
         implementations);

    __PBK_PROFILE_END(PbkProfiling::EEComUiExtensionScan);
    PBK_DEBUG_PRINT(PBK_DEBUG_STRING("Done Ecom extension scan"));

    __PBK_PROFILE_START(PbkProfiling::EEComUiExtensionLoadPrepare);
    CleanupStack::PushL(TCleanupItem(CleanupResetAndDestroy, 
                                     &implementations));

    const TInt count = implementations.Count();
    iExtensions.SetReserveL(iExtensions.Count() + count);
    __PBK_PROFILE_END(PbkProfiling::EEComUiExtensionLoadPrepare);
    for (TInt i = count-1; i >= 0; --i)
        {
        __PBK_PROFILE_START(PbkProfiling::EEComUiExtensionLoadPrepare);
        CImplementationInformation* implInfo = implementations[i];
        CleanupStack::PushL(implInfo);
        implementations.Remove(i);
        __PBK_PROFILE_END(PbkProfiling::EEComUiExtensionLoadPrepare);
        
        CPbkExtensionLoader* extension = NULL;
        TRAPD(err,
              extension = CPbkExtensionLoader::NewL(implInfo->ImplementationUid()));
        if (err == KErrNone)
            {
            CleanupStack::PushL(extension);
            iExtensions.AppendL(extension);
            CleanupStack::Pop(extension);
            }

        CleanupStack::PopAndDestroy(implInfo);
        }
    CleanupStack::PopAndDestroy(); // implementations
    }

// End of File
