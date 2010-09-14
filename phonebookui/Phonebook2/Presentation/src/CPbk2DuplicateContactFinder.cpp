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
* Description:  A class for finding contact duplicates from stores
*
*/


#include <CPbk2DuplicateContactFinder.h>

// From Phonebook2
#include <MPbk2DuplicateContactObserver.h>
#include <MPbk2ContactNameFormatter.h>

// From Virtual Phonebook
#include <MVPbkFieldType.h>
#include <MVPbkBaseContactFieldCollection.h>
#include <MVPbkStoreContact.h>
#include <MVPbkContactFieldTextData.h>
#include <MVPbkContactStore.h>
#include <CVPbkContactManager.h>
#include <MVPbkContactOperation.h>
#include <MVPbkContactLinkArray.h>

#include <vpbkeng.rsg>
namespace
    {
    #ifdef _DEBUG
        enum TPanicCode
            {
            EPanic_RetrieveContactL_OOB = 1
            };
            
        void Panic( TPanicCode aPanic )
            {
            _LIT( KPanicCat, "CPbk2DuplicateContactFinder" );
            User::Panic( KPanicCat(), aPanic );
            }
    #endif // _DEBUG    
    }
 
enum TPbk2DuplicateContactFinderState
    {
    ENoDuplicatesFound,
    EStartFind,
    ERetrieveContact,
    EComplete
    };

const TInt KNameFormattingFlags = 
    MPbk2ContactNameFormatter::EPreserveLeadingSpaces;
    
// ---------------------------------------------------------------------------
// CPbk2DuplicateContactFinder::CPbk2DuplicateContactFinder
// ---------------------------------------------------------------------------
//
CPbk2DuplicateContactFinder::CPbk2DuplicateContactFinder( 
        CVPbkContactManager& aContactManager, 
        MPbk2ContactNameFormatter& aNameFormatter,
        const MVPbkFieldTypeList& aFieldTypesForFind,
        RPointerArray<MVPbkStoreContact>& aDuplicateContacts ):
            CActive( EPriorityStandard ),
            iContactManager( aContactManager ),
            iNameFormatter( aNameFormatter ),
            iFieldTypesForFind( aFieldTypesForFind ),
            iFindText( KNullDesC ),
            iDuplicates( aDuplicateContacts )
    {
    }


// ---------------------------------------------------------------------------
// CPbk2DuplicateContactFinder::ConstructL
// ---------------------------------------------------------------------------
//
void CPbk2DuplicateContactFinder::ConstructL()
    {
    CActiveScheduler::Add( this );
    }

// ---------------------------------------------------------------------------
// CPbk2DuplicateContactFinder::NewL
// ---------------------------------------------------------------------------
//
EXPORT_C CPbk2DuplicateContactFinder* CPbk2DuplicateContactFinder::NewL(
        CVPbkContactManager& aContactManager, 
        MPbk2ContactNameFormatter& aNameFormatter,
        const MVPbkFieldTypeList& aFieldTypesForFind,
        RPointerArray<MVPbkStoreContact>& aDuplicateContacts )
    {
    CPbk2DuplicateContactFinder* self = 
        new( ELeave ) CPbk2DuplicateContactFinder( aContactManager, 
            aNameFormatter, aFieldTypesForFind, aDuplicateContacts );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// destructor
// ---------------------------------------------------------------------------
//
CPbk2DuplicateContactFinder::~CPbk2DuplicateContactFinder()
    {
    Cancel();
    delete iContactOperation;
    delete iFindResults;
    delete iContactTitle;
    }

// ---------------------------------------------------------------------------
// CPbk2DuplicateContactFinder::StartL
// ---------------------------------------------------------------------------
//
EXPORT_C void CPbk2DuplicateContactFinder::StartL( 
        const MVPbkBaseContact& aContact,
        MVPbkContactStore& aStore, 
        MPbk2DuplicateContactObserver& aObserver,
        TInt aMaxDuplicatesToFind )
    {
    iStore = &aStore;
    StartL( aContact, aObserver, aMaxDuplicatesToFind );
    }
    
// ---------------------------------------------------------------------------
// CPbk2DuplicateContactFinder::StartL
// ---------------------------------------------------------------------------
//
EXPORT_C void CPbk2DuplicateContactFinder::StartL( 
        const MVPbkBaseContact& aContact,
        MPbk2DuplicateContactObserver& aObserver,
        TInt aMaxDuplicatesToFind )
    {
    iObserver = &aObserver;
    iContactToCompare = &aContact;
    iContactIndex = 0;
    iMaxDuplicatesToFind = aMaxDuplicatesToFind;
    
    iState = ENoDuplicatesFound;
    iFindText.Set( FindText() );
    if ( iFindText.Length() > 0 )
        {
        iState = EStartFind;
        }
    IssueRequest();
    }
    
// ---------------------------------------------------------------------------
// From class CActive.
// CPbk2DuplicateContactFinder::RunL
// ---------------------------------------------------------------------------
//
void CPbk2DuplicateContactFinder::RunL()
    {
    switch ( iState )
        {
        case EStartFind:
            {
            StartFindL();
            break;
            }
        case ERetrieveContact:
            {
            RetrieveContactL();
            break;
            }
        case EComplete:
            {
            CompleteL();
            break;
            }
        case ENoDuplicatesFound: // FALLTHROUGH
        default:
            {
            iObserver->DuplicateFindComplete();
            break;
            }
        }
    }

// ---------------------------------------------------------------------------
// From class CActive.
// CPbk2DuplicateContactFinder::DoCancel
// ---------------------------------------------------------------------------
//    
void CPbk2DuplicateContactFinder::DoCancel()
    {
    DestroyOperation();
    }

// ---------------------------------------------------------------------------
// From class CActive.
// CPbk2DuplicateContactFinder::RunError
// ---------------------------------------------------------------------------
//        
TInt CPbk2DuplicateContactFinder::RunError( TInt aError )
    {
    iObserver->DuplicateFindFailed( aError );
    return KErrNone;
    }
    
// ---------------------------------------------------------------------------
// From class MVPbkContactFindObserver.
// CPbk2DuplicateContactFinder::FindCompleteL
// ---------------------------------------------------------------------------
//
void CPbk2DuplicateContactFinder::FindCompleteL( 
        MVPbkContactLinkArray* aResults )
    {
    iFindResults = aResults;
    iState = ENoDuplicatesFound;
    if ( !LastContactRetrieved() )
        {
        iState = ERetrieveContact;
        }
    
    IssueRequest();
    }

// ---------------------------------------------------------------------------
// From class MVPbkContactFindObserver.
// CPbk2DuplicateContactFinder::FindFailed
// ---------------------------------------------------------------------------
//
void CPbk2DuplicateContactFinder::FindFailed( TInt aError )
    {
    iObserver->DuplicateFindFailed( aError );
    }

// ---------------------------------------------------------------------------
// From class MVPbkSingleContactOperationObserver.
// CPbk2DuplicateContactFinder::VPbkSingleContactOperationComplete
// ---------------------------------------------------------------------------
//    
void CPbk2DuplicateContactFinder::VPbkSingleContactOperationComplete(
        MVPbkContactOperationBase& /*aOperation*/, 
        MVPbkStoreContact* aContact )
    {
    TRAPD( res, CheckDuplicateL( aContact ) );
        
    ++iContactIndex;
    if ( res != KErrNone )
        {
        iObserver->DuplicateFindFailed( res );
        }
    else if ( LastContactRetrieved() )
        {
        iState = EComplete;
        IssueRequest();
        }
    else
        {
        // Continue retrieving
        IssueRequest();
        }
    }

// ---------------------------------------------------------------------------
// From class MVPbkSingleContactOperationObserver.
// CPbk2DuplicateContactFinder::VPbkSingleContactOperationFailed
// ---------------------------------------------------------------------------
//    
void CPbk2DuplicateContactFinder::VPbkSingleContactOperationFailed( 
        MVPbkContactOperationBase& /*aOperation*/, TInt aError )
    {
    iObserver->DuplicateFindFailed( aError );
    }
    
// ---------------------------------------------------------------------------
// CPbk2DuplicateContactFinder::IssueRequest
// ---------------------------------------------------------------------------
//
void CPbk2DuplicateContactFinder::IssueRequest()
    {
    TRequestStatus* st = &iStatus;
    User::RequestComplete( st, KErrNone );
    SetActive();
    }
    
// ---------------------------------------------------------------------------
// CPbk2DuplicateContactFinder::FindText
// ---------------------------------------------------------------------------
//
TPtrC CPbk2DuplicateContactFinder::FindText()
    {
    const MVPbkBaseContactFieldCollection& fields = 
        iContactToCompare->Fields();
    // Find a text from the contact in the order of iFieldTypesForFind.
    // the first data that is found is used as a find text
    const TInt findTypeCount = iFieldTypesForFind.FieldTypeCount();
    for ( TInt i = 0; i < findTypeCount; ++i )
        {
        const MVPbkFieldType& findType = iFieldTypesForFind.FieldTypeAt( i );
        const TInt fieldCount = fields.FieldCount();
        for ( TInt j = 0; j < fieldCount; ++j )
            {
            const MVPbkFieldType* type = 
                fields.FieldAt( j ).BestMatchingFieldType();
            if ( type && findType.IsSame( *type ) )
                {
                const MVPbkContactFieldData& data = 
                    fields.FieldAt( j ).FieldData();
                if ( data.DataType() == EVPbkFieldStorageTypeText )
                    {
                    TPtrC text( MVPbkContactFieldTextData::Cast( 
                        data ).Text() );
                    if ( text.Length() > 0 )
                        {
                        return text;
                        }
                    }
                }
            }    
        }
    // No find text -> no duplicates for contact.
    return TPtrC( KNullDesC );
    }
    
// ---------------------------------------------------------------------------
// CPbk2DuplicateContactFinder::FindOperationL
// ---------------------------------------------------------------------------
//
MVPbkContactOperationBase* CPbk2DuplicateContactFinder::FindOperationL( 
        const TDesC& aFindText )
    {
    if ( iStore )
        {
        MVPbkContactOperation* op = iStore->CreateFindOperationL( aFindText, 
            iFieldTypesForFind, *this );
        CleanupDeletePushL( op );
        op->StartL();
        CleanupStack::Pop(); // op
        return op;
        }
    else
        {
        return iContactManager.FindL( aFindText,
            iFieldTypesForFind, *this );
        }
    }

// ---------------------------------------------------------------------------
// CPbk2DuplicateContactFinder::StartFindL
// ---------------------------------------------------------------------------
//
void CPbk2DuplicateContactFinder::StartFindL()
    {
    delete iFindResults;
    iFindResults = NULL;
    
    delete iContactTitle;
    iContactTitle = NULL;
    iContactTitle = iNameFormatter.GetContactTitleL( 
        iContactToCompare->Fields(), KNameFormattingFlags );
    
    DestroyOperation();
    iContactOperation = FindOperationL( iFindText );
    }

// ---------------------------------------------------------------------------
// CPbk2DuplicateContactFinder::RetrieveContactL
// ---------------------------------------------------------------------------
//    
void CPbk2DuplicateContactFinder::RetrieveContactL()
    {
    DestroyOperation();
    __ASSERT_DEBUG( iFindResults->Count() > iContactIndex,
        Panic( EPanic_RetrieveContactL_OOB ) );
    iContactOperation = iContactManager.RetrieveContactL(
        iFindResults->At( iContactIndex ), *this );
    }

// ---------------------------------------------------------------------------
// CPbk2DuplicateContactFinder::CompleteL
// ---------------------------------------------------------------------------
//        
void CPbk2DuplicateContactFinder::CompleteL()
    {
    iObserver->DuplicateFindComplete();
    }

// ---------------------------------------------------------------------------
// CPbk2DuplicateContactFinder::LastContactRetrieved
// ---------------------------------------------------------------------------
//        
TBool CPbk2DuplicateContactFinder::LastContactRetrieved()
    {
    if ( iDuplicates.Count() >= iMaxDuplicatesToFind ||
         iContactIndex >= iFindResults->Count() )
        {
        return ETrue;
        }
    return EFalse;
    }
    
// ---------------------------------------------------------------------------
// CPbk2DuplicateContactFinder::DestroyOperation
// ---------------------------------------------------------------------------
//        
void CPbk2DuplicateContactFinder::DestroyOperation()
    {
    delete iContactOperation;
    iContactOperation = NULL;
    }

// ---------------------------------------------------------------------------
// CPbk2DuplicateContactFinder::CheckDuplicateL
// ---------------------------------------------------------------------------
//            
void CPbk2DuplicateContactFinder::CheckDuplicateL( 
        MVPbkStoreContact* aContact )
    {    
    aContact->PushL();
    
    TBool duplicate = EFalse;
    HBufC* candidateTitle = iNameFormatter.GetContactTitleOrNullL( 
        aContact->Fields(), KNameFormattingFlags );
    // If the aContact is not a group and it has a title, compare it with iContactTitle.
    // This is because some group has the same name with the contact and they can't be
    // considered as duplicate.
    if ( !aContact->Group() && candidateTitle )
        {
        CleanupStack::PushL( candidateTitle );
        if ( iContactTitle->CompareF( *candidateTitle ) == 0 )
            {
            // Compare the first and last name.
            if ( IsFieldMatched( aContact, R_VPBK_FIELD_TYPE_FIRSTNAME ) && 
                    IsFieldMatched( aContact, R_VPBK_FIELD_TYPE_LASTNAME ) )
                {
                duplicate = ETrue;
                }
            }
        CleanupStack::PopAndDestroy( candidateTitle );
        }
    
    if ( duplicate )
        {
        iDuplicates.AppendL( aContact );
        CleanupStack::Pop(); // aContact
        }
    else
        {
        CleanupStack::PopAndDestroy(); // aContact
        }
    }

TBool CPbk2DuplicateContactFinder::IsFieldMatched( const MVPbkBaseContact* aContact, TInt aFieldId )
    {
    TBool result = EFalse;
    
    const MVPbkBaseContactField* compareContactField = FindFieldById( iContactToCompare, aFieldId );
    const MVPbkBaseContactField* contactField = FindFieldById( aContact, aFieldId );
    if ( NULL == compareContactField && NULL == contactField )
        {
        result = ETrue;
        }
    else if ( compareContactField && contactField )
        {
        const TDesC& compareContactFieldText = 
                MVPbkContactFieldTextData::Cast(compareContactField->FieldData()).Text();
        const TDesC& contactFieldText = 
                MVPbkContactFieldTextData::Cast(contactField->FieldData()).Text();
        if ( compareContactFieldText.CompareF( contactFieldText ) == 0 )
            {
            result = ETrue;
            }
        }
    
    return result;
    }

const MVPbkBaseContactField* CPbk2DuplicateContactFinder::FindFieldById( const MVPbkBaseContact* aContact,
        TInt aFieldId )
    {
    const TInt fieldCount = aContact->Fields().FieldCount();
    const MVPbkBaseContactFieldCollection& fieldSet = aContact->Fields();
    for ( TInt i = 0; i < fieldCount; ++i )
        {
        const MVPbkBaseContactField& field = fieldSet.FieldAt(i);
        if ( field.BestMatchingFieldType() )
            {
            TInt fieldTypeId = field.BestMatchingFieldType()->FieldTypeResId();
            if ( fieldTypeId == aFieldId )
                {
                return &field;
                }
            }
        }
    return NULL;
    }

