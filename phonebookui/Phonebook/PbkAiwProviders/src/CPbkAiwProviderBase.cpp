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
*     Implements generic AIW provider functionality.
*
*/


// INCLUDE FILES
#include "CPbkAiwProviderBase.h"

#include <barsread.h>
#include <phonebook.rsg>
#include <CPbkContactEngine.h>
#include <AknNoteWrappers.h>
#include <CPbkCommandStore.h>
#include <aknnavide.h>
#include "PbkDataCaging.hrh"
#include <pbkaiwprovidersres.rsg>
#include <pbkdebug.h>


/// Unnamed namespace for local definitions
namespace {

_LIT(KDllResFileName, "PbkAiwProvidersRes.rsc");

#ifdef _DEBUG
enum TPanicCode
    {
    EPanicPreCond_RestoreStatusPaneL = 1
    };

void Panic(TPanicCode aReason)
    {
    _LIT(KPanicText, "CPbkAiwProviderBase");
    User::Panic(KPanicText, aReason);
    }

#endif  // _DEBUG
}

// ================= MEMBER FUNCTIONS =======================

CPbkAiwProviderBase::CPbkAiwProviderBase()
    : iResourceFile(*CCoeEnv::Static())
    {
    }

void CPbkAiwProviderBase::BaseConstructL()
    {
    PBK_DEBUG_PRINT(PBK_DEBUG_STRING
        ("CPbkAiwProviderBase::ConstructL start"));

    _LIT(KSeparator, "\\");

    {
    TFileName fileName;
    fileName.Copy(KPbkRomFileDrive);
    fileName.Append(KDC_RESOURCE_FILES_DIR);
    fileName.Append(KSeparator);
    fileName.Append(KDllResFileName);
    iResourceFile.OpenL(fileName);
    } // TFileName goes out of scope

    TRAPD(err, iEngine = CPbkContactEngine::NewL(&CCoeEnv::Static()->FsSession()));
    if (err == KErrCorrupt)
        {
        // Database cannot be opened or recovered -> replace it with an
        // empty one. Always notify caller about DB corruption, even if
        // replacing fails.
        CAknNoteWrapper* note = new(ELeave) CAknNoteWrapper;
        note->ExecuteLD(R_PBKAIW_NOTE_DATABASE_CORRUPTED);
        iEngine = CPbkContactEngine::ReplaceL(&CCoeEnv::Static()->FsSession());
        }
    else if (err != KErrNone)
        {
        User::Leave(err);
        }

    PBK_DEBUG_PRINT(PBK_DEBUG_STRING
        ("CPbkAiwProviderBase::ConstructL end"));
    }

CPbkAiwProviderBase::~CPbkAiwProviderBase()
    {
    delete iEngine;
    delete iCommandStore;
    iResourceFile.Close();
    }

void CPbkAiwProviderBase::InitialiseL
        (MAiwNotifyCallback& /*aFrameworkCallback*/,
        const RCriteriaArray& aInterest)
    {
    iInterest = &aInterest;
    }

void CPbkAiwProviderBase::AddAndExecuteCommandL(MPbkCommand* aCmd)
    {
    if (!iCommandStore)
        {
        iCommandStore = CPbkCommandStore::NewL();
        }
    iCommandStore->AddAndExecuteCommandL(aCmd);
    }

void CPbkAiwProviderBase::SaveStatusPaneL()
    {
    CEikStatusPane* statusPane = iAvkonAppUi->StatusPane();
    if (statusPane)
        {
        // Store status pane resource id
        iStatusPaneResId = statusPane->CurrentLayoutResId();

        if (statusPane->PaneCapabilities
			(TUid::Uid(EEikStatusPaneUidNavi)).IsPresent())
            {
            // Get the navigation control container
            CAknNavigationControlContainer* naviContainer;
            naviContainer= static_cast<CAknNavigationControlContainer*>
                (statusPane->ControlL(TUid::Uid(EEikStatusPaneUidNavi)));
               
            if (naviContainer)
                {
                iNaviContainer = naviContainer;

                // Store the navi pane
                iNaviPane = iNaviContainer->Top();
                }
            }
        }
    }

void CPbkAiwProviderBase::RestoreStatusPaneL()
    {
    __ASSERT_DEBUG(iNaviContainer, 
        Panic(EPanicPreCond_RestoreStatusPaneL));

    CEikStatusPane* statusPane = iAvkonAppUi->StatusPane();
    if (statusPane)
        {
        // Restore status pane
        statusPane->SwitchLayoutL(iStatusPaneResId);
        }
    
    if (iNaviContainer && iNaviPane)
        {
        // Restore saved navi pane
        iNaviContainer->PushL(*iNaviPane);
        }
    }

// End of File

