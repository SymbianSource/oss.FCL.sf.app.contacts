/*
* Copyright (c) 2006-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Virtual Phonebook VCard engine
*
*/



// INCLUDES
#include "CVPbkVCardEng.h"
#include "CVPbkVCardOperationFactory.h"
#include "CVPbkVCardData.h"

#include <MVPbkContactOperation.h>
#include <CVPbkContactManager.h>
#include <MVPbkFieldType.h>

#include <vcard.h>

CVPbkVCardEng::CVPbkVCardEng(CVPbkContactManager& aContactManager)
:   iContactManager( aContactManager )                   
    {
    }

void CVPbkVCardEng::ConstructL()
    {
    iData = CVPbkVCardData::NewL(iContactManager);
    }
        
CVPbkVCardEng::~CVPbkVCardEng() 
    {
    delete iData;
    }

EXPORT_C 
CVPbkVCardEng* CVPbkVCardEng::NewL(CVPbkContactManager& aContactManager)
    {
    CVPbkVCardEng* self = new (ELeave) CVPbkVCardEng( aContactManager );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

EXPORT_C MVPbkContactOperationBase* CVPbkVCardEng::ImportVCardL(
    MVPbkContactStore& aTargetStore,
    RReadStream& aSourceStream,
    MVPbkContactCopyObserver& aObserver)
    {
    MVPbkContactOperation* operation = 
        VPbkVCardOperationFactory::CreateImportOperationL( *iData, 
        aSourceStream, aTargetStore, aObserver, EFalse );
        
    if( operation )
        {
        CleanupDeletePushL( operation );
        operation->StartL();
        CleanupStack::Pop(); // operation
        }

    return operation;
    }

EXPORT_C MVPbkContactOperationBase* CVPbkVCardEng::ImportVCardL(
        RPointerArray<MVPbkStoreContact>& aImportedContacts,
        MVPbkContactStore& aTargetStore, 
        RReadStream &aSourceStream,
        MVPbkSingleContactOperationObserver& aObserver )
    {
    MVPbkContactOperation* operation = 
        VPbkVCardOperationFactory::CreateImportOperationL( *iData, 
            aImportedContacts, aSourceStream, aTargetStore, aObserver );
        
    if( operation )
        {
        CleanupDeletePushL( operation );
        operation->StartL();
        CleanupStack::Pop(); // operation
        }

    return operation;
    }

EXPORT_C MVPbkContactOperationBase* CVPbkVCardEng::ImportCompactBusinessCardL(
    MVPbkContactStore& aTargetStore,
	RReadStream &aSourceStream,
    MVPbkContactCopyObserver& aObserver )
    {
    MVPbkContactOperation* operation = 
        VPbkVCardOperationFactory::CreateImportCompactBCardOperationL( 
            *iData, aTargetStore, aSourceStream, aObserver );

    if( operation )
        {
        CleanupDeletePushL( operation );
        operation->StartL();
        CleanupStack::Pop(); // operation
        }

    return operation;
    }

EXPORT_C MVPbkContactOperationBase* CVPbkVCardEng::ImportCompactBusinessCardL(
        RPointerArray<MVPbkStoreContact>& aImportedContacts,
        MVPbkContactStore& aTargetStore,
        RReadStream &aSourceStream,
        MVPbkSingleContactOperationObserver& aObserver )
    {
    MVPbkContactOperation* operation = 
        VPbkVCardOperationFactory::CreateImportCompactBCardOperationL( 
            *iData, aImportedContacts, aTargetStore, aSourceStream, 
            aObserver );

    if( operation )
        {
        CleanupDeletePushL( operation );
        operation->StartL();
        CleanupStack::Pop(); // operation
        }

    return operation;
    }
            
EXPORT_C MVPbkContactOperationBase* CVPbkVCardEng::ExportVCardL(
            RWriteStream& aDestStream, 
            const MVPbkContactLink& aContactLink,
            MVPbkSingleContactOperationObserver& aObserver)
    {
    MVPbkContactOperation* operation = 
        VPbkVCardOperationFactory::CreateExportOperationL( 
            *iData, 
            aDestStream, 
            aContactLink, 
            aObserver, 
            iContactManager,
            ETrue );

    if( operation )
        {
        CleanupDeletePushL( operation );
        operation->StartL();
        CleanupStack::Pop(); // operation
        }

    return operation;
    }

EXPORT_C MVPbkContactOperationBase* CVPbkVCardEng::ExportVCardL( 
            RWriteStream& aDestStream, 
            const MVPbkStoreContact& aSourceItem,
            MVPbkSingleContactOperationObserver& aObserver)
    {
    MVPbkContactOperation* operation = 
        VPbkVCardOperationFactory::CreateExportOperationL( *iData, 
                                                           aDestStream, 
                                                           aSourceItem, 
                                                           aObserver,
                                                           ETrue );
    if( operation )
        {
        CleanupDeletePushL( operation );
        operation->StartL();
        CleanupStack::Pop(); // operation
        }

    return operation;
    }
    
EXPORT_C MVPbkContactOperationBase* CVPbkVCardEng::ExportVCardForSyncL(
            RWriteStream& aDestStream, 
            const MVPbkContactLink& aContactLink,
            MVPbkSingleContactOperationObserver& aObserver)
    {
    MVPbkContactOperation* operation = 
        VPbkVCardOperationFactory::CreateExportOperationL( 
            *iData, 
            aDestStream, 
            aContactLink, 
            aObserver, 
            iContactManager,
            EFalse );

    if( operation )
        {
        CleanupDeletePushL( operation );
        operation->StartL();
        CleanupStack::Pop(); // operation
        }

    return operation;
    }

EXPORT_C MVPbkContactOperationBase* CVPbkVCardEng::ExportVCardForSyncL( 
            RWriteStream& aDestStream, 
            const MVPbkStoreContact& aSourceItem,
            MVPbkSingleContactOperationObserver& aObserver)
    {
    MVPbkContactOperation* operation = 
        VPbkVCardOperationFactory::CreateExportOperationL( *iData, 
                                                           aDestStream, 
                                                           aSourceItem, 
                                                           aObserver,
                                                           EFalse );
    if( operation )
        {
        CleanupDeletePushL( operation );
        operation->StartL();
        CleanupStack::Pop(); // operation
        }

    return operation;
    }

EXPORT_C TBool CVPbkVCardEng::SupportsFieldType(const MVPbkFieldType& aFieldType)
    {
    return iData->SupportedFieldTypes().ContainsSame( aFieldType );
    }

EXPORT_C const MVPbkFieldTypeList& CVPbkVCardEng::SupportedFieldTypes()
    {
    return iData->SupportedFieldTypes();
    }

EXPORT_C MVPbkContactOperationBase* CVPbkVCardEng::ImportVCardForSyncL(
    MVPbkContactStore& aTargetStore,
    RReadStream& aSourceStream,
    MVPbkContactCopyObserver& aObserver)
    {
    MVPbkContactOperation* operation = 
        VPbkVCardOperationFactory::CreateImportOperationL( *iData, 
        aSourceStream, aTargetStore, aObserver, ETrue );
        
    if( operation )
        {
        CleanupDeletePushL( operation );
        operation->StartL();
        CleanupStack::Pop(); // operation
        }

    return operation;
    }

// ----------------------------------------------------------------------------
// CVPbkVCardEng::ImportVCardMergeL
// ----------------------------------------------------------------------------
EXPORT_C MVPbkContactOperationBase* CVPbkVCardEng::ImportVCardMergeL(
        const MVPbkContactLink& aReplaceContact,
        MVPbkContactStore& aTargetStore, 
        RReadStream& aSourceStream,
        MVPbkSingleContactOperationObserver& aObserver )
    {
    MVPbkContactOperation* operation = 
        VPbkVCardOperationFactory::CreateImportOperationL( 
            *iData, 
            aReplaceContact, 
            aTargetStore, 
            aSourceStream,
            aObserver );

    if( operation )
        {
        CleanupDeletePushL( operation );
        operation->StartL();
        CleanupStack::Pop(); // operation
        }

    return operation;
    }


// End of file
