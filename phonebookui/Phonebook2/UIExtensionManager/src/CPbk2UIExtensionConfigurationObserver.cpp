/*
* Copyright (c) 2002-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  A store configuration observer
*
*/



// INCLUDE FILES
#include "CPbk2UIExtensionConfigurationObserver.h"

#include "CPbk2UIExtensionLoader.h"
#include "CPbk2UIExtensionInformation.h"
#include "CPbk2UIExtensionPlugin.h"

#include <CPbk2StorePropertyArray.h>
#include <CPbk2StoreConfiguration.h>
#include <barsread.h>
#include <coemain.h>

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CPbk2UIExtensionConfigurationObserver::CPbk2UIExtensionConfigurationObserver
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CPbk2UIExtensionConfigurationObserver::CPbk2UIExtensionConfigurationObserver(
        CPbk2StorePropertyArray& aPropertyArray, 
        CPbk2UIExtensionLoader& aExtensionLoader,
        CPbk2StoreConfiguration& aStoreConfiguration) :   
    iPropertyArray(aPropertyArray),
    iExtensionLoader(aExtensionLoader),
    iStoreConfiguration(aStoreConfiguration)
    {
    }

// -----------------------------------------------------------------------------
// CPbk2UIExtensionConfigurationObserver::ConstructL
// -----------------------------------------------------------------------------
//
void CPbk2UIExtensionConfigurationObserver::ConstructL() 
    {
    AddPropertiesFromExtensionsL();
    iStoreConfiguration.AddObserverL(*this);
    }

// -----------------------------------------------------------------------------
// CPbk2UIExtensionConfigurationObserver::NewL
// -----------------------------------------------------------------------------
//    
CPbk2UIExtensionConfigurationObserver* 
        CPbk2UIExtensionConfigurationObserver::NewL(
            CPbk2StorePropertyArray& aPropertyArray,
            CPbk2UIExtensionLoader& aExtensionLoader,
            CPbk2StoreConfiguration& aStoreConfiguration)
    {
    CPbk2UIExtensionConfigurationObserver* self = 
        new( ELeave ) CPbk2UIExtensionConfigurationObserver(aPropertyArray, 
            aExtensionLoader, aStoreConfiguration);
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// CPbk2UIExtensionConfigurationObserver::~CPbk2UIExtensionConfigurationObserver
// -----------------------------------------------------------------------------
//
CPbk2UIExtensionConfigurationObserver::~CPbk2UIExtensionConfigurationObserver()
    {
    iStoreConfiguration.RemoveObserver(*this);
    }

// -----------------------------------------------------------------------------
// CPbk2UIExtensionConfigurationObserver::ConfigurationChanged
// -----------------------------------------------------------------------------
//
void CPbk2UIExtensionConfigurationObserver::ConfigurationChanged()
    {
	// Update array from extensions
	TRAPD(res, AddPropertiesFromExtensionsL());
	if (res != KErrNone)
	    {
	    CCoeEnv::Static()->HandleError(res);
	    }
    }

// -----------------------------------------------------------------------------
// CPbk2UIExtensionConfigurationObserver::ConfigurationChangedComplete
// -----------------------------------------------------------------------------
//
void CPbk2UIExtensionConfigurationObserver::ConfigurationChangedComplete()
	{
	// Do nothing
	}

// -----------------------------------------------------------------------------
// CPbk2UIExtensionConfigurationObserver::AddPropertiesFromExtensionsL
// -----------------------------------------------------------------------------
//
void CPbk2UIExtensionConfigurationObserver::AddPropertiesFromExtensionsL()
    {
    TArray<CPbk2UIExtensionInformation*> info = 
        iExtensionLoader.PluginInformation();
    const TInt count = info.Count();
    for (TInt i = 0; i < count; ++i)
        {
        if (info[i]->HasStorePropertyArray())
            {
            // Extension defines store properties in resource
            TResourceReader reader;
            info[i]->CreateStorePropertyArrayReaderLC(reader);
            iPropertyArray.AppendFromResourceL(reader);
            CleanupStack::PopAndDestroy();
            }
            
        TUid uid(info[i]->ImplementationUid());
        // Called only for loaded extensions
        if (iExtensionLoader.IsLoaded(uid))
            {
            iExtensionLoader.LoadedPlugin(uid)->UpdateStorePropertiesL(iPropertyArray);
            }
        }
    }
//  End of File  
