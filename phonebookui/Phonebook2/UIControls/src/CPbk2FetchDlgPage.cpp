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
* Description:  Phonebook 2 fetch dialog page.
*
*/


#include "CPbk2FetchDlgPage.h"

// Phonebook 2
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
    _LIT(KPanicText, "CPbk2FetchDlgPage");
    User::Panic(KPanicText, aReason);
    }
#endif // _DEBUG

} /// namespace


// --------------------------------------------------------------------------
// CPbk2FetchDlgPage::CPbk2FetchDlgPage
// --------------------------------------------------------------------------
//
CPbk2FetchDlgPage::CPbk2FetchDlgPage
        ( MPbk2FetchDlg& aParentDlg, TInt aControlId ) :
            iParentDlg( aParentDlg ),
            iControlId( aControlId ),
            iIsReady( EFalse )
    {
    }

// --------------------------------------------------------------------------
// CPbk2FetchDlgPage::~CPbk2FetchDlgPage
// --------------------------------------------------------------------------
//
CPbk2FetchDlgPage::~CPbk2FetchDlgPage()
    {
    if (iContactView)
        {
        iContactView->RemoveObserver( *iControl );
        }
    }

// --------------------------------------------------------------------------
// CPbk2FetchDlgPage::ConstructL
// --------------------------------------------------------------------------
//
inline void CPbk2FetchDlgPage::ConstructL()
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
// CPbk2FetchDlgPage::NewL
// --------------------------------------------------------------------------
//
CPbk2FetchDlgPage* CPbk2FetchDlgPage::NewL
        ( MPbk2FetchDlg& aParentDlg, TInt aControlId )
    {
    CPbk2FetchDlgPage* self = new ( ELeave ) CPbk2FetchDlgPage
        ( aParentDlg, aControlId );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// --------------------------------------------------------------------------
// CPbk2FetchDlgPage::FetchDlgPageId
// --------------------------------------------------------------------------
//
TInt CPbk2FetchDlgPage::FetchDlgPageId() const
    {
    return iControlId;
    }

// --------------------------------------------------------------------------
// CPbk2FetchDlgPage::ActivateFetchDlgPageL
// --------------------------------------------------------------------------
//
void CPbk2FetchDlgPage::ActivateFetchDlgPageL()
    {
    }

// --------------------------------------------------------------------------
// CPbk2FetchDlgPage::DeactivateFetchDlgPage
// --------------------------------------------------------------------------
//
void CPbk2FetchDlgPage::DeactivateFetchDlgPage()
    {
    }

// --------------------------------------------------------------------------
// CPbk2FetchDlgPage::FocusedContactL
// --------------------------------------------------------------------------
//
const MVPbkBaseContact* CPbk2FetchDlgPage::FocusedContactL() const
    {
    return iControl->FocusedContactL();
    }

// --------------------------------------------------------------------------
// CPbk2FetchDlgPage::Control
// --------------------------------------------------------------------------
//
MPbk2ContactUiControl& CPbk2FetchDlgPage::Control() const
    {
    return *iControl;
    }

// --------------------------------------------------------------------------
// CPbk2FetchDlgPage::View
// --------------------------------------------------------------------------
//
MVPbkContactViewBase& CPbk2FetchDlgPage::View() const
    {
    return iControl->View();
    }

// --------------------------------------------------------------------------
// CPbk2FetchDlgPage::SetViewL
// --------------------------------------------------------------------------
//
void CPbk2FetchDlgPage::SetViewL( MVPbkContactViewBase& aView )
    {
    iControl->SetViewL( aView );
    }

// --------------------------------------------------------------------------
// CPbk2FetchDlgPage::DlgPageReady
// --------------------------------------------------------------------------
//
TBool CPbk2FetchDlgPage::DlgPageReady() const
    {
    return iIsReady;
    }

// --------------------------------------------------------------------------
// CPbk2FetchDlgPage::DlgPageEmpty
// --------------------------------------------------------------------------
//
TBool CPbk2FetchDlgPage::DlgPageEmpty() const
    {
    return ( iControl->NumberOfContacts() == 0 );
    }

// --------------------------------------------------------------------------
// CPbk2FetchDlgPage::SelectContactL
// --------------------------------------------------------------------------
//
void CPbk2FetchDlgPage::SelectContactL
        ( const MVPbkContactLink& aContactLink, TBool aSelect )
    {
    MVPbkContactViewBase& view = iControl->View();
    TInt index = view.IndexOfLinkL( aContactLink );
    if ( index >= KErrNone )
        {
        iControl->SetSelectedContactL( index, aSelect );
        }    
    }

// --------------------------------------------------------------------------
// CPbk2FetchDlgPage::HandleResourceChange
// --------------------------------------------------------------------------
//
void CPbk2FetchDlgPage::HandleResourceChange( TInt aType )
    {
    if ( aType == KEikDynamicLayoutVariantSwitch )
        {
        AknLayoutUtils::LayoutControl
            ( iControl, iParentDlg.FetchDlgClientRect(),
              AKN_LAYOUT_WINDOW_list_gen_pane( 0 ) );
        }
    }

// --------------------------------------------------------------------------
// CPbk2FetchDlgPage::HandleControlEventL
// --------------------------------------------------------------------------
//
void CPbk2FetchDlgPage::HandleControlEventL
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
// CPbk2FetchDlgPage::HandleContactSelectionL
// --------------------------------------------------------------------------
//
void CPbk2FetchDlgPage::HandleContactSelectionL
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
// CPbk2FetchDlgPage::SetPagesReadyStateL
// --------------------------------------------------------------------------
//
void CPbk2FetchDlgPage::SetPagesReadyStateL( TBool aReadyState )
    {
    iIsReady = aReadyState;
    iParentDlg.FetchDlgPageChangedL( *this ); // call this to update CBAs
    }

// --------------------------------------------------------------------------
// CPbk2FetchDlgPage::UpdateListEmptyTextL
// --------------------------------------------------------------------------
//
void CPbk2FetchDlgPage::UpdateListEmptyTextL( TInt aListState )
    {
    HBufC* text = ListEmptyTextLC( aListState );

    if ( text )
        {
        iControl->SetTextL( *text );
        CleanupStack::PopAndDestroy(); // text
        }
    }

// --------------------------------------------------------------------------
// CPbk2FetchDlgPage::ListEmptyTextLC
// --------------------------------------------------------------------------
//
HBufC* CPbk2FetchDlgPage::ListEmptyTextLC( TInt aListState )
    {
    HBufC* text = NULL;

    switch ( aListState )
        {
        case CPbk2NamesListControl::EStateEmpty:        // FALLTHROUGH
        case CPbk2NamesListControl::EStateNotReady:
            {
            text = StringLoader::LoadLC( R_QTN_PHOB_NO_ENTRIES_LONG );
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
