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
*       Provides Phonebook Application class methods.
*
*/


// INCLUDE FILES
#include "CPbkApplication.h"
#include <coemain.h>
#include <eikstart.h>
#include "CPbkDocument.h"
#include "PbkUID.h"

#include <pbkdebug.h>
#include "PbkProfiling.h"


// ================= MEMBER FUNCTIONS =======================

RFs& CPbkApplication::FsSession()
    {
    return iCoeEnv->FsSession();
    }

CCoeEnv& CPbkApplication::CoeEnv()
    {
    return *iCoeEnv;
    }

/**
 * Destructor.
 */
CPbkApplication::~CPbkApplication()
    {
    }

/**
 * Creates the Phonebook document object (CPbkDocument). 
 * Called by the application framework.
 * 
 * @return New Phonebook document object (CPbkDocument).
 */
CApaDocument* CPbkApplication::CreateDocumentL()
    {
    return CPbkDocument::NewL(*this);
    }

/**
 * Returns the UID of this application.
 */	    
TUid CPbkApplication::AppDllUid() const
    {
    return TUid::Uid(KPbkUID3);
    }


//  OTHER EXPORTED FUNCTIONS
//=============================================================================

/**
 * Creates a new Phonebook application object (CPbkApplication). 
 * Called by the application framework.
 * @return  The new application object (CPbkApplication). 
 *          NULL if out of memory.
 */
EXPORT_C CApaApplication* NewApplication()
    {
    // Reset all performance profiles
#ifdef PBK_ENABLE_PROFILE
    for (TInt bin = 0; bin < PbkProfiling::KEndProfileBins; ++bin)
        {
        __PBK_PROFILE_RESET(bin);
        }
#endif // PBK_ENABLE_PROFILE

    // Ending this profile is in CPbkNamesListAppView::HandleContactViewListControlEventL
    __PBK_PROFILE_START(PbkProfiling::EFullStartup);
    // Ending this profile is in CPbkAppUi::ConstructL
    __PBK_PROFILE_START(PbkProfiling::EBeforeAppUiConstruction);

	return new CPbkApplication;
    }

/**
 * Exe entry point (Phonebook.exe).
 */
GLDEF_C TInt E32Main()
	{
	return EikStart::RunApplication(NewApplication);
	}

//  End of File
