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
* Description:  A class that reads one contact at a time using ETel
*                RMobilePhoneStore Read
*
*/



// INCLUDE FILES
#include "CSingleReadCmd.h"

#include "CStoreBase.h"
#include "CVPbkSimContact.h"
#include "CVPbkSimCntField.h"
#include "CContactArray.h"
#include "VPbkSimStoreImplError.h"

// From VPbkEng
#include "VPbkDebug.h"

namespace VPbkSimStoreImpl {


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CSingleReadBase::CSingleReadBase
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
template<class ETelContact>
CSingleReadBase<ETelContact>::CSingleReadBase( 
    CStoreBase& aStore, TInt aIndexToRead )
    :   CActive( EPriorityStandard ),
        iStore( aStore ),
        iETelContactPckg( iETelContact ),
        iCurrentIndex( aIndexToRead ),
        iLastIndexToRead( aIndexToRead ),
        iSimIndexRef( KVPbkSimStoreInvalidETelIndex )
    {
    CActiveScheduler::Add( this );
    }

// -----------------------------------------------------------------------------
// CSingleReadBase::CSingleReadBase
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
template<class ETelContact>
CSingleReadBase<ETelContact>::CSingleReadBase( CStoreBase& aStore )
    :   CActive( EPriorityStandard ),
        iStore( aStore ),
        iETelContactPckg( iETelContact ),
        iCurrentIndex( KVPbkSimStoreFirstETelIndex ),
        iLastIndexToRead( KVPbkSimStoreInvalidETelIndex ),
        iSimIndexRef( KVPbkSimStoreInvalidETelIndex )
    {
    CActiveScheduler::Add( this );
    }

// -----------------------------------------------------------------------------
// CSingleReadBase::CSingleReadBase
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
template<class ETelContact>
CSingleReadBase<ETelContact>::CSingleReadBase( 
        TInt& aIndexToReadRef, CStoreBase& aStore )
    :   CActive( EPriorityStandard ),
        iStore( aStore ),
        iETelContactPckg( iETelContact ),
        iCurrentIndex( KVPbkSimStoreInvalidETelIndex ),
        iLastIndexToRead( KVPbkSimStoreInvalidETelIndex ),
        iSimIndexRef( aIndexToReadRef )
    {
    CActiveScheduler::Add( this );
    }
    
// -----------------------------------------------------------------------------
// CSingleReadBase::Execute
// -----------------------------------------------------------------------------
//
template<class ETelContact>
void CSingleReadBase<ETelContact>::Execute()
    {
    __ASSERT_DEBUG( iObserver,
        VPbkSimStoreImpl::Panic( EPreCondCSingleReadBaseExecute ) );
        
    if ( iSimIndexRef != KVPbkSimStoreInvalidETelIndex )
        {
        iCurrentIndex = iSimIndexRef;
        iLastIndexToRead = iSimIndexRef;
        }
    else if ( iLastIndexToRead == KVPbkSimStoreInvalidETelIndex )
        {
        iLastIndexToRead = iStore.ETelStoreInfo().iTotalEntries;
        }

    // If noting to read then make a dummy request to complete command
    // asynchronously
    if ( !ReadNext() )
        {
        TRequestStatus* status = &iStatus;
        User::RequestComplete( status, KErrNone );
        SetActive();
        }
    }

// -----------------------------------------------------------------------------
// CSingleReadBase::AddObserverL
// -----------------------------------------------------------------------------
//
template<class ETelContact>
void CSingleReadBase<ETelContact>::AddObserverL( 
        MVPbkSimCommandObserver& aObserver )
    {
    __ASSERT_DEBUG( !iObserver,
        VPbkSimStoreImpl::Panic( EPreCondCSingleReadBaseAddObserverL ) );
    iObserver = &aObserver;
    }

// -----------------------------------------------------------------------------
// CSingleReadBase::CancelCmd
// -----------------------------------------------------------------------------
//
template<class ETelContact>
void CSingleReadBase<ETelContact>::CancelCmd()
    {
    CActive::Cancel();
    }

// -----------------------------------------------------------------------------
// CSingleReadBase::RunL
// -----------------------------------------------------------------------------
//
template<class ETelContact>
void CSingleReadBase<ETelContact>::RunL()
    {
    TInt result = iStatus.Int();

    VPBK_DEBUG_PRINT(VPBK_DEBUG_STRING(
        "VPbkSimStoreImpl: RMobilePhoneStore::Read h%d complete %d, index %d"),
        iStore.ETelStoreBase().SubSessionHandle(),result, 
        iETelContact.iIndex);
        
    HandleRunL();
    TBool readNext = ETrue;
    switch ( result )
        {
        case KErrNone: // FALLTHROUGH
        case KErrNotFound:
            {
            readNext = ReadNext();
            break;
            }
        default:
            {
            iObserver->CommandError( *this, result );
            break;
            }
        }

    if ( !readNext )
        {
        iObserver->CommandDone( *this );
        }
    }

// -----------------------------------------------------------------------------
// CSingleReadBase::DoCancel
// -----------------------------------------------------------------------------
//
template<class ETelContact>
void CSingleReadBase<ETelContact>::DoCancel()
    {
    iStore.ETelStoreBase().CancelAsyncRequest( EMobilePhoneStoreRead );
    }

// -----------------------------------------------------------------------------
// CSingleReadBase::RunError
// -----------------------------------------------------------------------------
//
template<class ETelContact>
TInt CSingleReadBase<ETelContact>::RunError( TInt aError )
    {
    iObserver->CommandError( *this, aError );
    return KErrNone;
    }
    
// -----------------------------------------------------------------------------
// CSingleReadBase::ReadNext
// -----------------------------------------------------------------------------
//
template<class ETelContact>
TBool CSingleReadBase<ETelContact>::ReadNext()
    {
    if ( iCurrentIndex <= iLastIndexToRead )
        {
        VPBK_DEBUG_PRINT(VPBK_DEBUG_STRING(
            "VPbkSimStoreImpl: RMobilePhoneStore::Read h%d index %d"),
            iStore.ETelStoreBase().SubSessionHandle(),iCurrentIndex);
        iETelContact.iIndex = iCurrentIndex;
        iStore.ETelStoreBase().Read( iStatus, iETelContactPckg );
        ++iCurrentIndex;
        SetActive();
        return ETrue;
        }
    return EFalse;
    }


// -----------------------------------------------------------------------------
// CSingleONReadCmd::CSingleONReadCmd
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CSingleONReadCmd::CSingleONReadCmd( CStoreBase& aStoreBase, 
        TInt aIndexToRead )
        :   CSingleReadBase<RMobileONStore::TMobileONEntryV1>(
                aStoreBase, aIndexToRead )
    {
    }

// -----------------------------------------------------------------------------
// CSingleONReadCmd::CSingleONReadCmd
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CSingleONReadCmd::CSingleONReadCmd( CStoreBase& aStoreBase )
        :   CSingleReadBase<RMobileONStore::TMobileONEntryV1>( aStoreBase )
    {
    }

// -----------------------------------------------------------------------------
// CSingleONReadCmd::CSingleONReadCmd
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CSingleONReadCmd::CSingleONReadCmd( TInt& aIndexToReadRef, 
        CStoreBase& aStoreBase )
        :   CSingleReadBase<RMobileONStore::TMobileONEntryV1>(
                aIndexToReadRef, aStoreBase)
    {
    }

// Destructor
CSingleONReadCmd::~CSingleONReadCmd()
    {
    CancelCmd();
    }

// -----------------------------------------------------------------------------
// CSingleONReadCmd::HandleRunL
// -----------------------------------------------------------------------------
//
void CSingleONReadCmd::HandleRunL()
    {
    TInt result = iStatus.Int();
    if ( result == KErrNone )
        {
        AddContactL();
        }
    }

// -----------------------------------------------------------------------------
// CSingleONReadCmd::AddContactL
// -----------------------------------------------------------------------------
//
void CSingleONReadCmd::AddContactL()
    {
    CVPbkSimContact* cnt = CVPbkSimContact::NewLC( iStore );
    cnt->SetSimIndex( iETelContact.iIndex );

    if ( iETelContact.iText.Length() > 0 )
        {
        CVPbkSimCntField* field = cnt->CreateFieldLC( EVPbkSimName );
        field->SetDataL( iETelContact.iText );
        cnt->AddFieldL( field );
        CleanupStack::Pop( field );
        }

    if ( iETelContact.iNumber.iTelNumber.Length() > 0 )
        {
        CVPbkSimCntField* field = cnt->CreateFieldLC( EVPbkSimGsmNumber );
        field->SetDataL( iETelContact.iNumber.iTelNumber );
        cnt->AddFieldL( field );
        CleanupStack::Pop( field );
        }

    if ( cnt->FieldArray().Count() > 0 )
        {
        iStore.Contacts().AddOrReplaceL( cnt );
        CleanupStack::Pop( cnt );
        }
    else
        {
        CleanupStack::PopAndDestroy( cnt );
        }
    }
} // namespace VPbkSimStoreImpl
//  End of File  
