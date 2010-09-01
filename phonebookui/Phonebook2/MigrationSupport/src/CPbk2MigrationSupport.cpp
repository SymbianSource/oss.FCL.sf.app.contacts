/*
* Copyright (c) 2002-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Migration support for Phonebook 2.
*
*/


// INCLUDES
#include "CPbk2MigrationSupport.h"

// Phonebook 2
#include <CPbk2ViewState.h>

// Virtual Phonebook
#include <CVPbkContactIdConverter.h>
#include <CVPbkContactManager.h>
#include <TVPbkContactStoreUriPtr.h>
#include <MVPbkContactStoreList.h>
#include <CVPbkContactLinkArray.h>
#include <VPbkContactStoreUris.h>

// Phonebook 1
#include <CPbkViewState.h>

/// Unnamed namespace for local definitions
namespace {

#ifdef _DEBUG

enum TPanicCode
    {
    EPanicPreCond_ConstructL = 1,
    EPanicPreCond_NewL
    };

void Panic(TPanicCode aPanicCode)
    {
    _LIT(KPanicText, "CPbk2MigrationSupport");
    User::Panic(KPanicText, aPanicCode);
    };

#endif // _DEBUG

} /// namespace


// --------------------------------------------------------------------------
// CPbk2MigrationSupport::CPbk2MigrationSupport
// --------------------------------------------------------------------------
//
inline CPbk2MigrationSupport::CPbk2MigrationSupport
        ( CVPbkContactManager* aContactManager ) :
            iContactManager( *aContactManager )
    {
    }

// --------------------------------------------------------------------------
// CPbk2MigrationSupport::~CPbk2MigrationSupport
// --------------------------------------------------------------------------
//
CPbk2MigrationSupport::~CPbk2MigrationSupport()
    {
    delete iIdConverter;
    }

// --------------------------------------------------------------------------
// CPbk2MigrationSupport::NewL
// --------------------------------------------------------------------------
//
CPbk2MigrationSupport* CPbk2MigrationSupport::NewL( TAny* aParam )
    {
    __ASSERT_DEBUG( aParam, Panic( EPanicPreCond_NewL ) );

    CVPbkContactManager* contactManager =
        static_cast<CVPbkContactManager*>( aParam );

    CPbk2MigrationSupport* self =
        new ( ELeave ) CPbk2MigrationSupport( contactManager );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// --------------------------------------------------------------------------
// CPbk2MigrationSupport::ConstructL
// --------------------------------------------------------------------------
//
inline void CPbk2MigrationSupport::ConstructL()
    {
    // Find Contacts Model store URI from the contact manager
    TVPbkContactStoreUriPtr uri( VPbkContactStoreUris::DefaultCntDbUri() );
    MVPbkContactStore* defaultStore =
            iContactManager.ContactStoresL().Find( uri );
    User::LeaveIfNull( defaultStore );
    iIdConverter = CVPbkContactIdConverter::NewL( *defaultStore );
    }

// --------------------------------------------------------------------------
// CPbk2MigrationSupport::ConvertViewStateL
// --------------------------------------------------------------------------
//
void CPbk2MigrationSupport::ConvertViewStateL
        ( const TDesC8& aCustomMessage, CPbk2ViewState& aViewState )
    {
    CPbkViewState* viewState = CPbkViewState::NewLC( aCustomMessage );

    // 1. Convert a contact id to a contact link for focus
    // TContactItemId FocusedContactId()
    //      -> void SetFocusedContact( MVPbkContactLink* aContact )
    if ( viewState->FocusedContactId() != KNullContactId )
        {
        MVPbkContactLink* focusIdLink = iIdConverter->IdentifierToLinkLC(
                viewState->FocusedContactId() );
        aViewState.SetFocusedContact( focusIdLink );
        CleanupStack::Pop(); // focusIdLink
        }

    // 2. Convert a contact id to contact link for top focus
    // TContactItemId TopContactId()
    //      -> void SetTopContact( MVPbkContactLink* aTopContact )
    if ( viewState->TopContactId() != KNullContactId )
        {
        MVPbkContactLink* topIdLink = iIdConverter->IdentifierToLinkLC(
                viewState->TopContactId() );
        aViewState.SetFocusedContact( topIdLink );
        CleanupStack::Pop(); // topIdLink
        }

    // 3. Convert a contact id to contact link for setting parent contact
    // TContactItemId ParentContactId()
    //      -> void SetParentContact( MVPbkContactLink* aParentContact )
    if ( viewState->ParentContactId() != KNullContactId )
        {
        MVPbkContactLink* parentIdLink = iIdConverter->IdentifierToLinkLC(
                viewState->ParentContactId() );
        aViewState.SetFocusedContact( parentIdLink );
        CleanupStack::Pop(); // parentIdLink
        }

    // 4. Convert a contact id array to contact link array
    //    for marking contacts
    // CContactIdArray* MarkedContactIds()
    //      -> void SetMarkedContacts( MVPbkContactLinkArray* aArray )
    if (viewState->MarkedContactIds())
        {
        CVPbkContactLinkArray* linkArray = CVPbkContactLinkArray::NewLC();
        CContactIdArray* ids = viewState->MarkedContactIds();
        for( TInt i = 0; i < ids->Count(); ++i )
            {
            TContactItemId cid = ( *ids )[i];
            MVPbkContactLink* markedIdLink =
                iIdConverter->IdentifierToLinkLC( cid );
            linkArray->AppendL( markedIdLink );
            CleanupStack::Pop(); // markedIdLink
            }
        aViewState.SetMarkedContacts( linkArray );
        CleanupStack::Pop(); // linkArray
        }

    // 5. Convert a field index to a field index for focus
    // TInt FocusedFieldIndex()
    //      -> void SetFocusedFieldIndex( TInt aIndex )
    aViewState.SetFocusedFieldIndex( viewState->FocusedFieldIndex() );

    // 6. Convert a top index to a index for top focus
    // TInt TopFieldIndex()
    //      -> void SetTopFieldIndex( TInt aIndex )
    aViewState.SetTopFieldIndex( viewState->TopFieldIndex() );

    // 7. Convert flags to flags
    // TUint Flags()
    //      -> void SetFlags( TUint aFlags )
    aViewState.SetFlags( viewState->Flags() );

    CleanupStack::PopAndDestroy( viewState );
    }

// End of File
