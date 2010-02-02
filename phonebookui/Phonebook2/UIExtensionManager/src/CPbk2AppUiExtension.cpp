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
* Description:  A application UI extension implementation
*
*/



// INCLUDE FILES
#include "CPbk2AppUiExtension.h"

#include "CPbk2UIExtensionLoader.h"
#include "CPbk2ThinUIExtensionLoader.h"
#include "ForEachUtil.h"
#include "CPbk2UIExtensionInformation.h"
#include "CPbk2UIExtensionPlugin.h"
#include "CPbk2UIExtensionThinPlugin.h"


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CPbk2AppUiExtension::CPbk2AppUiExtension
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CPbk2AppUiExtension::CPbk2AppUiExtension(
        CVPbkContactManager& aContactManager,
        CPbk2UIExtensionLoader& aExtensionLoader,
        CPbk2ThinUIExtensionLoader& aThinExtensionLoader) : 
    iExtensionLoader(aExtensionLoader),
    iThinExtensionLoader(aThinExtensionLoader),
    iContactManager(aContactManager)  
    {
    }

// -----------------------------------------------------------------------------
// CPbk2AppUiExtension::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CPbk2AppUiExtension::ConstructL()
    {
    // Create extensions from UI extension plugins
    TArray<CPbk2UIExtensionPlugin*> plugins = iExtensionLoader.Plugins();
    const TInt count = plugins.Count();
    for (TInt i = 0; i < count; ++i)
        {
        MPbk2AppUiExtension* ext = 
            plugins[i]->CreatePbk2AppUiExtensionL(iContactManager);
        if (ext)
            {
            CleanupDeletePushL(ext);
            iExtensions.AppendL(ext);
            CleanupStack::Pop(); // ext
            }
        }
    }

// -----------------------------------------------------------------------------
// CPbk2AppUiExtension::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CPbk2AppUiExtension* CPbk2AppUiExtension::NewL(
        CVPbkContactManager& aContactManager,
        CPbk2UIExtensionLoader& aExtensionLoader,
        CPbk2ThinUIExtensionLoader& aThinExtensionLoader)
    {
    CPbk2AppUiExtension* self = new( ELeave ) CPbk2AppUiExtension(
        aContactManager, aExtensionLoader, aThinExtensionLoader);
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// CPbk2AppUiExtension::~CPbk2AppUiExtension
// -----------------------------------------------------------------------------
//
CPbk2AppUiExtension::~CPbk2AppUiExtension()
    {
    iExtensions.ResetAndDestroy();
    iStartupObservers.Close();
    }

// -----------------------------------------------------------------------------
// CPbk2AppUiExtension::ExtensionStartupL
// -----------------------------------------------------------------------------
//
void CPbk2AppUiExtension::ExtensionStartupL(
        MPbk2StartupMonitor& aStartupMonitor)
    {
    iStartupMonitor = &aStartupMonitor;
    // Reset state.
    iContactUiReady = EFalse;
    // Start listening to monitor events. Due to "Always On" feature
    // Deregister first.
    iStartupMonitor->DeregisterEvents( *this );
    iStartupMonitor->RegisterEventsL( *this );
    // Reset counters.
    iNumOfCompletedExtensions = 0;
    iNumOfFailedExtensions = 0;
    
    // Call all loaded UI extensions
    ForEachL(
        iExtensions,
        VoidMemberFunction
        (&MPbk2AppUiExtension::ExtensionStartupL),
        *this );
    
    // Call all thin UI extensions
    const TInt count = iThinExtensionLoader.PluginCount();
    for (TInt i = 0; i < count; ++i)
        {
        iThinExtensionLoader.PluginAt(i).ExtensionStartupL( *this );
        }
    }

// -----------------------------------------------------------------------------
// CPbk2AppUiExtension::ApplyDynamicPluginTabGroupContainerChangesL
// -----------------------------------------------------------------------------
//
void CPbk2AppUiExtension::ApplyDynamicPluginTabGroupContainerChangesL(
        const CPbk2ViewNode& aNode, 
        CAknTabGroup& aAknTabGroup,
        CPbk2IconInfoContainer& aTabIcons,
        TInt aViewCount )
    {
    // Call all loaded UI extensions
    ForEachL(
            iExtensions,
            VoidMemberFunction
            (&MPbk2AppUiExtension::ApplyDynamicPluginTabGroupContainerChangesL),
            aNode,
            aAknTabGroup,
            aTabIcons,
            aViewCount );
    }

// -----------------------------------------------------------------------------
// CPbk2AppUiExtension::DoRelease
// -----------------------------------------------------------------------------
//
void CPbk2AppUiExtension::DoRelease()
    {
    delete this;
    }

// -----------------------------------------------------------------------------
// CPbk2AppUiExtension::HandleStartupComplete
// -----------------------------------------------------------------------------
//
void CPbk2AppUiExtension::HandleStartupComplete()
    {
    // Update "success" counter
    ++iNumOfCompletedExtensions;
    
    if ( iStartupMonitor && AllExtensionsCompleted() )
        {
        // All extensions have completed.
        iStartupMonitor->HandleStartupComplete();
        StopObservingMonitor();
        }
    }

// -----------------------------------------------------------------------------
// CPbk2AppUiExtension::HandleStartupFailed
// -----------------------------------------------------------------------------
//
void CPbk2AppUiExtension::HandleStartupFailed(TInt aError)
    {
    // Update "failed" counter
    ++iNumOfFailedExtensions;
    
    if ( iStartupMonitor && AllExtensionsCompleted() )
        {
        if (iNumOfCompletedExtensions > 0)
            {
            // If at least 1 extension completed succesfully
            iStartupMonitor->HandleStartupComplete();
            }
        else
            {
            iStartupMonitor->HandleStartupFailed( aError );
            }
        StopObservingMonitor();
        }
    }

// -----------------------------------------------------------------------------
// CPbk2AppUiExtension::RegisterEventsL
// -----------------------------------------------------------------------------
//    
void CPbk2AppUiExtension::RegisterEventsL(MPbk2StartupObserver& aObserver)
    {   
    if ( iStartupObservers.Find( &aObserver) == KErrNotFound )
        {
        iStartupObservers.AppendL( &aObserver );    
        }
    }

// -----------------------------------------------------------------------------
// CPbk2AppUiExtension::DeregisterEvents
// -----------------------------------------------------------------------------
//    
void CPbk2AppUiExtension::DeregisterEvents(MPbk2StartupObserver& aObserver)
    {
    TInt pos = iStartupObservers.Find( &aObserver );
    if ( pos != KErrNotFound )
        {
        iStartupObservers.Remove( pos );
        }
    }

// -----------------------------------------------------------------------------
// CPbk2AppUiExtension::NotifyViewActivationL
// -----------------------------------------------------------------------------
//    
void CPbk2AppUiExtension::NotifyViewActivationL(
        TUid aViewId)
    {
    if ( iStartupMonitor )
        {
        iStartupMonitor->NotifyViewActivationL( aViewId );
        }
    }

// -----------------------------------------------------------------------------
// CPbk2AppUiExtension::NotifyViewActivationL
// -----------------------------------------------------------------------------
//        
void CPbk2AppUiExtension::NotifyViewActivationL(
        TUid aViewId, MVPbkContactViewBase& aContactView )
    {
    if ( iStartupMonitor )
        {
        iStartupMonitor->NotifyViewActivationL( aViewId, aContactView );
        }
    }
    
// -----------------------------------------------------------------------------
// CPbk2AppUiExtension::ContactUiReadyL
// -----------------------------------------------------------------------------
//        
void CPbk2AppUiExtension::ContactUiReadyL( 
        MPbk2StartupMonitor& /*aStartupMonitor*/)
    {
    // Core Pbk2 UI is ready -> update state and forward message.
    iContactUiReady = ETrue;
    // Call all loaded UI extensions
    TInt count = iStartupObservers.Count();
    for ( TInt i = count - 1; i >= 0; --i)
        {
        iStartupObservers[i]->ContactUiReadyL( *this );
        }
    
    StopObservingMonitor();
    }

// -----------------------------------------------------------------------------
// CPbk2AppUiExtension::StopObservingMonitor
// -----------------------------------------------------------------------------
//        
void CPbk2AppUiExtension::StopObservingMonitor()
    {
    if ( iStartupMonitor && iContactUiReady && AllExtensionsCompleted() )
        {
        iStartupMonitor->DeregisterEvents( *this );
        iStartupMonitor = NULL;
        }
    }
    
// -----------------------------------------------------------------------------
// CPbk2AppUiExtension::AllExtensionsCompleted
// -----------------------------------------------------------------------------
//        
TBool CPbk2AppUiExtension::AllExtensionsCompleted() const
    {
    return ( ( iNumOfCompletedExtensions + iNumOfFailedExtensions ) ==
            NumOfExtensions() );
    }
    
// -----------------------------------------------------------------------------
// CPbk2AppUiExtension::NumOfExtensions
// -----------------------------------------------------------------------------
//        
TInt CPbk2AppUiExtension::NumOfExtensions() const
    {
    return iExtensions.Count() + iThinExtensionLoader.PluginCount();
    }
//  End of File
