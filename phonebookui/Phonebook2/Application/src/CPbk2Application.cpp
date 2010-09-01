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
* Description:  Phonebook 2 application class.
*
*/


// INCLUDE FILES
#include "CPbk2Application.h"

// Phonebook 2
#include "CPbk2Document.h"
#include <Pbk2UID.h>

// System includes
#include <coemain.h>
#include <eikstart.h>

// Debugging headers
#include <Pbk2Debug.h>
#include <Pbk2Profile.h>


/**
 * Creates and returns a new Phonebook 2 application object.
 * Called by the application framework.
 * Returns NULL if out of memory.
 *
 * @return  Phonebook application object.
 */
EXPORT_C CApaApplication* NewApplication()
    {
    PBK2_PROFILE_START(Pbk2Profile::EFullStartup);
    return new CPbk2Application;
    }

/**
 * Exe entry point (Phonebook2.exe).
 */
GLDEF_C TInt E32Main()
    {
    return EikStart::RunApplication(NewApplication);
    }


// --------------------------------------------------------------------------
// CPbk2Application::CPbk2Application
// --------------------------------------------------------------------------
//
CPbk2Application::CPbk2Application()
    {
    }

// --------------------------------------------------------------------------
// CPbk2Application::~CPbk2Application
// --------------------------------------------------------------------------
//
CPbk2Application::~CPbk2Application()
    {
    PBK2_PROFILE_DELETE();
    }

// --------------------------------------------------------------------------
// CPbk2Application::FsSession
// --------------------------------------------------------------------------
//
RFs& CPbk2Application::FsSession()
    {
    return iCoeEnv->FsSession();
    }

// --------------------------------------------------------------------------
// CPbk2Application::CoeEnv
// --------------------------------------------------------------------------
//
CCoeEnv& CPbk2Application::CoeEnv()
    {
    return *iCoeEnv;
    }

// --------------------------------------------------------------------------
// CPbk2Application::CreateDocumentL
// Creates and returns the Phonebook2 document object (CPbk2Document).
// Called by the application framework.
// --------------------------------------------------------------------------
//
CApaDocument* CPbk2Application::CreateDocumentL()
    {
    return CPbk2Document::NewL( *this );
    }

// --------------------------------------------------------------------------
// CPbk2Application::AppDllUid
// Returns the UID of this application.
// --------------------------------------------------------------------------
//
TUid CPbk2Application::AppDllUid() const
    {
    return TUid::Uid(KPbk2UID3);
    }

//  End of File
