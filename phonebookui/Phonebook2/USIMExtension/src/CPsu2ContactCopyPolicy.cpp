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
* Description:  A copy policy for copying to sim store
*
*/


// INCLUDE FILES
#include "CPsu2ContactCopyPolicy.h"

#include "CPsu2SimContactProcessor.h"
#include "CPsu2CopyToSimFieldInfoArray.h"

// From Phonebook 2 
#include <MPbk2FieldProperty.h>
#include <CPbk2FieldPropertyArray.h>
#include <CPbk2PresentationContact.h>
#include <CPbk2SortOrderManager.h>
#include <Pbk2ContactNameFormatterFactory.h>
#include <MPbk2ContactNameFormatter.h>
#include <CPbk2StorePropertyArray.h>
#include <Pbk2ContactFieldCopy.h>

// From Virtual Phonebook
#include <MVPbkContactFieldCopyObserver.h>
#include <CVPbkContactLinkArray.h>
#include <CVPbkContactManager.h>
#include <MVPbkContactFieldTextData.h>
#include <MVPbkFieldType.h>
#include <MVPbkContactStore.h>
#include <MVPbkStoreContactField.h>
#include <MVPbkContactStoreProperties.h>
#include <MVPbkContactObserver.h>
#include <MVPbkContactCopyObserver.h>
#include <MVPbkContactOperationBase.h>
#include <VPbkContactStoreUris.h>
#include <CVPbkFieldTypeRefsList.h>
#include <CVPbkContactFieldIterator.h>

namespace {

// ============================ LOCAL FUNCTIONS ================================

/**
* A dummy operation that completes the operation successfully
*/
NONSHARABLE_CLASS( CFieldNotSupportedOperation ):  public CActive,
                                          public MVPbkContactOperationBase
    {
    public: // Construction and destruction

        /**
        * Two-phased constructor.
        * @param aCopyObserver Copy observer
        * @return a new instance of this class
        */
        static CFieldNotSupportedOperation* NewL(
            MVPbkContactFieldCopyObserver& aCopyObserver);

        /**
        * Destructor
        */
        ~CFieldNotSupportedOperation();
        
    public: // Functions from base class

        /**
        * From CActive
        */
        void RunL();

        /**
        * From CActive
        */
        void DoCancel();
    
    private:   
        CFieldNotSupportedOperation(
            MVPbkContactFieldCopyObserver& aCopyObserver);
        void IssueRequest();

    private:    // Data
        MVPbkContactFieldCopyObserver& iCopyObserver;
    };

/**
* An operation that adds field to the sim contact
* if the conversion is found
*/
NONSHARABLE_CLASS( CAddFieldOperation ): public CActive,
                           public MVPbkContactOperationBase
    {
    public: // Construction and destruction
        
        /**
        * Two-phased constructor.
        * @param aCopyPolicy    Copy policy
        * @param aContact       Contact
        * @param aFieldType     Field type
        * @param aFieldData     Field data
        * @param aFieldLabel    Field label
        * @param aCopyObserver  Copy observer
        * @param aNameFormatter Name formatter
        * @return a new instance of this class
        */
        static CAddFieldOperation* NewL(
                CPsu2ContactCopyPolicy& aCopyPolicy,
                MVPbkStoreContact& aContact, 
                const MVPbkFieldType& aFieldType,
                const TDesC& aFieldData,
                const TDesC* aFieldLabel,
                MVPbkContactFieldCopyObserver& aCopyObserver,
                MPbk2ContactNameFormatter& aNameFormatter );
        
        ~CAddFieldOperation();
        
    public: // Functions from base classes

        /**
        * From CActive
        */
        void RunL();

        /**
        * From CActive
        */
        void DoCancel();

        /**
        * From CActive
        */
        TInt RunError(TInt aError);
                
    private: // Construction

        /**
        * C++ contructor
        */
        CAddFieldOperation(CPsu2ContactCopyPolicy& aCopyPolicy,
            MVPbkContactFieldCopyObserver& aCopyObserver,
            MPbk2ContactNameFormatter& aNameFormatter );

        /**
        * 2nd phase constructor
        */
        void ConstructL(
                MVPbkStoreContact& aContact, 
                const MVPbkFieldType& aFieldType,
                const TDesC& aFieldData,
                const TDesC* aFieldLabel);

    private:    // New functions
        void IssueRequest(TInt aResult);
        
    private: // Data
        /// Ref: Copy policy
        CPsu2ContactCopyPolicy& iCopyPolicy;
        /// Ref: copy observer
        MVPbkContactFieldCopyObserver& iCopyObserver;
        /// Ref: for title field copying
        MPbk2ContactNameFormatter& iNameFormatter;
        /// Own: Field to be added
        MVPbkStoreContactField* iField;
        /// Ref: contact to add the fields to
        MVPbkStoreContact* iContact;
    };

/**
* A class for commiting a sim contact. Splits contacts if necessary.
*/
NONSHARABLE_CLASS( CSaveContactOperation ): public CActive,
                                  public MVPbkContactOperationBase,
                                  public MVPbkContactObserver
    {
    public: // Construction and destruction
        /** 
         * Static constructor
         *
         * @param aContactToCommit      Store contact pointer, this class 
         *                              takes the ownership of a contact
         * @param aCopyObserver         Copy observer
         * @param aSimContactProcessor  Sim contact processor
         * @return New instance of this classs
         */
        static CSaveContactOperation* NewL(
                MVPbkStoreContact* aContactToCommit, 
                MVPbkContactCopyObserver& aCopyObserver,
                CPsu2SimContactProcessor& aSimContactProcessor);

        /** 
         * Static constructor
         *
         * @param aSourceContact        Store contact reference
         * @param aCopyObserver         Copy observer
         * @param aSimContactProcessor  Sim contact processor
         * @return New instance of this classs
         */
        static CSaveContactOperation* NewL(
                MVPbkStoreContact& aSourceContact, 
                MVPbkContactCopyObserver& aCopyObserver,
                CPsu2SimContactProcessor& aSimContactProcessor);

        /** 
         * Destructor
         */                
        ~CSaveContactOperation();
        
    public: // Functions from base classes

        /**
        * From CActive
        */
        void RunL();

        /**
        * From CActive
        */
        void DoCancel();

        /**
        * From CActive
        */
        TInt RunError(TInt aError);        
    
    public: // from MVPbkContactObserver
        void ContactOperationCompleted(TContactOpResult aResult);
        void ContactOperationFailed(TContactOp aOpCode, TInt aErrorCode, 
            TBool aErrorNotified);
                    
    private: // Implementation
        CSaveContactOperation(MVPbkContactCopyObserver& aCopyObserver,
            CPsu2SimContactProcessor& aSimContactProcessor);
        void ConstructL(MVPbkStoreContact& aContact);
        MVPbkStoreContact* NextSimContact();
        void HandleContactOperationCompletedL();
        void IssueRequest(TInt aResult);
        
    private: // Data
        /// Ref: copy observer
        MVPbkContactCopyObserver& iCopyObserver;
        CPsu2SimContactProcessor& iSimContactProcessor;
        /// Own: a new contact in the case of copying contact
        MVPbkStoreContact* iContact;
        /// Own: the current sim contact to copy
        MVPbkStoreContact* iCurrentSimContact;
        /// An array of 'to be copied' sim contacts
        RPointerArray<MVPbkStoreContact> iSimContacts;
        /// Own: for results
        CVPbkContactLinkArray* iLinkArray;
    };

/**
* A class for copy title operation
*/
NONSHARABLE_CLASS( CCopyTitlesOperation )
    :   public CActive,
        public MVPbkContactOperationBase
    {
    public: // Construction and destruction
        /**
         * Static constructor
         *
         * @param aSourceContact    Copy source store contact
         * @param aTargetContact    Copy target store contact 
         * @param aCopyObserver     Copy observer
         * @param aNameFormatter    Name formatter
         * @return New instance of this class
         */
        static CCopyTitlesOperation* NewL(
            const MVPbkStoreContact& aSourceContact,
            MVPbkStoreContact& aTargetContact,
            MVPbkContactFieldsCopyObserver& aCopyObserver,
            MPbk2ContactNameFormatter& aNameFormatter,
            CPsu2SimContactProcessor& aSimContactProcessor);
            
        /**
         * Destructor
         */
        ~CCopyTitlesOperation();
    
    private: // From CActive
        void RunL();
        void DoCancel();
        
    private:
        CCopyTitlesOperation( const MVPbkStoreContact& aSourceContact,
            MVPbkStoreContact& aTargetContact,
            MVPbkContactFieldsCopyObserver& aCopyObserver,
            MPbk2ContactNameFormatter& aNameFormatter,
            CPsu2SimContactProcessor& aSimContactProcessor);
        void ConstructL();
        void IssueRequest();
        
    private: // Data
        /// The source of the title fields
        const MVPbkStoreContact& iSourceContact;
        /// Target of the copy
        MVPbkStoreContact& iTargetContact;
        /// An observer for asyncrhonous operation
        MVPbkContactFieldsCopyObserver& iCopyObserver;
        /// The name formatter for title field checking 
        MPbk2ContactNameFormatter& iNameFormatter;
        CPsu2SimContactProcessor& iSimContactProcessor;
    };

// -----------------------------------------------------------------------------
// CFieldNotSupportedOperation::CFieldNotSupportedOperation
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CFieldNotSupportedOperation::CFieldNotSupportedOperation(
        MVPbkContactFieldCopyObserver& aCopyObserver)
        :   CActive(EPriorityStandard),
            iCopyObserver(aCopyObserver)
    {
    CActiveScheduler::Add(this);
    }

// -----------------------------------------------------------------------------
// CFieldNotSupportedOperation::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CFieldNotSupportedOperation* CFieldNotSupportedOperation::NewL(
        MVPbkContactFieldCopyObserver& aCopyObserver)
    {
    CFieldNotSupportedOperation* self= 
        new(ELeave) CFieldNotSupportedOperation(aCopyObserver);
    self->IssueRequest();
    return self;
    }

// Destructor
CFieldNotSupportedOperation::~CFieldNotSupportedOperation()
    {
    Cancel();
    }

// -----------------------------------------------------------------------------
// CFieldNotSupportedOperation::RunL
// -----------------------------------------------------------------------------
//    
void CFieldNotSupportedOperation::RunL()
    {
    iCopyObserver.FieldAddingFailed( *this, KErrNotSupported );
    }

// -----------------------------------------------------------------------------
// CFieldNotSupportedOperation::DoCancel
// -----------------------------------------------------------------------------
//    
void CFieldNotSupportedOperation::DoCancel()
    {
    // Notghing to cancel in the dummy operation
    }

// -----------------------------------------------------------------------------
// CFieldNotSupportedOperation::IssueRequest
// -----------------------------------------------------------------------------
//        
void CFieldNotSupportedOperation::IssueRequest()
    {
    TRequestStatus* st = &iStatus;
    User::RequestComplete(st, KErrNone);
    SetActive();
    }
    
    
// -----------------------------------------------------------------------------
// CAddFieldOperation::CAddFieldOperation
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//    
CAddFieldOperation::CAddFieldOperation(
        CPsu2ContactCopyPolicy& aCopyPolicy, 
        MVPbkContactFieldCopyObserver& aCopyObserver,
        MPbk2ContactNameFormatter& aNameFormatter) 
        :   CActive( EPriorityStandard ),
            iCopyPolicy(aCopyPolicy),
            iCopyObserver(aCopyObserver),
            iNameFormatter( aNameFormatter )
    {
    CActiveScheduler::Add(this);
    }

// -----------------------------------------------------------------------------
// CAddFieldOperation::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//  
CAddFieldOperation* CAddFieldOperation::NewL(
        CPsu2ContactCopyPolicy& aCopyPolicy,
        MVPbkStoreContact& aContact, 
        const MVPbkFieldType& aFieldType,
        const TDesC& aFieldData,
        const TDesC* aFieldLabel,
        MVPbkContactFieldCopyObserver& aCopyObserver,
        MPbk2ContactNameFormatter& aNameFormatter)
    {
    CAddFieldOperation* self = new(ELeave) CAddFieldOperation(
            aCopyPolicy, aCopyObserver, aNameFormatter );
    CleanupStack::PushL(self);
    self->ConstructL(
        aContact, aFieldType, aFieldData, aFieldLabel);
    CleanupStack::Pop(self);
    return self;    
    }

// -----------------------------------------------------------------------------
// CAddFieldOperation::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CAddFieldOperation::ConstructL(
        MVPbkStoreContact& aContact, 
        const MVPbkFieldType& aFieldType,
        const TDesC& aFieldData,
        const TDesC* aFieldLabel)
    {
    TInt opResult = KErrNone;
    iContact = &aContact;
    const MVPbkFieldType* simType = NULL;
    
    if ( iNameFormatter.IsTitleFieldType( aFieldType ) )
        {
        // Check it the contact already has a title field
        HBufC* curTitle = iNameFormatter.GetContactTitleOrNullL( 
            aContact.Fields(), 
            MPbk2ContactNameFormatter::EPreserveLeadingSpaces );
        if ( !curTitle )
            {
            // Contact didn't have title so add to the SIM's name type
            simType = &iCopyPolicy.SimFieldInfos().SimNameType();
            }
        delete curTitle;
        }
    else
        {
        // Type was not title type. Try to make conversion.
        simType = iCopyPolicy.SimFieldInfos().ConvertToSimType(aFieldType);    
        }
    
    const MVPbkContactStoreProperties& props = 
        aContact.ParentStore().StoreProperties();
    if (simType && props.SupportedFields().ContainsSame( *simType ) )
        {
        // If data is 0 then don't add field but complete with success
        if ( aFieldData.Length() > 0 )
            {
            CPbk2PresentationContact* cnt = 
                CPbk2PresentationContact::NewL(aContact, 
                    iCopyPolicy.FieldProperties());
            CleanupStack::PushL(cnt);
            if (cnt->IsFieldAdditionPossibleL( *simType ))
                {
                iField = aContact.CreateFieldLC( *simType );
                CleanupStack::Pop();
                if (aFieldLabel && iField->SupportsLabel())
                    {
                    iField->SetFieldLabelL(*aFieldLabel);
                    }
                MVPbkContactFieldTextData& data = 
                    MVPbkContactFieldTextData::Cast(iField->FieldData());
                data.SetTextL(aFieldData.Left(data.MaxLength()));
                }
            else
                {
                opResult = KErrOverflow;
                }
            CleanupStack::PopAndDestroy(cnt);    
            }
        }
    else
        {
        opResult = KErrNotSupported;
        }
    IssueRequest(opResult);
    }

// Destructor
CAddFieldOperation::~CAddFieldOperation()
    {
    delete iField;
    }

// -----------------------------------------------------------------------------
// CAddFieldOperation::RunL
// -----------------------------------------------------------------------------
//        
void CAddFieldOperation::RunL()
    {
    if (iField)
        {
        iContact->AddFieldL(iField);
        iField = NULL; // Ownership taken by iContact
        iCopyObserver.FieldAddedToContact(*this);
        }
    else if ( iStatus.Int() == KErrNone )
        {
        iCopyObserver.FieldAddedToContact(*this);
        }
    else
        {
        iCopyObserver.FieldAddingFailed(*this, iStatus.Int());
        }
    }

// -----------------------------------------------------------------------------
// CAddFieldOperation::DoCancel
// -----------------------------------------------------------------------------
//            
void CAddFieldOperation::DoCancel()
    {
    // Do nothing
    }

// -----------------------------------------------------------------------------
// CAddFieldOperation::RunError
// -----------------------------------------------------------------------------
//                
TInt CAddFieldOperation::RunError(TInt aError)
    {
    iCopyObserver.FieldAddingFailed(*this, aError);
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CAddFieldOperation::IssueRequest
// -----------------------------------------------------------------------------
//                    
void CAddFieldOperation::IssueRequest(TInt aResult)
    {
    TRequestStatus* status = &iStatus;
    User::RequestComplete(status, aResult);
    SetActive();
    }
    
// -----------------------------------------------------------------------------
// CSaveContactOperation::CSaveContactOperation
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//        
CSaveContactOperation::CSaveContactOperation(
        MVPbkContactCopyObserver& aCopyObserver,
        CPsu2SimContactProcessor& aSimContactProcessor)
        :   CActive( EPriorityStandard ),
            iCopyObserver(aCopyObserver),
            iSimContactProcessor(aSimContactProcessor)
    {
    CActiveScheduler::Add(this);
    }

CSaveContactOperation::~CSaveContactOperation()
    {
    Cancel();
    delete iLinkArray;
    delete iContact;
    delete iCurrentSimContact;
    iSimContacts.ResetAndDestroy();
    }

CSaveContactOperation* CSaveContactOperation::NewL(
        MVPbkStoreContact* aContactToCommit, 
        MVPbkContactCopyObserver& aCopyObserver,
        CPsu2SimContactProcessor& aSimContactProcessor)
    {
    CSaveContactOperation* self = new(ELeave) CSaveContactOperation(
        aCopyObserver, aSimContactProcessor);
    CleanupStack::PushL(self);
    self->ConstructL(*aContactToCommit);
    // Take ownership
    self->iContact = aContactToCommit;
    CleanupStack::Pop(self);
    return self;
    }

CSaveContactOperation* CSaveContactOperation::NewL(
        MVPbkStoreContact& aSourceContact, 
        MVPbkContactCopyObserver& aCopyObserver,
        CPsu2SimContactProcessor& aSimContactProcessor)
    {
    CSaveContactOperation* self = new(ELeave) CSaveContactOperation(
        aCopyObserver, aSimContactProcessor);
    CleanupStack::PushL(self);
    self->ConstructL(aSourceContact);
    CleanupStack::Pop(self);
    return self;
    }

// -----------------------------------------------------------------------------
// CSaveContactOperation::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CSaveContactOperation::ConstructL(MVPbkStoreContact& aContact)
    {
    iLinkArray = CVPbkContactLinkArray::NewL();
    iSimContactProcessor.CreateSimContactsL(aContact, iSimContacts);
    iCurrentSimContact = NextSimContact();
    if ( iCurrentSimContact )
        {
        iCurrentSimContact->CommitL(*this);         
        }
    else
        {
        // There is not any simcontacts which can be saved
        IssueRequest( KErrGeneral );
        }
    }
    
// -----------------------------------------------------------------------------
// CSaveContactOperation::RunL
// -----------------------------------------------------------------------------
//        
void CSaveContactOperation::RunL()
    {
    User::LeaveIfError( iStatus.Int() );
    }

// -----------------------------------------------------------------------------
// CSaveContactOperation::DoCancel
// -----------------------------------------------------------------------------
//            
void CSaveContactOperation::DoCancel()
    {
    // Do nothing
    }

// -----------------------------------------------------------------------------
// CSaveContactOperation::RunError
// -----------------------------------------------------------------------------
//                
TInt CSaveContactOperation::RunError(TInt aError)
    {
    ContactOperationFailed( 
        MVPbkContactObserver::EContactCommit, aError, EFalse );
    return KErrNone;
    }    

// -----------------------------------------------------------------------------
// CSaveContactOperation::ContactOperationCompleted
// -----------------------------------------------------------------------------
//    
void CSaveContactOperation::ContactOperationCompleted(
        TContactOpResult /*aResult*/)
    {
    // Do not access member data after HandleContactOperationCompletedL
    // because the operation might be deleted by observer if case has been 
    // handled succesfully.
    TRAPD( error, HandleContactOperationCompletedL() );
    if ( error != KErrNone )
        {
        // In case of an error it is safe to access member data.
        iCopyObserver.ContactsSavingFailed( *this, error );
        }
    }

// -----------------------------------------------------------------------------
// CSaveContactOperation::ContactOperationFailed
// -----------------------------------------------------------------------------
//    
void CSaveContactOperation::ContactOperationFailed(TContactOp /*aOpCode*/, 
        TInt aErrorCode, TBool /*aErrorNotified*/)
    {
    TInt res = KErrNone;
    if (iSimContactProcessor.HandleSimError(aErrorCode))
        {
        // If the error was handled it means that the SIM contact
        // must be splitted and resulted contacts must be copied.
        TRAP(res, iSimContactProcessor.CreateFixedSimContactsL(
            *iCurrentSimContact, iSimContacts));
        }
    else
        {
        res = aErrorCode;
        }
    
    // Destroy the contact that couldn't be saved
    delete iCurrentSimContact; 
    iCurrentSimContact = NULL;
    
    if (res != KErrNone)
        {
        iCopyObserver.ContactsSavingFailed(*this, res);
        }
    else if (iSimContacts.Count() > 0)
        {
        // There are still SIM contacts to copy
        iCurrentSimContact = NextSimContact();
        if ( iCurrentSimContact )
            {
            TRAPD( error, iCurrentSimContact->CommitL( *this ) );
            if ( error != KErrNone )
                {
                iCopyObserver.ContactsSavingFailed( *this, error );
                }            
            }
        }
    else
        {
        // Done with this contact 
        iCopyObserver.ContactsSaved(*this, iLinkArray);
        }
    
    }

// -----------------------------------------------------------------------------
// CSaveContactOperation::NextSimContact
// -----------------------------------------------------------------------------
//
MVPbkStoreContact* CSaveContactOperation::NextSimContact()
    {
    if (iSimContacts.Count() > 0)
        {
        const TInt firstPos = 0;
        MVPbkStoreContact* contact = iSimContacts[firstPos];
        iSimContacts.Remove(firstPos);
        return contact;
        }
    return NULL;
    }

// -----------------------------------------------------------------------------
// CSaveContactOperation::HandleContactOperationCompletedL
// -----------------------------------------------------------------------------
//      
void CSaveContactOperation::HandleContactOperationCompletedL()
    {
    MVPbkContactLink* link = iCurrentSimContact->CreateLinkLC();
    iLinkArray->AppendL( link );
    CleanupStack::Pop(); // link
    
    if (iSimContacts.Count() == 0)
        {
        MVPbkContactLinkArray* res = iLinkArray;
        iLinkArray = NULL;
        // Ownership of array is given to observer
        // Don't access any member after this because
        // observer can delete the operation.
        iCopyObserver.ContactsSaved( *this, res );
        }
    else
        {
        delete iCurrentSimContact;
        iCurrentSimContact = NextSimContact();
        if ( iCurrentSimContact )
            {
            iCurrentSimContact->CommitL( *this );            
            }
        }    
    }
    
// -----------------------------------------------------------------------------
// CSaveContactOperation::IssueRequest
// -----------------------------------------------------------------------------
//                    
void CSaveContactOperation::IssueRequest(TInt aResult)
    {
    TRequestStatus* status = &iStatus;
    User::RequestComplete(status, aResult);
    SetActive();
    }    
    
// -----------------------------------------------------------------------------
// CCopyTitlesOperation::CCopyTitlesOperation
// -----------------------------------------------------------------------------
//
CCopyTitlesOperation::CCopyTitlesOperation( 
        const MVPbkStoreContact& aSourceContact,
        MVPbkStoreContact& aTargetContact,
        MVPbkContactFieldsCopyObserver& aCopyObserver,
        MPbk2ContactNameFormatter& aNameFormatter,
        CPsu2SimContactProcessor& aSimContactProcessor )
        :   CActive( EPriorityStandard ),
            iSourceContact( aSourceContact ),
            iTargetContact( aTargetContact ),
            iCopyObserver( aCopyObserver ),
            iNameFormatter( aNameFormatter ),
            iSimContactProcessor(aSimContactProcessor)
    {
    }

// -----------------------------------------------------------------------------
// CCopyTitlesOperation::ConstructL
// -----------------------------------------------------------------------------
//    
void CCopyTitlesOperation::ConstructL()
    {
    CActiveScheduler::Add( this );
    
    // if copy from cnt model to sim
    if ((iSourceContact.ParentStore().StoreProperties().Uri().UriDes().CompareC(
            VPbkContactStoreUris::DefaultCntDbUri()) == 0)
        &&
        (iTargetContact.ParentStore().StoreProperties().Uri().UriDes().CompareC(
                VPbkContactStoreUris::SimGlobalAdnUri()) == 0)
        )
        {
        CVPbkFieldTypeRefsList* list = CVPbkFieldTypeRefsList::NewL();
        CleanupStack::PushL( list );
        // Get fields that actually a part of the formatted name
        CVPbkBaseContactFieldTypeListIterator* itr =
            iNameFormatter.ActualTitleFieldsLC( *list, iTargetContact.Fields() );
        
        // Get and delete the name field
        while ( itr->HasNext() )
            {
        	const MVPbkBaseContactField* fieldName = itr->Next();
        	
        	const MVPbkBaseContactFieldCollection& fields = iTargetContact.Fields();	
        	const TInt fieldCount = fields.FieldCount();
            
            for ( TInt i=0; i<fieldCount; i++ ) 
            	{
            	if( &fields.FieldAt(i) == fieldName )
            		{
            		iTargetContact.RemoveField(i);
            		break;
            		}
            	}
            }
        CleanupStack::PopAndDestroy(2, list);	// list, itr
        // Add the new name field
        iSimContactProcessor.AddNameFieldsL(
            *(const_cast<MVPbkStoreContact*>(&iSourceContact)), iTargetContact);
        }
    else
    	{
    	Pbk2ContactFieldCopy::CopyTitleFieldsL( iSourceContact,
    			iTargetContact, iNameFormatter );
    	}

    IssueRequest();
    }

// -----------------------------------------------------------------------------
// CCopyTitlesOperation::NewL
// -----------------------------------------------------------------------------
//        
CCopyTitlesOperation* CCopyTitlesOperation::NewL(
        const MVPbkStoreContact& aSourceContact,
        MVPbkStoreContact& aTargetContact,
        MVPbkContactFieldsCopyObserver& aCopyObserver,
        MPbk2ContactNameFormatter& aNameFormatter,
        CPsu2SimContactProcessor& aSimContactProcessor )
    {
    CCopyTitlesOperation* self = 
        new( ELeave ) CCopyTitlesOperation( aSourceContact, aTargetContact, 
    		aCopyObserver, aNameFormatter, aSimContactProcessor );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// CCopyTitlesOperation::~CCopyTitlesOperation
// -----------------------------------------------------------------------------
//            
CCopyTitlesOperation::~CCopyTitlesOperation()
    {
    Cancel();
    }

// -----------------------------------------------------------------------------
// CCopyTitlesOperation::RunL
// -----------------------------------------------------------------------------
//                
void CCopyTitlesOperation::RunL()
    {
    iCopyObserver.FieldsCopiedToContact( *this );
    }

// -----------------------------------------------------------------------------
// CCopyTitlesOperation::DoCancel
// -----------------------------------------------------------------------------
//                    
void CCopyTitlesOperation::DoCancel()
    {
    // Do nothing
    }

// -----------------------------------------------------------------------------
// CCopyTitlesOperation::IssueRequest
// -----------------------------------------------------------------------------
//                            
void CCopyTitlesOperation::IssueRequest()
    {
    TRequestStatus* st = &iStatus;
    User::RequestComplete( st, KErrNone );
    SetActive();
    }
} // namespace


// ============================ MEMBER FUNCTIONS ===============================


// -----------------------------------------------------------------------------
// CPsu2ContactCopyPolicy::CPsu2ContactCopyPolicy
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CPsu2ContactCopyPolicy::CPsu2ContactCopyPolicy(
        CVPbkContactManager& aContactManager) 
        :   iContactManager(aContactManager)
    {
    }

// -----------------------------------------------------------------------------
// CPsu2ContactCopyPolicy::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
inline void CPsu2ContactCopyPolicy::ConstructL()
    {
    iPropertyArray = 
        CPbk2FieldPropertyArray::NewL(iContactManager.FieldTypes(),
            iContactManager.FsSession() );
    iSimFieldInfos = 
        CPsu2CopyToSimFieldInfoArray::NewL(iContactManager.FieldTypes(),
        iContactManager.FsSession() );
    iSortOrderManager = CPbk2SortOrderManager::NewL(
        iContactManager.FieldTypes());
    
    iNameFormatter = Pbk2ContactNameFormatterFactory::CreateL(
        iContactManager.FieldTypes(),
        *iSortOrderManager,
        &iContactManager.FsSession() );
    }

// -----------------------------------------------------------------------------
// CPsu2ContactCopyPolicy::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CPsu2ContactCopyPolicy* CPsu2ContactCopyPolicy::NewL(TParam* aParam)
    {
    CPsu2ContactCopyPolicy* self = 
        new(ELeave) CPsu2ContactCopyPolicy(aParam->iContactManager);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

// Destructor
CPsu2ContactCopyPolicy::~CPsu2ContactCopyPolicy()
    {
    delete iPropertyArray;
    delete iSimFieldInfos;
    delete iNameFormatter;
    delete iSortOrderManager;
    delete iSimContactProcessor;
    }

// -----------------------------------------------------------------------------
// CPsu2ContactCopyPolicy::AddFieldToContactL
// -----------------------------------------------------------------------------
//
MVPbkContactOperationBase* CPsu2ContactCopyPolicy::AddFieldToContactL(
                MVPbkStoreContact& /*aContact*/, 
                const MVPbkFieldType& /*aFieldType*/,
                const TDateTime& /*aFieldData*/,
                const TDesC* /*aFieldLabel*/,
                const MVPbkContactAttribute* /*aContactAttribute*/,
                MVPbkContactFieldCopyObserver& aCopyObserver)
    {
    return CFieldNotSupportedOperation::NewL(aCopyObserver);
    }

// -----------------------------------------------------------------------------
// CPsu2ContactCopyPolicy::AddFieldToContactL
// -----------------------------------------------------------------------------
//    
MVPbkContactOperationBase* CPsu2ContactCopyPolicy::AddFieldToContactL(
        MVPbkStoreContact& aContact, 
        const MVPbkFieldType& aFieldType,
        const TDesC& aFieldData,
        const TDesC* aFieldLabel,
        const MVPbkContactAttribute* /*aContactAttribute*/,
        MVPbkContactFieldCopyObserver& aCopyObserver)
    {            
    // Attributes are not supported by store
    return CAddFieldOperation::NewL( *this, aContact, aFieldType, aFieldData, 
        aFieldLabel, aCopyObserver, *iNameFormatter );
    }

// -----------------------------------------------------------------------------
// CPsu2ContactCopyPolicy::AddFieldToContactL
// -----------------------------------------------------------------------------
//    
MVPbkContactOperationBase* CPsu2ContactCopyPolicy::AddFieldToContactL(
        MVPbkStoreContact& /*aContact*/, 
        const MVPbkFieldType& /*aFieldType*/,
        const TDesC8& /*aFieldData*/,
        const TDesC* /*aFieldLabel*/,
        const MVPbkContactAttribute* /*aContactAttribute*/,
        MVPbkContactFieldCopyObserver& aCopyObserver)
    {
    return CFieldNotSupportedOperation::NewL(aCopyObserver);
    }

// -----------------------------------------------------------------------------
// CPsu2ContactCopyPolicy::CopyTitleFieldsL
// -----------------------------------------------------------------------------
//
MVPbkContactOperationBase* CPsu2ContactCopyPolicy::CopyTitleFieldsL(
        const MVPbkStoreContact& aSourceContact,
        MVPbkStoreContact& aTargetContact,
        MVPbkContactFieldsCopyObserver& aCopyObserver )
    {
    ReplaceSimContactProcessorL(aTargetContact.ParentStore());

    return CCopyTitlesOperation::NewL(
        aSourceContact,
        aTargetContact,
        aCopyObserver,
        *iNameFormatter,
        *iSimContactProcessor );
    }
    
// -----------------------------------------------------------------------------
// CPsu2ContactCopyPolicy::CommitContactL
// -----------------------------------------------------------------------------
//
MVPbkContactOperationBase* CPsu2ContactCopyPolicy::CommitContactL(
        MVPbkStoreContact* aContact,
        MVPbkContactCopyObserver& aCopyObserver)
    {
    CleanupDeletePushL(aContact);
    ReplaceSimContactProcessorL(aContact->ParentStore());
    // Takes ownership of aContact
    MVPbkContactOperationBase* op = CSaveContactOperation::NewL(
        aContact, aCopyObserver, *iSimContactProcessor);
    CleanupStack::Pop(); // aContact
    return op;
    }
    
// -----------------------------------------------------------------------------
// CPsu2ContactCopyPolicy::CopyContactL
// -----------------------------------------------------------------------------
//                
MVPbkContactOperationBase* CPsu2ContactCopyPolicy::CopyContactL(
        MVPbkStoreContact& aContact,
        MVPbkContactStore& aTargetStore,
        MVPbkContactCopyObserver& aCopyObserver)
    {
    ReplaceSimContactProcessorL(aTargetStore);
    return CSaveContactOperation::NewL(
        aContact, aCopyObserver, *iSimContactProcessor);
    }

// -----------------------------------------------------------------------------
// CPsu2ContactCopyPolicy::SupportsContactMerge
// -----------------------------------------------------------------------------
//                           
TBool CPsu2ContactCopyPolicy::SupportsContactMerge() const
    {
    return EFalse;
    }

// -----------------------------------------------------------------------------
// CPsu2ContactCopyPolicy::MergeAndSaveContactsL
// -----------------------------------------------------------------------------
//                           
MVPbkContactOperationBase* CPsu2ContactCopyPolicy::MergeAndSaveContactsL(
        RPointerArray<MVPbkStoreContact>& /*aSourceContacts*/, 
        MVPbkStoreContact& /*aTarget*/,
        MVPbkContactCopyObserver& /*aCopyObserver*/ )
    {
    // Not specified for SIM contacts
    User::Leave( KErrNotSupported );
    return NULL;    
    }
    
// -----------------------------------------------------------------------------
// CPsu2ContactCopyPolicy::ReplaceSimContactProcessorL
// -----------------------------------------------------------------------------
//                           
void CPsu2ContactCopyPolicy::ReplaceSimContactProcessorL(
        MVPbkContactStore& aTargetStore) 
    {
    if (!iSimContactProcessor || 
            aTargetStore.StoreProperties().Uri().Compare(
            iSimContactProcessor->TargetStore().StoreProperties().Uri(),
            TVPbkContactStoreUriPtr::EContactStoreUriAllComponents) != 0)
        {
        delete iSimContactProcessor;
        iSimContactProcessor = NULL;
        iSimContactProcessor = CPsu2SimContactProcessor::NewL(aTargetStore, 
            *iSimFieldInfos, *iNameFormatter, iContactManager.FieldTypes(), 
            iContactManager.FsSession() );
        }
    }


// End of File
