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
* Description:  Phonebook 2 group popup.
*
*/


#include "CPguGroupPopup.h"

// Phonebook 2
#include <CPbk2InputAbsorber.h>
#include <MPbk2ContactNameFormatter.h>
#include <MPbk2ApplicationServices.h>
#include <MPbk2AppUi.h>

// Virtual Phonebook
#include <MVPbkContactViewBase.h>
#include <MVPbkViewContact.h>
#include <MVPbkContactLink.h>
#include <MVPbkContactLinkArray.h>
#include <CVPbkFilteredContactView.h>
#include <CVPbkContactManager.h>

// System includes
#include <aknlists.h>
#include <aknPopup.h>
#include <avkon.rsg>
#include <StringLoader.h>

/// Unnamed namespace for local definitions
namespace {

const TInt KMaxListBoxText( 256 );

} /// namespace

// CLASS DECLARATION

class CPopupList : public CAknPopupList
    {
    public: // Construction and destruction

        /**
         * Creates a new instance of this class.
         *
         * @param aListbox      The listbox to use in the list.
         * @param aCbaResource  Softkey resource id.
         * @return  A new instance of this class.
         */
        static CPopupList* NewLC(
                CEikListBox* aListBox,
                TInt aCbaResource );

        /**
         * Destructor.
         */
        ~CPopupList();

    public: // Interface

        /**
         * Sets the object to resets the self pointer after destruction.
         *
         * @param aSelf     Self pointer.
         */
        void ResetWhenDestroyed(
                CPopupList** aSelf );

    protected: // Implementation
        CPopupList();

    private: // Data
        /// Ref: Self pointer
        CPopupList** iSelfPtr;
    };

class CListBoxModel : public CBase,
                      public MDesCArray
    {
    public: // Construction

        /**
         * Constructor.
         *
         * @param aView             View.
         * @param aNameFormatter    Name formatter
         */
        CListBoxModel(
                MVPbkContactViewBase& aView,
                MPbk2ContactNameFormatter& aNameFormatter ) :
                    iView( aView ),
                    iNameFormatter( aNameFormatter )
                        {
                        }

    public: // From MDesCArray
        TInt MdcaCount() const;
        TPtrC MdcaPoint(
                TInt aIndex ) const;

    private: // Implementation
        void FormatBufferL(
                TInt aIndex ) const;

    private: // Data
        // Ref: View
        MVPbkContactViewBase& iView;
        // Ref: Name formatter
        MPbk2ContactNameFormatter& iNameFormatter;
        // Own: Formatting buffer
        mutable TBuf<KMaxListBoxText> iFormattingBuffer;
        // Own: Counter
        mutable TInt iPreviousCount;
    };

// --------------------------------------------------------------------------
// CListBoxModel::MdcaCount
// --------------------------------------------------------------------------
//
TInt CListBoxModel::MdcaCount() const
    {
    TInt result = iPreviousCount;
    TRAPD( err, result = iView.ContactCountL() );
    if ( err != KErrNone )
        {
        CCoeEnv::Static()->HandleError( err );
        }
    iPreviousCount = result;
    return result;
    }

// --------------------------------------------------------------------------
// CListBoxModel::MdcaPoint
// --------------------------------------------------------------------------
//
TPtrC CListBoxModel::MdcaPoint( TInt aIndex ) const
    {
    iFormattingBuffer.Zero();

    TRAPD( err, FormatBufferL( aIndex ) );
    switch ( err )
        {
        case KErrNone:
            {
            // OK
            break;
            }
        case KErrNotFound:      // FALLTHROUGH
        case KErrAccessDenied:
            {
            // Do not report these errors as they occur sometimes when
            // the view is under a massive update
            break;
            }
        default:
            {
            // Report error
            CCoeEnv::Static()->HandleError( err );
            break;
            }
        }
    return iFormattingBuffer;
    }

// --------------------------------------------------------------------------
// CListBoxModel::FormatBufferL
// --------------------------------------------------------------------------
//
void CListBoxModel::FormatBufferL( TInt aIndex ) const
    {
    const TInt KDefaultListFormatting =
        MPbk2ContactNameFormatter::EUseSeparator |
        MPbk2ContactNameFormatter::EPreserveLeadingSpaces;

    const MVPbkViewContact& contact = iView.ContactAtL( aIndex );
    HBufC* name = iNameFormatter.GetContactTitleL(
        contact.Fields(),
        KDefaultListFormatting );
    iFormattingBuffer.Append( *name );
    delete name;
    }

// --------------------------------------------------------------------------
// CPopupList::CPopupList
// --------------------------------------------------------------------------
//
inline CPopupList::CPopupList()
    {
    }

// --------------------------------------------------------------------------
// CPopupList::NewLC
// --------------------------------------------------------------------------
//
CPopupList* CPopupList::NewLC( CEikListBox* aListBox, TInt aCbaResource )
    {
    CPopupList* self = new(ELeave) CPopupList;
    CleanupStack::PushL(self);
    self->CAknPopupList::ConstructL(
            aListBox,
            aCbaResource,
            AknPopupLayouts::EMenuWindow );
    return self;
    }

// --------------------------------------------------------------------------
// CPopupList::~CPopupList
// --------------------------------------------------------------------------
//
CPopupList::~CPopupList()
    {
    }

// --------------------------------------------------------------------------
// CPguGroupPopup::CPguGroupPopup
// --------------------------------------------------------------------------
//
CPguGroupPopup::CPguGroupPopup
        ( MVPbkContactLinkArray* aGroupsJoined, TInt32 aTitleResId,
          TInt32 aEmptyTextResId, TInt32 aSoftKeyResId,
          TInt32 aListBoxFlags ) :
            iTitleResId( aTitleResId ),
            iEmptyTextResId( aEmptyTextResId ),
            iSoftKeyResId( aSoftKeyResId ),
            iListBoxFlags( aListBoxFlags ),
            iGroupsJoined( aGroupsJoined )
    {
    }

// --------------------------------------------------------------------------
// CPguGroupPopup::ConstructL
// --------------------------------------------------------------------------
//
inline void CPguGroupPopup::ConstructL
        ( MVPbkContactViewBase& aAllGroupsView )
    {
    iGroupView = CVPbkFilteredContactView::NewL
        ( aAllGroupsView, *this, *this );
    }

// --------------------------------------------------------------------------
// CPguGroupPopup::NewL
// --------------------------------------------------------------------------
//
CPguGroupPopup* CPguGroupPopup::NewL
        ( MVPbkContactLinkArray* aGroupsJoined,
          TInt32 aTitleResId, TInt32 aEmptyTextResId,
          TInt32 aSoftKeyResId, TInt32 aListBoxFlags,
          MVPbkContactViewBase& aAllGroupsView )
    {
    CPguGroupPopup* self = new ( ELeave ) CPguGroupPopup
        ( aGroupsJoined, aTitleResId, aEmptyTextResId,
          aSoftKeyResId, aListBoxFlags );
    CleanupStack::PushL( self );
    self->ConstructL( aAllGroupsView );
    CleanupStack::Pop( self );
    return self;
    }

// --------------------------------------------------------------------------
// CPguGroupPopup::~CPguGroupPopup
// --------------------------------------------------------------------------
//
CPguGroupPopup::~CPguGroupPopup()
    {
    if ( iGroupView )
        {
        iGroupView->RemoveObserver( *this );
        }

    delete iGroupView;
    delete iInputAbsorber;
    delete iSelectedGroup;
    }

// --------------------------------------------------------------------------
// CPguGroupPopup::ExecuteLD
// --------------------------------------------------------------------------
//
MVPbkContactLink* CPguGroupPopup::ExecuteLD()
    {
    CleanupStack::PushL( this );

    iInputAbsorber = CPbk2InputAbsorber::NewL( R_AVKON_SOFTKEYS_CANCEL );
    iInputAbsorber->SetCommandObserver( this );

    iInputAbsorber->Wait();

    MVPbkContactLink* result = iSelectedGroup;
    iSelectedGroup = NULL;
    CleanupStack::PopAndDestroy( this );
    return result;
    }

// --------------------------------------------------------------------------
// CPguGroupPopup::ContactViewReady
// --------------------------------------------------------------------------
//
void CPguGroupPopup::ContactViewReady( MVPbkContactViewBase& /*aView*/ )
    {
    TRAPD( err, RunPopupL() );
    if ( err != KErrNone )
        {
        CCoeEnv::Static()->HandleError( err );
        Close();
        }
    }

// --------------------------------------------------------------------------
// CPguGroupPopup::ContactViewUnavailable
// --------------------------------------------------------------------------
//
void CPguGroupPopup::ContactViewUnavailable
        ( MVPbkContactViewBase& /*aView*/ )
    {
    //Do nothing
    }

// --------------------------------------------------------------------------
// CPguGroupPopup::ContactAddedToView
// --------------------------------------------------------------------------
//
void CPguGroupPopup::ContactAddedToView(
        MVPbkContactViewBase& /*aView*/,
        TInt /*aIndex*/,
        const MVPbkContactLink& /*aContactLink*/ )
    {
    //Do nothing
    }

// --------------------------------------------------------------------------
// CPguGroupPopup::ContactRemovedFromView
// --------------------------------------------------------------------------
//
void CPguGroupPopup::ContactRemovedFromView(
        MVPbkContactViewBase& /*aView*/,
        TInt /*aIndex*/,
        const MVPbkContactLink& /*aContactLink*/ )
    {
    //Do nothing
    }

// --------------------------------------------------------------------------
// CPguGroupPopup::ContactViewError
// --------------------------------------------------------------------------
//
void CPguGroupPopup::ContactViewError(
        MVPbkContactViewBase& /*aView*/,
        TInt /*aError*/,
        TBool /*aErrorNotified*/ )
    {
    //Do nothing
    }

// --------------------------------------------------------------------------
// CPguGroupPopup::ProcessCommandL
// --------------------------------------------------------------------------
//
void CPguGroupPopup::ProcessCommandL( TInt aCommandId )
    {
    switch ( aCommandId )
        {
        case EAknSoftkeyCancel: // FALLTHROUGH
        case EAknSoftkeyBack:   // FALLTHROUGH
        case EAknSoftkeyNo:     // FALLTHROUGH
        case EAknSoftkeyClose:  // FALLTHROUGH
        case EAknSoftkeyExit:
            {
            Close();
            break;
            }

        default:
            break;
        }
    }

// --------------------------------------------------------------------------
// CPguGroupPopup::Close
// --------------------------------------------------------------------------
//
void CPguGroupPopup::Close()
    {
    delete this;
    }

// --------------------------------------------------------------------------
// CPguGroupPopup::RunPopupL
// --------------------------------------------------------------------------
//
void CPguGroupPopup::RunPopupL()
    {
    CEikColumnListBox* listBox = static_cast<CEikColumnListBox*>(
        EikControlFactory::CreateByTypeL(
            EAknCtSinglePopupMenuListBox ).iControl );
    CleanupStack::PushL( listBox );

    CPopupList* popupList = CPopupList::NewLC( listBox, iSoftKeyResId );
    listBox->ConstructL( popupList,
        iListBoxFlags | CEikListBox::ELeftDownInViewRect );

    // Create own listbox model
    CListBoxModel* listBoxModel =
            new (ELeave) CListBoxModel( *iGroupView,
            Phonebook2::Pbk2AppUi()->ApplicationServices().NameFormatter() );

    listBox->Model()->SetOwnershipType( ELbmOwnsItemArray );
    listBox->Model()->SetItemTextArray( listBoxModel );
    listBox->CreateScrollBarFrameL( ETrue );
    listBox->ScrollBarFrame()->SetScrollBarVisibilityL(
            CEikScrollBarFrame::EOff,
            CEikScrollBarFrame::EAuto );

    HBufC* prompt = StringLoader::LoadLC( iTitleResId );
    popupList->SetTitleL( *prompt );
    CleanupStack::PopAndDestroy(); // prompt

    HBufC* empty = StringLoader::LoadLC( iEmptyTextResId );
    listBox->View()->SetListEmptyTextL( *empty );
    CleanupStack::PopAndDestroy(); // empty

    iResult = popupList->ExecuteLD();
    CleanupStack::Pop( popupList ); // destroyed in ExecuteLD
    if ( iResult )
        {
        TInt index = listBox->CurrentItemIndex();
        iSelectedGroup = iGroupView->ContactAtL( index ).CreateLinkLC();
        CleanupStack::Pop();
        }
    CleanupStack::PopAndDestroy( listBox );

    // Wait is started in ExecuteLD()
    iInputAbsorber->StopWait();
    }

// --------------------------------------------------------------------------
// CPguGroupPopup::IsContactIncluded
// --------------------------------------------------------------------------
//
TBool CPguGroupPopup::IsContactIncluded(
        const MVPbkBaseContact& aContact )
    {
    TBool result = EFalse;
    const TInt count = iGroupsJoined->Count();
    for ( TInt i(0); i < count; ++i )
        {
        if ( iGroupsJoined->At( i ).RefersTo( aContact ) )
            {
            result = ETrue;
            break;
            }
        }
    return result;
    }
//  End of File
