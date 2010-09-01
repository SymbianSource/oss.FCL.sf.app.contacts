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
*       Composite AppUi extension.
*
*/


// INCLUDE FILES
#include "CPbkMultiAppUiExtension.h"
#include "ForEachUtil.h"

const TInt KExtensionGranularity = 1;

// ==================== MEMBER FUNCTIONS ====================
CPbkMultiAppUiExtension::CPbkMultiAppUiExtension() :
    iAppUis(KExtensionGranularity),
    iStartupStatus(EStartupNotStarted)
    {
    }

CPbkMultiAppUiExtension::~CPbkMultiAppUiExtension()
    {
    // This really cannot leave since this is a template
    // method and actually Release method is called
    ForEachL(iAppUis,
             &Release);
    iAppUis.Reset();
    }


CPbkMultiAppUiExtension* CPbkMultiAppUiExtension::NewL()
    {
    CPbkMultiAppUiExtension* self = new (ELeave) CPbkMultiAppUiExtension;
    return self;
    }

void CPbkMultiAppUiExtension::AppendL(MPbkAppUiExtension* aAppUi)
    {
    iAppUis.AppendL(aAppUi);
    }

void CPbkMultiAppUiExtension::ApplyExtensionViewGraphChangesL
        (CDigViewGraph& aViewGraph)
    {
    ForEachL(iAppUis, 
             VoidMemberFunction(&MPbkAppUiExtension::ApplyExtensionViewGraphChangesL),
             aViewGraph);
    }

void CPbkMultiAppUiExtension::ExtensionStartupL
        (MPbkExtensionStartupObserver& aObserver,
        CPbkContactEngine& aEngine)
    {
    iStartupStatus = EStartupInProgress;
    iStartupObserver = &aObserver;
    
    ForEachL(iAppUis, 
             VoidMemberFunction(&MPbkAppUiExtension::ExtensionStartupL),
             *this,
             aEngine);
    // If there were no extensions we signal observer immeaditely
    if (iAppUis.Count() == 0)
        {
        iStartupStatus = EStartupCompleted;
        aObserver.HandleStartupComplete();
        }
    }

void CPbkMultiAppUiExtension::DoRelease()
    {
    delete this;
    }

void CPbkMultiAppUiExtension::HandleStartupComplete()
    {
    ++iStartedExtensions;
    if (iFailedExtensions + iStartedExtensions == iAppUis.Count())
        {
        iStartupStatus = EStartupCompleted;
        iStartupObserver->HandleStartupComplete();
        }
    }

void CPbkMultiAppUiExtension::HandleStartupFailedL(TInt aError)
    {
    ++iFailedExtensions;
    if (iFailedExtensions + iStartedExtensions == iAppUis.Count())
        {
        if (iStartedExtensions > 0)
            {
            iStartupStatus = EStartupCompleted;
            iStartupObserver->HandleStartupComplete();
            }
        else
            {
            iStartupStatus = EStartupFailed;
            iStartupObserver->HandleStartupFailedL(aError);
            }
        }
    }

MPbkAppUiExtension::TStartupStatus CPbkMultiAppUiExtension::StartupStatus() const
    {
    return iStartupStatus;
    }

// End of File
