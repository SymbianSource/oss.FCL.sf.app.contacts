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
* Description:  A class that reads the icon information of the extension
*
*/



// INCLUDE FILES
#include "CPbk2ThinUIExtensionLoader.h"

#include "CPbk2UIExtensionThinPlugin.h"
#include <Pbk2ExtensionUID.h>
#include <coemain.h>
#include <barsread.h>
#include <Pbk2Profile.h>

// ============================ LOCAL FUNCTIONS ================================

namespace
    {
    void CleanupResetAndDestroy(TAny* aObj)
        {
        if (aObj)
            {
            static_cast<RImplInfoPtrArray*>(aObj)->ResetAndDestroy();
            }
        }
    }

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CPbk2ThinUIExtensionLoader::CPbk2ThinUIExtensionLoader
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CPbk2ThinUIExtensionLoader::CPbk2ThinUIExtensionLoader()
    {
    }

// -----------------------------------------------------------------------------
// CPbk2ThinUIExtensionLoader::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CPbk2ThinUIExtensionLoader::ConstructL()
    {
    RImplInfoPtrArray implementations;
    TEComResolverParams resolverParams;

    resolverParams.SetWildcardMatch(ETrue);
    _LIT8(KPbk2ExtensionData, KPbk2ThinUiExtensionDefaultDataString);
    resolverParams.SetDataType(KPbk2ExtensionData);

    PBK2_PROFILE_START(Pbk2Profile::EThinUiExtensionLoaderListImplementations);
    REComSession::ListImplementationsL(
        TUid::Uid(KPbk2ThinUiExtensionInterfaceUID),
        resolverParams, implementations);
    PBK2_PROFILE_END(Pbk2Profile::EThinUiExtensionLoaderListImplementations);

    CleanupStack::PushL(TCleanupItem(CleanupResetAndDestroy, &implementations));

    // load all the thin extensions
    PBK2_PROFILE_START(Pbk2Profile::EThinUiExtensionLoaderLoopImplementations);
    const TInt count = implementations.Count();
    for (TInt i = count-1; i >= 0; --i)
        {
        CImplementationInformation* implInfo = implementations[i];
        CleanupStack::PushL(implInfo);
        implementations.Remove(i);
        // associate URIs with plugin implementations
        LoadImplementationsL(*implInfo);
        CleanupStack::PopAndDestroy(implInfo);
        }
    PBK2_PROFILE_END(Pbk2Profile::EThinUiExtensionLoaderLoopImplementations);

    CleanupStack::PopAndDestroy(); // implementations
    }

// -----------------------------------------------------------------------------
// CPbk2ThinUIExtensionLoader::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CPbk2ThinUIExtensionLoader* CPbk2ThinUIExtensionLoader::NewL()
    {
    CPbk2ThinUIExtensionLoader* self =
        new( ELeave ) CPbk2ThinUIExtensionLoader();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// Destructor
CPbk2ThinUIExtensionLoader::~CPbk2ThinUIExtensionLoader()
    {
    iPlugins.ResetAndDestroy();
    }

// -----------------------------------------------------------------------------
// CPbk2ThinUIExtensionLoader::LoadImplementations
// -----------------------------------------------------------------------------
//
void CPbk2ThinUIExtensionLoader::LoadImplementationsL(
        CImplementationInformation& aImplInfo)
    {
    // instantiate the thin plugin objects
    TUid uid(aImplInfo.ImplementationUid());
    if (!FindImplementation(uid))
        {
        CPbk2UIExtensionThinPlugin* plugin =
            CPbk2UIExtensionThinPlugin::NewL(uid);
        CleanupStack::PushL(plugin);
        iPlugins.AppendL(plugin);
        CleanupStack::Pop(plugin);
        }
    }

// -----------------------------------------------------------------------------
// CPbk2ThinUIExtensionLoader::FindImplementation
// -----------------------------------------------------------------------------
//
CPbk2UIExtensionThinPlugin*
    CPbk2ThinUIExtensionLoader::FindImplementation(TUid& aUid)
    {
    const TInt count = iPlugins.Count();
    for (TInt i = 0; i < count; ++i)
        {
        if (iPlugins[i]->ImplementationUid() == aUid)
            {
            return iPlugins[i];
            }
        }
    return NULL;
    }
//  End of File
