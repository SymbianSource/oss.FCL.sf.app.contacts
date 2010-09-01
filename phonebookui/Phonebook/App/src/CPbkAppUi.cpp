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
*/


// INCLUDE FILES
#include "CPbkAppUi.h"       // This class' declaration

#include <pbkconfig.hrh>
#include <barsread.h>        // TResourceReader
#include <eikmenup.h>
#include <aknnavide.h>       // CAknNavigationDecorator
#include <aknnotewrappers.h>
#include <FeatMgr.h>
#include <hlplch.h>          // HlpLauncher

#include <Phonebook.rsg>     // Phonebook resources
#include "CPbkApplication.h" // Phonebook application class
#include "CPbkDocument.h"    // Phonebook document class
#include "PbkAppViewFactory.h"   // Application view factory
#include "CViewActivationTransactionImpl.h"
#include "CPbkAppGlobals.h"
#include "CPbkViewNavigator.h"
#include "CPbkStartupView.h"

//SendUi
#include <sendui.h>                 // Send UI API
#include <SenduiMtmUids.h>          // Send UI MTM uids


// PbkEng.dll header files
#include <DigViewGraph.h>

// PbkView.dll header files
#include <CPbkViewState.h>
#include <CPbkCompressUiImpl.h>  // Phonebook compression UI
#include <CPbkFFSCheck.h>

// PbkUI.dll header files
#include <CPbkAppViewBase.h>
#include <MPbkCommandFactory.h>

// From PbkExt
#include <MPbkExtensionFactory.h>
#include <CPbkExtGlobals.h>

// Engine classes
#include    <CPbkContactEngine.h> // Phonebook engine class
#include    <CPbkConstants.h>

// Debugging headers
#include <pbkdebug.h>
#include "PbkProfiling.h"


// CONSTANTS
// View UID constants
const TUid CPbkAppUi::KPbkNamesListViewUid          = { EPbkNamesListViewId         };
const TUid CPbkAppUi::KPbkGroupsListViewUid         = { EPbkGroupsListViewId        };
const TUid CPbkAppUi::KPbkGroupMembersListViewUid   = { EPbkGroupMembersListViewId  };
const TUid CPbkAppUi::KPbkContactInfoViewUid        = { EPbkContactInfoViewId       };
const TUid CPbkAppUi::KPbkPhotoViewUid              = { EPbkPhotoViewViewId         };
const TUid CPbkAppUi::KPbkPersonalInfoViewUid       = { EPbkPersonalInfoViewId      };

/// Unnamed namespace for local definitions
namespace {

// LOCAL CONSTANTS AND MACROS
#ifdef _DEBUG
enum TPanicCode
    {
    EPanicPreCond_ConstructL = 1,
    EPanicPostCond_ConstructL,
    };
    
#else
// Amount of free heap memory required for always-on functionality
const TInt KAlwaysOnMinimumTreshold = 200*1024; // 200 kB

#endif


// ==================== LOCAL FUNCTIONS ====================

#ifdef _DEBUG
void Panic(TPanicCode aReason)
    {
    _LIT(KPanicText, "CPbkAppUi");
    User::Panic(KPanicText, aReason);
    }
#endif

}  // namespace


// ================= MEMBER FUNCTIONS =======================

CPbkAppUi::CPbkAppUi() :
    iViewResourceFile(*iCoeEnv)
    {
    }

inline void CPbkAppUi::CreateEngineL()
    {
    TRAPD(err, PbkDocument()->CreateEngineL(EFalse));
    if (err == KErrCorrupt)
        {
        // Database cannot be opened or recoverd -> replace it with an
        // empty one. Always notify caller about DB corruption, even if
        // replacing fails.
        CAknNoteWrapper* note = new(ELeave) CAknNoteWrapper;
        note->ExecuteLD(R_PBK_NOTE_DATABASE_CORRUPTED);
        PbkDocument()->CreateEngineL(ETrue);
        }
    else if (err != KErrNone)
        {
        User::Leave(err);
        }
    }

/**
 * Loads view navigation graph from resources.
 */
inline void CPbkAppUi::ReadViewGraphL()
    {
    TResourceReader reader;
    iCoeEnv->CreateResourceReaderLC(reader, R_PBK_VIEWNODES);
    iViewGraph = CDigViewGraph::NewL(reader);
    CleanupStack::PopAndDestroy(); // reader

    iAppUiExtension->ApplyExtensionViewGraphChangesL(*iViewGraph);
    }

void CPbkAppUi::ConstructL()
    {
    __ASSERT_DEBUG(
        !iViewGraph && !iNaviPane && !iCompressUi,
        Panic(EPanicPreCond_ConstructL));

    PBK_DEBUG_PRINT(PBK_DEBUG_STRING("PHONEBOOK: started"));

    // Init base class
    __PBK_PROFILE_START(PbkProfiling::EAppUiBaseConstruct);
    BaseConstructL(EAknEnableSkin);
    __PBK_PROFILE_END(PbkProfiling::EAppUiBaseConstruct);

    // Initialize feature manager
    FeatureManager::InitializeLibL();

    // Create Phonebook engine
    PBK_DEBUG_PRINT(PBK_DEBUG_STRING("Engine construct begin"));
    CreateEngineL();
    PBK_DEBUG_PRINT(PBK_DEBUG_STRING("Engine construct end"));
    PbkDocument()->CreateGlobalsL();

    PBK_DEBUG_PRINT(PBK_DEBUG_STRING("Extension construct start"));
    CPbkExtGlobals* extGlobal = CPbkExtGlobals::InstanceL();
    extGlobal->PushL();
    iAppUiExtension = extGlobal->FactoryL().
        CreatePbkAppUiExtensionL(*PbkDocument()->Engine());
    CleanupStack::PopAndDestroy(extGlobal);
    PBK_DEBUG_PRINT(PBK_DEBUG_STRING("Extension construct end"));

    // Create UI elements
    PBK_DEBUG_PRINT(PBK_DEBUG_STRING("Viewnavi construct start"));
    __PBK_PROFILE_START(PbkProfiling::EViewNaviConstruct);
    ReadViewGraphL();
    iNaviPane = static_cast<CAknNavigationControlContainer*>
        (StatusPane()->ControlL(TUid::Uid(EEikStatusPaneUidNavi)));
    iViewNavigator = CPbkViewNavigator::NewL(*iViewGraph, *this);
    __PBK_PROFILE_END(PbkProfiling::EViewNaviConstruct);
    PBK_DEBUG_PRINT(PBK_DEBUG_STRING("Viewnavi construct end"));

    // Load view dll resource file
    iViewResourceFile.OpenL();
    // Create FFS space checker
    iFFSCheck = CPbkFFSCheck::NewL(iCoeEnv);
    // Create the compression UI
    iCompressUi = CPbkCompressUiImpl::NewL(*PbkDocument()->Engine());
    // Enable compression always
    iCompressUi->EnableCompression(ETrue);

    PBK_DEBUG_PRINT(PBK_DEBUG_STRING("Views construct start"));
    CPbkStartupView* startupView = CPbkStartupView::NewLC(*iAppUiExtension);
    AddViewL(startupView);
    CleanupStack::Pop(startupView);
    SetDefaultViewL(*startupView);
    CreateViewsL();
    PBK_DEBUG_PRINT(PBK_DEBUG_STRING("Views construct end"));

    CCoeEnv::Static()->AddForegroundObserverL(*this);

    __PBK_PROFILE_START(PbkProfiling::EStartupViewActivation);

    __ASSERT_DEBUG(
        iViewGraph && iNaviPane && iCompressUi,
        Panic(EPanicPostCond_ConstructL));
    }

CPbkAppUi::~CPbkAppUi()
    {
    PBK_DEBUG_PRINT(PBK_DEBUG_STRING("CPbkAppUi::~CPbkAppUi()"));

    CCoeEnv::Static()->RemoveForegroundObserver(*this);

    // delete data members
    delete iCompressUi;
    delete iFFSCheck;
    iViewResourceFile.Close();
    // Application views will be deleted by the base class destructor.
    delete iViewNavigator;
    delete iViewGraph;
    delete iAppGlobals;
    Release(iAppUiExtension);

    FeatureManager::UnInitializeLib();
    }

CPbkDocument* CPbkAppUi::PbkDocument() const
    {
    // Explicit cast: the document must always be of type CPbkDocument
    return static_cast<CPbkDocument*>(Document());
    }

CPbkApplication* CPbkAppUi::PbkApplication() const
    {
    // Explicit cast: the application must always be of type CPbkApplication
    return static_cast<CPbkApplication*>(Application());
    }

void CPbkAppUi::ActivatePhonebookViewL
        (TUid aViewId,
        const CPbkViewState* aViewState/*=NULL*/)
    {
    if (aViewState)
        {
        HBufC8* stateBuf = aViewState->PackLC();
        ActivateLocalViewL(aViewId, CPbkViewState::Uid(), *stateBuf);
        CleanupStack::PopAndDestroy();  // stateBuf
        }
    else
        {
        ActivateLocalViewL(aViewId);
        }
    }

void CPbkAppUi::ActivatePreviousViewL
        (const CPbkViewState* aViewState /*=NULL*/)
    {
    // Find current view in the graph
    CDigViewNode* viewNode = iViewGraph->FindNodeWithViewId(iView->Id());
    if (viewNode)
        {
        // get previous node
        CDigViewNode* prevNode = viewNode->PreviousNode();
        if (!prevNode)
            {
            prevNode = viewNode->DefaultPreviousNode();
            }
        if (prevNode)
            {
            if (prevNode->ExitNode())
                {
                ExitL();
                }
            else
                {
                ActivatePhonebookViewL(prevNode->ViewId(), aViewState);
                }
            }
        }
    }

CAknNavigationControlContainer* CPbkAppUi::NaviPane()
    {
    return iNaviPane;
    }

void CPbkAppUi::FFSClCheckL(const MPbkCommandHandler& aCommandHandler,
                            TInt aBytesToWrite /*=0*/)
    {
    iFFSCheck->FFSClCheckL(aCommandHandler, aBytesToWrite);
    }

CPbkAppUi::CViewActivationTransaction* CPbkAppUi::HandleViewActivationLC
        (const TUid& aViewId, const TVwsViewId& aPrevViewId,
        const TDesC* aTitlePaneText,
        const CEikImage* aContextPanePicture,
        TUint aFlags)
    {
    // Create and return an instance of view activation transaction
    return CViewActivationTransactionImpl::NewLC
        (*this, aViewId, aPrevViewId, aTitlePaneText,
        aContextPanePicture, aFlags);
    }

CPbkAppGlobalsBase* CPbkAppUi::AppGlobalsL()
    {
    if (!iAppGlobals)
        {
        iAppGlobals = CPbkAppGlobals::NewL();
        }
    return iAppGlobals;
    }

void CPbkAppUi::ExitL()
    {
    // Set this flag to EFalse to signal that we're either dying or going
    // to background.
    iIsRunningForeground = EFalse;

#ifndef _DEBUG

    // Check that we have enough free heap memory to go to background.
    // User::Available() does not check for the possibility to grow the heap,
    // so we'll need to allocate a dummy block and release it immediately.
    TAny* tmp = User::Alloc(KAlwaysOnMinimumTreshold);
    delete tmp;
    if (!iEndKeyExit && tmp &&
        PbkDocument()->Engine()->Constants()->LocallyVariatedFeatureEnabled(
            EPbkLVAlwaysOnPhonebook))
        {
        CPbkViewState* state = CPbkViewState::NewLC();
        state->SetFlags(CPbkViewState::EInitialized | CPbkViewState::ESendToBackground);
        // Activation of Names list view will send application to background.
        // Application can not be sent here to background because
        // activation is asynchronous operation and application could be set
        // to background before the view activation.
        ActivatePhonebookViewL(KPbkNamesListViewUid, state);
        CleanupStack::PopAndDestroy();  // state
        }
    else
        {
        CAknViewAppUi::ProcessCommandL(EAknCmdExit);
        }
#else // _DEBUG
    CAknViewAppUi::ProcessCommandL(EAknCmdExit);
#endif // _DEBUG
    }

void CPbkAppUi::Exit()
    {
    CPbkAppUiBase::Exit();
    }

TBool CPbkAppUi::IsRunningForeground() const
    {
    return iIsRunningForeground;
    }

/**
 * Returns true if aViewId is a phonebook view.
 */
TBool CPbkAppUi::IsPhonebookView(const TVwsViewId& aViewId) const
    {
    if (aViewId.iAppUid == Application()->AppDllUid())
        {
        const CDigViewNode* viewNode =
            iViewGraph->FindNodeWithViewId(aViewId.iViewUid);
        return viewNode && !viewNode->ExitNode();
        }
    else
        {
        return EFalse;
        }
    }

inline void CPbkAppUi::CmdExitL()
    {
    PBK_DEBUG_PRINT(PBK_DEBUG_STRING("CPbkAppUi::CmdExitL()"));
    // notify views before the exit is executed
    CAknViewAppUi::ProcessCommandL(EPbkCmdExit);
    CAknViewAppUi::Exit();
    }

void CPbkAppUi::SendAppToBackgroundL()
    {
    // Get the current task
    RWsSession ws;
    User::LeaveIfError(ws.Connect());
    TApaTaskList tasklist(ws);

    TApaTask task = tasklist.FindApp(Application()->AppDllUid());
    CleanupClosePushL(ws);
    if (task.Exists())
        {
        task.SendToBackground();
        }
    CleanupStack::PopAndDestroy(); // ws
    }

void CPbkAppUi::HandleCommandL(TInt aCommand)
    {
    switch (aCommand)
        {
        case EAknCmdHelp:
            {
            HlpLauncher::LaunchHelpApplicationL(iEikonEnv->WsSession(),
                AppHelpContextL());
            break;
            }

        case EEikCmdExit:
            {
            // standard exit command
            CmdExitL();
            break;
            }

        case EAknSoftkeyExit:
        case EAknCmdExit:
        case EPbkCmdExit:
            {
            ExitL();
            break;
            }

        default:
            {
            break;
            }
        }
    }

void CPbkAppUi::DynInitMenuPaneL
        (TInt aResourceId,
        CEikMenuPane* aMenuPane)
    {
    switch (aResourceId)
        {
        case R_PHONEBOOK_SYSTEM_MENU:
            {
            if (!FeatureManager::FeatureSupported(KFeatureIdHelp))
                {
                // remove non-supported help from menu
                aMenuPane->SetItemDimmed(EAknCmdHelp, ETrue);
                }
            break;
            }

        default:
            {
            break;
            }
        }
    }

TKeyResponse CPbkAppUi::HandleKeyEventL
        (const TKeyEvent& aKeyEvent,
        TEventCode aType)
    {
    if (PbkProcessKeyEventL(aKeyEvent, aType))
        {
        PBK_DEBUG_PRINT(PBK_DEBUG_STRING("CPbkAppUi::HandleKeyEventL() processed key event"));
        return EKeyWasConsumed;
        }
    return EKeyWasNotConsumed;
    }

void CPbkAppUi::HandleForegroundEventL(TBool aForeground)
    {
    // Call base class
    CAknViewAppUi::HandleForegroundEventL(aForeground);
    }

void CPbkAppUi::HandleResourceChangeL(TInt aType)
    {
    CPbkAppUiBase::HandleResourceChangeL(aType);
    // This is needed to change the application
    // icon when the skin changes (especially if
    // a contact with a thumbnail is currently
    // selected and the app icon is not visible)
    CEikStatusPane* pane = StatusPane();
    if (pane)
        {
        pane->HandleResourceChange(aType);
        }

    // The resource change event has to be forwarded to
    // view navigator
    if(iViewNavigator)
        {
        iViewNavigator->HandleResourceChange(aType);
        }
    }

void CPbkAppUi::HandleWsEventL(const TWsEvent& aEvent, CCoeControl* aDestination)
    {
    switch (aEvent.Type())
        {
        case KAknUidValueEndKeyCloseEvent:
            {
            // Make a real exit on end-key event
            iEndKeyExit = ETrue;
            } // fallthrough
        default:
            {
            CPbkAppUiBase::HandleWsEventL(aEvent, aDestination);
            break;
            }
        }
    }

TBool CPbkAppUi::PbkProcessKeyEventL
        (const TKeyEvent& aKeyEvent, TEventCode aType)
    {
    // Ensure that navigator has already been constructed
    if (iViewNavigator)
        {
        return iViewNavigator->HandleNavigationKeyEventL(aKeyEvent,aType);
        }
    return EFalse;
    }

void CPbkAppUi::CreateViewsL()
    {
    PBK_DEBUG_PRINT(PBK_DEBUG_STRING("CPbkAppUi::CreateViewsL start"));

    CPbkAppViewFactory* factory = CPbkAppViewFactory::NewLC();

    // Create and add application views
    for (TInt i = 0; i < iViewGraph->Count(); ++i)
        {
        const CDigViewNode& viewNode = (*iViewGraph)[i];
        if (!viewNode.ExitNode())
            {
            CAknView* view = factory->CreateAppViewLC(viewNode.ViewId());
            PBK_DEBUG_PRINT(PBK_DEBUG_STRING(
                "CPbkAppUi::CreateViewsL created view %d"), viewNode.ViewId());

            AddViewL(view);
            CleanupStack::Pop();  // view
            }
        }
    CleanupStack::PopAndDestroy(); // factory
    }

void CPbkAppUi::HandleGainingForeground()
    {
    PBK_DEBUG_PRINT(PBK_DEBUG_STRING("void CPbkAppUi::HandleGainingForeground()"));

    if (PbkDocument()->Engine()->Constants()->
            LocallyVariatedFeatureEnabled(EPbkLVAlwaysOnPhonebook))
        {
        // If phonebook is running in background (i.e. user has exited it) the
        // Names list view is active and it is incorrectly marked as being in
        // foreground (this happens during the view's activation
        // when exiting Phonebook). Therefore its HandleForegroundEventL() will
        // not be called unless we first mark the view explicitly as being in
        // background. Now when CPbkAppUi::HandleForegroundEventL(ETrue) is
        // called, the NamesListView's HandleForegroundEventL() is called
        // correctly
        if (!iIsRunningForeground && iView &&
            iView->Id() == KPbkNamesListViewUid)
            {
            TRAPD(err, HandleForegroundEventL(EFalse));
            if (KErrNone != err)
                {
                CCoeEnv::Static()->HandleError(err);
                }
            }        
        // this if-case is here, because if the end-key is pressed to execute 
        // the exit, the activation of names list view causes additional call of
        // HandleGainingForeground and HandleLosingForeground methods. 
        // In end-key-exit-case the phonebook should not be shown in fast
        // swap menu, and the additional gaining foreground call causes
        // the visibility of phonebook in fast swap if it is not 
        // blocked.        
        if ( !iEndKeyExit )
            {
            iIsRunningForeground = ETrue;
            }
        else
            {            
            iEndKeyExit = EFalse;
            }
        HideApplicationFromFSW(!iIsRunningForeground);
        }
    }

void CPbkAppUi::HandleLosingForeground()
    {
    PBK_DEBUG_PRINT(PBK_DEBUG_STRING("void CPbkAppUi::HandleLosingForeground()"));

    if (PbkDocument()->Engine()->Constants()->
            LocallyVariatedFeatureEnabled(EPbkLVAlwaysOnPhonebook))
        {
        HideApplicationFromFSW(!iIsRunningForeground);
        }
    }

//  End of File
