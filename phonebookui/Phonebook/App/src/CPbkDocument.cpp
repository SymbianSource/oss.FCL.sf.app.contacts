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
*          Provides phonebook document class methods
*
*/


// INCLUDE FILES
#include    "CPbkDocument.h"
#include    "CPbkApplication.h" // Phonebook application class
#include    "CPbkAppUi.h"       // Phonebook application UI class
#include    <pbkdebug.h>

// Engine classes
#include    <CPbkContactEngine.h> // Phonebook engine class
#include    <CPbkConstants.h>

#include    <apgwgnam.h>

// From PbkExt
#include    <CPbkExtGlobals.h>

// ================= MEMBER FUNCTIONS =======================

inline CPbkDocument::CPbkDocument(CEikApplication& aApp) :
    CPbkDocumentBase(aApp)
    {
    }    

inline void CPbkDocument::ConstructL()
    {
    }

CPbkDocument* CPbkDocument::NewL(CEikApplication& aApp)
    {
    CPbkDocument* self = new(ELeave) CPbkDocument(aApp);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

CPbkApplication* CPbkDocument::PbkApplication() const
    {
    // Explicit cast: application must always be of type CPbkApplication.
    return static_cast<CPbkApplication*>(Application());
    }

void CPbkDocument::CreateEngineL(TBool aReplace)
    {
    if (!iEngine)
        {
        if (aReplace)
            {
            iEngine = CPbkContactEngine::ReplaceL(&PbkApplication()->FsSession());
            }
        else
            {
            iEngine = CPbkContactEngine::NewL(&PbkApplication()->FsSession());
            }
        }
    }

void CPbkDocument::CreateGlobalsL()
    {
    if (!iExtGlobal)
        {
        iExtGlobal = CPbkExtGlobals::InstanceL();
        }
    }

CPbkContactEngine* CPbkDocument::Engine() const
    {
    return iEngine;
    }

/**
 * Destructor. Closes the engine.
 */
CPbkDocument::~CPbkDocument()
    {
    Release(iExtGlobal);
    // Destroy the engine
    delete iEngine;    
    }

/**
 * Creates and returns the Phonebook application UI object (CPbkAppUi). 
 * Called by the application framework.
 *
 * @return new Phonebook application UI object
 * @exception KErrNoMemory if out of memory.
 */
CEikAppUi* CPbkDocument::CreateAppUiL()
    {
    // Create an CPbkAppUi instance and return it.
    return (new(ELeave) CPbkAppUi);
    }

//  End of File  
