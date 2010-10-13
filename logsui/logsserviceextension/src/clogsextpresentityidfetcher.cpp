/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Fetches the presentity id for a certain contact link.
*
*/

#include <s32mem.h>

//for service settings table
#include <spsettings.h>
#include <spentry.h>
#include <spproperty.h>
#include <spdefinitions.h>

//Virtual Phonebook
#include <TVPbkContactStoreUriPtr.h>
#include <CVPbkContactManager.h>
#include <CVPbkContactStoreUriArray.h>
#include <MVPbkContactStoreList.h>
#include <MVPbkContactStore.h>
#include <MVPbkContactLink.h>
#include <CVPbkFieldTypeRefsList.h>    
#include <MVPbkStoreContact.h>    
#include <MVPbkContactFieldData.h>
#include <CVPbkContactFieldIterator.h>
#include <MVPbkContactOperationBase.h>    
#include <VPbkEng.rsg>  // url field resource id
#include <CVPbkContactLinkArray.h>
#include <MVPbkContactFieldUriData.h>

#include "clogsextpresentityidfetcher.h"
#include "mlogsextpresentityidfetcherobserver.h"
#include "logsextconsts.h"
#include "simpledebug.h"

_LIT( KAt, "@" );

// ---------------------------------------------------------------------------
// NewL
// ---------------------------------------------------------------------------
//
CLogsExtPresentityIdFetcher* CLogsExtPresentityIdFetcher::NewL(
        const TUint32 aServiceId,
        const TDesC8& aCntLink,
        const TLogId aLogId,
        MLogsExtPresentityIdFetcherObserver& aObserver)
    {    
    CLogsExtPresentityIdFetcher* self = 
        CLogsExtPresentityIdFetcher::NewLC( aServiceId, 
                                            aCntLink, 
                                            aLogId, 
                                            aObserver );
    CleanupStack::Pop( self );
    return self;
    }


// ---------------------------------------------------------------------------
// NewLC
// ---------------------------------------------------------------------------
//
CLogsExtPresentityIdFetcher* CLogsExtPresentityIdFetcher::NewLC(
        const TUint32 aServiceId,
        const TDesC8& aCntLink,
        const TLogId aLogId, 
        MLogsExtPresentityIdFetcherObserver& aObserver )
    {
    _LOG("CLogsExtPresentityIdFetcher::NewLC(): begin" )
    CLogsExtPresentityIdFetcher* self = 
        new( ELeave ) CLogsExtPresentityIdFetcher( aServiceId, 
                                                   aLogId, 
                                                   aObserver );
    CleanupStack::PushL( self );
    self->ConstructL( aCntLink );
    _LOG("CLogsExtPresentityIdFetcher::NewLC(): end" )
    return self;
    }

// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
//
CLogsExtPresentityIdFetcher::CLogsExtPresentityIdFetcher(
    const TUint32 aServiceId,
    const TLogId aLogId,    
    MLogsExtPresentityIdFetcherObserver& aObserver) : 
        iServiceId( aServiceId ),
        iFieldTypeResId( R_VPBK_FIELD_TYPE_IMPP ),
        iLogId( aLogId ),
        iObserver( &aObserver ),
        iCntStore( NULL ),
        iPresentityId( NULL ),
        iContactStoreId( NULL ),
        iContactManager( NULL ),
        iContactLinkArray( NULL ),
        iContactOperationBase( NULL )
    {    
    }


// ---------------------------------------------------------------------------
// ConstructL 
// ---------------------------------------------------------------------------
//
void CLogsExtPresentityIdFetcher::ConstructL( const TDesC8& aCntLink )
    {
    iCntLink = aCntLink.AllocL();
    }
    
    
// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CLogsExtPresentityIdFetcher::~CLogsExtPresentityIdFetcher()
    {
    _LOG("CLogsExtPresentityIdFetcher::~CLogsExtPresenceIdFetcher(): begin" )

    if ( iCntStore )
        {
        // its safe to close the store even if it was never opened before
        iCntStore->Close( *this );
        }
    delete iCntLink;
    delete iPresentityId;
    delete iContactStoreId;    
    delete iContactLinkArray;
    delete iContactOperationBase;
    delete iContactManager;

    _LOG("CLogsExtPresentityIdFetcher::~CLogsExtPresenceIdFetcher(): end" )
    }


// ---------------------------------------------------------------------------
// Starts the fetching process.
// ---------------------------------------------------------------------------
//
TInt CLogsExtPresentityIdFetcher::Fetch()
    {
    _LOG("CLogsExtPresentityIdFetcher::Fetch: begin" )   
    
    TInt error;
    TRAP( error, DoPresenceIdFetchL() );

    _LOGP("CLogsExtPresentityIdFetcher::Fetch: end returns=%d", error )
    return error;   
    }


// ---------------------------------------------------------------------------
// Initiates the actual fetching.
// ---------------------------------------------------------------------------
//
void CLogsExtPresentityIdFetcher::DoPresenceIdFetchL()
    {
    _LOG("CLogsExtPresentityIdFetcher::DoPresenceIdFetchL(): begin" )
    
    CSPSettings* spSettings = CSPSettings::NewLC();
    
    // get the contactstoreid from the service provider settings table
    GetContactStoreIdL( *spSettings );
    
    CleanupStack::PopAndDestroy( spSettings );
    spSettings = NULL;        
        
    CVPbkContactStoreUriArray* contactStoreUriArray = 
        CVPbkContactStoreUriArray::NewLC();
        
    const TVPbkContactStoreUriPtr storeUri( *iContactStoreId );          
    contactStoreUriArray->AppendL( storeUri ); 
    
    iContactManager = CVPbkContactManager::NewL( *contactStoreUriArray );
              
    iContactLinkArray = iContactManager->CreateLinksLC( iCntLink->Des() );
    
    CleanupStack::Pop(); // iContactLinkArray
    CleanupStack::PopAndDestroy( contactStoreUriArray );
    contactStoreUriArray = NULL;
        
    iContactManager->LoadContactStoreL( storeUri );    
    iCntStore = iContactManager->ContactStoresL().Find( storeUri );
    
    if ( iCntStore )
        {
        _LOG("this::DoPresenceIdFetchL(): open contact store" )
        // remember to call close when finished/cancelled/deleted! 
        iCntStore->OpenL( *this );   
        }
    else
        {
        _LOG("CLogsExtPresentityIdFetcher::DoPresenceIdFetchL(): error=-1" )
        User::Leave( KErrNotFound );
        }
   
    _LOG("CLogsExtPresentityIdFetcher::DoPresenceIdFetchL(): end" )
    }


// ---------------------------------------------------------------------------
// Gets the contact store id of a certain service from the SPSettings Table.
// ---------------------------------------------------------------------------
//
void CLogsExtPresentityIdFetcher::GetContactStoreIdL(
        CSPSettings& aSPSettings )
    {
    _LOG("CLogsExtPresentityIdFetcher::GetContactStoreIdL: begin")
    delete iContactStoreId;
    iContactStoreId = NULL;
    iContactStoreId = HBufC::NewL( KSPMaxDesLength );
    
    CSPProperty* spProperty = CSPProperty::NewLC();
    
    User::LeaveIfError( 
        aSPSettings.FindPropertyL( 
            ServiceId(),
            EPropertyContactStoreId,
            *spProperty ) );
    
    User::LeaveIfNull( spProperty );
    TPtr contactStoreIdPtr( iContactStoreId->Des() );
    User::LeaveIfError( spProperty->GetValue( contactStoreIdPtr ) );

    CleanupStack::PopAndDestroy( spProperty );
    spProperty = NULL;
      
    _LOG("CLogsExtPresentityIdFetcher::GetContactStoreIdL: end")
    }


// ---------------------------------------------------------------------------
// Gets the contact store id of a certain service from the SPSettings Table.
// ---------------------------------------------------------------------------
//
void CLogsExtPresentityIdFetcher::GetPresentityIDFieldTypeL(
        CSPSettings& aSPSettings )
    {
    _LOG("CLogsExtPresentityIdFetcher::GetPresentityIDFieldTypeL: begin")
    
    CSPProperty* spProperty = CSPProperty::NewLC();
    
    User::LeaveIfError( 
        aSPSettings.FindPropertyL( 
            ServiceId(),
            ESubPropertyPresencePresentityIDFieldType,
            *spProperty ) );
    
    User::LeaveIfNull( spProperty );  
    User::LeaveIfError( spProperty->GetValue( iFieldTypeResId ) );
    
    CleanupStack::PopAndDestroy( spProperty );
    spProperty = NULL;

    _LOG("CLogsExtPresentityIdFetcher::GetPresentityIDFieldTypeL: end")
    }


// ---------------------------------------------------------------------------
// Called when the operation is completed.
//
// A client has the operation as a member and it can delete the operation
// instance in this function call. If the implementation of the store
// calls the function from the operation instance it must not handle
// any member data after calling it.
// ---------------------------------------------------------------------------
//    
void CLogsExtPresentityIdFetcher::VPbkSingleContactOperationComplete(
        MVPbkContactOperationBase& /*aOperation*/,
        MVPbkStoreContact* aContact )
    {
    _LOG("CLogsExtPresentityIdFetcher::VPbkSingleContactOperationComplete...")
    _LOG("...begin")
    
    // delete the operation, since it is completed
    delete iContactOperationBase;
    iContactOperationBase = NULL;
    
    // Handles the retrieved contact
    TRAP_IGNORE( HandleRetrievedContactL( aContact ) )  

    _LOG("... VPbkSingleContactOperationComplete: end")
    }


// ---------------------------------------------------------------------------
// Handles the completion of an operation.
// ---------------------------------------------------------------------------
//
void CLogsExtPresentityIdFetcher::HandleRetrievedContactL(
        MVPbkStoreContact* aContact )
    {
    _LOG("CLogsExtPresentityIdFetcher::HandleRetrievedContactL: begin")

    aContact->PushL(); //Ownership transferred
   
    const MVPbkFieldType* fieldType = 
        iContactManager->FieldTypes().Find( iFieldTypeResId );
    if ( fieldType )
        {        
        _LOG("CLogsExtPresentityIdFetcher::HandleRetrievedContactL...")
        _LOG("...fieldType exists.")
        
        TPtrC fieldData( KNullDesC );
        CVPbkBaseContactFieldTypeIterator* itr = 
            CVPbkBaseContactFieldTypeIterator::NewLC( *fieldType, 
                aContact->Fields() );
        
        // If several fields with same fieldId: 
        // use the first with right prefix
        TBool found( EFalse );
        while( itr->HasNext() && !found )
            {
            _LOG("CLogsExtPresentityIdFetcher::HandleRetrievedContactL...")
            _LOG("...field exists.")
            const MVPbkBaseContactField* field = itr->Next();
            
            // Check field type before casting.
            const MVPbkContactFieldData& contactFieldData = field->FieldData();
            if ( EVPbkFieldStorageTypeUri == contactFieldData.DataType() )
                {
                // get field data
                const MVPbkContactFieldUriData& data = 
                    MVPbkContactFieldUriData::Cast( contactFieldData );
                fieldData.Set( data.Uri() );
                
                CSPEntry* entry = CSPEntry::NewLC();
                CSPSettings* settings = CSPSettings::NewLC();
                TInt err = settings->FindEntryL( iServiceId, *entry );
                if ( err )
                    {
                    // If service name not found leave with error code
                    User::Leave( err );
                    }
                const TDesC& serviceName = entry->GetServiceName();

                if ( 0 == fieldData.Find( serviceName ) )
                    {
					// Service name found from position 0
                    found = ETrue;
                    
                    delete iPresentityId;
                    iPresentityId = NULL;
                    iPresentityId = fieldData.AllocL();
                    _LIT( KSkypeServiceName, "Skype" );
                    if ( KErrNotFound == iPresentityId->Find( KAt ) &&
                         serviceName.Compare( KSkypeServiceName ) )
                        {
                        // TODO: This "if ()" branch can be removed when meco automatically
                        // completes domain part
                        CSPProperty* property = CSPProperty::NewLC();
                            
                        User::LeaveIfError( settings->FindPropertyL( 
                                                ServiceId(),
                                                ESubPropertyPresenceAddrScheme,
                                                *property ) );        
                        
                        User::LeaveIfNull( property );
                        
                        // property was found, read the value    
                        HBufC* domain = HBufC::NewLC( KSPMaxDesLength );
                        TPtr domainPtr( domain->Des() );
                        User::LeaveIfError( property->GetValue( domainPtr ) );
    
                        iPresentityId = iPresentityId->ReAllocL( 
                            iPresentityId->Length() + 
                            KAt().Length() + 
                            domain->Length() );
                        iPresentityId->Des().Append( KAt );
                        iPresentityId->Des().Append( *domain );
    
                        CleanupStack::PopAndDestroy( domain );
                        CleanupStack::PopAndDestroy( property );
                        }
                    _LOG("...presentityId==(nextline)")
                    _LOGDES(PresentityId())
                    iObserver->PresentityIdFetchDoneL( ServiceId(), 
                                                       PresentityId(), 
                                                       LogId() );
                    }
                CleanupStack::PopAndDestroy( settings );
                CleanupStack::PopAndDestroy( entry );
                }
            }
        if ( !found )
            {            
            User::Leave( KErrNotFound );
            }
        CleanupStack::PopAndDestroy( itr );
        itr = NULL;
        } 
    else
        {        
        User::Leave( KErrNotFound ); 
        } 
    
    CleanupStack::PopAndDestroy( aContact ); // aContact
    aContact = NULL;
    
    _LOG("CLogsExtPresentityIdFetcher::HandleRetrievedContactL: end")
    }


// ---------------------------------------------------------------------------
// Called if the operation fails.
//
// A client has the operation as a member and it can delete the operation
// instance in this function call. If the implementation of the store
// calls the function from the operation instance it must not handle
// any member data after calling it.
// ---------------------------------------------------------------------------
//
void CLogsExtPresentityIdFetcher::VPbkSingleContactOperationFailed(
            MVPbkContactOperationBase& /*aOperation*/, 
            TInt aError )
    {
    _LOG("CLogsExtPresentityIdFetcher::VPbkSingleContactOperationFailed...")
    _LOGP("...begin : aError = %d", aError)
    
    // delete the operation, since it is completed
    delete iContactOperationBase;
    iContactOperationBase = NULL;
    
    _LOG("CLogsExtPresentityIdFetcher::VPbkSingleContactOperationFailed: end ")
    }


// ---------------------------------------------------------------------------
// Called when a contact store is ready to use.
// ---------------------------------------------------------------------------
//     
void CLogsExtPresentityIdFetcher::StoreReady( 
        MVPbkContactStore& /*aContactStore*/ )
    {
    _LOG("CLogsExtPresentityIdFetcher::StoreReady: begin")

    
    const MVPbkContactLink& contactLink = iContactLinkArray->At( 0 );
    
    delete iContactOperationBase;
    iContactOperationBase = NULL;
    
    TRAP_IGNORE( iContactOperationBase = 
        iContactManager->RetrieveContactL( contactLink, *this ); )
    
    _LOG("CLogsExtPresentityIdFetcher::StoreReady: end")
    }


// ---------------------------------------------------------------------------
// Called when a contact store becomes unavailable.
// ---------------------------------------------------------------------------
//    
void CLogsExtPresentityIdFetcher::StoreUnavailable( 
        MVPbkContactStore& /*aContactStore*/, 
        TInt /*aReason*/ )
    {
    _LOG("CLogsExtPresentityIdFetcher::StoreUnavailable: begin")
    _LOG("CLogsExtPresentityIdFetcher::StoreUnavailable: end")
    }


// ---------------------------------------------------------------------------
// Called when changes occur in the contact store.
// ---------------------------------------------------------------------------
//    
void CLogsExtPresentityIdFetcher::HandleStoreEventL(
            MVPbkContactStore& /*aContactStore*/, 
            TVPbkContactStoreEvent /*aStoreEvent*/ )
    {
    _LOG("CLogsExtPresentityIdFetcher::HandleStoreEventL: begin")
    _LOG("CLogsExtPresentityIdFetcher::HandleStoreEventL: end")
    }
    
// ---------------------------------------------------------------------------
// Returns the Service id.
// ---------------------------------------------------------------------------
//  
TUint32 CLogsExtPresentityIdFetcher::ServiceId()
    {    
    _LOGP("CLogsExtPresentityIdFetcher::ServiceId = %d", iServiceId)
    return iServiceId;
    }


// ---------------------------------------------------------------------------
// Returns the unique log event id,which the presentity id is fetched for.
// ---------------------------------------------------------------------------
//
TLogId CLogsExtPresentityIdFetcher::LogId()
    {    
    _LOGP("CLogsExtPresentityIdFetcher::logid = %d", iLogId)
    return iLogId;
    }


// ---------------------------------------------------------------------------
// Returns the presentity id.
// ---------------------------------------------------------------------------
//
const TDesC& CLogsExtPresentityIdFetcher::PresentityId()
    {    
    if( iPresentityId )
        {
         _LOGDES( *iPresentityId )
         return *iPresentityId;
        }    
    _LOG("CLogsExtPresentityIdFetcher::iPresentityId end")
    return KNullDesC();
    }
