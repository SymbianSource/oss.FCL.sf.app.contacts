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
* Description:  A find operation that uses word parser function from client
*
*/


#include "CFindWithParserOperation.h"

#include "CContactStore.h"
#include "CFieldTypeMappings.h"
#include "CContactLink.h"

#include <MVPbkFieldType.h>
#include <MVPbkContactStoreInfo.h>
#include <CVPbkContactLinkArray.h>
#include <MVPbkContactFindFromStoresObserver.h>
#include <TVPbkWordParserCallbackParam.h>
#include <CVPbkContactFindPolicy.h>
#include <CVPbkSimContactBuf.h>
#include <CVPbkSimCntField.h>
#include <MVPbkSimCntStore.h>

namespace VPbkSimStore {

// Max amount of synchrnous contact reads from sim server
// at one step
const TInt KMaxContactReadInOneStep = 50;
const TInt KContinueIdle = 1;
const TInt KDontContinueIdle = 0;

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CFindWithParserOperation::CFindWithParserOperation
// ---------------------------------------------------------------------------
//
CFindWithParserOperation::CFindWithParserOperation( CContactStore& aStore,
        MVPbkContactFindFromStoresObserver& aObserver, 
        const TCallBack& aWordParserCallBack )
        :   iStore( aStore ),
            iSimStore( aStore.NativeStore() ),
            iObserver( aObserver ),
            iWordParserCallBack( aWordParserCallBack )
    {
    }


// ---------------------------------------------------------------------------
// CFindWithParserOperation::ConstructL
// ---------------------------------------------------------------------------
//
void CFindWithParserOperation::ConstructL( const MDesC16Array& aSearchStrings,
        const MVPbkFieldTypeList& aFieldTypes )
    {
    // Copy search strings
    const TInt stringCount = aSearchStrings.MdcaCount();
    TInt granularity = stringCount;
    if ( stringCount == 0 )
        {
        // default granularity
        granularity = 2;
        }
    
    iSearchStrings = new( ELeave ) CDesCArrayFlat( granularity );
    for ( TInt i = 0; i < stringCount; ++i )
        {
        iSearchStrings->AppendL( aSearchStrings.MdcaPoint( i ) );
        }
    
    // Convert field types
    const TInt fieldTypeCount = aFieldTypes.FieldTypeCount();
    for ( TInt j = 0; j < fieldTypeCount; ++j )
        {
        TVPbkSimCntFieldType simType = 
            iStore.FieldTypeMappings().Match( aFieldTypes.FieldTypeAt( j ) );
        if ( simType != EVPbkSimUnknownType )
            {
            iSimFieldTypes.AppendL( simType );
            }
        }
    
    iIdle = CIdle::NewL( CActive::EPriorityStandard );
    iResults = CVPbkContactLinkArray::NewL();
    iCurSimContact = CVPbkSimContactBuf::NewL( iSimStore );
    granularity = 4;
    iParsedWords = new( ELeave ) CDesCArrayFlat( granularity );
    // Assumes that find policy implementation exists
    iFindPolicy = CVPbkContactFindPolicy::NewL();
    }


// ---------------------------------------------------------------------------
// CFindWithParserOperation::NewL
// ---------------------------------------------------------------------------
//
CFindWithParserOperation* CFindWithParserOperation::NewL(
        CContactStore& aStore,
        const MDesC16Array& aSearchStrings,
        const MVPbkFieldTypeList& aFieldTypes,
        MVPbkContactFindFromStoresObserver& aObserver, 
        const TCallBack& aWordParserCallBack )
    {
    CFindWithParserOperation* self = new( ELeave ) CFindWithParserOperation( 
        aStore, aObserver, aWordParserCallBack );
    CleanupStack::PushL( self );
    self->ConstructL( aSearchStrings, aFieldTypes );
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// CFindWithParserOperation::~CFindWithParserOperation
// ---------------------------------------------------------------------------
//
CFindWithParserOperation::~CFindWithParserOperation()
    {
    Cancel();
    delete iFindPolicy;
    delete iParsedWords;
    delete iCurSimContact;
    delete iResults;
    delete iIdle;
    delete iSearchStrings;
    iSimFieldTypes.Close();
    }

// ---------------------------------------------------------------------------
// From class MVPbkContactOperation
// CFindWithParserOperation::StartL
// ---------------------------------------------------------------------------
//
void CFindWithParserOperation::StartL()
    {
    iTotalSimSlots = iStore.StoreInfo().MaxNumberOfContactsL();
    // SIM indexes start from 1
    iCurSimIndex = 1;
    
    if ( iSearchStrings->Count() == 0 || iSimFieldTypes.Count() == 0 )
        {
        // Set to zero so that find will stop immediately in callback
        iTotalSimSlots = 0;    
        }
        
    iIdle->Start( TCallBack( LoopContacts, this ) );
    }

// ---------------------------------------------------------------------------
// From class MVPbkContactOperation
// CFindWithParserOperation::Cancel
// ---------------------------------------------------------------------------
//
void CFindWithParserOperation::Cancel()
    {
    if ( iIdle )
        {
        iIdle->Cancel();
        }
    }

// ---------------------------------------------------------------------------
// CFindWithParserOperation::LoopContacts
// ---------------------------------------------------------------------------
//
TInt CFindWithParserOperation::LoopContacts( TAny* aThis )
    {
    CFindWithParserOperation* self = 
        static_cast<CFindWithParserOperation*>( aThis );
    TInt continueIdle = KDontContinueIdle;
    TRAPD( res, continueIdle = self->LoopContactsL() );
    if ( res != KErrNone )
        {
        self->HandleError( res );
        }
    return continueIdle;
    }

// ---------------------------------------------------------------------------
// CFindWithParserOperation::LoopContacts
// ---------------------------------------------------------------------------
//
TInt CFindWithParserOperation::LoopContactsL()
    {
    TInt contactsRead = 0;
    
    // Implementation comment: contact are read from server one contact
    // at a time. If this is too slow with big USIM cards then there are
    // couple of possibilities: 1) read many contacts in one request,
    // 2) do a find to SIM store that matches contacts that have search
    // strings anywhere in the field data. Then refine the result here.
    while ( iCurSimIndex <= iTotalSimSlots &&
            contactsRead <= KMaxContactReadInOneStep )
        {
        const TDesC8* etelCnt = iSimStore.AtL( iCurSimIndex );
        if ( etelCnt )
            {
            // reuse iCurSimContact
            iCurSimContact->SetL( *etelCnt );
            if ( IsCurrentContactMatchL() )
                {
                iResults->AppendL( 
                    CContactLink::NewLC( iStore, iCurSimIndex ) );
                CleanupStack::Pop(); // link
                }
            }
        ++iCurSimIndex;
        ++contactsRead;
        }
    
    if ( contactsRead == 0 )
        {
        // Results are ready.
        MVPbkContactLinkArray* results = iResults;
        iResults = NULL;
        iObserver.FindFromStoreSucceededL( iStore, results );
        iObserver.FindFromStoresOperationComplete();
        return KDontContinueIdle;
        }
    return KContinueIdle;
    }

// ---------------------------------------------------------------------------
// CFindWithParserOperation::IsCurrentContactMatchL
// ---------------------------------------------------------------------------
//    
TBool CFindWithParserOperation::IsCurrentContactMatchL()
    {
    TBool fieldMatched = EFalse;
    const TInt fieldCount = iCurSimContact->FieldCount();
    const TInt searchStringCount = iSearchStrings->Count();
    for ( TInt i = 0; i < fieldCount && !fieldMatched; ++i )
        {
        const CVPbkSimCntField& field = iCurSimContact->ConstFieldAt( i );
        // Check that is the type of the field in the given types
        if ( iSimFieldTypes.Find( field.Type() ) != KErrNotFound )
            {
            // Call clients parser function. It separates field data
            // into words and appends them into iParsedWords
            iParsedWords->Reset();
            TVPbkWordParserCallbackParam param( &field.Data(), iParsedWords );
            iWordParserCallBack.iFunction( &param );
            
            // All the search strings must match. If there are more
            // search strings than the words then it's not a match
            if ( searchStringCount <= iParsedWords->Count() )
                {
                TInt matchedWords = 0;
                for ( TInt j = 0; j < searchStringCount; ++j )
                    {
                    const TInt wordCount = iParsedWords->Count();
                    for ( TInt k = 0; k < wordCount; ++k )
                        {
                        if ( iFindPolicy->MatchRefineL( (*iParsedWords)[k], 
                            (*iSearchStrings)[j] ))
                            {
                            // Remove word that matched so that it's not
                            // matched again
                            iParsedWords->Delete( k );
                            ++matchedWords;
                            break;                           
                            }
                        }
                    }
                
                // If all words matched then the field data matched
                if ( matchedWords == searchStringCount )
                    {
                    fieldMatched = ETrue;
                    }
                }
            }
        }
    return fieldMatched;
    }
    
// ---------------------------------------------------------------------------
// CFindWithParserOperation::HandleError
// ---------------------------------------------------------------------------
//    
void CFindWithParserOperation::HandleError( TInt aError )
    {
    if ( aError != KErrNone )
        {
        iObserver.FindFromStoreFailed( iStore, aError );
        iObserver.FindFromStoresOperationComplete();
        }
    }
} // namespace VPbkSimStore
