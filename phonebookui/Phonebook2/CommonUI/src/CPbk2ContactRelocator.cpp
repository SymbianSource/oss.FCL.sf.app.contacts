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
* Description:  Contacts relocator.
*
*/



// INCLUDE FILES
#include <CPbk2ContactRelocator.h>

// Phonebook 2
#include <CPbk2StoreConfiguration.h>
#include <MPbk2ApplicationServices.h>
#include <Phonebook2PrivateCRKeys.h>
#include <MPbk2ContactRelocatorObserver.h>
#include <Pbk2CommonUi.rsg>
#include <CPbk2StoreProperty.h>
#include <CPbk2StorePropertyArray.h>
#include <MPbk2ContactNameFormatter.h>
#include <TPbk2DestructionIndicator.h>

// Virtual Phonebook
#include <MVPbkStoreContact.h>
#include <MVPbkContactStore.h>
#include <MVPbkContactStoreList.h>
#include <MVPbkContactStoreProperties.h>
#include <VPbkContactStoreUris.h>
#include <CVPbkContactStoreUriArray.h>
#include <CVPbkContactManager.h>
#include <MVPbkContactOperationBase.h>
#include <MVPbkContactLink.h>
#include <TVPbkContactStoreUriPtr.h>
#include <CVPbkContactLinkArray.h>
#include <CPbk2ApplicationServices.h>
#include <MVPbkContactCopyPolicy.h>
#include <CVPbkContactCopyPolicyManager.h>

// System includes
#include <centralrepository.h>
#include <StringLoader.h>
#include <AknQueryDialog.h>

/// Unnamed namespace for local definitions
namespace {

// LOCAL CONSTANTS AND MACROS

const TInt KFirstElement = 0;
const TInt KOneContact = 1;
const TInt KDefaultTitleFormat = MPbk2ContactNameFormatter::EUseSeparator;

#ifdef _DEBUG

// Definition for many contacts relocation
const TInt KLinkArrayRelocationMinCount = 2;

enum TPanicCode
    {
    EPanicPreCond_ContactsSaved,
    EPanicLogic_AskConfirmationL,
    EPanicPreCond_RelocateContactL,
    EPanicPreCond_RelocateContactsL,
    EPanicPreCond_RelocateAndLockContactL,
    EPanicPreCond_PrepareToRelocateContactL,
    EPanicPreCond_DeleteSourceContact,
    EPanicPreCond_VerifyRelocationQueryType
    };

void Panic( TInt aReason )
    {
    _LIT( KPanicText, "CPbk2ContactRelocator" );
    User::Panic( KPanicText, aReason );
    }

#endif // _DEBUG

/**
 * Fetches contact's store name, which can be use in relocation queries.
 * If store name cannot be fetched from global store properties, then
 * contact's store property is returned.
 *
 * @param aContact Reference to contact.
 * @param aStoreProperties Reference to store properties.
 * @return Pointer to store name. Ownership is not transferred
 */
const TDesC* StoreName(
        const MVPbkStoreContact* aContact,
        CPbk2StorePropertyArray& aStoreProperties )
    {
    const TDesC* storeName = NULL;
    if ( aContact )
        {
        // Fetch store name
        TVPbkContactStoreUriPtr uri =
            aContact->ParentStore().StoreProperties().Uri();
        const CPbk2StoreProperty* storeProperty =
            aStoreProperties.FindProperty( uri );

        if (storeProperty)
            {
            storeName = &storeProperty->StoreName();
            }
        else
            {
            storeName = &uri.UriDes();
            }        
        }
    return storeName;
    }

} // namespace


// --------------------------------------------------------------------------
// CPbk2ContactRelocator::CPbk2ContactRelocator
// --------------------------------------------------------------------------
//
CPbk2ContactRelocator::CPbk2ContactRelocator() :
        CActive( CActive::EPriorityStandard )
    {
    CActiveScheduler::Add( this );
    }

// --------------------------------------------------------------------------
// CPbk2ContactRelocator::~CPbk2ContactRelocator
// --------------------------------------------------------------------------
//
CPbk2ContactRelocator::~CPbk2ContactRelocator()
    {
    Cancel();
    delete iSourceContact;
    delete iSourceContacts;
    delete iRelocatedContact;
    delete iContactRetriever;
    delete iSavedContactsRetriever;
    delete iContactCopier;
    delete iCopyPolicyManager;
    delete iQueryDialog;

    if ( iTargetStore )
        {
        iTargetStore->Close( *this );
        }

    if ( iDestroyedPtr )
        {
        *iDestroyedPtr = ETrue;
        }
    Release( iAppServices ); 
    }

// --------------------------------------------------------------------------
// CPbk2ContactRelocator::NewL
// --------------------------------------------------------------------------
//
EXPORT_C CPbk2ContactRelocator* CPbk2ContactRelocator::NewL()
    {
    CPbk2ContactRelocator* self =
        new ( ELeave ) CPbk2ContactRelocator();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// --------------------------------------------------------------------------
// CPbk2ContactRelocator::ConstructL
// --------------------------------------------------------------------------
//
inline void CPbk2ContactRelocator::ConstructL()
    {
    iAppServices = CPbk2ApplicationServices::InstanceL();
    iRelocationPolicy = ReadRelocationPolicyL();
    }

// --------------------------------------------------------------------------
// CPbk2ContactRelocator::RelocateContactL
// --------------------------------------------------------------------------
//
EXPORT_C TBool CPbk2ContactRelocator::RelocateContactL(
        MVPbkStoreContact* aContact,
        MPbk2ContactRelocatorObserver& aObserver,
        Pbk2ContactRelocator::TPbk2ContactRelocationQueryPolicy aQueryPolicy,
        TUint32 aFlags )
    {
    __ASSERT_DEBUG( aContact, Panic( EPanicPreCond_RelocateContactL ) );

    // Adjust policy
    if ( iRelocationPolicy == Pbk2ContactRelocator::EPbk2CopyContactToPhoneMemoryAndLock )
        {
        iRelocationPolicy = Pbk2ContactRelocator::EPbk2CopyContactToPhoneMemory;
        }
    else if ( iRelocationPolicy == Pbk2ContactRelocator::EPbk2MoveContactToPhoneMemoryAndLock )
        {
        iRelocationPolicy = Pbk2ContactRelocator::EPbk2MoveContactToPhoneMemory;
        }

    return PrepareToRelocateContactL( aContact, NULL, aObserver,
        aQueryPolicy, aFlags );
    }

// --------------------------------------------------------------------------
// CPbk2ContactRelocator::RelocateAndLockContactL
// --------------------------------------------------------------------------
//
EXPORT_C TBool CPbk2ContactRelocator::RelocateAndLockContactL(
        MVPbkStoreContact* aContact,
        MPbk2ContactRelocatorObserver& aObserver,
        Pbk2ContactRelocator::TPbk2ContactRelocationQueryPolicy aQueryPolicy,
        TUint32 aFlags )
    {
    __ASSERT_DEBUG( aContact, Panic( EPanicPreCond_RelocateAndLockContactL ) );

    // Adjust policy
    if ( iRelocationPolicy == Pbk2ContactRelocator::EPbk2CopyContactToPhoneMemory )
        {
        iRelocationPolicy = Pbk2ContactRelocator::EPbk2CopyContactToPhoneMemoryAndLock;
        }
    else if ( iRelocationPolicy == Pbk2ContactRelocator::EPbk2MoveContactToPhoneMemory )
        {
        iRelocationPolicy = Pbk2ContactRelocator::EPbk2MoveContactToPhoneMemoryAndLock;
        }

    return PrepareToRelocateContactL( aContact, NULL, aObserver,
        aQueryPolicy, aFlags );
    }

// --------------------------------------------------------------------------
// CPbk2ContactRelocator::RelocateContactsL
// --------------------------------------------------------------------------
//
EXPORT_C TBool CPbk2ContactRelocator::RelocateContactsL(
        CVPbkContactLinkArray* aContacts,
        MPbk2ContactRelocatorObserver& aObserver,
        Pbk2ContactRelocator::TPbk2ContactRelocationQueryPolicy aQueryPolicy )
    {
    __ASSERT_DEBUG( aContacts, Panic(EPanicPreCond_RelocateContactsL ) );

    return PrepareToRelocateContactL( NULL, aContacts, aObserver,
        aQueryPolicy, EPbk2RelocatorExistingContact );
    }

// --------------------------------------------------------------------------
// CPbk2ContactRelocator::IsPhoneMemoryContact
// --------------------------------------------------------------------------
//
EXPORT_C TBool CPbk2ContactRelocator::IsPhoneMemoryContact(
        const MVPbkStoreContact& aContact) const
    {
    TBool ret = EFalse;

    TVPbkContactStoreUriPtr uri =
        aContact.ParentStore().StoreProperties().Uri();

    TVPbkContactStoreUriPtr phoneMemoryUri
        ( VPbkContactStoreUris::DefaultCntDbUri() );

    if (uri.Compare( phoneMemoryUri,
        TVPbkContactStoreUriPtr::EContactStoreUriStoreType ) == 0)
        {
        ret = ETrue;
        }

    return ret;
    }

// --------------------------------------------------------------------------
// CPbk2ContactRelocator::IsPhoneMemoryInConfigurationL
// --------------------------------------------------------------------------
//
EXPORT_C TBool CPbk2ContactRelocator::IsPhoneMemoryInConfigurationL() const
    {
    TBool ret = EFalse;

    // Get current configuration
    CPbk2StoreConfiguration& storeConfig = iAppServices->StoreConfiguration();

    CVPbkContactStoreUriArray* uriArray = storeConfig.CurrentConfigurationL();

    // Check is phone memory included
    ret = uriArray->IsIncluded( VPbkContactStoreUris::DefaultCntDbUri() );
    delete uriArray;

    return ret;
    }

// --------------------------------------------------------------------------
// CPbk2ContactRelocator::RunL
// --------------------------------------------------------------------------
//
void CPbk2ContactRelocator::RunL()
    {
    // This methods gets called only when the relocation was cancelled or
    // otherwise stopped at the very beginning. The observer is needed to
    // be called back asynchronously, so the active object is set running.
    if ( iSourceContact )
        {
        MVPbkStoreContact* sourceContact = iSourceContact;
        iSourceContact = NULL;
        iObserver->ContactRelocationFailed( iErrorCode, sourceContact );
        }
    else if ( iSourceContacts )
        {
        CVPbkContactLinkArray* sourceContacts = iSourceContacts;
        iSourceContacts = NULL;
        iObserver->ContactsRelocationFailed( iErrorCode, sourceContacts );
        }
    }

// --------------------------------------------------------------------------
// CPbk2ContactRelocator::DoCancel
// --------------------------------------------------------------------------
//
void CPbk2ContactRelocator::DoCancel()
    {
    // Do nothing
    }

// --------------------------------------------------------------------------
// CPbk2ContactRelocator::RunError
// --------------------------------------------------------------------------
//
TInt CPbk2ContactRelocator::RunError( TInt /*aError*/ )
    {
    // No leaving code in RunL
    return KErrNone;
    }

// --------------------------------------------------------------------------
// CPbk2ContactRelocator::StoreReady
// Phone memory store was succesfully opened.
// --------------------------------------------------------------------------
//
void CPbk2ContactRelocator::StoreReady
        ( MVPbkContactStore& /*aContactStore*/ )
    {
    iTargetStoreOpen = ETrue;
    // Safe to ignore error
    TRAPD( err, DoRelocateContactL() );
    if ( err == KErrDiskFull )
        {
        IssueRequest();
        }
    }

// --------------------------------------------------------------------------
// CPbk2ContactRelocator::IssueRequest
// Inform failure asynchronously
// --------------------------------------------------------------------------
//
void CPbk2ContactRelocator::IssueRequest()
    {
    TRequestStatus* status = &iStatus;
    User::RequestComplete( status, KErrNone );
    SetActive();
    }

// --------------------------------------------------------------------------
// CPbk2ContactRelocator::StoreUnavailable
// --------------------------------------------------------------------------
//
void CPbk2ContactRelocator::StoreUnavailable
        ( MVPbkContactStore& /*aContactStore*/, TInt aReason )
    {
    // Problem in opening phone memory store
    iObserver->ContactRelocationFailed( aReason, iSourceContact );
    iSourceContact = NULL; // ownership was given away
    }

// --------------------------------------------------------------------------
// CPbk2ContactRelocator::HandleStoreEventL
// --------------------------------------------------------------------------
//
void CPbk2ContactRelocator::HandleStoreEventL
        ( MVPbkContactStore& /*aContactStore*/,
        TVPbkContactStoreEvent /*aStoreEvent*/ )
    {
    // Do nothing
    }

// --------------------------------------------------------------------------
// CPbk2ContactRelocator::FieldAddedToContact
// --------------------------------------------------------------------------
//
void CPbk2ContactRelocator::FieldAddedToContact
        ( MVPbkContactOperationBase& /*aOperation*/ )
    {
    // Do nothing
    }

// --------------------------------------------------------------------------
// CPbk2ContactRelocator::FieldAddingFailed
// --------------------------------------------------------------------------
//
void CPbk2ContactRelocator::FieldAddingFailed
        ( MVPbkContactOperationBase& /*aOperation*/, TInt /*aError*/ )
    {
    // Do nothing
    }

// --------------------------------------------------------------------------
// CPbk2ContactRelocator::ContactsSaved
// Contact was succesfully saved to phone memory
// --------------------------------------------------------------------------
//
void CPbk2ContactRelocator::ContactsSaved
        ( MVPbkContactOperationBase& /*aOperation*/,
        MVPbkContactLinkArray* aResults )
    {
    // There should be only one link in the results array
    __ASSERT_DEBUG( aResults && aResults->Count() == KOneContact,
        Panic( EPanicPreCond_ContactsSaved ) );

    TRAPD( err,
        {
        CleanupDeletePushL( aResults );

        // Next, retrieve the saved contact
        RetrieveSavedContactL( aResults->At( KFirstElement ) );
        CleanupStack::PopAndDestroy(); // aResults
        });

    if ( err != KErrNone )
        {
        iObserver->ContactRelocationFailed( err, iSourceContact );
        iSourceContact = NULL; // ownership was given away
        }

    }

// --------------------------------------------------------------------------
// CPbk2ContactRelocator::ContactsSavingFailed
// --------------------------------------------------------------------------
//
void CPbk2ContactRelocator::ContactsSavingFailed
        ( MVPbkContactOperationBase& /*aOperation*/, TInt aError )
    {
    // Contact was not succesfully saved to phone memory
    iObserver->ContactRelocationFailed( aError, iSourceContact );
    iSourceContact = NULL; // ownership was given away

    // Move to next request, if any. Safe to ignore error.
    TRAP_IGNORE( DoRelocateContactL() );
    }

// --------------------------------------------------------------------------
// CPbk2ContactRelocator::VPbkSingleContactOperationComplete
// Contact was succesfully retrieved from a link.
// --------------------------------------------------------------------------
//
void CPbk2ContactRelocator::VPbkSingleContactOperationComplete
        ( MVPbkContactOperationBase& aOperation,
        MVPbkStoreContact* aContact )
    {
    if ( &aOperation == iSavedContactsRetriever )
        {
        // Destroy the last relocated contact
        delete iRelocatedContact;
        iRelocatedContact = NULL;
        // Assign a new relocated contact
        iRelocatedContact = aContact;
        if ( ( iActivePolicy == Pbk2ContactRelocator::EPbk2CopyContactToPhoneMemoryAndLock ) ||
            ( iActivePolicy == Pbk2ContactRelocator::EPbk2MoveContactToPhoneMemoryAndLock ) )
            {
            // Lock the contact
            TRAPD( err, iRelocatedContact->LockL( *this ) );
            if ( err != KErrNone )
                {
                iObserver->ContactRelocationFailed( err, iSourceContact );
                iSourceContact = NULL; // ownership was given away

                // Move to next request, if any. Safe to ignore error.
                TRAP_IGNORE( DoRelocateContactL() );
                }
            }
        else if ( iActivePolicy == Pbk2ContactRelocator::EPbk2MoveContactToPhoneMemory )
            {
            DeleteSourceContact();
            }
        else // EPbk2CopyContactToPhoneMemory
            {
            DoHandleContactRelocated();
            TRAP_IGNORE( DoRelocateContactL() );
            FinalizeIfReady();
            }
        }
    else if ( &aOperation == iContactRetriever )
        {
        // One contact from given link array was retrieved, now it has
        // to be stored and then relocated
        iSourceContact = aContact;
        DoRelocateContactL();
        }
    }

// --------------------------------------------------------------------------
// CPbk2ContactRelocator::VPbkSingleContactOperationFailed
// Contact was not succesfully retrieved from a link.
// --------------------------------------------------------------------------
//
void CPbk2ContactRelocator::VPbkSingleContactOperationFailed
        ( MVPbkContactOperationBase& /*aOperation*/, TInt aError )
    {
    if ( aError != KErrDiskFull )
        {
        iObserver->ContactRelocationFailed( aError, iSourceContact );
        iSourceContact = NULL; // ownership was given away

        // Move to next request, if any
        TRAP_IGNORE( DoRelocateContactL() );
        }
    else
        {
        IssueRequest();
        }
    }

// --------------------------------------------------------------------------
// CPbk2ContactRelocator::ContactOperationCompleted
// --------------------------------------------------------------------------
//
void CPbk2ContactRelocator::ContactOperationCompleted
        ( TContactOpResult aResult )
    {
    if ( aResult.iOpCode == MVPbkContactObserver::EContactLock )
        {
        if ( iActivePolicy == Pbk2ContactRelocator::EPbk2MoveContactToPhoneMemoryAndLock )
            {
            // One step to -> delete the original contact
            DeleteSourceContact();
            }
        else
            {
            // Relocation is done with current contact
            DoHandleContactRelocated();
            // Continue if there are still contacts to relocate
            TRAP_IGNORE( DoRelocateContactL() );
            FinalizeIfReady();
            }
        }
    else if ( aResult.iOpCode == MVPbkContactObserver::EContactDelete )
        {
        // The original contact was succesfully deleted.
        // Notify has already been called, so move to next request, if any.
        // In practise:
        DoHandleContactRelocated();
        TRAP_IGNORE( DoRelocateContactL() );
        FinalizeIfReady();
        }
    }

// --------------------------------------------------------------------------
// CPbk2ContactRelocator::ContactOperationFailed
// --------------------------------------------------------------------------
//
void CPbk2ContactRelocator::ContactOperationFailed
        ( TContactOp /*aOpCode*/, TInt aErrorCode,
        TBool /*aErrorNotified*/ )
    {
    // Request of the deletion of the original contact
    // was not succesfully completed
    // Give ownership of the failed contact to the observer
    iObserver->ContactRelocationFailed( aErrorCode, iSourceContact );
    iSourceContact = NULL;

    // Move to next request, if any
    TRAP_IGNORE( DoRelocateContactL() );
    }

// --------------------------------------------------------------------------
// CPbk2ContactRelocator::ReadRelocationPolicyL
// --------------------------------------------------------------------------
//
Pbk2ContactRelocator::TPbk2ContactRelocationPolicy
        CPbk2ContactRelocator::ReadRelocationPolicyL()
    {
    // Default to copy option
    Pbk2ContactRelocator::TPbk2ContactRelocationPolicy ret = Pbk2ContactRelocator::EPbk2CopyContactToPhoneMemory;
    TInt result = 0;

    // Get real setting from central repository
    CRepository* repository = CRepository::NewL
        ( TUid::Uid( KCRUidPhonebookInternalConfig ) );
    CleanupStack::PushL( repository );
    TInt err = repository->Get( KPhonebookContactRelocationPolicy, result );
    if ( err == KErrNone )
        {
        ret  = Pbk2ContactRelocator::TPbk2ContactRelocationPolicy( result );
        }

    CleanupStack::PopAndDestroy( repository );
    return ret;
    }

// --------------------------------------------------------------------------
// CPbk2ContactRelocator::OpenTargetStoreL
// --------------------------------------------------------------------------
//
void CPbk2ContactRelocator::OpenTargetStoreL()
    {
    if ( !iTargetStore )
        {
        const TVPbkContactStoreUriPtr phoneMemoryUri =
            VPbkContactStoreUris::DefaultCntDbUri();
        CVPbkContactManager& contactManager =
            iAppServices->ContactManager();
        contactManager.LoadContactStoreL( phoneMemoryUri );

        MVPbkContactStoreList& storeList = contactManager.ContactStoresL();

        iTargetStore = storeList.Find( phoneMemoryUri );
        User::LeaveIfNull( iTargetStore );
        iTargetStore->OpenL( *this );
        }
    }

// --------------------------------------------------------------------------
// CPbk2ContactRelocator::DoRelocateContactL
// --------------------------------------------------------------------------
//
void CPbk2ContactRelocator::DoRelocateContactL()
    {
    // No previous multirelocate request exists,
    // cleanup the previous request
    if ( iSourceContacts && iSourceContacts->Count() < KOneContact )
        {
        // All contacts were sent to relocation, its important
        // to clean the array and set it NULL
        delete iSourceContacts;
        iSourceContacts = NULL;
        }

    if ( iSourceContact )
        {
        // Start the copy operation
        CopyContactL( *iSourceContact );
        }
    else if ( iSourceContacts )
        {
        // Since we can not use batch copy operation (it does not give
        // copied contact links back) we have to retrieve all the contacts
        // and relocate them one by one
        if ( iSourceContacts->Count() >= KOneContact )
            {
            // Take the first contact link of the array
            MVPbkContactLink* contactLink =
                iSourceContacts->At( KFirstElement ).CloneLC();
            // Then remove it from the array
            iSourceContacts->Delete( KFirstElement );

            // Then retrieve it
            delete iContactRetriever;
            iContactRetriever = NULL;
            iContactRetriever =
                iAppServices->ContactManager().
                    RetrieveContactL( *contactLink, *this );
            CleanupStack::PopAndDestroy(); // contactLink
            }
        }
    }

// --------------------------------------------------------------------------
// CPbk2ContactRelocator::InitCopyPolicyL
// --------------------------------------------------------------------------
//
inline void CPbk2ContactRelocator::InitCopyPolicyL()
    {
    if (!iCopyPolicy)
        {
        // There is no copy policy manager if we are in this method
        // so creating a new one is always ok
        delete iCopyPolicyManager;
        iCopyPolicyManager = NULL;
        iCopyPolicyManager = CVPbkContactCopyPolicyManager::NewL();
        iCopyPolicy = iCopyPolicyManager->GetPolicyL
            ( iAppServices->ContactManager(),
            VPbkContactStoreUris::DefaultCntDbUri());
        }
    }

// --------------------------------------------------------------------------
// CPbk2ContactRelocator::DoHandleContactRelocated
// --------------------------------------------------------------------------
//
void CPbk2ContactRelocator::DoHandleContactRelocated()
    {
    if ( iRelocatedContact )
        {
        MVPbkStoreContact* relocatedContact = iRelocatedContact;
        iRelocatedContact = NULL;
        // Give ownership of the relocated contact to client.
        TRAPD( res, iObserver->ContactRelocatedL( relocatedContact ) );
        if ( res != KErrNone )
            {
            MVPbkStoreContact* sourceContact = iSourceContact;
            iSourceContact = NULL;
            // Give source contact as documented in observer. Client
            // takes the ownership
            iObserver->ContactRelocationFailed( res, sourceContact );
            }

        delete iSourceContact;
        iSourceContact = NULL;
        }
    }

// --------------------------------------------------------------------------
// CPbk2ContactRelocator::DeleteSourceContact
// --------------------------------------------------------------------------
//
void CPbk2ContactRelocator::DeleteSourceContact()
    {
    // Source contact is deleted only if policy is MOVE
    __ASSERT_DEBUG( iActivePolicy == Pbk2ContactRelocator::EPbk2MoveContactToPhoneMemory ||
        iActivePolicy == Pbk2ContactRelocator::EPbk2MoveContactToPhoneMemoryAndLock,
        Panic( EPanicPreCond_DeleteSourceContact ) );

    // Delete original contact
    if ( iSourceContact )
        {
        TRAPD( err, iSourceContact->DeleteL( *this ) );
        if ( err != KErrNone )
            {
            MVPbkStoreContact* sourceContact = iSourceContact;
            iSourceContact = NULL;
            // Give source contact as documented in observer. Client
            // takes the ownership
            iObserver->ContactRelocationFailed( err, sourceContact );
            if ( !iSourceContacts )
                {
                // If no more contacts to relocate then complete the relocation
                iObserver->RelocationProcessComplete();
                }
            }
        }
    }

// --------------------------------------------------------------------------
// CPbk2ContactRelocator::AskConfirmationL
// --------------------------------------------------------------------------
//
TBool CPbk2ContactRelocator::AskConfirmationL(
        MVPbkStoreContact* aContact,
        CVPbkContactLinkArray* aContacts,
        TPbk2RelocationQueryType aRelocationQueryType )
    {
    TBool ret = ETrue;

    HBufC* prompt = NULL;
    HBufC* nameBuffer = NULL;
    const TDesC* storeName = 
        StoreName( aContact,
            iAppServices->StoreProperties() );

    if ( aContact )
        {
        nameBuffer = 
                iAppServices->NameFormatter().
                    GetContactTitleL( aContact->Fields(), KDefaultTitleFormat );

        CleanupStack::PushL( nameBuffer );    
        }   
                    
    switch( aRelocationQueryType )
        {
        case EPbk2NoRelocationQuery:
        //don't show the following queries anymore
        case EPbk2CopyOneToStoreQuery:
        case EPbk2MoveOneToStoreQuery:
        case EPbk2CopyOneToPhoneQuery:
        case EPbk2MoveOneToPhoneQuery:
        case EPbk2CopyManyToStoreQuery:
        case EPbk2MoveManyToStoreQuery:
            {
            // Do nothing
            }
            break;
        default:
            {
            __ASSERT_DEBUG( EFalse,
                Panic( EPanicLogic_AskConfirmationL ) );
            break;
            }            
        }
    
    if ( prompt )
        {
        CleanupStack::PushL( prompt );
        delete iQueryDialog;
        iQueryDialog = NULL;
        iQueryDialog = CAknQueryDialog::NewL();
        iQueryDialog->SetPromptL( *prompt );

        CleanupStack::PopAndDestroy( prompt );

        ret = iQueryDialog->ExecuteLD( R_PBK2_COMMONUI_CONFIRMATION_QUERY );
        iQueryDialog = NULL;
        }

    if ( aContact )
        {
        CleanupStack::PopAndDestroy(); // nameBuffer    
        }
    return ret;    
    }

// --------------------------------------------------------------------------
// CPbk2ContactRelocator::RetrieveSavedContactL
// --------------------------------------------------------------------------
//
void CPbk2ContactRelocator::RetrieveSavedContactL
        ( const MVPbkContactLink& aLink )
    {
    delete iSavedContactsRetriever;
    iSavedContactsRetriever = NULL;
    iSavedContactsRetriever =
        iAppServices->ContactManager().
            RetrieveContactL( aLink, *this );
    }

// --------------------------------------------------------------------------
// CPbk2ContactRelocator::VerifyPolicy
// --------------------------------------------------------------------------
//
inline void CPbk2ContactRelocator::VerifyPolicy(
        MVPbkStoreContact* aContact, TUint32 aFlags )
    {
    // Reset policy
    iActivePolicy = iRelocationPolicy;

    // Read-only contacts can not be moved so
    // adjust active policy if necessary. New contacts are also not moved.
    if ( iRelocationPolicy == Pbk2ContactRelocator::EPbk2MoveContactToPhoneMemory ||
         iRelocationPolicy == Pbk2ContactRelocator::EPbk2MoveContactToPhoneMemoryAndLock )
        {
        if ( aContact )
            {
            if (aContact->ParentStore().StoreProperties().ReadOnly() ||
                aFlags & EPbk2RelocatorNewContact )
                {
                iActivePolicy = Pbk2ContactRelocator::EPbk2CopyContactToPhoneMemory;
                if ( iRelocationPolicy == Pbk2ContactRelocator::EPbk2MoveContactToPhoneMemoryAndLock )
                    {
                    iActivePolicy = Pbk2ContactRelocator::EPbk2CopyContactToPhoneMemoryAndLock;
                    }
                }            
            }
        }
    }

// --------------------------------------------------------------------------
// CPbk2ContactRelocator::PrepareToRelocateContactL
// --------------------------------------------------------------------------
//
TBool CPbk2ContactRelocator::PrepareToRelocateContactL(
        MVPbkStoreContact* aContact,
        CVPbkContactLinkArray* aContacts,
        MPbk2ContactRelocatorObserver& aObserver,
        Pbk2ContactRelocator::TPbk2ContactRelocationQueryPolicy aQueryPolicy,
        TUint32 aFlags )
    {
    __ASSERT_DEBUG( !iSourceContact && !iSourceContacts,
        Panic( EPanicPreCond_PrepareToRelocateContactL ) );

    iObserver = &aObserver;

    TBool result = EFalse;

    // If this is deleted when confirmation note returns do not access
    // any member data.
    TBool thisDestroyed = EFalse;
    iDestroyedPtr = &thisDestroyed;
    TPbk2DestructionIndicator indicator
        ( &thisDestroyed, iDestroyedPtr );

    
    if ( aContact || aContacts )
        {
        VerifyPolicy( aContact, aFlags );
        TPbk2RelocationQueryType relocationQueryType =
            SelectRelocationQueryType( aContact, aContacts, aQueryPolicy );        
            
        // Take ownership
        iSourceContact = aContact;
        iSourceContacts = aContacts;
        
        result = AskConfirmationL( aContact, aContacts, relocationQueryType );
        }
    else
        {
        iErrorCode  = KErrArgument;
        }
            
    if ( !thisDestroyed )
        {
        if ( !result )
            {
            iErrorCode = KErrCancel;
            }

        if ( iErrorCode != KErrNone )
            {
            IssueRequest();
            }
        else
            {
            if ( !iTargetStoreOpen )
                {
                OpenTargetStoreL();
                }
            else
                {
                DoRelocateContactL();
                }
            }
        }

    return result;
    }

// --------------------------------------------------------------------------
// CPbk2ContactRelocator::CopyContactL
// --------------------------------------------------------------------------
//
void CPbk2ContactRelocator::CopyContactL( MVPbkStoreContact& aContact )
    {
    // Init copy policy
    InitCopyPolicyL();

    delete iContactCopier;
    iContactCopier = NULL;
    iContactCopier = iCopyPolicy->CopyContactL
        ( aContact, *iTargetStore, *this );
    }

// --------------------------------------------------------------------------
// CPbk2ContactRelocator::FinalizeIfReady
// --------------------------------------------------------------------------
//
void CPbk2ContactRelocator::FinalizeIfReady()
    {
    // If there are no more contacts waiting to be relocated in this
    // request, signal process complete
    if ( !iSourceContacts )
        {
        iObserver->RelocationProcessComplete();
        }
    }

// --------------------------------------------------------------------------
// CPbk2ContactRelocator::SelectRelocationQueryType
// --------------------------------------------------------------------------
//
CPbk2ContactRelocator::TPbk2RelocationQueryType 
    CPbk2ContactRelocator::SelectRelocationQueryType(
        MVPbkStoreContact* aContact,
        CVPbkContactLinkArray* aContacts,
        Pbk2ContactRelocator::TPbk2ContactRelocationQueryPolicy
        aQueryPolicy )
    {    
    TPbk2RelocationQueryType relocationQueryType(EPbk2NoRelocationQuery);
    TBool moveRelocation(EFalse);
    
    if ( Pbk2ContactRelocator::EPbk2MoveContactToPhoneMemory == iActivePolicy || 
         Pbk2ContactRelocator::EPbk2MoveContactToPhoneMemoryAndLock == iActivePolicy )
        {
        moveRelocation = ETrue;
        }

    switch (aQueryPolicy)
        {
        case Pbk2ContactRelocator::EPbk2DisplayNoQueries:
            {
            relocationQueryType = EPbk2NoRelocationQuery;
            }
            break;
        
        case Pbk2ContactRelocator::EPbk2DisplayBasicQuery:
            {
            relocationQueryType = EPbk2CopyOneToStoreQuery;
            if ( moveRelocation )
                {
                relocationQueryType = EPbk2MoveOneToStoreQuery;
                }
            }
            break;
    
        case Pbk2ContactRelocator::EPbk2DisplayStoreDoesNotSupportQuery:
            {
            relocationQueryType = EPbk2CopyOneToPhoneQuery;
            if ( moveRelocation )
                {
                relocationQueryType = EPbk2MoveOneToPhoneQuery;
                }
            }
            break;

        default:
            {
            __ASSERT_DEBUG( EFalse,
                Panic( EPanicPreCond_VerifyRelocationQueryType ) );
            break;
            }
        }
        
    if ( !aContact && aContacts && 
         aQueryPolicy != Pbk2ContactRelocator::EPbk2DisplayNoQueries )
        {
        // Relocation is going to be done for many contacts
        // and query policy is defined.
        __ASSERT_DEBUG( aContacts->Count() >= KLinkArrayRelocationMinCount, 
                        Panic( EPanicPreCond_VerifyRelocationQueryType ) );
        
        relocationQueryType = EPbk2CopyManyToStoreQuery;
        if ( moveRelocation )
            {
            relocationQueryType = EPbk2MoveManyToStoreQuery;
            }        
        }
    return relocationQueryType;        
    }

// End of File
