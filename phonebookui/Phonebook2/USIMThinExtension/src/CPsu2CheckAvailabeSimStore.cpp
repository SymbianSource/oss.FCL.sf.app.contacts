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
* Description:  Phonebook 2 USIM UI Extension view manager.
*
*/


// INCLUDE FILES
#include "CPsu2CheckAvailabeSimStore.h"

// Phonebook 2
#include <MPbk2AppUi.h>
#include <MPbk2ApplicationServices.h>

// Virtual Phonebook
#include <CVPbkContactManager.h>
#include <TVPbkContactStoreUriPtr.h>
#include <MVPbkContactStoreList.h>
#include <MVPbkContactStore.h>
#include <CVPbkContactStoreUriArray.h>

#include <VPbkContactStoreUris.h>
#include <MVPbkContactStoreInfo.h>
#include <MVPbkContactStoreProperties.h>

// Debugging headers
#include <Pbk2Debug.h>


// --------------------------------------------------------------------------
// CPsu2CheckAvailabeSimStore::CPsu2CheckAvailabeSimStore
// --------------------------------------------------------------------------
//
CPsu2CheckAvailabeSimStore::CPsu2CheckAvailabeSimStore()
    {
    // Do nothing
    }

// --------------------------------------------------------------------------
// CPsu2CheckAvailabeSimStore::~CPsu2CheckAvailabeSimStore
// --------------------------------------------------------------------------
//
CPsu2CheckAvailabeSimStore::~CPsu2CheckAvailabeSimStore()
    {
    PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING
        ("CPsu2CheckAvailabeSimStore::~CPsu2CheckAvailabeSimStore") );
        
    CloseStores();        
    delete iValidSourceStoreUris;
    delete iReadyStores;
    }

// --------------------------------------------------------------------------
// CPsu2CheckAvailabeSimStore::ConstructL
// --------------------------------------------------------------------------
//
 void CPsu2CheckAvailabeSimStore::ConstructL()
    {
    PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING
        ("CPsu2CheckAvailabeSimStore::ConstructL") );
    
    iReadyStores = CVPbkContactStoreUriArray::NewL();
    
    iValidSourceStoreUris = CVPbkContactStoreUriArray::NewL();
    iValidSourceStoreUris->AppendL( 
        VPbkContactStoreUris::SimGlobalOwnNumberUri() );
    iValidSourceStoreUris->AppendL( 
        VPbkContactStoreUris::SimGlobalSdnUri() );  
        
    iStoreList = &Phonebook2::Pbk2AppUi()->ApplicationServices().
                    ContactManager().ContactStoresL();
                    
    OpenSimStoresL();                    
    }

// --------------------------------------------------------------------------
// CPsu2CheckAvailabeSimStore::NewL
// --------------------------------------------------------------------------
//
CPsu2CheckAvailabeSimStore* CPsu2CheckAvailabeSimStore::NewL()
    {
    CPsu2CheckAvailabeSimStore* self = new(ELeave)CPsu2CheckAvailabeSimStore;
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// --------------------------------------------------------------------------
// CPsu2CheckAvailabeSimStore::IsContactsAvailabe
// --------------------------------------------------------------------------
//
TBool CPsu2CheckAvailabeSimStore::IsContactsAvailabe( const TDesC& aUri )
    {    
    if ( iReadyStores->IsIncluded( TVPbkContactStoreUriPtr(aUri) ) )
        {
        MVPbkContactStore* source = iStoreList->Find( aUri );
        if ( source )
            {
            TInt result(0);
            TRAPD( error, result = source->StoreInfo().NumberOfContactsL() );
            if ( !error )
                {
                return TBool(result);
                }
            }
        }    
    return EFalse;        
    }

// --------------------------------------------------------------------------
// CPsu2CheckAvailabeSimStore::StoreReady
// --------------------------------------------------------------------------
//
void CPsu2CheckAvailabeSimStore::StoreReady( 
        MVPbkContactStore& aContactStore )
	{
	PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING
	    ("CPsu2CheckAvailabeSimStore::StoreReady") );
    
    TVPbkContactStoreUriPtr uri = aContactStore.StoreProperties().Uri();
    if ( !iReadyStores->IsIncluded( uri ) )
        {
        TRAPD( error, iReadyStores->AppendL( uri ) ); // TRAPD
        if ( error != KErrNone )
            {
            StoreUnavailable( aContactStore, error );
            }
        }            
	}

// --------------------------------------------------------------------------
// CPsu2CheckAvailabeSimStore::StoreUnavailable
// --------------------------------------------------------------------------
//
void CPsu2CheckAvailabeSimStore::StoreUnavailable
        ( MVPbkContactStore& aContactStore, TInt /*aReason*/ )
    {
    PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING
        ("CPsu2CheckAvailabeSimStore::StoreUnavailable") );
    
    TVPbkContactStoreUriPtr uri = aContactStore.StoreProperties().Uri();
    if ( iReadyStores->IsIncluded( uri ) )
        {
        iReadyStores->Remove( uri );
        }    
    }

// --------------------------------------------------------------------------
// CPsu2CheckAvailabeSimStore::HandleStoreEventL
// --------------------------------------------------------------------------
//
void CPsu2CheckAvailabeSimStore::HandleStoreEventL
        ( MVPbkContactStore& /*aContactStore*/,
          TVPbkContactStoreEvent /*aStoreEvent*/ )
    {
    // No action
    }

// --------------------------------------------------------------------------
// CPsu2CheckAvailabeSimStore::OpenSimStoresL
// --------------------------------------------------------------------------
//
void CPsu2CheckAvailabeSimStore::OpenSimStoresL()
    {
    PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING
        ("CPsu2CheckAvailabeSimStore::OpenSimStoresL") );
    
	const TInt count = iValidSourceStoreUris->Count();
	for ( TInt i = 0; i < count; ++i )
		{
		Phonebook2::Pbk2AppUi()->ApplicationServices().ContactManager().
		    LoadContactStoreL( (*iValidSourceStoreUris)[i] );
		MVPbkContactStore* source =
		iStoreList->Find( (*iValidSourceStoreUris)[i] );
        if ( source )		    
            {            
		    source->OpenL( *this );
            }
		}
    }
    
// --------------------------------------------------------------------------
// CPsu2CheckAvailabeSimStore::CloseStores
// --------------------------------------------------------------------------
//
void CPsu2CheckAvailabeSimStore::CloseStores()
    {
    const TInt count = iValidSourceStoreUris->Count();
    for ( TInt i = 0; i < count; ++i )
        {
        MVPbkContactStore* source =
            iStoreList->Find( (*iValidSourceStoreUris)[i] );
        if ( source )		    
            {            
		    source->Close( *this );
            }
        }
    }    

    
//  End of File
