/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:     Declares container control for application.
*
*/






// INCLUDE FILES
#include <spdctrl.rsg>

#include "speeddialprivate.h"
#include "SpdiaCallingVmbxContainer.h"
#include <CVPbkContactStoreUriArray.h>
#include <VPbkContactStoreUris.h>
#include <TVPbkContactStoreUriPtr.h>
#include <CPbk2StoreConfiguration.h>

// ========================= MEMBER FUNCTIONS ================================

// ---------------------------------------------------------------------------
// CSpdiaCallingVmbxContainer::CSpdiaCallingVmbxContainer
// Constructor
// ---------------------------------------------------------------------------
CSpdiaCallingVmbxContainer::CSpdiaCallingVmbxContainer():
							iContactManager(NULL),iControl(NULL)
															
    {
    }

// ---------------------------------------------------------------------------
// CSpdiaCallingVmbxContainer::NewL
// Symbian two-phased constructor
// ---------------------------------------------------------------------------
CSpdiaCallingVmbxContainer* CSpdiaCallingVmbxContainer::NewL()
    {
    CSpdiaCallingVmbxContainer* self = CSpdiaCallingVmbxContainer::NewLC();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// CSpdiaCallingVmbxContainer::NewLC
// Symbian two-phased constructor
// ---------------------------------------------------------------------------
CSpdiaCallingVmbxContainer* CSpdiaCallingVmbxContainer::NewLC()
    {
    CSpdiaCallingVmbxContainer* self =
        new ( ELeave ) CSpdiaCallingVmbxContainer;
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// ---------------------------------------------------------------------------
// CSpdiaCallingVmbxContainer::ConstructL
// Symbian two phased constructor
// ---------------------------------------------------------------------------
void CSpdiaCallingVmbxContainer::ConstructL()
    {
    CreateWindowL();

    iControl = NULL;
    
    CPbk2StoreConfiguration* configuration = CPbk2StoreConfiguration::NewL();
    CleanupStack::PushL(configuration);
    CVPbkContactStoreUriArray* uriArray = configuration->CurrentConfigurationL();
    CleanupStack::PushL(uriArray);
    iContactManager = CVPbkContactManager::NewL(*uriArray);
    CleanupStack::PopAndDestroy(2); // uriArray, configuration
    
    iControl = CSpeedDialPrivate::NewL(iContactManager);

    ActivateL();
    }

// ---------------------------------------------------------------------------
// CSpdiaCallingVmbxContainer::~CSpdiaCallingVmbxContainer
// Destructor
// ---------------------------------------------------------------------------
CSpdiaCallingVmbxContainer::~CSpdiaCallingVmbxContainer()
    {
    
    delete iControl;
    delete iContactManager;	
    iControl = NULL;
    
    }

// ---------------------------------------------------------------------------
// CSpdiaCallingVmbxContainer::SizeChanged
// Called by framework when the view size is changed
// ---------------------------------------------------------------------------
void CSpdiaCallingVmbxContainer::SizeChanged()
    {
    iControl->SetLayout( Rect() );
    }

// ---------------------------------------------------------------------------
// CSpdiaCallingVmbxContainer::CountComponentControls
//
// ---------------------------------------------------------------------------
TInt CSpdiaCallingVmbxContainer::CountComponentControls() const
    {
    return 1;
    }

// ---------------------------------------------------------------------------
// CSpdiaCallingVmbxContainer::ComponentControl
//
// ---------------------------------------------------------------------------
CCoeControl* CSpdiaCallingVmbxContainer::ComponentControl(
                                             TInt /*aIndex*/ ) const
    {
    return iControl;
    }

// End of File  
