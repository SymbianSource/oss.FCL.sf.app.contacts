/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies).
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


// INCLUDES
#include <spsettings.h>
#include <spproperty.h>
#include <MVPbkContactStore.h>
#include <CVPbkContactManager.h>
#include <VPbkContactStoreUris.h>
#include <CVPbkContactLinkArray.h>
#include <MVPbkContactStoreList.h>
#include <CVPbkFieldTypeRefsList.h>
#include <CVPbkContactIdConverter.h>
#include <CPbk2StoreConfiguration.h>
#include <MVPbkContactOperationBase.h>
#include <MVPbkContactStoreProperties.h>
#include <TVPbkWordParserCallbackParam.h>
#include <CVPbkContactStoreUriArray.h>
#include "cmsphonebookproxy.h"
#include "cmsservercontact.h"
#include "cmsdefines.h"
#include "cmsserver.h"
#include "cmsdebug.h"

#include "cmsfindlinkedcontact.h"
#include <MVPbkContactLink.h>

// CONSTANTS
const TInt KArrayGranularity = 2;

// ----------------------------------------------------------
// CCmsPhonebookProxy::CCmsPhonebookProxy
//
// ----------------------------------------------------------
//
CCmsPhonebookProxy::CCmsPhonebookProxy( MCmsPhonebookOperationsObserver& aCmsStoreOpencompleteObserver,
                                        RFs& aFileSessionHandle ) :
                                           iAtLeastOneStoreReady( EFalse ),
                                           iOpenComplete( EFalse ),
                                           iXspStoresInstalled( EFalse ),
                                           iCmsPhonebookOperationsObserver( aCmsStoreOpencompleteObserver ),
                                           iFileSessionHandle( aFileSessionHandle ),
                                           ixSPStoresArray( KArrayGranularity )
    {
    }

// ----------------------------------------------------------
// CCmsPhonebookProxy::NewL
//
// ----------------------------------------------------------
//
CCmsPhonebookProxy* CCmsPhonebookProxy::NewL(
    MCmsPhonebookOperationsObserver& aCmsStoreOpencompleteObserver,
    RFs& aFileSessionHandle )
    {
    CCmsPhonebookProxy* self = new ( ELeave ) CCmsPhonebookProxy(
        aCmsStoreOpencompleteObserver, aFileSessionHandle );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();
    return self;
    }

// ----------------------------------------------------------
// CCmsPhonebookProxy::ConstructL
//
// ----------------------------------------------------------
//
void CCmsPhonebookProxy::ConstructL()
    {
    PRINT ( _L("Start CCmsPhonebookProxy::ConstructL()") );
    iStoreConfiguration = CPbk2StoreConfiguration::NewL();
    iStoreConfiguration->AddObserverL( *this );
    CreateConfigurationL();
    PRINT ( _L("End CCmsPhonebookProxy::ConstructL()") );
    }

// ----------------------------------------------------------
// CCmsPhonebookProxy::~CCmsPhonebookProxy
//
// ----------------------------------------------------------
//
CCmsPhonebookProxy::~CCmsPhonebookProxy()
    {
    delete iOperation;
    delete iUriList;
    delete iFieldTypeRefList;
    delete iContactManager;
    if( iStoreConfiguration )
        {
        iStoreConfiguration->RemoveObserver( *this );
        delete iStoreConfiguration;
        iStoreConfiguration = NULL;
        }
    iObserverArray.Close();
    ixSPStoresArray.Reset();
    }

// ----------------------------------------------------------
// CCmsPhonebookProxy::StoreOpenStatus
//
// ----------------------------------------------------------
//
TBool CCmsPhonebookProxy::StoreOpenStatus()
    {
    if(iAtLeastOneStoreReady && iOpenComplete)
        {
        return ETrue;
        }

    return EFalse;
    }

// ----------------------------------------------------------
// CCmsPhonebookProxy::InitStoresL
//
// ----------------------------------------------------------
//
void CCmsPhonebookProxy::InitStoresL()
    {
    PRINT ( _L("Start CCmsPhonebookProxy::InitStoresL()") );
    if( !iOpenComplete )
        {
        iStoreList->OpenAllL( *this );
        }
    PRINT ( _L("End CCmsPhonebookProxy::InitStoresL()") );
    }

// ----------------------------------------------------------
// CCmsPhonebookProxy::FetchContactL
//
// ----------------------------------------------------------
//
TInt CCmsPhonebookProxy::FetchContactL( TInt32 aContactId, MCmsContactInterface* aContactInterface )
    {
    PRINT ( _L("Start CCmsPhonebookProxy::FetchContactL()") );
    PRINT1 ( _L("CCmsPhonebookProxy::FetchContactL() - Contact ID: %d"), aContactId  );

    if( iContactStore == NULL )  //Cannot do anything if phonebook store is not available
        {
        return KErrNotReady;
        }

    TInt error = KErrNotFound;
    iContactInterface = aContactInterface;
    CVPbkContactIdConverter* converter = CVPbkContactIdConverter::NewL( *iContactStore );
    CleanupStack::PushL( converter );
    MVPbkContactLink* link = converter->IdentifierToLinkLC( aContactId );
    if( link )
        {
        if( iOperation )
            {
            delete iOperation;
            iOperation = NULL;
            }
        iOperation = iContactManager->RetrieveContactL( *link, *this );
        error = KErrNone;
        }
    CleanupStack::PopAndDestroy( 2 );  //link, converter
    PRINT ( _L("End CCmsPhonebookProxy::FetchContactL()") );
    return error;
    }

// ----------------------------------------------------------
// CCmsPhonebookProxy::FetchContactL
//
// ----------------------------------------------------------
//
TInt CCmsPhonebookProxy::FetchContactL( const TDesC8& aPackedLink, MCmsContactInterface* aContactInterface )
    {
    //Whenever we get contact link, there can be the following possibilities
    //Contact could belong to any of the foll stores - Sim Store, PBK2Store, XSP Store    
    
    //If contact is from Sim Store, PBK2Store no extra effort is needed
    //just use the passed link and retreive the contact info
    
    //XSPContact
    //**********
    //If contacts is from XSP Store - then the challenge is to find whether this xspid 
    //is linked to the contact in DefaultCntDbUri or not    
    //How?
    //We need to search the entire DefaultCntDbUri() for any contact which is linked to this xspcontact
    //if more than 1 contact is linked to the same xspid, smartly choose anyone link(first link)
    //Once having found this linked contact in the DefaultCntDbUri(), use this found
    //contact to retreive the info
    
    PRINT ( _L("Start CCmsPhonebookProxy::FetchContactL()") );
    TInt error = KErrNotFound;
    iContactInterface = aContactInterface;
    CVPbkContactLinkArray* linkArray = CVPbkContactLinkArray::NewLC( aPackedLink, *iStoreList );
    if( linkArray->Count() > 0 )
        {    
        MVPbkContactLink* contactLinkToFetch = NULL;
        const MVPbkContactLink& contactLink = linkArray->At( 0 );
        
        CCmsFindLinkedContact* findLinkedContact = NULL;
        
        //Check whether the contact belongs to xsp store or not
        if ( IsXspContact( contactLink ) )
            {
            findLinkedContact = CCmsFindLinkedContact::NewL(
                                                *iContactManager );
            CleanupStack::PushL( findLinkedContact );  
            //Its been proved that the contact passed is an XSP Contact
            //Now fetch the contact in DefaultCntDbUri which is linked to
            //the passed XSP contact
            contactLinkToFetch = 
                        (findLinkedContact->FetchLinkedContactL( contactLink )).CloneLC();            
            
            }
        else
            {
            contactLinkToFetch = contactLink.CloneLC();
            }
        
        //Retreive the contact Info
        if( iOperation )
            {
            delete iOperation;
            iOperation = NULL;
            }
        iOperation = iContactManager->RetrieveContactL( *contactLinkToFetch, *this );
        CleanupStack::PopAndDestroy();  //contactLinkToFetch
        
        if ( findLinkedContact )
            {
            CleanupStack::PopAndDestroy();  //findLinkedContact
            }
        
        error = KErrNone;
        }

    CleanupStack::PopAndDestroy();  //linkArray
    PRINT ( _L("End CCmsPhonebookProxy::FetchContactL()") );
    return error;
    }

// ----------------------------------------------------------
// CCmsPhonebookProxy::FetchContactL
//
// ----------------------------------------------------------
//
TInt CCmsPhonebookProxy::FetchContactL( const TDesC& aPhoneNbr, MCmsContactInterface* aContactInterface )
    {
    PRINT ( _L("Start CCmsPhonebookProxy::FetchContactL()") );

    // Write the aPhoneNbr to log file
    PRINT1 ( _L("CCmsPhonebookProxy::FetchContactL() - Phone number: %S"), &aPhoneNbr);
    iContactInterface = aContactInterface;
    
    if( iOperation )
        {
        delete iOperation;
        iOperation = NULL;
        }
    iOperation = iContactManager->MatchPhoneNumberL( aPhoneNbr, aPhoneNbr.Length(), *this );
    PRINT ( _L("End CCmsPhonebookProxy::FetchContactL()") );
    return KErrNone;
    }

// ----------------------------------------------------------
// CCmsPhonebookProxy::AddObserverL
//
// ----------------------------------------------------------
//
void CCmsPhonebookProxy::AddObserverL( MCmsContactInterface* aObserver )
    {
    PRINT ( _L("Start CCmsPhonebookProxy::AddObserverL()") );

    PRINT1 ( _L( "CCmsPhonebookProxy::AddObserverL(): 0x%x" ), &aObserver );
    User::LeaveIfError( iObserverArray.Append( aObserver ) );

    PRINT ( _L("End CCmsPhonebookProxy::AddObserverL()") );
    }

// ----------------------------------------------------------
// CCmsPhonebookProxy::RemoveObserver
//
// ----------------------------------------------------------
//
void CCmsPhonebookProxy::RemoveObserver( MCmsContactInterface* aObserver )
    {
    TInt index( iObserverArray.Find( aObserver ) );
    if( 0 <= index )
        {
        PRINT1( _L( "CCmsPhonebookProxy::RemoveObserver(): 0x%x" ), &aObserver );
        iObserverArray.Remove( index );
        }
    }

// ----------------------------------------------------------
// CCmsPhonebookProxy::FindXSPContactL
//
// ----------------------------------------------------------
//
MVPbkContactOperationBase* CCmsPhonebookProxy::FindXSPContactL( const TDesC& aXspId, /*const TDesC& aExcludedStoreUri,*/
                                          MVPbkContactFindFromStoresObserver& aContacthandler )
    {
    if( iFieldTypeRefList )
        {
        iFieldTypeRefList->Reset();
        delete iFieldTypeRefList;
        iFieldTypeRefList = NULL;
        }
    iFieldTypeRefList = CVPbkFieldTypeRefsList::NewL();
    TCallBack parser( CCmsPhonebookProxy::WordParser, this );
    iFieldTypeRefList->AppendL( *iContactManager->FieldTypes().Find( R_VPBK_FIELD_TYPE_IMPP ) );
    CDesCArrayFlat* searchString = new( ELeave) CDesCArrayFlat( 1 );
    CleanupStack::PushL( searchString );
    searchString->AppendL( aXspId );

    MVPbkContactOperationBase* XSPFindOperation = iContactManager->FindL(
        *searchString, *iFieldTypeRefList, aContacthandler, parser, ixSPStoresArray );
    CleanupStack::PopAndDestroy( searchString );

    return XSPFindOperation;
    }

// ----------------------------------------------------------
// CCmsPhonebookProxy::NumberOfStoresInFind
//
// ----------------------------------------------------------
//
TInt CCmsPhonebookProxy::NumberOfStoresInFind( const TDesC& aExcludedStoreUri )
    {
    TVPbkContactStoreUriPtr excludedUri( aExcludedStoreUri );
    if ( iUriList->IsIncluded( excludedUri ) )
        {
        return iUriList->Count() - 1;
        }
    else
        {
        return iUriList->Count();
        }
    }

// ----------------------------------------------------------
// CCmsPhonebookProxy::UpdateXSPStoresListL
//
// ----------------------------------------------------------
//
void CCmsPhonebookProxy::UpdateXSPStoresListL()
    {
    CDesCArrayFlat* xSPStoresArray = new (ELeave)
        CDesCArrayFlat( KArrayGranularity );
    CleanupStack::PushL( xSPStoresArray );
    FindXSPStoresL( *xSPStoresArray );

    TBool configurationChanged = EFalse;
    TInt oldStoresCount = ixSPStoresArray.Count();
    TInt newStoresCount = xSPStoresArray->Count();

    if ( oldStoresCount != newStoresCount )
        {
        configurationChanged = ETrue;
        }

    for ( TInt i = 0; i < oldStoresCount && !configurationChanged; i++ )
        {
        if ( ixSPStoresArray.operator[](i).Compare(
            xSPStoresArray->operator[](i) ) != 0 )
            {
            configurationChanged = ETrue;
            }
        }

    if ( configurationChanged )
        {
        CreateConfigurationL();
        InitStoresL();
        }

    CleanupStack::PopAndDestroy( xSPStoresArray );
    }

// ----------------------------------------------------------
// CCmsPhonebookProxy::WordParser
//
// ----------------------------------------------------------
//
TInt CCmsPhonebookProxy::WordParser( TAny* aWordParserParam )
    {
    TVPbkWordParserCallbackParam* param = static_cast<TVPbkWordParserCallbackParam*>( aWordParserParam );
    TPtrC data( param->iStringToParse->Ptr() );

    PRINT1( _L( "CCmsPhonebookProxy::WordParser(): %S" ), &data );

    param->iWordArray->AppendL( data );
    return 0;
    }

// ----------------------------------------------------------
// CCmsPhonebookProxy::ExternalStoresIntalled
//
// ----------------------------------------------------------
//
TBool CCmsPhonebookProxy::ExternalStoresIntalled() const
    {
    return iXspStoresInstalled;
    }

// ----------------------------------------------------------
// CCmsPhonebookProxy::ContactManager
//
// ----------------------------------------------------------
//
CVPbkContactManager* CCmsPhonebookProxy::ContactManager() const
    {
    return iContactManager;
    }

// ----------------------------------------------------------
// CCmsPhonebookProxy::OpenComplete
//
// ----------------------------------------------------------
//
void CCmsPhonebookProxy::OpenComplete()
    {
    PRINT( _L("Start CCmsPhonebookProxy::OpenComplete()" ) );
    iOpenComplete = ETrue;
    if ( iAtLeastOneStoreReady )
        {
        iCmsPhonebookOperationsObserver.StoreOpenComplete();
        }
    PRINT( _L("End CCmsPhonebookProxy::OpenComplete()" ) );
    }

// ----------------------------------------------------------
// CCmsPhonebookProxy::FindCompleteL
//
// ----------------------------------------------------------
//
void CCmsPhonebookProxy::FindCompleteL( MVPbkContactLinkArray* aResults )
    {
    PRINT1( _L( "CCmsPhonebookProxy::FindCompleteL() - Count: %d" ), aResults->Count() );
    ResetData();
    TInt count( aResults->Count() );
    if( count > 0 )
        {
        if( iOperation )
            {
            delete iOperation;
            iOperation = NULL;
            }
        iOperation = iContactManager->RetrieveContactL( aResults->At( 0 ), *this );
        }
    else
        {
        PRINT( _L( " CCmsPhonebookProxy::FindCompleteL() : No results -> complete with KErrNotFound" ) );
        CompleteContactRequestL( KErrNotFound, NULL );
        iCmsPhonebookOperationsObserver.CmsSingleContactOperationComplete( KErrNotFound );
        }
    delete aResults;
    aResults = NULL;
    PRINT( _L( "End CCmsPhonebookProxy::FindCompleteL()" ) );
    }

// ----------------------------------------------------------
// CCmsPhonebookProxy::FindFailed
//
// ----------------------------------------------------------
//
void CCmsPhonebookProxy::FindFailed( TInt aError )
    {
    PRINT1( _L( "CCmsPhonebookProxy::FindFailed() - Error: %d" ), aError );
    ResetData();
    CompleteContactRequestL( aError, NULL );
    iCmsPhonebookOperationsObserver.CmsSingleContactOperationComplete( aError );
    }

// ----------------------------------------------------------
// CCmsPhonebookProxy::FindFailed
//
// ----------------------------------------------------------
//
void CCmsPhonebookProxy::CompleteContactRequestL( TInt aError, MVPbkStoreContact* aContact )
    {
    PRINT1( _L( "CCmsPhonebookProxy::CompleteContactRequestL() - Error: %d" ), aError );
    if( iContactInterface )
        {
        iContactInterface->ContactReadyL( aError, aContact );
        }
    else
        {
        PRINT( _L( " CCmsPhonebookProxy::CompleteContactRequestL(): No contact interface => Panic with ENullInterfaceHandle" ) );
        CCmsServer::PanicServer( ENullInterfaceHandle );
        }
    }

// ----------------------------------------------------------
// CCmsPhonebookProxy::StoreReady
//
// ----------------------------------------------------------
//
void CCmsPhonebookProxy::StoreReady( MVPbkContactStore& aContactStore )
    {
    PRINT1( _L( "CCmsPhonebookProxy::StoreReady(): Store: %S" ), (&aContactStore.StoreProperties().Uri().UriDes()) );
    //Set iAtLeastOneStoreReady to ETrue to indicate that
    //at least one store is available
    iAtLeastOneStoreReady = ETrue;
    using namespace VPbkContactStoreUris;
    if( aContactStore.StoreProperties().Uri().UriDes().Compare( DefaultCntDbUri() ) == 0 )
        {
        iContactStore = &aContactStore;
        }
    }

// ----------------------------------------------------------
// CCmsPhonebookProxy::StoreUnavailable
//
// ----------------------------------------------------------
//
void CCmsPhonebookProxy::StoreUnavailable( MVPbkContactStore& aContactStore, TInt /*aReason*/ )
    {
    PRINT1( _L( "CCmsPhonebookProxy::StoreUnavailable(): Store: %S" ),
        &aContactStore.StoreProperties().Uri().UriDes() );
    }

// ----------------------------------------------------------
// CCmsPhonebookProxy::HandleStoreEventL
//
// ----------------------------------------------------------
//
void CCmsPhonebookProxy::HandleStoreEventL( MVPbkContactStore& /*aContactStore*/,
                                            TVPbkContactStoreEvent aStoreEvent )
    {
    PRINT( _L("Start CCmsPhonebookProxy::HandleStoreEventL()" ) );
    
    TBool found = EFalse;
    const TInt count = iObserverArray.Count();
    for( TInt i = 0;!found && i < count;i++ )
        {
        MCmsContactInterface* observer( iObserverArray[i] );
        if( observer )
            {
            TCmsPhonebookEvent event = SelectEventType( aStoreEvent.iEventType );
            found = observer->OfferContactEventL( event, aStoreEvent.iContactLink );
            }
        }
    PRINT( _L("End CCmsPhonebookProxy::HandleStoreEventL()" ) );
    }

// ----------------------------------------------------------
// CCmsPhonebookProxy::ConfigurationChanged
//
// ----------------------------------------------------------
//
void CCmsPhonebookProxy::ConfigurationChanged()
    {
    PRINT( _L( "CCmsPhonebookProxy::ConfigurationChanged()" ) );
    }

// ----------------------------------------------------------
// CCmsPhonebookProxy::ConfigurationChangedComplete
//
// ----------------------------------------------------------
//
void CCmsPhonebookProxy::ConfigurationChangedComplete()
    {
    PRINT( _L( "Start CCmsPhonebookProxy::ConfigurationChangedComplete()" ) );
    CreateConfigurationL();
    InitStoresL();
    PRINT( _L( "End CCmsPhonebookProxy::ConfigurationChangedComplete()" ) );
    }

// ----------------------------------------------------------
// CCmsPhonebookProxy::CreateConfigurationL
//
// ----------------------------------------------------------
//
void CCmsPhonebookProxy::CreateConfigurationL()
    {
    PRINT( _L( "Start CCmsPhonebookProxy::CreateConfigurationL()" ) );

    iAtLeastOneStoreReady = EFalse;
    iOpenComplete = EFalse;
    delete iUriList;
    iUriList = NULL;
    iContactStore = NULL;

    // Support all contact stores, not only those defined by phonebook2 setting
    iUriList = iStoreConfiguration->SupportedStoreConfigurationL();

    ixSPStoresArray.Reset();
    FindXSPStoresL( ixSPStoresArray );

#ifdef _DEBUG
    PRINT( _L( " CCmsPhonebookProxy::CreateConfigurationL(): Current store configuration:" ) );
    for( TInt i = 0; i < iUriList->Count(); i++ )
        {
        PRINT2( _L( "  [%d] = %S" ), i, &iUriList->operator[](i).UriDes() );
        }
#endif

    delete iContactManager;
    iContactManager = NULL;
    iContactManager = CVPbkContactManager::NewL( *iUriList, &iFileSessionHandle );
    iStoreList = &iContactManager->ContactStoresL();

    PRINT( _L( "End CCmsPhonebookProxy::CreateConfigurationL()" ) );
    }

// ----------------------------------------------------
// CCmsServerAsyncContact::FindXSPStoresL
//
// ----------------------------------------------------
//
void CCmsPhonebookProxy::FindXSPStoresL( CDesCArrayFlat& aArray )
    {
    PRINT( _L( "Start CCmsPhonebookProxy::OpenXSPStoresL()" ) );

    RIdArray ids;
    CleanupClosePushL( ids );
    iXspStoresInstalled = EFalse;
    CSPSettings* settings = CSPSettings::NewL();
    CleanupStack::PushL( settings );
    TInt error = settings->FindServiceIdsL( ids );
    const TInt count = ids.Count();
    PRINT2( _L( "CCmsPhonebookProxy::OpenXSPStoresL : Error: %d Count: %d" ), error, count );

    for( TInt i = 0; i < count; i++)
        {
        CSPProperty* property = CSPProperty::NewLC();
        error = settings->FindPropertyL( ids[i], EPropertyContactStoreId, *property );
        if( KErrNone == error )
            {
            RBuf uri;
            uri.Create( KSPMaxDesLength );
            CleanupClosePushL( uri );
            property->GetValue( uri );
            TVPbkContactStoreUriPtr uriPtr( uri );
            if( !iUriList->IsIncluded( uriPtr ) ) 
            	{
            	iUriList->AppendL( uriPtr );            
            	}
            aArray.AppendL( uri );
            CleanupStack::PopAndDestroy();  //uri
            iXspStoresInstalled = ETrue;
            }
        CleanupStack::PopAndDestroy();  //property
        }
    CleanupStack::PopAndDestroy( 2 );  //settings, ids
    aArray.Sort();

    PRINT( _L( "End CCmsPhonebookProxy::OpenXSPStoresL()" ) );
    }

// ----------------------------------------------------------
// CCmsPhonebookProxy::SelectEventType
//
// ----------------------------------------------------------
//
TCmsPhonebookEvent CCmsPhonebookProxy::SelectEventType( TVPbkContactStoreEvent::TVPbkContactStoreEventType aPhonebookEvent )
    {
    TCmsPhonebookEvent event = ( TCmsPhonebookEvent )-1;
    switch( aPhonebookEvent )
        {
        case TVPbkContactStoreEvent::EContactDeleted:
            {
            event = ECmsContactDeleted;
            break;
            }
        case TVPbkContactStoreEvent::EContactChanged:
            {
            event = ECmsContactModified;
            break;
            }
        default:
            {
            event = ECmsContactModified;
            break;
            }
        }
    return event;
    }


// ----------------------------------------------------------
// CCmsPhonebookProxy::ResetData
//
// ----------------------------------------------------------
//
void CCmsPhonebookProxy::ResetData()
    {
    PRINT( _L( "Start CCmsPhonebookProxy::ResetData()" ) );
    if( iOperation )
        {
        delete iOperation;
        iOperation = NULL;
        }
    PRINT( _L( "End CCmsPhonebookProxy::ResetData()" ) );
    }

// ----------------------------------------------------------
// CCmsPhonebookProxy::VPbkSingleContactOperationComplete
//
// ----------------------------------------------------------
//
void CCmsPhonebookProxy::VPbkSingleContactOperationComplete(
        MVPbkContactOperationBase& /*aOperation*/,
        MVPbkStoreContact* aContact )
    {
    PRINT( _L( "Start CCmsPhonebookProxy::VPbkSingleContactOperationComplete()" ) );

    ResetData();
    CompleteContactRequestL( KErrNone, aContact );
    iCmsPhonebookOperationsObserver.CmsSingleContactOperationComplete( KErrNone );

    PRINT( _L( "End CCmsPhonebookProxy::VPbkSingleContactOperationComplete()" ) );
    }

// ----------------------------------------------------------
// CCmsPhonebookProxy::~CCmsPhonebookProxy
//
// ----------------------------------------------------------
//
void CCmsPhonebookProxy::VPbkSingleContactOperationFailed(
        MVPbkContactOperationBase& /*aOperation*/,
        TInt aError )
    {
    PRINT1( _L( "CCmsPhonebookProxy::VPbkSingleContactOperationFailed(): Error: %d" ), aError );

    ResetData();
    CompleteContactRequestL( aError, NULL );
    iCmsPhonebookOperationsObserver.CmsSingleContactOperationComplete( aError );
    }


// ----------------------------------------------------------
// CCmsPhonebookProxy::IsXspContact
//
// ----------------------------------------------------------
//
TBool CCmsPhonebookProxy::IsXspContact( const MVPbkContactLink& aContactLink ) const
    {
    //find whether the contact belongs to XSP Store using the StoreProperties
    TBool xspContact = ETrue;
    const TDesC& storeName = aContactLink.ContactStore().StoreProperties().Uri().UriDes();
    
    using namespace VPbkContactStoreUris;
    if( ( storeName.CompareF( DefaultCntDbUri() ) == 0 ) ||
            ( storeName.CompareF( SimGlobalAdnUri() ) == 0 ) ||
            ( storeName.CompareF( SimGlobalFdnUri() ) == 0 ) ||
            ( storeName.CompareF( SimGlobalSdnUri() ) == 0 ) )
        {
        xspContact = EFalse;
        }

    return xspContact;    
    }


// End of File
