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
*
*/


// INCLUDE FILES
#include "CPbkStartupView.h"
#include "CPbkAppUi.h"
#include "CPbkDocument.h"
#include <CPbkControlContainer.h>
#include <Phonebook.hrh>

// Debugging headers
#include <pbkdebug.h>
#include "PbkProfiling.h"


// CONSTANTS
const TInt KViewActivationTimeout = 5000000;

inline CPbkStartupView::CPbkStartupView(MPbkAppUiExtension& aAppUiExtension) :
    iAppUiExtension(aAppUiExtension)
    {
    }

inline void CPbkStartupView::ConstructL()
    {
    __PBK_PROFILE_START(PbkProfiling::EViewBaseConstruct);
    BaseConstructL();
    __PBK_PROFILE_END(PbkProfiling::EViewBaseConstruct);
    }

CPbkStartupView* CPbkStartupView::NewLC(MPbkAppUiExtension& aAppUiExtension)
    {
	CPbkStartupView* self = new(ELeave) CPbkStartupView(aAppUiExtension);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
    }

CPbkStartupView* CPbkStartupView::NewL(MPbkAppUiExtension& aAppUiExtension)
    {
	CPbkStartupView* self = CPbkStartupView::NewLC(aAppUiExtension);
	CleanupStack::Pop(self);
	return self;
    }

CPbkStartupView::~CPbkStartupView()
    {
    if (iContainer)
        {
        AppUi()->RemoveFromViewStack(*this, iContainer);
        delete iContainer;
        }
    if (iTimer)
        {
        iTimer->Cancel();
        delete iTimer;
        }
    }

void CPbkStartupView::DoActivateL(const TVwsViewId& /*aPrevViewId*/, 
                                  TUid /*aCustomMessageId*/, 
                                  const TDesC8& /*aCustomMessage*/)
    {
    __PBK_PROFILE_END(PbkProfiling::EStartupViewActivation);

    __PBK_PROFILE_START(PbkProfiling::EStartupViewDoActivateL);

    if (!iContainer)
        {
        iContainer = CContainer::NewL(this, *this);
        CCoeControl* control = new(ELeave) CCoeControl;
        CleanupStack::PushL(control);
        control->SetContainerWindowL(*iContainer);
        CleanupStack::Pop(control);
        iContainer->SetControl(control, ClientRect());
        iContainer->ActivateL();
        AppUi()->AddToViewStackL(*this, iContainer);
        }
    iContainer->Control()->DrawNow();

    __PBK_PROFILE_END(PbkProfiling::EStartupViewDoActivateL);

    if (iAppUiExtension.StartupStatus() == MPbkAppUiExtension::EStartupNotStarted)
        {
        // we have to make this check to make sure that extension startup is not started multiple times
        // for example in case of screen saver popping up when startup is in progress or when user
        // visits some other application during startup

        // Ending this profile is in CPbkStartupView::HandleStartupComplete
        __PBK_PROFILE_START(PbkProfiling::EExtensionStartup);
        CPbkContactEngine* engine = Engine();    
        iAppUiExtension.ExtensionStartupL(*this, *engine);
        }
    }

void CPbkStartupView::DoDeactivate()
    {
    if (iTimer)
        {
        // at this point the names list view is already activated
        // so we know that activation was successful and we can
        // cancel the timer
        iTimer->Cancel();
        delete iTimer;
        iTimer = NULL;
        }

    if (iContainer)
        {
        AppUi()->RemoveFromViewStack(*this, iContainer);
        delete iContainer;
        iContainer = NULL;
        }
    }

TUid CPbkStartupView::Id() const
    {
    return TUid::Uid(EPbkStartupViewId);
    }

void CPbkStartupView::HandleStartupComplete()
    {
    __PBK_PROFILE_END(PbkProfiling::EExtensionStartup);

    __PBK_PROFILE_START(PbkProfiling::EHandleStartupComplete);
       
    TRAPD(error, 
        {
        PbkAppUi()->ActivatePhonebookViewL(TUid::Uid(EPbkNamesListViewId));
        if (!iTimer)
            {
            iTimer = CPeriodic::NewL(CPeriodic::EPriorityIdle);
            }
        });

    if (error != KErrNone)
        {
        // view activation failed - exit gracefully
        iCoeEnv->HandleError(error);
        PbkAppUi()->Exit();
        }

    if (iContainer)
        {
        // If the iContainer exists, it means that this view is still active, so
        // we have to start the timer to see if names list view was activated

        // Start timer for view activation. If view activation times
        // out, the application is closed.
        iTimer->Start(KViewActivationTimeout, 
                      1, // this is irrelevant because the event is not repeated
                      TCallBack(CPbkStartupView::ViewActivationTimeout, this));
        }

    __PBK_PROFILE_END(PbkProfiling::EHandleStartupComplete);

    // Ending this profile is in CPbkNamesListView::DoActivateL
    __PBK_PROFILE_START(PbkProfiling::ENamesListViewActivation);
    }

void CPbkStartupView::HandleStartupFailedL(TInt aError)
    {
    // initialisation failed - exit gracefully
    iCoeEnv->HandleError(aError);
    PbkAppUi()->Exit();
    }

TInt CPbkStartupView::ViewActivationTimeout(TAny* aThis)
    {
    return static_cast<CPbkStartupView*>(aThis)->ViewActivationTimeout();
    }

TInt CPbkStartupView::ViewActivationTimeout()
    {
    PbkAppUi()->Exit();
    return 0;
    }

// End of File  
