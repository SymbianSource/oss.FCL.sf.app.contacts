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
* Description:  Phonebook 2 view explorer.
*
*/


// INCLUDE FILES
#include "CPbk2ViewExplorer.h"

// Phonebook 2
#include "CPbk2ViewActivationTransaction.h"
#include "CPbk2ViewState.h"
#include "CPbk2AppViewFactory.h"
#include "CPbk2AppUi.h"
#include "CPbk2Document.h"
#include <Phonebook2.rsg>
#include <CPbk2ViewGraph.h>
#include <CPbk2AppViewBase.h>
#include <CPbk2IconInfoContainer.h>
#include <CPbk2IconFactory.h>
#include <CPbk2UIExtensionManager.h>
#include <Pbk2UID.h>
#include <TPbk2IconId.h>

// System includes
#include <barsread.h>
#include <aknnavide.h>
#include <eikapp.h>
#include <aknview.h>

// Debugging headers
#include <Pbk2Debug.h>
#include <Pbk2Profile.h>


/// Unnamed namespace for local definitions
namespace {

#ifdef _DEBUG

enum TPanicCode
    {
    EPanicPreCond_ConstructL = 1,
    EPanicPostCond_ConstructL,
    };

void Panic(TPanicCode aReason)
    {
    _LIT( KPanicText, "CPbk2ViewExplorer" );
    User::Panic( KPanicText, aReason );
    }

#endif // _DEBUG

} /// namespace


// --------------------------------------------------------------------------
// CPbk2ViewExplorer::CPbk2ViewExplorer
// --------------------------------------------------------------------------
//
CPbk2ViewExplorer::CPbk2ViewExplorer( CPbk2AppUi& aAppUi ) :
        iAppUi( aAppUi )
    {
    }

// --------------------------------------------------------------------------
// CPbk2ViewExplorer::~CPbk2ViewExplorer
// --------------------------------------------------------------------------
//
CPbk2ViewExplorer::~CPbk2ViewExplorer()
    {
    delete iViewGraph;
    }

// --------------------------------------------------------------------------
// CPbk2ViewExplorer::NewL
// --------------------------------------------------------------------------
//
CPbk2ViewExplorer* CPbk2ViewExplorer::NewL( CPbk2AppUi& aAppUi )
    {
    CPbk2ViewExplorer* self =
        new ( ELeave ) CPbk2ViewExplorer( aAppUi );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// --------------------------------------------------------------------------
// CPbk2ViewExplorer::ConstructL
// --------------------------------------------------------------------------
//
void CPbk2ViewExplorer::ConstructL()
    {
    PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING
        ("CPbk2ViewExplorer::ConstructL(0x%x)"), this);

    __ASSERT_DEBUG( !iViewGraph, Panic( EPanicPreCond_ConstructL ) );

    // Create UI elements
    PBK2_PROFILE_START(Pbk2Profile::EViewExplorerConstructionReadViewGraph);
    // Read view graph from resources and apply
    // UI extension view graph changes
    ReadViewGraphL();
    PBK2_PROFILE_END(Pbk2Profile::EViewExplorerConstructionReadViewGraph);

    __ASSERT_DEBUG( iViewGraph , Panic( EPanicPostCond_ConstructL ) );

    PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING
        ("CPbk2ViewExplorer::ConstructL end"));
    }

// --------------------------------------------------------------------------
// CPbk2ViewExplorer::CreateViewsL
// --------------------------------------------------------------------------
//
void CPbk2ViewExplorer::CreateViewsL()
    {
    PBK2_PROFILE_START(Pbk2Profile::EViewExplorerCreateAppViewFactory);
    CPbk2AppViewFactory* factory = CPbk2AppViewFactory::NewLC( iAppUi );
    PBK2_PROFILE_END(Pbk2Profile::EViewExplorerCreateAppViewFactory);

    // Create and add application views
    for (TInt i = 0; i < iViewGraph->Count(); ++i)
        {
        const CPbk2ViewNode& viewNode = (*iViewGraph)[i];
        if (!viewNode.ExitNode())
            {
            PBK2_PROFILE_START(Pbk2Profile::EViewExplorerCreateAppView);
            CAknView* view = factory->CreateAppViewLC( viewNode.ViewId() );
            PBK2_PROFILE_END(Pbk2Profile::EViewExplorerCreateAppView);

            PBK2_PROFILE_START(Pbk2Profile::EViewExplorerAddView);
            iAppUi.AddViewL( view );
            PBK2_PROFILE_END(Pbk2Profile::EViewExplorerAddView);
            CleanupStack::Pop( view );
            }
        }
    CleanupStack::PopAndDestroy( factory );
    }

// --------------------------------------------------------------------------
// CPbk2ViewExplorer::IsPhonebook2View
// --------------------------------------------------------------------------
//
TBool CPbk2ViewExplorer::IsPhonebook2View
        ( const TVwsViewId& aViewId ) const
    {
    TBool ret = EFalse;

    if ( aViewId.iAppUid == iAppUi.Application()->AppDllUid() )
        {
        const CPbk2ViewNode* viewNode = iViewGraph->FindNodeWithViewId
            ( aViewId.iViewUid );
        ret = viewNode && !viewNode->ExitNode();
        }

    return ret;
    }

// --------------------------------------------------------------------------
// CPbk2ViewExplorer::ActivatePhonebook2ViewL
// --------------------------------------------------------------------------
//
void CPbk2ViewExplorer::ActivatePhonebook2ViewL
        (TUid aViewId, const CPbk2ViewState* aViewState ) const
    {
    if ( aViewState )
        {
        HBufC8* stateBuf = aViewState->PackLC();
        iAppUi.ActivateLocalViewL
            ( aViewId, CPbk2ViewState::Uid(), *stateBuf );
        CleanupStack::PopAndDestroy();  // stateBuf
        }
    else
        {
        iAppUi.ActivateLocalViewL( aViewId );
        }
    }

// --------------------------------------------------------------------------
// CPbk2ViewExplorer::ActivatePreviousViewL
// --------------------------------------------------------------------------
//
void CPbk2ViewExplorer::ActivatePreviousViewL
        ( const CPbk2ViewState* aViewState )
    {
    // Find current view in the graph
    CPbk2ViewNode* viewNode =
        iViewGraph->FindNodeWithViewId( iAppUi.ActiveView()->Id() );

    if ( viewNode )
        {
        // Get previous node
        CPbk2ViewNode* prevNode = viewNode->PreviousNode();
        if ( !prevNode )
            {
            prevNode = viewNode->DefaultPreviousNode();
            }
        if ( prevNode )
            {
            if ( prevNode->ExitNode() )
                {
                iAppUi.ExitL();
                }
            else
                {
                ActivatePhonebook2ViewL( prevNode->ViewId(), aViewState );
                }
            }
        }
    }

// --------------------------------------------------------------------------
// CPbk2ViewExplorer::HandleViewActivationLC
// --------------------------------------------------------------------------
//
MPbk2ViewActivationTransaction* CPbk2ViewExplorer::HandleViewActivationLC
        ( const TUid& aViewId, const TVwsViewId& aPrevViewId,
          const TDesC* aTitlePaneText, const CEikImage* aContextPanePicture,
          TUint aFlags )
    {
    // Create and return an instance of view activation transaction
    return CPbk2ViewActivationTransaction::NewLC
            ( iAppUi, *this, aViewId, aPrevViewId, aTitlePaneText,
              aContextPanePicture, aFlags );
    }

// --------------------------------------------------------------------------
// CPbk2ViewExplorer::ViewGraph
// --------------------------------------------------------------------------
//
CPbk2ViewGraph& CPbk2ViewExplorer::ViewGraph() const
    {
    return *iViewGraph;
    }

// --------------------------------------------------------------------------
// CPbk2ViewExplorer::ReadViewGraphL
// Loads view navigation graph from resources.
// --------------------------------------------------------------------------
//
void CPbk2ViewExplorer::ReadViewGraphL()
    {
    TResourceReader reader;
    CCoeEnv::Static()->CreateResourceReaderLC( reader, R_PBK2_VIEWNODES );
    iViewGraph = CPbk2ViewGraph::NewL( reader );
    CleanupStack::PopAndDestroy(); // reader

    iAppUi.PhonebookDocument()->ExtensionManager().
            ApplyExtensionViewGraphChangesL( *iViewGraph );
    }

// End of File
