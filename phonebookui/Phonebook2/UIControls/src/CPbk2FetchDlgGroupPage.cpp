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
* Description:  Phonebook 2 fetch dialog group page.
*
*/


#include "CPbk2FetchDlgGroupPage.h"

// Phonebook 2
#include "CPbk2FetchDlgGroupMarker.h"
#include <CPbk2NamesListControl.h>
#include <MPbk2FetchDlg.h>
#include <MPbk2FetchDlgObserver.h>
#include <CPbk2StoreConfiguration.h>
#include <MPbk2Command.h>
#include <Pbk2UIControls.hrh>
#include <Pbk2UIControls.rsg>
#include <CPbk2ViewState.h>

// Virtual Phonebook
#include <MVPbkContactViewBase.h>
#include <CVPbkContactLinkArray.h>
#include <MVPbkContactLink.h>
#include <MVPbkBaseContact.h>
#include <CVPbkContactManager.h>
#include <CVPbkContactStoreUriArray.h>
#include <MVPbkContactStore.h>
#include <MVPbkContactStoreList.h>
#include <MVPbkContactStoreProperties.h>
#include <TVPbkContactStoreUriPtr.h>

// System includes
#include <calslbs.h>
#include <aknlayout.cdl.h>
#include <StringLoader.h>

/// Unnamed namespace for local definitions
namespace {

// LOCAL CONSTANTS AND MACROS
#ifdef _DEBUG
enum TPanicCode
    {
    EPanicPreCond_HandleContactViewListControlEventL = 1
    };

void Panic(TPanicCode aReason)
    {
    _LIT(KPanicText, "CPbk2FetchDlgGroupPage");
    User::Panic(KPanicText, aReason);
    }
#endif // _DEBUG

} /// namespace


// --------------------------------------------------------------------------
// CPbk2FetchDlgGroupPage::CPbk2FetchDlgGroupPage
// --------------------------------------------------------------------------
//
CPbk2FetchDlgGroupPage::CPbk2FetchDlgGroupPage
        ( MPbk2FetchDlg& aParentDlg, TInt aControlId,
          CVPbkContactManager& aContactManager ) :
            iParentDlg( aParentDlg ),
            iContactManager( aContactManager ),
            iControlId( aControlId ),
            iIsReady( EFalse )
    {
    }

// --------------------------------------------------------------------------
// CPbk2FetchDlgGroupPage::~CPbk2FetchDlgGroupPage
// --------------------------------------------------------------------------
//
CPbk2FetchDlgGroupPage::~CPbk2FetchDlgGroupPage()
    {
    delete iMarker;

    if (iContactView)
        {
        iContactView->RemoveObserver( *iControl );
        }
    }

// --------------------------------------------------------------------------
// CPbk2FetchDlgGroupPage::ConstructL
// --------------------------------------------------------------------------
//
inline void CPbk2FetchDlgGroupPage::ConstructL()
    {
    iControl = static_cast<CPbk2NamesListControl*>
        ( iParentDlg.FetchDlgControl( iControlId ) );
    iContactView = &iParentDlg.FetchDlgViewL( iControlId );

    iControl->AddObserverL( *this );
    iContactView->AddObserverL( *iControl );

    AknLayoutUtils::LayoutControl
        ( iControl, iParentDlg.FetchDlgClientRect(),
          AknLayout::list_gen_pane( 0 ) );
    }

// --------------------------------------------------------------------------
// CPbk2FetchDlgGroupPage::NewL
// --------------------------------------------------------------------------
//
CPbk2FetchDlgGroupPage* CPbk2FetchDlgGroupPage::NewL
        ( MPbk2FetchDlg& aParentDlg, TInt aControlId,
          CVPbkContactManager& aContactManager )
    {
    CPbk2FetchDlgGroupPage* self = new ( ELeave ) CPbk2FetchDlgGroupPage
        ( aParentDlg, aControlId, aContactManager );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// --------------------------------------------------------------------------
// CPbk2FetchDlgGroupPage::FetchDlgPageId
// --------------------------------------------------------------------------
//
TInt CPbk2FetchDlgGroupPage::FetchDlgPageId() const
    {
    return iControlId;
    }

// --------------------------------------------------------------------------
// CPbk2FetchDlgGroupPage::ActivateFetchDlgPageL
// --------------------------------------------------------------------------
//
void CPbk2FetchDlgGroupPage::ActivateFetchDlgPageL()
    {
    }

// --------------------------------------------------------------------------
// CPbk2FetchDlgGroupPage::DeactivateFetchDlgPage
// --------------------------------------------------------------------------
//
void CPbk2FetchDlgGroupPage::DeactivateFetchDlgPage()
    {
    }

// --------------------------------------------------------------------------
// CPbk2FetchDlgGroupPage::FocusedContactL
// --------------------------------------------------------------------------
//
const MVPbkBaseContact* CPbk2FetchDlgGroupPage::FocusedContactL() const
    {
    return iControl->FocusedContactL();
    }

// --------------------------------------------------------------------------
// CPbk2FetchDlgGroupPage::Control
// --------------------------------------------------------------------------
//
MPbk2ContactUiControl& CPbk2FetchDlgGroupPage::Control() const
    {
    return *iControl;
    }

// --------------------------------------------------------------------------
// CPbk2FetchDlgGroupPage::View
// --------------------------------------------------------------------------
//
MVPbkContactViewBase& CPbk2FetchDlgGroupPage::View() const
    {
    return iControl->View();
    }

// --------------------------------------------------------------------------
// CPbk2FetchDlgGroupPage::SetViewL
// --------------------------------------------------------------------------
//
void CPbk2FetchDlgGroupPage::SetViewL( MVPbkContactViewBase& aView )
    {
    iControl->SetViewL( aView );
    }

// --------------------------------------------------------------------------
// CPbk2FetchDlgGroupPage::DlgPageReady
// --------------------------------------------------------------------------
//
TBool CPbk2FetchDlgGroupPage::DlgPageReady() const
    {
    return iIsReady;
    }

// --------------------------------------------------------------------------
// CPbk2FetchDlgGroupPage::DlgPageEmpty
// --------------------------------------------------------------------------
//
TBool CPbk2FetchDlgGroupPage::DlgPageEmpty() const
    {
    return ( iControl->NumberOfContacts() == 0 );
    }

// --------------------------------------------------------------------------
// CPbk2FetchDlgGroupPage::SelectContactL
// --------------------------------------------------------------------------
//
void CPbk2FetchDlgGroupPage::SelectContactL
        ( const MVPbkContactLink& /*aContactLink*/, TBool /*aSelect*/ )
    {
    if ( !iMarker )
        {
        iMarker = CPbk2FetchDlgGroupMarker::NewL
            ( iContactManager, iParentDlg.FetchDlgSelection(),
              *iContactView, *iControl );
        }

    iMarker->Cancel();
    iMarker->MarkSelectedGroupsL();
    }

// --------------------------------------------------------------------------
// CPbk2FetchDlgGroupPage::HandleResourceChange
// --------------------------------------------------------------------------
//
void CPbk2FetchDlgGroupPage::HandleResourceChange( TInt aType )
    {
    if ( aType == KEikDynamicLayoutVariantSwitch )
        {
        AknLayoutUtils::LayoutControl
            ( iControl, iParentDlg.FetchDlgClientRect(),
              AKN_LAYOUT_WINDOW_list_gen_pane( 0 ) );
        }
    }

// --------------------------------------------------------------------------
// CPbk2FetchDlgGroupPage::HandleControlEventL
// --------------------------------------------------------------------------
//
void CPbk2FetchDlgGroupPage::HandleControlEventL
        ( MPbk2ContactUiControl& aControl, const TPbk2ControlEvent& aEvent )
    {
    __ASSERT_DEBUG(&aControl==iControl,
        Panic(EPanicPreCond_HandleContactViewListControlEventL));

    switch ( aEvent.iEventType )
        {
        case TPbk2ControlEvent::EReady:
            {
            SetPagesReadyStateL( ETrue );
            iControl->ShowThumbnail();
            break;
            }

        case TPbk2ControlEvent::EUnavailable:
            {
            SetPagesReadyStateL( EFalse );
            break;
            }

        case TPbk2ControlEvent::EContactSelected:
            {
            HandleContactSelectionL
                ( aControl.FocusedContactL(), aEvent.iInt, ETrue );
            break;
            }

        case TPbk2ControlEvent::EContactUnselected:
            {
            HandleContactSelectionL
                ( aControl.FocusedContactL(), aEvent.iInt, EFalse );
            break;
            }

        case TPbk2ControlEvent::EItemAdded:
            {
            // Notify parent dialog that this page's contents has changed
            iParentDlg.FetchDlgPageChangedL( *this );
            break;
            }

        case TPbk2ControlEvent::EItemRemoved:
            {
            // Notify parent dialog that this page's contents has changed
            iParentDlg.FetchDlgPageChangedL( *this );
            break;
            }

        case TPbk2ControlEvent::EContactSetChanged:
            {
            // Notify parent dialog that this page's contents has changed
            iParentDlg.FetchDlgPageChangedL( *this );
            break;
            }

        case TPbk2ControlEvent::EControlStateChanged:
            {
            UpdateListEmptyTextL( aEvent.iInt );
            iControl->DrawDeferred();
            break;
            }

        case TPbk2ControlEvent::EContactUnselectedAll:  // FALLTHROUGH
        default:
            {
            // Do nothing
            break;
            };
        }

    // Forward event to parent dialog
    iParentDlg.HandleControlEventL( aControl, aEvent );
    }

// --------------------------------------------------------------------------
// CPbk2FetchDlgGroupPage::HandleContactSelectionL
// --------------------------------------------------------------------------
//
void CPbk2FetchDlgGroupPage::HandleContactSelectionL
        ( const MVPbkBaseContact* aContact, TInt /*aIndex*/,
          TBool aSelected )
    {
    if ( aContact )
        {
        MVPbkContactLink* link = aContact->CreateLinkLC();
        iParentDlg.SelectContactL( *link, aSelected );
        CleanupStack::PopAndDestroy();
        }
    }

// --------------------------------------------------------------------------
// CPbk2FetchDlgGroupPage::SetPagesReadyStateL
// --------------------------------------------------------------------------
//
void CPbk2FetchDlgGroupPage::SetPagesReadyStateL( TBool aReadyState )
    {
    iIsReady = aReadyState;
    iParentDlg.FetchDlgPageChangedL( *this ); // call this to update CBAs
    }

// --------------------------------------------------------------------------
// CPbk2FetchDlgGroupPage::UpdateListEmptyTextL
// --------------------------------------------------------------------------
//
void CPbk2FetchDlgGroupPage::UpdateListEmptyTextL( TInt aListState )
    {
    HBufC* text = ListEmptyTextLC( aListState );

    if ( text )
        {
        iControl->SetTextL( *text );
        CleanupStack::PopAndDestroy(); // text
        }
    }

// --------------------------------------------------------------------------
// CPbk2FetchDlgGroupPage::ListEmptyTextLC
// --------------------------------------------------------------------------
//
HBufC* CPbk2FetchDlgGroupPage::ListEmptyTextLC( TInt aListState )
    {
    HBufC* text = NULL;

    switch ( aListState )
        {
        case CPbk2NamesListControl::EStateEmpty:        // FALLTHROUGH
        case CPbk2NamesListControl::EStateNotReady:
            {
            text = StringLoader::LoadLC( R_QTN_PHOB_NO_GROUPS_IN_FETCH );
            break;
            }

        case CPbk2NamesListControl::EStateReady:        // FALLTHROUGH
        case CPbk2NamesListControl::EStateFiltered:
            {
            text = StringLoader::LoadLC( R_PBK2_FIND_NO_MATCHES );
            break;
            }

        default:
            {
            text = KNullDesC().AllocLC();
            break;
            }
        }
    return text;
    }

// End of File
