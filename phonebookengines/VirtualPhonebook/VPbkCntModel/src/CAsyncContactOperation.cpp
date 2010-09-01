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
* Description:  The virtual phonebook asynchronous contact operation
*
*/



// INCLUDES
#include "CAsyncContactOperation.h"
#include <cntdb.h>
#include <cntitem.h>
#include "CContactStore.h"
#include "CContact.h"
#include <CVPbkDiskSpaceCheck.h>
#include <MVPbkContactStoreProperties.h>


namespace VPbkCntModel {

const TInt KDiskSpaceNeedUnknown = 0;

inline CAsyncContactOperation::CAsyncContactOperation
        ( CContactStore& aContactStore ) :
    CAsyncOneShot( CActive::EPriorityIdle ),
    iContactStore( aContactStore ),
    iOpCode( MVPbkContactObserver::EContactOperationUnknown )
    {
    }

CAsyncContactOperation* CAsyncContactOperation::NewL
		( CContactStore& aContactStore )
	{
	CAsyncContactOperation* self = 
	    new(ELeave) CAsyncContactOperation( aContactStore );
	CleanupStack::PushL( self );
	self->ConstructL();
	CleanupStack::Pop( self );
	return self;
	}

CAsyncContactOperation::~CAsyncContactOperation()
    {
    CActive::Cancel();
    delete iDiskSpaceChecker;
    iFs.Close();
    delete iViewDef;
    }
    
void CAsyncContactOperation::ConstructL()
    {
    User::LeaveIfError( iFs.Connect() );
    // Get contact store location drive for disk space check
    const TPtrC ptr( iContactStore.StoreProperties().Uri().Component
        ( TVPbkContactStoreUriPtr::EContactStoreUriStoreDrive ) );
    TInt drive( EDriveA ); // c is usually the default location
    User::LeaveIfError( iFs.CharToDrive
        ( ptr[0], drive) );
    iDiskSpaceChecker = VPbkEngUtils::CVPbkDiskSpaceCheck::NewL( iFs, drive );
    
    iViewDef = CContactItemViewDef::NewL(
                      CContactItemViewDef::EIncludeFields, 
                      CContactItemViewDef::EMaskHiddenFields);

    iViewDef->AddL( KUidContactFieldMatchAll );
    
    }    

void CAsyncContactOperation::PrepareL
        ( MVPbkContactObserver::TContactOp aOpCode, 
        TContactItemId aContactId,
        MVPbkContactObserver& aObserver )
    {
    if ( IsActive() )
        {
        User::Leave( KErrInUse );
        }
    iTargetContactId = aContactId;
    iOpCode = aOpCode;
    iObserver = &aObserver;
    iClientContact = NULL;
    }

void CAsyncContactOperation::PrepareL
        ( MVPbkContactObserver::TContactOp aOpCode, 
        const CContact& aContact,
        MVPbkContactObserver& aObserver )
    {
    if ( IsActive() )
        {
        User::Leave( KErrInUse );
        }
    iOpCode = aOpCode;
    iObserver = &aObserver;
    iClientContact = &aContact;
    }

void CAsyncContactOperation::Cancel( CContactItem* aContact )
	{
	if ( iClientContact &&
			aContact == iClientContact->NativeContact() )
		{
		// Do not call CActive::Cancel(), it is not desired since not all
		// operations are affected. The client contact is just not
		// valid anymore, so let's reset it.
		iClientContact = NULL;
		}
    }

void CAsyncContactOperation::DoCancel()
    {
    iOpCode = MVPbkContactObserver::EContactOperationUnknown;
    }

void CAsyncContactOperation::RunL()
    {
    MVPbkContactObserver::TContactOpResult result;
    result.iOpCode = iOpCode;
    result.iStoreContact = NULL;
    result.iExtension = NULL;

    switch( iOpCode )
        {
        case MVPbkContactObserver::EContactRead:
            {
            CContactItem* ci = 
                iContactStore.NativeDatabase().ReadContactLC( iTargetContactId );
            result.iStoreContact = CContact::NewL( iContactStore, ci );
            CleanupStack::Pop( ci );
            break;
            }

        case MVPbkContactObserver::EContactReadAndLock:
            {          
            CContactItem* ci = 
                iContactStore.NativeDatabase().OpenContactLX( iTargetContactId, *iViewDef );
            CleanupStack::PushL( ci );
            
            CContact* contact = CContact::NewL( iContactStore, ci );
            result.iStoreContact = contact;
            contact->SetModified();
            CleanupStack::Pop( 2 );  // ci, lock
            break;
            }

        case MVPbkContactObserver::EContactDelete:
            {
            iContactStore.NativeDatabase().DeleteContactL( iTargetContactId );
            break;
            }

        case MVPbkContactObserver::EContactLock:
            {
            if( iClientContact )
            	{
            	CContactItem* ci = iContactStore.NativeDatabase().OpenContactLX(
            			iClientContact->NativeContact()->Id(), *iViewDef );
	            CleanupStack::PushL( ci );
	            const_cast<CContact*>( iClientContact )->SetContact( ci );
	            const_cast<CContact*>( iClientContact )->SetModified();
	            CleanupStack::Pop( 2 );  // ci, lock
            	}
            break;
            }
            
        case MVPbkContactObserver::EContactCommit:
            {
            if( iClientContact )
            	{	
            	iDiskSpaceChecker->DiskSpaceCheckL( KDiskSpaceNeedUnknown );
	            if (iClientContact->IsNewContact())
	                {
	                iContactStore.NativeDatabase().AddNewContactL( 
	                                    *iClientContact->NativeContact() );
	                }
	            else
	                {
	                iContactStore.NativeDatabase().CommitContactL(
	                                    *iClientContact->NativeContact() );
	                }
            	}	
            break;
            }
        case MVPbkContactObserver::EContactSetOwn:
        	{
        	iContactStore.NativeDatabase().SetOwnCardL( *iClientContact->NativeContact() );
        	break;
        	}
        default:
            {
            // Operation was canceled
            return;
            }
        }

    iObserver->ContactOperationCompleted( result );
    }

TInt CAsyncContactOperation::RunError( TInt aError )
    {
    iObserver->ContactOperationFailed( iOpCode, aError, EFalse );
    return KErrNone;
    }

} // namespace VPbkCntModel

//End of file
