/*
* Copyright (c) 2005-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Phonebook 2 contact image assigner.
*
*/


#include "CPbk2ContactImageAssigner.h"

// Phonebook 2
#include "MPbk2ContactAssignerObserver.h"
#include "Pbk2AssignValidateField.h"
#include <CPbk2ImageManager.h>
#include <CPbk2DrmManager.h>
#include <CPbk2PresentationContact.h>
#include <CPbk2FieldPropertyArray.h>

// Virtual Phonebook
#include <CVPbkContactManager.h>
#include <MVPbkFieldType.h>
#include <MVPbkContactFieldTextData.h>
#include <VPbkEng.rsg>

// System includes
#include <featmgr.h>

/// Unnamed namespace for local definitions
namespace {

#ifdef _DEBUG

enum TPanicCode
    {
    EInvalidFieldIndex,
    };

void Panic(TPanicCode aReason)
    {
    _LIT( KPanicText, "CPbk2ContactImageAssigner" );
    User::Panic( KPanicText, aReason );
    }
#endif //_DEBUG

} /// namespace


// --------------------------------------------------------------------------
// CPbk2ContactImageAssigner::CPbk2ContactImageAssigner
// --------------------------------------------------------------------------
//
CPbk2ContactImageAssigner::CPbk2ContactImageAssigner
        ( MPbk2ContactAssignerObserver& aObserver,
          CVPbkContactManager& aContactManager,
          CPbk2FieldPropertyArray& aFieldProperties ):
            CActive( EPriorityIdle ), iObserver( aObserver ),
            iContactManager( aContactManager ),
            iFieldProperties( aFieldProperties ),
            iIndex( KErrNotSupported )
    {
    CActiveScheduler::Add( this );
    }

// --------------------------------------------------------------------------
// CPbk2ContactImageAssigner::~CPbk2ContactImageAssigner
// --------------------------------------------------------------------------
//
CPbk2ContactImageAssigner::~CPbk2ContactImageAssigner()
    {
    Cancel();
    delete iImageOperation;
    delete iImageManager;
    delete iDrmManager;
    }

// --------------------------------------------------------------------------
// CPbk2ContactImageAssigner::NewL
// --------------------------------------------------------------------------
//
CPbk2ContactImageAssigner* CPbk2ContactImageAssigner::NewL
        ( MPbk2ContactAssignerObserver& aObserver,
          CVPbkContactManager& aContactManager,
          CPbk2FieldPropertyArray& aFieldProperties )
    {
    CPbk2ContactImageAssigner* self =
        new ( ELeave ) CPbk2ContactImageAssigner
            ( aObserver, aContactManager, aFieldProperties );
    CleanupStack::PushL( self );
    self->ConstructL( aContactManager );
    CleanupStack::Pop( self );
    return self;
    }

// --------------------------------------------------------------------------
// CPbk2ContactImageAssigner::ConstructL
// --------------------------------------------------------------------------
//
void CPbk2ContactImageAssigner::ConstructL
        ( CVPbkContactManager& aContactManager )
    {
    iImageManager = CPbk2ImageManager::NewL( aContactManager );
    iDrmManager = CPbk2DrmManager::NewL();
    }

// --------------------------------------------------------------------------
// CPbk2ContactImageAssigner::AssignDataL
// --------------------------------------------------------------------------
//
void CPbk2ContactImageAssigner::AssignDataL
        ( MVPbkStoreContact& aStoreContact,
          MVPbkStoreContactField* /*aContactField*/,
          const MVPbkFieldType* aFieldType, const HBufC* aDataBuffer )
    {
    iContact = &aStoreContact;
    iFieldType = aFieldType;
    iDataBuffer = aDataBuffer;

    iState = EAssigningImage;
    IssueRequest();
    }

// --------------------------------------------------------------------------
// CPbk2ContactImageAssigner::AssignAttributeL
// --------------------------------------------------------------------------
//
void CPbk2ContactImageAssigner::AssignAttributeL
        ( MVPbkStoreContact& /*aStoreContact*/,
          MVPbkStoreContactField* /*aContactField*/,
          TPbk2AttributeAssignData /*aAttributeAssignData*/ )
    {
    // Not supported
    User::Leave( KErrNotSupported );
    }

// --------------------------------------------------------------------------
// CPbk2ContactImageAssigner::RunL
// --------------------------------------------------------------------------
//
void CPbk2ContactImageAssigner::RunL()
    {
   switch ( iState )
        {
        case EAssigningImage:
            {
            AssignImageL();
            break;
            }
        case EAssigningImageFileName:
            {
            AssignImageFileNameL();
            break;
            }
        case EStopping:
            {
            if ( iError == KErrNone )
                {
                iObserver.AssignComplete( *this, iIndex );
                }
            else
                {
                iObserver.AssignFailed( *this, iError );
                }
            break;
            }
        case EDrmProtected:
            {
            iObserver.AssignFailed( *this, iError );
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
// CPbk2ContactImageAssigner::DoCancel
// --------------------------------------------------------------------------
//
void CPbk2ContactImageAssigner::DoCancel()
    {
    // Nothing to do
    }

// --------------------------------------------------------------------------
// CPbk2ContactImageAssigner::RunError
// --------------------------------------------------------------------------
//
TInt CPbk2ContactImageAssigner::RunError( TInt aError )
    {
    iObserver.AssignFailed( *this, aError );
    return KErrNone;
    }

// --------------------------------------------------------------------------
// CPbk2ContactImageAssigner::Pbk2ImageSetComplete
// --------------------------------------------------------------------------
//
void CPbk2ContactImageAssigner::Pbk2ImageSetComplete
        ( MPbk2ImageOperation& /*aOperation*/ )
    {
    delete iImageOperation;
    iImageOperation = NULL;

    iState = EAssigningImageFileName;
    IssueRequest();
    }

// --------------------------------------------------------------------------
// CPbk2ContactImageAssigner::Pbk2ImageSetFailed
// --------------------------------------------------------------------------
//
void CPbk2ContactImageAssigner::Pbk2ImageSetFailed
        ( MPbk2ImageOperation& /*aOperation*/, TInt aError )
    {
    delete iImageOperation;
    iImageOperation = NULL;

    iState = EStopping;
    iError = aError;
    IssueRequest();
    }

// --------------------------------------------------------------------------
// CPbk2ContactImageAssigner::PassesDrmCheckL
// --------------------------------------------------------------------------
//
TInt CPbk2ContactImageAssigner::PassesDrmCheckL( 
        const HBufC* aDataBuffer,
        TBool& aIsProtected )
    {
    aIsProtected = ETrue;
    TInt error( KErrNone );
    
    if ( aDataBuffer )
        {
        error = iDrmManager->IsThumbnailForbidden( *aDataBuffer, aIsProtected );
        }

    return error;
    }

// --------------------------------------------------------------------------
// CPbk2ContactImageAssigner::AssignImageL
// --------------------------------------------------------------------------
//
void CPbk2ContactImageAssigner::AssignImageL()
    {
    TBool isProtected( ETrue );
    TInt error = PassesDrmCheckL( iDataBuffer, isProtected );
    if ( !isProtected && error == KErrNone )
        {
        delete iImageOperation;
        iImageOperation = NULL;

        // First delete a previous thumbnail, if any
        iImageManager->RemoveImage( *iContact, *iFieldType );

        // Then set new image
        iImageOperation = iImageManager->SetImageAsyncL
            ( *iContact, *iFieldType, *this, *iDataBuffer );
        }
    else
        {
        // Notify observer asynchronously
        iError = error;
        iState = EDrmProtected;
        IssueRequest();
        }
    }

// --------------------------------------------------------------------------
// CPbk2ContactImageAssigner::AssignImageFileNameL
// Assigns image file name, does not assign caller object text
// --------------------------------------------------------------------------
//
void CPbk2ContactImageAssigner::AssignImageFileNameL()
    {
    const MVPbkFieldType* codFieldType = NULL;
    if ( FeatureManager::FeatureSupported( KFeatureIdCallImagetext ) )
        {
        codFieldType = iContactManager.FieldTypes().Find
            ( R_VPBK_FIELD_TYPE_CALLEROBJIMG );
        }

    if ( codFieldType )
        {
        CPbk2PresentationContact* contact = CPbk2PresentationContact::NewL
            ( *iContact, iFieldProperties );
        CleanupStack::PushL( contact );

        // Check is the field already used
        TInt fieldIndex = KErrNotFound;
        TInt result =
            Pbk2AssignValidateField::ValidateFieldTypeUsageInContactL
                ( *contact, *codFieldType, iContactManager.FsSession(),
                  fieldIndex );

        MVPbkStoreContactField* field = NULL;
        if ( result == KErrAlreadyExists || result == KErrPathNotFound )
            {
            // If the contact has the field, then the field should be updated
            __ASSERT_DEBUG( fieldIndex > KErrNotFound,
                Panic( EInvalidFieldIndex ) );
            field = &iContact->Fields().FieldAt( fieldIndex );
            if ( field )
                {
                InsertTextDataL( *field, *iDataBuffer );
                }
            }
        else
            {
            field = iContact->CreateFieldLC( *codFieldType );
            InsertTextDataL( *field, *iDataBuffer );
            CleanupStack::Pop(); // field
            iIndex = iContact->AddFieldL( field ); // takes ownership
            }

        CleanupStack::PopAndDestroy( contact );
        }

    iState = EStopping;
    IssueRequest();
    }

// --------------------------------------------------------------------------
// CPbk2ContactImageAssigner::InsertTextDataL
// --------------------------------------------------------------------------
//
void CPbk2ContactImageAssigner::InsertTextDataL
        ( MVPbkStoreContactField& aField, const HBufC& aDataBuffer ) const
    {
    MVPbkContactFieldTextData::Cast( aField.FieldData() ).
        SetTextL( aDataBuffer );
    }

// --------------------------------------------------------------------------
// CPbk2ContactImageAssigner::IssueRequest
// --------------------------------------------------------------------------
//
void CPbk2ContactImageAssigner::IssueRequest()
    {
    TRequestStatus* status = &iStatus;
    User::RequestComplete( status, KErrNone );
    SetActive();
    }

// End of File
