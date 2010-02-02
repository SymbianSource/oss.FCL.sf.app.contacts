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
* Description:  Phonebook 2 own numbers list box model.
*
*/


#include    "CPsu2OwnNumberListBoxModel.h"

// From Virtual Phonebook
#include    <MVPbkContactViewBase.h>
#include    <MVPbkViewContact.h>
#include    <MVPbkBaseContactFieldCollection.h>
#include    <MVPbkBaseContactField.h>
#include    <MVPbkContactFieldData.h>
#include    <MVPbkFieldType.h>
#include    <TVPbkFieldVersitProperty.h>
#include    <MVPbkContactFieldTextData.h>
#include    <MVPbkStoreContact.h>

// System includes
#include    <coecntrl.h>
#include    <eikenv.h>

// Debugging headers
#include <Pbk2Debug.h>

/// Unnamed namespace for local definitions
namespace 
    {

const TInt KOOBLimit = -1;
const TInt KMaxListBoxBufferSize = 128;
_LIT(KListBoxIconFormat, "%d\t");
_LIT(KListBoxFieldSeparator, "\t");

/**
 * Checks from given array is there given field type name in it.
 * @param aArray    Array of field types.
 * @param aName     Field type name to look for.
 * @return  ETrue if name is found, EFalse if it is not.
 */
TBool ContainsFieldType(
        TArray<TVPbkFieldVersitProperty> aArray,
        TVPbkFieldTypeName aName )
    {
    TBool ret = EFalse;
    const TInt size = aArray.Count();
    for (TInt i=0; i<size; ++i)
        {
        if (aArray[i].Name() == aName)
            {
            ret = ETrue;
            break;
            }
        }
    return ret;        
    }

/**
 * Checks from given fields for given field type and prints
 * it to given descriptor.
 *
 * @param aField    Field collection.
 * @param aPtr      Descriptor.
 * @param aType     Field type name.
 */
void AddDataFormatL( 
        const MVPbkBaseContactFieldCollection& aFields, 
        TPtr& aPtr, 
        TVPbkFieldTypeName aType )
    {
    const TInt fieldCount( aFields.FieldCount() );
    for( TInt i(0); i < fieldCount; ++i )
        {
        const MVPbkBaseContactField& field =  aFields.FieldAt( i );
        const MVPbkFieldType* fieldType = field.BestMatchingFieldType();
        if ( field.FieldData().DataType() == EVPbkFieldStorageTypeText )
            {
            if (fieldType && 
                ContainsFieldType(fieldType->VersitProperties(), aType))
                {
                const MVPbkContactFieldTextData* textData =
                        &MVPbkContactFieldTextData::Cast( field.FieldData() );
                aPtr.Append(textData->Text());
                aPtr.Append(KListBoxFieldSeparator);
                }
            }
        }       
    }
        
#ifdef _DEBUG
enum TPanicCode
    {
    EPanicPreCond_RefreshL = 1
    };

void Panic(TPanicCode aReason)
    {
    _LIT( KPanicText, "CPsu2OwnNumberListBoxModel" );
    User::Panic( KPanicText, aReason );
    }
#endif // _DEBUG

} //// namespace

// --------------------------------------------------------------------------
// CPsu2OwnNumberListBoxModel::CPsu2OwnNumberListBoxModel
// --------------------------------------------------------------------------
//
CPsu2OwnNumberListBoxModel::CPsu2OwnNumberListBoxModel
    ( MVPbkContactViewBase& aView, CCoeControl& aObserver ):
        CActive(EPriorityStandard),
        iView( aView ),
        iObserver( aObserver )
    {
    }

// --------------------------------------------------------------------------
// CPsu2OwnNumberListBoxModel::~CPsu2OwnNumberListBoxModel
// --------------------------------------------------------------------------
//
CPsu2OwnNumberListBoxModel::~CPsu2OwnNumberListBoxModel()
    {
    iView.RemoveObserver( *this );
    Cancel();
    if ( iArray )
        {
        iArray->Reset();        
        }
    delete iArray;
    }

// --------------------------------------------------------------------------
// CPsu2OwnNumberListBoxModel::ConstructL
// --------------------------------------------------------------------------
//
void CPsu2OwnNumberListBoxModel::ConstructL()
    {
    CActiveScheduler::Add( this );
    iArray = new ( ELeave ) CDesCArraySeg( 1 );
    iView.AddObserverL( *this );
    }

// --------------------------------------------------------------------------
// CPsu2OwnNumberListBoxModel::NewL
// --------------------------------------------------------------------------
//
CPsu2OwnNumberListBoxModel* CPsu2OwnNumberListBoxModel::NewL
        (  MVPbkContactViewBase& aView, CCoeControl& aObserver )
    {
    CPsu2OwnNumberListBoxModel* self = 
        new( ELeave ) CPsu2OwnNumberListBoxModel( aView, aObserver );
    
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// --------------------------------------------------------------------------
// CPsu2OwnNumberListBoxModel::MdcaCount
// --------------------------------------------------------------------------
//
TInt CPsu2OwnNumberListBoxModel::MdcaCount() const
    {
    TInt result( 0 );
    TRAPD( error, result = iView.ContactCountL() );
    if ( error != KErrNone )
        {
        CEikonEnv::Static()->NotifyIdleErrorWhileRedrawing( error );
        }
    return result;
    }
    
// --------------------------------------------------------------------------
// CPsu2OwnNumberListBoxModel::MdcaPoint
// --------------------------------------------------------------------------
//
TPtrC16 CPsu2OwnNumberListBoxModel::MdcaPoint(TInt aIndex) const
    {
    PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING
            ("CPsu2OwnNumberListBoxModel(%x)::MdcaPoint() Index=%d "),
                this, aIndex);
    if ( aIndex > KOOBLimit && aIndex < iArray->Count())
        {
        return (*iArray)[aIndex];
        }
    return KNullDesC();
    }
    
// --------------------------------------------------------------------------
// CPsu2OwnNumberListBoxModel::AddToArrayL
// --------------------------------------------------------------------------
//
void CPsu2OwnNumberListBoxModel::AddToArrayL( MVPbkStoreContact* aContact )
    {   
    PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING
        ("CPsu2OwnNumberListBoxModel(%x)::AddToArrayL() "), this);
        
    if ( aContact )
        {
        aContact->PushL();
        HBufC* buffer = HBufC::NewLC(KMaxListBoxBufferSize);
        TPtr ptr( buffer->Des() );
                
        ptr.Format(KListBoxIconFormat, 0 ); 
        const MVPbkBaseContactFieldCollection& fields = aContact->Fields();
        AddDataFormatL( fields, ptr, EVPbkVersitNameN );
        AddDataFormatL( fields, ptr, EVPbkVersitNameTEL );
        iArray->AppendL(ptr);
        CleanupStack::PopAndDestroy( buffer );
        CleanupStack::PopAndDestroy( aContact );
        }    
    }
    
// --------------------------------------------------------------------------
// CPsu2OwnNumberListBoxModel::ReadContactsL
// --------------------------------------------------------------------------
//
void CPsu2OwnNumberListBoxModel::ReadContactsL()
    {
    iCurrentContact = 0;
    const MVPbkViewContact& contact = iView.ContactAtL(iCurrentContact);
    contact.ReadL( *this );    
    }

// --------------------------------------------------------------------------
// CPsu2OwnNumberListBoxModel::RefreshL
// --------------------------------------------------------------------------
//    
void CPsu2OwnNumberListBoxModel::RefreshL()
    {
    __ASSERT_DEBUG( iArray, Panic(EPanicPreCond_RefreshL));
    
    // Reset array
    iArray->Reset();

    if ( iView.ContactCountL() > 0 )
        {
        ReadContactsL();
        }
    else
        {
        iObserver.DrawDeferred();
        }
    }

// --------------------------------------------------------------------------
// CPsu2OwnNumberListBoxModel::ContactOperationCompleted
// --------------------------------------------------------------------------
//
void CPsu2OwnNumberListBoxModel::ContactOperationCompleted
        ( TContactOpResult aResult )
    {
    PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING
        ("CPsu2OwnNumberListBoxModel(%x)::ContactOperationCompleted() "),
            this);
        
    TRAPD( err, AddToArrayL( aResult.iStoreContact ) )
    ++iCurrentContact;
    if ( err != KErrNone )
        {
        CEikonEnv::Static()->HandleError( err );
        }
    else
        {
        IssueRequest();
        }
    }

// --------------------------------------------------------------------------
// CPsu2OwnNumberListBoxModel::ContactOperationFailed
// --------------------------------------------------------------------------
//
void CPsu2OwnNumberListBoxModel::ContactOperationFailed(
        TContactOp /*aOpCode*/, 
        TInt aErrorCode, 
        TBool /*aErrorNotified*/)
    {
    CEikonEnv::Static()->HandleError( aErrorCode );
    }

// --------------------------------------------------------------------------
// CPsu2OwnNumberListBoxModel::RunL
// --------------------------------------------------------------------------
//
void CPsu2OwnNumberListBoxModel::RunL()
    {    
    if ( iCurrentContact < iView.ContactCountL() )    
        {
        const MVPbkViewContact& contact = iView.ContactAtL(iCurrentContact);
        contact.ReadL( *this );    
        }
    else
        {        
        iObserver.DrawDeferred();
        }
    }

// --------------------------------------------------------------------------
// CPsu2OwnNumberListBoxModel::DoCancel
// --------------------------------------------------------------------------
//
void CPsu2OwnNumberListBoxModel::DoCancel()
    {
    // Nothing to do
    }

// --------------------------------------------------------------------------
// CPsu2OwnNumberListBoxModel::RunError
// --------------------------------------------------------------------------
//
TInt CPsu2OwnNumberListBoxModel::RunError(TInt aError)
    {
    CCoeEnv::Static()->HandleError(aError);
    return KErrNone;
    }

// --------------------------------------------------------------------------
// CPsu2OwnNumberListBoxModel::IssueRequest
// --------------------------------------------------------------------------
//
void CPsu2OwnNumberListBoxModel::IssueRequest()
    {
    TRequestStatus* status = &iStatus;
    User::RequestComplete(status, KErrNone);
    SetActive();
    }
    
// -----------------------------------------------------------------------------
// CPsu2OwnNumberListBoxModel::ContactViewReady
// -----------------------------------------------------------------------------
//     
void CPsu2OwnNumberListBoxModel::ContactViewReady(
        MVPbkContactViewBase& aView )
    {
    if ( &iView == &aView )
        {
        TRAPD( error, RefreshL() );
        if ( error != KErrNone )
            {
            CCoeEnv::Static()->HandleError( error );
            }
        }    
    }

// -----------------------------------------------------------------------------
// CPsu2OwnNumberListBoxModel::ContactViewUnavailable
// -----------------------------------------------------------------------------
//                     
void CPsu2OwnNumberListBoxModel::ContactViewUnavailable(
        MVPbkContactViewBase& /*aView*/ )
    {
    // Do nothing  
    }

// -----------------------------------------------------------------------------
// CPsu2OwnNumberListBoxModel::ContactAddedToView
// -----------------------------------------------------------------------------
//     
void CPsu2OwnNumberListBoxModel::ContactAddedToView(
        MVPbkContactViewBase& /*aView*/, 
        TInt /*aIndex*/, 
        const MVPbkContactLink& /*aContactLink*/ )
    {
    // Do nothing
    }
    
// -----------------------------------------------------------------------------
// CPsu2OwnNumberListBoxModel::ContactRemovedFromView
// -----------------------------------------------------------------------------
//     
void CPsu2OwnNumberListBoxModel::ContactRemovedFromView(
        MVPbkContactViewBase& /*aView*/, 
        TInt /*aIndex*/, 
        const MVPbkContactLink& /*aContactLink*/ )
    {
    // Do nothing
    }
    
// -----------------------------------------------------------------------------
// CPsu2OwnNumberListBoxModel::ContactViewError
// -----------------------------------------------------------------------------
//     
void CPsu2OwnNumberListBoxModel::ContactViewError(
        MVPbkContactViewBase& /*aView*/, 
        TInt /*aError*/, 
        TBool /*aErrorNotified*/ )
    {
    // Do nothing
    }

// End of File
