/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
*  Description: This class provides interface to find the First Contact in DefaultCntDbUri 
*  which is linked to an XSPId.
*
*/


// INCLUDES
#include <MVPbkContactStore.h>
#include <CVPbkContactManager.h>
#include <VPbkContactStoreUris.h>
#include <CVPbkContactLinkArray.h>
#include <MVPbkContactStoreList.h>
#include <CVPbkFieldTypeRefsList.h>
#include <VPbkEng.rsg>

#include <MVPbkContactOperationBase.h>
#include <MVPbkContactStoreProperties.h>
#include <TVPbkWordParserCallbackParam.h>
#include <CVPbkContactStoreUriArray.h>
#include "cmsfindlinkedcontact.h"
#include "cmsdebug.h"

#include <MVPbkStoreContact.h>
#include <MVPbkContactStore.h>
#include <MVPbkContactStoreProperties.h> 
#include <VPbkContactStoreUris.h>
#include <MVPbkContactFieldUriData.h>
#include <MVPbkContactLink.h>

// CONSTANTS
const TInt KArrayGranularity = 2;


// ----------------------------------------------------------
// CCmsLinkedContact::NewL
//
// ----------------------------------------------------------
//
CCmsFindLinkedContact* CCmsFindLinkedContact::NewL( CVPbkContactManager& aContactManager )
    {
    CCmsFindLinkedContact* self = new ( ELeave ) CCmsFindLinkedContact(
                                aContactManager );
    return self;
    }


// ----------------------------------------------------------
// CCmsFindLinkedContact::CCmsFindLinkedContact
//
// ----------------------------------------------------------
//
CCmsFindLinkedContact::CCmsFindLinkedContact( CVPbkContactManager& aContactManager )
: CActive( CActive::EPriorityHigh ),
iContactManager( aContactManager ),
iState( ERetreiveXSPInfo )

    {
    CActiveScheduler::Add( this );
    }

// ----------------------------------------------------------
// CCmsFindLinkedContact::~CCmsFindLinkedContact
//
// ----------------------------------------------------------
//
CCmsFindLinkedContact::~CCmsFindLinkedContact()
    {
    if ( iWait.IsStarted() )
        {
        iWait.AsyncStop();
        }
    Cancel();
    delete iContactLink;
    iOperationArray.ResetAndDestroy();
    delete iRetreivedXSPContact;
    }

// ----------------------------------------------------------
// CCmsFindLinkedContact::FetchLinkedContactL
//
// ----------------------------------------------------------
//
const MVPbkContactLink& CCmsFindLinkedContact::FetchLinkedContactL( 
                            const MVPbkContactLink& aXSPContactLink )
    {
    __ASSERT_DEBUG (ERetreiveXSPInfo == iState,
                                User::Leave( KErrGeneral ));
    
    //if the XSP contact is linked to the a contact in DefaultCntDbUri(), 
    //       then we return the contact in DefaultCntDbUri() 
    //else we return the same XSP Contact        
    if ( iContactLink )
        {
        delete iContactLink;
        iContactLink = NULL;
        }        
    iContactLink = aXSPContactLink.CloneLC();
    CleanupStack::Pop();    
    if ( !iWait.IsStarted() )
        {
        //Start the operation
        IssueRequest();
        
        //Start waiting here till the operation completes
        iWait.Start();    
        }
    
    __ASSERT_DEBUG (NULL != iContactLink,
                                    User::Leave( KErrGeneral ));
    //return the contact link
    return *iContactLink;
    }


// --------------------------------------------------------------------------
// CCmsFindLinkedContact::IssueRequest
// --------------------------------------------------------------------------
//
void CCmsFindLinkedContact::IssueRequest()
    {
    TRequestStatus* status = &iStatus;
    User::RequestComplete( status, KErrNone );
    SetActive();
    }

    
// --------------------------------------------------------------------------
// CCmsFindLinkedContact::RunL
// --------------------------------------------------------------------------
//
void CCmsFindLinkedContact::RunL()
    {
    PRINT1( _L( "CCmsFindLinkedContact::RunL(): %d" ), iState );
    switch( iState )
        {
        case ERetreiveXSPInfo:
            {  
            MVPbkContactOperationBase* operation = 
                                iContactManager.RetrieveContactL( *iContactLink, *this );
            CleanupStack::PushL( operation );
            iOperationArray.AppendL( operation );
            CleanupStack::Pop( operation );                    
            break;
            }
        
        case EFindLinkedContact:
            {
            __ASSERT_DEBUG (NULL != iRetreivedXSPContact,
                                                User::Leave( KErrGeneral ));
            RetreiveAndFindLinkedContactL( *iRetreivedXSPContact );            
            break;
            }       
            
        case EComplete:
            {
            if ( iWait.IsStarted() )
                {
                iWait.AsyncStop();
                }            
            break;  
            }
        default:
            {
            // Do nothing
            break;
            }
        }
    }

// --------------------------------------------------------------------------
// CCmsFindLinkedContact::DoCancel
// --------------------------------------------------------------------------
//
void CCmsFindLinkedContact::DoCancel()
    {
    }

// --------------------------------------------------------------------------
// CVIMPSTStorageVPbkStoreHandler::RunError
// --------------------------------------------------------------------------
//
TInt CCmsFindLinkedContact::RunError( TInt /*aError*/ )
    {    
    return KErrNone;
    }


// ----------------------------------------------------------
// CCmsFindLinkedContact::RetreiveAndFindLinkedContactL
//
// ----------------------------------------------------------
//
void CCmsFindLinkedContact::RetreiveAndFindLinkedContactL( 
                        const MVPbkStoreContact& aXSPContact )
    {
    TInt fieldCount = aXSPContact.Fields().FieldCount();
    
    HBufC* xspId = NULL;
    //Retreive the XSP Id of the Contact
    for( TInt index = 0 ; index < fieldCount ; index++ )
        {
        const MVPbkStoreContactField& contactField = aXSPContact.Fields().FieldAt(index);
        const MVPbkFieldType* fieldType = contactField.BestMatchingFieldType();        
        
        if ( fieldType &&  
                ( fieldType->FieldTypeResId() == R_VPBK_FIELD_TYPE_IMPP ))
            {
            //Found the XSP Id
            xspId = MVPbkContactFieldUriData::Cast(
                    contactField.FieldData()).Uri().AllocLC();
            break;
            }                       
        }
            
    if (xspId)
        {            
        //Find the Contact in DefaultCntDbUri() which is linked to the xspId
        
        CVPbkFieldTypeRefsList* fieldTypeRefList = CVPbkFieldTypeRefsList::NewL();
        TCallBack parser( CCmsFindLinkedContact::WordParserL, this );
        CleanupStack::PushL( fieldTypeRefList );
        fieldTypeRefList->AppendL( *iContactManager.FieldTypes().Find( R_VPBK_FIELD_TYPE_IMPP ) );
        
        //Append the XSP Id that needs to be searched
        CDesCArrayFlat* searchString = new( ELeave) CDesCArrayFlat( 1 );  
        CleanupStack::PushL( searchString );
        searchString->AppendL( *xspId );        
        
        //The Store to search is DefaultCntDbUri()
        CDesCArrayFlat* xSPStoresArray = new (ELeave)
                CDesCArrayFlat( KArrayGranularity );
        CleanupStack::PushL( xSPStoresArray );
        xSPStoresArray->AppendL( VPbkContactStoreUris::DefaultCntDbUri() );              
        
        //Start the find operation of the XSP id from the stores mentioned in xSPStoresArray
        MVPbkContactOperationBase* xSPFindOperation = iContactManager.FindL(
                *searchString, *fieldTypeRefList, *this, parser, *xSPStoresArray );
        
        CleanupStack::PopAndDestroy( 4 ); //xSPStoresArray, searchString, fieldTypeRefList, xspId
        
        CleanupStack::PushL( xSPFindOperation );
        iOperationArray.AppendL( xSPFindOperation );
        CleanupStack::Pop( xSPFindOperation );    
        
        }
    else
        {
        iState = EComplete;
        IssueRequest();
        }
    }
    

// ----------------------------------------------------------
// CCmsFindLinkedContact::WordParser
//
// ----------------------------------------------------------
//
TInt CCmsFindLinkedContact::WordParserL( TAny* aWordParserParam )
    {
    TVPbkWordParserCallbackParam* param = 
            static_cast<TVPbkWordParserCallbackParam*>( aWordParserParam );
    TPtrC data( param->iStringToParse->Ptr() );

    PRINT1( _L( "CCmsFindLinkedContact::WordParserL(): %S" ), &data );

    param->iWordArray->AppendL( data );
    return 0;
    }

// ----------------------------------------------------------
// CCmsFindLinkedContact::VPbkSingleContactOperationComplete
//
// ----------------------------------------------------------
//
void CCmsFindLinkedContact::VPbkSingleContactOperationComplete(
        MVPbkContactOperationBase& /*aOperation*/,
        MVPbkStoreContact* aContact )
    {
    PRINT( _L( "Start CCmsFindLinkedContact::VPbkSingleContactOperationComplete()" ) );    
   
    //We were successfull in finding the Contact
    //now its time to retreive the XSP ID from this contact
    //and initiate the find operation of the XSP Id
    iRetreivedXSPContact = aContact;
    iState = EFindLinkedContact;
    IssueRequest();    
    
    PRINT( _L( "End CCmsFindLinkedContact::VPbkSingleContactOperationComplete()" ) );
    }

// ----------------------------------------------------------
// CCmsFindLinkedContact::~CCmsFindLinkedContact
//
// ----------------------------------------------------------
//
void CCmsFindLinkedContact::VPbkSingleContactOperationFailed(
        MVPbkContactOperationBase& /*aOperation*/,
        TInt aError )
    {
    PRINT1( _L( "CCmsFindLinkedContact::VPbkSingleContactOperationFailed(): Error: %d" ), 
            aError );
    //We were not successfull in finding the XSP Contact
    //there is no point in proceeding so complete this operation  
    iState = EComplete;
    IssueRequest();
    }

// ----------------------------------------------------------
// CCmsFindLinkedContact::FindFromStoreSucceededL
//
// ----------------------------------------------------------
//
void CCmsFindLinkedContact::FindFromStoreSucceededL( MVPbkContactStore& /*aStore*/, 
    MVPbkContactLinkArray* aResults )
    {
    PRINT1( _L( "CCmsFindLinkedContact::FindCompleteL() - Count: %d" ), aResults->Count() );
    //callback from FindL operation success
    TInt count( aResults->Count() );
    if( count > 0 )
        {    
        //Found atleast one contact
        if ( iContactLink )
            {
            delete iContactLink;
            iContactLink = NULL;
            }
        //choose the First Found Item
        iContactLink = aResults->At( 0 ).CloneLC();
        CleanupStack::Pop();        
        }    
    delete aResults;
    aResults = NULL;
    PRINT( _L( "End CCmsFindLinkedContact::FindCompleteL()" ) );
    }

// ----------------------------------------------------------
// CCmsFindLinkedContact::FindFromStoreFailed
//
// ----------------------------------------------------------
//
void CCmsFindLinkedContact::FindFromStoreFailed( 
        MVPbkContactStore& /*aStore*/, TInt aError )
    {
    //callback from FindL operation failure
    PRINT1( _L( "CCmsFindLinkedContact::FindFailed() - Error: %d" ), aError );    
    }

// ----------------------------------------------------------
// CCmsFindLinkedContact::FindFromStoresOperationComplete
//
// ----------------------------------------------------------
//
void CCmsFindLinkedContact::FindFromStoresOperationComplete()
    {
    //Final callback from FindL operation that denotes the end
    //of the operation
    iState = EComplete;
    IssueRequest();
    PRINT( _L( "CCmsFindLinkedContact::FindFromStoresOperationComplete()" ) ); 
    }


// End of File
