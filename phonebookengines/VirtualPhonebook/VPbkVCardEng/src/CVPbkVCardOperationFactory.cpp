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
* Description:  
*
*/


#include "CVPbkVCardOperationFactory.h"
#include "CVPbkVCardImporter.h"
#include "CVPbkVCardCompactBCardImporter.h"
#include "CVPbkImportToStoreOperation.h"
#include "CVPbkImportToContactsOperation.h"
#include "CVPbkVCardPropertyExporter.h"
#include "CVPbkVCardData.h"
#include "CVPbkImportToContactsMergeOperation.h"

#include <CVPbkContactManager.h>
#include <MVPbkStoreContact.h>

MVPbkContactOperation* VPbkVCardOperationFactory::CreateExportOperationL( 
    CVPbkVCardData& aData,
    RWriteStream& aDestStream, 
    const MVPbkStoreContact& aSourceItem, 
    MVPbkSingleContactOperationObserver& aObserver,
    const TBool aBeamed )
    {
    CVPbkVCardPropertyExporter* exporter = CVPbkVCardPropertyExporter::NewLC(
        aData,
        &aSourceItem,
        aDestStream,
        aObserver,
        aBeamed);
    CleanupStack::Pop( exporter );
    return exporter;
    }

MVPbkContactOperation* VPbkVCardOperationFactory::CreateExportOperationL( 
    CVPbkVCardData& aData,
    RWriteStream& aDestStream, 
    const MVPbkContactLink& aContactLink, 
    MVPbkSingleContactOperationObserver& aObserver,
    CVPbkContactManager& aContactManager,
    const TBool aBeamed)
    {
    CVPbkVCardPropertyExporter* exporter = CVPbkVCardPropertyExporter::NewLC(
        aData,
        aDestStream,
        aObserver,
        aBeamed);
    exporter->InitLinkL( aContactLink, aContactManager );
    CleanupStack::Pop( exporter );

    return exporter;
    }

MVPbkContactOperation* VPbkVCardOperationFactory::CreateImportOperationL( 
    CVPbkVCardData& aData,
    RReadStream& aSourceStream, 
    MVPbkContactStore& aTargetStore, 
    MVPbkContactCopyObserver& aObserver,
    const TBool aSync )
    {
    return CVPbkImportToStoreOperation::NewL( EVCard, aTargetStore, 
        aSourceStream, aData, aObserver, aSync );
    }

MVPbkContactOperation* VPbkVCardOperationFactory::CreateImportOperationL( 
    CVPbkVCardData& aData,
    RPointerArray<MVPbkStoreContact>& aImportedContacts,
    RReadStream& aSourceStream, 
    MVPbkContactStore& aTargetStore, 
    MVPbkSingleContactOperationObserver& aObserver )
    {
    return CVPbkImportToContactsOperation::NewL( EVCard,
        aData, aImportedContacts, aTargetStore, aSourceStream, aObserver );
    }
            
MVPbkContactOperation* VPbkVCardOperationFactory::CreateImportCompactBCardOperationL(
    CVPbkVCardData& aData,
    MVPbkContactStore& aTargetStore,
	RReadStream &aSourceStream,
    MVPbkContactCopyObserver& aObserver )
    {
    return CVPbkImportToStoreOperation::NewL( ECompactBusinessCard,
        aTargetStore, aSourceStream, aData, aObserver, EFalse );
    }

MVPbkContactOperation* VPbkVCardOperationFactory::CreateImportCompactBCardOperationL(
    CVPbkVCardData& aData,
    RPointerArray<MVPbkStoreContact>& aImportedContacts,
    MVPbkContactStore& aTargetStore,
    RReadStream &aSourceStream,
    MVPbkSingleContactOperationObserver& aObserver )
    {
    return CVPbkImportToContactsOperation::NewL( ECompactBusinessCard,
        aData, aImportedContacts, aTargetStore, aSourceStream, aObserver );
    }

// ----------------------------------------------------------------------------
// VPbkVCardOperationFactory::CreateImportOperationL
// ----------------------------------------------------------------------------
MVPbkContactOperation* VPbkVCardOperationFactory::CreateImportOperationL( 
    CVPbkVCardData& aData, 
    const MVPbkContactLink& aReplaceContact, 
    MVPbkContactStore& aTargetStore, 
    RReadStream& aSourceStream,
    MVPbkSingleContactOperationObserver& aObserver )
    {
    return CVPbkImportToContactsMergeOperation::NewL( EVCard,
            aData, aReplaceContact, aTargetStore, aSourceStream, aObserver );
    }
// End of file
