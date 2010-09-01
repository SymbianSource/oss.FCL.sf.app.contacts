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
#include <f32file.h>
#include <bautils.h>
#include <barsread.h>
#include <CVPbkSortOrder.h>
#include <MVPbkContactLink.h>
#include <MVPbkStoreContact.h>
#include <MVPbkContactStore.h>
#include <CVPbkContactManager.h>
#include <MVPbkContactViewBase.h>
#include <CVPbkContactLinkArray.h>          
#include <MVPbkContactLinkArray.h>
#include <MVPbkContactStoreList.h>
#include <CVPbkContactIdConverter.h>
#include <TVPbkContactStoreUriPtr.h>
#include <MVPbkContactOperationBase.h>
#include <CVPbkContactViewDefinition.h>
#include <MVPbkBaseContactFieldCollection.h>
#include <CVPbkContactStoreUriArray.h>
#include <creatorserver.rsg>
#include "cmstestercontactcreator.h"

// ----------------------------------------------------------
// CCmsTesterContactCreator::CCmsTesterContactCreator
// 
// ----------------------------------------------------------
//
CCmsTesterContactCreator::CCmsTesterContactCreator( RFs& aFileSession ) :
                                             iFileSession( aFileSession ),
                                             iStoreReady( EFalse ),
                                             iSelectorState( ESelectorIdle )
    {
    }

// ----------------------------------------------------------
// CCmsTesterContactCreator::NewL
// 
// ----------------------------------------------------------
//    
CCmsTesterContactCreator* CCmsTesterContactCreator::NewL( RFs& aFileSession, const TDesC& aStoreUri  )
    {
    CCmsTesterContactCreator* self = new ( ELeave ) CCmsTesterContactCreator( aFileSession );
    CleanupStack::PushL( self );
    self->ConstructL( aStoreUri );
    CleanupStack::Pop();
    return self;
    }

// ----------------------------------------------------------
// CCmsTesterContactCreator::ConstructL
// 
// ----------------------------------------------------------
//        
void CCmsTesterContactCreator::ConstructL( const TDesC& aStoreUri )
    {
    iStoreUri = aStoreUri.AllocL();
    OpenResourceFileL( iCmsResourceFile, KCreatorResourcePath );
    CVPbkContactStoreUriArray* uriList = CVPbkContactStoreUriArray::NewLC();
    uriList->AppendL( TVPbkContactStoreUriPtr( *iStoreUri ) );
    iContactManager = CVPbkContactManager::NewL( *uriList, &iFileSession );
    CleanupStack::PopAndDestroy();  //uriList
    iStoreList = &iContactManager->ContactStoresL();
    }

// ----------------------------------------------------------
// CCmsTesterContactCreator::~CCmsTesterContactCreator
// 
// ----------------------------------------------------------
//
CCmsTesterContactCreator::~CCmsTesterContactCreator()
    {
    delete iStoreUri;
    delete iLinkArray;
    delete iContactManager;
    iCmsResourceFile.Close();
    }

// ----------------------------------------------------------
// CCmsTesterContactCreator::InitStoresL
// 
// ----------------------------------------------------------
//
void CCmsTesterContactCreator::InitStoresL( MVPbkContactViewObserver& aViewObserver )
    {
    iViewObserver = &aViewObserver;
    if( !iStoreReady )
        {
        iStoreList->OpenAllL( *this );
        iSelectorState = ESelectorInit;
        }
    }

// ----------------------------------------------------------
// CCmsTesterContactCreator::InitStoresL
// 
// ----------------------------------------------------------
//
void CCmsTesterContactCreator::OpenResourceFileL( RResourceFile& aResFile, const TDesC& aResourceFile )
    {
    TFileName resourceFileName( aResourceFile );
    BaflUtils::NearestLanguageFile( iFileSession, resourceFileName );
    aResFile.OpenL( iFileSession, resourceFileName );
    aResFile.ConfirmSignatureL( aResFile.SignatureL() );
    }


// ----------------------------------------------------------
// CCmsTesterContactCreator::InitStoresL
// 
// ----------------------------------------------------------
//
MVPbkContactStore& CCmsTesterContactCreator::ContactStore() const
    {
    return *iContactStore;
    }


// ----------------------------------------------------------
// CCmsTesterContactCreator::InitStoresL
// 
// ----------------------------------------------------------
//
CVPbkContactManager& CCmsTesterContactCreator::ContactManager() const
    {
    return *iContactManager;
    }

// ----------------------------------------------------------
// CCmsTesterContactCreator::SetupContactViewL
// 
// ----------------------------------------------------------
//
void CCmsTesterContactCreator::SetupContactViewL()
    {
    CVPbkContactViewDefinition* def = NULL;
    TPtrC storeUri( iStoreUri->Des() );
    def = storeUri.Compare( KCmsCreatorDbUri ) == 0 ? CreateViewDefLC( R_CREATOR_CONTACT_VIEW_STRUCTURE_1 ):
                                                      CreateViewDefLC( R_CREATOR_CONTACT_VIEW_STRUCTURE_2 );
    CVPbkSortOrder* sortOrder = CreateSortOrderLC( R_CREATOR_SORTORDER_LASTNAME_FIRSTNAME );
    iContactView = iContactManager->CreateContactViewLC( *iViewObserver, *def, *sortOrder );
    CleanupStack::Pop(); //iContactView
    CleanupStack::PopAndDestroy( 2 );  //sortOrder, def
    }

// ----------------------------------------------------------
// CCmsTesterContactCreator::CreateViewDefLC
// 
// ----------------------------------------------------------
//
CVPbkContactViewDefinition* CCmsTesterContactCreator::CreateViewDefLC( TInt aResourceId )
    {
    TResourceReader reader;
    CreateResourceReaderLC( reader, aResourceId );
    CVPbkContactViewDefinition* viewDef = CVPbkContactViewDefinition::NewL( reader );
    CleanupStack::PopAndDestroy();  //reader
    CleanupStack::PushL( viewDef );
    return viewDef;
    }

// ----------------------------------------------------------
// CCmsTesterContactCreator::CreateSortOrderLC
// 
// ----------------------------------------------------------
//
CVPbkSortOrder* CCmsTesterContactCreator::CreateSortOrderLC( TInt aResourceId )
    {
    TResourceReader reader;
    CreateResourceReaderLC( reader, aResourceId );
    CVPbkSortOrder* sortOrder = CVPbkSortOrder::NewL( reader, iContactManager->FieldTypes() );
    CleanupStack::PopAndDestroy();  //reader
    CleanupStack::PushL( sortOrder );
    return sortOrder;
    }

// ----------------------------------------------------------
// CCmsTesterContactCreator::CreateResourceReaderLC
// 
// ----------------------------------------------------------
//
void CCmsTesterContactCreator::CreateResourceReaderLC( TResourceReader& aReader, TInt aResourceId )
    {
    HBufC8* dataBuffer = iCmsResourceFile.AllocReadLC( aResourceId );
    aReader.SetBuffer( dataBuffer );
    }

// ----------------------------------------------------------
// CCmsTesterContactCreator::OpenComplete
// 
// ----------------------------------------------------------
//
void CCmsTesterContactCreator::OpenComplete()
    {
    iContactStore = iStoreList->Find( TVPbkContactStoreUriPtr( *iStoreUri ) );
    SetupContactViewL();
    }

// ----------------------------------------------------------
// CCmsTesterContactCreator::StoreReady
// 
// ----------------------------------------------------------
//      
void CCmsTesterContactCreator::StoreReady( MVPbkContactStore& /*aContactStore*/ )
    {
    }

// ----------------------------------------------------------
// CCmsTesterContactCreator::StoreUnavailable
// 
// ----------------------------------------------------------
//
void CCmsTesterContactCreator::StoreUnavailable( MVPbkContactStore& /*aContactStore*/, TInt aReason )
    {
    TInt reason = aReason;
    RDebug::Print( _L( "%d" ), reason );
    }

// ----------------------------------------------------------
// CCmsTesterContactCreator::HandleStoreEventL
// 
// ----------------------------------------------------------
//
void CCmsTesterContactCreator::HandleStoreEventL( MVPbkContactStore& /*aContactStore*/,
                                             TVPbkContactStoreEvent /*aStoreEvent*/ )
    { 
    }



// ----------------------------------------------------------
// CCmsTesterContactCreator::ResetData
// 
// ----------------------------------------------------------
//
void CCmsTesterContactCreator::ResetData()
    {
    delete iLinkArray;
    iLinkArray = NULL;
    delete iOperation;
    iOperation = NULL;
    delete iContactLink;
    iContactLink = NULL;
    }

// ----------------------------------------------------------
// CCmsTesterContactCreator::ResetData
// 
// ----------------------------------------------------------
//
void CCmsTesterContactCreator::ResetData( MVPbkContactOperationBase& aOperation )
    {
    delete iLinkArray;
    iLinkArray = NULL;
    if( iOperation == &aOperation )
        {
        delete iOperation;
        iOperation = NULL;
        delete iContactLink;
        iContactLink = NULL;
        }
    }

// ----------------------------------------------------------
// CCmsTesterContactCreator::VPbkSingleContactOperationComplete
// 
// ----------------------------------------------------------
//
void CCmsTesterContactCreator::VPbkSingleContactOperationComplete( MVPbkContactOperationBase& aOperation,
                                                                 MVPbkStoreContact* aContact )
    {
    ResetData( aOperation );
    TInt result = reinterpret_cast<TInt>( aContact );
    User::RequestComplete( iClientStatus, result );
    iSelectorState = ESelectorIdle;
    }

// ----------------------------------------------------------
// CCmsTesterContactCreator::~CCmsTesterContactCreator
// 
// ----------------------------------------------------------
//
void CCmsTesterContactCreator::VPbkSingleContactOperationFailed( MVPbkContactOperationBase& /*aOperation*/, TInt aError )
    {
    ResetData();
    iSelectorState = ESelectorIdle;
    User::RequestComplete( iClientStatus, aError );
    }

// End of File
