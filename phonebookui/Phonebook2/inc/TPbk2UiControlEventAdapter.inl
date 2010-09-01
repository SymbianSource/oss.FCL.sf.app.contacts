/*
* Copyright (c) 2007-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Adapter between Pbk2 commands and command users.
*
*/


/// Unnamed namespace for local definitions
namespace {

#ifdef _DEBUG

enum TPanicCode
    {
    EPbk2AdapterPanic1 = 1,
    EPbk2AdapterPanic2,
    EPbk2AdapterPanic3,
    EPbk2AdapterPanic4,
    EPbk2AdapterPanic5,
    EPbk2AdapterPanic6,
    EPbk2AdapterPanic7,
    EPbk2AdapterPanic8,
    EPbk2AdapterPanic9,
    EPbk2AdapterPanic10,
    EPbk2AdapterPanic11,
    EPbk2AdapterPanic12,
    EPbk2AdapterPanic13,
    EPbk2AdapterPanic14,
    EPbk2AdapterPanic15,
    EPbk2AdapterPanic16,
    EPbk2AdapterPanic17,
    EPbk2AdapterPanic18,
    EPbk2AdapterPanic19,
    EPbk2AdapterPanic20,
    EPbk2AdapterPanic21,
    EPbk2AdapterPanic22,
    EPbk2AdapterPanic23,
    EPbk2AdapterPanic24,
    EPbk2AdapterPanic25,
    EPbk2AdapterPanic26,
    EPbk2AdapterPanic27,
    EPbk2AdapterPanic28,
    EPbk2AdapterPanic29
    };

void Panic(TPanicCode aReason)
    {
    _LIT( KPanicText, "CPbk2CtrlAdapter" );
    User::Panic( KPanicText, aReason );
    }

#endif // _DEBUG

    } // namespace

inline void TPbk2UiControlEventAdapter::SetContact( MVPbkStoreContact& aStoreContact )
    {
    iStoreContact = &aStoreContact;
    }

inline void TPbk2UiControlEventAdapter::SetObserver( MPbk2UiControlAdapterObserver & aObserver )
    {
    iObserver = &aObserver;
    }

inline MPbk2ContactUiControl* TPbk2UiControlEventAdapter::ParentControl() const
    {
    __ASSERT_DEBUG( EFalse, Panic( EPbk2AdapterPanic1 ) );
    return NULL;
    }

inline TInt TPbk2UiControlEventAdapter::NumberOfContacts() const
    {
    return iStoreContact ? 1 : 0; // If iStoreContact has been set, then one, otherwise zero.
    }

inline const MVPbkBaseContact* TPbk2UiControlEventAdapter::FocusedContactL() const
    {
    return iStoreContact;
    }

inline const MVPbkViewContact* TPbk2UiControlEventAdapter::FocusedViewContactL() const
    {
    __ASSERT_DEBUG( EFalse, Panic( EPbk2AdapterPanic2 ) );
    return NULL;
    }

inline const MVPbkStoreContact* TPbk2UiControlEventAdapter::FocusedStoreContact() const
    {
    return iStoreContact;
    }

inline void TPbk2UiControlEventAdapter::SetFocusedContactL(
        const MVPbkBaseContact& aContact )
    {
    if ( iObserver )
        {
        iObserver->ContactContextChanged( aContact );   
        }
    }

inline void TPbk2UiControlEventAdapter::SetFocusedContactL(
        const MVPbkContactBookmark& /*aContactBookmark*/ )
    {
    __ASSERT_DEBUG( EFalse, Panic( EPbk2AdapterPanic3 ) );
    }

inline void TPbk2UiControlEventAdapter::SetFocusedContactL(
        const MVPbkContactLink& /*aContactLink*/ )
    {
    __ASSERT_DEBUG( EFalse, Panic( EPbk2AdapterPanic4 ) );
    }

inline TInt TPbk2UiControlEventAdapter::FocusedContactIndex() const
    {
    __ASSERT_DEBUG( EFalse, Panic( EPbk2AdapterPanic5 ) );
    return 0;
    }

inline void TPbk2UiControlEventAdapter::SetFocusedContactIndexL(
        TInt /*aIndex*/ )
    {
    __ASSERT_DEBUG( EFalse, Panic( EPbk2AdapterPanic1 ) );
    }

inline TInt TPbk2UiControlEventAdapter::NumberOfContactFields() const
    {
    __ASSERT_DEBUG( EFalse, Panic( EPbk2AdapterPanic6 ) );
    return 0;
    }

inline const MVPbkBaseContactField* TPbk2UiControlEventAdapter::FocusedField() const
    {
    __ASSERT_DEBUG( EFalse, Panic( EPbk2AdapterPanic7 ) );
    return NULL;
    }

inline TInt TPbk2UiControlEventAdapter::FocusedFieldIndex() const
    {
    __ASSERT_DEBUG( EFalse, Panic( EPbk2AdapterPanic8 ) );
    return 0;
    }

inline void TPbk2UiControlEventAdapter::SetFocusedFieldIndex(
        TInt /*aIndex*/ )
    {
    __ASSERT_DEBUG( EFalse, Panic( EPbk2AdapterPanic9 ) );
    }

inline TBool TPbk2UiControlEventAdapter::ContactsMarked() const
    {
    //TODO
    return EFalse;
    }

inline MVPbkContactLinkArray* TPbk2UiControlEventAdapter::SelectedContactsL() const
    {
    //TODO
    return NULL;
    }

inline MVPbkContactLinkArray*
TPbk2UiControlEventAdapter::SelectedContactsOrFocusedContactL() const
    {
    //TODO THIS FUNCTION IS IMPORTANT!!!
    return NULL;
    }

inline MPbk2ContactLinkIterator*
TPbk2UiControlEventAdapter::SelectedContactsIteratorL() const
    {
    //TODO
    //TODO THIS FUNCTION IS IMPORTANT FOR CERTAIN COMMANDS, BUT NOT FOR DEFAULTS.
    return NULL;
    }

inline 
CArrayPtr<MVPbkContactStore>* TPbk2UiControlEventAdapter::SelectedContactStoresL() const
    {
    __ASSERT_DEBUG( EFalse, Panic( EPbk2AdapterPanic10 ) );
    return NULL; //TODO
    }

inline void TPbk2UiControlEventAdapter::ClearMarks()
    {
    __ASSERT_DEBUG( EFalse, Panic( EPbk2AdapterPanic11 ) );
    }

inline void TPbk2UiControlEventAdapter::SetSelectedContactL(
        TInt /*aIndex*/,
        TBool /*aSelected*/ )
    {
    __ASSERT_DEBUG( EFalse, Panic( EPbk2AdapterPanic12 ) );
    }

inline TInt TPbk2UiControlEventAdapter::CommandItemCount() const
    {
    __ASSERT_DEBUG( EFalse, Panic( EPbk2AdapterPanic13 ) );
    return 0;
    }

inline const MPbk2UiControlCmdItem& TPbk2UiControlEventAdapter::CommandItemAt( TInt /*aIndex*/ ) const
    {
    __ASSERT_DEBUG( EFalse, Panic( EPbk2AdapterPanic14 ) );
    MPbk2UiControlCmdItem* dummyItem = NULL;
    return *dummyItem;
    }

inline const MPbk2UiControlCmdItem* TPbk2UiControlEventAdapter::FocusedCommandItem() const
    {
    __ASSERT_DEBUG( EFalse, Panic( EPbk2AdapterPanic15 ) );
    const MPbk2UiControlCmdItem* dummyItem = NULL;
    return dummyItem;
    }

inline void TPbk2UiControlEventAdapter::DeleteCommandItemL( TInt /*aIndex*/ )
    {
    __ASSERT_DEBUG( EFalse, Panic( EPbk2AdapterPanic16 ) );
    }

inline void TPbk2UiControlEventAdapter::AddCommandItemL(MPbk2UiControlCmdItem* /*aCommand*/, TInt /*aIndex*/)
    {
    __ASSERT_DEBUG( EFalse, Panic( EPbk2AdapterPanic17 ) );
    }

inline void TPbk2UiControlEventAdapter::SetSelectedContactL(
        const MVPbkContactBookmark& /*aContactBookmark*/,
        TBool /*aSelected*/ )
    {
    __ASSERT_DEBUG( EFalse, Panic( EPbk2AdapterPanic18 ) );
    }

inline void TPbk2UiControlEventAdapter::SetSelectedContactL(
        const MVPbkContactLink& /*aContactLink*/,
        TBool /*aSelected*/ )
    {
    __ASSERT_DEBUG( EFalse, Panic( EPbk2AdapterPanic19 ) );
    }

inline void TPbk2UiControlEventAdapter::DynInitMenuPaneL(
        TInt /*aResourceId*/,
        CEikMenuPane* /*aMenuPane*/ ) const
    {
    __ASSERT_DEBUG( EFalse, Panic( EPbk2AdapterPanic20 ) );
    }

inline void TPbk2UiControlEventAdapter::ProcessCommandL(
        TInt /*aCommandId*/ ) const
    {
    }

inline void TPbk2UiControlEventAdapter::UpdateAfterCommandExecution()
    {
    if ( iObserver )
        {
        iObserver->Pbk2CommandFinished();
        }
    }

inline TInt TPbk2UiControlEventAdapter::GetMenuFilteringFlagsL() const
    {
    __ASSERT_DEBUG( EFalse, Panic( EPbk2AdapterPanic21 ) );
    return 0;
    }

inline CPbk2ViewState* TPbk2UiControlEventAdapter::ControlStateL() const
    {
    __ASSERT_DEBUG( EFalse, Panic( EPbk2AdapterPanic22 ) );
    return NULL;
    }

inline void TPbk2UiControlEventAdapter::RestoreControlStateL(
        CPbk2ViewState* /*aState*/ )
    {
    __ASSERT_DEBUG( EFalse, Panic( EPbk2AdapterPanic23 ) );
    }

inline const TDesC& TPbk2UiControlEventAdapter::FindTextL()
    {
    __ASSERT_DEBUG( EFalse, Panic( EPbk2AdapterPanic24 ) );
    return KNullDesC;
    }

inline void TPbk2UiControlEventAdapter::ResetFindL()
    {
    __ASSERT_DEBUG( EFalse, Panic( EPbk2AdapterPanic25 ) );
    }

inline void TPbk2UiControlEventAdapter::HideThumbnail()
    {
    __ASSERT_DEBUG( EFalse, Panic( EPbk2AdapterPanic26 ) );
    }

inline void TPbk2UiControlEventAdapter::ShowThumbnail()
    {
    __ASSERT_DEBUG( EFalse, Panic( EPbk2AdapterPanic27 ) );
    }

inline void TPbk2UiControlEventAdapter::SetBlank(
        TBool /*aBlank*/ )
    {
    __ASSERT_DEBUG( EFalse, Panic( EPbk2AdapterPanic28 ) );
    }

inline void TPbk2UiControlEventAdapter::RegisterCommand(
        MPbk2Command* /*aCommand*/ )
    {
    }

inline void TPbk2UiControlEventAdapter::SetTextL(
        const TDesC& /*aText*/ )
    {
    __ASSERT_DEBUG( EFalse, Panic( EPbk2AdapterPanic29 ) );
    }

//  End of File  
