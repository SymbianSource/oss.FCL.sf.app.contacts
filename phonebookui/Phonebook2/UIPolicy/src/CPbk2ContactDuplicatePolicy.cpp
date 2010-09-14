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
* Description:  Phonebook 2 contact duplicate policy.
*
*/


#include "CPbk2ContactDuplicatePolicy.h"

// Phonebook2
#include <CPbk2SortOrderManager.h>
#include <Pbk2ContactNameFormatterFactory.h>
#include <MPbk2ContactNameFormatter.h>
#include <CPbk2DuplicateContactFinder.h>
#include <MPbk2DuplicateContactObserver.h>
#include <RPbk2LocalizedResourceFile.h>
#include "Pbk2DataCaging.hrh"
#include "Pbk2PresentationUtils.h"
#include <pbk2presentation.rsg>
#include <CVPbkFieldTypeSelector.h>
#include <CVPbkFieldTypeIterator.h>
#include <CVPbkFieldTypeRefsList.h>

// Virtual Phonebook
#include <CVPbkContactManager.h>
#include <MVPbkContactOperationBase.h>
#include <CVPbkContactLinkArray.h>
#include <MVPbkStoreContact.h>

// System includes
#include <barsread.h>


/// Unnamed namespace for local definitions
namespace {

NONSHARABLE_CLASS(CPbk2DuplicateOperation) :
            public CBase,
            public MVPbkContactOperationBase,
            private MPbk2DuplicateContactObserver
    {
    public: // Construction and destruction

        /**
         * Creates a new instance of this class.
         *
         * @param aFinder               Duplicate contact finder.
         * @param aOwnDuplicates        Array of duplicates.
         * @param aDuplicates           Array of duplicates.
         * @param aContact              Contact.
         * @param aTargetStore          Target store.
         * @param aObserver             Find observer.
         * @param aMaxDuplicatesToFind  Maximum amount to find.
         * @return  A new instance of this class.
         */
        static CPbk2DuplicateOperation* NewL(
                CPbk2DuplicateContactFinder& aFinder,
                RPointerArray<MVPbkStoreContact>& aOwnDuplicates,
                RPointerArray<MVPbkStoreContact>& aDuplicates,
                const MVPbkBaseContact& aContact,
                MVPbkContactStore& aTargetStore,
                MVPbkContactFindObserver& aObserver,
                TInt aMaxDuplicatesToFind );

        /**
         * Destructor.
         */
        ~CPbk2DuplicateOperation();

    private: // From MPbk2DuplicateContactObserver
        void DuplicateFindComplete();
        void DuplicateFindFailed(
                TInt aError );

    private: // Implementation
        CPbk2DuplicateOperation(
                CPbk2DuplicateContactFinder& aFinder,
                RPointerArray<MVPbkStoreContact>& aOwnDuplicates,
                RPointerArray<MVPbkStoreContact>& aDuplicates,
                MVPbkContactFindObserver& aObserver );
        void ConstructL(
                const MVPbkBaseContact& aContact,
                MVPbkContactStore& aTargetStore,
                TInt aMaxDuplicatesToFind );
        void HandleFindCompleteL();

    private: // Data
        /// Ref: Array of duplicates
        RPointerArray<MVPbkStoreContact>& iDuplicates;
        /// Ref: Array of duplicates
        RPointerArray<MVPbkStoreContact>& iOwnDuplicates;
        /// Ref: Duplicate finder
        CPbk2DuplicateContactFinder& iFinder;
        /// Ref: Observer
        MVPbkContactFindObserver& iObserver;
    };

// --------------------------------------------------------------------------
// CPbk2DuplicateOperation::CPbk2DuplicateOperation
// --------------------------------------------------------------------------
//
CPbk2DuplicateOperation::CPbk2DuplicateOperation(
        CPbk2DuplicateContactFinder& aFinder,
        RPointerArray<MVPbkStoreContact>& aOwnDuplicates,
        RPointerArray<MVPbkStoreContact>& aDuplicates,
        MVPbkContactFindObserver& aObserver ) :
            iDuplicates( aDuplicates ),
            iOwnDuplicates( aOwnDuplicates ),
            iFinder( aFinder ),
            iObserver( aObserver )
    {
    }

// --------------------------------------------------------------------------
// CPbk2DuplicateOperation::~CPbk2DuplicateOperation
// --------------------------------------------------------------------------
//
CPbk2DuplicateOperation::~CPbk2DuplicateOperation()
    {
    }

// --------------------------------------------------------------------------
// CPbk2DuplicateOperation::NewL
// --------------------------------------------------------------------------
//
CPbk2DuplicateOperation* CPbk2DuplicateOperation::NewL(
        CPbk2DuplicateContactFinder& aFinder,
        RPointerArray<MVPbkStoreContact>& aOwnDuplicates,
        RPointerArray<MVPbkStoreContact>& aDuplicates,
        const MVPbkBaseContact& aContact,
        MVPbkContactStore& aTargetStore,
        MVPbkContactFindObserver& aObserver,
        TInt aMaxDuplicatesToFind )
    {
    CPbk2DuplicateOperation* self = new( ELeave ) CPbk2DuplicateOperation(
        aFinder, aOwnDuplicates, aDuplicates, aObserver );
    CleanupStack::PushL( self );
    self->ConstructL( aContact, aTargetStore, aMaxDuplicatesToFind );
    CleanupStack::Pop( self );
    return self;
    }

// --------------------------------------------------------------------------
// CPbk2DuplicateOperation::ConstructL
// --------------------------------------------------------------------------
//
void CPbk2DuplicateOperation::ConstructL
        ( const MVPbkBaseContact& aContact, MVPbkContactStore& aTargetStore,
          TInt aMaxDuplicatesToFind )
    {
    iFinder.StartL( aContact, aTargetStore, *this, aMaxDuplicatesToFind );
    }

// --------------------------------------------------------------------------
// CPbk2DuplicateOperation::DuplicateFindComplete
// --------------------------------------------------------------------------
//
void CPbk2DuplicateOperation::DuplicateFindComplete()
    {
    TRAPD( res, HandleFindCompleteL() );
    if ( res != KErrNone )
        {
        DuplicateFindFailed( res );
        }
    }

// --------------------------------------------------------------------------
// CPbk2DuplicateOperation::DuplicateFindFailed
// --------------------------------------------------------------------------
//
void CPbk2DuplicateOperation::DuplicateFindFailed( TInt aError )
    {
    iObserver.FindFailed( aError );
    }

// --------------------------------------------------------------------------
// CPbk2DuplicateOperation::HandleFindCompleteL
// --------------------------------------------------------------------------
//
void CPbk2DuplicateOperation::HandleFindCompleteL()
    {
    CVPbkContactLinkArray* links = CVPbkContactLinkArray::NewLC();
    const TInt count = iOwnDuplicates.Count();
    for ( TInt i = count - 1; i >= 0; --i )
        {
        MVPbkStoreContact* contact = iOwnDuplicates[i];

        // Create links for results
        links->AppendL( iOwnDuplicates[i]->CreateLinkLC() );
        CleanupStack::Pop(); // link

        // Ownership of the contact changes from
        // iOwnDuplicates to iDuplicates
        iDuplicates.InsertL( contact, 0 );
        iOwnDuplicates.Remove(i);
        }

    CleanupStack::Pop( links );
    // Give ownership to client
    iObserver.FindCompleteL( links );
    }

} /// namespace

// --------------------------------------------------------------------------
// CPbk2ContactDuplicatePolicy::CPbk2ContactDuplicatePolicy
// --------------------------------------------------------------------------
//
CPbk2ContactDuplicatePolicy::CPbk2ContactDuplicatePolicy(
        CVPbkContactManager& aContactManager ) :
            iContactManager( aContactManager )
    {
    }

// --------------------------------------------------------------------------
// CPbk2ContactDuplicatePolicy::~CPbk2ContactDuplicatePolicy
// --------------------------------------------------------------------------
//
CPbk2ContactDuplicatePolicy::~CPbk2ContactDuplicatePolicy()
    {
    iDuplicates.ResetAndDestroy();
    delete iDuplicateFinder;
    delete iNameFormatter;
    delete iSortOrderManager;
    delete iFieldTypeRefsList;
    }

// --------------------------------------------------------------------------
// CPbk2ContactDuplicatePolicy::NewL
// --------------------------------------------------------------------------
//
CPbk2ContactDuplicatePolicy* CPbk2ContactDuplicatePolicy::NewL
        ( TParam* aParam )
    {
    CPbk2ContactDuplicatePolicy* self =
        new( ELeave ) CPbk2ContactDuplicatePolicy( aParam->iContactManager );
    CleanupStack::PushL( self );
    self->ConstructL( aParam->iFieldTypesForFind );
    CleanupStack::Pop( self );
    return self;
    }

// --------------------------------------------------------------------------
// CPbk2ContactDuplicatePolicy::ConstructL
// --------------------------------------------------------------------------
//
void CPbk2ContactDuplicatePolicy::ConstructL
        ( const MVPbkFieldTypeList* aFieldTypeForFind )
    {
    iSortOrderManager = CPbk2SortOrderManager::NewL(
        iContactManager.FieldTypes() );
    iNameFormatter = Pbk2ContactNameFormatterFactory::CreateL(
        iContactManager.FieldTypes(), *iSortOrderManager,
        &iContactManager.FsSession() );

    const MVPbkFieldTypeList* fieldTypesForFind = aFieldTypeForFind;
    if ( !fieldTypesForFind )
        {
        iFieldTypeRefsList = CreateFieldTypesForFindL(iContactManager);
        fieldTypesForFind = iFieldTypeRefsList;
        }
    iDuplicateFinder = CPbk2DuplicateContactFinder::NewL( iContactManager,
        *iNameFormatter, *fieldTypesForFind, iDuplicates );
    }

// --------------------------------------------------------------------------
// CPbk2ContactDuplicatePolicy::FindDuplicatesL
// --------------------------------------------------------------------------
//
MVPbkContactOperationBase* CPbk2ContactDuplicatePolicy::FindDuplicatesL
        ( const MVPbkBaseContact& aContact,
          MVPbkContactStore& aTargetStore,
          RPointerArray<MVPbkStoreContact>& aDuplicates,
          MVPbkContactFindObserver& aObserver,
          TInt aMaxDuplicatesToFind )
    {
    iDuplicates.ResetAndDestroy();
    return CPbk2DuplicateOperation::NewL( *iDuplicateFinder, iDuplicates,
        aDuplicates, aContact, aTargetStore, aObserver,
        aMaxDuplicatesToFind );
    }

// --------------------------------------------------------------------------
// CPbk2ContactDuplicatePolicy::CreateFieldTypesForFindL
// --------------------------------------------------------------------------
//
MVPbkFieldTypeList* CPbk2ContactDuplicatePolicy::CreateFieldTypesForFindL
        (CVPbkContactManager& aContactManager) const
    {
    RFs fs = aContactManager.FsSession();
    RPbk2LocalizedResourceFile resFile(&fs);
    resFile.OpenLC(KPbk2RomFileDrive, 
        KDC_RESOURCE_FILES_DIR, 
        Pbk2PresentationUtils::PresentationResourceFile());

    // Create resource reader.
    TResourceReader resReader;
    resReader.SetBuffer(resFile.AllocReadLC(R_TITLE_FIELD_SELECTOR));

    // Create title field selector.
    CVPbkFieldTypeSelector* titleFieldSelector = CVPbkFieldTypeSelector::NewL
        (resReader, aContactManager.FieldTypes());
    CleanupStack::PushL(titleFieldSelector);
    
    // Create field type list for find.
    CVPbkFieldTypeRefsList* fieldTypeRefsList = CVPbkFieldTypeRefsList::NewL();
    CleanupStack::PushL(fieldTypeRefsList);
    const MVPbkFieldType* fieldType = NULL;

    // Create field type iterator.
    CVPbkFieldTypeIterator* fieldTypeIterator =
        CVPbkFieldTypeIterator::NewLC(*titleFieldSelector, 
            aContactManager.FieldTypes());
    while(fieldTypeIterator->HasNext())
        {
        fieldType = fieldTypeIterator->Next();
        if (fieldType)
            {
            // Filter the Versit type for find.
            if (EVPbkNonVersitTypeNone == fieldType->NonVersitType())
                {
                fieldTypeRefsList->AppendL(*fieldType);
                }
            }
        }

    CleanupStack::PopAndDestroy();  // fieldTypeIterator
    CleanupStack::Pop(fieldTypeRefsList);
    CleanupStack::PopAndDestroy();  // titleFieldSelector
    CleanupStack::PopAndDestroy();  // resReader
    CleanupStack::PopAndDestroy();  // resFile

    return fieldTypeRefsList;
    }

// End of File
