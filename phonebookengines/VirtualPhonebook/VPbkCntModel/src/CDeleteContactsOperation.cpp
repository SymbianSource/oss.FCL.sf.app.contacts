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
* Description:  An operation object to delete contacts from contacts database
*
*/


#include "CDeleteContactsOperation.h"

// VPbkCntModel
#include "CContactLink.h"
#include "CContactStore.h"
#include "CVPbkDiskSpaceCheck.h"
#include "CContact.h"

// Virtual Phonebook engine
#include <MVPbkBatchOperationObserver.h>

// Contact Model
#include <cntdef.h>
#include <cntdb.h>
#include <cntitem.h>


namespace VPbkCntModel {

// Definition for delete array size
const TInt KDeleteArraySize = 3;

// Definition for FreeDiskSpaceRequest towards DOSSERVER
const TInt KFreeSpaceRequiredInKB = 140 * 1024; // 140 kB

// -----------------------------------------------------------------------------
// CDeleteContactsOperation::CDeleteContactsOperation
// -----------------------------------------------------------------------------
//
CDeleteContactsOperation::CDeleteContactsOperation(
        CContactStore& aContactStore,
        MVPbkBatchOperationObserver& aObserver,
        RSharedDataClient* aSharedDataClient,
        VPbkEngUtils::CVPbkDiskSpaceCheck& aDiskSpaceChecker ) :
    CActive( CActive::EPriorityIdle ),
    iContactStore( aContactStore ),
    iObserver( aObserver ),
    iState( EDelete ),
    iSharedDataClient( aSharedDataClient ),
    iDiskSpaceChecker( aDiskSpaceChecker )
    {
    CActiveScheduler::Add(this);
    }

// -----------------------------------------------------------------------------
// CDeleteContactsOperation::~CDeleteContactsOperation
// -----------------------------------------------------------------------------
//
CDeleteContactsOperation::~CDeleteContactsOperation()
    {
    Cancel();
    delete iRemainingContactIds;
    delete iCurrentContactIds;
    }

// -----------------------------------------------------------------------------
// CDeleteContactsOperation::NewL
// -----------------------------------------------------------------------------
//
CDeleteContactsOperation* CDeleteContactsOperation::NewL(
        CContactStore& aContactStore,
        const MVPbkContactLinkArray& aLinks,
        MVPbkBatchOperationObserver& aObserver,
        RSharedDataClient* aSharedDataClient,
        VPbkEngUtils::CVPbkDiskSpaceCheck& aDiskSpaceChecker )
    {
    CDeleteContactsOperation* self =
        new( ELeave ) CDeleteContactsOperation( aContactStore, aObserver,
                                                aSharedDataClient, aDiskSpaceChecker );
    CleanupStack::PushL( self );
    self->ConstructL( aLinks );
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// CDeleteContactsOperation::ConstructL
// -----------------------------------------------------------------------------
//
inline void CDeleteContactsOperation::ConstructL
        (const MVPbkContactLinkArray& aLinks)
    {
    iRemainingContactIds = CContactIdArray::NewL();
    iCurrentContactIds = CContactIdArray::NewL();
    
    iGroupCount = iContactStore.NativeDatabase().GroupCount();
    
    const TInt count = aLinks.Count();
    for ( TInt i=0 ; i < count; ++i )
        {
        const CContactLink& link =
            static_cast<const CContactLink&>( aLinks.At( i ) );
        iRemainingContactIds->AddL( link.ContactId() );
        }
    PrepareNextDelete();
    }

// -----------------------------------------------------------------------------
// CDeleteContactsOperation::DoCancel
// -----------------------------------------------------------------------------
//
void CDeleteContactsOperation::DoCancel()
    {
    // Do nothing
    }

// -----------------------------------------------------------------------------
// CDeleteContactsOperation::RunL
// -----------------------------------------------------------------------------
//
void CDeleteContactsOperation::RunL()
    {
    switch ( iState )
        {
        case EDelete:
            {
            RunDeleteL();
            IssueRequest();
            break;
            }
        case EComplete:
            {
            iObserver.OperationComplete( *this );
            break;
            }
        }
    }

// -----------------------------------------------------------------------------
// CDeleteContactsOperation::RunError
// -----------------------------------------------------------------------------
//
TInt CDeleteContactsOperation::RunError(TInt aError)
    {    
    if ( iFreeDiskSpaceRequired )
        {
        // Notify shared data that we do not need free space anymore
        DoCancelFreeDiskSpaceRequest();
        iFreeDiskSpaceRequired = EFalse;
        }

    // if iCurrentContactIndex is valid, than it's delete error
    // otherwise, RunDeleteL leaved outside the delete loop
    TBool deleteError = iCurrentContactIndex >= 0 &&
    					iCurrentContactIndex < iCurrentContactIds->Count();

    // if error outside the delete loop, than skip all current contacts,
    // otherwise, remove failed contact and try again
    TInt stepSize = iCurrentContactIds->Count();
    if ( deleteError )
    	{
    	stepSize = 1;
    	}
    
    if ( iObserver.StepFailed( *this, stepSize, aError ) )
        {
        if ( iCurrentContactIds->Count() > 1 && deleteError )
        	{
        	//remove failed contact and try without it again
        	iCurrentContactIds->Remove( iCurrentContactIndex );
        	iState = EDelete;
        	}
        else
        	{
        	//skip iCurrentContactIds, try remaining
        	PrepareNextDelete();
        	}
        IssueRequest();
        }
 
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CDeleteContactsOperation::StartL
// -----------------------------------------------------------------------------
//
void CDeleteContactsOperation::StartL()
    {   
    IssueRequest();
    }

// -----------------------------------------------------------------------------
// CDeleteContactsOperation::Cancel
// -----------------------------------------------------------------------------
//
void CDeleteContactsOperation::Cancel()
    {
    CActive::Cancel();
    }

// -----------------------------------------------------------------------------
// CDeleteContactsOperation::IssueRequest
// -----------------------------------------------------------------------------
//
void CDeleteContactsOperation::IssueRequest()
    {
    TRequestStatus* status = &iStatus;
    User::RequestComplete( status, KErrNone );
    SetActive();
    }

// -----------------------------------------------------------------------------
// CDeleteContactsOperation::UpdateTimeStampOfAllContactsInGroupL
// -----------------------------------------------------------------------------
//
void CDeleteContactsOperation::UpdateTimeStampOfAllContactsInGroupL()
    {
    CContactItem* groupItem = iContactStore.NativeDatabase().
                                ReadContactLC( ( *iCurrentContactIds )[iCurrentContactIndex] );
    
    CContact* group = CContact::NewL( iContactStore, groupItem );
    CleanupStack::Pop(groupItem);
    CleanupStack::PushL( group );
   
    group->UpdateTimeStampOfAllContactsInGroupL();
    
    CleanupStack::PopAndDestroy( group ); 
    }

// -----------------------------------------------------------------------------
// CDeleteContactsOperation::CurrentItemIsGroupL
// -----------------------------------------------------------------------------
//
TBool CDeleteContactsOperation::CurrentItemIsGroupL()
    {
    TBool isGroup = EFalse;
    
    // If groups exists, we need to read a minimal contact to get it's type
    if (iGroupCount > 0)
        {
        CContactItem* item = iContactStore.NativeDatabase().
            ReadMinimalContactLC( ( *iCurrentContactIds )[iCurrentContactIndex] );
                   
        if (item->Type() == KUidContactGroup)
            {
            isGroup = ETrue;
            }
        
        CleanupStack::PopAndDestroy(item); 
        }
    
    return isGroup;
    }

// -----------------------------------------------------------------------------
// CDeleteContactsOperation::RunDeleteL
// -----------------------------------------------------------------------------
//
inline void CDeleteContactsOperation::RunDeleteL()
    {
    StartTransactionLC();

    TInt stepSize = iCurrentContactIds->Count();
    for ( iCurrentContactIndex = 0;
    	  iCurrentContactIndex < stepSize;
      	  ++iCurrentContactIndex )
        {
        DoRequestFreeDiskSpaceLC();        
        
        // If contact item is a group, we need to update the timestamps 
        // of the contacts in the group before deleting the group.
        // This is needed for sync services.
        if ( CurrentItemIsGroupL() )
            {
            UpdateTimeStampOfAllContactsInGroupL();
            }
      
        // Cannot use DeleteContactsL here, since it does not
        // send contact removed events
        iContactStore.NativeDatabase().
            DeleteContactL( ( *iCurrentContactIds )[iCurrentContactIndex] );
        
        CleanupStack::PopAndDestroy();  // RequestFreeDiskSpaceLC            

        RunCompressL(EFalse);
        }
    
    // Commit database transaction
    CommitTransactionLP();
    
    iObserver.StepComplete( *this, stepSize );
    RunCompressL(ETrue);
    PrepareNextDelete();
    }

void CDeleteContactsOperation::PrepareNextDelete()
	{
    iCurrentContactIds->Reset();
    for ( TInt i = Min(iRemainingContactIds->Count(), KDeleteArraySize) - 1;
    	  i >= 0;
    	  --i )
        {
        iCurrentContactIds->AddL( ( *iRemainingContactIds )[i] );
        iRemainingContactIds->Remove( i );
        }
    
    iState = EDelete;
    iCurrentContactIndex = -1; //make index invalid, it's checked in RunError
    						   //to detect StartTransactionLC leave
    if ( iCurrentContactIds->Count() == 0 )
        {
        // No more contacts to delete
        iState = EComplete;
        }
	}

// -----------------------------------------------------------------------------
// CDeleteContactsOperation::RunCompressL
// -----------------------------------------------------------------------------
//
inline void CDeleteContactsOperation::RunCompressL(TBool aForce)
    {
    if ( aForce || iContactStore.NativeDatabase().CompressRequired() )
        {
        DoRequestFreeDiskSpaceLC();        
        // Compress database
        iContactStore.NativeDatabase().CompactL();
        CleanupStack::PopAndDestroy();  // RequestFreeDiskSpaceLC        
        }    
    }

// -----------------------------------------------------------------------------
// CDeleteContactsOperation::StartTransactionLC
// -----------------------------------------------------------------------------
//
inline void CDeleteContactsOperation::StartTransactionLC()
    {
    // If database transaction starting leaves --> 
    // Reserved disk space if freed in CDeleteContactsOperation::RunError
    DoRequestFreeDiskSpace();
    iFreeDiskSpaceRequired = ETrue;
    
    iContactStore.NativeDatabase().DatabaseBeginLC( EFalse );
    
    DoCancelFreeDiskSpaceRequest();
    iFreeDiskSpaceRequired = EFalse;
    }
    
// -----------------------------------------------------------------------------
// CDeleteContactsOperation::CommitTransactionLC
// -----------------------------------------------------------------------------
//
inline void CDeleteContactsOperation::CommitTransactionLP()
    {
    // If database commit leaves --> Reserved disk space if freeed in
    // CDeleteContactsOperation::RunError
    DoRequestFreeDiskSpace();
    iFreeDiskSpaceRequired = ETrue;
    
    iContactStore.NativeDatabase().DatabaseCommitLP( EFalse );
    
    DoCancelFreeDiskSpaceRequest();
    iFreeDiskSpaceRequired = EFalse;  
    }

// -----------------------------------------------------------------------------
// CDeleteContactsOperation::DoRequestFreeDiskSpace
// -----------------------------------------------------------------------------
//
void CDeleteContactsOperation::DoRequestFreeDiskSpace()
    {
    if ( iSharedDataClient )
        {
        iSharedDataClient->RequestFreeDiskSpace(KFreeSpaceRequiredInKB);
        }
    }

// -----------------------------------------------------------------------------
// CDeleteContactsOperation::DoRequestFreeDiskSpaceLC
// -----------------------------------------------------------------------------
//
void CDeleteContactsOperation::DoRequestFreeDiskSpaceLC()
    {
    if ( iSharedDataClient )
        {
        iSharedDataClient->RequestFreeDiskSpaceLC(KFreeSpaceRequiredInKB);        
        }
    else
        {
        // place token in cleanup stack
        TAny* const nop = NULL;
        CleanupStack::PushL(nop);
        }
    }

// -----------------------------------------------------------------------------
// CDeleteContactsOperation::CancelFreeDiskSpaceRequest
// -----------------------------------------------------------------------------
//
void CDeleteContactsOperation::DoCancelFreeDiskSpaceRequest()
    {
    if ( iSharedDataClient )
        {
        iSharedDataClient->CancelFreeDiskSpaceRequest();
        }
    }

} // namespace VPbkCntModel

// End of File
