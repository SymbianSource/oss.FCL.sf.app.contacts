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
* Description:     Declares document for application.
*
*/






// INCLUDE FILES
#include <CPbkContactEngine.h>      // Phonebook Engine
#include <CPbkContactItem.h>        // Phonebook Contact

#include "speeddial.hrh"
#include "SpdiaAppUi.h"
#include "SpdiaDocument.h"

// ================= MEMBER FUNCTIONS =======================

// destructor
CSpdiaDocument::~CSpdiaDocument()
    {
    }

// C++ default constructor can NOT contain any code, that
// might leave.
//
CSpdiaDocument::CSpdiaDocument(CEikApplication& aApp)
    : CAknDocument(aApp)
    {
    }

// Symbian default constructor can leave.
void CSpdiaDocument::ConstructL()
    {
    }

    // Two-phased constructor.
CSpdiaDocument* CSpdiaDocument::NewL(
        CEikApplication& aApp)     // CSpeedDialApp reference
    {
    CSpdiaDocument* self = new (ELeave) CSpdiaDocument(aApp);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop();
    return self;
    }

// ----------------------------------------------------
// CSpdiaDocument::CreateAppUiL()
// constructs CSpdiaAppUi
//
// ----------------------------------------------------
CEikAppUi* CSpdiaDocument::CreateAppUiL()
    {
    return new(ELeave) CSpdiaAppUi;
    }

// End of File  
