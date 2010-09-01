/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
#include <creatorserver2.rsg>
#include "cmstestercontactcreator2.h"


// ----------------------------------------------------------
//
CCmsTesterContactCreator2::CCmsTesterContactCreator2( RFs& aFileSession ) :
                                             iFileSession( aFileSession ),
                                             iStoreReady( EFalse ),
                                             iSelectorState( ESelectorIdle )
    {
    }


// ----------------------------------------------------------
//    
CCmsTesterContactCreator2* CCmsTesterContactCreator2::NewL( RFs& aFileSession )
    {
    CCmsTesterContactCreator2* self = new ( ELeave ) CCmsTesterContactCreator2( aFileSession );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();
    return self;
    }

// ----------------------------------------------------------
//        
void CCmsTesterContactCreator2::ConstructL()
    {
    OpenResourceFileL( iCmsResourceFile, KCreatorResourcePath );
    CVPbkContactStoreUriArray* uriList = CVPbkContactStoreUriArray::NewLC();
    uriList->AppendL( TVPbkContactStoreUriPtr( KCmsCreatorDbUri ) );
    iContactManager = CVPbkContactManager::NewL( *uriList, &iFileSession );
    CleanupStack::PopAndDestroy();  //uriList
    iStoreList = &iContactManager->ContactStoresL();
    }

// ----------------------------------------------------------
//
CCmsTesterContactCreator2::~CCmsTesterContactCreator2()
    {        
    delete iLinkArray;
    delete iContactManager;
    iCmsResourceFile.Close();
    }

// ----------------------------------------------------------
//
void CCmsTesterContactCreator2::InitStoresL( MVPbkContactViewObserver& aViewObserver )
    {
    iViewObserver = &aViewObserver;
    if( !iStoreReady )
        {
        iStoreList->OpenAllL( *this );
        iSelectorState = ESelectorInit;
        }
    }

// ----------------------------------------------------------
//
void CCmsTesterContactCreator2::OpenResourceFileL( RResourceFile& aResFile, const TDesC& aResourceFile )
    {
    TFileName resourceFileName( aResourceFile );
    BaflUtils::NearestLanguageFile( iFileSession, resourceFileName );
    aResFile.OpenL( iFileSession, resourceFileName );
    aResFile.ConfirmSignatureL( aResFile.SignatureL() );
    }



// ----------------------------------------------------------
//
MVPbkContactStore& CCmsTesterContactCreator2::ContactStore() const
    {
    return *iContactStore;
    }

// ----------------------------------------------------------
//
CVPbkContactManager& CCmsTesterContactCreator2::ContactManager() const
    {
    return *iContactManager;
    }

// ----------------------------------------------------------
//
void CCmsTesterContactCreator2::SetupContactViewL()
    {
    CVPbkContactViewDefinition* def = CreateViewDefLC(
        R_CREATOR_CONTACT_VIEW_STRUCTURE );
    CVPbkSortOrder* sortOrder = CreateSortOrderLC(
        R_CREATOR_SORTORDER_LASTNAME_FIRSTNAME );
    iContactView = iContactManager->CreateContactViewLC( *iViewObserver, *def, *sortOrder );
    CleanupStack::Pop(); //iContactView
    CleanupStack::PopAndDestroy( 2 );  //sortOrder, def
    }

// ----------------------------------------------------------
//
CVPbkContactViewDefinition* CCmsTesterContactCreator2::CreateViewDefLC( TInt aResourceId )
    {
    TResourceReader reader;
    CreateResourceReaderLC( reader, aResourceId );
    CVPbkContactViewDefinition* viewDef = CVPbkContactViewDefinition::NewL( reader );
    CleanupStack::PopAndDestroy();  //reader
    CleanupStack::PushL( viewDef );
    return viewDef;
    }

// ----------------------------------------------------------
//
CVPbkSortOrder* CCmsTesterContactCreator2::CreateSortOrderLC( TInt aResourceId )
    {
    TResourceReader reader;
    CreateResourceReaderLC( reader, aResourceId );
    CVPbkSortOrder* sortOrder = CVPbkSortOrder::NewL( reader, iContactManager->FieldTypes() );
    CleanupStack::PopAndDestroy();  //reader
    CleanupStack::PushL( sortOrder );
    return sortOrder;
    }
 
// ----------------------------------------------------------
//
void CCmsTesterContactCreator2::CreateResourceReaderLC( TResourceReader& aReader, TInt aResourceId )
    {
    HBufC8* dataBuffer = iCmsResourceFile.AllocReadLC( aResourceId );
    aReader.SetBuffer( dataBuffer );
    }

// ----------------------------------------------------------
//
void CCmsTesterContactCreator2::OpenComplete()
    {
    iContactStore = iStoreList->Find( TVPbkContactStoreUriPtr( KCmsCreatorDbUri ) );
    SetupContactViewL();
    }

// ----------------------------------------------------------
//      
void CCmsTesterContactCreator2::StoreReady( MVPbkContactStore& /*aContactStore*/ )
    {
    }

// ----------------------------------------------------------
//
void CCmsTesterContactCreator2::StoreUnavailable( MVPbkContactStore& /*aContactStore*/, TInt /*aReason*/ )
    { 
    }

// ----------------------------------------------------------
//
void CCmsTesterContactCreator2::HandleStoreEventL( MVPbkContactStore& /*aContactStore*/,
                                             TVPbkContactStoreEvent /*aStoreEvent*/ )
    { 
    }

// ----------------------------------------------------------
//
void CCmsTesterContactCreator2::ResetData()
    {
    delete iLinkArray;
    iLinkArray = NULL;
    delete iOperation;
    iOperation = NULL;
    delete iContactLink;
    iContactLink = NULL;
    }

// ----------------------------------------------------------
//
void CCmsTesterContactCreator2::ResetData( MVPbkContactOperationBase& aOperation )
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
//
void CCmsTesterContactCreator2::VPbkSingleContactOperationComplete( MVPbkContactOperationBase& aOperation,
                                                                 MVPbkStoreContact* aContact )
    {
    ResetData( aOperation );
    TInt result = reinterpret_cast<TInt>( aContact );
    User::RequestComplete( iClientStatus, result );
    iSelectorState = ESelectorIdle;
    }


// ----------------------------------------------------------
//
void CCmsTesterContactCreator2::VPbkSingleContactOperationFailed( MVPbkContactOperationBase& /*aOperation*/, TInt aError )
    {
    ResetData();
    iSelectorState = ESelectorIdle;
    User::RequestComplete( iClientStatus, aError );
    }

// End of File
