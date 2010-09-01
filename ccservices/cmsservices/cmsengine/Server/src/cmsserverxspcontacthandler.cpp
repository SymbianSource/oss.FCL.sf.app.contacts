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
* Description: 
*       
*
*/


#include <MVPbkContactStore.h>
#include <MVPbkStoreContact.h>
#include <CVPbkContactManager.h>
#include <MVPbkContactLinkArray.h>
#include <CVPbkContactLinkArray.h>
#include <MVPbkContactOperationBase.h>
#include <MVPbkContactStoreProperties.h>
#include <MVPbkContactLink.h>
#include "cmsserver.h"
#include "cmsserverutils.h"
#include "cmsservercontact.h"
#include "cmsphonebookproxy.h"
#include "cmsserverxspcontacthandler.h"
#include "cmsdebug.h"

// ----------------------------------------------------
// CCmsServerXSPContactHandler::CCmsServerXSPContactHandler
// 
// ----------------------------------------------------
//
CCmsServerXSPContactHandler::CCmsServerXSPContactHandler( CCmsServerContact& aContact,
                                                          CCmsServer& aCmsServer ) :
                                                          iServerContact( aContact ),
                                                          iCmsServer( aCmsServer )
    {

    }

// ----------------------------------------------------
// CCmsServerXSPContactHandler::NewL
// 
// ----------------------------------------------------
//
CCmsServerXSPContactHandler* CCmsServerXSPContactHandler::NewL(
    CCmsServerContact& aContact, CCmsServer& aCmsServer )
    {
    CCmsServerXSPContactHandler* self = new( ELeave ) CCmsServerXSPContactHandler(
        aContact, aCmsServer );
    CleanupStack::PushL( self );
    CleanupStack::Pop();
    return self;
    }

// ----------------------------------------------------
// CCmsServerXSPContactHandler::~CCmsServerXSPContactHandler
// 
// ----------------------------------------------------
//
CCmsServerXSPContactHandler::~CCmsServerXSPContactHandler()
    {
    iContactArray.ResetAndDestroy();
    iContactArray.Close();
    delete iContactLinks;
    }

// ----------------------------------------------------
// CCmsServerXSPContactHandler::SaveMessage
// 
// ----------------------------------------------------
//
void CCmsServerXSPContactHandler::SaveMessage( const RMessage2& aMessage )
    {
    iMessage = aMessage;
    }

// ----------------------------------------------------
// CCmsServerXSPContactHandler::CompleteMessage
// 
// ----------------------------------------------------
//
void CCmsServerXSPContactHandler::Cancel()
    {
    PRINT( _L( "CCmsServerAsyncContact::Cancel()" ) );
    
    iCompletionCode = KErrCancel;
    }

// ----------------------------------------------------
// CCmsServerXSPContactHandler::NumberOfFinds
// 
// ----------------------------------------------------
//
void CCmsServerXSPContactHandler::NumberOfFinds( TInt aCount )
	{
	iNumberOfFinds = aCount;
	}

// ----------------------------------------------------
// CCmsServerXSPContactHandler::NumberOfStoresInFind
// 
// ----------------------------------------------------
//
void CCmsServerXSPContactHandler::NumberOfStoresInFind( TInt aStoresNumber )
	{
	iNumberOfStoresInFind = aStoresNumber;
	}

// ----------------------------------------------------
// CCmsServerXSPContactHandler::ContactCount
// 
// ----------------------------------------------------
//
TInt CCmsServerXSPContactHandler::ContactCount() const
    {
    return iContactArray.Count();
    }

// ----------------------------------------------------
// CCmsServerXSPContactHandler::Contact
// 
// ----------------------------------------------------
//
MVPbkStoreContact* CCmsServerXSPContactHandler::Contact( TInt aIndex ) const
    {   
    const TInt count( iContactArray.Count() );
    return count > 0 && ( aIndex < count && aIndex >= 0 ) ? iContactArray[aIndex] : NULL;
    }
        
// ----------------------------------------------------
// CCmsServerXSPContactHandler::FindFromStoreSucceededL
// 
// ----------------------------------------------------
//
void CCmsServerXSPContactHandler::FindFromStoreSucceededL( MVPbkContactStore& aStore,
                                                           MVPbkContactLinkArray* aResultsFromStore )
    {
  	TPtrC uri( aStore.StoreProperties().Uri().UriDes() );
    const TInt resultCount( aResultsFromStore->Count() );
   
    PRINT1( _L( "CCmsServerAsyncContact::FindFromStoreSucceededL():  Store:        %S" ),
        &aStore.StoreProperties().Uri().UriDes() );
    PRINT1( _L( "CCmsServerAsyncContact::FindFromStoreSucceededL():Result count: %d" ),
        resultCount );
      
    //If the contact is from the same store as the contact we are searching with, do nothing
    if( resultCount > 0 && uri.Compare( iServerContact.StoreUri() ) != 0 )
        {
        CVPbkContactManager* manager = iCmsServer.PhonebookProxyHandle().ContactManager();
        for( TInt i = 0; i < resultCount; i++ )
            {
            manager->RetrieveContactL( aResultsFromStore->At( i ), *this );
            }
        }

     iNumberOfCompletedStoresInFind++;   
     
     // if all stores were checked for current find string
    if ( iNumberOfCompletedStoresInFind == iNumberOfStoresInFind )
    	{
    	iNumberOfCompletedStoresInFind = 0;
	    iNumberOfCompletedFinds++;
    	}
    	
    if ( iNumberOfCompletedFinds < iNumberOfFinds && iNumberOfCompletedStoresInFind == 0 )
    	{
    	// continue with the next find operation
    	iServerContact.StartNextFindOperationL( iNumberOfCompletedFinds );	
    	}
    
    delete aResultsFromStore;
    aResultsFromStore = NULL;
    }

// ----------------------------------------------------
// CCmsServerXSPContactHandler::FindFromStoreFailed
// 
// ----------------------------------------------------
//
void CCmsServerXSPContactHandler::FindFromStoreFailed( MVPbkContactStore& aStore, TInt aError )
    {
    iNumberOfCompletedStoresInFind++;
    // if all stores were checked for current find string
    if ( iNumberOfCompletedStoresInFind == iNumberOfStoresInFind )
    	{
    	iNumberOfCompletedStoresInFind = 0;
	    iNumberOfCompletedFinds++;
    	}
    
    if ( iNumberOfCompletedFinds < iNumberOfFinds && iNumberOfCompletedStoresInFind == 0 )
    	{
    	// continue with the next find operation
    	iServerContact.StartNextFindOperationL( iNumberOfCompletedFinds );	
    	}

    
    PRINT1( _L( "CCmsServerXSPContactHandler::FindFromStoreFailed():  Store:  %S" ),
         &aStore.StoreProperties().Uri().UriDes());
    PRINT1( _L( "CCmsServerXSPContactHandler::FindFromStoreFailed(): Error:    %d" ), aError );
    }

// ----------------------------------------------------
// CCmsServerXSPContactHandler::FindFromStoresOperationComplete
// 
// ----------------------------------------------------
//
void CCmsServerXSPContactHandler::FindFromStoresOperationComplete()
    {
    PRINT( _L( "Start CCmsServerAsyncContact::FindFromStoresOperationComplete()" ) );
    
    if( !iMessage.IsNull() )
        {
        iMessage.Complete( KErrNone == iCompletionCode ? KErrNone : iCompletionCode );
        }
    
    PRINT( _L( "End CCmsServerAsyncContact::FindFromStoresOperationComplete()" ) );     
    }

// ----------------------------------------------------------
// CCmsServerXSPContactHandler::VPbkSingleContactOperationComplete
// 
// ----------------------------------------------------------
//
void CCmsServerXSPContactHandler::VPbkSingleContactOperationComplete( MVPbkContactOperationBase& aOperation,
                                                                      MVPbkStoreContact* aContact )
    {
    PRINT( _L( "Start CCmsPhonebookProxy::VPbkSingleContactOperationComplete()" ) );
    
    MVPbkContactOperationBase* operation = ( MVPbkContactOperationBase* )&aOperation;
    delete operation;
    operation = NULL;
    TInt error( iContactArray.Append( aContact ) );
    PRINT( _L( "End CCmsPhonebookProxy::VPbkSingleContactOperationComplete()" ) );    
    }

// ----------------------------------------------------------
// CCmsServerXSPContactHandler::VPbkSingleContactOperationFailed
// 
// ----------------------------------------------------------
//
void CCmsServerXSPContactHandler::VPbkSingleContactOperationFailed( MVPbkContactOperationBase& aOperation, TInt aError )
    {
    PRINT1( _L( "CCmsPhonebookProxy::VPbkSingleContactOperationFailed() - Error: %d" ), aError );
    
    MVPbkContactOperationBase* operation = ( MVPbkContactOperationBase* )&aOperation;
    delete operation;
    operation = NULL;
    }
