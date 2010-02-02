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
* Description:  A operation class for setting/getting speeddial information
*
*/


#include "CSpeedDialAttributeOperation.h"
#include "CContactStore.h"
#include "TContactField.h"
#include "CContact.h"
#include "CContactLink.h"

#include <CVPbkSpeedDialAttribute.h>

#include <MVPbkStoreContactField.h>
#include <MVPbkContactAttributeManager.h>
#include <MVPbkContactAttribute.h>
#include <CVPbkAsyncOperation.h>
#include <CVPbkAsyncCallback.h>
#include <CVPbkContactManager.h>
#include <MVPbkContactStoreList.h>
#include <VPbkStoreUriLiterals.h>
#include <TVPbkContactStoreUriPtr.h>
#include <MVPbkContactFindObserver.h>
#include <CVPbkContactLinkArray.h>
#include <CVPbkBatchOperation.h>

// From Contacts Model
#include <cntitem.h>

// Debug includes
#include <VPbkDebug.h>

#ifdef _DEBUG
namespace {
    enum TPanic
        {
        EPreCond_FindSpeedDialsL,
        EPreCond_FindSpeedDialL
        };
        
    void Panic(TPanic aPanic)
        {
        _LIT(KPanicCat, "CSpeedDialAttributeOperation");
        User::Panic(KPanicCat, aPanic);
        }    
}
#endif // _DEBUG

namespace VPbkCntModel {

const TInt KMinPhoneNumberBufLength = 50;
const TInt KMaxPhoneNumberBufLength = 200;

static const TInt speedDialUids[] =
   {0, // to make this array 1-based
    KUidSpeedDialOneValue,
    KUidSpeedDialTwoValue,
    KUidSpeedDialThreeValue,
    KUidSpeedDialFourValue,
    KUidSpeedDialFiveValue,
    KUidSpeedDialSixValue,
    KUidSpeedDialSevenValue,
    KUidSpeedDialEightValue,
    KUidSpeedDialNineValue};

// ---------------------------------------------------------------------------
// CSpeedDialAttributeOperation::CSpeedDialAttributeOperation
// ---------------------------------------------------------------------------
//
CSpeedDialAttributeOperation::CSpeedDialAttributeOperation(
        TOperationMode aOperationMode,
        CContactStore& aContactStore,
        MVPbkStoreContactField& aField,
        MVPbkSetAttributeObserver& aObserver) :
    iOperationMode(aOperationMode),
    iContactStore(aContactStore),
    iField(&aField),
    iSetObserver(&aObserver)
    {
    }

// ---------------------------------------------------------------------------
// CSpeedDialAttributeOperation::CSpeedDialAttributeOperation
// ---------------------------------------------------------------------------
//
CSpeedDialAttributeOperation::CSpeedDialAttributeOperation(
        TOperationMode aOperationMode,
        CContactStore& aContactStore,
        MVPbkContactFindObserver& aObserver) :
    iOperationMode(aOperationMode),
    iContactStore(aContactStore),
    iFindObserver(&aObserver)
    {
    }

// ---------------------------------------------------------------------------
// CSpeedDialAttributeOperation::ConstructL
// ---------------------------------------------------------------------------
//
void CSpeedDialAttributeOperation::ConstructL(
        const CVPbkSpeedDialAttribute& aAttribute)
    {
    iAsyncOperation = new(ELeave) VPbkEngUtils::CVPbkAsyncOperation;
    iAttribute = static_cast<CVPbkSpeedDialAttribute*>(aAttribute.CloneLC());
    CleanupStack::Pop();
    }

// ---------------------------------------------------------------------------
// CSpeedDialAttributeOperation::NewSetLC
// ---------------------------------------------------------------------------
//
CSpeedDialAttributeOperation* CSpeedDialAttributeOperation::NewSetLC(
        CContactStore& aContactStore,
        MVPbkStoreContactField& aField,
        const CVPbkSpeedDialAttribute& aAttribute,
        MVPbkSetAttributeObserver& aObserver)
    {
    CSpeedDialAttributeOperation* self = new(ELeave) CSpeedDialAttributeOperation(
            ESet, aContactStore, aField, aObserver);
    CleanupStack::PushL(self);
    self->ConstructL(aAttribute);
    return self;
    }

// ---------------------------------------------------------------------------
// CSpeedDialAttributeOperation::NewRemoveLC
// ---------------------------------------------------------------------------
//
CSpeedDialAttributeOperation* CSpeedDialAttributeOperation::NewRemoveLC(
        CContactStore& aContactStore,
        MVPbkStoreContactField& aField,
        const CVPbkSpeedDialAttribute& aAttribute,
        MVPbkSetAttributeObserver& aObserver)
    {
    CSpeedDialAttributeOperation* self = new(ELeave) CSpeedDialAttributeOperation(
            ERemove, aContactStore, aField, aObserver);
    CleanupStack::PushL(self);
    self->ConstructL(aAttribute);
    return self;
    }

// ---------------------------------------------------------------------------
// CSpeedDialAttributeOperation::NewListLC
// ---------------------------------------------------------------------------
//
CSpeedDialAttributeOperation* CSpeedDialAttributeOperation::NewListLC(
        CContactStore& aContactStore,
        const CVPbkSpeedDialAttribute& aAttribute,
        MVPbkContactFindObserver& aObserver)
    {
    CSpeedDialAttributeOperation* self = new(ELeave) CSpeedDialAttributeOperation(
            EList, aContactStore, aObserver);
    CleanupStack::PushL(self);
    self->ConstructL(aAttribute);
    return self;
    }

// ---------------------------------------------------------------------------
// CSpeedDialAttributeOperation::~CSpeedDialAttributeOperation
// ---------------------------------------------------------------------------
//
CSpeedDialAttributeOperation::~CSpeedDialAttributeOperation()
    {
    delete iAsyncOperation;
    delete iAttribute;
    delete iLinkArray;
    delete iEmptyOperation;
    }

// ---------------------------------------------------------------------------
// From class MVPbkContactOperation
// CSpeedDialAttributeOperation::StartL
// ---------------------------------------------------------------------------
//
void CSpeedDialAttributeOperation::StartL()
    {
    if (iOperationMode == EList)
        {
        VPbkEngUtils::MAsyncCallback* callback =
            VPbkEngUtils::CreateAsyncCallbackLC(
                *this,
                &CSpeedDialAttributeOperation::DoListOperationL,
                &CSpeedDialAttributeOperation::ListOperationError,
                *iFindObserver);
        iAsyncOperation->CallbackL(callback);
        CleanupStack::Pop(callback);
        }
    else
        {
        VPbkEngUtils::MAsyncCallback* callback =
            VPbkEngUtils::CreateAsyncCallbackLC(
                *this,
                &CSpeedDialAttributeOperation::DoSetOperationL,
                &CSpeedDialAttributeOperation::SetOperationError,
                *iSetObserver);
        iAsyncOperation->CallbackL(callback);
        CleanupStack::Pop(callback);
        }
    }

// ---------------------------------------------------------------------------
// From class MVPbkContactOperation
// CSpeedDialAttributeOperation::Cancel
// ---------------------------------------------------------------------------
//
void CSpeedDialAttributeOperation::Cancel()
    {
    iAsyncOperation->Purge();
    }
    
// ---------------------------------------------------------------------------
// From class MVPbkBatchOperationObserver
// CSpeedDialAttributeOperation::StepComplete
// ---------------------------------------------------------------------------
//
void CSpeedDialAttributeOperation::StepComplete(
    MVPbkContactOperationBase& /*aOperation*/,
    TInt /*aStepSize*/ )
    {
    // Do nothing
    }

// ---------------------------------------------------------------------------
// From class MVPbkBatchOperationObserver
// CSpeedDialAttributeOperation::StepFailed
// ---------------------------------------------------------------------------
//
TBool CSpeedDialAttributeOperation::StepFailed(
                MVPbkContactOperationBase& /*aOperation*/,
                TInt /*aStepSize*/,
                TInt /*aError*/ )
    {
    // Do not continue batch operation.
    return EFalse;
    }

// ---------------------------------------------------------------------------
// From class MVPbkBatchOperationObserver
// CSpeedDialAttributeOperation::OperationComplete
// ---------------------------------------------------------------------------
//
void CSpeedDialAttributeOperation::OperationComplete
    ( MVPbkContactOperationBase& /*aOperation*/ )
    {
    TRAPD( error, HandleOperationCompleteL() );
    if ( error != KErrNone )
        {
        // If error has occured, we don't continue.
        iFindObserver->FindFailed( error );        
        }
    }

// ---------------------------------------------------------------------------
// CSpeedDialAttributeOperation::HandleOperationCompleteL()
// ---------------------------------------------------------------------------
//
void CSpeedDialAttributeOperation::HandleOperationCompleteL()
    {
    if ( iAttribute->Index() == 
        CVPbkSpeedDialAttribute::KSpeedDialIndexNotDefined )
        {
        // Find all
        FindSpeedDialsL();
        }
    else
        {
        // Find specified
        FindSpeedDialL();
        }
    }

// ---------------------------------------------------------------------------
// CSpeedDialAttributeOperation::DoSetOperationL
// ---------------------------------------------------------------------------
//
void CSpeedDialAttributeOperation::DoSetOperationL(
        MVPbkSetAttributeObserver& aObserver)
    {
    if (&iField->ContactStore() == &iContactStore)
        {
        CContact& contact = static_cast<CContact&>(iField->ParentContact());
        TContactField& field = static_cast<TContactField&>(*iField);

        if (iOperationMode == ESet)
            {
            TContactItemId cid = contact.NativeContact()->Id();
            const TInt nativeFieldIndex = field.NativeFieldIndex();

            iContactStore.NativeDatabase().SetFieldAsSpeedDialL(
                *contact.NativeContact(),
                nativeFieldIndex,
                iAttribute->Index());

            // SetFieldAsSpeedDialL commits the contact, so we want to load it again
            CContactItem* contactItem = iContactStore.NativeDatabase().OpenContactL(cid);
            contact.SetContact(contactItem);
            // This is called because we want the parent contact's field collection
            // iCurrentField to be refreshed to point again to this field
            contact.Fields().FieldAt(nativeFieldIndex);
            }
        else if (iOperationMode == ERemove)
            {
            if (iAttribute->Index() == CVPbkSpeedDialAttribute::KSpeedDialIndexNotDefined)
                {
                for (TInt i = 1; i <= KCntMaxSpeedDialIndex; ++i)
                    {
                    TBuf<KSpeedDialPhoneLength> phoneNumber;
                    TContactItemId cid =
                        iContactStore.NativeDatabase().GetSpeedDialFieldL(i, phoneNumber);

                    if (cid == contact.NativeContact()->Id() &&
                        field.NativeField()->ContentType().ContainsFieldType(
                            TFieldType::Uid(speedDialUids[i])))
                        {
                        iContactStore.NativeDatabase().CommitContactL(
                                              *contact.NativeContact());

                        iContactStore.NativeDatabase().RemoveSpeedDialFieldL(
                                contact.NativeContact()->Id(),
                                i);

                        const TInt fieldIndex = NativeFieldIndex(contact, field);
                        // RemoveSpeedDialFieldL commits the contact, so we want to
                        // load it again
                        CContactItem* contactItem =
                                        iContactStore.NativeDatabase().OpenContactL(cid);
                        contact.SetContact(contactItem);
                        // This is called because we want the parent contact's field
                        // collection iCurrentField to be refreshed to point again
                        // to this field
                        contact.Fields().FieldAt(fieldIndex);
                        }
                    }
                }
            else
                {
                const TInt index( iAttribute->Index() );
                TBuf<KSpeedDialPhoneLength> phoneNumber;
                TContactItemId cid =
                    iContactStore.NativeDatabase().GetSpeedDialFieldL(
                        index, phoneNumber);
                        
                if ( cid == contact.NativeContact()->Id() )
                    {
                    iContactStore.NativeDatabase().CommitContactL(*contact.NativeContact());

                    iContactStore.NativeDatabase().RemoveSpeedDialFieldL( cid, index );

                    const TInt fieldIndex = NativeFieldIndex(contact, field);
                    // RemoveSpeedDialFieldL commits the contact, so we want to load it again
                    CContactItem* contactItem = iContactStore.NativeDatabase().OpenContactL(cid);
                    contact.SetContact(contactItem);
                    // This is called because we want the parent contact's field collection
                    // iCurrentField to be refreshed to point again to this field
                    contact.Fields().FieldAt(fieldIndex);
                    }
                else
                    {
                    User::Leave( KErrArgument );
                    }
                }
            }
        }

    aObserver.AttributeOperationComplete(*this);
    }

// ---------------------------------------------------------------------------
// CSpeedDialAttributeOperation::SetOperationError
// ---------------------------------------------------------------------------
//
void CSpeedDialAttributeOperation::SetOperationError(
        MVPbkSetAttributeObserver& aObserver,
        TInt aError)
    {
    aObserver.AttributeOperationFailed(*this, aError);
    }

// ---------------------------------------------------------------------------
// CSpeedDialAttributeOperation::DoListOperationL
// ---------------------------------------------------------------------------
//
void CSpeedDialAttributeOperation::DoListOperationL(
        MVPbkContactFindObserver& /*aObserver*/)
    {
    VPBK_DEBUG_PRINT( VPBK_DEBUG_STRING(
        "CSpeedDialAttributeOperation::DoListOperationL" ) );

    if ( iEmptyOperation )
        {
        delete iEmptyOperation;
        iEmptyOperation = NULL;
        }

    if ( iLinkArray )
        {
        delete iLinkArray;
        iLinkArray = NULL;
        }

    iEmptyOperation = CVPbkBatchOperation::NewLC( *this );
    CleanupStack::Pop( iEmptyOperation );
    iEmptyOperation->StartL();
        
    VPBK_DEBUG_PRINT( VPBK_DEBUG_STRING(
        "CSpeedDialAttributeOperation::DoListOperationL end" ) );
    }

// ---------------------------------------------------------------------------
// CSpeedDialAttributeOperation::ListOperationError
// ---------------------------------------------------------------------------
//
void CSpeedDialAttributeOperation::ListOperationError(
        MVPbkContactFindObserver& aObserver,
        TInt aError)
    {
    aObserver.FindFailed(aError);
    }
     
// ---------------------------------------------------------------------------
// CSpeedDialAttributeOperation::NativeFieldIndex
// ---------------------------------------------------------------------------
//
TInt CSpeedDialAttributeOperation::NativeFieldIndex( CContact& aContact,
        TContactField& aField ) const
    {
    TInt result = KErrNotFound;

    CContactItemFieldSet& fields = aContact.NativeContact()->CardFields();
    const TInt count = fields.Count();
    for (TInt i = 0; i < count; ++i)
        {
        if (&fields[i] == aField.NativeField())
            {
            result = i;
            break;
            }
        }

    return result;
    }
    
// ---------------------------------------------------------------------------
// CSpeedDialAttributeOperation::FindSpeedDialsL
// Find all contacst that have speed dial
// ---------------------------------------------------------------------------
//
void CSpeedDialAttributeOperation::FindSpeedDialsL()
    {
    __ASSERT_DEBUG( iAttribute->Index() == 
        CVPbkSpeedDialAttribute::KSpeedDialIndexNotDefined,
        Panic( EPreCond_FindSpeedDialsL ) );
    
    // Symbian GetSpeedDialFieldL documentation defines these
    const TInt firstSpeedDialIndex = 1;
    const TInt lastSpeedDialIndex = 9;
    
    CVPbkContactLinkArray* results = CVPbkContactLinkArray::NewLC();
    for ( TInt i = firstSpeedDialIndex; i <= lastSpeedDialIndex; ++i )
        {
        CContactLink* link = DoFindSpeedDialLC( i );
        if ( link )
            {
            results->AppendL( link );
            CleanupStack::Pop( link );
            }
        }
    CleanupStack::Pop( results );
    iFindObserver->FindCompleteL( results );
    }

// ---------------------------------------------------------------------------
// CSpeedDialAttributeOperation::FindSpeedDialL
// Find contact for specified speed dial index
// ---------------------------------------------------------------------------
//
void CSpeedDialAttributeOperation::FindSpeedDialL()
    {
    __ASSERT_DEBUG( iAttribute->Index() != 
        CVPbkSpeedDialAttribute::KSpeedDialIndexNotDefined,
        Panic( EPreCond_FindSpeedDialL ) );
    
    CVPbkContactLinkArray* results = CVPbkContactLinkArray::NewLC();
    CContactLink* link = DoFindSpeedDialLC( iAttribute->Index() );
    if ( link )
        {
        results->AppendL( link );
        CleanupStack::Pop( link );
        }
    CleanupStack::Pop( results );
    iFindObserver->FindCompleteL( results );
    }
    
// ---------------------------------------------------------------------------
// CSpeedDialAttributeOperation::DoFindSpeedDialLC
// Returns a link if speed dial was set, otherwise NULL (not in CleanupStack)
// ---------------------------------------------------------------------------
//
CContactLink* CSpeedDialAttributeOperation::DoFindSpeedDialLC( TInt aIndex )
    {
    VPBK_DEBUG_PRINT( VPBK_DEBUG_STRING(
        "CSpeedDialAttributeOperation::FindSpeedDialL index %d)" ), aIndex );
    
    const TInt doubleFactor = 2;
    TInt bufSize = KMinPhoneNumberBufLength;
    TContactItemId cid = KNullContactId;
    TBool speedDialFound = EFalse;
    // Contacts model responds with KErrOverflow if the buffer is too small
    // Grow the buffer until it's big enough.
    do
        {
        HBufC* tmp = HBufC::NewLC( bufSize );
        TPtr tmpPtr( tmp->Des() );
        // Get native contact id from Contacts Model
        cid = iContactStore.NativeDatabase().GetSpeedDialFieldL( aIndex, 
            tmpPtr );
        if ( cid == KErrOverflow )
            {
            // Double the size of the buffer
            bufSize *= doubleFactor;
            }
        else if ( tmpPtr.Length() > 0 )
            {
            // Symbian documentation says that if the speed dial hasn't been
            // set then tmpPtr should be KNullDesC
            speedDialFound = ETrue;
            }
        CleanupStack::PopAndDestroy( tmp );
        }
    while ( ( cid == KErrOverflow ) && ( bufSize <= KMaxPhoneNumberBufLength ) );
    
    CContactLink* result = NULL;
    if ( speedDialFound )
        {
        result = CContactLink::NewLC( iContactStore, cid );
        }
    
    return result;
    }
} // namespace VPbkCntModel

// End of File
