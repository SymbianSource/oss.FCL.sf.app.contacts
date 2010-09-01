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
* Description:  Phonebook 2 contact copy policy.
*
*/


#include "CPbk2ContactCopyPolicy.h"

// Phonebook 2
#include <MPbk2FieldProperty.h>
#include <MPbk2ContactNameFormatter.h>
#include <Pbk2ContactNameFormatterFactory.h>
#include <CPbk2SortOrderManager.h>
#include <CPbk2FieldPropertyArray.h>
#include <CPbk2PresentationContact.h>
#include <CPbk2ContactMerge.h>
#include <Pbk2ContactFieldCopy.h>
#include <RPbk2LocalizedResourceFile.h>
#include <Pbk2DataCaging.hrh>
#include <Pbk2Presentation.rsg>

// Virtual Phonebook
#include <MVPbkContactFieldCopyObserver.h>
#include <CVPbkContactFieldIterator.h>
#include <CVPbkContactLinkArray.h>
#include <CVPbkContactManager.h>
#include <MVPbkContactFieldDateTimeData.h>
#include <MVPbkContactFieldTextData.h>
#include <MVPbkContactFieldBinaryData.h>
#include <MVPbkContactFieldUriData.h>
#include <MVPbkFieldType.h>
#include <MVPbkContactStore.h>
#include <MVPbkContactStoreProperties.h>
#include <MVPbkContactObserver.h>
#include <MVPbkContactCopyObserver.h>
#include <MVPbkContactOperationBase.h>
#include <MVPbkContactAttributeManager.h>
#include <VPbkUtils.h>
#include <VPbkEng.rsg>

// System includes
#include <coemain.h>

/// Unnamed namespace for local definitions
namespace {

_LIT(KResourceFile, "pbk2presentation.rsc");


/**
 * Add field operation.
 * Responsible for adding a field to given contact.
 */
class CPbk2AddFieldOperation : public CActive,
                               public MVPbkContactOperationBase,
                               public MVPbkSetAttributeObserver
    {
    public: // Construction and destruction

        /**
         * Creates a new instance of this class.
         *
         * @param aCopyPolicy           Copy policy.
         * @param aContactManager       Contact manager.
         * @param aContact              Store contact.
         * @param aFieldType            Field type.
         * @param aFieldData            Field data.
         * @param aFieldLabel           Field label.
         * @param aContactAttribute     Contact attribute.
         * @param aCopyObserver         Copy observer.
         * @return  A new instance of this class.
         */
        static CPbk2AddFieldOperation* NewL(
                CPbk2ContactCopyPolicy& aCopyPolicy,
                CVPbkContactManager& aContactManager,
                MVPbkStoreContact& aContact,
                const MVPbkFieldType& aFieldType,
                const TDateTime& aFieldData,
                const TDesC* aFieldLabel,
                const MVPbkContactAttribute* aContactAttribute,
                MVPbkContactFieldCopyObserver& aCopyObserver );

        /**
         * Creates a new instance of this class.
         *
         * @param aCopyPolicy           Copy policy.
         * @param aContactManager       Contact manager.
         * @param aContact              Store contact.
         * @param aFieldType            Field type.
         * @param aFieldData            Field data.
         * @param aFieldLabel           Field label.
         * @param aContactAttribute     Contact attribute.
         * @param aCopyObserver         Copy observer.
         * @return  A new instance of this class.
         */
        static CPbk2AddFieldOperation* NewL(
                CPbk2ContactCopyPolicy& aCopyPolicy,
                CVPbkContactManager& aContactManager,
                MVPbkStoreContact& aContact,
                const MVPbkFieldType& aFieldType,
                const TDesC& aFieldData,
                const TDesC* aFieldLabel,
                const MVPbkContactAttribute* aContactAttribute,
                MVPbkContactFieldCopyObserver& aCopyObserver );

        /**
         * Creates a new instance of this class.
         *
         * @param aCopyPolicy           Copy policy.
         * @param aContactManager       Contact manager.
         * @param aContact              Store contact.
         * @param aFieldType            Field type.
         * @param aFieldData            Field data.
         * @param aFieldLabel           Field label.
         * @param aContactAttribute     Contact attribute.
         * @param aCopyObserver         Copy observer.
         * @return  A new instance of this class.
         */
        static CPbk2AddFieldOperation* NewL(
                CPbk2ContactCopyPolicy& aCopyPolicy,
                CVPbkContactManager& aContactManager,
                MVPbkStoreContact& aContact,
                const MVPbkFieldType& aFieldType,
                const TDesC8& aFieldData,
                const TDesC* aFieldLabel,
                const MVPbkContactAttribute* aContactAttribute,
                MVPbkContactFieldCopyObserver& aCopyObserver );

        /**
         * Destructor.
         */
        ~CPbk2AddFieldOperation();

    public: // From CActive
        void RunL();
        void DoCancel();
        TInt RunError(
                TInt aError );

    public: // From MVPbkSetAttributeObserver
        void AttributeOperationComplete(
                MVPbkContactOperationBase& aOperation );
        void AttributeOperationFailed(
                MVPbkContactOperationBase& aOperation,
                TInt aError );

    private: // Implementation
        CPbk2AddFieldOperation(
                CPbk2ContactCopyPolicy& aCopyPolicy,
                CVPbkContactManager& aContactManager );
        void CommonConstructL(
                MVPbkStoreContact& aContact,
                const MVPbkFieldType& aFieldType,
                const TDesC* aFieldLabel,
                const MVPbkContactAttribute* aContactAttribute,
                MVPbkContactFieldCopyObserver& aCopyObserver );
        TBool CanAddFieldL(
                MVPbkStoreContact& aContact,
                const MVPbkFieldType& aFieldType );
        void IssueRequest();

    private: // Data
        /// Ref: Copy policy
        CPbk2ContactCopyPolicy& iCopyPolicy;
        /// Ref: Contact manager
        CVPbkContactManager& iContactManager;
        /// Own: Field to be added
        MVPbkStoreContactField* iField;
        /// Ref: Contact to add the fields to
        MVPbkStoreContact* iContact;
        /// Ref: Copy observer
        MVPbkContactFieldCopyObserver* iCopyObserver;
        /// Ref: Contact attribute
        const MVPbkContactAttribute* iContactAttribute;
        /// Own: Attribute set operation
        MVPbkContactOperationBase* iSetOperation;
    };

/**
 * Save contact operation.
 * Responsible for saving the contact to the target store.
 */
class CPbk2SaveContactOperation : public CBase,
                                  public MVPbkContactOperationBase,
                                  public MVPbkContactObserver
    {
    public: // Construction and destruction

        /**
         * Creates a new instance of this class.
         *
         * @param aContactToCommit  Contact to commit.
         * @param aCopyObserver     Copy observer.
         * @param aFs               File server handle
         * @return  A new instance of this class.
         */
        static CPbk2SaveContactOperation* NewL(
                MVPbkStoreContact* aContactToCommit,
                MVPbkContactCopyObserver& aCopyObserver,
                RFs& aFs );

        /**
         * Creates a new instance of this class.
         *
         * @param aSourceContact    Contact to copy.
         * @param aCopyObserver     Copy observer.
         * @param aTargetStore      Target store.
         * @param aFs               File server handle
         * @param aCopyNameToFirstName If source contact does not have
         *           first name field, name is copied to first name field.
         * @return  A new instance of this class.
         */
        static CPbk2SaveContactOperation* NewL(
                MVPbkStoreContact& aSourceContact,
                MVPbkContactCopyObserver& aCopyObserver,
                MVPbkContactStore& aTargetStore,
                RFs& aFs, 
                TBool aCopyNameToFirstName );
        /**
         * Destructor.
         */
        ~CPbk2SaveContactOperation();

    public: // From MVPbkContactObserver
        void ContactOperationCompleted(
                TContactOpResult aResult );
        void ContactOperationFailed(
                TContactOp aOpCode,
                TInt aErrorCode,
                TBool aErrorNotified );

    private: // Implementation
        CPbk2SaveContactOperation(
                MVPbkContactCopyObserver& aCopyObserver );
        void ConstructL(
                MVPbkStoreContact& aContact,
                MVPbkContactStore* aTargetStore, RFs& aFs, 
                TBool aCopyNameToFirstName );
        void CopyContactDataL(
                MVPbkStoreContact& aSourceContact,
                MVPbkStoreContact& aTargetContact,
                TBool aCopyNameToFirstName );

    private: // Data
        /// Ref: Copy observer
        MVPbkContactCopyObserver& iCopyObserver;
        /// Own: A new contact in case of a copy
        MVPbkStoreContact* iContact;

    };

/**
 * Merge and save contact operation.
 * Responsible for merging contacts into a one.
 */
class CPbk2MergeAndSaveOperation : public CActive,
                                   public MVPbkContactOperationBase,
                                   public MVPbkContactObserver
    {
    public: // Construction and destruction

        /**
         * Creates a new instance of this class.
         *
         * @param aContactMerge     Contact merger.
         * @param aSourceContacts   Source contacts to merge.
         * @param aTarget           Target contact.
         * @param aCopyObserver     Copy observer.
         * @return  A new instance of this class.
         */
        static CPbk2MergeAndSaveOperation* NewL(
                CPbk2ContactMerge& aContactMerge,
                RPointerArray<MVPbkStoreContact>& aSourceContacts,
                MVPbkStoreContact& aTarget,
                MVPbkContactCopyObserver& aCopyObserver );

        /**
         * Destructor.
         */
        ~CPbk2MergeAndSaveOperation();

    public: // From CActive
        void RunL();
        void DoCancel();

    public: // From MVPbkContactObserver
        void ContactOperationCompleted(
                TContactOpResult aResult );
        void ContactOperationFailed(
                TContactOp aOpCode,
                TInt aErrorCode,
                TBool aErrorNotified );

    private: // Implementation
        CPbk2MergeAndSaveOperation(
                CPbk2ContactMerge& aContactMerge,
                MVPbkStoreContact& aTarget,
                MVPbkContactCopyObserver& aCopyObserver );
        void ConstructL(
                RPointerArray<MVPbkStoreContact>& aSourceContacts );
        void IssueRequest();
        void MergeL();
        void HandleContactOperationL(
                TContactOp aOpCode );
        void SendOperationCompleted();

    private: // Data
        /// Ref: Contact merger
        CPbk2ContactMerge& iContactMerge;
        /// Ref: The target of the merge
        MVPbkStoreContact& iTarget;
        /// Ref: Copy observer
        MVPbkContactCopyObserver& iCopyObserver;
        /// Own: Source contacts for the merge
        RPointerArray<MVPbkStoreContact> iSourceContacts;
        /// Own: Saved contacts
        CVPbkContactLinkArray* iResults;
    };

/**
 * Copy titles operation.
 * Responsible for copying title fields.
 */
class CPbk2CopyTitlesOperation : public CActive,
                                 public MVPbkContactOperationBase
    {
    public: // Construction and destruction

        /**
         * Creates a new instance of this class.
         *
         * @param aSourceContact    Source contact
         * @param aTargetContact    Target contact.
         * @param aCopyObserver     Copy observer.
         * @param aNameFormatter    Name formatter.
         * @return  A new instance of this class.
         */
        static CPbk2CopyTitlesOperation* NewL(
                const MVPbkStoreContact& aSourceContact,
                MVPbkStoreContact& aTargetContact,
                MVPbkContactFieldsCopyObserver& aCopyObserver,
                MPbk2ContactNameFormatter& aNameFormatter );

        /**
         * Destructor.
         */
        ~CPbk2CopyTitlesOperation();

    public: // From CActive
        void RunL();
        void DoCancel();

    private: // Implementation
        CPbk2CopyTitlesOperation(
                const MVPbkStoreContact& aSourceContact,
                MVPbkStoreContact& aTargetContact,
                MVPbkContactFieldsCopyObserver& aCopyObserver,
                MPbk2ContactNameFormatter& aNameFormatter );
        void ConstructL();
        void IssueRequest();

    private: // Data
        /// Ref: The source of the title fields
        const MVPbkStoreContact& iSourceContact;
        /// Ref: Target of the copy
        MVPbkStoreContact& iTargetContact;
        /// Ref: An observer for asynchronous operation
        MVPbkContactFieldsCopyObserver& iCopyObserver;
        /// Ref: The name formatter for title field checking
        MPbk2ContactNameFormatter& iNameFormatter;
    };

// --------------------------------------------------------------------------
// CPbk2AddFieldOperation::CPbk2AddFieldOperation
// --------------------------------------------------------------------------
//
CPbk2AddFieldOperation::CPbk2AddFieldOperation(
        CPbk2ContactCopyPolicy& aCopyPolicy,
        CVPbkContactManager& aContactManager) :
            CActive(EPriorityStandard),
            iCopyPolicy(aCopyPolicy),
            iContactManager(aContactManager)
    {
    CActiveScheduler::Add(this);
    }

// --------------------------------------------------------------------------
// CPbk2AddFieldOperation::~CPbk2AddFieldOperation
// --------------------------------------------------------------------------
//
CPbk2AddFieldOperation::~CPbk2AddFieldOperation()
    {
    Cancel();
    delete iField;
    delete iSetOperation;
    }

// --------------------------------------------------------------------------
// CPbk2AddFieldOperation::NewL
// --------------------------------------------------------------------------
//
CPbk2AddFieldOperation* CPbk2AddFieldOperation::NewL(
        CPbk2ContactCopyPolicy& aCopyPolicy,
        CVPbkContactManager& aContactManager,
        MVPbkStoreContact& aContact,
        const MVPbkFieldType& aFieldType,
        const TDateTime& aFieldData,
        const TDesC* aFieldLabel,
        const MVPbkContactAttribute* aContactAttribute,
        MVPbkContactFieldCopyObserver& aCopyObserver)
    {
    CPbk2AddFieldOperation* self = new(ELeave) CPbk2AddFieldOperation(
            aCopyPolicy, aContactManager);
    CleanupStack::PushL(self);
    self->CommonConstructL
        (aContact, aFieldType, aFieldLabel,
        aContactAttribute, aCopyObserver);
    if (self->iField)
        {
        MVPbkContactFieldDateTimeData::Cast(
            self->iField->FieldData()).SetDateTime(aFieldData);
        }
    CleanupStack::Pop(self);
    return self;
    }

// --------------------------------------------------------------------------
// CPbk2AddFieldOperation::NewL
// --------------------------------------------------------------------------
//
CPbk2AddFieldOperation* CPbk2AddFieldOperation::NewL(
        CPbk2ContactCopyPolicy& aCopyPolicy,
        CVPbkContactManager& aContactManager,
        MVPbkStoreContact& aContact,
        const MVPbkFieldType& aFieldType,
        const TDesC& aFieldData,
        const TDesC* aFieldLabel,
        const MVPbkContactAttribute* aContactAttribute,
        MVPbkContactFieldCopyObserver& aCopyObserver)
    {
    CPbk2AddFieldOperation* self = new(ELeave) CPbk2AddFieldOperation(
            aCopyPolicy, aContactManager);
    CleanupStack::PushL(self);
    self->CommonConstructL
        (aContact, aFieldType, aFieldLabel,
        aContactAttribute, aCopyObserver);
    if (self->iField)
        {
        if ( self->iField->FieldData().DataType() == EVPbkFieldStorageTypeText )
        	{
	        MVPbkContactFieldTextData::Cast(
	            self->iField->FieldData()).SetTextL(aFieldData);
        	}
		else if ( self->iField->FieldData().DataType() == EVPbkFieldStorageTypeUri )
	  	    {
	        MVPbkContactFieldUriData::Cast(
		            self->iField->FieldData()).SetUriL(aFieldData);
	    	}        	
        }
        	        
    CleanupStack::Pop(self);
    return self;
    }

// --------------------------------------------------------------------------
// CPbk2AddFieldOperation::NewL
// --------------------------------------------------------------------------
//
CPbk2AddFieldOperation* CPbk2AddFieldOperation::NewL(
        CPbk2ContactCopyPolicy& aCopyPolicy,
        CVPbkContactManager& aContactManager,
        MVPbkStoreContact& aContact,
        const MVPbkFieldType& aFieldType,
        const TDesC8& aFieldData,
        const TDesC* aFieldLabel,
        const MVPbkContactAttribute* aContactAttribute,
        MVPbkContactFieldCopyObserver& aCopyObserver)
    {
    CPbk2AddFieldOperation* self = new(ELeave) CPbk2AddFieldOperation(
            aCopyPolicy, aContactManager);
    CleanupStack::PushL(self);
    self->CommonConstructL(aContact, aFieldType, aFieldLabel,
            aContactAttribute, aCopyObserver);
    if (self->iField)
        {
        MVPbkContactFieldBinaryData::Cast(
            self->iField->FieldData()).SetBinaryDataL(aFieldData);
        }
    CleanupStack::Pop(self);
    return self;
    }

// --------------------------------------------------------------------------
// CPbk2AddFieldOperation::CommonConstructL
// --------------------------------------------------------------------------
//
void CPbk2AddFieldOperation::CommonConstructL(
        MVPbkStoreContact& aContact,
        const MVPbkFieldType& aFieldType,
        const TDesC* aFieldLabel,
        const MVPbkContactAttribute* aContactAttribute,
        MVPbkContactFieldCopyObserver& aCopyObserver)
    {
    if (iCopyObserver)
        {
        User::Leave(KErrInUse);
        }
    iContactAttribute = aContactAttribute;
    iCopyObserver = &aCopyObserver;
    iContact = &aContact;
    delete iField;
    iField = NULL;

    if (CanAddFieldL(aContact, aFieldType))
        {
        iField = aContact.CreateFieldLC(aFieldType);
        CleanupStack::Pop();
        if (aFieldLabel && aFieldLabel->Length() > 0)
            {
            iField->SetFieldLabelL(*aFieldLabel);
            }
        }
    IssueRequest();
    }

// --------------------------------------------------------------------------
// CPbk2AddFieldOperation::RunL
// --------------------------------------------------------------------------
//
void CPbk2AddFieldOperation::RunL()
    {
    // If both are set, add the attribute first because the AddFieldL
    // takes the ownership of the iField.
    if (iField && iContactAttribute)
        {
        // if contact attribute is set, set it for the field
        iSetOperation =
            iContactManager.ContactAttributeManagerL().SetFieldAttributeL(
                *iField, *iContactAttribute, *this);
        // If the operation wont be created, try to add the field atleast.
        if (!iSetOperation)
            {
            iContactAttribute = NULL;
            IssueRequest();
            }
        }
    else if(iField && !iContactAttribute)
        {
        iContact->AddFieldL(iField);
        iField = NULL; // Ownership taken by iContact
        iCopyObserver->FieldAddedToContact(*this);
        }
    else if (!iField)
        {
        // If field can not be added to contact, inform client
        iCopyObserver->FieldAddingFailed(*this, KErrNotSupported);
        }
    }

// --------------------------------------------------------------------------
// CPbk2AddFieldOperation::DoCancel
// --------------------------------------------------------------------------
//
void CPbk2AddFieldOperation::DoCancel()
    {
    }

// --------------------------------------------------------------------------
// CPbk2AddFieldOperation::RunError
// --------------------------------------------------------------------------
//
TInt CPbk2AddFieldOperation::RunError(TInt aError)
    {
    iCopyObserver->FieldAddingFailed(*this, aError);
    return KErrNone;
    }

// --------------------------------------------------------------------------
// CPbk2AddFieldOperation::AttributeOperationComplete
// --------------------------------------------------------------------------
//
void CPbk2AddFieldOperation::AttributeOperationComplete(
    MVPbkContactOperationBase& aOperation)
    {
    if (iSetOperation == &aOperation)
        {
        delete iSetOperation;
        iSetOperation = NULL;

        // Attribute has been set. Set to NULL to indicate that.
        iContactAttribute = NULL;
        IssueRequest();
        }
    }

// --------------------------------------------------------------------------
// CPbk2AddFieldOperation::AttributeOperationFailed
// --------------------------------------------------------------------------
//
void CPbk2AddFieldOperation::AttributeOperationFailed(
    MVPbkContactOperationBase& aOperation, TInt aError)
    {
    if (iSetOperation == &aOperation)
        {
        delete iSetOperation;
        iSetOperation = NULL;
        iCopyObserver->FieldAddingFailed(*this, aError);
        }
    }

// --------------------------------------------------------------------------
// CPbk2AddFieldOperation::CanAddFieldL
// --------------------------------------------------------------------------
//
TBool CPbk2AddFieldOperation::CanAddFieldL(
        MVPbkStoreContact& aContact, const MVPbkFieldType& aFieldType)
    {
    const MPbk2FieldProperty* fieldProperty =
        iCopyPolicy.FieldProperties().FindProperty(aFieldType);
    if (fieldProperty)
        {
        if (fieldProperty->Multiplicity() == EPbk2FieldMultiplicityOne)
            {
            const MVPbkStoreContactFieldCollection& fields =
                aContact.Fields();
            const TInt fieldCount = fields.FieldCount();

            const TInt maxMatchPriority =
                iContactManager.FieldTypes().MaxMatchPriority();
            for (TInt matchPriority = 0;
                    matchPriority <= maxMatchPriority; ++matchPriority)
                {
                for (TInt i = 0; i < fieldCount; ++i)
                    {
                    const MVPbkFieldType* fieldType =
                            fields.FieldAt(i).MatchFieldType(matchPriority);
                    if (fieldType && fieldType->IsSame(aFieldType))
                        {
                        return EFalse;
                        }
                    }
                }
            }
        }

    return ETrue;
    }

// --------------------------------------------------------------------------
// CPbk2AddFieldOperation::IssueRequest
// --------------------------------------------------------------------------
//
void CPbk2AddFieldOperation::IssueRequest()
    {
    TRequestStatus* status = &iStatus;
    User::RequestComplete(status, KErrNone);
    SetActive();
    }

// --------------------------------------------------------------------------
// CPbk2SaveContactOperation::CPbk2SaveContactOperation
// --------------------------------------------------------------------------
//
CPbk2SaveContactOperation::CPbk2SaveContactOperation(
        MVPbkContactCopyObserver& aCopyObserver) :
            iCopyObserver(aCopyObserver)
    {
    }

// --------------------------------------------------------------------------
// CPbk2SaveContactOperation::~CPbk2SaveContactOperation
// --------------------------------------------------------------------------
//
CPbk2SaveContactOperation::~CPbk2SaveContactOperation()
    {
    delete iContact;
    }

// --------------------------------------------------------------------------
// CPbk2SaveContactOperation::NewL
// --------------------------------------------------------------------------
//
CPbk2SaveContactOperation* CPbk2SaveContactOperation::NewL(
        MVPbkStoreContact* aContactToCommit,
        MVPbkContactCopyObserver& aCopyObserver,
        RFs& aFs)
    {
    CPbk2SaveContactOperation* self = new(ELeave) CPbk2SaveContactOperation(
        aCopyObserver);
    CleanupStack::PushL(self);
    self->ConstructL(*aContactToCommit, NULL, aFs, EFalse);
    // Take ownership
    self->iContact = aContactToCommit;
    CleanupStack::Pop(self);
    return self;
    }

// --------------------------------------------------------------------------
// CPbk2SaveContactOperation::NewL
// --------------------------------------------------------------------------
//
CPbk2SaveContactOperation* CPbk2SaveContactOperation::NewL(
        MVPbkStoreContact& aSourceContact,
        MVPbkContactCopyObserver& aCopyObserver,
        MVPbkContactStore& aTargetStore,
        RFs& aFs,
        TBool aCopyNameToFirstName )
    {
    CPbk2SaveContactOperation* self = new(ELeave) CPbk2SaveContactOperation(
        aCopyObserver);
    CleanupStack::PushL(self);
    self->ConstructL(aSourceContact, &aTargetStore, aFs, aCopyNameToFirstName );
    CleanupStack::Pop(self);
    return self;
    }

// --------------------------------------------------------------------------
// CPbk2SaveContactOperation::ConstructL
// --------------------------------------------------------------------------
//
void CPbk2SaveContactOperation::ConstructL(
        MVPbkStoreContact& aContact,
        MVPbkContactStore* aTargetStore,
        RFs& aFs,
        TBool aCopyNameToFirstName )
    {
    if (aTargetStore)
        {
        iContact = aTargetStore->CreateNewContactLC();
        CleanupStack::Pop(); // data
        CopyContactDataL(aContact, *iContact, aCopyNameToFirstName );

        // Verify syncronization field existance and content
        VPbkUtils::VerifySyncronizationFieldL(
            aFs,
            iContact->ParentStore().StoreProperties().
            SupportedFields(), *iContact);

        iContact->CommitL(*this);
        }
    else
        {
        // Verify syncronization field existance and content
        VPbkUtils::VerifySyncronizationFieldL(
            aFs,
            aContact.ParentStore().StoreProperties().
            SupportedFields(), aContact);

        aContact.CommitL(*this);
        }
    }

// --------------------------------------------------------------------------
// CPbk2SaveContactOperation::ContactOperationCompleted
// --------------------------------------------------------------------------
//
void CPbk2SaveContactOperation::ContactOperationCompleted
        ( TContactOpResult /*aResult*/ )
    {
    TRAPD( error,
        CVPbkContactLinkArray* linkArray = CVPbkContactLinkArray::NewLC();
        // Copying contacts to contact model data base
        // results always only one contact
        MVPbkContactLink* link = iContact->CreateLinkLC();
        linkArray->AppendL(link);
        CleanupStack::Pop(); // link

        // Ownership of array is given to observer
        iCopyObserver.ContactsSaved(*this, linkArray);
        CleanupStack::Pop(linkArray);
    ); // TRAPD
    if ( error != KErrNone )
        {
        iCopyObserver.ContactsSavingFailed(*this, error);
        }
    }

// --------------------------------------------------------------------------
// CPbk2SaveContactOperation::ContactOperationFailed
// --------------------------------------------------------------------------
//
void CPbk2SaveContactOperation::ContactOperationFailed
        (TContactOp /*aOpCode*/,  TInt aErrorCode,
        TBool /*aErrorNotified*/)
    {
    iCopyObserver.ContactsSavingFailed(*this, aErrorCode);
    }

// --------------------------------------------------------------------------
// CPbk2SaveContactOperation::CopyContactDataL
// --------------------------------------------------------------------------
//
void CPbk2SaveContactOperation::CopyContactDataL(
        MVPbkStoreContact& aSourceContact,
        MVPbkStoreContact& aTargetContact,
        TBool aCopyNameToFirstName )
    {
    const TInt maxMatchPriority = aSourceContact.ContactStore().
            StoreProperties().SupportedFields().MaxMatchPriority();

    // If source store does not support first name, copy last name to
    // first name if aCopyNameToFirstName is set
    TBool copyNameToFirstName( EFalse );
    if ( aCopyNameToFirstName )
        {
        const MVPbkFieldType* fieldType = 
            aSourceContact.ContactStore().StoreProperties().SupportedFields().
             Find( R_VPBK_FIELD_TYPE_FIRSTNAME );
        if ( !fieldType )
            {
            copyNameToFirstName = ETrue;
            }
        }

    MVPbkStoreContactFieldCollection& fields = aSourceContact.Fields();
    const TInt fieldCount = fields.FieldCount();
    for (TInt i = 0; i < fieldCount; ++i)
        {
        MVPbkStoreContactField& field = fields.FieldAt(i);
        const MVPbkFieldType* fieldType = NULL;

        for (TInt matchPriority = 0;
             !fieldType && matchPriority <= maxMatchPriority;
             ++matchPriority)
            {
            fieldType = field.MatchFieldType(matchPriority);
            }

        if (fieldType)
            {
            if ( copyNameToFirstName && fieldType->FieldTypeResId() == 
                 R_VPBK_FIELD_TYPE_LASTNAME )
                {
                const MVPbkFieldType* firstNamefieldType(
                            aTargetContact.ContactStore().
                            StoreProperties().SupportedFields().
                            Find( R_VPBK_FIELD_TYPE_FIRSTNAME ) );
                // Change type to first name
                if ( firstNamefieldType )
                    {
                    fieldType = firstNamefieldType;
                    }
                }
            Pbk2ContactFieldCopy::CopyFieldL( field, *fieldType,
                aTargetContact );
            }
        }
    }

// --------------------------------------------------------------------------
// CPbk2MergeAndSaveOperation::CPbk2MergeAndSaveOperation
// --------------------------------------------------------------------------
//
CPbk2MergeAndSaveOperation::CPbk2MergeAndSaveOperation(
        CPbk2ContactMerge& aContactMerge,
        MVPbkStoreContact& aTarget,
        MVPbkContactCopyObserver& aCopyObserver ) :
            CActive( EPriorityStandard ),
            iContactMerge( aContactMerge ),
            iTarget( aTarget ),
            iCopyObserver( aCopyObserver )
    {
    }

// --------------------------------------------------------------------------
// CPbk2MergeAndSaveOperation::~CPbk2MergeAndSaveOperation
// --------------------------------------------------------------------------
//
CPbk2MergeAndSaveOperation::~CPbk2MergeAndSaveOperation()
    {
    Cancel();
    delete iResults;
    iSourceContacts.ResetAndDestroy();
    }

// --------------------------------------------------------------------------
// CPbk2MergeAndSaveOperation::NewL
// --------------------------------------------------------------------------
//
CPbk2MergeAndSaveOperation* CPbk2MergeAndSaveOperation::NewL(
        CPbk2ContactMerge& aContactMerge,
        RPointerArray<MVPbkStoreContact>& aSourceContacts,
        MVPbkStoreContact& aTarget,
        MVPbkContactCopyObserver& aCopyObserver )
    {
    CPbk2MergeAndSaveOperation* self =
        new( ELeave ) CPbk2MergeAndSaveOperation( aContactMerge, aTarget,
            aCopyObserver );
    CleanupStack::PushL( self );
    self->ConstructL( aSourceContacts );
    CleanupStack::Pop( self );
    return self;
    }

// --------------------------------------------------------------------------
// CPbk2MergeAndSaveOperation::ConstructL
// --------------------------------------------------------------------------
//
void CPbk2MergeAndSaveOperation::ConstructL(
        RPointerArray<MVPbkStoreContact>& aSourceContacts )
    {
    CActiveScheduler::Add( this );
    iResults = CVPbkContactLinkArray::NewL();

    const TInt count = aSourceContacts.Count();
    for ( TInt i = count - 1; i >= 0; --i )
        {
        // Take ownerships of the contacts
        iSourceContacts.InsertL( aSourceContacts[i], 0 );
        aSourceContacts.Remove( i );
        }

    if ( iSourceContacts.Count() == 0 )
        {
        IssueRequest();
        }
    else
        {
        iTarget.LockL( *this );
        }
    }

// --------------------------------------------------------------------------
// CPbk2MergeAndSaveOperation::RunL
// --------------------------------------------------------------------------
//
void CPbk2MergeAndSaveOperation::RunL()
    {
    SendOperationCompleted();
    }

// --------------------------------------------------------------------------
// CPbk2MergeAndSaveOperation::DoCancel
// --------------------------------------------------------------------------
//
void CPbk2MergeAndSaveOperation::DoCancel()
    {
    }

// --------------------------------------------------------------------------
// CPbk2MergeAndSaveOperation::ContactOperationCompleted
// --------------------------------------------------------------------------
//
void CPbk2MergeAndSaveOperation::ContactOperationCompleted(
        TContactOpResult aResult )
    {
    TRAPD( res, HandleContactOperationL( aResult.iOpCode ) );
    if ( res != KErrNone )
        {
        iCopyObserver.ContactsSavingFailed( *this, res );
        }
    }

// --------------------------------------------------------------------------
// CPbk2MergeAndSaveOperation::ContactOperationFailed
// --------------------------------------------------------------------------
//
void CPbk2MergeAndSaveOperation::ContactOperationFailed(
        TContactOp /*aOpCode*/, TInt aErrorCode, TBool /*aErrorNotified*/ )
    {
    iCopyObserver.ContactsSavingFailed( *this, aErrorCode );
    }

// --------------------------------------------------------------------------
// CPbk2MergeAndSaveOperation::IssueRequest
// --------------------------------------------------------------------------
//
void CPbk2MergeAndSaveOperation::IssueRequest()
    {
    TRequestStatus* st = &iStatus;
    User::RequestComplete( st, KErrNone );
    SetActive();
    }

// --------------------------------------------------------------------------
// CPbk2MergeAndSaveOperation::MergeL
// --------------------------------------------------------------------------
//
void CPbk2MergeAndSaveOperation::MergeL()
    {
    const TInt fieldCount = iTarget.Fields().FieldCount();
    const TInt sourceCount = iSourceContacts.Count();
    for ( TInt i = sourceCount - 1; i >= 0; --i )
        {
        iContactMerge.MergeDataL( *iSourceContacts[i], iTarget );
        }

    // The logic of the merge and save for contact model contacts is
    // that if contact was merged somehow then the existing contact
    // is committed and source contacts deleted. If there was nothing to
    // merge then nothing is saved i.e. source contacts are ignored.
    if ( iTarget.Fields().FieldCount() > fieldCount )
        {
        // Commit the existing contact
        iTarget.CommitL( *this );
        }
    else
        {
        SendOperationCompleted();
        }
    }

// --------------------------------------------------------------------------
// CPbk2MergeAndSaveOperation::HandleContactOperationL
// --------------------------------------------------------------------------
//
void CPbk2MergeAndSaveOperation::HandleContactOperationL
        ( TContactOp aOpCode )
    {
    if ( aOpCode == EContactLock )
        {
        MergeL();
        }
    else if ( aOpCode == EContactCommit )
        {
        iResults->AppendL( iTarget.CreateLinkLC() );
        CleanupStack::Pop(); // link
        SendOperationCompleted();
        }
    else
        {
        // Should never come here
        iCopyObserver.ContactsSavingFailed( *this, KErrUnknown );
        }
    }

// --------------------------------------------------------------------------
// CPbk2MergeAndSaveOperation::SendOperationCompleted
// --------------------------------------------------------------------------
//
void CPbk2MergeAndSaveOperation::SendOperationCompleted()
    {
    CVPbkContactLinkArray* links = iResults;
    iResults = NULL;
    // Client takes the ownership of the links
    iCopyObserver.ContactsSaved( *this, links );
    }

// --------------------------------------------------------------------------
// CPbk2CopyTitlesOperation::CPbk2CopyTitlesOperation
// --------------------------------------------------------------------------
//
CPbk2CopyTitlesOperation::CPbk2CopyTitlesOperation(
        const MVPbkStoreContact& aSourceContact,
        MVPbkStoreContact& aTargetContact,
        MVPbkContactFieldsCopyObserver& aCopyObserver,
        MPbk2ContactNameFormatter& aNameFormatter ) :
            CActive( EPriorityStandard ),
            iSourceContact( aSourceContact ),
            iTargetContact( aTargetContact ),
            iCopyObserver( aCopyObserver ),
            iNameFormatter( aNameFormatter )
    {
    }

// --------------------------------------------------------------------------
// CPbk2CopyTitlesOperation::~CPbk2CopyTitlesOperation
// --------------------------------------------------------------------------
//
CPbk2CopyTitlesOperation::~CPbk2CopyTitlesOperation()
    {
    Cancel();
    }

// --------------------------------------------------------------------------
// CPbk2CopyTitlesOperation::NewL
// --------------------------------------------------------------------------
//
CPbk2CopyTitlesOperation* CPbk2CopyTitlesOperation::NewL(
        const MVPbkStoreContact& aSourceContact,
        MVPbkStoreContact& aTargetContact,
        MVPbkContactFieldsCopyObserver& aCopyObserver,
        MPbk2ContactNameFormatter& aNameFormatter )
    {
    CPbk2CopyTitlesOperation* self =
        new( ELeave ) CPbk2CopyTitlesOperation( aSourceContact, aTargetContact,
            aCopyObserver, aNameFormatter );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// --------------------------------------------------------------------------
// CPbk2CopyTitlesOperation::ConstructL
// --------------------------------------------------------------------------
//
void CPbk2CopyTitlesOperation::ConstructL()
    {
    CActiveScheduler::Add( this );

    Pbk2ContactFieldCopy::CopyTitleFieldsL( iSourceContact,
        iTargetContact, iNameFormatter );
    IssueRequest();
    }

// --------------------------------------------------------------------------
// CPbk2CopyTitlesOperation::RunL
// --------------------------------------------------------------------------
//
void CPbk2CopyTitlesOperation::RunL()
    {
    iCopyObserver.FieldsCopiedToContact( *this );
    }

// --------------------------------------------------------------------------
// CPbk2CopyTitlesOperation::DoCancel
// --------------------------------------------------------------------------
//
void CPbk2CopyTitlesOperation::DoCancel()
    {
    }

// --------------------------------------------------------------------------
// CPbk2CopyTitlesOperation::IssueRequest
// --------------------------------------------------------------------------
//
void CPbk2CopyTitlesOperation::IssueRequest()
    {
    TRequestStatus* st = &iStatus;
    User::RequestComplete( st, KErrNone );
    SetActive();
    }

} /// namespace


// --------------------------------------------------------------------------
// CPbk2ContactCopyPolicy::CPbk2ContactCopyPolicy
// --------------------------------------------------------------------------
//
CPbk2ContactCopyPolicy::CPbk2ContactCopyPolicy
        ( CVPbkContactManager& aContactManager ) :
            iContactManager(aContactManager)
    {
    }

// --------------------------------------------------------------------------
// CPbk2ContactCopyPolicy::~CPbk2ContactCopyPolicy
// --------------------------------------------------------------------------
//
CPbk2ContactCopyPolicy::~CPbk2ContactCopyPolicy()
    {
    delete iContactMerge;
    delete iFieldProperties;
    delete iNameFormatter;
    delete iSortOrderManager;
    }

// --------------------------------------------------------------------------
// CPbk2ContactCopyPolicy::NewL
// --------------------------------------------------------------------------
//
CPbk2ContactCopyPolicy* CPbk2ContactCopyPolicy::NewL( TParam* aParam )
    {
    CPbk2ContactCopyPolicy* self = new ( ELeave ) CPbk2ContactCopyPolicy
        ( aParam->iContactManager );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// --------------------------------------------------------------------------
// CPbk2ContactCopyPolicy::ConstructL
// --------------------------------------------------------------------------
//
inline void CPbk2ContactCopyPolicy::ConstructL()
    {

    iFieldProperties = CPbk2FieldPropertyArray::NewL
        ( iContactManager.FieldTypes(), iContactManager.FsSession() );
    
    RPbk2LocalizedResourceFile resFile( &iContactManager.FsSession() );
    resFile.OpenLC( KPbk2RomFileDrive, KDC_RESOURCE_FILES_DIR,
        KResourceFile );

    HBufC8* buffer = resFile.AllocReadLC( R_QTN_PHOB_SIM_NAME_COPY_TO_FIRST_NAME );

    _LIT8(KPbk2SimNameCopyFirstNameCompareStr, "*1*");
    if ( buffer->MatchF( KPbk2SimNameCopyFirstNameCompareStr ) == 0 )
        {
        iCopyNameToFirstName = ETrue;
        }
    CleanupStack::PopAndDestroy( 2 ); // buffer, resFile

    }

// --------------------------------------------------------------------------
// CPbk2ContactCopyPolicy::FieldProperties
// --------------------------------------------------------------------------
//
CPbk2FieldPropertyArray& CPbk2ContactCopyPolicy::FieldProperties() const
    {
    return *iFieldProperties;
    }

// --------------------------------------------------------------------------
// CPbk2ContactCopyPolicy::AddFieldToContactL
// --------------------------------------------------------------------------
//
MVPbkContactOperationBase* CPbk2ContactCopyPolicy::AddFieldToContactL(
        MVPbkStoreContact& aContact,
        const MVPbkFieldType& aFieldType,
        const TDateTime& aFieldData,
        const TDesC* aFieldLabel,
        const MVPbkContactAttribute* aContactAttribute,
        MVPbkContactFieldCopyObserver& aCopyObserver)
    {
    return CPbk2AddFieldOperation::NewL(*this, iContactManager, aContact,
            aFieldType, aFieldData, aFieldLabel, aContactAttribute,
            aCopyObserver);
    }

// --------------------------------------------------------------------------
// CPbk2ContactCopyPolicy::AddFieldToContactL
// --------------------------------------------------------------------------
//
MVPbkContactOperationBase* CPbk2ContactCopyPolicy::AddFieldToContactL(
        MVPbkStoreContact& aContact,
        const MVPbkFieldType& aFieldType,
        const TDesC& aFieldData,
        const TDesC* aFieldLabel,
        const MVPbkContactAttribute* aContactAttribute,
        MVPbkContactFieldCopyObserver& aCopyObserver)
    {
    return CPbk2AddFieldOperation::NewL(*this, iContactManager, aContact,
            aFieldType, aFieldData, aFieldLabel, aContactAttribute,
            aCopyObserver);
    }

// --------------------------------------------------------------------------
// CPbk2ContactCopyPolicy::AddFieldToContactL
// --------------------------------------------------------------------------
//
MVPbkContactOperationBase* CPbk2ContactCopyPolicy::AddFieldToContactL(
        MVPbkStoreContact& aContact,
        const MVPbkFieldType& aFieldType,
        const TDesC8& aFieldData,
        const TDesC* aFieldLabel,
        const MVPbkContactAttribute* aContactAttribute,
        MVPbkContactFieldCopyObserver& aCopyObserver)
    {
    return CPbk2AddFieldOperation::NewL(*this, iContactManager, aContact,
            aFieldType, aFieldData, aFieldLabel, aContactAttribute,
            aCopyObserver);
    }

// --------------------------------------------------------------------------
// CPbk2ContactCopyPolicy::CopyTitleFieldsL
// --------------------------------------------------------------------------
//
MVPbkContactOperationBase* CPbk2ContactCopyPolicy::CopyTitleFieldsL(
        const MVPbkStoreContact& aSourceContact,
        MVPbkStoreContact& aTargetContact,
        MVPbkContactFieldsCopyObserver& aCopyObserver)
    {
    return CPbk2CopyTitlesOperation::NewL(
        aSourceContact,
        aTargetContact,
        aCopyObserver,
        NameFormatterL() );
    }

// --------------------------------------------------------------------------
// CPbk2ContactCopyPolicy::CommitContactL
// --------------------------------------------------------------------------
//
MVPbkContactOperationBase* CPbk2ContactCopyPolicy::CommitContactL(
        MVPbkStoreContact* aContact,
        MVPbkContactCopyObserver& aCopyObserver)
    {
    return CPbk2SaveContactOperation::NewL(aContact, aCopyObserver,
            iContactManager.FsSession());
    }

// --------------------------------------------------------------------------
// CPbk2ContactCopyPolicy::CopyContactL
// --------------------------------------------------------------------------
//
MVPbkContactOperationBase* CPbk2ContactCopyPolicy::CopyContactL(
        MVPbkStoreContact& aContact,
        MVPbkContactStore& aTargetStore,
        MVPbkContactCopyObserver& aCopyObserver)
    {
    return CPbk2SaveContactOperation::NewL
        (aContact, aCopyObserver, aTargetStore,
        iContactManager.FsSession(), iCopyNameToFirstName );
    }

// --------------------------------------------------------------------------
// CPbk2ContactCopyPolicy::SupportsContactMerge
// --------------------------------------------------------------------------
//
TBool CPbk2ContactCopyPolicy::SupportsContactMerge() const
    {
    return ETrue;
    }

// --------------------------------------------------------------------------
// CPbk2ContactCopyPolicy::MergeAndSaveContactsL
// --------------------------------------------------------------------------
//
MVPbkContactOperationBase* CPbk2ContactCopyPolicy::MergeAndSaveContactsL(
        RPointerArray<MVPbkStoreContact>& aSourceContacts,
        MVPbkStoreContact& aTarget,
        MVPbkContactCopyObserver& aCopyObserver )
    {
    return CPbk2MergeAndSaveOperation::NewL( ContactMergeL(), aSourceContacts,
        aTarget, aCopyObserver );
    }

// --------------------------------------------------------------------------
// CPbk2ContactCopyPolicy::NameFormatterL
// --------------------------------------------------------------------------
//
MPbk2ContactNameFormatter& CPbk2ContactCopyPolicy::NameFormatterL()
    {
    if ( !iSortOrderManager )
        {
        iSortOrderManager = CPbk2SortOrderManager::NewL
            ( iContactManager.FieldTypes() );
        }

    if ( !iNameFormatter )
        {
        iNameFormatter = Pbk2ContactNameFormatterFactory::CreateL
            ( iContactManager.FieldTypes(), *iSortOrderManager,
              &iContactManager.FsSession() );
        }
    return *iNameFormatter;
    }

// --------------------------------------------------------------------------
// CPbk2ContactCopyPolicy::ContactMergeL
// --------------------------------------------------------------------------
//
CPbk2ContactMerge& CPbk2ContactCopyPolicy::ContactMergeL()
    {
    if ( !iContactMerge )
        {
        iContactMerge = CPbk2ContactMerge::NewL( iContactManager,
            NameFormatterL(), *iFieldProperties );
        }
    return *iContactMerge;
    }

// End of File
