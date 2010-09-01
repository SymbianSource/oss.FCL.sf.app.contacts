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
*       PbkExt.dll global variables. A singleton class.
*
*/


// INCLUDE FILES
#include "CPbkExtGlobals.h"
#include "CPbkMultiExtensionFactory.h"
#include "CPbkExtensionLoader.h"

// Debugging headers
#include <PbkDebug.h>
#include "PbkProfiling.h"

#include <ecom/ecom.h>

// Unnamed namespace for local definitions
namespace {

const TInt KExtensionGranularity = 1;

/**
 * Returns global instance of CPbkExtGlobals
 */
inline CPbkExtGlobals* Instance()
    {
    return static_cast<CPbkExtGlobals*>(Dll::Tls());
    }

} // namespace


// ==================== MEMBER FUNCTIONS ====================                 
CPbkExtGlobals::CPbkExtGlobals() :
    iExtensions(KExtensionGranularity)
    {
	}	

CPbkExtGlobals::~CPbkExtGlobals()
    {
    delete iMultiFactory;
    iExtensions.ResetAndDestroy();

    // Cleanup ECom session
    REComSession::FinalClose();
    }

CPbkExtGlobals* CPbkExtGlobals::NewLC()
    {
    CPbkExtGlobals* self = new (ELeave) CPbkExtGlobals;
    CleanupStack::PushL( self );

    return self;
    }

void CPbkExtGlobals::IncRef()
    {
    ++iRefCount;
    }

TInt CPbkExtGlobals::DecRef()
    {
    return --iRefCount;
    }

EXPORT_C CPbkExtGlobals* CPbkExtGlobals::InstanceL()
    {
    CPbkExtGlobals* instance = Instance();

    if (!instance)
        {
        instance = CPbkExtGlobals::NewLC();
        User::LeaveIfError(Dll::SetTls(instance));
        CleanupStack::Pop(instance);
        }

    instance->IncRef();

    return instance;
    }

EXPORT_C MPbkExtensionFactory& CPbkExtGlobals::FactoryL()
    {    
    if (!iMultiFactory)
		{
        __PBK_PROFILE_START(PbkProfiling::EPbkExtGlobalsScannerConstruct);
        // Make sure that there are no extensions when scanning
        iExtensions.ResetAndDestroy();
        // If scanning fails we leave here so that iMultiFactory
        // doesn't get created. So when calling this function
        // again scanning is tried again.
        CPbkExtensionScanner* scanner = 
            CPbkExtensionScanner::NewLC(iExtensions);
        __PBK_PROFILE_END(PbkProfiling::EPbkExtGlobalsScannerConstruct);
        __PBK_PROFILE_START(PbkProfiling::EExtensionScan);
        scanner->ScanL();
        CleanupStack::PopAndDestroy(scanner);
        __PBK_PROFILE_END(PbkProfiling::EExtensionScan);

        __PBK_PROFILE_START(PbkProfiling::EMultiFactoryConstruct);
		iMultiFactory = CPbkMultiExtensionFactory::NewL();

		const TInt count = iExtensions.Count();
		for (TInt i=0; i<count; ++i)
			{
			iMultiFactory->AppendL(iExtensions.At(i)->ExtensionFactory());
			}
        __PBK_PROFILE_END(PbkProfiling::EMultiFactoryConstruct);
		}

    return *iMultiFactory;
    }

void CPbkExtGlobals::DoRelease()
    {
    if (DecRef() == 0)
        {
        Dll::SetTls(NULL);
        delete this;
        }
    }

// End of File
