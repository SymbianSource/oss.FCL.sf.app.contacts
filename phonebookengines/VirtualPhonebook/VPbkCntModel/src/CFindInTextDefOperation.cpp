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
* Description:  A find operation that uses
*                CContactDataBase::FindInTextDefAsyncL
*
*/


#include "CFindInTextDefOperation.h"

// VPbkCntModel internal
#include "CContactStore.h"
#include "CFieldFactory.h"
#include "CContactLink.h"

// Virtual Phonebook
#include <MVPbkFieldType.h>
#include <MVPbkContactFindFromStoresObserver.h>
#include <CVPbkContactLinkArray.h>

// Other system includes
#include <cntfield.h>

namespace VPbkCntModel {

// Set a limit to the contact id conversion. If for some reason
// the result is huge.
const TInt KMaxConversionInOneStep = 100;
const TInt KContinueIdle = 1;
const TInt KDontContinueIdle = 0;
const TInt KGranularity( 2 );

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CFindInTextDefOperation::CFindInTextDefOperation
// ---------------------------------------------------------------------------
//
CFindInTextDefOperation::CFindInTextDefOperation( CContactStore& aContactStore,
        MVPbkContactFindFromStoresObserver& aObserver,
        const TCallBack& aWordParserCallBack )
        :   iStore( aContactStore ),
            iObserver( aObserver ),
            iWordParserCallBack( aWordParserCallBack )
    {

    }

// ---------------------------------------------------------------------------
// CFindInTextDefOperation::ConstructL
// ---------------------------------------------------------------------------
//
void CFindInTextDefOperation::ConstructL( const MDesC16Array& aSearchStrings,
        const MVPbkFieldTypeList& aFieldTypes )
    {
    // Copy search strings
    const TInt stringCount = aSearchStrings.MdcaCount();
    TInt granularity = stringCount;
    if ( stringCount == 0 )
        {
        // default granularity
        granularity = KGranularity;
        }

    iSearchStrings = new( ELeave ) CDesCArrayFlat( granularity );
    for ( TInt i = 0; i < stringCount; ++i )
        {
        iSearchStrings->AppendL( aSearchStrings.MdcaPoint( i ) );
        }

    // Convert fieldtypes to contact model text definitions
    iTextDefs = CContactTextDef::NewL();
    const TInt typeCount = aFieldTypes.FieldTypeCount();
    for ( TInt j = 0; j < typeCount; ++j )
        {
        const MVPbkFieldType& fieldType = aFieldTypes.FieldTypeAt( j );
        const CContactItemField* field =
            iStore.FieldFactory().FindField( fieldType );
        if ( field )
            {
            iTextDefs->AppendL(
                TContactTextDefItem( field->ContentType().FieldType(0) ) );
            }
        }

    iIdle = CIdle::NewL( CActive::EPriorityStandard );
    iResults = CVPbkContactLinkArray::NewL();
    }

// ---------------------------------------------------------------------------
// CFindInTextDefOperation::NewL
// ---------------------------------------------------------------------------
//
CFindInTextDefOperation* CFindInTextDefOperation::NewL(
        CContactStore& aContactStore,
        const MDesC16Array& aSearchStrings,
        const MVPbkFieldTypeList& aFieldTypes,
        MVPbkContactFindFromStoresObserver& aObserver,
        const TCallBack& aWordParserCallBack )
    {
    CFindInTextDefOperation* self = new( ELeave ) CFindInTextDefOperation(
        aContactStore, aObserver, aWordParserCallBack );
    CleanupStack::PushL( self );
    self->ConstructL( aSearchStrings, aFieldTypes );
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// CFindInTextDefOperation::~CFindInTextDefOperation
// ---------------------------------------------------------------------------
//
CFindInTextDefOperation::~CFindInTextDefOperation()
    {
    Cancel();
    delete iAsyncFinder;
    delete iIdle;
    delete iResults;
    delete iContactIds;
    delete iSearchStrings;
    delete iTextDefs;
    }

// ---------------------------------------------------------------------------
// From class MVPbkContactOperation.
// CFindInTextDefOperation::StartL
// ---------------------------------------------------------------------------
//
void CFindInTextDefOperation::StartL()
    {
    if ( iTextDefs->Count() == 0 || iSearchStrings->Count() == 0 )
        {
        iIdle->Start( TCallBack( ConvertResults, this ) );
        }
    else
        {
        iAsyncFinder = iStore.NativeDatabase().FindInTextDefAsyncL(
            *iSearchStrings, iTextDefs, this, iWordParserCallBack );
        }
    }

// ---------------------------------------------------------------------------
// From class MVPbkContactOperation.
// CFindInTextDefOperation::Cancel
// ---------------------------------------------------------------------------
//
void CFindInTextDefOperation::Cancel()
    {
    if ( iAsyncFinder )
        {
        iAsyncFinder->Cancel();
        delete iAsyncFinder;
        iAsyncFinder = NULL;
        }

    if ( iIdle )
        {
        iIdle->Cancel();
        }
    }

// ---------------------------------------------------------------------------
// From class MIdleFindObserver.
// CFindInTextDefOperation::IdleFindCallback
// ---------------------------------------------------------------------------
//
void CFindInTextDefOperation::IdleFindCallback()
    {
    if ( iAsyncFinder->IsComplete() )
        {
        iContactIds = iAsyncFinder->TakeContactIds();
        iIdle->Start( TCallBack( ConvertResults, this ) );
        }
    }

// ---------------------------------------------------------------------------
// CFindInTextDefOperation::ConvertResults
// ---------------------------------------------------------------------------
//
TInt CFindInTextDefOperation::ConvertResults( TAny* aThis )
    {
    CFindInTextDefOperation* self =
        static_cast<CFindInTextDefOperation*>( aThis );
    TInt continueIdle = KDontContinueIdle;
    TRAPD( res, continueIdle = self->ConvertResultsL() );
    if ( res != KErrNone )
        {
        self->HandleError( res );
        }
    return continueIdle;
    }

// ---------------------------------------------------------------------------
// CFindInTextDefOperation::ConvertResultsL
// ---------------------------------------------------------------------------
//
TInt CFindInTextDefOperation::ConvertResultsL()
    {
    TInt convertedIds = 0;
    const TInt firstContact = 0;
    // Convert only reasonable amount of ids in one cycle
    if ( iContactIds )
        {
        while ( iContactIds->Count() > 0 &&
            convertedIds < KMaxConversionInOneStep )
            {
            CContactLink* link = CContactLink::NewLC( iStore,
                (*iContactIds)[firstContact] );
            iContactIds->Remove( firstContact );
            iResults->AppendL( link );
            CleanupStack::Pop(); // link
            ++convertedIds;
            }
        }

    // iContactIds not necessary exists if find wasn't done in StartL
    if ( !iContactIds || iContactIds->Count() == 0 )
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
// CFindInTextDefOperation::HandleError
// ---------------------------------------------------------------------------
//
void CFindInTextDefOperation::HandleError( TInt aError )
    {
    if ( aError != KErrNone )
        {
        iObserver.FindFromStoreFailed( iStore, aError );
        iObserver.FindFromStoresOperationComplete();
        }
    }
} // namespace VPbkCntModel
