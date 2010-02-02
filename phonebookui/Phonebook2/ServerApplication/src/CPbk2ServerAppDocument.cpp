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
* Description:  Phonebook 2 server application document.
*
*/


#include "CPbk2ServerAppDocument.h"

// Phonebook 2
#include "CPbk2ServerAppAppUi.h"
#include "CPbk2ApplicationServices.h"
#include <CPbk2UIExtensionManager.h>

// --------------------------------------------------------------------------
// CPbk2ServerAppDocument::CPbk2ServerAppDocument
// --------------------------------------------------------------------------
//
CPbk2ServerAppDocument::CPbk2ServerAppDocument( CEikApplication& aApp ):
        CPbk2DocumentBase( aApp )
    {
    }

// --------------------------------------------------------------------------
// CPbk2ServerAppDocument::~CPbk2ServerAppDocument
// --------------------------------------------------------------------------
//
CPbk2ServerAppDocument::~CPbk2ServerAppDocument()
    {
    Release( iExtensionManager );
    Release( iAppServices );
    }

// --------------------------------------------------------------------------
// CPbk2ServerAppDocument::NewL
// --------------------------------------------------------------------------
//
CPbk2ServerAppDocument* CPbk2ServerAppDocument::NewL( CEikApplication& aApp )
    {
    CPbk2ServerAppDocument* self =
        new(ELeave) CPbk2ServerAppDocument( aApp );
    return self;
    }

// --------------------------------------------------------------------------
// CPbk2ServerAppDocument::CreateGlobalsL
// --------------------------------------------------------------------------
//
void CPbk2ServerAppDocument::CreateGlobalsL()
    {
    iExtensionManager = CPbk2UIExtensionManager::InstanceL();
    }

// --------------------------------------------------------------------------
// CPbk2ServerAppDocument::ApplicationServicesL
// --------------------------------------------------------------------------
//
MPbk2ApplicationServices* CPbk2ServerAppDocument::ApplicationServicesL()
    {
    iAppServices = CPbk2ApplicationServices::InstanceL();
    return iAppServices;
    }

// --------------------------------------------------------------------------
// CPbk2ServerAppDocument::CreateAppUiL
// --------------------------------------------------------------------------
//
CEikAppUi* CPbk2ServerAppDocument::CreateAppUiL()
    {
    return new ( ELeave ) CPbk2ServerAppAppUi;
    }

// --------------------------------------------------------------------------
// CPbk2ServerAppDocument::ExtensionManager
// --------------------------------------------------------------------------
//
CPbk2UIExtensionManager& CPbk2ServerAppDocument::ExtensionManager() const
    {
    return *iExtensionManager;
    }

// End of File
