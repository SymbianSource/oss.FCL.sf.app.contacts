/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: CNewContactLauncherDocument implementation
*
*/

// INCLUDE FILES
#include "NewContactLauncherAppUi.h"
#include "NewContactLauncherDocument.h"
#include <eikapp.h>

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CNewContactLauncherDocument::NewL()
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CNewContactLauncherDocument* CNewContactLauncherDocument::NewL(
        CEikApplication& aApp)
    {
    CNewContactLauncherDocument* self = NewLC(aApp);
    CleanupStack::Pop(self);
    return self;
    }

// -----------------------------------------------------------------------------
// CNewContactLauncherDocument::NewLC()
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CNewContactLauncherDocument* CNewContactLauncherDocument::NewLC(
        CEikApplication& aApp)
    {
    CNewContactLauncherDocument* self =
            new (ELeave) CNewContactLauncherDocument(aApp);

    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
    }

// -----------------------------------------------------------------------------
// CNewContactLauncherDocument::ConstructL()
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CNewContactLauncherDocument::ConstructL()
    {
    // No implementation required
    }

// -----------------------------------------------------------------------------
// CNewContactLauncherDocument::CNewContactLauncherDocument()
// C++ default constructor can NOT contain any code, that might leave.
// -----------------------------------------------------------------------------
//
CNewContactLauncherDocument::CNewContactLauncherDocument(
        CEikApplication& aApp) :
    CAknDocument(aApp)
    {
    // No implementation required
    }

// ---------------------------------------------------------------------------
// CNewContactLauncherDocument::~CNewContactLauncherDocument()
// Destructor.
// ---------------------------------------------------------------------------
//
CNewContactLauncherDocument::~CNewContactLauncherDocument()
    {
    // No implementation required
    }

// ---------------------------------------------------------------------------
// CNewContactLauncherDocument::CreateAppUiL()
// Constructs CreateAppUi.
// ---------------------------------------------------------------------------
//
CEikAppUi* CNewContactLauncherDocument::CreateAppUiL()
    {
    // Create the application user interface, and return a pointer to it;
    // the framework takes ownership of this object
    return new (ELeave) CNewContactLauncherAppUi;
    }

// ----------------------------------------------------
// CIMCVAppDocument::UpdateTaskNameL()
// Makes Startup-application hidden in menu shell and fastswap window
// ----------------------------------------------------
void CNewContactLauncherDocument::UpdateTaskNameL( CApaWindowGroupName* aWgName )
    {
    CEikDocument::UpdateTaskNameL( aWgName );
    aWgName->SetHidden( ETrue );
    }

// End of File
