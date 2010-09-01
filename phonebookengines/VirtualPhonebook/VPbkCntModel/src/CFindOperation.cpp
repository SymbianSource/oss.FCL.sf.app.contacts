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
* Description:  The virtual phonebook find operation
*
*/



#include "CFindOperation.h"
#include "CContactLink.h"
#include "CContactStore.h"
#include "CFieldFactory.h"
#include <cntfield.h>

#include <MVPbkFieldType.h>
#include <MVPbkContactFindObserver.h>

namespace VPbkCntModel {

CFindOperation::CFindOperation(
        CContactStore& aContactStore, 
        MVPbkContactFindObserver& aObserver ) :
    CActive( CActive::EPriorityIdle ),
    iContactStore( aContactStore),
    iObserver( aObserver )
    {
    CActiveScheduler::Add( this );
    }

inline void CFindOperation::ConstructL(
        const TDesC& aSearchString, 
        const MVPbkFieldTypeList& aFieldTypes )
    {
    iSearchString = aSearchString.AllocL();
    iFieldDef = new ( ELeave ) CContactItemFieldDef;
    
    const TInt fieldCount = aFieldTypes.FieldTypeCount();
    iFieldDef->SetReserveL( fieldCount );
    for ( TInt i = 0; i < fieldCount; ++i )
        {
        const CContactItemField* field = 
            iContactStore.FieldFactory().FindField( aFieldTypes.FieldTypeAt(i) );
        if ( field )
            {
            iFieldDef->AppendL( field->ContentType().FieldType(0) );
            }
        }
    iResults = CVPbkContactLinkArray::NewL();
    }

CFindOperation* CFindOperation::NewL(
        CContactStore& aContactStore, 
        const TDesC& aSearchString, 
        const MVPbkFieldTypeList& aFieldTypes,
        MVPbkContactFindObserver& aObserver )
    {
	CFindOperation* self = new ( ELeave ) CFindOperation(
        aContactStore, aObserver );
    CleanupStack::PushL( self );
    self->ConstructL( aSearchString, aFieldTypes );
    CleanupStack::Pop( self );
	return self;
	}

CFindOperation::~CFindOperation()
    {
    Cancel();
    
    delete iSearchString;
    delete iResults;
    delete iAsyncFinder;
    delete iContactIds;
    delete iFieldDef;
    }

void CFindOperation::IssueRequest()
    {
    TRequestStatus* status = &iStatus;
    User::RequestComplete( status, KErrNone );
    SetActive();
    }

void CFindOperation::DoCancel()
    {
    iState = ECancelled;
    if ( !IsActive() )
        {
        IssueRequest();
        }
    }

void CFindOperation::RunL()
    {
    switch ( iState )
        {
        case EBuildLinks:
            {
            if ( iContactIds->Count() > 0 )
                {
                CContactLink* link = CContactLink::NewLC( iContactStore, 
                                                    (*iContactIds)[0] );
                iContactIds->Remove( 0 );
                iResults->AppendL( link );
                CleanupStack::Pop( link );
                IssueRequest();
                }
            else
                {
                delete iContactIds;
                iContactIds = NULL;
                iState = EComplete;
                IssueRequest();
                }
            break;
            }
        case EComplete:
            {
            // gives ownership to observer
            CVPbkContactLinkArray* results = iResults;
            iResults = NULL;
            iObserver.FindCompleteL( results );
            break;
            }
        case ECancelled:
            {
            iObserver.FindFailed( KErrCancel );
            break;
            }
        }

    }

TInt CFindOperation::RunError( TInt aError )
    {
    iObserver.FindFailed( aError );
    return KErrNone;
    }

void CFindOperation::StartL()
    {
    iAsyncFinder = iContactStore.NativeDatabase().FindAsyncL(
        *iSearchString, iFieldDef, this );
	}
	
void CFindOperation::IdleFindCallback()
    {
    if ( iAsyncFinder->IsComplete() )
        {
        iState = EBuildLinks;
        iContactIds = iAsyncFinder->TakeContactIds();
        IssueRequest();
        }
    }

void CFindOperation::Cancel()
    {
    CActive::Cancel();
	}

} // namespace VPbkCntModel 
