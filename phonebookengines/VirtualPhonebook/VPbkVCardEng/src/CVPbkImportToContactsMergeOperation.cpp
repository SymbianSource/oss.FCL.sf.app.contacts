/*
* Copyright (c) 2007-2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  An operation for importing and updating contacts to the store
*
*/


#include "CVPbkImportToContactsMergeOperation.h"
#include "VPbkVCardEngError.h"
#include "CVPbkVCardImporter.h"

// Virtual Phonebook
#include <MVPbkStoreContact.h>
#include <MVPbkContactStore.h>
#include <MVPbkContactLink.h>
#include <CVPbkVCardEng.h>
#include <CVPbkContactManager.h>
#include <MVPbkFieldType.h>
#include <MVPbkContactFieldData.h>
#include <MVPbkContactFieldUriData.h>
#include <MVPbkContactFieldTextData.h>
#include <MVPbkSingleContactOperationObserver.h>
#include "CVPbkDefaultAttribute.h"
const TInt KGranularity = 4;

namespace{
    #ifdef _DEBUG
       
    enum TPanic
        {
        EPanicInvalidState = 1,
        EPanicInvalidToRetrieve,
        EPanicInvalidToLock,
        EPanicInvalidToCommit,
        EPanicInvalidToUpdate,
        };

    void Panic(TPanic aPanic)
        {
        _LIT(KPanicCat, "CVPbkImportToContactsMergeOperation");
        User::Panic(KPanicCat, aPanic);
        }
    
    #endif // _DEBUG
} //namespace

// ---------------------------------------------------------------------------
// CVPbkImportToContactsMergeOperation::CVPbkImportToContactsMergeOperation
// ---------------------------------------------------------------------------
//
CVPbkImportToContactsMergeOperation::CVPbkImportToContactsMergeOperation(
        const MVPbkContactLink& aReplaceContact,
        MVPbkSingleContactOperationObserver& aObserver,
        MVPbkContactStore& aTargetStore,
        CVPbkVCardData& aData
        )
        :   CActive( EPriorityStandard ),
            iState ( EImport ),
            iTargetStore( aTargetStore ),
            iReplaceContact( aReplaceContact ),
            iObserver( aObserver ),
            iData(aData)
    {
    CActiveScheduler::Add(this);
    iGroupcardHandler = NULL;
    iOperationImpl = NULL;
    iVPbkOperation = NULL;
    }


// ---------------------------------------------------------------------------
// CVPbkImportToContactsMergeOperation::ConstructL
// ---------------------------------------------------------------------------
//
void CVPbkImportToContactsMergeOperation::ConstructL( TVPbkImportCardType aType, RReadStream& aSourceStream)
    {

    if ( aType == EVCard )
        {
        iOperationImpl = CVPbkVCardImporter::NewL(iImportedContacts, aSourceStream, iTargetStore, iData );
        }
    else
        {
        User::Leave( KErrArgument );
        }    
    iGroupcardHandler = CVPbkGroupCardHandler::NewL(iData);   
    }

// ---------------------------------------------------------------------------
// CVPbkImportToContactsMergeOperation::NewL
// ---------------------------------------------------------------------------
//
CVPbkImportToContactsMergeOperation * CVPbkImportToContactsMergeOperation::NewL( 
             TVPbkImportCardType aType,
             CVPbkVCardData& aData, 
             const MVPbkContactLink& aReplaceContact, 
             MVPbkContactStore& aTargetStore, 
             RReadStream& aSourceStream, 
             MVPbkSingleContactOperationObserver& aObserver )
    {
    CVPbkImportToContactsMergeOperation* self =
    new( ELeave ) CVPbkImportToContactsMergeOperation( aReplaceContact,aObserver, aTargetStore, aData);
    CleanupStack::PushL( self );
    self->ConstructL( aType, aSourceStream);
    CleanupStack::Pop( self );
    return self;

    }


// ---------------------------------------------------------------------------
// CVPbkImportToContactsMergeOperation::~CVPbkImportToContactsMergeOperation
// ---------------------------------------------------------------------------
//
CVPbkImportToContactsMergeOperation::~CVPbkImportToContactsMergeOperation()
    {

    if(iOperationImpl)
        {
        delete iOperationImpl;
        iOperationImpl = NULL;
        }
    if (iGroupcardHandler)
        {
        delete iGroupcardHandler;
        iGroupcardHandler = NULL;
        }
    if(iVPbkOperation)
        delete iVPbkOperation;
    if( iSetAttributeOperation )
        {
        delete iSetAttributeOperation;
        iSetAttributeOperation = NULL;
        }
    if( iDefaultAttributes )
        {
        delete iDefaultAttributes;
        iDefaultAttributes = NULL;
        }
    iImportedContacts.ResetAndDestroy();

    CActive::Cancel();
    }

void CVPbkImportToContactsMergeOperation::StartL()
    {
    __ASSERT_DEBUG( iOperationImpl, VPbkVCardEngError::Panic(
        VPbkVCardEngError::EPreCond_CVPbkImportToStoreOperation_StartL ) );

    iOperationImpl->SetObserver( *this );
    iOperationImpl->StartL();
    }

// ---------------------------------------------------------------------------
// From class MVPbkContactOperation
// CVPbkImportToStoreOperation::Cancel
// ---------------------------------------------------------------------------
//

void CVPbkImportToContactsMergeOperation::Cancel()
    {
    if(iOperationImpl)
    delete iOperationImpl;
    iOperationImpl = NULL;
    }

// ---------------------------------------------------------------------------
// CVPbkImportToContactsMergeOperation::RunL
// from CActive
// ---------------------------------------------------------------------------
//
void CVPbkImportToContactsMergeOperation::RunL()
    {
    if ( iStatus == KErrNone)
        {
        switch( iState )
            {
            case ERetrieve:
                {
                RetrieveContactL();
                break;
                }
            case    ELock:
                {
                LockContactL();
                break;
                }
            case EUpdateContact:
                {
                UpdateContactL();
                break;
                }
            case EReplaceFields:
                {
                ReplaceFieldL( *(iImportedContacts[0]), *iContact );
                break;
                }
            case ESetAttributes:
                {
                SetNextL();
                break;
                }
            case ECommit:
                {
                CommitContactL();
                break;
                }
            case EReRetrieve:
                {
                ReRetrieveContactL();
                break;
                }
            case EComplete:
                {
                if(iGroupcardHandler && ((CVPbkVCardImporter *)iOperationImpl)->IsGroupcard())
                    {
                    iGroupcardHandler->BuildContactGroupsHashMapL(iContact->ParentStore());
                    const MVPbkContactLink* contact = iContact->CreateLinkLC();
                    iGroupcardHandler->GetContactGroupStoreL(*contact);
                    CleanupStack::PopAndDestroy(); // For contact
                    iGroupcardHandler->DecodeContactGroupInVCardL(((CVPbkVCardImporter *)iOperationImpl)->GetGroupcardvalue());
                    }
                iObserver.VPbkSingleContactOperationComplete( *this, iContact );
                iContact = NULL;
                break;
                }
            default:
                __ASSERT_DEBUG( EFalse, Panic(EPanicInvalidState) );
            }
        }
    }

// ---------------------------------------------------------------------------
// CVPbkImportToContactsMergeOperation::RunError
// from CActive
// ---------------------------------------------------------------------------
//
TInt CVPbkImportToContactsMergeOperation::RunError( TInt aError )
    {
    HandleError( aError );
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CVPbkImportToContactsMergeOperation::DoCancel
// from CActive
// ---------------------------------------------------------------------------
//
void CVPbkImportToContactsMergeOperation::DoCancel()
    {
    delete iOperationImpl;
    iOperationImpl = NULL;
    }

// ---------------------------------------------------------------------------
// From class MVPbkImportOperationObserver
// CVPbkImportToStoreOperation::ContactsImported
// ---------------------------------------------------------------------------
//
void CVPbkImportToContactsMergeOperation::ContactsImported()
    {
    if (iVPbkOperation)
        delete iVPbkOperation;
    iVPbkOperation = NULL;

    //At the Start of contact replace,aContact will be Null, it will be retrived later
    HandleContactReplacing(NULL); 
    }

// ---------------------------------------------------------------------------
// From class MVPbkImportOperationObserver
// CVPbkImportToStoreOperation::ContactsImportedCompleted
// ---------------------------------------------------------------------------
//
void CVPbkImportToContactsMergeOperation::ContactsImportingCompleted()
    {
    //not used
    }

// ---------------------------------------------------------------------------
// From class MVPbkImportOperationObserver
// CVPbkImportToStoreOperation::ContactImportingFailed
// ---------------------------------------------------------------------------
//
void CVPbkImportToContactsMergeOperation::ContactImportingFailed( TInt aError )
    {
    if(iVPbkOperation)
        delete iVPbkOperation;
    iVPbkOperation = NULL;

    HandleError( aError );
    }

// ---------------------------------------------------------------------------
// CVPbkImportToContactsMergeOperation::HandleContactReplacing
// To initiate and start contact replacing
// ---------------------------------------------------------------------------
//
void CVPbkImportToContactsMergeOperation::HandleContactReplacing(MVPbkStoreContact* aContact)
    {
    switch( iState )
        {
        case EImport:
            {
            // initiate contact replacing
            if ( iImportedContacts.Count() == 1 )
                {
                NextState( ERetrieve );
                }
            else
                {
                // invalid arguments
                HandleError( KErrArgument );
                }
            break;
            }
        case ERetrieve:
            {
            delete iContact;
            iContact = aContact;
            NextState( ELock );
            break;
            }
        case EReRetrieve:
            {
            delete iContact;
            iContact = aContact;
            NextState( EComplete );
            break;
            }
        default:
            __ASSERT_DEBUG( EFalse, Panic(EPanicInvalidState) );
            HandleError( KErrGeneral );
        }
    }
// ---------------------------------------------------------------------------
// CVPbkImportToContactsMergeOperation::VPbkSingleContactOperationComplete
// from class MVPbkSingleContactOperationObserver
// ---------------------------------------------------------------------------
//
void CVPbkImportToContactsMergeOperation::VPbkSingleContactOperationComplete(
                MVPbkContactOperationBase& /*aOperation*/,
                MVPbkStoreContact* aContact )
    {
    if (iVPbkOperation)
        delete iVPbkOperation;
    iVPbkOperation = NULL;

    HandleContactReplacing(aContact);
    }

// ---------------------------------------------------------------------------
// CVPbkImportToContactsMergeOperation::VPbkSingleContactOperationFailed
// from class MVPbkSingleContactOperationObserver
// ---------------------------------------------------------------------------
//
void CVPbkImportToContactsMergeOperation::VPbkSingleContactOperationFailed(
                MVPbkContactOperationBase& /*aOperation*/, 
                TInt aError )
    {
    delete iVPbkOperation;
    iVPbkOperation = NULL;

    HandleError( aError );
    }

// ---------------------------------------------------------------------------
// CVPbkImportToContactsMergeOperation::ContactOperationCompleted
// from class MVPbkContactObserver
// ---------------------------------------------------------------------------
//
void CVPbkImportToContactsMergeOperation::ContactOperationCompleted(
        TContactOpResult aResult)
    {
    delete aResult.iStoreContact;
    switch( iState )
        {
        case ELock:
            {
            NextState( EUpdateContact );
            break;
            }
        case ECommit:
            {
            NextState( EReRetrieve );
            break;
            }
        default:
            __ASSERT_DEBUG( EFalse, Panic(EPanicInvalidState) );
        }
    }

// ---------------------------------------------------------------------------
// CVPbkImportToContactsMergeOperation::ContactOperationFailed
// from class MVPbkContactObserver
// ---------------------------------------------------------------------------
//
void CVPbkImportToContactsMergeOperation::ContactOperationFailed(
        TContactOp    /*aOpCode*/, TInt aErrorCode, TBool /*aErrorNotified*/)
    {
    HandleError( aErrorCode );
    }

// ---------------------------------------------------------------------------
// CVPbkImportToContactsMergeOperation::HandleError
// ---------------------------------------------------------------------------
//
void CVPbkImportToContactsMergeOperation::HandleError(TInt aError)
    {
    iObserver.VPbkSingleContactOperationFailed( *this, aError );
    }

// ---------------------------------------------------------------------------
// CVPbkImportToContactsMergeOperation::RetrieveContactL
// ---------------------------------------------------------------------------
//
inline void CVPbkImportToContactsMergeOperation::RetrieveContactL()
    {
    iVPbkOperation = iData.GetContactManager().RetrieveContactL(iReplaceContact,*this );
    }

// ---------------------------------------------------------------------------
// CVPbkImportToContactsMergeOperation::ReRetrieveContactL
// ---------------------------------------------------------------------------
//
inline void CVPbkImportToContactsMergeOperation::ReRetrieveContactL()
    {
    __ASSERT_DEBUG( iContact, Panic(EPanicInvalidToRetrieve) );
    
    MVPbkContactLink* link = iContact->CreateLinkLC(); 
    iVPbkOperation = iData.GetContactManager().RetrieveContactL( *link, *this );
    CleanupStack::PopAndDestroy();
    }

// ---------------------------------------------------------------------------
// CVPbkImportToContactsMergeOperation::LockContactL
// ---------------------------------------------------------------------------
//
inline void CVPbkImportToContactsMergeOperation::LockContactL()
    {
    __ASSERT_DEBUG( iContact, Panic(EPanicInvalidToLock) );
    iContact->LockL( *this );
    }

// ---------------------------------------------------------------------------
// CVPbkImportToContactsMergeOperation::CommitContactL
// ---------------------------------------------------------------------------
//
inline void CVPbkImportToContactsMergeOperation::CommitContactL()
    {
    __ASSERT_DEBUG( iContact, Panic(EPanicInvalidToCommit) );
    
    iContact->CommitL( *this );
    }

// ---------------------------------------------------------------------------
// CVPbkImportToContactsMergeOperation::UpdateContactL
// ---------------------------------------------------------------------------
//
inline void CVPbkImportToContactsMergeOperation::UpdateContactL()
    {
    __ASSERT_DEBUG( iContact, Panic(EPanicInvalidToUpdate) );
    __ASSERT_DEBUG( iImportedContacts.Count()>0, Panic(EPanicInvalidToUpdate) );
    __ASSERT_DEBUG( iImportedContacts[0], Panic(EPanicInvalidToUpdate) );
    iContact->RemoveAllFields();
    iCurrentFieldIndex = 0;
    iFieldCount = iImportedContacts[0]->Fields().FieldCount();

    ReplaceFieldL( *(iImportedContacts[0]), *iContact );
    }

// ---------------------------------------------------------------------------
// From class MVPbkImportOperationObserver
// ---------------------------------------------------------------------------
//
void CVPbkImportToContactsMergeOperation::ReplaceFieldL(
        const MVPbkStoreContact& aSrc,
        MVPbkStoreContact& aTarget )
    {
    if( iCurrentFieldIndex < iFieldCount )
        {
        const MVPbkStoreContactFieldCollection& sourceFields = aSrc.Fields();
        const MVPbkFieldType* type = sourceFields.FieldAt(iCurrentFieldIndex).BestMatchingFieldType();
        if ( type )
            {
            TRAPD( err,
                CopyFieldL( sourceFields.FieldAt(iCurrentFieldIndex), *type, aTarget );
                // Check whether the sorce field has attribute,
                // If the sorce field has attribute, set attribute to the target contact's field
                TInt type = EVPbkDefaultTypeUndefined + 1;
                if( iData.AttributeManagerL().HasFieldAttributeL( CVPbkDefaultAttribute::Uid(), 
                    sourceFields.FieldAt(iCurrentFieldIndex) ) )
                    {
                    GetDefaultAttributsL( sourceFields.FieldAt(iCurrentFieldIndex) );
                    if( iDefaultAttributes && iDefaultAttributes->Count() > 0 )
                        {
                        SetNextL();
                        }
                    }
                else
                    {
                    if( ++iCurrentFieldIndex < iFieldCount )
                        {
                        ReplaceFieldL(aSrc, aTarget);
                        }
                    else
                        {
                        delete iImportedContacts[0];
                        iImportedContacts.Remove(0);
                        NextState( ECommit );
                        }
                    }
                );
            if( err != KErrNone )
                {
                delete iImportedContacts[0];
                iImportedContacts.Remove(0);
                HandleError( err );
                }
            }
        }
    else
        {
        delete iImportedContacts[0];
        iImportedContacts.Remove(0);
        NextState( ECommit );
        }
    }

// ---------------------------------------------------------------------------
// CVPbkImportToContactsMergeOperation::CopyFieldL
// ---------------------------------------------------------------------------
//
inline void CVPbkImportToContactsMergeOperation::CopyFieldL( 
        const MVPbkStoreContactField& aSourceField,
        const MVPbkFieldType& aType,
        MVPbkStoreContact& aTargetContact )
    {
    MVPbkStoreContactField* newField = aTargetContact.CreateFieldLC( aType );
    if ( newField->SupportsLabel() )
        {
        TPtrC fieldLabel = aSourceField.FieldLabel();
        if (fieldLabel.Length() > 0)
            {
            newField->SetFieldLabelL(fieldLabel);
            }
        }
    newField->FieldData().CopyL(aSourceField.FieldData());
    aTargetContact.AddFieldL(newField);
    CleanupStack::Pop(); // newField
    }

// --------------------------------------------------------------------------
// CVPbkImportToContactsMergeOperation::GetDefaultAttributsL
// Gets the attributes from aField.
// --------------------------------------------------------------------------
//
void CVPbkImportToContactsMergeOperation::GetDefaultAttributsL( const MVPbkStoreContactField& aField )
    {
    delete iDefaultAttributes;
    iDefaultAttributes = NULL;
    iDefaultAttributes = new( ELeave ) CArrayFixFlat<TVPbkDefaultType>( KGranularity );
    TInt type = EVPbkDefaultTypeUndefined + 1;
    while( EVPbkLastDefaultType != type )
        {
        CVPbkDefaultAttribute* attr = CVPbkDefaultAttribute::NewL( (TVPbkDefaultType)type );
        CleanupStack::PushL( attr );

        if( iData.AttributeManagerL().HasFieldAttributeL( *attr, aField ) )
            {
            iDefaultAttributes->AppendL( (TVPbkDefaultType)type );
            }
        
        CleanupStack::PopAndDestroy( attr );
        type++;
        }
    }

// --------------------------------------------------------------------------
// CVPbkImportToContactsMergeOperation::NextAttribute
// Gets next attribute from the array. The array is processed backwards.
// --------------------------------------------------------------------------
//
inline TVPbkDefaultType CVPbkImportToContactsMergeOperation::NextAttribute()
    {
    TVPbkDefaultType attribute = EVPbkDefaultTypeUndefined;
    TInt count = 0;
    if ( iDefaultAttributes )
        {
        count = iDefaultAttributes->Count();
        }

    if ( count > 0 )
        {
        attribute = iDefaultAttributes->At( count - 1 ); // zero-based
        iDefaultAttributes->Delete( count -1 ); // zero-based
        }
    return attribute;
    }

// --------------------------------------------------------------------------
// CPbk2DefaultAttributeProcess::SetNextL
// --------------------------------------------------------------------------
//
void CVPbkImportToContactsMergeOperation::SetNextL()
    {
    TVPbkDefaultType attributeType = NextAttribute();
    if ( attributeType != EVPbkDefaultTypeUndefined )
        {
        delete iSetAttributeOperation;
        iSetAttributeOperation = NULL;
        CVPbkDefaultAttribute* attr = CVPbkDefaultAttribute::NewL( attributeType );
        CleanupStack::PushL( attr );
        iSetAttributeOperation = iData.AttributeManagerL().SetFieldAttributeL(iContact->Fields().FieldAt(iCurrentFieldIndex), *attr, *this);
        CleanupStack::PopAndDestroy( attr );
        }
    else
        {
        // Finished
        ++iCurrentFieldIndex;
        NextState( EReplaceFields );
        }
    }

// ---------------------------------------------------------------------------
// CVPbkImportToContactsMergeOperation::NextState
// ---------------------------------------------------------------------------
//
void CVPbkImportToContactsMergeOperation::NextState( TState aNextState )
    {
    iState = aNextState;
    TRequestStatus* status = &iStatus;
    User::RequestComplete( status, KErrNone );
    SetActive();
    }

// --------------------------------------------------------------------------
// CVPbkImportToContactsMergeOperation::AttributeOperationComplete
// --------------------------------------------------------------------------
//
void CVPbkImportToContactsMergeOperation::AttributeOperationComplete( MVPbkContactOperationBase& aOperation )
    {
    if( &aOperation == iSetAttributeOperation )
        {
        NextState( ESetAttributes );
        }
    }

// --------------------------------------------------------------------------
// CVPbkImportToContactsMergeOperation::AttributeOperationFailed
// --------------------------------------------------------------------------
//
void CVPbkImportToContactsMergeOperation::AttributeOperationFailed(
        MVPbkContactOperationBase& aOperation,
        TInt aError )
    {
    if( &aOperation == iSetAttributeOperation )
        {
        delete iImportedContacts[0];
        iImportedContacts.Remove(0);
        HandleError( aError );
        }
    }

// End of file
