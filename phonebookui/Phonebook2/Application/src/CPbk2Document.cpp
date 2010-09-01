/*
* Copyright (c) 2005-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Phonebook 2 document.
*
*/


// INCLUDE FILES
#include "CPbk2Document.h"

// Phonebook 2
#include "CPbk2Application.h"
#include "CPbk2AppUi.h"
#include "CPbk2UIExtensionManager.h"
#include "CPbk2UIApplicationServices.h"

// Debugging headers
#include <Pbk2Debug.h>
#include <Pbk2Profile.h>

// --------------------------------------------------------------------------
// CPbk2Document::CPbk2Document
// --------------------------------------------------------------------------
//
inline CPbk2Document::CPbk2Document( CEikApplication& aApp ) :
        CPbk2DocumentBase( aApp )
    {
    }

// --------------------------------------------------------------------------
// CPbk2Document::~CPbk2Document
// --------------------------------------------------------------------------
//
CPbk2Document::~CPbk2Document()
    {
    Release( iExtensionManager );
    delete iAppServices;
    }

// --------------------------------------------------------------------------
// CPbk2Document::NewL
// --------------------------------------------------------------------------
//
CPbk2Document* CPbk2Document::NewL( CEikApplication& aApp )
    {
    CPbk2Document* self = new ( ELeave ) CPbk2Document( aApp );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// --------------------------------------------------------------------------
// CPbk2Document::ConstructL
// --------------------------------------------------------------------------
//
inline void CPbk2Document::ConstructL()
    {
    PBK2_DEBUG_PRINT( PBK2_DEBUG_STRING
        ( "CPbk2Document::ConstructL(0x%x)" ), this );
    }

// --------------------------------------------------------------------------
// CPbk2Document::Pbk2Application
// --------------------------------------------------------------------------
//
CPbk2Application* CPbk2Document::Pbk2Application() const
    {
    // Explicit cast: application must always be of type CPbk2Application
    return static_cast<CPbk2Application*>(Application());
    }

// --------------------------------------------------------------------------
// CPbk2Document::CreateGlobalsL
// --------------------------------------------------------------------------
//
void CPbk2Document::CreateGlobalsL()
    {
    PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING
        ("CPbk2Document::CreateGlobalsL(0x%x)"), this);

    PBK2_PROFILE_START(Pbk2Profile::EDocumentCreateExtensionManager);
    iExtensionManager = CPbk2UIExtensionManager::InstanceL();
    PBK2_PROFILE_END(Pbk2Profile::EDocumentCreateExtensionManager);

    PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING
        ("CPbk2Document::CreateGlobalsL(0x%x) end"), this);
    }

// --------------------------------------------------------------------------
// CPbk2Document::ApplicationServicesL
// --------------------------------------------------------------------------
//
MPbk2ApplicationServices* CPbk2Document::ApplicationServicesL()
    {
    if (!iAppServices)
        {
        iAppServices = CPbk2UIApplicationServices::NewL();
        }

    return iAppServices;
    }

// --------------------------------------------------------------------------
// CPbk2Document::ExtensionManager
// --------------------------------------------------------------------------
//
CPbk2UIExtensionManager& CPbk2Document::ExtensionManager() const
    {
    return *iExtensionManager;
    }

// --------------------------------------------------------------------------
// CPbk2Document::CreateAppUiL
// --------------------------------------------------------------------------
//
CEikAppUi* CPbk2Document::CreateAppUiL()
    {
    PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING
        ( "CPbk2Document::CreateAppUiL(0x%x)" ), this );

    return ( new ( ELeave ) CPbk2AppUi );
    }

//  End of File
